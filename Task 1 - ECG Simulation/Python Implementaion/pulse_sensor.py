import numpy as np
import matplotlib.pyplot as plt
import serial

# Initialize serial connection
ser = serial.Serial('COM5', 9600, timeout=1)

# ECG simulation function
def ecg_simulate(heart_rate, sampling_rate):
    rr_interval = 60 / heart_rate  # RR interval
    t = np.linspace(0, rr_interval, int(sampling_rate * rr_interval))
    p_wave = 0.1 * np.exp(-((t - 0.1 * rr_interval) ** 2) / (2 * 0.015 ** 2))
    qrs_wave = (-0.1 * np.exp(-((t - 0.25 * rr_interval) ** 2) / (2 * 0.002 ** 2)) +  # Q wave
                 1.5 * np.exp(-((t - 0.3 * rr_interval) ** 2) / (2 * 0.005 ** 2)) -  # R wave
                 0.15 * np.exp(-((t - 0.35 * rr_interval) ** 2) / (2 * 0.003 ** 2)))  # S wave
    t_wave = 0.2 * np.exp(-((t - 0.6 * rr_interval) ** 2) / (2 * 0.025 ** 2))
    ecg_waveform = p_wave + qrs_wave + t_wave
    return t, ecg_waveform

# Function to read heart rate from Arduino
def read_heart_rate_from_arduino():
    try:
        if ser.in_waiting > 0:
            line = ser.readline().decode('utf-8').strip()
            return int(line)
    except ValueError:
        return None

# Set up the plot
plt.ion()  # Interactive mode for live updating
fig, ax = plt.subplots(figsize=(10, 4))
line, = ax.plot([], [])
ax.set_title('Simulated ECG Waveform')
ax.set_xlabel('Time (s)')
ax.set_ylabel('Amplitude')
ax.grid(True)

# Define sampling rate
sampling_rate = 1000

while True:
    heart_rate = read_heart_rate_from_arduino()
    if heart_rate is not None:
        print(f"Heart rate: {heart_rate} bpm")
        time, ecg_signal = ecg_simulate(heart_rate, sampling_rate)
        
        # Update plot data
        line.set_data(time, ecg_signal)
        ax.set_xlim(0, max(time))
        ax.set_ylim(min(ecg_signal) - 0.1, max(ecg_signal) + 0.1)
        
        # Redraw the plot
        fig.canvas.draw_idle()  # Use draw_idle instead of draw
        plt.pause(0.01)  # Small pause to allow the update to render
