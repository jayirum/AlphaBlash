#ifndef __CCLIENT_SOCKET_MQH__
#define __CCLIENT_SOCKET_MQH__

#include "socketlib.mqh"

#include "_incCommon.mqh"
#include "Protocol.mqh"

enum EN_RECV_RET { RECVRET_OK=0, RECVRET_DISCONN, RECVRET_ERR, RECVRET_TIMEOUT};

#define RECV_BUF_SIZE   512


class CClientSocket
{
public:
   CClientSocket();
   ~CClientSocket();
   
   bool        Initialize(string sIP, ushort nPort, bool bInitSockLib);
   bool        ConnectSvr();
   bool        SendToSvr(_In_ string& sendBuf, int nSendLen);
   EN_RECV_RET ReceiveData(_Out_ string& o_sRecvPacket, _Out_ int& o_nRecvSize);
      
   string GetMsg() { return m_sMsg;}
private:
   void  DeInitialize();   
private:
   bool        m_bInitSockLib;
   SOCKET64    m_sock;
   sockaddr_in m_SockAddr;
   string      m_sSvrIp;
   ushort      m_usSvrPort;
   string      m_sMsg;
};


CClientSocket::CClientSocket()
{
   m_sock = INVALID_HANDLE;
}

CClientSocket::~CClientSocket()
{}


void CClientSocket::DeInitialize()
{
   if(m_sock!=INVALID_SOCKET64)
   {
      if(shutdown(m_sock,SD_BOTH)==SOCKET_ERROR) Print("-Shutdown failed error: "+WSAErrorDescript(WSAGetLastError()));
      closesocket(m_sock); m_sock=INVALID_SOCKET64;
   }
   
   if(m_bInitSockLib)
      WSACleanup();
}


bool CClientSocket::Initialize(string sIP, ushort nPort, bool bInitSockLib)
{
   m_sSvrIp       = sIP;
   m_usSvrPort    = nPort;
   m_bInitSockLib = bInitSockLib;
   
   if(m_bInitSockLib)
   {
      char wsaData[]; ArrayResize(wsaData,sizeof(WSAData));
      int res=WSAStartup(MAKEWORD(2,2), wsaData);
      if(res!=0) { 
         m_sMsg = StringFormat("WSAStartup failed error(%s)", res); 
         return false; 
      }
   }
   
   m_sock=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
   if(m_sock==INVALID_SOCKET64) { 
      m_sMsg = StringFormat("Create failed error(%s)", WSAErrorDescript(WSAGetLastError()));  
      return false; 
   }
   
   char ch[]; StringToCharArray(m_sSvrIp,ch);
   
   m_SockAddr.sin_family=AF_INET;
   m_SockAddr.sin_addr.u.S_addr=inet_addr(ch);
   m_SockAddr.sin_port=htons(m_usSvrPort);
   
   return true;
}

bool CClientSocket::ConnectSvr()
{
   ref_sockaddr ref; 
   ref.in=m_SockAddr;
   int res=connect(m_sock,ref.ref,sizeof(m_SockAddr));
   if(res==SOCKET_ERROR)
   {
      int err=WSAGetLastError();
      if(err!=WSAEISCONN) { 
         m_sMsg = StringFormat("SendSock Connect failed error(%s)", WSAErrorDescript(err)); 
         return false; 
      }
   }

   // set to nonblocking mode
   int non_block=1;
   res=ioctlsocket(m_sock,(int)FIONBIO,non_block);
   if(res!=NO_ERROR) { 
      m_sMsg = StringFormat("SendSock ioctlsocket failed error(%s)", string(res)); 
      return false; 
   }
   
   m_sMsg = StringFormat("Connect Server ok (%s)(%d)", m_sSvrIp, m_usSvrPort);
   return true;
}


bool CClientSocket::SendToSvr(_In_ string& sendBuf, int nSendLen)
{
   bool bRetval = true;
   uchar arr[];
   StringToCharArray(sendBuf, arr);
   
   int res=send(m_sock, arr,nSendLen,0);
   if(res==SOCKET_ERROR){ 
      m_sMsg = StringFormat("Send error(%s)",WSAErrorDescript(WSAGetLastError())); 
      return false; 
   }
   return true;
}



EN_RECV_RET CClientSocket::ReceiveData(_Out_ string& o_sRecvPacket, _Out_ int& o_nRecvSize)
{
   uchar FullData[];
   char recvBuf[RECV_BUF_SIZE]; 
   int nAllRecvLen=0; 
   bool bNext=false;
   int res = 0;
   
   
   while(true)
   {
      res=recv(m_sock, recvBuf, RECV_BUF_SIZE, 0);
      if(res<0)
      {
         int err=WSAGetLastError();
         if(err==WSAEWOULDBLOCK) { 
            return RECVRET_TIMEOUT; 
         }
         else { 
            //Print("-Receive failed error: "+string(err)+" "+WSAErrorDescript(err)); /*CloseClean();*/ 
            return RECVRET_ERR; 
         }
            
      }
      else if(res==0 && nAllRecvLen==0) { 
         return RECVRET_DISCONN ; 
      }
      else if(res>0) { 
         nAllRecvLen+=res; 
         ArrayCopy(FullData, recvBuf, ArraySize(FullData),0,res); 
      }
      
      
      if(res>=0 && res<RECV_BUF_SIZE) 
         break;
   }

   if(nAllRecvLen<=0)
      return RECVRET_ERR;
      
   
   o_sRecvPacket = CharArrayToString(FullData);

   o_nRecvSize = StringLen(o_sRecvPacket);
   
   //PrintFormat("[RECEIVE](%s)", o_sRecvPacket);
 
   return RECVRET_OK;  
}

#endif