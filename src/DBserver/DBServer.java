package DBserver;

import java.io.IOException;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;
import java.net.SocketException;
import java.sql.SQLException;
import java.util.Scanner;

public class DBServer extends Thread {

    private DatagramSocket socket;
    private boolean running;

    public DBServer(final int port) throws SocketException {
        this.socket = new DatagramSocket(port);
        System.out.println("server socket bound to port " + this.socket.getLocalPort());
        this.running = true;
    }

    private enum MESSAGETYPE {
        SAVE_DB_MESSAGE("SAVE"),
        GET_DB_MESSAGE("GET");
        private String name;
        MESSAGETYPE(final String name) {
            this.name = name;
        }
        public String getName() {
            return this.name;
        }
    }

    public void run() {
        while (running) {
            byte[] buf = new byte[256];
            DatagramPacket packet = new DatagramPacket(buf, buf.length);
            try {
                socket.receive(packet);
                System.out.println("Received packet " + new String(packet.getData()));
                if (isDbSaveMessage(packet)) {
                    System.out.println("SAVE request received");
                    String receivedString = new String(packet.getData());
                    String[] arr = receivedString.split(",");
                    DatabaseCommunicator.saveMessage(Integer.parseInt(arr[1]), arr[2]);
                    buf = "Successfully saved message".getBytes();
                }
                if (isDbGetMessage(packet)) {
                    System.out.println("DB GET request received:");
                    String receivedString = new String(packet.getData());
                    String[] arr = receivedString.split(",");
                    buf = ("DB entry -> " + DatabaseCommunicator.getMessage(Integer.parseInt(arr[1].trim()))).getBytes();
                }
            } catch (SQLException e) {
                e.printStackTrace();
                buf = e.toString().getBytes();
            } catch (IOException e) {
                e.printStackTrace();
            }
            InetAddress address = packet.getAddress();
            int port = packet.getPort();
            packet = new DatagramPacket(buf, buf.length, address, port);
            try {
                socket.send(packet);
            } catch (IOException e) {
                e.printStackTrace();
            }
        }
        socket.close();
    }

    public void close() {
        this.running = false;
    }
    private boolean isDbSaveMessage(final DatagramPacket packet) {
        return firstWordInPacket(packet, ",").equals(MESSAGETYPE.SAVE_DB_MESSAGE.getName());
    }
    private boolean isDbGetMessage(final DatagramPacket packet) {
        return firstWordInPacket(packet, ",").equals(MESSAGETYPE.GET_DB_MESSAGE.getName());
    }
    private String firstWordInPacket(final DatagramPacket packet, final String regex) {
        String receivedString = new String(packet.getData());
        String[] arrOfstring = receivedString.split(regex);
        return arrOfstring[0];
    }

    public static void main(final String[] args) throws IOException {
        DBServer server = new DBServer(2390); // listen to port 2390
        server.start();
        Scanner scanner = new Scanner(System.in);
        System.out.println("server started. type \"close\" to close the server and exit the program.");
        while (!"close".equals(scanner.next())) { }
        scanner.close();
        server.close();
        System.out.println("server closed. exiting program");
        System.exit(0);
    }

}

