//*CMZ :  2.24/03 29/04/2000  11.22.48  by  Rene Brun
//*CMZ :  2.23/12 23/01/2000  20.01.39  by  Rene Brun
//*CMZ :  2.23/09 17/11/99  16.18.58  by  Rene Brun
//*CMZ :  2.23/08 01/11/99  14.33.24  by  Rene Brun
//*CMZ :  2.23/04 05/10/99  08.24.00  by  Fons Rademakers
//*CMZ :  2.23/03 16/09/99  12.46.25  by  Rene Brun
//*CMZ :  2.23/02 04/09/99  12.16.48  by  Rene Brun
//*CMZ :  2.23/01 26/08/99  14.24.02  by  Rene Brun
//*CMZ :  2.23/00 30/07/99  17.23.23  by  Rene Brun
//*-- Author :    Rene Brun   05/11/98
/////////////////////////////////////////////////////////////////
//
//    R O O T   T E S T  S U I T E  and  B E N C H M A R K S
//    ======================================================
//
// The suite of programs below test the essential parts of Root.
// In particular, there is an extensive test of the I/O and Trees.
// The test can be run in batch or with the interpreter.
// You must run
//   gmake  on Unix systems
//   nmake  on Windows
// To run in batch, do
//   stress   : with no parameters, run standard test with 1000 events
//   stress 30: run test with 30 events only
//
// To run interactively, do
// root -b
//  Root > .x stress.cxx(1000)  run standard test with 1000 events
//  Root > .x stress.cxx(30)    run with 30 events only
//
// The standard test with 1000 events will create several files.
// The size of all files is around 100 Mbytes.
// The test with 30 events only require around  20 Mbytes
//
// The tests runs sequentially 16 tests. Each test will produce
// one line (Test OK or Test failed) with some result parameters.
// At the end of the test a table is printed showing the global results
// with the amount of I/O, Real Time and Cpu Time.
// One single number (ROOTMARKS) is also calculated showing the relative
// performance of your machine compared to a reference machine (HP735/99).
//
// An example of output when all the tests run OK is shown below:
// ******************************************************************
// *  Starting  R O O T - S T R E S S test suite with 1000 events
// ******************************************************************
// Test  1 : Functions, Random Numbers, Histogram Fits............. OK
// Test  2 : Check size & compression factor of a Root file........ OK
// Test  3 : Purge, Reuse of gaps in TFile......................... OK
// Test  4 : Test of 2-d histograms, functions, 2-d fits........... OK
// Test  5 : Test graphics & Postscript............................ OK
// Test  6 : Test subdirectories in a Root file.................... OK
// Test  7 : TNtuple, selections, TCut, TCutG, TEventList.......... OK
// Test  8 : Trees split and compression modes..................... OK
// Test  9 : Analyze Event.root file of stress 8................... OK
// Test 10 : Create 10 files starting from Event.root.............. OK
// Test 11 : Test chains of Trees using the 10 files............... OK
// Test 12 : Compare histograms of test 9 and 11................... OK
// Test 13 : Test merging files of a chain......................... OK
// Test 14 : Check correct rebuilt of Event.root in test 13........ OK
// Test 15 : Divert Tree branches to separate files................ OK
// Test 16 : CINT test (3 nested loops) with LHCb trigger.......... OK
// ******************************************************************
// *  Linux pcnotebrun.cern.ch 2.0.33 #3 Sun Apr 12 10:53:37 METD
// ******************************************************************
// stress    : Total I/O =  425.6 Mbytes, I =  296.4, O = 129.3
// stress    : Compr I/O =  407.3 Mbytes, I =  286.3, O = 121.0
// stress    : Real Time = 365.89 seconds Cpu Time = 323.66 seconds
// ******************************************************************
// *  ROOTMARKS =  35.1   *  Root 2.00/13  981110/1312
// ******************************************************************
//
//_____________________________batch only_____________________
#ifndef __CINT__

#include <TROOT.h>
#include <TH1.h>
#include <TH2.h>
#include <TFile.h>
#include <TMath.h>
#include <TF1.h>
#include <TF2.h>
#include <TProfile.h>
#include <TKey.h>
#include <TCanvas.h>
#include <TGraph.h>
#include <TRandom.h>
#include <TPostScript.h>
#include <TNtuple.h>
#include <TChain.h>
#include <TCut.h>
#include <TCutG.h>
#include <TEventList.h>
#include <TBenchmark.h>
#include <TSystem.h>
#include <TApplication.h>
#include <TClassTable.h>
#include "Event.h"

   void stress(Int_t nevent);
   void stress1();
   void stress2();
   void stress3();
   void stress4();
   void stress5();
   void stress6();
   void stress7();
   void stress8(Int_t nevent);
   void stress9tree(TTree *tree);
   void stress9();
   void stress10();
   void stress11();
   void stress12(Int_t testid);
   void stress13();
   void stress14();
   void stress15();
   void stress16();
   void cleanup();


int main(int argc, char **argv)
{
   TROOT root("stress","The Root test program");
   TApplication theApp("App", &argc, argv);
   gBenchmark = new TBenchmark();
   Int_t nevent = 1000;      // by default create 1000 events
   if (argc > 1)  nevent = atoi(argv[1]);
   stress(nevent);
   return 0;
}

#endif

class TH1;
class TTree;

//_______________________common part_________________________

Double_t ntotin=0, ntotout=0;

void stress(Int_t nevent)
{
   //Main control function invoking all test programs

   //Delete all possible objects in memory (to execute stress several times)
   gROOT->GetListOfFunctions()->Delete();
   gROOT->GetList()->Delete();

   printf("******************************************************************\n");
   printf("*  Starting  R O O T - S T R E S S test suite with %d events\n",nevent);
   printf("******************************************************************\n");
   //Run the standard test suite
   gBenchmark->Start("stress");
   stress1();
   stress2();
   stress3();
   stress4();
   stress5();
   stress6();
   stress7();
   stress8(nevent);
   stress9();
   stress10();
   stress11();
   stress12(12);
   stress13();
   stress14();
   stress15();
   stress16();
   gBenchmark->Stop("stress");

   cleanup();

   //Print table with results
   Bool_t UNIX = strcmp(gSystem->GetName(), "Unix") == 0;
   printf("******************************************************************\n");
   if (UNIX) {
      FILE *fp = gSystem->OpenPipe("uname -a", "r");
      char line[60];
      fgets(line,60,fp); line[59] = 0;
      printf("*  %s\n",line);
      gSystem->ClosePipe(fp);
   } else {
      const char *os = gSystem->Getenv("OS");
      if (!os) printf("*  Windows 95\n");
      else     printf("*  %s %s \n",os,gSystem->Getenv("PROCESSOR_IDENTIFIER"));
   }

   printf("******************************************************************\n");
   Float_t mbtot = (Float_t)(ntotin+ntotout)/1000000.;
   Float_t mbin  = (Float_t)ntotin/1000000.;
   Float_t mbout = (Float_t)ntotout/1000000.;
   printf("stress    : Total I/O =%7.1f Mbytes, I =%7.1f, O =%6.1f\n",mbtot,mbin,mbout);
   Float_t mbin1  = (Float_t)(TFile::GetFileBytesRead()/1000000.);
   Float_t mbout1 = (Float_t)(TFile::GetFileBytesWritten()/1000000.);
   Float_t mbtot1 = mbin1+mbout1;
   printf("stress    : Compr I/O =%7.1f Mbytes, I =%7.1f, O =%6.1f\n",mbtot1,mbin1,mbout1);
   gBenchmark->Print("stress");
#ifndef __CINT__
   Float_t rt_hp10   = 171;  //HP times with the native compiler
   Float_t cp_hp10   = 152;
   Float_t rt_hp1000 = 674;
   Float_t cp_hp1000 = 411;
#else
   Float_t rt_hp10   = 476;  //HP times with CINT
   Float_t cp_hp10   = 451;  //The difference is essentially coming from stress16
   Float_t rt_hp1000 = 1165;
   Float_t cp_hp1000 = 828;
#endif
   Float_t cp_hp = cp_hp1000 - (cp_hp1000 - cp_hp10)*(1000-nevent)/(1000-10);
   Float_t rt_hp = rt_hp1000 - (rt_hp1000 - rt_hp10)*(1000-nevent)/(1000-10);
   Float_t rt = gBenchmark->GetRealTime("stress");
   Float_t ct = gBenchmark->GetCpuTime("stress");
   Float_t rootmarks = 27*(rt_hp + cp_hp)/(rt + ct);
   printf("******************************************************************\n");
   printf("*  ROOTMARKS =%6.1f   *  Root%-8s  %d/%d\n",rootmarks,gROOT->GetVersion(),gROOT->GetVersionDate(),gROOT->GetVersionTime());
   printf("******************************************************************\n");
}

