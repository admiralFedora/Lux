CC=arm-linux-gcc
FLAGS= -L $(EC535)/gumstix/bluez/bluez-libs/usr/lib -I ${EC535}/gumstix/bluez/include/bluetooth -lbluetooth
light3: light3.o luxcalc.o
	$(CC) $(FLAGS) light3.o luxcalc.o -s -o light3
luxcalc.o: luxcalc.c
	$(CC) -c luxcalc.c -o luxcalc.o
light3.o: light3.c
	$(CC) $(FLAGS) -c light3.c
clean:
	rm -rf *.o light3
