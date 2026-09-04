import csv
import sys
import matplotlib.pyplot as plt

UMBRA_DARK = "#100d23"
UMBRA_CODE_BG = "#1f1547"
UMBRA_GREEN = "#00FF9C"
UMBRA_MAGENTA = "#d57bff"
UMBRA_CYAN = "#00b0ff"
TEXT_LIGHT = "#e9e6ff"

filename = sys.argv[1] if len(sys.argv) > 1 else "whisker_log.csv"

timestamps, whisker = [], []
with open(filename, newline="") as f:
    reader = csv.reader(f)
    next(reader)
    for row in reader:
        timestamps.append(float(row[0]))
        whisker.append(int(row[1]))

t0 = timestamps[0]
elapsed = [t - t0 for t in timestamps]

fig, ax = plt.subplots(figsize=(10, 5), facecolor=UMBRA_DARK)
ax.set_facecolor(UMBRA_CODE_BG)

ax.plot(elapsed, whisker, color=UMBRA_GREEN, linewidth=1.5, label="whisker")

ax.set_xlabel("time (s)", color=TEXT_LIGHT)
ax.set_ylabel("raw ADC value", color=TEXT_LIGHT)
ax.set_title("Whisker signal", color=TEXT_LIGHT)

ax.tick_params(colors=TEXT_LIGHT)
for spine in ax.spines.values():
    spine.set_color(UMBRA_CYAN)

ax.grid(True, color=UMBRA_CYAN, alpha=0.15)

legend = ax.legend(facecolor=UMBRA_CODE_BG, edgecolor=UMBRA_CYAN)
for text in legend.get_texts():
    text.set_color(TEXT_LIGHT)

plt.tight_layout()
plt.savefig(filename.replace(".csv", "_plot.png"), facecolor=UMBRA_DARK, dpi=150)
plt.show()
