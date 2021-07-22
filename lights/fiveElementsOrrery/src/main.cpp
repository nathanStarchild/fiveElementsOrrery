#include <Arduino.h>
#include <painlessMesh.h>
#include <FastLED.h>

#define BUILTIN_LED 2
#define DATA_PIN 5

#define   MESH_SSID       "Orrery"
#define   MESH_PASSWORD   "airEarthFireMetalWater"
#define   MESH_PORT       1045

#define NUM_LEDS 512
#define INTERVAL 20000 //50 fps

CRGB leds[NUM_LEDS];
CRGB oldLeds[NUM_LEDS];
CRGB outLeds[NUM_LEDS];

painlessMesh  mesh;
bool calc_delay = false;
SimpleList<uint32_t> nodes;

// WS2815 - 220ns, 360ns, 220ns
// template <uint8_t DATA_PIN, EOrder RGB_ORDER = RGB>
// class WS2815Controller : public ClocklessController<DATA_PIN, C_NS(220), C_NS(360), C_NS(220), RGB_ORDER> {};

// template<uint8_t DATA_PIN, EOrder RGB_ORDER> class WS2815 : public WS2815Controller<DATA_PIN, RGB_ORDER> {};

// FastLED.addLeds<WS2815, RX, GRB>(leds, NUM_LEDS);

void setup() {

  pinMode(BUILTIN_LED, OUTPUT);
  pinMode(DATA_PIN, OUTPUT);

  Serial.begin(9600);
  while (!Serial) { }
  Serial.println("ESP32 Startup");

  FastLED.addLeds<WS2813, DATA_PIN, RGB>(leds, NUM_LEDS); 

  mesh.setDebugMsgTypes(ERROR | DEBUG);  // set before init() so that you can see startup messages
  mesh.init(MESH_SSID, MESH_PASSWORD, MESH_PORT);
  mesh.onReceive(&receivedCallback);
  mesh.onNewConnection(&newConnectionCallback);
  mesh.onChangedConnections(&changedConnectionCallback);
  mesh.onNodeTimeAdjusted(&nodeTimeAdjustedCallback);
  mesh.onNodeDelayReceived(&delayReceivedCallback);
  
}

void loop() {
  Serial.println("Yo");
  delay(1000);
}

void receivedCallback(uint32_t from, String &msg) {
  Serial.printf("Received from %u msg=%s\n", from, msg.c_str());
  // msgp = msg;
  // inbox = true;
}

void newConnectionCallback(uint32_t nodeId) {
  Serial.printf("New Connection, nodeId = %u\n", nodeId);
  // alone = false;
  // if (audienceSpot == 1) {
  //   boredTimer.resetTimer();
  //   stdMessage(35);//sync everyone's timer.
  // }
}

void changedConnectionCallback() {
  Serial.printf("Changed connections %s\n", mesh.subConnectionJson().c_str());

  // nodes = mesh.getNodeList();

  // Serial.printf("Num nodes: %d\n", nodes.size());
  // Serial.printf("Connection list:");

  // SimpleList<uint32_t>::iterator node = nodes.begin();
  // while (node != nodes.end()) {
  //   Serial.printf(" %u", *node);
  //   node++;
  // }
  // Serial.println();
  // calc_delay = true;
}

void nodeTimeAdjustedCallback(int32_t offset) {
  Serial.printf("Adjusted time %u. Offset = %d\n", mesh.getNodeTime(), offset);
  //!!! deal with pattnerstate updatetime here
  // mainState.nextUpdate = mesh.getNodeTime() + INTERVAL;
}

void delayReceivedCallback(uint32_t from, int32_t delay) {
  Serial.printf("Delay to node %u is %d us\n", from, delay);
}