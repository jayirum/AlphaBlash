#ifndef __CCLIENT_SOCKET_LEGACY_MQH__
#define __CCLIENT_SOCKET_LEGACY_MQH__

/* *******************************************************************************

Socket library, for both MT4 and MT5 (32-bit and 64-bit)

Features:

   * Both client and server sockets
   * Both send and receive
   * Both MT4 and MT5 (32-bit and 64-bit)
   * Optional event-driven handling (EAs only, not scripts or indicators),
     offering faster responses to socket events than OnTimer()
   * Direct use of Winsock; no need for a custom DLL sitting 
     between this code and ws2_32.dll

Based on the following forum posts:

   https://www.mql5.com/en/forum/203049#comment_5232176
   https://www.mql5.com/en/forum/160115/page3#comment_3817302


CLIENT SOCKETS
--------------

You create a connection to a server using one of the following two 
constructors for ClientSocket:

   ClientSocket(ushort localport);
   ClientSocket(string HostnameOrIPAddress, ushort port);

The first connects to a port on localhost (127.0.0.1). The second
connects to a remote server, which can be specified either by
passing an IP address such as "123.123.123.123" or a hostname
such as "www.myserver.com".

After creating the instance of the class, and periodically afterwards,
you should check the value of IsSocketConnected(). If false, then 
the connection has failed (or has later been closed). You then need to 
destroy the class and create a new connection. One common pattern of usage
therefore looks like the following:

   ClientSocket * glbConnection = NULL;

   void OnTick()
   {
      // Create a socket if none already exists
      if (!glbConnection) glbConnection = new ClientSocket(12345);
      
      if (glbConnection.IsSocketConnected()) {
         // Socket is okay. Do some action such as sending or receiving
      }
      
      // Socket may already have been dead, or now detected as failed
      // following the attempt above at sending or receiving.
      // If so, delete the socket and try a new one on the next call to OnTick()
      if (!glbConnection.IsSocketConnected()) {
         delete glbConnection;
         glbConnection = NULL;            
      }
   }

You send data down a socket using the simple Send() method, which takes
a string parameter. Any failure to send returns false, which will
also mean that IsSocketConnected() then returns false. The format
of the data which you are sending to the server is obviously
entirely up to you...

You can receive pending incoming data on a socket using Receive(), which
returns either the pending data or an empty string. You will normally want
to call Receive() from OnTimer(), or using the event handling described below.

A non-blank return value from Receive() does not necessarily mean that 
the socket is still active. The server may have sent some data *and* closed 
the socket. 

   string strMessage = MySocket.Receive();
   if (strMessage != "") {
      // Process the message
   }
   
   // Regardless of whether there was any data, the socket may
   // now be dead.
   if (!MySocket.IsSocketConnected()) {
      // ... socket has been closed
   }

You can also give Receive() an optional message terminator, such as "\r\n".
It will then store up data, and only return complete messages (minus the
terminator). If you use a terminator then there may have been multiple
complete messages since your last call to Receive(), and you should
keep calling Receive() until it returns an empty string, in order to collect
all the messages. For example:

   string strMessage;
   do {
      strMessage = MySocket.Receive("\r\n");
      if (strMessage != "") {
         // Do something with the message
      }
   } (while strMessage != "");

You close a socket simply by destroying the ClientSocket object.



EVENT-DRIVEN HANDLING
---------------------

The timing infrastructure in Windows does not normally have millisecond 
granularity. EventSetMillisecondTimer(1) is usually in fact equivalent 
to EventSetMillisecondTimer(16). Therefore, checks for socket
activity in OnTimer() potentially have a delay of at least 16 milliseconds
before you respond to a new connection or incoming data.

In an EA (but not a script or indicator) you can achieve <1ms response
times using event-driven handling. The way this works is that the 
socket library generates dummy key-down messages to OnChartEvent() 
when socket activity occurs. Responding to these events can be 
significantly faster than a periodic check in OnTimer().

You need to request the event-driven handling by #defining SOCKET_LIBRARY_USE_EVENTS
before including the library. For example:

   #define SOCKET_LIBRARY_USE_EVENTS
   #include <socket-library-mt4-mt5.mqh>

(Note that this has no effect in a custom indicator or script. It only
works with EAs.)

You then process notifications in OnChartEvent as follows:

   void OnChartEvent(const int id, const long& lparam, const double& dparam, const string& sparam)
   {
      if (id == CHARTEVENT_KEYDOWN) {
         // May be a real key press, or a dummy notification
         // resulting from socket activity. If lparam matches
         // any .GetSocketHandle() then it's from a socket.
         // If not, it's a real key press. (If lparam>256 then
         // it's also pretty reliably a socket message rather 
         // than a real key press.)
         
         if (lparam == MyServerSocket.GetSocketHandle()) {
            // Activity on a server socket
         } else if (lparam == MyClientSocket.GetSocketHandle()) {
            // Activity on a client socket
         } else {
            // Doesn't match a socket. Assume real key pres
         }
      }
   }

For a comprehensive example of using the event-driven handling,
see the example socket server code.


NOTES ON MT4/5 CROSS-COMPATIBILITY
----------------------------------

It appears to be safe for a 64-bit application to use 4-byte socket
handles, despite the fact that the Win32 SDK defines SOCKET as 8-byte
on x64. Nevertheless, this code uses 8-byte handles when running
on 64-bit MT5.

The area which definitely does cause problems is gethostbyname(),
because it returns a memory block containing pointers whose size
depends on the environment. (The issue here is 32-bit vs 64-bit,
not MT4 vs MT5.)

This code not only needs to handle 4-byte vs 8-byte memory pointers.
The further problem is that MQL5 has no integer data type whose
size varies with the environment. Therefore, it's necessary to
have two versions of the #import of gethostbyname(), and to
force the compiler to use the applicable one despite the fact
that they only really vary by their return type. Manipulating
the hostent* returned by gethostbyname() is then very ugly,
made doubly so by the need for different paths of execution
on 32-bit and 64-bit, using a range of different #imports of
the RtlMoveMemory() function which the code uses to
process the pointers.

******************************************************************************* */


