// OctoWS2811 LED Library (see https://www.pjrc.com/teensy/td_libs_OctoWS2811.html)
#include <OctoWS2811.h>

// Number of channels on the device (OctoWS2811 has 8)
const int CHANNEL_COUNT = 8;

// The maximum number of LEDs per output is fixed at 512. This it because, at 800kHz, it takes
// 30μs to update each LED + 50μs to reset. So, for 512 LEDs, the update time is 15410μs or just 
// above 15ms. This fits inside the 16.6ms allocation for a 60Hz refresh rate. It should be noted
// that the OctoWS2811 updates each output independently. By default, we use 512 but this can be 
// adjusted via serial command.
int ledsPerChannel = 512;

// Default configuration for the LEDs used. It is a combination of color order and speed. These 
// values can be changed over serial.
//
// Color order can be one of: WS2811_RGB, WS2811_RBG, WS2811_GRB, WS2811_GBR, WS2811_BRG,
// WS2811_BGR, WS2811_RGBW, WS2811_RBGW, WS2811_GRBW, WS2811_GBRW, WS2811_BRGW, WS2811_BGRW,
// WS2811_WRGB, WS2811_WRBG, WS2811_WGRB, WS2811_WGBR, WS2811_WBRG, WS2811_WBGR, WS2811_RWGB, 
// WS2811_RWBG, WS2811_GWRB, WS2811_GWBR, WS2811_BWRG, WS2811_BWGR, WS2811_RGWB, WS2811_RBWG, 
// WS2811_GRWB, WS2811_GBWR, WS2811_BRWG or WS2811_BGWR.
//
// Speed can be one of: WS2811_800kHz, WS2811_400kHz or WS2813_800kHz.
int ledConfig = WS2811_GRB | WS2811_800kHz;

// Preparation of display memory and OctoWS2811 pointer
DMAMEM int* displayMemory;
OctoWS2811* leds;

// Creation of serial input buffer. The max buffer size (49154) is for 4096 pixels, each taking 12
// characters (RGB values + commas) plus the initial '>' and the trailing '\n'.
char buffer[49154];

// Current buffer position (for reading)
unsigned int bufferPos = 0;

// Serial connection status
bool connected = false;

void setup() {

  // On the Teensy, the USB hardware initialization happens before setup() is run and the baud rate 
  // used is always the full native USB speed (either 12 or 480 Mbit/sec). So, there is no need to 
  // call Serial.begin(). Details: https://www.pjrc.com/teensy/td_serial.html

  // Configure LEDs
  configureLEDs();

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

void processConfig(char* command) {

  // Expecting format %<color_order>,<speed>,<leds_per_output>\n
  char* colorOrder = strtok(command + 1, ",");
  char* speed = strtok(NULL, ",");
  char* numLEDsStr = strtok(NULL, ",");

  // Set color order
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
  } else if (strcmp(colorOrder, "RGBW") == 0) {
    ledConfig = WS2811_RGBW;
  } else if (strcmp(colorOrder, "RBGW") == 0) {
    ledConfig = WS2811_RBGW;
  } else if (strcmp(colorOrder, "GRBW") == 0) {
    ledConfig = WS2811_GRBW;
  } else if (strcmp(colorOrder, "GBRW") == 0) {
    ledConfig = WS2811_GBRW;
  } else if (strcmp(colorOrder, "BRGW") == 0) {
    ledConfig = WS2811_BRGW;
  } else if (strcmp(colorOrder, "BGRW") == 0) {
    ledConfig = WS2811_BGRW;
  } else if (strcmp(colorOrder, "WRGB") == 0) {
    ledConfig = WS2811_WRGB;
  } else if (strcmp(colorOrder, "WRBG") == 0) {
    ledConfig = WS2811_WRBG;
  } else if (strcmp(colorOrder, "WGRB") == 0) {
    ledConfig = WS2811_WGRB;
  } else if (strcmp(colorOrder, "WGBR") == 0) {
    ledConfig = WS2811_WGBR;
  } else if (strcmp(colorOrder, "WBRG") == 0) {
    ledConfig = WS2811_WBRG;
  } else if (strcmp(colorOrder, "WBGR") == 0) {
    ledConfig = WS2811_WBGR;
  } else if (strcmp(colorOrder, "RWGB") == 0) {
    ledConfig = WS2811_RWGB;
  } else if (strcmp(colorOrder, "RWBG") == 0) {
    ledConfig = WS2811_RWBG;
  } else if (strcmp(colorOrder, "GWRB") == 0) {
    ledConfig = WS2811_GWRB;
  } else if (strcmp(colorOrder, "GWBR") == 0) {
    ledConfig = WS2811_GWBR;
  } else if (strcmp(colorOrder, "BWRG") == 0) {
    ledConfig = WS2811_BWRG;
  } else if (strcmp(colorOrder, "BWGR") == 0) {
    ledConfig = WS2811_BWGR;
  } else if (strcmp(colorOrder, "RGWB") == 0) {
    ledConfig = WS2811_RGWB;
  } else if (strcmp(colorOrder, "RBWG") == 0) {
    ledConfig = WS2811_RBWG;
  } else if (strcmp(colorOrder, "GRWB") == 0) {
    ledConfig = WS2811_GRWB;
  } else if (strcmp(colorOrder, "GBWR") == 0) {
    ledConfig = WS2811_GBWR;
  } else if (strcmp(colorOrder, "BRWG") == 0) {
    ledConfig = WS2811_BRWG;
  } else if (strcmp(colorOrder, "BGWR") == 0) {
    ledConfig = WS2811_BGWR;
  } else {
    Serial.println("Invalid color order");
    return;
  }

  // Set speed
  if (strcmp(speed, "800") == 0) {
    ledConfig |= WS2811_800kHz;
  } else if (strcmp(speed, "400") == 0) {
    ledConfig |= WS2811_400kHz;
  } else if (strcmp(speed, "WS2813_800") == 0) {
    ledConfig |= WS2813_800kHz;
  } else {
    Serial.println("Invalid speed");
    return;
  }

  // Set number of LEDs per output
  int numLEDs = atoi(numLEDsStr);

  if (numLEDs >= 1 && numLEDs <= 512) {
    ledsPerChannel = numLEDs;
  } else {
    Serial.println("Invalid number of LEDs.");
    return;
  }

  // Reinitialize the OctoWS2811 object with the new configuration
  configureLEDs();

}

