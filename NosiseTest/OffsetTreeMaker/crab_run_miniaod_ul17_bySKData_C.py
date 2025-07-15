from CRABClient.UserUtilities import config
config = config()

#config.General.transferLogs = True
config.General.workArea = 'crab/ZeroBias'

config.JobType.pluginName = 'Analysis'
#RunPeriod = "F"
#RunPeriod = "E"
#RunPeriod = "D"
RunPeriod = "C"
#RunPeriod = "B"
config.JobType.psetName = 'run_miniaod_ul17_%s.py'%(RunPeriod)
config.JobType.allowUndistributedCMSSW = True
config.JobType.inputFiles = ["pileup_2016.txt", "pileup_2017.txt", "pileup_2018.txt"]
config.JobType.outputFiles = ["Offset_Data_UL2017%s.root"%(RunPeriod)]

config.Data.inputDBS = 'global'
config.Data.splitting = 'LumiBased'
config.Data.unitsPerJob = 20
#config.Data.unitsPerJob = 40
#config.Data.unitsPerJob = 50
#config.Data.unitsPerJob = 100
config.Data.lumiMask = './Json/Cert_294927-306462_13TeV_UL2017_Collisions17_GoldenJSON.txt'


#config.Site.storageSite = 'T3_KR_KISTI'
#config.Site.storageSite = 'T2_KR_KNU'
#config.Site.storageSite = 'T2_CH_CERNBOX'
config.Site.storageSite = 'T3_KR_KNU'
config.Data.outLFNDirBase = '/store/user/sha/JERNosieStudy/Run17'

if __name__ == '__main__':
    
    from CRABAPI.RawCommand import crabCommand

    if RunPeriod ==  "B":
        config.General.requestName = 'SingleCone_Run2017Bv1-v2p3'
        config.Data.inputDataset = '/ZeroBias/Run2017B-UL2017_MiniAODv2-v1/MINIAOD'
        crabCommand('submit', config = config)

    if RunPeriod ==  "C":
        config.General.requestName = 'SingleCone_Run2017Cv1-v2p3'
        config.Data.inputDataset = '/ZeroBias/Run2017C-UL2017_MiniAODv2-v1/MINIAOD'
        crabCommand('submit', config = config)

    if RunPeriod ==  "D":
        config.General.requestName = 'SingleCone_Run2017Dv1-v2p3'
        config.Data.inputDataset = '/ZeroBias/Run2017D-UL2017_MiniAODv2-v1/MINIAOD'
        crabCommand('submit', config = config)

    if RunPeriod ==  "E":
        config.General.requestName = 'SingleCone_Run2017Ev1-v2p3'
        config.Data.inputDataset = '/ZeroBias/Run2017E-UL2017_MiniAODv2-v1/MINIAOD'
        crabCommand('submit', config = config)

    if RunPeriod ==  "F":
        #config.General.requestName = 'Run2017Fv1-v3'
        config.General.requestName = 'SingleCone_Run2017Fv1-v2p3'
        config.Data.inputDataset = '/ZeroBias/Run2017F-UL2017_MiniAODv2-v1/MINIAOD'
        crabCommand('submit', config = config)



    #
    #
