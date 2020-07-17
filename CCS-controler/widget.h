#ifndef WIDGET_H
#define WIDGET_H

#include <QTcpSocket>

#include "qcustomplot.h"
#include "axistag.h"
#include "cyhpidcontroller.h"
#include "lrxmodbusmaster.h"
#include "zypcontrolobject.h"

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
    void putData();

    void on_connectButton_clicked();

    void on_communicateButton_clicked();

    void on_PIDChangeButton_clicked();

    void on_followButton_clicked();

private:
    Ui::Widget *ui;
    double angle_X;
    double position_X;
    cyhPIDController *positionPIDController;
    cyhPIDController *anglePIDController;
    zypControlObject *conObj;
    lrxModbusMaster *mbMaster;
    bool mbConnectState;
    bool communicateEnable;
    float finalPosition;

    float currentPosition;
    float currentAngle;

    float uk;
    int serverID = 1;

};

#endif // WIDGET_H
