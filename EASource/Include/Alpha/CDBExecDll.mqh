#ifndef __DB_EXEC_DLL_H__
#define __DB_EXEC_DLL_H__


#include "_IncCommon.mqh"

#import "AlphaUtils_mt4.dll"

   int Alpha_DBOpen(char& zSvrIpPort[], char& zUser[], char& zPwd[], char& zDBName[], _Out_ char& zMsg[]);
   int Alpha_DBExec(char& zQ[], _Out_ char& zMsg[]);
   int Alpha_DBClose();
#import

class CDBExecDll
{
public:
   CDBExecDll(){};
   ~CDBExecDll(){DeInit();};
   
   bool  Init(string sSvrIpPort, string sUser, string sPwd, string sDBName);   
   void  DeInit();
   bool  Exec(string sQ);
   int   GetErrCode() { return m_nErrCode;}
   bool  Connect();
   
private:
   int  Execute(string sQ);   
private:
   char m_zMsg[1024];
   
   char  m_zSvrIpPort[128];
   char  m_zUser[32];
   char  m_zPwd[32];
   char  m_zDBName[32];
   int   m_nErrCode;
};


bool CDBExecDll::Init(string sSvrIpPort, string sUser, string sPwd, string sDBName)
{
   StringToCharArray(sSvrIpPort, m_zSvrIpPort);
   StringToCharArray(sUser, m_zUser);
   StringToCharArray(sPwd, m_zPwd);
   StringToCharArray(sDBName, m_zDBName);
   
   return Connect();
}

bool CDBExecDll::Connect()
{
   m_nErrCode = Alpha_DBOpen(m_zSvrIpPort, m_zUser, m_zPwd, m_zDBName, m_zMsg);
   if( m_nErrCode!=E_OK )
   {
      Print( CharArrayToString(m_zMsg) );
      return false;
   }
   PrintFormat("DB Open OK(%s)(%s)", CharArrayToString(m_zSvrIpPort), CharArrayToString(m_zDBName));
   return true;
}

void CDBExecDll::DeInit()
{
   Alpha_DBClose();
}


bool CDBExecDll::Exec(string sQ)
{
   int ret;
   for( int i=0; i<2; i++ )
   {
      ret=Execute(sQ);
      if( ret==E_OK )
         return true;
         
      if( ret==E_DB_NOT_OPENED )
      {
         Connect();
      }      
   }
   
   return false;
}

int CDBExecDll::Execute(string sQ)
{
   char zQ[1024];
   StringToCharArray(sQ, zQ);
   m_nErrCode = Alpha_DBExec(zQ, m_zMsg);
   if( m_nErrCode!=E_OK )
   {
      Print( CharArrayToString(m_zMsg) );   
   }
   return m_nErrCode;
}
   
   
#endif