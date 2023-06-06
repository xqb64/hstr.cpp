SRC := $(wildcard src/*.cpp)

CC := g++

CXXFLAGS += -g
CXXFLAGS += -Wshadow -Wall -Wextra -Wpedantic
CXXFLAGS += -Wswitch-enum
CXXFLAGS += -O3
CXXFLAGS += --std=c++20
LDLIBS = -lncursesw

obj/%.o: src/%.cpp $(wildcard src/*.h)
	mkdir -vp obj && $(CC) -c $(CXXFLAGS) $< -o $@

all: hstr

hstr: $(SRC:src/%.cpp=obj/%.o)
	$(CC) $(CXXFLAGS) $^ -o $@ $(LDLIBS)

clean:
	rm -rvf obj hstr
