#ifndef SERIALPHYSICALLAYER_HPP
#define SERIALPHYSICALLAYER_HPP

#include <QObject>
#include <QSerialPort>
#include <QSerialPortInfo>

#include <QEventLoop>

class SerialPhysicalLayer : public QObject
{
    Q_OBJECT
public:
    SerialPhysicalLayer(const QString serialPort, QObject *parent = 0);
    ~SerialPhysicalLayer();

    bool isConnected () const;

    bool disconnect ();

    bool connect (ulong timeOut);

    bool send (const QString &data);

    bool receive (ulong timeOut, QString &data);

    bool receiveUntil (char criteria, ulong maxSize, ulong timeOut, QString &data);

    ulong error() const;

    QString errorString() const;

private slots:
    void receive();
    void timeOut();

signals:
    void frameReceived(const QString &frame);

private:
    QSerialPort *_port;
    QSerialPortInfo _info;

    QByteArray _writeBuffer;
    QByteArray _readBuffer;

    QEventLoop _waitLoop;

    bool _waitingResponse;

};

#endif // SERIALPHYSICALLAYER_HPP
