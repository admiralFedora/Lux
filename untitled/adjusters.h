#ifndef ADJUSTERS_H
#define ADJUSTERS_H

#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QString>
#include <list>

/*
    This class houses the adjusters the user can use to change the settings
    It also houses the values selected
*/


class Adjusters : public QVBoxLayout
{
    Q_OBJECT

public:
    double current;
    QPushButton *top;
    QPushButton *bottom;
    Adjusters(QString text); // changed by Ted
    ~Adjusters();

private:
    QLabel *whoami;
    QLabel *text;
	QString label;
    std::list<double> values;
    std::list<double>::iterator pos;

    void initValues(QString text);
private slots:
    void handleUp();
    void handleDown();
};

#endif // ADJUSTERS_H
