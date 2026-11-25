# PYTHON configuration file for class: OffsetTreeMaker
# Author: C. Harrington
# Updated by Minsuk Kim for MINIAOD
# Date:  19 - February - 2018

import FWCore.ParameterSet.Config as cms

process = cms.Process("Ana")

process.load("FWCore.MessageService.MessageLogger_cfi")
process.MessageLogger.cerr.FwkReport.reportEvery = 1000

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(10) )
process.options   = cms.untracked.PSet( wantSummary = cms.untracked.bool(True) )
process.options.allowUnscheduled = cms.untracked.bool(True)

readFiles = cms.untracked.vstring()
process.source = cms.Source ("PoolSource", fileNames = readFiles)
readFiles.extend( [
  'file:/u/user/sha/Develop/ServiceWork/MakeNtuples/Run3_v1/20251029_v2/AODNtuplizer/CMSSW_15_0_15_patch4/src/AODFiles/00c6d550-41fc-4ddf-b074-9d985c934912.root'
] );

#isMC = cms.bool(False)
isMC = cms.bool(True)

# ----------------------------------------------------------------------
# True  -> print [DEBUG] output for SetRandomCone and SetRandomCone_PUPPI
# False -> silent (use for production / large dataset runs)
isDebug         = cms.bool(True)
usePuppiWsumCut = cms.bool(True)   # PUPPI RC: isPFInCone requires puppiWsum > 0.0001
# ----------------------------------------------------------------------

if isMC:
  OutputName = "_MC"
  process.load( "Configuration.Geometry.GeometryIdeal_cff" )
  process.load( "Configuration.StandardSequences.MagneticField_AutoFromDBCurrent_cff" )
  process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')
  from Configuration.AlCa.GlobalTag import GlobalTag
  process.GlobalTag = GlobalTag( process.GlobalTag, '150X_mcRun3_2025_realistic_v6' )
  eraName      = "Summer24Prompt24_V3_MC"
  jetType_name = "AK4PFchs"

else:
  OutputName = "_Data"
  process.load( "Configuration.Geometry.GeometryIdeal_cff" )
  process.load( "Configuration.StandardSequences.MagneticField_AutoFromDBCurrent_cff" )
  process.load( "Configuration.StandardSequences.FrontierConditions_GlobalTag_condDBv2_cff" )
  from Configuration.AlCa.GlobalTag import GlobalTag
  process.GlobalTag = GlobalTag( process.GlobalTag, '150X_dataRun3_Prompt_v1' )

  # ZeroBias Trigger
  process.HLTZeroBias = cms.EDFilter("HLTHighLevel",
    TriggerResultsTag = cms.InputTag("TriggerResults","","HLT"),
    HLTPaths = cms.vstring('HLT_ZeroBias_part*','HLT_ZeroBias_v*'),
    eventSetupPathsKey = cms.string(''),
    andOr = cms.bool(True),
    throw = cms.bool(False)
  )

  # Beam Halo
  process.load('RecoMET.METFilters.CSCTightHaloFilter_cfi')

  # HCAL HBHE
  process.load('CommonTools.RecoAlgos.HBHENoiseFilterResultProducer_cfi')
  process.HBHENoiseFilterResultProducer.minZeros = cms.int32(99999)
  process.ApplyBaselineHBHENoiseFilter = cms.EDFilter('BooleanFlagFilter',
    inputLabel = cms.InputTag('HBHENoiseFilterResultProducer','HBHENoiseFilterResultRun2Tight'),
    reverseDecision = cms.bool(False)
  )
  run         = "A"
  OutputName  = "_Data_UL2018" + run
  eraName     = "Summer19UL18_Run" + run + "_V2_SimpleL1_DATA"
  jetType_name= "AK4PFchs"

process.TFileService = cms.Service("TFileService",
    fileName = cms.string("Offset" + OutputName + ".root")
)

process.pf = cms.EDAnalyzer("OffsetTreeMaker",
    numSkip    = cms.int32(1),
    puFileName = cms.string("pileup_2025.txt"),
    isMC       = isMC,
    writeCands     = cms.bool(True),
    writeParticles = cms.bool(False),   # Data has no GenParticles

    # ----------------------------------------------------------------------
    # True  -> [DEBUG] output per event for both RC functions (development only)
    # False -> silent (production / large dataset)
    isDebug        = isDebug,
    usePuppiWsumCut = cms.bool(False),  # True: isPFInCone requires puppiWsum > 0.0001
    # ----------------------------------------------------------------------

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

    # MC-only tags
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

    # ----------------------------------------------------------------------
    etaBinTag = cms.vdouble(
        0, 0.261, 0.522, 0.783, 1.044, 1.305, 1.566,
        1.740, 1.930, 2.043, 2.172, 2.322, 2.500, 2.650,
        2.853, 2.964, 3.139, 3.489, 3.839, 5.191
    ),
    era            = cms.string(eraName),
    jet_type       = cms.string(jetType_name),          # PFCHS JEC (e.g. AK4PFchs)
    jet_type_puppi = cms.string("AK4PFPuppi"),           # PUPPI JEC
    doL1L2L3Res    = cms.bool(True),
    # ----------------------------------------------------------------------

    # Jet Veto Map
    doJetVeto       = cms.bool(True),
    JetVetoFileName = cms.string("Winter25Prompt25_RunCDEFG.root"),
    JetVetoHistName = cms.vstring("jetvetomap"),
)

process.myseq = cms.Sequence( process.pf )

if isMC:
  process.p = cms.Path( process.myseq )
else:
  process.p = cms.Path(
      process.HLTZeroBias *
      process.CSCTightHaloFilter *
      process.HBHENoiseFilterResultProducer *
      process.ApplyBaselineHBHENoiseFilter *
      process.myseq
  )
