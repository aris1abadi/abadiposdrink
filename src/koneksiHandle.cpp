#include <WiFi.h>

// extern "C"
//{
// #include "freertos/FreeRTOS.h"
// #include "freertos/timers.h"
// }
#include <AsyncMqttClient.h>
#include <ArduinoJson.h>

#include "mekanikHandle.h"

// kode id pada kategori minum
#define ID_ESTEH      "M07"
#define ID_ESJERUK    "M08"
#define ID_TEHPANAS    "M09"
#define ID_JERUKPANAS   "M10"

#define WIFI_SSID "Lele Bakar"
#define WIFI_PASSWORD "@LesehanPundong#123"

// #define MQTT_HOST         IPAddress(103,172,204,40)
#define MQTT_HOST IPAddress(192, 168, 100, 119)
// #define MQTT_HOST         "abadinet.my.id"        // Broker address
#define MQTT_PORT 2000

const char *PubTopic = "dapur2-resp"; // Topic to publish

const char *SubTopic = "dapur2-cmd";



int antrianCount = 0;

bool mqttReconect = false;
bool wifiReconect = false;

int mqttReconectCount = 0;
int wifiReconectCount = 0;

unsigned long timeDelay = millis();

extern int onProses[100];
extern bool sedangProses;
extern bool isBusy;
extern int prosesCount;

AsyncMqttClient mqttClient;

void connectToWifi()
{
  Serial.println("Connecting to Wi-Fi...");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
}

void connectToMqtt()
{
  Serial.println("Connecting to MQTT...");
  mqttClient.connect();
  
}

void WiFiEvent(WiFiEvent_t event)
{
  switch (event)
  {

  case SYSTEM_EVENT_STA_GOT_IP:
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
    connectToMqtt();
    break;

  case SYSTEM_EVENT_STA_DISCONNECTED:
    Serial.println("WiFi lost connection");
    mqttReconect = false;
    wifiReconect = true;
    // xTimerStop(mqttReconnectTimer, 0); // ensure we don't reconnect to MQTT while reconnecting to Wi-Fi
    // xTimerStart(wifiReconnectTimer, 0);
    break;


  default:
    break;
  }
}

void printSeparationLine()
{
  Serial.println("************************************************");
}

void onMqttConnect(bool sessionPresent)
{
  Serial.print("Connected to MQTT broker: ");
  Serial.print(MQTT_HOST);
  Serial.print(", port: ");
  Serial.println(MQTT_PORT);
  Serial.print("PubTopic: ");
  Serial.println(PubTopic);

  printSeparationLine();
  Serial.print("Session present: ");
  Serial.println(sessionPresent);

  uint16_t packetIdSub = mqttClient.subscribe(SubTopic, 1);
  Serial.print("Subscribing at QoS 2, packetId: ");
  Serial.println(packetIdSub);

  mqttClient.publish(PubTopic, 0, true, "Dapur2 siap");
  Serial.println("Publishing at QoS 0");

  printSeparationLine();
}

void onMqttDisconnect(AsyncMqttClientDisconnectReason reason)
{
  (void)reason;

  Serial.println("Disconnected from MQTT.");

  if (WiFi.isConnected())
  {
    mqttReconect = true;
  }
}

void onMqttSubscribe(const uint16_t &packetId, const uint8_t &qos)
{
  Serial.println("Subscribe acknowledged.");
  Serial.print("  packetId: ");
  Serial.println(packetId);
  Serial.print("  qos: ");
  Serial.println(qos);
}

void onMqttUnsubscribe(const uint16_t &packetId)
{
  Serial.println("Unsubscribe acknowledged.");
  Serial.print("  packetId: ");
  Serial.println(packetId);
}

