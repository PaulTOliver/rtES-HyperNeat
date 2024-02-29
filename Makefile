LIBRARY := lib/libHyperNeat.a
SOURCES := $(wildcard src/*.cpp) $(wildcard src/Utils/*.cpp)
OBJECTS := $(patsubst src/%.cpp,obj/%.o,$(SOURCES))
DEPS    := $(patsubst %.o,%.d,$(OBJECTS))
CFLAGS  := -c -O3 -Wall -std=c++11 -MMD -Iinclude -IC:/cereal/include
LFLAGS  := rvs

all: $(OBJECTS)
	ar $(LFLAGS) $(LIBRARY) $(OBJECTS)
	make -C plugins/CppnExplorer

-include $(DEPS)

$(OBJECTS): $(patsubst obj/%.o,src/%.cpp,$@)
	g++ $(CFLAGS) $(patsubst obj/%.o,src/%.cpp,$@) -o $@

rebuild: clean all

clean:
	del lib\*.a
	del obj\*.o
	del obj\*.d
	del obj\Utils\*.o
	del obj\Utils\*.d
