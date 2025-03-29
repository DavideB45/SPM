CXX                = g++ -std=c++17
OPTFLAGS		   = -O3 -march=native -ffast-math
CXXFLAGS          += -Wall 
INCLUDES	   	   = -I.
LIBS               = -pthread
SOURCES            = $(wildcard *.cpp)
TARGET             = $(SOURCES:.cpp=)

.PHONY: all clean cleanall 

%: %.cpp
	$(CXX) $(INCLUDES) $(CXXFLAGS) $(OPTFLAGS) -o $@.out $< $(LIBS)

all: $(TARGET)

clean: 
	-rm -fr *.out *~
cleanall: clean
	-rm -fr $(TARGET)
