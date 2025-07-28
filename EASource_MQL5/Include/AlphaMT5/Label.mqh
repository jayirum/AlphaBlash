#ifndef __ALPHA_LABEL_H__
#define __ALPHA_LABEL_H__



//+------------------------------------------------------------------+
//|                                                      BPLabel.mqh |
//|                        Copyright 2019, MetaQuotes Software Corp. |
//|                                             https://www.mql5.com |
//+------------------------------------------------------------------+
#property copyright "Copyright 2019, MetaQuotes Software Corp."
#property link      "https://www.mql5.com"
#property strict

/*
    3 Labels
    - Logo
    - Message
    - Trade info
*/

enum {idxLogo=0, idxMsg, idxWarn, idxErr, idxOpenClose, idxTicket, idxSymbol, idxBsTp, idxPrc, idxVol};
int  COLOR[] = {clrAqua, clrGreen, clrYellow, clrRed, clrMistyRose, clrMistyRose, clrMistyRose, clrMistyRose, clrMistyRose, clrMistyRose};
string NAMES[] = {"LOGO", "MSG","WARNING","ERROR", "OPENCLOSE", "TICKET", "SYMBOL", "BS_TP", "PRICE", "VOLUME"};

#define LEFT_INDENT     10
#define LOGO_X          230
#define TOP_INDENT      20
#define CATEGORY_GAP    30
#define LABEL_GAP       10
#define FONT_SIZE       13

class CAlphaLabel
{
public:
    CAlphaLabel(){ m_bDeleted = false;};
    ~CAlphaLabel(){Delete();};
    
    bool    Create(int nStartPosY=150);
    void    Delete();
    void    DeleteMsg();
    void    DeleteWarn();
    void    DeleteErr();
    
    void    Logo(string s);
    void    Msg(string s);
    void    Warning(string s);
    void    Error(string s);
    void    Trade(string sOpenClode, int ticket, string symbol, int cmd, double price, double volume);
    void    Tick(string symbol, double bid, double ask, string time );
private:
    bool    m_bDeleted;
};


void CAlphaLabel::DeleteMsg()
{
   if( ObjectName(idxMsg)==NAMES[idxMsg] )
   {
      ObjectDelete(0, NAMES[idxWarn]);
      ChartRedraw();
   }
}

