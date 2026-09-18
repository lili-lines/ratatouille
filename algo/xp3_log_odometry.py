import serial
import csv
import time

PORT = "COM3"       # ← mets ton port ici
BAUD = 115200
speed = 130         # ← doit correspondre au MOTOR_SPEED uploadé dans xp_odometry_b.cpp

ser = serial.Serial(PORT, BAUD)
filename = f"xp_motor{speed}_odometry_log_{int(time.time())}.csv"

with open(filename, "w", newline="") as f:
    writer = csv.writer(f)
    writer.writerow(["timestamp", "headingDeg", "encoderCount"])
    print(f"Enregistrement dans {filename} — Ctrl+C pour arrêter")
    try:
        while True:
            line = ser.readline().decode().strip()
            if line and '\t' in line:
                heading, count = line.split('\t')
                writer.writerow([time.time(), heading, count])
                print(line)
    except KeyboardInterrupt:
        print("Arrêté.")
