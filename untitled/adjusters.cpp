#include "adjusters.h"
#include <QIcon>
#include <QPixmap>
#include <math.h>
#include <stdio.h>

Adjusters::Adjusters(QString text):QVBoxLayout()
{
    // initialization
    this->top = new QPushButton(QIcon(QPixmap(":/Images/up.png")),"",0);
    this->bottom = new QPushButton(QIcon(QPixmap(":/Images/down.png")),"",0);
    this->text = new QLabel(text);
    this->initValues(text);
    this->whoami = new QLabel(text);
	this->label = text;

    // adding widgets
    this->addWidget(this->whoami);
    this->addWidget(top);
    this->addWidget(this->text);
    this->addWidget(bottom);

    // styling
    top->setIconSize(QSize(50,50));
    bottom->setIconSize(QSize(50,50));
    top->setStyleSheet("QPushButton{border: none; outline: none;}");
    bottom->setStyleSheet("QPushButton{border: none; outline: none;}");
    bottom->setFixedWidth(20);
    top->setFixedWidth(20);
    this->setAlignment(whoami,Qt::AlignHCenter);
    this->setAlignment(bottom,Qt::AlignHCenter);
    this->setAlignment(top,Qt::AlignHCenter);
    this->setAlignment(this->text,Qt::AlignHCenter);


    // connecting our signal handlers
    connect(this->top,SIGNAL(released()),this,SLOT(handleUp()));
    connect(this->bottom,SIGNAL(released()),this,SLOT(handleDown()));
    
}

Adjusters::~Adjusters()
{
    delete top;
    delete bottom;
    delete text;
}

// this function calculates all the values required by the adjusters on construction 
//    of the object depending on the type of the adjuster
void Adjusters::initValues(QString text)
{
    char sample[30];
    int j = 0;
    if(text == "iso")
    {// iso simply doubles from 100
        for(double i = 100.0; i <= 3200.0; i = i*2.0)
        {
            values.push_back(i);
        }
    }
    else if(text == "aperture")
    { // aperture increases by 1.41^x
        for(double i = 1.0; i <= 9.0; i += 0.25)
        {
            values.push_back(pow(1.41,i));
        }
    }
    else if(text == "shutter")
    { // denominator value of the shutter decreases by 2 times starting from 4000
        for(int i = 4000; i >=1; i = i/2)
        {
            values.push_back((double)i);
        }
    }
    else if(text == "exposure")
    { // exposure compensation typically goes from -3 to +3
        for(double i = -3.0; i <= 3.0; i++)
        {
            values.push_back(i);
        }
    }

    std::list<double>::iterator i = values.begin();
    // we want the values to start at the middle of the list
    while(values.size()/2 != j++) i++;

    this->pos = i;
    current = *i;

    // depending on values you are adjusting, you will need to have different formats for printing
	if(text == "shutter")
		sprintf(sample,"1/%1g",current);
	else if(text == "aperture")
		sprintf(sample,"%.2f",current);
	else
    	sprintf(sample,"%1g",current);
    this->text->setText(QString(sample));
}


// following functions handle button presses to change values 

void Adjusters::handleUp()
{
    char temp[30];
    if(pos != (--values.end()))
    { // depending on values you are adjusting, you will need to have different formats for printing
		if(label == "shutter")
			sprintf(temp,"1/%1g",*(++pos));
		else if(label == "aperture")
			sprintf(temp,"%.2f",*(++pos));
		else
    		sprintf(temp,"%1g",*(++pos));
        this->text->setText(QString(temp));
    }
    current = *pos;
}

void Adjusters::handleDown()
{
    char temp[30];
    if(pos != values.begin())
    { // depending on values you are adjusting, you will need to have different formats for printing
        if(label == "shutter")
			sprintf(temp,"1/%1g",*(--pos));
		else if(label == "aperture")
			sprintf(temp,"%.2f",*(--pos));
		else
    		sprintf(temp,"%1g",*(--pos));
        this->text->setText(QString(temp));
    }
    current = *pos;
}
