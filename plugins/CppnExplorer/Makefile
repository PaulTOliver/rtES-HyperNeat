LIBRARY := lib/libCppnExplorer.a
SOURCES := $(wildcard src/*.cpp)
OBJECTS := $(patsubst src/%.cpp,obj/%.o,$(SOURCES))
DEPS    := $(patsubst %.o,%.d,$(OBJECTS))
CFLAGS  := -c -O3 -Wall -std=c++11 -MMD -Iinclude -I../../include -IC:/SFML/include
LFLAGS  := rvs

all: $(OBJECTS)
	ar $(LFLAGS) $(LIBRARY) $(OBJECTS)

-include $(DEPS)

$(OBJECTS): $(patsubst obj/%.o,src/%.cpp,$@)
	g++ $(CFLAGS) $(patsubst obj/%.o,src/%.cpp,$@) -o $@

clean:
	del lib\*.a
	del obj\*.o
	del obj\*.d
