#include "SerialPhysicalLayer.hpp"
#include <include/CommonErrors.hpp>

#include <QTimer>

const ulong TimeOut = 1000; //1 sec
const ulong MaxBufferSize = 1024; // 1Kb

SerialPhysicalLayer::SerialPhysicalLayer(const QString serialPort, QObject *parent)
    :QObject(parent)
{
    _port = new QSerialPort(serialPort);
//    {
//        _port->setBaudRate(QSerialPort::Baud115200);
//        _port->setDataBits(QSerialPort::Data8);
//        _port->setParity(QSerialPort::NoParity);
//        _port->setStopBits(QSerialPort::OneStop);
//        _port->setFlowControl(QSerialPort::NoFlowControl);
//    }

    _waitingResponse = false;

    QObject::connect(_port, SIGNAL(readyRead()), this, SLOT(receive()));
}

SerialPhysicalLayer::~SerialPhysicalLayer()
{
    if (isConnected())
    {
        _port->close();
    }
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
    {
        result = _port->open(QIODevice::ReadWrite);

        if (result)
        {
            _port->setBaudRate(QSerialPort::Baud115200);
            _port->setDataBits(QSerialPort::Data8);
            _port->setParity(QSerialPort::NoParity);
            _port->setStopBits(QSerialPort::OneStop);
            _port->setFlowControl(QSerialPort::NoFlowControl);
        }
    }

    return result;
}


bool SerialPhysicalLayer::send(const QString &data)
{
    if (!isConnected())
        connect(TimeOut);

    if (isConnected())
    {
        _writeBuffer.clear();
        _writeBuffer.append(data);

        _port->write(_writeBuffer);
    }

    return !(bool)error();
}

bool SerialPhysicalLayer::receive (ulong timeOut, QString &data)
{        
    // Wait for new message from device

    QTimer timer;
    {
        timer.start(timeOut);
        QObject::connect(&timer, SIGNAL(timeout()), &_waitLoop, SLOT(quit()));
        _waitLoop.exec();
    }

    if (!timer.isActive()) //timeOut
        return false;

    data = _readBuffer;
    return true;
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
    _readBuffer = _port->readAll();

    if (_readBuffer.startsWith(_writeBuffer))
        return;

    if (_waitLoop.isRunning())
        _waitLoop.quit();
    else
        emit frameReceived(QString(_readBuffer));
}
