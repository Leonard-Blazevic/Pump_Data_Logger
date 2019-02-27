#include <ESP8266WiFi.h>

void sendData(long state);

// Wi-Fi Settings
const char* ssid = "Jure"; // your wireless network name (SSID)
const char* password = "ZDBOlebo0317"; // your Wi-Fi network password

WiFiClient client;

// ThingSpeak Settings
const int channelID = 554588;
String writeAPIKey = "SLRX0866AGPT8VJO"; // write API key for your ThingSpeak Channel
const char* server = "api.thingspeak.com";
int postingInterval = 20 * 1000; // post data every 20 seconds

//current measurement
const int sensorIn = A0;
int mVperAmp = 100; // use 100 for 20A Module and 66 for 30A Module
double Voltage = 0;
double VRMS = 0;
double AmpsRMS = 0;

int randNum = 0;

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
}

void loop() {
  Voltage = getVPP();
  VRMS = (Voltage/2.0)*0.707; 
  AmpsRMS = (VRMS * 1000)/mVperAmp;
  /*Serial.print(AmpsRMS);
  Serial.println(" Amps RMS");*/

  /*if(postingInterval < 0){
    postingInterval = 20;
    Serial.println("entered");*/
    if (client.connect(server, 80)) {
      // Check the state
      long state = AmpsRMS > 5 ? 1 : 0;

      sendData(state);
      //Serial.println("SENT!!!");
    }
    client.stop();
  //}
  
  //postingInterval--;
  //Serial.println(postingInterval);

  // wait and then post again
  delay(postingInterval);
}


void sendData(long state){
  // Construct API request body
  String body = "field1=";
  body += String(state);
  
  client.println("POST /update HTTP/1.1");
  client.println("Host: api.thingspeak.com");
  client.println("User-Agent: ESP8266 (nothans)/1.0");
  client.println("Connection: close");
  client.println("X-THINGSPEAKAPIKEY: " + writeAPIKey);
  client.println("Content-Type: application/x-www-form-urlencoded");
  client.println("Content-Length: " + String(body.length()));
  client.println("");
  client.print(body);
}


float getVPP()
{
  float result;
  
  int readValue;             //value read from the sensor
  int maxValue = 0;          // store max value here
  int minValue = 1024;          // store min value here
  
   uint32_t start_time = millis();
   while((millis()-start_time) < 1000) //sample for 1 Sec
   {
       readValue = analogRead(sensorIn);
       // see if you have a new maxValue
       if (readValue > maxValue) 
       {
           /*record the maximum sensor value*/
           maxValue = readValue;
       }
       if (readValue < minValue) 
       {
           /*record the maximum sensor value*/
           minValue = readValue;
       }
   }
   
   // Subtract min from max
   result = ((maxValue - minValue) * 5.0)/1024.0;
      
   return result;
}

