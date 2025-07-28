#ifndef __LINE_TEXT_NAME_H__
#define __LINE_TEXT_NAME_H__


#define X_START   30
#define Y_START   30
#define DISTANCE_LINES      30
#define DISTANCE_MULTILINES 20

#define MAX_LINES 10

struct TText
{
   string name;
   string msg;
   int cnt;
   int startIndex;
   int endIndex;
   color clr;
   int font_size;
   int maxLength;
   int hidden;
   
   TText()
   {
      hidden = 0;
      cnt=0;
   }
};



class CLineTextName
{
public:
   CLineTextName();
   ~CLineTextName();
   
   bool AddLine(string name,string msg,color clr = clrYellow,int font_size = 12,int maxLength = 77);
   bool Hide(string name);
   bool Show(string name);
   bool UpdateText(string name,string msg,color clr=0,int font_size=0);
   bool UpdateColor(string name,color clr);
   bool UpdateFontSize(string name,int size);
   bool Delete(string name);
   bool DeleteAll();
   
private:
   int GetLastIndex();
private:
   int   m_nowCount;
   TText m_Texts[100];
};

CLineTextName::CLineTextName()
{
   m_nowCount  = 0;
};

CLineTextName::~CLineTextName()
{
   DeleteAll();
}


bool CLineTextName::AddLine(string name,string msg,color clr,int font_size,int maxLength)
{
   if(ObjectFind(0,name+IntegerToString(0))!=-1)
   {
      Print("Object Already Exists name = "+name);
      return false;
   }
   bool added = false;
   int x=X_START;
   int y=GetLastIndex();
   m_Texts[m_nowCount].name = name;
   m_Texts[m_nowCount].msg = msg;
   m_Texts[m_nowCount].clr = clr;
   m_Texts[m_nowCount].font_size = font_size;
   m_Texts[m_nowCount].maxLength = maxLength;
   m_Texts[m_nowCount].startIndex = y;
   m_Texts[m_nowCount].cnt=0;
   do
   {  
      y+=added?DISTANCE_MULTILINES:DISTANCE_LINES;
      LabelCreate(0,name+IntegerToString(m_Texts[m_nowCount].cnt++),0,x,y,CORNER_LEFT_UPPER,StringSubstr(msg,0,maxLength),"Arial",font_size,clr);
      msg = StringSubstr(msg,maxLength+1);
      m_Texts[m_nowCount].endIndex = y;
      added= true;
   }while(StringLen(msg)>0);
   
   m_nowCount++;
   
   return true;
}



bool CLineTextName::Hide(string name)
{
   for(int i=0;i<m_nowCount;i++)
   {
      if(m_Texts[i].name == name)
      {
         m_Texts[i].hidden = 1;
         for(int j=0;j<m_Texts[i].cnt;j++)
         {
            ObjectDelete(0,name+IntegerToString(j));
         }
         break;
      }
   }
   return true;
}



bool CLineTextName::Show(string name)
{
   for(int i=0;i<m_nowCount;i++)
   {
      if(m_Texts[i].name == name)
      {
         m_Texts[i].hidden = 0;
         int x = X_START;
         int y = m_Texts[i].startIndex;
         string msg = m_Texts[i].msg;
         bool added = false;
         int maxLength = m_Texts[i].maxLength;
         int font_size = m_Texts[i].font_size;
         color clr = m_Texts[i].clr;
         for(int j=0;j<m_Texts[i].cnt;j++)
         {
            y+=added?DISTANCE_MULTILINES:DISTANCE_LINES;
            LabelCreate(0,name+IntegerToString(j),0,x,y,CORNER_LEFT_UPPER,StringSubstr(msg,0,maxLength),"Arial",font_size,clr);
            msg = StringSubstr(msg,maxLength+1);
            added= true;
         }
         break;
      }
   }
   return true;
}

