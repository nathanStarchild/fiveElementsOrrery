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
#define   BLINK_PERIOD    3000000 // microseconds until cycle repeat
#define   BLINK_DURATION  100000  // microseconds LED is on for

CRGB leds[NUM_LEDS];
CRGB oldLeds[NUM_LEDS];
CRGB outLeds[NUM_LEDS];

painlessMesh  mesh;

typedef struct patternState {
  uint32_t lastUpdate;
  uint32_t nextUpdate;
  bool stale;
  uint8_t hue;
  uint8_t fadeRate;
  uint16_t patternStep;
  uint16_t stepRate;
  uint16_t frameCount;
};
patternState mainState = {0, 0, true, 0, 100, 0, 1, 0};

typedef struct messageState {
  bool inbox;
  bool outbox;
  String currentMessage;
  String newMessage;
};
messageState messaging = {false, false,};

typedef struct networkState {
  bool calc_delay;
  bool alone;
  bool ledOn;
  SimpleList<uint32_t> nodes;
};
networkState network = {false, true, false};


#include <pattern.h>
#include <rainbow.h>

void updateStatusLed();
void blendFrames();
void updatePatterns();
void processMessage();
void receivedCallback(uint32_t from, String &msg);
void newConnectionCallback(uint32_t nodeId);
void changedConnectionCallback();
void delayCalcs();
void nodeTimeAdjustedCallback(int32_t offset);
void delayReceivedCallback(uint32_t from, int32_t delay);




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
  // network housekeeping
  mesh.update();
  if (network.calc_delay) {
    delayCalcs();
  }
  updateStatusLed();

  // output to the lights
  if ( (int) mainState.nextUpdate - (int) mesh.getNodeTime() < 0) {
    blendFrames();
    FastLED.show();
    mainState.lastUpdate = mainState.nextUpdate;
    mainState.nextUpdate = mainState.lastUpdate + INTERVAL;
    mainState.stale = true;
  }

  // messaging housekeeping
  if (messaging.inbox) {
    // boredTimer.resetTimer();
    // last_message = millis();
    processMessage();
    messaging.inbox = false;
  }
  if (messaging.outbox) {
    mesh.sendBroadcast(messaging.newMessage);
    messaging.outbox = false;
  }
  
  // pattern housekeeping
  if (mainState.stale) {
    updatePatterns(); 
    mainState.stale = false;
  }
}

void updateStatusLed() {
  // run the blinking status light
  network.ledOn = true;
  uint32_t cycleTime = mesh.getNodeTime() % BLINK_PERIOD;
  for (uint8_t i = 0; i < (mesh.getNodeList().size() + 1); i++) {
    uint32_t onTime = BLINK_DURATION * i * 2;
    if (cycleTime > onTime && cycleTime < onTime + BLINK_DURATION)
      network.ledOn = false;
  }
  digitalWrite(BUILTIN_LED, network.ledOn);
}

void blendFrames(){
  uint8_t ratio = map(mainState.frameCount % mainState.stepRate, 0, mainState.stepRate-1, 0, 255);
  for (int i = 0; i < NUM_LEDS; i++) {
    outLeds[i] = blend( oldLeds[i], leds[i], ratio );
//      outLeds[i] = leds[(i + 10) % NUM_LEDS];
  }
}

void updatePatterns() {
  // pass
}

void processMessage() {
  // pass
}

void receivedCallback(uint32_t from, String &msg) {
  Serial.printf("Received from %u msg=%s\n", from, msg.c_str());
  messaging.currentMessage = msg;
  messaging.inbox = true;
}

void newConnectionCallback(uint32_t nodeId) {
  Serial.printf("New Connection, nodeId = %u\n", nodeId);
  network.alone = false;
  // if (audienceSpot == 1) {
  //   boredTimer.resetTimer();
  //   stdMessage(35);//sync everyone's timer.
  // }
}

void changedConnectionCallback() {
  Serial.printf("Changed connections %s\n", mesh.subConnectionJson().c_str());

  network.nodes = mesh.getNodeList();
  Serial.printf("Num nodes: %d\n", network.nodes.size());
  if (network.nodes.size() == 0) {
    network.alone = true;
  } else {
    Serial.printf("Connection list:");

    SimpleList<uint32_t>::iterator node = network.nodes.begin();
    while (node != network.nodes.end()) {
      Serial.printf(" %u", *node);
      node++;
    }
    Serial.println();
    network.calc_delay = true;
  }
}

void delayCalcs() {
  SimpleList<uint32_t>::iterator node = network.nodes.begin();
  while (node != network.nodes.end()) {
    mesh.startDelayMeas(*node);
    node++;
  }
  network.calc_delay = false;  
}

void nodeTimeAdjustedCallback(int32_t offset) {
  Serial.printf("Adjusted time %u. Offset = %d\n", mesh.getNodeTime(), offset);
  //!!! deal with pattnerstate updatetime here
  mainState.nextUpdate += offset;
}

void delayReceivedCallback(uint32_t from, int32_t delay) {
  Serial.printf("Delay to node %u is %d us\n", from, delay);
}