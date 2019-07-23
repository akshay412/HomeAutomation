
#include <WiFi.h>
#include <WiFiUdp.h>
#include <WiFiServer.h>
#include <WiFiClient.h>


#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"

#define WIFI_SSID "TP-LINK_F05A"
#define WIFI_PASS "sramlr11"

#define MQTT_SERV "io.adafruit.com"
#define MQTT_PORT 1883
#define MQTT_NAME "AdityaGawali"
#define MQTT_PASS "7f4f002ef67f4fd09a88ca8b84b93677"

#define BedroomLight 23
#define KitchenLight 22
//Set up MQTT and WiFi clients
WiFiClient client;
Adafruit_MQTT_Client mqtt(&client, MQTT_SERV, MQTT_PORT, MQTT_NAME, MQTT_PASS);

//Set up the feed you're subscribing to
Adafruit_MQTT_Subscribe Bedroom = Adafruit_MQTT_Subscribe(&mqtt, MQTT_NAME "/f/Bedroom");
Adafruit_MQTT_Subscribe Kitchen = Adafruit_MQTT_Subscribe(&mqtt, MQTT_NAME "/f/Kitchen");


void setup()
{
  Serial.begin(115200);

  //Connect to WiFi
  Serial.print("\n\nConnecting Wifi... ");
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
  }

  Serial.println("OK!");

  //Subscribe to the  feed
  mqtt.subscribe(&Bedroom);
  mqtt.subscribe(&Kitchen);

  pinMode(BedroomLight, OUTPUT);
  pinMode(KitchenLight, OUTPUT);
  digitalWrite(BedroomLight, LOW);
  digitalWrite(KitchenLight, LOW);
}

void loop()
{
  MQTT_connect();
  
  //Read from our subscription queue until we run out, or
  //wait up to 5 seconds for subscription to update
  Adafruit_MQTT_Subscribe * subscription;
  while ((subscription = mqtt.readSubscription(5000)))
  {
    //If we're in here, a subscription updated...
    if (subscription == &Bedroom)
    {
      //Print the new value to the serial monitor
      Serial.print("Bedroom: ");
      Serial.println((char*) Bedroom.lastread);
      
      //If the new value is  "ON", turn the light on.
      //Otherwise, turn it off.
      if (!strcmp((char*) Bedroom.lastread, "ON"))
      {
        //Active low logic
        digitalWrite(BedroomLight, HIGH);
      }
      else
      {
        digitalWrite(BedroomLight, LOW);
      }
    }

    if(subscription == &Kitchen)
    {
      Serial.print("Kitchen: ");
      Serial.println((char*)Kitchen.lastread);
      if(!strcmp((char*)Kitchen.lastread,"ON"))
      {
        digitalWrite(KitchenLight,HIGH);
      }
      else 
      {
        digitalWrite(KitchenLight,LOW);
      }
    }




    
  }

  // ping the server to keep the mqtt connection alive
  if (!mqtt.ping())
  {
    mqtt.disconnect();
  }
}


/***************************************************
  Adafruit MQTT Library ESP8266 Example
  Must use ESP8266 Arduino from:
    https://github.com/esp8266/Arduino
  Works great with Adafruit's Huzzah ESP board & Feather
  ----> https://www.adafruit.com/product/2471
  ----> https://www.adafruit.com/products/2821
  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!
  Written by Tony DiCola for Adafruit Industries.
  MIT license, all text above must be included in any redistribution
 ****************************************************/

void MQTT_connect() 
{
  int8_t ret;

  // Stop if already connected.
  if (mqtt.connected()) 
  {
    return;
  }

  Serial.print("Connecting to MQTT... ");

  uint8_t retries = 3;
  while ((ret = mqtt.connect()) != 0) // connect will return 0 for connected
  { 
       Serial.println(mqtt.connectErrorString(ret));
       Serial.println("Retrying MQTT connection in 5 seconds...");
       mqtt.disconnect();
       delay(5000);  // wait 5 seconds
       retries--;
       if (retries == 0) 
       {
         // basically die and wait for WDT to reset me
         while (1);
       }
  }
  Serial.println("MQTT Connected!");
}
