import serial
import csv
import time

PORT = "COM3"       # ← mets ton port ici
BAUD = 115200
speed = 206

ser = serial.Serial(PORT, BAUD)
# filename = f"xp_motorOFF_whisker_log_{int(time.time())}.csv"
# filename = f"xp_motor{speed}_whisker_log_{int(time.time())}.csv"

# filename = f"xp_motor{speed}_noRC_whisker_log_{int(time.time())}.csv"
# filename = f"xp_motor{speed}_noCondD_whisker_log_{int(time.time())}.csv"
# filename = f"xp_motor{speed}_noCondR_whisker_log_{int(time.time())}.csv"

filename = f"xp_motor{speed}_nostar_whisker_log_{int(time.time())}.csv"

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


