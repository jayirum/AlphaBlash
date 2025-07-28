#ifndef __CREPEAT_DAY_H__
#define __CREPEAT_DAY_H__


/*
   Repeat some action on the specific time
   - only one time
   - every day
   - every week day
   
   ex) 
   CRepeatType*   repeatTp = new CRepeatType;
   
     
   repeatTp.SetRepeatType_Once("2021.09.10", "10:00");
   or
   repeatTp.SetRepeatType_EveryDay("10:00");
   or
   repeatTp.SetRepeatType_Monay("10:00");
         or/and
   repeatTp.SetRepeatType_Tuesday("11:00");
         or/and
   repeatTp.SetRepeatType_Wednesday("12:00");
         or/and
   repeatTp.SetRepeatType_Thursday("13:00");
         or/and
   repeatTp.SetRepeatType_Friday("14:00");
   
   
   void OnTimer()
   {
      if( IsTime_ToRepeatAction() )
      {
         // do action
      }
   }
*/

#include "UtilDateTime.mqh"

enum EN_REPEAT_TP { REPEAT_NONE = -1, REPEAT_ONCE, REPEAT_EVERDAY, REPEAT_WEEKDAY };
enum EN_TIME_IDX { IDX_ONCE=0, IDX_EVERYDAY, IDX_MON, IDX_TUE, IDX_WED, IDX_THUR, IDX_FRI } ;

#define ACTIONTIME_CNT 7

class CRepeatType
{
public:
   CRepeatType();
   ~CRepeatType(){};
   
   bool  SetRepeatType_Once      (string sActionDate, string sActionTime);
   bool  SetRepeatType_EveryDay  (string sActionTime);
   bool  SetRepeatType_Monday    (string sActionTime);
   bool  SetRepeatType_Tuesday   (string sActionTime);
   bool  SetRepeatType_Wednesday (string sActionTime);
   bool  SetRepeatType_Thursday  (string sActionTime);
   bool  SetRepeatType_Friday    (string sActionTime);
   
   bool  IsTime_ToRepeatAction(datetime i_dtNow=0);
   
   string   GetMsg(){ return m_sMsg; }
   
private:
   void  Reset_RepeatType();
   bool  Check_WeekDay(string sActionTime);   
   
private:
   EN_REPEAT_TP      m_enRepeatTp;
   string            m_arrActionTime[ACTIONTIME_CNT];
   string            m_sLastWorkedDate;
   string            m_sActionDateForOnce;
   string            m_sMsg;
};

CRepeatType::CRepeatType()
{
   Reset_RepeatType();
}




void CRepeatType::Reset_RepeatType()
{
   m_enRepeatTp = REPEAT_NONE ;
   for( int i=0; i<ACTIONTIME_CNT; i++ )
      m_arrActionTime[i] = "";
}

bool CRepeatType::SetRepeatType_Once(string sActionDate, string sActionTime)
{
   if( m_enRepeatTp==REPEAT_EVERDAY || m_enRepeatTp==REPEAT_WEEKDAY )
   {
      m_sMsg = "EveryDay type or WeekDay type is already set. Once type and others are mutually exclusive";
      Print(m_sMsg);
      return false;
   }

   if( !__ValidateTime_HH_MM(sActionTime) )
   {
      m_sMsg = "TimeFormat must be hh:mm";
      Print(m_sMsg);
      return false;
   }
   
   if(!__ValidateDate_YYYYdotMMdotDD(sActionDate))
   {
      m_sMsg = "Date format must be yyyy.mm.dd";
      Print(m_sMsg);
      return false;
   }
   
   
   Reset_RepeatType();
   
   m_sActionDateForOnce       = sActionDate;
   m_arrActionTime[IDX_ONCE] = sActionTime;
   m_enRepeatTp               = REPEAT_ONCE;
   
   return true;
}


bool CRepeatType::SetRepeatType_EveryDay(string sActionTime)
{   
   if( m_enRepeatTp==REPEAT_ONCE || m_enRepeatTp==REPEAT_WEEKDAY )
   {
      m_sMsg = "Once type or WeekDay type is already set. Everyday type and others are mutually exclusive";
      Print(m_sMsg);
      return false;
   }

   if( !__ValidateTime_HH_MM(sActionTime) )
   {
      m_sMsg = StringFormat("[Repeat Daily]TimeFormat must be hh:mm(%s)", sActionTime);
      Print(m_sMsg);
      return false;
   }

   Reset_RepeatType();
   
   m_arrActionTime[IDX_EVERYDAY] = sActionTime;
   m_enRepeatTp                  = REPEAT_EVERDAY;
   
   return true;
}


