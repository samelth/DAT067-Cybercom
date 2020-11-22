package DBserver;

import java.io.IOException;
import java.net.*;
import java.util.Scanner;

final class DBClient {

    private DatagramSocket socket;
    private InetAddress address;
    private int port;


    public DBClient(final int port) throws SocketException, UnknownHostException {
        socket = new DatagramSocket();
        address = InetAddress.getByName("188.148.194.26");
        socket.connect(address, port);
        socket.setSoTimeout(10000);
        this.port = port;
    }

    public String sendEcho(final String msg) throws IOException {
        String received = "";
        byte[] buf = msg.getBytes();
        DatagramPacket packet = new DatagramPacket(buf, buf.length, address, this.port);
        socket.send(packet);
        buf = new byte[256];
        packet = new DatagramPacket(buf, buf.length, address, this.port);
        try {
            socket.receive(packet);
            received = "String received: " + new String(packet.getData(), 0, packet.getLength());
            System.out.println(received);
        } catch (SocketTimeoutException e) {
            System.out.println("Cant connect to server");
        }
        return received;
    }

    public void close() {
        socket.close();
    }
    public static void main(String[] args) throws IOException {
        System.out.println(" Client started. Write commands as GET,<packetID> to receive a entry from the DB \n "
                            + " SAVE,<packetID>,<message> to save a message in the DB");
        DBClient e = new DBClient(2390);
        Scanner sc = new Scanner(System.in);
        while(true) {
            if(sc.hasNextLine()) {
                e.sendEcho(sc.nextLine());
            }
        }
    }
}
