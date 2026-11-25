# CRAB configuration for Run3 2025 ZeroBias Data
# Runs: C, D, E, F, G (Winter25Prompt25) - v1 and v2
# Usage: python crab_run_miniaod_2025_data.py

from CRABClient.UserUtilities import config
from CRABAPI.RawCommand import crabCommand

config = config()

# ==============================================================================
# USER SETTINGS
# ==============================================================================

# Submission version tag -- increment when resubmitting (e.g. "v1", "v2")
submission_version = "v1"

# Select which jobs to submit
# Format: "Run_DatasetVersion"
# Comment out any line to skip that job
jobs_to_submit = [
#    "C_v1",
#    "C_v2",
    "D_v1",
    "E_v1",
    "F_v1",
    "F_v2",
    "G_v1",
]

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

# ==============================================================================
# Common CRAB settings
# ==============================================================================

config.General.workArea = 'crab/ZeroBias_Run2025'

config.JobType.pluginName  = 'Analysis'
config.JobType.psetName    = 'run_aod_data_2025_all.py'
config.JobType.allowUndistributedCMSSW = True
config.JobType.inputFiles  = [
    "pileup_2025.txt"
#    "Winter25Prompt25_RunCDEFG.root",
]
config.JobType.outputFiles = ["Offset_Data_Run2025.root"]

config.Data.inputDBS    = 'global'
config.Data.splitting   = 'LumiBased'
config.Data.unitsPerJob = 10
config.Data.lumiMask    = 'Cert_Collisions2025_391658_398860_Golden.json'

config.Site.storageSite   = 'T3_KR_KNU'
config.Data.outLFNDirBase = '/store/user/sha/JERNoiseStudy/Run3/2025/ZB'

# ==============================================================================
# Submit loop
# ==============================================================================

if __name__ == '__main__':

    for job_key in jobs_to_submit:

        if job_key not in datasets:
            print("[SKIP] Unknown job key: " + job_key)
            continue

        # Parse run period and dataset version (e.g. "C_v1" -> run="C", dver="v1")
        run, dver = job_key.split("_")

        # Pass run period to the pset via pyCfgParams
        config.JobType.pyCfgParams = ['run=' + run]

        config.General.requestName = (
            'PUPPINoiseStudy_Run2025' + run
            + '_' + dver
            + '_' + submission_version
        )
        config.Data.inputDataset = datasets[job_key]

        print("=" * 70)
        print("Submitting: Run 2025" + run + " (dataset " + dver + ")")
        print("  requestName  : " + config.General.requestName)
        print("  inputDataset : " + config.Data.inputDataset)
        print("  pyCfgParams  : " + str(config.JobType.pyCfgParams))
        print("  psetName     : " + config.JobType.psetName)
        print("  outputDir    : " + config.Data.outLFNDirBase)
        print("=" * 70)

        crabCommand('submit', config=config)