//_______________________________________________________________
Double_t f1int(Double_t *x, Double_t *p)
{
   //Compute a function sum of 3 gaussians
   Double_t e1 = (x[0]-p[1])/p[2];
   Double_t e2 = (x[0]-p[4])/p[5];
   Double_t e3 = (x[0]-p[7])/p[8];
   Double_t f  = p[0]*TMath::Exp(-0.5*e1*e1)
                +p[3]*TMath::Exp(-0.5*e2*e2)
                +p[6]*TMath::Exp(-0.5*e3*e3);
   return f;
}

//_______________________________________________________________
void Bprint(Int_t id, const char *title)
{
  // Print test program number and its title
   const Int_t kMAX = 65;
   char header[80];
   sprintf(header,"Test %2d : %s",id,title);
   Int_t nch = strlen(header);
   for (Int_t i=nch;i<kMAX;i++) header[i] = '.';
   header[kMAX] = 0;
   header[kMAX-1] = ' ';
   printf("%s",header);
}

//_______________________________________________________________
void stress1()
{
   //Generate two functions supposed to produce the same result
   //One function "f1form" will be computed by the TFormula class
   //The second function "f1int" will be
   //   - compiled when running in batch mode
   //   - interpreted by CINT when running in interactive mode

   Bprint(1,"Functions, Random Numbers, Histogram Fits");

   //Start with a function inline expression (managed by TFormula)
   Double_t f1params[9] = {100,-3,3,60,0,0.5,40,4,0.7};
   TF1 *f1form = new TF1("f1form","gaus(0)+gaus(3)+gaus(6)",-10,10);
   f1form->SetParameters(f1params);

   //Create an histogram and fill it randomly with f1form
   gRandom->SetSeed();
   TH1F *h1form = new TH1F("h1form","distribution from f1form",100,-10,10);
   TH1F *h1diff = (TH1F*)h1form->Clone();
   h1diff->SetName("h1diff");
   h1form->FillRandom("f1form",10000);

   //Fit h1form with original function f1form
   h1form->Fit("f1form","q0");

   //same operation with an interpreted function f1int
   TF1 *f1 = new TF1("f1int",f1int,-10,10,9);
   f1->SetParameters(f1params);

   //Create an histogram and fill it randomly with f1int
   gRandom->SetSeed(); //make sure we start with the same random numbers
   TH1F *h1int = new TH1F("h1int","distribution from f1int",100,-10,10);
   h1int->FillRandom("f1int",10000);

   //Fit h1int with original function f1int
   h1int->Fit("f1int","q0");

   //The difference between the two histograms must be null
   h1diff->Add(h1form, h1int, 1, -1);
   Double_t hdiff = h1diff->Integral(0,101);

   //Compare fitted parameters and value of integral of f1form in [-8,6]
   Int_t npar = f1form->GetNpar();
   Double_t pdiff, pdifftot = 0;
   for (Int_t i=0;i<npar;i++) {
      pdiff = (f1form->GetParameter(i) - f1->GetParameter(i))/f1form->GetParameter(i);
      pdifftot += TMath::Abs(pdiff);
   }
   // The integral in the range [-8,6] must be = 1923.74578
   Double_t rint = TMath::Abs(f1form->Integral(-8,6) - 1923.74578);

   //Some slight differences are authorized to take into account
   //different math libraries used by the compiler, CINT and TFormula
   Bool_t OK = kTRUE;
   if (hdiff > 0.1 || pdifftot > 2.e-3 || rint > 0.1) OK = kFALSE;
   if (OK) printf("OK\n");
   else    {
      printf("failed\n");
      printf("%-8s hdiff=%g, pdifftot=%g, rint=%g\n"," ",hdiff,pdifftot,rint);
   }
   //Save all objects in a Root file (will be checked by stress2)
   TFile local("stress.root","recreate");
   f1form->Write();
   f1->Write();
   h1form->Write();
   h1int->Write();
   ntotout += local.GetBytesWritten();
   //do not close the file. should be done by the destructor automatically
}

//_______________________________________________________________
void stress2()
{
   //check length and compression factor in stress.root
   Bprint(2,"Check size & compression factor of a Root file");
   TFile f("stress.root");
   Int_t last = f.GetEND();
   Float_t comp = f.GetCompressionFactor();

   Bool_t OK = kTRUE;
   Int_t lastgood = 4913;
   if (last <lastgood-200 || last > lastgood+200 || comp <1.35 || comp > 1.41) OK = kFALSE;
   if (OK) printf("OK\n");
   else    {
      printf("failed\n");
      printf("%-8s last =%d, comp=%f\n"," ",last,comp);
   }
}

//_______________________________________________________________
void stress3()
{
   //Open stress.root, read all objects, save 10 times and purge
   //This function tests the generation and reuse of gaps in files

   Bprint(3,"Purge, Reuse of gaps in TFile");
   TFile f("stress.root","update");
   f.ReadAll();
   for (Int_t i=0;i<10;i++) {
      f.Write();
   }
   f.Purge();
   f.Write();

   //check length and compression level in stress.root
   ntotin  += f.GetBytesRead();
   ntotout += f.GetBytesWritten();
   Int_t last = f.GetEND();
   Float_t comp = f.GetCompressionFactor();
   Bool_t OK = kTRUE;
   Int_t lastgood = 43222;
   if (last <lastgood-300 || last > lastgood+300 || comp <1.37 || comp > 1.43) OK = kFALSE;
   if (OK) printf("OK\n");
   else    {
      printf("failed\n");
      printf("%-8s last =%d, comp=%f\n"," ",last,comp);
   }
}