#include "_incCommon.mqh"
#include "Protocol.mqh"

#property strict

#define SOCKET_LIBRARY_USE_EVENTS

// -------------------------------------------------------------
// Winsock constants and structures
// -------------------------------------------------------------

#define SOCKET_HANDLE32       uint
#define SOCKET_HANDLE64       ulong
#define AF_INET               2
#define SOCK_STREAM           1
#define IPPROTO_TCP           6
#define INVALID_SOCKET32      0xFFFFFFFF
#define INVALID_SOCKET64      0xFFFFFFFFFFFFFFFF
#define SOCKET_ERROR          -1
#define INADDR_NONE           0xFFFFFFFF
#define FIONBIO               0x8004667E
#define WSAWOULDBLOCK         10035
#define WSAETIMEDOUT          10060
#define SO_LINGER       0x0080          /* linger on close if data present */
#define SOL_SOCKET      0xffff          /* options for socket level */
#define SO_SNDTIMEO     0x1005          /* send timeout */
#define SO_RCVTIMEO     0x1006          /* receive timeout */



struct sockaddr {
   short family;
   ushort port;
   uint address;
   ulong ignore;
};

struct linger {
   ushort onoff;
   ushort linger_seconds;
};

// -------------------------------------------------------------
// DLL imports
// -------------------------------------------------------------

#import "ws2_32.dll"
   // Imports for 32-bit environment
   SOCKET_HANDLE32 socket(int, int, int); // Artificially differs from 64-bit version based on 3rd parameter
   int connect(SOCKET_HANDLE32, sockaddr&, int);
   int closesocket(SOCKET_HANDLE32);
   int send(SOCKET_HANDLE32, uchar&[],int,int);
   int recv(SOCKET_HANDLE32, uchar&[], int, int);
   int ioctlsocket(SOCKET_HANDLE32, uint, uint&);
   int bind(SOCKET_HANDLE32, sockaddr&, int);
   int listen(SOCKET_HANDLE32, int);
   SOCKET_HANDLE32 accept(SOCKET_HANDLE32, int, int);
   int WSAAsyncSelect(SOCKET_HANDLE32, int, uint, int);
   int shutdown(SOCKET_HANDLE32, int);
   int setsockopt(SOCKET_HANDLE32, int, int, uint&, int);
   
   // Imports for 64-bit environment
   SOCKET_HANDLE64 socket(int, int, uint); // Artificially differs from 32-bit version based on 3rd parameter
   int connect(SOCKET_HANDLE64, sockaddr&, int);
   int closesocket(SOCKET_HANDLE64);
   int send(SOCKET_HANDLE64, uchar&[], int, int);
   int recv(SOCKET_HANDLE64, uchar&[], int, int);
   int ioctlsocket(SOCKET_HANDLE64, uint, uint&);
   int bind(SOCKET_HANDLE64, sockaddr&, int);
   int listen(SOCKET_HANDLE64, int);
   SOCKET_HANDLE64 accept(SOCKET_HANDLE64, int, int);
   int WSAAsyncSelect(SOCKET_HANDLE64, long, uint, int);
   int shutdown(SOCKET_HANDLE64, int);
   //int setsockopt(SOCKET_HANDLE64, int, int, linger&, int);
   int setsockopt(SOCKET_HANDLE32, int, int, uint&, int);
   
   // gethostbyname() has to vary between 32/64-bit, because
   // it returns a memory pointer whose size will be either
   // 4 bytes or 8 bytes. In order to keep the compiler
   // happy, we therefore need versions which take 
   // artificially-different parameters on 32/64-bit
   uint gethostbyname(uchar&[]); // For 32-bit
   ulong gethostbyname(char&[]); // For 64-bit

   // Neutral; no difference between 32-bit and 64-bit
   uint inet_addr(uchar&[]);
   int WSAGetLastError();
   uint htonl(uint);
   ushort htons(ushort);
   int WSAStartup();
