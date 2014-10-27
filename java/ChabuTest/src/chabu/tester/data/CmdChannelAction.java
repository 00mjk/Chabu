package chabu.tester.data;

import java.nio.ByteBuffer;

public class CmdChannelAction extends ACmdScheduled {

	public final int channelId;
	public final int txCount;
	public final int rxCount;

	public CmdChannelAction(long schedTime, int channelId, int txCount, int rxCount) {
		super( CommandId.CHANNEL_ACTION, schedTime );
		this.channelId = channelId;
		this.txCount = txCount;
		this.rxCount = rxCount;
	}

	@Override
	public void encode(ByteBuffer buf) {
		buf.put( (byte)this.commandId.getId() );
		buf.putLong( this.schedTime );
		buf.put( (byte)this.channelId );
		buf.putShort( (short)this.txCount );
		buf.putShort( (short)this.rxCount );
	}
	
}
