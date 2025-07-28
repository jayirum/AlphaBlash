#ifndef __CTELE_NOTI_H__
#define __CTELE_NOTI_H__


#include <Telegram.mqh>

class CTelegramNoti
{
public:
   CTelegramNoti();
   CTelegramNoti(string TeleToken, int TeleChatId);
   ~CTelegramNoti();
   
   void  SetTeleInfo(string TeleToken, int TeleChatId);
   void  SendAlarm(string sMsg);
   
private:
   CCustomBot     m_TeleBot;
   string         m_TeleToken;
   int            m_TeleChatId;

};

CTelegramNoti::CTelegramNoti()
{
}
CTelegramNoti::CTelegramNoti(string TeleToken, int TeleChatId)
{
   SetTeleInfo(TeleToken, TeleChatId);
}
CTelegramNoti::~CTelegramNoti()
{
}
   
void CTelegramNoti::SetTeleInfo(string TeleToken, int TeleChatId)
{
   m_TeleToken    = TeleToken;
   m_TeleChatId   = TeleChatId;
   
   m_TeleBot.Token(m_TeleToken);  
}

void CTelegramNoti::SendAlarm(string sMsg)
{
   m_TeleBot.SendMessage((long)m_TeleChatId, sMsg);
}


#endif