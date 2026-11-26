// -*- C++ -*-
//
// Package:    treemaker/OffsetTreeMaker
// Class:      OffsetTreeMaker
//
/**\class OffsetTreeMaker OffsetTreeMaker.cc treemaker/OffsetTreeMaker/plugins/OffsetTreeMaker.cc

 Description: [one line class summary]

 Implementation:
     [Notes on implementation]
*/
//
// Original Author:  charles harrington
//         Created:  Mon, 09 Nov 2015 17:09:43 GMT
//
//

// system include files
#include <memory>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/one/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/Framework/interface/LuminosityBlock.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/VertexReco/interface/Vertex.h"

// --- Changed for AOD Migration ---
#include "DataFormats/ParticleFlowCandidate/interface/PFCandidate.h"
#include "DataFormats/ParticleFlowCandidate/interface/PFCandidateFwd.h"
#include "DataFormats/HepMCCandidate/interface/GenParticle.h"
#include "DataFormats/HepMCCandidate/interface/GenParticleFwd.h"
#include "DataFormats/JetReco/interface/GenJet.h"
#include "DataFormats/JetReco/interface/GenJetCollection.h"
#include "DataFormats/JetReco/interface/PFJet.h"
#include "DataFormats/JetReco/interface/PFJetCollection.h"
#include "DataFormats/Common/interface/ValueMap.h"
// ----------------------------------

#include "DataFormats/Common/interface/Ref.h"
#include <SimDataFormats/PileupSummaryInfo/interface/PileupSummaryInfo.h>
#include "SimDataFormats/GeneratorProducts/interface/GenEventInfoProduct.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"

// Trigger includes
#include "FWCore/Common/interface/TriggerNames.h"
#include "DataFormats/Common/interface/TriggerResults.h"
/// needed for trigger 
#include "FWCore/Common/interface/TriggerNames.h"
#include "DataFormats/Common/interface/TriggerResults.h"
#include "DataFormats/PatCandidates/interface/TriggerPath.h" 
//#include "DataFormats/PatCandidates/interface/PackedTriggerPrescales.h" // MINIAOD

#include "HLTrigger/HLTcore/interface/HLTConfigProvider.h" // Prescale
#include "HLTrigger/HLTcore/interface/HLTPrescaleProvider.h"
// --- HCAL DetID and Geometry (for Depth analysis) ---
#include "DataFormats/HcalDetId/interface/HcalDetId.h"

#include "FWCore/Framework/interface/Run.h"
#include "FWCore/Utilities/interface/Exception.h"


#include "parsePileUpJSON2.h"
#include "JetVeto.hpp"
#include <vector>
#include "TMath.h"
//root files
#include <TFile.h>
#include <TTree.h>
#include <TH1.h>
#include <TH2.h>
#include <TRandom3.h>
#include <TLorentzVector.h>

///for JEC
#include "CondFormats/JetMETObjects/interface/FactorizedJetCorrector.h"
#include "CondFormats/JetMETObjects/interface/JetCorrectorParameters.h"
#include "CondFormats/JetMETObjects/interface/JetCorrectionUncertainty.h"

//#include "DataFormats/ParticleFlowReco/interface/PFCandidate.h"
#include "DataFormats/ParticleFlowReco/interface/PFBlockElement.h"
#include "DataFormats/ParticleFlowReco/interface/PFBlockElementCluster.h"
#include "DataFormats/ParticleFlowReco/interface/PFCluster.h"
#include "DataFormats/ParticleFlowReco/interface/PFClusterFwd.h"
#include "DataFormats/ParticleFlowCandidate/interface/PFCandidate.h"
#include "DataFormats/ParticleFlowCandidate/interface/PFCandidateFwd.h"

#include "DataFormats/ParticleFlowReco/interface/PFBlock.h"
#include "DataFormats/ParticleFlowReco/interface/PFBlockElement.h"
#include "DataFormats/ParticleFlowReco/interface/PFBlockElementCluster.h"
#include "DataFormats/HcalDetId/interface/HcalDetId.h"

#include "DataFormats/DetId/interface/DetId.h"


using namespace std;

struct RandCone {
    // --- Legacy PFCHS fields ---
    double diff_;
    int num_pfcan_rc1;
    int num_pfcan_rc2;
    bool findRC;
    TLorentzVector rc1;
    TLorentzVector rc2;
    TLorentzVector vecrc1;
    TLorentzVector vecrc2;

    // --- Stage 1: PUPPI ET sum (same cone direction as PFCHS) ---
    double rc1_puppiET;   // Sum(pf_et * pf_puppiW) in cone1
    double rc2_puppiET;   // Sum(pf_et * pf_puppiW) in cone2
    double puppi_diff_;   // rc1_puppiET - rc2_puppiET

    // --- Stage 1: PUPPI weight sum (effective signal particle count) ---
    double rc1_puppiWsum;  // Sum(pf_puppiW) in cone1
    double rc2_puppiWsum;  // Sum(pf_puppiW) in cone2

    // --- Stage 2: per-particle-type raw ET ---
    double rc1_et_nh,  rc2_et_nh;   // neutral hadron  (type=2)
    double rc1_et_ne,  rc2_et_ne;   // photon          (type=3)
    double rc1_et_chm, rc2_et_chm;  // charged matched (type=0)
    double rc1_et_chu, rc2_et_chu;  // charged PU      (type=1)
    double rc1_et_hfh, rc2_et_hfh;  // HF hadron       (type=4)
    double rc1_et_hfe, rc2_et_hfe;  // HF EM           (type=5)

    // --- Stage 2: per-particle-type PUPPI-weighted ET ---
    double rc1_puppiET_nh,  rc2_puppiET_nh;
    double rc1_puppiET_ne,  rc2_puppiET_ne;
    double rc1_puppiET_chm, rc2_puppiET_chm;
    double rc1_puppiET_hfh, rc2_puppiET_hfh;
    double rc1_puppiET_hfe, rc2_puppiET_hfe;
    // chu omitted: PUPPI weight ~ 0 for pileup charged hadrons

    // --- Stage 4: HCAL depth ET sum (depth 1~7, index 0~6) ---
    double rc1_hcalDepth[7];
    double rc2_hcalDepth[7];
};



const int ETA_BINS = 82;
const int ETA_BINS_GME = 18;
const int PHI_BINS_GME = 11;
const double GRID_AREA = (10./ETA_BINS_GME)*(2*M_PI/PHI_BINS_GME);
const int MAXNPV = 150;
const int MAXJETS = 4;
const int MAXGENJETS = 4;

/// for Data File Dist...by Check out for 2018 DATA///
// Run2017 pileup reweight tables -- no longer used for Run3 2025
// float muWeight_Run2017D[80] = { ... };
// float muWeight_Run2017E[80] = { ... };

/// Bin for eta 
float etabins[ETA_BINS+1] =
  {-5.191, -4.889, -4.716, -4.538, -4.363, -4.191, -4.013, -3.839, -3.664, -3.489, -3.314, -3.139, -2.964, -2.853, -2.65,
   -2.5, -2.322, -2.172, -2.043, -1.93, -1.83, -1.74, -1.653, -1.566, -1.479, -1.392, -1.305, -1.218, -1.131, -1.044, -0.957,
   -0.879, -0.783, -0.696, -0.609, -0.522, -0.435, -0.348, -0.261, -0.174, -0.087, 0,
   0.087, 0.174, 0.261, 0.348, 0.435, 0.522, 0.609, 0.696, 0.783, 0.879, 0.957, 1.044, 1.131, 1.218, 1.305, 1.392, 1.479,
   1.566, 1.653, 1.74, 1.83, 1.93, 2.043, 2.172, 2.322, 2.5, 2.65, 2.853, 2.964, 3.139, 3.314, 3.489, 3.664, 3.839, 4.013,
   4.191, 4.363, 4.538, 4.716, 4.889, 5.191};

/// Bin for phi 
float phibins[PHI_BINS_GME+1] = 
  {-3.142, -2.57, -1.999, -1.428, -0.8568, -0.2856, 0.2856, 0.8568, 1.428, 1.999, 2.57, 3.142};

class OffsetTreeMaker : public edm::one::EDAnalyzer<edm::one::SharedResources, edm::one::WatchRuns>  {
  public:
    explicit OffsetTreeMaker(const edm::ParameterSet&);

  private:
    virtual void beginJob() override;
    virtual void analyze(const edm::Event&, const edm::EventSetup&) override;
    virtual void beginRun(const edm::Run&, const edm::EventSetup&) override;
    virtual void endRun(const edm::Run&, const edm::EventSetup&) override {}
    virtual void endJob() override;
    int getEtaIndex(float eta);
    bool IsPFCHS(int itype);
    //double SetRandomCone(double upperEta, double lowerEta);
    RandCone SetRandomCone(double upperEta, double lowerEta, bool isDebug = false);
    RandCone SetRandomCone_PUPPI(double upperEta, double lowerEta,
                                  bool isDebug = false,
                                  bool usePuppiWsumCut = false);  // if true: isPFInCone requires puppiWsum > 0.0001
    enum Flavor{
      chm = 0, chu, nh, ne, hfh, hfe, lep, untrk, numFlavors, X //undefined
    };
    Flavor getFlavor(int id);

    int counter;
    //TFile* root_file;
    edm::Service<TFileService> root_file;
    TTree* tree;




    TH1F* h;
    TH2F* h2_GME; // 2d ET_eta_phi histo 
    TH2F* h2_finnereta;
//    TH2F *h2_GME_gen, *h2_finnereta_gen; // for GenParticle
    TH2F *h2_twopi, *h2_twopi_gen, *h2_twopi_det, *h2_twopi_scaled; // for phi from 0 to 2*Pi
    TRandom3* rand;

    int nEta;
    float energy[ETA_BINS], eRMS[ETA_BINS], et[ETA_BINS], etMED[ETA_BINS], etMEAN[ETA_BINS];
    float et_gme[ETA_BINS_GME][PHI_BINS_GME];
//    float et_gme_gen[ETA_BINS_GME][PHI_BINS_GME];
    UChar_t f[numFlavors][ETA_BINS];  //energy fraction by flavor
//    float et_gen[ETA_BINS], etMED_gen[ETA_BINS], etMEAN_gen[ETA_BINS];
    float et_twopi[ETA_BINS_GME][PHI_BINS_GME], et_twopi_gen[ETA_BINS_GME][PHI_BINS_GME], et_twopi_det[ETA_BINS_GME][PHI_BINS_GME], et_twopi_scaled[ETA_BINS_GME][PHI_BINS_GME];
    float rho_gme, rho_gme_gen, rho_gme_det, rho_gme_scaled;

    ULong64_t event;
    int run, lumi, bx;
    float mu, muWeight;
    float mua[3];    // per-BX actual PU interactions (BX=-1,0,+1)
    float puz[50];   // PU vertex z-positions for in-time BX
    float rho, rhoC0, rhoCC;
    float rhoCentral, rhoCentralCalo;
    float weight;
    int nGenParticles;
    int idHardScat1, idHardScat2, idxHardScat;

    int nPVall, nPV;
    float pv_ndof[MAXNPV], pv_z[MAXNPV], pv_rho[MAXNPV];

    float ht;
    int nJets;
    vector<float> v_jet_eta, v_jet_phi, v_jet_pt, v_jet_area;
    // AK4 PUPPI jets, raw kinematics plus L2L3 (and residual for data) correction factor
    int nJetsPuppi;
    vector<float> v_jetpuppi_eta, v_jetpuppi_phi, v_jetpuppi_pt, v_jetpuppi_area, v_jetpuppi_jec;
    vector<float> v_jet_ch, v_jet_nh, v_jet_ne, v_jet_hfh, v_jet_hfe, v_jet_lep;
    // RC cone variable //
    vector<int> v_rc1jet_numpf, v_rc2jet_numpf;
    vector<float> v_rc1jet_eta, v_rc1jet_phi, v_rc1jet_pt, v_rc1jet_energy;
    vector<float> v_rc2jet_eta, v_rc2jet_phi, v_rc2jet_pt, v_rc2jet_energy;
    vector<float> v_rc_diff;
    vector<float> v_rc1jet_cor,v_rc2jet_cor;
    vector<float> v_rc1jet_coruncUp,v_rc2jet_coruncUp;
    vector<float> v_rc1jet_coruncDown,v_rc2jet_coruncDown;
    vector<float> v_rc1jet_corL1,v_rc2jet_corL1;
    vector<float> v_rc1jet_corL1RC,v_rc2jet_corL1RC;
    vector<float> v_rc1jet_corL1Nega,v_rc2jet_corL1Nega;
    vector<float> v_rc1jet_corL1RCNega,v_rc2jet_corL1RCNega;
    vector<float> v_rc1jet_corAll,v_rc2jet_corAll;
    vector<float> v_rc1jet_corAllRC,v_rc2jet_corAllRC;
    vector<float> v_rc1jet_corAllNega,v_rc2jet_corAllNega;
    vector<float> v_rc1jet_corAllRCNega,v_rc2jet_corAllRCNega;
    std::vector<bool> v_isRC;

    // --- PUPPI RC: cone direction and kinematics ---
    std::vector<bool>  v_isRC_puppi;
    vector<int>   v_rc1jet_puppi_numpf, v_rc2jet_puppi_numpf;
    vector<float> v_rc1jet_puppi_eta,   v_rc2jet_puppi_eta;
    vector<float> v_rc1jet_puppi_phi,   v_rc2jet_puppi_phi;
    vector<float> v_rc1jet_puppi_pt,    v_rc2jet_puppi_pt;
    vector<float> v_rc1jet_puppi_energy,v_rc2jet_puppi_energy;

    // --- PUPPI RC: JEC (same 9 variants as PFCHS) ---
    vector<float> v_rc1jet_puppi_cor,        v_rc2jet_puppi_cor;
    vector<float> v_rc1jet_puppi_coruncUp,   v_rc2jet_puppi_coruncUp;
    vector<float> v_rc1jet_puppi_coruncDown, v_rc2jet_puppi_coruncDown;
    vector<float> v_rc1jet_puppi_corL1,      v_rc2jet_puppi_corL1;
    vector<float> v_rc1jet_puppi_corL1RC,    v_rc2jet_puppi_corL1RC;
    vector<float> v_rc1jet_puppi_corL1Nega,  v_rc2jet_puppi_corL1Nega;
    vector<float> v_rc1jet_puppi_corL1RCNega,v_rc2jet_puppi_corL1RCNega;
    vector<float> v_rc1jet_puppi_corAll,     v_rc2jet_puppi_corAll;
    vector<float> v_rc1jet_puppi_corAllRC,   v_rc2jet_puppi_corAllRC;
    vector<float> v_rc1jet_puppi_corAllNega, v_rc2jet_puppi_corAllNega;
    vector<float> v_rc1jet_puppi_corAllRCNega,v_rc2jet_puppi_corAllRCNega;

    // --- Stage 1: PUPPI ET sum ---
    vector<float> v_rc1jet_puppiET, v_rc2jet_puppiET;
    vector<float> v_rc_puppi_diff;

    // --- Stage 1: PUPPI weight sum ---
    vector<float> v_rc1jet_puppiWsum, v_rc2jet_puppiWsum;

    // --- Stage 2: per-particle-type raw ET ---
    vector<float> v_rc1jet_et_nh,  v_rc2jet_et_nh;
    vector<float> v_rc1jet_et_ne,  v_rc2jet_et_ne;
    vector<float> v_rc1jet_et_chm, v_rc2jet_et_chm;
    vector<float> v_rc1jet_et_chu, v_rc2jet_et_chu;
    vector<float> v_rc1jet_et_hfh, v_rc2jet_et_hfh;
    vector<float> v_rc1jet_et_hfe, v_rc2jet_et_hfe;

    // --- Stage 2: per-particle-type PUPPI-weighted ET ---
    vector<float> v_rc1jet_puppiET_nh,  v_rc2jet_puppiET_nh;
    vector<float> v_rc1jet_puppiET_ne,  v_rc2jet_puppiET_ne;
    vector<float> v_rc1jet_puppiET_chm, v_rc2jet_puppiET_chm;
    vector<float> v_rc1jet_puppiET_hfh, v_rc2jet_puppiET_hfh;
    vector<float> v_rc1jet_puppiET_hfe, v_rc2jet_puppiET_hfe;

    // --- Stage 4: HCAL depth ET sum per cone ---
    vector<float> v_rc1jet_hcalDepth1, v_rc2jet_hcalDepth1;
    vector<float> v_rc1jet_hcalDepth2, v_rc2jet_hcalDepth2;
    vector<float> v_rc1jet_hcalDepth3, v_rc2jet_hcalDepth3;
    vector<float> v_rc1jet_hcalDepth4, v_rc2jet_hcalDepth4;
    vector<float> v_rc1jet_hcalDepth5, v_rc2jet_hcalDepth5;
    vector<float> v_rc1jet_hcalDepth6, v_rc2jet_hcalDepth6;
    vector<float> v_rc1jet_hcalDepth7, v_rc2jet_hcalDepth7;
//    float jet_eta[MAXJETS], jet_phi[MAXJETS], jet_pt[MAXJETS], jet_area[MAXJETS];
//    float jet_ch[MAXJETS], jet_nh[MAXJETS], jet_ne[MAXJETS], jet_hfh[MAXJETS], jet_hfe[MAXJETS], jet_lep[MAXJETS];

    float genht;
    int nGenJets;
    vector<float> v_genjet_eta, v_genjet_phi, v_genjet_pt, v_genjet_area;

    /// varialbes for particle flow object ///
    vector<int> pf_type, pf_fromPV;
    vector<bool> pf_fromPVs;
    // Index of the vertex whose track list holds the charged PF track (-1 if none or neutral)
    vector<int> pf_vtxIdx;
    // Track dz with respect to vertex 0 for charged PF candidates (-999 for neutral or no track)
    vector<float> pf_dzPV0;
    vector<float> pf_pt, pf_eta, pf_phi, pf_et, pf_energy, pf_m, pf_puppiW;
    vector<int> particle_id, particle_charge;
    vector<float> particle_et, particle_pt, particle_eta, particle_phi;
    vector<float> particle_et_scaled;
    vector<bool> isPromptFinalState;


    /// Trigger information // 
    typedef std::vector<pat::TriggerPath>  TriggerPathCollection;
    std::vector<bool> trigger_pass;
    std::vector<bool> trigger_run;
    std::vector<bool> trigger_error;
    std::vector<string> trigger_name;
    //std::vector<unsigned int> trigger_prescale;
    std::vector<double> trigger_prescale;
    bool trigPass_;
    bool trigRun_;
    bool trigError_;
    bool doJetVeto_;

    edm::EDGetTokenT< vector<reco::Vertex> > pvTag_;
    edm::EDGetTokenT< vector<reco::Track> > trackTag_;
    edm::EDGetTokenT< vector<PileupSummaryInfo> > muTag_;
    //edm::EDGetTokenT< vector<pat::PackedCandidate> > pfTag_; // MINIAOD
    //edm::EDGetTokenT< vector<pat::PackedGenParticle> > genTag_; // MINIAOD
    //edm::EDGetTokenT< vector<pat::PackedCandidate> > pfTag_;
    //edm::EDGetTokenT< vector<pat::PackedGenParticle> > genTag_;
    edm::EDGetTokenT< reco::PFCandidateCollection > pfTag_;           // Changed from pat::PackedCandidate
    edm::EDGetTokenT< reco::GenParticleCollection > genparticlesTag_; // Standard AOD GenParticles
    //edm::EDGetTokenT< vector<reco::GenParticle> > genparticlesTag_;
    edm::EDGetTokenT< GenEventInfoProduct > generatorTag_;

    // Note: In AOD, Puppi weights are often stored in a separate ValueMap.
    // If you need Puppi info, we should add a token for it.
    edm::EDGetTokenT< edm::ValueMap<float> > puppiWeightTag_; // PUPPI Weight! 

