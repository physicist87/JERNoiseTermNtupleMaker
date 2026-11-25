# PYTHON configuration file for class: OffsetTreeMaker
# Run3 2025 ZeroBias Data - All Runs (C/D/E/F/G)
# Uses VarParsing to receive run period as argument
#
# Local test:
#   cmsRun run_aod_data_2025_all.py run=C
# CRAB:
#   config.JobType.psetName    = 'run_aod_data_2025_all.py'
#   config.JobType.pyCfgParams = ['run=C']

import FWCore.ParameterSet.Config as cms
from FWCore.ParameterSet.VarParsing import VarParsing

# ==============================================================================
# Argument parsing
# ==============================================================================
options = VarParsing('analysis')
options.register(
    'run',
    'D',                          # default
    VarParsing.multiplicity.singleton,
    VarParsing.varType.string,
    'Run period: C, D, E, F, G'
)
options.register(
    'isDebug',
    False,                        # default: silent for production
    VarParsing.multiplicity.singleton,
    VarParsing.varType.bool,
    'Enable debug printout for SetRandomCone functions'
)
options.register(
    'usePuppiWsumCut',
    False,                        # default: any particle in cone (standard mode)
    VarParsing.multiplicity.singleton,
    VarParsing.varType.bool,
    'If True: PUPPI RC isPFInCone requires puppiWsum > 0.0001'
)
options.parseArguments()

# ==============================================================================
# Derived settings
# ==============================================================================
run = options.run

era_map = {
    'C': 'Winter25Prompt25_RunC_V3_DATA',
    'D': 'Winter25Prompt25_RunD_V3_DATA',
    'E': 'Winter25Prompt25_RunE_V3_DATA',
    'F': 'Winter25Prompt25_RunF_V3_DATA',
    'G': 'Winter25Prompt25_RunG_V3_DATA',
}

if run not in era_map:
    raise RuntimeError(
        "Unknown run period: '" + run + "'. "
        "Valid options are: " + ", ".join(sorted(era_map.keys()))
    )

eraName         = era_map[run]
OutputName      = '_Data_Run2025' + run
isMC            = cms.bool(False)
isDebug         = cms.bool(options.isDebug)
usePuppiWsumCut = cms.bool(options.usePuppiWsumCut)

print("=" * 60)
print("Run period      : 2025" + run)
print("JEC era         : " + eraName)
print("Output          : Offset" + OutputName + ".root")
print("isDebug         : " + str(options.isDebug))
print("usePuppiWsumCut : " + str(options.usePuppiWsumCut))
print("=" * 60)

# ==============================================================================
# Process
# ==============================================================================
process = cms.Process("Ana")

process.load("FWCore.MessageService.MessageLogger_cfi")
process.MessageLogger.cerr.FwkReport.reportEvery = 1000

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(10) )
process.options   = cms.untracked.PSet( wantSummary = cms.untracked.bool(True) )
process.options.allowUnscheduled = cms.untracked.bool(True)

# ------------------------------------------------------------------------------
# Input source
# Default: Run2025D local test file
# CRAB overrides this automatically
# Local test for other runs:
#   cmsRun run_aod_data_2025_all.py run=C
# ------------------------------------------------------------------------------
input_files = {

    'C': ['file:/u/user/sha/007e8524-9d77-4de8-bed9-259c9d9973ea.root'],
    'D': ['/store/data/Run2025D/ZeroBias/AOD/PromptReco-v1/000/394/391/00000/0a06e3f3-5730-481c-808b-65d045cdff27.root'],
    'E': ['file:/store/data/Run2025E/ZeroBias/AOD/PromptReco-v1/PLACEHOLDER_E.root'],
    'F': ['file:/store/data/Run2025F/ZeroBias/AOD/PromptReco-v1/PLACEHOLDER_F.root'],
    'G': ['file:/store/data/Run2025G/ZeroBias/AOD/PromptReco-v1/PLACEHOLDER_G.root'],
}

process.source = cms.Source("PoolSource",
    fileNames = cms.untracked.vstring(input_files[run])
)

# ------------------------------------------------------------------------------
# Geometry, MagneticField, GlobalTag
# ------------------------------------------------------------------------------
process.load("Configuration.Geometry.GeometryIdeal_cff")
process.load("Configuration.StandardSequences.MagneticField_AutoFromDBCurrent_cff")
process.load("Configuration.StandardSequences.FrontierConditions_GlobalTag_cff")
from Configuration.AlCa.GlobalTag import GlobalTag
process.GlobalTag = GlobalTag(process.GlobalTag, '150X_dataRun3_Prompt_v1')

