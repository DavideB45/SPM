#if !defined _UTILITY_HPP
#define _UTILITY_HPP

#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <cstdlib>
#include <cstdio>
#include <cstring>

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <string>
#include <atomic>

#include<config.hpp>

// map the file pointed by filepath in memory
// if size is zero, it looks for file size
// if everything is ok, it returns the memory pointer ptr
static inline bool mapFile(const char fname[], size_t &size, unsigned char *&ptr) {
    // open input file.
    int fd = open(fname,O_RDONLY);
    if (fd<0) {
		if (QUITE_MODE>=1) {
			perror("mapFile open");
			std::fprintf(stderr, "Failed opening file %s\n", fname);
		}
		return false;
    }
    if (size==0) {
		struct stat s;
		if (fstat (fd, &s)) {
			if (QUITE_MODE>=1) {
				perror("fstat");
				std::fprintf(stderr, "Failed to stat file %s\n", fname);
			}
			return false;
		}
		size=s.st_size;
    }
	
    // map all the file in memory
    ptr = (unsigned char *) mmap (0, size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (ptr == MAP_FAILED) {
		if (QUITE_MODE>=1) {
			perror("mmap");
			std::fprintf(stderr, "Failed to memory map file %s\n", fname);
		}
		return false;
    }
    close(fd);
    return true;
}
// unmap a previously memory-mapped file
static inline void unmapFile(unsigned char *ptr, size_t size) {
    if (munmap(ptr, size)<0) {
		if (QUITE_MODE>=1) {
			perror("nummap");
			std::fprintf(stderr, "Failed to unmap file\n");
		}
    }
}
// create an empty file of size 'size' and maps it in memory returning
// the pointer into 'ptr'
static inline bool allocateFile(const char fname[], const size_t size, unsigned char *&ptr) {
	
    int fd = open(fname, O_RDWR | O_CREAT | O_TRUNC, 0666);
    if (fd < 0) {
		if (QUITE_MODE>=1) 
			std::fprintf(stderr, "Failed to open output file: %s\n", fname);
        return false;
    }	
    // Resize the file
    if (ftruncate(fd, size) < 0) {
		if (QUITE_MODE>=1) 
			std::fprintf(stderr, "Error setting file size with ftruncate %s\n", strerror(errno));
        close(fd);
        return false;
    }
	
    ptr = (unsigned char*)mmap(nullptr, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (ptr == MAP_FAILED) {
		if (QUITE_MODE>=1) 
			std::fprintf(stderr, "Error mapping file %s\n", strerror(errno));		
        close(fd);
        return false;
    }
	close(fd);
	return true;
}

// check if dir is '.' or '..'
static inline bool isdot(const char dir[]) {
  int l = strlen(dir);  
  if ( (l>0 && dir[l-1] == '.') ) return true;
  return false;
}
// returns true if 'p' is a directory, false if it is a file
// if it is a regular file, filesize will containe the size of the regular file
static inline bool isDirectory(const std::filesystem::path& p, size_t& filesize) {
    try {
		if (std::filesystem::is_directory(p)) {
			return true;
		}
		if (std::filesystem::is_regular_file(p)) {
			filesize = std::filesystem::file_size(p);
			return false;
		}
		filesize=0;
		return false;
    } catch (const std::filesystem::filesystem_error& e) {
		std::fprintf(stderr, "Filesystem error: %s\n", e.what());
        return false;
    }
}

// If compdecomp is true (compressing), it checks if fname has the suffix SUFFIX,
// if yes it returns true
// If compdecomp is false (decompressing), it checks if fname has the suffix SUFFIX,
// if yes it returns false
static inline bool discardIt(const char *fname, const bool compdecomp) {
    const int lensuffix=strlen(SUFFIX);
    const int len      = strlen(fname);
    if (len>lensuffix &&
		(strncmp(&fname[len-lensuffix], SUFFIX, lensuffix)==0)) {
		return compdecomp; // true or false depends on we are compressing or decompressing;
    }
    return !compdecomp;
}

// check if the string 's' is a number, otherwise it returns false
static bool isNumber(const char* s, long &n) {
    try {
		size_t e;
		n=std::stol(s, &e, 10);
		return e == strlen(s);
    } catch (const std::invalid_argument&) {
		return false;
    } catch (const std::out_of_range&) {
		return false;
    }
}
// check if option is present in the command line
// if it is, it returns the value of the option, otherwise it returns nullptr
static inline char* getOption(char **begin, char **end, const std::string &option) {
    char **itr = std::find(begin, end, option);
    if (itr != end && ++itr != end) return *itr;
    return nullptr;
}

// it compresses the data pointed by 'ptr' and having size 'size'
// fname stores the file name of the file containing the data pointed by 'ptr'
// return true if okay, false in case of errors
static inline bool compressData(unsigned char *ptr, size_t size, const std::string &fname) {
	unsigned char * inPtr  = ptr;
	size_t          inSize = size;
	// get an estimation of the maximum compression size
	size_t cmp_len = compressBound(inSize);
	// allocate memory to store compressed data in memory
	unsigned char *ptrOut = new unsigned char[cmp_len];
	if (compress(ptrOut, &cmp_len, (const unsigned char *)inPtr, inSize) != Z_OK) {
		if (QUITE_MODE>=1) 
			std::fprintf(stderr, "Failed to compress file in memory\n");    
		delete [] ptrOut;
		return false;
	}
	std::string outfile = fname + SUFFIX;
	std::ofstream outFile(outfile, std::ios::binary);
	if (!outFile.is_open()) {
		std::fprintf(stderr, "Failed to open output file: %s\n", outfile.c_str());
		delete [] ptrOut;
		return false;
	}
	
	// Write first the original file size (not really needed, but simplify decompression)
	outFile.write(reinterpret_cast<const char*>(&inSize), sizeof(inSize));	
	// Write the compressed data
	outFile.write(reinterpret_cast<const char*>(ptrOut), cmp_len);
	
	if (REMOVE_ORIGIN) {
		unlink(fname.c_str());
	}
	
	delete[] ptrOut;
	outFile.close();
    return true;
}
// it decompresses the data pointed by 'ptr' and having size 'size'
// fname stores the file name of the file containing the data pointed by 'ptr'
// return true if okay, false in case of errors
static inline bool decompressData(unsigned char *ptr, size_t size, const std::string &fname) {
    size_t decompressedSize= reinterpret_cast<size_t*>(ptr)[0];  // read the original size
	ptr += sizeof(size_t); // advance the pointer

    // Write the decompressed data to a file
	unsigned char *decompressed_data=nullptr;
    std::string outfile = fname.substr(0, fname.size() - 4);  // remove the SUFFIX (i.e., .zip)

	// allocate the space in a file for the uncompressed data. The file is memory mapped.
	if (!allocateFile(outfile.c_str(), decompressedSize, decompressed_data)) return false;
	// decompress the data 	
    if (uncompress(decompressed_data, &decompressedSize, ptr, size) != Z_OK) {
		if (QUITE_MODE>=1) 
			std::fprintf(stderr, "uncompress failed!\n");
		unmapFile(decompressed_data, decompressedSize);
        return false;
    }
	// write the data into the disk
    unmapFile(decompressed_data, decompressedSize);
	
    if (REMOVE_ORIGIN) {
        unlink(fname.c_str());
    }

    return true;
}

// entry-point for compression and decompression 
// returns false in case of error
static inline bool doWork(const char fname[], size_t size, const bool comp) {
    unsigned char *ptr = nullptr;
    if (!mapFile(fname, size, ptr)) {
		if (QUITE_MODE>=1) 
			std::fprintf(stderr, "mapFile %s failed\n", fname);
		return false;
	}
	bool r = (comp) ?
		compressData(ptr, size, fname) :
		decompressData(ptr, size, fname);

    unmapFile(ptr, size);
	return r;
}

// 'dname' is a directory; traverse it and call doWork() for each file
// returns false in case of error
static inline bool walkDir(const char dname[], const bool comp) {
	if (chdir(dname) == -1) {
		if (QUITE_MODE>=1) {
			perror("chdir");
			std::fprintf(stderr, "Error: chdir %s\n", dname);
		}
		return false;
    }
    DIR *dir;	
    if ((dir=opendir(".")) == NULL) {
		if (QUITE_MODE>=1) {
			perror("opendir");
			std::fprintf(stderr, "Error: opendir %s\n", dname);
		}
		return false;
    }
    struct dirent *file;
    bool error=false;
    while((errno=0, file =readdir(dir)) != NULL) {
		struct stat statbuf;
		if (stat(file->d_name, &statbuf)==-1) {
			if (QUITE_MODE>=1) {		
				perror("stat");
				std::fprintf(stderr, "Error: stat %s\n", file->d_name);
			}
			return false;
		}
		if(S_ISDIR(statbuf.st_mode)) {
			if ( !isdot(file->d_name) ) {				
				if (walkDir(file->d_name, comp)) {
					if (chdir("..") == -1) {
						perror("chdir");
						std::fprintf(stderr, "Error: chdir ..\n");
						error = true;
					}
				}
				else error  = true;
			}
		} else {
			if (discardIt(file->d_name, comp)) {
				if (QUITE_MODE>=2){
					if (comp) {
						std::fprintf(stderr, "%s has already a %s suffix -- ignored\n", file->d_name, SUFFIX);
					} else {
						std::fprintf(stderr, "%s does not have a %s suffix -- ignored\n", file->d_name, SUFFIX);
					}
				}
				continue;									
			}
			if (!doWork(file->d_name, statbuf.st_size, comp)) error = true;
		}
    }
    if (errno != 0) {
		if (QUITE_MODE>=1) perror("readdir");
		error=true;
    }
    closedir(dir);
    return !error;
}

/* Here some functions have been slightly changed to enable usage of OpenMP
 * parallelism. The funcitons are much similar to before, the one that changed
 * the most is compressDataPar, that is been modified to handle big files by
 * splitting the data into chunks of size BLOCK_SIZE.
*/

// it compresses the data pointed by 'ptr' and having size 'size'
// fname stores the file name of the file containing the data pointed by 'ptr'
// return true if okay, false in case of errors
static inline bool compressDataPar(unsigned char *ptr, size_t size, const std::string &fname) {
	if(BLOCK_SIZE < size){
		#pragma omp task shared(ptr) untied
		{
		size_t          inSize = size;
		// decide hpw many blocks to use
		size_t nblocks = size / BLOCK_SIZE;
		std::atomic<bool> error(false);
		// allocate array of pointers to store the compressed blocks
		unsigned char* *ptrOut = new unsigned char* [nblocks];
		size_t* cmp_len = new size_t[nblocks];
		// forse posso creare task anche qui' e poi lasciare un thread alla fine che ripiglia le cose
		for(size_t b = 0; b < nblocks; b++){
			#pragma omp task shared(error)
			{
				// get the size of the block
				size_t blockSize = BLOCK_SIZE;
				if (b == nblocks - 1) {
					blockSize = size - b * BLOCK_SIZE;
				}
				unsigned char * inPtr  = ptr + b * BLOCK_SIZE;
				// get an estimation of the maximum compression size
				cmp_len[b] = compressBound(blockSize);
				// allocate memory to store compressed data in memory
				ptrOut[b] = new unsigned char[cmp_len[b]];
				// compress the data
				if (compress(ptrOut[b], &(cmp_len[b]), (const unsigned char *)inPtr, blockSize) != Z_OK) {
					if (QUITE_MODE>=1) 
						std::fprintf(stderr, "Failed to compress file in memory\n");    
					error.store(true, std::memory_order_relaxed);
				}
			}
		}
		// wait for all tasks to finish
		#pragma omp taskwait
		if(error.load(std::memory_order_relaxed)){
			if (QUITE_MODE>=1) 
				std::fprintf(stderr, "Failed to compress file in memory\n");    
			// free the allocated memory
			for (size_t b = 0; b < nblocks; b++) {
				delete[] ptrOut[b];
			}
			delete[] ptrOut;
			delete[] cmp_len;
			return false;
		}
		std::string outfile = fname + ".b" + SUFFIX;
		std::ofstream outFile(outfile, std::ios::binary);
		if (!outFile.is_open()) {
			std::fprintf(stderr, "Failed to open output file: %s\n", outfile.c_str());
			// free the allocated memory
			for (size_t b = 0; b < nblocks; b++) {
				delete[] ptrOut[b];
			}
			delete[] ptrOut;
			delete[] cmp_len;
			return false;
		}
	
		// Write first the original file size (not really needed, but simplify decompression)
		outFile.write(reinterpret_cast<const char*>(&inSize), sizeof(inSize));	
		// Write the compressed data
		for (size_t b = 0; b < nblocks; b++) {
			// Write the size of the compressed block
			outFile.write(reinterpret_cast<const char*>(&cmp_len[b]), sizeof(cmp_len[b]));
			// Write the compressed data
			outFile.write(reinterpret_cast<const char*>(ptrOut[b]), cmp_len[b]);
			// free the allocated memory
			delete[] ptrOut[b];
		}

		// clean stuff
		if (REMOVE_ORIGIN) {
			unlink(fname.c_str());
		}
		delete[] cmp_len;
		delete[] ptrOut;
		outFile.close();
		}// end of task
		return true;
	} else {
		// what is a reasonable block size?
		return compressData(ptr, size, fname);
	}
}

// entry-point for compression and decompression 
// returns false in case of error
static inline bool doWorkPar(const char fname[], size_t size, const bool comp) {
    unsigned char *ptr = nullptr;
    if (!mapFile(fname, size, ptr)) {
		if (QUITE_MODE>=1) 
			std::fprintf(stderr, "mapFile %s failed\n", fname);
		return false;
	}
	bool r = (comp) ?
		compressDataPar(ptr, size, fname) :
		decompressData(ptr, size, fname); //TODO: decompressDataPar

    unmapFile(ptr, size);
	return r;
}

// 'dname' is a directory; traverse it and call doWork() for each file
// returns false in case of error
static inline void walkDirPar(const char dname[], const bool comp, std::atomic<bool> &error) {
	if (chdir(dname) == -1) {
		if (QUITE_MODE>=1) {
			perror("chdir");
			std::fprintf(stderr, "Error: chdir %s\n", dname);
		}
		error.store(true, std::memory_order_relaxed);
		return;
    }
    DIR *dir;	
    if ((dir=opendir(".")) == NULL) {
		if (QUITE_MODE>=1) {
			perror("opendir");
			std::fprintf(stderr, "Error: opendir %s\n", dname);
		}
		error.store(true, std::memory_order_relaxed);
		return;
    }
    struct dirent *file;
    while((errno=0, file =readdir(dir)) != NULL) {
		struct stat statbuf;
		if (stat(file->d_name, &statbuf)==-1) {
			if (QUITE_MODE>=1) {		
				perror("stat");
				std::fprintf(stderr, "Error: stat %s\n", file->d_name);
			}
			error.store(true, std::memory_order_relaxed);
		}
		if(S_ISDIR(statbuf.st_mode)) {
			if ( !isdot(file->d_name) ) {				
				walkDirPar(file->d_name, comp, error);
				if (chdir("..") == -1) {
					perror("chdir");
					std::fprintf(stderr, "Error: chdir ..\n");
					error.store(true, std::memory_order_relaxed);
				}
			}
		} else {
			char *name_cp = strdup(file->d_name); 
			if (discardIt(name_cp, comp)) {
				if (QUITE_MODE>=2){
					if (comp) {
						std::fprintf(stderr, "%s has already a %s suffix -- ignored\n", name_cp, SUFFIX);
					} else {
						std::fprintf(stderr, "%s does not have a %s suffix -- ignored\n", name_cp, SUFFIX);
					}
				}							
			}
			else{
				#pragma omp task shared(error)
				if (!doWorkPar(name_cp, statbuf.st_size, comp)) error.store(true, std::memory_order_relaxed);
			}
		}
	}
    if (errno != 0) {
		if (QUITE_MODE>=1) perror("readdir");
		error.store(true, std::memory_order_relaxed);
    }
    closedir(dir);
}

#endif  // _UTILITY_HPP
