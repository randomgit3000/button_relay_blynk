
//#include <Arduino.h>

#define BLYNK_PRINT Serial

#define BlynkVP_varONOFF 1

#define BlynkVP_varIndication 4

#define BlynkVP_var_Relay_Status 12

#define BlynkVP_Terminal_Pin 14


#define RelayPin D1 //


#include <BlynkSimpleEsp8266.h>
#include <ESP8266WiFi.h>


WidgetTerminal terminal(BlynkVP_Terminal_Pin);

#include <credentials.h>
const char *auth = BLYNK_AUTH;
const char *ssid = WIFI_SSID;
const char *pass = WIFI_PASSWD;

BlynkTimer timer; // запускает выполнение функций по таймеру

int RelayState, varIndication, rssi;


void sendDATA() // функция отсылки в блинк
{
  Serial.println("sendDATA():");
delay(2000);

Blynk.virtualWrite(BlynkVP_Terminal_Pin, "\nrssi:  ", rssi);
  varIndication = digitalRead(RelayPin);
  Serial.print("sendDATA(): varIndication = ");
  Serial.println(varIndication);
  if (varIndication == 1)
  {
    Blynk.virtualWrite(BlynkVP_varIndication, 255);
    Blynk.virtualWrite(BlynkVP_Terminal_Pin, "\n111: ");

  }
  else
  {
    Blynk.virtualWrite(BlynkVP_varIndication, 0);
    Blynk.virtualWrite(BlynkVP_Terminal_Pin, "\n000: ");
  }
 
  Blynk.virtualWrite(BlynkVP_var_Relay_Status, varIndication);


}

void wifi_reconnection_ON() // включает режим автопереподключения
{
  if (WiFi.getAutoConnect() != true) //
  {
    WiFi.setAutoConnect(true);   //
    WiFi.setAutoReconnect(true); //
  }
}

void connection_control()
{
  if (WiFi.status() != WL_CONNECTED)
  {
    Serial.println("connection_control(): Not connected to WiFi");
    rssi = WiFi.RSSI();
    WiFi.begin(ssid, pass);
  }
  else
  {
    rssi = WiFi.RSSI();
    Serial.println("");
    Serial.print("Connected to ");
    Serial.println(ssid);
    Serial.println(WiFi.localIP());
    Serial.print("rssi: ");
    Serial.println(rssi);

    if (!Blynk.connected())
    {
      Serial.println("connection_control(): Not connected to Blynk server - Blynk.connect()");
      Blynk.connect(); // try to connect to server with default timeout
    }
    else
    {
      Serial.println("connection_control(): Connected to Blynk server");
    }
  }
  if (Blynk.connected())
  {
    Serial.println("connection_control(): if (Blynk.connected()) - Blynk.run()");
    Blynk.run();
  }
}







BLYNK_WRITE(BlynkVP_varONOFF) //
{
  if (param.asInt() == 1)
  {
    RelayState = 1;
    Blynk.virtualWrite(BlynkVP_Terminal_Pin, "\nmanual_mode HIGH");
    digitalWrite(RelayPin, HIGH); //
    sendDATA();
  }
  

  if (param.asInt() == 0)
  {
    RelayState = 0;
    Blynk.virtualWrite(BlynkVP_Terminal_Pin, "\nmanual_mode LOW");
    digitalWrite(RelayPin, LOW); //
    sendDATA();
  }
  
}



//-----------------------------------------------------------------------------------

void setup()
{
  Serial.begin(115200);
  //WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, pass);
  delay(1000);
  wifi_reconnection_ON();
  Blynk.config(auth, IPAddress(192, 168, 2, 77), 8080); // Blynk.config(auth, server, port); адрес и порт локального сервера
  Blynk.connect();
  Blynk.virtualWrite(BlynkVP_Terminal_Pin, "clr");
  Blynk.virtualWrite(BlynkVP_Terminal_Pin, "\nStart");
  //Blynk.begin(auth, ssid, pass, IPAddress(192, 168, 2, 77), 8080);

  // пин в режим выхода
  pinMode(RelayPin, OUTPUT);
  //digitalWrite(RelayPin, LOW);



 
  timer.setInterval(59000L, sendDATA);
  


BLYNK_CONNECTED()
{
  Blynk.syncAll();
}

void loop()

{
  connection_control();
  timer.run();
}
