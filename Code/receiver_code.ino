// Receiver Code for 4-Channel Wireless Control (Ornithopter Project)
// Receives throttle, pitch, roll, yaw via nRF24L01 and controls motors

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Servo.h>

Servo ch1, ch2, ch3, ch4;

int ch_width_1 = 0;
int ch_width_2 = 0;
int ch_width_3 = 0;
int ch_width_4 = 0;

// Structure to hold incoming signal data
struct Signal {
  byte throttle;
  byte pitch;
  byte roll;
  byte yaw;
};

Signal data;

const uint64_t pipeIn = 0xE9E8F0F0E1LL; // Same address as transmitter
RF24 radio(7, 8); // CE, CSN pins

unsigned long lastRecvTime = 0;

// Reset values if no signal
void ResetData() {
  data.throttle = 127;
  data.pitch = 127;
  data.roll = 127;
  data.yaw = 127;
  digitalWrite(10, LOW);
  digitalWrite(9, LOW);
}

void setup() {
  Serial.begin(9600);

  pinMode(10, OUTPUT); // Motor 1
  pinMode(9, OUTPUT);  // Motor 2

  ch1.attach(2); // Throttle
  ch2.attach(3); // Pitch
  ch3.attach(4); // Roll
  ch4.attach(5); // Yaw

  ResetData();

  radio.begin();
  radio.openReadingPipe(1, pipeIn);
  radio.startListening();

  Serial.println("Receiver Ready");
}

void recvData() {
  while (radio.available()) {
    radio.read(&data, sizeof(Signal));
    lastRecvTime = millis();
  }
}

void loop() {
  recvData();

  // Check for signal timeout
  unsigned long now = millis();
  if (now - lastRecvTime > 1000) {
    ResetData();
    Serial.println("Signal Lost! Resetting...");
  }

  // Map received values to servo pulse widths
  ch_width_1 = map(data.throttle, 0, 255, 1000, 2000);
  ch_width_2 = map(data.pitch,    0, 255, 1000, 2000);
  ch_width_3 = map(data.roll,     0, 255, 1000, 2000);
  ch_width_4 = map(data.yaw,      0, 255, 1000, 2000);

  // Send signals to servos
  ch1.writeMicroseconds(ch_width_1);
  ch2.writeMicroseconds(ch_width_2);
  ch3.writeMicroseconds(ch_width_3);
  ch4.writeMicroseconds(ch_width_4);

  // Motor control using yaw
  if (data.yaw < 80) {
    digitalWrite(9, LOW);
    delay(10);
    digitalWrite(10, HIGH);
    delay(10);
  } else if (data.yaw > 180) {
    digitalWrite(10, LOW);
    delay(10);
    digitalWrite(9, HIGH);
    delay(10);
  } else {
    digitalWrite(10, LOW);
    digitalWrite(9, LOW);
  }

  delay(100);
}
