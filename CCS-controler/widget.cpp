#include "widget.h"
#include "ui_widget.h"
#include "cclopengl3d.h"


#include <QTimer>
#include <QtCore/qmath.h>
#include <QUrl>
#include <QDebug>

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);

    uk = 1.0;
    serverID = 0;

    if(ui->IPLineEdit->text().isEmpty())
        ui->IPLineEdit->setText(QLatin1Literal("127.0.0.1:1502"));

    mbMaster = new lrxModbusMaster();
    mbConnectState = false;
    communicateEnable = false;

    conObj = new zypControlObject(0,0,0);

    finalPosition = 0.5;
    currentPosition = 0.0;
    currentAngle = 0.0;

    positionPIDController = new cyhPIDController(0.0001, 0.01, -1);
    anglePIDController = new cyhPIDController(0.0001, 0.1, 1);

    QTimer *timerCurve = new QTimer(this);
    connect(timerCurve, &QTimer::timeout, this, &Widget::putData);
    timerCurve->start(25);

    connect(ui->upButton, &QPushButton::clicked, ui->openGL3DWidget, &cclOpenGL3D::upRotate);
    connect(ui->downButton, &QPushButton::clicked, ui->openGL3DWidget, &cclOpenGL3D::downRotate);
    connect(ui->leftButton, &QPushButton::clicked, ui->openGL3DWidget, &cclOpenGL3D::leftRotate);
    connect(ui->rightButton, &QPushButton::clicked, ui->openGL3DWidget, &cclOpenGL3D::rightRotate);
    connect(ui->resetButton, &QPushButton::clicked, ui->openGL3DWidget, &cclOpenGL3D::resetRotate);

    QPalette P=ui->connectButton->palette();
    P.setColor(QPalette::Window,Qt::green);
    P.setBrush(QPalette::Window,Qt::blue);
    P.setColor(QPalette::ButtonText,Qt::white);
    QFont font;
    font.setFamily(QString::fromUtf8("Arial"));
    font.setPointSize(11);
    font.setBold(true);
    font.setWeight(75);

    ui->connectButton->setPalette(P);
    ui->connectButton->setBackgroundRole(QPalette::Button);
    ui->connectButton->setAutoFillBackground(true);
    ui->connectButton->setStyleSheet("background-color:rgb(70,207,45)");
    ui->connectButton->setFont(font);

    ui->communicateButton->setPalette(P);
    ui->communicateButton->setBackgroundRole(QPalette::Button);
    ui->communicateButton->setAutoFillBackground(true);
    ui->communicateButton->setFont(font);
    ui->communicateButton->setStyleSheet("background-color:rgb(255,0,0)");

    P.setColor(QPalette::ButtonText,Qt::black);
    ui->followButton->setPalette(P);

    ui->followButton->setBackgroundRole(QPalette::Button);
    ui->followButton->setAutoFillBackground(true);
    ui->followButton->setFont(font);
    ui->followButton->setStyleSheet("background-color:rgb(231,245,103)");

    ui->followButton->setText("Notfollow");

}

void Widget::putData()
{
    mbConnectState = mbMaster->getConnState();

    if(mbConnectState && communicateEnable){
        ui->connectButton->setText("DisConnect");
        mbMaster->readFlagData(serverID);
        if(mbMaster->getIsSlaveFree()){
            mbMaster->readData(serverID);
            currentPosition = mbMaster->getPosition();
            currentAngle = mbMaster->getAngle();
            uk = positionPIDController->PIDOutput(finalPosition, currentPosition) - \
                    anglePIDController->PIDOutput(0,currentAngle);
            qDebug()<<"uk:"<<uk;
            mbMaster->setUk(uk,serverID);
            mbMaster->disableFreeFlag(serverID);
        }
    }else if(!mbConnectState && communicateEnable){
        QMessageBox::information(this, QString::fromLocal8Bit("警告"),QString::fromLocal8Bit("modbus通信失败,请检查IP:端口,serverId是否配置正确,modbus从机是否处于监听状态"));
        ui->connectButton->setText("Connect");
        communicateEnable = false;
        ui->communicateButton->setText("Start");

        QPalette P=ui->connectButton->palette();
        P.setColor(QPalette::Window,Qt::green);
        P.setBrush(QPalette::Window,Qt::blue);
        P.setColor(QPalette::ButtonText,Qt::white);
        ui->communicateButton->setPalette(P);
        ui->communicateButton->setBackgroundRole(QPalette::Button);
        ui->communicateButton->setAutoFillBackground(true);
        ui->communicateButton->setStyleSheet("background-color:rgb(255,0,0)");
    }


    ui->openglCurveWidget->setControllerOutPut(uk);
    ui->openglCurveWidget->setVehiclePosition(currentPosition);
    ui->openglCurveWidget->setPulleyAngle(currentAngle);
    ui->openglCurveWidget->animate();

    ui->openGL3DWidget->setVehiclePosition(currentPosition);
    ui->openGL3DWidget->setPulleyAngle(currentAngle);
    ui->openGL3DWidget->animate();
}

