#include "ComObjects.mqh"

#define CLSCTX_INPROC_SERVER 1

#define RPC_C_AUTHN_LEVEL_DEFAULT 0
#define RPC_C_IMP_LEVEL_IMPERSONATE 3
#define EOAC_NONE 0
#define RPC_C_AUTHZ_NONE 0
#define RPC_C_AUTHN_WINNT 10
#define RPC_C_AUTHN_LEVEL_CALL 3
#define WBEM_INFINITE (0xFFFFFFFF)

enum tag_WBEM_GENERIC_FLAG_TYPE
{
   WBEM_FLAG_RETURN_IMMEDIATELY	= 0x10,
   WBEM_FLAG_RETURN_WBEM_COMPLETE	= 0,
   WBEM_FLAG_BIDIRECTIONAL	= 0,
   WBEM_FLAG_FORWARD_ONLY	= 0x20,
   WBEM_FLAG_NO_ERROR_OBJECT	= 0x40,
   WBEM_FLAG_RETURN_ERROR_OBJECT	= 0,
   WBEM_FLAG_SEND_STATUS	= 0x80,
   WBEM_FLAG_DONT_SEND_STATUS	= 0,
   WBEM_FLAG_ENSURE_LOCATABLE	= 0x100,
   WBEM_FLAG_DIRECT_READ	= 0x200,
   WBEM_FLAG_SEND_ONLY_SELECTED	= 0,
   WBEM_RETURN_WHEN_COMPLETE	= 0,
   WBEM_RETURN_IMMEDIATELY	= 0x10,
   WBEM_MASK_RESERVED_FLAGS	= 0x1f000,
   WBEM_FLAG_USE_AMENDED_QUALIFIERS	= 0x20000,
   WBEM_FLAG_STRONG_VALIDATION	= 0x100000
} WBEM_GENERIC_FLAG_TYPE;



#import "Ole32.dll"
   HRESULT CoInitializeSecurity( PVOID pSecDesc, int cAuthSvc, PVOID asAuthSvc, PVOID pReserved1, uint dwAuthnLevel, uint dwImpLevel, PVOID pAuthList, uint dwCapabilities, PVOID pReserved3 );
   HRESULT CoSetProxyBlanket( PVOID pProxy, uint dwAuthnSvc, uint dwAuthzSvc, PVOID pServerPrincName, uint dwAuthnLevel, uint dwImpLevel, PVOID pAuthInfo, uint dwCapabilities );
#import


