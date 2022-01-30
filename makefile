.PHONY: all cls build test docs


all: cls build


cls:
	clear

build:
	$(CXX) Src/ledittest.cpp -o a.exe

test:
	$(CXX) test/grtest.cpp -o a2.exe $(shell pkg-config readline --cflags --libs)
	$(CXX) test/keytest.cpp -o a3.exe $(shell pkg-config readline --cflags --libs)

docs:
	doxygen