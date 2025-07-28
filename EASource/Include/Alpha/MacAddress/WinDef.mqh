#ifdef __MQL5__
   #ifdef _WIN64
      #define PVOID ulong
      #define ptrdiff_t long
   #else
      #define PVOID uint
      #define ptrdiff_t int
   #endif
#else
   #define PVOID uint
   #define ptrdiff_t int
#endif

#define LPCVOID const PVOID
#define LPVOID PVOID
#define ULONG_PTR PVOID
#define HMODULE PVOID
#define HANDLE PVOID
#define HWND PVOID
#define size_t PVOID
#define SIZE_T PVOID

#define BOOL int
#define FALSE 0
#define TRUE  1

#ifndef BYTE 
#define BYTE uchar
#endif 

#ifndef WORD 
#define WORD ushort
#endif 

#ifndef DWORD 
#define DWORD uint
#endif 

#define UINT uint
#define LONG int
#define ULONG uint


#define CHAR char
#define WCHAR ushort
#define wchar_t ushort