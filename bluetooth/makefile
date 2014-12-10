CC=arm-linux-gcc
FLAGS= -L $(EC535)/gumstix/bluez/bluez-libs/usr/lib -I ${EC535}/gumstix/bluez/include/bluetooth -lbluetooth
server: server.o
	$(CC) $(FLAGS) server.o -s -o server
server.o: server.c
	$(CC) $(FLAGS) -c server.c
clean:
	rm -rf *.o server
