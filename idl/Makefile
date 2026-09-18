IDL = Logger.idl
GERADOS = Logger.hh LoggerSK.cc

.PHONY: all clean

all: $(GERADOS)

$(GERADOS): $(IDL)
	omniidl -bcxx $(IDL)

clean:
	rm -f Logger.hh LoggerSK.cc *.o
