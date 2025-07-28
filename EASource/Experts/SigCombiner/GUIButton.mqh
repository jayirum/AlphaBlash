//+------------------------------------------------------------------+
//|                                                  PanelDialog.mqh |
//|                   Copyright 2009-2015, MetaQuotes Software Corp. |
//|                                              http://www.mql5.com |
//+------------------------------------------------------------------+
#include <Controls\Dialog.mqh>
#include <Controls\Button.mqh>
#include <Controls\Edit.mqh>
#include <Controls\ComboBox.mqh>
#include <Controls\SpinEdit.mqh>
#include <Controls\Label.mqh>

//+------------------------------------------------------------------+
//| defines                                                          |
//+------------------------------------------------------------------+
//--- indents and gaps
#define INDENT_LEFT                         (11)      // indent from left (with allowance for border width)
#define INDENT_TOP                          (11)      // indent from top (with allowance for border width)
#define INDENT_RIGHT                        (11)      // indent from right (with allowance for border width)
#define INDENT_BOTTOM                       (11)      // indent from bottom (with allowance for border width)
#define CONTROLS_GAP_X                      (10)      // gap by X coordinate
#define CONTROLS_GAP_Y                      (10)      // gap by Y coordinate
//--- for buttons
#define BUTTON_WIDTH                        (100)     // size by X coordinate
#define BUTTON_HEIGHT                       (20)      // size by Y coordinate
//--- for the indication area
#define EDIT_HEIGHT                         (30)      // size by Y coordinate
#define EDIT_WIDTH                          (100)

//#define LABEL_HEIGHT                         (35)      // size by Y coordinate
#define GUILABEL_WIDTH                          (50)

//+------------------------------------------------------------------+
//| Class CPanelDialog                                               |
//| Usage: main dialog of the SimplePanel application                |
//+------------------------------------------------------------------+


struct TCordinates
{
   int x1;
   int x2;
   int y1;
   int y2;
};

class CPanelDialog : public CAppDialog
  {
private:
   
   CEdit             m_edtSymbol; // the display field object
   CEdit             m_edtBuy; // the display field object
   CEdit             m_edtSell; // the display field object
   CButton           m_btnOrd;     // the button object
   

public:
                     CPanelDialog(void);
                    ~CPanelDialog(void);
   //--- create
   virtual bool      Create(const long chart,const string name,const int subwin,const int x1,const int y1,const int x2,const int y2);
   //--- chart event handler
   virtual bool      OnEvent(const int id,const long &lparam,const double &dparam,const string &sparam);


   //////////////////////////////////////////////////////////////////////////////
   // my functions
public:
   bool     Is_BtnOrd_Clicked();
   string   getEditSymgol()   { return m_edtSymbol.Text(); }
   int      getBuyCount()     { return (int)StringToInteger(m_edtBuy.Text()); }
   int      getSellCount()    { return (int)StringToInteger(m_edtSell.Text()); }
   
private:
   TCordinates    m_corEditSymbol;
   TCordinates    m_corEditBuy;
   TCordinates    m_corEditSell;
   TCordinates    m_corBtnOrd;
   
   bool           m_bOrder;
   
protected:
   //--- create dependent controls
   bool           CreateEdit(void);
   bool           CreateBtnOrd(void);

   //--- internal event handlers
   virtual bool      OnResize(void);
   //--- handlers of the dependent controls events
   void              OnClickBtnOrd(void);
   
   bool              OnDefault(const int id,const long &lparam,const double &dparam,const string &sparam);
  };
//+------------------------------------------------------------------+
//| Event Handling                                                   |
//+------------------------------------------------------------------+
EVENT_MAP_BEGIN(CPanelDialog)
ON_EVENT(ON_CLICK,m_btnOrd,     OnClickBtnOrd)
ON_OTHER_EVENTS(OnDefault)
EVENT_MAP_END(CAppDialog)


//+------------------------------------------------------------------+
//| Constructor                                                      |
//+------------------------------------------------------------------+
CPanelDialog::CPanelDialog(void)
{
   m_bOrder    = false;
}

