#include <TH1F.h>
#include <sstream>
#include <string>
#include <TFile.h>
#include <TLegend.h>
#include <TCanvas.h>
#include <TPaveStats.h>
#include <stdio.h>
#include <iostream>
#include <vector>
#include <TKey.h>
#include <TStyle.h>
using namespace std;

////////////////////////////
///// Global Varialbes /////
////////////////////////////
TString OutDir = "./";
vector<TString> v_paths;
vector<TString> v_samplenames;

//// Declare of functions to use 
void SimpleOverlay(vector<TString>v_path,vector<TString> v_SampleName);
void SetOutputDir(TString outdir );
void OverlayAnsSave (TString tarName, TString refName , std::vector<TH1F*> v_htar, std::vector<TH1F*> v_href);
void AutoOverlayAnsSave (std::vector<TString> v_name,  vector< vector<TH1F*> > v_V_h);

///////////////////////////
/// Start Main Function ///
///////////////////////////
int main(int argc, char **argv)
{
   printf("The number of options is: %i\n",argc-1);

   if(argc<3)
   {
      printf("At least, you have to set 1, 2\n");
      printf("1. Input filelist\n");
      printf("2. Output Dir \n");
      printf("3. Configure File of SampleName\n");
      return 1;
   }

   for (int iopt=0; iopt<argc; iopt++)
   {
      printf("Option %i = %s\n",iopt,argv[iopt]);
   }

   char *inputfile = argv[1];
   printf("Input filelist = %s\n",inputfile);

   TString outputdir = argv[2];
   printf("Output Dir = %s\n",outputdir.Data());

   char *configfile = argv[3];
   printf("Output Dir = %s\n",configfile);
   /////////////////////
   /// Get File Path ///
   /////////////////////
   FILE *filelist;
   char filename[1000];
   string filelistDir, filelistName, filelistPath;

   filelistDir = "./input/";
 
   cout << endl;
   filelistName = inputfile;
   filelistPath = filelistDir + filelistName;
   filelist = fopen(filelistPath.c_str(),"r");
   while(filelist==NULL)
   {
      cout << "File not found, please try again." << endl;
      cout << "Filelist you want to use: " << filelistDir;
      cin >> filelistName;      
      filelistPath = filelistDir + filelistName;
      filelist=fopen(filelistPath.c_str(),"r");
   }
   cout << "filelistPath ? " << filelistPath << endl;

   TFile* f;
   while (fscanf(filelist, "%s", filename) != EOF)
   {  
      v_paths.push_back(filename);
   }
   fclose(filelist);

   //merge files
   FILE *conffile;
   string conffileDir, conffileName, conffilePath;

   conffileDir = "./Config/";
 
   cout << endl;
   conffileName = configfile;
   conffilePath = conffileDir + conffileName;
   conffile = fopen(conffilePath.c_str(),"r");
   
   while (fscanf(conffile, "%s", filename) != EOF)
   {  
      v_samplenames.push_back(filename);
   }
   fclose(conffile);
   /// Variables ///
   SetOutputDir(outputdir);
   
   SimpleOverlay(v_paths,v_samplenames);
   
   cout << "End Process !!!" << endl;
   return 0;
}

void SetOutputDir(TString outdir)
{
   OutDir = outdir;
}

