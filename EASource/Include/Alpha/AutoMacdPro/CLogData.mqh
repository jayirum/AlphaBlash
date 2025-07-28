#ifndef __CLOG_DATA_H__
#define __CLOG_DATA_H__


class CLogData
{
public:
   CLogData(){};
   ~CLogData(){};
   
   bool  OpenFile(string sFileName, bool& bAlreadyExists);
   bool  logData(string sData);
   string   GetMsg() { return m_sMsg; }
private:
   int      m_fd;
   string   m_fileName;
   string   m_sMsg;
};




bool CLogData::OpenFile(string sFileName, bool& bAlreadyExists)
{
   m_fileName = sFileName;
   
   bAlreadyExists = false;  

   if( FileIsExist(m_fileName) )
   {
      bAlreadyExists = true;
      FileClose(m_fd);
   }

   m_fd = FileOpen(m_fileName
                    ,FILE_READ | FILE_WRITE | FILE_CSV | FILE_ANSI | FILE_SHARE_READ
                    );
   if( m_fd==INVALID_HANDLE)
   {
      m_sMsg = StringFormat("open file failed:%s, %d", m_fileName, GetLastError());
      return false;
   }
 
   if(!FileSeek(m_fd, 0, SEEK_END)) 
   {
      m_sMsg = StringFormat("FileSeek failed:%s, %d", m_fileName, GetLastError());
      return false;
   }
   
   return true;
}


bool  CLogData::logData(string sData)
{
   int ret = (int)FileWriteString(m_fd , sData);
   if(ret>0 )
     FileFlush(m_fd);
   else
      PrintFormat("Write data Error:%d",GetLastError());
      
   return (ret>0);
}

#endif