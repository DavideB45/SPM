CXXFLAGS            += -std=c++20

OPTIMIZE_FLAGS  += -O3 -ffast-math

INCS            += -I ./fastflow -I .
CXXFLAGS            += -Wall
LIBS                 = -pthread
INCLUDES             = $(INCS)

SOURCES              = $(wildcard *.cpp)
TARGET               = $(SOURCES:.cpp=)

.PHONY: all clean cleanall 
.SUFFIXES: .c .cpp .o

%.d: %.cpp
	set -e; $(CXX) -MM $(INCLUDES) $(CXXFLAGS) $< \
		| sed 's/\($*\)\.o[ :]*/\1.o $@ : /g' > $@; \
		[ -s $@ ] || rm -f $@
%.d: %.c
	set -e; $(CC) -MM $(INCLUDES) $(CFLAGS)  $< \
		| sed 's/\($*\)\.o[ :]*/\1.o $@ : /g' > $@; \
		[ -s $@ ] || rm -f $@
%.o: %.c
	$(CC) $(INCLUDES) $(CFLAGS) -c -o $@ $<
%: %.cpp
	$(CXX) $(INCLUDES) $(CXXFLAGS) $(OPTIMIZE_FLAGS) -o $@.a $< $(LDFLAGS) $(LIBS)

all: $(TARGET)



clean: 
	-rm -fr *.o *~
cleanall: clean
	-rm -fr $(TARGET) *.d ./socket*

include $(OBJS:.o=.d)