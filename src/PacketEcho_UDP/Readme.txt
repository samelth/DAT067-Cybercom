@Samuel Leckborn
@2020-11-27

This client program sends numbered UDP packets to a server that echoes every packet it receives.
Every other second the program sends a new packet and prints all the responses on the incoming buffer. 
It also features a "Pause menu" where some useful data can be accessed.
The pause menu as of now is only accessable via serial input:

'p' - Pause/Unpause

	When paused:
		1. Print current ip address
		2. Print current signal strength
		3. Print current carrier
		4. Reconnect.

The program will connect and start sending automatically when uploaded to the Arduino, assuming you have a working SIM card inserted.

To configure this to work with your local server you will have to add your server IP address as a declaration in the top of the code:
ex:
"
IPAddress VictorServer(188, 149, 54, 45); // Victor's echo server
IPAddress SamuelServer(83, 252, 118, 131); // Samuel's echo server
IPAddress yourServerIP(x,x,x,x);
"

You will also have to port forward in your router, 
port:2390 ip:local ip of server.

EchoServer program is available in repo/src/EchoServer.

