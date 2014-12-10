#include "controls.h"
#include <math.h>
#include <iostream>

controls::controls()
{ // control objects communicates to the motor
    turner = new motor(510,"/dev/motor");
    calib = 250.0; // according to wikipedia
    
}

int controls::calculate(double iso, double aperture, double shutter, double exposure)
{
    double luxRequired;
    int diff;
    int ret, lux;
    bool okay = true; // used to tell whether we've reached the value we wanted or not
    char bf[6];

    // read initial value from the sensor so we can figure out what direction we want to head 
    this->sensor = fopen("/proc/luxVal","r"); // hard-coded, that's where our proc file exists
    fread(bf,6,1,this->sensor);
    sscanf(bf,"%d",&lux);
    fclose(sensor);

    // run our calcuation
    luxRequired = ((calib)*pow(aperture+exposure,2.0)/(pow(shutter,-1)*iso));

    // figure out our difference
    diff = (int) (luxRequired - lux);

    printf("LuxRequired: %f, Diff: %d\n",luxRequired,diff);
    // making the enviornment brighter
    if(diff > 0)
    { // following loop runs until it either reaches within the 100 Lux of the request Lux value
        // or until the motor maxes out
        do
        {
            ret = turner->moveMotor('b',10);
            usleep(3*1E5L);
            if(ret != -5)
                turner->steps += 10;
            printf("ret: %d\n",ret);
            this->sensor = fopen("/proc/luxVal","r");
            fread(bf,6,1,this->sensor);
            sscanf(bf,"%d",&lux);
            fclose(sensor);
            diff = (int) (luxRequired - lux);
            std::cout<<"lux: "<< lux<<" diff:" << diff<< std::endl;
            printf("steps %d\n",turner->steps);
            
        }while(diff > 100 && (turner->steps != turner->maxStep));

        if(diff > 100)
            okay = false;
    }
    // making the enviornment dimmer
    else
    { // following loop runs until it either reaches within the 100 Lux of the request Lux value
        // or until the motor maxes out
        do
        {
           ret = turner->moveMotor('f',10);
           usleep(3*1E5L);
           if(ret != -5)
                turner->steps -= 10;
           printf("ret: %d\n",ret);
            this->sensor = fopen("/proc/luxVal","r");
            fread(bf,6,1,this->sensor);
            sscanf(bf,"%d",&lux);
            fclose(sensor);
            diff = (int) (luxRequired - lux);
            std::cout<<"lux: "<< lux<<" diff:" << diff<< std::endl;
            printf("steps %d\n",turner->steps);
        }while(diff < (-100) && (turner->steps != 0));

        if(diff < (-100))
            okay = false;
    }
    printf("ret: %d\n",ret);
    printf("total step: %d", turner->steps);

    // save block .////////////////////
    char buffer[10];

    FILE *sav = fopen("/etc/lux/stepsData","w");
    // write current position to file
    sprintf(buffer,"%d",this->turner->steps);
    printf("buffer %s\n",buffer);
    fwrite(buffer,10,1,sav);
    fclose(sav);
///////////////////////////////////////////

    // sends a signal back to the GUI thread to check whether the motor has gone to max or min
    emit errorOccurs(turner->steps, turner->maxStep,okay);
}

controls::~controls()
{
    fclose(this->sensor);
    delete(this->turner);
}
