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
    // Map the measured BPM to the range 70 - 150
    int mappedBPM = map(BPM, 0, 200, 70, 150); // Assuming the sensor might read up to 200 BPM
    heartRate = constrain(mappedBPM, 70, 150); // Constrain to ensure it's within the desired range

    Serial.print("Heart rate: ");
    Serial.println(heartRate);
  }

  // Simulate and send ECG waveform based on the detected heart rate
  simulateAndSendECG(heartRate);

  delay(100);  // Small delay to prevent overwhelming the serial output
}

void simulateAndSendECG(int heartRate) {
  // Calculate RR interval in milliseconds (time between heartbeats)
  float rr_interval = 60000.0 / heartRate;  // RR interval in ms

  // Time for one heartbeat cycle (ECG waveform)
  unsigned long t = millis() % (int)rr_interval;

  // Simulate basic ECG waveform (P wave, QRS complex, T wave)
  float ecg_value = 0.0;

  // P wave (small, smooth bump)
  if (t < 0.1 * rr_interval) {
    ecg_value = 0.2 * exp(-pow((t - 0.05 * rr_interval) / (0.04 * rr_interval), 2));
  }

  // QRS complex (sharp spike)
  else if (t < 0.3 * rr_interval) {
    ecg_value = -0.15 * exp(-pow((t - 0.18 * rr_interval) / (0.015 * rr_interval), 2)) +  // Q wave
                 1.2 * exp(-pow((t - 0.2 * rr_interval) / (0.008 * rr_interval), 2)) -    // R wave
                 0.2 * exp(-pow((t - 0.22 * rr_interval) / (0.012 * rr_interval), 2));    // S wave
  }

  // T wave (small, smooth bump after QRS)
  else if (t < 0.6 * rr_interval) {
    ecg_value = 0.3 * exp(-pow((t - 0.5 * rr_interval) / (0.1 * rr_interval), 2));
  }

  // Send the ECG value to the Serial Plotter
  Serial.println(ecg_value);
}
