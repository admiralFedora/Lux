#ifndef WIDGET_H
#define WIDGET_H

#include "adjusters.h"
#include <QWidget>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QMessageBox>
#include <controls.h>  // added by Ted
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <fcntl.h>
#include <unistd.h>
#include "meterWidget.h"

/*
    This class is the main widget that gets shown to the user at start up
*/

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();
    void setParameter(); // added by Ted
	static void * staticEntryPoint(void *c);


private:
    QVBoxLayout *overAll;
    QHBoxLayout *boxBox;
    Adjusters *iso;
    Adjusters *shutter;
    Adjusters *aperture;
    Adjusters *exposure;
    QPushButton *calibrate;
    QPushButton *swap;
    QMessageBox *alert;
    Ui::Widget *ui;
    controls *motor_control; // added by Ted
	bool thread_alive;
	pthread_t thread;
    bool LampMode;
    meterWidget *n;
	
    
public slots:
    void setTimer();
    void showDialog(int,int,bool);
    void swapMode();
};

#endif // WIDGET_H
