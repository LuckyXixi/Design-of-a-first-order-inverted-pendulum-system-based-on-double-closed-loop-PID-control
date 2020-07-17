#include "lrxmodbusmaster.h"
#include <QModbusTcpClient>
#include <QUrl>
#include <iostream>
#include <QVariant>

#include <bitset>
#include <QModbusDataUnit>

lrxModbusMaster::lrxModbusMaster()
{
     modbusDevice = new QModbusTcpClient();
}

bool lrxModbusMaster::connectSocketConn(const QUrl url){
    bool readyToConnect = (modbusDevice->state() != QModbusDevice::ConnectedState);
    if (readyToConnect) {
        modbusDevice->setConnectionParameter(QModbusDevice::NetworkPortParameter, url.port());
        modbusDevice->setConnectionParameter(QModbusDevice::NetworkAddressParameter, url.host());
        modbusDevice->setTimeout(200);
        modbusDevice->setNumberOfRetries(1);
        if (!modbusDevice->connectDevice()) {
            std::cout << "Connect failed: " << modbusDevice->errorString().toStdString() <<std::endl;
            return false;
        }else{
            return true;
        }
    }
    return false;
}

void lrxModbusMaster::setUk(float f,int serverId ){
    QModbusDataUnit writeUnit = writeRequest();
    //浮点数转换为2个quint16
    const quint32 *raw = reinterpret_cast<const quint32 *>(&f);

    QVector<quint16> reg;
    // append the high word first, then the low word
    // the endianess of the words should be handled by the modbus layer
    reg.append(*raw >> 16);
    reg.append(*raw & 0xFFFF);

    for (uint i = 0; i < writeUnit.valueCount(); i++) {
        writeUnit.setValue(i, reg[i + writeUnit.startAddress()]);
    }
    if (auto *reply = modbusDevice->sendWriteRequest(writeUnit, serverId)) {
        if (!reply->isFinished()) {
            connect(reply, &QModbusReply::finished, this, [reply]() {
                if (reply->error() == QModbusDevice::ProtocolError) {
                    std::cout<<"Write response error: %1 (Mobus exception: 0x%2)"<<reply->errorString().toStdString()<<reply->rawResult().exceptionCode()<<std::endl;
                } else if (reply->error() != QModbusDevice::NoError) {
                    std::cout<<"Write response error: %1 (code: 0x%2)"<<reply->errorString().toStdString()<<reply->error()<<std::endl;
                }
                reply->deleteLater();
            });
        } else {
            // broadcast replies return immediately
            reply->deleteLater();
        }
    } else {
        std::cout<<"Write error: "<<modbusDevice->errorString().toStdString()<<std::endl;
    }
}

bool lrxModbusMaster::getConnState(){
    if(modbusDevice->state()==QModbusDevice::ConnectedState)return true;
    else return false;
}

float lrxModbusMaster::getAngle(){
    std::cout<<"angle:"<<angle<<std::endl;
    return angle;
}

float lrxModbusMaster::getPosition(){
    std::cout<<"position:"<<position<<std::endl;
    return position;

}

bool lrxModbusMaster::getIsSlaveFree(){
    return isSlaveFree;
}



void lrxModbusMaster::readData(int serverId ){
    if (auto *reply = modbusDevice->sendReadRequest(readRequest(), serverId)) {
        if (!reply->isFinished())
            QObject::connect(reply, &QModbusReply::finished, this, &lrxModbusMaster::readReady);
        else
            delete reply; // broadcast replies return immediately
    } else {
        std::cout<<"Read error: "<<modbusDevice->errorString().toStdString() <<std::endl;
    }

}

void lrxModbusMaster::readFlagData(int serverId ){
    if (auto *reply = modbusDevice->sendReadRequest(readFlagRequest(), serverId)) {
        if (!reply->isFinished())
            QObject::connect(reply, &QModbusReply::finished, this, &lrxModbusMaster::readFlagReady);
        else
            delete reply; // broadcast replies return immediately
    } else {
        std::cout<<"Read error: "<<modbusDevice->errorString().toStdString() <<std::endl;
    }

}

QModbusDataUnit lrxModbusMaster::readRequest() const
{

    int startAddress = 2;
    Q_ASSERT(startAddress >= 0 && startAddress < 10);

    // do not go beyond 10 entries
    int numberOfEntries = 4;
    return QModbusDataUnit(QModbusDataUnit::HoldingRegisters, startAddress, numberOfEntries);
}

QModbusDataUnit lrxModbusMaster::readFlagRequest() const
{

    int startAddress = slave_free_addr;
    Q_ASSERT(startAddress >= 0 && startAddress < 10);

    // do not go beyond 10 entries
    int numberOfEntries = 1;
    return QModbusDataUnit(QModbusDataUnit::HoldingRegisters, startAddress, numberOfEntries);
}

QModbusDataUnit lrxModbusMaster::writeRequest() const
{

    int startAddress = 0;
    Q_ASSERT(startAddress >= 0 && startAddress < 10);

    // do not go beyond 10 entries
    int numberOfEntries = 2;
    return QModbusDataUnit(QModbusDataUnit::HoldingRegisters, startAddress, numberOfEntries);
}

