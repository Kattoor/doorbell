#include <WiFiNINA.h>

const int buttonPin = 8;
const int rPin = 13;
const int bPin = 12;
const int gPin = 11;

int buttonState = 0;
const WiFiClient client;

void setup() {
  pinMode(rPin, OUTPUT);
  pinMode(bPin, OUTPUT);
  pinMode(gPin, OUTPUT);
  pinMode(buttonPin, INPUT);

  setLed(0, 0, 255);

  const char* ssid = "WiFi-2.4-3ECC";
  const char* pass = "";
  int status = WL_IDLE_STATUS;
  while (status != WL_CONNECTED) {
    status = WiFi.begin(ssid, pass);
  }

  setLed(255, 0, 0);
}

void loop() {
  int previousButtonState = buttonState;
  buttonState = digitalRead(buttonPin);

  boolean buttonPressed = buttonState == LOW && previousButtonState == HIGH;

  if (buttonPressed) {
    sendNotification();
    setLed(0, 255, 0);
    delay(2000);
    setLed(255, 0, 0);
  }
}

void setLed(byte red, byte green, byte blue) {
  analogWrite(rPin, red);
  analogWrite(gPin, green);
  analogWrite(bPin, blue);
}

void sendNotification() {
  if (client.connect("wirepusher.com", 80)) {    
      client.println("GET /send?id=&title=doorbell&message=open%20uw%20deur%21&type=doorbell HTTP/1.1");
      client.println("Host: wirepusher.com");
      client.println("Connection: close");
      client.println();
    }
}
