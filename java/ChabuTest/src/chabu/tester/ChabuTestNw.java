package chabu.tester;

import java.io.IOException;
import java.net.InetSocketAddress;
import java.nio.ByteBuffer;
import java.nio.channels.ReadableByteChannel;
import java.nio.channels.SelectionKey;
import java.nio.channels.Selector;
import java.nio.channels.SocketChannel;
import java.nio.channels.WritableByteChannel;
import java.util.ArrayDeque;
import java.util.Iterator;
import java.util.Set;
import java.util.TreeMap;

import chabu.INetwork;
import chabu.INetworkUser;
import chabu.Utils;
import chabu.tester.data.ACommand;
import chabu.tester.data.CmdDutConnect;

public class ChabuTestNw {
	private String name;
	private boolean doShutDown;
	private Selector selector;
	
	private CtrlNetwork ctrlNw = new CtrlNetwork();
	private Thread thread;
	private boolean isStarted = false;

	private TreeMap<DutId, DutState> dutStates = new TreeMap<>();
	
	class DutState {
		ArrayDeque<ACommand> commands = new ArrayDeque<>(100);
		public SocketChannel socketChannel;
	}
	
	class CtrlNetwork extends Network {
	
	}

	class Network implements INetwork {
		
//		ServerSocketChannel serverSocket;
//		SocketChannel socketChannel;
		ByteBuffer rxBuffer = ByteBuffer.allocate(0x10000);
		ByteBuffer txBuffer = ByteBuffer.allocate(0x10000);
		
		INetworkUser user;
		boolean userRequestRecv = false;
		boolean userRequestXmit = false;
		boolean netwRequestRecv = true;
		boolean netwRequestXmit = true;

		Network(){
			rxBuffer.position(rxBuffer.limit());
		}
		public void setNetworkUser(INetworkUser user) {
			this.user = user;
		}
		public void evUserXmitRequest() {
			userRequestXmit = true;
			netwRequestRecv = true;
			selector.wakeup();
		}
		public void evUserRecvRequest() {
			userRequestRecv = true;
			netwRequestXmit = true;
			selector.wakeup();
		}
		public void handleRequests() {
			if( userRequestRecv ){
				userRequestRecv = false;
				user.evRecv(rxBuffer);
			}
			if( userRequestXmit ){
				userRequestXmit = false;
				user.evXmit(txBuffer);
			}
		}
//		public void connectionClose() throws IOException {
//			socketChannel.close();
//		}
	};
	
	public void setCtrlNetworkUser(INetworkUser user) {
		this.ctrlNw.user = user;
	}
	
	public ChabuTestNw(String name) throws IOException, InterruptedException {

		this.name = name;
		
		selector = Selector.open();

//		ctrlNw.serverSocket = ServerSocketChannel.open();
//		ctrlNw.serverSocket.configureBlocking(false);
//		ctrlNw.serverSocket.register( selector, 0 );
		
		thread = new Thread( this::run, name );
		thread.start();
		synchronized(this){
			while( !isStarted ){
				this.wait();
			}
		}
	}
	
