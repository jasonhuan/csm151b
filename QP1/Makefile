bpred: bpred.cpp
	g++ -std=c++11 *.cpp -o bpred

a: bpred test.txt
	./bpred test.txt 0

b: bpred test.txt
	./bpred test.txt 1

c: bpred test.txt
	./bpred test.txt 2

clean: cpusim
	rm ./bpred
