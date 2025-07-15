
import FWCore.ParameterSet.Config as cms
from FWCore.ParameterSet.VarParsing import VarParsing
options = VarParsing ('analysis')
options.register ("isData", False, VarParsing.multiplicity.singleton, VarParsing.varType.bool, "If 'isData' is true, input files should be Real Data...")
options.register ("studyName", "", VarParsing.multiplicity.singleton, VarParsing.varType.string, "StudyName-> outputDir")
options.parseArguments()

process = cms.Process("Ana")

process.load("FWCore.MessageService.MessageLogger_cfi")
#process.MessageLogger.destinations = ['cout', 'cerr']
process.MessageLogger.cerr.FwkReport.reportEvery = 1000

print "MaxEvent %s"%(options.maxEvents)
process.maxEvents = cms.untracked.PSet(
	    input = cms.untracked.int32 (options.maxEvents)
)
process.options   = cms.untracked.PSet( wantSummary = cms.untracked.bool(True) )
process.options.allowUnscheduled = cms.untracked.bool(True)

#readFiles = cms.untracked.vstring(options.inputFiles)

process.source = cms.Source ("PoolSource",
	  fileNames = cms.untracked.vstring (options.inputFiles),
)

print  "options.inputFiles ? %s " %(options.inputFiles) 
#readFiles.extend( [
  #'/store/mc/RunIISummer19UL17MiniAOD/DYJetsToLL_M-50_TuneCP5_13TeV-madgraphMLM-pythia8/MINIAODSIM/106X_mc2017_realistic_v6-v2/280000/54C32B50-BAA7-0F4A-AE88-8742F92B46A8.root'
  #'/store/mc/RunIISummer19UL17MiniAODv2/DYJetsToLL_M-50_TuneCP5_13TeV-madgraphMLM-pythia8/MINIAODSIM/106X_mc2017_realistic_v9-v1/260000/01E437E0-B757-5B45-950F-CF648C57CE32.root'
  #'/store/data/Run2017B/ZeroBias/MINIAOD/09Aug2019_UL2017-v1/260000/0228BC4C-26B0-C04E-8BD3-349FAEC6AABF.root'
  #'/store/data/Run2018A/ZeroBias/MINIAOD/12Nov2019_UL2018-v2/100000/032F9DB5-A22C-5246-A1B3-E0EB0C539A8E.root'
  #'file:/d0/scratch/sha/Analyses/ServiceWork/JERC/NoiseTerm_v1/MakeNtuple_v1/ForSummer20_v1/Summer20/Epsilon/1F947FF9-D51E-DB40-A555-07750268210E.root'
#  options.inputFiles
#] );

#isMC = cms.bool(False)
#isMC = cms.bool(True)
isMC = cms.bool(not options.isData) #data/MC switch
print "=== isMC === %s"%isMC
if isMC:
  OutputName = "_MC"
  process.load( "Configuration.StandardSequences.FrontierConditions_GlobalTag_condDBv2_cff" )
  from Configuration.AlCa.GlobalTag import GlobalTag
  #process.GlobalTag = GlobalTag( process.GlobalTag, '106X_mc2017_realistic_v6' )
  #process.GlobalTag = GlobalTag( process.GlobalTag, '106X_mc2017_realistic_v9' )
  process.GlobalTag = GlobalTag( process.GlobalTag, '106X_mcRun2_asymptotic_v17' )# Epsilon ///
  #eraName = "Summer19UL18_V5_MC"
  eraName = "Summer19UL16_V7_MC"
  jetType_name = "AK4PFchs" # or "AK4PF"