Widget::~Widget()
{
    mbMaster->shutdownMaster();
    delete ui;
}



void Widget::on_connectButton_clicked()
{
    //Connect btn
    if(ui->connectButton->text() == "Connect"){
        serverID = ui->serverIDBox->currentIndex();
        qDebug() <<"serverId:"<< serverID;
        QUrl url = QUrl::fromUserInput(ui->IPLineEdit->text());
        qDebug() << url;
        if(!mbMaster->connectSocketConn(url)){
            QMessageBox::information(this, QString::fromLocal8Bit("警告"),QString::fromLocal8Bit("modbus连接请求失败"));
        }else{
           ui->connectButton->setText("Connecting");
           QPalette P=ui->connectButton->palette();
           P.setColor(QPalette::Window,Qt::green);
           P.setBrush(QPalette::Window,Qt::blue);
           P.setColor(QPalette::ButtonText,Qt::white);
           ui->connectButton->setPalette(P);
           ui->connectButton->setBackgroundRole(QPalette::Button);
           ui->connectButton->setAutoFillBackground(true);
           ui->connectButton->setStyleSheet("background-color:rgb(255,0,0)");

        }
    }else{
        ui->connectButton->setText("Connect");
        mbConnectState = false;
        mbMaster->disconnectSocketConn();
        QPalette P=ui->connectButton->palette();
        P.setColor(QPalette::Window,Qt::green);
        P.setBrush(QPalette::Window,Qt::blue);
        P.setColor(QPalette::ButtonText,Qt::white);
        ui->connectButton->setPalette(P);
        ui->connectButton->setBackgroundRole(QPalette::Button);
        ui->connectButton->setAutoFillBackground(true);
        ui->connectButton->setStyleSheet("background-color:rgb(70,207,45)");
    }
}

void Widget::on_communicateButton_clicked()
{
    if(ui->communicateButton->text() == "Start"){
        communicateEnable = true;
        ui->communicateButton->setText("Stop");

        QPalette P=ui->connectButton->palette();
        P.setColor(QPalette::Window,Qt::green);
        P.setBrush(QPalette::Window,Qt::blue);
        P.setColor(QPalette::ButtonText,Qt::white);
        ui->communicateButton->setPalette(P);
        ui->communicateButton->setBackgroundRole(QPalette::Button);
        ui->communicateButton->setAutoFillBackground(true);
        ui->communicateButton->setStyleSheet("background-color:rgb(70,207,45)");

    }else{
        communicateEnable = false;
        ui->communicateButton->setText("Start");

        QPalette P=ui->connectButton->palette();
        P.setColor(QPalette::Window,Qt::green);
        P.setBrush(QPalette::Window,Qt::blue);
        P.setColor(QPalette::ButtonText,Qt::white);
        ui->communicateButton->setPalette(P);
        ui->communicateButton->setBackgroundRole(QPalette::Button);
        ui->communicateButton->setAutoFillBackground(true);
        ui->communicateButton->setStyleSheet("background-color:rgb(255,0,0)");
    }
}

void Widget::on_PIDChangeButton_clicked()
{
    QString strPosiKpLineEdit = ui->KpLineEdit->text();
    QString strPosiKdLineEdit = ui->KdLineEdit->text();
    QString strAngleKpLineEdit = ui->KpLineEdit_2->text();
    QString strAngleKdLineEdit = ui->KdLineEdit_2->text();
    QString strFinalPostion = ui->positionEdit->text();

    positionPIDController->setKp(strPosiKpLineEdit.toFloat());
    positionPIDController->setKd(strPosiKdLineEdit.toFloat());
    anglePIDController->setKp(strAngleKpLineEdit.toFloat());
    anglePIDController->setKd(strAngleKdLineEdit.toFloat());
    finalPosition = strFinalPostion.toFloat();

}

void Widget::on_followButton_clicked()
{
    if(ui->followButton->text() == "Follow"){
        ui->openGL3DWidget->followEnable(true);
        ui->followButton->setText("Notfollow");


        QPalette P=ui->connectButton->palette();
        P.setColor(QPalette::Window,Qt::green);
        P.setBrush(QPalette::Window,Qt::blue);
        P.setColor(QPalette::ButtonText,Qt::white);
        ui->followButton->setPalette(P);
        ui->followButton->setBackgroundRole(QPalette::Button);
        ui->followButton->setAutoFillBackground(true);
        ui->followButton->setStyleSheet("background-color:rgb(231,245,103)");
    }else{
        ui->openGL3DWidget->followEnable(false);
        ui->followButton->setText("Follow");


        QPalette P=ui->connectButton->palette();
        P.setColor(QPalette::Window,Qt::green);
        P.setBrush(QPalette::Window,Qt::blue);
        P.setColor(QPalette::ButtonText,Qt::white);
        ui->followButton->setPalette(P);
        ui->followButton->setBackgroundRole(QPalette::Button);
        ui->followButton->setAutoFillBackground(true);
        ui->followButton->setStyleSheet("background-color:rgb(104,151,237)");
    }
}
