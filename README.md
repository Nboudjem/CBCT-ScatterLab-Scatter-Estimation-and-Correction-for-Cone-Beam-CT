# CBCT-ScatterLab

### Scatter Estimation and Correction for Cone-Beam CT

> 🚧 **Under construction — research project in active development**

**CBCT-ScatterLab** is a research repository focused on the **estimation, modeling, and correction of X-ray scatter in Cone-Beam Computed Tomography (CBCT)**.

The project investigates mathematical, physics-based, inverse-problem, and potentially deep-learning approaches for reducing scatter-related artifacts directly from CBCT projection data.

The initial experiments are based on the **Walnut CBCT dataset** from the *WalnutReconstructionCodes* project.

---

## 🎯 Motivation

X-ray scatter is one of the major sources of image degradation in CBCT.

Scatter can contribute to:

* inaccurate CT numbers (HU)
* cupping and shading artifacts
* loss of contrast
* image non-uniformity
* streaking and other reconstruction artifacts
* reduced quantitative accuracy

A simplified measurement model is:

$$
I_{\mathrm{raw}}
=
I_{\mathrm{primary}}
+
I_{\mathrm{scatter}}
+
\epsilon
$$

where:

* \(I_{\mathrm{raw}}\) is the measured detector signal,
* \(I_{\mathrm{primary}}\) is the primary radiation,
* \(I_{\mathrm{scatter}}\) is the scattered radiation,
* \(\epsilon\) represents noise and other measurement effects.

The central problem investigated in this repository is therefore:

$$
\boxed{
I_{\mathrm{raw}}
\rightarrow
\hat I_{\mathrm{scatter}}
\rightarrow
I_{\mathrm{corrected}}
}
$$

with

$$
I_{\mathrm{corrected}}
=
I_{\mathrm{raw}}
-
\hat I_{\mathrm{scatter}}.
$$

---

# 🥜 Dataset — Walnut CBCT Data

The initial development and experiments in this repository use the **Walnut CBCT dataset** provided through the **WalnutReconstructionCodes** project.

The dataset contains X-ray CT projection data for **42 walnuts** and was designed specifically as a data collection for machine-learning research in cone-beam X-ray CT.

The dataset is described in:

> Henri Der Sarkissian, Felix Lucka, Maureen van Eijnatten, Giulia Colacicco, Sophia Bethany Coban, Kees Joost Batenburg,
> **"A Cone-Beam X-Ray CT Data Collection Designed for Machine Learning,"**
> *Scientific Data*, 6, 215 (2019).

