#ifndef __LINE_TEXT_H__
#define __LINE_TEXT_H__


//#define X_START   30
//#define Y_START   20
#define DISTANCE_LINES      30
//#define DISTANCE_MULTILINES 20

#define MAX_LINES 30


enum EN_CORNER_TP
{
   CORNER_LEFT,
   CORNER_RIGHT   
};
  
struct TText
{
   string name;
   string msg;
   
   int   startIndex;
   int   endIndex;
   color clr;
   int   font_size;
   int   hidden;
   
   TText()
   {
      hidden = 0;
   }
};

class CLineText
{
public:
   CLineText(EN_CORNER_TP cornerTp=CORNER_RIGHT);
   ~CLineText();
   
   // handling by name => inefficient
   bool AddLine(string msg,color clr = clrYellow,int font_size = 11);
   bool Hide(int idx);
   bool Show(int idx);
   bool UpdateText(int i,string msg,color clr=0,int font_size=0);
   bool UpdateColor(int idx,color clr);
   bool UpdateFontSize(int idx,int size);
   //bool Delete(int idx);
   bool DeleteAll();
   
private:
   bool     AddLine(string name,string msg,color clr,int font_size);
   int      GetLastIndex();
   string   ComposeName(int idx);
   bool     OutOfRange(int idx);
private:
   int               m_seed;
   int               m_nowCount;
   TText             m_Texts[MAX_LINES];
   ENUM_BASE_CORNER  m_cornerTp;
   int               m_StartX;
   int               m_StartY;
};

CLineText::CLineText(EN_CORNER_TP cornerTp=CORNER_RIGHT)
{
   m_seed = MathRand();
   m_nowCount  = 0;
   m_cornerTp  = (cornerTp==CORNER_RIGHT)? CORNER_RIGHT_UPPER : CORNER_LEFT_UPPER;
   m_StartX    = (cornerTp==CORNER_RIGHT)? 350 : 30;
   m_StartY    = (cornerTp==CORNER_RIGHT)? 0 : 20;
};

CLineText::~CLineText()
{
   DeleteAll();
}

bool CLineText::OutOfRange(int idx)
{
   return (idx >= m_nowCount);
}

string CLineText::ComposeName(int idx)
{
   return StringFormat("Label_%d_%d",m_seed, idx);
}


bool CLineText::DeleteAll()
{
   for(int i=0;i<m_nowCount;i++)
   {
      ObjectDelete(0,m_Texts[i].name);
   }
   m_nowCount=0;
   return true;
}

bool CLineText::AddLine(string msg,color clr,int font_size)
{
   string name = ComposeName(m_nowCount);
   return AddLine(name, msg, clr,font_size);
}


bool CLineText::AddLine(string name,string msg,color clr,int font_size)
{
   if(ObjectFind(0,name)>=0)
   {
      //Print("Object Already Exists name = "+name);
      ObjectDelete(0, name);
   }
   bool added = false;
   int x=m_StartX;
   int y=GetLastIndex();
   m_Texts[m_nowCount].name = name;
   m_Texts[m_nowCount].msg = msg;
   m_Texts[m_nowCount].clr = clr;
   m_Texts[m_nowCount].font_size = font_size;
   m_Texts[m_nowCount].startIndex = y;
   
   y += DISTANCE_LINES;
   LabelCreate(0,name,0,x,y,m_cornerTp,msg,"Arial",font_size,clr);
   m_Texts[m_nowCount].endIndex = y;
   
   m_nowCount++;
   
   return true;
}


bool CLineText::Hide(int idx)
{
   if(OutOfRange(idx))  return false;
   
   string name = ComposeName(idx);
   m_Texts[idx].hidden = 1;
   //ObjectDelete(0,name);
   ObjectSet(name,OBJPROP_XDISTANCE,100000);
   return true;
}


bool CLineText::Show(int idx)
{
   if(OutOfRange(idx))  return false;
   
   string name = ComposeName(idx);
   
   m_Texts[idx].hidden = 0;
   ObjectSet(name,OBJPROP_XDISTANCE,m_StartX);
   
//   int x = m_StartX;
//   int y = m_Texts[idx].startIndex;
//   
//   string msg     = m_Texts[idx].msg;
//   int font_size  = m_Texts[idx].font_size;
//   color clr      = m_Texts[idx].clr;
//
//   y += DISTANCE_LINES;
//   LabelCreate(0,name,0,x,y,m_cornerTp,msg,"Arial",font_size,clr);
   
   return true;
}