    edm::EDGetTokenT<double> rhoTag_;
    edm::EDGetTokenT<double> rhoC0Tag_;
    edm::EDGetTokenT<double> rhoCCTag_;
    edm::EDGetTokenT<double> rhoCentralTag_;
    edm::EDGetTokenT<double> rhoCentralCaloTag_;
    //edm::EDGetTokenT< vector<pat::Jet> > pfJetTag_;
    edm::EDGetTokenT< reco::PFJetCollection > pfJetTag_;
    edm::EDGetTokenT< reco::PFJetCollection > pfJetPuppiTag_;
    edm::EDGetTokenT< reco::GenJetCollection > genJetTag_;
    /// variables for trigger ///
    std::vector<std::string> triggerList;
    edm::EDGetTokenT<edm::TriggerResults> triggerBits_;
    //edm::EDGetTokenT<pat::PackedTriggerPrescales> triggerPrescales_; /// MINIAOD
    HLTConfigProvider hltConfig_;
    HLTPrescaleProvider hltPrescaleProvider_;
    std::string hltProcessName_;
    //std::vector<int> trigger_prescale_l1;
    //std::vector<int> trigger_prescale_hlt;
    vector<double> vEtaBins;

    std::vector<float> pf_hcalDepth1;
    std::vector<float> pf_hcalDepth2;
    std::vector<float> pf_hcalDepth3;
    std::vector<float> pf_hcalDepth4;
    std::vector<float> pf_hcalDepth5;
    std::vector<float> pf_hcalDepth6;
    std::vector<float> pf_hcalDepth7;


    TString RootFileName_;
    string puFileName_;
    string era_, jet_type_, jet_type_puppi_;
    string jer_sf_file;// = std::string(std::getenv("CMSSW_BASE")) + "/src/NosiseTest/OffsetTreeMaker/data";
    int numSkip_;
    bool isMC_, writeCands_, writeParticles_, doL1L2L3Res_, isDebug_, usePuppiWsumCut_;
    ///
    JetVeto* jveto;
    string JetVetoFileName_;
    //string JetVetoHistName_;
    vector<string> JetVetoHistName_;
};

OffsetTreeMaker::OffsetTreeMaker(const edm::ParameterSet& iConfig)
	  : hltPrescaleProvider_(iConfig, consumesCollector(), *this)  //

{
  numSkip_ = iConfig.getParameter<int> ("numSkip");
  //RootFileName_ = iConfig.getParameter<string>("RootFileName");
  puFileName_ = iConfig.getParameter<string>("puFileName");
  isMC_ = iConfig.getParameter<bool>("isMC");
  writeCands_ = iConfig.getParameter<bool>("writeCands");
  writeParticles_ = iConfig.getParameter<bool>("writeParticles");
  pvTag_ = consumes< vector<reco::Vertex> >( iConfig.getParameter<edm::InputTag>("pvTag") );
  trackTag_ = consumes< vector<reco::Track> >( iConfig.getParameter<edm::InputTag>("trackTag") );
  muTag_ = consumes< vector<PileupSummaryInfo> >( iConfig.getParameter<edm::InputTag>("muTag") );
  //pfTag_ = consumes< vector<pat::PackedCandidate> >( iConfig.getParameter<edm::InputTag>("pfTag") );
  //genTag_ = consumes< vector<pat::PackedGenParticle> >( iConfig.getParameter<edm::InputTag>("genTag") );
  pfTag_           = consumes< reco::PFCandidateCollection >( iConfig.getParameter<edm::InputTag>("pfTag") );
  pfJetTag_        = consumes< reco::PFJetCollection >( iConfig.getParameter<edm::InputTag>("pfJetTag") );
  puppiWeightTag_ = consumes< edm::ValueMap<float> >( iConfig.getParameter<edm::InputTag>("puppiWeightTag") );
  genparticlesTag_ = consumes< vector<reco::GenParticle> >( iConfig.getParameter<edm::InputTag>("GenParticles") );
  generatorTag_ = consumes<GenEventInfoProduct>( iConfig.getParameter<edm::InputTag>("Generator") );
  rhoTag_ = consumes<double>( iConfig.getParameter<edm::InputTag>("rhoTag") );
  rhoC0Tag_ = consumes<double>( iConfig.getParameter<edm::InputTag>("rhoC0Tag") );
  rhoCCTag_ = consumes<double>( iConfig.getParameter<edm::InputTag>("rhoCCTag") );
  rhoCentralTag_ = consumes<double>( iConfig.getParameter<edm::InputTag>("rhoCentralTag") );
  rhoCentralCaloTag_ = consumes<double>( iConfig.getParameter<edm::InputTag>("rhoCentralCaloTag") );
  //pfJetTag_ = consumes< vector<pat::Jet> >( iConfig.getParameter<edm::InputTag>("pfJetTag") );
  pfJetTag_ = consumes<reco::PFJetCollection>( iConfig.getParameter<edm::InputTag>("pfJetTag") );
  pfJetPuppiTag_ = consumes<reco::PFJetCollection>(
      iConfig.existsAs<edm::InputTag>("pfJetPuppiTag") ? iConfig.getParameter<edm::InputTag>("pfJetPuppiTag")
                                                       : edm::InputTag("ak4PFJetsPuppi") );
  triggerList       = iConfig.getParameter<std::vector<std::string>>("trigList") ;
  triggerBits_      = consumes<edm::TriggerResults>(iConfig.getParameter<edm::InputTag>("bits"));
  hltProcessName_ = iConfig.getUntrackedParameter<std::string>("hltProcessName", "HLT");
  //triggerPrescales_ = consumes<pat::PackedTriggerPrescales>(iConfig.getParameter<edm::InputTag>("prescales"));
  genJetTag_ = consumes< vector<reco::GenJet> >( iConfig.getParameter<edm::InputTag>("genJetTag") );
  vEtaBins =  iConfig.getParameter<std::vector<double> >("etaBinTag");
  era_            = iConfig.getParameter<string>("era");
  jet_type_       = iConfig.getParameter<string>("jet_type");
  jet_type_puppi_ = iConfig.getParameter<string>("jet_type_puppi");
  doL1L2L3Res_     = iConfig.getParameter<bool>("doL1L2L3Res");
  doJetVeto_       = iConfig.getParameter<bool>("doJetVeto");
  isDebug_         = iConfig.getParameter<bool>("isDebug");
  usePuppiWsumCut_ = iConfig.getParameter<bool>("usePuppiWsumCut");
  jer_sf_file = std::string(std::getenv("CMSSW_BASE")) + "/src/NosiseTest/OffsetTreeMaker/data/";
  JetVetoFileName_ = iConfig.getParameter<string>("JetVetoFileName");
  //JetVetoHistName_ = iConfig.getParameter<string>("JetVetoHistName");
  JetVetoHistName_ = iConfig.getParameter<vector<string>>("JetVetoHistName");
   
  //jveto = new JetVeto((jer_sf_file+"jetveto/"+JetVetoFileName_),JetVetoHistName_);
  jveto = new JetVeto((jer_sf_file+"jetveto/"+JetVetoFileName_),JetVetoHistName_);

}

