### For MC SinglePion ####
from CRABClient.UserUtilities import config 
config = config()


config.section_('General')
config.General.transferOutputs = True
config.section_('JobType')


config.General.workArea = 'crab/MC'
config.JobType.pluginName = 'Analysis'
config.JobType.psetName = 'run_miniaod_ul18_mc.py'
config.JobType.allowUndistributedCMSSW = True
config.JobType.inputFiles = ["pileup_2016.txt", "pileup_2017.txt", "pileup_2018.txt"]
config.JobType.outputFiles = ["Offset_MC.root"]

config.Data.inputDBS = 'global'
config.Data.inputDataset = '/SingleNeutrino/RunIISummer19UL18MiniAODv2-FlatPU0to70_106X_upgrade2018_realistic_v16_L1v1-v1/MINIAODSIM'


config.section_('Data')

config.Data.publication = False
config.Data.splitting = 'EventAwareLumiBased'
config.Data.unitsPerJob = 25000

config.section_('User')
config.section_('Site')
config.Site.storageSite = 'T3_KR_KNU'
config.Data.outLFNDirBase = '/store/user/sha/JERNosieStudy/'




#if __name__ == '__main__':
#    
#    from CRABAPI.RawCommand import crabCommand
#
#    config.General.requestName = 'SingleNeutrino_UL18_FlatPU0to70'
#    crabCommand('submit', config = config)

