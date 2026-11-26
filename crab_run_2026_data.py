#!/usr/bin/env python3

# CRAB configuration for Run3 2026 ZeroBias data (AOD)
#
# Usage:
#   RUN=B GT=<data GT> python3 crab_run_2026_data.py
#   RUN=C GT=<data GT> python3 crab_run_2026_data.py
#   RUN=D GT=<data GT> python3 crab_run_2026_data.py
#
# Lumi masks, copy both into this directory from
#   /eos/user/c/cmsdqm/www/CAF/certification/Collisions26/
#   B, D : Cert_Collisions2026_401624_403937_golden.json
#   C    : Cert_Collisions2026_lowPU.json (golden has no 2026C lumisections)

from CRABClient.UserUtilities import config
from CRABAPI.RawCommand import crabCommand
from CRABClient.ClientExceptions import ClientException
from http.client import HTTPException
import os
import sys

config = config()

submission_version = "v1"

run = os.environ.get("RUN", None)
gt  = os.environ.get("GT", None)
if run is None or gt is None:
    raise RuntimeError("Set RUN and GT, e.g. RUN=B GT=<data GT> python3 crab_run_2026_data.py")
run = run.upper()

datasets = {
    "B": "/ZeroBias/Run2026B-PromptReco-v1/AOD",
    "C": "/ZeroBias/Run2026C-PromptReco-v1/AOD",
    "D": "/ZeroBias/Run2026D-PromptReco-v1/AOD",
}
lumi_masks = {
    "B": "Cert_Collisions2026_401624_403937_golden.json",
    "C": "Cert_Collisions2026_lowPU.json",
    "D": "Cert_Collisions2026_401624_403937_golden.json",
}

if run not in datasets:
    print("[ERROR] Unknown run: " + run + ", valid: " + ", ".join(sorted(datasets.keys())))
    sys.exit(1)

request_name = "PUPPINoiseStudy_Run2026" + run + "_" + submission_version

# Must match run_aod_data_2026_all.py output name
output_file = "Offset_Data_Run2026" + run + ".root"

config.General.requestName     = request_name
config.General.workArea        = "crab/ZeroBias_Run2026"
config.General.transferOutputs = True
config.General.transferLogs    = False

config.JobType.pluginName = "Analysis"
config.JobType.psetName   = "run_aod_data_2026_all.py"
config.JobType.allowUndistributedCMSSW = True
config.JobType.pyCfgParams = [
    "run=" + run,
    "globalTag=" + gt,
]
config.JobType.inputFiles  = ["pileup_2026.txt"]
config.JobType.outputFiles = [output_file]

config.Data.inputDataset  = datasets[run]
config.Data.inputDBS      = "global"
config.Data.splitting     = "LumiBased"
config.Data.unitsPerJob   = 10
config.Data.lumiMask      = lumi_masks[run]
config.Data.publication   = False
config.Data.outputDatasetTag = request_name

config.Site.storageSite   = "T3_KR_KNU"
config.Data.outLFNDirBase = "/store/user/sha/JERNoiseStudy/Run3/2026/ZB"

print("Submitting   : Run 2026" + run)
print("  requestName  : " + config.General.requestName)
print("  inputDataset : " + config.Data.inputDataset)
print("  lumiMask     : " + config.Data.lumiMask)
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
