#include <WiFiNINA.h>

const int buttonPin = 8;
const int rPin = 13;
const int bPin = 12;
const int gPin = 11;

int buttonState = 0;

const WiFiClient client;
int status = WL_IDLE_STATUS;

void setup() {
  Serial.begin(9600);

  pinMode(rPin, OUTPUT);
  pinMode(bPin, OUTPUT);
  pinMode(gPin, OUTPUT);
  pinMode(buttonPin, INPUT);

  setLed(0, 0, 255);

  const char* ssid = "WiFi-2.4-3ECC";
  const char* pass = "";

  connectToWifi(ssid, pass);

  setLed(255, 0, 0);
}

char serialBuffer[128];
int index = 0;

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

  if (Serial.available() > 0) {
    bool messageFinished = readSerial(serialBuffer, &index);
    if (messageFinished) {
      handleMessage();
      resetSerialBuffer();
    }
  }
}

void configureWifi() {
  Serial.println("WIFI SETTINGS");

  Serial.print("> ssid:");
  char ssidBuffer[128];
  int ssidIndex = 0;
  bool doneReading = false;
  do {
    doneReading = readSerial(ssidBuffer, &ssidIndex);
  } while (!doneReading);

  Serial.println();

  Serial.print("> password:");
  char passwordBuffer[128];
  int passwordIndex = 0;
  doneReading = false;
  do {
    doneReading = readSerial(passwordBuffer, &passwordIndex);  
  } while (!doneReading);

  Serial.println();

  Serial.print("Ssid: ");
  Serial.println(ssidBuffer);
  Serial.print("Password: ");
  Serial.println(passwordBuffer);

  WiFi.end();
  connectToWifi(ssidBuffer, passwordBuffer);
}

void connectToWifi(char* ssid, char* password) {
  status = WL_IDLE_STATUS;

  Serial.print("Attempting connection to ");
  Serial.println(ssid);
  byte connectionAttempts = 0;
  while (status != WL_CONNECTED && connectionAttempts++ < 5) {
    status = WiFi.begin(ssid, password);
  }
  
  if (status != WL_CONNECTED) {
    Serial.println("Wrong ssid/password combination");  
  } else {
    Serial.print("Successfully connected to ");
    Serial.println(ssid);
  }  
}

void handleMessage() {
  if (strcmp(serialBuffer, "config") == 0) {
    configureWifi();
  }
}

bool readSerial(char buffer[], int* index) {
  if (Serial.available() > 0) {
    char charRead = Serial.read();
    if (charRead != '\n') {
      buffer[*index] = charRead;
      *index = *index + 1;
      return false;
    } else {
      buffer[*index] = '\0';
      return true;
    }
  }
}

void resetSerialBuffer() {
  memset(serialBuffer, 0, 128 * (sizeof(char)));
  index = 0;  
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
