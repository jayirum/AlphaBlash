//+------------------------------------------------------------------+
//|                                           AlphaTwoWayTcpTest.mq4 |
//|                        Copyright 2019, MetaQuotes Software Corp. |
//|                                             https://www.mql5.com |
//+------------------------------------------------------------------+
#property copyright "Copyright 2021, Retail Trader Tools"
//#property link      "https://www.mql5.com"
#property version   "1.00"
#property strict



/////////////////////////////////////////////////////////////////////////
#include "../include/AlphaMT5/DataLogger/LoggerCommon.mqh"
#include "../include/AlphaMT5/DataLogger/CLogFreq.mqh"
#include "../include/AlphaMT5/DataLogger/CMarketDataHandler.mqh"
#include "../include/AlphaMT5/DataLogger/CLogTime_Duration.mqh"
#include "../include/AlphaMT5/UtilDateTime.mqh"

/////////////////////////////////////////////////////////////////////////

//0804
const string START_TIME                = "00:05";  
const string STOP_TIME                 = "23:55";
const string TERMINATE_TIME_ON_FRIDAY  = "23:55";

input string   Symbols = "EURUSD,EURGBP,USDJPY,USDCAD,GBPUSD,USDCHF,NZDUSD,EURAUD";

input EN_USE_NOUSE      DateTime       =Enabled;  //MT4_Time
input EN_USE_NOUSE      BidPrice       =Enabled;  //Bid
input EN_USE_NOUSE      AskPrice       =Enabled;  //Ask
input EN_USE_NOUSE      HighPrice      =Enabled; //High
input EN_USE_NOUSE      LowPrice       =Enabled;  //Low
input EN_USE_NOUSE      CurrentSpread  =Enabled;  //Spread
input EN_USE_NOUSE      MinSpread      =Enabled;
input EN_USE_NOUSE      MaxSpread      =Enabled;
input EN_USE_NOUSE      MeanSpread     =Enabled;
input EN_USE_NOUSE      ModeSpread     =Enabled;
input string            StartLogging   ="00:05";   //Start Logging
input string            StopLogging    ="23:55";    //Stop Logging
input EN_MDLOGGER_FREQ  LogFrequency   =Tick;      //Log Frequency
input EN_DURATION       LogDuration    =Daily;      //Log Duration
input EN_OUTPUT_FOLDER  OutputFolder   =MQL4_Files; //Output Folder







int   _MSTIMEOUT_TICK_FETCH   = 5;
int   _MAX_FILE_COUNT         = 64;   

CLogFreq             *_logFreq = NULL;
CLogTime_Duration    *_timeNduration;
CMDHandler           _arrMD[];
CTimeElapse          _timeElapse;


int      _symbolCnt;
string   _sMsg;

bool     _bExpertEnabled = false;
bool     _bStarted = false;
//+------------------------------------------------------------------+
//| Expert initialization function                                   |
//+------------------------------------------------------------------+
int OnInit()
{   
   if( !Validate_InputData() )
   {
      return -1;
   }
   
   _logFreq = new CLogFreq(LOGGER_TP_MD);
   _logFreq.SetLogFreq(LogFrequency);

   _timeNduration = new CLogTime_Duration;
   _timeNduration.Set_WorkingTime(StartLogging, StopLogging);
   
   _timeNduration.Set_Duration(LogDuration);
   
   //
   __ShowAllSymbols();
   //

   _symbolCnt = 0;
   string  arrSymbolsName[];
   if( !RetrieveSymbols_fromInput(arrSymbolsName, _symbolCnt) )
   {
      PrintFormat("Input symbols error:%s", Symbols);
      return -1;
   }
      
   ArrayResize(_arrMD, _symbolCnt);
   for( int i=0; i<_symbolCnt; i++ )
   {
      _arrMD[i].InitData();
      _arrMD[i].symbol = arrSymbolsName[i];
      
      PrintFormat("[%d]symbol:%s",i, _arrMD[i].symbol);
         
      //if( !OpenDataFile(_arrMD[i]) )
      //   return -1;
      //PrintFormat("[%d]FileOpen(%s)", i, _arrMD[i].fileName);
   }
   PrintFormat("_arrMD size:%d", ArraySize(_arrMD));
      
   //__RunEA_Start();
   
   
   Sleep(1000);
   EventSetMillisecondTimer(_MSTIMEOUT_TICK_FETCH);

   Print("OnInit");
//---
   return(INIT_SUCCEEDED);
}


 
bool Validate_InputData()
{
   if( !Validate_InputTime(StartLogging, StopLogging) )
      return false;
      
   //0804. check the const value of time
   if( StringCompare(StartLogging, START_TIME) <0 ||
       StringCompare(StopLogging, STOP_TIME) > 0 )
   {
      Alert("Start/Stop time must be between "+START_TIME+" ~ "+STOP_TIME);
      return false;
   }
   return true;
}

