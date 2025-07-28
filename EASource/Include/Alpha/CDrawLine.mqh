#ifndef __DRAW_LINE_H__
#define __DRAW_LINE_H__


class CDrawLine
{
public:
   CDrawLine(string name);
   ~CDrawLine(){Delete();};


   virtual void Draw( double i, color lineColor=clrYellow, int width=1, bool dotStyle=true )=0;
   void Delete();

   void Pin()     { m_bFix = true; }
   void UnPin()   { m_bFix = false; }

   string   Name() { return m_name; }
protected:
   string   m_name;
   bool     m_bFix;
};

CDrawLine::CDrawLine(string name)
{
   m_name = StringFormat("%s_%d",name,rand());
   m_bFix = false;
}


void CDrawLine::Delete(void)
{
   if(m_name=="")
      return;
   
   if( ObjectFind(0, m_name) == 0 )
      ObjectDelete(0, m_name);
}


////////////////////////////////////////////////////////////////////////
//
//          CVertLine
//
////////////////////////////////////////////////////////////////////////
class CVertLine : public CDrawLine
{
public:
   CVertLine(string name):CDrawLine(name){}
   ~CVertLine(){};
   
   virtual void Draw( double iShift, color lineColor, int width, bool dotStyle );
   
};


void CVertLine::Draw(double iShift, color lineColor, int width, bool dotStyle)
{
   if( m_bFix) return;
   
   //Delete();
   
   ObjectCreate(m_name,OBJ_VLINE,0,Time[(int)iShift],0);
   ObjectSet(m_name,OBJPROP_COLOR, lineColor);
   ObjectSet(m_name,OBJPROP_WIDTH,width);
   
   if(dotStyle)   ObjectSet(m_name, OBJPROP_STYLE, STYLE_DOT);
   else           ObjectSet(m_name, OBJPROP_STYLE, STYLE_SOLID);   
}

class CVLineManager
{
public:
   CVLineManager(){};
   ~CVLineManager(){DeleteAll();};
   
   void AddLine(string name, int iShift);
   void DeleteAll();
   
private:
   CVertLine*   m_arrLine[];
};


void CVLineManager::DeleteAll()
{
   for ( int k=0; k<ArraySize(m_arrLine); k++ )
      delete m_arrLine[k];
}

void CVLineManager::AddLine(string name, int iShift)
{
   int nCnt = ArraySize(m_arrLine);
   ArrayResize(m_arrLine, nCnt+1);
   m_arrLine[nCnt] = new CVertLine(name);
   
   m_arrLine[nCnt].Draw(iShift);
}


////////////////////////////////////////////////////////////////////////
//
//          CHorizonLine
//
////////////////////////////////////////////////////////////////////////

class CHorizonLine : public CDrawLine
{
public:
   CHorizonLine(string name):CDrawLine(name){}
   ~CHorizonLine(){};
   
   virtual void Draw( double iPrc, color lineColor, int width, bool dotStyle );
   
};


void CHorizonLine::Draw(double iPrc, color lineColor, int width, bool dotStyle)
{
   if( m_bFix) return;
   
   Delete();
   
   ObjectCreate(m_name,OBJ_HLINE,0,0,iPrc);
   ObjectSet(m_name,OBJPROP_COLOR, lineColor);
   ObjectSet(m_name,OBJPROP_WIDTH,width);
   
   if(dotStyle)   ObjectSet(m_name, OBJPROP_STYLE, STYLE_DOT);
   else           ObjectSet(m_name, OBJPROP_STYLE, STYLE_SOLID);   
}

class CHLineManager
{
public:
   CHLineManager(){};
   ~CHLineManager(){DeleteAll();};
   
   void AddLine(string name, double dPrc, color lineColor=clrYellow);
   void DeleteAll();
   void Delete(string name);
private:
   CHorizonLine*   m_arrLine[];
};


void CHLineManager::DeleteAll()
{
   for ( int k=0; k<ArraySize(m_arrLine); k++ )
      delete m_arrLine[k];
}

void CHLineManager::Delete(string name)
{
   for ( int k=0; k<ArraySize(m_arrLine); k++ )
   {
      if( m_arrLine[k].Name()==name )
         delete m_arrLine[k];
   }
}

void CHLineManager::AddLine(string name, double dPrc, color lineColor)
{
   int nCnt = ArraySize(m_arrLine);
   ArrayResize(m_arrLine, nCnt+1);
   m_arrLine[nCnt] = new CHorizonLine(name);
   
   m_arrLine[nCnt].Draw(dPrc, lineColor);
}



#endif