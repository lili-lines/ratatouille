import serial
import csv
import time

PORT = "COM4"       # ← mets ton port ici
BAUD = 115200
speed = "130"  # OFF, 206, 130
defender = "nofilter"
# "all", "noRC", "noCondD", "noCondR", "nostar", "nofilter"

ser = serial.Serial(PORT, BAUD)
filename = f"xp2_motor{speed}_{defender}_whisk_log_{int(time.time())}.csv"


with open(filename, "w", newline="") as f:
    writer = csv.writer(f)
    writer.writerow(["timestamp", "whisker"])
    print(f"Enregistrement dans {filename} — Ctrl+C pour arrêter")
    try:
        while True:
            line = ser.readline().decode().strip()
            if line:
                writer.writerow([time.time(), line])
                print(line)
    except KeyboardInterrupt:
        print("Arrêté.")


