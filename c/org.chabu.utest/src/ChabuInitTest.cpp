/*
 * ChabuInitTest.cpp
 *
 *  Created on: 29.06.2016
 *      Author: Frank
 */

#include "ChabuInitTest.hpp"
#include "gtest/gtest.h"
#include "FakeFunctions.h"
#include <Chabu.h>


void ChabuInitTest::SetUp(){
	FakeFunctions_ResetAll();

}


#define APPL_VERSION 0x1234
#define RPS          0x200
#define APPL_NAME    "ABC"


TEST_F( ChabuInitTest, LastError_set_for_chabu_NULL ){
	EXPECT_EQ( Chabu_ErrorCode_CHABU_IS_NULL, Chabu_LastError( NULL ));
}

TEST_F( ChabuInitTest, LastError_set_for_chabu_uninitialized ){
	struct Chabu_Data chabu;
	memset( &chabu, 0, sizeof(chabu));
	EXPECT_EQ( Chabu_ErrorCode_CHABU_IS_NOT_INITIALIZED, Chabu_LastError( &chabu ));
}

TEST_F( ChabuInitTest, LastError_set_for_chabu_null_error_func ){
	struct Chabu_Data chabu;
	Chabu_Init(
			&chabu,
			0, NULL, 0,
			NULL, 0,
			NULL, 0,
			NULL, NULL, NULL, NULL, NULL,
			NULL );
	EXPECT_EQ( Chabu_ErrorCode_INIT_ERROR_FUNC_NULL, Chabu_LastError( &chabu ));
}

TEST_F( ChabuInitTest, on_error_lastError_is_set ){

	struct Chabu_Data chabu;
	Chabu_Init(
			&chabu,
			0, APPL_NAME, RPS,
			NULL, 0,
			NULL, 0,
			errorFunction, NULL, NULL, NULL, NULL,
			NULL );
	EXPECT_EQ( Chabu_ErrorCode_INIT_PARAM_CHANNELS_NULL, Chabu_LastError( &chabu )) <<  Chabu_LastErrorStr( &chabu );

}

TEST_F( ChabuInitTest, on_error_error_func_is_called_once ){

	struct Chabu_Data chabu;
	Chabu_Init(
			&chabu,
			0, NULL, 0,
			NULL, 0,
			NULL, 0,
			errorFunction, NULL, NULL, NULL, NULL,
			NULL );
	EXPECT_EQ( 1u, errorFunction_fake.call_count );
}

#define VERIFY_ERROR_INFO( code, msg ) \
	do{\
		ASSERT_EQ( 1u, errorFunction_fake.call_count );\
		EXPECT_EQ( code, errorFunction_fake.arg1_val ) <<  Chabu_LastErrorStr( &chabu );\
		EXPECT_TRUE( strstr( errorFunction_fake.arg4_val, msg) != NULL ) << errorFunction_fake.arg4_val;\
	}while(false)

TEST_F( ChabuInitTest, application_protocol_name_is_null ){

	struct Chabu_Data chabu;
	Chabu_Init(
			&chabu,
			0, NULL, 0,
			NULL, 0,
			NULL, 0,
			errorFunction, NULL, NULL, NULL, NULL,
			NULL );
	EXPECT_EQ( Chabu_ErrorCode_INIT_PARAM_APNAME_NULL, Chabu_LastError( &chabu ));

}

TEST_F( ChabuInitTest, application_protocol_name_is_too_long ){

	struct Chabu_Data chabu;
	Chabu_Init(
			&chabu,
			0, "123456789012345678901234567890123456789012345678901234567890", 0,
			NULL, 0,
			NULL, 0,
			errorFunction, NULL, NULL, NULL, NULL,
			NULL );
	EXPECT_EQ( Chabu_ErrorCode_INIT_PARAM_APNAME_TOO_LONG, Chabu_LastError( &chabu ));

}

TEST_F( ChabuInitTest, rps_range_too_low ){

	struct Chabu_Data chabu;
	Chabu_Init(
			&chabu,
			0, APPL_NAME, 0x100-1,
			NULL, 0,
			NULL, 0,
			errorFunction, NULL, NULL, NULL, NULL,
			NULL );
	EXPECT_EQ( Chabu_ErrorCode_INIT_PARAM_RPS_RANGE, Chabu_LastError( &chabu ));

}
#define ASSERT_NO_ERROR() ASSERT_EQ( Chabu_ErrorCode_OK_NOERROR, Chabu_LastError( &chabu )) << Chabu_LastErrorStr( &chabu )

