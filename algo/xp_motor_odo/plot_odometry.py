"""Trace les logs d'odometrie (timestamp, headingDeg, encoderCount).

Un seul run par PWM : le plus long. Les deux echantillons courts (~18 s)
sont ecartes, trop brefs pour juger d'une derive.

Usage  : python plot_odometry.py
Sortie : ../../assets/img/xp_odometry_plot.png
"""

import csv
import glob
import os
import re
import statistics

import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt

HERE = os.path.dirname(os.path.abspath(__file__))
OUT = os.path.normpath(os.path.join(HERE, "..", "..", "assets", "img", "xp_odometry_plot.png"))

MM_PER_TICK = 100.0 / 1151.0

# palette de pwm_duty.gif
BG = "#1f1547"          # fond violet fonce
FG = "#e9e6ff"          # texte
MUTED = "#6e64aa"       # grille, axes, labels secondaires
# PWM 0 en blanc casse : c'est la ligne de reference, elle doit rester lisible
COLORS = {0: FG, 130: "#d57bff", 206: "#00ff9c"}


def load(path):
    with open(path) as f:
        rows = list(csv.DictReader(f))
    t0 = float(rows[0]["timestamp"])
    t = [float(r["timestamp"]) - t0 for r in rows]
    h0 = float(rows[0]["headingDeg"])
    h = [float(r["headingDeg"]) - h0 for r in rows]
    c0 = int(rows[0]["encoderCount"])
    c = [abs(int(r["encoderCount"]) - c0) for r in rows]
    return t, h, c


def speed_windows(t, c, win=1.0):
    """Vitesse moyenne (mm/s) sur des fenetres glissantes de `win` secondes."""
    out, i = [], 0
    while i < len(t):
        j = i
        while j < len(t) and t[j] - t[i] < win:
            j += 1
        if j - 1 > i:
            out.append(((t[i] + t[j - 1]) / 2,
                        (c[j - 1] - c[i]) * MM_PER_TICK / (t[j - 1] - t[i])))
        i = j
    return out


def merge(centres, gap=2.0):
    """Regroupe les fenetres signalees en intervalles continus."""
    spans = []
    for tc in centres:
        a, b = tc - 0.5, tc + 0.5
        if spans and a - spans[-1][1] <= gap:
            spans[-1][1] = b
        else:
            spans.append([a, b])
    return spans


# on ne garde que le run le plus long de chaque PWM
runs = {}
for path in sorted(glob.glob(os.path.join(HERE, "xp_motor*_odometry_log_*.csv"))):
    speed = int(re.search(r"xp_motor(\d+)_", os.path.basename(path)).group(1))
    t, h, c = load(path)
    if speed not in runs or t[-1] > runs[speed][0][-1]:
        runs[speed] = (t, h, c)

fig, (ax1, ax2) = plt.subplots(2, 1, figsize=(9, 6.5), sharex=True)
fig.patch.set_facecolor(BG)

for speed in sorted(runs):
    t, h, c = runs[speed]
    color = COLORS.get(speed, FG)
    d = [x * MM_PER_TICK / 1000.0 for x in c]
    win = speed_windows(t, c)
    speed_mms = statistics.median([v for _, v in win])
    # une fenetre s'ecartant de plus de 25 % de la mediane est signalee.
    # on ignore les 1,5 premieres secondes : c'est la rampe de demarrage.
    odd = [tc for tc, v in win
           if tc > 1.5 and speed_mms > 10 and abs(v - speed_mms) > 0.25 * speed_mms]
    odd = merge(odd)

    ax1.plot(t, h, color=color, linewidth=1.6, label=f"PWM {speed}")
    ax2.plot(t, d, color=color, linewidth=1.6, label=f"PWM {speed}")
    ax2.annotate(f"{speed_mms:.0f} mm/s", xy=(t[-1], d[-1]), xytext=(5, 0),
                 textcoords="offset points", color=color, fontsize=9, va="center")
    for k, (a, b) in enumerate(odd):
        ax2.axvspan(a, b, color=color, alpha=0.18, linewidth=0,
                    label="_" if k else f"PWM {speed}: off the median by >25%")

for ax in (ax1, ax2):
    ax.set_facecolor(BG)
    ax.grid(True, color=MUTED, alpha=0.35, linewidth=0.7)
    for s in ax.spines.values():
        s.set_color(MUTED)
    ax.tick_params(colors=MUTED, labelsize=9)

ax1.axhspan(-5, 5, color=MUTED, alpha=0.22)
ax1.text(2, 4.3, "±5° criterion", color=MUTED, fontsize=8.5, va="top")
ax1.set_ylabel("heading drift (°)", color=FG, fontsize=10)
ax1.set_title("Calibrated gyro: the heading stays stable, even with the motors running",
              color=FG, fontsize=11, loc="left")
ax1.legend(frameon=False, fontsize=9, labelcolor=FG, ncol=3, loc="upper right")

ax2.set_ylabel("equivalent distance (m)", color=FG, fontsize=10)
ax2.set_xlabel("time (s)", color=FG, fontsize=10)
ax2.set_title("Encoder: the slope gives the speed (free wheel, no slip)",
              color=FG, fontsize=11, loc="left")

handles = [h for h, l in zip(*ax2.get_legend_handles_labels()) if "median" in l]
if handles:
    ax2.legend(handles=handles, frameon=False, fontsize=8.5,
               labelcolor=MUTED, loc="upper left")
ax2.set_xlim(right=max(t[-1] for t, _, _ in runs.values()) * 1.14)

fig.tight_layout()
fig.savefig(OUT, dpi=140, facecolor=BG)
print("ecrit :", os.path.normpath(OUT))
