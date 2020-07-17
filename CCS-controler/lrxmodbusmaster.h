#ifndef LRXMODBUSMASTER_H
#define LRXMODBUSMASTER_H

#include <QModbusClient>

class lrxModbusMaster:public QObject{
Q_OBJECT
public:
    lrxModbusMaster();
    bool connectSocketConn( const QUrl url);
    void setUk(float f,int serverId);
    void readData(int serverId );
    void readFlagData(int serverId );
    bool getIsSlaveFree();
    bool getConnState();
    float getPosition( );
    float getAngle( );
    void enableFreeFlag(int serverId);
    void disableFreeFlag(int serverId);
    void shutdownMaster( );
    void disconnectSocketConn();
private:
    QModbusClient *modbusDevice;
    float position=0;
    float angle=0;

    QModbusDataUnit readRequest() const;
    QModbusDataUnit readFlagRequest() const;
    QModbusDataUnit writeRequest() const;
    QModbusDataUnit writeFlagRequest() const;

    quint16 const uk1_addr = 0;
    quint16 const uk2_addr = 1;
    quint16 const pos1_addr = 2;
    quint16 const pos2_addr = 3;
    quint16 const ang1_addr = 4;
    quint16 const ang2_addr = 5;
    quint16 const slave_free_addr = 6;
    quint16 const master_free_addr = 7;

    bool isSlaveFree = false;
private slots:
    void readReady();
    void readFlagReady();



};

#endif // LRXMODBUSMASTER_H