#import

// For navigating the Winsock hostent structure, with indescribably horrible
// variation between 32-bit and 64-bit
#import "kernel32.dll"
   void RtlMoveMemory(uint&, uint, int);
   void RtlMoveMemory(ushort&, uint, int);
   void RtlMoveMemory(ulong&, ulong, int);
   void RtlMoveMemory(ushort&, ulong, int);
#import

// -------------------------------------------------------------
// Forward definitions of classes
// -------------------------------------------------------------

class ClientSocket;


// -------------------------------------------------------------
// Client socket class
// -------------------------------------------------------------

class ClientSocket
{
   private:
      // Need different socket handles for 32-bit and 64-bit environments
      SOCKET_HANDLE32 m_sock32;
      SOCKET_HANDLE64 m_sock64;
      
      // Other state variables
      bool m_bConnected;
      int m_nLastError;
      string mConnected; // Backlog of incoming data, if using a message-terminator in Receive()
      string m_sMsg;
      string m_sPendingData;
      
      string m_sSvrIP;
      ushort m_nPort;
      uint m_nSendTimeout;
      uint m_nRecvTimeout;
      
      uchar m_arrBuffer[MAX_BUF];
      string m_sBuffer;
      
      // Event handling
      bool m_bDoneEventHandling;
      
      bool m_bLogRecvData;
      
   private:   
      void SetupSocketEventHandling();
      
   public:
      // Constructors for connecting to a server, either locally or remotely
      //ClientSocket(ushort localport);
      //ClientSocket(string HostnameOrIPAddress, ushort port);
      ClientSocket(bool bLogRecvData=false);
      ~ClientSocket();
      
      void Initialize(string sIP, ushort nPort, int nSendTimeout, int nRecvTimeout);
      string GetMsg() { return m_sMsg;}
      uint GetSvrAddress(string sIP);
      int ConnectSvr();
      void CloseSocket();
      
      // Simple send and receive methods
      int Send(string strMsg);
      int Send(string sSendBuff, int nToSend);
      ALPHA_RET Receive(/*out*/string & sOutBuff, /*out*/int &nRecvSize);
      ALPHA_RET RecvWithBuffering(_Out_ int& nRecvSize);
      bool GetOnePacket(_Out_ string& sBuff, _Out_ bool& bErr);
      // State information
      bool IsSocketConnected() {return m_bConnected;}
      int GetLastSocketError() {return m_nLastError;}
      ulong GetSocketHandle() {return (m_sock32 ? m_sock32 : m_sock64);}
      
      // Buffer sizes, overwriteable once the class has been created
      
};


// -------------------------------------------------------------
// Constructor for a simple connection to 127.0.0.1
// -------------------------------------------------------------


ClientSocket::ClientSocket(bool bLogRecvData)
{
   m_sock32 = INVALID_SOCKET32;
   m_sock64 = INVALID_SOCKET64;
   m_bLogRecvData = bLogRecvData;
}


//
//struct WSAData32
//{
//   ushort            wVersion;
//   ushort            wHighVersion;
//   char              szDescription[256+1];
//   char              szSystemStatus[128+1];
//   unsigned short    iMaxSockets;
//   unsigned short    iMaxUdpDg;
//   char              &lpVendorInfo[];
//};
//
//
//struct WSAData64
//{
//   ushort         wVersion;
//   ushort         wHighVersion;
//   unsigned short iMaxSockets;
//   unsigned short iMaxUdpDg;
//   char           &lpVendorInfo[];
//   char           szDescription[256+1];
//   char           szSystemStatus[128+1];
//};

