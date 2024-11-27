# Medical Instruments Tasks
This repository is a documentation for Medical Instruments Tasks for third year biomedical engineering students in Cairo University (Class of 2026).

## Task 1: ECG Simulation
We collected raw data from pulse sensor and filtered it to contain normal ranges then simulated an ECG curve based on the measured values using a collection of equations with common values for QRS to simulate the pulses based on the measured heartrate. This requires working with python as Arduino IDE cannot handle csv files. Alternatively, you can plot the simulated ECG based on the raw data directly in Arduino IDE. In both cases, it won't be very accurate, but it's meant as a simulation.
![image](https://github.com/user-attachments/assets/9d8fbbdc-dcab-4f85-96d3-a6a31e0f0ad5)

## Task 2: Baby Incubator
Here's another useful repo from class of 2025: https://github.com/Sondos-Mahmoud/Baby-incubator-device--main

## Task 3: Defibrillator
We implemented a simple RC circuit to simulate shock delivery. A switch started the simulation to go from:

Normal ECG --> Atrial Fibrillation --> Shock Delivery --> Back to Normal ECG.

This was done in PyQt5. We used real normal ecg data and simulated atrial fibrillation using equations.

Note: you can't connect both Arduino IDE and python to arduino. Upload your arduino code onto your arduino then close the IDE and read the switch state in your python code to start the simulation.

## Task 4: Infusion Pump
We implemented an infusion pump using water pump, and flow sensor. We used ultrasonic sensor to indicate near end of infusion and controlled flow rate based on patient temperature.

This is a separate repo for the ultrasonic sensor on its own: https://github.com/NadaKhaled157/Ultrasonic-Water-Level-Detector

## Task 5: Hemodialysis
