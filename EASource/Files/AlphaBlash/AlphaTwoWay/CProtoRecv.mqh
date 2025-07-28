#ifndef __CPROTO_RECV_H__
#define __CPROTO_RECV_H__


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



class CProtoLogin : public CProtoGet
{
public:
    CProtoLogin(){};
    ~CProtoLogin(){};
    
    bool    GetLoginInfo(_In_ char& RecvPack[], _In_ int nRecvLen, 
                         _Out_ int &retVal, _Out_ string& sSymbol, _Out_ int& iSymbol, _Out_ int& iSide); 
protected:
    bool    ParsingPacket(char& RecvPack[], int nRecvLen);
    
private:
   int     m_nRet;
   string   m_sData;
   int     m_retVal;
   string  m_sSymbol;
   int     m_iSymbol;
   int     m_iSide;
};


bool CProtoLogin::GetLoginInfo(_In_ char& RecvPack[], _In_ int nRecvLen, 
                         _Out_ int &retVal, _Out_ string& sSymbol, _Out_ int& iSymbol, _Out_ int& iSide)
{
   if( !ParsingPacket(RecvPack, nRecvLen) )
      return false;

      
   retVal   = m_retVal;
   sSymbol  = m_sSymbol;
   iSymbol  = m_iSymbol;
   iSide    = m_iSide;
   return true;
}

bool CProtoLogin::ParsingPacket(char& RecvPack[], int nRecvLen)
{
    string temp;
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
        switch(nField)
        {
        case FDN_ERR_CODE:
            m_retVal = StringToInteger(unit[1]);
            break;
        case FDN_SYMBOL_IDX:
            m_iSymbol = StringToInteger(unit[1]);
            break;
        case FDS_SYMBOL:
            m_sSymbol = unit[1];
            break;
        case FDN_SIDE_IDX:
            m_iSide = StringToInteger(unit[1]);
            break;
            
        }//switch(nField)
    }//for(int i=0; i<nCnt; i++)

    return true;
}


//////////////////////////////////////////////////////////////////////////////
class CProtoOrd : public CProtoGet
{
public:
    CProtoOrd(){};
    ~CProtoOrd(){};
    
    bool    GetOrdInfo(_In_ char& RecvPack[],   _In_ int       nRecvLen, 
                         _Out_ int &retVal,     _Out_ string&  sClrTp, 
                         _Out_ string& sSymbol, _Out_ int&     nTicket,
                         _Out_ int& iSide,      _Out_ double&  dLots
                         ); 
protected:
    bool    ParsingPacket(char& RecvPack[], int nRecvLen);
    
private:
   int     m_nRet;
   string   m_sData;
   
   int      m_retVal;
   int      m_nTicket;
   int      m_iSide;
   string   m_sSymbol;
   string   m_sClrTp;
   double   m_dLots;
};


bool CProtoOrd::GetOrdInfo(_In_ char& RecvPack[],   _In_ int       nRecvLen, 
                         _Out_ int &retVal,        _Out_ string&  sClrTp, 
                         _Out_ string& sSymbol,    _Out_ int&     nTicket,
                         _Out_ int& iSide,         _Out_ double&  dLots
                         )
{
   if( !ParsingPacket(RecvPack, nRecvLen) )
      return false;

      
   retVal   = m_retVal;
   sSymbol  = m_sSymbol;
   sClrTp   = m_sClrTp;
   nTicket  = m_nTicket;
   iSide    = m_iSide;
   dLots    = m_dLots;
   return true;
}

bool CProtoOrd::ParsingPacket(char& RecvPack[], int nRecvLen)
{
    string temp;
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
        switch(nField)
        {
        case FDN_ERR_CODE:
            m_retVal = StringToInteger(unit[1]);
            break;
        case FDS_MT4_TICKET:
            m_nTicket = StringToInteger(unit[1]);
            break;
        case FDS_SYMBOL:
            m_sSymbol = unit[1];
            break;
        case FDS_CLR_TP:
            m_sClrTp = unit[1];
            break;
        case FDN_SIDE_IDX:
            m_iSide = StringToInteger(unit[1]);
            break;
        case FDD_LOTS:
            m_dLots = StringToDouble(unit[1]);
            break;
            
        }//switch(nField)
    }//for(int i=0; i<nCnt; i++)

    return true;
}





#endif