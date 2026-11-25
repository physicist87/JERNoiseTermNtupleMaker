# OffsetTreeMaker -- PUPPI Noise Analysis

Run3 2025 PUPPI noise study using random cone method.  
Produces ROOT NTuples with PFCHS and PUPPI RC offset information,
particle-type breakdown, and HCAL depth energy sums.

---

## Directory Structure

```
OffsetTreeMaker/
|-- src/
|   `-- OffsetTreeMaker.cc          # Main EDAnalyzer
|-- data/
|   |-- Summer24Prompt24_V3_MC/     # MC JEC files
|   |-- Winter25Prompt25_RunC_V3_DATA/
|   |-- Winter25Prompt25_RunD_V3_DATA/
|   |-- Winter25Prompt25_RunE_V3_DATA/
|   |-- Winter25Prompt25_RunF_V3_DATA/
|   |-- Winter25Prompt25_RunG_V3_DATA/
|   |-- Winter25Prompt25_RunCDEFG.root  # Jet veto map
|   `-- jetveto/
|-- run_miniaod_2025_mc.py          # MC config (cmsRun)
|-- run_aod_data_2025_all.py        # Data config with VarParsing (cmsRun / CRAB pset)
|-- crab_run_miniaod_2025_data.py   # CRAB submission script
`-- README.md
```

---

## Setup

```bash
# CMSSW environment (Run3 2025)
source /cvmfs/cms.cern.ch/cmsset_default.sh
cd $CMSSW_BASE/src
cmsenv
scram b -j8
```

---

## Running Locally with cmsRun

### MC

```bash
cmsRun run_miniaod_2025_mc.py
```

Output: `Offset_MC.root`

**Debug mode** (prints RC summary per event):

Edit `run_miniaod_2025_mc.py`:
```python
isDebug = cms.bool(True)   # line 29
```

---

### Data (Run3 2025, Runs C-G)

```bash
# Run C (default)
cmsRun run_aod_data_2025_all.py run=C

# Run D
cmsRun run_aod_data_2025_all.py run=D

# Run E
cmsRun run_aod_data_2025_all.py run=E

# Run F
cmsRun run_aod_data_2025_all.py run=F

# Run G
cmsRun run_aod_data_2025_all.py run=G
```

Output: `Offset_Data_Run2025{X}.root`

**Debug mode:**

```bash
cmsRun run_aod_data_2025_all.py run=C isDebug=True
```

**Available arguments:**

| Argument | Default | Options | Description |
|----------|---------|---------|-------------|
| `run` | `C` | C, D, E, F, G | Run period |
| `isDebug` | `False` | True, False | Debug printout |

**JEC era mapping:**

| Run | JEC Era |
|-----|---------|
| C | Winter25Prompt25_RunC_V3_DATA |
| D | Winter25Prompt25_RunD_V3_DATA |
| E | Winter25Prompt25_RunE_V3_DATA |
| F | Winter25Prompt25_RunF_V3_DATA |
| G | Winter25Prompt25_RunG_V3_DATA |

> Note: Before running locally, update the input file path in  
> `run_aod_data_2025_all.py` under `process.source`.

---

## Running with CRAB

### Prerequisites

```bash
# CRAB environment
source /cvmfs/cms.cern.ch/crab3/crab.sh
voms-proxy-init --voms cms --valid 168:00
```

### Submit all runs (C, D, E, F, G -- v1 and v2)

```bash
python crab_run_miniaod_2025_data.py
```

This submits 7 jobs in a single command:

| Job key | Run | Dataset version | Request name |
|---------|-----|-----------------|--------------|
| C_v1 | 2025C | PromptReco-v1 | PUPPINoiseStudy_Run2025C_v1_v1 |
| C_v2 | 2025C | PromptReco-v2 | PUPPINoiseStudy_Run2025C_v2_v1 |
| D_v1 | 2025D | PromptReco-v1 | PUPPINoiseStudy_Run2025D_v1_v1 |
| E_v1 | 2025E | PromptReco-v1 | PUPPINoiseStudy_Run2025E_v1_v1 |
| F_v1 | 2025F | PromptReco-v1 | PUPPINoiseStudy_Run2025F_v1_v1 |
| F_v2 | 2025F | PromptReco-v2 | PUPPINoiseStudy_Run2025F_v2_v1 |
| G_v1 | 2025G | PromptReco-v1 | PUPPINoiseStudy_Run2025G_v1_v1 |

### Submit selected runs only

Edit `crab_run_miniaod_2025_data.py`:
```python
jobs_to_submit = [
    "C_v2",   # submit only Run C v2
    "G_v1",   # and Run G v1
]
```

### Resubmit (increment version)

```python
submission_version = "v2"   # was "v1"
```

### CRAB job management

```bash
# Check status
crab status -d crab/ZeroBias_Run2025/crab_PUPPINoiseStudy_Run2025C_v1_v1/

# Resubmit failed jobs
crab resubmit -d crab/ZeroBias_Run2025/crab_PUPPINoiseStudy_Run2025C_v1_v1/

