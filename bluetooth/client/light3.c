#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include "i2c-dev.h"
#include <errno.h>
#include <string.h>
#include <time.h>
#include "luxcalc.h"
#include <sys/socket.h>
#include <error.h>
#include <bluetooth/bluetooth.h> //from bluez library
#include <bluetooth/rfcomm.h> //from bluez library


#define COMMAND 0x80
#define CONTROL 0x00
#define ON 0x03
#define OFF 0x00
#define TIMING 0x01
#define WORD 0x20
#define LOW0 0x0C
#define HIGH0 0x0D
#define LOW1 0x0E
#define HIGH1 0x0F
#define GAIN 0x00
#define ID 0x0A
#define BLOCK 0x10

/*
	This is a service application that reads data from the TSL2561 and calculates the Lux.
	Values are written to the application folder under /etc/lux/
*/

#define BDADDR_SERVER (&(bdaddr_t) {{0x23,0x2B,0x50,0x19,0x03,0x00}});

int main()
{
	int file;
	volatile unsigned addr = 0x29;
	char *filename = "/dev/i2c-0";
	unsigned buf,buf2, channel0, channel1;
	int Lux;
	int avgLux;
	char str[10];
	int loop;
	int avg;
	// bluetooth connection setup
	struct sockaddr_rc bluetooth_addr;
	int cc, status;

	cc = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);
	if(cc < 0){
		perror("Socket failed");
		exit(1);
	}
	// set the connection parameters (who to connect to)
	bluetooth_addr.rc_family = AF_BLUETOOTH;
	bluetooth_addr.rc_channel = (uint8_t) 6;
	bluetooth_addr.rc_bdaddr = *BDADDR_SERVER;
	// connect to server
	do{
		sleep(1);
		status = connect(cc, (struct sockaddr *)&bluetooth_addr, sizeof(bluetooth_addr));
		if(status < 0){
			printf("connection failed\n");
		}
	}while(status < 0);
	printf("connection success\n");
	// end of bluetooth setup. 


	avg = 3;
	if((file = open(filename, O_RDWR)) < 0)
	{
		printf("Failed to open\n");
		exit(1);
	}
	if (ioctl(file,I2C_SLAVE_FORCE,addr) < 0)
	{
		printf("Failed\n");
		exit(1);
	}

	i2c_smbus_write_byte_data(file,COMMAND|CONTROL,ON);

	i2c_smbus_write_byte_data(file,COMMAND|TIMING,GAIN|TIMING);

	buf = i2c_smbus_read_byte_data(file,COMMAND|ID);
	printf("ID %x\n",buf);
	while(1)
	{
		printf("loop\n");
		avgLux = 0;
		loop = 0;
		while(loop<avg)
		{
			i2c_smbus_write_byte_data(file,COMMAND|CONTROL,ON);
			usleep(101000);
			
			/*buf2 = i2c_smbus_read_byte_data(file,COMMAND|WORD|LOW0);
			buf = i2c_smbus_read_byte_data(file,COMMAND|WORD|HIGH0);
			buf <<= 8;
			buf |= buf2;
			channel0 = buf;*/
			channel0 = i2c_smbus_read_word_data(file,COMMAND|WORD|LOW0);

			/*buf2 = i2c_smbus_read_byte_data(file,COMMAND|WORD|LOW1);
			buf = i2c_smbus_read_byte_data(file,COMMAND|WORD|HIGH1);
			buf <<= 8;
			buf |= buf2;
			channel1 = buf;*/
			channel1 = i2c_smbus_read_word_data(file,COMMAND|WORD|LOW1);


			Lux = CalculateLux(0,1,channel0,channel1,0);
			avgLux += Lux;
			
			loop++;
			i2c_smbus_write_byte_data(file,COMMAND|CONTROL,OFF);
			printf("avgLux %d\n",avgLux);
		}

		avgLux = avgLux/avg;
		if(avgLux >= 40000)
			avgLux = 500;
		else if (avgLux == 0)
			avgLux = 100;
		sprintf(str,"%d",avgLux);
		printf("sending data: %s \n",str);
		
		status = write(cc, str, sizeof(str));

		
		if( status < 0 ){
			perror("data sending failed\n");
			exit(1);
		}
	}

	close(file);
}
