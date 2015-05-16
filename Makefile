CC=cc
INCLUDES=-I./lib
PWD=`pwd`
CCFLAGS=-g -Wall $(INCLUDES) -Wl,-rpath=$(PWD)/build/lib -v
LDFLAGS=-Lbuild/lib
all:
	mkdir -p build/lib build/sbin pkg
	$(CC) -c -fpic -o build/lib/libgpio_control.o $(CCFLAGS) lib/libgpio_control.c 
	$(CC) -shared -o build/lib/libgpio_control.so $(CCFLAGS) build/lib/libgpio_control.o
	$(CC) display.c -o build/sbin/display -lpthread  $(CCFLAGS) $(LDFLAGS) -lgpio_control	
	rm build/lib/*.o
	chmod 755 build/lib/*.so
clean:
	rm -Rf build
	rm -Rf pkg
tar:
	tar cpvf pkg/libgpio_control.tar.gz -C ./build .
