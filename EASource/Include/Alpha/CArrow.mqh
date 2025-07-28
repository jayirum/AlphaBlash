#ifndef __DRAW_ARROW_H__
#define __DRAW_ARROW_H__


#define COLOR_UP        clrGreen
#define COLOR_DN        clrRed
#define DEFAULT_WIDTH   5

class CArrow
{
public:
   CArrow(string name);
   ~CArrow(){ Delete();};


   void UpArrow( int iShift, int width=DEFAULT_WIDTH );
   void DnArrow( int iShift, int width=DEFAULT_WIDTH );
   
   void Delete();
   
private:
   void DrawArrow(bool bUp, int iShift, int width);
   
protected:
   
   string   m_name;
   
};

CArrow::CArrow(string name)
{
   m_name = StringFormat("%s_%d",name,rand());
}


void CArrow::Delete()
{
   if(m_name=="")
      return;
   
   if( ObjectFind(0, m_name) == 0 )
      ObjectDelete(0, m_name);
}




void CArrow::UpArrow( int iShift, int width )
{
   DrawArrow(true, iShift, width);
}


void CArrow::DnArrow( int iShift, int width )
{
   DrawArrow(false, iShift, width);
}

void CArrow::DrawArrow(bool bUp, int iShift, int width)
{
   Delete();
   int lineColor;
   if( bUp )
   {
      lineColor = COLOR_UP;
      ObjectCreate(0, m_name, OBJ_ARROW, 0, Time[iShift], Low[iShift]-(30*Point)  );
      ObjectSet(m_name, OBJPROP_ARROWCODE, SYMBOL_ARROWUP);
      //PrintFormat("<%d> Up (%d)", iShift, Time[iShift]);
   }
   else
   {
      lineColor = COLOR_DN;
      ObjectCreate(0, m_name, OBJ_ARROW, 0, Time[iShift], High[iShift]+(30*Point)  );
      ObjectSet(m_name, OBJPROP_ARROWCODE, SYMBOL_ARROWDOWN);
      
      //PrintFormat("<%d> Dn (%d)", iShift, Time[iShift]);
   }
   ObjectSet(m_name, OBJPROP_STYLE, STYLE_SOLID);   
   ObjectSet(m_name,OBJPROP_COLOR, lineColor);
   ObjectSet(m_name,OBJPROP_WIDTH,width);
   
}

/////////////////////////////////////////////////////////////////////////////
//
//
//
//
/////////////////////////////////////////////////////////////////////////////
class CArrowManager
{
public:
   CArrowManager(){};
   ~CArrowManager(){DeleteAll();};
   
   void AddArrow(string name, bool bUp, int iShift, int width=DEFAULT_WIDTH);
   void DeleteAll();
   
private:
   CArrow*   m_arrArrow[];
};


void CArrowManager::DeleteAll()
{
   for ( int k=0; k<ArraySize(m_arrArrow); k++ )
      delete m_arrArrow[k];
}

void CArrowManager::AddArrow(string name, bool bUp, int iShift, int width)
{
   int nCnt = ArraySize(m_arrArrow);
   ArrayResize(m_arrArrow, nCnt+1);
   m_arrArrow[nCnt] = new CArrow(name);
   
   if( bUp )   m_arrArrow[nCnt].UpArrow(iShift, width);
   if( !bUp )  m_arrArrow[nCnt].DnArrow(iShift, width);
}

#endif