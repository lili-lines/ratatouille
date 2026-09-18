import csv
import glob
import os
import sys
import matplotlib.pyplot as plt

# trace les enregistrements de xp1_log_as5600_2.py : les 3 moustaches (0..4095) en fonction du temps
# usage : python xp1_plot_as5600.py [dossier_des_csv] [dossier_sortie] [id_du_run]
#   id_du_run : le timestamp du fichier (ex. 1789539115) pour ne tracer que celui-la

BG, FG, MUTED = "#1f1547", "#e9e6ff", "#6e64aa"
GREEN, CYAN, MAGENTA = "#00ff9c", "#00b0ff", "#d57bff"

src = sys.argv[1] if len(sys.argv) > 1 else "."
out = sys.argv[2] if len(sys.argv) > 2 else src
run_id = sys.argv[3] if len(sys.argv) > 3 else "*"
files = sorted(glob.glob(os.path.join(src, f"xp1_as5600_log_{run_id}.csv")))

runs = []
for f in files:
    rows = list(csv.DictReader(open(f)))
    t0 = float(rows[0]["timestamp"])
    runs.append({
        "name": os.path.basename(f).replace("xp1_as5600_log_", "").replace(".csv", ""),
        "t": [float(r["timestamp"]) - t0 for r in rows],
        "L": [int(r["whiskerL"]) for r in rows],
        "F": [int(r["whiskerF"]) for r in rows],
        "R": [int(r["whiskerR"]) for r in rows],
    })

n = len(runs)
fig, axes = plt.subplots(3, n, figsize=(7 * n, 8), facecolor=BG, squeeze=False, sharex="col")

for col, r in enumerate(runs):
    for row, (key, color, label) in enumerate((("L", GREEN, "left"), ("F", CYAN, "front"), ("R", MAGENTA, "right"))):
        ax = axes[row][col]
        ax.plot(r["t"], r[key], color=color, lw=0.9)
        lo, hi = min(r[key]), max(r[key])
        pad = max(20, (hi - lo) * 0.05)
        ax.set_ylim(1500, 4200)   # each whisker on its own range
        for rail in(0, 4095):            # show the ADC rails only if the trace reaches them
            if lo - pad <= rail <= hi + pad:
                ax.axhline(rail, color=MUTED, lw=0.6, ls="--")
        ax.set_ylabel(f"whisker {label}", color=FG, fontsize=9)
        if row == 0:
            ax.set_title(f"run {r['name']}", color=FG, fontsize=10)
        if row == 2:
            ax.set_xlabel("time (s)", color=FG, fontsize=9)
        ax.set_facecolor(BG)
        ax.tick_params(colors=FG, labelsize=8)
        for s in ax.spines.values():
            s.set_color(MUTED)
        ax.grid(True, color=MUTED, alpha=0.25)

fig.suptitle("AS5600 whiskers, raw 12-bit reading (0..4095)", color=FG, fontsize=12)
plt.tight_layout(rect=[0, 0, 1, 0.96])
png = os.path.join(out, "xp1_as5600_plot.png")
plt.savefig(png, facecolor=BG, dpi=140)
print("Saved:", png)