CLSID CLSID_WbemLocator = { 0x4590f811, 0x1d3a, 0x11d0, { 0x89, 0x1f, 0x00, 0xaa, 0x00, 0x4b, 0x2e, 0x24 } };
GUID IID_IWbemLocator = { 0xdc12a687, 0x737f, 0x11cf, { 0x88, 0x4d, 0x00, 0xaa, 0x00, 0x4b, 0x2e, 0x24 } };
//-------------------------------------------------------------------------------------------------
struct lpWbemClassObject : public LPUNKNOWN
{
   HRESULT Get( string wszName, Variant& pVal );
   HRESULT BeginEnumeration();
   HRESULT Next( string& strName, Variant& pVal );
};
//-------------------------------------------------------------------------------------------------
struct lpEnumWbemClassObject : public LPUNKNOWN
{
   HRESULT Next( lpWbemClassObject& apObjects, uint& puReturned );
};
//-------------------------------------------------------------------------------------------------
struct lpWbemServices : public LPUNKNOWN
{
   HRESULT ExecQuery( string strQuery, lpEnumWbemClassObject& ppEnum );
};
//-------------------------------------------------------------------------------------------------
struct lpWbemLocator : public LPUNKNOWN
{
   HRESULT ConnectServer( string strNetworkResource, string strUser, string strPassword, string strLocale, lpWbemServices& ppNamespace );
};
//-------------------------------------------------------------------------------------------------
// Implementation
//-------------------------------------------------------------------------------------------------
// Struct lpWbemClassObject
//-------------------------------------------------------------------------------------------------
HRESULT lpWbemClassObject::Get( string wszName, Variant& pVal )
{
   PVOID pVarParams[5];
   ushort prgvt[5];
   
   Variant Data0( wszName );
   pVarParams[0] = Data0.Address();
   prgvt[0] = Data0.Type();
   
   Variant Data1( 0 );
   pVarParams[1] = Data1.Address();
   prgvt[1] = Data1.Type();
   
   Variant Data2 = Variant::ByRef( pVal );
   pVarParams[2] = Data2.Address();
   prgvt[2] = Data2.Type();
   
   Variant Data3( VT_NULL, 0 );
   pVarParams[3] = Data3.Address();
   prgvt[3] = Data3.Type();
   
   Variant Data4( VT_NULL, 0 );
   pVarParams[4] = Data4.Address();
   prgvt[4] = Data4.Type();
   
   Variant varResult;
   HRESULT result = DispCallFunc( mInterface, VTABLEOFFSET(4), CC_STDCALL, VT_EMPTY, 5, prgvt, pVarParams, varResult.Address() );
   if( result == S_OK ){
      return varResult.toUInt();
   }
   return result;
}
//-------------------------------------------------------------------------------------------------
HRESULT lpWbemClassObject::BeginEnumeration()
{
   PVOID pVarParams[1];
   ushort prgvt[1];
   
   Variant Data0( 0 );
   pVarParams[0] = Data0.Address();
   prgvt[0] = Data0.Type();
   
   Variant varResult;
   HRESULT result = DispCallFunc( mInterface, VTABLEOFFSET(8), CC_STDCALL, VT_EMPTY, 1, prgvt, pVarParams, varResult.Address() );
   if( result == S_OK ){
      return varResult.toUInt();
   }
   return result;
}
//-------------------------------------------------------------------------------------------------
HRESULT lpWbemClassObject::Next( string& strName, Variant& pVal )
{
   PVOID pVarParams[5];
   ushort prgvt[5];
   
   Variant Data0( 0 );
   pVarParams[0] = Data0.Address();
   prgvt[0] = Data0.Type();
   
   BSTR pName = 0;
   
   Variant Data1 = Variant::ByRef( pName );
   pVarParams[1] = Data1.Address();
   prgvt[1] = Data1.Type();
   
   Variant Data2 = Variant::ByRef( pVal );
   pVarParams[2] = Data2.Address();
   prgvt[2] = Data2.Type();
   
   Variant Data3( VT_NULL, 0 );
   pVarParams[3] = Data3.Address();
   prgvt[3] = Data3.Type();
   
   Variant Data4( VT_NULL, 0 );
   pVarParams[4] = Data4.Address();
   prgvt[4] = Data4.Type();
   
   Variant varResult;
   HRESULT result = DispCallFunc( mInterface, VTABLEOFFSET(9), CC_STDCALL, VT_EMPTY, 5, prgvt, pVarParams, varResult.Address() );
   if( result != S_OK ){
      return result;
   }
   
   uint strLen = SysStringLen( pName ) + 1;
   if( strLen > 1 ){
      ushort shortArray[];
      if( ArrayResize( shortArray, strLen ) == strLen ){
         uint bytes = strLen * 2;
         if( memcpy( shortArray, pName, bytes ) != 0 ){
            strName = ShortArrayToString( shortArray );
         }
         ArrayFree( shortArray );
      }
   }
   
   SysFreeString( pName );
   return varResult.toUInt();
}
//-------------------------------------------------------------------------------------------------
// Struct lpEnumWbemClassObject
//-------------------------------------------------------------------------------------------------
HRESULT lpEnumWbemClassObject::Next( lpWbemClassObject& apObjects, uint& puReturned )
{
   PVOID pVarParams[4];
   ushort prgvt[4];
   
   Variant Data0( WBEM_INFINITE );
   pVarParams[0] = Data0.Address();
   prgvt[0] = Data0.Type();
   
   Variant Data1( 1 );
   pVarParams[1] = Data1.Address();
   prgvt[1] = Data1.Type();
   
   Variant Data2 = Variant::ByRef( apObjects );
   pVarParams[2] = Data2.Address();
   prgvt[2] = Data2.Type();
   
   Variant Data3 = Variant::ByRef( puReturned );
   pVarParams[3] = Data3.Address();
   prgvt[3] = Data3.Type();
   
   Variant varResult;
   HRESULT result = DispCallFunc( mInterface, VTABLEOFFSET(4), CC_STDCALL, VT_EMPTY, 4, prgvt, pVarParams, varResult.Address() );
   if( result == S_OK ){
      return varResult.toUInt();
   }
   return result;
}
//-------------------------------------------------------------------------------------------------
// Struct lpWbemServices
//-------------------------------------------------------------------------------------------------
HRESULT lpWbemServices::ExecQuery( string strQuery, lpEnumWbemClassObject& ppEnum )
{
   PVOID pVarParams[5];
   ushort prgvt[5];
   
   int lFlags = WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY;
   
   Variant Data0( "WQL" );
   pVarParams[0] = Data0.Address();
   prgvt[0] = Data0.Type();

   Variant Data1( strQuery );
   pVarParams[1] = Data1.Address();
   prgvt[1] = Data1.Type();

   Variant Data2( lFlags );
   pVarParams[2] = Data2.Address();
   prgvt[2] = Data2.Type();
   
   Variant Data3( VT_NULL, 0 );
   pVarParams[3] = Data3.Address();
   prgvt[3] = Data3.Type();
   
   Variant Data4 = Variant::ByRef( ppEnum );
   pVarParams[4] = Data4.Address();
   prgvt[4] = Data4.Type();
   
   Variant varResult;
   HRESULT result = DispCallFunc( mInterface, VTABLEOFFSET(20), CC_STDCALL, VT_EMPTY, 5, prgvt, pVarParams, varResult.Address() );
   if( result == S_OK ){
      return varResult.toUInt();
   }
   return result;
}
//-------------------------------------------------------------------------------------------------
// Struct lpWbemLocator
//-------------------------------------------------------------------------------------------------
HRESULT lpWbemLocator::ConnectServer( string strNetworkResource, string strUser, string strPassword, string strLocale, lpWbemServices& ppNamespace )
{
   PVOID pVarParams[8];
   ushort prgvt[8];
   
   Variant Data0( strNetworkResource );
   pVarParams[0] = Data0.Address();
   prgvt[0] = Data0.Type();
   
   Variant Data1 = StringLen( strUser ) > 0 ? Variant( strUser ) : Variant( VT_NULL, 0 );
   pVarParams[1] = Data1.Address();
   prgvt[1] = Data1.Type();
   
   Variant Data2 = StringLen( strPassword ) > 0 ? Variant( strPassword ) : Variant( VT_NULL, 0 );
   pVarParams[2] = Data2.Address();
   prgvt[2] = Data2.Type();
   
   Variant Data3 = StringLen( strLocale ) > 0 ? Variant( strLocale ) : Variant( VT_NULL, 0 );
   pVarParams[3] = Data3.Address();
   prgvt[3] = Data3.Type();
   
   Variant Data4( VT_INT, 0 );
   pVarParams[4] = Data4.Address();
   prgvt[4] = Data4.Type();
   
   Variant Data5( VT_BSTR, 0 );
   pVarParams[5] = Data5.Address();
   prgvt[5] = Data5.Type();
   
   Variant Data6( VT_NULL, 0 );
   pVarParams[6] = Data6.Address();
   prgvt[6] = Data6.Type();
   
   Variant Data7 = Variant::ByRef( ppNamespace );
   pVarParams[7] = Data7.Address();
   prgvt[7] = Data7.Type();
   
   Variant varResult;
   HRESULT result = DispCallFunc( mInterface, VTABLEOFFSET(3), CC_STDCALL, VT_EMPTY, 8, prgvt, pVarParams, varResult.Address() );
   if( result == S_OK ){
      return varResult.toUInt();
   }
   return result;
}
//+------------------------------------------------------------------+