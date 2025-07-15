### For MC SinglePion ####
from CRABClient.UserUtilities import config 
config = config()


config.section_('General')
config.General.transferOutputs = True
config.section_('JobType')


config.General.workArea = 'crab/MC'
config.JobType.pluginName = 'Analysis'
config.JobType.psetName = 'run_miniaod_ul16_mc_prevfp.py'
config.JobType.allowUndistributedCMSSW = True
config.JobType.inputFiles = ["pileup_2016.txt", "pileup_2017.txt", "pileup_2018.txt"]
config.JobType.outputFiles = ["Offset.root"]
#config.JobType.maxMemoryMB = 4500

config.Data.inputDBS = 'global'
#config.Data.inputDataset = '/SingleNeutrino/RunIISummer19UL18MiniAODv2-FlatPU0to70_106X_upgrade2018_realistic_v16_L1v1-v1/MINIAODSIM'
#config.Data.inputDataset = '/SingleNeutrino/RunIISummer19UL18MiniAODv2-FlatPU0to70_UL18HEMreReco_106X_upgrade2018_realistic_v16_L1v1-v1/MINIAODSIM'
                           #'/SingleNeutrino/RunIISummer19UL18MiniAODv2-FlatPU0to70_UL18HEMreReco_106X_upgrade2018_realistic_v16_L1v1-v1/MINIAODSIM'
#config.Data.inputDataset = '/TTbar_13TeV_TuneCP5_Pythia8/RunIISummer19UL18MiniAODv2-106X_upgrade2018_realistic_v16_L1v1-v1/MINIAODSIM'
#config.Data.inputDataset = '/QCD_Pt-15to7000_TuneCP5_Flat2018_13TeV_pythia8/RunIISummer19UL18MiniAODv2-FlatPU0to70_106X_upgrade2018_realistic_v16_L1v1-v1/MINIAODSIM'


config.section_('Data')

config.Data.publication = False
#config.Data.splitting = 'FileBased'
#config.Data.splitting = 'EventAwareLumiBased'
#config.Data.unitsperjob = 25000
config.Data.splitting = 'EventAwareLumiBased'
config.Data.unitsPerJob = 25000

#config.Data.unitsPerJob = 1
#config.Data.runRange = ''
#config.Data.lumiMask  = ''
#config.Data.totalUnits = 120 #number of total files (FileBased)
config.section_('User')
config.section_('Site')
config.Site.storageSite = 'T3_KR_KNU'
#config.Data.outLFNDirBase = '/store/user/sha/JERNosieStudy/SingleNeutrino-HEMreReco'
config.Data.outLFNDirBase = '/store/user/sha/JERNosieStudy/SingleNeutrino/Run16'
#config.Data.outLFNDirBase = '/store/user/sha/JERNosieStudy/TTbar'
#config.Data.outLFNDirBase = '/store/user/sha/JERNosieStudy/QCD'




if __name__ == '__main__':
    
    from CRABAPI.RawCommand import crabCommand

    #config.General.requestName = 'SingleNeutrino_UL18_FlatPU0to75-v1'
    #config.General.requestName = 'SingleNeutrino_UL18_FlatPU0to70_HEM-2'
    #config.General.requestName = 'SingleNeutrino_UL18_FlatPU0to70_v2'
    #config.General.requestName = 'SingleNeutrino_UL18_FlatPU0to70_HEM_v2'

    #config.General.requestName = 'SingleNeutrino_UL18_FlatPU0to70_v9'
    #config.Data.inputDataset = '/SingleNeutrino/RunIISummer19UL18MiniAODv2-FlatPU0to70_106X_upgrade2018_realistic_v16_L1v1-v1/MINIAODSIM'
    #crabCommand('submit', config = config)

    #config.General.requestName = 'SingleNeutrino_UL18_FlatPU0to70_HEM_v9'
    #config.Data.inputDataset = '/SingleNeutrino/RunIISummer19UL18MiniAODv2-FlatPU0to70_UL18HEMreReco_106X_upgrade2018_realistic_v16_L1v1-v1/MINIAODSIM'
    #crabCommand('submit', config = config)

    #config.General.requestName = 'SingleNeutrino_UL16_PreVFP_MiniAODv2_v9'
    #config.Data.inputDataset = '/SingleNeutrino/RunIISummer20UL16MiniAODAPVv2-FlatPU0to75_106X_mcRun2_asymptotic_preVFP_v11_ext1-v1/MINIAODSIM'
    #crabCommand('submit', config = config)

    config.General.requestName = 'SingleCone_UL16_FlatPU0to75_PreVFP_v2p3'
    #config.Data.inputDataset = '/SingleNeutrino/RunIISummer20UL16MiniAODAPVv2-FlatPU0to75_106X_mcRun2_asymptotic_preVFP_v11_ext1-v1/MINIAODSIM'
    #config.Data.inputDataset = '/SingleNeutrino/RunIISummer20UL16MiniAODv2-FlatPU0to70_106X_mcRun2_asymptotic_v17-v1/MINIAODSIM'
    config.Data.inputDataset = '/SingleNeutrino/RunIISummer20UL16MiniAODAPVv2-FlatPU0to75_106X_mcRun2_asymptotic_preVFP_v11_ext1-v1/MINIAODSIM'
    crabCommand('submit', config = config)
