#ifndef MOTOR_H
#define MOTOR_H

#include <stdio.h>

/*
	Max steps for this motor while attached to dimmer is 510
    
    This is a class for controlling the motor from the GUI
    Communicates to the character device driver for the motor
*/

class motor
{
public:
    motor(int max, char* device);
    int moveMotor(char dir,int step);
    int getSteps();
    int steps; // steps away from the off position
    ~motor();
    int maxStep; // light switch does not do a full rotation
    char *deviceName;
    FILE *dev;
    FILE *save;
};

#endif // MOTOR_H