TEST_F( ChabuInitTest, rps_range_low ){

	struct Chabu_Data chabu;
	Chabu_Init(
			&chabu,
			0, APPL_NAME, 0x100,
			NULL, 0,
			NULL, 0,
			errorFunction, NULL, NULL, NULL, NULL,
			NULL );

	ASSERT_NE( Chabu_ErrorCode_INIT_PARAM_RPS_RANGE, Chabu_LastError( &chabu ));

}

TEST_F( ChabuInitTest, rps_range_too_high ){

	struct Chabu_Data chabu;
	Chabu_Init(
			&chabu,
			0, APPL_NAME, 0x10000000+1,
			NULL, 0,
			NULL, 0,
			errorFunction, NULL, NULL, NULL, NULL,
			NULL );
	EXPECT_EQ( Chabu_ErrorCode_INIT_PARAM_RPS_RANGE, Chabu_LastError( &chabu ));

}

TEST_F( ChabuInitTest, rps_range_high ){

	struct Chabu_Data chabu;
	Chabu_Init(
			&chabu,
			0, APPL_NAME, 0x10000000,
			NULL, 0,
			NULL, 0,
			errorFunction, NULL, NULL, NULL, NULL,
			NULL );

	ASSERT_NE( Chabu_ErrorCode_INIT_PARAM_RPS_RANGE, Chabu_LastError( &chabu ));

}

TEST_F( ChabuInitTest, channels_null ){

	struct Chabu_Data chabu;
	Chabu_Init(
			&chabu,
			0, APPL_NAME, RPS,
			NULL, 0,
			NULL, 0,
			errorFunction, NULL, NULL, NULL, NULL,
			NULL );

	VERIFY_ERROR_INFO( Chabu_ErrorCode_INIT_PARAM_CHANNELS_NULL, "channels must not be NULL");

}


TEST_F( ChabuInitTest, channels_count_zero ){

	struct Chabu_Data chabu;
	struct Chabu_Channel_Data channels[1];
	Chabu_Init(
			&chabu,
			0, APPL_NAME, RPS,
			channels, 0,
			NULL, 0,
			errorFunction, NULL, NULL, NULL, NULL,
			NULL );

	VERIFY_ERROR_INFO( Chabu_ErrorCode_INIT_PARAM_CHANNELS_RANGE, "count of channels");

}

TEST_F( ChabuInitTest, channels_count_too_high ){

	struct Chabu_Data chabu;
	struct Chabu_Channel_Data channels[1];
	Chabu_Init(
			&chabu,
			0, APPL_NAME, RPS,
			channels, 10000,
			NULL, 0,
			errorFunction, NULL, NULL, NULL, NULL,
			NULL );

	VERIFY_ERROR_INFO( Chabu_ErrorCode_INIT_PARAM_CHANNELS_RANGE, "count of channels");

}

TEST_F( ChabuInitTest, priorities_null ){

	struct Chabu_Data chabu;
	struct Chabu_Channel_Data channels[1];
	Chabu_Init(
			&chabu,
			0, APPL_NAME, RPS,
			channels, countof(channels),
			NULL, 0,
			errorFunction, NULL, NULL, NULL, NULL,
			NULL );

	VERIFY_ERROR_INFO( Chabu_ErrorCode_INIT_PARAM_PRIORITIES_NULL, "priorities must not be NULL");

}

TEST_F( ChabuInitTest, priorities_count_zero ){

	struct Chabu_Data chabu;
	struct Chabu_Channel_Data channels[1];
	struct Chabu_Priority_Data priorities[1];
	Chabu_Init(
			&chabu,
			0, APPL_NAME, RPS,
			channels, countof(channels),
			priorities, 0,
			errorFunction, NULL, NULL, NULL, NULL,
			NULL );

	VERIFY_ERROR_INFO( Chabu_ErrorCode_INIT_PARAM_PRIORITIES_RANGE, "count of priorities");

}

TEST_F( ChabuInitTest, priorities_count_too_high ){

	struct Chabu_Data chabu;
	struct Chabu_Channel_Data channels[1];
	struct Chabu_Priority_Data priorities[1];
	Chabu_Init(
			&chabu,
			0, APPL_NAME, RPS,
			channels, countof(channels),
			priorities, 10000,
			errorFunction, NULL, NULL, NULL, NULL,
			NULL );

	VERIFY_ERROR_INFO( Chabu_ErrorCode_INIT_PARAM_PRIORITIES_RANGE, "count of priorities");

}

