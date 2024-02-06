/* Fill-in information from Blynk Device Info here */
#define BLYNK_TEMPLATE_ID "TMPL6OKtbpfOr"
#define BLYNK_TEMPLATE_NAME "Pemberi Pakan Otomatis Ayam"
#define BLYNK_AUTH_TOKEN "mbEGLsVAVNygUZ5YZayAmDhVKe5ETTDt"

/* Comment this out to disable prints and save space */
#define BLYNK_PRINT Serial

#include <ESP8266_Lib.h>
#include <BlynkSimpleShieldEsp8266.h>
#include <Servo.h>

Servo servo;
int pos = 0;    // variable to store the servo position
bool servoMoving = false;

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = ".";
char pass[] = "Ricky123";

// Hardware Serial on Mega, Leonardo, Micro...
//#define EspSerial Serial1

// or Software Serial on Uno, Nano...
#include <SoftwareSerial.h>
SoftwareSerial EspSerial(3, 2); // RX, TX

// Your ESP8266 baud rate:
#define ESP8266_BAUD 9600

ESP8266 wifi(&EspSerial);
BlynkTimer timer;

// Define component pins
#define trigPin 6
#define echoPin 5

int previousPakan = 0;
bool ultrasonicActive = false;

BLYNK_WRITE(V1)
{
  if (!servoMoving) {
    servoMoving = true; // indicate that servo is moving
    servo.write(param.asInt());
    servoMoving = false; /s/ indicate that servo has stopped moving
  }
}

BLYNK_WRITE(V2)
{
  int buttonState = param.asInt(); // Mendapatkan status tombol dari parameter

  if (!servoMoving) {
    servoMoving = true; // Menandakan bahwa servo sedang bergerak
    if (buttonState == HIGH) {
      servo.write(49); // Membuka servo sebanyak 102 derajat
    } else {
      servo.write(5); // Menutup servo
    }
    servoMoving = false; // Menandakan bahwa servo telah berhenti bergerak
  }
}

BLYNK_WRITE(V3)
{
  servoMoving = true; // indicate that servo is moving
  servo.write(49);
  delay(3000);
  servo.write(5);
  servoMoving = false; // indicate that servo has stopped moving
}

BLYNK_WRITE(V4)
{
  if (param.asInt() == 1) {
    ultrasonicActive = true; // Mengaktifkan sensor ultrasonik
  } else {
    ultrasonicActive = false; // Menonaktifkan sensor ultrasonik
  }
}

BLYNK_WRITE(V5)
{
  int servoPos = param.asInt(); // Mendapatkan posisi servo dari parameter
  if (servoPos >= 5 && servoPos <= 49) {
    if (!servoMoving) {
      servoMoving = true; // Menandakan bahwa servo sedang bergerak
      servo.write(servoPos);
      servoMoving = false; // Menandakan bahwa servo telah berhenti bergerak
    }
  }
}

void setup()
{
  Serial.begin(9600);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  servo.attach(9);
  // Debug console
  EspSerial.begin(ESP8266_BAUD);

  Blynk.begin(BLYNK_AUTH_TOKEN, wifi, ssid, pass);
  // You can also specify server:
  //Blynk.begin(BLYNK_AUTH_TOKEN, wifi, ssid, pass, "blynk.cloud", 80);
  //Blynk.begin(BLYNK_AUTH_TOKEN, wifi, ssid, pass, IPAddress(192,168,1,100), 8080);
}

void loop()
{
  Blynk.run();
  if (ultrasonicActive) {
    long duration, ketinggian, pakan;
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);
    duration = pulseIn(echoPin, HIGH);
    ketinggian = (duration / 2) / 29.1;
    pakan = 36 - ketinggian;

    if (pakan != previousPakan) {
      Serial.println("ketinggian pakan :");
      Serial.print(pakan);
      Serial.println(" cm");
      Blynk.virtualWrite(V0, pakan);
      previousPakan = pakan;
    }
  }
  delay(2000);
}
