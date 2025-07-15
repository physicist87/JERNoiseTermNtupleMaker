////////////////////////////////////////////////
//                                            //
//                                            //
//  Author: Seungkyu Ha, seungkyu.ha@cern.ch  //
//                                            //
//                                            //
////////////////////////////////////////////////

#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <ctime>
#include <TROOT.h>
#include <TUnixSystem.h>
#include <TChain.h>
#include <TStyle.h>
#include <TApplication.h>
#include <TString.h>
#include <TH2.h>
#include <TCanvas.h>
#include <TFile.h>
#include "./include/Simple_Overlay.hpp"

using namespace std;

TROOT root ("Plots", "Program for dream analysis");

//argc: # of arguments, argv:array for arguments
int main(int argc, char **argv)
{
   printf("The number of options is: %i\n",argc-1);

   if(argc<3)
   {
      printf("At least, you have to set 1, 2\n");
      printf("1. Input filelist\n");
      printf("2. HistDir. Name\n");
      printf("3. Channel  \n");
      return 1;
   }

   for (int iopt=0; iopt<argc; iopt++)
   {
      printf("Option %i = %s\n",iopt,argv[iopt]);
   }

   ///read input options

   //cout << "argc: " << argc << endl;

   char *inputfile = argv[1];
   printf("Input filelist = %s\n",inputfile);

   cout << "End processing..." << endl << endl;
   return 0;
}