//_______________________________________________________________
void stress4()
{
// Test of 2-d histograms, functions, 2-d fits

   Bprint(4,"Test of 2-d histograms, functions, 2-d fits");

   Double_t f2params[15] = {100,-3,3,-3,3,160,0,0.8,0,0.9,40,4,0.7,4,0.7};
   TF2 *f2form = new TF2("f2form","xygaus(0)+xygaus(5)+xygaus(10)",-10,10,-10,10);
   f2form->SetParameters(f2params);

   //Create an histogram and fill it randomly with f2form
   gRandom->SetSeed();
   TH2F *h2form = new TH2F("h2form","distribution from f2form",40,-10,10,40,-10,10);
   Int_t nentries = 100000;
   h2form->FillRandom("f2form",nentries);
   //Fit h2form with original function f2form
   Float_t ratio = 4*nentries/100000;
   f2params[ 0] *= ratio;
   f2params[ 5] *= ratio;
   f2params[10] *= ratio;
   f2form->SetParameters(f2params);
   h2form->Fit("f2form","q0");
   //Update stress.root
   TFile f("stress.root","update");
   h2form->Write();
   f2form->Write();

   ntotin  += f.GetBytesRead();
   ntotout += f.GetBytesWritten();

   //Compare results of fit with expected parameters
   Double_t th0=394.079, th5=612.216, th10=152.194;
   Double_t dp0  = TMath::Abs((f2form->GetParameter(0) -th0)/th0);
   Double_t dp5  = TMath::Abs((f2form->GetParameter(5) -th5)/th5);
   Double_t dp10 = TMath::Abs((f2form->GetParameter(10)-th10)/th10);
   Bool_t OK = kTRUE;
   if (dp0 > 1.e-2 || dp5 > 1.e-2 || dp10 > 1.e-2) OK = kFALSE;
   if (OK) printf("OK\n");
   else    {
      printf("failed\n");
      printf("%-8s dp0=%g, dp5=%g, dp10=%g\n"," ",dp0,dp5,dp10);
   }
}

//_______________________________________________________________
void stress5()
{
// Test of Postscript.
// Make a complex picture. Verify number of lines on ps file
// Testing automatically the graphics package is a complex problem.
// The best way we have found is to generate a Postscript image
// of a complex canvas containing many objects.
// The number of lines in the ps file is compared with a reference run.
// A few lines (up to 2 or 3) of difference may be expected because
// Postscript works with floats. The date and time of the run are also
// different.
// You can also inspect visually the ps file with a ps viewer.

   Bprint(5,"Test graphics & Postscript");

   TCanvas *c1 = new TCanvas("c1","stress canvas",800,600);
   gROOT->LoadClass("TPostScript","Postscript");
   TPostScript ps("stress.ps",112);

   //Get objects generated in previous test
   TFile f("stress.root");
   TF1  *f1form = (TF1*)f.Get("f1form");
   TF2  *f2form = (TF2*)f.Get("f2form");
   TH1F *h1form = (TH1F*)f.Get("h1form");
   TH2F *h2form = (TH2F*)f.Get("h2form");

   //Divide the canvas in subpads. Plot with different options
   c1->Divide(2,2);
   c1->cd(1);
   f1form->Draw();
   c1->cd(2);
   h1form->Draw();
   c1->cd(3);
   h2form->Draw("box");
   f2form->Draw("cont1same");
   c1->cd(4);
   f2form->Draw("surf");

   ps.Close();

   //count number of lines in ps file
   FILE *fp = fopen("stress.ps","r");
   char line[260];
   Int_t nlines = 0;
   Int_t nlinesGood = 952;
   while (fgets(line,255,fp)) {
      nlines++;
   }
   fclose(fp);
   ntotin  += f.GetBytesRead();
   ntotout += f.GetBytesWritten();
   Bool_t OK = kTRUE;
   if (nlines < nlinesGood-20 || nlines > nlinesGood+20) OK = kFALSE;
   if (OK) printf("OK\n");
   else    {
      printf("failed\n");
      printf("%-8s nlines in stress.ps file = %d\n"," ",nlines);
   }
}

//_______________________________________________________________
void stress6()
{
// Test subdirectories in a Root file
// Create many TH1S histograms, make operations between them

   Bprint(6,"Test subdirectories in a Root file");

   TFile f("stress.root","update");
   // create a new subdirectory for each plane
   gRandom->SetSeed();
   const Int_t nplanes = 10;
   const Int_t ncounters = 100;
   char dirname[50];
   char hname[20];
   char htitle[80];
   TH1S *hn[ncounters];
   TH1S *hs[ncounters];
   Int_t i,j,k,id;
   TH1F *hsumPlanes = new TH1F("hsumPlanes","Sum of all planes",100,0,100);
   //Create a subdirectory per detector plane
   for (i=0;i<nplanes;i++) {
      sprintf(dirname,"plane%d",i);
      TDirectory *cdplane = f.mkdir(dirname);
      if (cdplane == 0) continue;
      cdplane->cd();
      // create counter histograms
      for (j=0;j<ncounters;j++) {
         sprintf(hname,"h%d_%dN",i,j);
         sprintf(htitle,"hist for counter:%d in plane:%d North",j,i);
         hn[j] = new TH1S(hname,htitle,100,0,100);
         sprintf(hname,"h%d_%dS",i,j);
         sprintf(htitle,"hist for counter:%d in plane:%d South",j,i);
         hs[j] = new TH1S(hname,htitle,100,0,100);
      }
      // fill counter histograms randomly
      for (k=0;k<10000;k++) {
         id = Int_t(ncounters*gRandom->Rndm());
         hn[id]->Fill(gRandom->Gaus(60,10));
         hs[id]->Fill(gRandom->Gaus(40,5));
      }
      // Write all objects in directory in memory to disk
      cdplane->Write();
      // Delete all objects from memory
      cdplane->GetList()->Delete();
      f.cd();
   }
   // Now read back all objects from all subdirectories
   // Add North and south histograms in hsumPlanes
   for (i=0;i<nplanes;i++) {
      sprintf(dirname,"plane%d",i);
      f.cd(dirname);
      for (j=0;j<ncounters;j++) {
         sprintf(hname,"h%d_%dN",i,j);
         TH1S *hnorth = (TH1S*)gDirectory->Get(hname);
         sprintf(hname,"h%d_%dS",i,j);
         TH1S *hsouth = (TH1S*)gDirectory->Get(hname);
         if (hnorth == 0 || hsouth == 0) continue;
         hsumPlanes->Add(hnorth);
         hsumPlanes->Add(hsouth);
         delete hnorth; delete hsouth;
      }
      f.cd();    // change current directory to top
   }
   // Verify number of entries, rms and mean value
   ntotin  += f.GetBytesRead();
   ntotout += f.GetBytesWritten();
   Int_t nentries = (Int_t)hsumPlanes->GetEntries();
   Double_t rms   = hsumPlanes->GetRMS();
   Double_t mean  = hsumPlanes->GetMean();
   Int_t nentriesGood = 200000;
   Double_t rmsGood  = 12.745;
   Double_t meanGood = 50.01;
   Double_t diffrms  = TMath::Abs(rmsGood -rms)/rmsGood;
   Double_t diffmean = TMath::Abs(meanGood -mean)/meanGood;
   Bool_t OK = kTRUE;
   if (nentriesGood != nentries || diffrms > 1.e-2 || diffmean > 1.e-2) OK = kFALSE;
   if (OK) printf("OK\n");
   else    {
      printf("failed\n");
      printf("%-8s nentries=%d, diffmean=%g, diffrms=%g\n"," ",nentries,diffmean,diffrms);
   }
}

