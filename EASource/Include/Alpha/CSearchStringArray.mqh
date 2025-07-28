#ifndef __SEARCH_STRING_ARRAY_H__
#define __SEARCH_STRING_ARRAY_H__


#include "_IncCommon.mqh"

class CSearchStringArray
{
public:
   CSearchStringArray(){};
   ~CSearchStringArray(){};

public:
   void AddNewValue(string sInput);
   bool FindSameData(string sInput);
   void PrintAll();
   
private:
   bool search(_InOut_ int& begin, _InOut_ int& end, _InOut_ int& size, _In_ string sInput);
private:
   string   m_arr[];   
   
};


bool CSearchStringArray::FindSameData(string sInput)
{
   int begin = 0;
   int end = ArraySize(m_arr) - 1;
   int size = end - begin + 1;

   for( int i=0; i<ArraySize(m_arr); i++)
   {
      bool bFind = search(begin, end, size, sInput);

      if(bFind) 
         return true;
         
         
      if( size==1 )
      {
         if( m_arr[begin]==sInput )
            return true;
            
         break;
      }
   }
   
   return false;
}

bool CSearchStringArray::search(_InOut_ int& begin, _InOut_ int& end, _InOut_ int& size, _In_ string sInput)
{
   int mid = begin + (size / 2);
   
   int comp = StringCompare( m_arr[mid], sInput );
   if( comp==0 )
      return true;
      
   if( comp > 0 )
   {
      begin = begin;
      end = mid - 1;
      size = end - begin + 1;
   }
   else if( comp < 0 )
   {
      begin = mid + 1;
      end = end;
      size = end - begin + 1;
   }

   if( begin <0 || end < 0 || begin>end || size<=0 )
      return false;


   return false;
}


void CSearchStringArray::AddNewValue(string sInput)
{
   int nowArraySize = ArraySize(m_arr);
   if( nowArraySize==0 )
   {
      ArrayResize(m_arr, 1);
      m_arr[0] = sInput;
      return;
   }
   
   string newArray[]; 
   
   
   int pos = 0;
   bool bAlreadyInserted = false;
   for( int i=0; i<nowArraySize; i++ )
   {
      if(bAlreadyInserted)
      {
         ArrayResize(newArray, ArraySize(newArray)+1);
         newArray[pos++] = m_arr[i];
         continue;
      }
      
      int comp = StringCompare(m_arr[i], sInput, true);
      
      if( comp > 0 )
      {
         ArrayResize(newArray, ArraySize(newArray)+2);
         
         newArray[pos] = sInput;
         pos++;

         newArray[pos] = m_arr[i];
         pos++;
         
         bAlreadyInserted = true;
      }
      else if( comp==0 )
      {
         continue;
      }
      else
      {
         ArrayResize(newArray, ArraySize(newArray)+1);
         newArray[pos] = m_arr[i];
         pos++;
      }
   }  

   if(!bAlreadyInserted )
   {
      ArrayResize(newArray, ArraySize(newArray)+1);
      newArray[pos] = sInput;
   }
      
   ArrayFree(m_arr);
   ArrayResize(m_arr, ArraySize(newArray));
   ArrayCopy(m_arr, newArray);   
}


void CSearchStringArray::PrintAll()
{
   string buffer;
   for( int i=0; i<ArraySize(m_arr); i++ )
   {
      buffer += (m_arr[i] + ", ");
   }
   Print(buffer);
}



#endif