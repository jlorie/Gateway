#include <QStringList>
#include <QString>
#include <QMap>

#include <serialport/qserialportinfo.h>
#include <gammu/gammu.h>


typedef QMap<QString, QStringList> PortsByDevice;
class PortsInfo
{
public:
    QStringList availablePorts(const QString &deviceId) const
    {
        QStringList ports;
        QList<QSerialPortInfo> infos(QSerialPortInfo::availablePorts());

        QString trimmedId(deviceId);
        {
            int pos(0);

            // remove 0 at front
            while(trimmedId.at(pos) == '0')
                trimmedId.remove(pos, 1);

            pos = trimmedId.indexOf(":");
            if (pos > 0)
            {
                // remove 0 after ":"
                while(trimmedId.at(pos + 1) == '0')
                    trimmedId.remove(pos + 1, 1);
            }
        }

        foreach (QSerialPortInfo info, infos)
        {
            QString devId = QString::number(info.vendorIdentifier(), 16)
                    + ":"
                    + QString::number(info.productIdentifier(), 16);

            if (trimmedId == devId) // el dispositivo esta libre
            {
                QString serialPort("/dev/" + info.portName()); // FIXME obtener la direccion multiplataforma
                ports.insert(info.productIdentifier(), serialPort);
            }
        }

        return ports;
    }

    PortsByDevice availablePorts() const
    {
        QList<QSerialPortInfo> infos = QSerialPortInfo::availablePorts();
        PortsByDevice ports;

        foreach (QSerialPortInfo info, infos)
        {
            //format: vendorId:productId
            QString devId = QString::number(info.vendorIdentifier(), 16)
                    + ":"
                    + QString::number(info.productIdentifier(), 16);

            QString serialPort("/dev/" + info.portName()); // FIXME obtener la direccion multiplataforma

            if (!ports.contains(devId))
            {
                ports.insert(devId, QStringList(serialPort));
            }
            else
            {
                QStringList existingPorts(ports.value(devId));
                existingPorts.append(serialPort);

                ports.insert(devId, existingPorts);
            }
        }

        return ports;
    }
};

QString getIMEI(const QString &serialPort)
{
    QString imei;

    GSM_Error error;
    GSM_Config *cfg;

    GSM_InitLocales(NULL);
    GSM_StateMachine *stateMachine(GSM_AllocStateMachine());
    if (!stateMachine)
    {
        qWarning("Error initiating state machine");
        return imei;
    }

    cfg = GSM_GetConfig(stateMachine, 0);
    cfg->Device = strdup(qPrintable(serialPort));
    cfg->Connection = strdup("at");

    /* We have one valid configuration */
    GSM_SetConfigNum(stateMachine, 1);

    qDebug("Starting phone communication (%s) ...", qPrintable(serialPort));
    error = GSM_InitConnection(stateMachine, 1);
    if (error != ERR_NONE)
    {
        qWarning("Error starting connection (%d): %s", error, GSM_ErrorString(error));
        return imei;
    }

    char IMEI[20];
    error = GSM_GetIMEI(stateMachine, IMEI);
    if (error != ERR_NONE)
    {
        qWarning("Error getting IMEI (%d): %s", error, GSM_ErrorString(error));
        return imei;
    }

    imei = QString(IMEI);

    error = GSM_TerminateConnection(stateMachine);
    if (error !=  ERR_NONE)
    {
        qWarning("Error terminating connection (%d): %s", error, GSM_ErrorString(error));
    }
    GSM_FreeStateMachine(stateMachine);

    return imei;
}

int main()
{
//    PortsInfo pInfo;

//// Para pedir la informacion de todos los puertos conectados

//    PortsByDevice devices(pInfo.availablePorts());
//    foreach (QString devId, devices.keys())
//    {
//        qDebug("[Device]");
//        qDebug("device_id=%s", qPrintable(devId));

//        qDebug("[Ports]");

//        foreach (QString serialPort, devices.value(devId))
//        {
//            qDebug("serial_port=%s", qPrintable(serialPort));
//        }
//    }

//// Para pedir los puertos para un deviceId

//    qDebug("[Ports]");
//    QStringList ports(pInfo.availablePorts("12d1:1406"));
//    foreach (QString serialPort, ports)
//    {
//        qDebug("serial_port=%s", qPrintable(serialPort));
//    }

//// Para pedir el imei por un puerto

    qDebug("IMEI: %s", qPrintable(getIMEI("/dev/ttyUSB1")));

    return 0;
}