//+------------------------------------------------------------------+
//| Destructor                                                       |
//+------------------------------------------------------------------+
CPanelDialog::~CPanelDialog(void)
{
}

bool CPanelDialog::Is_BtnOrd_Clicked()
{
   bool b = m_bOrder;
   if(m_bOrder)
      m_bOrder = false;
   
   return b;
}



//+------------------------------------------------------------------+
//| Create                                                           |
//+------------------------------------------------------------------+
bool CPanelDialog::Create(const long chart,const string name,const int subwin,const int x1,const int y1,const int x2,const int y2)
{
   if(!CAppDialog::Create(chart,name,subwin,x1,y1,x2,y2))
      return(false);
      
   //--- create dependent controls
   if(!CreateEdit())    return(false);
   if(!CreateBtnOrd())  return(false);
   
   return(true);
}


//+------------------------------------------------------------------+
//| Create the display field                                         |
//+------------------------------------------------------------------+

//#define INDENT_LEFT                         (11)      // indent from left (with allowance for border width)
//#define INDENT_TOP                          (11)      // indent from top (with allowance for border width)
//#define INDENT_RIGHT                        (11)      // indent from right (with allowance for border width)
//#define INDENT_BOTTOM                       (11)      // indent from bottom (with allowance for border width)
//#define CONTROLS_GAP_X                      (10)      // gap by X coordinate
//#define CONTROLS_GAP_Y                      (10)      // gap by Y coordinate
////--- for buttons
//#define BUTTON_WIDTH                        (100)     // size by X coordinate
//#define BUTTON_HEIGHT                       (20)      // size by Y coordinate
////--- for the indication area
//#define EDIT_HEIGHT                         (20)      // size by Y coordinate


//+------------------------------------------------------------------+
//| Create the "Symbol" Editbox                                      |
//+------------------------------------------------------------------+
bool CPanelDialog::CreateEdit(void)
{
   //--- coordinates
   m_corEditSymbol.x1 = INDENT_LEFT;
   m_corEditSymbol.y1 = INDENT_TOP;
   m_corEditSymbol.x2 = m_corEditSymbol.x1 + EDIT_WIDTH; //  ClientAreaWidth()-(INDENT_RIGHT+BUTTON_WIDTH+CONTROLS_GAP_X);
   m_corEditSymbol.y2 = m_corEditSymbol.y1 + EDIT_HEIGHT;
   
   m_corEditBuy.x1 = m_corEditSymbol.x2 + CONTROLS_GAP_X;
   m_corEditBuy.y1 = INDENT_TOP;
   m_corEditBuy.x2 = m_corEditBuy.x1 + EDIT_WIDTH; //  ClientAreaWidth()-(INDENT_RIGHT+BUTTON_WIDTH+CONTROLS_GAP_X);
   m_corEditBuy.y2 = m_corEditBuy.y1 + EDIT_HEIGHT;

   m_corEditSell.x1 = m_corEditBuy.x2 + CONTROLS_GAP_X;
   m_corEditSell.y1 = INDENT_TOP;
   m_corEditSell.x2 = m_corEditSell.x1 + EDIT_WIDTH; //  ClientAreaWidth()-(INDENT_RIGHT+BUTTON_WIDTH+CONTROLS_GAP_X);
   m_corEditSell.y2 = m_corEditSell.y1 + EDIT_HEIGHT;
   
   //--- create
   if(!m_edtSymbol.Create(m_chart_id,m_name+"EditSymbol",m_subwin,
                              m_corEditSymbol.x1,
                              m_corEditSymbol.y1,
                              m_corEditSymbol.x2,
                              m_corEditSymbol.y2
                              ))
      return(false);
      
   if(!m_edtBuy.Create(m_chart_id,m_name+"EditBuy",m_subwin,
                              m_corEditBuy.x1,
                              m_corEditBuy.y1,
                              m_corEditBuy.x2,
                              m_corEditBuy.y2
                              ))
      return(false);

   if(!m_edtSell.Create(m_chart_id,m_name+"EditSell",m_subwin,
                              m_corEditSell.x1,
                              m_corEditSell.y1,
                              m_corEditSell.x2,
                              m_corEditSell.y2
                              ))
      return(false);

   if(!Add(m_edtSymbol)) return(false);
   if(!Add(m_edtBuy))   return(false);
   if(!Add(m_edtSell))  return(false);

   m_edtSymbol.Alignment(WND_ALIGN_WIDTH,INDENT_LEFT,0,INDENT_RIGHT+BUTTON_WIDTH+CONTROLS_GAP_X,0);
   m_edtBuy.Alignment(WND_ALIGN_WIDTH,INDENT_LEFT,0,INDENT_RIGHT+BUTTON_WIDTH+CONTROLS_GAP_X,0);
   m_edtSell.Alignment(WND_ALIGN_WIDTH,INDENT_LEFT,0,INDENT_RIGHT+BUTTON_WIDTH+CONTROLS_GAP_X,0);
   
   //--- succeed
   return(true);
}


  
//+------------------------------------------------------------------+
//| Create the "Button1" button                                      |
//+------------------------------------------------------------------+
bool CPanelDialog::CreateBtnOrd(void)
{
   //--- coordinates
   m_corBtnOrd.x1 = ClientAreaWidth()-(INDENT_RIGHT+BUTTON_WIDTH);
   m_corBtnOrd.y1 = INDENT_TOP;
   m_corBtnOrd.x2 = m_corBtnOrd.x1+BUTTON_WIDTH;
   m_corBtnOrd.y2 = m_corBtnOrd.y1+BUTTON_HEIGHT;
   
   //--- create
   if(!m_btnOrd.Create(m_chart_id,m_name+"BtnBuy",m_subwin, 
                        m_corBtnOrd.x1, 
                        m_corBtnOrd.y1, 
                        m_corBtnOrd.x2, 
                        m_corBtnOrd.y2
                        ))
      return(false);
      
   if(!m_btnOrd.Text("Order"))
      return(false);

   if(!Add(m_btnOrd))
      return(false);

   m_btnOrd.Alignment(WND_ALIGN_RIGHT,0,0,INDENT_RIGHT,0);

   return(true);
}


