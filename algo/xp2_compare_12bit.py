import csv
import glob
import os
import re
import statistics
import matplotlib.pyplot as plt

UMBRA_DARK = "#100d23"
UMBRA_CODE_BG = "#1f1547"
UMBRA_GREEN = "#00FF9C"
UMBRA_MAGENTA = "#d57bff"
UMBRA_CYAN = "#00b0ff"
TEXT_LIGHT = "#e9e6ff"

# chemins depuis la racine du repo (le script est dans algo/)
ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
FOLDER = os.path.join(ROOT, "assets", "data_xp", "as5600_12bit")   # les CSV
PLOTS = os.path.join(ROOT, "assets", "plot_xp")                     # les images du post

LABELS = {
    "motorOFF": "motor OFF (baseline)",
    "motor130": "motor 130, all defenders",
    "motor130_noRC": "motor 130, no RC filter",
    "motor130_noCondD": "motor 130, no decoupling",
    "motor130_noCondR": "motor 130, no reservoir",
    "motor130_nostar": "motor 130, no star ground",
    "motor130_star": "motor 130, star ground only",
    "motor130_nofilter": "motor 130, no defender at all",
    "motor206": "motor 206, all defenders",
    "motor206_noRC": "motor 206, no RC filter",
    "motor206_noCondD": "motor 206, no decoupling",
    "motor206_noCondR": "motor 206, no reservoir",
    "motor206_nostar": "motor 206, no star ground",
    "motor206_star": "motor 206, star ground only",
    "motor206_nofilter": "motor 206, no defender at all",
}

ORDER = list(LABELS.keys())

# xp_motorOFF_all_whisk_log_<ts>.csv, xp2_motor130_nofilter_whisk_log_<ts>.csv ...
# the "_all" suffix means all defenders and maps to the bare "motorXXX" key
FILE_PATTERN = re.compile(r"^xp2?_(motor(?:OFF|\d+))_([A-Za-z]+)_whisk_log_\d+\.csv$")

def load(path):
    vals = []
    with open(path, newline="") as f:
        reader = csv.reader(f)
        next(reader)
        for row in reader:
            vals.append(int(row[1]))
    return vals

N_SAMPLES = 1500  # keep only the first N values of each run, so every condition is compared over the same window

files = glob.glob(os.path.join(FOLDER, "xp*_motor*_whisk_log_*.csv"))
data = {}
for path in files:
    m = FILE_PATTERN.match(os.path.basename(path))
    if not m:
        continue
    motor, cond = m.groups()
    key = motor if cond == "all" else f"{motor}_{cond}"
    if key in LABELS:
        data[key] = load(path)[:N_SAMPLES]

print(f"{'condition':28s} {'n':>5s} {'mean':>8s} {'std':>8s} {'std/mean':>9s} {'MAD':>5s} {'5-95%':>6s} {'p2p':>8s}")
stats = {}
for key in ORDER:
    if key not in data:
        continue
    vals = data[key]
    mean = statistics.mean(vals)
    std = statistics.pstdev(vals)
    p2p = max(vals) - min(vals)
    cv = 100 * std / mean
    # MAD = ecart median, le "fuzz" permanent, insensible aux pics rares ; 5-95 % = largeur de la bande
    med = statistics.median(vals)
    mad = statistics.median(abs(v - med) for v in vals)
    srt = sorted(vals)
    w90 = srt[int(0.95 * len(srt))] - srt[int(0.05 * len(srt))]
    stats[key] = (mean, std, p2p, cv, mad, w90)
    print(f"{key:28s} {len(vals):5d} {mean:8.1f} {std:8.2f} {cv:8.2f}% {mad:5.1f} {w90:6d} {p2p:8d}")

present = [k for k in ORDER if k in data]
n = len(present)
cols = 3
rows = (n + cols - 1) // cols

fig, axes = plt.subplots(rows, cols, figsize=(15, 3.2 * rows), facecolor=UMBRA_DARK)
axes = axes.flatten()

for i, key in enumerate(present):
    ax = axes[i]
    vals = data[key]
    ax.set_facecolor(UMBRA_CODE_BG)
    if key == "motorOFF":
        color = UMBRA_CYAN            # baseline
    elif "_" not in key:
        color = UMBRA_GREEN           # all defenders
    else:
        color = UMBRA_MAGENTA         # one or more defenders removed
    ax.plot(vals, color=color, linewidth=0.8)
    mean, std, p2p, cv, mad, w90 = stats[key]
    ax.set_ylim(mean - 20, mean + 20)  # centered on this condition's own mean
    ax.set_title(f"{LABELS[key]}\nstd={std:.2f} ({cv:.2f}%)  p2p={p2p}", color=TEXT_LIGHT, fontsize=9)
    ax.tick_params(colors=TEXT_LIGHT, labelsize=7)
    for spine in ax.spines.values():
        spine.set_color(UMBRA_CYAN)
    ax.grid(True, color=UMBRA_CYAN, alpha=0.1)

for j in range(len(present), len(axes)):
    axes[j].axis("off")

fig.suptitle("Whisker signal — noise defenders comparison (12-bit)", color=TEXT_LIGHT, fontsize=14)
plt.tight_layout(rect=[0, 0, 1, 0.96])
plt.savefig(os.path.join(PLOTS, "xp_compare_all_12b.png"), facecolor=UMBRA_DARK, dpi=150)
print(f"\nSaved: {os.path.join(PLOTS, 'xp_compare_all_12b.png')}")

with open(os.path.join(FOLDER, "xp_stats.csv"), "w", newline="") as f:
    writer = csv.writer(f)
    writer.writerow(["condition", "label", "n", "mean", "std", "std/mean %", "MAD", "5-95 %", "p2p"])
    for key in present:
        mean, std, p2p, cv, mad, w90 = stats[key]
        writer.writerow([key, LABELS[key], len(data[key]), f"{mean:.1f}", f"{std:.2f}", f"{cv:.2f}", f"{mad:.1f}", w90, p2p])

fig2, ax2 = plt.subplots(figsize=(12, 0.5 + 0.35 * len(present)), facecolor=UMBRA_DARK)
ax2.axis("off")
rows_data = [[LABELS[k], len(data[k]), f"{stats[k][0]:.1f}", f"{stats[k][1]:.2f}", f"{stats[k][3]:.2f}%", f"{stats[k][4]:.1f}", stats[k][5], stats[k][2]] for k in present]
table = ax2.table(
    cellText=rows_data,
    colLabels=["condition", "n", "mean", "std", "std/mean", "MAD", "5-95 %", "p2p"],
    cellLoc="center",
    loc="center",
    colWidths=[0.30, 0.08, 0.11, 0.10, 0.12, 0.09, 0.10, 0.09],
)
table.auto_set_font_size(False)
table.set_fontsize(9)
table.scale(1, 1.4)
for (row, col), cell in table.get_celld().items():
    if col == 0:
        cell.set_text_props(ha="left")
        cell.PAD = 0.02
for (row, col), cell in table.get_celld().items():
    cell.set_facecolor(UMBRA_CODE_BG if row > 0 else UMBRA_DARK)
    cell.set_edgecolor(UMBRA_CYAN)
    cell.set_text_props(color=TEXT_LIGHT)
plt.tight_layout()
plt.savefig(os.path.join(PLOTS, "xp_stats_table_all_12b.png"), facecolor=UMBRA_DARK, dpi=150)
print(f"Saved: {os.path.join(PLOTS, 'xp_stats_table_all_12b.png')}")
print(f"Saved: {os.path.join(FOLDER, 'xp_stats.csv')}")
