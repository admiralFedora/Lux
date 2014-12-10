CC=arm-linux-gcc
light: light3.o luxcalc.o
	$(CC) light3.o luxcalc.o -s -o light3
light.o: light3.c
	$(CC) -c light3.c
luxcalc.o: luxcalc.c
	$(CC) -c luxcalc.c -o luxcalc.o
clean:
	rm -rf *.o light3