void ClientSocket::Initialize(string sIP, ushort nPort, int nSendTimeout, int nRecvTimeout)
{
//   WSAData32 wsaData32;
//   WSAData64 wsaData64;
//   
//   if(__IsMT4() )
//      WSAStartup(MAKEWORD(2, 2), wsaData32);
//   else
//      WSAStartup(MAKEWORD(2, 2), wsaData64);
   
   m_sSvrIP = sIP;
   m_nPort = nPort;
   m_nSendTimeout = nSendTimeout;
   m_nRecvTimeout = nRecvTimeout;
}

// Not an IP address. Need to look up the name
// .......................................................................................
// Unbelievably horrible handling of the hostent structure depending on whether
// we're in 32-bit or 64-bit, with different-length memory pointers. 
// Ultimately, we're having to deal here with extracting a uint** from
// the memory block provided by Winsock - and with additional 
// complications such as needing different versions of gethostbyname(),
// because the return value is a pointer, which is 4 bytes in x86 and
// 8 bytes in x64. So, we must artifically pass different types of buffer
// to gethostbyname() depending on the environment, so that the compiler
// doesn't treat them as imports which differ only by their return type.
uint ClientSocket::GetSvrAddress(string sIP)
{
   // Is the host parameter an IP address?
   uchar arrName[];
   StringToCharArray(sIP, arrName);
   ArrayResize(arrName, ArraySize(arrName) + 1);
   uint addr = inet_addr(arrName);
   
   if (addr == INADDR_NONE) 
   {
      if (__IsMT5() )
      {
         char arrName64[];
         ArrayResize(arrName64, ArraySize(arrName));
         for (int i = 0; i < ArraySize(arrName); i++) arrName64[i] = (char)arrName[i];
         ulong nres = gethostbyname(arrName64);
         if (nres == 0) 
         {
            // Name lookup failed
            m_nLastError = WSAGetLastError();
            m_sMsg = StringFormat("Name-resolution in gethostbyname() failed, 64-bit, error: ", m_nLastError);
            return 0;
         } 
         else 
         {
            // Need to navigate the hostent structure. Very, very ugly...
            ushort addrlen;
            RtlMoveMemory(addrlen, nres + 18, 2);
            if (addrlen == 0)
            {
               // No addresses associated with name
               m_sMsg = StringFormat("Name-resolution in gethostbyname() returned no addresses, 64-bit, error: ", m_nLastError);
               return 0;
            } 
            else 
            {
               ulong ptr1, ptr2, ptr3;
               RtlMoveMemory(ptr1, nres + 24, 8);
               RtlMoveMemory(ptr2, ptr1, 8);
               RtlMoveMemory(ptr3, ptr2, 4);
               addr = (uint)ptr3;
            }
         }
      }  // if (__IsMT5() )
      
      if (__IsMT4() )
      {
         uint nres = gethostbyname(arrName);
         if (nres == 0) 
         {
            // Name lookup failed
            m_nLastError = WSAGetLastError();
            m_sMsg = StringFormat("Name-resolution in gethostbyname() failed, 32-bit, error: ", m_nLastError);
            return 0;
         } else 
         {
            // Need to navigate the hostent structure. Very, very ugly...
            ushort addrlen;
            RtlMoveMemory(addrlen, nres + 10, 2);
            if (addrlen == 0) 
            {
               // No addresses associated with name
               m_sMsg = StringFormat("Name-resolution in gethostbyname() returned no addresses, 32-bit, error: ", m_nLastError);
               return 0;
            } 
            else 
            {
               int ptr1, ptr2;
               RtlMoveMemory(ptr1, nres + 12, 4);
               RtlMoveMemory(ptr2, ptr1, 4);
               RtlMoveMemory(addr, ptr2, 4);
            }
         }
      } // if (__IsMT4() )
   
   } // if (addr == INADDR_NONE) 
   else 
   {
      // The HostnameOrIPAddress parameter is an IP address,
      // which we have stored in addr
   }
   
   return addr;
}

