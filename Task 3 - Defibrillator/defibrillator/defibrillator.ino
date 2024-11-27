int ledPin = 13;     // Pin where the LED is connected
int switchPin = 2;   // Pin where the switch is connected (adjust as needed)
int lastSwitchState = -1;  // Store the last state of the switch to avoid redundant serial writes

void setup() {
  pinMode(ledPin, OUTPUT);
  pinMode(switchPin, INPUT_PULLUP);  // Set switch pin as input with pull-up resistor
  Serial.begin(9600);                // Start serial communication at 9600 baud
}

void loop() {
  // Check if the switch state has changed
  int switchState = digitalRead(switchPin);  // Read the switch input (HIGH = normal, LOW = abnormal)

  // Only send data if the state has changed
  if (switchState != lastSwitchState) {
    lastSwitchState = switchState;

    if (switchState == HIGH) {
      Serial.println("0");  // Send '0' for normal if the switch is in HIGH position
    } else {
      Serial.println("1");  // Send '1' for abnormal if the switch is in LOW position
    }
  }

  // Check if there's a signal from the Python application to control the LED
  if (Serial.available() > 0) {
    char signal = Serial.read();     // Read the signal from serial port
    if (signal == '1') {
      digitalWrite(ledPin, HIGH);    // Turn on LED for abnormal data
    } else if (signal == '0') {
      digitalWrite(ledPin, LOW);     // Turn off LED for normal data
    }
  }

  delay(100);  // Small delay to debounce the switch and control the communication rate
}
