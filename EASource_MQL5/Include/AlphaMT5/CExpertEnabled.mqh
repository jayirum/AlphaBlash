#ifndef __CEXPERT_ENABLED_H__
#define __CEXPERT_ENABLED_H__

/*

   Check the Expert is enabled or not.
   
   ex)
   CExpertEnabled *expertEnabled = new CExpertEnabled;
   
   void OnTimer()
   {
      if( !_expertEnabled.Is_Enabled() )
      {
         showMsg("EA is stopped");   
         return;
      }

   }
*/


#include "_IncCommon.mqh"
#include "Utils.mqh"


class CExpertEnabled
{
public:
   CExpertEnabled();
   ~CExpertEnabled();
   
   bool  Is_Enabled();
   
private:
   bool  m_bExpertEnabled;
};


CExpertEnabled::CExpertEnabled()
{
   m_bExpertEnabled = false;
}
 
CExpertEnabled::~CExpertEnabled()
{
}

   
bool CExpertEnabled::Is_Enabled()
{
   if( IsStopped())  // Checks the forced shutdown of an mql4 program.
   {
      // The Expert Advisor is not stopped immediately as you call ExpertRemove(); just a flag to stop the EA operation is set. 
      //That is, any next event won't be processed, 
      // OnDeinit() will be called and the Expert Advisor will be unloaded and removed from the chart.
      ExpertRemove();
      return false;
   }
   
   if( !m_bExpertEnabled )
   {
      if( __IsExpertEnabled() )
      {
         m_bExpertEnabled = true;
         Print("Expert is enabled");
      }
      else
      {
         m_bExpertEnabled = false;
      }
   }
   else
   {
      if( __IsExpertEnabled() )
      {
         m_bExpertEnabled = true;
      }
      else
      {
         m_bExpertEnabled = false;
         Print("Expert is disabled");
      }
   }
   
   return m_bExpertEnabled;
}


   

#endif