//_______________________________________________________________
void stress7()
{
// Test TNtuple class with several selection mechanisms
// Test expression cuts
// Test graphical cuts
// Test event lists and operations on event lists
// Compare results of TTree::Draw with results of an explict loop

   Bprint(7,"TNtuple, selections, TCut, TCutG, TEventList");

   TFile f("stress.root","update");
   // Create and fill a TNtuple
   gRandom->SetSeed();
   TNtuple *ntuple = new TNtuple("ntuple","Demo ntuple","px:py:pz:random:i");
   Float_t px, py, pz;
   Int_t nall = 50000;
   Int_t i;
   for (i = 0; i < nall; i++) {
      gRandom->Rannor(px,py);
      pz = px*px + py*py;
      Float_t random = gRandom->Rndm(1);
      ntuple->Fill(px,py,pz,random,i);
   }
   ntuple->Write();

   // Create a graphical cut. Select only events in cut
   TCutG *cutg = new TCutG("cutg",9);
   cutg->SetVarX("py");
   cutg->SetVarY("px");
   cutg->SetPoint(0,-1.75713,2.46193);
   cutg->SetPoint(1,-2.58656,-0.786802);
   cutg->SetPoint(2,-0.179195,-0.101523);
   cutg->SetPoint(3,2.12702,-1.49746);
   cutg->SetPoint(4,2.2484,1.95431);
   cutg->SetPoint(5,0.630004,0.583756);
   cutg->SetPoint(6,-0.381495,2.28426);
   cutg->SetPoint(7,-1.27161,1.01523);
   cutg->SetPoint(8,-1.75713,2.46193);
   TH2F *hpxpy = new TH2F("hpxpy","px vx py with cutg",40,-4,4,40,-4,4);
   ntuple->Draw("px:py>>hpxpy","cutg","goff");
   Int_t npxpy = (Int_t)hpxpy->GetEntries();
   Int_t npxpyGood = 28037;
   hpxpy->Write();
   cutg->Write();

   // Fill a TEventList using the standard cut
   ntuple->Draw(">>elist","py<0 && pz>4 && random<0.5","goff");
   TEventList *elist = (TEventList*)gDirectory->Get("elist");
   // Fill hist htemp using the standard cut
   ntuple->Draw("px>>htemp0","py<0 && pz>4 && random<0.5","goff");
   TH1F *htemp0 = (TH1F*)gDirectory->Get("htemp0");
   Double_t pxmean0 = htemp0->GetMean();
   Double_t pxrms0  = htemp0->GetRMS();

   // Fill hist hcut using a TCut = the standard cut
   TCut cut1 = "py<0 && pz>4 && random<0.5";
   TCut vcut = "px>>hcut";
   ntuple->Draw(vcut,cut1,"goff");
   // Fill hist helist looping on the eventlist in TTree::Draw
   ntuple->SetEventList(elist);
   ntuple->Draw("px>>helist","","goff");
   ntuple->SetEventList(0);
   TH1F *hcut   = (TH1F*)gDirectory->Get("hcut");
   TH1F *helist = (TH1F*)gDirectory->Get("helist");
   Int_t n1 = (Int_t)hcut->GetEntries();
   Int_t n2 = (Int_t)helist->GetEntries();
   htemp0->Write();
   cut1.Write();
   helist->Write();
   hcut->Write();

   // now loop on eventlist explicitely and fill helist again
   Float_t pxr;
   ntuple->SetBranchAddress("px",&pxr);
   TH1F *helistc = (TH1F*)helist->Clone();
   helistc->Reset();
   helistc->SetName("helistc");
   Int_t nlist = elist->GetN();
   for (i=0;i<nlist;i++) {
      Int_t event = elist->GetEntry(i);
      ntuple->GetEntry(event);
      helistc->Fill(pxr);
   }
   Int_t n3 = (Int_t)helistc->GetEntries();
   Double_t pxmean2 = helistc->GetMean();
   Double_t pxrms2  = helistc->GetRMS();
   helistc->Write();
   elist->Write();

   // Generate several TEventlist objects + total and save them
   char elistname[20];
   char cutname[20];
   TEventList *el[10];
   TEventList elistall("elistall","Sum of all cuts");
   for (i=0;i<10;i++) {
      sprintf(elistname,">>elist%d",i);
      sprintf(cutname,"i 10 == %d",i); cutname[1] ='%';
      ntuple->Draw(elistname,cutname,"goff");
      el[i] = (TEventList*)gDirectory->Get(&elistname[2]);
      el[i]->Write();
      elistall.Add(el[i]);
   }
   elistall.Write();

   // Read big list from file and check that the distribution with the list
   // correspond to all events (no cuts)
   delete ntuple;
   TNtuple *nt = (TNtuple*)gDirectory->Get("ntuple");
   nt->SetBranchAddress("px",&pxr);
   TH1F *hpx = new TH1F("hpx","hpx",100,-3,3);
   nt->Draw("px>>hpx","","goff");
   TEventList *all = (TEventList*)gDirectory->Get("elistall");
   nt->SetEstimate(nall); //must be done because the order in eventlist is different
   nt->SetEventList(all);
   TH1F *hall = (TH1F*)hpx->Clone();
   hall->SetName("hall");
   nt->Draw("px>>hall","","goff");
   // Take the difference between the two histograms. Must be empty
   TH1F hcomp = (*hall) - (*hpx);
   Double_t compsum = hcomp.GetSum();
   ntotin  += f.GetBytesRead();
   ntotout += f.GetBytesWritten();

   // We can compare entries, means and rms
   Bool_t OK = kTRUE;
   if (n1 != n2 || n1 != n3 || n3 != nlist || nall !=elistall.GetN()
                || npxpy != npxpyGood
                || compsum != 0
                || TMath::Abs(pxmean0-pxmean2) > 0.1
                || TMath::Abs(pxrms0-pxrms2) > 0.01) OK = kFALSE;
   if (OK) printf("OK\n");
   else    {
      printf("failed\n");
      printf("%-8s n1=%d, n2=%d, n3=%d, elistallN=%d\n"," ",n1,n2,n3,elistall.GetN());
      printf("%-8s pxmean0=%g, pxmean2=%g, pxrms0=%g\n"," ",pxmean0,pxmean2,pxrms0);
      printf("%-8s pxrms2=%g, compsum=%g, npxpy=%d\n"," ",pxrms2,compsum,npxpy);
   }
}

//_______________________________________________________________
Int_t stress8read(Int_t nevent)
{
//  Read the event file
//  Loop on all events in the file (reading everything).
//  Count number of bytes read

   TFile *hfile = new TFile("Event.root");
   TTree *tree = (TTree*)hfile->Get("T");
   Event *event = 0;
   tree->SetBranchAddress("event",&event);
   Int_t nentries = (Int_t)tree->GetEntries();
   Int_t nev = TMath::Max(nevent,nentries);
   Int_t nb = 0;
   for (Int_t ev = 0; ev < nev; ev++) {
      nb += tree->GetEntry(ev);        //read complete event in memory
   }
   ntotin  += hfile->GetBytesRead();

   delete event;
   delete hfile;
   return nb;
}


