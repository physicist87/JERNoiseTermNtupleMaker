#define JetVeto_cxx

#include <iostream>
#include <sstream>
#include <vector>
#include <stdio.h>
#include <stdlib.h>

#include "TMath.h"
#include "TH2.h"
#include "TStyle.h"
#include "TCanvas.h"

#include "JetVeto.hpp"

using namespace std;

bool JetVeto::VetoCheck(double eta, double phi)
{
   bool tmp =false;
   //cout << "eta : " << eta << " phi " << phi << endl;
   int etaBin = h_->GetXaxis()->FindBin(eta);
   int phiBin = h_->GetYaxis()->FindBin(phi);
   if( h_->GetBinContent(etaBin,phiBin) < 1) tmp = true;
   //if (!tmp) {cout << " Error !! eta : " << eta << " phi " << phi << endl;} 
   return tmp;
}
