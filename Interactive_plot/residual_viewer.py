
import os
import sys
import numpy as np

import matplotlib
matplotlib.use("Qt5Agg")

from matplotlib.figure import Figure
from matplotlib.backends.backend_qt5agg import FigureCanvasQTAgg
from matplotlib.widgets import Slider, RadioButtons
from PyQt5.QtWidgets import QApplication, QMainWindow, QVBoxLayout, QWidget


# --------------------------------------------------
# Configuration
# --------------------------------------------------

RAW_FILE = os.path.expanduser(
    "/home/nboudjem/Downloads/Walnut1/Reconstructions/Interactive_plot/cbct_residual.raw"
)

SHAPE = (501, 501, 501)  # Z, Y, X
DTYPE = np.float32


# --------------------------------------------------
# Load residual volume
# --------------------------------------------------

if not os.path.isfile(RAW_FILE):
    raise FileNotFoundError(f"Residual file not found: {RAW_FILE}")

expected_bytes = int(np.prod(SHAPE)) * np.dtype(DTYPE).itemsize
actual_bytes = os.path.getsize(RAW_FILE)

if actual_bytes != expected_bytes:
    raise ValueError(
        f"Unexpected file size: {actual_bytes:,} bytes; "
        f"expected {expected_bytes:,} bytes."
    )

residual = np.fromfile(RAW_FILE, dtype=DTYPE).reshape(SHAPE)

print("Residual loaded:", residual.shape)
print("Minimum:", float(residual.min()))
print("Maximum:", float(residual.max()))


# --------------------------------------------------
# Interactive viewer
# --------------------------------------------------

class ResidualViewer(QMainWindow):

    def __init__(self):
        super().__init__()

        self.setWindowTitle("Interactive CBCT Residual Viewer")
        self.resize(1100, 750)

        central = QWidget()
        self.setCentralWidget(central)
        layout = QVBoxLayout(central)

        self.figure = Figure(figsize=(10, 7))
        self.canvas = FigureCanvasQTAgg(self.figure)
        layout.addWidget(self.canvas)

        # Profile plot
        self.ax = self.figure.add_axes([0.10, 0.38, 0.78, 0.54])

        self.x0 = 250
        self.y0 = 250
        self.z0 = 250

        self.line, = self.ax.plot([], [], linewidth=1.0)
        self.ax.axhline(0, color="red", linestyle="--", linewidth=0.8)
        self.ax.set_xlabel("Voxel coordinate")
        self.ax.set_ylabel("Residual intensity")
        self.ax.grid(True, alpha=0.3)

        # X, Y, Z sliders
        self.ax_x = self.figure.add_axes([0.15, 0.27, 0.62, 0.025])
        self.ax_y = self.figure.add_axes([0.15, 0.21, 0.62, 0.025])
        self.ax_z = self.figure.add_axes([0.15, 0.15, 0.62, 0.025])

        self.slider_x = Slider(
            self.ax_x, "X", 0, 500, valinit=self.x0, valstep=1
        )
        self.slider_y = Slider(
            self.ax_y, "Y", 0, 500, valinit=self.y0, valstep=1
        )
        self.slider_z = Slider(
            self.ax_z, "Z", 0, 500, valinit=self.z0, valstep=1
        )

        # Select profile direction
        self.ax_radio = self.figure.add_axes([0.82, 0.13, 0.12, 0.18])
        self.radio = RadioButtons(
            self.ax_radio, ("X", "Y", "Z"), active=1
        )
        self.ax_radio.set_title("Profile axis", fontsize=9)

        self.slider_x.on_changed(self.update_plot)
        self.slider_y.on_changed(self.update_plot)
        self.slider_z.on_changed(self.update_plot)
        self.radio.on_clicked(self.update_plot)

        self.update_plot()

    def update_plot(self, _=None):
        x = int(self.slider_x.val)
        y = int(self.slider_y.val)
        z = int(self.slider_z.val)

        axis = self.radio.value_selected

        # Array order is (Z, Y, X)
        if axis == "X":
            profile = residual[z, y, :]
        elif axis == "Y":
            profile = residual[z, :, x]
        else:
            profile = residual[:, y, x]

        self.line.set_data(np.arange(profile.size), profile)

        self.ax.set_xlim(0, profile.size - 1)

        pmin = float(np.min(profile))
        pmax = float(np.max(profile))
        margin = max((pmax - pmin) * 0.05, 1e-8)
        self.ax.set_ylim(pmin - margin, pmax + margin)

        self.ax.set_title(
            f"Residual along {axis} | X={x}, Y={y}, Z={z}"
        )
        self.ax.set_xlabel(f"{axis} coordinate (voxel)")

        self.canvas.draw_idle()


if __name__ == "__main__":
    app = QApplication(sys.argv)
    window = ResidualViewer()
    window.show()
    sys.exit(app.exec_())