// ------------ method called once each job just before starting event loop  ------------
void  OffsetTreeMaker::beginJob() {

  /// Define output root file ///
  //root_file = new TFile(RootFileName_,"RECREATE");
  /// Define tree in output root file ///
  //tree = new TTree("T","Offset Tree");
  tree = root_file->make<TTree>("T", "Offset Tree");//("T","Offset Tree");

  counter = -1;
  /// Define histogram for pileup distribution ///
  //h = new TH1F("mu", "mu", 100, 0, 50);
  h = root_file->make<TH1F>("mu" , "mu" , 100 , 0 , 50);
  /// Not used variable in this code... ///
  rand = new TRandom3;
  //h2_GME = new TH2F("GME_2D", "GME_2D_yPhi_xEta", ETA_BINS_GME, -5.0, 5.0, PHI_BINS_GME , -1*M_PI , M_PI);
  h2_GME = root_file->make<TH2F>("GME_2D", "GME_2D_yPhi_xEta", ETA_BINS_GME, -5.0, 5.0, PHI_BINS_GME , -1*M_PI , M_PI);
  //h2_finnereta = new TH2F("finnereta_ET", "yPhi_xEta", ETA_BINS, etabins, PHI_BINS_GME, phibins);
  h2_finnereta = root_file->make<TH2F>("finnereta_ET", "yPhi_xEta", ETA_BINS, etabins, PHI_BINS_GME, phibins);
//  h2_GME_gen = new TH2F("GME_2D_gen", "GME_2D_yPhi_xEta", ETA_BINS_GME, -5.0, 5.0, PHI_BINS_GME , -1*M_PI , M_PI);
//  h2_finnereta_gen = new TH2F("finnereta_ET_gen", "yPhi_xEta", ETA_BINS, etabins, PHI_BINS_GME, phibins);
  //h2_twopi = new TH2F("GME_twopi", "GME_2D_yPhi_xEta", ETA_BINS_GME, -5.0, 5.0, PHI_BINS_GME , 0 , 2*M_PI);
  h2_twopi = root_file->make<TH2F>("GME_twopi", "GME_2D_yPhi_xEta", ETA_BINS_GME, -5.0, 5.0, PHI_BINS_GME , 0 , 2*M_PI);
  //h2_twopi_gen = new TH2F("GME_twopi_gen", "GME_2D_yPhi_xEta", ETA_BINS_GME, -5.0, 5.0, PHI_BINS_GME , 0 , 2*M_PI);
  h2_twopi_gen = root_file->make<TH2F>("GME_twopi_gen", "GME_2D_yPhi_xEta", ETA_BINS_GME, -5.0, 5.0, PHI_BINS_GME , 0 , 2*M_PI);
  //h2_twopi_det = new TH2F("GME_twopi_det", "GME_2D_yPhi_xEta", ETA_BINS_GME, -5.0, 5.0, PHI_BINS_GME , 0 , 2*M_PI);
  h2_twopi_det = root_file->make<TH2F>("GME_twopi_det", "GME_2D_yPhi_xEta", ETA_BINS_GME, -5.0, 5.0, PHI_BINS_GME , 0 , 2*M_PI);
  //h2_twopi_scaled = new TH2F("GME_twopi_scaled", "GME_2D_yPhi_xEta", ETA_BINS_GME, -5.0, 5.0, PHI_BINS_GME , 0 , 2*M_PI);
  h2_twopi_scaled = root_file->make<TH2F>("GME_twopi_scaled", "GME_2D_yPhi_xEta", ETA_BINS_GME, -5.0, 5.0, PHI_BINS_GME , 0 , 2*M_PI);
  if (!isMC_){
    //cout << "puFileName_ : " << puFileName_ << endl;
    parsePileUpJSON2( puFileName_ );/// Load pileup values from pileuptxt file ///

    tree->Branch("run", &run, "run/I");
    tree->Branch("lumi", &lumi, "lumi/I");
    tree->Branch("bx", &bx, "bx/I");
    tree->Branch("event", &event, "event/l");
  }

  if (writeCands_) {
    tree->Branch("pf_type", "std::vector<int>",   &pf_type);
    tree->Branch("pf_fromPV", "std::vector<int>",   &pf_fromPV);
    tree->Branch("pf_fromPVs", "std::vector<bool>",   &pf_fromPVs);
    tree->Branch("pf_vtxIdx", "std::vector<int>",   &pf_vtxIdx);
    tree->Branch("pf_dzPV0", "std::vector<float>", &pf_dzPV0);
    tree->Branch("pf_pt",   "std::vector<float>", &pf_pt);
    tree->Branch("pf_eta",  "std::vector<float>", &pf_eta);
    tree->Branch("pf_phi",  "std::vector<float>", &pf_phi);
    tree->Branch("pf_et",  "std::vector<float>", &pf_et);
    tree->Branch("pf_energy",  "std::vector<float>", &pf_energy);
    tree->Branch("pf_m",  "std::vector<float>", &pf_m);
    tree->Branch("pf_puppiW",  "std::vector<float>", &pf_puppiW);
    // --- Add HCAL Depth branches to the tree ---
    tree->Branch("pf_hcalDepth1", "std::vector<float>", &pf_hcalDepth1);
    tree->Branch("pf_hcalDepth2", "std::vector<float>", &pf_hcalDepth2);
    tree->Branch("pf_hcalDepth3", "std::vector<float>", &pf_hcalDepth3);
    tree->Branch("pf_hcalDepth4", "std::vector<float>", &pf_hcalDepth4);
    tree->Branch("pf_hcalDepth5", "std::vector<float>", &pf_hcalDepth5);
    tree->Branch("pf_hcalDepth6", "std::vector<float>", &pf_hcalDepth6);
    tree->Branch("pf_hcalDepth7", "std::vector<float>", &pf_hcalDepth7);
  }

  tree->Branch("mu", &mu, "mu/F");
  tree->Branch("muWeight", &muWeight, "muWeight/F");

  tree->Branch("rho",   &rho,   "rho/F");
  tree->Branch("rhoC0", &rhoC0, "rhoC0/F");
  tree->Branch("rhoCC", &rhoCC, "rhoCC/F");
  tree->Branch("rhoCentral", &rhoCentral, "rhoCentral/F");
  tree->Branch("rhoCentralCalo", &rhoCentralCalo, "rhoCentralCalo/F");

  tree->Branch("nPVall",  &nPVall, "nPVall/I");
  tree->Branch("nPV",     &nPV,    "nPV/I");
  tree->Branch("pv_ndof", pv_ndof, "pv_ndof[nPVall]/F");
  tree->Branch("pv_z",    pv_z,    "pv_z[nPVall]/F");
  tree->Branch("pv_rho",  pv_rho,  "pv_rho[nPVall]/F");

  tree->Branch("nEta",   &nEta,  "nEta/I");
  tree->Branch("energy", energy, "energy[nEta]/F");
  tree->Branch("et",     et,     "et[nEta]/F");
  tree->Branch("eRMS",   eRMS,   "eRMS[nEta]/F");

  tree->Branch("etMED",     etMED,     "etMED[nEta]/F");
  tree->Branch("etMEAN",    etMEAN,    "etMEAN[nEta]/F");

  tree->Branch("et_gme",       et_gme,     "et_gme[18][11]/F");
  tree->Branch("et_twopi",     et_twopi,     "et_twopi[18][11]/F");
  tree->Branch("rho_gme",   &rho_gme,  "rho_gme/F");
  if (isMC_) {
//    tree->Branch("et_gme_gen",   et_gme_gen, "et_gme_gen[18][11]/F");
    tree->Branch("et_twopi_gen", et_twopi_gen, "et_twopi_gen[18][11]/F");
    tree->Branch("rho_gme_gen", &rho_gme_gen, "rho_gme_gen/F");
    tree->Branch("et_twopi_det", et_twopi_det, "et_twopi_det[18][11]/F");
    tree->Branch("rho_gme_det", &rho_gme_det, "rho_gme_det/F");
    tree->Branch("et_twopi_scaled", et_twopi_scaled, "et_twopi_scaled[18][11]/F");
    tree->Branch("rho_gme_scaled", &rho_gme_scaled, "rho_gme_scaled/F");
//    tree->Branch("et_gen",     et_gen,     "et_gen[nEta]/F");
//    tree->Branch("etMED_gen",  etMED_gen,  "etMED_gen[nEta]/F");
//    tree->Branch("etMEAN_gen", etMEAN_gen, "etMEAN_gen[nEta]/F");
    tree->Branch("weight",        &weight,        "weight/F");
    tree->Branch("nGenParticles", &nGenParticles, "nGenParticles/I");
    tree->Branch("idHardScat1", &idHardScat1, "idHardScat1/I");
    tree->Branch("idHardScat2", &idHardScat2, "idHardScat2/I");
    tree->Branch("idxHardScat", &idxHardScat, "idxHardScat/I");
    if (writeParticles_) {
      tree->Branch("particle_id",   "std::vector<int>",   &particle_id);
      tree->Branch("particle_pt",   "std::vector<float>", &particle_pt);
      tree->Branch("particle_eta",  "std::vector<float>", &particle_eta);
      tree->Branch("particle_phi",  "std::vector<float>", &particle_phi);
      tree->Branch("particle_et",   "std::vector<float>", &particle_et);
      tree->Branch("particle_charge", "std::vector<int>", &particle_charge);
      tree->Branch("isPromptFinalState", "std::vector<bool>", &isPromptFinalState);
      tree->Branch("particle_et_scaled",   "std::vector<float>", &particle_et_scaled);
    }
  }

  tree->Branch("fchm",   f[chm],   "fchm[nEta]/b");
  tree->Branch("fchu",   f[chu],   "fchu[nEta]/b");
  tree->Branch("fnh",    f[nh],    "fnh[nEta]/b");
  tree->Branch("fne",    f[ne],    "fne[nEta]/b");
  tree->Branch("fhfh",   f[hfh],   "fhfh[nEta]/b");
  tree->Branch("fhfe",   f[hfe],   "fhfe[nEta]/b");
  tree->Branch("flep",   f[lep],   "flep[nEta]/b");
  tree->Branch("funtrk", f[untrk], "funtrk[nEta]/b");


  tree->Branch("ht", &ht, "ht/F");
  tree->Branch("nJets",    &nJets,   "nJets/I");

  tree->Branch("jet_eta",  "std::vector<float>", &v_jet_eta  );
  tree->Branch("jet_phi",  "std::vector<float>", &v_jet_phi  );
  tree->Branch("jet_pt",   "std::vector<float>", &v_jet_pt   );
  tree->Branch("jet_area", "std::vector<float>", &v_jet_area );

  tree->Branch("nJetsPuppi",    &nJetsPuppi, "nJetsPuppi/I");
  tree->Branch("jetpuppi_eta",  "std::vector<float>", &v_jetpuppi_eta  );
  tree->Branch("jetpuppi_phi",  "std::vector<float>", &v_jetpuppi_phi  );
  tree->Branch("jetpuppi_pt",   "std::vector<float>", &v_jetpuppi_pt   );
  tree->Branch("jetpuppi_area", "std::vector<float>", &v_jetpuppi_area );
  tree->Branch("jetpuppi_jec",  "std::vector<float>", &v_jetpuppi_jec  );

  tree->Branch("jet_ch","std::vector<float>",  &v_jet_ch);
  tree->Branch("jet_nh","std::vector<float>", &v_jet_nh);
  tree->Branch("jet_ne","std::vector<float>", &v_jet_ne);
  tree->Branch("jet_hfh","std::vector<float>", &v_jet_hfh);
  tree->Branch("jet_hfe","std::vector<float>", &v_jet_hfe);

  /// RC Jet variable ///
  tree->Branch("rc1jet_numpf",  "std::vector<int>", &v_rc1jet_numpf  );
  tree->Branch("rc1jet_eta",  "std::vector<float>", &v_rc1jet_eta  );
  tree->Branch("rc1jet_phi",  "std::vector<float>", &v_rc1jet_phi  );
  tree->Branch("rc1jet_pt",   "std::vector<float>", &v_rc1jet_pt   );
  tree->Branch("rc1jet_energy",   "std::vector<float>", &v_rc1jet_energy   );
  tree->Branch("rc1jet_cor",  "std::vector<float>", &v_rc1jet_cor  ); // L23
  tree->Branch("rc1jet_coruncUp",  "std::vector<float>", &v_rc1jet_coruncUp  ); // Unc  
  tree->Branch("rc1jet_coruncDown",  "std::vector<float>", &v_rc1jet_coruncDown  ); // Unc  
  tree->Branch("rc1jet_corL1",  "std::vector<float>", &v_rc1jet_corL1  ); 
  tree->Branch("rc1jet_corL1RC",  "std::vector<float>", &v_rc1jet_corL1RC  ); 
  tree->Branch("rc1jet_corL1Nega",  "std::vector<float>", &v_rc1jet_corL1Nega  ); 
  tree->Branch("rc1jet_corL1RCNega",  "std::vector<float>", &v_rc1jet_corL1RCNega  ); 
  tree->Branch("rc1jet_corAll",  "std::vector<float>", &v_rc1jet_corAll  );
  tree->Branch("rc1jet_corAllRC",  "std::vector<float>", &v_rc1jet_corAllRC  );
  tree->Branch("rc1jet_corAllNega",  "std::vector<float>", &v_rc1jet_corAllNega  );
  tree->Branch("rc1jet_corAllRCNega",  "std::vector<float>", &v_rc1jet_corAllRCNega  );

  tree->Branch("rc2jet_numpf",  "std::vector<int>", &v_rc2jet_numpf  );
  tree->Branch("rc2jet_eta",  "std::vector<float>", &v_rc2jet_eta  );
  tree->Branch("rc2jet_phi",  "std::vector<float>", &v_rc2jet_phi  );
  tree->Branch("rc2jet_pt",   "std::vector<float>", &v_rc2jet_pt   );
  tree->Branch("rc2jet_energy",   "std::vector<float>", &v_rc2jet_energy   );
  tree->Branch("rc2jet_cor",  "std::vector<float>", &v_rc2jet_cor  );
  tree->Branch("rc2jet_coruncUp",  "std::vector<float>", &v_rc2jet_coruncUp  );
  tree->Branch("rc2jet_coruncDown",  "std::vector<float>", &v_rc2jet_coruncDown  );
  tree->Branch("rc2jet_corL1",  "std::vector<float>", &v_rc2jet_corL1  );
  tree->Branch("rc2jet_corL1RC",  "std::vector<float>", &v_rc2jet_corL1RC  );
  tree->Branch("rc2jet_corL1Nega",  "std::vector<float>", &v_rc2jet_corL1Nega  );
  tree->Branch("rc2jet_corL1RCNega",  "std::vector<float>", &v_rc2jet_corL1RCNega  );
  tree->Branch("rc2jet_corAll",  "std::vector<float>", &v_rc2jet_corAll  );
  tree->Branch("rc2jet_corAllRC",  "std::vector<float>", &v_rc2jet_corAllRC  );
  tree->Branch("rc2jet_corAllNega",  "std::vector<float>", &v_rc2jet_corAllNega  );
  tree->Branch("rc2jet_corAllRCNega",  "std::vector<float>", &v_rc2jet_corAllRCNega  );

  tree->Branch("rc_diff",   "std::vector<float>", &v_rc_diff  );
  tree->Branch("rc_ispair", "std::vector<bool>",  &v_isRC     );

  // --- PUPPI RC: cone direction ---
  tree->Branch("rc_ispair_puppi",         "std::vector<bool>",  &v_isRC_puppi            );
  tree->Branch("rc1jet_puppi_numpf",      "std::vector<int>",   &v_rc1jet_puppi_numpf    );
  tree->Branch("rc2jet_puppi_numpf",      "std::vector<int>",   &v_rc2jet_puppi_numpf    );
  tree->Branch("rc1jet_puppi_eta",        "std::vector<float>", &v_rc1jet_puppi_eta      );
  tree->Branch("rc2jet_puppi_eta",        "std::vector<float>", &v_rc2jet_puppi_eta      );
  tree->Branch("rc1jet_puppi_phi",        "std::vector<float>", &v_rc1jet_puppi_phi      );
  tree->Branch("rc2jet_puppi_phi",        "std::vector<float>", &v_rc2jet_puppi_phi      );
  tree->Branch("rc1jet_puppi_pt",         "std::vector<float>", &v_rc1jet_puppi_pt       );
  tree->Branch("rc2jet_puppi_pt",         "std::vector<float>", &v_rc2jet_puppi_pt       );
  tree->Branch("rc1jet_puppi_energy",     "std::vector<float>", &v_rc1jet_puppi_energy   );
  tree->Branch("rc2jet_puppi_energy",     "std::vector<float>", &v_rc2jet_puppi_energy   );

  // --- PUPPI RC: JEC (same 9 variants as PFCHS) ---
  tree->Branch("rc1jet_puppi_cor",          "std::vector<float>", &v_rc1jet_puppi_cor         );
  tree->Branch("rc2jet_puppi_cor",          "std::vector<float>", &v_rc2jet_puppi_cor         );
  tree->Branch("rc1jet_puppi_coruncUp",     "std::vector<float>", &v_rc1jet_puppi_coruncUp    );
  tree->Branch("rc2jet_puppi_coruncUp",     "std::vector<float>", &v_rc2jet_puppi_coruncUp    );
  tree->Branch("rc1jet_puppi_coruncDown",   "std::vector<float>", &v_rc1jet_puppi_coruncDown  );
  tree->Branch("rc2jet_puppi_coruncDown",   "std::vector<float>", &v_rc2jet_puppi_coruncDown  );
  tree->Branch("rc1jet_puppi_corL1",        "std::vector<float>", &v_rc1jet_puppi_corL1       );
  tree->Branch("rc2jet_puppi_corL1",        "std::vector<float>", &v_rc2jet_puppi_corL1       );
  tree->Branch("rc1jet_puppi_corL1RC",      "std::vector<float>", &v_rc1jet_puppi_corL1RC     );
  tree->Branch("rc2jet_puppi_corL1RC",      "std::vector<float>", &v_rc2jet_puppi_corL1RC     );
  tree->Branch("rc1jet_puppi_corL1Nega",    "std::vector<float>", &v_rc1jet_puppi_corL1Nega   );
  tree->Branch("rc2jet_puppi_corL1Nega",    "std::vector<float>", &v_rc2jet_puppi_corL1Nega   );
  tree->Branch("rc1jet_puppi_corL1RCNega",  "std::vector<float>", &v_rc1jet_puppi_corL1RCNega );
  tree->Branch("rc2jet_puppi_corL1RCNega",  "std::vector<float>", &v_rc2jet_puppi_corL1RCNega );
  tree->Branch("rc1jet_puppi_corAll",       "std::vector<float>", &v_rc1jet_puppi_corAll      );
  tree->Branch("rc2jet_puppi_corAll",       "std::vector<float>", &v_rc2jet_puppi_corAll      );
  tree->Branch("rc1jet_puppi_corAllRC",     "std::vector<float>", &v_rc1jet_puppi_corAllRC    );
  tree->Branch("rc2jet_puppi_corAllRC",     "std::vector<float>", &v_rc2jet_puppi_corAllRC    );
  tree->Branch("rc1jet_puppi_corAllNega",   "std::vector<float>", &v_rc1jet_puppi_corAllNega  );
  tree->Branch("rc2jet_puppi_corAllNega",   "std::vector<float>", &v_rc2jet_puppi_corAllNega  );
  tree->Branch("rc1jet_puppi_corAllRCNega", "std::vector<float>", &v_rc1jet_puppi_corAllRCNega);
  tree->Branch("rc2jet_puppi_corAllRCNega", "std::vector<float>", &v_rc2jet_puppi_corAllRCNega);

  // --- Stage 1: PUPPI ET sum ---
  tree->Branch("rc1jet_puppiET", "std::vector<float>", &v_rc1jet_puppiET);
  tree->Branch("rc2jet_puppiET", "std::vector<float>", &v_rc2jet_puppiET);
  tree->Branch("rc_puppi_diff",  "std::vector<float>", &v_rc_puppi_diff );

  // --- Stage 1: PUPPI weight sum ---
  tree->Branch("rc1jet_puppiWsum", "std::vector<float>", &v_rc1jet_puppiWsum);
  tree->Branch("rc2jet_puppiWsum", "std::vector<float>", &v_rc2jet_puppiWsum);

  // --- Stage 2: per-particle-type raw ET ---
  tree->Branch("rc1jet_et_nh",  "std::vector<float>", &v_rc1jet_et_nh );
  tree->Branch("rc2jet_et_nh",  "std::vector<float>", &v_rc2jet_et_nh );
  tree->Branch("rc1jet_et_ne",  "std::vector<float>", &v_rc1jet_et_ne );
  tree->Branch("rc2jet_et_ne",  "std::vector<float>", &v_rc2jet_et_ne );
  tree->Branch("rc1jet_et_chm", "std::vector<float>", &v_rc1jet_et_chm);
  tree->Branch("rc2jet_et_chm", "std::vector<float>", &v_rc2jet_et_chm);
  tree->Branch("rc1jet_et_chu", "std::vector<float>", &v_rc1jet_et_chu);
  tree->Branch("rc2jet_et_chu", "std::vector<float>", &v_rc2jet_et_chu);
  tree->Branch("rc1jet_et_hfh", "std::vector<float>", &v_rc1jet_et_hfh);
  tree->Branch("rc2jet_et_hfh", "std::vector<float>", &v_rc2jet_et_hfh);
  tree->Branch("rc1jet_et_hfe", "std::vector<float>", &v_rc1jet_et_hfe);
  tree->Branch("rc2jet_et_hfe", "std::vector<float>", &v_rc2jet_et_hfe);

  // --- Stage 2: per-particle-type PUPPI-weighted ET ---
  tree->Branch("rc1jet_puppiET_nh",  "std::vector<float>", &v_rc1jet_puppiET_nh );
  tree->Branch("rc2jet_puppiET_nh",  "std::vector<float>", &v_rc2jet_puppiET_nh );
  tree->Branch("rc1jet_puppiET_ne",  "std::vector<float>", &v_rc1jet_puppiET_ne );
  tree->Branch("rc2jet_puppiET_ne",  "std::vector<float>", &v_rc2jet_puppiET_ne );
  tree->Branch("rc1jet_puppiET_chm", "std::vector<float>", &v_rc1jet_puppiET_chm);
  tree->Branch("rc2jet_puppiET_chm", "std::vector<float>", &v_rc2jet_puppiET_chm);
  tree->Branch("rc1jet_puppiET_hfh", "std::vector<float>", &v_rc1jet_puppiET_hfh);
  tree->Branch("rc2jet_puppiET_hfh", "std::vector<float>", &v_rc2jet_puppiET_hfh);
  tree->Branch("rc1jet_puppiET_hfe", "std::vector<float>", &v_rc1jet_puppiET_hfe);
  tree->Branch("rc2jet_puppiET_hfe", "std::vector<float>", &v_rc2jet_puppiET_hfe);

  // --- Stage 4: HCAL depth ET sum per cone ---
  tree->Branch("rc1jet_hcalDepth1", "std::vector<float>", &v_rc1jet_hcalDepth1);
  tree->Branch("rc2jet_hcalDepth1", "std::vector<float>", &v_rc2jet_hcalDepth1);
  tree->Branch("rc1jet_hcalDepth2", "std::vector<float>", &v_rc1jet_hcalDepth2);
  tree->Branch("rc2jet_hcalDepth2", "std::vector<float>", &v_rc2jet_hcalDepth2);
  tree->Branch("rc1jet_hcalDepth3", "std::vector<float>", &v_rc1jet_hcalDepth3);
  tree->Branch("rc2jet_hcalDepth3", "std::vector<float>", &v_rc2jet_hcalDepth3);
  tree->Branch("rc1jet_hcalDepth4", "std::vector<float>", &v_rc1jet_hcalDepth4);
  tree->Branch("rc2jet_hcalDepth4", "std::vector<float>", &v_rc2jet_hcalDepth4);
  tree->Branch("rc1jet_hcalDepth5", "std::vector<float>", &v_rc1jet_hcalDepth5);
  tree->Branch("rc2jet_hcalDepth5", "std::vector<float>", &v_rc2jet_hcalDepth5);
  tree->Branch("rc1jet_hcalDepth6", "std::vector<float>", &v_rc1jet_hcalDepth6);
  tree->Branch("rc2jet_hcalDepth6", "std::vector<float>", &v_rc2jet_hcalDepth6);
  tree->Branch("rc1jet_hcalDepth7", "std::vector<float>", &v_rc1jet_hcalDepth7);
  tree->Branch("rc2jet_hcalDepth7", "std::vector<float>", &v_rc2jet_hcalDepth7);

/*  tree->Branch("jet_eta",  jet_eta,  "jet_eta[nJets]/F");
  tree->Branch("jet_phi",  jet_phi,  "jet_phi[nJets]/F");
  tree->Branch("jet_pt",   jet_pt,   "jet_pt[nJets]/F");
  tree->Branch("jet_area", jet_area, "jet_area[nJets]/F");

  tree->Branch("jet_ch",  jet_ch,  "jet_ch[nJets]/F");
  tree->Branch("jet_nh",  jet_nh,  "jet_nh[nJets]/F");
  tree->Branch("jet_ne",  jet_ne,  "jet_ne[nJets]/F");
  tree->Branch("jet_hfh", jet_hfh, "jet_hfh[nJets]/F");
  tree->Branch("jet_hfe", jet_hfe, "jet_hfe[nJets]/F");
  tree->Branch("jet_lep", jet_lep, "jet_lep[nJets]/F");
*/
  ///Trigger ///
  tree->Branch("trigger_pass", "std::vector<bool>",   &trigger_pass);
  tree->Branch("trigger_error", "std::vector<bool>",   &trigger_error);
  tree->Branch("trigger_run", "std::vector<bool>",   &trigger_run);
  //tree->Branch("trigger_prescale", "std::vector<unsigned int>",   &trigger_prescale);
  tree->Branch("trigger_prescale", "std::vector<double>",   &trigger_prescale);
  tree->Branch("trigger_name", "std::vector<string>",   &trigger_name);

  /// Gen. Jet ///
  tree->Branch("genht", &genht, "genht/F");
  tree->Branch("nGenJets",    &nGenJets,   "nGenJets/I");
  tree->Branch("genjet_eta",  "std::vector<float>", &v_genjet_eta );
  tree->Branch("genjet_phi",  "std::vector<float>", &v_genjet_phi );
  tree->Branch("genjet_pt",   "std::vector<float>", &v_genjet_pt );
  tree->Branch("genjet_area", "std::vector<float>", &v_genjet_area );
}

// --- Ensure this function is implemented exactly like this ---
void OffsetTreeMaker::beginRun(const edm::Run& iRun, const edm::EventSetup& iSetup) {
  bool changed = true;
  
  // 1. Initialize HLTConfigProvider (for trigger names/indices)
  if (!hltConfig_.init(iRun, iSetup, hltProcessName_, changed)) {
    throw cms::Exception("HLTConfigProvider")
      << "Failed to initialize HLTConfigProvider with process name: " << hltProcessName_;
  }

  // 2. Initialize HLTPrescaleProvider (CRUCIAL for prescale calculations)
  // This step was missing or failed in your previous run
  if (!hltPrescaleProvider_.init(iRun, iSetup, hltProcessName_, changed)) {
    throw cms::Exception("HLTPrescaleProvider")
      << "Failed to initialize HLTPrescaleProvider with process name: " << hltProcessName_;
  }
}

