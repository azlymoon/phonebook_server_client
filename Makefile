GCC_FLAGS = -Wextra -Werror -Wall -Wno-gnu-folding-constant -I include/ -ljsoncpp

all: obj/handler.o obj/server.o
	g++ $(GCC_FLAGS) obj/server.o obj/handler.o -o server

obj/server.o: server.cpp | obj
	g++ $(GCC_FLAGS) -c server.cpp -o obj/server.o

obj/handler.o: handler.cpp | obj
	g++ $(GCC_FLAGS) -c handler.cpp -o obj/handler.o

obj:
	mkdir obj

.PHONY: clean
clean:
	rm -rf obj
