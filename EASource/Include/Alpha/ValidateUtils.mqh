#ifndef __VALIDATE_INPUTS_H__
#define __VALIDATE_INPUTS_H__



// yyyy.mm.dd
bool validate_date(string sDate)
{
   bool ret = true;
   
   if( StringLen(sDate)!=10 ) ret = false;
   if( StringSubstr(sDate,4,1) != "." ) ret = false;
   if( StringSubstr(sDate,7,1) != "." ) ret = false;
   if( StringToInteger(StringSubstr(sDate,0,4))==0 )   ret = false;
   
   int mon = (int)StringToInteger(StringSubstr(sDate,5,2));
   if( mon < 1 || mon > 12 )   ret = false;
   
   int day = (int)StringToInteger(StringSubstr(sDate,8,2));
   if( day < 1 ) ret = false;
   if( mon==1 || mon==3 || mon==5 || mon==7 || mon==8 || mon==10 || mon==12 )
   {
      if( day > 31 ) ret = false;
   }
   if( mon==4 || mon==6 || mon==9 || mon==11 )
   {
      if( day > 30 ) ret = false;
   }
   if( mon==2 )
   {
      if( day > 29 ) ret = false;
   }
   
   return ret;
}

// hh:mm
bool validate_time(string sTime)
{
   bool ret = true;
   
   if( StringLen(sTime)!=5 ) ret = false;
   if( StringSubstr(sTime,2,1) != ":" ) ret = false;

   int val = (int)StringToInteger(StringSubstr(sTime,0,2));
   if( val < 1 || val > 24 ) ret = false;
   
   val = (int)StringToInteger(StringSubstr(sTime,3,2));
   if( val < 1 || val > 59 ) ret = false;
   
   return ret;
}

#endif