int ClientSocket::ConnectSvr()
{
   CloseSocket();

   // Default buffer sizes
   
   // Need to create either a 32-bit or 64-bit socket handle
   m_bConnected = false;
   m_nLastError = 0;
   if (__IsMT5() ) 
   {
      uint proto = IPPROTO_TCP;
      m_sock64 = socket(AF_INET, SOCK_STREAM, proto);
      if (m_sock64 == INVALID_SOCKET64) 
      {
         m_nLastError = WSAGetLastError();
         m_sMsg = StringFormat("create socket error(%d)", m_nLastError);
         return E_CREATE_SOCK;
      }
   } 
   else 
   {
      int proto = IPPROTO_TCP;
      m_sock32 = socket(AF_INET, SOCK_STREAM, proto);
      if (m_sock32 == INVALID_SOCKET32) 
      {
         m_nLastError = WSAGetLastError();
         m_sMsg = StringFormat("create socket error(%d)", m_nLastError);
         return E_CREATE_SOCK;
      }
   }

   uint addr = GetSvrAddress(m_sSvrIP);
   if( addr==0 )
      return E_GET_NETWORK_ADDR;
      
   // Fill in the address and port into a sockaddr_in structure
   sockaddr server;
   server.family = AF_INET;
   server.port = htons(m_nPort);
   server.address = addr; // Already in network-byte-order

   // connect() call has to differ between 32-bit and 64-bit
   int res;
   if (__IsMT5())
      res = connect(m_sock64, server, sizeof(sockaddr));
   else
      res = connect(m_sock32, server, sizeof(sockaddr));
      
   if (res == SOCKET_ERROR) 
   {
      // Ooops
      m_nLastError = WSAGetLastError();
      m_sMsg = StringFormat("connect() to server failed(%d)(ip:%s,port%d) ", m_nLastError, m_sSvrIP, m_nPort);
      return E_CONNECT;
   } 
   else 
   {
      m_bConnected = true;   

      // Set up event handling. Can fail if called in OnInit() when
      // MT4/5 is still loading, because no window handle is available
      // SetupSocketEventHandling();
      
      if (__IsMT5() ) 
      {
         //ioctlsocket(m_sock64, FIONBIO, nonblock);
         setsockopt((unsigned int)m_sock64, SOL_SOCKET, SO_SNDTIMEO, m_nSendTimeout, sizeof(m_nSendTimeout));
	      setsockopt((unsigned int)m_sock64, SOL_SOCKET, SO_RCVTIMEO, m_nRecvTimeout, sizeof(m_nRecvTimeout));
      } 
      else 
      {
         //ioctlsocket(m_sock32, FIONBIO, nonblock);
         setsockopt((unsigned int)m_sock32, SOL_SOCKET, SO_SNDTIMEO, m_nSendTimeout, sizeof(m_nSendTimeout));
	      setsockopt((unsigned int)m_sock32, SOL_SOCKET, SO_RCVTIMEO, m_nRecvTimeout, sizeof(m_nRecvTimeout));
      }
   }
   return E_OK;
}


// -------------------------------------------------------------
// Destructor. Close the socket if created
// -------------------------------------------------------------

ClientSocket::~ClientSocket()
{
   CloseSocket();
}

void ClientSocket::CloseSocket()
{
   linger ling;
	ling.onoff = 1;   // 0 ? use default, 1 ? use new value
	ling.linger_seconds = 0;  // close session in this time
   
   if (__IsMT5())
   {
      if (m_sock64 != INVALID_SOCKET64) 
      {
   		//setsockopt( m_sock64, SOL_SOCKET, SO_LINGER, (ushort) ling&, sizeof(ling));
   		//-We can avoid TIME_WAIT on both of client and server side as we code above.
   		
         shutdown(m_sock64, 2);
         closesocket(m_sock64);
   		m_sock64 = INVALID_SOCKET64;
      }
   } 
   else 
   {
      if (m_sock32 != INVALID_SOCKET32) 
      {
   		//setsockopt(m_sock32, SOL_SOCKET, SO_LINGER, (ushort)ling, sizeof(ling));

         shutdown(m_sock32, 2);
         closesocket(m_sock32);
         m_sock32 = INVALID_SOCKET32;
      }
   }  
}

// -------------------------------------------------------------
// Simple send function which takes a string parameter
// -------------------------------------------------------------

