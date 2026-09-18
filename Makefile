.PHONY: all idl cliente servidor clean

all: idl cliente servidor

idl:
	$(MAKE) -C idl

cliente: idl
	$(MAKE) -C cliente

servidor: idl
	$(MAKE) -C servidor

clean:
	$(MAKE) -C idl clean
	$(MAKE) -C cliente clean
	$(MAKE) -C servidor clean