//+------------------------------------------------------------------+
//| Handler of resizing                                              |
//+------------------------------------------------------------------+
bool CPanelDialog::OnResize(void)
{
   //--- call method of parent class
   if(!CAppDialog::OnResize()) return(false);
   
   //--- coordinates
   int x=ClientAreaLeft()+INDENT_LEFT;
   //int y=m_radio_group.Top();
   //int y=m_check_group.Top();
   int sx=(ClientAreaWidth()-(INDENT_LEFT+INDENT_RIGHT+BUTTON_WIDTH))/3-CONTROLS_GAP_X;
   
   //--- move and resize the "RadioGroup" element
//   m_radio_group.Move(x,y);
//   m_radio_group.Width(sx);
////--- move and resize the "CheckGroup" element
   x=ClientAreaLeft()+INDENT_LEFT+sx+CONTROLS_GAP_X;
   //m_check_group.Move(x,y);
   //m_check_group.Width(sx);

//--- move and resize the "ListView" element
//   x=ClientAreaLeft()+ClientAreaWidth()-(sx+INDENT_RIGHT+BUTTON_WIDTH+CONTROLS_GAP_X);
//   m_lstSL.Move(x,y);
//   m_lstSL.Width(sx);
////--- succeed
   return(true);
  }
//+------------------------------------------------------------------+
//| Event handler                                                    |
//+------------------------------------------------------------------+
void CPanelDialog::OnClickBtnOrd(void)
{
   m_bOrder = true;
   //Print("OnClickBtnOrd");
}
//+------------------------------------------------------------------+
//| Rest events handler                                                    |
//+------------------------------------------------------------------+
bool CPanelDialog::OnDefault(const int id,const long &lparam,const double &dparam,const string &sparam)
  {
//--- restore buttons' states after mouse move'n'click
   //if(id==CHARTEVENT_CLICK)
   //   m_radio_group.RedrawButtonStates();
//--- let's handle event by parent
   return(false);
  }
//+------------------------------------------------------------------+
