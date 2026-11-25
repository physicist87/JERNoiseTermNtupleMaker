# CRAB configuration for Run3 2025 MC (SingleNeutrino, AOD)
# Dataset: /SingleNeutrino_E-10_gun/Run3Winter25Reco-FlatPU0to120_142X_mcRun3_2025_realistic_v9-v3/AODSIM
# Usage: python crab_run_miniaod_2025_mc.py

from CRABClient.UserUtilities import config
from CRABAPI.RawCommand import crabCommand

config = config()

# ==============================================================================
# USER SETTINGS
# ==============================================================================

# Submission version tag -- increment when resubmitting
submission_version = "v1"

# ==============================================================================
# General
# ==============================================================================
config.General.transferOutputs = True
config.General.workArea        = 'crab/MC_Run3_2025'

# ==============================================================================
# JobType
# ==============================================================================
config.JobType.pluginName              = 'Analysis'
config.JobType.psetName                = 'run_aod_2025_mc.py'   # fixed: was run_aod_2025_mc.py
config.JobType.allowUndistributedCMSSW = True
config.JobType.inputFiles              = ["pileup_2025.txt"]
config.JobType.outputFiles             = ["Offset_MC.root"]

# ==============================================================================
# Data
# ==============================================================================
config.Data.inputDBS    = 'global'
config.Data.publication = False
config.Data.splitting   = 'FileBased'
config.Data.unitsPerJob = 1
config.Data.runRange    = ''
config.Data.lumiMask    = ''

# ==============================================================================
# Site
# ==============================================================================
config.Site.storageSite   = 'T3_KR_KNU'
config.Data.outLFNDirBase = '/store/user/sha/JERNoiseStudy/Run3/2025/MC'

# ==============================================================================
# Submit
# ==============================================================================
if __name__ == '__main__':

    # NOTE: usePuppiWsumCut is set directly in run_miniaod_2025_mc.py:
    #   usePuppiWsumCut = cms.bool(True)

    # --------------------------------------------------------------------------
    # SingleNeutrino FlatPU 0-120 (AOD) -- primary MC for noise study
    # --------------------------------------------------------------------------
    config.General.requestName = 'PUPPINoiseStudy_SingleNu_FlatPU0to120_' + submission_version
    config.Data.inputDataset   = (
        '/SingleNeutrino_E-10_gun/Run3Winter25Reco-FlatPU0to120_142X_mcRun3_2025_realistic_v9-v3/AODSIM'
    )
    print("Submitting: " + config.General.requestName)
    print("  dataset: " + config.Data.inputDataset)
    crabCommand('submit', config=config)
