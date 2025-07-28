#ifndef __PACKET_PARSER_H__
#define __PACKET_PARSER_H__

#include "_IncCommon.mqh"
#include "Protocol.mqh"
#include <Generic\Queue.mqh>

class CPacketParser
{
public:
   CPacketParser(){};
   ~CPacketParser(){ Clear(); }
   
   int Size() { return m_que.Count();} 
   int Count() {return Size(); }
   int Add_Packet(_In_ string & sRecvPacket);   // return the number of the remain packets in the queue
   int Get_OnePacket(_Out_ string & sPacket, _Out_ bool& bExist); // return the number of the remain packets in the queue
   void Clear();
private:
   CQueue<string>   m_que;
   
};


// Get one full packet and push into the queue
// return the number of the remain packets in the queue
//  0x02+LEN+DATA : LEN = StringLen(Data)
//  0x0299=1000x011=10010x01
//  0x02 : STX, 99=100 : DataLen, 1=1001 : code, 0x01 : delimiter
// 
//	[ Packet structure ]
//	STX
//	134=0020	// length of Body (without header)
//	DEF_DELI
//	Body
// ETX


int CPacketParser::Add_Packet(_In_ string & sRecvPacket)
{
   string sLenCode = StringFormat("%d=", FDS_PACK_LEN);   //134=
   
   while(true)
   {
      if( StringLen(sRecvPacket)<=0 )
         break;
    
      int nPos = StringFind(sRecvPacket, sLenCode);
      if(nPos<0)
      {
         PrintFormat("[E]CPacketParser::Add_Packet. No Length Code(%s)(%s)", sLenCode, sRecvPacket);
         return -1;
      }
      
      int nDataLen      = (int)StringToInteger( StringSubstr(sRecvPacket, nPos+4, 4 ) );  // 134=0129      
      int nOnePacketLen = nDataLen + DEF_HEADER_SIZE + 1;   //ADD 1 FOR ETX
      string sOnePacket = StringSubstr(sRecvPacket, nPos-1, nOnePacketLen);  // Copy packet from STX
      
      m_que.Add(sOnePacket);
      
      int nTotalLen = StringLen(sRecvPacket);
      int nRemainLen = nTotalLen - nOnePacketLen;
      if(nRemainLen<=0)
         break;
         
      string sTempBuf = StringSubstr(sRecvPacket, nOnePacketLen, nRemainLen);
      StringInit(sRecvPacket, 0);
      sRecvPacket = sTempBuf;
      
   }
   
   
   return (m_que.Count());
   
   
}
   
 // return the number of the remain packets in the queue
int CPacketParser::Get_OnePacket(_Out_ string & sPacket, _Out_ bool& bExist)
{
   bExist = false;
   if( m_que.Count()>0 )
   {
      sPacket = m_que.Dequeue();
      bExist = true;
   }
   
   return m_que.Count();
}

void CPacketParser::Clear()
{
   m_que.Clear();
}






#endif