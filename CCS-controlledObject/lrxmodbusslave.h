#ifndef LRXMODBUSSLAVE_H
#define LRXMODBUSSLAVE_H

#include <QModbusServer>

class lrxModbusSlave
{
public:
    lrxModbusSlave();
    void listenSocketConn(QUrl url ,QString serAddr);
    float getUk( );
    void setPosition( float f );
    void setAngle( float f );
    void shutdownSlave( );
    void disconnectSocketConn();
    void enableFreeFlag();
    void disableFreeFlag();
    bool getIsMasterFree();
    bool getIsFree();
private:

    quint16 const uk1_addr = 0;
    quint16 const uk2_addr = 1;
    quint16 const pos1_addr = 2;
    quint16 const pos2_addr = 3;
    quint16 const ang1_addr = 4;
    quint16 const ang2_addr = 5;
    quint16 const slave_free_addr = 6;
    quint16 const master_free_addr = 7;

    QModbusServer *modbusDevice;
};

#endif // LRXMODBUSSLAVE_H
