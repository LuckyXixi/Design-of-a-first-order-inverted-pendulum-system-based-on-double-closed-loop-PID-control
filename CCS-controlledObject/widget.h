#ifndef WIDGET_H
#define WIDGET_H

#include "zypcontrolobject.h"
#include "lrxmodbusslave.h"

#include <QWidget>

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();

private slots:
    void on_connectButton_clicked();

private:
    Ui::Widget *ui;
    zypControlObject *conObj;
    float finalPosition;
    float currentPosition;
    float currentAngle;
    float uk;
    QString strFinalPosition;
    QString strCurrentPosition;
    QString strCurrentAngle;
    QString struk;

    lrxModbusSlave *mbSlave;

    bool mbConnectState;
    bool communicateEnable;

private slots:
    void putData();
    void on_setValueButton_clicked();
};

#endif // WIDGET_H
