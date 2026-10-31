from CRABClient.UserUtilities import config
config = config()

#config.General.transferLogs = True
config.General.workArea = 'crab/ZeroBias'

config.JobType.pluginName = 'Analysis'
RunPeriod = "B"
config.JobType.psetName = 'run_miniaod_ul17_%s.py'%(RunPeriod)
config.JobType.allowUndistributedCMSSW = True
config.JobType.inputFiles = ["pileup_2016.txt", "pileup_2017.txt", "pileup_2018.txt"]
config.JobType.outputFiles = ["Offset_Data_UL2017%s.root"%(RunPeriod)]

config.Data.inputDBS = 'global'
config.Data.splitting = 'LumiBased'
#config.Data.unitsPerJob = 20
config.Data.unitsPerJob = 50
config.Data.lumiMask = './Json/Cert_294927-306462_13TeV_UL2017_Collisions17_GoldenJSON.txt'


#config.Site.storageSite = 'T3_KR_KISTI'
#config.Site.storageSite = 'T2_KR_KNU'
#config.Site.storageSite = 'T2_CH_CERNBOX'
config.Site.storageSite = 'T3_KR_KNU'
config.Data.outLFNDirBase = '/store/user/sha/JERNosieStudy/Run17'

if __name__ == '__main__':
    
    from CRABAPI.RawCommand import crabCommand

    config.General.requestName = 'Run2017Bv1-v1'
    config.Data.inputDataset = '/ZeroBias/Run2017B-09Aug2019_UL2017-v1/MINIAOD'
    crabCommand('submit', config = config)

    config.General.requestName = 'Run2017Cv1-v1'
    config.Data.inputDataset = '/ZeroBias/Run2017C-09Aug2019_UL2017-v1/MINIAOD'
    crabCommand('submit', config = config)

    config.General.requestName = 'Run2017Dv1-v1'
    config.Data.inputDataset = '/ZeroBias/Run2017D-09Aug2019_UL2017-v1/MINIAOD'
    crabCommand('submit', config = config)

    config.General.requestName = 'Run2017Ev1-v1'
    config.Data.inputDataset = '/ZeroBias/Run2017E-09Aug2019_UL2017-v1/MINIAOD'
    crabCommand('submit', config = config)

    config.General.requestName = 'Run2017Fv1-v1'
    config.Data.inputDataset = '/ZeroBias/Run2017F-09Aug2019_UL2017-v1/MINIAOD'
    crabCommand('submit', config = config)

    #
    #
