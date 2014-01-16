#ifndef PHONENUMBER_HPP
#define PHONENUMBER_HPP

#include <include/IPhoneNumber.hpp>

#include <QString>
#include <QMap>
#include <QObject>
#include <QByteArray>
#include <QTimer>

#include "NetworkManager.hpp"
#include "Message.hpp"

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
        void unreadMessages();

    public slots:
        void processResponse(qlonglong requestId, QByteArray response);

    protected slots:
        void poll();

    private:
        void processSendMessageRequest(const IMessage *message, const QByteArray &response);
        void processUnreadMessageRequest(const QByteArray &response);

    private:
        NetworkManager *_networkManager;
        QString _number;
        bool _configuring;

        uint _smsListPageSize;
        IMessage *_lastMessage;

        QMap<qlonglong, RequestType> _pendingResponses;
        QMap<qlonglong, const IMessage *> _pendingMessageResponse;

        QTimer _pollingTimer;
};



#endif // PHONENUMBER_HPP