# ------------------------------------------------------------------------------
# Output
# ------------------------------------------------------------------------------
process.TFileService = cms.Service("TFileService",
    fileName = cms.string("Offset" + OutputName + ".root")
)

# ------------------------------------------------------------------------------
# Filters (Data only)
# ------------------------------------------------------------------------------

# ZeroBias HLT trigger
process.HLTZeroBias = cms.EDFilter("HLTHighLevel",
    TriggerResultsTag  = cms.InputTag("TriggerResults","","HLT"),
    HLTPaths           = cms.vstring('HLT_ZeroBias_part*', 'HLT_ZeroBias_v*'),
    eventSetupPathsKey = cms.string(''),
    andOr              = cms.bool(True),
    throw              = cms.bool(False)
)

# Beam Halo filter
process.load('RecoMET.METFilters.CSCTightHaloFilter_cfi')

# HCAL HBHE noise filter
process.load('CommonTools.RecoAlgos.HBHENoiseFilterResultProducer_cfi')
process.HBHENoiseFilterResultProducer.minZeros = cms.int32(99999)
process.ApplyBaselineHBHENoiseFilter = cms.EDFilter('BooleanFlagFilter',
    inputLabel      = cms.InputTag('HBHENoiseFilterResultProducer',
                                   'HBHENoiseFilterResultRun2Tight'),
    reverseDecision = cms.bool(False)
)

# ------------------------------------------------------------------------------
# Main analyzer
# ------------------------------------------------------------------------------
process.pf = cms.EDAnalyzer("OffsetTreeMaker",
    numSkip    = cms.int32(1),
    puFileName = cms.string("pileup_2025.txt"),
    isMC       = isMC,
    writeCands     = cms.bool(True),
    writeParticles = cms.bool(False),

    isDebug        = isDebug,
    usePuppiWsumCut = usePuppiWsumCut,

    # InputTags (AOD)
    pfTag    = cms.InputTag("particleFlow"),
    pvTag    = cms.InputTag("offlinePrimaryVertices"),
    muTag    = cms.InputTag("addPileupInfo"),
    trackTag = cms.InputTag("generalTracks"),

    # Rho tags
    rhoTag            = cms.InputTag("fixedGridRhoFastjetAll"),
    rhoC0Tag          = cms.InputTag("fixedGridRhoFastjetCentralNeutral"),
    rhoCCTag          = cms.InputTag("fixedGridRhoFastjetCentralChargedPileUp"),
    rhoCentralTag     = cms.InputTag("fixedGridRhoFastjetCentral"),
    rhoCentralCaloTag = cms.InputTag("fixedGridRhoFastjetCentralCalo"),

    # Jets
    pfJetTag = cms.InputTag("ak4PFJetsCHS"),

    # PUPPI weights (AOD)
    puppiWeightTag = cms.InputTag("puppi"),

    # MC-only tags (kept to avoid code mismatch; ignored when isMC=False)
    Generator    = cms.InputTag("generator"),
    GenParticles = cms.InputTag("genParticles"),
    genTag       = cms.InputTag("genParticles"),
    genJetTag    = cms.InputTag("ak4GenJets"),

    # HLT / Trigger
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

    # JEC
    etaBinTag = cms.vdouble(
        0, 0.261, 0.522, 0.783, 1.044, 1.305, 1.566,
        1.740, 1.930, 2.043, 2.172, 2.322, 2.500, 2.650,
        2.853, 2.964, 3.139, 3.489, 3.839, 5.191
    ),
    era            = cms.string(eraName),
    jet_type       = cms.string("AK4PFchs"),
    jet_type_puppi = cms.string("AK4PFPuppi"),
    doL1L2L3Res    = cms.bool(True),

    # Jet Veto Map
    doJetVeto       = cms.bool(True),
    JetVetoFileName = cms.string("Winter25Prompt25_RunCDEFG.root"),
    JetVetoHistName = cms.vstring("jetvetomap"),
)

# ------------------------------------------------------------------------------
# Path
# ------------------------------------------------------------------------------
process.myseq = cms.Sequence(process.pf)

process.p = cms.Path(
    process.HLTZeroBias                   *
    process.CSCTightHaloFilter            *
    process.HBHENoiseFilterResultProducer *
    process.ApplyBaselineHBHENoiseFilter  *
    process.myseq
)
