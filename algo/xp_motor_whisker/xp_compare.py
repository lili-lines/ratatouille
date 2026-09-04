import csv
import glob
import os
import statistics
import matplotlib.pyplot as plt

UMBRA_DARK = "#100d23"
UMBRA_CODE_BG = "#1f1547"
UMBRA_GREEN = "#00FF9C"
UMBRA_MAGENTA = "#d57bff"
UMBRA_CYAN = "#00b0ff"
TEXT_LIGHT = "#e9e6ff"

FOLDER = "clean"

LABELS = {
    "motorOFF": "motor OFF (baseline)",
    "motor130": "motor 130, all defenders",
    "motor130_noRC": "motor 130, no RC filter",
    "motor130_noCondD": "motor 130, no decoupling",
    "motor130_noCondR": "motor 130, no reservoir",
    "motor130_nostar": "motor 130, no star ground",
    "motor206": "motor 206, all defenders",
    "motor206_noRC": "motor 206, no RC filter",
    "motor206_noCondD": "motor 206, no decoupling",
    "motor206_noCondR": "motor 206, no reservoir",
    "motor206_nostar": "motor 206, no star ground",
}

ORDER = list(LABELS.keys())

def load(path):
    vals = []
    with open(path, newline="") as f:
        reader = csv.reader(f)
        next(reader)
        for row in reader:
            vals.append(int(row[1]))
    return vals

files = glob.glob(os.path.join(FOLDER, "xp_*.csv"))
data = {}
for path in files:
    name = os.path.basename(path)
    for key in ORDER:
        tag = f"xp_{key}_whisker_log"
        if name.startswith(tag):
            data[key] = load(path)
            break

print(f"{'condition':28s} {'n':>5s} {'mean':>8s} {'std':>8s} {'p2p':>8s}")
stats = {}
for key in ORDER:
    if key not in data:
        continue
    vals = data[key]
    mean = statistics.mean(vals)
    std = statistics.pstdev(vals)
    p2p = max(vals) - min(vals)
    stats[key] = (mean, std, p2p)
    print(f"{key:28s} {len(vals):5d} {mean:8.1f} {std:8.2f} {p2p:8d}")

present = [k for k in ORDER if k in data]
n = len(present)
cols = 3
rows = (n + cols - 1) // cols

fig, axes = plt.subplots(rows, cols, figsize=(15, 3.2 * rows), facecolor=UMBRA_DARK)
axes = axes.flatten()

ymin, ymax = 480, 510

for i, key in enumerate(present):
    ax = axes[i]
    vals = data[key]
    ax.set_facecolor(UMBRA_CODE_BG)
    color = UMBRA_MAGENTA if "no" in key.split("_", 1)[-1] and key != "motorOFF" else UMBRA_GREEN
    if key == "motorOFF":
        color = UMBRA_CYAN
    ax.plot(vals, color=color, linewidth=0.8)
    ax.set_ylim(ymin, ymax)
    mean, std, p2p = stats[key]
    ax.set_title(f"{LABELS[key]}\nstd={std:.1f}  p2p={p2p}", color=TEXT_LIGHT, fontsize=9)
    ax.tick_params(colors=TEXT_LIGHT, labelsize=7)
    for spine in ax.spines.values():
        spine.set_color(UMBRA_CYAN)
    ax.grid(True, color=UMBRA_CYAN, alpha=0.1)

for j in range(len(present), len(axes)):
    axes[j].axis("off")

fig.suptitle("Whisker signal — noise defenders comparison", color=TEXT_LIGHT, fontsize=14)
plt.tight_layout(rect=[0, 0, 1, 0.96])
plt.savefig(os.path.join(FOLDER, "xp_compare_all.png"), facecolor=UMBRA_DARK, dpi=150)
print(f"\nSaved: {os.path.join(FOLDER, 'xp_compare_all.png')}")

with open(os.path.join(FOLDER, "xp_stats.csv"), "w", newline="") as f:
    writer = csv.writer(f)
    writer.writerow(["condition", "label", "n", "mean", "std", "p2p"])
    for key in present:
        mean, std, p2p = stats[key]
        writer.writerow([key, LABELS[key], len(data[key]), f"{mean:.1f}", f"{std:.2f}", p2p])

fig2, ax2 = plt.subplots(figsize=(9, 0.5 + 0.35 * len(present)), facecolor=UMBRA_DARK)
ax2.axis("off")
rows_data = [[LABELS[k], len(data[k]), f"{stats[k][0]:.1f}", f"{stats[k][1]:.2f}", stats[k][2]] for k in present]
table = ax2.table(
    cellText=rows_data,
    colLabels=["condition", "n", "mean", "std", "p2p"],
    cellLoc="center",
    loc="center",
)
table.auto_set_font_size(False)
table.set_fontsize(9)
table.scale(1, 1.4)
for (row, col), cell in table.get_celld().items():
    cell.set_facecolor(UMBRA_CODE_BG if row > 0 else UMBRA_DARK)
    cell.set_edgecolor(UMBRA_CYAN)
    cell.set_text_props(color=TEXT_LIGHT)
plt.tight_layout()
plt.savefig(os.path.join(FOLDER, "xp_stats_table.png"), facecolor=UMBRA_DARK, dpi=150)
print(f"Saved: {os.path.join(FOLDER, 'xp_stats_table.png')}")
print(f"Saved: {os.path.join(FOLDER, 'xp_stats.csv')}")