bool CLineText::UpdateText(int i,string msg,color clr,int font_size)
{
   if(OutOfRange(i))  return false;
   
   string name = ComposeName(i);
   
   int distance = 0;
   int below=0;
   
   if(clr!=0)        m_Texts[i].clr = clr;
   if(font_size!=0)  m_Texts[i].font_size = font_size;
   m_Texts[i].msg = msg;

   int x = m_StartX;
   int y = m_Texts[i].startIndex;
   int newEndY = y;
   below = m_Texts[i].endIndex;

   //{  
   //   y+=added?DISTANCE_MULTILINES:DISTANCE_LINES;
   //   LabelCreate(0,name+IntegerToString(m_Texts[i].cnt++),0,x,y,m_cornerTp,StringSubstr(newmsg,0,m_Texts[i].maxLength),"Arial",newfont_size,newclr);
   //   newmsg = StringSubstr(newmsg,m_Texts[i].maxLength+1);
   //   newEndY = y;
   //   added= true;
   //}while(StringLen(newmsg)>0);
   //distance = newEndY-m_Texts[i].endIndex;
   
   ObjectSetString(0, name, OBJPROP_TEXT,       m_Texts[i].msg);
   ObjectSetInteger(0,name, OBJPROP_COLOR,      m_Texts[i].clr);
   ObjectSetInteger(0,name, OBJPROP_FONTSIZE,   m_Texts[i].font_size);
   
   return true;
}

bool CLineText::UpdateColor(int idx,color clr)
{
   if(OutOfRange(idx))  return false;
   
   string name = ComposeName(idx);
   
   ObjectSetInteger(0, name, OBJPROP_COLOR, clr);
   
   return true;
}
bool CLineText::UpdateFontSize(int idx,int size)
{
   if(OutOfRange(idx))  return false;
   
   string name = ComposeName(idx);
   
   ObjectSetInteger(0, name, OBJPROP_FONTSIZE, size);
   return true;
}


int CLineText::GetLastIndex()
{
   int n=m_StartY;
   for(int i=0;i<m_nowCount;i++)
   {
      n=MathMax(n,m_Texts[i].endIndex);
   }
   return n;
}



bool LabelCreate(const long              chart_ID=0,               // chart's ID
                 const string            name="Label",             // label name
                 const int               sub_window=0,             // subwindow index
                 const int               x=0,                      // X coordinate
                 const int               y=0,                      // Y coordinate
                 const ENUM_BASE_CORNER  corner=CORNER_RIGHT_UPPER, // chart corner for anchoring
                 const string            text="Label",             // text
                 const string            font="Arial",             // font
                 const int               font_size=10,             // font size
                 const color             clr=clrRed,               // color
                 const double            angle=0.0,                // text slope
                 const ENUM_ANCHOR_POINT anchor=ANCHOR_LEFT_UPPER, // anchor type
                 const bool              back=false,               // in the background
                 const bool              selection=false,          // highlight to move
                 const bool              hidden=true,              // hidden in the object list
                 const long              z_order=0)                // priority for mouse click
  {
//--- reset the error value
   ResetLastError();
//--- create a text label
   if(!ObjectCreate(chart_ID,name,OBJ_LABEL,sub_window,0,0))
   {
      if( GetLastError()!=4200 ) // already exists
      {
         Print(__FUNCTION__,": failed to create text label! Error code = ",GetLastError());
         return(false);
      }
   }
//--- set label coordinates
   ObjectSetInteger(chart_ID,name,OBJPROP_XDISTANCE,x);
   ObjectSetInteger(chart_ID,name,OBJPROP_YDISTANCE,y);
//--- set the chart's corner, relative to which point coordinates are defined
   ObjectSetInteger(chart_ID,name,OBJPROP_CORNER,corner);
//--- set the text
   ObjectSetString(chart_ID,name,OBJPROP_TEXT,text);
//--- set text font
   ObjectSetString(chart_ID,name,OBJPROP_FONT,font);
//--- set font size
   ObjectSetInteger(chart_ID,name,OBJPROP_FONTSIZE,font_size);
//--- set the slope angle of the text
   ObjectSetDouble(chart_ID,name,OBJPROP_ANGLE,angle);
//--- set anchor type
   ObjectSetInteger(chart_ID,name,OBJPROP_ANCHOR,anchor);
//--- set color
   ObjectSetInteger(chart_ID,name,OBJPROP_COLOR,clr);
//--- display in the foreground (false) or background (true)
   ObjectSetInteger(chart_ID,name,OBJPROP_BACK,back);
//--- enable (true) or disable (false) the mode of moving the label by mouse
   ObjectSetInteger(chart_ID,name,OBJPROP_SELECTABLE,selection);
   ObjectSetInteger(chart_ID,name,OBJPROP_SELECTED,selection);
//--- hide (true) or display (false) graphical object name in the object list
   ObjectSetInteger(chart_ID,name,OBJPROP_HIDDEN,hidden);
//--- set the priority for receiving the event of a mouse click in the chart
   ObjectSetInteger(chart_ID,name,OBJPROP_ZORDER,z_order);
//--- successful execution
   return(true);
  }
  
#endif