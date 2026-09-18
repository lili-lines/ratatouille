import csv
import glob
import os
import sys
import matplotlib.pyplot as plt

# trace les enregistrements de xp_log_system.py (check1_xp_system.cpp) :
# cap gyro, distance encodeur, deflexion des moustaches, en fonction du temps
# usage : python xp_plot_system.py [dossier_des_csv] [dossier_sortie]

BG, FG, MUTED = "#1f1547", "#e9e6ff", "#6e64aa"
MAGENTA, GREEN, CYAN = "#d57bff", "#00ff9c", "#00b0ff"
MM_PER_TICK = 100.0 / 1151.0

src = sys.argv[1] if len(sys.argv) > 1 else "."
out = sys.argv[2] if len(sys.argv) > 2 else src
files = sorted(glob.glob(os.path.join(src, "xp_system_log_*.csv")))

runs = []
for f in files:
    rows = list(csv.DictReader(open(f)))
    t0 = float(rows[0]["timestamp"])
    runs.append({
        "name": os.path.basename(f).replace("xp_system_log_", "").replace(".csv", ""),
        "t": [float(r["timestamp"]) - t0 for r in rows],
        "h": [float(r["headingDeg"]) for r in rows],
        "encR": [int(r["encCountR"]) for r in rows],
        "encL": [int(r["encCountL"]) for r in rows],
        "wL": [int(r["wL"]) for r in rows],
        "wF": [int(r["wF"]) for r in rows],
        "wR": [int(r["wR"]) for r in rows],
    })

n = len(runs)
fig, axes = plt.subplots(3, n, figsize=(4.2 * n, 8.5), facecolor=BG, squeeze=False)

for col, r in enumerate(runs):
    t = r["t"]
    ax_h, ax_e, ax_w = axes[0][col], axes[1][col], axes[2][col]

    ax_h.plot(t, r["h"], color=CYAN, lw=1.2)
    ax_h.set_title(f"run {r['name']}\nheading {r['h'][0]:.1f} -> {r['h'][-1]:.1f} deg", color=FG, fontsize=9)
    ax_h.set_ylabel("heading (deg)", color=FG, fontsize=8)

    # distance depuis le debut du run, roue par roue
    dR = [(v - r["encR"][0]) * MM_PER_TICK for v in r["encR"]]
    dL = [(v - r["encL"][0]) * MM_PER_TICK for v in r["encL"]]
    ax_e.plot(t, dL, color=GREEN, lw=1.2, label=f"L  {dL[-1]:.0f} mm")
    ax_e.plot(t, dR, color=MAGENTA, lw=1.2, label=f"R  {dR[-1]:.0f} mm")
    ax_e.set_ylabel("distance (mm)", color=FG, fontsize=8)
    ax_e.legend(fontsize=7, facecolor=BG, edgecolor=MUTED, labelcolor=FG, loc="upper left")

    # deflexion = ecart a la valeur de depart, pour voir les 3 moustaches sur la meme echelle
    for key, c in (("wL", GREEN), ("wF", CYAN), ("wR", MAGENTA)):
        w0 = r[key][0]
        ax_w.plot(t, [v - w0 for v in r[key]], color=c, lw=1.0, label=f"{key} (start {w0})")
    ax_w.set_ylabel("whisker - start (counts)", color=FG, fontsize=8)
    ax_w.set_xlabel("time (s)", color=FG, fontsize=8)
    ax_w.legend(fontsize=7, facecolor=BG, edgecolor=MUTED, labelcolor=FG, loc="upper left")

    for ax in (ax_h, ax_e, ax_w):
        ax.set_facecolor(BG)
        ax.tick_params(colors=FG, labelsize=7)
        for s in ax.spines.values():
            s.set_color(MUTED)
        ax.grid(True, color=MUTED, alpha=0.25)

fig.suptitle("check1_xp_system -- pousse a la main, moteurs OFF", color=FG, fontsize=12)
plt.tight_layout(rect=[0, 0, 1, 0.96])
png = os.path.join(out, "xp_system_plot.png")
plt.savefig(png, facecolor=BG, dpi=140)
print("Saved:", png)
