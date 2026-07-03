// Transmitter Code for 4-Channel Wireless Control (Ornithopter Project)
// Sends throttle, pitch, roll, yaw data via nRF24L01

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

const uint64_t pipeOut = 0xE9E8F0F0E1LL; // Communication pipe

RF24 radio(7, 8); // CE, CSN pins

// Structure to hold control signals
struct Signal {
  byte throttle;
  byte pitch;
  byte roll;
  byte yaw;
};

Signal data;

// Set default neutral values
void ResetData() {
  data.throttle = 127;
  data.pitch = 127;
  data.roll = 127;
  data.yaw = 127;
}

void setup() {
  Serial.begin(9600);
  radio.begin();
  radio.openWritingPipe(pipeOut);
  radio.stopListening();
  ResetData();
  Serial.println("Transmitter Ready");
}

// Map analog joystick values to 0–255 with center at 127
int mapJoystickValues(int val, int lower, int middle, int upper, bool reverse) {
  val = constrain(val, lower, upper);
  if (val < middle) {
    val = map(val, lower, middle, 0, 128);
  } else {
    val = map(val, middle, upper, 128, 255);
  }
  return (reverse ? 255 - val : val);
}

void loop() {
  data.throttle = mapJoystickValues(analogRead(A0), 0, 524, 1023, true);
  data.roll     = mapJoystickValues(analogRead(A1), 0, 524, 1023, true);
  data.pitch    = mapJoystickValues(analogRead(A2), 0, 524, 1023, true);
  data.yaw      = mapJoystickValues(analogRead(A3), 0, 524, 1023, true);

  if (radio.write(&data, sizeof(Signal))) {
    Serial.print("Data Sent - T:");
    Serial.print(data.throttle);
    Serial.print(" R:");
    Serial.print(data.roll);
    Serial.print(" P:");
    Serial.print(data.pitch);
    Serial.print(" Y:");
    Serial.println(data.yaw);
  } else {
    Serial.println("Transmission Failed");
  }

  delay(100);
}
