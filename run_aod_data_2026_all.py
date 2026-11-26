# Configuration for OffsetTreeMaker, Run3 2026 ZeroBias data (AOD), runs B, C, D
#
# Local test:
#   cmsRun run_aod_data_2026_all.py run=B maxEvents=10 inputFiles=<AOD file>
# CRAB:
#   config.JobType.psetName    = 'run_aod_data_2026_all.py'
#   config.JobType.pyCfgParams = ['run=B']

import FWCore.ParameterSet.Config as cms
from FWCore.ParameterSet.VarParsing import VarParsing

options = VarParsing('analysis')
options.register('run', 'B',
    VarParsing.multiplicity.singleton, VarParsing.varType.string,
    'Run period: B, C, D')
options.register('globalTag', '161X_dataRun3_Prompt_v1',
    VarParsing.multiplicity.singleton, VarParsing.varType.string,
    'Data global tag, default is the PPD 2026 MINI and NANO prompt GT')
options.register('jecEra', 'Winter25Prompt25_RunG_V3_DATA',
    VarParsing.multiplicity.singleton, VarParsing.varType.string,
    'JEC era in data/, interim 2025 JEC until 2026 JEC is available')
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

run = options.run
validRuns = ['B', 'C', 'D']
if run not in validRuns:
    raise RuntimeError("Unknown run period: " + run + ", valid: " + ", ".join(validRuns))

OutputName = '_Data_Run2026' + run

print("Run period      : 2026" + run)
print("Global tag      : " + options.globalTag)
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
input_files = {
    'B': ['/store/data/Run2026B/ZeroBias/AOD/PromptReco-v1/PLACEHOLDER_B.root'],
    'C': ['/store/data/Run2026C/ZeroBias/AOD/PromptReco-v1/PLACEHOLDER_C.root'],
    'D': ['/store/data/Run2026D/ZeroBias/AOD/PromptReco-v1/PLACEHOLDER_D.root'],
}
fileNames = options.inputFiles if len(options.inputFiles) > 0 else input_files[run]
process.source = cms.Source("PoolSource",
    fileNames = cms.untracked.vstring(fileNames)
)

process.load("Configuration.Geometry.GeometryIdeal_cff")
process.load("Configuration.StandardSequences.MagneticField_AutoFromDBCurrent_cff")
process.load("Configuration.StandardSequences.FrontierConditions_GlobalTag_cff")
from Configuration.AlCa.GlobalTag import GlobalTag
process.GlobalTag = GlobalTag(process.GlobalTag, options.globalTag)

process.TFileService = cms.Service("TFileService",
    fileName = cms.string("Offset" + OutputName + ".root")
)

# ZeroBias HLT
process.HLTZeroBias = cms.EDFilter("HLTHighLevel",
    TriggerResultsTag  = cms.InputTag("TriggerResults","","HLT"),
    HLTPaths           = cms.vstring('HLT_ZeroBias_part*', 'HLT_ZeroBias_v*'),
    eventSetupPathsKey = cms.string(''),
    andOr              = cms.bool(True),
    throw              = cms.bool(False)
)

# Beam halo filter
process.load('RecoMET.METFilters.CSCTightHaloFilter_cfi')

# HBHE noise filter
process.load('CommonTools.RecoAlgos.HBHENoiseFilterResultProducer_cfi')
process.HBHENoiseFilterResultProducer.minZeros = cms.int32(99999)
process.ApplyBaselineHBHENoiseFilter = cms.EDFilter('BooleanFlagFilter',
    inputLabel      = cms.InputTag('HBHENoiseFilterResultProducer',
                                   'HBHENoiseFilterResultRun2Tight'),
    reverseDecision = cms.bool(False)
)

process.pf = cms.EDAnalyzer("OffsetTreeMaker",
    numSkip    = cms.int32(1),
    puFileName = cms.string("pileup_2026.txt"),
    isMC       = cms.bool(False),
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

    # MC only tags, ignored for data
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

process.myseq = cms.Sequence(process.pf)

process.p = cms.Path(
    process.HLTZeroBias                   *
    process.CSCTightHaloFilter            *
    process.HBHENoiseFilterResultProducer *
    process.ApplyBaselineHBHENoiseFilter  *
    process.myseq
)
