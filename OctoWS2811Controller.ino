// OctoWS2811 LED Library (see https://www.pjrc.com/teensy/td_libs_OctoWS2811.html)
#include <OctoWS2811.h>

// Number of channels on the device (this is fixed since OctoWS2811 has 8)
const int CHANNEL_COUNT = 8;

// Hard-coded maximum number of LEDs (heuristically fixed at 768 for now)
const int MAX_LEDS_PER_CHANNEL = 768;

// The actual number of LEDs per output defaults to 512. This it because, at 800kHz (which is 
// currently the most common speed), it takes 30μs to update each LED + 50μs to reset. So, for 
// 512 LEDs, the update time is 15410μs or just above 15ms. This fits inside the 16.6ms
// allocation for a 60Hz refresh rate. It should be noted that the OctoWS2811 updates each 
// output independently. By default, we use 512 but this can be adjusted by sending the 
// appropriate serial command. Note that, using a lower refresh rate than 60Hz, you can use as 
// many as 768 LEDs per channel (this is the hard-coded maximum).
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

// RGB needs 6 bytes per colour channel, RGBW needs 8 (we default to RGB)
int bytesPerChannel = 6;

// Preparation of display memory and OctoWS2811 pointer
DMAMEM int* displayMemory;
OctoWS2811* leds;