bool CRepeatType::Check_WeekDay(string sActionTime)
{
   if( m_enRepeatTp==REPEAT_ONCE || m_enRepeatTp==REPEAT_EVERDAY )
   {
      m_sMsg = "Once type or EveryDay type is already set. Weekday type and others are mutually exclusive";
      Print(m_sMsg);
      return false;
   }
   
   
   if( sActionTime == "hh:mm" )
      return true;
   
   if( !__ValidateTime_HH_MM(sActionTime) )
   {
      m_sMsg = StringFormat("[Repeat Weekday]TimeFormat must be hh:mm(%s)", sActionTime);
      Print(m_sMsg);
      return false;
   }


   m_enRepeatTp = REPEAT_WEEKDAY;
   return true;
}

bool CRepeatType::SetRepeatType_Monday(string sActionTime)
{
   if( !Check_WeekDay(sActionTime) )
      return false;
      
   m_arrActionTime[IDX_MON] = sActionTime;
   return true;
}

bool CRepeatType::SetRepeatType_Tuesday(string sActionTime)
{
   if( !Check_WeekDay(sActionTime) )
      return false;
      
   m_arrActionTime[IDX_TUE] = sActionTime;
   return true;
}

bool CRepeatType::SetRepeatType_Wednesday(string sActionTime)
{
   if( !Check_WeekDay(sActionTime) )
      return false;
      
   m_arrActionTime[IDX_WED] = sActionTime;
   return true;
}

bool CRepeatType::SetRepeatType_Thursday(string sActionTime)
{
   if( !Check_WeekDay(sActionTime) )
      return false;
      
   m_arrActionTime[IDX_THUR] = sActionTime;
   return true;
}

bool CRepeatType::SetRepeatType_Friday(string sActionTime)
{
   if( !Check_WeekDay(sActionTime) )
      return false;
      
   m_arrActionTime[IDX_FRI] = sActionTime;
   return true;
}
   
   
bool CRepeatType::IsTime_ToRepeatAction(datetime i_dtNow)
{
   datetime dtNow = i_dtNow;
   if( dtNow<=0 )
      dtNow = TimeCurrent();
      
   string sWeekDay = __DayOfWeek(dtNow);
   if( sWeekDay == DEF_SATURDAY || sWeekDay == DEF_SUNDAY )
      return false;

   // already done on today
   string sToday = __TimeTo_YYYYdotMMdotDD(dtNow);
   if( sToday == m_sLastWorkedDate )
      return false;

   ////////////////////////////////////////////////////////////////
   // check time
   string sNow = __TimeTo_HH_MM(dtNow);
      
      
   /////////////////////////////////////////////////////////////
   // check repeat type and today
   bool bRes = false;
   if( m_enRepeatTp == REPEAT_ONCE )                        // repeat type is ONCE
   {
      if( m_sActionDateForOnce != sToday )
      {
         bRes = false;
      }
      else
      {   
         bRes = ( sNow == m_arrActionTime[IDX_ONCE] ); 
      }
   }
   else if( m_enRepeatTp == REPEAT_EVERDAY )                // repeat type is everday
   {
      bRes = ( sNow == m_arrActionTime[IDX_EVERYDAY] ); 
   }
   else if( m_enRepeatTp == REPEAT_WEEKDAY )                // repeat type is WeekDay
   {
      if(sWeekDay==DEF_MONDAY)
      {
         bRes = ( sNow == m_arrActionTime[IDX_MON] ) ;
      } 
      else if(sWeekDay==DEF_TUESDAY)
      {
         bRes = ( sNow == m_arrActionTime[IDX_TUE] );
      } 
      else if(sWeekDay==DEF_WEDNESDAY)
      {
         bRes = ( sNow == m_arrActionTime[IDX_WED] );
      } 
      else if(sWeekDay==DEF_THURSDAY)
      {
         bRes = ( sNow == m_arrActionTime[IDX_THUR] );
      } 
      else if(sWeekDay==DEF_FRIDAY)
      {
         bRes = ( sNow == m_arrActionTime[IDX_FRI] );
      }
      else
         bRes = false;
   }
   
   if(bRes)   
      m_sLastWorkedDate = sToday;
   
   return bRes;   
}
   
   
#endif