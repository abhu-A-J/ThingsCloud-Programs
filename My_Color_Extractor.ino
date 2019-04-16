
#include <SoftwareSerial.h>
SoftwareSerial mySerial(12, 13); // RX, TX

#include <ESP8266WiFi.h>          //https://github.com/esp8266/Arduino
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include "WiFiManager.h"          //https://github.com/tzapu/WiFiManager
#include <WiFiClientSecure.h>

int count=0,i,m,j,k;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
const int red_pin=12;
const int green_pin=13;
const int blue_pin=15;


const int S2=5;
const int S3=4;
const int Out=14;
unsigned int freq;

int rcolor=0;
int gcolor=0;
int bcolor=0;
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////// ALL DECLARATIONS for CLOUD //////////////////////////////
const char* host = "api.thingsai.io";                             
const char* post_url = "/devices/deviceData";       
const char* time_server = "baas.thethingscloud.com";             //this is to convert timestamp
const int httpPort = 80;
const int httpsPort = 443;
const char*  server = "api.thingsai.io";  
char timestamp[10];
WiFiClient client;

/////////////////////////////////////////////////////////////////////////////////////////////////////////
void configModeCallback (WiFiManager *myWiFiManager) 
{
  Serial.println("Entered config mode");             //*-*-*-*-*-*-*-*-*-*-*-*-*-*if control enters this function then net is not connected
  Serial.println(WiFi.softAPIP());                  // "WiFi.softAPIP() is for AP" , "WiFi.localIP() is for STA",
                                                                
  Serial.println(myWiFiManager->getConfigPortalSSID());             //if you used auto generated SSID, print it
}
/////////////////////////////////////// TIMESTAMP CALCULATION function///////////////////////////////////////
int GiveMeTimestamp()
{
  unsigned long timeout = millis();

  while (client.available() == 0)
  {
    if (millis() - timeout > 50000)
    {
      client.stop();
      return 0;
    }
  }

while (client.available())
      {
        String line = client.readStringUntil('\r');                    //indexOf() is a funtion to search for smthng , it returns -1 if not found
        int pos = line.indexOf("\"timestamp\"");                       //search for "\"timestamp\"" from beginning of response got and copy all data after that , it'll be your timestamp
        if (pos >= 0)                                                     
        {
          int j = 0;
          for(j=0;j<10;j++)
          {
            timestamp[j] = line[pos + 12 + j];
          }
        }
      }
}  
////////////////////////////////////////////////////////////////////////////////////////////////////////

void setup() 
{
  //////////////////////////////////////////////////////////////////////////////////////////////////////
  Serial.begin(115200);
  pinMode(red_pin,OUTPUT);
  pinMode(green_pin,OUTPUT);
  pinMode(blue_pin,OUTPUT);
  pinMode(S2,OUTPUT);
  pinMode(S3,OUTPUT);
  pinMode(Out,INPUT);
//  digitalWrite(S0,LOW);
//  digitalWrite(S1,HIGH);
  //////////////////////////////////////////////////////////////////////////////////////////////////////
      //(19200,SERIAL_8E1) - data size = 8 bits , parity = Even , stop bit =  1bit
  mySerial.begin(115200);
  WiFiManager wifiManager;
  wifiManager.setAPCallback(configModeCallback);
                                                                                                    
  if(!wifiManager.autoConnect("ASUS","abhushan"))                   //wifiManager.autoConnect("AP-NAME", "AP-PASSWORD"); (OR) wifiManager.autoConnect("AP-NAME"); only ID no password (OR) wifiManager.autoConnect(); this will generate a ID by itself
  {
    Serial.println("failed to connect and hit timeout");     //control comes here after long time of creating Access point "NodeMCU" by NodeMCU and still it has not connected
    //reset and try again, or maybe put it to deep sleep
    ESP.reset();
    delay(1000);
  } 

  //if you come here you have connected to the WiFi
  Serial.println("connected...yeey :");
 
}

void display_LED(int R,int G,int B)
{
  analogWrite(red_pin,R);
  analogWrite(green_pin,G);
  analogWrite(blue_pin,B);
  
}