TEST_F( ChabuInitTest, func_configure_null ){

	struct Chabu_Data chabu;
	struct Chabu_Channel_Data channels[1];
	struct Chabu_Priority_Data priorities[1];
	Chabu_Init(
			&chabu,
			0, APPL_NAME, RPS,
			channels, countof(channels),
			priorities, countof(priorities),
			errorFunction,
			NULL,//configureChannels,
			networkRegisterWriteRequest,
			networkRecvBuffer,
			networkXmitBuffer,
			NULL );

	VERIFY_ERROR_INFO( Chabu_ErrorCode_INIT_CONFIGURE_FUNC_NULL, "callback is null");

}

TEST_F( ChabuInitTest, func_write_req_null ){

	struct Chabu_Data chabu;
	struct Chabu_Channel_Data channels[1];
	struct Chabu_Priority_Data priorities[1];
	Chabu_Init(
			&chabu,
			0, APPL_NAME, RPS,
			channels, countof(channels),
			priorities, countof(priorities),
			errorFunction,
			configureChannels,
			NULL,//networkRegisterWriteRequest,
			networkRecvBuffer,
			networkXmitBuffer,
			NULL );

	VERIFY_ERROR_INFO( Chabu_ErrorCode_INIT_NW_WRITE_REQ_FUNC_NULL, "callback is null");

}

TEST_F( ChabuInitTest, func_read_null ){

	struct Chabu_Data chabu;
	struct Chabu_Channel_Data channels[1];
	struct Chabu_Priority_Data priorities[1];
	Chabu_Init(
			&chabu,
			0, APPL_NAME, RPS,
			channels, countof(channels),
			priorities, countof(priorities),
			errorFunction,
			configureChannels,
			networkRegisterWriteRequest,
			NULL,//networkRecvBuffer,
			networkXmitBuffer,
			NULL );

	VERIFY_ERROR_INFO( Chabu_ErrorCode_INIT_NW_READ_FUNC_NULL, "callback is null");

}

TEST_F( ChabuInitTest, func_write_null ){

	struct Chabu_Data chabu;
	struct Chabu_Channel_Data channels[1];
	struct Chabu_Priority_Data priorities[1];
	Chabu_Init(
			&chabu,
			0, APPL_NAME, RPS,
			channels, countof(channels),
			priorities, countof(priorities),
			errorFunction,
			configureChannels,
			networkRegisterWriteRequest,
			networkRecvBuffer,
			NULL,//networkXmitBuffer,
			NULL );

	VERIFY_ERROR_INFO( Chabu_ErrorCode_INIT_NW_WRITE_FUNC_NULL, "callback is null");

}

TEST_F( ChabuInitTest, func_configure_called ){

	struct Chabu_Data chabu;
	struct Chabu_Channel_Data channels[1];
	struct Chabu_Priority_Data priorities[1];
	Chabu_Init(
			&chabu,
			0, APPL_NAME, RPS,
			channels, countof(channels),
			priorities, countof(priorities),
			errorFunction,
			configureChannels,
			networkRegisterWriteRequest,
			networkRecvBuffer,
			networkXmitBuffer,
			NULL );

	ASSERT_EQ( 1u, configureChannels_fake.call_count );

}

TEST_F( ChabuInitTest, channels_not_configured ){

	struct Chabu_Data chabu;
	struct Chabu_Channel_Data channels[1];
	struct Chabu_Priority_Data priorities[1];
	Chabu_Init(
			&chabu,
			0, APPL_NAME, RPS,
			channels, countof(channels),
			priorities, countof(priorities),
			errorFunction,
			configureChannels,
			networkRegisterWriteRequest,
			networkRecvBuffer,
			networkXmitBuffer,
			NULL );

	VERIFY_ERROR_INFO( Chabu_ErrorCode_INIT_CHANNELS_NOT_CONFIGURED, "channel was not configured");

}

struct TestData {
	struct Chabu_Data* chabu;
	int channelId;
	Chabu_ChannelGetXmitBuffer * userCallback_ChannelGetXmitBuffer;
	Chabu_ChannelXmitCompleted * userCallback_ChannelXmitCompleted;
	Chabu_ChannelGetRecvBuffer * userCallback_ChannelGetRecvBuffer;
	Chabu_ChannelRecvCompleted * userCallback_ChannelRecvCompleted;

};
static void configureChannels_Cfg1( void* userData ){
	struct TestData* data = (struct TestData*)userData;
	struct Chabu_Data* chabu = data->chabu;
	Chabu_ConfigureChannel(chabu, data->channelId, 0,
			data->userCallback_ChannelGetXmitBuffer,
			data->userCallback_ChannelXmitCompleted,
			data->userCallback_ChannelGetRecvBuffer,
			data->userCallback_ChannelRecvCompleted, NULL );
}

