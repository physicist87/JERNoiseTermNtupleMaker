from CRABClient.UserUtilities import config
config = config()

#config.General.transferLogs = True
config.General.workArea = 'crab/ZeroBias'

config.JobType.pluginName = 'Analysis'
#config.JobType.psetName = 'run_miniaod_ul18.py'
config.JobType.psetName = 'run_miniaod_ul18_D.py'
config.JobType.allowUndistributedCMSSW = True
config.JobType.inputFiles = ["pileup_2016.txt", "pileup_2017.txt", "pileup_2018.txt"]
config.JobType.outputFiles = ["Offset_Data.root"]

config.Data.inputDBS = 'global'
config.Data.splitting = 'LumiBased'
#config.Data.unitsPerJob = 20
config.Data.unitsPerJob = 50
config.Data.lumiMask = 'Cert_314472-325175_13TeV_Legacy2018_Collisions18_JSON.txt'


#config.Site.storageSite = 'T3_KR_KISTI'
#config.Site.storageSite = 'T2_KR_KNU'
#config.Site.storageSite = 'T2_CH_CERNBOX'
config.Site.storageSite = 'T3_KR_KNU'
config.Data.outLFNDirBase = '/store/user/sha/JERNosieStudy'

if __name__ == '__main__':
    
    from CRABAPI.RawCommand import crabCommand

    #config.General.requestName = 'Run2018Av2'
    #config.General.requestName = 'Run2018Av2-3'
    #config.Data.inputDataset = '/ZeroBias/Run2018A-12Nov2019_UL2018-v2/MINIAOD'
    #crabCommand('submit', config = config)
    """
    config.General.requestName = 'Run2018Bv2'
    config.Data.inputDataset = '/ZeroBias/Run2018B-12Nov2019_UL2018-v2/MINIAOD'
    crabCommand('submit', config = config)

    config.General.requestName = 'Run2018Cv1'
    config.Data.inputDataset = '/ZeroBias/Run2018C-12Nov2019_UL2018_rsb-v1/MINIAOD'
    crabCommand('submit', config = config)
    """

    config.General.requestName = 'Run2018Dv1-2'
    config.Data.inputDataset = '/ZeroBias/Run2018D-12Nov2019_UL2018_rsb-v1/MINIAOD'
    crabCommand('submit', config = config)
