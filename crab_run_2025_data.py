#!/usr/bin/env python3

# CRAB configuration for Run3 2025 ZeroBias Data
# Runs: C, D, E, F, G (Winter25Prompt25) - v1 and v2
#
# Usage:
#   RUN=C DATASET_VERSION=v1 python3 crab_run_2025_data.py
#   RUN=D DATASET_VERSION=v1 python3 crab_run_2025_data.py
#   RUN=E DATASET_VERSION=v1 python3 crab_run_2025_data.py

from CRABClient.UserUtilities import config
from CRABAPI.RawCommand import crabCommand
from CRABClient.ClientExceptions import ClientException
from http.client import HTTPException
import os
import sys

config = config()

# ==============================================================================
# USER SETTINGS
# ==============================================================================

# Submission version tag -- increment when resubmitting, e.g. "v1", "v2"
submission_version = "v2"

# One job per python process.
# Set from shell:
#   RUN=C DATASET_VERSION=v1 python3 crab_run_2025_data.py
run = os.environ.get("RUN", None)
dver = os.environ.get("DATASET_VERSION", "v1")

if run is None:
    raise RuntimeError("Please set RUN environment variable, e.g. RUN=C")

run = run.upper()
dver = dver.lower()

job_key = run + "_" + dver

# ==============================================================================
# Dataset map: run_dataversion -> DAS path
# ==============================================================================

datasets = {
    "C_v1": "/ZeroBias/Run2025C-PromptReco-v1/AOD",
    "C_v2": "/ZeroBias/Run2025C-PromptReco-v2/AOD",

    "D_v1": "/ZeroBias/Run2025D-PromptReco-v1/AOD",

    "E_v1": "/ZeroBias/Run2025E-PromptReco-v1/AOD",

    "F_v1": "/ZeroBias/Run2025F-PromptReco-v1/AOD",
    "F_v2": "/ZeroBias/Run2025F-PromptReco-v2/AOD",

    "G_v1": "/ZeroBias/Run2025G-PromptReco-v1/AOD",
}

if job_key not in datasets:
    print("[ERROR] Unknown job key: " + job_key)
    print("[ERROR] Available job keys:")
    for key in sorted(datasets.keys()):
        print("  " + key)
    sys.exit(1)

# ==============================================================================
# Names
# ==============================================================================

request_name = (
    "PUPPINoiseStudy_Run2025" + run
    + "_" + dver
    + "_" + submission_version
)

input_dataset = datasets[job_key]

# Must match run_aod_data_2025_all.py:
#   OutputName = '_Data_Run2025' + run
#   fileName   = 'Offset' + OutputName + '.root'
output_file = "Offset_Data_Run2025" + run + ".root"

# ==============================================================================
# Common CRAB settings
# ==============================================================================

config.General.requestName = request_name
config.General.workArea = "crab/ZeroBias_Run2025"
config.General.transferOutputs = True
config.General.transferLogs = False

config.JobType.pluginName = "Analysis"
config.JobType.psetName = "run_aod_data_2025_all.py"
config.JobType.allowUndistributedCMSSW = True

# Pass run period and algorithm options to CMSSW cfg
config.JobType.pyCfgParams = [
    "run=" + run,
    "usePuppiWsumCut=True",   # PUPPI RC: isPFInCone requires puppiWsum > 0.0001
]

# Auxiliary input files used by the CMSSW cfg.
config.JobType.inputFiles = [
    "pileup_2025.txt",
#    "Winter25Prompt25_RunCDEFG.root",
]

# Important: this must match the actual TFileService output filename.
config.JobType.outputFiles = [output_file]

config.Data.inputDataset = input_dataset
config.Data.inputDBS = "global"
config.Data.splitting = "LumiBased"
config.Data.unitsPerJob = 10

# Golden JSON
config.Data.lumiMask = "Cert_Collisions2025_391658_398860_Golden.json"

# Ntuple ROOT files are not EDM files, so do not publish.
config.Data.publication = False
config.Data.outputDatasetTag = request_name

config.Site.storageSite = "T3_KR_KNU"
config.Data.outLFNDirBase = "/store/user/sha/JERNoiseStudy/Run3/2025/ZB"

# ==============================================================================
# Submit
# ==============================================================================

print("=" * 70)
print("Submitting: Run 2025" + run + " dataset " + dver)
print("  requestName  : " + config.General.requestName)
print("  inputDataset : " + config.Data.inputDataset)
print("  lumiMask     : " + config.Data.lumiMask)
print("  pyCfgParams  : " + str(config.JobType.pyCfgParams))
print("  psetName     : " + config.JobType.psetName)
print("  inputFiles   : " + str(config.JobType.inputFiles))
print("  outputFiles  : " + str(config.JobType.outputFiles))
print("  unitsPerJob  : " + str(config.Data.unitsPerJob))
print("  outputDir    : " + config.Data.outLFNDirBase)
print("=" * 70)

try:
    crabCommand("submit", config=config)

except HTTPException as hte:
    print("[ERROR] HTTPException during CRAB submit:")
    print(hte)
    sys.exit(1)

except ClientException as cle:
    print("[ERROR] ClientException during CRAB submit:")
    print(cle)
    sys.exit(1)
