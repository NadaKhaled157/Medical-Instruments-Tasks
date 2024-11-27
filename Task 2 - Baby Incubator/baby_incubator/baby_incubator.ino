#include <DHT.h>
#include <PulseSensorPlayground.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>  // Include the I2C LCD library
#include <SoftwareSerial.h>

// Set the LCD address to 0x27 for a 16 chars and 2 line display (adjust according to your LCD)
LiquidCrystal_I2C lcd(0x27, 16, 2);

#define REMOTEXY_MODE__SOFTSERIAL

#include <SoftwareSerial.h>

// RemoteXY connection settings 
#define REMOTEXY_SERIAL_RX 3
#define REMOTEXY_SERIAL_TX 2
#define REMOTEXY_SERIAL_SPEED 9600


#include <RemoteXY.h>

// RemoteXY GUI configuration  
#pragma pack(push, 1)  
uint8_t RemoteXY_CONF[] =   // 324 bytes
  { 255,0,0,7,0,61,1,19,0,0,0,0,31,1,106,200,1,1,25,0,
  130,254,237,113,205,27,31,130,251,27,115,182,11,16,129,6,10,55,9,64,
  8,72,101,97,108,116,104,32,83,116,97,116,117,115,0,130,5,40,30,37,
  11,161,129,13,44,13,5,64,16,72,101,97,114,116,0,129,14,49,11,5,
  64,16,82,97,116,101,0,130,39,40,30,37,11,31,130,73,40,30,37,11,
  31,129,42,46,23,4,64,24,84,101,109,112,101,114,97,116,117,114,101,0,
  129,79,46,17,4,64,24,72,117,109,105,100,105,116,121,32,0,74,9,60,
  13,10,1,31,31,64,55,48,0,129,23,64,9,4,0,31,66,77,80,0,
  74,47,57,13,10,1,8,8,64,50,53,46,54,0,129,59,54,4,10,64,
  8,194,176,0,74,78,57,13,10,1,8,8,64,52,54,0,129,91,57,7,
  9,64,8,37,0,130,12,83,30,37,11,31,129,20,89,13,5,64,8,78,
  111,105,115,101,0,129,16,95,23,5,64,8,69,120,105,115,116,101,110,99,
  101,32,0,130,49,83,45,37,11,31,129,62,91,18,6,64,8,83,116,97,
  116,117,115,0,74,54,102,34,9,1,8,161,64,78,111,114,109,97,108,0,
  69,40,158,20,20,0,161,74,12,104,29,7,1,8,8,64,78,111,110,101,
  0,129,23,144,53,7,64,8,65,98,110,111,114,109,97,108,32,83,116,97,
  116,117,115,0 };
  
// this structure defines all the variables and events of your control interface 
struct {

    // output variables
  uint8_t heart_rate_value; // from 0 to 1
  uint8_t temp_value; // from 0 to 1
  uint8_t humidity_value; // from 0 to 1
  uint8_t status_value; // from 0 to 1
  int16_t sound_01; // =0 no sound, else ID of sound, =1001 for example, look sound list in app
  uint8_t noise_value; // from 0 to 1

    // other variable
  uint8_t connect_flag;  // =1 if wire connected, else =0

} RemoteXY;   
#pragma pack(pop)

#define DHTPIN 13
#define soundSensorPin 12
#define DHTTYPE DHT11
#define YELLOWLED 10
#define pulsePin A0
#define relayPin A2
#define S0 4
#define S1 5
#define S2 6
#define S3 7
#define Out 8

int x = 0;
int R, G, B;
int soundDetected = HIGH;
const int threshold = 550;  // Threshold value for heart rate detection
int heartRate = 60;         // Initial heart rate (will be updated by sensor)
float temperature, humidity;
boolean bSound = false;
boolean normal = true;

unsigned long soundStartTime;

DHT dht(DHTPIN, DHTTYPE);
PulseSensorPlayground pulseSensor;
 


