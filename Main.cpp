#include <iostream>
#include <fstream>
#include <unordered_map>
#include <vector>
#include <utility>
using namespace std;

#include "Protocol.h"
#include "StatUtil.h"

int main()
{
    // StreamId->{MsgCounts}
    unordered_map<size_t, StreamStats> stream2StatsMap;

    // Open pcap
    ifstream reader("OUCHLMM2.incoming.packets", ios::binary);

    // temp vars (to avoid reallocation)
    PCAP_Header    packetHeader;
    size_t         packetLen;
    size_t         streamId;
    char           msgType;
    size_t         msgLen;
    OUCH_Header    ouchHeader;
    ExecutedMsg    executedMsg;

    streampos      msgStartPos;
    unordered_map<size_t, pair<size_t, char*> > pendingOuchMsgPerStream;

    while(reader.read(reinterpret_cast<char*>(&packetHeader), sizeof(packetHeader)))
    {
        // Interpret pcap
        streamId  = packetHeader.GetStreamId();
        packetLen = packetHeader.GetPacketLen();
        //cout << "StreamId[" << streamId << "] PacketLen[" << packetLen << "]" << endl;
        if (packetLen > 0)
        {
            if (pendingOuchMsgPerStream.find(streamId) != pendingOuchMsgPerStream.end())
            {
                size_t cachedSize = pendingOuchMsgPerStream[streamId].first;
                char merge[cachedSize + packetLen];
                std::memcpy(merge, pendingOuchMsgPerStream[streamId].second, cachedSize);
                reader.read(&merge[cachedSize], packetLen);
    
                ouchHeader = *reinterpret_cast<OUCH_Header*>(&merge);
                msgType = ouchHeader.GetMsgType();
                msgLen  = ouchHeader.GetMsgLen();
                //cout << "MsgType[" << msgType << "] MsgLen[" << msgLen << "]" << endl;
                stream2StatsMap[streamId].IncrementMsgCounter(msgType);

                if (static_cast<MsgType>(msgType) == MsgType::ExecutedMsg)
                {
                    executedMsg = *reinterpret_cast<ExecutedMsg*>(&merge[sizeof(ouchHeader)]);
                    //cout << "Executed Shares[" << executedMsg.GetExecutedShares() << "]" << endl;
                    stream2StatsMap[streamId].IncrementExecShareCounter(executedMsg.GetExecutedShares());
                }

                delete(pendingOuchMsgPerStream[streamId].second);
                pendingOuchMsgPerStream.erase(streamId);
                continue;
            }

            msgStartPos = reader.tellg();
            if (packetLen > sizeof(OUCH_Header))
            {
                reader.read(reinterpret_cast<char*>(&ouchHeader), sizeof(ouchHeader));
                msgType = ouchHeader.GetMsgType();
                msgLen  = ouchHeader.GetMsgLen();
                if (msgLen <= packetLen - sizeof(OUCH_Header::msgLen))
                {
                    // Full message present is in the packet
                    // cout << "MsgType[" << msgType << "] MsgLen[" << msgLen << "]" << endl;
                    stream2StatsMap[streamId].IncrementMsgCounter(msgType);

                    if (static_cast<MsgType>(msgType) == MsgType::ExecutedMsg)
                    {
                        reader.read(reinterpret_cast<char*>(&executedMsg), sizeof(executedMsg));
                        //cout << "Executed Shares[" << executedMsg.GetExecutedShares() << "]" << endl;
                        stream2StatsMap[streamId].IncrementExecShareCounter(executedMsg.GetExecutedShares());
                    }
                }
                else
                {
                    // Incomplete Ouch message
                    reader.seekg(msgStartPos);
                    char* cache = new char [packetLen];
                    reader.read(cache, packetLen);
                    pendingOuchMsgPerStream[streamId] = make_pair(packetLen, cache);
                }
            }
            else
            {
                // Incomplete Ouch header
                char* cache = new char [packetLen];
                reader.read(cache, packetLen);
                pendingOuchMsgPerStream[streamId] = make_pair(packetLen, cache);
            }
            reader.seekg(msgStartPos);
        }
        reader.seekg(packetLen, ios::cur);
    }

    // Close pcap
    reader.close();

    // Display Stats
    for(auto mapItr : stream2StatsMap)
    {
        cout << "Stream " << mapItr.first << "\n\t" << mapItr.second << endl;
    }

    return 0;
}