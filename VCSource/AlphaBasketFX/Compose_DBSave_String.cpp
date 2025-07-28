#include "Inc.h"
#include "Compose_DBSave_String.h"



/*

create Procedure [dbo].[AlphaBasket_Save_OpenTriggered]
    @ISymbol int
    ,@Symbol varchar(20)
    ,@OpenGap int
    ,@OpenThreshold int

    ,@B_BrokerKey varchar(20)
    ,@B_OpenPrcTriggered varchar(20)
    ,@B_OpenOppPrc varchar(20)
    ,@B_Lots decimal(5,2)

    ,@S_BrokerKey varchar(20)
    ,@S_OpenPrcTriggered varchar(20)
    ,@S_OpenOppPrc varchar(20)
    ,@S_Lots decimal(5,2)
    
    */
void AlphaBasket_Save_OpenTriggered(int iSymbol, _In_ TData* pData, _Out_ char* pzBuffer)
{
    sprintf(pzBuffer,
        "AlphaBasket_Save_OpenTriggered "
        "%d"    //@ISymbol int
        ",'%s'"   // @Symbol varchar(20)
        ",%d"   // @OpenGap int
        ",%d"   // @OpenThreshold int

        ",'%s'"   // @B_BrokerKey varchar(20)
        ",'%s'"   // @B_OpenPrcTriggered varchar(20)
        ",'%s'"   // @B_OpenOppPrc varchar(20)
        ",%.2f"   // @B_Lots decimal(5, 2)

        ",'%s'"   // @S_BrokerKey varchar(20)
        ",'%s'"   // @S_OpenPrcTriggered varchar(20)
        ",'%s'"   // @S_OpenOppPrc varchar(20)
        ",%.2f"     // @S_Lots decimal(5, 2)
        ",%ld"      // magic no

        ",'%s'"     // @B_MDTime
        ",'%s'"     // @S_MDTime

        , iSymbol
        , pData->symbol
        , pData->BestPrcForOpen.nGapBidAsk
        , pData->Spec.nThresholdOpenPt

        , pData->Long.zBrokerKey
        , pData->Long.zOpenPrc_Triggered
        , pData->Long.zOpenOppPrc
        , pData->Long.dLots

        , pData->Short.zBrokerKey
        , pData->Short.zOpenPrc_Triggered
        , pData->Short.zOpenOppPrc
        , pData->Short.dLots
        , pData->lMagicNo

        , pData->Long.zMDTime_OpenPrcTriggered
        , pData->Short.zMDTime_OpenPrcTriggered
    );
}


/*

ALTER Procedure [dbo].[AlphaBasket_Save_OpenMT4]
    @sSerial	VARCHAR(18)
    ,@B_OpenTimeMT4 varchar(20)
    ,@B_OpenPrc varchar(20)
    ,@B_OpenSlippage int
    ,@B_Ticket varchar(20)

    ,@S_OpenTimeMT4 varchar(20)
    ,@S_OpenPrc varchar(20)
    ,@S_OpenSlippage int
    ,@S_Ticket varchar(20)
*/
void AlphaBasket_Save_OpenMT4(int iSymbol, _In_ TData* pData, _Out_ char* pzBuffer)
{
    sprintf(pzBuffer,
        "AlphaBasket_Save_OpenMT4 "
        " '%s'"   //@sSerial	VARCHAR(18)
        ",%d"
        ",'%s'"   // @B_OpenTimeMT4 varchar(20)
        ",'%s'"   // @B_OpenPrc varchar(20)
        ",%d"   // @B_OpenSlippage int
        ",'%s'"   // @B_Ticket varchar(20)

        ",'%s'"   // @S_OpenTimeMT4 varchar(20)
        ",'%s'"   // @S_OpenPrc varchar(20)
        ",%d"   // @S_OpenSlippage int
        ",'%s'"   // @S_Ticket varchar(20)

        , pData->zDBSerial
        , pData->nOrdStatus
        , pData->Long.zOpenTmMT4
        , pData->Long.zOpenPrc
        , pData->Long.nOpenSlippage
        , pData->Long.zTicket

        , pData->Short.zOpenTmMT4
        , pData->Short.zOpenPrc
        , pData->Short.nOpenSlippage
        , pData->Short.zTicket
    );
}