* [Scientific Data / Nature](https://doi.org/10.1038/s41597-019-0235-y)
* [arXiv:1905.04787](https://arxiv.org/abs/1905.04787)

The original **WalnutReconstructionCodes** repository provides Python and MATLAB scripts for loading, preprocessing, and reconstructing the projection data.

### Reconstruction methods provided by the original project

The dataset includes reconstruction scripts based on the available projection data:

* `FDKReconstruction.py`

These compute FDK reconstructions using data from a **single source-detector orbit**. Because of the limited angular coverage in the cone-beam geometry, these reconstructions can contain high cone-angle artifacts.

The project also provides:

* `GroundTruthReconstruction.py`

These perform iterative reconstruction using data from **all three source-detector orbits**, producing reconstructions that are largely free of the high cone-angle artifacts associated with the single-orbit FDK reconstruction.

This distinction is particularly useful for the present project because the reconstructed volumes can provide a reference when evaluating the effect of projection-domain scatter correction.

---

## 📦 Walnut Dataset Downloads

The complete dataset is distributed through Zenodo:

| Walnuts | Dataset                                          |
| ------- | ------------------------------------------------ |
| 1–8     | [Zenodo](https://doi.org/10.5281/zenodo.2686725) |
| 9–16    | [Zenodo](https://doi.org/10.5281/zenodo.2686970) |
| 17–24   | [Zenodo](https://doi.org/10.5281/zenodo.2687386) |
| 25–32   | [Zenodo](https://doi.org/10.5281/zenodo.2687634) |
| 33–37   | [Zenodo](https://doi.org/10.5281/zenodo.2687896) |
| 38–42   | [Zenodo](https://doi.org/10.5281/zenodo.2688111) |

The raw projection data will be used as the primary input for investigating scatter estimation and correction.

---

# 🔬 Research Direction

The main research direction is **projection-domain scatter correction**.

Rather than treating scatter reduction only as an image-to-image translation problem,

$$
\mathrm{CBCT} \rightarrow \mathrm{CT},
$$

the project focuses on the underlying inverse problem:

$$
I_{\mathrm{raw}}
=
I_{\mathrm{primary}}
+
I_{\mathrm{scatter}}
+
\epsilon.
$$

The goal is to estimate the scatter component:

$$
\hat I_{\mathrm{scatter}}
$$

and subsequently obtain corrected projections:

$$
\boxed{
I_{\mathrm{corrected}}
=
I_{\mathrm{raw}}
-
\hat I_{\mathrm{scatter}}
}
$$

before reconstruction.

---

# 📐 Inverse Problem

A general formulation considered in this project is:

$$
\hat S
=
\arg\min_S
\left[
D(I_{\mathrm{raw}}-S,\mathcal{P}(\mu))
+
\lambda R(S)
\right]
$$

where:

* \(S\) is the estimated scatter,
* \(I_{\mathrm{raw}}\) is the measured projection,
* \(\mathcal{P}(\mu)\) is a forward projection of an attenuation model,
* \(D\) is a data-consistency term,
* \(R(S)\) is a regularization term,
* \(\lambda\) controls the regularization strength.

For the 1200+ projection acquisitions available in the Walnut dataset, spatial and angular regularization can also be investigated:

$$
R(S)
=
\lambda_s
\|\nabla_{u,v}S\|^2
+
\lambda_\theta
\|\nabla_\theta S\|^2.
$$

This allows scatter to be modeled as a component that is smooth both across the detector and across neighboring projection angles.

---

# 🧠 CBCT → CT and Scatter

CBCT-to-CT image translation can reduce the **appearance and consequences** of scatter-related artifacts, but synthetic CT generation and explicit scatter estimation are not necessarily the same problem.

This project therefore focuses primarily on **estimating the scatter component in projection data**, while also investigating whether CBCT-to-synthetic-CT methods can provide useful priors for the inverse problem.

A possible future pipeline is:

```text
Raw CBCT projections
        │
        ▼
Scatter estimation
        │
        ▼
Scatter correction
        │
        ▼
Corrected projections
        │
        ▼
Log transformation
        │
        ▼
FDK reconstruction
        │
        ▼
Scatter-reduced CBCT
```

A more advanced approach may incorporate synthetic CT:

```text
Raw CBCT projections
        │
        ▼
Initial reconstruction
        │
        ▼
CBCT → synthetic CT
        │
        ▼
Forward projection
        │
        ▼
Primary / scatter estimation
        │
        ▼
Projection correction
        │
        ▼
FDK reconstruction
```

---

# 🧪 Experimental Strategy

The Walnut dataset provides several useful reconstruction references.

The initial experiments will investigate:

### 1. Raw projection analysis

* detector response
* projection intensity distributions
* spatial frequency characteristics
* angular variation
* potential scatter signatures

### 2. Mathematical scatter estimation

Possible approaches include:

* low-frequency filtering
* polynomial models
* spline models
* scatter-kernel methods
* basis-function representations
* regularized inverse problems

### 3. Projection correction

$$
I_{\mathrm{corrected}}
=
I_{\mathrm{raw}}
-
\hat I_{\mathrm{scatter}}
$$

followed by logarithmic transformation and reconstruction.

### 4. Reconstruction comparison

Compare:

$$
\text{uncorrected CBCT}
$$

against

$$
\text{scatter-corrected CBCT}
$$

and, where appropriate, against the available Walnut reconstruction references.

### 5. Machine learning

Future experiments may investigate:

* CNN-based scatter estimation
* U-Net architectures
* physics-informed neural networks
* CBCT → synthetic CT
* synthetic-CT-based scatter estimation
* Monte Carlo-supervised scatter prediction

---

# 📊 Evaluation

Potential evaluation metrics include:

### Projection domain

* scatter estimation error
* RMSE
* MAE
* relative scatter error
* residual error
* spatial-frequency analysis

### Image domain

* HU accuracy
* MAE / RMSE
* SSIM
* PSNR
* image uniformity
* CNR
* cupping artifact reduction
* quantitative CT-number accuracy

The goal is to evaluate both **visual improvement** and **quantitative correction**.

---

# 🛠️ Requirements

The original Walnut reconstruction scripts require the **ASTRA Toolbox**.

For the reconstruction code, use:

* ASTRA Toolbox newer than v2.1, or
* a development version newer than 1.9.0dev.

For Conda, the development version is available through the:

`astra-toolbox/label/dev`

channel.

The original MATLAB `GroundTruthReconstruction.m` additionally uses the **SPOT toolbox**.

These requirements may evolve as the scatter-correction pipeline is developed.

---

# 🏗️ Repository Structure

The repository is currently being organized around the following structure:

```text
cbct-scatterlab/
│
├── README.md
│
├── data/
│   └── README.md
│
├── src/
│   ├── preprocessing/
│   ├── scatter/
│   ├── reconstruction/
│   ├── models/
│   └── evaluation/
│
├── notebooks/
│   ├── exploration/
│   └── experiments/
│
├── configs/
├── tests/
├── results/
└── docs/
```

The Walnut dataset itself should **not be duplicated inside the repository**. Instead, the repository will contain instructions for downloading and organizing the data locally.

---

# 🚀 Roadmap

* [ ] Set up Walnut dataset
* [ ] Load raw projection data
* [ ] Visualize individual projections
* [ ] Analyze all 1201+ projections
* [ ] Implement preprocessing pipeline
* [ ] Implement FDK reconstruction
* [ ] Establish uncorrected baseline
* [ ] Implement initial low-frequency scatter model
* [ ] Implement projection-domain correction
* [ ] Quantify reconstruction improvements
* [ ] Investigate regularized inverse formulation
* [ ] Investigate synthetic-CT-based scatter estimation
* [ ] Investigate Monte Carlo reference data
* [ ] Develop deep-learning scatter estimator
* [ ] Compare analytical, model-based, and learned approaches

---

# ⚠️ Project Status

> **🚧 UNDER CONSTRUCTION**

This repository is currently an **early-stage research project**.

The mathematical formulation, preprocessing pipeline, experimental methodology, and code structure are actively being developed.

Results should therefore be considered **preliminary and experimental**.

---

# 📚 Main References

### Walnut Dataset

Henri Der Sarkissian, Felix Lucka, Maureen van Eijnatten, Giulia Colacicco, Sophia Bethany Coban, Kees Joost Batenburg.

**A Cone-Beam X-Ray CT Data Collection Designed for Machine Learning.**

*Scientific Data*, 6, 215 (2019).

DOI: https://doi.org/10.1038/s41597-019-0235-y

arXiv: https://arxiv.org/abs/1905.04787

### Walnut Reconstruction Codes

The initial reconstruction and preprocessing work is based on the **WalnutReconstructionCodes** project accompanying the dataset.

---

## 👤 Project

**CBCT-ScatterLab**

*Exploring mathematical and computational approaches to scatter estimation and correction in Cone-Beam CT.*

> **Research prototype — work in progress.**

