#!/usr/bin/env python3

# CRAB configuration for Run3 2026 SingleNeutrino MC (AODSIM)
#
# Usage:
#   SAMPLE=Flat    python3 crab_run_2026_mc.py
#   SAMPLE=Nominal python3 crab_run_2026_mc.py
#   SAMPLE=LowPU   python3 crab_run_2026_mc.py

from CRABClient.UserUtilities import config
from CRABAPI.RawCommand import crabCommand
from CRABClient.ClientExceptions import ClientException
from http.client import HTTPException
import os
import sys

config = config()

submission_version = "v1"

sample = os.environ.get("SAMPLE", None)
if sample is None:
    raise RuntimeError("Set SAMPLE, e.g. SAMPLE=Flat python3 crab_run_2026_mc.py")

datasets = {
    "Flat":    "/SingleNeutrino_E-10_gun/Run3Winter26Reco-FlatPU0to120_150X_mcRun3_2026_realistic_v5-v2/AODSIM",
    "Nominal": "/SingleNeutrino_E-10_gun/Run3Winter26Reco-150X_mcRun3_2026_realistic_v5-v2/AODSIM",
    "LowPU":   "/SingleNeutrino_E-10_gun/RunIII2026LowPUSummer26DR-DRLowPU_160X_mcRun3_2026_lowPU_v3-v2/AODSIM",
}

if sample not in datasets:
    print("[ERROR] Unknown sample: " + sample + ", valid: " + ", ".join(sorted(datasets.keys())))
    sys.exit(1)

request_name = "PUPPINoiseStudy_SingleNu_2026_" + sample + "_" + submission_version

# Must match run_aod_2026_mc.py output name
output_file = "Offset_MC_2026_" + sample + ".root"

config.General.requestName     = request_name
config.General.workArea        = "crab/MC_Run3_2026"
config.General.transferOutputs = True
config.General.transferLogs    = False

config.JobType.pluginName = "Analysis"
config.JobType.psetName   = "run_aod_2026_mc.py"
config.JobType.allowUndistributedCMSSW = True
config.JobType.pyCfgParams = ["sample=" + sample]
config.JobType.inputFiles  = ["pileup_2026.txt"]
config.JobType.outputFiles = [output_file]

config.Data.inputDataset = datasets[sample]
config.Data.inputDBS     = "global"
config.Data.splitting    = "FileBased"
config.Data.unitsPerJob  = 1
config.Data.publication  = False
config.Data.outputDatasetTag = request_name

config.Site.storageSite   = "T3_KR_KNU"
config.Data.outLFNDirBase = "/store/user/sha/JERNoiseStudy/Run3/2026/MC"

print("Submitting   : " + sample)
print("  requestName  : " + config.General.requestName)
print("  inputDataset : " + config.Data.inputDataset)
print("  pyCfgParams  : " + str(config.JobType.pyCfgParams))
print("  outputFiles  : " + str(config.JobType.outputFiles))
print("  outputDir    : " + config.Data.outLFNDirBase)

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
