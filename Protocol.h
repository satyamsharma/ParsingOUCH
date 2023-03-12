#pragma once
#include "ByteSwap.h"

#pragma pack(push, 1)
struct PCAP_Header
{
    uint16_t streamId;
    uint32_t packetLen;

    uint16_t GetStreamId()  { return bswap(streamId); }
    uint32_t GetPacketLen() { return bswap(packetLen); }
};

struct OUCH_Header
{
    uint16_t msgLen;
    char     packetType;
    char     msgType;
    uint64_t timeStamp;

    uint16_t GetMsgLen()  { return bswap(msgLen); }
    char     GetMsgType() { return msgType; }
};

enum class MsgType
{
    SystemEventMsg = 'S',
    AcceptedMsg    = 'A',
    ReplacedMsg    = 'U',
    ExecutedMsg    = 'E',
    CanceledMsg    = 'C'
};

// msgType : S
struct SystemEventMsg
{
    char     eventEnum;
};

// msgType : A
struct AcceptedMsg
{
    char     orderToken[14];
    char     side;
    uint16_t shares;
    char     symbol[8];
    uint16_t price;
    uint16_t timeInForce;
    char     firm[4];
    char     display;
    uint64_t orderRefNumber;
    char     orderCapacity;
    char     intermediateSweep;
    uint16_t minQty;
    char     crossType;
    char     orderState;
};

// msgType : U
struct ReplacedMsg
{
    char     orderToken[14];
    char     side;
    uint16_t shares;
    char     symbol[8];
    uint16_t price;
    uint16_t timeInForce;
    char     firm[4];
    char     display;
    uint64_t orderRefNumber;
    char     orderCapacity;
    char     intermediateSweep;
    uint16_t minQty;
    char     crossType;
    char     orderState;
    char     prevOrderToken[14];
};

// msgType : E
struct ExecutedMsg
{
    char     orderToken[14];
    uint32_t executedShares;
    uint32_t executedPrice;
    char     liquidityFlag;
    uint64_t matchNum;

    uint16_t GetExecutedShares()  { return bswap(executedShares); }
};

// msgType : C
struct CanceledMsg
{
    char     orderToken[14];
    uint16_t decrementShares;
    char     reason;
};
#pragma pack(pop)