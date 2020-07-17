#include "lrxmodbusslave.h"
#include <QUrl>
#include <iostream>
#include <QModbusTcpServer>


lrxModbusSlave::lrxModbusSlave(){
    modbusDevice = new QModbusTcpServer();
    QModbusDataUnitMap reg;
    reg.insert(QModbusDataUnit::HoldingRegisters, { QModbusDataUnit::HoldingRegisters, 0, 8 });
    modbusDevice->setMap(reg);
    enableFreeFlag();
    std::cout<<getIsMasterFree()<<std::endl;

}

void lrxModbusSlave::listenSocketConn(QUrl url, QString serAddr){
    bool intendToConnect = (modbusDevice->state() == QModbusDevice::UnconnectedState);
    if (intendToConnect) {
        modbusDevice->setConnectionParameter(QModbusDevice::NetworkPortParameter, url.port());
        modbusDevice->setConnectionParameter(QModbusDevice::NetworkAddressParameter, url.host());
        modbusDevice->setServerAddress(serAddr.toInt());

        if (!modbusDevice->connectDevice()) {
            std::cout << "Connect failed: " << modbusDevice->errorString().toStdString() <<std::endl;
        }
    }else {
        modbusDevice->disconnectDevice();
    }
}

float lrxModbusSlave::getUk(){
    float uk;
    quint16 uk_1;
    quint16 uk_2;
    quint32 ukk;

    modbusDevice->data(QModbusDataUnit::HoldingRegisters, uk1_addr, &uk_1);
    modbusDevice->data(QModbusDataUnit::HoldingRegisters, uk2_addr, &uk_2);

    ukk = (uk_1<<16)|uk_2;
    uk = reinterpret_cast<float&>(ukk);
    std::cout<<"uk:"<<uk<<std::endl;
    return uk;

}

void lrxModbusSlave::setPosition( float f ){
    const quint32 *raw = reinterpret_cast<const quint32 *>(&f);
    std::cout<<"position:"<<f<<std::endl;
    QVector<quint16> reg;
    // append the high word first, then the low word
    // the endianess of the words should be handled by the modbus layer
    reg.append(*raw >> 16);
    reg.append(*raw & 0xFFFF);
    modbusDevice->setData(QModbusDataUnit::HoldingRegisters, pos1_addr, reg[0]);
    modbusDevice->setData(QModbusDataUnit::HoldingRegisters, pos2_addr, reg[1]);

}
void lrxModbusSlave::setAngle( float f ){
    const quint32 *raw = reinterpret_cast<const quint32 *>(&f);
    std::cout<<"angle:"<<f<<std::endl;
    QVector<quint16> reg;
    // append the high word first, then the low word
    // the endianess of the words should be handled by the modbus layer
    reg.append(*raw >> 16);
    reg.append(*raw & 0xFFFF);
    modbusDevice->setData(QModbusDataUnit::HoldingRegisters, ang1_addr, reg[0]);
    modbusDevice->setData(QModbusDataUnit::HoldingRegisters, ang2_addr, reg[1]);
}

void lrxModbusSlave::enableFreeFlag(){
    quint16 flag = 1;
    modbusDevice->setData(QModbusDataUnit::HoldingRegisters, slave_free_addr, flag);
}

void lrxModbusSlave::disableFreeFlag(){
    quint16 flag = 0;
    modbusDevice->setData(QModbusDataUnit::HoldingRegisters, slave_free_addr, flag);
}

bool lrxModbusSlave::getIsFree(){
    quint16 data;
    modbusDevice->data(QModbusDataUnit::HoldingRegisters, slave_free_addr, &data);
    std::cout<<data<<std::endl;
    if(!data){
        return false;
    }else{
        return true;
    }
}

bool lrxModbusSlave::getIsMasterFree(){
    quint16 data;
    modbusDevice->data(QModbusDataUnit::HoldingRegisters, master_free_addr, &data);
    std::cout<<data<<std::endl;
    if(!data){
        return false;
    }else{
        return true;
    }
}

void lrxModbusSlave::shutdownSlave( ){
    if (modbusDevice)
        disconnectSocketConn();
    delete modbusDevice;
}

void lrxModbusSlave::disconnectSocketConn(){
    modbusDevice->disconnectDevice();
}