// ------------ method called for each event  ------------
void OffsetTreeMaker::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup) {

  /// Debug part ///
  ++counter;
  if (counter%numSkip_ != 0) return;
  //cout << "counter : " << counter << endl;
  //cout << "iEvent.id().event() : "  << iEvent.id().event() << endl;
  if (counter==0) {rand->SetSeed(iEvent.id().event()*8728);}
//------------ Pileup ------------//

  if (isMC_){
    edm::Handle< vector<PileupSummaryInfo> > pileups;
    iEvent.getByToken(muTag_, pileups);

    // Safety check: require at least 2 pileup entries (BX=-1, 0, +1)
    if (!pileups.isValid() || pileups->size() < 2) {
      mu = 0;
    } else {
      mu = pileups->at(1).getTrueNumInteractions();
    }

    // Store per-BX pileup info (from v2)
    int j = 0;
    std::vector<PileupSummaryInfo>::const_iterator pileupinfo;
    for (pileupinfo = pileups->begin(); pileupinfo != pileups->end(); ++pileupinfo) {
      if (j < 3) mua[j] = pileupinfo->getPU_NumInteractions();
      // Store PU vertex z-positions for in-time BX only
      if (pileupinfo->getBunchCrossing() == 0) {
        for (int i = 0; i != pileupinfo->getPU_NumInteractions() && i < 50; ++i) {
          puz[i] = (pileupinfo->getPU_zpositions())[i];
        }
      }
      ++j;
    }
  }
  else {
    run   = int(iEvent.id().run());
    lumi  = int(iEvent.getLuminosityBlock().luminosityBlock());
    bx    = iEvent.bunchCrossing();
    event = iEvent.id().event();

    mu = getAvgPU(run, lumi);

    // Run2025: no per-run reweighting needed
    // (Run2017 reweight tables removed)
    muWeight = 1;

    // Skip events where pileup info is unavailable
    // (run/lumi not found in pileup txt file)
    if (mu == 0) {
      if (isDebug_) {
        std::cerr << "[PU] run=" << run << " lumi=" << lumi
                  << " not found in pileup file, skipping event" << std::endl;
      }
      return;
    }
  }
//------------ Primary Vertices ------------//

  edm::Handle< vector<reco::Vertex> > primaryVertices;
  iEvent.getByToken(pvTag_, primaryVertices);

  nPVall = primaryVertices->size();
  nPV = 0;

  for (int i = 0; i != nPVall; ++i){
    reco::Vertex pv = primaryVertices->at(i);

    if( !pv.isFake() && pv.ndof() > 4 && pv.z() <= 24 && pv.position().rho() <= 2 ) {++nPV; }

    if(i >= MAXNPV) continue;
    pv_ndof[i] = pv.ndof();
    pv_z[i] = pv.z();
    pv_rho[i] = pv.position().rho();

  }
  if(nPVall > MAXNPV) nPVall = MAXNPV;

//------------ Rho ------------//

  edm::Handle<double> rhoHandle;
  iEvent.getByToken(rhoTag_, rhoHandle);
  rho = *rhoHandle;

  edm::Handle<double> rhoC0Handle;
  iEvent.getByToken(rhoC0Tag_, rhoC0Handle);
  rhoC0 = *rhoC0Handle;

  edm::Handle<double> rhoCCHandle;
  iEvent.getByToken(rhoCCTag_, rhoCCHandle);
  rhoCC = *rhoCCHandle;

  edm::Handle<double> rhoCentralHandle;
  iEvent.getByToken(rhoCentralTag_, rhoCentralHandle);
  rhoCentral = *rhoCentralHandle;

  edm::Handle<double> rhoCentralCaloHandle;
  iEvent.getByToken(rhoCentralCaloTag_, rhoCentralCaloHandle);
  rhoCentralCalo = *rhoCentralCaloHandle;

//------------ Gen Particles -----------//
// --- Modified GenParticle Section for AOD ---
  if (isMC_) {
    edm::Handle<GenEventInfoProduct> genInfo;
    iEvent.getByToken(generatorTag_, genInfo);
    weight = genInfo->weight();

    edm::Handle<reco::GenParticleCollection> pruned; // Changed to reco::GenParticleCollection
    iEvent.getByToken(genparticlesTag_, pruned);

    // Note: In AOD, we usually use one main GenParticle collection
    // Instead of pat::PackedGenParticle, we use reco::GenParticle
    nGenParticles = pruned->size();
    if( pruned->size() == 0 ) return;

    memset(et_twopi_gen,  0, sizeof(et_twopi_gen));
    memset(et_twopi_det,  0, sizeof(et_twopi_det));
    memset(et_twopi_scaled,  0, sizeof(et_twopi_scaled));

    h2_twopi_gen->Reset(); h2_twopi_det->Reset(); h2_twopi_scaled->Reset();

    particle_id.clear(); particle_et.clear(); particle_pt.clear(); 
    particle_eta.clear(); particle_phi.clear(); particle_charge.clear();
    isPromptFinalState.clear(); particle_et_scaled.clear();

    int id_hard1 = 0, id_hard2 = 0, n_hard = 0;
    for (size_t i = 0; i < pruned->size(); ++i ) {
      const reco::GenParticle & p = (*pruned)[i];
      if (p.status() == 21) { // Incoming particles of the hardest subprocess
        if (n_hard==0) id_hard1 = p.pdgId();
        else if (n_hard==1) id_hard2 = p.pdgId();
        n_hard++;
      }
    }
    
    int idx_hard = -1;
    if (id_hard1 == 21 && id_hard2 == 21) idx_hard = 2; // gg
    else if (id_hard1 == 21 || id_hard2 == 21) idx_hard = 1; // gq
    else if (id_hard1 != 0 && id_hard2 != 0 && fabs(id_hard1) < 6 && fabs(id_hard2) < 6) idx_hard = 0; // qq

    int nGenPar = 0;
    for (reco::GenParticleCollection::const_iterator i_particle = pruned->begin(); i_particle != pruned->end(); ++i_particle) {
      
      // Only consider final state particles (status == 1) for detector simulation studies
      if (i_particle->status() != 1) continue;

      int pdgIdAbs = abs(i_particle->pdgId());
      float newPhi = (i_particle->phi()>=0) ? i_particle->phi() : i_particle->phi() + 2*M_PI;
      
      // Neutrinos are excluded from energy deposits
      if (pdgIdAbs != 12 && pdgIdAbs != 14 && pdgIdAbs != 16) {
        h2_twopi_gen->Fill(i_particle->eta(), newPhi, i_particle->et());
      }

      int charge = i_particle->charge();
      float et = i_particle->et(); 
      float et_scaled = et; 

      // Neutral Hadron Response Scaling Logic (as in original code)
      if (charge == 0 && pdgIdAbs != 22 && pdgIdAbs != 12 && pdgIdAbs != 14 && pdgIdAbs != 16) {
        float et_nhHHe = 0.45 * et; 
        float et_nhHCAL = 0.55 * 0.5 * et; 
        float et_nhECAL = 0.55 * 0.5 * et;
        float et_nhECAL_scaled = et_nhECAL * 1.076 * (1 - 1.403 * pow(et_nhECAL, 0.646 - 1));
        et_scaled = et_nhHHe + et_nhHCAL + et_nhECAL_scaled;
      }

      // Selection cuts for filling scaled histograms
      if (((pdgIdAbs == 11 || pdgIdAbs == 13 || pdgIdAbs == 15 || pdgIdAbs == 22) && et > 0.3) || 
          ((pdgIdAbs < 11 || pdgIdAbs > 16) && ((charge!=0 && et > 0.4) || (charge==0 && pdgIdAbs != 22 && et_scaled > 3)))) {
          h2_twopi_scaled->Fill(i_particle->eta(), newPhi, et_scaled);
      }

      // Skip low energy particles for storage
      if (((pdgIdAbs == 11 || pdgIdAbs == 13 || pdgIdAbs == 15 || pdgIdAbs == 22) && et < 0.3) || 
          ((pdgIdAbs < 11 || pdgIdAbs > 16) && ((charge!=0 && et < 0.4) || (charge==0 && pdgIdAbs != 22 && et < 3)))) continue;
      
      ++nGenPar;

      if (writeParticles_) {
        particle_id.push_back( i_particle->pdgId() );
        particle_et.push_back( i_particle->et() );
        particle_pt.push_back( i_particle->pt() );
        particle_eta.push_back( i_particle->eta() );
        particle_phi.push_back( i_particle->phi() );
        particle_charge.push_back( i_particle->charge() );
        isPromptFinalState.push_back( i_particle->isPromptFinalState() );
        particle_et_scaled.push_back( et_scaled );
      }

      if (pdgIdAbs != 12 && pdgIdAbs != 14 && pdgIdAbs != 16) h2_twopi_det->Fill(i_particle->eta(), newPhi, et);
    }
    nGenParticles = nGenPar;
    idHardScat1 = id_hard1; idHardScat2 = id_hard2; idxHardScat = idx_hard;
  }


  
  //------------ PF Particles ------------//
  // --- Modified PF Candidate Section for AOD ---
  edm::Handle<reco::PFCandidateCollection> pfCandidates;
  iEvent.getByToken(pfTag_, pfCandidates);

  // Retrieve Puppi weights from ValueMap in AOD
  edm::Handle<edm::ValueMap<float>> puppiWeights;
  if (writeCands_) {
    iEvent.getByToken(puppiWeightTag_, puppiWeights);
  }

  memset(energy, 0, sizeof(energy));
  memset(eRMS,   0, sizeof(eRMS));
  memset(et,     0, sizeof(et));
  memset(etMED,  0, sizeof(etMED));
  memset(etMEAN, 0, sizeof(etMEAN));

  nEta = ETA_BINS;

  float eFlavor[numFlavors][ETA_BINS] = {};
  float e2[ETA_BINS] = {};
  int nPart[ETA_BINS] = {};

  memset(et_gme,   0, sizeof(et_gme));
  memset(et_twopi, 0, sizeof(et_twopi));

  pf_type.clear(); pf_fromPV.clear(); pf_fromPVs.clear();
  pf_vtxIdx.clear(); pf_dzPV0.clear();
  pf_pt.clear(); pf_eta.clear(); pf_phi.clear();
  pf_et.clear(); pf_energy.clear(); pf_m.clear(); pf_puppiW.clear();
  
  // HCAL Depth Info Vectors (Stage 4)
  pf_hcalDepth1.clear(); pf_hcalDepth2.clear(); pf_hcalDepth3.clear();
  pf_hcalDepth4.clear(); pf_hcalDepth5.clear(); pf_hcalDepth6.clear();
  pf_hcalDepth7.clear();

  h2_GME->Reset();
  h2_finnereta->Reset();
  h2_twopi->Reset();

  // Get Primary Vertex for Neutral Vertex Association
  reco::VertexRef primaryVtx = primaryVertices->empty() || primaryVertices->begin()->isFake() ? 
                               reco::VertexRef() : reco::VertexRef(primaryVertices, 0);

  for (reco::PFCandidateCollection::const_iterator i_pf = pfCandidates->begin(); i_pf != pfCandidates->end(); ++i_pf) {
    int etaIndex = getEtaIndex( i_pf->eta() );
    Flavor flavor = getFlavor( fabs(i_pf->pdgId()) );

    if (etaIndex == -1 || flavor == X) continue;

    // --- Enhanced Vertex Association for AOD ---
    int fromPV_val = 0; 
    bool attached = false;
    int vtxIdx_val = -1;
    float dzPV0_val = -999.;
    
    if (i_pf->charge() != 0) {
      // For Charged Particles: Traditional Track-to-Vertex matching
      reco::TrackRef pftrack = i_pf->trackRef();
      if (pftrack.isNonnull() && !primaryVertices->empty())
        dzPV0_val = pftrack->dz( primaryVertices->at(0).position() );
      
      vector<reco::Vertex>::const_iterator i_pv, endpv = primaryVertices->end();
      for (i_pv = primaryVertices->begin(); i_pv != endpv && !attached; ++i_pv) {
        if (!i_pv->isFake() && i_pv->ndof() >= 4 && fabs(i_pv->z()) < 24) {
          for (auto i_vtxTrk = i_pv->tracks_begin(); i_vtxTrk != i_pv->tracks_end() && !attached; ++i_vtxTrk) {
            reco::TrackRef vtxTrk(i_vtxTrk->castTo<reco::TrackRef>());
            if (vtxTrk == pftrack) {
              attached = true;
              vtxIdx_val = static_cast<int>( std::distance(primaryVertices->begin(), i_pv) );
            }
          }
        }
      }
      if (attached) {
        fromPV_val = 3; 
      } else {
        flavor = chu; // Unmatched charged hadron (Pileup)
        fromPV_val = 0;
      }
    } else {
      // For Neutral Particles (Neutral Hadron, Photon):
      // classify based on proximity to the leading PV z-position in AOD
      if (primaryVtx.isNonnull()) {
        float dz = std::abs(i_pf->vz() - primaryVtx->z());
        if (dz < 0.2) fromPV_val = 2; // Association for neutrals
        else fromPV_val = 0;
      }
    }

    float e = i_pf->energy();
    energy[etaIndex] += e;
    et[etaIndex] += i_pf->et();
    eFlavor[flavor][etaIndex] += e;

    h2_GME->Fill(i_pf->eta(), i_pf->phi(), i_pf->et());
    float newPhi = (i_pf->phi() >= 0) ? i_pf->phi() : i_pf->phi() + 2*M_PI;
    h2_twopi->Fill(i_pf->eta(), newPhi, i_pf->et());
    h2_finnereta->Fill(i_pf->eta(), i_pf->phi(), i_pf->et());

    e2[etaIndex] += (e*e);
    nPart[etaIndex]++;

    if (writeCands_) {
      reco::PFCandidateRef pfRef(pfCandidates, distance(pfCandidates->begin(), i_pf));

      pf_type.push_back( static_cast<int>(flavor) );
      pf_fromPV.push_back( fromPV_val );
      pf_vtxIdx.push_back( vtxIdx_val );
      pf_dzPV0.push_back( dzPV0_val );
      pf_pt.push_back( i_pf->pt() );
      pf_eta.push_back( i_pf->eta() );
      pf_phi.push_back( i_pf->phi() );
      pf_et.push_back( i_pf->et() );
      pf_energy.push_back( i_pf->energy() );
      pf_m.push_back( i_pf->mass() );

      float pW = (*puppiWeights)[pfRef];
      pf_puppiW.push_back( pW );

      // Manual check for HCAL energy and scaling (Stage 4)
      float hcal_e = i_pf->hcalEnergy();
      
      pf_hcalDepth1.push_back( i_pf->hcalDepthEnergyFraction(1) * hcal_e );
      pf_hcalDepth2.push_back( i_pf->hcalDepthEnergyFraction(2) * hcal_e );
      pf_hcalDepth3.push_back( i_pf->hcalDepthEnergyFraction(3) * hcal_e );
      pf_hcalDepth4.push_back( i_pf->hcalDepthEnergyFraction(4) * hcal_e );
      pf_hcalDepth5.push_back( i_pf->hcalDepthEnergyFraction(5) * hcal_e );
      pf_hcalDepth6.push_back( i_pf->hcalDepthEnergyFraction(6) * hcal_e );
      pf_hcalDepth7.push_back( i_pf->hcalDepthEnergyFraction(7) * hcal_e );

      // Debugging Output
      /*std::cout << "i_pf->energy(): " << e << "  hcal_e: " << hcal_e << std::endl;
      std::cout << "i_pf->hcalDepthEnergyFraction(1) * hcal_e: " << i_pf->hcalDepthEnergyFraction(1) * hcal_e << std::endl;
      std::cout << "i_pf->hcalDepthEnergyFraction(2) * hcal_e: " << i_pf->hcalDepthEnergyFraction(2) * hcal_e << std::endl;
      std::cout << "i_pf->hcalDepthEnergyFraction(3) * hcal_e: " << i_pf->hcalDepthEnergyFraction(3) * hcal_e << std::endl;
      std::cout << "i_pf->hcalDepthEnergyFraction(4) * hcal_e: " << i_pf->hcalDepthEnergyFraction(4) * hcal_e << std::endl;
      std::cout << "i_pf->hcalDepthEnergyFraction(5) * hcal_e: " << i_pf->hcalDepthEnergyFraction(5) * hcal_e << std::endl;
      std::cout << "i_pf->hcalDepthEnergyFraction(6) * hcal_e: " << i_pf->hcalDepthEnergyFraction(6) * hcal_e << std::endl;
      std::cout << "i_pf->hcalDepthEnergyFraction(7) * hcal_e: " << i_pf->hcalDepthEnergyFraction(7) * hcal_e << std::endl;
      std::cout << "PFCandidate weight: " << pW << std::endl;*/
    }
  }
  /// what is xg ??
  vector<double> xg(ETA_BINS_GME*PHI_BINS_GME); vector<double> xg_gen(ETA_BINS_GME*PHI_BINS_GME);
  vector<double> xg_det(ETA_BINS_GME*PHI_BINS_GME); vector<double> xg_scaled(ETA_BINS_GME*PHI_BINS_GME);
  for (int ieta = 1; ieta != (ETA_BINS_GME+1); ++ieta){
    for (int iphi = 1; iphi != (PHI_BINS_GME+1); ++iphi){
      int igrid = PHI_BINS_GME*(ieta-1)+(iphi-1);
      et_gme[ieta-1][iphi-1] =  h2_GME->GetBinContent(ieta, iphi);
      et_twopi[ieta-1][iphi-1] =  h2_twopi->GetBinContent(ieta, iphi);
      xg[igrid] = et_twopi[ieta-1][iphi-1]/GRID_AREA;
      if (isMC_) {
//        et_gme_gen[ieta-1][iphi-1] =  h2_GME_gen->GetBinContent(ieta, iphi);
        et_twopi_gen[ieta-1][iphi-1] =  h2_twopi_gen->GetBinContent(ieta, iphi);
        xg_gen[igrid] = et_twopi_gen[ieta-1][iphi-1]/GRID_AREA;
        et_twopi_det[ieta-1][iphi-1] =  h2_twopi_det->GetBinContent(ieta, iphi);
        xg_det[igrid] = et_twopi_det[ieta-1][iphi-1]/GRID_AREA;
        et_twopi_scaled[ieta-1][iphi-1] =  h2_twopi_scaled->GetBinContent(ieta, iphi);
        xg_scaled[igrid] = et_twopi_scaled[ieta-1][iphi-1]/GRID_AREA;
      }
    }
  }
  sort(xg.begin(),xg.end()); rho_gme = 0.5*(xg[98]+xg[99]); // out of 11*18=198 entries
  if (isMC_) {
    sort(xg_gen.begin(),xg_gen.end()); rho_gme_gen = 0.5*(xg_gen[98]+xg_gen[99]);
    sort(xg_det.begin(),xg_det.end()); rho_gme_det = 0.5*(xg_det[98]+xg_det[99]);
    sort(xg_scaled.begin(),xg_scaled.end()); rho_gme_scaled = 0.5*(xg_scaled[98]+xg_scaled[99]);
  }

  for (int ieta = 1; ieta != (ETA_BINS+1); ++ieta){
    vector<double> x;
    double et_sum = 0;
    for (int iphi = 1; iphi != PHI_BINS_GME+1; ++iphi){
      x.push_back(h2_finnereta->GetBinContent(ieta, iphi));
      et_sum += h2_finnereta->GetBinContent(ieta, iphi);
    }
    sort(x.begin(),x.end()); 
    float median = x[5]; // for 11 phi bins
    etMED[ieta-1] = median;
    etMEAN[ieta-1] = float(et_sum)/(11);
  }


//------------ Tracks ------------//

  edm::Handle< vector<reco::Track> > tracks;
  iEvent.getByToken(trackTag_, tracks);

  vector<reco::Track>::const_iterator i_trk, endtrk = tracks->end();
  for (i_trk = tracks->begin(); i_trk != endtrk; ++i_trk) {

    if ( !i_trk->quality(reco::Track::tight) ) continue;
    bool matched = false;

    reco::PFCandidateCollection::const_iterator i_pf, endpf = pfCandidates->end();
    for (i_pf = pfCandidates->begin();  i_pf != endpf && !matched; ++i_pf) {

      if ( &(*i_trk) == i_pf->trackRef().get() )
        matched = true;      
    }
    if (matched) continue;

    int etaIndex = getEtaIndex( i_trk->eta() );
    if (etaIndex == -1) continue;

    float e = i_trk->p();

    energy[etaIndex] += e;
    et[etaIndex] += i_trk->pt();
    eFlavor[untrk][etaIndex] += e;
    e2[etaIndex] += (e*e);
    nPart[etaIndex] ++;
  }

  for (int i=0; i != nEta; ++i){

    for (int flav = 0; flav != numFlavors; ++flav){
      UChar_t f_value; float eFlav = eFlavor[flav][i]; float E = energy[i];

      if (eFlav == 0)      f_value = 0;
      else if (eFlav == E) f_value = 255;
      else                 f_value = int( eFlav * 256 / E );

      f[flav][i] = f_value;
    }

    nPart[i] == 0 ? eRMS[i] = 0 : eRMS[i] = sqrt( e2[i]/nPart[i] );
  }


//------------ PF Jets ------------//

  edm::Handle<reco::PFJetCollection> pfJets;
  iEvent.getByToken(pfJetTag_, pfJets);

  ht = 0;
  for (reco::PFJetCollection::const_iterator i_jet = pfJets->begin(); i_jet != pfJets->end(); ++i_jet) {
    float pt = i_jet->pt();
    if (pt > 10) ht += pt;
  }
  //pfJets->size()<MAXJETS ? nJets = pfJets->size() : nJets = MAXJETS;
  nJets = pfJets->size();
  v_jet_eta.clear();
  v_jet_phi.clear();
  v_jet_pt.clear();
  v_jet_area.clear();

  v_jet_ch.clear();
  v_jet_nh.clear();
  v_jet_ne.clear();
  v_jet_hfh.clear();
  v_jet_hfe.clear();
  v_jet_lep.clear();

  for (int i=0; i != nJets; ++i){
    const reco::PFJet &jet = pfJets->at(i);
    float jetE = jet.energy();

    v_jet_eta.push_back(jet.eta());
    v_jet_phi.push_back(jet.phi());
    v_jet_pt.push_back(jet.pt());
    v_jet_area.push_back(jet.jetArea());

    // Protective logic for zero or negative energy
    if (jetE > 1e-6) { // Use a small epsilon for safety
      v_jet_ch.push_back(jet.chargedHadronEnergy() / jetE);
      v_jet_nh.push_back(jet.neutralHadronEnergy() / jetE);
      v_jet_ne.push_back(jet.photonEnergy() / jetE);
      v_jet_hfh.push_back(jet.HFHadronEnergy() / jetE);
      v_jet_hfe.push_back(jet.HFEMEnergy() / jetE);
      v_jet_lep.push_back((jet.chargedEmEnergy() + jet.chargedMuEnergy()) / jetE);
    } else {
      // Use -999 to distinguish from physical 0.0 fraction
      v_jet_ch.push_back(-999); 
      v_jet_nh.push_back(-999); 
      v_jet_ne.push_back(-999);
      v_jet_hfh.push_back(-999); 
      v_jet_hfe.push_back(-999); 
      v_jet_lep.push_back(-999);
    }
  }

//--------- Trigger Information by SK--------//

   edm::Handle<edm::TriggerResults> triggerBits;
   iEvent.getByToken(triggerBits_, triggerBits);

   // MiniAOD-only prescales (do not use in AOD)
   // edm::Handle<pat::PackedTriggerPrescales> triggerPrescales;
   // iEvent.getByToken(triggerPrescales_, triggerPrescales);

   const edm::TriggerNames &names = iEvent.triggerNames(*triggerBits);

   bool trigPass_;
   bool trigError_;
   bool trigRun_;

   trigger_name.clear();
   trigger_error.clear();
   trigger_run.clear();
   trigger_pass.clear();
   trigger_prescale.clear();

   for (unsigned int i = 0, n = triggerBits->size(); i < n; ++i)
   {
      // Loop over the user-defined trigger patterns
      for (unsigned int j = 0; j < triggerList.size(); ++j)
      {
         // Reset per (i, j) check
         trigPass_  = false;
         trigError_ = false;
         trigRun_   = false;

         // Store prescale as an integer (AOD: read via HLTConfigProvider)
         int trigPreScale_ = -1;

         if (TString(names.triggerName(i)).Contains(triggerList.at(j)) )
         {
            // Basic trigger info from TriggerResults
            trigPass_  = triggerBits->accept(i);
            trigError_ = triggerBits->error(i);
            trigRun_   = triggerBits->wasrun(i);

            // AOD prescale: depends on run/lumi, provided by HLTConfigProvider
            //trigPreScale_ = hltConfig_.prescaleValue(iEvent, iSetup, names.triggerName(i));
            //auto psPair = hltPrescaleProvider_.prescaleValues(iEvent, iSetup, names.triggerName(i));
	    auto prescales = hltPrescaleProvider_.prescaleValues<double, double>(iEvent, iSetup, names.triggerName(i));
            trigPreScale_ = prescales.first * prescales.second; 
            trigger_pass.push_back(trigPass_);
            trigger_error.push_back(trigError_);
            trigger_run.push_back(trigRun_);
            trigger_prescale.push_back(trigPreScale_);
	    cout << " names.triggerName(i) " << names.triggerName(i) << "trigPreScale_ : " << trigPreScale_ << endl;
            trigger_name.push_back(names.triggerName(i));
         }
      } // j
   } // i

//------------ Gen Jets ------------//
  if (isMC_){
     edm::Handle< vector<reco::GenJet> > genJets;
     iEvent.getByToken(genJetTag_, genJets);
     
     
     genht = 0;
     //cout << "genJets->size()  : " << genJets->size() << endl;
     nGenJets = genJets->size();
     //genJets->size()<MAXGENJETS ? nGenJets = genJets->size() : nGenJets = MAXGENJETS;
     //cout << "nGenJets  : " << nGenJets << endl;
     v_genjet_eta.clear();
     v_genjet_phi.clear();
     v_genjet_pt.clear();
     v_genjet_area.clear();
     
     for (vector<reco::GenJet>::const_iterator itgJet = genJets->begin() ; itgJet !=genJets->end(); itgJet++) {
     
       float pt = itgJet->pt();
       if (pt > 10) genht += pt;
       //cout << "itgJet-genJets->begin() " << itgJet-genJets->begin() << endl;
       v_genjet_eta.push_back(itgJet->eta());
       v_genjet_phi.push_back(itgJet->phi());
       v_genjet_pt.push_back(itgJet->pt());
       v_genjet_area.push_back(itgJet->jetArea());
     
      
       /*genjet_eta[itgJet-genJets->begin()] = itgJet->eta();
       genjet_phi[itgJet-genJets->begin()] = itgJet->phi();
       genjet_pt[itgJet-genJets->begin()] = itgJet->pt();
       genjet_area[itgJet-genJets->begin()] = itgJet->jetArea();*/
     }
  }


  v_rc1jet_numpf.clear();
  v_rc1jet_eta.clear();
  v_rc1jet_phi.clear();
  v_rc1jet_pt.clear();
  v_rc1jet_energy.clear();
  v_rc2jet_numpf.clear();
  v_rc2jet_eta.clear();
  v_rc2jet_phi.clear();
  v_rc2jet_pt.clear();
  v_rc2jet_energy.clear();
  v_isRC.clear();
  v_rc_diff.clear();

  // --- PUPPI RC: cone direction ---
  v_isRC_puppi.clear();
  v_rc1jet_puppi_numpf.clear();   v_rc2jet_puppi_numpf.clear();
  v_rc1jet_puppi_eta.clear();     v_rc2jet_puppi_eta.clear();
  v_rc1jet_puppi_phi.clear();     v_rc2jet_puppi_phi.clear();
  v_rc1jet_puppi_pt.clear();      v_rc2jet_puppi_pt.clear();
  v_rc1jet_puppi_energy.clear();  v_rc2jet_puppi_energy.clear();

  // --- PUPPI RC: JEC ---
  v_rc1jet_puppi_cor.clear();          v_rc2jet_puppi_cor.clear();
  v_rc1jet_puppi_coruncUp.clear();     v_rc2jet_puppi_coruncUp.clear();
  v_rc1jet_puppi_coruncDown.clear();   v_rc2jet_puppi_coruncDown.clear();
  v_rc1jet_puppi_corL1.clear();        v_rc2jet_puppi_corL1.clear();
  v_rc1jet_puppi_corL1RC.clear();      v_rc2jet_puppi_corL1RC.clear();
  v_rc1jet_puppi_corL1Nega.clear();    v_rc2jet_puppi_corL1Nega.clear();
  v_rc1jet_puppi_corL1RCNega.clear();  v_rc2jet_puppi_corL1RCNega.clear();
  v_rc1jet_puppi_corAll.clear();       v_rc2jet_puppi_corAll.clear();
  v_rc1jet_puppi_corAllRC.clear();     v_rc2jet_puppi_corAllRC.clear();
  v_rc1jet_puppi_corAllNega.clear();   v_rc2jet_puppi_corAllNega.clear();
  v_rc1jet_puppi_corAllRCNega.clear(); v_rc2jet_puppi_corAllRCNega.clear();

  // --- Stage 1: PUPPI ---
  v_rc1jet_puppiET.clear(); v_rc2jet_puppiET.clear();
  v_rc_puppi_diff.clear();
  v_rc1jet_puppiWsum.clear(); v_rc2jet_puppiWsum.clear();

  // --- Stage 2: per-particle-type raw ET ---
  v_rc1jet_et_nh.clear();  v_rc2jet_et_nh.clear();
  v_rc1jet_et_ne.clear();  v_rc2jet_et_ne.clear();
  v_rc1jet_et_chm.clear(); v_rc2jet_et_chm.clear();
  v_rc1jet_et_chu.clear(); v_rc2jet_et_chu.clear();
  v_rc1jet_et_hfh.clear(); v_rc2jet_et_hfh.clear();
  v_rc1jet_et_hfe.clear(); v_rc2jet_et_hfe.clear();

  // --- Stage 2: per-particle-type PUPPI-weighted ET ---
  v_rc1jet_puppiET_nh.clear();  v_rc2jet_puppiET_nh.clear();
  v_rc1jet_puppiET_ne.clear();  v_rc2jet_puppiET_ne.clear();
  v_rc1jet_puppiET_chm.clear(); v_rc2jet_puppiET_chm.clear();
  v_rc1jet_puppiET_hfh.clear(); v_rc2jet_puppiET_hfh.clear();
  v_rc1jet_puppiET_hfe.clear(); v_rc2jet_puppiET_hfe.clear();

  // --- Stage 4: HCAL depth ---
  v_rc1jet_hcalDepth1.clear(); v_rc2jet_hcalDepth1.clear();
  v_rc1jet_hcalDepth2.clear(); v_rc2jet_hcalDepth2.clear();
  v_rc1jet_hcalDepth3.clear(); v_rc2jet_hcalDepth3.clear();
  v_rc1jet_hcalDepth4.clear(); v_rc2jet_hcalDepth4.clear();
  v_rc1jet_hcalDepth5.clear(); v_rc2jet_hcalDepth5.clear();
  v_rc1jet_hcalDepth6.clear(); v_rc2jet_hcalDepth6.clear();
  v_rc1jet_hcalDepth7.clear(); v_rc2jet_hcalDepth7.clear();
/// Make R.C. jets ///
  for (vector<double>::iterator ibins = vEtaBins.begin() ; ibins !=vEtaBins.end(); ibins++) {
     //cout << "ibins: " << *ibins << endl;
     unsigned int etabinIndex = distance(vEtaBins.begin(), ibins);
     if (etabinIndex!=vEtaBins.size()-1){
        RandCone tmp;
        RandCone tmp_puppi;
        tmp       = SetRandomCone(vEtaBins[etabinIndex+1], vEtaBins[etabinIndex], isDebug_);
        tmp_puppi = SetRandomCone_PUPPI(vEtaBins[etabinIndex+1], vEtaBins[etabinIndex],
                                         isDebug_, usePuppiWsumCut_);
        //cout << vEtaBins[etabinIndex+1] << " " << vEtaBins[etabinIndex] << endl; 
        /// RC information ///
        v_isRC.push_back(tmp.findRC);
        v_rc_diff.push_back(tmp.diff_);
        v_rc1jet_numpf.push_back(tmp.num_pfcan_rc1);
        v_rc1jet_eta.push_back(tmp.rc1.Eta());
        v_rc1jet_phi.push_back(tmp.rc1.Phi());
        v_rc1jet_pt.push_back(tmp.rc1.Pt());
        v_rc1jet_energy.push_back(tmp.rc1.Energy());
        v_rc2jet_numpf.push_back(tmp.num_pfcan_rc2);
        v_rc2jet_eta.push_back(tmp.rc2.Eta());
        v_rc2jet_phi.push_back(tmp.rc2.Phi());
        v_rc2jet_pt.push_back(tmp.rc2.Pt());
        v_rc2jet_energy.push_back(tmp.rc2.Energy());

        // --- PUPPI RC: cone direction ---
        v_isRC_puppi.push_back(tmp_puppi.findRC);
        v_rc1jet_puppi_numpf.push_back(tmp_puppi.num_pfcan_rc1);
        v_rc1jet_puppi_eta.push_back(tmp_puppi.rc1.Eta());
        v_rc1jet_puppi_phi.push_back(tmp_puppi.rc1.Phi());
        v_rc1jet_puppi_pt.push_back(tmp_puppi.rc1.Pt());
        v_rc1jet_puppi_energy.push_back(tmp_puppi.rc1.Energy());
        v_rc2jet_puppi_numpf.push_back(tmp_puppi.num_pfcan_rc2);
        v_rc2jet_puppi_eta.push_back(tmp_puppi.rc2.Eta());
        v_rc2jet_puppi_phi.push_back(tmp_puppi.rc2.Phi());
        v_rc2jet_puppi_pt.push_back(tmp_puppi.rc2.Pt());
        v_rc2jet_puppi_energy.push_back(tmp_puppi.rc2.Energy());

        // --- Stage 1: PUPPI ET sum ---
        v_rc1jet_puppiET.push_back(tmp_puppi.rc1_puppiET);
        v_rc2jet_puppiET.push_back(tmp_puppi.rc2_puppiET);
        v_rc_puppi_diff.push_back(tmp_puppi.puppi_diff_);
        v_rc1jet_puppiWsum.push_back(tmp_puppi.rc1_puppiWsum);
        v_rc2jet_puppiWsum.push_back(tmp_puppi.rc2_puppiWsum);

        // --- Stage 2: per-particle-type raw ET ---
        v_rc1jet_et_nh.push_back(tmp_puppi.rc1_et_nh);   v_rc2jet_et_nh.push_back(tmp_puppi.rc2_et_nh);
        v_rc1jet_et_ne.push_back(tmp_puppi.rc1_et_ne);   v_rc2jet_et_ne.push_back(tmp_puppi.rc2_et_ne);
        v_rc1jet_et_chm.push_back(tmp_puppi.rc1_et_chm); v_rc2jet_et_chm.push_back(tmp_puppi.rc2_et_chm);
        v_rc1jet_et_chu.push_back(tmp_puppi.rc1_et_chu); v_rc2jet_et_chu.push_back(tmp_puppi.rc2_et_chu);
        v_rc1jet_et_hfh.push_back(tmp_puppi.rc1_et_hfh); v_rc2jet_et_hfh.push_back(tmp_puppi.rc2_et_hfh);
        v_rc1jet_et_hfe.push_back(tmp_puppi.rc1_et_hfe); v_rc2jet_et_hfe.push_back(tmp_puppi.rc2_et_hfe);

        // --- Stage 2: per-particle-type PUPPI-weighted ET ---
        v_rc1jet_puppiET_nh.push_back(tmp_puppi.rc1_puppiET_nh);   v_rc2jet_puppiET_nh.push_back(tmp_puppi.rc2_puppiET_nh);
        v_rc1jet_puppiET_ne.push_back(tmp_puppi.rc1_puppiET_ne);   v_rc2jet_puppiET_ne.push_back(tmp_puppi.rc2_puppiET_ne);
        v_rc1jet_puppiET_chm.push_back(tmp_puppi.rc1_puppiET_chm); v_rc2jet_puppiET_chm.push_back(tmp_puppi.rc2_puppiET_chm);
        v_rc1jet_puppiET_hfh.push_back(tmp_puppi.rc1_puppiET_hfh); v_rc2jet_puppiET_hfh.push_back(tmp_puppi.rc2_puppiET_hfh);
        v_rc1jet_puppiET_hfe.push_back(tmp_puppi.rc1_puppiET_hfe); v_rc2jet_puppiET_hfe.push_back(tmp_puppi.rc2_puppiET_hfe);

        // --- Stage 4: HCAL depth ET sum ---
        v_rc1jet_hcalDepth1.push_back(tmp_puppi.rc1_hcalDepth[0]); v_rc2jet_hcalDepth1.push_back(tmp_puppi.rc2_hcalDepth[0]);
        v_rc1jet_hcalDepth2.push_back(tmp_puppi.rc1_hcalDepth[1]); v_rc2jet_hcalDepth2.push_back(tmp_puppi.rc2_hcalDepth[1]);
        v_rc1jet_hcalDepth3.push_back(tmp_puppi.rc1_hcalDepth[2]); v_rc2jet_hcalDepth3.push_back(tmp_puppi.rc2_hcalDepth[2]);
        v_rc1jet_hcalDepth4.push_back(tmp_puppi.rc1_hcalDepth[3]); v_rc2jet_hcalDepth4.push_back(tmp_puppi.rc2_hcalDepth[3]);
        v_rc1jet_hcalDepth5.push_back(tmp_puppi.rc1_hcalDepth[4]); v_rc2jet_hcalDepth5.push_back(tmp_puppi.rc2_hcalDepth[4]);
        v_rc1jet_hcalDepth6.push_back(tmp_puppi.rc1_hcalDepth[5]); v_rc2jet_hcalDepth6.push_back(tmp_puppi.rc2_hcalDepth[5]);
        v_rc1jet_hcalDepth7.push_back(tmp_puppi.rc1_hcalDepth[6]); v_rc2jet_hcalDepth7.push_back(tmp_puppi.rc2_hcalDepth[6]);
         
     }
  }
 
/// Apply JEC to R.C. jets ///
  vector<JetCorrectorParameters> jetParsL1; // 
  vector<JetCorrectorParameters> jetParsL1RC; // 
  vector<JetCorrectorParameters> jetParsL1Nega; // 
  vector<JetCorrectorParameters> jetParsL1RCNega; // 
  vector<JetCorrectorParameters> jetParsL23; // 
  vector<JetCorrectorParameters> jetParsAll; // L1L2L3
  vector<JetCorrectorParameters> jetParsAllNega; // L1L2L3
  vector<JetCorrectorParameters> jetParsAllRC; // L1L2L3
  vector<JetCorrectorParameters> jetParsAllRCNega; // L1L2L3

  FactorizedJetCorrector* jetCorrectorsL1;
  FactorizedJetCorrector* jetCorrectorsL1RC;
  FactorizedJetCorrector* jetCorrectorsL1Nega;
  FactorizedJetCorrector* jetCorrectorsL1RCNega;
  FactorizedJetCorrector* jetCorrectorsL23;
  FactorizedJetCorrector* jetCorrectorsAll;
  FactorizedJetCorrector* jetCorrectorsAllRC;
  FactorizedJetCorrector* jetCorrectorsAllNega;
  FactorizedJetCorrector* jetCorrectorsAllRCNega;
  //CMSAnalyses/SSBAnalyzer/data/Spring16_25nsV10a/Spring16_25nsV10a_MC_SF_AK4PFchs.txt
  //string jer_sf_file = std::string(std::getenv("CMSSW_BASE")) + "/src/NosiseTest/OffsetTreeMaker/data";
  //JetCorrectorParameters* L1JetPars  = new JetCorrectorParameters("/d0/scratch/sha/Analyses/ServiceWork/JERC/NoiseTerm_v1/MakeNtuple_v1/V4/CMSSW_10_6_8_patch1/src/NosiseTest/OffsetTreeMaker/data/"+ era_ + "/" + era_ + "_L1FastJet_"    +  jet_type_ + ".txt");

  // Helper lambda: return path if file exists, else fallback to base L1FastJet file
  auto safeJECPath = [&](const std::string& path, const std::string& fallback) -> std::string {
    std::ifstream f(path);
    if (f.good()) return path;
    edm::LogWarning("OffsetTreeMaker") << "JEC file not found: " << path
                                       << " -- falling back to: " << fallback;
    return fallback;
  };

  std::string l1fj_chs     = jer_sf_file + era_ + "/" + era_ + "_L1FastJet_"    + jet_type_       + ".txt";
  std::string l1fj_puppi   = jer_sf_file + era_ + "/" + era_ + "_L1FastJet_"    + jet_type_puppi_ + ".txt";
  std::string l1fjNeg_chs  = safeJECPath(jer_sf_file + era_ + "/" + era_ + "_L1FastJet_" + jet_type_       + "_Negative.txt", l1fj_chs);
  std::string l1fjNeg_puppi= safeJECPath(jer_sf_file + era_ + "/" + era_ + "_L1FastJet_" + jet_type_puppi_ + "_Negative.txt", l1fj_puppi);

  JetCorrectorParameters* L1JetPars     = new JetCorrectorParameters(l1fj_chs);
  JetCorrectorParameters* L1RCPars      = new JetCorrectorParameters(l1fj_chs);        // L1RC not available in Run3; use L1FastJet
  JetCorrectorParameters* L1JetParsNeg  = new JetCorrectorParameters(l1fjNeg_chs);
  JetCorrectorParameters* L1RCParsNeg   = new JetCorrectorParameters(l1fjNeg_chs);     // L1RC not available in Run3; use L1FastJet
  JetCorrectorParameters* L2JetPars     = new JetCorrectorParameters(jer_sf_file + era_ + "/" + era_ + "_L2Relative_"   + jet_type_ + ".txt");
  JetCorrectorParameters* L3JetPars     = new JetCorrectorParameters(jer_sf_file + era_ + "/" + era_ + "_L3Absolute_"   + jet_type_ + ".txt");
  JetCorrectorParameters* ResJetPars    = new JetCorrectorParameters(jer_sf_file + era_ + "/" + era_ + "_L2L3Residual_" + jet_type_ + ".txt");

  JetCorrectionUncertainty* JecUncert   = new JetCorrectionUncertainty(jer_sf_file + era_ + "/" + era_ + "_Uncertainty_" + jet_type_ + ".txt");

  // --- PUPPI JEC parameters (AK4PFPuppi) ---
  // Note: L1RC not available in Run3 JEC database; L1FastJet used as substitute
  JetCorrectorParameters* L1JetPars_puppi     = new JetCorrectorParameters(l1fj_puppi);
  JetCorrectorParameters* L1RCPars_puppi      = new JetCorrectorParameters(l1fj_puppi);      // L1RC not available; use L1FastJet
  JetCorrectorParameters* L1JetParsNeg_puppi  = new JetCorrectorParameters(l1fjNeg_puppi);
  JetCorrectorParameters* L1RCParsNeg_puppi   = new JetCorrectorParameters(l1fjNeg_puppi);   // L1RC not available; use L1FastJet
  JetCorrectorParameters* L2JetPars_puppi     = new JetCorrectorParameters(jer_sf_file + era_ + "/" + era_ + "_L2Relative_"   + jet_type_puppi_ + ".txt");
  JetCorrectorParameters* L3JetPars_puppi     = new JetCorrectorParameters(jer_sf_file + era_ + "/" + era_ + "_L3Absolute_"   + jet_type_puppi_ + ".txt");
  JetCorrectorParameters* ResJetPars_puppi    = new JetCorrectorParameters(jer_sf_file + era_ + "/" + era_ + "_L2L3Residual_" + jet_type_puppi_ + ".txt");

  JetCorrectionUncertainty* JecUncert_puppi   = new JetCorrectionUncertainty(jer_sf_file + era_ + "/" + era_ + "_Uncertainty_" + jet_type_puppi_ + ".txt");

  // --- PUPPI FactorizedJetCorrector objects ---
  vector<JetCorrectorParameters> jetParsL1_puppi;
  vector<JetCorrectorParameters> jetParsL1RC_puppi;
  vector<JetCorrectorParameters> jetParsL1Nega_puppi;
  vector<JetCorrectorParameters> jetParsL1RCNega_puppi;
  vector<JetCorrectorParameters> jetParsL23_puppi;
  vector<JetCorrectorParameters> jetParsAll_puppi;
  vector<JetCorrectorParameters> jetParsAllNega_puppi;
  vector<JetCorrectorParameters> jetParsAllRC_puppi;
  vector<JetCorrectorParameters> jetParsAllRCNega_puppi;


  if (doL1L2L3Res_){
    //jetPars.push_back( *L1JetPars );
    jetParsL23.push_back( *L2JetPars );
    jetParsL23.push_back( *L3JetPars );
    if (!isMC_)jetParsL23.push_back( *ResJetPars );

    jetParsAll.push_back( *L1JetPars );
    jetParsAll.push_back( *L2JetPars );
    jetParsAll.push_back( *L3JetPars );
    if (!isMC_)jetParsAll.push_back( *ResJetPars );

    jetParsAllRC.push_back( *L1RCPars );
    jetParsAllRC.push_back( *L2JetPars );
    jetParsAllRC.push_back( *L3JetPars );
    if (!isMC_)jetParsAllRC.push_back( *ResJetPars );

    jetParsAllNega.push_back( *L1JetParsNeg );
    jetParsAllNega.push_back( *L2JetPars );
    jetParsAllNega.push_back( *L3JetPars );
    if (!isMC_)jetParsAllNega.push_back( *ResJetPars );

    jetParsAllRCNega.push_back( *L1RCParsNeg );
    jetParsAllRCNega.push_back( *L2JetPars );
    jetParsAllRCNega.push_back( *L3JetPars );
    if (!isMC_)jetParsAllRCNega.push_back( *ResJetPars );


    jetParsL1.push_back( *L1JetPars );
    jetParsL1RC.push_back(*L1RCPars); /// Only L1RC ///
    jetParsL1Nega.push_back( *L1JetParsNeg );
    jetParsL1RCNega.push_back( *L1RCParsNeg ); /// Only L1RC ///

  }
  jetCorrectorsL1 = new FactorizedJetCorrector( jetParsL1 );
  jetCorrectorsL1RC = new FactorizedJetCorrector( jetParsL1RC );
  jetCorrectorsL1Nega = new FactorizedJetCorrector( jetParsL1Nega );
  jetCorrectorsL1RCNega = new FactorizedJetCorrector( jetParsL1RCNega );
  jetCorrectorsL23 = new FactorizedJetCorrector( jetParsL23 );
  jetCorrectorsAll = new FactorizedJetCorrector( jetParsAll );
  jetCorrectorsAllRC = new FactorizedJetCorrector( jetParsAllRC );
  jetCorrectorsAllNega = new FactorizedJetCorrector( jetParsAllNega );
  jetCorrectorsAllRCNega = new FactorizedJetCorrector( jetParsAllRCNega );

  // --- Build PUPPI correctors (AK4PFPuppi) ---
  if (doL1L2L3Res_) {
    jetParsL23_puppi.push_back( *L2JetPars_puppi );
    jetParsL23_puppi.push_back( *L3JetPars_puppi );
    if (!isMC_) jetParsL23_puppi.push_back( *ResJetPars_puppi );

    jetParsAll_puppi.push_back( *L1JetPars_puppi );
    jetParsAll_puppi.push_back( *L2JetPars_puppi );
    jetParsAll_puppi.push_back( *L3JetPars_puppi );
    if (!isMC_) jetParsAll_puppi.push_back( *ResJetPars_puppi );

    jetParsAllRC_puppi.push_back( *L1RCPars_puppi );
    jetParsAllRC_puppi.push_back( *L2JetPars_puppi );
    jetParsAllRC_puppi.push_back( *L3JetPars_puppi );
    if (!isMC_) jetParsAllRC_puppi.push_back( *ResJetPars_puppi );

    jetParsAllNega_puppi.push_back( *L1JetParsNeg_puppi );
    jetParsAllNega_puppi.push_back( *L2JetPars_puppi );
    jetParsAllNega_puppi.push_back( *L3JetPars_puppi );
    if (!isMC_) jetParsAllNega_puppi.push_back( *ResJetPars_puppi );

    jetParsAllRCNega_puppi.push_back( *L1RCParsNeg_puppi );
    jetParsAllRCNega_puppi.push_back( *L2JetPars_puppi );
    jetParsAllRCNega_puppi.push_back( *L3JetPars_puppi );
    if (!isMC_) jetParsAllRCNega_puppi.push_back( *ResJetPars_puppi );

    jetParsL1_puppi.push_back( *L1JetPars_puppi );
    jetParsL1RC_puppi.push_back( *L1RCPars_puppi );
    jetParsL1Nega_puppi.push_back( *L1JetParsNeg_puppi );
    jetParsL1RCNega_puppi.push_back( *L1RCParsNeg_puppi );
  }

  FactorizedJetCorrector* jetCorrectorsL1_puppi       = new FactorizedJetCorrector( jetParsL1_puppi );
  FactorizedJetCorrector* jetCorrectorsL1RC_puppi      = new FactorizedJetCorrector( jetParsL1RC_puppi );
  FactorizedJetCorrector* jetCorrectorsL1Nega_puppi    = new FactorizedJetCorrector( jetParsL1Nega_puppi );
  FactorizedJetCorrector* jetCorrectorsL1RCNega_puppi  = new FactorizedJetCorrector( jetParsL1RCNega_puppi );
  FactorizedJetCorrector* jetCorrectorsL23_puppi       = new FactorizedJetCorrector( jetParsL23_puppi );
  FactorizedJetCorrector* jetCorrectorsAll_puppi       = new FactorizedJetCorrector( jetParsAll_puppi );
  FactorizedJetCorrector* jetCorrectorsAllRC_puppi     = new FactorizedJetCorrector( jetParsAllRC_puppi );
  FactorizedJetCorrector* jetCorrectorsAllNega_puppi   = new FactorizedJetCorrector( jetParsAllNega_puppi );
  FactorizedJetCorrector* jetCorrectorsAllRCNega_puppi = new FactorizedJetCorrector( jetParsAllRCNega_puppi );

  // AK4 PUPPI jets: raw pt and L2L3 correction (L2L3Residual added for data)
  // Corrected pt = jetpuppi_pt * jetpuppi_jec. jec is -1 when doL1L2L3Res is false
  edm::Handle<reco::PFJetCollection> pfJetsPuppi;
  iEvent.getByToken(pfJetPuppiTag_, pfJetsPuppi);
  v_jetpuppi_eta.clear(); v_jetpuppi_phi.clear(); v_jetpuppi_pt.clear();
  v_jetpuppi_area.clear(); v_jetpuppi_jec.clear();
  nJetsPuppi = pfJetsPuppi.isValid() ? static_cast<int>(pfJetsPuppi->size()) : 0;
  for (int i = 0; i != nJetsPuppi; ++i) {
    const reco::PFJet &pjet = pfJetsPuppi->at(i);
    v_jetpuppi_eta.push_back( pjet.eta() );
    v_jetpuppi_phi.push_back( pjet.phi() );
    v_jetpuppi_pt.push_back( pjet.pt() );
    v_jetpuppi_area.push_back( pjet.jetArea() );
    float pjec = -1.;
    if (doL1L2L3Res_) {
      jetCorrectorsL23_puppi->setJetEta( pjet.eta() );
      jetCorrectorsL23_puppi->setJetPt( pjet.pt() );
      jetCorrectorsL23_puppi->setJetA( pjet.jetArea() );
      jetCorrectorsL23_puppi->setRho( rho );
      pjec = jetCorrectorsL23_puppi->getCorrection();
    }
    v_jetpuppi_jec.push_back( pjec );
  }

  //cout << "v_rc1jet_eta : " << v_rc1jet_eta.size() << endl;

  v_rc1jet_cor.clear(); v_rc2jet_cor.clear();
  v_rc1jet_corL1.clear(); v_rc2jet_corL1.clear();
  v_rc1jet_corL1RC.clear(); v_rc2jet_corL1RC.clear();
  v_rc1jet_corL1Nega.clear(); v_rc2jet_corL1Nega.clear();
  v_rc1jet_corL1RCNega.clear(); v_rc2jet_corL1RCNega.clear();
  v_rc1jet_corAll.clear(); v_rc2jet_corAll.clear();
  v_rc1jet_corAllRC.clear(); v_rc2jet_corAllRC.clear();
  v_rc1jet_corAllNega.clear(); v_rc2jet_corAllNega.clear();
  v_rc1jet_corAllRCNega.clear(); v_rc2jet_corAllRCNega.clear();
  v_rc1jet_coruncUp.clear(); v_rc2jet_coruncUp.clear();
  v_rc1jet_coruncDown.clear(); v_rc2jet_coruncDown.clear();
  double jetArea_ = TMath::Pi()*pow(0.4,2);
  double l2l3jec_ = -999;
  for (unsigned int ijet = 0; ijet < v_rc1jet_eta.size(); ++ijet){
    if (v_isRC[ijet] == true){
      
       l2l3jec_ = -999; 
       jetCorrectorsL23->setJetEta( v_rc1jet_eta.at(ijet) );
       //jetCorrectorsL23->setJetPt( v_rc1jet_pt.at(ijet) );
       jetCorrectorsL23->setJetPt( 15.0 ); // To avoid wrong JEC with low energy region... //
       jetCorrectorsL23->setJetA( jetArea_ );
       jetCorrectorsL23->setRho(rho);
       l2l3jec_ = jetCorrectorsL23->getCorrection();
       v_rc1jet_cor.push_back(l2l3jec_); 
       
       jetCorrectorsAll->setJetEta( v_rc1jet_eta.at(ijet) );
       //jetCorrectorsAll->setJetPt( v_rc1jet_pt.at(ijet) );
       jetCorrectorsAll->setJetPt( 15.0 );
       jetCorrectorsAll->setJetA( jetArea_ );
       jetCorrectorsAll->setRho(rho);
       v_rc1jet_corAll.push_back(jetCorrectorsAll->getCorrection());
       
       jetCorrectorsAllRC->setJetEta( v_rc1jet_eta.at(ijet) );
       //jetCorrectorsAllRC->setJetPt( v_rc1jet_pt.at(ijet) );
       jetCorrectorsAllRC->setJetPt( 15.0 );
       jetCorrectorsAllRC->setJetA( jetArea_ );
       jetCorrectorsAllRC->setRho(rho);
       v_rc1jet_corAllRC.push_back(jetCorrectorsAllRC->getCorrection());
       
       jetCorrectorsAllNega->setJetEta( v_rc1jet_eta.at(ijet) );
       //jetCorrectorsAllNega->setJetPt( v_rc1jet_pt.at(ijet) );
       jetCorrectorsAllNega->setJetPt( 15.0 );
       jetCorrectorsAllNega->setJetA( jetArea_ );
       jetCorrectorsAllNega->setRho(rho);
       v_rc1jet_corAllNega.push_back(jetCorrectorsAllNega->getCorrection());
       
       jetCorrectorsAllRCNega->setJetEta( v_rc1jet_eta.at(ijet) );
       //jetCorrectorsAllRCNega->setJetPt( v_rc1jet_pt.at(ijet) );
       jetCorrectorsAllRCNega->setJetPt( 15.0 );
       jetCorrectorsAllRCNega->setJetA( jetArea_ );
       jetCorrectorsAllRCNega->setRho(rho);
       v_rc1jet_corAllRCNega.push_back(jetCorrectorsAllRCNega->getCorrection());
 
       jetCorrectorsL1RC->setJetEta( v_rc1jet_eta.at(ijet) );
       jetCorrectorsL1RC->setJetPt( l2l3jec_*v_rc1jet_pt.at(ijet) );
       //jetCorrectorsL1RC->setJetPt( 15.0 );
       jetCorrectorsL1RC->setJetA( jetArea_ );
       jetCorrectorsL1RC->setRho(rho);
       v_rc1jet_corL1RC.push_back(jetCorrectorsL1RC->getCorrection()); 
 
       jetCorrectorsL1RCNega->setJetEta( v_rc1jet_eta.at(ijet) );
       jetCorrectorsL1RCNega->setJetPt( l2l3jec_*v_rc1jet_pt.at(ijet) );
       //jetCorrectorsL1RC->setJetPt( 15.0 );
       jetCorrectorsL1RCNega->setJetA( jetArea_ );
       jetCorrectorsL1RCNega->setRho(rho);
       v_rc1jet_corL1RCNega.push_back(jetCorrectorsL1RCNega->getCorrection());       

       jetCorrectorsL1->setJetEta( v_rc1jet_eta.at(ijet) );
       //jetCorrectorsL1->setJetPt( jetCorrectorsL23->getCorrection()*v_rc1jet_pt.at(ijet) );
       //cout << "l2l3jec_*v_rc1jet_pt.at(ijet) : " << l2l3jec_*v_rc1jet_pt.at(ijet) << endl;
       jetCorrectorsL1->setJetPt( l2l3jec_*v_rc1jet_pt.at(ijet) );
       //jetCorrectorsL1->setJetPt( 15.0 );
       jetCorrectorsL1->setJetA( jetArea_ );
       jetCorrectorsL1->setRho(rho);
       v_rc1jet_corL1.push_back(jetCorrectorsL1->getCorrection()); 
 
       jetCorrectorsL1Nega->setJetEta( v_rc1jet_eta.at(ijet) );
       //jetCorrectorsL1->setJetPt( jetCorrectorsL23->getCorrection()*v_rc1jet_pt.at(ijet) );
       //cout << "l2l3jec_*v_rc1jet_pt.at(ijet) : " << l2l3jec_*v_rc1jet_pt.at(ijet) << endl;
       jetCorrectorsL1Nega->setJetPt( l2l3jec_*v_rc1jet_pt.at(ijet) );
       //jetCorrectorsL1->setJetPt( 15.0 );
       jetCorrectorsL1Nega->setJetA( jetArea_ );
       jetCorrectorsL1Nega->setRho(rho);
       v_rc1jet_corL1Nega.push_back(jetCorrectorsL1Nega->getCorrection()); 
      
       
       JecUncert->setJetEta(v_rc1jet_eta.at(ijet));
       ////JecUncert->setJetPt(v_rc1jet_pt.at(ijet));
       JecUncert->setJetPt(15.0);
       v_rc1jet_coruncUp.push_back(1*JecUncert->getUncertainty(true)); 
       //v_rc1jet_coruncDown.push_back(-1*JecUncert->getUncertainty(true));
    } 
    else {

       v_rc1jet_corL1RCNega.push_back(-999); 
       v_rc1jet_corL1Nega.push_back(-999); 
       v_rc1jet_corL1RC.push_back(-999); 
       v_rc1jet_corL1.push_back(-999); 
       v_rc1jet_cor.push_back(-999); 
       v_rc1jet_corAll.push_back(-999);
       v_rc1jet_corAllRC.push_back(-999);
       v_rc1jet_corAllNega.push_back(-999);
       v_rc1jet_corAllRCNega.push_back(-999);
       v_rc1jet_coruncUp.push_back(-999); 
       v_rc1jet_coruncDown.push_back(-999);

    }
//    if (v_isRC[ijet])cout << "L1 RC : " <<  jetCorrectorsL1RC->getCorrection() << " L1 Jet : " << jetCorrectorsL1->getCorrection() << endl;
  }
  for (unsigned int ijet = 0; ijet < v_rc2jet_eta.size(); ++ijet){
    if (v_isRC[ijet] == true){

       l2l3jec_ =-999; 
       jetCorrectorsL23->setJetEta( v_rc2jet_eta.at(ijet) );
       //jetCorrectorsL23->setJetPt( v_rc2jet_pt.at(ijet) );
       jetCorrectorsL23->setJetPt( 15.0 );
       jetCorrectorsL23->setJetA( jetArea_ );
       jetCorrectorsL23->setRho(rho);
       l2l3jec_ = jetCorrectorsL23->getCorrection();
       v_rc2jet_cor.push_back(l2l3jec_); 
       
       jetCorrectorsAll->setJetEta( v_rc2jet_eta.at(ijet) );
       //jetCorrectorsAll->setJetPt( v_rc2jet_pt.at(ijet) );
       jetCorrectorsAll->setJetPt( 15.0 );
       jetCorrectorsAll->setJetA( jetArea_ );
       jetCorrectorsAll->setRho(rho);
       v_rc2jet_corAll.push_back(jetCorrectorsAll->getCorrection());
       
       jetCorrectorsAllRC->setJetEta( v_rc2jet_eta.at(ijet) );
       //jetCorrectorsAllRC->setJetPt( v_rc2jet_pt.at(ijet) );
       jetCorrectorsAllRC->setJetPt( 15.0 );
       jetCorrectorsAllRC->setJetA( jetArea_ );
       jetCorrectorsAllRC->setRho(rho);
       v_rc2jet_corAllRC.push_back(jetCorrectorsAllRC->getCorrection()); 
       
       jetCorrectorsAllNega->setJetEta( v_rc2jet_eta.at(ijet) );
       //jetCorrectorsAllNega->setJetPt( v_rc2jet_pt.at(ijet) );
       jetCorrectorsAllNega->setJetPt( 15.0 );
       jetCorrectorsAllNega->setJetA( jetArea_ );
       jetCorrectorsAllNega->setRho(rho);
       v_rc2jet_corAllNega.push_back(jetCorrectorsAllNega->getCorrection()); 
       
       jetCorrectorsAllRCNega->setJetEta( v_rc2jet_eta.at(ijet) );
       //jetCorrectorsAllRCNega->setJetPt( v_rc2jet_pt.at(ijet) );
       jetCorrectorsAllRCNega->setJetPt( 15.0 );
       jetCorrectorsAllRCNega->setJetA( jetArea_ );
       jetCorrectorsAllRCNega->setRho(rho);
       v_rc2jet_corAllRCNega.push_back(jetCorrectorsAllRCNega->getCorrection()); 
 
       jetCorrectorsL1RC->setJetEta( v_rc2jet_eta.at(ijet) );
       jetCorrectorsL1RC->setJetPt( l2l3jec_*v_rc2jet_pt.at(ijet) );
       //jetCorrectorsL1RC->setJetPt( 15.0 );
       jetCorrectorsL1RC->setJetA( jetArea_ );
       jetCorrectorsL1RC->setRho(rho);
       v_rc2jet_corL1RC.push_back(jetCorrectorsL1RC->getCorrection());

       jetCorrectorsL1RCNega->setJetEta( v_rc2jet_eta.at(ijet) );
       jetCorrectorsL1RCNega->setJetPt( l2l3jec_*v_rc2jet_pt.at(ijet) );
       //jetCorrectorsL1RC->setJetPt( 15.0 );
       jetCorrectorsL1RCNega->setJetA( jetArea_ );
       jetCorrectorsL1RCNega->setRho(rho);
       v_rc2jet_corL1RCNega.push_back(jetCorrectorsL1RCNega->getCorrection());       

       
       jetCorrectorsL1->setJetEta( v_rc2jet_eta.at(ijet) );
       jetCorrectorsL1->setJetPt( l2l3jec_*v_rc2jet_pt.at(ijet) );
       //jetCorrectorsL1->setJetPt( 15.0 );
       jetCorrectorsL1->setJetA( jetArea_ );
       jetCorrectorsL1->setRho(rho);
       v_rc2jet_corL1.push_back(jetCorrectorsL1->getCorrection()); 
 
       jetCorrectorsL1Nega->setJetEta( v_rc2jet_eta.at(ijet) );
       //jetCorrectorsL1->setJetPt( jetCorrectorsL23->getCorrection()*v_rc2jet_pt.at(ijet) );
       //cout << "l2l3jec_*v_rc2jet_pt.at(ijet) : " << l2l3jec_*v_rc2jet_pt.at(ijet) << endl;
       jetCorrectorsL1Nega->setJetPt( l2l3jec_*v_rc2jet_pt.at(ijet) );
       //jetCorrectorsL1->setJetPt( 15.0 );
       jetCorrectorsL1Nega->setJetA( jetArea_ );
       jetCorrectorsL1Nega->setRho(rho);
       v_rc2jet_corL1Nega.push_back(jetCorrectorsL1Nega->getCorrection()); 
                  
       JecUncert->setJetEta(v_rc2jet_eta.at(ijet));
       ////JecUncert->setJetPt(v_rc2jet_pt.at(ijet));
       JecUncert->setJetPt(15.0);
       v_rc2jet_coruncUp.push_back(1*JecUncert->getUncertainty(true));
       //v_rc2jet_coruncDown.push_back(-1*JecUncert->getUncertainty(true));
    }

    else {

       v_rc2jet_corL1RCNega.push_back(-999); 
       v_rc2jet_corL1Nega.push_back(-999); 
       v_rc2jet_corL1RC.push_back(-999); 
       v_rc2jet_corL1.push_back(-999); 
       v_rc2jet_cor.push_back(-999); 
       v_rc2jet_corAll.push_back(-999);
       v_rc2jet_corAllRC.push_back(-999);
       v_rc2jet_corAllNega.push_back(-999);
       v_rc2jet_corAllRCNega.push_back(-999);
       v_rc2jet_coruncUp.push_back(-999); 
       v_rc2jet_coruncDown.push_back(-999);

    }


  }

  // -- PUPPI RC JEC loop (same 9 variants as PFCHS, independent cones) ------
  double l2l3jec_puppi_ = -999;
  for (unsigned int ijet = 0; ijet < v_rc1jet_puppi_eta.size(); ++ijet) {

    if (v_isRC_puppi[ijet] == true) {

      // RC1
      l2l3jec_puppi_ = -999;
      jetCorrectorsL23_puppi->setJetEta( v_rc1jet_puppi_eta.at(ijet) );
      jetCorrectorsL23_puppi->setJetPt( 15.0 );
      jetCorrectorsL23_puppi->setJetA( jetArea_ );
      jetCorrectorsL23_puppi->setRho( rho );
      l2l3jec_puppi_ = jetCorrectorsL23_puppi->getCorrection();
      v_rc1jet_puppi_cor.push_back(l2l3jec_puppi_);

      jetCorrectorsAll_puppi->setJetEta( v_rc1jet_puppi_eta.at(ijet) );
      jetCorrectorsAll_puppi->setJetPt( 15.0 );
      jetCorrectorsAll_puppi->setJetA( jetArea_ );
      jetCorrectorsAll_puppi->setRho( rho );
      v_rc1jet_puppi_corAll.push_back(jetCorrectorsAll_puppi->getCorrection());

      jetCorrectorsAllRC_puppi->setJetEta( v_rc1jet_puppi_eta.at(ijet) );
      jetCorrectorsAllRC_puppi->setJetPt( 15.0 );
      jetCorrectorsAllRC_puppi->setJetA( jetArea_ );
      jetCorrectorsAllRC_puppi->setRho( rho );
      v_rc1jet_puppi_corAllRC.push_back(jetCorrectorsAllRC_puppi->getCorrection());

      jetCorrectorsAllNega_puppi->setJetEta( v_rc1jet_puppi_eta.at(ijet) );
      jetCorrectorsAllNega_puppi->setJetPt( 15.0 );
      jetCorrectorsAllNega_puppi->setJetA( jetArea_ );
      jetCorrectorsAllNega_puppi->setRho( rho );
      v_rc1jet_puppi_corAllNega.push_back(jetCorrectorsAllNega_puppi->getCorrection());

      jetCorrectorsAllRCNega_puppi->setJetEta( v_rc1jet_puppi_eta.at(ijet) );
      jetCorrectorsAllRCNega_puppi->setJetPt( 15.0 );
      jetCorrectorsAllRCNega_puppi->setJetA( jetArea_ );
      jetCorrectorsAllRCNega_puppi->setRho( rho );
      v_rc1jet_puppi_corAllRCNega.push_back(jetCorrectorsAllRCNega_puppi->getCorrection());

      jetCorrectorsL1RC_puppi->setJetEta( v_rc1jet_puppi_eta.at(ijet) );
      jetCorrectorsL1RC_puppi->setJetPt( l2l3jec_puppi_ * v_rc1jet_puppi_pt.at(ijet) );
      jetCorrectorsL1RC_puppi->setJetA( jetArea_ );
      jetCorrectorsL1RC_puppi->setRho( rho );
      v_rc1jet_puppi_corL1RC.push_back(jetCorrectorsL1RC_puppi->getCorrection());

      jetCorrectorsL1RCNega_puppi->setJetEta( v_rc1jet_puppi_eta.at(ijet) );
      jetCorrectorsL1RCNega_puppi->setJetPt( l2l3jec_puppi_ * v_rc1jet_puppi_pt.at(ijet) );
      jetCorrectorsL1RCNega_puppi->setJetA( jetArea_ );
      jetCorrectorsL1RCNega_puppi->setRho( rho );
      v_rc1jet_puppi_corL1RCNega.push_back(jetCorrectorsL1RCNega_puppi->getCorrection());

      jetCorrectorsL1_puppi->setJetEta( v_rc1jet_puppi_eta.at(ijet) );
      jetCorrectorsL1_puppi->setJetPt( l2l3jec_puppi_ * v_rc1jet_puppi_pt.at(ijet) );
      jetCorrectorsL1_puppi->setJetA( jetArea_ );
      jetCorrectorsL1_puppi->setRho( rho );
      v_rc1jet_puppi_corL1.push_back(jetCorrectorsL1_puppi->getCorrection());

      jetCorrectorsL1Nega_puppi->setJetEta( v_rc1jet_puppi_eta.at(ijet) );
      jetCorrectorsL1Nega_puppi->setJetPt( l2l3jec_puppi_ * v_rc1jet_puppi_pt.at(ijet) );
      jetCorrectorsL1Nega_puppi->setJetA( jetArea_ );
      jetCorrectorsL1Nega_puppi->setRho( rho );
      v_rc1jet_puppi_corL1Nega.push_back(jetCorrectorsL1Nega_puppi->getCorrection());

      JecUncert_puppi->setJetEta( v_rc1jet_puppi_eta.at(ijet) );
      JecUncert_puppi->setJetPt( 15.0 );
      v_rc1jet_puppi_coruncUp.push_back(  1 * JecUncert_puppi->getUncertainty(true));
      v_rc1jet_puppi_coruncDown.push_back(-1 * JecUncert_puppi->getUncertainty(true));

      // RC2
      l2l3jec_puppi_ = -999;
      jetCorrectorsL23_puppi->setJetEta( v_rc2jet_puppi_eta.at(ijet) );
      jetCorrectorsL23_puppi->setJetPt( 15.0 );
      jetCorrectorsL23_puppi->setJetA( jetArea_ );
      jetCorrectorsL23_puppi->setRho( rho );
      l2l3jec_puppi_ = jetCorrectorsL23_puppi->getCorrection();
      v_rc2jet_puppi_cor.push_back(l2l3jec_puppi_);

      jetCorrectorsAll_puppi->setJetEta( v_rc2jet_puppi_eta.at(ijet) );
      jetCorrectorsAll_puppi->setJetPt( 15.0 );
      jetCorrectorsAll_puppi->setJetA( jetArea_ );
      jetCorrectorsAll_puppi->setRho( rho );
      v_rc2jet_puppi_corAll.push_back(jetCorrectorsAll_puppi->getCorrection());

      jetCorrectorsAllRC_puppi->setJetEta( v_rc2jet_puppi_eta.at(ijet) );
      jetCorrectorsAllRC_puppi->setJetPt( 15.0 );
      jetCorrectorsAllRC_puppi->setJetA( jetArea_ );
      jetCorrectorsAllRC_puppi->setRho( rho );
      v_rc2jet_puppi_corAllRC.push_back(jetCorrectorsAllRC_puppi->getCorrection());

      jetCorrectorsAllNega_puppi->setJetEta( v_rc2jet_puppi_eta.at(ijet) );
      jetCorrectorsAllNega_puppi->setJetPt( 15.0 );
      jetCorrectorsAllNega_puppi->setJetA( jetArea_ );
      jetCorrectorsAllNega_puppi->setRho( rho );
      v_rc2jet_puppi_corAllNega.push_back(jetCorrectorsAllNega_puppi->getCorrection());

      jetCorrectorsAllRCNega_puppi->setJetEta( v_rc2jet_puppi_eta.at(ijet) );
      jetCorrectorsAllRCNega_puppi->setJetPt( 15.0 );
      jetCorrectorsAllRCNega_puppi->setJetA( jetArea_ );
      jetCorrectorsAllRCNega_puppi->setRho( rho );
      v_rc2jet_puppi_corAllRCNega.push_back(jetCorrectorsAllRCNega_puppi->getCorrection());

      jetCorrectorsL1RC_puppi->setJetEta( v_rc2jet_puppi_eta.at(ijet) );
      jetCorrectorsL1RC_puppi->setJetPt( l2l3jec_puppi_ * v_rc2jet_puppi_pt.at(ijet) );
      jetCorrectorsL1RC_puppi->setJetA( jetArea_ );
      jetCorrectorsL1RC_puppi->setRho( rho );
      v_rc2jet_puppi_corL1RC.push_back(jetCorrectorsL1RC_puppi->getCorrection());

      jetCorrectorsL1RCNega_puppi->setJetEta( v_rc2jet_puppi_eta.at(ijet) );
      jetCorrectorsL1RCNega_puppi->setJetPt( l2l3jec_puppi_ * v_rc2jet_puppi_pt.at(ijet) );
      jetCorrectorsL1RCNega_puppi->setJetA( jetArea_ );
      jetCorrectorsL1RCNega_puppi->setRho( rho );
      v_rc2jet_puppi_corL1RCNega.push_back(jetCorrectorsL1RCNega_puppi->getCorrection());

      jetCorrectorsL1_puppi->setJetEta( v_rc2jet_puppi_eta.at(ijet) );
      jetCorrectorsL1_puppi->setJetPt( l2l3jec_puppi_ * v_rc2jet_puppi_pt.at(ijet) );
      jetCorrectorsL1_puppi->setJetA( jetArea_ );
      jetCorrectorsL1_puppi->setRho( rho );
      v_rc2jet_puppi_corL1.push_back(jetCorrectorsL1_puppi->getCorrection());

      jetCorrectorsL1Nega_puppi->setJetEta( v_rc2jet_puppi_eta.at(ijet) );
      jetCorrectorsL1Nega_puppi->setJetPt( l2l3jec_puppi_ * v_rc2jet_puppi_pt.at(ijet) );
      jetCorrectorsL1Nega_puppi->setJetA( jetArea_ );
      jetCorrectorsL1Nega_puppi->setRho( rho );
      v_rc2jet_puppi_corL1Nega.push_back(jetCorrectorsL1Nega_puppi->getCorrection());

      JecUncert_puppi->setJetEta( v_rc2jet_puppi_eta.at(ijet) );
      JecUncert_puppi->setJetPt( 15.0 );
      v_rc2jet_puppi_coruncUp.push_back(  1 * JecUncert_puppi->getUncertainty(true));
      v_rc2jet_puppi_coruncDown.push_back(-1 * JecUncert_puppi->getUncertainty(true));

    } else {
      // PUPPI RC failed -> fill -999 for all JEC variants
      v_rc1jet_puppi_cor.push_back(-999);         v_rc2jet_puppi_cor.push_back(-999);
      v_rc1jet_puppi_coruncUp.push_back(-999);    v_rc2jet_puppi_coruncUp.push_back(-999);
      v_rc1jet_puppi_coruncDown.push_back(-999);  v_rc2jet_puppi_coruncDown.push_back(-999);
      v_rc1jet_puppi_corL1.push_back(-999);       v_rc2jet_puppi_corL1.push_back(-999);
      v_rc1jet_puppi_corL1RC.push_back(-999);     v_rc2jet_puppi_corL1RC.push_back(-999);
      v_rc1jet_puppi_corL1Nega.push_back(-999);   v_rc2jet_puppi_corL1Nega.push_back(-999);
      v_rc1jet_puppi_corL1RCNega.push_back(-999); v_rc2jet_puppi_corL1RCNega.push_back(-999);
      v_rc1jet_puppi_corAll.push_back(-999);      v_rc2jet_puppi_corAll.push_back(-999);
      v_rc1jet_puppi_corAllRC.push_back(-999);    v_rc2jet_puppi_corAllRC.push_back(-999);
      v_rc1jet_puppi_corAllNega.push_back(-999);  v_rc2jet_puppi_corAllNega.push_back(-999);
      v_rc1jet_puppi_corAllRCNega.push_back(-999);v_rc2jet_puppi_corAllRCNega.push_back(-999);
    }

  } // end PUPPI JEC loop

  delete jetCorrectorsL1;
  delete jetCorrectorsL1RC;
  delete jetCorrectorsL1Nega;
  delete jetCorrectorsL1RCNega;
  delete jetCorrectorsL23;
  delete jetCorrectorsAll;
  delete jetCorrectorsAllRC;
  delete jetCorrectorsAllNega;
  delete jetCorrectorsAllRCNega;

  // PUPPI correctors
  delete jetCorrectorsL1_puppi;
  delete jetCorrectorsL1RC_puppi;
  delete jetCorrectorsL1Nega_puppi;
  delete jetCorrectorsL1RCNega_puppi;
  delete jetCorrectorsL23_puppi;
  delete jetCorrectorsAll_puppi;
  delete jetCorrectorsAllRC_puppi;
  delete jetCorrectorsAllNega_puppi;
  delete jetCorrectorsAllRCNega_puppi;

  delete L1JetPars;
  delete L1RCPars;
  delete L1JetParsNeg;
  delete L1RCParsNeg;
  delete L2JetPars;
  delete L3JetPars;
  delete ResJetPars;
  delete JecUncert;

  // PUPPI JEC parameters
  delete L1JetPars_puppi;
  delete L1RCPars_puppi;
  delete L1JetParsNeg_puppi;
  delete L1RCParsNeg_puppi;
  delete L2JetPars_puppi;
  delete L3JetPars_puppi;
  delete ResJetPars_puppi;
  delete JecUncert_puppi;

//------------ Fill Tree ------------//

  tree->Fill();
}


