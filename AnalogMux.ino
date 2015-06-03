
/*
 *  This sketch based on the example simple HTTP-like server.
 *  The server will perform 3 functions depending on the request
 *  1.  http://ESP8266-IP:SVRPORT/gpio/0 will set the GPIO16 low,
 *  2.  http://ESP8266-IP:SVRPORT/gpio/1 will set the GPIO16 high
 *  3.  http://ESP8266-IP:SVRPORT/?request=GetSensors will return a json string with sensor (Ain) values
 *
 *  ESP8266-IP is the IP address of the ESP8266 module
 *  SVRPORT is the TCP port number the server listens for connections on
 */

#include <ESP8266WiFi.h>

#include <UtilityFunctions.h>

extern "C" {
#include "user_interface.h"
}

//Server actions
#define SET_LED_OFF 0
#define SET_LED_ON  1
#define Get_SENSORS 2

#define SERBAUD 74880
#define SVRPORT 9701
#define ONEJSON 0
#define FIRSTJSON 1
#define NEXTJSON 2
#define LASTJSON 3

//GPIO pin assignments
#define DEBUGEN 4       // Debug enable - pin 5 on ESP8266
#define DS18B20 5       // Temperature Sensor pin (DS18B20) - pin 4 on ESP8266
#define AMUXSEL0 14     // AMUX Selector 0
#define AMUXSEL1 12     // AMUX Selector 1
#define AMUXSEL2 13     // AMUX Selector 2
#define LED_IND 16      // LED used for initial code testing (not included in final hardware design)

#define DHTTYPE DHT11   // DHT 11 

const char* ssid = "<wifi-ssid>";
const char* password = "<wifi-password>";
const IPAddress ipadd(192,168,0,132);     //-------these 3 require-------------- 
const IPAddress ipgat(192,168,0,1);       //--32 more bytes than const uint8_t--
const IPAddress ipsub(255,255,255,0);     //------------------------------------

//globals
int lc=0;
bool complete=false;
char Ain0[20],Ain1[20],Ain2[20],Ain3[20],Ain4[20],Ain5[20],Ain6[20],Ain7[20]; 
uint32_t state=0;
char szT[30];
float Ain;


// Create an instance of the server
// specify the port to listen on as an argument
WiFiServer server(SVRPORT);

void printStatus(char * status, int s) {
//    if(digitalRead(DEBUGEN)==1) {
      Serial.print(system_get_free_heap_size());
      delay(100);
      Serial.print(" ");
      delay(100);
      Serial.print(millis()/1000);
      delay(100);
      Serial.print(" ");
      delay(100);
      if(s>=0) Serial.print(s);
      else Serial.print("");
      delay(100);
      Serial.print(" ");
      delay(100);
      Serial.println(status);
//    }
    delay(100);
}
void startWIFI(void) {
  //set IP if not correct
  IPAddress ip = WiFi.localIP();
  //if( (ip[0]!=ipadd[0]) || (ip[1]!=ipadd[1]) || (ip[2]!=ipadd[2]) || (ip[3]!=ipadd[3]) ) { 
  if( ip!= ipadd) { 
      WiFi.config(ipadd, ipgat, ipsub);  //dsa added 12.04.2015
      Serial.println();
      delay(10);
      Serial.print("ESP8266 IP:");
      delay(10);
      Serial.println(ip);
      delay(10);
      Serial.print("Fixed   IP:");
      delay(10);
      Serial.println(ipadd);
      delay(10);
      Serial.print("IP now set to: ");
      delay(10);
      Serial.println(WiFi.localIP());
      delay(10);
  }
  // Connect to WiFi network
  Serial.println();
  delay(10);
  Serial.println();
  delay(10);
  Serial.print("Connecting to ");
  delay(10);
  Serial.println(ssid);
  delay(10); 
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println("");
  Serial.println("WiFi connected");
  
  // Start the server
  server.begin();
  Serial.println("Server started");

  // Print the IP address
  Serial.print("ESP8266 IP: ");
  Serial.println(WiFi.localIP());

  Serial.print("ESP8266 WebServer Port: ");
  Serial.println(SVRPORT);
  delay(300);

}


