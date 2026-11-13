# CBCT-ScatterLab / Under construction!!!!!!!!!!!

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

```math
I_{\mathrm{raw}}
=
I_{\mathrm{primary}}
+
I_{\mathrm{scatter}}
+
\epsilon
```

where:

* $$\(I_{\mathrm{raw}}\)$$ is the measured detector signal,
* $$\(I_{\mathrm{primary}}\)$$ is the primary radiation,
* $$\(I_{\mathrm{scatter}}\)$$ is the scattered radiation,
* $$\(\epsilon\)$$ represents noise and other measurement effects.

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

```math
I_{\mathrm{corrected}}
=
I_{\mathrm{raw}}
-
\hat I_{\mathrm{scatter}}.
```

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

```math
\mathrm{CBCT} \rightarrow \mathrm{CT},
```

the project focuses on the underlying inverse problem:

```math
I_{\mathrm{raw}}
=
I_{\mathrm{primary}}
+
I_{\mathrm{scatter}}
+
\epsilon.
```

The goal is to estimate the scatter component:

```math
\hat I_{\mathrm{scatter}}
```

and subsequently obtain corrected projections:

```math
\boxed{
I_{\mathrm{corrected}}
=
I_{\mathrm{raw}}
-
\hat I_{\mathrm{scatter}}
}
```

before reconstruction.

---

# 📐 Inverse Problem

A general formulation considered in this project is:

```math
\hat S
=
\arg\min_S
\left[
D(I_{\mathrm{raw}}-S,\mathcal{P}(\mu))
+
\lambda R(S)
\right]
```

where:

* $$\(S\)$$ is the estimated scatter,
* $$\(I_{\mathrm{raw}}\)$$ is the measured projection,
* $$\(\mathcal{P}(\mu)\)$$ is a forward projection of an attenuation model,
* $$\(D\)$$ is a data-consistency term,
* $$\(R(S)\)$$ is a regularization term,
* $$\(\lambda\)$$ controls the regularization strength.

For the 1200+ projection acquisitions available in the Walnut dataset, spatial and angular regularization can also be investigated:

```math
R(S)
=
\lambda_s
\|\nabla_{u,v}S\|^2
+
\lambda_\theta
\|\nabla_\theta S\|^2.
```

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

```math
I_{\mathrm{corrected}}
=
I_{\mathrm{raw}}
-
\hat I_{\mathrm{scatter}}
```

followed by logarithmic transformation and reconstruction.

### 4. Reconstruction comparison

Compare:

```math
\text{uncorrected CBCT}
```

against

```math
\text{scatter-corrected CBCT}
```

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

### 💻 Computational Resources

The project is intended to support experimentation both locally and through **Google Colab**, particularly for GPU-intensive reconstruction and machine-learning experiments.

### Google Colab

