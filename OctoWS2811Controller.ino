// OctoWS2811 LED Library (see https://www.pjrc.com/teensy/td_libs_OctoWS2811.html)
#include <OctoWS2811.h>

// The maximum number of LEDs per output is fixed at 512. This it because, at 800kHz, it takes
// 30μs to update each LED + 50μs to reset. So, for 512 LEDs, the update time is 15410μs or just 
// above 15ms. This fits inside the 16.6ms allocation for a 60Hz refresh rate. It should be noted
// that the OctoWS2811 updates each output independently.
const int LEDS_PER_OUTPUT = 512;
const int LEDS_TOTAL = LEDS_PER_OUTPUT * 8;

// Default configuration of the LEDs used. Color order is one of WS2811_RGB, WS2811_RBG, WS2811_GRB
// or WS2811_GBR and speed is either WS2811_800kHz or WS2811_400kHz. This can be changed over serial
// by sending the appropriate command (e.g. %RGB,400\n)
int ledConfig = WS2811_GRB | WS2811_800kHz;

// Preparation of a block of memory for memory display and assignment of pointer for OctoWS2811 
// object.
DMAMEM int displayMemory[LEDS_PER_OUTPUT * 6];
OctoWS2811* leds;

// Creation of serial input buffer. The max buffer size (49154) is for 4096 pixels, each taking 12
// characters (RGB values + commas) plus the initial '>' and the trailing '\n'.
char buffer[49154]; 
unsigned int bufferPos = 0;

// Serial connection status
bool connected = false;

void setup() {

  // On the Teensy, the USB hardware initialization happens before setup() is run and the baud rate 
  // used is always the full native USB speed (either 12 or 480 Mbit/sec). So, there is no need to 
  // call Serial.begin(). Details: https://www.pjrc.com/teensy/td_serial.html

  // Initialize object and update LEDs
  initLEDs();

}

void loop() {

  // Check connection state
  if (Serial.dtr()) {
    if (!connected) {
      connected = true;
      Serial.println("Connected");
    } 
  } else {
    if (connected) connected = false;
    delay(1); 
    return;
  }

  // Read serial input
  while (Serial.available()) {

    char c = Serial.read();
    
    if (c == '\n') {

      // If trailing \n is found, we can process the data and then reset the buffer position.
      buffer[bufferPos] = '\0';
      processData();
      bufferPos = 0;

    } else if (bufferPos < sizeof(buffer) - 1) {

      // Read and store character
      buffer[bufferPos++] = c;

    } else {

      // Buffer overflow protection: reset the buffer if it exceeds the size
      Serial.println("Buffer overflow, resetting.");
      bufferPos = 0;

    }

  }

}

void processData() {

  // Check if this is a configuration command (starts with '%') or an LED update command (starts
  // with '>')
  if (buffer[0] == '%') {
    processConfig(buffer);
  } else if (buffer[0] == '>') {
    updateLEDs();
  }

}

void updateLEDs() {

  // Skip the ">" character and retrieve the first value (red)
  char *ptr = strtok(buffer + 1, ",");
  int i = 0;

  while (ptr != NULL && i < LEDS_TOTAL) {

    int r = atoi(ptr);
    ptr = strtok(NULL, ",");
    if (ptr == NULL) break;

    int g = atoi(ptr);
    ptr = strtok(NULL, ",");
    if (ptr == NULL) break;

    int b = atoi(ptr);
    ptr = strtok(NULL, ",");

    leds->setPixel(i, r, g, b);
    i++;

  }

  // Check if OctoWS2811 is busy and, if not, update the LEDs. When using 400kHz LEDs, this may
  // skip updates but at least it prevents the serial buffer from overflowing.
  if (!leds->busy()) leds->show();

}

void processConfig(char* command) {

  // Expecting format %<colour_order>,<speed>\n
  char* colorOrder = strtok(command + 1, ",");
  char* speed = strtok(NULL, ",");

  // Set colour order
  if (strcmp(colorOrder, "RGB") == 0) {
    ledConfig = WS2811_RGB;
  } else if (strcmp(colorOrder, "RBG") == 0) {
    ledConfig = WS2811_RBG;
  } else if (strcmp(colorOrder, "GRB") == 0) {
    ledConfig = WS2811_GRB;
  } else if (strcmp(colorOrder, "GBR") == 0) {
    ledConfig = WS2811_GBR;
  } else if (strcmp(colorOrder, "BRG") == 0) {
    ledConfig = WS2811_BRG;
  } else if (strcmp(colorOrder, "BGR") == 0) {
    ledConfig = WS2811_BGR;
  } else {
    Serial.println("Invalid color order");
    return;
  }

  // Set speed
  if (strcmp(speed, "800") == 0) {
    ledConfig |= WS2811_800kHz;
  } else if (strcmp(speed, "400") == 0) {
    ledConfig |= WS2811_400kHz;
  } else {
    Serial.println("Invalid speed");
    return;
  }

  // Reinitialize the OctoWS2811 object with the new configuration
  initLEDs();

}

void initLEDs() {

  // Free previous OctoWS2811 object (if any exists)
  if (leds != nullptr) delete leds;

  // Create a new OctoWS2811 object with the curently defined config
  leds = new OctoWS2811(LEDS_PER_OUTPUT, displayMemory, NULL, ledConfig);

  // Extract and print color order (mask out the speed bits with 0x3F)
  int colorOrder = ledConfig & 0x3F;
  const char* colorOrderStr;

  switch (colorOrder) {
    case WS2811_RGB:
      colorOrderStr = "RGB";
      break;
    case WS2811_RBG:
      colorOrderStr = "RBG";
      break;
    case WS2811_GRB:
      colorOrderStr = "GRB";
      break;
    case WS2811_GBR:
      colorOrderStr = "GBR";
      break;
    case WS2811_BRG:
      colorOrderStr = "BRG";
      break;
    case WS2811_BGR:
      colorOrderStr = "BGR";
      break;
    case WS2811_RGBW:
      colorOrderStr = "RGBW";
      break;
    case WS2811_RBGW:
      colorOrderStr = "RBGW";
      break;
    case WS2811_GRBW:
      colorOrderStr = "GRBW";
      break;
    case WS2811_GBRW:
      colorOrderStr = "GBRW";
      break;
    default:
      colorOrderStr = "Unknown";
      break;
  }

  // Extract and print speed configuration
  int speedConfig = ledConfig & 0xC0;  // Mask for the speed bits (top two bits)
  const char* speedStr;

  switch (speedConfig) {
    case WS2811_800kHz:
      speedStr = "800kHz";
      break;
    case WS2811_400kHz:
      speedStr = "400kHz";
      break;
    case WS2813_800kHz:
      speedStr = "800kHz (WS2813)";
      break;
    default:
      speedStr = "Unknown";
      break;
  }

  // Print out the configuration used
  Serial.print("Configuration: ");
  Serial.print(colorOrderStr);
  Serial.print(", ");
  Serial.println(speedStr);

  // Initialize the object and turn off all LEDs
  leds->begin();
  leds->show();

}