TEST_F( ChabuInitTest, channels_funcs_null ){

	struct Chabu_Data chabu;
	struct Chabu_Channel_Data channels[1];
	struct Chabu_Priority_Data priorities[1];

	struct TestData tdata;
	memset(&tdata, 0, sizeof(tdata));
	tdata.chabu = &chabu;
	tdata.channelId = 0;
	configureChannels_fake.custom_fake = configureChannels_Cfg1;

	Chabu_Init(
			&chabu,
			0, APPL_NAME, RPS,
			channels, countof(channels),
			priorities, countof(priorities),
			errorFunction,
			configureChannels,
			networkRegisterWriteRequest,
			networkRecvBuffer,
			networkXmitBuffer,
			&tdata );

	VERIFY_ERROR_INFO( Chabu_ErrorCode_INIT_CHANNEL_FUNCS_NULL, "channel callback was NULL");

}

TEST_F( ChabuInitTest, channels_configured ){

	struct Chabu_Data chabu;
	struct Chabu_Channel_Data channels[1];
	struct Chabu_Priority_Data priorities[1];

	struct TestData tdata;
	tdata.chabu = &chabu;
	tdata.userCallback_ChannelGetXmitBuffer = channelGetXmitBuffer;
	tdata.userCallback_ChannelXmitCompleted = channelXmitCompleted;
	tdata.userCallback_ChannelGetRecvBuffer = channelGetRecvBuffer;
	tdata.userCallback_ChannelRecvCompleted = channelRecvCompleted;
	tdata.channelId = 0;
	configureChannels_fake.custom_fake = configureChannels_Cfg1;

	Chabu_Init(
			&chabu,
			0, APPL_NAME, RPS,
			channels, countof(channels),
			priorities, countof(priorities),
			errorFunction,
			configureChannels,
			networkRegisterWriteRequest,
			networkRecvBuffer,
			networkXmitBuffer,
			&tdata );

	ASSERT_EQ( Chabu_ErrorCode_OK_NOERROR, Chabu_LastError(&chabu));

}

TEST_F( ChabuInitTest, channel_config_invalid_channel_id_low ){

	struct Chabu_Data chabu;
	struct Chabu_Channel_Data channels[1];
	struct Chabu_Priority_Data priorities[1];

	struct TestData tdata;
	tdata.chabu = &chabu;
	tdata.userCallback_ChannelGetXmitBuffer = channelGetXmitBuffer;
	tdata.userCallback_ChannelXmitCompleted = channelXmitCompleted;
	tdata.userCallback_ChannelGetRecvBuffer = channelGetRecvBuffer;
	tdata.userCallback_ChannelRecvCompleted = channelRecvCompleted;
	tdata.channelId = -1;
	configureChannels_fake.custom_fake = configureChannels_Cfg1;

	Chabu_Init(
			&chabu,
			0, APPL_NAME, RPS,
			channels, countof(channels),
			priorities, countof(priorities),
			errorFunction,
			configureChannels,
			networkRegisterWriteRequest,
			networkRecvBuffer,
			networkXmitBuffer,
			&tdata );

	VERIFY_ERROR_INFO( Chabu_ErrorCode_INIT_CONFIGURE_INVALID_CHANNEL, "channel id invalid");

}

TEST_F( ChabuInitTest, channel_config_invalid_channel_id_high ){

	struct Chabu_Data chabu;
	struct Chabu_Channel_Data channels[1];
	struct Chabu_Priority_Data priorities[1];

	struct TestData tdata;
	tdata.chabu = &chabu;
	tdata.userCallback_ChannelGetXmitBuffer = channelGetXmitBuffer;
	tdata.userCallback_ChannelXmitCompleted = channelXmitCompleted;
	tdata.userCallback_ChannelGetRecvBuffer = channelGetRecvBuffer;
	tdata.userCallback_ChannelRecvCompleted = channelRecvCompleted;
	tdata.channelId = 1;
	configureChannels_fake.custom_fake = configureChannels_Cfg1;

	Chabu_Init(
			&chabu,
			0, APPL_NAME, RPS,
			channels, countof(channels),
			priorities, countof(priorities),
			errorFunction,
			configureChannels,
			networkRegisterWriteRequest,
			networkRecvBuffer,
			networkXmitBuffer,
			&tdata );

	VERIFY_ERROR_INFO( Chabu_ErrorCode_INIT_CONFIGURE_INVALID_CHANNEL, "channel id invalid");

}
