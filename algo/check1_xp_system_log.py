import serial
import csv
import time

PORT = "COM3"       # ← port teensy
BAUD = 115200

ser = serial.Serial(PORT, BAUD)
name = "L"  # systeme, L, 30cm
filename = f"xp_{name}_log_{int(time.time())}.csv"

with open(filename, "w", newline="") as f:
    writer = csv.writer(f)
    writer.writerow(["timestamp", "headingDeg", "encCountR", "encCountL", "wL", "wF", "wR"])
    print(f"Enregistrement dans {filename} — Ctrl+C pour arrêter")
    try:
        while True:
            line = ser.readline().decode().strip()
            if line and line.count("\t") == 5:
                heading, encR, encL, wL, wF, wR = line.split("\t")
                writer.writerow([time.time(), heading, encR, encL, wL, wF, wR])
                print(line)
    except KeyboardInterrupt:
        print("Arrêté.")
