void Overlay()
{
   TString fName_ref = "For_2016_PostVFP_Bin100.root";
   TString fName_tar = "For_2016_PostVFP_Bin99.root";

   TString hName_ref = "pileup";
   TString hName_tar = "pileup";
 
   TFile* f_ref = TFile::Open(Form("%s",fName_ref.Data()),"READ");
   TFile* f_tar = TFile::Open(Form("%s",fName_tar.Data()),"READ");

   TH1D* h_ref = (TH1D*)f_ref->Get(hName_ref.Data()); 
   TH1D* h_tar = (TH1D*)f_tar->Get(hName_tar.Data());

   h_ref->Draw();
   gPad->Update(); //IMPORTANT


   TPaveStats *tps1 = (TPaveStats*) h_ref->FindObject("stats");
   tps1->SetName("Hist1 Stats");
   double X1 = tps1->GetX1NDC();
   double Y1 = tps1->GetY1NDC();
   double X2 = tps1->GetX2NDC();
   double Y2 = tps1->GetY2NDC();

   cout << "X1 " << X1 << endl;
   cout << "X2 " << X2 << endl;
   cout << "Y1 " << Y1 << endl;
   cout << "Y2 " << Y2 << endl;
   cout << "Y2-Y1 " << Y2-Y1 << endl;
   cout << "Y1-(Y2-Y1) " << Y1-(Y2-Y1) << endl;
   /*Create Histogram 2, fill it some data and draw*/
   TCanvas *c1 = new TCanvas();
   h_tar->Draw();
   gPad->Update(); //IMPORTANT

   /* collect stat of the second histogram (h_tar) */
   TPaveStats *tps2 = (TPaveStats*) h_tar->FindObject("stats");
   tps2->SetTextColor(kRed);
   tps2->SetLineColor(kRed);
   tps2->SetX1NDC(X1);
   tps2->SetX2NDC(X2);
   tps2->SetY1NDC(Y1-(Y2-Y1));
   tps2->SetY2NDC(Y1);

   /* Draw all (two histograms and their stat boxes in one canvas */
   TCanvas *c3 = new TCanvas();
   h_ref->Draw();
   h_tar->Draw("same");
   tps1->Draw("same");
   tps2->Draw("same");

}