void readSensorIsr() {
  yield();
  switch(state++) {
    case 0:
      //Set 8-1 amux to position 0
      digitalWrite(AMUXSEL0, 0);
      digitalWrite(AMUXSEL1, 0);
      digitalWrite(AMUXSEL2, 0);
      delay(100);
      //Read analog input
      Ain = (float) analogRead(A0);
      ftoa(Ain,Ain0, 2);
     break;
    case 1:
      //Set 8-1 amux to position 1
      digitalWrite(AMUXSEL0, 1);
      digitalWrite(AMUXSEL1, 0);
      digitalWrite(AMUXSEL2, 0);
      delay(100);
       //Read analog input
      Ain = (float) analogRead(A0);
      ftoa(Ain,Ain1, 2);
      break;
    case 2:
      //Set 8-1 amux to position 2
      digitalWrite(AMUXSEL0, 0);
      digitalWrite(AMUXSEL1, 1);
      digitalWrite(AMUXSEL2, 0);
      delay(100);
       //Read analog input
      Ain = (float) analogRead(A0);
      ftoa(Ain,Ain2, 2);
      break;
    case 3:
      //Set 8-1 amux to position 3
      digitalWrite(AMUXSEL0, 1);
      digitalWrite(AMUXSEL1, 1);
      digitalWrite(AMUXSEL2, 0);
      delay(100);
       //Read analog input
      Ain = (float) analogRead(A0);
      ftoa(Ain,Ain3, 2);
      break;
    case 4:
      //Set 8-1 amux to position 4
      digitalWrite(AMUXSEL0, 0);
      digitalWrite(AMUXSEL1, 0);
      digitalWrite(AMUXSEL2, 1);
      delay(100);
       //Read analog input
      Ain = (float) analogRead(A0);
      ftoa(Ain,Ain4, 2);
      break;
    case 5:
      //Set 8-1 amux to position 5
      digitalWrite(AMUXSEL0, 1);
      digitalWrite(AMUXSEL1, 0);
      digitalWrite(AMUXSEL2, 1);
      delay(100);
       //Read analog input
      Ain = (float) analogRead(A0);
      ftoa(Ain,Ain5, 2);
      break;
    case 6:
      //Set 8-1 amux to position 6
      digitalWrite(AMUXSEL0, 0);
      digitalWrite(AMUXSEL1, 1);
      digitalWrite(AMUXSEL2, 1);
      delay(100);
       //Read analog input
      Ain = (float) analogRead(A0);
      ftoa(Ain,Ain6, 2);
      break;
    case 7:
      //Set 8-1 amux to position 7
      digitalWrite(AMUXSEL0, 1);
      digitalWrite(AMUXSEL1, 1);
      digitalWrite(AMUXSEL2, 1);
      delay(100);
       //Read analog input
      Ain = (float) analogRead(A0);
      ftoa(Ain,Ain7, 2);
      state = 0;
      break;
    default:
      break;
  }
  ESP.wdtFeed(); 
  yield();
}

