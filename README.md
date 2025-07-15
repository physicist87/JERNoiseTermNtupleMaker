# JERNoiseTermNtupleMaker

An ntuple producer for studying the Noise Term in Jet Energy Resolution (JER) using Run 2 data.

This project is developed to produce specialized ntuples for the study of the Noise Term in Jet Energy Resolution (JER) based on CMS Run 2 data.
The ntuples are designed to facilitate the direct extraction of the noise term by generating two random cones per event, summing the transverse energy (∑Eₜ) of particles within each cone, and evaluating the pₜ balance between them.

It is implemented within the CMSSW_10_6_30_patch1 framework and supports both interactive and batch processing environments, including CRAB.
Current studies are based on **ZeroBias** data samples and **Single Neutrino** MC samples with flat pileup distributions.

---

## 🔧 Directory Structure

```
NosiseTest/
└── OffsetTreeMaker/
    ├── plugins/         # CMSSW plugin code (OffsetTreeMaker.cc, JetVeto.cpp, etc.)
    ├── python/          # Python modules for CRAB job setup
    ├── data/            # JEC/JER correction files (Summer19UL*)
    ├── Json/            # Certified JSON files
    ├── crab/            # CRAB input samples and working dirs (excluded from Git)
    └── *.py             # Configuration files for cmsRun and CRAB
```

## 💡 Key Features

- CMSSW plugin-based ntuple producer for Jet Energy Resolution (JER) studies
- Configuration files like `run_miniaod_ul16_mc_postvfp.py` are executed with `cmsRun`
- Corresponding CRAB job submission scripts like `crab_run_miniaod_ul16_mc_postvfp.py` are run with `python3`
- Organized support for multiple Run 2 eras (UL16, UL17, UL18), both Data and MC
- External corrections and certified JSON files included for reproducibility

---

## 🚀 Quick Start

### 1. Set up the CMSSW environment

source /cvmfs/cms.cern.ch/cmsset_default.sh

```bash
cmsrel CMSSW_10_6_30_patch1
cd CMSSW_10_6_30_patch1/src
cmsenv
```

If needed:

```bash
source /cvmfs/cms.cern.ch/cmsset_default.sh

cmssw-el7 #(we need to use Singularity)

```

### 2. Clone this repository (select the desired branch)

```bash
git clone -b Run2NoiseTermStudy_v1 https://github.com/physicist87/JERNoiseTermNtupleMaker.git

scram b -j4

cd JERNoiseTermNtupleMaker/NosiseTest/OffsetTreeMaker
```

### 3. Run locally using `cmsRun`

```bash
cmsRun run_miniaod_ul16_mc_postvfp.py
```

### 4. Submit jobs via CRAB

First, initialize proxy:

```bash
voms-proxy-init -voms cms
```

Then, submit:

```bash
python3 crab_run_miniaod_ul16_mc_postvfp.py
```

> Make sure your CRAB configuration files are correctly set up for the dataset and era you're processing.

