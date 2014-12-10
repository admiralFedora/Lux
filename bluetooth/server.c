#include <stdio.h>
#include <error.h>
#include <sys/socket.h>
#include <bluetooth/bluetooth.h> //from bluez library
#include <bluetooth/rfcomm.h> //from bluez library

/*
	Service level program for the Bluetooth Server (Where the GUI resides)
	Make sure that this program is backgrounded otherwise it will proceed to block everything
*/



// hard-coded address of the client that we want to connection
// icky, but it works; strangely the address is read in 2 byte blocks from right to left... definitely not an endinaness issue....
#define BDADDR_CLIENT	(&(bdaddr_t) {{0x45,0x29,0x50,0x19,0x03,0x00}})


int main()
{
	//RFCOMM structure
	struct sockaddr_rc myAddress, remoteAddress;
	int listenfd, client, bytes_read;
	char buf[100];
	char str[10];
	FILE * pFile;
	listenfd = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);
	if(listenfd < 0){
		perror("RFCOMM Socket");
		return 1;
	}
	myAddress.rc_family = AF_BLUETOOTH;
	myAddress.rc_bdaddr = *BDADDR_ANY;
	myAddress.rc_channel = (uint8_t) 6; //defining binding port 6, cause everyone else is boring and using channel 1

	remoteAddress.rc_bdaddr = *BDADDR_CLIENT; // define the address of the remote address we want to connect to

	bind(listenfd, (struct sockaddr *)&myAddress, sizeof(myAddress));
	// listen to socket
	// following function will block until a sucessful connection occurs
	listen(listenfd, 6);
	//accept the requested connection
	int fsize = sizeof(remoteAddress);
	client = accept(listenfd, (struct sockaddr *)&remoteAddress, &fsize);
	memset(buf, 0, sizeof(buf));
	/*
	ba2str takes 6-byte remoteAddress.rc_bdaddr and copy it into buf
	*/
	listenfd = ba2str( &remoteAddress.rc_bdaddr, buf );
	if(listenfd<0){
		perror("ba2Str");
	}
	printf("Connected to '%s'\n",buf);
	
	// loop will run forever once a connection is received
	while (1){
		pFile = fopen("/proc/luxVal", "w");
		memset(buf, 0, sizeof(buf));
		// read data from the client
		bytes_read = read(client, buf, sizeof(buf));

		if(!pFile)
			printf("file error\n");
		// write data to our proc file system
		fwrite(buf,1,sizeof(buf),pFile);
		fclose(pFile);

		//printf("received '%s'\n",buf);
	}
	// close connection
	// never happens...
	close(client);
	close(listenfd);
	return 0;
}
