package chabu;

public enum ChabuErrorCode {
	OK_NOERROR       ( 0 ),
	UNKNOWN          ( 1 ),
	ASSERT           ( 2 ), 
	NOT_ACTIVATED              ( 3 ),
	IS_ACTIVATED               ( 4 ),

	CONFIGURATION_PRIOCOUNT   ( 11 ), 
	CONFIGURATION_NETWORK     ( 12 ), 
	CONFIGURATION_CH_ID       ( 13 ), 
	CONFIGURATION_CH_PRIO     ( 14 ), 
	CONFIGURATION_CH_USER     ( 15 ), 
	CONFIGURATION_CH_RECVSZ   ( 16 ), 
	CONFIGURATION_NO_CHANNELS ( 17 ), 
	CONFIGURATION_VALIDATOR   ( 18 ),
	
	SETUP_LOCAL_MAXRECVSIZE     ( 21 ),
	SETUP_LOCAL_APPLICATIONNAME ( 23 ), 
	
	SETUP_REMOTE_CHABU_VERSION  ( 31 ),
	SETUP_REMOTE_MAXRECVSIZE    ( 32 ),
	
	PROTOCOL_LENGTH             ( 50 ),
	PROTOCOL_PCK_TYPE           ( 51 ),
	PROTOCOL_ABORT_MSG_LENGTH   ( 52 ), 
	PROTOCOL_SETUP_TWICE        ( 53 ),
	PROTOCOL_ACCEPT_TWICE       ( 54 ),
	
	REMOTE_ABORT                 (100), 

	// Application can use code starting with 0x100 .. 0x1FF
	APPLICATION_VALIDATOR   ( 0x100 ),  
	;
	
	private final int code;

	private ChabuErrorCode( int code ){
		this.code = code;
	}
	
	public int getCode() {
		return code;
	}
	
}