void onMqttMessage(char *topic, char *payload, const AsyncMqttClientMessageProperties &properties,
                   const size_t &len, const size_t &index, const size_t &total)
{
  (void)payload;
  /*
    Serial.println("Publish received.");
    Serial.print("  topic: ");
    Serial.println(topic);
    Serial.print("  qos: ");

    Serial.print("payload: ");
    Serial.println(payload);
    */
  if (!sedangProses)
  {
    DynamicJsonDocument antrianDapur2(1023); // ESP.getMaxAllocHeap()
    DeserializationError error = deserializeJson(antrianDapur2, payload, len);
    if (error)
    {
      Serial.print(F("deserializeJson() failed: "));
      Serial.println(error.c_str());
    }

    antrianCount = 0;
    prosesCount = 0;
    for (int i = 0; i < antrianDapur2.size(); i++)
    {
      String id_0 = antrianDapur2[i]["id"];
      Serial.print("Id: ");
      Serial.println(id_0);
      Serial.println("------------------------------------");

      for (int a = 0; a < antrianDapur2[i]["item"].size(); a++)
      {

        String item = antrianDapur2[i]["item"][a]["nama"];
        String itemId = antrianDapur2[i]["item"][a]["id"];
        String jml = antrianDapur2[i]["item"][a]["jml"];
        // Serial.print("item: ");
        // Serial.print(item);
        // Serial.print(" ");
        // Serial.println(jml);

        if (!antrianDapur2[i]["item"][a]["isReady"])
        {
          int orderNow = 0;
          if (itemId == ID_ESTEH)
          {
            orderNow = ES_TEH;
          }
          else if (itemId == ID_ESJERUK)
          {
            orderNow = ES_JERUK;
          }
          else if (itemId == ID_JERUKPANAS)
          {
            orderNow = JERUK_PANAS;
          }
          else if (itemId == ID_TEHPANAS)
          {
            orderNow = TEH_PANAS;
          }
          int c = jml.toInt();
          for (int i = 0; i < c; i++)
          {
            onProses[antrianCount] = orderNow;
            antrianCount += 1;
          }
        }
      }
      Serial.println("======================================");
      // tanda antar order
      onProses[antrianCount] = '-';
      antrianCount += 1;
    }
    sedangProses = true;
    prosesBikinMinum();
  }
  else
  {
    Serial.println("Sedang proses Bikin Minum!!! data belum bisa diterima");
  }
}
/*

int menuCount = 0;
    for (int i = 0; i < antrianDapur2.size(); i++)
    {
      String id_0 = antrianDapur2[i]["id"];
      Serial.print("Id: ");
      Serial.println(id_0);
      Serial.println("------------------------------------");

      for (int a = 0; a < antrianDapur2[i]["item"].size(); a++)
      {

        String item = antrianDapur2[i]["item"][a]["nama"];
        String itemId = antrianDapur2[i]["item"][a]["id"];
        String jml = antrianDapur2[i]["item"][a]["jml"];
        // Serial.print("item: ");
        // Serial.print(item);
        // Serial.print(" ");
        // Serial.println(jml);

        if (!antrianDapur2[i]["item"][a]["isReady"])
        {
          int orderNow = 0;
          if (itemId == ID_ESTEH)
          {
            orderNow = ES_TEH;
          }
          else if (itemId == ID_ESJERUK)
          {
            orderNow = ES_JERUK;
          }
          else if (itemId == ID_JERUKPANAS)
          {
            orderNow = JERUK_PANAS;
          }
          else if (itemId == ID_TEHPANAS)
          {
            orderNow = TEH_PANAS;
          }
          int c = jml.toInt();
          for (int i = 0; i < c;)
          {
              //bikinMinum(orderNow);
              Serial.print("Sedang bikin ");
              Serial.println(orderNow);
              i += 1;
          }
        }
      }
      Serial.println("======================================");
    }
  }
*/

void cekProses()
{
  if (sedangProses)
  {
  }
}

void onMqttPublish(const uint16_t &packetId)
{
  Serial.println("Publish acknowledged.");
  Serial.print("  packetId: ");
  Serial.println(packetId);
}

void mqttInit()
{
  // Serial.begin(115200);

  Serial.print("\nStarting FullyFeature_ESP32 on ");
  Serial.println(ARDUINO_BOARD);

  WiFi.onEvent(WiFiEvent);

  mqttClient.onConnect(onMqttConnect);
  mqttClient.onDisconnect(onMqttDisconnect);
  mqttClient.onSubscribe(onMqttSubscribe);
  mqttClient.onUnsubscribe(onMqttUnsubscribe);
  mqttClient.onMessage(onMqttMessage);
  mqttClient.onPublish(onMqttPublish);
  mqttClient.setCleanSession(true);
 

  mqttClient.setServer(MQTT_HOST, MQTT_PORT);

  connectToWifi();
}

void mqttLoop()
{
  if ((millis() - timeDelay) > 1000)
  {
    timeDelay = millis();
    cekProses();

    if (wifiReconect)
    {
      if (++wifiReconectCount > 2)
      {
        connectToWifi();
        wifiReconectCount = 0;
        wifiReconect = false;
      }
    }
    if (mqttReconect)
    {
      if (++mqttReconectCount > 2)
      {
        connectToMqtt();
        mqttReconectCount = 0;
        mqttReconect = false;
      }
    }
  }
}