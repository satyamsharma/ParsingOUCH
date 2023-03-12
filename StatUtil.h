#pragma once
#include <iostream>
using namespace std;

#include "Protocol.h"

class StreamStats
{
    size_t acceptedMsgCounter;
    size_t canceledMsgCounter;
    size_t executedMsgCounter;
    size_t systemEventMsgCounter;
    size_t replacedMsgCounter;

    size_t executedSharesCounter;

    public:
        inline void IncrementMsgCounter(char msgType);
        inline void IncrementExecShareCounter(size_t shares);
        friend ostream& operator<<(ostream& os, const StreamStats& s);
};

inline void StreamStats::IncrementMsgCounter(char msgType)
{
    switch(static_cast<MsgType>(msgType))
    {
        case MsgType::AcceptedMsg:
            acceptedMsgCounter++;
            break;
        case MsgType::CanceledMsg:
            canceledMsgCounter++;
            break;
        case MsgType::ExecutedMsg:
            executedMsgCounter++;
            break;
        case MsgType::SystemEventMsg:
            systemEventMsgCounter++;
            break;
        case MsgType::ReplacedMsg:
            replacedMsgCounter++;
            break;
    }
}

inline void StreamStats::IncrementExecShareCounter(size_t shares)
{
    executedSharesCounter += shares;
}

ostream& operator<<(ostream& os, const StreamStats& s)
{
    os << "Accepted: " << s.acceptedMsgCounter << " messages\n\t"
       << "System Event: " << s.systemEventMsgCounter << " messages\n\t"
       << "Replaced: " << s.replacedMsgCounter << " messages\n\t"
       << "Canceled: " << s.canceledMsgCounter << " messages\n\t"
       << "Executed: " << s.executedMsgCounter << " messages: " << s.executedSharesCounter << " executed shares\n";

    return os;
}