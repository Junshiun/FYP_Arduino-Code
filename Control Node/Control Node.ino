#include <ESP8266WiFi.h>
#include <FirebaseArduino.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

// Set these to run example.
#define FIREBASE_HOST "xxx"
#define FIREBASE_AUTH "xxx"

#define WIFI_SSID "xxx"
#define WIFI_PASSWORD "xxx"

const long utcOffsetInSeconds = 28800;
String formattedDate;
String Day;
char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds);

int Node_01_Status;
int Node_02_Status;
int Node_03_Status;
int CurrentTime;
int Operating_Time_ST;
int Operating_Time_ET;
int Peak_Time_ST;
int Peak_Time_ET;

int Hours;
int Minutes;
String Time;
int distribution;
String disPath;
String disPath02;

String Path01;
String Path02;
int Sequence;
int Alarm;

const int buzzer = D1;

void setup() {
  Serial.begin(9600);
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);

  Serial.print("Scan start ... ");
  int n = WiFi.scanNetworks();
  Serial.print(n);
  Serial.println(" network(s) found");
  for (int i = 0; i < n; i++)
  {
    Serial.println(WiFi.SSID(i));
  }
  Serial.println();

  delay(5000);
  
  // connect to wifi.
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("connecting");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.print("connected: ");
  Serial.println(WiFi.localIP());

  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  timeClient.begin();

  pinMode(buzzer, OUTPUT);
}

void loop() {
  /*Firebase.setInt("Node/Alarm/Status", 1);*/
  timeClient.update();
  
  Day = daysOfTheWeek[timeClient.getDay()];
  Serial.print(Day);
  Serial.print(", ");
  Hours = timeClient.getHours();
  Minutes = timeClient.getMinutes();
  Time = timeClient.getFormattedTime();
  Serial.println(Time);
  CurrentTime = Hours*100 + Minutes;
  Serial.println(CurrentTime);
  
  if (Firebase.failed()) {
    Serial.print("setting /truth failed:");
    Serial.println(Firebase.error());  
    return;
  }
  else{
    Serial.println("FirebaseConnected");
  }

  Get_Data();

  AlarmF();

  PreOn();

  Peak_Time();

  delay(5000);
}

void Get_Data()
{
  Path01 = "Schedule/Day/" + Day + "/Operation_Hour/Start_Time/";
  Path02 = Firebase.getInt(Path01);
  Operating_Time_ST = String(Path02 + "00").toInt();
  Serial.println(Operating_Time_ST);

  Path01 = "Schedule/Day/" + Day + "/Operation_Hour/End_Time/";
  Path02 = Firebase.getInt(Path01);
  Operating_Time_ET = String(Path02 + "00").toInt();
  Serial.println(Operating_Time_ET);

  Path01 = "Schedule/Day/" + Day + "/Peak_Hour/Start_Time/";
  Path02 = Firebase.getInt(Path01);
  Peak_Time_ST = String(Path02 + "00").toInt();
  Serial.println(Peak_Time_ST);

  Path01 = "Schedule/Day/" + Day + "/Peak_Hour/End_Time/";
  Path02 = Firebase.getInt(Path01);
  Peak_Time_ET = String(Path02 + "00").toInt();
  Serial.println(Peak_Time_ET);

  Sequence = Firebase.getInt("Node/Sequence/");
  Alarm = Firebase.getInt("Node/Alarm/Status");
}

void AlarmF()
{
  if (Alarm ==1)
  {
    tone(buzzer, 1000);
  }
  else
  {
    noTone(buzzer); 
  }
}

void PreOn()
{
   if ((CurrentTime >= Operating_Time_ST - 100) && (CurrentTime <= Operating_Time_ST))
   {
     if (distribution == 0)
     {
      if (Sequence ==3)
      {
        Sequence = 1;
        Firebase.setInt("Node/Node_01/PreOn", 1);
      }
      else if (Sequence ==2)
      {
        Sequence = 3;
        Firebase.setInt("Node/Node_03/PreOn", 1);
      }
      else if (Sequence ==1)
      {
        Sequence = 2;
        Firebase.setInt("Node/Node_02/PreOn", 1);       
      }
      Firebase.setInt("Node/Sequence/", Sequence);
      distribution = 1;
     }
   }

   else if ((CurrentTime >= Peak_Time_ST - 100) && (CurrentTime <= Peak_Time_ST))
   {
    Firebase.setInt("Node/Node_01/PreOn", 1);    
    Firebase.setInt("Node/Node_02/PreOn", 1);    
    Firebase.setInt("Node/Node_03/PreOn", 1);    
    distribution = 0;
   }

   else
   {
     Firebase.setInt("Node/Node_01/PreOn", 0);
     Firebase.setInt("Node/Node_02/PreOn", 0);    
     Firebase.setInt("Node/Node_03/PreOn", 0);    
     distribution = 0;    
   }
}

void Peak_Time()
{
  if ((CurrentTime >= Peak_Time_ST) && (CurrentTime <= Peak_Time_ET))
  {
    Firebase.setInt("Node/Node_01/Status/", 1);
    Firebase.setInt("Node/Node_02/Status/", 1);
    Firebase.setInt("Node/Node_03/Status/", 1);
    distribution = 0;
  }
  else
  {
    Operation_Time();
  }
}

void Operation_Time()
{
  if ((CurrentTime >= Operating_Time_ST) && (CurrentTime <= Operating_Time_ET))
  {
    if (Sequence ==3)
    {
      Firebase.setInt("Node/Node_03/Status", 1);
    }
    else if (Sequence ==2)
    {
      Firebase.setInt("Node/Node_02/Status", 1);
    }
    else
    {
      Firebase.setInt("Node/Node_01/Status", 1);       
    }
  }
  else
  {
    Firebase.setInt("Node/Node_01/Status/", 0);
    Firebase.setInt("Node/Node_02/Status/", 0);
    Firebase.setInt("Node/Node_03/Status/", 0);
  }
}