[Google Colab](https://colab.research.google.com/) can be used to run the notebooks without requiring a local GPU installation.

Colab is particularly useful for:

* exploring the Walnut projection data
* preprocessing CBCT projections
* testing reconstruction methods
* running GPU-accelerated reconstruction
* training and evaluating deep-learning models
* experimenting with projection-domain scatter estimation
* prototyping CBCT → synthetic CT models

A typical workflow is:

```text
Walnut dataset
      │
      ▼
Google Colab
      │
      ├── Data loading
      ├── Preprocessing
      ├── Scatter estimation
      ├── Reconstruction
      └── Deep-learning experiments
      │
      ▼
GPU acceleration
```

## GPU Resources

GPU acceleration is expected to be particularly useful for:

* iterative reconstruction
* large-scale projection processing
* Monte Carlo experiments
* 3D neural networks
* CBCT → CT translation
* CNN-based scatter estimation
* physics-informed models

The project will therefore provide notebooks that can detect and use an available CUDA-enabled GPU when running in Colab.

Example environment check:

```python
import torch

print("PyTorch:", torch.__version__)
print("CUDA available:", torch.cuda.is_available())

if torch.cuda.is_available():
    print("GPU:", torch.cuda.get_device_name(0))
```

For CUDA-enabled environments, computationally intensive operations should preferentially be performed on the GPU where supported.

## Reproducibility

Because Google Colab GPU availability and hardware can vary between sessions, experiments should record:

* GPU model
* CUDA version
* Python version
* PyTorch version
* ASTRA Toolbox version
* dataset / walnut identifier
* reconstruction parameters
* model configuration
* random seeds where applicable

This information should be saved alongside experimental results whenever possible.

## Local vs. Colab Execution

The project is intended to support both workflows:

| Environment      | Intended use                                          |
| ---------------- | ----------------------------------------------------- |
| 💻 Local CPU     | preprocessing, visualization, lightweight experiments |
| 🖥️ Local GPU    | large-scale reconstruction and deep learning          |
| ☁️ Google Colab  | reproducible notebooks and GPU experiments            |
| 🚀 GPU resources | computationally intensive training and reconstruction |

The repository will progressively provide **Colab-compatible notebooks** for the main experimental pipelines.


---

## 👤 Project

**CBCT-ScatterLab**

*Exploring mathematical and computational approaches to scatter estimation and correction in Cone-Beam CT.*

> **Research prototype — work in progress.**

# Empirecal Scattering correction (ESC) method

# Mathematical Formulation of Empirical Scatter Correction (ESC) for CBCT

Based on: **Trapp et al., *Medical Physics*, 2022, 49, 4566–4584.**  
DOI: [10.1002/mp.15656](https://doi.org/10.1002/mp.15656)

---

## 1. Introduction and Mathematical Principle

Cone-beam computed tomography (CBCT) measurements are affected by scattered radiation. Scatter adds unwanted intensity to the measured X-ray projections, producing shading, cupping artifacts, and inaccurate CT values.

The objective of **Empirical Scatter Correction (ESC)** is to estimate the scatter intensity from the measured projections and subtract it before image reconstruction.

ESC uses low-frequency basis images to model the scatter distribution. Their contributions are combined using coefficients that are optimized by minimizing a cost function defined in the reconstructed image domain.

The method has two important characteristics:
* **Scatter subtraction** is performed in the projection intensity domain.
* **Parameter optimization** is performed in the reconstructed image domain.

---

## 2. Equation (1): Corrected Projection Intensity

The corrected projection intensity is defined as:

```math

I_{\mathrm{cor}}(u,v,\alpha) = I(u,v,\alpha) - k_\alpha \sum_i c_i B_i(u,v,\alpha) \tag{1}

```

Where:
* $I(u,v,\alpha)$: measured normalized projection intensity.
* $I_{\mathrm{cor}}(u,v,\alpha)$: corrected projection intensity.
* $(u,v)$: detector pixel coordinates.
* $\alpha$: projection angle.
* $B_i(u,v,\alpha)$: low-frequency scatter basis images.
* $c_i$: coefficients weighting the individual basis images.
* $k_\alpha$: scaling factor controlling the correction strength for each projection.

The estimated scatter intensity is:

$$
I_S(u,v,\alpha) = k_\alpha \sum_i c_i B_i(u,v,\alpha)
$$

Therefore:

$$
I_{\mathrm{cor}} = I - I_S
$$

**Interpretation:** ESC estimates the scatter contribution using a weighted combination of basis images and subtracts it from the measured intensity.

---

## 3. Equation (2): Scaling Factor and Prevention of Negative Intensities

To prevent negative corrected intensities, the correction strength is limited by a projection-dependent scaling factor:

```math
k_\alpha = \begin{cases} \displaystyle \min\left(1,\;\min_{u,v}\left[\frac{I(u,v,\alpha)}{\sum_i c_i B_i(u,v,\alpha)}(1-\delta) \right] \right),&\displaystyle\min_{u,v}\left[I(u,v,\alpha) - \sum_i c_i B_i(u,v,\alpha)\right] < 0 \\[14pt] 1, & \text{otherwise} \end{cases} \tag{2}
```

The paper uses:

$$
\delta = \frac{1}{11}
$$

Thus:

$$
1 - \delta = \frac{10}{11}
$$

**Interpretation:**
* If the estimated scatter does not exceed the measured intensity, the scaling factor remains equal to $1$.
* If the estimated scatter would produce negative intensities, the correction is reduced.
* The factor is calculated separately for each projection angle.

This constraint ensures that the corrected intensity remains positive, with a safety margin.

---

## 4. Equation (3): Threshold-Based Optimization Cost Function

The ESC optimization is performed in the reconstructed image domain. The cost function penalizes reconstructed voxels whose CT values lie outside predefined thresholds in the air and soft tissue regions:

```math
C(f) =\sum_{x,y,z}\delta(f(x,y,z))\left(1 +\sqrt{\left|f(x,y,z) - T_{\min,\max}(x,y,z)\right|}\right) \tag{3}
```

Where:
* $f(x,y,z)$: reconstructed CT value at voxel $(x,y,z)$.
* $T_{\min,\max}(x,y,z)$: applicable lower or upper threshold.
* $\delta(f(x,y,z))$: penalty indicator function defined in Equation (4).

The cost increases when reconstructed values fall outside their allowed ranges. The objective of the optimization is:

$$
\min_{\{c_i\}, c_I} C(f)
$$

The coefficients are adjusted to improve the homogeneity of air and soft tissue regions.

---

## 5. Equation (4): Threshold Penalty Indicator

The penalty indicator is defined as:

```math
\delta(f(x,y,z)) = \begin{cases} 0, & T_{\min}(x,y,z) \leq f(x,y,z) \leq T_{\max}(x,y,z) \\ 1, & \text{otherwise} \end{cases} \tag{4}
```

**Interpretation:**
* If the reconstructed voxel is inside the allowed interval, its penalty is zero.
* If the reconstructed voxel is outside the allowed interval, its penalty is one.

The cost function therefore penalizes voxels that do not satisfy the expected CT value thresholds. The thresholds are applied according to the voxel's segmentation class: air, soft tissue, or neither.

---

## 6. Equation (5): CT-Value Thresholds for Air and Soft Tissue

The lower and upper thresholds for each segmented region are determined using:

```math
T_{\min,\max} = \mu_{\mathrm{segment,ref}} \pm 3\sigma_{\mathrm{segment}} \tag{5}
```

Where:
* $\mu_{\mathrm{segment,ref}}$: reference CT value for the segmented region.
* $\sigma_{\mathrm{segment}}$: expected standard deviation of CT values in that region.

The paper gives the following reference values:
* **Air:** $\mu_{\mathrm{air,ref}} = -1000\ \mathrm{HU}$, yielding threshold bounds:
  $$T_{\min,\mathrm{air}} = -1000 - 3\sigma_{\mathrm{air}}, \qquad T_{\max,\mathrm{air}} = -1000 + 3\sigma_{\mathrm{air}}$$
* **Water-equivalent soft tissue:** $\mu_{\mathrm{soft\ tissue,ref}} = 0\ \mathrm{HU}$, yielding threshold bounds:
  $$T_{\min,\mathrm{soft}} = -3\sigma_{\mathrm{soft}}, \qquad T_{\max,\mathrm{soft}} = +3\sigma_{\mathrm{soft}}$$

The standard deviations are estimated from regions of interest in the initial reconstruction.

> **Important:** These are the reference values used in the paper. The standard deviations are measurement-dependent and must be estimated for the dataset being corrected.

---

## 7. Scatter Basis Model: PEP Model

ESC uses low-frequency basis images to estimate the scatter intensity. The paper uses a modification of the PEP model proposed by Ohnesorge et al.

### Equation (6): PEP Scatter Model
The scatter intensity is approximated by:

```math
I_S(u,v) = K_S(u,v) * \left[ W(I(u,v))^m (-\ln(I(u,v)))^n \right] \tag{6}
```

Where:
* $I(u,v)$: normalized measured projection intensity.
* $I_S(u,v)$: estimated scatter intensity.
* $W(I)$: intensity-dependent transfer function.
* $m, n$: empirical model parameters.
* $K_S$: scatter convolution kernel.
* $*$: two-dimensional convolution operator.

The model generates a smooth, low-frequency representation of the scatter distribution.

### Equation (7): Transfer Function
The transfer function is:

```math
W(I(u,v)) =\begin{cases} I(u,v), & I(u,v) < \epsilon \\
0, & \text{otherwise} \end{cases} \tag{7}
```

Where $\epsilon$ is an empirical threshold parameter. This function selects projection intensities below the specified threshold. The selected intensities are combined with the logarithmic projection information and smoothed using the convolution kernel.

### Equation (8): Asymmetric Gaussian Convolution Kernel
The scatter convolution kernel is defined as:

```math
K_S(u - u', v - v') = \exp\left( -\frac{(u-u')^2}{2\sigma_u^2} -\frac{(v-v')^2}{2\sigma_v^2} \right) \tag{8}
```

Where:
* $\sigma_u$: Gaussian standard deviation in the horizontal detector direction.
* $\sigma_v$: Gaussian standard deviation in the vertical detector direction.
* $(u',v')$: coordinates within the convolution neighborhood.

The kernel is truncated to finite dimensions:

$$
K_u = 2\lfloor 3\sigma_u \rfloor + 1, \qquad K_v = 2\lfloor 3\sigma_v \rfloor + 1
$$

The different values of $\sigma_u$ and $\sigma_v$ allow the kernel to model anisotropic scatter distributions.

---

## 8. Construction of the Basis Images

The paper uses four different PEP basis images with different empirical parameters. The parameter sets illustrated in Figure 2 are:

| Basis | $m$ | $n$ | $\epsilon$ | $\sigma_u$ (px) | $\sigma_v$ (px) |
| :---: | :---: | :---: | :---: | :---: | :---: |
| **A** | $2.0$ | $1.8$ | $0.90$ | $85$ | $55$ |
| **B** | $1.3$ | $0.9$ | $0.92$ | $42$ | $42$ |
| **C** | $2.0$ | $1.8$ | $0.90$ | $55$ | $85$ |
| **D** | $0.8$ | $1.1$ | $1.00$ | $32$ | $32$ |

Each parameter set produces a different low-frequency basis image:

$$
B_1 = f_1(I), \quad B_2 = f_2(I), \quad B_3 = f_3(I), \quad B_4 = f_4(I)
$$

The paper also includes a constant-offset basis image:

$$
B_5(u,v,\alpha) = 1
$$

This additional basis allows ESC to account for intensity offsets. For patient data acquired with a bowtie filter, the authors also used a ramp-like basis image. That additional basis is specific to that acquisition situation and is not automatically required for every dataset.

---

## 9. Optimization Parameters

The unknown parameters optimized by ESC are:

$$
\boldsymbol{\theta} = (c_1, c_2, c_3, c_4, c_5, c_I)
$$

Where:
* $c_1, c_2, c_3, c_4$: weights of the four PEP basis images.
* $c_5$: weight of the constant-offset basis.
* $c_I$: intensity scaling parameter applied to the reconstructed volume before cost evaluation.

The coefficients $c_i$ are initialized at small positive values, while $c_I$ is initialized near one. The coefficients $c_i$ are constrained to positive values to prevent physically invalid negative scatter contributions.

---

## 10. Initial Reconstruction and Segmentation

Before optimization, ESC performs an initial reconstruction using the uncorrected normalized projections. The logarithmic projection data are:

$$
p(u,v,\alpha) = -\ln(I(u,v,\alpha))
$$

The initial reconstructed volume is:

$$
f_0 = \mathcal{R}\left[p(u,v,\alpha)\right]
$$

Where $\mathcal{R}$ denotes the reconstruction operator, such as FDK. The initial volume is segmented using Otsu thresholding to generate binary masks $M_{\mathrm{air}}(x,y,z)$ and $M_{\mathrm{soft}}(x,y,z)$, which identify the air and soft tissue regions used to evaluate the cost function. The threshold ranges in Equation (5) are then established for these regions.

---

## 11. Corrected Reconstruction During Optimization

For a given set of coefficients $c_i$, the estimated scatter is:

$$
I_S(u,v,\alpha) = k_\alpha \sum_i c_i B_i(u,v,\alpha)
$$

The corrected intensities and logarithmic projections are:

$$
I_{\mathrm{cor}}(u,v,\alpha) = I(u,v,\alpha) - I_S(u,v,\alpha)
$$

$$
p_{\mathrm{cor}}(u,v,\alpha) = -\ln\left(I(u,v,\alpha) - k_\alpha \sum_i c_i B_i(u,v,\alpha)\right)
$$

The intermediate reconstructed volume used for optimization is:

$$
f = c_I \, \mathcal{R}\left[p_{\mathrm{cor}}(u,v,\alpha)\right]
$$

The coefficient $c_I$ compensates for possible intensity calibration offsets. The cost function is evaluated on this intermediate volume: $C = C(f)$.

---

## 12. Nelder–Mead Optimization

ESC uses the Nelder–Mead simplex optimization algorithm:

```math
\boldsymbol{\theta}_{\mathrm{opt}} = \underset{\boldsymbol{\theta}}{\operatorname{argmin}} \ C\left(f(\boldsymbol{\theta})\right) \quad \text{subject to } c_i \geq 0
```

With optimization parameters $\boldsymbol{\theta} = (c_1, c_2, c_3, c_4, c_5, c_I)$, the procedure is:
1. Initialize the coefficients $c_i$ and $c_I$.
2. Calculate the scaling factor $k_\alpha$.
3. Estimate scatter from the basis images.
4. Subtract scatter from measured intensities.
5. Calculate the negative logarithm of corrected intensities.
6. Reconstruct the corrected projection data.
7. Multiply the reconstructed volume by $c_I$.
8. Calculate the threshold-based cost function.
9. Update the coefficients using Nelder–Mead.
10. Repeat until the prescribed iteration count is reached.

The paper reports that $50$ Nelder–Mead iterations were sufficient in its experiments.

---

## 13. Final Projection Correction

After optimization, the estimated basis coefficients are used to correct the original measured projections:

$$
I_{\mathrm{cor}}(u,v,\alpha) = I(u,v,\alpha) - k_\alpha \sum_i c_{i,\mathrm{opt}} B_i(u,v,\alpha)
$$

Because optimization may use reduced-resolution projections and a subset of projection angles, the basis images are bilinearly upsampled to the original detector resolution, and missing projection-angle basis images are interpolated. The final corrected logarithmic projection data are:

$$
p_{\mathrm{cor}}(u,v,\alpha) = -\ln\left(I_{\mathrm{cor}}(u,v,\alpha)\right)
$$

---

## 14. Final Reconstruction

The final scatter-corrected volume is obtained using:

$$
f_{\mathrm{cor}} = \mathcal{R}\left[-\ln(I_{\mathrm{cor}}(u,v,\alpha))\right]
$$

For our implementation, the reconstruction operator is ASTRA's `FDK_CUDA` algorithm. The final volume can then be compared with the original uncorrected FDK reconstruction.

---

## 15. Complete ESC Algorithm: Mathematical Sequence

* **Step 1 — Normalize projection intensities:** $I = \frac{I_{\mathrm{raw}} - D}{F - D}$, where $D$ is the dark-field image and $F$ is the flat-field image.
* **Step 2 — Select and downsample projection data:** $I(u,v,\alpha) \longrightarrow I'(u',v',\alpha')$.
* **Step 3 — Initial reconstruction:** $f_0 = \mathcal{R}[-\ln(I')]$.
* **Step 4 — Generate air and soft tissue masks:** $$M_{\mathrm{air}}, M_{\mathrm{soft}} = \operatorname{Otsu}(f_0)$$.
* **Step 5 — Calculate threshold intervals:** $T_{\min,\max} = \mu_{\mathrm{segment,ref}} \pm 3\sigma_{\mathrm{segment}}$.
* **Step 6 — Generate PEP basis images:** $B_i = K_{S,i} * \left[ W(I')^{m_i}(-\ln I')^{n_i} \right]$.
* **Step 7 — Initialize optimization parameters:** $\boldsymbol{\theta}_0 = (c_1, c_2, c_3, c_4, c_5, c_I)$.
* **Step 8 — Calculate the scaling factor:** $k_{\alpha'}$ via Equation (2).
* **Step 9 — Estimate and subtract scatter:** $I'_{\mathrm{cor}} = I' - k_{\alpha'} \sum_i c_i B'_i$.
* **Step 10 — Convert to logarithmic projections:** $p'_{\mathrm{cor}} = -\ln(I'_{\mathrm{cor}})$.
* **Step 11 — Reconstruct the intermediate volume:** $f = c_I \mathcal{R}'(p'_{\mathrm{cor}})$.
* **Step 12 — Evaluate the cost function:** $C(f) = \sum_{x,y,z} \delta(f)\left(1 + \sqrt{|f - T_{\min,\max}|}\right)$.
* **Step 13 — Optimize the coefficients:** $\boldsymbol{\theta}_{\mathrm{opt}} = \operatorname{argmin}_{\boldsymbol{\theta}} C(f)$ (repeat Steps 8–13 for $50$ iterations).
* **Step 14 — Upsample and interpolate basis images:** $B'_i \longrightarrow B_i$.
* **Step 15 — Correct the original projection intensities:** $I_{\mathrm{cor}} = I - k_\alpha \sum_i c_{i,\mathrm{opt}} B_i$.
* **Step 16 — Perform final FDK reconstruction:** $f_{\mathrm{cor}} = \mathcal{R}\left[-\ln(I_{\mathrm{cor}})\right]$.

---

## 16. Application to the Walnut1 Dataset

For our study, we apply the ESC formulation to the `Walnut1` dataset, using only the `tubeV1` projections. The implementation retains the original ASTRA geometry and FDK reconstruction framework.

### Computational Stages
1. Load the $1200$ projection images.
2. Apply dark-field and flat-field normalization.
3. Preserve normalized intensity data before logarithmic transformation.
4. Generate the initial FDK reconstruction.
5. Generate four PEP basis images and a constant-offset basis.
6. Segment air and soft tissue in the initial reconstruction.
7. Optimize the basis coefficients and intensity scaling parameter.
8. Subtract estimated scatter from the original normalized projection intensities.
9. Apply the negative logarithm to corrected intensities.
10. Reconstruct the final scatter-corrected Walnut volume using ASTRA `FDK_CUDA`.

> **Important Methodological Note:** The four PEP parameter sets listed above are those illustrated in the paper. They are starting values for reproducing the published formulation, not proof that they are optimal for the `Walnut1` acquisition. Their suitability must be evaluated for this dataset.

---

## Reference

Trapp, P., Maier, J., Susenburger, M., Sawall, S., and Kachelrieß, M. (2022). Empirical scatter correction: CBCT scatter artifact reduction without prior information. *Medical Physics*, 49, 4566–4584. [https://doi.org/10.1002/mp.15656](https://doi.org/10.1002/mp.15656)


