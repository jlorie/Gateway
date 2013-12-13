#include "SerialPhysicalLayer.hpp"

#include <QDebug>

const ulong TimeOut = 1000; //1 sec

SerialPhysicalLayer::SerialPhysicalLayer(const QSerialPortInfo &info, QObject *parent)
    :QObject(parent), _info(info)
{
    _port = new QSerialPort(info, this);
    {
        _port->setBaudRate(QSerialPort::Baud9600);
        _port->setDataBits(QSerialPort::Data8);
        _port->setParity(QSerialPort::NoParity);
        _port->setStopBits(QSerialPort::OneStop);
        _port->setFlowControl(QSerialPort::NoFlowControl);
    }

    _waitingResponse = false;

    QObject::connect(_port, SIGNAL(readyRead()), this, SLOT(receive()));
}

SerialPhysicalLayer::~SerialPhysicalLayer()
{
    _port->close();
    delete _port;
}

bool SerialPhysicalLayer::isConnected() const
{
    return _port->isOpen();
}

bool SerialPhysicalLayer::disconnect()
{
    if (isConnected())
        _port->close();

    return true;
}


bool SerialPhysicalLayer::connect(ulong timeOut)
{
    Q_UNUSED(timeOut);

    bool result(true);

    if (!isConnected())
        result = _port->open(QIODevice::ReadWrite);

    return result;
}


bool SerialPhysicalLayer::send(const QString &data)
{
    if (!isConnected())
        connect(TimeOut);

    if (isConnected())
    {
        _port->write(data.toLocal8Bit(), TimeOut);
        if (!error())
        {
            _port->waitForBytesWritten(TimeOut);
        }
    }

    return !(bool)error();
}

bool SerialPhysicalLayer::receive (ulong timeOut, QString &data)
{
    if (!isConnected())
        connect(timeOut);

    if (isConnected())
    {
        data.clear();

        _waitingResponse = true;
        while (_port->waitForReadyRead(timeOut))
        {
            data += _port->readAll();
        }
        _waitingResponse = false;
    }

    return !(bool)error();
}

bool SerialPhysicalLayer::receiveUntil (char criteria, ulong maxSize, ulong timeOut, QString &data)
{
    bool result(true);

    if (!isConnected())
        connect(timeOut);

    if (!(result = (bool)error()))
    {
        if (!(result = _port->waitForReadyRead(timeOut)))
        {
            QByteArray received;
            forever
            {
                received = _port->read(1);
                if (received.isEmpty())
                    break;

                data+= received;
                if (received.contains(criteria) || (ulong)data.size() >=maxSize)
                    break;
            }

            result = error();
        }
    }

    return result;
}

ulong SerialPhysicalLayer::error() const
{
    return _port->error();
}

QString SerialPhysicalLayer::errorString() const
{
    if (!error())
        return QString ("Success!!!");

    return _port->errorString();
}

void SerialPhysicalLayer::receive()
{
    if (!_waitingResponse)
    {
        QString data;
        do
        {
            data += _port->readAll();
        }
        while (_port->waitForReadyRead(TimeOut));

        emit frameReceived(data);
    }
}
