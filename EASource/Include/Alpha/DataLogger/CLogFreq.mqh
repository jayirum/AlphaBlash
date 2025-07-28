#ifndef __LAST_SAVE_TIME__
#define __LAST_SAVE_TIME__

#include "LoggerCommon.mqh"




class CLogFreq
{
public:
   CLogFreq(EN_LOGGER_TP tp);
   ~CLogFreq(){};
   
   void  SetLogFreq(int nLogFreq){m_logFreq = nLogFreq;};
   bool  Is_TimeToSave(string sCurrTime);


private:
   bool  Is_TimeToSave_Tick(string sCurrTime);
   bool  Is_TimeToSave_Sec(string sCurrTime);
   bool  Is_TimeToSave_MinFreq(string sCurrTime);
   bool  Is_TimeToSave_HourFreq(string sCurrTime);
   
   
private:
   int      m_logFreq;
   string   m_sLastSaveTime;
   int      m_loggerTp;
};

CLogFreq::CLogFreq(EN_LOGGER_TP tp)
{
   m_loggerTp = tp;
   m_logFreq=-1;
};

bool  CLogFreq::Is_TimeToSave(string sCurrTime)
{
   bool bRes = false;
   
   if( m_loggerTp==LOGGER_TP_MD )
   {
      #ifdef __MD_LOGGER__   
      #ifndef __FREE_VERSION__
      if(m_logFreq==EN_MDLOGGER_FREQ::Tick)
      {
         bRes = Is_TimeToSave_Tick(sCurrTime);
      }
      #endif
      #endif      
   }
   else
   {
      #ifdef __USER_LOGGER__   
      #ifndef __FREE_VERSION__
      if(m_logFreq==EN_USERLOGGER_FREQ::Sec)
      {
         bRes = Is_TimeToSave_Sec(sCurrTime);
      }
      #endif
      #endif      
   }
   
   switch(m_logFreq)
   {
#ifdef __MD_LOGGER__      

   #ifndef __FREE_VERSION__
      case EN_MDLOGGER_FREQ::M5:
      case EN_MDLOGGER_FREQ::M15:
      case EN_MDLOGGER_FREQ::M30:
         bRes = Is_TimeToSave_MinFreq(sCurrTime);   
         break;
      case EN_MDLOGGER_FREQ::H1:
      case EN_MDLOGGER_FREQ::H4:
         bRes = Is_TimeToSave_HourFreq(sCurrTime);   
         break;
   #endif
      case EN_MDLOGGER_FREQ::M1:
      bRes = Is_TimeToSave_MinFreq(sCurrTime);   
         break;
#endif

#ifdef __USER_LOGGER__     
 
   #ifndef __FREE_VERSION__
      case EN_USERLOGGER_FREQ::M5:
      case EN_USERLOGGER_FREQ::M15:
      case EN_USERLOGGER_FREQ::M30:
         bRes = Is_TimeToSave_MinFreq(sCurrTime);   
         break;
      case EN_USERLOGGER_FREQ::H1:
      case EN_USERLOGGER_FREQ::H4:
         bRes = Is_TimeToSave_HourFreq(sCurrTime);   
         break;
   #endif
      case EN_USERLOGGER_FREQ::M1:
         bRes = Is_TimeToSave_MinFreq(sCurrTime);   
         break;
   
   
#endif
   }
   
   return bRes;
}




bool  CLogFreq::Is_TimeToSave_Tick(string sCurrTime)
{
   return true;
}



// hh:mm:ss
bool  CLogFreq::Is_TimeToSave_Sec(string sCurrTime)
{
   return true;
}


// hh:mm:ss
bool  CLogFreq::Is_TimeToSave_MinFreq(string sCurrTime)
{
   bool bRes = false;

   int nCurrMin = (int)StringToInteger( StringSubstr(sCurrTime,3,2) ); //mm

   int nDivisor = 1;
   switch(m_logFreq)
   {
#ifdef __MD_LOGGER__   
   
   #ifndef __FREE_VERSION__
      case EN_MDLOGGER_FREQ::M5: nDivisor = 5; break;
      case EN_MDLOGGER_FREQ::M15: nDivisor = 15; break;
      case EN_MDLOGGER_FREQ::M30: nDivisor = 30; break;
   #endif   
      case EN_MDLOGGER_FREQ::M1: nDivisor = 1; break;   
   
#endif   

#ifdef __USER_LOGGER__   
   #ifndef __FREE_VERSION__
      case EN_USERLOGGER_FREQ::M5: nDivisor = 5; break;
      case EN_USERLOGGER_FREQ::M15: nDivisor = 15; break;
      case EN_USERLOGGER_FREQ::M30: nDivisor = 30; break;
   #endif
      case EN_USERLOGGER_FREQ::M1: nDivisor = 1; break;
#endif   
   }
   
   int nRemain = (int)MathMod(nCurrMin, nDivisor);
   if( nRemain == 0 )
   {
      if( StringSubstr(m_sLastSaveTime,0,5) != StringSubstr(sCurrTime,0,5)  )
         bRes = true;
   }
   
   if(bRes)
      m_sLastSaveTime = sCurrTime;
      
   return bRes;
}


// hh:mm:ss
bool  CLogFreq::Is_TimeToSave_HourFreq(string sCurrTime)
{
   bool bRes = false;
   
   int nCurrHour = (int)StringToInteger( StringSubstr(sCurrTime,0,2) ); //hh

   int nDivisor = 1;
   switch(m_logFreq)
   {
   
#ifdef __MD_LOGGER__   
   #ifndef __FREE_VERSION__
      case EN_MDLOGGER_FREQ::H1: nDivisor = 1; break;
      case EN_MDLOGGER_FREQ::H4: nDivisor = 4; break;
   #endif
#endif   
   
#ifdef __USER_LOGGER__   
   #ifndef __FREE_VERSION__
      case EN_USERLOGGER_FREQ::H1: nDivisor = 1; break;
      case EN_USERLOGGER_FREQ::H4: nDivisor = 4; break;
   #endif   
#endif
   
   }
      
   int nRemain = (int)MathMod(nCurrHour, nDivisor);
   if( nRemain == 0 )
   {
      if( StringSubstr(m_sLastSaveTime,0,2) != StringSubstr(sCurrTime,0,2)  )
         bRes = true;
   }
   
   if(bRes)
      m_sLastSaveTime = sCurrTime;
      
   return bRes;
}



#endif