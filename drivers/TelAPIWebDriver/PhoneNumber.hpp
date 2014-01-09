#ifndef PHONENUMBER_HPP
#define PHONENUMBER_HPP

#include <include/IPhoneNumber.hpp>

#include <QString>
#include <QMap>
#include <QObject>
#include <QByteArray>

#include "NetworkManager.hpp"
#include <include/IPhoneNumber.hpp>

enum class RequestType
{
    None,
    SendMessage,
    UnreadMessages
} ;

class PhoneNumber : public IPhoneNumber
{
    Q_OBJECT
    public:
        PhoneNumber(NetworkManager *networkManager, const QString &number);
        ~PhoneNumber();

        QString number() const;
        void sendMessage(const IMessage *message);
        MessageList unreadMessages() const;

    public slots:
        void processResponse(qlonglong requestId, QByteArray response);
        void processMessageRequest(const IMessage *message, const QByteArray &response);

    private:
        NetworkManager *_networkManager;
        QString _number;

        QMap<qlonglong, RequestType> _pendingResponses;
        QMap<qlonglong, const IMessage *> _pendingMessageResponse;
};



#endif // PHONENUMBER_HPP
