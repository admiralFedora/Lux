#ifndef METERWIDGET_H
#define METERWIDGET_H

#include "adjusters.h"
#include <QWidget>
#include <QPushButton>
#include <QLabel>
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


/*
    This class is the secondary widget that gets shown when the user clicks the top button to swap modes
*/

class meterWidget : public QWidget
{
    Q_OBJECT

public:
    explicit meterWidget(QWidget *parent = 0);
    ~meterWidget();


private:
    QVBoxLayout *overAll;
    QHBoxLayout *boxBox;
    Adjusters *iso;
    Adjusters *aperture;
    QPushButton *swap;
    QLabel *shutter;
    FILE* sensor;
    static void * staticEntryPoint(void *c);
	pthread_t thread;

public slots:
    void swapMode();
    void calculation();
};

#endif // METERWIDGET_H