// ------------ method called once each job just after ending the event loop  ------------
void OffsetTreeMaker::endJob() {

//    root_file->Write();
//    root_file->Close();
    delete jveto;
}

int OffsetTreeMaker::getEtaIndex(float eta){

  for (int i=0; i != ETA_BINS; ++i){
    if (etabins[i] <= eta && eta < etabins[i+1]) return i;
  }
  if (eta == etabins[ETA_BINS]) return ETA_BINS-1;
  else return -1;
}


OffsetTreeMaker::Flavor OffsetTreeMaker::getFlavor(int id)
{
    if (id == 211) //h
        return chm;     //initially matched charged hadron
    else if (id == 11) //e
        return lep;
    else if (id == 13) //mu
        return lep;
    else if (id == 22) //gamma
        return ne;
    else if (id == 130) //h0
        return nh;
    else if (id == 1) //h_HF
        return hfh;
    else if (id == 2) //egamma_HF
        return hfe;
    else
        return X;
}
bool OffsetTreeMaker::IsPFCHS(int itype)
{
  /* Original Flavor Mapping:
     chm : 0, chu : 1, nh : 2, ne : 3, hfh : 4, hfe : 5, lep : 6, untrk : 7
  */

  bool isInteresting = false;

  // We typically keep Charged Hadron Matched (chm) and Leptons (lep) for CHS jets
  if (itype == 0) isInteresting = true; // chm (Charged Hadron Matched to PV)
  if (itype == 6) isInteresting = true; // lep (Leptons)

  // In some analyses, you might want to include chu (1) or others,
  // but for standard CHS, we focus on PV-associated tracks.

  return isInteresting;
}
RandCone OffsetTreeMaker::SetRandomCone(double upperEta, double lowerEta, bool isDebug)
{
   // Safety check for random generator
   if (!rand) {
      std::cout << "[DEBUG] Random generator is null!" << std::endl;
      RandCone tmp;
      tmp.findRC = false;
      tmp.diff_ = -99999999;
      return tmp;
   }

   // Safety check for PF candidate vectors
   if (pf_pt.empty() || 
       pf_pt.size() != pf_eta.size() || 
       pf_pt.size() != pf_phi.size() || 
       pf_pt.size() != pf_energy.size() ||
       pf_pt.size() != pf_et.size() ||
       pf_pt.size() != pf_type.size() ||
       pf_pt.size() != pf_puppiW.size() ||
       pf_pt.size() != pf_hcalDepth1.size()) {
      std::cout << "[DEBUG] PF candidate vectors size mismatch or empty!" << std::endl;
      RandCone tmp;
      tmp.findRC = false;
      tmp.diff_ = -99999999;
      return tmp;
   }

   bool isPFInCone1 = false;
   bool isPFInCone2 = false;

   TLorentzVector clusterVectorE;
   TLorentzVector SumRCone1;
   TLorentzVector SumRCone2;
   TLorentzVector rndmCone1;
   TLorentzVector rndmCone2;

   double eta1 = -999;
   double eta2 = -999;
   double phi1 = -999;
   double phi2 = -999;

   ////////////////////////////////////////////
   /// How many choose random cone in event ///
   ////////////////////////////////////////////

   int NumOfSelct = 0;
   int maxSelct = 10;  // Increased for better success rate

   double RangeEta = (upperEta - lowerEta) * 2;
   double RCone = 0.4;

   double depositETRC1 = 0;
   double depositETRC2 = 0;
   int num_rc1_pf = 0;
   int num_rc2_pf = 0;

   const size_t nPF = pf_pt.size();  // Cache size

   // Debug counters
   int rejectedByDeltaR = 0;
   int rejectedByJetVeto = 0;
   int rejectedByOverlap = 0;
   int rejectedByNoPF = 0;

   /// RC ///
   while ((isPFInCone1 == false || isPFInCone2 == false) && NumOfSelct < maxSelct)
   {
      NumOfSelct++;
      
      // Generate random phi for both cones
      phi1 = rand->Uniform(-TMath::Pi(), TMath::Pi());
      phi2 = rand->Uniform(-TMath::Pi(), TMath::Pi());

      // Generate random eta for RC1
      double rndValue = rand->Rndm();
      if (rndValue < 0.5) { 
         eta1 = -upperEta + rndValue * RangeEta; 
      } else { 
         eta1 = lowerEta + (rndValue - 0.5) * RangeEta; 
      }

      // Generate random eta for RC2
      rndValue = rand->Rndm();
      if (rndValue < 0.5) { 
         eta2 = -upperEta + rndValue * RangeEta; 
      } else { 
         eta2 = lowerEta + (rndValue - 0.5) * RangeEta; 
      }

      // Ensure opposite eta signs
      if (eta1 * eta2 > 0.) { 
         eta2 = -1 * eta2; 
      }

      // Set direction of RCs (unit vectors)
      rndmCone1.SetPtEtaPhiE(1, eta1, phi1, 1);
      rndmCone2.SetPtEtaPhiE(1, eta2, phi2, 1);

      // Check if cones are well-separated
      if (rndmCone1.DeltaR(rndmCone2) < 2 * RCone) { 
         rejectedByDeltaR++;
         continue; 
      }

      // Jet veto check
      if (doJetVeto_) {
         if (!jveto->VetoCheck(eta1, phi1)) { 
            rejectedByJetVeto++;
            continue; 
         }
         if (!jveto->VetoCheck(eta2, phi2)) { 
            rejectedByJetVeto++;
            continue; 
         }
      }

      // Reset variables for this iteration
      SumRCone1.SetXYZT(0, 0, 0, 0);
      SumRCone2.SetXYZT(0, 0, 0, 0);
      depositETRC1 = 0;
      depositETRC2 = 0;
      num_rc1_pf = 0;
      num_rc2_pf = 0;
      isPFInCone1 = false;
      isPFInCone2 = false;

      bool overlayed_ = false;

      // Loop over PF candidates (using cached size)
      for (size_t i = 0; i < nPF; i++)
      {
         if (!IsPFCHS(pf_type[i])) {  // Use [] instead of at() for speed
            continue; 
         }
         
         clusterVectorE.SetPtEtaPhiE(pf_pt[i], pf_eta[i], 
                                     pf_phi[i], pf_energy[i]);

         bool tempInCone1 = false;
         bool tempInCone2 = false;

         double dR1 = rndmCone1.DeltaR(clusterVectorE);
         double dR2 = rndmCone2.DeltaR(clusterVectorE);

         // Check if PF candidate is in RC1
         if (dR1 < RCone)
         {
            SumRCone1 += clusterVectorE;
            isPFInCone1 = true;
            tempInCone1 = true;
            depositETRC1 += pf_et[i];
            num_rc1_pf++;
         }

         // Check if PF candidate is in RC2
         if (dR2 < RCone)
         {
            SumRCone2 += clusterVectorE;
            isPFInCone2 = true;
            tempInCone2 = true;
            depositETRC2 += pf_et[i];
            num_rc2_pf++;
         }

         // Check for overlap: same PF candidate in both cones
         if (tempInCone1 && tempInCone2) {
            overlayed_ = true;
            break;  // No need to continue loop
         }
      }

      // If overlap detected, try again with new random cones
      if (overlayed_) {
         rejectedByOverlap++;
         continue;
      }

      // Check if we failed to find PF candidates in both cones
      if (!isPFInCone1 || !isPFInCone2) {
         rejectedByNoPF++;
      }

   } /// End of while

   // Debug output -- controlled by isDebug flag in config
   if (isDebug) {
     std::cout << "[DEBUG] SetRandomCone Summary:" << std::endl;
     std::cout << "  Total attempts: " << NumOfSelct << std::endl;
     std::cout << "  Rejected by DeltaR: "  << rejectedByDeltaR  << std::endl;
     std::cout << "  Rejected by JetVeto: " << rejectedByJetVeto << std::endl;
     std::cout << "  Rejected by Overlap: " << rejectedByOverlap << std::endl;
     std::cout << "  Rejected by No PF: "   << rejectedByNoPF   << std::endl;
     std::cout << "  Success: " << ((isPFInCone1 && isPFInCone2) ? "YES" : "NO") << std::endl;
     if (isPFInCone1 && isPFInCone2) {
        std::cout << "  RC1: eta=" << eta1 << ", phi=" << phi1
                  << ", ET=" << depositETRC1 << ", nPF=" << num_rc1_pf << std::endl;
        std::cout << "  RC2: eta=" << eta2 << ", phi=" << phi2
                  << ", ET=" << depositETRC2 << ", nPF=" << num_rc2_pf << std::endl;
        std::cout << "  ET difference: " << (depositETRC1 - depositETRC2) << std::endl;
     }
     std::cout << std::endl;
   }

   /// Prepare return value
   RandCone tmp;
   tmp.diff_ = -99999999;
   tmp.findRC = false;
   tmp.num_pfcan_rc1 = num_rc1_pf;
   tmp.num_pfcan_rc2 = num_rc2_pf;
   tmp.vecrc1 = SumRCone1;
   tmp.vecrc2 = SumRCone2;

   if ((NumOfSelct <= maxSelct) && isPFInCone1 && isPFInCone2)
   {
      tmp.findRC = true;
      tmp.diff_ = depositETRC1 - depositETRC2;
      tmp.rc1.SetPtEtaPhiE(depositETRC1, eta1, phi1, depositETRC1);
      tmp.rc2.SetPtEtaPhiE(depositETRC2, eta2, phi2, depositETRC2);
   }

   return tmp;
}