void setup() {
  // RemoteXY_Init (); 
  Serial.begin(9600);
  dht.begin();
  pinMode(soundSensorPin, INPUT);
  pinMode(YELLOWLED, OUTPUT);
  pinMode(relayPin, OUTPUT);
  digitalWrite(relayPin, LOW) ;
  pulseSensor.analogInput(pulsePin);
  pulseSensor.setThreshold(threshold);
  if (pulseSensor.begin()) {
    Serial.println("Pulse sensor started successfully!");
  }
  pinMode(S0, OUTPUT);
  pinMode(S1, OUTPUT);
  pinMode(S2, OUTPUT);
  pinMode(S3, OUTPUT);
  pinMode(Out, INPUT); 
  
  //20% scalling
  digitalWrite(S0, HIGH); 
  digitalWrite(S1, LOW);
  
  lcd.init();
  lcd.backlight();  
  // Display initial message
  lcd.setCursor(0, 0);  // Set cursor to the first row, first column
  lcd.print("Sensors Ready...");
  delay(2000);
  lcd.clear();          
}

void loop() {
  // RemoteXY_Handler ();
  // dtostrf(temperature, 0, 1, RemoteXY.temp_value);
  // dtostrf(heartRate, 0, 1, RemoteXY.heart_rate_value); 
  // dtostrf(humidity, 0, 1, RemoteXY.humidity_value);
  // if (bSound){
  //     RemoteXY.noise_value = "None";
  // }
  // else{
  //     RemoteXY.noise_value = "Detected";

  // }
  //I'm not sure if there's some mechanism to calling the functions
  //If the delays in the functions is not enough we might need to use a timer
  //to spread apart the readings of each sensor individually
  // timer.setInterval(5000, calculateTempAndHumidity) //delay of 5000 bet. each function call
  checkForNoise();
  updateTemperatureAndHumidity();
  // checkForNoise();
  initializePulseSensor();
  unsigned long one_minute = millis();
  // if (millis() - one_minute > 10000){
  simulateAndSendECG(heartRate); // Simulate and send ECG waveform based on the detected heart rate
  // }
  updateColor();
  displaySensorData();
  

  // delay(200);  // Small delay to prevent overwhelming the serial output

}

// void loop() {
//   unsigned long currentMillis = millis();

//   // Humidity and Temperature
//   if (currentMillis - dhtSensorPreviousMillis >= dhtSensorInterval) {
//     dhtSensorPreviousMillis = currentMillis;
//     updateTemperatureAndHumidity();
//   }

//   //Sound
//   if (currentMillis - soundSensorPreviousMillis >= soundSensorInterval) {
//     soundSensorPreviousMillis = currentMillis;
//     checkForNoise();
//   }

//   // // Pulse sensor function
//   // if (currentMillis - pulseSensorPreviousMillis >= pulseSensorInterval) {
//   //   pulseSensorPreviousMillis = currentMillis;
//   //   readPulseSensor();
//   // }

//   // // Color sensor function
//   // if (currentMillis - colorSensorPreviousMillis >= colorSensorInterval) {
//   //   colorSensorPreviousMillis = currentMillis;
//   //   readColorSensor();
//   // }
// }

void updateTemperatureAndHumidity(){
   humidity = dht.readHumidity();
   temperature = dht.readTemperature();
  //  Serial.print("Temp: ");
  //  Serial.println(temperature);

   static bool relayState = LOW;  // Track the last state of the relay

   if (temperature < 36) {  // If temperature is below 36 and relay is off
       digitalWrite(relayPin, LOW);  // Turn relay on
       relayState = HIGH;  // Update the relay state
      //  Serial.println("iam in the low temp so i run the lamp");
   } 
   else if (temperature >= 36) {  // If temperature is 36 or above and relay is on
       digitalWrite(relayPin, HIGH);  // Turn relay off
      //  Serial.println("iam in the high temp so i off the lamp");
   }
}