//_______________________________________________________________
Int_t stress8write(Int_t nevent, Int_t comp, Int_t split)
{
//  Create the Event file in various modes
   // comp = compression level
   // split = 1 split mode, 0 = no split

   // Create the Event file, the Tree and the branches
   TFile *hfile = new TFile("Event.root","RECREATE","TTree benchmark ROOT file");
   hfile->SetCompressionLevel(comp);

   // Create one event
   Event *event = new Event();

   // Create a ROOT Tree and one superbranch
   TTree *tree = new TTree("T","An example of a ROOT tree");
   tree->SetAutoSave(100000000);  // autosave when 100 Mbytes written
   Int_t bufsize = 64000;
   if (split)  bufsize /= 4;
   tree->Branch("event", "Event", &event, bufsize,split);

   //Fill the Tree
   Int_t ev, t, ntrack, nb=0, meanTracks=600;
   for (ev = 0; ev < nevent; ev++) {
      Float_t sigmat, sigmas;
      gRandom->Rannor(sigmat,sigmas);
      ntrack   = Int_t(meanTracks +meanTracks*sigmat/120.);
      Float_t random = gRandom->Rndm(1);

      event->SetHeader(ev, 200, 960312, random);
      event->SetNseg(Int_t(10*ntrack+20*sigmas));
      event->SetNvertex(1);
      event->SetFlag(UInt_t(random+0.5));
      event->SetTemperature(random+20.);

      //  Create and Fill the Track objects
      for (t = 0; t < ntrack; t++) event->AddTrack(random);

      nb += tree->Fill();  //fill the tree

      event->Clear();
   }
   hfile->Write();
   ntotout += hfile->GetBytesWritten();
   delete event;
   delete hfile;
   return nb;
}


//_______________________________________________________________
void stress8(Int_t nevent)
{
//  Run the $ROOTSYS/test/Event program in several configurations.

   Bprint(8,"Trees split and compression modes");

  // First step: make sure the Event shared library exists
  // This test dynamic linking when running in interpreted mode
   if (!TClassTable::GetDict("Event")) {
      Bool_t UNIX = strcmp(gSystem->GetName(), "Unix") == 0;
      Int_t st1 = gSystem->Load("$(ROOTSYS)/test/libEvent");
      if (st1 == -1) {
         printf("===>stress8 will try to build the libEvent library\n");
         if (UNIX) gSystem->Exec("(cd $ROOTSYS/test; make Event)");
         else      gSystem->Exec("(cd %ROOTSYS%\\test && nmake libEvent.dll)");
         st1 = gSystem->Load("$(ROOTSYS)/test/libEvent");
      }
   }

   // Create the file not compressed, in no-split mode and read it back
   gRandom->SetSeed();
   Int_t nbw0 = stress8write(100,0,0);
   Int_t nbr0 = stress8read(0);
   Event::Reset();

   // Create the file compressed, in no-split mode and read it back
   gRandom->SetSeed();
   Int_t nbw1 = stress8write(100,1,0);
   Int_t nbr1 = stress8read(0);
   Event::Reset();

   // Create the file compressed, in split mode and read it back
   gRandom->SetSeed();
   Int_t nbw2 = stress8write(nevent,1,1);
   Int_t nbr2 = stress8read(0);
   Event::Reset();

   Bool_t OK = kTRUE;
   if (nbw0 != nbr0 || nbw1 != nbr1 || nbw2 != nbr2) OK = kFALSE;
   if (nbw0 != nbw1) OK = kFALSE;
   if (OK) printf("OK\n");
   else    {
      printf("failed\n");
      printf("%-8s nbw0=%d, nbr0=%d, nbw1=%d\n"," ",nbw0,nbr0,nbw1);
      printf("%-8s nbr1=%d, nbw2=%d, nbr2=%d\n"," ",nbr1,nbw2,nbr2);
   }
}

//_______________________________________________________________
Int_t HistCompare(TH1 *h1, TH1 *h2)
{
// Compare histograms h1 and h2
// Check number of entries, mean and rms
// if means differ by more than 1/1000 of the range return -1
// if rms differs in percent by more than 1/1000 return -2
// Otherwise return difference of number of entries

   Int_t n1       = (Int_t)h1->GetEntries();
   Double_t mean1 = h1->GetMean();
   Double_t rms1  = h1->GetRMS();
   Int_t n2       = (Int_t)h2->GetEntries();
   Double_t mean2 = h2->GetMean();
   Double_t rms2  = h2->GetRMS();
   Float_t xrange = h1->GetXaxis()->GetXmax() - h1->GetXaxis()->GetXmin();
   if (TMath::Abs((mean1-mean2)/xrange) > 0.001*xrange) return -1;
   if (TMath::Abs((rms1-rms2)/rms1) > 0.001) return -2;
   return n1-n2;
}

