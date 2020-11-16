import java.io.IOException;
import java.util.Scanner;
import java.time.LocalTime;
public class Client {

	public static void main(String[] args) throws IOException {
		EchoClient e= new EchoClient(2390);
		while(true){
			System.out.println(e.sendEcho( "Hej Viktor" ));
			try {
				Thread.sleep(10000);
			} catch (InterruptedException ex) {
				ex.printStackTrace();
			}
		}
	}
}