int ClientSocket::Send(string strMsg)
{
   if (!m_bConnected) return E_NON_CONNECT;

   //SetupSocketEventHandling();
   
   int nToSend = StringLen(strMsg);
      
   bool bRetval = true;
   uchar arr[];
   StringToCharArray(strMsg, arr);
   
   while (nToSend > 0) 
   {
      int res; //, nAmountToSend = (nToSend > m_nSendBufferSize ? m_nSendBufferSize : nToSend);
      if (__IsMT5()) {
         res = send(m_sock64, arr, nToSend, 0);
      } else {
         res = send(m_sock32, arr, nToSend, 0);
      }
      if(res == 0) 
         break;
         
      if (res == SOCKET_ERROR )
      {
         m_nLastError = WSAGetLastError();
         if (m_nLastError == WSAETIMEDOUT) 
         {
            m_sMsg = "Send Timeout Error";
            return E_SEND;
         } 
         else 
         {
            m_sMsg = StringFormat("send() failed, error: ", m_nLastError);

            // Assume death of socket for any other type of error
            nToSend = -1;
            bRetval = false;
            m_bConnected = false;
            return E_SEND;
         }
      } 
      else 
      {
         nToSend -= res;
         if (nToSend > 0) 
         {
            // If further data remains to be sent, shuffle the array downwards
            // by copying it onto itself. Note that the MQL4/5 documentation
            // says that the result of this is "undefined", but it seems
            // to work reliably in real life (because it almost certainly
            // just translates inside MT4/5 into a simple call to RtlMoveMemory,
            // which does allow overlapping source & destination).
            ArrayCopy(arr, arr, 0, res, nToSend);
         }
      }
   }

   return ERR_OK;
}


// -------------------------------------------------------------
// Simple send function which takes an array of uchar[], 
// instead of a string. Can optionally be given a start-index
// within the array (rather then default zero) and a number 
// of bytes to send.
// -------------------------------------------------------------
//int Send(string sSendBuff, int nToSend = -1);
int ClientSocket::Send(string sSendBuff, int nToSend)  //(uchar & callerBuffer[], int nToSend = -1)
{
   if (!m_bConnected) return E_NON_CONNECT;

   //SetupSocketEventHandling();
   
   // Process the start-at and send-size parameters
   int arraySize = nToSend;   
   
   // Take a copy of the array 
   uchar arr[];
   StringToCharArray(sSendBuff, arr);
      
   bool bRetval = true;
   
   while (nToSend > 0) 
   {
      int res; //, szAmountToSend = (szToSend > m_nSendBufferSize ? m_nSendBufferSize : szToSend);
      if (__IsMT5()) {
         res = send(m_sock64, arr, nToSend, 0);
      } 
      else {
         res = send(m_sock32, arr, nToSend, 0);
      }
      
      if( res==0 )
         break;
         
      if (res == SOCKET_ERROR )  // || res == 0) 
      {
         m_nLastError = WSAGetLastError();
         if (m_nLastError == WSAETIMEDOUT) 
         {
            m_sMsg = "Send Timeout Error";
            return E_SEND;
         }  
         else 
         {
            m_sMsg = StringFormat("send() failed, error:%d", m_nLastError);

            // Assume death of socket for any other type of error
            nToSend = -1;
            bRetval = false;
            m_bConnected = false;
            return E_SEND;
         }
      } 
      else 
      {
         nToSend -= res;
         if (nToSend > 0) {
            // If further data remains to be sent, shuffle the array downwards
            // by copying it onto itself. Note that the MQL4/5 documentation
            // says that the result of this is "undefined", but it seems
            // to work reliably in real life (because it almost certainly
            // just translates inside MT4/5 into a simple call to RtlMoveMemory,
            // which does allow overlapping source & destination).
            ArrayCopy(arr, arr, 0, res, nToSend);
         }
      }
   } // while (nToSend > 0) 

   return ERR_OK;
}

