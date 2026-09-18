CXX = g++
CXXFLAGS = -std=c++17 -Wall -I../idl
LDFLAGS = -lomniORB4 -lomniDynamic4 -lomnithread -lCOS4 -pthread

BIN = ../bin/servidor
OBJS = servidor.o LoggerI.o ../idl/LoggerSK.o

.PHONY: all clean

all: $(BIN)

$(BIN): $(OBJS)
	@mkdir -p ../bin
	$(CXX) -o $@ $(OBJS) $(LDFLAGS)

servidor.o: servidor.cpp LoggerI.h ../idl/Logger.hh
	$(CXX) $(CXXFLAGS) -c servidor.cpp -o servidor.o

LoggerI.o: LoggerI.cpp LoggerI.h ../idl/Logger.hh
	$(CXX) $(CXXFLAGS) -c LoggerI.cpp -o LoggerI.o

../idl/LoggerSK.o: ../idl/LoggerSK.cc
	$(CXX) $(CXXFLAGS) -c ../idl/LoggerSK.cc -o ../idl/LoggerSK.o

clean:
	rm -f servidor.o LoggerI.o $(BIN)