bool OpenDataFile(_InOut_ CMDHandler& md)
{
   if( !GetFileName(md))
      return false;

   bool bAlreadExists = false;  

   if( OutputFolder==Terminal_Common_Files)
   {
      if( FileIsExist(md.fileName, FILE_COMMON) )
      {
         bAlreadExists = true;
         FileClose(md.fd);
      }

      md.fd = FileOpen(md.fileName
                       ,FILE_READ | FILE_WRITE | FILE_CSV | FILE_ANSI | FILE_SHARE_READ | FILE_COMMON
                       );
   }
   else
   {
      if( FileIsExist(md.fileName) )
      {
         bAlreadExists = true;
         FileClose(md.fd);
      }

      md.fd = FileOpen(md.fileName
                       ,FILE_READ | FILE_WRITE | FILE_CSV | FILE_ANSI | FILE_SHARE_READ
                       );
   }

   if( md.fd==INVALID_HANDLE)
   {
      PrintFormat("open file failed:%s, %d", md.fileName, GetLastError());
      return false;
   }
   
   if( !bAlreadExists )
   {
      WriteFieldsName(md);
   }
 
   if(!FileSeek(md.fd, 0, SEEK_END)) 
   {
      PrintFormat("FileSeek failed:%s, %d", md.fileName, GetLastError());
      return false;
   }     
   
   return true;
}

bool WriteFieldsName(_In_ CMDHandler& md)
{
   string sFields = "MT4_Time,Bid,Ask,High,Low,Spread,MinSpread,MaxSpread,MeanSpread,ModeSpread\n";  //,AccBalance,AccEquity,FreeMargin,AccPnL\n";
   int ret = (int)FileWriteString(md.fd , sFields);
   if(ret>0 )
     FileFlush(md.fd);
   else
      PrintFormat("Write Fields Error:%d",GetLastError());
   return (ret>0);
}

bool RetrieveSymbols_fromInput(_Out_ string& arrSymbolsName[], _Out_ int& nSymbolCnt)
{
   ushort deli = StringGetCharacter(",",0);
   int nCnt = StringSplit(Symbols, deli, arrSymbolsName);
   
   if( nCnt==0 )
   {
      Alert("Please input symbols to log data");
      return false;
   }
   if( nCnt>_MAX_FILE_COUNT )
   {
      Alert("Symbols can not excess 64");
      return false;
   }
   
   for( int i=0; i<nCnt; i++ )
   {
      if( arrSymbolsName[i]!="" )
      {
         if( !SymbolSelect(arrSymbolsName[0], true) )
         {
            Alert("Please input the exact symbol code of the broker");
            return false;
         }
         nSymbolCnt++;
      }
         
   }


   PrintFormat("[RetrieveSymbols_fromInput] array size:%d", nSymbolCnt);
   
   return true;

}

bool GetFileName(_InOut_ CMDHandler& md)
{
   string sFreq;
   
   if(LogFrequency==EN_MDLOGGER_FREQ::Tick)     sFreq="tick";
   else if(LogFrequency==EN_MDLOGGER_FREQ::M1)  sFreq="m1";
   else if(LogFrequency==EN_MDLOGGER_FREQ::M5)  sFreq="m5";
   else if(LogFrequency==EN_MDLOGGER_FREQ::M15) sFreq="m15";
   else if(LogFrequency==EN_MDLOGGER_FREQ::M30) sFreq="m30";
   else if(LogFrequency==EN_MDLOGGER_FREQ::H1)  sFreq="h1";
   else if(LogFrequency==EN_MDLOGGER_FREQ::H4)  sFreq="h4";
   else return false;
   
   datetime now = TimeCurrent();
   string sNow = __TimeToStr(now);  // 2021.05.18 18:16:54.633
   string sDate = StringSubstr(sNow, 0, 4)+"-"+StringSubstr(sNow, 5,2)+"-"+StringSubstr(sNow, 8, 2);
   md.fileName = FILE_SUB_DIR+"\\"+sDate+"_"+StringSubstr(__GetBrokerName(),0,5)+"_"+md.symbol+"_"+sFreq+".csv";
   return true;
}

  
//+------------------------------------------------------------------+
//| Expert deinitialization function                                 |
//+------------------------------------------------------------------+
void OnDeinit(const int reason)
{
   EventKillTimer();
   
   delete _logFreq;        _logFreq = NULL;
   delete _timeNduration;  _timeNduration = NULL;
}

//+------------------------------------------------------------------+
//| Expert tick function                                             |
//+------------------------------------------------------------------+
void OnTick()
  {
//---
   
  }
  