	private void run() {
		try {
			@SuppressWarnings("unused")
			int connectionOpenIndex = 0;

			synchronized(this){
				isStarted = true;
				notifyAll();
			}
			
			while (!doShutDown) {

				
				selector.select(500);

				Set<SelectionKey> readyKeys = selector.selectedKeys();

				ctrlNw.handleRequests();
				Iterator<SelectionKey> iterator = readyKeys.iterator();
				while (iterator.hasNext()) {
					SelectionKey key = iterator.next();
					iterator.remove();
					if( key.isValid() ){
						if (key.isConnectable()) {
							SocketChannel channel = (SocketChannel)key.channel();
							if (channel.isConnectionPending()){
								if( channel.finishConnect() ){
									channel.register( selector, SelectionKey.OP_READ|SelectionKey.OP_WRITE, key.attachment() );
								}
							}
						}
//						if (key.isAcceptable()) {
//							if( key.channel() == ctrlNw.serverSocket ){
//								SocketChannel acceptedChannel = ctrlNw.serverSocket.accept();
//								ctrlNw.socketChannel.close();
//								ctrlNw.socketChannel = acceptedChannel;
//								ctrlNw.socketChannel.configureBlocking(false);
//								ctrlNw.serverSocket.register(selector, SelectionKey.OP_ACCEPT, ctrlNw);
//							}
//							else {
//								Utils.ensure(false , "invalid state" );
//							}
//							synchronized(this){
//								notifyAll();
//							}
//
//						} 
						if (key.isWritable() || key.isReadable() ) {
							System.out.printf("Server selector %s %s %s\n", name, key.isReadable(), key.isWritable());
							Network nw = (Network)key.attachment();
							nw.netwRequestRecv = true;
							if( key.isReadable() ){								
								nw.rxBuffer.compact();
								int readSz = ((ReadableByteChannel)key.channel()).read(nw.rxBuffer);
								if( readSz < 0 ){
									key.cancel();
//									nw.connectionClose();
								}
								nw.rxBuffer.flip();
								nw.user.evRecv(nw.rxBuffer);
							}
							
							if( key.isValid() && key.isWritable() ) {
								nw.netwRequestXmit = false;
								nw.user.evXmit(nw.txBuffer);
								nw.txBuffer.flip();
								System.out.printf("%s Xmit %d\n", name, nw.txBuffer.remaining() );
								((WritableByteChannel)key.channel()).write(nw.txBuffer);
								if( nw.txBuffer.hasRemaining() ){
									nw.netwRequestXmit = true;
								}
								nw.txBuffer.compact();
							}
							
							if( key.isValid()){
								int interestOps = 0;
								if( nw.netwRequestRecv ){
									interestOps |= SelectionKey.OP_READ;
								}
								if( nw.netwRequestXmit ){
									interestOps |= SelectionKey.OP_WRITE;
								}
								System.out.printf("%s %x\n", name, interestOps );
								key.channel().register( selector, interestOps, nw );
							}
						}
					}
				}
			}
		} catch (Exception e) {
			e.printStackTrace();
		} finally {

			try {
//				if( ctrlNw.socketChannel != null ){
//					ctrlNw.socketChannel.socket().close();
//					ctrlNw.socketChannel.close();
//				}
//				if( ctrlNw.serverSocket != null ){
//					ctrlNw.serverSocket.close();
//				}
				if( selector != null ) {
					selector.close();
				}
				synchronized( this ){
					notifyAll();
				}
			} catch (Exception e) {
				//					startupException = e;
				e.printStackTrace();
			}
		}
	}

	public void start() {
		Utils.ensure( thread == null );
		thread = new Thread(this::run);
		thread.start();
	}


	public void shutDown() {
		doShutDown = true;
		selector.wakeup();
	}

	public synchronized void forceShutDown() {
		shutDown();
		while( selector.isOpen() ){
			Utils.waitOn(this);
		}
	}

	public synchronized void addCommand(DutId dut, ACommand cmd) throws IOException {
		if( dut == DutId.ALL ){
			Utils.ensure( !(cmd instanceof CmdDutConnect) );
			for( DutState ds : dutStates.values() ){
				ds.commands.add( cmd );
			}
		}
		else {
			if( !dutStates.containsKey(dut)){
				dutStates.put( dut, new DutState());
			}
			DutState ds = dutStates.get(dut);
			
			if( cmd instanceof CmdDutConnect ){
				Utils.ensure( ds.socketChannel == null );
				CmdDutConnect cmdDutConnect = (CmdDutConnect)cmd;
				SocketChannel socketChannel = SocketChannel.open();
				socketChannel.configureBlocking(false);
				socketChannel.connect( new InetSocketAddress( cmdDutConnect.address, cmdDutConnect.port ));
				ds.socketChannel = socketChannel;
				socketChannel.register( selector, SelectionKey.OP_CONNECT, ds );
			}
			else {
				ds.commands.add(cmd);
			}
		}
		notifyAll();
	}

	public synchronized void flush( DutId dut ) throws InterruptedException {
		while(true){
			boolean isEmpty = true;
			if( dut == DutId.ALL ){
				for( DutState ds : dutStates.values() ){
					if( !ds.commands.isEmpty() ){
						isEmpty = false;
					}
				}
			}
			else {
				DutState ds = dutStates.get(dut);
				if( ds != null && !ds.commands.isEmpty() ){
					isEmpty = false;
				}
			}

			if( isEmpty ){
				break;
			}
			wait();
		}
	}

}
