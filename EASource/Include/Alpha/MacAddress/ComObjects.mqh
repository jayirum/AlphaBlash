#include "Variant.mqh"

#define VARIANTARG VARIANT

//+-----------------------------------------------------------------------------------------------+
struct CLSID
{
   uint  Data1;
   ushort Data2;
   ushort Data3;
   uchar  Data4[8];
};
//+-----------------------------------------------------------------------------------------------+
class ComObject : public Variant
{
public:
   ComObject( const string objectName );
   ComObject( const CLSID& ClassId );
   ComObject( const Variant& var );

   bool isValid() const;

   Variant Function( const string Name );
   Variant Function( const string Name, const Variant& Var );
   Variant Function( const string Name, const Variant& Var1, const Variant& Var2 );
   Variant Function( const string Name, const Variant& Var1, const Variant& Var2, const Variant& Var3 );
   Variant Function( const string Name, const Variant& Var1, const Variant& Var2, const Variant& Var3, const Variant& Var4 );
   Variant Function( const string Name, const Variant& Var1, const Variant& Var2, const Variant& Var3, const Variant& Var4, const Variant& Var5 );
   Variant Function( const string Name, const Variant& Var1, const Variant& Var2, const Variant& Var3, const Variant& Var4, const Variant& Var5, const Variant& Var6 );
   Variant Function( const string Name, const Variant& Var1, const Variant& Var2, const Variant& Var3, const Variant& Var4, const Variant& Var5, const Variant& Var6, const Variant& Var7 );
   Variant Function( const string Name, const Variant& Var1, const Variant& Var2, const Variant& Var3, const Variant& Var4, const Variant& Var5, const Variant& Var6, const Variant& Var7, const Variant& Var8 );
   Variant Function( const string Name, const Variant& Var1, const Variant& Var2, const Variant& Var3, const Variant& Var4, const Variant& Var5, const Variant& Var6, const Variant& Var7, const Variant& Var8, const Variant& Var9 );
   Variant Function( const string Name, const Variant& Var1, const Variant& Var2, const Variant& Var3, const Variant& Var4, const Variant& Var5, const Variant& Var6, const Variant& Var7, const Variant& Var8, const Variant& Var9, const Variant& Var10 );
   Variant Function( const string Name, const Variant& Var1, const Variant& Var2, const Variant& Var3, const Variant& Var4, const Variant& Var5, const Variant& Var6, const Variant& Var7, const Variant& Var8, const Variant& Var9, const Variant& Var10, const Variant& Var11 );
   Variant Function( const string Name, const Variant& Var1, const Variant& Var2, const Variant& Var3, const Variant& Var4, const Variant& Var5, const Variant& Var6, const Variant& Var7, const Variant& Var8, const Variant& Var9, const Variant& Var10, const Variant& Var11, const Variant& Var12 );

   Variant GetProperty( const string Name ) const;
   Variant GetProperty( const string Name, const Variant& Var ) const;
   Variant GetProperty( const string Name, const Variant& Var1, const Variant& Var2 ) const;
   Variant GetProperty( const string Name, const Variant& Var1, const Variant& Var2, const Variant& Var3 ) const;
   Variant GetProperty( const string Name, const Variant& Var1, const Variant& Var2, const Variant& Var3, const Variant& Var4 ) const;
   Variant GetProperty( const string Name, const Variant& Var1, const Variant& Var2, const Variant& Var3, const Variant& Var4, const Variant& Var5 ) const;

   void SetProperty( const string Name, const Variant& Var );
};
//+-----------------------------------------------------------------------------------------------+
//  Implementation
//+-----------------------------------------------------------------------------------------------+
#define CLSCTX_SERVER 21
#define DISPATCH_METHOD         0x1
#define DISPATCH_PROPERTYGET    0x2
#define DISPATCH_PROPERTYPUT    0x4
#define DISPATCH_PROPERTYPUTREF 0x8
#define DISPID_PROPERTYPUT	    (-3)

const uint dispidNamed = DISPID_PROPERTYPUT;

