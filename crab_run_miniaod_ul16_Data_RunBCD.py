from CRABClient.UserUtilities import config
config = config()

#config.General.transferLogs = True
config.General.workArea = 'crab/ZeroBias'

config.JobType.pluginName = 'Analysis'
config.JobType.psetName = 'run_miniaod_ul16_data_RunBCD.py'
config.JobType.allowUndistributedCMSSW = True
config.JobType.inputFiles = ["pileup_2016.txt", "pileup_2017.txt", "pileup_2018.txt"]
config.JobType.outputFiles = ["Offset.root"]

config.Data.inputDBS = 'global'
config.Data.splitting = 'LumiBased'
#config.Data.unitsPerJob = 20
config.Data.unitsPerJob = 50
config.Data.lumiMask = './Json/Cert_271036-284044_13TeV_Legacy2016_Collisions16_JSON.txt'

config.Site.storageSite = 'T3_KR_KNU'
config.Data.outLFNDirBase = '/store/user/sha/JERNosieStudy/Run16'### sha is SK's directory 

if __name__ == '__main__':
    
    from CRABAPI.RawCommand import crabCommand

    config.General.requestName = 'SingleCone_Run2016B_ver1_v2p3'
    config.Data.inputDataset = '/ZeroBias/Run2016B-ver1_HIPM_UL2016_MiniAODv2-v1/MINIAOD'
    crabCommand('submit', config = config)

    config.General.requestName = 'SingleCone_Run2016B_ver2_v2p3'
    config.Data.inputDataset = '/ZeroBias/Run2016B-ver2_HIPM_UL2016_MiniAODv2-v1/MINIAOD'
    crabCommand('submit', config = config)

    config.General.requestName = 'SingleCone_Run2016C_v2p3'
    config.Data.inputDataset = '/ZeroBias/Run2016C-HIPM_UL2016_MiniAODv2-v1/MINIAOD'
    crabCommand('submit', config = config)

    config.General.requestName = 'SingleCone_Run2016D_v2p3'
    config.Data.inputDataset = '/ZeroBias/Run2016D-HIPM_UL2016_MiniAODv2-v1/MINIAOD'
    crabCommand('submit', config = config)


