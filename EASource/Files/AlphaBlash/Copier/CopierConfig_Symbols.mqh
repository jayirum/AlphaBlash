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
			
EURUSD=EURUSD/USDJPY=USDJPY/GBPUSD=GBPUSD/USDCAD=USDCAD

*/

#define MAX_SYMBOLS  20

class CSymbolMapping : public CProtoGet
{
public:
   CSymbolMapping(){m_nSymbolCnt=0;};
   ~CSymbolMapping(){};
   
   int     AddSymbolsByTr(string sSymbols, _Out_ string& pSymbols);
   int     AddSymbols(char& pzPacket[], int nRecvLen, _Out_ string& pSymbols);
   bool    GetMySymbol(string sMasterSymbol, _Out_ string& sMySymbol);
   
protected:
   bool    ParsingPacket(char& RecvPack[], int nRecvLen);
private:
   void  InitSymbols();
private:
   string   m_sMaster[MAX_SYMBOLS];
   string   m_sCopier[MAX_SYMBOLS];
   int      m_nSymbolCnt;
   int      m_nRet;
   string   m_sData;
};

void CSymbolMapping::InitSymbols()
{
   for( int i=0; i<MAX_SYMBOLS; i++)
   {
      m_sMaster[i] = "";
      m_sCopier[i] = "";
   }
   m_nSymbolCnt = 0;
}

int CSymbolMapping::AddSymbolsByTr(string sSymbols, _Out_ string& pSymbols)
{
   InitSymbols();
   
   string arrSymbol[];
   int nCnt = CProtoUtils::SplitInnerArray(/*in*/sSymbols, /*out*/arrSymbol);

   string sOneSymbolPair;
   pSymbols = StringFormat("[%d]", nCnt);
   for( int i=0; i<nCnt; i++ )
   {
      sOneSymbolPair = "";
      if( StringLen(arrSymbol[i])==0 )
         continue;
         
      sOneSymbolPair = arrSymbol[i];
      int nEqualPos = StringFind(sOneSymbolPair, ":", 0);
      
      m_sMaster[m_nSymbolCnt] = StringSubstr(sOneSymbolPair, 0, nEqualPos);
      m_sCopier[m_nSymbolCnt] = StringSubstr(sOneSymbolPair, nEqualPos+1, StringLen(sOneSymbolPair)-nEqualPos-1);
      
      if(m_nSymbolCnt==0)
         pSymbols += m_sCopier[m_nSymbolCnt];
      else
         pSymbols += ","+m_sCopier[m_nSymbolCnt];
         
      m_nSymbolCnt++;      
   }
   return E_OK;
}

int CSymbolMapping::AddSymbols(char& pzPacket[], int nRecvLen, _Out_ string& pSymbols)
{
   // parsing to m_sData
   if( ParsingPacket(pzPacket, nRecvLen)==false )
      return m_nRet;
   
   AddSymbolsByTr(m_sData, pSymbols);  

   return E_OK;
}

bool CSymbolMapping::GetMySymbol(string sMasterSymbol, string& sMySymbol)
{
   bool bFind = false;
   for(int i=0; i<m_nSymbolCnt; i++)
   {
      if( m_sMaster[i]==sMasterSymbol )
      {
         bFind = true;
         sMySymbol = m_sCopier[i];
      }
   }
    
   return bFind;
}


bool CSymbolMapping::ParsingPacket(char& RecvPack[], int nRecvLen)
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
            if(sMCTp!="C")
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