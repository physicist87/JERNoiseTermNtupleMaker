#ifndef _Simple_Overlay_

#define _Simple_Overlay_
  
#include <set>
#include <string>
#include <cassert>
#include <map>
#include <vector>
#include <string>
#include <TH1.h>
#include <TH2.h> 
#include <TFile.h>
#include <TH1F.h>
#include <TH2F.h>
#include <TH3F.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TH3D.h>
#include <THStack.h>
#include <TGraphErrors.h>
#include <TGraphAsymmErrors.h>
#include <TKey.h>
#include <TStyle.h>
#include <TF1.h>
#include "TMath.h"
#include "TSystem.h"
#include <TCanvas.h> 
#include <sstream>
#include <iostream>    
#include <TLegend.h>
#include "../include/tdrStyle.h"
using namespace std;

class Simple_Overlay
{
   public:
      Simple_Overlay();
      ~Simple_Overlay();

      //user define functions
      void SetChannel( TString ch);
      void SetHistOutputDir(TString HistDir);
      void SetDatOutputDir(TString DatDir);
      void SetOutputDatName(TString outname);
      std::map<TString,TH1F*> GetHistInfo( TFile* f );
      std::map<TString,TH1F*> SelectedGetHistInfo( TFile* f,TString selname );
      void GetColorCode( vector<TString> v_sn, std::map<TString,string> m_sn_col, std::map<TString,int> m_sn_subcol );
      void GetRebinSize( std::map<TString,int> m_sn_subcol );
      void GetReScale( std::map<TString,double> m_sn_scale_ );
      void SetCondition( vector<TString> v_sn, vector< std::map< TString,TH1F* > > v_map_hist , TString ref);
      void Overlay();
      void SetXLabel(TH1F* h1, TString h_name);
      void CalAsymVari(TH1F* h1);
      void MakeDAT();
      void Clear();
      void End();

   private:
      //put variables that you want
      std::map<TString,TH1F*> m_hn_hist;
      std::map<TString,TH1F*> m_hn_hist_data;
      std::map<TString,TH1F*> m_hn_hist_mc;
      std::map<TString,Color_t> m_sn_color;
      std::map<TString,double> m_sn_scale;
      std::map<TString,int> m_hn_rebin;
      std::vector< std::map< TString,TH1F* > > v_map_Hist;
      std::vector<TString> v_sampleName;
      std::vector<double> v_asymvari;
      std::vector<TString> v_asymvariName;
      ///---input variables---
      char *outfile;
      TFile *f1;
      TFile *fout;
      TString channel;
      TString chan;
      TString outName;//Dat file Name //
      TString SaveHistDir;
      TString SaveDatDir;
      std::vector<TString> v_syst_Name;
      bool isSys;
      double Lumi;
      //
      double Test;
   public:
      
      THStack *MCStack;
      TH1F *Tot_MC;
      TCanvas *c1;
};

#endif

#ifdef Simple_Overlay_cxx

Simple_Overlay::Simple_Overlay()
{
}

Simple_Overlay::~Simple_Overlay()
{
}

#endif
   