void checkForNoise() {
  soundDetected = digitalRead(soundSensorPin);

  // if (soundDetected == LOW) {
  //  Serial.println("Sound Detected");
  //   digitalWrite(YELLOWLED, HIGH);
  //   // delay(1500);
  //   delay(200);    
  // } else {
  //  Serial.println("No sound detected");
  //   digitalWrite(YELLOWLED, LOW);
  //   // delay(100);
  //   delay(200);
  // }
  //Serial.println("HI");
  if (soundDetected == LOW) { // Sound detected
  //Serial.println("Noise");
        if (!bSound) { // If sound was not already detected
            soundStartTime = millis();
           // Serial.println("Noisy Environment");
            digitalWrite(YELLOWLED, HIGH); // Turn LED on
            bSound = true;
        }
        // Check if sound is still detected for more than 1000 ms
        if (millis() - soundStartTime >= 1000) {
            // Keep the LED on until sound is turned off
            digitalWrite(YELLOWLED, HIGH); // Ensure LED stays on
        }
    } else { // No sound detected
    //Serial.println("No sound");
        if (bSound) { // If sound was detected previously
         //   Serial.println("Quiet Environment");
            digitalWrite(YELLOWLED, LOW); // Turn LED off
            bSound = false; // Reset sound detection status
        }
    }
}

void initializePulseSensor(){
  int BPM = pulseSensor.getBeatsPerMinute();

  // If a new beat is detected, update the heart rate
  if (pulseSensor.sawStartOfBeat()) {
    heartRate = BPM;
  //   Serial.print("Heart rate: ");
  //   Serial.println(heartRate);
  }
}

unsigned long previousMillis = 0;
const unsigned long interval = 50;  // Interval for 50 ms delay

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

  // Check if 50 ms has passed
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    // Update the previous time
    previousMillis = currentMillis;

    // Print the ECG value
    Serial.println(ecg_value);
  }
}


void updateColor(){
  digitalWrite(S2, LOW);
  digitalWrite(S3, LOW);
  x = pulseIn(Out, LOW);
  R= x;
  //Serial.print("R= ");
  //Serial.print(x);
  //Serial.print("  ");
  
  
  // Setting Green
  digitalWrite(S2, HIGH);
  digitalWrite(S3, HIGH);
  x = pulseIn(Out, LOW);
 // Serial.print("G= ");
  G=x;
  //Serial.print(x);
  //Serial.print("  ");
  
  // Setting Blue
  digitalWrite(S2, LOW);
  digitalWrite(S3, HIGH);
  x = pulseIn(Out, LOW);
  //Serial.print("B= ");
  //Serial.print(x);
  B=x;
 // Serial.println("  ");

}

void displaySensorData() {
  static unsigned long lcd_delay_one = 0;
  static unsigned long lcd_delay_two = 0;
  static unsigned long lcd_delay_three = 0;

  unsigned long currentMillis = millis();

  // Display temperature and humidity on the first row
  if (currentMillis - lcd_delay_one >= 2000) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Temp: ");
    lcd.print(temperature);
    lcd.print(" C");

    lcd.setCursor(0, 1);
    lcd.print("Hum: ");
    lcd.print(humidity);
    lcd.print(" %");

    lcd_delay_one = currentMillis; // Update delay timer after executing this block
  }

  // Display noise and BPM on the second row
  if (currentMillis - lcd_delay_two >= 4000) { // Offset timing for next block
    lcd.clear();
    
    lcd.setCursor(0, 0);
    lcd.print("Noise: ");
    if (soundDetected == 1) {
      lcd.print("No Noise");
    } else if (soundDetected == 0) {
      lcd.print("Noisy");
    }
    
    lcd.setCursor(0, 1);
    lcd.print("BPM: ");
    lcd.print(heartRate);

    lcd_delay_two = currentMillis; // Update delay timer after executing this block
  }

  // Display jaundice reading on the third row
  if (currentMillis - lcd_delay_three >= 6000) { // Offset for next block
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Jaundice: ");
    if ((R >= 15 && R <= 60) && (G >= 30 && G <= 85) && (B >= 30 && B <= 100)) {
      lcd.print("+ve");
    } else {
      lcd.print("-ve");
    }
    lcd_delay_three = currentMillis; // Update delay timer after executing this block
  }
}


