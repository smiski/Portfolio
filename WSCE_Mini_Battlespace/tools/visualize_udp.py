import socket
import json
import matplotlib.pyplot as plt

TELEM_PORT = 49000
HOST = "127.0.0.1"

def main():
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    sock.bind((HOST, TELEM_PORT))
    sock.settimeout(1.0)

    plt.ion()
    fig, ax = plt.subplots()
    scat = ax.scatter([], [])
    labels = {}

    ax.set_title("WSCE Mini Battlespace (Top-Down X/Y)")
    ax.set_xlabel("X (m)")
    ax.set_ylabel("Y (m)")
    ax.grid(True)

    while True:
        try:
            data, _ = sock.recvfrom(65535)
        except socket.timeout:
            plt.pause(0.01)
            continue

        msg = data.decode("utf-8", errors="replace").strip()
        try:
            obj = json.loads(msg)
        except json.JSONDecodeError:
            # If you ever change telemetry format, keep running gracefully.
            continue

        ents = obj.get("entities", [])
        t = obj.get("t", 0.0)

        xs, ys = [], []
        names = []

        for e in ents:
            name = e.get("name", "unknown")
            pos = e.get("pos", [0, 0, 0])
            x, y = float(pos[0]), float(pos[1])
            xs.append(x); ys.append(y); names.append(name)

        scat.set_offsets(list(zip(xs, ys)))
        ax.set_title(f"WSCE Mini Battlespace (Top-Down X/Y)  t={t:.2f}s")

        # Auto-scale view based on current points
        if xs and ys:
            pad = 500.0
            ax.set_xlim(min(xs) - pad, max(xs) + pad)
            ax.set_ylim(min(ys) - pad, max(ys) + pad)

        # Update text labels (simple approach: delete + redraw)
        for txt in labels.values():
            txt.remove()
        labels.clear()
        for x, y, n in zip(xs, ys, names):
            labels[n] = ax.text(x, y, f" {n}", fontsize=9)

        plt.pause(0.01)

if __name__ == "__main__":
    main()