QModbusDataUnit lrxModbusMaster::writeFlagRequest() const
{

    int startAddress = slave_free_addr;
    Q_ASSERT(startAddress >= 0 && startAddress < 10);

    // do not go beyond 10 entries
    int numberOfEntries = 1;
    return QModbusDataUnit(QModbusDataUnit::HoldingRegisters, startAddress, numberOfEntries);
}

void lrxModbusMaster::readReady()
{
    auto reply = qobject_cast<QModbusReply *>(sender());
    if (!reply)
        return;

    if (reply->error() == QModbusDevice::NoError) {
        quint16 dt_1;
        quint16 dt_2;
        quint32 data;
        const QModbusDataUnit unit = reply->result();
        //解码position
        dt_1 = unit.value(0);
        dt_2 = unit.value(1);

        data = (dt_1<<16)|dt_2;
        position = reinterpret_cast<float&>(data);
        //解码angle
        dt_1 = unit.value(2);
        dt_2 = unit.value(3);

        data = (dt_1<<16)|dt_2;
        angle = reinterpret_cast<float&>(data);

    } else if (reply->error() == QModbusDevice::ProtocolError) {
        std::cout<<"Read response error: %1 (Mobus exception: 0x%2)"<<reply->errorString().toStdString()<<reply->rawResult().exceptionCode()<<std::endl;
    } else {
        std::cout<<"Read response error: %1 (code: 0x%2)"<<reply->errorString().toStdString()<<reply->error()<<std::endl;
    }

    reply->deleteLater();
}

void lrxModbusMaster::readFlagReady()
{
    auto reply = qobject_cast<QModbusReply *>(sender());
    if (!reply)
        return;

    if (reply->error() == QModbusDevice::NoError) {
        quint16 dt_1;
        const QModbusDataUnit unit = reply->result();
        //解码position
        dt_1 = unit.value(0);
        if(!dt_1){
            isSlaveFree = false;
        }
        else{
            isSlaveFree = true;
        }

    } else if (reply->error() == QModbusDevice::ProtocolError) {
        std::cout<<"Read response error: %1 (Mobus exception: 0x%2)"<<reply->errorString().toStdString()<<reply->rawResult().exceptionCode()<<std::endl;
    } else {
        std::cout<<"Read response error: %1 (code: 0x%2)"<<reply->errorString().toStdString()<<reply->error()<<std::endl;
    }

    reply->deleteLater();
}

void lrxModbusMaster::enableFreeFlag(int serverId){
    QModbusDataUnit writeUnit = writeFlagRequest();
    quint16 flag = 1;
    writeUnit.setValue(slave_free_addr,flag);
    if (auto *reply = modbusDevice->sendWriteRequest(writeUnit, serverId)) {
        if (!reply->isFinished()) {
            connect(reply, &QModbusReply::finished, this, [reply]() {
                if (reply->error() == QModbusDevice::ProtocolError) {
                    std::cout<<"Write response error: %1 (Mobus exception: 0x%2)"<<reply->errorString().toStdString()<<reply->rawResult().exceptionCode()<<std::endl;
                } else if (reply->error() != QModbusDevice::NoError) {
                    std::cout<<"Write response error: %1 (code: 0x%2)"<<reply->errorString().toStdString()<<reply->error()<<std::endl;
                }
                reply->deleteLater();
            });
        } else {
            // broadcast replies return immediately
            reply->deleteLater();
        }
    } else {
        std::cout<<"Write error: "<<modbusDevice->errorString().toStdString()<<std::endl;
    }

}

void lrxModbusMaster::disableFreeFlag(int serverId){
    QModbusDataUnit writeUnit = writeFlagRequest();
    quint16 flag = 0;
    writeUnit.setValue(slave_free_addr,flag);
    if (auto *reply = modbusDevice->sendWriteRequest(writeUnit, serverId)) {
        if (!reply->isFinished()) {
            connect(reply, &QModbusReply::finished, this, [reply]() {
                if (reply->error() == QModbusDevice::ProtocolError) {
                    std::cout<<"Write response error: %1 (Mobus exception: 0x%2)"<<reply->errorString().toStdString()<<reply->rawResult().exceptionCode()<<std::endl;
                } else if (reply->error() != QModbusDevice::NoError) {
                    std::cout<<"Write response error: %1 (code: 0x%2)"<<reply->errorString().toStdString()<<reply->error()<<std::endl;
                }
                reply->deleteLater();
            });
        } else {
            // broadcast replies return immediately
            reply->deleteLater();
        }
    } else {
        std::cout<<"Write error: "<<modbusDevice->errorString().toStdString()<<std::endl;
    }
}

void lrxModbusMaster::shutdownMaster( ){
    if (modbusDevice)
        disconnectSocketConn();
    delete modbusDevice;
}

void lrxModbusMaster::disconnectSocketConn(){
    modbusDevice->disconnectDevice();
}
