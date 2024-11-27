#include <PulseSensorPlayground.h>

const int pulsePin = A0;    // Pulse Sensor connected to analog pin A0
const int threshold = 550;  // Threshold value for heart rate detection
int heartRate = 60;         // Initial heart rate (will be updated by sensor)

// Pulse sensor object
PulseSensorPlayground pulseSensor;

void setup() {
  Serial.begin(9600);  // Start serial communication at 9600 baud
  pulseSensor.analogInput(pulsePin);
  pulseSensor.setThreshold(threshold);

  if (pulseSensor.begin()) {
    Serial.println("Pulse sensor started successfully!");
  }
}

void loop() {
  // Get Beats Per Minute (BPM) from pulse sensor
  int BPM = pulseSensor.getBeatsPerMinute();

  // If a new beat is detected, update the heart rate
  if (pulseSensor.sawStartOfBeat()) {
    heartRate = BPM;
    Serial.print("Heart rate: ");
    Serial.println(heartRate);
  }

  // Simulate and send ECG waveform based on the detected heart rate
  simulateAndSendECG(heartRate);

  delay(200);  // Small delay to prevent overwhelming the serial output
}

void simulateAndSendECG(int heartRate) {
  // Calculate RR interval in milliseconds (time between heartbeats)
  float rr_interval = 60000.0 / heartRate;  // RR interval in ms

  // Time for one heartbeat cycle (ECG waveform)
  unsigned long t = millis() % (int)rr_interval;

  // Simulate basic ECG waveform (P wave, QRS complex, T wave)
  float ecg_value = 0.0;

  // P wave (small bump)
  if (t < 0.1 * rr_interval) {
    ecg_value = 0.1 * exp(-pow((t - 0.1 * rr_interval) / (0.015 * rr_interval), 2));
  }

  // QRS complex (sharp spike)
  else if (t < 0.3 * rr_interval) {
    ecg_value = -0.1 * exp(-pow((t - 0.25 * rr_interval) / (0.002 * rr_interval), 2)) +
                 1.5 * exp(-pow((t - 0.3 * rr_interval) / (0.005 * rr_interval), 2)) -
                 0.15 * exp(-pow((t - 0.35 * rr_interval) / (0.003 * rr_interval), 2));
  }

  // T wave (small bump after QRS)
  else if (t < 0.6 * rr_interval) {
    ecg_value = 0.2 * exp(-pow((t - 0.6 * rr_interval) / (0.025 * rr_interval), 2));
  }

  // Send the ECG value to the Serial Plotter
  Serial.println(ecg_value);
}
