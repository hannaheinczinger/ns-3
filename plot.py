import os
import glob
import pandas as pd
import matplotlib.pyplot as plt

THROUGHPUT_DIR = "results/throughput_time"
OUTPUT_DIR = "plots"

os.makedirs(OUTPUT_DIR, exist_ok=True)

files = sorted(glob.glob(os.path.join(THROUGHPUT_DIR, "*.csv")))
colors = ["#ce6693", "#0076be", "#48bf91", "#5c53a5", "#f4a259", "#d7263d"]

# all throughput on the same plot
plt.figure(figsize=(10, 6))

for i, f in enumerate(files):
    data = pd.read_csv(f)
    label = os.path.basename(f).replace(".csv", "")
    color = colors[i % len(colors)]

    plt.plot(data["time_s"], data["totalThroughputMbps"], label=label, color=color, linewidth=2)

plt.xlabel("Time (s)")
plt.ylabel("Total Throughput (Mbps)")
plt.title("Network Throughput Over Time")
plt.legend(fontsize=8)
plt.grid(True, alpha=0.3)
plt.tight_layout()
plt.savefig(os.path.join(OUTPUT_DIR, "throughputs.png"), dpi=300)
plt.close()

# individual throughput plots
for i, f in enumerate(files):
    data = pd.read_csv(f)
    label = os.path.basename(f).replace(".csv", "")
    color = colors[i % len(colors)]

    plt.figure(figsize=(8, 5))
    plt.plot(data["time_s"], data["totalThroughputMbps"], label=label, color=color, linewidth=2)

    plt.xlabel("Time (s)")
    plt.ylabel("Total Throughput (Mbps)")
    plt.title(f"Throughput Over Time: {label}") # label is currently filename
    plt.legend()
    plt.grid(True, alpha=0.3)
    plt.tight_layout()

    out_name = os.path.join(OUTPUT_DIR, f"{label}.png")
    plt.savefig(out_name, dpi=300)
    plt.close()