void manipulate(int r,int g,int b)
{
  if(r>g && g>b)
  {
    r=255;
    g=g/2;
    b=0;
  }

  if(r>b && b>g)
  {
    r=255;
    g=0;
    b=b/2;
  }

  if(g>r && r>b)
  {
    g=255;
    r=r/2;
    b=0;
  }
  if(g>b && b>r)
  {
    g=255;
    b=b/2;
    r=0;
  }
  if(b>r && r>g)
  {
    b=255;
    r=r/2;
    g=0;

  }
  if(b>g && g>r)
  {
    b=255;
    g=g/2;
    r=0;
  }
  
   rcolor=r;bcolor=b;gcolor=g;
   gcolor=gcolor;
   bcolor=bcolor*0.25;
   Serial.print("Red is: ");Serial.print(rcolor);
   Serial.print("  Green is: ");Serial.print(gcolor);
   Serial.print("  Blue is: ");Serial.println(bcolor);
   display_LED(rcolor,gcolor,bcolor);
   //delay(1000);

}

void loop() 
{
  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  digitalWrite(S2,LOW);
  digitalWrite(S3,LOW);
  freq=pulseIn(Out,LOW);
  rcolor=freq;
   
  //Serial.print("Red freq:");Serial.println(freq);

  digitalWrite(S2,LOW);
  digitalWrite(S3,HIGH);
  freq=pulseIn(Out,LOW);
  bcolor=freq;
 
  //Serial.print("Blue freq:");Serial.println(freq);
  
  digitalWrite(S2,HIGH);
  digitalWrite(S3,HIGH);
  freq=pulseIn(Out,LOW);
  gcolor=freq;
   
 //Serial.print("Green freq:");Serial.println(freq);
  rcolor=map(rcolor,500,3000,255,0);
  gcolor=map(gcolor,500,3000,255,0);
  bcolor=map(bcolor,500,3000,255,0);
  
  manipulate(rcolor,gcolor,bcolor);
   
  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  Serial.print("connecting to ");
  Serial.println(host);                          //defined upside :- host = devapi2.thethingscloud.com or 139.59.26.117

///////////////////////////////////// TIMESTAMP CODE SNIPPET /////////////////////////
Serial.println("inside get timestamp\n");
  if (!client.connect(time_server, httpPort)) 
  {
    return;                                                        //*-*-*-*-*-*-*-*-*-*
  }

  client.println("GET /api/timestamp HTTP/1.1");                            //Whats this part doing, i didnt get
  client.println("Host: baas.thethingscloud.com");
  client.println("Cache-Control: no-cache");
  client.println("Postman-Token: ea3c18c6-09ba-d049-ccf3-369a22a284b8");
  client.println();

GiveMeTimestamp();                        //it'll call the function which will get the timestamp response from the server
Serial.println("timestamp receieved");
Serial.println(timestamp);
///////////////////////////////////////////////////////////////////////////////

  Serial.println("inside ThingsCloudPost");

 
String PostValue = "{\"device_id\":  61121695915   , \"slave_id\": 2";

//////////////////////////////////////////////////////////////////////////////////////////////////
PostValue = PostValue + ",\"dts\":" +timestamp;
PostValue = PostValue +",\"data\":{\"bcolor\":" + bcolor +",\"gcolor\":"+gcolor +",\"rcolor\":"+rcolor+ "}"+"}";///////////////////////////////////////////////////////////////////////////////////////////////////
  
  Serial.println(PostValue);

/* create an instance of WiFiClientSecure */
    WiFiClientSecure client;
    
    Serial.println("Connect to server via port 443");
    if (!client.connect(server, 443)){
        Serial.println("Connection failed!");
    } else {
        Serial.println("Connected to server!");
        /* create HTTP request */

        client.println("POST /devices/deviceData HTTP/1.1");
        client.println("Host: api.thingsai.io");
        //client.println("Connection: close");
        client.println("Content-Type: application/json");
        client.println("cache-control: no-cache");
        client.println("Authorization: Bearer eyJ0eXAiOiJKV1QiLCJhbGciOiJIUzI1NiJ9.IjVhMzBkZDFkN2QwYjNhNGQzODkwYzQ4OSI.kaY6OMj5cYlWNqC2PNTkXs9PKy6_m9tdW5AG7ajfVlY");
        client.print("Content-Length: ");
        client.println(PostValue.length());
        client.println();
        client.println(PostValue);
//////////////////////////////////POSTING the data on to the cloud is done and now get the response form cloud server//////////////////
 Serial.print("Waiting for response ");
        while (!client.available()){
            delay(50); //
            Serial.print(".");
        }  
        /* if data is available then receive and print to Terminal */
        while (client.available()) {
            char c = client.read();
            Serial.write(c);
        }

        /* if the server disconnected, stop the client */
        if (!client.connected()) {
            Serial.println();
            Serial.println("Server disconnected");
            client.stop();
        }
    }
Serial.println("//////////////////////    THE END     /////////////////////");

}
