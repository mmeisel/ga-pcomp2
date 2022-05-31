/*---------------------------------------------------------------------------------------------

  Open Sound Control (OSC) library for the ESP8266/ESP32

  Example for receiving open sound control (OSC) messages on the ESP8266/ESP32
  Send integers '0' or '1' to the address "/led" to turn on/off the built-in LED of the esp8266.

  This example code is in the public domain.

--------------------------------------------------------------------------------------------- */
#ifdef ESP8266
#include <ESP8266WiFi.h>
#else
#include <WiFi.h>
#endif
#include <WiFiUdp.h>
#include <OSCMessage.h>
#include <OSCBundle.h>
#include <OSCData.h>
#include <FastLED.h>

// How many leds are in the strip?
#define NUM_LEDS 72

// Which pin is the data line connected to?
#define DATA_PIN 21

// This is an array of leds.  One item for each led in your strip.
CRGB leds[NUM_LEDS];

char ssid[] = "*****************"; // your network SSID (name)
char pass[] = "*******"; // your network password

// A UDP instance to let us send and receive packets over UDP
WiFiUDP Udp;
const unsigned int localPort = 8888; // local port to listen for UDP packets (here's where we send the packets)

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);

  Serial.begin(57600);

  // Set up the LEDs and tell the controller about them
  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);
  FastLED.setBrightness(40);

  // Connect to WiFi network
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, pass);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");

  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  Serial.println("Starting UDP");
  Udp.begin(localPort);
  Serial.print("Local port: ");
#ifdef ESP32
  Serial.println(localPort);
#else
  Serial.println(Udp.localPort());
#endif

}

void led(OSCMessage &msg) {
  bool turnOn = msg.getFloat(0) > 0.0;
  Serial.print("led:");
  Serial.println(turnOn);

  // Update the whole strip
  for (int i = 0; i < NUM_LEDS; i++) {
    // TouchOSC toggles send either 0.0 or 1.0
    // See https://github.com/FastLED/FastLED/wiki/Pixel-reference for available colors and more
    leds[i] = turnOn ? CRGB::Red : CRGB::Black;
  }
  FastLED.show();
}

void loop() {
  OSCMessage msg;
  int size = Udp.parsePacket();

  if (size > 0) {
    while (size--) {
      msg.fill(Udp.read());
    }
    if (!msg.hasError()) {
      // This will print the raw OSC messages just to make sure you're getting data
      msg.send(Serial);
      Serial.println();
      // TouchOSC "Simple" Layout, Page 1
      // All of these are "float" values between 0 and 1
      // Faders:
      // "/1/fader1"
      // "/1/fader2"
      // "/1/fader3"
      // "/1/fader4"
      // "/1/fader5"
      // Toggles:
      // "/1/toggle1"
      // "/1/toggle2"
      // "/1/toggle3"
      // "/1/toggle4"
      msg.dispatch("/1/toggle1", led);
    } else {
      OSCErrorCode error = msg.getError();
      Serial.print("error: ");
      Serial.println(error);
    }
  }
}
