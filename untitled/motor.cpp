#include "motor.h"
#include <stdlib.h>
#include <cstring>
#include <stdio.h>
#include <ctype.h>
#include <signal.h>
#include <fcntl.h>
#include <unistd.h>
motor::motor(int max, char* device)
{
    char buffR[4];
    char buffW[5];

    this->deviceName = device; // save the character device location

    this->save = fopen("/etc/lux/stepsData","r"); // we want to load in saved data in the case the user loses power to the board and hasn't resetted the switch back to 0
    if(!this->dev)
        printf("no device found\n");
    if(!this->save)
    {
        this->steps = 0;
    }
    else
    {
        printf("reading data\n");
        fread(buffR,4,1,this->save);
        this->steps = atoi(buffR);
        printf("got steps %d\n",this->steps);
    }

    this->maxStep = max;
}

int motor::getSteps()
{
    return steps;
}

motor::~motor()
{
    char buffer[10];

    fopen("/etc/lux/stepsData","w");
    // write current position to file
    fread(buffer,10,1,this->dev);
    fwrite(buffer,10,1,save);

    fclose(this->dev);
    fclose(this->save);
}


// error codes
// -1 can't move motor past pos 0
// 0 a-okay
// 1 can't move motor past max pos
// 2 write error
int motor::moveMotor(char dir, int step)
{
    // commanding the motor
    // EX: 'f10' - foward 10 steps
    // EX: 'b50' - backwards 50 steps
    char command[4];

    if(!this->dev)
        printf("file llost \n");

    if(dir == 'f' && steps == 0)
    {
        return -5;
    }
    else if(dir == 'b' && steps == maxStep)
    {
        return -5;
    }


    if(dir == 'f' && steps-step < 0) // prevent the motor from going beyond off pos
    {
        step = steps;
    }
    else if(dir == 'b' && steps+step >= maxStep) // prevent the motor from going beyond the max pos
    {
        step = maxStep - steps;
    }
    sprintf(command,"%cg%d",dir,step);
    printf("%s\n",command);
    this->dev = fopen(this->deviceName,"w");
    
    int ret = fwrite(command,sizeof(command),1,dev);
    fclose(this->dev);
    if(ret != sizeof(command))
    {
        printf("fwrite: %d\n",ret);
        return 2;
    }
    return 0;
}

