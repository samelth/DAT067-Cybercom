#include <MKRNB.h>

void setup()
{
  Serial.begin(115200);
  while (!Serial);
  MODEM.begin();

  Serial.println("> > Module information");
  sendCommand("ATI",1000);

  Serial.println("> > Modem version, application version. Latest versions as of 2020-12-09:");
  Serial.println("> > L0.0.00.00.05.11, A.02.16");
  sendCommand("ATI9",1000);

  Serial.println("> > Set minimum functionality mode");
  sendCommand("AT+CFUN=0",180000);

  Serial.println("> > Set carrier profile to standard Europe");
  sendCommand("AT+UMNOPROF=100",1000);

  Serial.println("> > Deregister from network");
  sendCommand("AT+COPS=2",180000);

  Serial.println("> > Set Radio Access Technology (RAT) to NB-IoT");
  sendCommand("AT+URAT=8",10000);

  Serial.println("> > Set NB1 band mask to bands 3 and 20");
  sendCommand("AT+UBANDMASK=1,524292",1000);
  apply();
  confirm();
  Serial.println("> > Program is now finished.");
}

void apply()
{
  Serial.println("> > Applying changes..");
  delay(1000);
  sendCommand("AT+CFUN=15",180000);
  Serial.print("> > ");
  while(!MODEM.noop())
  {
    Serial.print(".");
    delay(1000);
  }
  Serial.println();
  Serial.println("> > Settings applied.");
  delay(1000);
}

void confirm()
{
  Serial.println("> > Confirmation:");
  delay(1000);
  sendCommand("AT+UMNOPROF?",1000);
  sendCommand("AT+URAT?",10000);
  sendCommand("AT+UBANDMASK?",1000);
  Serial.println("> > Confirmation done (please check manually).");
  delay(1000);
}

void sendCommand(String command, unsigned long timeout)
{
  while(!MODEM.noop());
  String response;
  Serial.print("~ ~ ");
  Serial.print(command);
  Serial.println();
  MODEM.send(command);
  MODEM.waitForResponse(timeout, &response);
  if(response)
  {
    Serial.println(response);
  } else {
    Serial.println("No response.");
  }
  Serial.println();
  Serial.println();
  delay(1000);
}

void loop()
{

}