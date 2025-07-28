//+------------------------------------------------------------------+
//|                                                ConfigSymbols.mqh |
//|                        Copyright 2019, MetaQuotes Software Corp. |
//|                                             https://www.mql5.com |
//+------------------------------------------------------------------+
#property copyright "Copyright 2019, MetaQuotes Software Corp."
#property link      "https://www.mql5.com"
#property strict

#include "../Protocol.mqh"
#include "../AlphaErrorCode.mqh"
#include "MasterOMS.mqh"

/*
CProtoSet	set;
			set.Begin();
			//if(nQDataTp== QDATA_TP_CONFIG)
			set.SetVal(FDS_CODE, zCode);
			set.SetVal(FDS_COMMAND, __ALPHA::TP_COMMAND);

			__ALPHA::Now(zTime);
			set.SetVal(FDS_TM_HEADER, zTime);
			set.SetVal(FDS_USERID_MINE, zUserID);
			set.SetVal(FDS_ACCNO_MINE, zMT4Acc);
			set.SetVal(FDS_MASTERCOPIER_TP, zMCTp);
			set.SetVal(FDS_CONFIG_DATA, zMCTp);
			
EURUSD___USDJPY___GBPUSD___USDCAD

*/


class CConfigSymbol : public CProtoGet
{
public:
    CConfigSymbol(){};
    ~CConfigSymbol(){};
    
    int     AddSymbolsByTr(string sSymbols, _Out_ string& pSymbols);
    int     AddSymbols(char& pzPacket[], int nRecvLen, _Out_ string& pSymbols);
    bool    IsTradableSymbol(string sSymbol);
    
protected:
    bool    ParsingPacket(char& RecvPack[], int nRecvLen);
    
private:
    int     m_nRet;
    string  m_sData;
};


int CConfigSymbol::AddSymbolsByTr(string sSymbols, _Out_ string& pSymbols)
{
   string arrSymbol[];
   int nCnt = CProtoUtils::SplitInnerArray(sSymbols, arrSymbol);
   AlphaOMS_TradableSymbols_Reset();   
   
   pSymbols = StringFormat("[%d]", nCnt);
   for( int i=0; i<nCnt; i++ )
   {
      char zSymbol[32]; 
      StringToCharArray(arrSymbol[i], zSymbol);
      AlphaOMS_TradableSymbols_Set(zSymbol);
      if(i==0)
         pSymbols += arrSymbol[i];
      else
         pSymbols += ","+arrSymbol[i];
   }
   //Alert(StringFormat("[AddSymbolsByTr]end", nCnt));
   return E_OK;
}

int CConfigSymbol::AddSymbols(char& pzPacket[], int nRecvLen, _Out_ string& pSymbols)
{
   //parsing to m_sData
   if( ParsingPacket(pzPacket, nRecvLen)==false ){    
      return m_nRet;
   }
        
    //string symbols[];
    //int nCnt = StringSplit(m_sData, DEF_DELI_ARRAY, symbols);
    //for( int i=0; i<nCnt; i++ )
    //{
    //    char zSymbol[32]; StringToCharArray(symbols[i], zSymbol);
    //    AlphaOMS_TradableSymbols_Set(zSymbol);
    //}
    
   AddSymbolsByTr(m_sData, pSymbols);
   return E_OK;
}

bool CConfigSymbol::IsTradableSymbol(string sSymbol)
{
    char zSymbol[32]; StringToCharArray(sSymbol, zSymbol);
    int ret = AlphaOMS_IsTradableSymbol(zSymbol);
    
    return (ret==E_OK);
}


bool CConfigSymbol::ParsingPacket(char& RecvPack[], int nRecvLen)
{
    string sMCTp;
    m_sData = "";
    /*
        1=1004/2=C/ array
    */
    string result[];
    
    int nCnt = CProtoGet::SplitPacket(RecvPack, nRecvLen, result);
    
    int nField;
    string unit[];
    ushort equal = StringGetCharacter("=",0);
    for(int i=0; i<nCnt; i++)
    {
        int kCnt = StringSplit(result[i], equal, unit);
        
        // this value is zero(0) when the loop count is the last
        //if( (i!=nCnt-1) && (kCnt!=2))
        if( kCnt!=2 )
        {
            if( i == (nCnt-1) )
            {    
                m_sMsg = "";    //StringFormat("[i:%d, k:%d]this is not error", i, kCnt);
            }
            else{
                m_sMsg = StringFormat("[i:%d, k:%d]Packet Unit error(%s)", i, kCnt, result[i]);
            }
            continue;
        }
        nField =  (int)StringToInteger(unit[0]);
        
        //printlog(StringFormat("[%d][Code:%d]nField",i, nField));
        
        switch(nField)
        {
        case FDS_MASTERCOPIER_TP:
            sMCTp = unit[1];
            if(sMCTp!="M")
            {
                m_nRet = E_INVALIDE_MASTERCOPIER;
                return false;
            }
            break;
        case FDS_CONFIG_DATA:
        case FDS_ARRAY_SYMBOL:
            m_sData = unit[1];
            break;
            
        }//switch(nField)
    }//for(int i=0; i<nCnt; i++)

    return true;
}