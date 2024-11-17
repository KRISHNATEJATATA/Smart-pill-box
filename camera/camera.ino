#include "esp_camera.h"
#include "WiFi.h"
#include "Arduino.h"
#include <WebSocketsClient.h>

// Replace with your network credentials
const char* ssid = "wifi ssid ";
const char* password = "password";

// WebSocket client instance
WebSocketsClient webSocket;

#define BUTTON_PIN 14  // GPIO pin for the button
#define WEBSOCKET_SERVER_IP "ip address"
#define WEBSOCKET_SERVER_PORT 80

// Pin definition for CAMERA_MODEL_AI_THINKER
#define PWDN_GPIO_NUM     32
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM      0
#define SIOD_GPIO_NUM     26
#define SIOC_GPIO_NUM     27

#define Y9_GPIO_NUM       35
#define Y8_GPIO_NUM       34
#define Y7_GPIO_NUM       39
#define Y6_GPIO_NUM       36
#define Y5_GPIO_NUM       21
#define Y4_GPIO_NUM       19
#define Y3_GPIO_NUM       18
#define Y2_GPIO_NUM        5
#define VSYNC_GPIO_NUM    25
#define HREF_GPIO_NUM     23
#define PCLK_GPIO_NUM     22

void setup() {
  // Disable brownout detector
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);
  Serial.begin(115200);

  // Initialize the camera
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;

  if(psramFound()){
    config.frame_size = FRAMESIZE_UXGA;
    config.jpeg_quality = 10;
    config.fb_count = 2;
  } else {
    config.frame_size = FRAMESIZE_SVGA;
    config.jpeg_quality = 12;
    config.fb_count = 1;
  }

  // Init Camera
  if (esp_camera_init(&config) != ESP_OK) {
    Serial.println("Camera init failed");
    return;
  }

  // Initialize WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi connected");

  // Setup WebSocket connection to the laptop
  webSocket.begin(WEBSOCKET_SERVER_IP, WEBSOCKET_SERVER_PORT, "/");
  webSocket.onEvent(webSocketEvent);

  // Initialize button
  pinMode(BUTTON_PIN, INPUT_PULLUP);
}

void loop() {
  // Listen for WebSocket events
  webSocket.loop();

  // Check if the button is pressed
  if (digitalRead(BUTTON_PIN) == LOW) {
    // Debounce delay
    delay(50);
    if (digitalRead(BUTTON_PIN) == LOW) {
      // Take picture and send over WebSocket
      takeAndSendPicture();
    }
    // Wait for button to be released
    while (digitalRead(BUTTON_PIN) == LOW);
  }
}

// Take a picture and send it over WebSocket
void takeAndSendPicture() {
  camera_fb_t* fb = esp_camera_fb_get();
  if (!fb) {
    Serial.println("Camera capture failed");
    return;
  }

  // Send the image over WebSocket
  webSocket.sendBIN(fb->buf, fb->len);
  Serial.println("Image sent over WebSocket");

  // Return the frame buffer back to the driver
  esp_camera_fb_return(fb);
}

// WebSocket event handler
void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {
  if (type == WStype_DISCONNECTED) {
    Serial.println("WebSocket Disconnected");
  } else if (type == WStype_CONNECTED) {
    Serial.println("WebSocket Connected");
  }
}