//
//bool ClientSocket::GetOnePacket(/*out*/ string& sOutBuff)
//{
//   StringInit(sOutBuff,0);
//   
//   int nTotLen = StringLen(m_sBuffer);
//   int idxStx = -1;
//   
//PrintFormat("[1]TotalLen:%d", nTotLen);   
//   
//   for( int idxLoop=0; idxLoop<nTotLen; idxLoop++)
//   {
//      Sleep(0);
//      
//      // try to find STX
//      ushort charCode = StringGetChar(m_sBuffer, idxLoop);
//      
//      if( charCode==DEF_STX )
//      {
//PrintFormat("[2]Find STX(%d)", idxLoop);
//         idxStx = idxLoop;
//         
//         // Pure Data Length
//      	// STX134=00950x01
//      	string sLen = StringSubstr(m_sBuffer, idxStx+4+1, DEF_PACKETLEN_SIZE); // DEF_PACKETLEN_SIZE  135
//      	int nDataLen = (int)StringToInteger(sLen);
//      	if( nDataLen > 0 )
//      	{
//      	   int nPacketLen = 1   // stx
//      	                  + 3   // FDS_PACK_LEN		         134
//      	                  + 1   // =
//      	                  + 4   // FDS_PACK_LEN value
//      	                  + 1   // DEF_DELI
//      	                  + nDataLen
//      	                  ;
//            // copy
//            sOutBuff = StringSubstr( m_sBuffer, idxStx, nPacketLen );
//PrintFormat("[4]Copy buffer(%d)(%s)", nPacketLen, sOutBuff);
//            
//            if( nTotLen > nPacketLen )
//            {
//               int nNewStart = idxStx + nPacketLen;
//               string temp = StringSubstr( m_sBuffer, nNewStart, (nTotLen-nNewStart+1) ); 
//               StringInit(m_sBuffer, 0);
//               m_sBuffer = temp;
//            }
//            else
//               StringInit(m_sBuffer, 0);
//               
//            /**********/
//            return true;
//            /**********/
//         }
//         else
//         {
//            m_sMsg = StringFormat("[PACKET ERROR]STX ok, but packet length error(%s)", m_sBuffer);
//            StringInit(m_sBuffer, 0);
//            
//            /**********/
//            return false;
//            /**********/
//         }
//      } // if( charCode==DEF_STX )
//
//   } // for( int i=0; i<nTotLen; i++)
//   
//   if(idxStx<0)
//   {
//      m_sMsg = StringFormat("[PACKET ERROR]No STX(%s)", m_sBuffer);
//   }
//   // There is no STX. Clear the buffer
//   StringInit(m_sBuffer, 0);
//      
//   return false;
//}



// Assume strongly that the 1st data must be STX
bool ClientSocket::GetOnePacket(/*out*/ string& sOutBuff, _Out_ bool& bErr)
{
   bErr= false;
   
   int nTotLen = StringLen(m_sBuffer);
   if( nTotLen==0 )
      return false;
      
   StringInit(sOutBuff,0);
   int idxStx = 0;
   
//PrintFormat("[1]TotalLen:%d", nTotLen);   
    
   // Pure Data Length
	// STX134=00950x01
	string sLen = StringSubstr(m_sBuffer, idxStx+4+1, DEF_PACKETLEN_SIZE); // DEF_PACKETLEN_SIZE  135
	int nDataLen = (int)StringToInteger(sLen);
	if( nDataLen > 0 )
	{
	   int nPacketLen = 1   // stx
	                  + 3   // FDS_PACK_LEN		         134
	                  + 1   // =
	                  + 4   // FDS_PACK_LEN value
	                  + 1   // DEF_DELI
	                  + nDataLen
	                  ;
      // copy
      sOutBuff = StringSubstr( m_sBuffer, idxStx, nPacketLen );
//PrintFormat("[4]Copy buffer(PackLen:%d)(%s)", nPacketLen, sOutBuff);
      
      if( nTotLen > nPacketLen )
      {
         int nNewStart = idxStx + nPacketLen;
         string temp = StringSubstr( m_sBuffer, nNewStart, (nTotLen-nNewStart+1) ); 
         StringInit(m_sBuffer, 0);
         m_sBuffer = temp;
      }
      else
         StringInit(m_sBuffer, 0);
         
      /**********/
      return true;
      /**********/
   }
   else
   {
      bErr = true;
      m_sMsg = StringFormat("[PACKET ERROR]packet length error(%s)", m_sBuffer);
      StringInit(m_sBuffer, 0);
   }
   
      
   return false;
}


