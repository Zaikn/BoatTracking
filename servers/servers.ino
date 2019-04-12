
//If you use Dragino IoT Mesh Firmware, uncomment below lines.
//For product: LG01.
#define BAUDRATE 115200

//If you use Dragino Yun Mesh Firmware , uncomment below lines.
//#define BAUDRATE 250000

#include <Console.h>
#include <SPI.h>
#include <RH_RF95.h>

//Bridge
#include <Bridge.h>
#include <BridgeServer.h>
#include <BridgeClient.h>
BridgeServer server;

String var[4] = {"", "", "", ""};
String bateau1[4] = {"", "", "", "1"};
String bateau2[4] = {"", "", "", "2"};
String bateau3[4] = {"", "", "", "3"};
// Singleton instance of the radio driver
RH_RF95 rf95;

int led = A2;
float frequency = 868.0;

void setup()
{
  pinMode(13, OUTPUT);
  digitalWrite(13, LOW);
  Bridge.begin(BAUDRATE);
  digitalWrite(13, HIGH);
  Console.begin();
  Console.println("Start Sketch");
  if (!rf95.init())
    Console.println("init failed");

  // Setup ISM frequency
  rf95.setFrequency(frequency);

  // Setup Power,dBm
  rf95.setTxPower(13);
  rf95.setSpreadingFactor(7);
  rf95.setSignalBandwidth(125000);
  rf95.setCodingRate4(5);

  Console.print("Listening on frequency: ");
  Console.println(frequency);

  // Listen for incoming connection only from localhost
  // (no one from the external network could connect)
  server.listenOnLocalhost();
  server.begin();

}

void loop()
{
  /*
   * **************************************************
   **************** RECEIVE DATA + SEND REPLY***********************
   * **************************************************
  */

  if (rf95.available())
  {
    // Should be a message for us now
    uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
    uint8_t len = sizeof(buf);
    if (rf95.recv(buf, &len))
    {
      //RECEIVE
      digitalWrite(led, HIGH);
      RH_RF95::printBuffer("request: ", buf, len);
      Console.print("got request: ");
      Console.println((char*)buf);
      Serial.println((char*)buf);
      Console.print("RSSI: ");
      Console.println(rf95.lastRssi(), DEC);

      //TRADUCTION
      // Get les données dans des var
      // RAPPEL : the format of datasend is longtitude,latitude,altitude,DeviceID,
      char *token;
      /* get the first token */
      token = strtok(buf, ",");

      int i = 0;
      Console.println("T'ENVOIES OU PAS FDPPPPP");
      /* walk through other tokens */
      while ( token != NULL ) {

        Console.println(token);
        var[i] = token;
        i++;
        token = strtok(NULL, ",");
      }

      if (var[3] == "1")
      {
        for (int i = 0; i < 4; i++)
        {
          bateau1[i] = var[i];
          Console.print(bateau1[i] + ",");
        }
        Console.print("\n");
      }
      if (var[3] == "2")
      {
        for (int i = 0; i < 4; i++)
        {
          bateau2[i] = var[i];
        }
      }

      if (var[3] == "3")
      {
        for (int i = 0; i < 4; i++)
        {
          bateau3[i] = var[i];
        }
      }

      // Send a reply
      uint8_t data[] = "And hello back to you";
      rf95.send(data, sizeof(data));
      rf95.waitPacketSent();
      Console.println("Sent a reply");
      digitalWrite(led, LOW);
    }
    else
    {
      Console.println("recv failed");
    }

    /*
    * *******************************************************************
    **************** GET CLIENTS COMING FROM SERVER***********************
    * *******************************************************************
    */
    BridgeClient client = server.accept();


    // There is a new client?
    if (client) {
      // Process request

      String command = client.readStringUntil('/');
      //10.130.1.1/arduino/
      client.println("Status: 200");
      client.println("Content-type: application/json; charset=utf-8");
      client.println(); //mandatory blank line
      /*client.println(F("{"));
      client.println(F("\"MODULES\" : "));*/
      client.println(F("{"));
      client.print(F("\"MODULE1\" : \"" + bateau1[3] + "\"" + ",");
      client.print(F("\"LON1\" : \"" + bateau1[0] + "\"" + ","));
      client.print(F("\"LAT1\" : \"" + bateau1[1] + "\"" + ","));
      client.print(F("\"MODULE2\" : \"" + bateau2[3] + "\"" + ","));
      client.print(F("\"LON2\" : \"" + bateau2[0] + "\"" + ","));
      client.print(F("\"LAT2\" : \"" + bateau2[1] + "\"" + ","));
      client.print(F("\"MODULE3\" : \"" + bateau3[3] + "\"" + ","));
      client.print(F("\"LON3\" : \"" + bateau3[0] + "\"" + ","));
      client.print(F("\"LAT3\" : \"" + bateau3[1] + "\""));
      client.println(F("}"));
      //client.println(F("}"));

     /* String message = "{ ";
      message += "\"MODULE1\":\"";
      message += bateau1[3];
      message += ",\"LON1\":\"";
      message += bateau1[0];
      message += ",\"LAT1\":\"";
      message += bateau1[1];
      
     message += ",\"MODULE2\":\"";
      message += bateau2[3];
      message += ",\"LON2\":\"";
      message += bateau2[0];
      message += ",\"LAT2\":\"";
      message += bateau2[1];
      
      message += ",\"MODULE3\":\"";
      message += bateau3[3];
      message += ",\"LON3\":\"";
      message += bateau3[0];
      message += ",\"LAT3\":\"";
      message += bateau3[1];
      message += "\"}";*/
      
      

      //client.send(200, "application/json", message);
      //Serial.println(message);

      // Close connection and free resources.
      client.stop();
    }

    delay(50); // Poll every 50ms
  }
}


