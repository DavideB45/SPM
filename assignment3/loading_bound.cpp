#include <config.hpp>
#include <cmdline.hpp>
#include <utility.hpp>

#include <hpc_helpers.hpp>

int main(int argc, char *argv[]) {
	if (argc < 2) {
		usage(argv[0]);
		return -1;
	}
	// parse command line arguments and set some global variables
	long start=parseCommandLine(argc, argv);
	if (start<0) return -1;

	bool success = true;
	
	TIMERSTART(list_files);
	// define all the files to process
	std::vector<std::pair<char*, size_t>> files;
	for (int i = start; i < argc; ++i) {
		size_t filesize=0;
		if(isDirectory(argv[i], filesize)) {
			// recursively add files to the list
			// TODO: implement a function to add files to the list
			success &= expandDir(argv[i], files);
		} else {
			files.emplace_back(argv[i], filesize);
		}
	}
	TIMERSTOP(list_files);

	unsigned char *ptrs[files.size()];

	TIMERSTART(map_files);
	// map all the files in memory
	for (size_t i = 0; i < files.size(); i++){
		if (!mapFile(files[i].first, files[i].second, ptrs[i])) {
			success = false;
			break;
		}
	}
	TIMERSTOP(map_files);

	TIMERSTART(duplicate)
	//#pragma omp parallel for num_threads(8)
	for (size_t i = 0; i < files.size(); i++) {
		std::string originalFileName = files[i].first;
		std::string copyFileName = originalFileName + ".zip";

		FILE *copyFile = fopen(copyFileName.c_str(), "wb");
		if (!copyFile) {
			perror(("Failed to create file: " + copyFileName).c_str());
			success = false;
		}

		if (fwrite(ptrs[i], 1, files[i].second, copyFile) != files[i].second) {
			perror(("Failed to write to file: " + copyFileName).c_str());
			success = false;
			fclose(copyFile);
		}

		fclose(copyFile);
	}
	TIMERSTOP(duplicate)
	
	TIMERSTART(unmap_files);
	// unmap all the files
	for (size_t i = 0; i < files.size(); i++){
		unmapFile(ptrs[i], files[i].second);
	}
	TIMERSTOP(unmap_files);
	


	if (!success) {
		printf("Exiting with (some) Error(s)\n");
		return -1;
	}
	printf("Exiting with Success\n");
	return 0;
}
 