/*

create Procedure [dbo].[AlphaBasket_Save_CloseTriggered]
    @sSerial	VARCHAR(18)
    ,@CloseNetPL int
    ,@CloseThreshold int
    ,@B_ClosePrcTriggered varchar(20)
    ,@S_ClosePrcTriggered varchar(20) 
    
    AlphaBasket_Save_CloseTriggered  '202105100000000291',4,0,35,,
*/
void AlphaBasket_Save_CloseTriggered(int iSymbol, _In_ TData* pData, BOOL bMarketClose, _Out_ char* pzBuffer)
{
    char cMarketCloseYN = (bMarketClose) ? 'Y' : 'N';
    sprintf(pzBuffer,
        "AlphaBasket_Save_CloseTriggered "
        " '%s'"
        ",%d"
        ",%d"       // CloseNetPLTriggered int
        ",%d"       // CloseThreshold
        ",'%s'"     // B_ClosePrcTriggered varchar(20)
        ",'%s'"     // @S_ClosePrcTriggered varchar(20) 
        ",'%c'"

        ",'%s'"     // @B_MDTime
        ",'%s'"     // @S_MDTime

        , pData->zDBSerial
        , pData->nOrdStatus
        , pData->nCloseNetPLTriggered
        , pData->Spec.nThresholdClosePt
        , pData->Long.zClosePrc_Triggered
        , pData->Short.zClosePrc_Triggered
        , cMarketCloseYN

        , pData->Long.zMDTime_ClosePrcTriggered
        , pData->Short.zMDTime_ClosePrcTriggered
    );
}


/*

create Procedure [dbo].[AlphaBasket_Close_MT4]
        @sSerial	VARCHAR(18)
	,@OrdStatus	int
	,@B_CloseTimeMT4 varchar(20) 
	,@B_ClosePrc varchar(20) 
	,@B_CloseSlippage int 
	,@B_Cmsn	decimal(10, 2) 
	,@B_ClosePL decimal(10, 2) 
	,@B_ClosePLPt int

	,@S_CloseTimeMT4 varchar(20) 
	,@S_ClosePrc varchar(20) 
	,@S_CloseSlippage int 
	,@S_Cmsn	decimal(10, 2) 
	,@S_ClosePL decimal(10, 2) 
	,@S_ClosePLPt int

    AlphaBasket_Save_OpenMT4  '202105060000000073',3,'','0001.39038',0,'','2021.05.06 08:19:31','0001.39042',-1,'177941669'
*/
void AlphaBasket_Save_CloseMT4(int iSymbol, _In_ TData* pData, _Out_ char* pzBuffer)
{
    sprintf(pzBuffer,
        "AlphaBasket_Save_CloseMT4 "
        " '%s'"     // sSerial
        ",%d"
        ",'%s'"     // @B_CloseTimeMT4 varchar(20)
        ",'%s'"     // @B_ClosePrc varchar(20)
        ",%d"       // @B_CloseSlippage int
        ",%.2f"     // @B_Cmsn
        ",%.2f"     //, @B_ClosePL decimal(10, 2)
        ",%d"
        ",%.2f"     // swap

        ",'%s'"     // @S_CloseTimeMT4 varchar(20)
        ",'%s'"     // @S_ClosePrc varchar(20)
        ",%d"       // @S_CloseSlippage int
        ",%.2f"     // @S_Cmsn
        ",%.2f"     // @S_ClosePL decimal(10, 2)
        ",%d"
        ",%.2f"     // swap

        , pData->zDBSerial
        , pData->nOrdStatus

        , pData->Long.zCloseTmMT4
        , pData->Long.zClosePrc
        , pData->Long.nCloseSlippage
        , pData->Long.dCmsn
        , pData->Long.dPL
        , pData->Long.nPLPt
        , pData->Long.dSwap

        , pData->Short.zCloseTmMT4
        , pData->Short.zClosePrc
        , pData->Short.nCloseSlippage
        , pData->Short.dCmsn
        , pData->Short.dPL
        , pData->Short.nPLPt
        , pData->Short.dSwap
    );
}


// pzOpenClose : OPEN / CLOSE 
void AlphaBasket_Error(int iSymbol, char cBuySellTp, _In_ TData* pData, _In_ char* pzErrMsg, _Out_ char* pzBuffer)
{

    sprintf(pzBuffer,
        "AlphaBasket_Error "
        " '%s'"     // sSerial
        ",'%c'"
        ",'%s'"

        , pData->zDBSerial
        , cBuySellTp
        , pzErrMsg
    );
}