else:
  OutputName = "_Data"

  process.load( "Configuration.Geometry.GeometryIdeal_cff" )
  process.load( "Configuration.StandardSequences.MagneticField_AutoFromDBCurrent_cff" )
  process.load( "Configuration.StandardSequences.FrontierConditions_GlobalTag_condDBv2_cff" )
  from Configuration.AlCa.GlobalTag import GlobalTag
  #process.GlobalTag = GlobalTag( process.GlobalTag, '106X_dataRun2_v20' ) #UL2017 B-F
  #process.GlobalTag = GlobalTag( process.GlobalTag, '106X_dataRun2_v24' ) #UL2018 A-C
  #process.GlobalTag = GlobalTag( process.GlobalTag, '106X_dataRun2_v26' ) #UL2018 D
  process.GlobalTag = GlobalTag( process.GlobalTag, '106X_dataRun2_v32' ) #UL2016 B-H

  # ZeroBias Trigger
  process.HLTZeroBias =cms.EDFilter("HLTHighLevel",
    TriggerResultsTag = cms.InputTag("TriggerResults","","HLT"),
    HLTPaths = cms.vstring('HLT_ZeroBias_part*','HLT_ZeroBias_v*'),
    eventSetupPathsKey = cms.string(''),
    andOr = cms.bool(True), #----- True = OR, False = AND between the HLTPaths
    throw = cms.bool(False)
  )

  #Beam Halo
  process.load('RecoMET.METFilters.CSCTightHaloFilter_cfi')

  #HCAL HBHE
  process.load('CommonTools.RecoAlgos.HBHENoiseFilterResultProducer_cfi')
  process.HBHENoiseFilterResultProducer.minZeros = cms.int32(99999)
  process.ApplyBaselineHBHENoiseFilter = cms.EDFilter('BooleanFlagFilter',
    inputLabel = cms.InputTag('HBHENoiseFilterResultProducer','HBHENoiseFilterResultRun2Tight'),
    reverseDecision = cms.bool(False)
  )
  run = "A"
  OutputName = "_Data_UL2018"+run
  eraName = "Summer19UL18_Run"+run+"_V2_SimpleL1_DATA"
  jetType_name = "AK4PFchs" # or "AK4PF"

print "options.outputFile %s"%(options.outputFile)

process.pf = cms.EDAnalyzer("OffsetTreeMaker",
    numSkip = cms.int32(1),
    #RootFileName = cms.string("Offset" + OutputName + ".root"),
    RootFileName = cms.string( options.studyName+"Offset" + options.outputFile + ".root"),
    #puFileName = cms.string("lumi-per-bx.root"),
    puFileName = cms.string("pileup_2016.txt"),
#    puFileName = cms.string("pileup_2017.txt"),
#    puFileName = cms.string("pileup_2018.txt"),
    isMC = isMC,
    writeCands = cms.bool(False),
    writeParticles = cms.bool(False),
    #trackTag = cms.InputTag("generalTracks"),
    Generator = cms.InputTag("generator"),
    GenParticles = cms.InputTag("prunedGenParticles"),
    genTag = cms.InputTag("packedGenParticles"),
    pfTag = cms.InputTag("packedPFCandidates"),
    pvTag = cms.InputTag("offlineSlimmedPrimaryVertices"),
    muTag = cms.InputTag("slimmedAddPileupInfo"),
    rhoTag = cms.InputTag("fixedGridRhoFastjetAll"),
    rhoC0Tag = cms.InputTag("fixedGridRhoFastjetCentralNeutral"),
    rhoCCTag = cms.InputTag("fixedGridRhoFastjetCentralChargedPileUp"),
    rhoCentralTag = cms.InputTag("fixedGridRhoFastjetCentral"),
    rhoCentralCaloTag = cms.InputTag("fixedGridRhoFastjetCentralCalo"),
    pfJetTag = cms.InputTag("slimmedJets"),
    trigList        = cms.vstring(
                                  ### SingleMuon ###
                                 'HLT_ZeroBias_part',
                                 'HLT_ZeroBias_v',
                                                        ),

    bits  = cms.InputTag("TriggerResults","","HLT"),
    prescales        = cms.InputTag("patTrigger"),
    genJetTag = cms.InputTag("slimmedGenJets"),
    #etaBinTag = cms.vdouble(0,0.5),
    #etaBinTag = cms.vdouble(0,0.5,0.8,1.1,1.3,1.7,1.9,2.1,2.3,2.5,2.8,3.0,3.2,4.7),
    etaBinTag = cms.vdouble(0,0.261, 0.522, 0.783, 1.044, 1.305, 1.566, 1.740, 1.930, 2.043, 2.172, 2.322, 2.500, 2.650, 2.853, 2.964, 3.139, 3.489, 3.839, 5.191),
    era = cms.string(eraName),
    jet_type = cms.string(jetType_name),
    doL1L2L3Res = cms.bool(True),
    doJetVeto = cms.bool(True), 
    JetVetoFileName = cms.string("hotjets-UL16.root"), 
    JetVetoHistName = cms.vstring("h2hot_ul16_plus_hbm2_hbp12_qie11","h2hot_mc")

)

process.myseq = cms.Sequence( process.pf )

if isMC :
  process.p = cms.Path( process.myseq )
else:
  process.p = cms.Path( process.HLTZeroBias * 
                        process.CSCTightHaloFilter *
                        process.HBHENoiseFilterResultProducer *
                        process.ApplyBaselineHBHENoiseFilter *
                        process.myseq )
