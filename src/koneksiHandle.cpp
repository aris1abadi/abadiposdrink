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
#define ID_ESTEH "M07"
#define ID_ESJERUK "M08"
#define ID_TEHPANAS "M09"
#define ID_JERUKPANAS "M10"

#define WIFI_SSID "Lele Bakar"
#define WIFI_PASSWORD "@LesehanPundong#123"

#define MQTT_HOST IPAddress(103, 172, 204, 40)
// #define MQTT_HOST IPAddress(192, 168, 100, 119)
//  #define MQTT_HOST         "abadinet.my.id"        // Broker address
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
extern bool stepFinish;

extern bool orderReady;
extern int orderCount;
String orderId[10];

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
    // Serial.println("IP address: ");
    // Serial.println(WiFi.localIP());
    connectToMqtt();
    break;

  case SYSTEM_EVENT_STA_DISCONNECTED:
    Serial.println("WiFi Putus");
    mqttReconect = false;
    wifiReconect = true;
    // xTimerStop(mqttReconnectTimer, 0); // ensure we don't reconnect to MQTT while reconnecting to Wi-Fi
    // xTimerStart(wifiReconnectTimer, 0);
    break;

  default:
    break;
  }
}

void onMqttConnect(bool sessionPresent)
{
  Serial.println("Terhubung ke Server pos ");

  uint16_t packetIdSub = mqttClient.subscribe(SubTopic, 1);
  mqttClient.publish(PubTopic, 1, true, "Dapur2 siap");
}

void onMqttDisconnect(AsyncMqttClientDisconnectReason reason)
{
  (void)reason;

  Serial.println("Putus dari Server.");

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
    DynamicJsonDocument antrianDapur2(1023);
    DeserializationError error = deserializeJson(antrianDapur2, payload, len);
    if (error)
    {
      Serial.print(F("deserializeJson() failed: "));
      Serial.println(error.c_str());
    }

    antrianCount = 0;
    prosesCount = 0;
    orderCount = 0;
    // for(int i = 0;i < 100;i++){
    //  onProses[i] = 0;
    //}

    for (int i = 0; i < antrianDapur2.size(); i++)
    {
      String id_0 = antrianDapur2[i]["id"];
      // Serial.print("Id: ");
      // Serial.println(id_0);

      for (int a = 0; a < antrianDapur2[i]["item"].size(); a++)
      {

        String item = antrianDapur2[i]["item"][a]["nama"];
        String itemId = antrianDapur2[i]["item"][a]["id"];
        String jml = antrianDapur2[i]["item"][a]["jml"];

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

      // tanda antar order
      if (antrianCount > 0)
      {
        onProses[antrianCount] = '+';
        orderId[orderCount] = id_0;
        orderCount += 1;
        antrianCount += 1;
      }
    }
    if (antrianCount > 0)
    {
      Serial.println("---------Pelanggan order----------");
      for (int i = 0; i < orderCount; i++)
      {
        Serial.print((orderCount + 1));
        Serial.print(". ");
        Serial.println(orderId[i]);
      }
      Serial.println("----------------------------------");
      onProses[antrianCount] = '-';
      sedangProses = true;
      stepFinish = true;
      orderCount = 0;
      prosesBikinMinum();
    }
  }
  else
  {
    Serial.println("Sedang proses (antrian baru dalam antrian)");
  }
}

void sendChange(String type, String content)
{
  String msg = type;
  msg += ';';
  msg += content;
  mqttClient.publish(PubTopic, 1, true, msg.c_str());
}

void cekProses()
{
  if (orderReady)
  {
    if (sedangProses)
    {
      sendChange("nextProses", orderId[orderCount]);
      orderReady = false;
      orderCount += 1;
    }
    else
    {
      sendChange("nextOrder", orderId[orderCount]);
      orderReady = false;
    }
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
  // mqttClient.onSubscribe(onMqttSubscribe);
  // mqttClient.onUnsubscribe(onMqttUnsubscribe);
  mqttClient.onMessage(onMqttMessage);
  // mqttClient.onPublish(onMqttPublish);
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