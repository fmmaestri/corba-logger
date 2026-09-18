CXX = g++
CXXFLAGS = -std=c++17 -Wall -I../idl
LDFLAGS = -lomniORB4 -lomniDynamic4 -lomnithread -lCOS4 -pthread

BIN = ../bin/cliente
OBJS = cliente.o ../idl/LoggerSK.o

.PHONY: all clean

all: $(BIN)

$(BIN): $(OBJS)
	@mkdir -p ../bin
	$(CXX) -o $@ $(OBJS) $(LDFLAGS)

cliente.o: cliente.cpp ../idl/Logger.hh
	$(CXX) $(CXXFLAGS) -c cliente.cpp -o cliente.o

../idl/LoggerSK.o: ../idl/LoggerSK.cc
	$(CXX) $(CXXFLAGS) -c ../idl/LoggerSK.cc -o ../idl/LoggerSK.o

clean:
	rm -f cliente.o $(BIN)