void jsonAdd(String *s, String key,String val) {
    *s += '"' + key + '"' + ":" + '"' + val + '"';
}
void jsonEncode(int pos, String * s, String key, String val) {
    switch (pos) {
      case ONEJSON:      
      case FIRSTJSON:
        *s += "{\r\n";
        jsonAdd(s,key,val);
        *s+= (pos==ONEJSON) ? "\r\n}" : ",\r\n";
        break;
      case NEXTJSON:    
        jsonAdd(s,key,val);
        *s+= ",\r\n";
        break;
      case LASTJSON:    
        jsonAdd(s,key,val);
        *s+= "\r\n}";
        break;
    }
}
void killclient(WiFiClient client, bool *busy) {
  lc=0;
  delay(1);
  client.flush();
  client.stop();
  complete=false;
  *busy = false;  
}
void sysloop() {
  static bool busy=false;
  static int timeout_busy=0;
  int amux;
  //connect wifi if not connected
  if (WiFi.status() != WL_CONNECTED) {
    delay(1);
    startWIFI();
    return;
  }
  //return if busy
  if(busy) {
    delay(1);
    if(timeout_busy++ >10000) {
      printStatus((char *)" Status: Busy timeout-resetting..",-1);
      ESP.reset(); 
      busy = false;
    }
    return;
  }
  else {
    busy = true;
    timeout_busy=0;
  }
  delay(1);
  //Read 1 sensor every 2.5 seconds
  if(lc++>2500) {
    lc=0;
    ftoa(Ain,szT, 2);
    printStatus((char *)szT,amux);
    readSensorIsr(); 
    busy = false;
    return;   
  }
  // Check if a client has connected
  WiFiClient client = server.available();
  if (!client) {
     busy = false;
     return;
  } 
  // Wait until the client sends some data
  while((!client.available())&&(timeout_busy++<5000)){
    delay(1);
    if(complete==true) {
      killclient(client, &busy);
      return;
    }
  }
  //kill client if timeout
  if(timeout_busy>=5000) {
    killclient(client, &busy);
    return;
  }
  
  complete=false; 
  ESP.wdtFeed(); 
  
  // Read the first line of the request
  String req = client.readStringUntil('\r');
  client.flush();
  if (req.indexOf("/favicon.ico") != -1) {
    client.stop();
    complete=true;
    busy = false;
    return;
  }
  Serial.print("Recv http: ");  
  Serial.println(req);
  delay(100);
  
    // Match the request
  int val;
  if (req.indexOf("/gpio/0") != -1)
    val = SET_LED_OFF;
  else if (req.indexOf("/gpio/1") != -1)
    val = SET_LED_ON;
  else if (req.indexOf("/?request=GetSensors") != -1) {
    val = Get_SENSORS;
    //Serial.println("Get Sensor Values Requested");
  }  
  else {
    Serial.println("invalid request");
    client.stop();
    complete=true;
    busy = false;
    return;
  }
  client.flush();

  // Prepare Response header
  String s = "HTTP/1.1 200 OK\r\n";
         s += "Access-Control-Allow-Origin: *\r\n";
  String v ="";
  ESP.wdtFeed(); 
      
  switch (val) {
    case SET_LED_OFF:
    case SET_LED_ON:
      // Set GPIO4 according to the request
      digitalWrite(LED_IND , val);
  
      // Prepare the response for GPIO state
      s += "Content-Type: text/html\r\n\r\n";
      s += "<!DOCTYPE HTML>\r\nGPIO is now ";
      s += (val)?"high":"low";
      s += "</html>\n";
      // Send the response to the client
      client.print(s);
      break;
    case Get_SENSORS:
      //Create JSON return string
      s += "Content-Type: application/json\r\n\r\n";
      jsonEncode(FIRSTJSON,&s,"Ain0", Ain0);
      jsonEncode(NEXTJSON,&s,"Ain1", Ain1);
      jsonEncode(NEXTJSON,&s,"Ain2", Ain2);
      jsonEncode(NEXTJSON,&s,"Ain3", Ain3);
      jsonEncode(NEXTJSON,&s,"Ain4", Ain4);
      jsonEncode(NEXTJSON,&s,"Ain5", Ain5);
      jsonEncode(NEXTJSON,&s,"Ain6", Ain6);
      jsonEncode(NEXTJSON,&s,"Ain7", Ain7);
      v = system_get_free_heap_size();
      jsonEncode(NEXTJSON,&s,"SYS_Heap", v);
      v = millis()/1000;
      jsonEncode(LASTJSON,&s,"SYS_Time", v);
      // Send the response to the client
      client.print(s);
      yield();
      //ESP.wdtFeed(); 
      break;
    default:
      break;
   }

    delay(1);
    v ="";
    s="";
    val=0;
    Serial.println("Ending it: Client disconnected");
    delay(150);
    complete=true;
    busy = false;
    ESP.wdtFeed(); 
  

  // The client will actually be disconnected 
  // when the function returns and 'client' object is destroyed
}

void setup() {
  ESP.wdtEnable();
  Serial.begin(SERBAUD);
  delay(10);
  //complete=false;
  startWIFI();
  
  // Set Indicator LED as output
  pinMode(LED_IND , OUTPUT);
  digitalWrite(LED_IND, 0);
  // Set AMUX Selector pins as outputs
  pinMode(AMUXSEL0 , OUTPUT);
  pinMode(AMUXSEL1 , OUTPUT);
  pinMode(AMUXSEL2 , OUTPUT);
  // Set DEBUGEN as INPUT
  pinMode(DEBUGEN , INPUT);

  // Print Free Heap
  printStatus((char *)" Status: End of Setup",-1);
  delay(500);
  
}

void loop() {
    sysloop();
}
