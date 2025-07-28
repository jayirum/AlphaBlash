#ifndef __TIMEFRAME_FREQ_H
#define __TIMEFRAME_FREQ_H

#include "UtilDateTime.mqh"


enum EN_TIME_FRAME
{
   //TFRAME_SEC = 0,
   //TFRAME_TICK,
   TFRAME_M1    = PERIOD_M1,
   TFRAME_M5    = PERIOD_M5,
   TFRAME_M15   = PERIOD_M15,
   TFRAME_M30   = PERIOD_M30,
   TFRAME_H1      = PERIOD_H1,
   TFRAME_H4      = PERIOD_H4,
   TFRAME_D1      = PERIOD_D1,
   TFRAME_W1      = PERIOD_W1,
   TFRAME_MN1     = PERIOD_MN1,
};



string TimeFrameToStr(EN_TIME_FRAME enTFrame)
{
   string s;
   switch(enTFrame)
   {
   //case TFRAME_SEC:     s = "Sec";  break;
   //case TFRAME_TICK:    s = "Tick";  break;
   case TFRAME_M1:    s = "1Min";  break;
   case TFRAME_M5:    s = "5Min";  break;
   case TFRAME_M15:   s = "15Min";  break;
   case TFRAME_M30:   s = "30Min";  break;
   case TFRAME_H1:      s = "1Hour";  break;
   case TFRAME_H4:      s = "4Hour";  break;
   case TFRAME_D1:      s = "1Day";  break;
   case TFRAME_W1:      s = "1Week";  break;
   case TFRAME_MN1:      s = "1Mon";  break;
   }
   
   return s;
}

class CTimeFrameFreq
{
public:
   CTimeFrameFreq();
   ~CTimeFrameFreq(){};
   
   
   string   GetErrMsg() { return m_sMsg; };

   bool     Is_TimeOfTimeFrame(datetime dtNow, EN_TIME_FRAME enTFrame);

private:
   bool  Is_Time_Sec(string sCurrTime);
   bool  Is_Time_Tick(string sCurrTime);
   
   bool  Is_Time_Min_TimeFrame(string sCurrTime, EN_TIME_FRAME nTimeFrame);
   bool  Is_Time_Hour_TimeFrame(string sCurrTime, EN_TIME_FRAME nTimeFrame);
   bool  Is_Time_Day_TimeFrame(string sCurrTime){ return true;};
   bool  Is_Time_Week_TimeFrame(string sCurrTime){ return true;};
   bool  Is_Time_Month_TimeFrame(string sCurrTime){ return true;};
   
   
private:

   string   m_sLastChkTime;
   string   m_sMsg;
};

CTimeFrameFreq::CTimeFrameFreq()
{
};

bool CTimeFrameFreq::Is_TimeOfTimeFrame(datetime dtNow, EN_TIME_FRAME enTFrame)
{
   string sCurrTime = __TimeTo_HH_MM_SS(dtNow); //hh:mm:ss

   bool bRes = false;
   
   switch(enTFrame)
   {
   //case TFRAME_SEC:
   //   bRes = Is_Time_Sec(sCurrTime);
   //   break;
   //case TFRAME_TICK:
   //   bRes = Is_Time_Tick(sCurrTime);
   //   break;
   case TFRAME_M1:
   case TFRAME_M5:
   case TFRAME_M15:
   case TFRAME_M30:
      bRes = Is_Time_Min_TimeFrame(sCurrTime, enTFrame);
      break;
   case TFRAME_H1:
   case TFRAME_H4:
      bRes = Is_Time_Hour_TimeFrame(sCurrTime, enTFrame);
      break;
   case TFRAME_D1:
      bRes = Is_Time_Day_TimeFrame(sCurrTime);
      break;
   case TFRAME_W1:
      bRes = Is_Time_Week_TimeFrame(sCurrTime);
      break;
   case TFRAME_MN1:
      bRes = Is_Time_Month_TimeFrame(sCurrTime);
      break;
   default:
      m_sMsg = "Wrong TimeFrame";
   }
   
   
   return bRes;
}




bool  CTimeFrameFreq::Is_Time_Tick(string sCurrTime)
{
   return true;
}



// hh:mm:ss
bool  CTimeFrameFreq::Is_Time_Sec(string sCurrTime)
{
   return true;
}


// hh:mm:ss
bool  CTimeFrameFreq::Is_Time_Min_TimeFrame(string sCurrTime, EN_TIME_FRAME nTimeFrame)
{
   int nCurrMin = (int)StringToInteger( StringSubstr(sCurrTime,3,2) ); //mm

   int nDivisor = 0;
   switch(nTimeFrame)
   {
   case TFRAME_M1:    nDivisor = 1;  break;
   case TFRAME_M5:    nDivisor = 5;  break;
   case TFRAME_M15:   nDivisor = 15;  break;
   case TFRAME_M30:   nDivisor = 30;  break;
   default:
      m_sMsg = "Min TimeFrame must be 1/5/15/30";
      return false;
   }
   
   
   bool bRes = false;
   int nRemain = (int)MathMod(nCurrMin, nDivisor);
   if( nRemain == 0 )
   {
      // hh:mm:ss
      if( StringSubstr(m_sLastChkTime,0,5) != StringSubstr(sCurrTime,0,5)  )
         bRes = true;
   }
   
   if(bRes)
      m_sLastChkTime = sCurrTime;
      
   return bRes;
}


// hh:mm:ss
bool  CTimeFrameFreq::Is_Time_Hour_TimeFrame(string sCurrTime, EN_TIME_FRAME nTimeFrame)
{
   bool bRes = false;
   
   int nCurrHour = (int)StringToInteger( StringSubstr(sCurrTime,0,2) ); //hh

   int nDivisor = 0;
   switch(nTimeFrame)
   {
   case TFRAME_H1:    nDivisor = 1;  break;
   case TFRAME_H4:    nDivisor = 4;  break;
   default:
      m_sMsg = "Hour TimeFrame must be 1 or 4";
      return false;
   }

      
   int nRemain = (int)MathMod(nCurrHour, nTimeFrame);
   if( nRemain == 0 )
   {
      return true;
   }
     
   return false;
}



#endif