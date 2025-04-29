CXX		= g++ -std=c++20
INCLUDES	= -I . -I miniz
CXXFLAGS  	+= -Wall

LDFLAGS 	= -pthread -fopenmp
OPTFLAGS	= -O3 -ffast-math -DNDEBUG

TARGETS		= minizseq minizpar minizforpar testWorkingAffinity

.PHONY: all clean cleanall
.SUFFIXES: .cpp 


%: %.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(OPTFLAGS) -o $@.a $< ./miniz/miniz.c $(LDFLAGS)

all		: $(TARGETS)

minizseq: minizseq.cpp cmdline.hpp utility.hpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(OPTFLAGS) -o $@.a $< ./miniz/miniz.c

minizseqpar: minizpar.cpp cmdline.hpp utility.hpp

minizforpar: minizforpar.cpp cmdline.hpp utility.hpp

testWorkingAffinity: testWorkingAffinity.cpp cmdline.hpp utility.hpp

clean		: 
	rm -f *.a 
cleanall	: clean
	\rm -f *.o *~