bool CLineTextName::UpdateText(string name,string msg,color clr,int font_size)
{
   int distance = 0;
   int below=0;
   int i,j;
   for(i=0;i<m_nowCount;i++)
   {
      if(m_Texts[i].name == name)
      {
         int newclr = m_Texts[i].clr;
         int newfont_size = m_Texts[i].font_size;
         
         if(clr!=0)
            newclr = clr;
         if(font_size!=0)
            newfont_size = font_size;
         string newmsg = m_Texts[i].msg;
         if(StringLen(msg)>0)
            newmsg = msg;
         m_Texts[i].msg = newmsg;
         for(j=0;j<m_Texts[i].cnt;j++)
         {
            ObjectDelete(0,name+IntegerToString(j));
         }
         int x = X_START;
         int y = m_Texts[i].startIndex;
         int newEndY = y;
         below = m_Texts[i].endIndex;
         bool added=false;
         m_Texts[i].cnt=0;
         do
         {  
            y+=added?DISTANCE_MULTILINES:DISTANCE_LINES;
            LabelCreate(0,name+IntegerToString(m_Texts[i].cnt++),0,x,y,CORNER_LEFT_UPPER,StringSubstr(newmsg,0,m_Texts[i].maxLength),"Arial",newfont_size,newclr);
            newmsg = StringSubstr(newmsg,m_Texts[i].maxLength+1);
            newEndY = y;
            added= true;
         }while(StringLen(newmsg)>0);
         distance = newEndY-m_Texts[i].endIndex;
         break;
      }
      
   }
   if(distance!=0)
   {
      for(i=0;i<m_nowCount;i++)
      {
         if(m_Texts[i].name!=name && m_Texts[i].startIndex>=below)
         {
            m_Texts[i].startIndex +=distance;
            m_Texts[i].endIndex +=distance;
            for(j=0;j<m_Texts[i].cnt;j++)
            {
               int old = ObjectGetInteger(0,m_Texts[i].name+IntegerToString(j),OBJPROP_YDISTANCE);
               ObjectSetInteger(0,m_Texts[i].name+IntegerToString(j),OBJPROP_YDISTANCE,old+distance);
            }
         }
      }
   }
   return true;
}

bool CLineTextName::UpdateColor(string name,color clr)
{
   for(int i=0;i<m_nowCount;i++)
   {
      if(m_Texts[i].name == name)
      {
         for(int j=0;j<m_Texts[i].cnt;j++)
         {
            ObjectSetInteger(0,name+IntegerToString(j),OBJPROP_COLOR,clr);
         }
         break;
      }
      
   }
   return true;
}
bool CLineTextName::UpdateFontSize(string name,int size)
{
   for(int i=0;i<m_nowCount;i++)
   {
      if(m_Texts[i].name == name)
      {
         for(int j=0;j<m_Texts[i].cnt;j++)
         {
            ObjectSetInteger(0,name+IntegerToString(j),OBJPROP_FONTSIZE,size);
         }
         break;
      }
      
   }
   return true;
}

bool CLineTextName::Delete(string name)
{
   int below = 0;
   int distance = 0;
   int i,j;
   for(i=0;i<m_nowCount;i++)
   {
      if(m_Texts[i].name == name)
      {
         below = m_Texts[i].startIndex;
         distance = m_Texts[i].endIndex;
         for(j=0;j<m_Texts[i].cnt;j++)
         {
            ObjectDelete(0,name+IntegerToString(j));
         }
         m_Texts[i]=m_Texts[m_nowCount-1];
         m_nowCount--;
         break;
      }
      
   }
   distance = -(distance-below);
   if(distance!=0)
   {
      for(i=0;i<m_nowCount;i++)
      {
         if(m_Texts[i].startIndex>=below)
         {
            m_Texts[i].startIndex +=distance;
            m_Texts[i].endIndex +=distance;
            for(j=0;j<m_Texts[i].cnt;j++)
            {
               int old = ObjectGetInteger(0,m_Texts[i].name+IntegerToString(j),OBJPROP_YDISTANCE);
               ObjectSetInteger(0,m_Texts[i].name+IntegerToString(j),OBJPROP_YDISTANCE,old+distance);
            }
         }
      }
   }
   return true;
}
bool CLineTextName::DeleteAll()
{
   for(int i=0;i<m_nowCount;i++)
   {
      for(int j=0;j<m_Texts[i].cnt;j++)
      {
         ObjectDelete(0,m_Texts[i].name+IntegerToString(j));
      }
   }
   m_nowCount=0;
   return true;
}

int CLineTextName::GetLastIndex()
{
   int n=Y_START;
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
                 const ENUM_BASE_CORNER  corner=CORNER_LEFT_UPPER, // chart corner for anchoring
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
      Print(__FUNCTION__,
            ": failed to create text label! Error code = ",GetLastError());
      return(false);
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