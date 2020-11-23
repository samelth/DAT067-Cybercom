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