//+------------------------------------------------------------------+
//| Timer function                                                   |
//+------------------------------------------------------------------+
void OnTimer()
{
   //_timeElapse.SetStartPoint();

   //if(!__RunEA_IsRunnable()) return;

   if(IsStopped())
   {
      OnDeinit(0);
      return;      
   }
   
   if( _bExpertEnabled != __IsExpertEnabled())
   {
      _bExpertEnabled = __IsExpertEnabled();
      if( _bExpertEnabled==true )
      {
         showMsg("EA is running");
      }   
      else
      {
         showMsg("EA is stopped");
      }
   }
   
   if(!_bExpertEnabled){
      return;
   }

   //0804
   datetime dtNow = TimeCurrent();
   
   //0804
   if(IsTerminateTime(dtNow, TERMINATE_TIME_ON_FRIDAY))
   {  
      EventKillTimer();
      Alert("Terminate EA because of market closing");
      ExpertRemove();
      return;
   }
   
   if( _timeNduration.CheckTime_Start(dtNow) )
   {
      for( int i=0; i<_symbolCnt; i++ )
      {
         FileClose(_arrMD[i].fd);
         if(OpenDataFile(_arrMD[i]))
            PrintFormat("[%d] Open file(%s)", i, _arrMD[i].fileName);
      }
   }

   if( _timeNduration.CheckTime_Stop(dtNow) )
   {
      for( int i=0; i<_symbolCnt; i++)
         _arrMD[i].ResetData();
   }         

   LogData_by_LogFreq();

   //PrintFormat("Time eslpased millisec(%s)", _timeElapse.GetElapsedMilliseconds());
}



void LogData_by_LogFreq()
{
   if( !_timeNduration.Is_RunningNow() )
      return;
      
   if( LogFrequency==EN_MDLOGGER_FREQ::Tick)
   {
      for( int i=0; i<_symbolCnt; i++)
      {
         datetime dtCurrTime = (datetime)SymbolInfoInteger(_arrMD[i].symbol, SYMBOL_TIME);
         if( _arrMD[i].time !=  dtCurrTime)
         {
            UpdateLastTick(i, dtCurrTime);

            WriteData(_arrMD[i]);
         }
      }
   }
   else
   {
      datetime dtCurrTime = TimeCurrent();
      string sCurrTime = __HHMMSSToStr(dtCurrTime);   //hh:mm:ss
      
      if( !_logFreq.Is_TimeToSave(sCurrTime) )
         return;
      
      for( int i=0; i<_symbolCnt; i++ )
      {
         UpdateLastTick(i, dtCurrTime);
            
         WriteData(_arrMD[i]);
      }
   }

}

//"DateTime,Bid,Ask,Hig,Low,CurrSpread,MinSpread,MaxSpread,MeanSpread,ModeSpread";
void UpdateLastTick(int iSymbol, datetime dtCurrTime)
{
   _arrMD[iSymbol].UpdateLastMD(
                              SymbolInfoDouble(_arrMD[iSymbol].symbol, SYMBOL_BID)     // double i_bid
                              , SymbolInfoDouble(_arrMD[iSymbol].symbol, SYMBOL_ASK)   // double i_ask
                              , SymbolInfoDouble(_arrMD[iSymbol].symbol, SYMBOL_LASTHIGH)          // double i_high
                              , SymbolInfoDouble(_arrMD[iSymbol].symbol, SYMBOL_LASTLOW)           // double i_low
                              , SymbolInfoInteger(_arrMD[iSymbol].symbol, SYMBOL_SPREAD)        // double i_spread
                              , dtCurrTime
                              );
}

//"MT_TIME,Bid,Ask,Hig,Low,CurrSpread,MinSpread,MaxSpread,MeanSpread,ModeSpread";
void WriteData(CMDHandler& md)
{
   string buf;
         
   if(DateTime==1)
   {
      if( LogFrequency == EN_MDLOGGER_FREQ::Tick )
      {
         buf = __HHMMSSToStr(md.time);
      }
      else if ( LogFrequency > EN_MDLOGGER_FREQ::Tick && LogFrequency < EN_MDLOGGER_FREQ::H1 )
      {
         buf = __HHMMToStr(md.time)+":00";
      }
      else if ( LogFrequency >= EN_MDLOGGER_FREQ::H1)
      {
         buf = StringSubstr(__HHMMToStr(md.time), 0, 2) +":00:00";
      }
   }
   
   buf += ",";
   
   if(BidPrice==1)         buf += StringFormat("%f",md.bid);
   buf += ",";
   
   if(AskPrice==1)         buf += StringFormat("%f",md.ask);
   buf += ",";
   
   if(HighPrice==1)        buf += StringFormat("%f",md.high);
   buf += ",";
   
   if(LowPrice==1)         buf += StringFormat("%f",md.low);
   buf += ",";
   
   if(CurrentSpread==1)      buf += StringFormat("%f",md.spread);
   buf += ",";
   
   if(MinSpread==1)  buf += StringFormat("%f",md.minSpread);
   buf += ",";

   if(MaxSpread==1)  buf += StringFormat("%f",md.maxSpread);
   buf += ",";

   if(MeanSpread==1)  buf += StringFormat("%f",md.meanSpread);
   buf += ",";
   
   if(ModeSpread==1)  buf += StringFormat("%f",md.modeSpread);
   buf += "\n";

   
   int ret = (int)FileWriteString(md.fd , buf);
   if(ret>0 ){
     FileFlush(md.fd);
     //PrintFormat("(%s)(%s)(%f)(%f)", md.symbol, __HHMMSSToStr(md.time), md.bid, md.ask);
   }
   //else
   //   PrintFormat("Write Data Error:%d",GetLastError());
         
}