//_______________________________________________________________
void stress9tree(TTree *tree)
{
// Test selections via TreeFormula
// tree is a TTree when called by stress9
// tree is a TChain when called from stres11
// This is a quite complex test checking the results of TTree::Draw
// or TChain::Draw with an explicit loop on events.
// Also a good test for the interpreter

   Event *event = new Event();
   tree->SetBranchAddress("event",&event);
   gROOT->cd();
   TDirectory *hfile = gDirectory;
   Double_t nrsave = TFile::GetFileBytesRead();

   // Each tree->Draw generates an histogram
   tree->Draw("fNtrack>>hNtrack",    "","goff");
   tree->Draw("fNseg>>hNseg",        "","goff");
   tree->Draw("fTemperature>>hTemp", "","goff");
   tree->Draw("fH.GetMean()>>hHmean","","goff");
   tree->Draw("fTracks.fPx>>hPx","fEvtHdr.fEvtNum%10 == 0","goff");
   tree->Draw("fTracks.fPy>>hPy","fEvtHdr.fEvtNum%10 == 0","goff");
   tree->Draw("fTracks.fPz>>hPz","fEvtHdr.fEvtNum%10 == 0","goff");
   tree->Draw("fRandom>>hRandom","fEvtHdr.fEvtNum%10 == 1","goff");
   tree->Draw("fMass2>>hMass2",  "fEvtHdr.fEvtNum%10 == 1","goff");
   tree->Draw("fBx>>hBx",        "fEvtHdr.fEvtNum%10 == 1","goff");
   tree->Draw("fBy>>hBy",        "fEvtHdr.fEvtNum%10 == 1","goff");
   tree->Draw("fXfirst>>hXfirst","fEvtHdr.fEvtNum%10 == 2","goff");
   tree->Draw("fYfirst>>hYfirst","fEvtHdr.fEvtNum%10 == 2","goff");
   tree->Draw("fZfirst>>hZfirst","fEvtHdr.fEvtNum%10 == 2","goff");
   tree->Draw("fXlast>>hXlast",  "fEvtHdr.fEvtNum%10 == 3","goff");
   tree->Draw("fYlast>>hYlast",  "fEvtHdr.fEvtNum%10 == 3","goff");
   tree->Draw("fZlast>>hZlast",  "fEvtHdr.fEvtNum%10 == 3","goff");
   tree->Draw("fCharge>>hCharge","fPx < 0","goff");
   tree->Draw("fNpoint>>hNpoint","fPx < 0","goff");
   tree->Draw("fValid>>hValid",  "fPx < 0","goff");
   ntotin  += TFile::GetFileBytesRead() -nrsave;

   //Get pointers to the histograms generated above
   TH1F *hNtrack = (TH1F*)hfile->Get("hNtrack");
   TH1F *hNseg   = (TH1F*)hfile->Get("hNseg");
   TH1F *hTemp   = (TH1F*)hfile->Get("hTemp");
   TH1F *hHmean  = (TH1F*)hfile->Get("hHmean");
   TH1F *hPx     = (TH1F*)hfile->Get("hPx");
   TH1F *hPy     = (TH1F*)hfile->Get("hPy");
   TH1F *hPz     = (TH1F*)hfile->Get("hPz");
   TH1F *hRandom = (TH1F*)hfile->Get("hRandom");
   TH1F *hMass2  = (TH1F*)hfile->Get("hMass2");
   TH1F *hBx     = (TH1F*)hfile->Get("hBx");
   TH1F *hBy     = (TH1F*)hfile->Get("hBy");
   TH1F *hXfirst = (TH1F*)hfile->Get("hXfirst");
   TH1F *hYfirst = (TH1F*)hfile->Get("hYfirst");
   TH1F *hZfirst = (TH1F*)hfile->Get("hZfirst");
   TH1F *hXlast  = (TH1F*)hfile->Get("hXlast");
   TH1F *hYlast  = (TH1F*)hfile->Get("hYlast");
   TH1F *hZlast  = (TH1F*)hfile->Get("hZlast");
   TH1F *hCharge = (TH1F*)hfile->Get("hCharge");
   TH1F *hNpoint = (TH1F*)hfile->Get("hNpoint");
   TH1F *hValid  = (TH1F*)hfile->Get("hValid");

   //We make clones of the generated histograms
   //We set new names and reset the clones.
   //We want to have identical histogram limits
   TH1F *bNtrack = (TH1F*)hNtrack->Clone(); bNtrack->SetName("bNtrack"); bNtrack->Reset();
   TH1F *bNseg   = (TH1F*)hNseg->Clone();   bNseg->SetName("bNseg");     bNseg->Reset();
   TH1F *bTemp   = (TH1F*)hTemp->Clone();   bTemp->SetName("bTemp");     bTemp->Reset();
   TH1F *bHmean  = (TH1F*)hHmean->Clone();  bHmean->SetName("bHmean");   bHmean->Reset();
   TH1F *bPx     = (TH1F*)hPx->Clone();     bPx->SetName("bPx");         bPx->Reset();
   TH1F *bPy     = (TH1F*)hPy->Clone();     bPy->SetName("bPy");         bPy->Reset();
   TH1F *bPz     = (TH1F*)hPz->Clone();     bPz->SetName("bPz");         bPz->Reset();
   TH1F *bRandom = (TH1F*)hRandom->Clone(); bRandom->SetName("bRandom"); bRandom->Reset();
   TH1F *bMass2  = (TH1F*)hMass2->Clone();  bMass2->SetName("bMass2");   bMass2->Reset();
   TH1F *bBx     = (TH1F*)hBx->Clone();     bBx->SetName("bBx");         bBx->Reset();
   TH1F *bBy     = (TH1F*)hBy->Clone();     bBy->SetName("bBy");         bBy->Reset();
   TH1F *bXfirst = (TH1F*)hXfirst->Clone(); bXfirst->SetName("bXfirst"); bXfirst->Reset();
   TH1F *bYfirst = (TH1F*)hYfirst->Clone(); bYfirst->SetName("bYfirst"); bYfirst->Reset();
   TH1F *bZfirst = (TH1F*)hZfirst->Clone(); bZfirst->SetName("bZfirst"); bZfirst->Reset();
   TH1F *bXlast  = (TH1F*)hXlast->Clone();  bXlast->SetName("bXlast");   bXlast->Reset();
   TH1F *bYlast  = (TH1F*)hYlast->Clone();  bYlast->SetName("bYlast");   bYlast->Reset();
   TH1F *bZlast  = (TH1F*)hZlast->Clone();  bZlast->SetName("bZlast");   bZlast->Reset();
   TH1F *bCharge = (TH1F*)hCharge->Clone(); bCharge->SetName("bCharge"); bCharge->Reset();
   TH1F *bNpoint = (TH1F*)hNpoint->Clone(); bNpoint->SetName("bNpoint"); bNpoint->Reset();
   TH1F *bValid  = (TH1F*)hValid->Clone();  bValid->SetName("bValid");   bValid->Reset();

   // Loop with user code on all events and fill the b histograms
   // The code below should produce identical results to the tree->Draw above

   TClonesArray *tracks = event->GetTracks();
   Int_t nev = (Int_t)tree->GetEntries();
   Int_t i, ntracks, evmod;
   Track *t;
   EventHeader *head;
   Int_t nbin = 0;
   for (Int_t ev=0;ev<nev;ev++) {
      nbin += tree->GetEntry(ev);
      head = event->GetHeader();
      evmod = head->GetEvtNum()%10;
      bNtrack->Fill(event->GetNtrack());
      bNseg->Fill(event->GetNseg());
      bTemp->Fill(event->GetTemperature());
      bHmean->Fill(event->GetHistogram()->GetMean());
      ntracks = event->GetNtrack();
      for (i=0;i<ntracks;i++) {
         t = (Track*)tracks->UncheckedAt(i);
         if (evmod == 0) bPx->Fill(t->GetPx());
         if (evmod == 0) bPy->Fill(t->GetPy());
         if (evmod == 0) bPz->Fill(t->GetPz());
         if (evmod == 1) bRandom->Fill(t->GetRandom());
         if (evmod == 1) bMass2->Fill(t->GetMass2());
         if (evmod == 1) bBx->Fill(t->GetBx());
         if (evmod == 1) bBy->Fill(t->GetBy());
         if (evmod == 2) bXfirst->Fill(t->GetXfirst());
         if (evmod == 2) bYfirst->Fill(t->GetYfirst());
         if (evmod == 2) bZfirst->Fill(t->GetZfirst());
         if (evmod == 3) bXlast->Fill(t->GetXlast());
         if (evmod == 3) bYlast->Fill(t->GetYlast());
         if (evmod == 3) bZlast->Fill(t->GetZlast());
         if (t->GetPx() < 0) {
            bCharge->Fill(t->GetCharge());
            bNpoint->Fill(t->GetNpoint());
            bValid->Fill(t->GetValid());
         }
      }
   }

   // Compare h and b histograms
   Int_t cNtrack = HistCompare(hNtrack,bNtrack);
   Int_t cNseg   = HistCompare(hNseg,bNseg);
   Int_t cTemp   = HistCompare(hTemp,bTemp);
   Int_t cHmean  = HistCompare(hHmean,bHmean);
   Int_t cPx     = HistCompare(hPx,bPx);
   Int_t cPy     = HistCompare(hPy,bPy);
   Int_t cPz     = HistCompare(hPz,bPz);
   Int_t cRandom = HistCompare(hRandom,bRandom);
   Int_t cMass2  = HistCompare(hMass2,bMass2);
   Int_t cBx     = HistCompare(hBx,bBx);
   Int_t cBy     = HistCompare(hBy,bBy);
   Int_t cXfirst = HistCompare(hXfirst,bXfirst);
   Int_t cYfirst = HistCompare(hYfirst,bYfirst);
   Int_t cZfirst = HistCompare(hZfirst,bZfirst);
   Int_t cXlast  = HistCompare(hXlast,bXlast);
   Int_t cYlast  = HistCompare(hYlast,bYlast);
   Int_t cZlast  = HistCompare(hZlast,bZlast);
   Int_t cCharge = HistCompare(hCharge,bCharge);
   Int_t cNpoint = HistCompare(hNpoint,bNpoint);
   Int_t cValid  = HistCompare(hValid,bValid);

   delete event;
   Event::Reset();
   ntotin += nbin;

   Bool_t OK = kTRUE;
   if (cNtrack || cNseg   || cTemp  || cHmean || cPx    || cPy     || cPz) OK = kFALSE;
   if (cRandom || cMass2  || cBx    || cBy    || cXfirst|| cYfirst || cZfirst) OK = kFALSE;
   if (cXlast  || cYlast  || cZlast || cCharge|| cNpoint|| cValid) OK = kFALSE;
   if (OK) printf("OK\n");
   else    {
      printf("failed\n");
      printf("%-8s cNtrak =%d, cNseg  =%d, cTemp  =%d, cHmean =%d\n"," ",cNtrack,cNseg,cTemp,cHmean);
      printf("%-8s cPx    =%d, cPy    =%d, cPz    =%d, cRandom=%d\n"," ",cPx,cPy,cPz,cRandom);
      printf("%-8s cMass2 =%d, cbx    =%d, cBy    =%d, cXfirst=%d\n"," ",cMass2,cBx,cBy,cXfirst);
      printf("%-8s cYfirst=%d, cZfirst=%d, cXlast =%d, cYlast =%d\n"," ",cYfirst,cZfirst,cXlast,cYlast);
      printf("%-8s cZlast =%d, cCharge=%d, cNpoint=%d, cValid =%d\n"," ",cZlast,cCharge,cNpoint,cValid);
   }
}

