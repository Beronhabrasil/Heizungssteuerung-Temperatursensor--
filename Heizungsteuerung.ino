
#include <WiFiClient.h>
#include <WiFiServer.h>
#include <WiFiUdp.h>
#include <Wire.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include "Arduino.h"
const int pinoRele = 14;//D5
int SensorWater = 20;
int SensorAir   = 20;

const int oneWireBus   = 2;   //D4  Data wire is plugged into digital pin 2 on the Arduino
float h=0,t=0;
const char  *MashineInbetrieb = "Mashine Inbetrieb Temperatur Luft "; 
const char  *MashineAussebetrieb = "Mashine Aussenbetrieb Temperatur  ";
const char *network = "VirusFrancoFontana";
const char *password = "JohannWolfgangvonGoethe/1832";

ESP8266WebServer server(80); 

// Setup a oneWire instance to communicate with any OneWire device
OneWire oneWire(oneWireBus);

#define DHTPIN 4     // Digital pin connected to the DHT sensor 
// Feather HUZZAH ESP8266 note: use pins 3, 4, 5, 12, 13 or 14 --
// Pin 15 can work but DHT must be disconnected during program upload.

// Uncomment the type of sensor in use:
#define DHTTYPE    DHT11     // DHT 11
//#define DHTTYPE    DHT22     // DHT 22 (AM2302)
//#define DHTTYPE    DHT21     // DHT 21 (AM2301)

// See guide for details on sensor wiring and usage:
//   https://learn.adafruit.com/dht/overview

DHT_Unified dht(DHTPIN, DHTTYPE);

uint32_t delayMS;
DallasTemperature sensors(&oneWire);

void setup() {
  pinMode(pinoRele, OUTPUT); 
  Serial.begin(9600);
  Serial.print("Conectando a ");
  Serial.println(network);
  WiFi.begin(network,password);
  while (WiFi.status() != WL_CONNECTED){
    Serial.print("connecting to ");
    Serial.println(WiFi.SSID()); 
    delay(500);
  }
  Serial.print("connected network ipadress! IP ESP -> ");
  Serial.println(WiFi.localIP()); //Imprime o IP local do ESP
/* HTML  */
  server.on("/", paginaInicial);
 
  server.begin(); //Inicia o servidor
  
  // Initialize device.
  sensors.begin();  // Start up the library
  dht.begin();
  Serial.println(F("test"));
  // Print temperature sensor details.
  sensor_t sensor;
  dht.temperature().getSensor(&sensor);
  delayMS = sensor.min_delay / 1000;
}

void loop() {
   server.handleClient();
  // Delay between measurements.
  delay(delayMS);
  // Get temperature event and print its value.
  sensors_event_t event;
  dht.temperature().getEvent(&event); 
  sensors.requestTemperatures(); 
  float temperatureC = sensors.getTempCByIndex(0);
  float temperatureF = sensors.getTempFByIndex(0);

 if(event.temperature <= 20){  //if temperature is less  then  20 degrees air and water  variable
  Serial.println("Mashine Inbetrieb ");
  digitalWrite(pinoRele, HIGH);
 }
 else if(temperatureC <= 20){
  Serial.println("Mashine Inbetrieb ");
  digitalWrite(pinoRele, HIGH);
 }
 else{
     Serial.println("Mashine aussenbetrieb ");
     digitalWrite(pinoRele, LOW);
 }
     
}
void paginaInicial(){
  
  sensors.requestTemperatures(); 
  float temperatureC = sensors.getTempCByIndex(0);
  float temperatureF = sensors.getTempFByIndex(0);
  delay(5000);
  Serial.println(temperatureC);
  sensors_event_t event;
  dht.temperature().getEvent(&event);
  if (isnan(event.temperature)) {
    Serial.print(F("reading temperature!"));
  }
  else {
    Serial.print(F("Temperature: "));
    Serial.print(event.temperature);
    Serial.print(F("°C"));
    Serial.println("==========================================================>");
  }
  

 if(event.temperature <= 20){  //if temperature is less  then  20 degrees air and water  variable
  Serial.println("Mashine Inbetrieb ");
 }
 else if(temperatureC <= 20){
  Serial.println("Mashine Inbetrieb ");
 }
 else{Serial.println("Mashine aussenbetrieb ");
 }
   

  //============================
   String htmlMessage = "<!DOCTYPE html>"
                             "<html>"
                             "<head>"
                             "<meta http-equiv=\"refresh\" content=\"15\">" //Atualizar a pagina a cada 1s
                             "<title> Mangold Systemadministrator Heizung</title>";
                             htmlMessage += "<link rel='stylesheet' href='https://maxcdn.bootstrapcdn.com/bootstrap/4.0.0/css/bootstrap.min.css' integrity='sha384-Gn5384xqQ1aoWXA+058RXPxPg6fy4IWvTNh0E263XmFcJlSAwiGgFAW/dAiS6JXm' crossorigin='anonymous'>";
                             htmlMessage +=   "</head>"
                                              "<body><font face=\"verdana\">"
                                              "<h4>Mangold Systemadministrator Heizung</h4>";
                             htmlMessage += "<div class='p-3 mb-2 bg-light text-dark'>" ;                
                           
                             htmlMessage += "<div class ='p-3 mb-2 bg-light text-dark'>Wasser Temperatur</div>";
                             htmlMessage += temperatureC;
                             htmlMessage += " ===> Celsius ";
                             htmlMessage += "</br>"; 
                             htmlMessage += "</br>"; 
                             htmlMessage += "<div class ='p-3 mb-2 bg-light text-dark'>Luft Temperatur</div>";
                             htmlMessage += event.temperature;
                             htmlMessage += " ===> Celsius ";
                             htmlMessage +="<hr>";
                             htmlMessage += "<div class ='p-3 mb-2 bg-light text-dark'>Relative humidity</div>";
                             dht.humidity().getEvent(&event);
                             if (isnan(event.relative_humidity)) {
                                  Serial.println(F("reading humidity!"));
                             }
                             else {
                             
                             }
                             htmlMessage += event.relative_humidity;
                             htmlMessage += "  ===>   %";
                             htmlMessage +="<hr>";
                             htmlMessage += "<div class ='glyphicon glyphicon-scale'></div>";
                          
                             htmlMessage += "<br>";
                             htmlMessage +="<hr>";
                             htmlMessage += "<span class ='text-info'>@Calheirocreations</span>";
                             htmlMessage += "</div>" ;
                             "</html>";
                             

  //============================
  server.send(200, "text/html", htmlMessage); //Retorna resposta HTTP
}
