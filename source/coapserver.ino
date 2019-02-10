#include <SPI.h>
#include <Dhcp.h>
#include <Dns.h>
#include <Ethernet.h>
#include <EthernetUdp.h>
#include "coap.h"
#include "DHT.h"




#define DHTTYPE DHT22
#define DHTPIN 2
#define LIGHTPIN 3
DHT dht(DHTPIN, DHTTYPE);

byte mac[] = { 0xEC, 0xB1, 0xD7, 0x59, 0xFC, 0xF4 };

// UDP and CoAP class
EthernetUDP Udp;
Coap coap(Udp);




// GET dht data
void callback_temperature(CoapPacket &packet, IPAddress ip, int port){
  char response[200];
  char char_t;
  
  // extract payload
  char p[packet.payloadlen + 1];
  memcpy(p, packet.payload, packet.payloadlen);
  p[packet.payloadlen] = 0;
  String message(p);

  float t = dht.readTemperature();

  if (isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    coap.sendResponse(ip, port, packet.messageid, (char*)"Error reading data from DHT11.");
    return;
  }
  
  Serial.println(t);

  //dtostrf(t, 2, 2, char_t);

  
  sprintf(response, "Temp: %d °C ", (int) t);

  if(packet.code==1){ 
    Serial.println("Received GET request for dht sensor.");
    coap.sendResponse(ip, port, packet.messageid, response);
  }
}


// GET humidity data
void callback_humidity(CoapPacket &packet, IPAddress ip, int port){
  char response[200];
  char char_h;
  
  // extract payload
  char p[packet.payloadlen + 1];
  memcpy(p, packet.payload, packet.payloadlen);
  p[packet.payloadlen] = 0;
  String message(p);


  float h = dht.readHumidity();

  if (isnan(h)) {
    Serial.println("Failed to read from DHT sensor!");
    coap.sendResponse(ip, port, packet.messageid, (char*)"Error reading data from DHT11.");
    return;
  }
  
  Serial.println(h);
  //dtostrf(h, 2, 0, char_h);
  
  sprintf(response, " humidity: %d%%", (int) h);

  if(packet.code==1){ 
    Serial.println("Received GET request for dht sensor.");
    coap.sendResponse(ip, port, packet.messageid, response);
  }
}


// GET light data
void callback_light(CoapPacket &packet, IPAddress ip, int port){
  char response[200];

  
  // extract payload
  char p[packet.payloadlen + 1];
  memcpy(p, packet.payload, packet.payloadlen);
  p[packet.payloadlen] = 0;
  String message(p);


  float l = analogRead(LIGHTPIN);
  
  if(l<200){
    sprintf(response, "Il fait nuit");
  }

  else{
    sprintf(response, "Il fait jour");
  }
  
  if(packet.code==1){ 
    Serial.println("Received GET request for dht sensor.");
    coap.sendResponse(ip, port, packet.messageid, response);
  }
}


void callback_wn(CoapPacket &packet, IPAddress ip, int port){
  char *response;

  Serial.println("Received request for /.well-known/core resources");

  /*
  response = coap.uri.getUris();
  Serial.println(response);
  coap.sendResponse(ip, port, packet.messageid, response);*/

  coap.sendResponse(ip, port, packet.messageid, (char*)"</.well-known/core>, </led>, </dht11>");
}


void setup() {
  //IPAddress myip;
  Serial.begin(9600);

  IPAddress myip(192, 168, 1, 4);
  Ethernet.begin(mac, myip);
  //myip = Ethernet.localIP();
  
  Serial.print("My ip: ");  
  Serial.println(myip);  

  Serial.println("Starting DHT sensor...");
  dht.begin();


  Serial.println("Starting endpoints...");        
  coap.server(callback_temperature, "temperature");
  coap.server(callback_humidity, "humidity");
  coap.server(callback_light, "light");
  coap.server(callback_wn, ".well-known/core");

  coap.start();
  Serial.println("Ready.");
}

void loop() {
  coap.loop();
}