// Creation of the serial input buffer. The max buffer size is for 6144 (8 ch. * 768 LEDs) LEDs, 
// each taking up to 16 characters (RGBW values, including commas) plus the initial '>' and the 
// trailing '\n'.
char buffer[CHANNEL_COUNT * MAX_LEDS_PER_CHANNEL * 16 + 2];

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
  char* colourOrder = strtok(command + 1, ",");
  char* speed = strtok(NULL, ",");
  char* numLEDsStr = strtok(NULL, ",");

  // Set color order
  if (strcmp(colourOrder, "RGB") == 0) {
    ledConfig = WS2811_RGB;
    bytesPerChannel = 6;
  } else if (strcmp(colourOrder, "RBG") == 0) {
    ledConfig = WS2811_RBG;
    bytesPerChannel = 6;
  } else if (strcmp(colourOrder, "GRB") == 0) {
    ledConfig = WS2811_GRB;
    bytesPerChannel = 6;
  } else if (strcmp(colourOrder, "GBR") == 0) {
    ledConfig = WS2811_GBR;
    bytesPerChannel = 6;
  } else if (strcmp(colourOrder, "BRG") == 0) {
    ledConfig = WS2811_BRG;
    bytesPerChannel = 6;
  } else if (strcmp(colourOrder, "BGR") == 0) {
    ledConfig = WS2811_BGR;
    bytesPerChannel = 6;
  } else if (strcmp(colourOrder, "RGBW") == 0) {
    ledConfig = WS2811_RGBW;
    bytesPerChannel = 8;
  } else if (strcmp(colourOrder, "RBGW") == 0) {
    ledConfig = WS2811_RBGW;
    bytesPerChannel = 8;
  } else if (strcmp(colourOrder, "GRBW") == 0) {
    ledConfig = WS2811_GRBW;
    bytesPerChannel = 8;
  } else if (strcmp(colourOrder, "GBRW") == 0) {
    ledConfig = WS2811_GBRW;
    bytesPerChannel = 8;
  } else if (strcmp(colourOrder, "BRGW") == 0) {
    ledConfig = WS2811_BRGW;
    bytesPerChannel = 8;
  } else if (strcmp(colourOrder, "BGRW") == 0) {
    ledConfig = WS2811_BGRW;
    bytesPerChannel = 8;
  } else if (strcmp(colourOrder, "WRGB") == 0) {
    ledConfig = WS2811_WRGB;
    bytesPerChannel = 8;
  } else if (strcmp(colourOrder, "WRBG") == 0) {
    ledConfig = WS2811_WRBG;
    bytesPerChannel = 8;
  } else if (strcmp(colourOrder, "WGRB") == 0) {
    ledConfig = WS2811_WGRB;
    bytesPerChannel = 8;
  } else if (strcmp(colourOrder, "WGBR") == 0) {
    ledConfig = WS2811_WGBR;
    bytesPerChannel = 8;
  } else if (strcmp(colourOrder, "WBRG") == 0) {
    ledConfig = WS2811_WBRG;
    bytesPerChannel = 8;
  } else if (strcmp(colourOrder, "WBGR") == 0) {
    ledConfig = WS2811_WBGR;
    bytesPerChannel = 8;
  } else if (strcmp(colourOrder, "RWGB") == 0) {
    ledConfig = WS2811_RWGB;
    bytesPerChannel = 8;
  } else if (strcmp(colourOrder, "RWBG") == 0) {
    ledConfig = WS2811_RWBG;
    bytesPerChannel = 8;
  } else if (strcmp(colourOrder, "GWRB") == 0) {
    ledConfig = WS2811_GWRB;
    bytesPerChannel = 8;
  } else if (strcmp(colourOrder, "GWBR") == 0) {
    ledConfig = WS2811_GWBR;
    bytesPerChannel = 8;
  } else if (strcmp(colourOrder, "BWRG") == 0) {
    ledConfig = WS2811_BWRG;
    bytesPerChannel = 8;
  } else if (strcmp(colourOrder, "BWGR") == 0) {
    ledConfig = WS2811_BWGR;
    bytesPerChannel = 8;
  } else if (strcmp(colourOrder, "RGWB") == 0) {
    ledConfig = WS2811_RGWB;
    bytesPerChannel = 8;
  } else if (strcmp(colourOrder, "RBWG") == 0) {
    ledConfig = WS2811_RBWG;
    bytesPerChannel = 8;
  } else if (strcmp(colourOrder, "GRWB") == 0) {
    ledConfig = WS2811_GRWB;
    bytesPerChannel = 8;
  } else if (strcmp(colourOrder, "GBWR") == 0) {
    ledConfig = WS2811_GBWR;
    bytesPerChannel = 8;
  } else if (strcmp(colourOrder, "BRWG") == 0) {
    ledConfig = WS2811_BRWG;
    bytesPerChannel = 8;
  } else if (strcmp(colourOrder, "BGWR") == 0) {
    ledConfig = WS2811_BGWR;
    bytesPerChannel = 8;
  } else {
    Serial.println("Invalid colour order");
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

  if (numLEDs >= 1 && numLEDs <= MAX_LEDS_PER_CHANNEL) {
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

  // Create a new OctoWS2811 object with the curently defined config. RGB needs 24 bytes, RGBW needs 32 bytes.
  displayMemory = (int*)malloc(ledsPerChannel * bytesPerChannel * sizeof(int));
  leds = new OctoWS2811(ledsPerChannel, displayMemory, NULL, ledConfig);

  // Initialize the object and turn off all LEDs
  leds->begin();
  leds->show();

  // Print the configuration over serial (for debugging purposes)
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
  int colourOrder = ledConfig & 0x3F;
  const char* colourOrderStr;

  switch (colourOrder) {
    case WS2811_RGB:
      colourOrderStr = "RGB";
      break;
    case WS2811_RBG:
      colourOrderStr = "RBG";
      break;
    case WS2811_GRB:
      colourOrderStr = "GRB";
      break;
    case WS2811_GBR:
      colourOrderStr = "GBR";
      break;
    case WS2811_BRG:
      colourOrderStr = "BRG";
      break;
    case WS2811_BGR:
      colourOrderStr = "BGR";
      break;
    case WS2811_RGBW:
      colourOrderStr = "RGBW";
      break;
    case WS2811_RBGW:
      colourOrderStr = "RBGW";
      break;
    case WS2811_GRBW:
      colourOrderStr = "GRBW";
      break;
    case WS2811_GBRW:
      colourOrderStr = "GBRW";
      break;
    case WS2811_BRGW:
      colourOrderStr = "BRGW";
      break;
    case WS2811_BGRW:
      colourOrderStr = "BGRW";
      break;
    case WS2811_WRGB:
      colourOrderStr = "WRGB";
      break;
    case WS2811_WRBG:
      colourOrderStr = "WRBG";
      break;
    case WS2811_WGRB:
      colourOrderStr = "WGRB";
      break;
    case WS2811_WGBR:
      colourOrderStr = "WGBR";
      break;
    case WS2811_WBRG:
      colourOrderStr = "WBRG";
      break;
    case WS2811_WBGR:
      colourOrderStr = "WBGR";
      break;
    case WS2811_RWGB:
      colourOrderStr = "RWGB";
      break;
    case WS2811_RWBG:
      colourOrderStr = "RWBG";
      break;
    case WS2811_GWRB:
      colourOrderStr = "GWRB";
      break;
    case WS2811_GWBR:
      colourOrderStr = "GWBR";
      break;
    case WS2811_BWRG:
      colourOrderStr = "BWRG";
      break;
    case WS2811_BWGR:
      colourOrderStr = "BWGR";
      break;
    case WS2811_RGWB:
      colourOrderStr = "RGWB";
      break;
    case WS2811_RBWG:
      colourOrderStr = "RBWG";
      break;
    case WS2811_GRWB:
      colourOrderStr = "GRWB";
      break;
    case WS2811_GBWR:
      colourOrderStr = "GBWR";
      break;
    case WS2811_BRWG:
      colourOrderStr = "BRWG";
      break;
    case WS2811_BGWR:
      colourOrderStr = "BGWR";
      break;
    default:
      colourOrderStr = "Unknown";
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
  Serial.print(colourOrderStr);
  Serial.print(", ");
  Serial.print(speedStr);
  Serial.print(", ");
  Serial.print(ledsPerChannel);
  Serial.println(" per channel");

}
