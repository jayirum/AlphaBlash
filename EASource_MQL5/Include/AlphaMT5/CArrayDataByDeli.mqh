#ifndef __CARRAYDATA_BY_DELI__
#define __CARRAYDATA_BY_DELI__
/*
   Manage the array data which is made from string composed of data and delimiter.
   
   ex) EURUSD,USDJPY,NZDJPY
       EURUSD/USDJPY/NZDJPY
*/

class CArrayDataByDeli
{
public:
   CArrayDataByDeli(){};
   ~CArrayDataByDeli(){};
   
   bool     Initialize(string sData, string sDeli);
   
   int      Count()        { return m_nArrCnt;  }
   string   Data(int idx)  { return m_arr[idx]; }
   
   void     PrintArray();
   
   string   operator[](const int idx) const { return m_arr[idx]; }
private:
   string   m_sOrgData;
   ushort   m_uDeli;
   int      m_nArrCnt;

   string   m_arr[];

};



bool CArrayDataByDeli::Initialize(string sData, string sDeli)
{
   // remove the last delimiter if it is located the end of the string
   // EURUSD,USDJPY,NZDJPY,   ==> last ,
   int nLen = StringLen(sData);
   if( StringSubstr(sData, nLen-1)==sDeli )
      m_sOrgData = StringSubstr(sData, 0, nLen-1);
   else
      m_sOrgData = sData;
      
      
   // trim the right space
   m_sOrgData = StringTrimRight(m_sOrgData);
   
   // check if sData doesn't have any delimiter
   if( StringFind(m_sOrgData, sDeli)<0 )
   {
      m_nArrCnt = 1;
      ArrayResize(m_arr,1);
      m_arr[0] = m_sOrgData;
   }
   else
   {
      m_uDeli     = StringGetChar(sDeli,0);
      
      m_nArrCnt = StringSplit(m_sOrgData, m_uDeli, m_arr);
   }
   
   PrintArray();
   return (m_nArrCnt>0);
}


void CArrayDataByDeli::PrintArray()
{
   PrintFormat("[PrintArray](cnt:%d) Original(%s)", m_nArrCnt, m_sOrgData);
   string buf;
   for( int i=0; i<m_nArrCnt; i++ )
   {
      buf += "["+m_arr[i] + "]";
   }
   PrintFormat("[PrintArray] Array(%s)", buf);
}


#endif