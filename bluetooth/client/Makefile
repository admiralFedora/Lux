CC=arm-linux-gcc
FLAGS= -L $(EC535)/gumstix/bluez/bluez-libs/usr/lib -I ${EC535}/gumstix/bluez/include/bluetooth -lbluetooth
client: client.o luxcalc.o
	$(CC) $(FLAGS) client.o luxcalc.o -s -o client
luxcalc.o: luxcalc.c
	$(CC) -c luxcalc.c -o luxcalc.o
client.o: client.c
	$(CC) $(FLAGS) -c client.c
clean:
	rm -rf *.o client
