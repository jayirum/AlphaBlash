#ifndef __SEARCH_INTEGER_ARRAY_H__
#define __SEARCH_INTEGER_ARRAY_H__


#include "_IncCommon.mqh"

class CSearchIntegerArray
{
public:
   CSearchIntegerArray(){};
   ~CSearchIntegerArray(){};

public:
   void AddNewValue(int nInput);
   bool FindSameData(int nInput);
   void PrintAll();
   
private:
   bool search(_InOut_ int& begin, _InOut_ int& end, _InOut_ int& size, _In_ int nInput);
private:
   int   m_arr[];   
   
};


bool CSearchIntegerArray::FindSameData(int nInput)
{
   int begin = 0;
   int end = ArraySize(m_arr) - 1;
   int size = end - begin + 1;

   for( int i=0; i<ArraySize(m_arr); i++)
   {
      bool bFind = search(begin, end, size, nInput);

      if(bFind) 
         return true;
         
      if( size==1 )
      {
         if( m_arr[begin]==nInput )
            return true;
            
         break;
      }
   }
   
   return false;
}

bool CSearchIntegerArray::search(_InOut_ int& begin, _InOut_ int& end, _InOut_ int& size, _In_ int nInput)
{
   int mid = begin + (size / 2);
   
   int comp = m_arr[mid] - nInput;
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


void CSearchIntegerArray::AddNewValue(int nInput)
{
   int nowArraySize = ArraySize(m_arr);
   if( nowArraySize==0 )
   {
      ArrayResize(m_arr, 1);
      m_arr[0] = nInput;
      return;
   }
   
   int newArray[]; 
   
   
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
      
      int comp = m_arr[i] - nInput;
      
      if( comp > 0 )
      {
         ArrayResize(newArray, ArraySize(newArray)+2);
         
         newArray[pos] = nInput;
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
      newArray[pos] = nInput;
   }
      
   ArrayFree(m_arr);
   ArrayResize(m_arr, ArraySize(newArray));
   ArrayCopy(m_arr, newArray);   
}


void CSearchIntegerArray::PrintAll()
{
   string buffer;
   for( int i=0; i<ArraySize(m_arr); i++ )
   {
      buffer += (IntegerToString(m_arr[i]) + ", ");
   }
   Print(buffer);
}



#endif