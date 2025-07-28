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
#define EDIT_WIDTH                          (70)

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
   CLabel            m_lblTicket[];
   CEdit             m_edit[];      // the display field object
   CButton           m_btnSave;     // the button object
   CButton           m_btnReset;    // the button object
   CButton           m_btnDestroy;  // the button object

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
   void  SetOrderCount(int nOrderCnt);
   bool  AddOrders(int idx, int nTicket);
   void  ReadAllTickets();
   
private:
   int            m_lastOrdIdx;
   TCordinates    m_corBtnSave;
   TCordinates    m_corBtnReset;
   TCordinates    m_corBtnDestroy;
   
protected:
   //--- create dependent controls
   bool           CreateBtn_Save(void);
   bool           CreateBtn_Reset(void);
   bool           CreateBtn_Destroy(void);
   void           RemoveEdit();

   //--- internal event handlers
   virtual bool      OnResize(void);
   //--- handlers of the dependent controls events
   void              OnClickBtnSave(void);
   void              OnClickBtnReset(void);
   void              OnClickBtnDestroy(void);
   
   bool              OnDefault(const int id,const long &lparam,const double &dparam,const string &sparam);
  };
//+------------------------------------------------------------------+
//| Event Handling                                                   |
//+------------------------------------------------------------------+
EVENT_MAP_BEGIN(CPanelDialog)
ON_EVENT(ON_CLICK,m_btnSave,     OnClickBtnSave)
ON_EVENT(ON_CLICK,m_btnReset,    OnClickBtnReset)
ON_EVENT(ON_CLICK,m_btnDestroy,  OnClickBtnDestroy)
ON_OTHER_EVENTS(OnDefault)
EVENT_MAP_END(CAppDialog)





void CPanelDialog::ReadAllTickets()
{
   //TODO
   //for( int i=0; i<m_check_group.GetTotalCount(); i++ )
   //{
   //   PrintFormat("[%d]%s",i, m_check_group.GetCheckBoxString(i));
   //}
}



void CPanelDialog::RemoveEdit()
{
   for( int k=0; k<ArraySize(m_edit); k++)
   {
      m_edit[k].Destroy();
      m_lblTicket[k].Destroy();
   }
}
//+------------------------------------------------------------------+
//| Constructor                                                      |
//+------------------------------------------------------------------+
CPanelDialog::CPanelDialog(void)
  {
   m_lastOrdIdx = 0;
  }
//+------------------------------------------------------------------+
//| Destructor                                                       |
//+------------------------------------------------------------------+
CPanelDialog::~CPanelDialog(void)
  {
  }
