from CRABClient.UserUtilities import config 
config = config()
config.section_('General')
config.General.transferOutputs = True
config.section_('JobType')


config.General.workArea = 'crab/MC'
config.JobType.pluginName = 'Analysis'

config.JobType.psetName = 'run_miniaod_ul17_mc.py'
config.JobType.allowUndistributedCMSSW = True
config.JobType.inputFiles = ["pileup_2016.txt", "pileup_2017.txt", "pileup_2018.txt"]
config.JobType.outputFiles = ["Offset_MC.root"]

config.Data.inputDBS = 'global'
config.section_('Data')

config.Data.publication = False
#config.Data.splitting = 'EventAwareLumiBased'
config.Data.splitting = 'FileBased'
config.Data.runRange = ''
config.Data.lumiMask  = ''
config.Data.unitsPerJob = 1
config.JobType.maxMemoryMB = 4000
config.section_('User')
config.section_('Site')
config.Site.storageSite= 'T3_KR_KNU'
#config.data.outlfndirbase = '/store/user/sha/jernosiestudy/singleneutrino-hemrereco'
config.Data.outLFNDirBase = '/store/user/sha/JERNosieStudy/SingleNeutrino/Run17'

#config.data.outlfndirbase = '/store/user/sha/jernosiestudy/ttbar'
#config.data.outlfndirbase = '/store/user/sha/jernosiestudy/qcd'





if __name__ == '__main__':
    
    from CRABAPI.RawCommand import crabCommand

    #config.General.requestName = 'SingleNeutrino_UL17_v5'
    #config.General.requestName = 'SingleNeutrino_UL17_v11-3'
    config.General.requestName = 'SingleCone_UL17_v2p3'
    #config.Data.inputDataset = '/SingleNeutrino/RunIISummer19UL17MiniAOD-FlatPU0to75_106X_mc2017_realistic_v6_ext2-v1/MINIAODSIM'
    config.Data.inputDataset = '/SingleNeutrino/RunIISummer20UL17MiniAODv2-106X_mc2017_realistic_v9-v2/MINIAODSIM'
    crabCommand('submit', config = config)