# Get output (if not on T3)
crab getoutput -d crab/ZeroBias_Run2025/crab_PUPPINoiseStudy_Run2025C_v1_v1/
```

Output location: `/store/user/sha/JERNoiseStudy_Run3_2025/`

---

## Debug Flag

Both `SetRandomCone` (PFCHS) and `SetRandomCone_PUPPI` print per-event
summaries when debug is enabled.

| Mode | cmsRun | CRAB |
|------|--------|------|
| ON | `isDebug=True` argument | `pyCfgParams = ['run=C', 'isDebug=True']` |
| OFF (default) | omit argument | omit argument |

Example debug output:

```
[DEBUG] SetRandomCone Summary:
  Total attempts: 2
  Rejected by DeltaR: 1
  Success: YES
  RC1: eta=0.09, phi=-2.99, ET=15.43, nPF=18
  RC2: eta=-0.003, phi=-0.25, ET=11.29, nPF=10
  ET difference: 4.14

[DEBUG] SetRandomCone_PUPPI Summary:
  Total attempts : 1 / 1000
  Success        : YES
  RC1: eta=0.008, phi=2.81, puppiET=0.317, nPF=16
  RC2: eta=-0.20, phi=-0.48, puppiET=0.000, nPF=19
  PUPPI diff    : 0.317
  RC1 type ET   : nh=0 ne=1.77 chm=7.16 chu=1.14 hfh=0 hfe=0
  RC1 HCAL depth: d1=0 d2=0 d3=0 d4=0 d5=0 d6=0 d7=0
```

> Disable debug (`isDebug=False`) for production runs --  
> debug output significantly slows down processing (~15 sec/event).

---

## NTuple Branch Summary

### PFCHS Random Cone (existing)

| Branch | Description |
|--------|-------------|
| `rc_ispair` | RC generation success flag |
| `rc1jet_pt/eta/phi/energy` | RC1 kinematics |
| `rc2jet_pt/eta/phi/energy` | RC2 kinematics |
| `rc_diff` | RC1 ET - RC2 ET |
| `rc1jet_cor` ~ `rc1jet_corAllRCNega` | JEC variants (9 types) |

### PUPPI Random Cone (new)

| Branch | Description |
|--------|-------------|
| `rc_ispair_puppi` | PUPPI RC success flag |
| `rc1jet_puppi_pt/eta/phi/energy` | PUPPI RC1 kinematics |
| `rc_puppi_diff` | PUPPI RC1 ET - RC2 ET |
| `rc1jet_puppiET` / `rc2jet_puppiET` | Sum(pf_et x pf_puppiW) |
| `rc1jet_puppi_cor` ~ `rc1jet_puppi_corAllRCNega` | PUPPI JEC variants (9 types) |

### Particle Type Breakdown (new, Stage 2)

| Branch | Type | Description |
|--------|------|-------------|
| `rc1jet_et_nh` / `rc2jet_et_nh` | Neutral Hadron | Raw ET |
| `rc1jet_et_ne` / `rc2jet_et_ne` | Photon | Raw ET |
| `rc1jet_et_chm` / `rc2jet_et_chm` | Charged (matched) | Raw ET |
| `rc1jet_et_chu` / `rc2jet_et_chu` | Charged (PU) | Raw ET |
| `rc1jet_et_hfh` / `rc2jet_et_hfh` | HF Hadron | Raw ET |
| `rc1jet_et_hfe` / `rc2jet_et_hfe` | HF EM | Raw ET |
| `rc1jet_puppiET_nh` ... | All types | PUPPI-weighted ET |

### HCAL Depth (new, Stage 4)

| Branch | Description |
|--------|-------------|
| `rc1jet_hcalDepth1` ~ `rc1jet_hcalDepth7` | HCAL depth ET sum in RC1 (nh + hfh) |
| `rc2jet_hcalDepth1` ~ `rc2jet_hcalDepth7` | HCAL depth ET sum in RC2 (nh + hfh) |

### Per-particle (existing, for NoiseTerm recomputation)

| Branch | Description |
|--------|-------------|
| `pf_pt/eta/phi/et/energy` | PF candidate kinematics |
| `pf_type` | Particle type (0=chm, 1=chu, 2=nh, 3=ne, 4=hfh, 5=hfe) |
| `pf_puppiW` | PUPPI weight per particle |
| `pf_hcalDepth1` ~ `pf_hcalDepth7` | HCAL depth energy per particle |

---

## Analysis Roadmap

| Stage | Description | Status |
|-------|-------------|--------|
| 1 | PUPPI RC offset (RC1 vs RC2) | NTuple ready |
| 2 | Particle type breakdown | NTuple ready |
| 3 | Fluctuation vs Mean correlation | NoiseTerm (analysis code) |
| 4 | HCAL depth information | NTuple ready |
| 5 | Event mixing (Minsuk) | Pending |
| 6 | Gain / Loss factorization | Pending |

---

## Contact

- Seungkyu Ha (seungkyu.ha@cern.ch)
- CMS JME -- JER/Noise study, Run3 2025
