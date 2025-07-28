#ifndef __CCOMMAND_ADMIN_H__
#define __CCOMMAND_ADMIN_H__

#include "CommandCodes.mqh"
#include "Protocol.mqh"

class CCommandAdmin
{
public:
   CCommandAdmin();
   ~CCommandAdmin();

   void  Excute_ByAdminCode(string sCommandCode);
   bool  Is_Paused() { return m_bPaused; }
private:
   bool  m_bPaused;
};


CCommandAdmin::CCommandAdmin()
{
   m_bPaused = false;
}

CCommandAdmin::~CCommandAdmin()
{}

void CCommandAdmin::Excute_ByAdminCode(string sCommandCode)
{
   if( sCommandCode==CMD_TERMINATE_EA )
   {
      __KillEA("Terminate EA by Command");
      return;
   }
   else if ( sCommandCode==CMD_PAUSE_EA )
   {
      m_bPaused = true;
   }
   else if ( sCommandCode==CMD_RESUME_EA )
   {
      m_bPaused = false;
   }
   
}




#endif