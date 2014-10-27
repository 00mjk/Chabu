package chabu.tester.data;

import java.nio.ByteBuffer;

public class CmdConnectionConnect extends ACmdScheduled {

	public final String address;
	public final int port;

	public CmdConnectionConnect( long schedTime, String address, int port ){
		super( CommandId.CONNECTION_CONNECT, schedTime );
		this.address = address;
		this.port = port;
	}
	
	@Override
	public void encode(ByteBuffer buf) {
		super.encode(buf);
		ACommand.encodeString( buf, address );
		buf.putShort( (short)this.port );
	}
}
