
all: serverthread serverfork



serverthread.o: serverthread.cpp
	$(CXX) -std=c++17 -Wall -c -O2 serverthread.cpp -I.

serverfork.o: serverfork.cpp
	$(CXX) -std=c++17 -Wall -c  -O2 serverfork.cpp -I.


serverfork: serverfork.o 
	$(CXX) -L./ -Wall -o serverfork serverfork.o

serverthread: serverthread.o 
	$(CXX) -L./ -Wall -o serverthread serverthread.o -lpthread


clean:
	rm *.o *.a perf_*.txt  tmp.* serverfork serverthread