void SimpleOverlay(vector<TString>v_path,vector<TString> v_SampleName)
{
   std::vector<TString> v_Paths;
   v_Paths.clear();
//   TFile *f1 = new TFile("Test.root");
//   TFile *f1 = new TFile("./RootFiles/"+ FileName+".root");
//   TFile *f2 = new TFile("./RootFiles/"+ FileName+".root");
//   v_Paths.push_back("/storage/palgongsan/sha/Analysis/CPViolation/ForMoriond_v3/RootFiles/V20_OnlyDiLeptonTrig_pTFunction/MuMu/TTJets_Signal.root");
//   v_Paths.push_back("/storage/palgongsan/sha/Analysis/CPViolation/ForMoriond_v3/RootFiles/V21_TopPtReweight/MuMu/TTJets_Signal.root");
   
   vector<TString> v_Names;
   v_Names.clear();
//   v_Names.push_back("V20_OnlyDiLeptonTrig_pTFunction");
//   v_Names.push_back("V21_TopPtReweight");

   v_Paths = v_path;
   v_Names = v_SampleName;

   vector<TH1F*>  v_Hists;
   vector<vector<TH1F*> > v_V_Hists;
   
   for (unsigned int i = 0; i < v_Paths.size(); ++i)
   {
      cout << v_Paths[i]<< endl;
      v_Hists.clear();
      TFile *f = new TFile(v_Paths[i]);
      string histkey, histclonname;
      TDirectory *current_sourcedir = gDirectory;
      TIter nextkey( current_sourcedir->GetListOfKeys() );
      TKey *key;
      TObject *obj;
      while ( (key= (TKey*)nextkey()))
      {
         f->cd();
         
         obj = key->ReadObj();
         
         if( obj->InheritsFrom("TH1"))
         {       
            histkey = key->GetName();
            f->cd();
            TH1F* Hist = (TH1F*)gDirectory->Get(key->GetName())->Clone(histkey.c_str());
            v_Hists.push_back(Hist);
            // Getting Entries,Mean and RMS //
            
         }// if
      }// while
      v_V_Hists.push_back(v_Hists);
   }
   cout << "v_V_Hists size : " << v_V_Hists.size() << endl; 
//   OverlayAnsSave(v_Names[0],v_Names[1],v_V_Hists[0],v_V_Hists[1]);

   AutoOverlayAnsSave(v_Names,v_V_Hists);
}
void OverlayAnsSave (TString tarName, TString refName , std::vector<TH1F*> v_htar, std::vector<TH1F*> v_href)
{
   TCanvas *c2 = new TCanvas("c2");
   TLegend *legend = new TLegend(.7,.45,.9,.9); 
   for (unsigned int i =0; i < v_htar.size(); ++i)
   {
      c2->cd();
      cout << "v_htar Name : " << v_htar[i]->GetName() << endl;
      v_htar[i]->SetLineColor(kBlue);
      v_href[i]->SetLineColor(kRed);
      v_htar[i]->Draw();
      v_href[i]->Draw("SameHist");
      legend->AddEntry(v_htar[i], tarName.Data(), "l");
      legend->AddEntry((TObject*)0,Form("Entries = %.0f",v_htar[i]->Integral()), "");
      legend->AddEntry((TObject*)0,Form("Mean = %.3f",v_htar[i]->GetMean()  ), "");
      legend->AddEntry((TObject*)0,Form("RMS = %.3f",v_htar[i]->GetRMS() ), "");
      legend->AddEntry(v_href[i], refName.Data(), "l");
      legend->AddEntry((TObject*)0,Form("Entries = %.0f",v_href[i]->Integral()), "");
      legend->AddEntry((TObject*)0,Form("Mean = %.3f",v_href[i]->GetMean()  ), "");
      legend->AddEntry((TObject*)0,Form("RMS = %.3f",v_href[i]->GetRMS() ), "");
      legend->SetFillColor(0);
      legend->SetTextSize(0.025);
      legend->Draw();
      c2->SaveAs(Form("./%s/%s_overlay.eps",OutDir.Data(),v_href[i]->GetName()));
      legend->Clear();
      c2->Clear();
   }
   delete c2;
   delete legend;
}
void AutoOverlayAnsSave (std::vector<TString> v_name,  vector< vector<TH1F*> > v_V_h)
{
   TCanvas *c2 = new TCanvas("c2");
   TLegend *legend = new TLegend(.7,.45,.9,.9);
   if (v_name.size() != v_V_h.size()) { cout << "V_Name != V-V-h"<< endl;return;}
   else {cout << "Let's Start Overlay !!!!" << endl;} 
   for (unsigned int i =0; i < v_V_h[0].size(); ++i)
   {
      c2->cd();
      for (unsigned int j =0; j < v_V_h.size(); ++j)
      {
         /*v_htar[i]->SetLineColor(kBlue);
         v_href[i]->SetLineColor(kRed);
         v_htar[i]->Draw();
         v_href[i]->Draw("SameHist");*/
         if (j == 0) 
         {
            v_V_h[j][i]->SetLineColor(kBlue);
         }
         else {
            v_V_h[j][i]->SetLineColor(kRed);
         }
         if (j == 0) 
         {
            v_V_h[j][i]->Draw();
         }
         else {
            v_V_h[j][i]->Draw("SameHist");
         }
         legend->AddEntry(v_V_h[j][i], v_name[j].Data(), "l");
         legend->AddEntry((TObject*)0,Form("Entries = %.0f",v_V_h[j][i]->Integral()), "");
         legend->AddEntry((TObject*)0,Form("Mean = %.3f",v_V_h[j][i]->GetMean()  ), "");
         legend->AddEntry((TObject*)0,Form("RMS = %.3f",v_V_h[j][i]->GetRMS() ), "");
      }
      legend->SetFillColor(0);
      legend->SetTextSize(0.025);
      legend->Draw();
      c2->SaveAs(Form("./%s/%s_overlay.eps",OutDir.Data(),v_V_h[0][i]->GetName()));
      legend->Clear();
      c2->Clear();
   }
   delete c2;
   delete legend;
}
double Integral(TH1F* h)
{
   double a;
   a = h->Integral();
   return a;
}
double Mean(TH1F* h)
{
   double a;
   a = h->GetMean();
   return a;
}
double RMS(TH1F* h)
{
   double a;
   a = h->GetRMS();
   return a;
}