//+------------------------------------------------------------------+
//| Create                                                           |
//+------------------------------------------------------------------+
bool CPanelDialog::Create(const long chart,const string name,const int subwin,const int x1,const int y1,const int x2,const int y2)
{
   if(!CAppDialog::Create(chart,name,subwin,x1,y1,x2,y2))
      return(false);
      
   //--- create dependent controls
   //if(!CreateEdit())
   //   return(false);
   if(!CreateBtn_Save())      return(false);
   if(!CreateBtn_Reset())     return(false);
   if(!CreateBtn_Destroy())   return(false);
   
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
bool CPanelDialog::AddOrders(int idx, int nTicket)
{
   string sTicket = IntegerToString(nTicket);
   
   
   //--- coordinates
   TCordinates c;
   c.x1 = INDENT_LEFT;
   c.y1 = INDENT_TOP + (EDIT_HEIGHT*idx);
   c.x2 = c.x1 + LABEL_WIDTH;
   c.y2 = c.y1 + EDIT_HEIGHT;
   
   //--- create
   string name = StringFormat("LABEL_SL_%d", nTicket);
   if(!m_lblTicket[idx].Create(m_chart_id, name,m_subwin, c.x1, c.y1, c.x2, c.y2))
      return(false);

   if(!Add(m_lblTicket[idx]))  return(false);      
   m_lblTicket[idx].Alignment(WND_ALIGN_WIDTH, c.x1, c.y1, c.x2, c.y2 );   
   m_lblTicket[idx].Text( StringFormat("[%s]",sTicket));
   
   
   TCordinates c2;
   c2.x1 = c.x2 + CONTROLS_GAP_X;
   c2.y1 = INDENT_TOP + (EDIT_HEIGHT*idx);
   c2.x2 = c2.x1 + EDIT_WIDTH;
   c2.y2 = c2.y1 + EDIT_HEIGHT;
     
   //--- create
   name = StringFormat("EDIT_SL_%d", nTicket);
   if(!m_edit[idx].Create(m_chart_id, name,m_subwin, c2.x1, c2.y1, c2.x2, c2.y2))
      return(false);

   if(!Add(m_edit[idx]))  return(false);
   m_edit[idx].Text("SL Point");      
   m_edit[idx].Alignment(WND_ALIGN_WIDTH, c2.x1, c2.y1, c2.x2, c2.y2);
   
   //--- succeed
   return(true);
}


void  CPanelDialog::SetOrderCount(int nOrderCnt)
{
   ArrayResize(m_edit,        nOrderCnt);
   ArrayResize(m_lblTicket,   nOrderCnt);
}
  
//  //+------------------------------------------------------------------+
////| Create the "CheckGroup" element                                  |
////+------------------------------------------------------------------+
//bool CPanelDialog::CreateCheckGroup(void)
//{
//   
//   int sx=(ClientAreaWidth()-(INDENT_LEFT+INDENT_RIGHT+BUTTON_WIDTH))/3-CONTROLS_GAP_X;
//
//   //--- coordinates
//   m_cordiCheckBox.x1=INDENT_LEFT;  //+sx+CONTROLS_GAP_X;
//   m_cordiCheckBox.y1=INDENT_TOP+EDIT_HEIGHT+CONTROLS_GAP_Y;
//   m_cordiCheckBox.x2=m_cordiCheckBox.x1+sx;
//   m_cordiCheckBox.y2=ClientAreaHeight()-INDENT_BOTTOM;
//   
//   //--- create
//   if(!m_check_group.Create(m_chart_id,m_name+"CheckGroup",m_subwin, m_cordiCheckBox.x1,m_cordiCheckBox.y1,m_cordiCheckBox.x2,m_cordiCheckBox.y2))
//      return(false);
//
//   if(!Add(m_check_group))
//      return(false);
//
//   m_check_group.Alignment(WND_ALIGN_HEIGHT,0,m_cordiCheckBox.y1,0,INDENT_BOTTOM);
//
//   
//   return(true);
//}
//  
  
//void CPanelDialog::AddOrders(int nTicket)
//{
//   string sTicket = IntegerToString(nTicket);
//   if( !m_check_group.AddItem(sTicket,nTicket) )
//   {
//      Alert("AddOrders Error");
//      return ;
//   }
//   
//   m_lastOrdIdx++;
//}
  
  
//+------------------------------------------------------------------+
//| Create the "Button1" button                                      |
//+------------------------------------------------------------------+
bool CPanelDialog::CreateBtn_Save(void)
{
   //--- coordinates
   m_corBtnSave.x1 = ClientAreaWidth()-(INDENT_RIGHT+BUTTON_WIDTH);
   m_corBtnSave.y1 = INDENT_TOP;
   m_corBtnSave.x2 = m_corBtnSave.x1+BUTTON_WIDTH;
   m_corBtnSave.y2 = m_corBtnSave.y1+BUTTON_HEIGHT;
   
   //--- create
   if(!m_btnSave.Create(m_chart_id,m_name+"BtnSave",m_subwin, m_corBtnSave.x1, m_corBtnSave.y1, m_corBtnSave.x2, m_corBtnSave.y2))
      return(false);
      
   if(!m_btnSave.Text("Save"))
      return(false);

   if(!Add(m_btnSave))
      return(false);

   m_btnSave.Alignment(WND_ALIGN_RIGHT,0,0,INDENT_RIGHT,0);

   return(true);
}

//+------------------------------------------------------------------+
//| Create the "Button2" button                                      |
//+------------------------------------------------------------------+
bool CPanelDialog::CreateBtn_Reset(void)
{
   //--- coordinates
   m_corBtnReset.x1 = ClientAreaWidth()-(INDENT_RIGHT+BUTTON_WIDTH);
   m_corBtnReset.y1 = m_corBtnSave.y2  + CONTROLS_GAP_Y;
   m_corBtnReset.x2 = m_corBtnReset.x1 + BUTTON_WIDTH;
   m_corBtnReset.y2 = m_corBtnReset.y1 + BUTTON_HEIGHT;

   //--- create
   if(!m_btnReset.Create(m_chart_id,m_name+"BtnReset",m_subwin, m_corBtnReset.x1, m_corBtnReset.y1, m_corBtnReset.x2, m_corBtnReset.y2))
      return(false);

   if(!m_btnReset.Text("Reset"))
      return(false);
   
   if(!Add(m_btnReset))
      return(false);
   
   m_btnReset.Alignment(WND_ALIGN_RIGHT,0,0,INDENT_RIGHT,0);

   return(true);
}
  
//+------------------------------------------------------------------+
//| Create the "Button3" fixed button                                |
//+------------------------------------------------------------------+
bool CPanelDialog::CreateBtn_Destroy(void)
{
   //--- coordinates
   m_corBtnDestroy.x1 = ClientAreaWidth()-(INDENT_RIGHT+BUTTON_WIDTH);
   m_corBtnDestroy.y1 = m_corBtnReset.y2   + CONTROLS_GAP_Y;
   m_corBtnDestroy.x2 = m_corBtnDestroy.x1 + BUTTON_WIDTH;
   m_corBtnDestroy.y2 = m_corBtnDestroy.y1 + BUTTON_HEIGHT;
   
   //--- create
   if(!m_btnDestroy.Create(m_chart_id,m_name+"BtnDestroy",m_subwin, m_corBtnDestroy.x1, m_corBtnDestroy.y1, m_corBtnDestroy.x2, m_corBtnDestroy.y2))
      return(false);

   if(!m_btnDestroy.Text("Remove"))
      return(false);

   if(!Add(m_btnDestroy))
      return(false);

   m_btnDestroy.Alignment(WND_ALIGN_RIGHT,0,0,INDENT_RIGHT,0);

   return(true);
}


//+------------------------------------------------------------------+
//| Create the "RadioGroup" element                                  |
//+------------------------------------------------------------------+
//bool CPanelDialog::CreateRadioGroup(void)
//  {
//   int sx=(ClientAreaWidth()-(INDENT_LEFT+INDENT_RIGHT+BUTTON_WIDTH))/3-CONTROLS_GAP_X;
////--- coordinates
//   int x1=INDENT_LEFT;
//   int y1=INDENT_TOP+EDIT_HEIGHT+CONTROLS_GAP_Y;
//   int x2=x1+sx;
//   int y2=ClientAreaHeight()-INDENT_BOTTOM;
////--- create
//   if(!m_radio_group.Create(m_chart_id,m_name+"RadioGroup",m_subwin,x1,y1,x2,y2))
//      return(false);
//   if(!Add(m_radio_group))
//      return(false);
//   m_radio_group.Alignment(WND_ALIGN_HEIGHT,0,y1,0,INDENT_BOTTOM);
////--- fill out with strings
//   for(int i=0;i<4;i++)
//      if(!m_radio_group.AddItem("Item "+IntegerToString(i),1<<i))
//         return(false);
////--- succeed
//   return(true);
//  }

//+------------------------------------------------------------------+
//| Create the "ListView" element                                    |
//+------------------------------------------------------------------+
//bool CPanelDialog::CreateListView_SL(void)
//{
//   int sx=(ClientAreaWidth()-(INDENT_LEFT+INDENT_RIGHT+BUTTON_WIDTH))/3-CONTROLS_GAP_X;
//
//   m_cordiSL.x1=m_cordiCheckBox.x2 + CONTROLS_GAP_X;
//   m_cordiSL.y1=INDENT_TOP+EDIT_HEIGHT+CONTROLS_GAP_Y;
//   m_cordiSL.x2=x1+sx;
//   m_cordiSL.y2=ClientAreaHeight()-INDENT_BOTTOM;
//   
//   //--- create
//   if(!m_lstSL.Create(m_chart_id,m_name+"ListView",m_subwin,m_cordiSL.x1,m_cordiSL.y1,m_cordiSL.x2,m_cordiSL.y2))
//      return(false);
//      
//   if(!Add(m_lstSL))
//      return(false);
//      
//   m_lstSL.Alignment(WND_ALIGN_HEIGHT,0,y1,0,INDENT_BOTTOM);
//
//   ////--- fill out with strings
//   //for(int i=0;i<16;i++)
//   //   if(!m_lstSL.ItemAdd("Item "+IntegerToString(i)))
//   //      return(false);
//
//
//   return(true);
//}
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
void CPanelDialog::OnClickBtnSave(void)
{
   //TODO m_edit.Text(__FUNCTION__);
}
//+------------------------------------------------------------------+
//| Event handler                                                    |
//+------------------------------------------------------------------+
void CPanelDialog::OnClickBtnReset(void)
{
   for( int k=0; k<ArraySize(m_edit); k++)
      m_edit[k].Text("");
}

void CPanelDialog::OnClickBtnDestroy(void)
{
   RemoveEdit();
}


//+------------------------------------------------------------------+
//| Event handler                                                    |
//+------------------------------------------------------------------+
//void CPanelDialog::OnClickButton3(void)
//  {
//   if(m_button3.Pressed())
//      m_edit.Text(__FUNCTION__+"On");
//   else
//      m_edit.Text(__FUNCTION__+"Off");
//  }
//+------------------------------------------------------------------+
//| Event handler                                                    |
//+------------------------------------------------------------------+
//void CPanelDialog::OnChangeListView(void)
//  {
//   m_edit.Text(__FUNCTION__+" \""+m_lstSL.Select()+"\"");
//  }
//+------------------------------------------------------------------+
//| Event handler                                                    |
//+------------------------------------------------------------------+
//void CPanelDialog::OnChangeRadioGroup(void)
//  {
//   m_edit.Text(__FUNCTION__+" : Value="+IntegerToString(m_radio_group.Value()));
//  }
//+------------------------------------------------------------------+
//| Event handler                                                    |
//+------------------------------------------------------------------+
//void CPanelDialog::OnChangeCheckGroup(void)
//  {
//   m_edit.Text(__FUNCTION__+" : Value="+IntegerToString(m_check_group.Value()));
//  }
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
