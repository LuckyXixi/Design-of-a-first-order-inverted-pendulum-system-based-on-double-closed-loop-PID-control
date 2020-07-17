#include "widget.h"
#include "ui_widget.h"

#include <QUrl>
#include <QTimer>
#include <QMovie>
#include <QDebug>

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
    finalPosition = 0;
    currentPosition = 0;
    currentAngle = 0;
    uk=0;
    conObj = new zypControlObject(uk, currentPosition, currentAngle);

    if(ui->IPLineEdit->text().isEmpty())
        ui->IPLineEdit->setText(QLatin1Literal("127.0.0.1:8086"));

    mbSlave = new lrxModbusSlave();

    communicateEnable = false;

    QMovie *movie = new QMovie(":/controlFigure.png");
    ui->photoLabel->setMovie(movie);
    movie->start();

    QTimer *timer = new QTimer;
    connect(timer,&QTimer::timeout,this,&Widget::putData);
    timer->start(25);
}

Widget::~Widget()
{
    delete ui;
}

void Widget::putData()
{
    if(communicateEnable){
       if(!mbSlave->getIsFree()){
            uk = mbSlave->getUk();
            conObj->objectWork(uk);

            currentPosition = conObj->getPosition();
            currentAngle = conObj->getAngle();

            mbSlave->setPosition(currentPosition);
            mbSlave->setAngle(currentAngle);
            mbSlave->enableFreeFlag();
        }

        strCurrentPosition = QString::number(currentPosition,'f',4);
        strCurrentAngle = QString::number(currentAngle*180,'f',4);
        struk = QString::number(uk,'f',4);

        ui->positionLineEdit->setText(strCurrentPosition);
        ui->angleLineEdit->setText(strCurrentAngle);
        ui->controllerOutLineEdit->setText(struk);

    }


}

void Widget::on_connectButton_clicked()
{
    if(ui->connectButton->text() == "Connect"){
        communicateEnable = true;
        ui->connectStateLineEdit->setText("Connected");
        QUrl url = QUrl::fromUserInput(ui->IPLineEdit->text());
        mbSlave->listenSocketConn(url, ui->slaveIDLineEdit->text());

        ui->connectButton->setText("Disconnect");
    }else{
        communicateEnable = false;
        ui->connectStateLineEdit->setText("Not Connect");
        ui->connectButton->setText("Connect");
        mbSlave->disconnectSocketConn();
    }
}


void Widget::on_setValueButton_clicked()
{
    if(!ui->setValueLineEdit->text().isEmpty()){
        strFinalPosition = ui->setValueLineEdit->text();
        finalPosition = strFinalPosition.toFloat();
    }
}
