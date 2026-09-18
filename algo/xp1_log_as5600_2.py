import serial
import csv
import time

PORT = "COM3"        # ← port number
BAUD = 115200

ser = serial.Serial(PORT, BAUD)
filename = f"xp1_as5600_log_{int(time.time())}.csv"

with open(filename, "w", newline="") as f:
    writer = csv.writer(f)
    writer.writerow(["timestamp", "whiskerL", "whiskerF", "whiskerR"])
    print(f"Enregistrement dans {filename} — Ctrl+C pour arrêter")
    try:
        while True:
            line = ser.readline().decode().strip()
            if line and line.count("\t") == 2:
                rawL, rawF, rawR = line.split("\t")
                writer.writerow([time.time(), rawL, rawF, rawR])
                print(line)
    except KeyboardInterrupt:
        print("Arrêté.")