#import "ole32.dll"
   HRESULT CoInitialize( LPVOID );
   void CoUninitialize();
   HRESULT CLSIDFromProgID( string, CLSID& classId );
   HRESULT CoCreateInstance( const CLSID& classId, PVOID, uint, GUID& riid, PINTERFACE& pInterface );
#import
//+-----------------------------------------------------------------------------------------------+
// Class ComObject
//+-----------------------------------------------------------------------------------------------+
ComObject::ComObject( const string objectName )
{
   CLSID ClassId;
   HRESULT result = CLSIDFromProgID( objectName, ClassId );
   if( result == S_OK ){
      LPDISPATCH dispatch = {0};
      result = CoCreateInstance( ClassId, 0, CLSCTX_SERVER, IID_IDispatch, dispatch.mInterface );
      if( result == S_OK ){
         Variant::operator=( dispatch );
         dispatch.Release();
         return;
      }
   }
   Variant varResult( VT_ERROR, result );
   Variant::operator=( varResult );
}
//+-----------------------------------------------------------------------------------------------+
ComObject::ComObject( const CLSID& ClassId )
{
   LPDISPATCH dispatch = {0};
   HRESULT result = CoCreateInstance( ClassId, 0, CLSCTX_SERVER, IID_IDispatch, dispatch.mInterface );
   if( result == S_OK ){
      Variant::operator=( dispatch );
      dispatch.Release();
      return;
   }
   Variant varResult( VT_ERROR, result );
   Variant::operator=( varResult );
}
//+-----------------------------------------------------------------------------------------------+
ComObject::ComObject( const Variant& var ) : Variant( var )
{
}
//+-----------------------------------------------------------------------------------------------+
bool ComObject::isValid() const
{
   return mData.vt == VT_DISPATCH;
}
//+-----------------------------------------------------------------------------------------------+
Variant ComObject::Function( const string Name )
{
   LPDISPATCH pDispatch = toDispatch();
   int Dispid = 0;
   HRESULT result = pDispatch.GetIDsOfNames( Name, Dispid );
   if( result == S_OK ){
      DISPPARAMS DispParams = { 0, 0, 0, 0 };
      Variant varResult;
      result = pDispatch.Invoke( Dispid, DISPATCH_METHOD, DispParams, varResult );
      if( result == S_OK ){
         pDispatch.Release();
         return varResult;
      }
   }
   pDispatch.Release();
   Variant varResult( VT_ERROR, result );
   return varResult;
}
//+-----------------------------------------------------------------------------------------------+
Variant ComObject::Function( const string Name, const Variant& Var )
{
   LPDISPATCH pDispatch = toDispatch();
   int Dispid = 0;
   HRESULT result = pDispatch.GetIDsOfNames( Name, Dispid );
   if( result == S_OK ){
      VARIANTARG args[1];
      args[0] = Variant::Data( Var );
      DISPPARAMS DispParams = { 0, 0, 1, 0 };
      DispParams.rgvarg = AddressOf( args[0] );
      Variant varResult;
      result = pDispatch.Invoke( Dispid, DISPATCH_METHOD, DispParams, varResult );
      if( result == S_OK ){
         pDispatch.Release();
         return varResult;
      }
   }
   pDispatch.Release();
   Variant varResult( VT_ERROR, result );
   return varResult;
}
//+-----------------------------------------------------------------------------------------------+
Variant ComObject::Function( const string Name, const Variant& Var1, const Variant& Var2 )
{
   LPDISPATCH pDispatch = toDispatch();
   int Dispid = 0;
   HRESULT result = pDispatch.GetIDsOfNames( Name, Dispid );
   if( result == S_OK ){
      VARIANTARG args[2];
      args[0] = Variant::Data( Var2 );
      args[1] = Variant::Data( Var1 );
      DISPPARAMS DispParams = { 0, 0, 2, 0 };
      DispParams.rgvarg = AddressOf( args[0] );
      Variant varResult;
      result = pDispatch.Invoke( Dispid, DISPATCH_METHOD, DispParams, varResult );
      if( result == S_OK ){
         pDispatch.Release();
         return varResult;
      }
   }
   pDispatch.Release();
   Variant varResult( VT_ERROR, result );
   return varResult;
}
//+-----------------------------------------------------------------------------------------------+
Variant ComObject::Function( const string Name, const Variant& Var1, const Variant& Var2, const Variant& Var3 )
{
   LPDISPATCH pDispatch = toDispatch();
   int Dispid = 0;
   HRESULT result = pDispatch.GetIDsOfNames( Name, Dispid );
   if( result == S_OK ){
      VARIANTARG args[3];
      args[0] = Variant::Data( Var3 );
      args[1] = Variant::Data( Var2 );
      args[2] = Variant::Data( Var1 );
      DISPPARAMS DispParams = { 0, 0, 3, 0 };
      DispParams.rgvarg = AddressOf( args[0] );
      Variant varResult;
      result = pDispatch.Invoke( Dispid, DISPATCH_METHOD, DispParams, varResult );
      if( result == S_OK ){
         pDispatch.Release();
         return varResult;
      }
   }
   pDispatch.Release();
   Variant varResult( VT_ERROR, result );
   return varResult;
}
//+-----------------------------------------------------------------------------------------------+
Variant ComObject::Function( const string Name, const Variant& Var1, const Variant& Var2, const Variant& Var3,
   const Variant& Var4 )
{
   LPDISPATCH pDispatch = toDispatch();
   int Dispid = 0;
   HRESULT result = pDispatch.GetIDsOfNames( Name, Dispid );
   if( result == S_OK ){
      VARIANTARG args[4];
      args[0] = Variant::Data( Var4 );
      args[1] = Variant::Data( Var3 );
      args[2] = Variant::Data( Var2 );
      args[3] = Variant::Data( Var1 );
      DISPPARAMS DispParams = { 0, 0, 4, 0 };
      DispParams.rgvarg = AddressOf( args[0] );
      Variant varResult;
      result = pDispatch.Invoke( Dispid, DISPATCH_METHOD, DispParams, varResult );
      if( result == S_OK ){
         pDispatch.Release();
         return varResult;
      }
   }
   pDispatch.Release();
   Variant varResult( VT_ERROR, result );
   return varResult;
}
//+-----------------------------------------------------------------------------------------------+
Variant ComObject::Function( const string Name, const Variant& Var1, const Variant& Var2, const Variant& Var3,
   const Variant& Var4, const Variant& Var5 )
{
   LPDISPATCH pDispatch = toDispatch();
   int Dispid = 0;
   HRESULT result = pDispatch.GetIDsOfNames( Name, Dispid );
   if( result == S_OK ){
      VARIANTARG args[5];
      args[0] = Variant::Data( Var5 );
      args[1] = Variant::Data( Var4 );
      args[2] = Variant::Data( Var3 );
      args[3] = Variant::Data( Var2 );
      args[4] = Variant::Data( Var1 );
      DISPPARAMS DispParams = { 0, 0, 5, 0 };
      DispParams.rgvarg = AddressOf( args[0] );
      Variant varResult;
      result = pDispatch.Invoke( Dispid, DISPATCH_METHOD, DispParams, varResult );
      if( result == S_OK ){
         pDispatch.Release();
         return varResult;
      }
   }
   pDispatch.Release();
   Variant varResult( VT_ERROR, result );
   return varResult;
}
//+-----------------------------------------------------------------------------------------------+
Variant ComObject::Function( const string Name, const Variant& Var1, const Variant& Var2, const Variant& Var3,
   const Variant& Var4, const Variant& Var5, const Variant& Var6 )
{
   LPDISPATCH pDispatch = toDispatch();
   int Dispid = 0;
   HRESULT result = pDispatch.GetIDsOfNames( Name, Dispid );
   if( result == S_OK ){
      VARIANTARG args[6];
      args[0] = Variant::Data( Var6 );
      args[1] = Variant::Data( Var5 );
      args[2] = Variant::Data( Var4 );
      args[3] = Variant::Data( Var3 );
      args[4] = Variant::Data( Var2 );
      args[5] = Variant::Data( Var1 );
      DISPPARAMS DispParams = { 0, 0, 6, 0 };
      DispParams.rgvarg = AddressOf( args[0] );
      Variant varResult;
      result = pDispatch.Invoke( Dispid, DISPATCH_METHOD, DispParams, varResult );
      if( result == S_OK ){
         pDispatch.Release();
         return varResult;
      }
   }
   pDispatch.Release();
   Variant varResult( VT_ERROR, result );
   return varResult;
}
//+-----------------------------------------------------------------------------------------------+
Variant ComObject::Function( const string Name, const Variant& Var1, const Variant& Var2, const Variant& Var3,
   const Variant& Var4, const Variant& Var5, const Variant& Var6, const Variant& Var7 )
{
   LPDISPATCH pDispatch = toDispatch();
   int Dispid = 0;
   HRESULT result = pDispatch.GetIDsOfNames( Name, Dispid );
   if( result == S_OK ){
      VARIANTARG args[7];
      args[0] = Variant::Data( Var7 );
      args[1] = Variant::Data( Var6 );
      args[2] = Variant::Data( Var5 );
      args[3] = Variant::Data( Var4 );
      args[4] = Variant::Data( Var3 );
      args[5] = Variant::Data( Var2 );
      args[6] = Variant::Data( Var1 );
      DISPPARAMS DispParams = { 0, 0, 7, 0 };
      DispParams.rgvarg = AddressOf( args[0] );
      Variant varResult;
      result = pDispatch.Invoke( Dispid, DISPATCH_METHOD, DispParams, varResult );
      if( result == S_OK ){
         pDispatch.Release();
         return varResult;
      }
   }
   pDispatch.Release();
   Variant varResult( VT_ERROR, result );
   return varResult;
}
//+-----------------------------------------------------------------------------------------------+
Variant ComObject::Function( const string Name, const Variant& Var1, const Variant& Var2, const Variant& Var3,
   const Variant& Var4, const Variant& Var5, const Variant& Var6, const Variant& Var7, const Variant& Var8 )
{
   LPDISPATCH pDispatch = toDispatch();
   int Dispid = 0;
   HRESULT result = pDispatch.GetIDsOfNames( Name, Dispid );
   if( result == S_OK ){
      VARIANTARG args[8];
      args[0] = Variant::Data( Var8 );
      args[1] = Variant::Data( Var7 );
      args[2] = Variant::Data( Var6 );
      args[3] = Variant::Data( Var5 );
      args[4] = Variant::Data( Var4 );
      args[5] = Variant::Data( Var3 );
      args[6] = Variant::Data( Var2 );
      args[7] = Variant::Data( Var1 );
      DISPPARAMS DispParams = { 0, 0, 8, 0 };
      DispParams.rgvarg = AddressOf( args[0] );
      Variant varResult;
      result = pDispatch.Invoke( Dispid, DISPATCH_METHOD, DispParams, varResult );
      if( result == S_OK ){
         pDispatch.Release();
         return varResult;
      }
   }
   pDispatch.Release();
   Variant varResult( VT_ERROR, result );
   return varResult;
}
//+-----------------------------------------------------------------------------------------------+
Variant ComObject::Function( const string Name, const Variant& Var1, const Variant& Var2, const Variant& Var3,
   const Variant& Var4, const Variant& Var5, const Variant& Var6, const Variant& Var7, const Variant& Var8,
   const Variant& Var9 )
{
   LPDISPATCH pDispatch = toDispatch();
   int Dispid = 0;
   HRESULT result = pDispatch.GetIDsOfNames( Name, Dispid );
   if( result == S_OK ){
      VARIANTARG args[9];
      args[0] = Variant::Data( Var9 );
      args[1] = Variant::Data( Var8 );
      args[2] = Variant::Data( Var7 );
      args[3] = Variant::Data( Var6 );
      args[4] = Variant::Data( Var5 );
      args[5] = Variant::Data( Var4 );
      args[6] = Variant::Data( Var3 );
      args[7] = Variant::Data( Var2 );
      args[8] = Variant::Data( Var1 );
      DISPPARAMS DispParams = { 0, 0, 9, 0 };
      DispParams.rgvarg = AddressOf( args[0] );
      Variant varResult;
      result = pDispatch.Invoke( Dispid, DISPATCH_METHOD, DispParams, varResult );
      if( result == S_OK ){
         pDispatch.Release();
         return varResult;
      }
   }
   pDispatch.Release();
   Variant varResult( VT_ERROR, result );
   return varResult;
}
//+-----------------------------------------------------------------------------------------------+
Variant ComObject::Function( const string Name, const Variant& Var1, const Variant& Var2, const Variant& Var3,
   const Variant& Var4, const Variant& Var5, const Variant& Var6, const Variant& Var7, const Variant& Var8,
   const Variant& Var9, const Variant& Var10 )
{
   LPDISPATCH pDispatch = toDispatch();
   int Dispid = 0;
   HRESULT result = pDispatch.GetIDsOfNames( Name, Dispid );
   if( result == S_OK ){
      VARIANTARG args[10];
      args[0] = Variant::Data( Var10 );
      args[1] = Variant::Data( Var9 );
      args[2] = Variant::Data( Var8 );
      args[3] = Variant::Data( Var7 );
      args[4] = Variant::Data( Var6 );
      args[5] = Variant::Data( Var5 );
      args[6] = Variant::Data( Var4 );
      args[7] = Variant::Data( Var3 );
      args[8] = Variant::Data( Var2 );
      args[9] = Variant::Data( Var1 );
      DISPPARAMS DispParams = { 0, 0, 10, 0 };
      DispParams.rgvarg = AddressOf( args[0] );
      Variant varResult;
      result = pDispatch.Invoke( Dispid, DISPATCH_METHOD, DispParams, varResult );
      if( result == S_OK ){
         pDispatch.Release();
         return varResult;
      }
   }
   pDispatch.Release();
   Variant varResult( VT_ERROR, result );
   return varResult;
}
//+-----------------------------------------------------------------------------------------------+
Variant ComObject::Function( const string Name, const Variant& Var1, const Variant& Var2, const Variant& Var3,
   const Variant& Var4, const Variant& Var5, const Variant& Var6, const Variant& Var7, const Variant& Var8,
   const Variant& Var9, const Variant& Var10, const Variant& Var11 )
{
   LPDISPATCH pDispatch = toDispatch();
   int Dispid = 0;
   HRESULT result = pDispatch.GetIDsOfNames( Name, Dispid );
   if( result == S_OK ){
      VARIANTARG args[11];
      args[0] = Variant::Data( Var11 );
      args[1] = Variant::Data( Var10 );
      args[2] = Variant::Data( Var9 );
      args[3] = Variant::Data( Var8 );
      args[4] = Variant::Data( Var7 );
      args[5] = Variant::Data( Var6 );
      args[6] = Variant::Data( Var5 );
      args[7] = Variant::Data( Var4 );
      args[8] = Variant::Data( Var3 );
      args[9] = Variant::Data( Var2 );
      args[10] = Variant::Data( Var1 );
      DISPPARAMS DispParams = { 0, 0, 11, 0 };
      DispParams.rgvarg = AddressOf( args[0] );
      Variant varResult;
      result = pDispatch.Invoke( Dispid, DISPATCH_METHOD, DispParams, varResult );
      if( result == S_OK ){
         pDispatch.Release();
         return varResult;
      }
   }
   pDispatch.Release();
   Variant varResult( VT_ERROR, result );
   return varResult;
}
//+-----------------------------------------------------------------------------------------------+
Variant ComObject::Function( const string Name, const Variant& Var1, const Variant& Var2, const Variant& Var3,
   const Variant& Var4, const Variant& Var5, const Variant& Var6, const Variant& Var7, const Variant& Var8,
   const Variant& Var9, const Variant& Var10, const Variant& Var11, const Variant& Var12 )
{
   LPDISPATCH pDispatch = toDispatch();
   int Dispid = 0;
   HRESULT result = pDispatch.GetIDsOfNames( Name, Dispid );
   if( result == S_OK ){
      VARIANTARG args[12];
      args[0] = Variant::Data( Var12 );
      args[1] = Variant::Data( Var11 );
      args[2] = Variant::Data( Var10 );
      args[3] = Variant::Data( Var9 );
      args[4] = Variant::Data( Var8 );
      args[5] = Variant::Data( Var7 );
      args[6] = Variant::Data( Var6 );
      args[7] = Variant::Data( Var5 );
      args[8] = Variant::Data( Var4 );
      args[9] = Variant::Data( Var3 );
      args[10] = Variant::Data( Var2 );
      args[11] = Variant::Data( Var1 );
      DISPPARAMS DispParams = { 0, 0, 12, 0 };
      DispParams.rgvarg = AddressOf( args[0] );
      Variant varResult;
      result = pDispatch.Invoke( Dispid, DISPATCH_METHOD, DispParams, varResult );
      if( result == S_OK ){
         pDispatch.Release();
         return varResult;
      }
   }
   pDispatch.Release();
   Variant varResult( VT_ERROR, result );
   return varResult;
}
//+-----------------------------------------------------------------------------------------------+
Variant ComObject::GetProperty( const string Name ) const
{
   LPDISPATCH pDispatch = toDispatch();
   int Dispid = 0;
   HRESULT result = pDispatch.GetIDsOfNames( Name, Dispid );
   if( result == S_OK ){
      DISPPARAMS DispParams = { 0, 0, 0, 0 };
      Variant varResult;
      result = pDispatch.Invoke( Dispid, DISPATCH_PROPERTYGET, DispParams, varResult );
      if( result == S_OK ){
         pDispatch.Release();
         return varResult;
      }
   }
   pDispatch.Release();
   Variant varResult( VT_ERROR, result );
   return varResult;
}
//+-----------------------------------------------------------------------------------------------+
Variant ComObject::GetProperty( const string Name, const Variant& Var ) const
{
   LPDISPATCH pDispatch = toDispatch();
   int Dispid = 0;
   HRESULT result = pDispatch.GetIDsOfNames( Name, Dispid );
   if( result == S_OK ){
      VARIANTARG args[1];
      args[0] = Variant::Data( Var );
      DISPPARAMS DispParams = { 0, 0, 1, 0 };
      DispParams.rgvarg = AddressOf( args[0] );
      Variant varResult;
      result = pDispatch.Invoke( Dispid, DISPATCH_PROPERTYGET, DispParams, varResult );
      if( result == S_OK ){
         pDispatch.Release();
         return varResult;
      }
   }
   pDispatch.Release();
   Variant varResult( VT_ERROR, result );
   return varResult;
}
//+-----------------------------------------------------------------------------------------------+
Variant ComObject::GetProperty( const string Name, const Variant& Var1, const Variant& Var2 ) const
{
   LPDISPATCH pDispatch = toDispatch();
   int Dispid = 0;
   HRESULT result = pDispatch.GetIDsOfNames( Name, Dispid );
   if( result == S_OK ){
      VARIANTARG args[2];
      args[0] = Variant::Data( Var2 );
      args[1] = Variant::Data( Var1 );
      DISPPARAMS DispParams = { 0, 0, 2, 0 };
      DispParams.rgvarg = AddressOf( args[0] );
      Variant varResult;
      result = pDispatch.Invoke( Dispid, DISPATCH_PROPERTYGET, DispParams, varResult );
      if( result == S_OK ){
         pDispatch.Release();
         return varResult;
      }
   }
   pDispatch.Release();
   Variant varResult( VT_ERROR, result );
   return varResult;
}
//+-----------------------------------------------------------------------------------------------+
Variant ComObject::GetProperty( const string Name, const Variant& Var1, const Variant& Var2, const Variant& Var3 ) const
{
   LPDISPATCH pDispatch = toDispatch();
   int Dispid = 0;
   HRESULT result = pDispatch.GetIDsOfNames( Name, Dispid );
   if( result == S_OK ){
      VARIANTARG args[3];
      args[0] = Variant::Data( Var3 );
      args[1] = Variant::Data( Var2 );
      args[2] = Variant::Data( Var1 );
      DISPPARAMS DispParams = { 0, 0, 3, 0 };
      DispParams.rgvarg = AddressOf( args[0] );
      Variant varResult;
      result = pDispatch.Invoke( Dispid, DISPATCH_PROPERTYGET, DispParams, varResult );
      if( result == S_OK ){
         pDispatch.Release();
         return varResult;
      }
   }
   pDispatch.Release();
   Variant varResult( VT_ERROR, result );
   return varResult;
}
//+-----------------------------------------------------------------------------------------------+
Variant ComObject::GetProperty( const string Name, const Variant& Var1, const Variant& Var2, const Variant& Var3,
   const Variant& Var4 ) const
{
   LPDISPATCH pDispatch = toDispatch();
   int Dispid = 0;
   HRESULT result = pDispatch.GetIDsOfNames( Name, Dispid );
   if( result == S_OK ){
      VARIANTARG args[4];
      args[0] = Variant::Data( Var4 );
      args[1] = Variant::Data( Var3 );
      args[2] = Variant::Data( Var2 );
      args[3] = Variant::Data( Var1 );
      DISPPARAMS DispParams = { 0, 0, 4, 0 };
      DispParams.rgvarg = AddressOf( args[0] );
      Variant varResult;
      result = pDispatch.Invoke( Dispid, DISPATCH_PROPERTYGET, DispParams, varResult );
      if( result == S_OK ){
         pDispatch.Release();
         return varResult;
      }
   }
   pDispatch.Release();
   Variant varResult( VT_ERROR, result );
   return varResult;
}
//+-----------------------------------------------------------------------------------------------+
Variant ComObject::GetProperty( const string Name, const Variant& Var1, const Variant& Var2, const Variant& Var3,
   const Variant& Var4, const Variant& Var5 ) const
{
   LPDISPATCH pDispatch = toDispatch();
   int Dispid = 0;
   HRESULT result = pDispatch.GetIDsOfNames( Name, Dispid );
   if( result == S_OK ){
      VARIANTARG args[5];
      args[0] = Variant::Data( Var5 );
      args[1] = Variant::Data( Var4 );
      args[2] = Variant::Data( Var3 );
      args[3] = Variant::Data( Var2 );
      args[4] = Variant::Data( Var1 );
      DISPPARAMS DispParams = { 0, 0, 5, 0 };
      DispParams.rgvarg = AddressOf( args[0] );
      Variant varResult;
      result = pDispatch.Invoke( Dispid, DISPATCH_PROPERTYGET, DispParams, varResult );
      if( result == S_OK ){
         pDispatch.Release();
         return varResult;
      }
   }
   pDispatch.Release();
   Variant varResult( VT_ERROR, result );
   return varResult;
}
//+-----------------------------------------------------------------------------------------------+
void ComObject::SetProperty( const string Name, const Variant& Var )
{
   LPDISPATCH pDispatch = toDispatch();
   int Dispid = 0;
   HRESULT result = pDispatch.GetIDsOfNames( Name, Dispid );
   if( result == S_OK ){
      VARIANTARG args[1];
      args[0] = Variant::Data( Var );
      DISPPARAMS DispParams = { 0, 0, 1, 1 };
      DispParams.rgvarg = AddressOf( args[0] );
      DispParams.rgdispidNamedArgs = AddressOf( dispidNamed );
      Variant varResult;
      result = pDispatch.Invoke( Dispid, DISPATCH_PROPERTYPUT, DispParams, varResult );
   }
   pDispatch.Release();
}
//+------------------------------------------------------------------+