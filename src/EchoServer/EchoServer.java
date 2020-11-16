import java.io.IOException;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;
import java.net.SocketException;
import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.Scanner;

public class EchoServer extends Thread {
    private DatagramSocket socket;
    private boolean running;
 
    public EchoServer(int port) throws SocketException {
        this.socket = new DatagramSocket(port);
        System.out.println("server socket bound to port " + this.socket.getLocalPort());
        this.running = true;
    }
 
    public void run() {
        while (this.running) {
            byte[] buf = new byte[256];
            DatagramPacket packet = new DatagramPacket(buf, buf.length);
            try {
                this.socket.receive(packet);
            } catch (IOException e) {
                e.printStackTrace();
            }
            InetAddress packetAddress = packet.getAddress();
            int packetPort = packet.getPort();
            String time = (new SimpleDateFormat("HH:mm:ss")).format(new Date());
            System.out.println(time + " - packet received from " + packetAddress + ":" + packetPort + "\n" + new String(packet.getData()));
            packet = new DatagramPacket(buf, buf.length, packetAddress, packetPort);
            try {
                this.socket.send(packet);
            } catch (IOException e) {
                e.printStackTrace();
            }
            System.out.println("packet echoed");
        }
        this.socket.close();
    }
    
    public void close() {
        this.running = false;
    }
    
    public static void main(String[] args) throws IOException {
        EchoServer server = new EchoServer(2390); // listen to port 2390
        server.start();
        Scanner scanner = new Scanner(System.in);
        System.out.println("server started. type \"close\" to close the server and exit the program.");
        while(!"close".equals(scanner.next()));
        scanner.close();
        server.close();
        System.out.println("server closed. exiting program");
        System.exit(0);
    }
}