//_______________________________________________________________
void stress9()
{
// Analyse the file Event.root generated in the last part of test8

   Bprint(9,"Analyze Event.root file of stress 8");

   gROOT->GetList()->Delete();
   TFile *hfile = new TFile("Event.root");
   TTree *tree = (TTree*)hfile->Get("T");

   stress9tree(tree);

   // Save test9 histograms
   delete tree;
   TFile f("stress_test9.root","recreate");
   gROOT->GetList()->Write();
   gROOT->GetList()->Delete();
   ntotout += f.GetBytesWritten();


   delete hfile;
}

//_______________________________________________________________
void stress10()
{
// Make 10 Trees starting from the Event.root tree.
// Events for which event_number%10 == 0 go to Event_0.root
// Events for which event_number%10 == 1 go to Event_1.root
//...
// Events for which event_number%10 == 9 go to Event_9.root

   Bprint(10,"Create 10 files starting from Event.root");

   TFile *hfile = new TFile("Event.root");
   TTree *tree = (TTree*)hfile->Get("T");

   Event *event = new Event();
   tree->SetBranchAddress("event",&event);

   // Create 10 clones of this tree
   char filename[20];
   TTree *chTree[10];
   TFile *chfile[10];
   Int_t file;
   for (file=0;file<10;file++) {
      sprintf(filename,"Event_%d.root",file);
      chfile[file] = new TFile(filename,"recreate");
      chTree[file] = (TTree*)tree->CloneTree(0);
      chTree[file]->SetBranchAddress("event",&event);
   }

   // Fill the small trees
   Int_t nev = (Int_t)tree->GetEntries();
   Int_t evmod, nbin=0, nbout=0;
   EventHeader *head;
   for (Int_t ev=0;ev<nev;ev++) {
      nbin +=tree->GetEntry(ev);
      head = event->GetHeader();
      evmod = head->GetEvtNum()%10;
      nbout += chTree[evmod]->Fill();
      event->Clear();
   }
   // save headers
   Int_t ntot = 0;
   for (file=0;file<10;file++) {
      ntot += (Int_t)chTree[file]->GetEntries();
      chfile[file]->Write();
      delete chfile[file];
   }
   event->ResetHistogramPointer(); // fH was deleted above!!
   delete event;
   delete hfile;
   Event::Reset();
   ntotin  += nbin;
   ntotout += nbout;

   //We compare the number of bytes read from the big file
   //with the total number of bytes written in the 10 small files
   Bool_t OK = kTRUE;
   if (nbin != nbout || nev != ntot) OK = kFALSE;
   if (OK) printf("OK\n");
   else    {
      printf("failed\n");
      printf("%-8s nbin=%d, nbout=%d, nev=%d, ntot=%d\n"," ",nbin,nbout,nev,ntot);
   }
}

//_______________________________________________________________
void stress11()
{
// Test chains of Trees
// We make a TChain using the 10 files generated in test10
// We expect the same results when analyzing the chain than
// in the analysis of the original big file Event.root in test9.
// Because TChain derives from TTree, we can use the same
// analysis procedure "stress9tree"

   Bprint(11,"Test chains of Trees using the 10 files");

   gROOT->GetList()->Delete();
   TChain *chain = new TChain("T");
   char filename[20];
   Int_t file;
   for (file=0;file<10;file++) {
      sprintf(filename,"Event_%d.root",file);
      chain->Add(filename);
   }

   stress9tree(chain);

   // Save test11 histograms
   delete chain;
   TFile f("stress_test11.root","recreate");
   gROOT->GetList()->Write();
   gROOT->GetList()->Delete();
   ntotout += f.GetBytesWritten();
}

//_______________________________________________________________
void stress12(Int_t testid)
{
// Compare histograms of stress9 with stress11

   if (testid == 12) Bprint(12,"Compare histograms of test 9 and 11");

   TFile f9("stress_test9.root");
   TFile f11("stress_test11.root");
   //Let's loop on all keys of second file
   //We expect to find the same keys in the original stress9 file
   TIter next(f11.GetListOfKeys());
   TKey *key;
   TH1F *h9, *h11;
   Int_t comp, ngood = 0;
   while ((key=(TKey*)next())) {
      h9  = (TH1F*)f9.Get(key->GetName());
      h11 = (TH1F*)f11.Get(key->GetName());
      if (h9 == 0 || h11 == 0) continue;
      comp = HistCompare(h9,h11);
      if (comp == 0) ngood++;
   }
   ntotin += f9.GetBytesRead();
   ntotin += f11.GetBytesRead();
   Bool_t OK = kTRUE;
   if (ngood < 40) OK = kFALSE;
   if (OK) printf("OK\n");
   else    {
      printf("failed\n");
      printf("%-8s ngood=%d\n"," ",ngood);
   }
}

//_______________________________________________________________
void stress13()
{
// test of TChain::Merge
// The 10 small Tree files generated in stress10 are again merged
// into one single file.
// Should be the same as the file generated in stress8, except
// that events will be in a different order.
// But global analysis histograms should be identical (checked by stress14)

   Bprint(13,"Test merging files of a chain");

   gROOT->GetList()->Delete();
   TChain *chain = new TChain("T");
   char filename[20];
   Int_t file;
   for (file=0;file<10;file++) {
      sprintf(filename,"Event_%d.root",file);
      chain->Add(filename);
   }

   Event *event = new Event();
   chain->SetBranchAddress("event",&event);

   chain->Merge("Event.root");

   event->ResetHistogramPointer(); // fH was deleted above!!
   delete event;
   Event::Reset();
   gROOT->GetList()->Delete();

   TFile f("Event.root");
   TTree *tree = (TTree*)f.Get("T");
   ntotin  += f.GetEND();
   ntotout += f.GetEND();

   Bool_t OK = kTRUE;
   if (chain->GetEntries() != tree->GetEntries()) OK = kFALSE;
   delete tree;
   delete chain;
   if (OK) printf("OK\n");
   else    {
      printf("failed\n");
   }
}

//_______________________________________________________________
void stress14()
{
// Verify that stress13 has correctly rebuild the original Event.root

   Bprint(14,"Check correct rebuilt of Event.root in test 13");

   stress12(14);
}

