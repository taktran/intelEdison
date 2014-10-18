#include <SPI.h>
#include <Ethernet.h>
#include <dht.h>

dht DHT;

#define DHT11_PIN 5

#define USERTOKEN      "..." // replace your theThings user token here
#define THINGTOKEN     "..." // replace your thing Token


// assign a MAC address for the ethernet controller.
// Newer Ethernet shields have a MAC address printed on a sticker on the shield
// fill in your address here:
byte mac[] = { 
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};

// fill in an available IP address on your network here,
// for manual configuration:
IPAddress ip(10,0,1,20);
// initialize the library instance:
EthernetClient client;

// if you don't want to use DNS (and reduce your sketch size)
// use the numeric IP instead of the name for the server:
//IPAddress server(10,10,51,10);      // numeric IP for api.theThings.io
char server[] = "api.thethings.io";   // name address for theThings API

unsigned long lastConnectionTime = 0;          // last time you connected to the server, in milliseconds
boolean lastConnected = false;                 // state of the connection last time through the main loop
const unsigned long postingInterval = 10*1000; //delay between updates to Pachube.com

void setup() {
 // Open serial communications and wait for port to open:
  Serial.begin(9600);
   while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }

  Serial.println("Start Ethernet...");

  // start the Ethernet connection:
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
    // DHCP failed, so use a fixed IP address:
    Ethernet.begin(mac, ip);
  }
}

void loop() {
  // read the analog sensor:
  //int sensorReading = analogRead(A0);
  int chk = DHT.read11(DHT11_PIN);
  switch (chk)
  {
  case DHTLIB_OK:  
    //Serial.println("DHT OK,\t"); 
    break;
  case DHTLIB_ERROR_CHECKSUM: 
    Serial.println("Checksum error,\t"); 
    break;
  case DHTLIB_ERROR_TIMEOUT: 
    //Serial.println("Time out error,\t"); 
    break;
  default: 
    Serial.println("Unknown error,\t"); 
    break;
  }
  // DISPLAY DATA
  delay(200);
  

  // if there's incoming data from the net connection.
  // send it out the serial port.  This is for debugging
  // purposes only:
  if (client.available()) {
    char c = client.read();
    Serial.print(c);
  }

  // if there's no net connection, but there was one last time
  // through the loop, then stop the client:
  if (!client.connected() && lastConnected) {
    Serial.println();
    Serial.println("disconnecting.");
    client.stop();
  }

  // if you're not connected, and ten seconds have passed since
  // your last connection, then connect again and send data:
  if(!client.connected() && (millis() - lastConnectionTime > postingInterval)) {
    sendData(DHT);
  }
  // store the state of the connection for next time through
  // the loop:
  lastConnected = client.connected();
}

// this method makes a HTTP connection to the server:
void sendData(dht DHT) 
{
  // if there's a successful connection:
  int temp = (DHT.temperature);
  int hum = (DHT.humidity);
  Serial.print(hum);
  Serial.print("%,\t");
  Serial.print(temp);
  Serial.println(" C");

  Serial.print("sendData to ");
  Serial.println(server);

  if (client.connect(server, 80)) 
  {
    Serial.println("connecting...");
    // send the HTTP PUT request:
    client.println("POST /v1/ThingWrite HTTP/1.1"); 
    client.println("Content-Type: application/json");
    client.println("Accept: application/json");
    client.print("Authorization:theThingsIO-Token: "); client.println(USERTOKEN); 
    client.println("Host: api.thethings.io");
    client.println("");
   
    // here's the actual content of the POST request:
 
    String jsonString = "{\"thing\":{\"id\": \"";
    jsonString += THINGTOKEN;
    jsonString +="\"},\"values\":[{\"key\": \"temperature\",\"value\": \"";
    jsonString += temp;
    jsonString +="\",\"units\": \"C\",\"type\": \"temporal\"}";
    jsonString +=", {\"key\":\"humidity\",\"value\":\"";
    jsonString += hum;
    jsonString +="\",\"units\": \"%\",\"type\": \"temporal\"}";    
    jsonString +="]}";

    // print it:
    Serial.println(jsonString);
    client.print("Content-Length: "); 
    client.println(jsonString.length());
    client.println(jsonString);
    client.println("");

    client.println("Connection: close");
    client.println();

    
    Serial.println(client);
  
  } 
  else 
  {
    // if you couldn't make a connection:
    Serial.println("connection failed");
    Serial.println();
    Serial.println("disconnecting.");
    client.stop();
  }
   // note the time that the connection was made or attempted:
  lastConnectionTime = millis();
}

