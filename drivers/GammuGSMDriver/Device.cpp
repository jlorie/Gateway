#include <include/CommonErrors.hpp>

#include "Device.hpp"
#include "PhoneNumber.hpp"
#include <signal.h>

namespace Gateway
{
namespace Driver
{
    Device::Device(const DeviceInfo &info)
    {
        _serialPort = info.value(QString("serial_port"), QString("/dev/gsm_device"));
    }

    Device::~Device()
    {
        /* Terminate connection */
        error = GSM_TerminateConnection(_stateMachine);

        /* Free up used memory */
        GSM_FreeStateMachine(_stateMachine);
    }

    /* Interrupt signal handler */
    void interrupt(int sign)
    {
        signal(sign, SIG_IGN);
    }

    bool Device::initialize()
    {
        bool result(true);

        /* Register signal handler */
        signal(SIGINT, interrupt);
        signal(SIGTERM, interrupt);

        /*
         * We don't need gettext, but need to set locales so that
         * charset conversion works.
         */
        GSM_InitLocales(NULL);

        /* Allocates state machine */
        _stateMachine = GSM_AllocStateMachine();
        result = (_stateMachine != NULL);

        if (result)
        {
            configure();
            result = connect();

            _numbers.append(new PhoneNumber("+584140937970", _stateMachine));
        }

        return result;
    }

    QString Device::deviceId()
    {
        char IMEI[30];
        GSM_GetIMEI(_stateMachine, IMEI);

        return QString(IMEI);
    }

    NumberList Device::phoneNumbers() const
    {
        return _numbers;
    }

    void Device::configure()
    {
        cfg = GSM_GetConfig(_stateMachine, 0);

        /*
         * Set configuration, first freeing old values.
         */
        free(cfg->Device);
        cfg->Device = strdup(_serialPort.toStdString().c_str());
        free(cfg->Connection);
        cfg->Connection = strdup("at");

        /* We have one valid configuration */
        GSM_SetConfigNum(_stateMachine, 1);

    }

    bool Device::connect()
    {
        GSM_Error error(GSM_InitConnection(_stateMachine, 1));
        bool result(ERR_NONE == error);

        if (!result)
            qWarning("Could not connet to device, %s", GSM_ErrorString(error));
        else
        {
            error = GSM_SetFastSMSSending(_stateMachine, TRUE);
            if (error != ERR_NONE)
                qDebug("Error setting fast sms sending, %s", GSM_ErrorString(error));
        }

        return result;
    }
}
}
