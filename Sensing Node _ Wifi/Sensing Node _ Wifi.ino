 #include <ESP8266WiFi.h>
#include <FirebaseArduino.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

#define FIREBASE_HOST "fyp1-5b7dc.firebaseio.com"
#define FIREBASE_AUTH "GwiXlJeMIXq6c4TaHx2nkBmC14AY2Eh4aboqXhTd"

#define WIFI_SSID "Avengers"
#define WIFI_PASSWORD "12345678"

float sensedVal;

const long utcOffsetInSeconds = 28800;
String formattedDate;
char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

String Time;
String Year;
String Month;
String Date;

int OperatingStatus;
int PreOn;

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds);

void setup() {
  Serial.begin(9600);
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);

  int n = WiFi.scanNetworks();

  delay(5000);
  
  // connect to wifi.
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }

  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  timeClient.begin();
}

int IDvalues = 0;

void loop() {
  timeClient.update();
  OperatingStatus = Firebase.getInt("Node/Node_01/Status/");
  PreOn = Firebase.getInt("Node/Node_01/PreOn/");

  PreOnF();
 
  delay(5000);
}

void PreOnF()
{
  if ((PreOn == 1) && (OperatingStatus ==1))
  {
    Serial.println("1,1");
  }

  else if ((PreOn == 0) && (OperatingStatus ==1))
  {
    Operating();
  }

  else if ((PreOn == 1) && (OperatingStatus ==0))
  {
    Serial.println("1,0");
  }

  else
  {
    Serial.println("0,0");
  }
}

void Operating()
{
  Serial.println("0,1");
  Date = timeClient.getDate();
  Month = timeClient.getMonth();
  Year = timeClient.getYear();
  Time = timeClient.getFormattedTime();
  sensedVal = Serial.readString().toFloat();
  String path = "Data/" + Year + "/";
  String path02 = path + Month + "/";
  String path03 = path02 + Date + "/Node_1/";
  String path04 = path03 + Time + "/";
  if (sensedVal < -0.4)
  {
    sensedVal = 0;
  }
  Firebase.setFloat(path04, sensedVal); 
  if (sensedVal >= 5)
  {
    Firebase.setInt("Node/Alarm/Status", 1); 
  }
}