//_______________________________________________________________
void stress15()
{
// Divert some branches to separate files

   Bprint(15,"Divert Tree branches to separate files");

   //Get old file, old tree and set top branch address
   //We want to copy only a few branches.
   TFile *oldfile = new TFile("Event.root");
   TTree *oldtree = (TTree*)oldfile->Get("T");
   Event *event   = new Event();
   oldtree->SetBranchAddress("event",&event);
   oldtree->SetBranchStatus("*",0);
   oldtree->SetBranchStatus("event",1);
   oldtree->SetBranchStatus("fNtrack",1);
   oldtree->SetBranchStatus("fNseg",1);
   oldtree->SetBranchStatus("fH",1);


   //Create a new file + a clone of old tree header. Do not copy events
   TFile *newfile = new TFile("stress_small.root","recreate");
   TTree *newtree = oldtree->CloneTree(0);

   //Divert branch fH to a separate file and copy all events
   newtree->GetBranch("fH")->SetFile("stress_fH.root");
   newtree->CopyEntries(oldtree);

   newfile->Write();
   ntotin  += oldfile->GetBytesRead();
   ntotout += newfile->GetBytesWritten();
   delete event;
   delete newfile;
   delete oldfile;
   Event::Reset();
   gROOT->GetList()->Delete();

   // Open small file, histogram fNtrack and fH
   newfile = new TFile("stress_small.root");
   newtree = (TTree*)newfile->Get("T");
   newtree->Draw("fNtrack>>hNtrack","","goff");
   newtree->Draw("fH.GetMean()>>hHmean","","goff");
   TH1F *hNtrack = (TH1F*)newfile->Get("hNtrack");
   TH1F *hHmean  = (TH1F*)newfile->Get("hHmean");
   ntotin  += newfile->GetBytesRead();

   // Open old reference file of stress9
   oldfile = new TFile("stress_test9.root");
   TH1F *bNtrack = (TH1F*)oldfile->Get("bNtrack");
   TH1F *bHmean  = (TH1F*)oldfile->Get("bHmean");
   Int_t cNtrack = HistCompare(hNtrack,bNtrack);
   Int_t cHmean  = HistCompare(hHmean, bHmean);
   delete newfile;
   delete oldfile;
   gROOT->GetList()->Delete();

   Bool_t OK = kTRUE;
   if (cNtrack || cHmean) OK = kFALSE;
   if (OK) printf("OK\n");
   else    {
      printf("failed\n");
      printf("%-8s cNtrack=%d, cHmean=%d\n"," ",cNtrack,cHmean);
   }
}

void stress16()
{
// Prototype trigger simulation for the LHCb experiment
// This test nested loops with the interpreter.
// Expected to run fast with the compiler, slow with the interpreter.
// This code is extracted from an original macro by Hans Dijkstra (LHCb)
// The program generates histograms and profile histograms.
// A canvas with subpads containing the results is sent to Postscript.
// We check graphics results by counting the number of lines in the ps file.

   Bprint(16,"CINT test (3 nested loops) with LHCb trigger");

   const int nbuf    = 153;    // buffer size
   const int nlev    = 4;      // number of trigger levels
   const int nstep   = 50000;  // number of steps
   const int itt[4]  = { 1000, 4000, 40000, 400000 }; // time needed per trigger
   const float a[4]  = { 0.25, 0.04, 0.25, 0 };       // acceptance/trigger (last always 0)

   int   i, il, istep, itim[192], itrig[192], it, im, ipass;
   float dead, sum[10];

   // create histogram and array of profile histograms
   TCanvas *c = new TCanvas("laten","latency simulation",700,600);
   gROOT->LoadClass("TPostScript","Postscript");
   TPostScript ps("stress_lhcb.ps",112);
   gRandom->SetSeed();
   TFile f("stress_lhcb.root", "recreate");
   TH1F *pipe = new TH1F("pipe", "free in pipeline", nbuf+1, -0.5, nbuf+0.5);
   pipe->SetLineColor(2);
   pipe->SetFillColor(2);

   TProfile *hp[nlev+1];
   for (i = 0; i <= nlev; i++) {
      char s[64];
      sprintf(s, "buf%d", i);
      hp[i] = new TProfile(s, "in buffers", 1000, 0,nstep, -1., 1000.);
      hp[i]->SetLineColor(2);
   }

   dead   = 0;
   sum[0] = nbuf;
   for (i = 1; i <= nlev; i++) sum[i] = 0;
   for (i = 0; i < nbuf; i++) { itrig[i] = 0; itim[i] = 0; }

   for (istep = 0; istep < nstep; istep++) {
      // evaluate status of buffer
      pipe->Fill(sum[0]);
      if ((istep+1)%10 == 0) {
         for (i = 0; i <= nlev; i++)
            hp[i]->Fill((float)istep, sum[i], 1.);
      }

      ipass = 0;
      for (i = 0; i < nbuf; i++) {
         it = itrig[i];
         if (it >= 1) {
            // add 25 ns to all times
            itim[i] += 25;
            im = itim[i];
            // level decisions
            for (il = 0; il < nlev; il++) {
               if (it == il+1 && im > itt[il]) {
                  if (gRandom->Rndm() > a[il]) {
                     itrig[i] = -1;
                     sum[0]++;
                     sum[il+1]--;
                  } else {
                     itrig[i]++;
                     sum[il+1]--;
                     sum[il+2]++;
                  }
               }
            }
         } else if (ipass == 0) {
            itrig[i] = 1;
            itim[i]  = 25;
            sum[0]--;
            sum[1]++;
            ipass++;
         }
      }
      if (ipass == 0) dead++;
   }
//   Float_t deadTime = 100.*dead/nstep;

   // View results in the canvas and make the Postscript file

   c->Divide(2,3);
   c->cd(1); pipe->Draw();
   c->cd(2); hp[0]->Draw();
   c->cd(3); hp[1]->Draw();
   c->cd(4); hp[2]->Draw();
   c->cd(5); hp[3]->Draw();
   c->cd(6); hp[4]->Draw();
   ps.Close();

   f.Write();
   ntotout += f.GetBytesWritten();

   // Check length of Postscript file
   FILE *fp = fopen("stress_lhcb.ps","r");
   char line[260];
   Int_t nlines = 0;
   Int_t nlinesGood = 4023;
   while (fgets(line,255,fp)) {
      nlines++;
   }
   fclose(fp);
   delete c;
   Bool_t OK = kTRUE;
   if (nlines < nlinesGood-10 || nlines > nlinesGood+10) OK = kFALSE;
   if (OK) printf("OK\n");
   else    {
      printf("failed\n");
      printf("%-8s nlines in stress_lhcb.ps file = %d\n"," ",nlines);
   }
}

void cleanup()
{
   gSystem->Unlink("Event.root");
   gSystem->Unlink("Event_0.root");
   gSystem->Unlink("Event_1.root");
   gSystem->Unlink("Event_2.root");
   gSystem->Unlink("Event_3.root");
   gSystem->Unlink("Event_4.root");
   gSystem->Unlink("Event_5.root");
   gSystem->Unlink("Event_6.root");
   gSystem->Unlink("Event_7.root");
   gSystem->Unlink("Event_8.root");
   gSystem->Unlink("Event_9.root");
   gSystem->Unlink("stress.ps");
   gSystem->Unlink("stress.root");
   gSystem->Unlink("stress_fH.root");
   gSystem->Unlink("stress_lhcb.ps");
   gSystem->Unlink("stress_lhcb.root");
   gSystem->Unlink("stress_small.root");
   gSystem->Unlink("stress_test9.root");
   gSystem->Unlink("stress_test11.root");
}
