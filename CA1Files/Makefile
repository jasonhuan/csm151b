cpusim: cpusim.cpp
	g++ -std=c++11 *.cpp -o cpusim

debug: cpusim trace/debug/instMem.txt
	./cpusim trace/debug/instMem.txt

test1: cpusim trace/test/instMem1.txt
	./cpusim trace/test/instMem1.txt

test2: cpusim trace/test/instMem2.txt
	./cpusim trace/test/instMem2.txt

clean: cpusim
	rm ./cpusim
