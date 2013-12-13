#ifndef SERIALPHYSICALLAYER_HPP
#define SERIALPHYSICALLAYER_HPP

#include <QObject>
#include <QSerialPort>
#include <QSerialPortInfo>

class SerialPhysicalLayer : public QObject
{
    Q_OBJECT
public:
    SerialPhysicalLayer(const QSerialPortInfo &info, QObject *parent = 0);
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

signals:
    void frameReceived(const QString &frame);

private:
    QSerialPort *_port;
    QSerialPortInfo _info;

    bool _waitingResponse;

};

#endif // SERIALPHYSICALLAYER_HPP