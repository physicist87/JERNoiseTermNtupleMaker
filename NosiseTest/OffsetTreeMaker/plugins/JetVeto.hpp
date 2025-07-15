#ifndef _JetVeto_

#define _JetVeto_
#include <set>
#include <string>
#include <cassert>
#include <map>
#include <vector>
#include <TH2D.h>
#include <TString.h>
#include <TFile.h>
using namespace std;

class JetVeto 
{
    public:
      //declare functions
      //JetVeto(TString fileName="", TString histName="");
      JetVeto(TString fileName, vector<string> v_hNames);
      virtual ~JetVeto();

      //user define functions

      bool VetoCheck(double eta, double phi);
 
   private:
      //put variables that you want
      TH2D* h_;
      TFile* f_;
};
#endif

#ifdef JetVeto_cxx

//JetVeto::JetVeto(TString fileName, TString histName)
JetVeto::JetVeto(TString fileName, vector<string> v_hNames)
{
   /// Read Text Configure file ///
   /*f_ = TFile::Open(Form("%s",fileName.Data()),"READ");
   if (f_==NULL) {cout << "NO FILE !!!!!" << endl;} 
   h_ = (TH2D*)f_->Get(histName.Data());
   if (f_==NULL) {cout << "NO HIST !!!!!" << endl;} */
   f_ = TFile::Open(Form("%s",fileName.Data()),"READ");
   if (f_==NULL) {cout << "NO FILE !!!!!" << endl;} 
   
   for (unsigned int i =0; i < v_hNames.size(); ++i) 
   {
      //TH2D* tmp = (TH2D*)f_->Get(v_histName[i].Data());
      TH2D* tmp = (TH2D*)f_->Get(v_hNames[i].c_str());
      //h_ = (TH2D*)f_->Get(histName.Data());
      if (i ==0 )h_ = (TH2D*)tmp->Clone("h_tmp");
      else {h_->Add(tmp);}
   }
}

JetVeto::~JetVeto()
{
}
#endif