ALPHA_RET ClientSocket::RecvWithBuffering(_Out_ int& o_nRecvSize)
{
   o_nRecvSize = 0;
   if (!m_bConnected)
   {
      m_sMsg = "Not connected";
      return E_NON_CONNECT;
   }
   
   int nRecvCnt = 0;   
   StringInit(m_sBuffer, 0);
   while(true)
   {
      ArrayInitialize(m_arrBuffer, 0);
      int nRecvLen = 0;
      
      if (__IsMT5()) 
      {
         nRecvLen = recv(m_sock64, m_arrBuffer, MAX_BUF, 0);
      } 
      else 
      {
         nRecvLen = recv(m_sock32, m_arrBuffer, MAX_BUF, 0);
      }
      
      if(nRecvLen>0)
      {
         string sBuf = CharArrayToString(m_arrBuffer);
         if(m_bLogRecvData) PrintFormat("[SOCK_RECV](%s)",sBuf);
         
         m_sBuffer   += sBuf;
         o_nRecvSize += nRecvLen;
         nRecvCnt++;
         /******************/
         continue;
         /******************/
      }
      
      else if(nRecvLen<0)
      {
         m_nLastError = WSAGetLastError();
         
         if (m_nLastError == WSAETIMEDOUT)
         {
            /******************/
            return E_TIMEOUT; 
            /******************/
         }

         m_sMsg = StringFormat("recv failed, WSAGetLastError(%d)" , m_nLastError);
         m_bConnected = false;
         /******************/
         return E_RECV;
         /******************/
      }
      
      else if (nRecvLen == 0 )
      {
         if( nRecvCnt==0 )
         {
      	   // Socket closed
            m_sMsg = "Socket closed";
      	   m_bConnected = false;
      	   /******************/
            return E_DISCONN_FROM_SVR;
            /******************/
         }
         else if (nRecvLen>0)
            break;
      }
   } // while(true)
   
   return ERR_OK;   
}
      
      

// -------------------------------------------------------------
// Receive function which fills an array, provided by reference.
// Always clears the array. Returns the number of bytes 
// put into the array.
// If you send and receive binary data, then you can no longer 
// use the built-in messaging protocol provided by this library's
// option to process a message terminator such as \r\n. You have
// to implement the messaging yourself.
// -------------------------------------------------------------

ALPHA_RET ClientSocket::Receive(/*out*/string & sOutBuff, /*out*/int &nRecvSize)
{
   if (!m_bConnected)
   {
      m_sMsg = "Not connected";
      return E_NON_CONNECT;
   }
   
   //SetupSocketEventHandling();

   nRecvSize = 0;
   
   //ArrayResize(m_totBuffer, 0);
   ArrayInitialize(m_arrBuffer, 0);

   //uint nonblock = 1;
   //if (TerminalInfoInteger(TERMINAL_X64)) 
   //{
   //   ioctlsocket(m_sock64, FIONBIO, nonblock);
   //} else {
   //   ioctlsocket(m_sock32, FIONBIO, nonblock);
   //}

   int res;
   if (__IsMT5()) 
   {
      res = recv(m_sock64, m_arrBuffer, MAX_BUF, 0);
   } 
   else 
   {
      res = recv(m_sock32, m_arrBuffer, MAX_BUF, 0);
   }
   
   if (res == 0) 
   {
	 // Socket closed
      m_sMsg = "Socket closed";
	   m_bConnected = false;
      return E_DISCONN_FROM_SVR;
   } 
   else if(res<0)
   {
      m_nLastError = WSAGetLastError();
      if (m_nLastError != WSAETIMEDOUT)
      {
         m_sMsg = StringFormat("recv() failed, result:, " , res, ", error: ", m_nLastError);
         m_bConnected = false;
         return E_RECV;
      }
      else{
         return E_TIMEOUT;
      }   
   }
   
   nRecvSize = res;
   sOutBuff = CharArrayToString(m_arrBuffer);
   if(m_bLogRecvData) PrintFormat("[SOCK_RECV](%s)",sOutBuff);
   return ERR_OK;   
}

// -------------------------------------------------------------
// Event handling in client socket
// -------------------------------------------------------------

void ClientSocket::SetupSocketEventHandling()
{
   #ifdef SOCKET_LIBRARY_USE_EVENTS
      if (m_bDoneEventHandling) return;
      
      // Can only do event handling in an EA. Ignore otherwise.
      if (MQLInfoInteger(MQL_PROGRAM_TYPE) != PROGRAM_EXPERT) {
         m_bDoneEventHandling = true;
         return;
      }
      
      long hWnd = ChartGetInteger(0, CHART_WINDOW_HANDLE);
      if (!hWnd) return;
      m_bDoneEventHandling = true; // Don't actually care whether it succeeds.
      
      if (__IsMT5()) 
      {
         WSAAsyncSelect(m_sock64, hWnd, 0x100 /* WM_KEYDOWN */, 0xFF /* All events */);
      } 
      else 
      {
         WSAAsyncSelect(m_sock32, (int)hWnd, 0x100 /* WM_KEYDOWN */, 0xFF /* All events */);
      }
   #endif
}



#endif