//=============================================================================
// SetRandomCone_PUPPI
// - All PF candidates included (no IsPFCHS filter)
// - isPFInCone decision: any particle present inside cone
// - Stage 1: PUPPI ET sum
// - Stage 2: per-particle-type raw ET + PUPPI-weighted ET
// - Stage 4: HCAL depth ET sum (nh and hfh only)
//=============================================================================
RandCone OffsetTreeMaker::SetRandomCone_PUPPI(double upperEta, double lowerEta,
                                               bool isDebug,
                                               bool usePuppiWsumCut)
{
   // Safety check
   if (!rand) {
      RandCone tmp; tmp.findRC = false; tmp.diff_ = -99999999;
      return tmp;
   }
   if (pf_pt.empty() ||
       pf_pt.size() != pf_eta.size()       ||
       pf_pt.size() != pf_phi.size()       ||
       pf_pt.size() != pf_energy.size()    ||
       pf_pt.size() != pf_et.size()        ||
       pf_pt.size() != pf_type.size()      ||
       pf_pt.size() != pf_puppiW.size()    ||
       pf_pt.size() != pf_hcalDepth1.size()) {
      RandCone tmp; tmp.findRC = false; tmp.diff_ = -99999999;
      return tmp;
   }

   bool isPFInCone1 = false;
   bool isPFInCone2 = false;

   TLorentzVector clusterVectorE;
   TLorentzVector SumRCone1, SumRCone2;
   TLorentzVector rndmCone1, rndmCone2;

   double eta1 = -999, eta2 = -999;
   double phi1 = -999, phi2 = -999;

   int NumOfSelct = 0;
   int maxSelct   = 10;

   double RangeEta = (upperEta - lowerEta) * 2;
   double RCone    = 0.4;

   // --- accumulators ---
   // Stage 1: PUPPI ET sum
   double puppiETRC1 = 0, puppiETRC2 = 0;

   // Stage 1: PUPPI weight sum (effective signal particle count)
   double puppiWsumRC1 = 0, puppiWsumRC2 = 0;
   const double kPuppiWsumCut = 0.0001;  // threshold for usePuppiWsumCut mode

   // Stage 2: per-particle-type raw ET
   double etRC1_nh  = 0, etRC2_nh  = 0;
   double etRC1_ne  = 0, etRC2_ne  = 0;
   double etRC1_chm = 0, etRC2_chm = 0;
   double etRC1_chu = 0, etRC2_chu = 0;
   double etRC1_hfh = 0, etRC2_hfh = 0;
   double etRC1_hfe = 0, etRC2_hfe = 0;

   // Stage 2: per-particle-type PUPPI-weighted ET
   double puppiETRC1_nh  = 0, puppiETRC2_nh  = 0;
   double puppiETRC1_ne  = 0, puppiETRC2_ne  = 0;
   double puppiETRC1_chm = 0, puppiETRC2_chm = 0;
   double puppiETRC1_hfh = 0, puppiETRC2_hfh = 0;
   double puppiETRC1_hfe = 0, puppiETRC2_hfe = 0;

   // Stage 4: HCAL depth ET sum
   double hcalDepthRC1[7] = {0}, hcalDepthRC2[7] = {0};

   int num_rc1_pf = 0, num_rc2_pf = 0;
   const size_t nPF = pf_pt.size();

   while ((isPFInCone1 == false || isPFInCone2 == false) && NumOfSelct < maxSelct)
   {
      NumOfSelct++;

      phi1 = rand->Uniform(-TMath::Pi(), TMath::Pi());
      phi2 = rand->Uniform(-TMath::Pi(), TMath::Pi());

      double rndValue = rand->Rndm();
      if (rndValue < 0.5) { eta1 = -upperEta + rndValue * RangeEta; }
      else                { eta1 =  lowerEta + (rndValue - 0.5) * RangeEta; }

      rndValue = rand->Rndm();
      if (rndValue < 0.5) { eta2 = -upperEta + rndValue * RangeEta; }
      else                { eta2 =  lowerEta + (rndValue - 0.5) * RangeEta; }

      // Ensure RC1 and RC2 are in opposite eta hemispheres
      if (eta1 * eta2 > 0.) { eta2 = -1 * eta2; }

      rndmCone1.SetPtEtaPhiE(1, eta1, phi1, 1);
      rndmCone2.SetPtEtaPhiE(1, eta2, phi2, 1);

      // Require the two cones to be well-separated
      if (rndmCone1.DeltaR(rndmCone2) < 2 * RCone) { continue; }

      // Jet veto
      if (doJetVeto_) {
         if (!jveto->VetoCheck(eta1, phi1)) { continue; }
         if (!jveto->VetoCheck(eta2, phi2)) { continue; }
      }

      // Reset accumulators
      SumRCone1.SetXYZT(0,0,0,0);
      SumRCone2.SetXYZT(0,0,0,0);
      isPFInCone1 = false;
      isPFInCone2 = false;
      num_rc1_pf  = 0;
      num_rc2_pf  = 0;
      puppiETRC1  = 0; puppiETRC2  = 0;
      puppiWsumRC1 = 0; puppiWsumRC2 = 0;
      etRC1_nh  = 0; etRC2_nh  = 0;
      etRC1_ne  = 0; etRC2_ne  = 0;
      etRC1_chm = 0; etRC2_chm = 0;
      etRC1_chu = 0; etRC2_chu = 0;
      etRC1_hfh = 0; etRC2_hfh = 0;
      etRC1_hfe = 0; etRC2_hfe = 0;
      puppiETRC1_nh  = 0; puppiETRC2_nh  = 0;
      puppiETRC1_ne  = 0; puppiETRC2_ne  = 0;
      puppiETRC1_chm = 0; puppiETRC2_chm = 0;
      puppiETRC1_hfh = 0; puppiETRC2_hfh = 0;
      puppiETRC1_hfe = 0; puppiETRC2_hfe = 0;
      for (int d = 0; d < 7; d++) { hcalDepthRC1[d] = 0; hcalDepthRC2[d] = 0; }

      bool overlayed_ = false;

      // PF candidate loop -- no IsPFCHS filter, all particle types included
      for (size_t i = 0; i < nPF; i++)
      {
         clusterVectorE.SetPtEtaPhiE(pf_pt[i], pf_eta[i], pf_phi[i], pf_energy[i]);

         double dR1 = rndmCone1.DeltaR(clusterVectorE);
         double dR2 = rndmCone2.DeltaR(clusterVectorE);

         bool tempInCone1 = false;
         bool tempInCone2 = false;

         float pw    = pf_puppiW[i];
         int   itype = pf_type[i];
         float iet   = pf_et[i];

         if (dR1 < RCone)
         {
            SumRCone1    += clusterVectorE;
            tempInCone1   = true;
            num_rc1_pf++;

            // Stage 1: PUPPI ET sum and weight sum
            puppiETRC1   += iet * pw;
            puppiWsumRC1 += pw;

            // isPFInCone logic:
            // default mode  : any particle present in cone
            // usePuppiWsumCut: require cumulative puppiWsum > threshold
            if (!usePuppiWsumCut) {
               isPFInCone1 = true;
            } else if (puppiWsumRC1 > kPuppiWsumCut) {
               isPFInCone1 = true;
            }

            // Stage 2: raw ET by particle type
            if      (itype == chm) { etRC1_chm += iet; }
            else if (itype == chu) { etRC1_chu += iet; }
            else if (itype == nh)  { etRC1_nh  += iet; }
            else if (itype == ne)  { etRC1_ne  += iet; }
            else if (itype == hfh) { etRC1_hfh += iet; }
            else if (itype == hfe) { etRC1_hfe += iet; }

            // Stage 2: PUPPI-weighted ET by particle type
            if      (itype == chm) { puppiETRC1_chm += iet * pw; }
            else if (itype == nh)  { puppiETRC1_nh  += iet * pw; }
            else if (itype == ne)  { puppiETRC1_ne  += iet * pw; }
            else if (itype == hfh) { puppiETRC1_hfh += iet * pw; }
            else if (itype == hfe) { puppiETRC1_hfe += iet * pw; }
            // chu omitted: PUPPI weight ~ 0 for pileup charged hadrons

            // Stage 4: HCAL depth ET sum (nh and hfh only)
            if (itype == nh || itype == hfh) {
               hcalDepthRC1[0] += pf_hcalDepth1[i];
               hcalDepthRC1[1] += pf_hcalDepth2[i];
               hcalDepthRC1[2] += pf_hcalDepth3[i];
               hcalDepthRC1[3] += pf_hcalDepth4[i];
               hcalDepthRC1[4] += pf_hcalDepth5[i];
               hcalDepthRC1[5] += pf_hcalDepth6[i];
               hcalDepthRC1[6] += pf_hcalDepth7[i];
            }
         }

         if (dR2 < RCone)
         {
            SumRCone2    += clusterVectorE;
            tempInCone2   = true;
            num_rc2_pf++;

            // Stage 1: PUPPI ET sum and weight sum
            puppiETRC2   += iet * pw;
            puppiWsumRC2 += pw;

            // isPFInCone logic
            if (!usePuppiWsumCut) {
               isPFInCone2 = true;
            } else if (puppiWsumRC2 > kPuppiWsumCut) {
               isPFInCone2 = true;
            }

            // Stage 2: raw ET by particle type
            if      (itype == chm) { etRC2_chm += iet; }
            else if (itype == chu) { etRC2_chu += iet; }
            else if (itype == nh)  { etRC2_nh  += iet; }
            else if (itype == ne)  { etRC2_ne  += iet; }
            else if (itype == hfh) { etRC2_hfh += iet; }
            else if (itype == hfe) { etRC2_hfe += iet; }

            // Stage 2: PUPPI-weighted ET by particle type
            if      (itype == chm) { puppiETRC2_chm += iet * pw; }
            else if (itype == nh)  { puppiETRC2_nh  += iet * pw; }
            else if (itype == ne)  { puppiETRC2_ne  += iet * pw; }
            else if (itype == hfh) { puppiETRC2_hfh += iet * pw; }
            else if (itype == hfe) { puppiETRC2_hfe += iet * pw; }

            // Stage 4: HCAL depth ET sum (nh and hfh only)
            if (itype == nh || itype == hfh) {
               hcalDepthRC2[0] += pf_hcalDepth1[i];
               hcalDepthRC2[1] += pf_hcalDepth2[i];
               hcalDepthRC2[2] += pf_hcalDepth3[i];
               hcalDepthRC2[3] += pf_hcalDepth4[i];
               hcalDepthRC2[4] += pf_hcalDepth5[i];
               hcalDepthRC2[5] += pf_hcalDepth6[i];
               hcalDepthRC2[6] += pf_hcalDepth7[i];
            }
         }

         // If the same particle falls in both cones, overlap detected -> retry
         if (tempInCone1 && tempInCone2) {
            overlayed_ = true;
            break;
         }
      }

      if (overlayed_) { continue; }

   } // end while

   // Return value
   RandCone tmp;
   tmp.findRC        = false;
   tmp.diff_         = -99999999;
   tmp.num_pfcan_rc1 = num_rc1_pf;
   tmp.num_pfcan_rc2 = num_rc2_pf;
   tmp.vecrc1        = SumRCone1;
   tmp.vecrc2        = SumRCone2;

   // Stage 1: PUPPI ET sum
   tmp.rc1_puppiET  = puppiETRC1;
   tmp.rc2_puppiET  = puppiETRC2;
   tmp.puppi_diff_  = puppiETRC1 - puppiETRC2;

   // Stage 1: PUPPI weight sum
   tmp.rc1_puppiWsum = puppiWsumRC1;
   tmp.rc2_puppiWsum = puppiWsumRC2;

   // Stage 2: raw ET by particle type
   tmp.rc1_et_nh  = etRC1_nh;  tmp.rc2_et_nh  = etRC2_nh;
   tmp.rc1_et_ne  = etRC1_ne;  tmp.rc2_et_ne  = etRC2_ne;
   tmp.rc1_et_chm = etRC1_chm; tmp.rc2_et_chm = etRC2_chm;
   tmp.rc1_et_chu = etRC1_chu; tmp.rc2_et_chu = etRC2_chu;
   tmp.rc1_et_hfh = etRC1_hfh; tmp.rc2_et_hfh = etRC2_hfh;
   tmp.rc1_et_hfe = etRC1_hfe; tmp.rc2_et_hfe = etRC2_hfe;

   // Stage 2: PUPPI-weighted ET by particle type
   tmp.rc1_puppiET_nh  = puppiETRC1_nh;  tmp.rc2_puppiET_nh  = puppiETRC2_nh;
   tmp.rc1_puppiET_ne  = puppiETRC1_ne;  tmp.rc2_puppiET_ne  = puppiETRC2_ne;
   tmp.rc1_puppiET_chm = puppiETRC1_chm; tmp.rc2_puppiET_chm = puppiETRC2_chm;
   tmp.rc1_puppiET_hfh = puppiETRC1_hfh; tmp.rc2_puppiET_hfh = puppiETRC2_hfh;
   tmp.rc1_puppiET_hfe = puppiETRC1_hfe; tmp.rc2_puppiET_hfe = puppiETRC2_hfe;

   // Stage 4: HCAL depth ET sum
   for (int d = 0; d < 7; d++) {
      tmp.rc1_hcalDepth[d] = hcalDepthRC1[d];
      tmp.rc2_hcalDepth[d] = hcalDepthRC2[d];
   }

   if ((NumOfSelct <= maxSelct) && isPFInCone1 && isPFInCone2)
   {
      tmp.findRC = true;
      tmp.diff_  = puppiETRC1 - puppiETRC2;
      tmp.rc1.SetPtEtaPhiE(puppiETRC1, eta1, phi1, puppiETRC1);
      tmp.rc2.SetPtEtaPhiE(puppiETRC2, eta2, phi2, puppiETRC2);
   }

   // Debug output -- controlled by isDebug flag in config
   if (isDebug) {
     std::cout << "[DEBUG] SetRandomCone_PUPPI Summary:" << std::endl;
     std::cout << "  Total attempts : " << NumOfSelct << " / " << maxSelct << std::endl;
     std::cout << "  Success        : " << (tmp.findRC ? "YES" : "NO") << std::endl;
     if (tmp.findRC) {
        std::cout << "  RC1: eta=" << eta1 << ", phi=" << phi1
                  << ", puppiET=" << puppiETRC1
                  << ", puppiWsum=" << puppiWsumRC1
                  << ", nPF=" << num_rc1_pf << std::endl;
        std::cout << "  RC2: eta=" << eta2 << ", phi=" << phi2
                  << ", puppiET=" << puppiETRC2
                  << ", puppiWsum=" << puppiWsumRC2
                  << ", nPF=" << num_rc2_pf << std::endl;
        std::cout << "  PUPPI diff    : " << tmp.puppi_diff_ << std::endl;
        std::cout << "  RC1 type ET   : nh=" << etRC1_nh  << " ne=" << etRC1_ne
                  << " chm=" << etRC1_chm << " chu=" << etRC1_chu
                  << " hfh=" << etRC1_hfh << " hfe=" << etRC1_hfe << std::endl;
        std::cout << "  RC2 type ET   : nh=" << etRC2_nh  << " ne=" << etRC2_ne
                  << " chm=" << etRC2_chm << " chu=" << etRC2_chu
                  << " hfh=" << etRC2_hfh << " hfe=" << etRC2_hfe << std::endl;
        std::cout << "  RC1 HCAL depth: d1=" << hcalDepthRC1[0] << " d2=" << hcalDepthRC1[1]
                  << " d3=" << hcalDepthRC1[2] << " d4=" << hcalDepthRC1[3]
                  << " d5=" << hcalDepthRC1[4] << " d6=" << hcalDepthRC1[5]
                  << " d7=" << hcalDepthRC1[6] << std::endl;
        std::cout << "  RC2 HCAL depth: d1=" << hcalDepthRC2[0] << " d2=" << hcalDepthRC2[1]
                  << " d3=" << hcalDepthRC2[2] << " d4=" << hcalDepthRC2[3]
                  << " d5=" << hcalDepthRC2[4] << " d6=" << hcalDepthRC2[5]
                  << " d7=" << hcalDepthRC2[6] << std::endl;
     }
     std::cout << std::endl;
   }

   return tmp;
}
DEFINE_FWK_MODULE(OffsetTreeMaker);