void CAlphaLabel::DeleteWarn()
{
   if( ObjectName(idxWarn)==NAMES[idxWarn] )
   {
      ObjectDelete(0, NAMES[idxWarn);
      ChartRedraw();
   }
}

void CAlphaLabel::Delete()
{
    if(m_bDeleted)
        return;
    for( int i=0; i<ArraySize(NAMES); i++ )
    {
        if( ObjectName(i)!="" )
           ObjectDelete(0, NAMES[i]);
    }
    ChartRedraw();
    m_bDeleted = true;
}

bool CAlphaLabel::Create(int nStartPosY)
{
    int y;
    
    for( int i=0; i<ArraySize(NAMES); i++)
    {
        if(!ObjectCreate(0, NAMES[i], OBJ_LABEL,0,0,0))
        {
            return false;
        }
        
        if(i==idxLogo){
            ObjectSetInteger(0,NAMES[i],OBJPROP_XDISTANCE, LOGO_X);
            ObjectSetInteger(0,NAMES[i],OBJPROP_YDISTANCE, 1);
            y = nStartPosY;
         }            
         else{
            ObjectSetInteger(0,NAMES[i],OBJPROP_XDISTANCE, LEFT_INDENT);
            ObjectSetInteger(0,NAMES[i],OBJPROP_YDISTANCE, y);
        }
        //ObjectSetInteger(0,NAMES[0],OBJPROP_CORNER,CORNER_LEFT_UPPER);
        
        //--- set text color
        ObjectSetInteger(0,NAMES[i],OBJPROP_COLOR, COLOR[i]);
    
        //--- set textC
        ObjectSetString(0,NAMES[i],OBJPROP_TEXT, "");
    
        ObjectSetInteger(0,NAMES[i],OBJPROP_FONTSIZE, FONT_SIZE);

        if ( i==idxLogo )
            y = nStartPosY;
        //else if ( i==1 )
        //    y = ObjectGetInteger(0, NAMES[i], OBJPROP_YDISTANCE) + (CATEGORY_GAP*1);
        else
            y = ObjectGetInteger(0, NAMES[i], OBJPROP_YDISTANCE) + (LABEL_GAP*1);
    }
    
    
    ObjectCreate(0, "BOTTOM", OBJ_EDIT, 0, 0, 0);
    ObjectSetInteger(0, "BOTTOM",OBJPROP_CORNER, CORNER_LEFT_LOWER);
    ObjectSetInteger(0, "BOTTOM",OBJPROP_XDISTANCE, 0);
    ObjectSetInteger(0, "BOTTOM",OBJPROP_YDISTANCE, 50);
    ObjectSetInteger(0, "BOTTOM",OBJPROP_XSIZE,800);
   ObjectSetInteger(0, "BOTTOM",OBJPROP_YSIZE,50);
    return true;
}


void CAlphaLabel::Logo(string s)
{
    ObjectSetString(0, NAMES[idxLogo], OBJPROP_TEXT, s);
    //ObjectSetInteger(0,NAMES[idxLogo],OBJPROP_FONTSIZE, FONT_SIZE);
    ChartRedraw();
}
 
 void CAlphaLabel::Msg(string s)
 {
    ObjectSetInteger(0,NAMES[idxMsg],OBJPROP_COLOR, COLOR[idxMsg]);
    ObjectSetString(0, NAMES[idxMsg], OBJPROP_TEXT, s);
    //ObjectSetInteger(0,NAMES[idxMsg],OBJPROP_FONTSIZE, FONT_SIZE);
    ChartRedraw();
 }
 
 void CAlphaLabel::Warning(string s)
 {
    ObjectSetInteger(0,NAMES[idxWarn],OBJPROP_COLOR, COLOR[idxWarn]);
    ObjectSetString(0, NAMES[idxWarn], OBJPROP_TEXT, s);
    ChartRedraw();
 }
 
 void CAlphaLabel::Error(string s)
 {
    ObjectSetInteger(0,NAMES[idxErr],OBJPROP_COLOR, COLOR[idxErr]);
    ObjectSetString(0, NAMES[idxErr], OBJPROP_TEXT, s);
    ChartRedraw();
 }
 
 
 string bstp(int cmd)
 {
    string s;
    switch(cmd)
    {
        case OP_BUY: s = "BUY"; break;
        case OP_SELL: s = "SELL"; break;
        case OP_BUYLIMIT: s = "BUY LIMIT"; break;
        case OP_SELLLIMIT: s = "SELL LIMIT"; break;
        case OP_BUYSTOP: s = "BUY STOP"; break;
        case OP_SELLSTOP: s = "SELL STOP"; break;
    }
    return s;
 }
 
 void CAlphaLabel::Trade(string sOpenClode, int ticket,string symbol, int cmd, double price, double volume)
 {
    ObjectSetString(0, NAMES[idxOpenClose], OBJPROP_TEXT, StringFormat("%6.6s : %s", "Actoin", sOpenClode));
    ObjectSetInteger(0,NAMES[idxOpenClose],OBJPROP_FONTSIZE, FONT_SIZE);
    
    ObjectSetString(0, NAMES[idxTicket], OBJPROP_TEXT, StringFormat("%6.6s : %d","Ticket", ticket));
    ObjectSetInteger(0,NAMES[idxTicket],OBJPROP_FONTSIZE, FONT_SIZE);

    ObjectSetString(0, NAMES[idxSymbol], OBJPROP_TEXT, StringFormat("%6.6s : %s", "Symbol", symbol));
    ObjectSetInteger(0,NAMES[idxSymbol],OBJPROP_FONTSIZE, FONT_SIZE);

    ObjectSetString(0, NAMES[idxBsTp], OBJPROP_TEXT, StringFormat("%6.6s : %s", "Type", bstp(cmd)));    //"Type : "+bstp(cmd));
    ObjectSetInteger(0,NAMES[idxBsTp],OBJPROP_FONTSIZE, FONT_SIZE);
    
    ObjectSetString(0, NAMES[idxPrc], OBJPROP_TEXT, StringFormat("%6.6s : %f", "Price", price));    //StringFormat("Pirce : %f", price));
    ObjectSetInteger(0,NAMES[idxPrc],OBJPROP_FONTSIZE, FONT_SIZE);
    
    ObjectSetString(0, NAMES[idxVol], OBJPROP_TEXT, StringFormat("%6.6s : %f", "Volume", volume));   //StringFormat("Volume : %f", volume));
    ObjectSetInteger(0,NAMES[idxVol],OBJPROP_FONTSIZE, FONT_SIZE);
    
    ChartRedraw();
      
 }
 
 void CAlphaLabel::Tick(string symbol, double bid, double ask, string time )
 {
    ObjectSetString(0, NAMES[idxSymbol], OBJPROP_TEXT, StringFormat("%6.6s:%s", "SYMBOL",symbol));
    //ObjectSetInteger(0,NAMES[idxSymbol],OBJPROP_FONTSIZE, FONT_SIZE);
    
    ObjectSetString(0, NAMES[idxBsTp], OBJPROP_TEXT,StringFormat("%6.6s:%f","BID", bid));
    //ObjectSetInteger(0,NAMES[idxBsTp],OBJPROP_FONTSIZE, FONT_SIZE);
    
    ObjectSetString(0, NAMES[idxPrc], OBJPROP_TEXT, StringFormat("%6.6s:%f", "ASK", ask));
    //ObjectSetInteger(0,NAMES[idxPrc],OBJPROP_FONTSIZE, FONT_SIZE);
    
    ObjectSetString(0, NAMES[idxVol], OBJPROP_TEXT, "TIME  :"+time);
    //ObjectSetInteger(0,NAMES[idxVol],OBJPROP_FONTSIZE, FONT_SIZE);
 }
 
 
 
 
 
 
 #endif