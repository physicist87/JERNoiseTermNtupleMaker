# PYTHON configuration file for class: OffsetTreeMaker
# Author: C. Harrington
# Updated for Run3 2025 Data (Winter25Prompt25)
# Date: June 2026

import FWCore.ParameterSet.Config as cms

process = cms.Process("Ana")

process.load("FWCore.MessageService.MessageLogger_cfi")
process.MessageLogger.cerr.FwkReport.reportEvery = 1000

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(10) )
process.options   = cms.untracked.PSet( wantSummary = cms.untracked.bool(True) )
process.options.allowUnscheduled = cms.untracked.bool(True)

# ==============================================================================
# USER SETTINGS - change these for each run
# ==============================================================================

# Run selection: "C", "D", "E", "F", "G"
run = "C"

# Debug flag
# True  -> print [DEBUG] output per event (development / small test)
# False -> silent (production / large dataset)
isDebug = cms.bool(True)

# Input ZeroBias Data AOD file for the selected run
# Replace with the actual file path for each run
inputFiles = {
    "C": [
        "/u/user/sha/007e8524-9d77-4de8-bed9-259c9d9973ea.root",
    ],
    "D": [
        "file:/path/to/Run2025D/ZeroBias/AOD/file.root",
    ],
    "E": [
        "file:/path/to/Run2025E/ZeroBias/AOD/file.root",
    ],
    "F": [
        "file:/path/to/Run2025F/ZeroBias/AOD/file.root",
    ],
    "G": [
        "file:/path/to/Run2025G/ZeroBias/AOD/file.root",
    ],
}

# ==============================================================================
# Derived settings (do not edit below unless necessary)
# ==============================================================================

isMC       = cms.bool(False)
eraName    = "Winter25Prompt25_Run" + run + "_V3_DATA"
OutputName = "_Data_Run2025" + run
jetType_name = "AK4PFchs"

# ------------------------------------------------------------------------------
# Geometry, MagneticField, GlobalTag
# ------------------------------------------------------------------------------
process.load("Configuration.Geometry.GeometryIdeal_cff")
process.load("Configuration.StandardSequences.MagneticField_AutoFromDBCurrent_cff")
#process.load("Configuration.StandardSequences.FrontierConditions_GlobalTag_condDBv2_cff")
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')
from Configuration.AlCa.GlobalTag import GlobalTag
process.GlobalTag = GlobalTag(process.GlobalTag, '150X_dataRun3_Prompt_v1')

# ------------------------------------------------------------------------------
# Input source
# ------------------------------------------------------------------------------
readFiles = cms.untracked.vstring()
process.source = cms.Source("PoolSource", fileNames = readFiles)
readFiles.extend(inputFiles[run])

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
    andOr              = cms.bool(True),   # OR between paths
    throw              = cms.bool(False)
)

# Beam Halo filter
process.load('RecoMET.METFilters.CSCTightHaloFilter_cfi')

# HCAL HBHE noise filter
process.load('CommonTools.RecoAlgos.HBHENoiseFilterResultProducer_cfi')
process.HBHENoiseFilterResultProducer.minZeros = cms.int32(99999)
process.ApplyBaselineHBHENoiseFilter = cms.EDFilter('BooleanFlagFilter',
    inputLabel    = cms.InputTag('HBHENoiseFilterResultProducer',
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
    writeParticles = cms.bool(False),   # No GenParticles in Data

    # Debug (set isDebug = True at the top of this file)
    isDebug = isDebug,

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
    prescales = cms.InputTag("patTrigger"),   # MiniAOD legacy (not used in AOD)

    # JEC Configuration
    etaBinTag = cms.vdouble(
        0, 0.261, 0.522, 0.783, 1.044, 1.305, 1.566,
        1.740, 1.930, 2.043, 2.172, 2.322, 2.500, 2.650,
        2.853, 2.964, 3.139, 3.489, 3.839, 5.191
    ),
    era            = cms.string(eraName),
    jet_type       = cms.string(jetType_name),   # PFCHS JEC
    jet_type_puppi = cms.string("AK4PFPuppi"),   # PUPPI JEC
    doL1L2L3Res    = cms.bool(True),

    # Jet Veto Map
    doJetVeto       = cms.bool(True),
    JetVetoFileName = cms.string("Winter25Prompt25_RunCDEFG.root"),
    JetVetoHistName = cms.vstring("jetvetomap"),
)

# ------------------------------------------------------------------------------
# Path (Data: HLT + MET filters + analyzer)
# ------------------------------------------------------------------------------
process.myseq = cms.Sequence(process.pf)

process.p = cms.Path(
    process.HLTZeroBias              *
    process.CSCTightHaloFilter       *
    process.HBHENoiseFilterResultProducer *
    process.ApplyBaselineHBHENoiseFilter  *
    process.myseq
)
