# Configuration for OffsetTreeMaker, Run3 2026 SingleNeutrino MC (AODSIM)
#
# Samples:
#   Flat    Run3Winter26Reco FlatPU0to120, main MC, reweighted to 2026BD mu
#   Nominal Run3Winter26Reco nominal PU, closure with Winter26 QCD
#   LowPU   RunIII2026LowPUSummer26DR, compared with 2026C
#
# Local test:
#   cmsRun run_aod_2026_mc.py sample=Flat maxEvents=10 inputFiles=file:test.root
# CRAB:
#   config.JobType.psetName    = 'run_aod_2026_mc.py'
#   config.JobType.pyCfgParams = ['sample=Flat']

import FWCore.ParameterSet.Config as cms
from FWCore.ParameterSet.VarParsing import VarParsing

options = VarParsing('analysis')
options.register('sample', 'Flat',
    VarParsing.multiplicity.singleton, VarParsing.varType.string,
    'MC sample: Flat, Nominal, LowPU')
options.register('globalTag', '',
    VarParsing.multiplicity.singleton, VarParsing.varType.string,
    'Override the MC global tag, empty means the GT from the dataset name')
options.register('jecEra', 'Summer24Prompt24_V3_MC',
    VarParsing.multiplicity.singleton, VarParsing.varType.string,
    'JEC era in data/, interim JEC until 2026 MC JEC is available')
options.register('jetVetoFile', 'Winter25Prompt25_RunCDEFG.root',
    VarParsing.multiplicity.singleton, VarParsing.varType.string,
    'Jet veto map file in data/jetveto/, interim 2025 map until 2026 map is available')
options.register('isDebug', False,
    VarParsing.multiplicity.singleton, VarParsing.varType.bool,
    'Enable debug printout for SetRandomCone functions')
options.register('usePuppiWsumCut', True,
    VarParsing.multiplicity.singleton, VarParsing.varType.bool,
    'If True, PUPPI RC isPFInCone requires puppiWsum above 0.0001')
options.parseArguments()

# GT taken from the AODSIM dataset names
gt_map = {
    'Flat':    '150X_mcRun3_2026_realistic_v5',
    'Nominal': '150X_mcRun3_2026_realistic_v5',
    'LowPU':   '160X_mcRun3_2026_lowPU_v3',
}
sample = options.sample
if sample not in gt_map:
    raise RuntimeError("Unknown sample: " + sample + ", valid: " + ", ".join(sorted(gt_map.keys())))
globalTag = options.globalTag if options.globalTag != '' else gt_map[sample]

OutputName = '_MC_2026_' + sample

print("Sample          : " + sample)
print("Global tag      : " + globalTag)
print("JEC era         : " + options.jecEra)
print("Jet veto map    : " + options.jetVetoFile)
print("Output          : Offset" + OutputName + ".root")
print("isDebug         : " + str(options.isDebug))
print("usePuppiWsumCut : " + str(options.usePuppiWsumCut))

process = cms.Process("Ana")

process.load("FWCore.MessageService.MessageLogger_cfi")
process.MessageLogger.cerr.FwkReport.reportEvery = 1000

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(options.maxEvents) )
process.options   = cms.untracked.PSet( wantSummary = cms.untracked.bool(True) )

# Local test input, CRAB overrides it
process.source = cms.Source("PoolSource",
    fileNames = cms.untracked.vstring(options.inputFiles)
)

process.load("Configuration.Geometry.GeometryIdeal_cff")
process.load("Configuration.StandardSequences.MagneticField_AutoFromDBCurrent_cff")
process.load("Configuration.StandardSequences.FrontierConditions_GlobalTag_cff")
from Configuration.AlCa.GlobalTag import GlobalTag
process.GlobalTag = GlobalTag(process.GlobalTag, globalTag)

process.TFileService = cms.Service("TFileService",
    fileName = cms.string("Offset" + OutputName + ".root")
)

process.pf = cms.EDAnalyzer("OffsetTreeMaker",
    numSkip    = cms.int32(1),
    puFileName = cms.string("pileup_2026.txt"),
    isMC       = cms.bool(True),
    writeCands     = cms.bool(True),
    writeParticles = cms.bool(False),

    isDebug         = cms.bool(options.isDebug),
    usePuppiWsumCut = cms.bool(options.usePuppiWsumCut),

    pfTag    = cms.InputTag("particleFlow"),
    pvTag    = cms.InputTag("offlinePrimaryVertices"),
    muTag    = cms.InputTag("addPileupInfo"),
    trackTag = cms.InputTag("generalTracks"),

    rhoTag            = cms.InputTag("fixedGridRhoFastjetAll"),
    rhoC0Tag          = cms.InputTag("fixedGridRhoFastjetCentralNeutral"),
    rhoCCTag          = cms.InputTag("fixedGridRhoFastjetCentralChargedPileUp"),
    rhoCentralTag     = cms.InputTag("fixedGridRhoFastjetCentral"),
    rhoCentralCaloTag = cms.InputTag("fixedGridRhoFastjetCentralCalo"),

    pfJetTag      = cms.InputTag("ak4PFJetsCHS"),
    pfJetPuppiTag = cms.InputTag("ak4PFJetsPuppi"),

    puppiWeightTag = cms.InputTag("puppi"),

    Generator    = cms.InputTag("generator"),
    GenParticles = cms.InputTag("genParticles"),
    genTag       = cms.InputTag("genParticles"),
    genJetTag    = cms.InputTag("ak4GenJets"),

    stageL1Trigger = cms.uint32(2),
    hltProcessName = cms.untracked.string("HLT"),
    trigList = cms.vstring(
        'HLT_ZeroBias_part',
        'HLT_ZeroBias_v',
        'HLT_L1MinimumBiasHF_OR_v',
        'HLT_L1MinimumBiasHF0OR_v'
    ),
    bits      = cms.InputTag("TriggerResults","","HLT"),
    prescales = cms.InputTag("patTrigger"),

    etaBinTag = cms.vdouble(
        0, 0.261, 0.522, 0.783, 1.044, 1.305, 1.566,
        1.740, 1.930, 2.043, 2.172, 2.322, 2.500, 2.650,
        2.853, 2.964, 3.139, 3.489, 3.839, 5.191
    ),
    era            = cms.string(options.jecEra),
    jet_type       = cms.string("AK4PFchs"),
    jet_type_puppi = cms.string("AK4PFPuppi"),
    doL1L2L3Res    = cms.bool(True),

    doJetVeto       = cms.bool(True),
    JetVetoFileName = cms.string(options.jetVetoFile),
    JetVetoHistName = cms.vstring("jetvetomap"),
)

process.p = cms.Path( process.pf )
