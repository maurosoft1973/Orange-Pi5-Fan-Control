all: orange-pi5-fan-control clean
install: orange-pi5-fan-control install clean

.ONESHELL:
orange-pi5-fan-control:
	gcc -o orange-pi5-fan-control src/library/argparse.c src/orange-pi5-fan-control.c -lwiringPi -Wmultichar

install:
	chmod +x orange-pi5-fan-control
	mv orange-pi5-fan-control /usr/local/bin/orange-pi5-fan-control

.PHONY: clean
clean:
	rm -rf src/*.o
	rm -rf src/library/*.o
