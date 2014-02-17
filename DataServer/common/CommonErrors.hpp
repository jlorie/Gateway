#ifndef COMMONERRORS_HPP
#define COMMONERRORS_HPP
namespace Gateway
{
    enum CommonErrors
    {
        errOK = 0,
        errConnectionRefused,
        errFetchingPendingMessages,
        errDispatchingMessages,
        errChangingStatus
    };
}

#endif // COMMONERRORS_HPP
