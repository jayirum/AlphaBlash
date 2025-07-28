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
#define LABEL_WIDTH                          (50)

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
   CEdit             m_edtSymbol;      // the display field object
   CButton           m_btnBuy;     // the button object
   CButton           m_btnSell;    // the button object

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
   bool           Is_BuyButton_Clicked();
   bool           Is_SellButton_Clicked();
   string         getEditSymgol() { return m_edtSymbol.Text(); }
private:
   TCordinates    m_corEdit;
   TCordinates    m_corBtnBuy;
   TCordinates    m_corBtnSell;
   
   bool           m_bOrderBuy;
   bool           m_bOrderSell;
   
protected:
   //--- create dependent controls
   bool           CreateEdit(void);
   bool           CreateBtn_Buy(void);
   bool           CreateBtn_Sell(void);

   //--- internal event handlers
   virtual bool      OnResize(void);
   //--- handlers of the dependent controls events
   void              OnClickBtnBuy(void);
   void              OnClickBtnSell(void);
   
   bool              OnDefault(const int id,const long &lparam,const double &dparam,const string &sparam);
  };
//+------------------------------------------------------------------+
//| Event Handling                                                   |
//+------------------------------------------------------------------+
EVENT_MAP_BEGIN(CPanelDialog)
ON_EVENT(ON_CLICK,m_btnBuy,     OnClickBtnBuy)
ON_EVENT(ON_CLICK,m_btnSell,    OnClickBtnSell)
ON_OTHER_EVENTS(OnDefault)
EVENT_MAP_END(CAppDialog)


//+------------------------------------------------------------------+
//| Constructor                                                      |
//+------------------------------------------------------------------+
CPanelDialog::CPanelDialog(void)
{
   m_bOrderBuy    = false;
   m_bOrderSell   = false;
}

//+------------------------------------------------------------------+
//| Destructor                                                       |
//+------------------------------------------------------------------+
CPanelDialog::~CPanelDialog(void)
{
}

bool CPanelDialog::Is_BuyButton_Clicked()
{
   bool b = m_bOrderBuy;
   if(m_bOrderBuy)
      m_bOrderBuy = false;
   
   return b;
}

bool CPanelDialog::Is_SellButton_Clicked()
{
   bool b = m_bOrderSell;
   if(m_bOrderSell)
      m_bOrderSell = false;
   
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
   if(!CreateEdit())         return(false);
   if(!CreateBtn_Buy())      return(false);
   if(!CreateBtn_Sell())     return(false);
   
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
   m_corEdit.x1 = INDENT_LEFT;
   m_corEdit.y1 = INDENT_TOP;
   m_corEdit.x2 = m_corEdit.x1 + EDIT_WIDTH; //  ClientAreaWidth()-(INDENT_RIGHT+BUTTON_WIDTH+CONTROLS_GAP_X);
   m_corEdit.y2 = m_corEdit.y1 + EDIT_HEIGHT;
   
   
   //--- create
   if(!m_edtSymbol.Create(m_chart_id,m_name+"Edit",m_subwin,m_corEdit.x1,m_corEdit.y1,m_corEdit.x2,m_corEdit.y2))
      return(false);
      
   if(!Add(m_edtSymbol))
      return(false);

   m_edtSymbol.Alignment(WND_ALIGN_WIDTH,INDENT_LEFT,0,INDENT_RIGHT+BUTTON_WIDTH+CONTROLS_GAP_X,0);
   //--- succeed
   return(true);
}


  
//+------------------------------------------------------------------+
//| Create the "Button1" button                                      |
//+------------------------------------------------------------------+
bool CPanelDialog::CreateBtn_Buy(void)
{
   //--- coordinates
   m_corBtnBuy.x1 = ClientAreaWidth()-(INDENT_RIGHT+BUTTON_WIDTH);
   m_corBtnBuy.y1 = INDENT_TOP;
   m_corBtnBuy.x2 = m_corBtnBuy.x1+BUTTON_WIDTH;
   m_corBtnBuy.y2 = m_corBtnBuy.y1+BUTTON_HEIGHT;
   
   //--- create
   if(!m_btnBuy.Create(m_chart_id,m_name+"BtnBuy",m_subwin, m_corBtnBuy.x1, m_corBtnBuy.y1, m_corBtnBuy.x2, m_corBtnBuy.y2))
      return(false);
      
   if(!m_btnBuy.Text("Buy"))
      return(false);

   if(!Add(m_btnBuy))
      return(false);

   m_btnBuy.Alignment(WND_ALIGN_RIGHT,0,0,INDENT_RIGHT,0);

   return(true);
}

//+------------------------------------------------------------------+
//| Create the "Button2" button                                      |
//+------------------------------------------------------------------+
bool CPanelDialog::CreateBtn_Sell(void)
{
   //--- coordinates
   m_corBtnSell.x1 = ClientAreaWidth()-(INDENT_RIGHT+BUTTON_WIDTH);
   m_corBtnSell.y1 = m_corBtnBuy.y2  + CONTROLS_GAP_Y;
   m_corBtnSell.x2 = m_corBtnSell.x1 + BUTTON_WIDTH;
   m_corBtnSell.y2 = m_corBtnSell.y1 + BUTTON_HEIGHT;

   //--- create
   if(!m_btnSell.Create(m_chart_id,m_name+"BtnSell",m_subwin, m_corBtnSell.x1, m_corBtnSell.y1, m_corBtnSell.x2, m_corBtnSell.y2))
      return(false);

   if(!m_btnSell.Text("Sell"))
      return(false);
   
   if(!Add(m_btnSell))
      return(false);
   
   m_btnSell.Alignment(WND_ALIGN_RIGHT,0,0,INDENT_RIGHT,0);

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
void CPanelDialog::OnClickBtnBuy(void)
{
   m_bOrderBuy = true;
}
//+------------------------------------------------------------------+
//| Event handler                                                    |
//+------------------------------------------------------------------+
void CPanelDialog::OnClickBtnSell(void)
{
   m_bOrderSell = true;
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
