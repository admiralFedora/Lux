#include "widget.h"
#include "meterWidget.h"
#include "ui_widget.h"
#include <QIcon>
#include <QPixmap>
#include "adjusters.h"
// added by Ted
#include <cstdio>
#include <unistd.h>
#include <cstdlib>
#include <signal.h>
#include <fcntl.h>
#include <string.h>
#include <pthread.h>
#include <iostream>
using namespace std;

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    //added by Ted
    cout << "creating widget..."<< endl;
	thread_alive = false;
    LampMode = true;
    motor_control = new controls();
	// stop adding
	
    ui->setupUi(this);
    iso = new Adjusters("iso");         
    shutter = new Adjusters("shutter");   
    aperture = new Adjusters("aperture"); 
    exposure = new Adjusters("exposure"); 
    calibrate = new QPushButton("Calibrate");
    swap = new QPushButton(QIcon(QPixmap(":/Images/camera.png")),"",0);
    alert = new QMessageBox();
    n = new meterWidget();

    overAll = new QVBoxLayout(this);
    boxBox = new QHBoxLayout();
    boxBox->addLayout(iso);
    boxBox->addLayout(shutter);
    boxBox->addLayout(aperture);
    boxBox->addLayout(exposure);
    overAll->addWidget(swap);
    overAll->addLayout(boxBox);
    overAll->addWidget(calibrate);
    overAll->setSpacing(10);
    swap->setIconSize(QSize(50,50));
    this->setStyleSheet("background-color: white;");

	// connect signal and slot
	connect(this->calibrate,SIGNAL(released()),this,SLOT(setTimer()));
    connect(this->motor_control,SIGNAL(errorOccurs(int,int,bool)),this,SLOT(showDialog(int,int,bool)));
    connect(this->swap,SIGNAL(released()),this,SLOT(swapMode()));
    showFullScreen();

	// stop adding
    cout << "widget creation complete..." << endl;
}

Widget::~Widget()
{
    delete ui;
}

// added by Ted

// our function that is to be on its own thread
void Widget::setParameter()
{
	int lux;
    // set our thread to be alive so we can make sure that we don't have two simulatenous threads
    thread_alive = true;
	sleep(3);
    // read lux reading from /proc/fortune
    lux = motor_control->calculate( iso->current, 
                                    aperture->current,
                                    shutter->current,
                                    exposure->current);
    // set our flag back to false so we can start a new thread since this one will die
	thread_alive = false;
}

// function necessary to handle the start of a thread
void * Widget::staticEntryPoint(void *c){
	((Widget *)c)->setParameter();
	return NULL;
}

// call-back function when the user clicks on calibrate
void Widget::setTimer(){
    // checks to see if we started a process to adjust the light
    // kills the process if it exists so we can start over again
    // we assume the user wants the most updated values to be used
	if (thread_alive){
		pthread_cancel(thread);
		thread_alive = false;
	}

    // thread creation
	int rc;
	rc = pthread_create(&thread,NULL, Widget::staticEntryPoint,this);
	
	
}

// our call-back function that the controls object throws at the end of its threaded function
void Widget::showDialog(int steps, int max, bool okay){
    if (steps == 0 && !okay){
        this->alert->setText("Lux value set to minimum, can't make enviornment darker.");
        this->alert->show();
    }
    else if (steps == max && !okay){
        this->alert->setText("Lux value set to maximum, unable to make enviornment any brighter");
        this->alert->show();
    }
    else if (!okay){
        this->alert->setText("Unknown Error Occured");
        this->alert->show();
    }
}

// our call-back function for the swap mode button that switches the visibility of the widgets
void Widget::swapMode(){
    //this->hide();
    n->show();
    n->raise();
    n->activateWindow();
    this->update();

}