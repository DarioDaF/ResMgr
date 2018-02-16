CPPFLAGS=-std=c++11 -Wall -I"./include"
ARFLAGS=rcs

all: ./bin/libResMgr.a

./bin/libResMgr.a: ./src/ResMgr.o
	$(AR) $(ARFLAGS) $@ $?

./src/*.o: ./src/*.cpp
	$(CPP) $(CPPFLAGS) -o $@ $^

clean:
	rm ./bin/libResMgr.a
	rm ./src/*.o
