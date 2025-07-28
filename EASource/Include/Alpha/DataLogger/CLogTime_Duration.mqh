#ifndef __LOGTIME_DURATION__
#define __LOGTIME_DURATION__


#include "LoggerCommon.mqh"
#include "../UtilDateTime.mqh"

class CLogTime_Duration
{
public:
   CLogTime_Duration();
   ~CLogTime_Duration();
   
   void     Set_WorkingTime(string startTime, string stopTime);
   void     Set_Duration(int duration);
   bool     CheckTime_Start(datetime dtNow );
   bool     CheckTime_Stop(datetime dtNow );
   
   bool     Is_RunningNow() { return m_bRunningNow; };
   
private:
   string   m_startTime;
   string   m_stopTime;
   int      m_duration;    // enum  {EN_DURATION_ONCE=0, EN_DURATION_REPEATED};
   
   string   m_sLastStartDate;
   
   bool     m_bRunningNow;
   //int      m_nRunningCnt;
};


CLogTime_Duration::CLogTime_Duration()
{
   m_bRunningNow  = false;
   //m_nRunningCnt  = 0;
}

CLogTime_Duration::~CLogTime_Duration()
{
}


void CLogTime_Duration::Set_WorkingTime(string startTime, string stopTime)
{
   m_startTime = startTime;
   m_stopTime  = stopTime;
}

void CLogTime_Duration::Set_Duration(int duration)
{
   m_duration = duration;
}



bool CLogTime_Duration::CheckTime_Start(datetime dtNow )
{
   if( Is_RunningNow() )
      return false;

   string sDayOfWeek = __DayOfWeek(dtNow);
   if( sDayOfWeek==DEF_SATURDAY || sDayOfWeek==DEF_SUNDAY )
      return false;
   
   string sToday = __YYYYMMDDToStr_withDot(dtNow); // yyyy.mm.dd
     
   if( m_sLastStartDate == sToday )        // start only ontime per day
   {
      return false;
   }
   else if ( m_sLastStartDate < sToday ) // check logDuration - once or daily  if( m_nRunningCnt > 0  && m_duration==EN_DURATION_ONCE )   
   {
      if( StringLen(m_sLastStartDate)>0 )
      {
         if( m_duration==EN_DURATION::Once )
            return false;
      }
   }
   
   
   string sNowTime = __HHMMToStr(dtNow); // hh:mm
   
   // start time <= now <= stop time
   if( StringCompare(m_startTime,sNowTime) <=0 &&
       StringCompare(sNowTime, m_stopTime) <0 )
   {
      m_bRunningNow = true;
      //m_nRunningCnt++;
      m_sLastStartDate = sToday;
      PrintFormat("[START] Now(%s) is WriteStartTime(%s). Duration(%d) LastStartDate(%s)", sNowTime, m_startTime, m_duration, m_sLastStartDate);
   }
   
   return m_bRunningNow;
}


bool CLogTime_Duration::CheckTime_Stop(datetime dtNow)
{
   if( !Is_RunningNow() )
      return false;
  
   string sNow = __HHMMToStr(dtNow); // hh:mm
   if( StringCompare(m_stopTime,sNow) <=0 )
   {
      m_bRunningNow = false;
      PrintFormat("[STOP] Now(%s) is WriteStoptTime(%s). Duration(%d)", sNow, m_stopTime, m_duration);
      return true;
   }
   
   return false;
}




#endif