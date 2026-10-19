
import sys
import os
import numpy as np

from PyQt5.QtWidgets import (
    QApplication, QWidget, QLabel, QSlider,
    QComboBox, QVBoxLayout, QHBoxLayout
)
from PyQt5.QtCore import Qt

import matplotlib
matplotlib.use("Qt5Agg")

from matplotlib.figure import Figure
from matplotlib.backends.backend_qt5agg import FigureCanvasQTAgg
from matplotlib.colors import Normalize


NX = NY = NZ = 501

DEFAULT_FILE = os.path.expanduser(
    "/home/nboudjem/Downloads/Walnut1/processing/Interactive_plot/Walnut1_AGD50_volume.raw"
)


class ResidualViewer(QWidget):
    def __init__(self, filename):
        super().__init__()

        self.setWindowTitle("3D Residual Slice Viewer")
        self.resize(900, 800)

        expected = NX * NY * NZ * np.dtype(np.float32).itemsize
        actual = os.path.getsize(filename)

        if actual != expected:
            raise ValueError(
                f"Unexpected file size: {actual} bytes; "
                f"expected {expected} bytes."
            )

        self.volume = np.memmap(
            filename,
            dtype=np.float32,
            mode="r",
            shape=(NZ, NY, NX)
        )

        self.plane = QComboBox()
        self.plane.addItems([
            "Axial (XY) — fix Z",
            "Coronal (XZ) — fix Y",
            "Sagittal (YZ) — fix X"
        ])

        self.slider = QSlider(Qt.Horizontal)
        self.slider.setRange(0, 500)
        self.slider.setValue(250)

        self.coordinate_label = QLabel()
        self.value_label = QLabel()

        # Create the Matplotlib figure and axes ONCE.
        self.figure = Figure(constrained_layout=True)
        self.canvas = FigureCanvasQTAgg(self.figure)
        self.ax = self.figure.add_subplot(111)

        # Create a single image and colorbar.
        initial = self.get_slice()
        self.limit = self.get_limit(initial)

        self.image = self.ax.imshow(
            initial,
            cmap="gray",
            origin="upper",
            interpolation="nearest",
            vmin=-self.limit,
            vmax=self.limit
        )

        self.colorbar = self.figure.colorbar(
            self.image, ax=self.ax, fraction=0.046, pad=0.04
        )
        self.colorbar.set_label("Residual")

        controls = QHBoxLayout()
        controls.addWidget(self.plane)
        controls.addWidget(self.slider)

        layout = QVBoxLayout(self)
        layout.addLayout(controls)
        layout.addWidget(self.coordinate_label)
        layout.addWidget(self.canvas, 1)
        layout.addWidget(self.value_label)

        self.plane.currentIndexChanged.connect(self.update_slice)
        self.slider.valueChanged.connect(self.update_slice)

        self.update_slice()

    def get_slice(self):
        i = self.slider.value()
        p = self.plane.currentIndex()

        if p == 0:
            return np.asarray(self.volume[i, :, :])  # Fix Z
        elif p == 1:
            return np.asarray(self.volume[:, i, :])  # Fix Y
        else:
            return np.asarray(self.volume[:, :, i])  # Fix X

    @staticmethod
    def get_limit(frame):
        finite = np.abs(frame[np.isfinite(frame)])

        if finite.size == 0:
            return 1e-12

        limit = float(np.percentile(finite, 99))
        return max(limit, 1e-12)

    def update_slice(self, *_):
        frame = self.get_slice()

        # Update the existing image rather than creating a new one.
        self.image.set_data(frame)

        # Keep contrast fixed while browsing slices in one plane.
        # This prevents colors from changing unpredictably per slice.
        self.image.set_clim(-self.limit, self.limit)

        plane_index = self.plane.currentIndex()
        axis_name = ["Z", "Y", "X"][plane_index]
        plane_name = ["Axial (XY)", "Coronal (XZ)",
                      "Sagittal (YZ)"][plane_index]

        self.coordinate_label.setText(
            f"{plane_name} | {axis_name} = {self.slider.value()} / 500"
        )
        self.value_label.setText(
            f"Color range: {-self.limit:.5g} to {self.limit:.5g}"
        )

        # Preserve the axes geometry and redraw only the canvas.
        self.canvas.draw_idle()


def main():
    app = QApplication(sys.argv)

    filename = sys.argv[1] if len(sys.argv) > 1 else DEFAULT_FILE

    try:
        window = ResidualViewer(filename)
    except Exception as e:
        from PyQt5.QtWidgets import QMessageBox
        QMessageBox.critical(None, "Residual Viewer Error", str(e))
        return 1

    window.show()
    return app.exec_()


if __name__ == "__main__":
    sys.exit(main())

