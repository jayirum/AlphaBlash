#pragma once


enum RET_SENTOSA
{
	ERR_SUCCESS						= 0
	, ERR_NO_USERID					= 1	//There is no user id
	, ERR_WRONG_PASSCODE			= 2	//Wrong passcode
	, ERR_ID_ALREADY_TAKEN			= 3	//UserId is already taken
	, ERR_EXCEED_MAX_EA				= 4	//Exceed max EA numbers
	, ERR_DEMO_DISALLOWED			= 5	//Your plan is allowing only demo account
	, ERR_INCORRECT_DATA			= 6	//Data is incorrect
	, ERR_EXCEPTION_DB				= 7	//Excption happened in DataBase
	, ERR_NO_RECORDSETS				= 8	//No recordsets
	, ERR_NO_RELAYSVR_INFO			= 9	//Can't find the RelayServer information
	, ERR_NO_VERSION_INFO			= 10	//No Version Info
	, ERR_NO_RET_CODE				= 11
	, ERR_DUALSESSION_DISALLOWED	= 12
	, ERR_NO_PACKET_CODE			= 13	// Wrong packet - No packet code
	, ERR_NOT_ALLOWED_PACKET		= 14
	, ERR_DBOPEN_ERROR				= 15	// Failed to connect DataBase
	, ERR_DB_EXEC					= 16	// Failed to execute DB
}
;