void configureLEDs() {

  // Free previous OctoWS2811 object (if any exists)
  if (leds != nullptr) delete leds;

  // Create a new OctoWS2811 object with the curently defined config
  displayMemory = (int*)malloc(ledsPerChannel * 6 * sizeof(int));
  leds = new OctoWS2811(ledsPerChannel, displayMemory, NULL, ledConfig);

  // Initialize the object and turn off all LEDs
  leds->begin();
  leds->show();

  // Print the configuration over serial
  sendConfigOverSerial();

}

void updateLEDs() {

  // Skip the ">" character and retrieve the first value (red)
  char *ptr = strtok(buffer + 1, ",");
  int i = 0;

  while (ptr != NULL && i < ledsPerChannel * CHANNEL_COUNT) {

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

void sendConfigOverSerial() {

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
    case WS2811_BRGW:
      colorOrderStr = "BRGW";
      break;
    case WS2811_BGRW:
      colorOrderStr = "BGRW";
      break;
    case WS2811_WRGB:
      colorOrderStr = "WRGB";
      break;
    case WS2811_WRBG:
      colorOrderStr = "WRBG";
      break;
    case WS2811_WGRB:
      colorOrderStr = "WGRB";
      break;
    case WS2811_WGBR:
      colorOrderStr = "WGBR";
      break;
    case WS2811_WBRG:
      colorOrderStr = "WBRG";
      break;
    case WS2811_WBGR:
      colorOrderStr = "WBGR";
      break;
    case WS2811_RWGB:
      colorOrderStr = "RWGB";
      break;
    case WS2811_RWBG:
      colorOrderStr = "RWBG";
      break;
    case WS2811_GWRB:
      colorOrderStr = "GWRB";
      break;
    case WS2811_GWBR:
      colorOrderStr = "GWBR";
      break;
    case WS2811_BWRG:
      colorOrderStr = "BWRG";
      break;
    case WS2811_BWGR:
      colorOrderStr = "BWGR";
      break;
    case WS2811_RGWB:
      colorOrderStr = "RGWB";
      break;
    case WS2811_RBWG:
      colorOrderStr = "RBWG";
      break;
    case WS2811_GRWB:
      colorOrderStr = "GRWB";
      break;
    case WS2811_GBWR:
      colorOrderStr = "GBWR";
      break;
    case WS2811_BRWG:
      colorOrderStr = "BRWG";
      break;
    case WS2811_BGWR:
      colorOrderStr = "BGWR";
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

  // Print the configuration used
  Serial.print("Configuration: ");
  Serial.print(colorOrderStr);
  Serial.print(", ");
  Serial.print(speedStr);
  Serial.print(", ");
  Serial.print(ledsPerChannel);
  Serial.println(" per channel");

}


