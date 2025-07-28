#ifndef __CEXPERT_RUNNINGTIME_ON_DAY_H__
#define __CEXPERT_RUNNINGTIME_ON_DAY_H__

/*

   Check the Expert running time.
   
   ex)
   CExpertGoStopByOperatingTime GoStopByTime;
   GoStopByTime.SetTime("00:01", "23:59");
   
   void OnTimer()
   {
      bool bFirstGoingOn;
      if( Is_GoingOn(0, bFirstGoingOn) )
      {
         // do work as now is operating time
         if( bFirstGoingOn )
         {
            // do initialize
         }
      }
      else
      {
         // do not work as now is not operating time
      }
   }

*/


#include "Utils.mqh"
#include "UtilDateTime.mqh"


class CExpertGoStopByOperatingTime
{
public:
   CExpertGoStopByOperatingTime(){ m_bNowRunning = false;};
   ~CExpertGoStopByOperatingTime(){};
   
   // hh:mm
   bool  SetTime(string sStartTime, string sEndTime);
   
   bool  Is_GoingOn(datetime i_dtNow, bool& bFirstGoingOn);
   
   string  GetMsg() { return m_sMsg; }
   
private:
   bool  IsToday_Weekend(datetime dtNow);
   
private:
   bool     m_bNowRunning;
   string   m_sStart, m_sEnd; //hh:mm
   string   m_sMsg;
};


bool CExpertGoStopByOperatingTime::SetTime(string sStartTime, string sEndTime)
{
   int ret = __Validate_HH_MM(sStartTime);
   if(ret<0){
      PrintFormat("[SetTime]StartTime format must be hh:mm(%s)(ret:%d)", sStartTime, ret);
      return false;
   }
   
   ret = __Validate_HH_MM(sEndTime);
   if(ret<0){
      PrintFormat("[SetTime]EndTime format must be hh:mm(%s)(ret:%d)", sEndTime, ret);
      return false;
   }
   
   if( StringCompare(sStartTime, sEndTime) >= 0 )
   {
      m_sMsg = "[SetTime]EndTime must be greater than StartTime";
      Print(m_sMsg);
      return false;
   }

   
   m_sStart = sStartTime;
   m_sEnd   = sEndTime;
   
   return true;
}

// bStartOnDay : Is this the 1st GoingOn?
bool CExpertGoStopByOperatingTime::Is_GoingOn(datetime i_dtNow, bool& bFirstGoingOn)
{
   bFirstGoingOn = false;
   
   datetime dtNow = i_dtNow;
   if( dtNow==0 )
      dtNow = TimeCurrent();
   
   // do not go on in weekend
   string sDayOfWeek = __DayOfWeek(dtNow);
   if( sDayOfWeek==DEF_SATURDAY || sDayOfWeek==DEF_SUNDAY )
      return false;
   
   
   string sNow = __TimeTo_HH_MM(dtNow);      // hh:mm
   
   //PrintFormat("start:%s, now:%s, end:%s", m_sStart, sNow, m_sEnd);   
   if( StringCompare(m_sStart,sNow) <=0 && StringCompare(sNow,m_sEnd) <0 )
   {
      // only for the logging
      if( !m_bNowRunning )
      {
         bFirstGoingOn  = true;  // the 1st going on at this day
         m_bNowRunning  = true;
         m_sMsg = StringFormat("[GoOn] Start(%s) <= Now(%s) < End(%s)", m_sStart, sNow, m_sEnd);
         Print(m_sMsg);
      }
      return true;
   }
   
   m_bNowRunning = false;
   
   return false;   
}



#endif