// @(#)root/graf:$Name:  $:$Id: TGraphBentErrors.cxx,v 1.11 2005/02/07 14:34:47 brun Exp $
// Author: Dave Morrison  30/06/2003

/*************************************************************************
 * Copyright (C) 1995-2003, Rene Brun and Fons Rademakers.               *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $ROOTSYS/LICENSE.                         *
 * For the list of contributors see $ROOTSYS/README/CREDITS.             *
 *************************************************************************/

#include <string.h>

#include "Riostream.h"
#include "TROOT.h"
#include "TGraphBentErrors.h"
#include "TStyle.h"
#include "TMath.h"
#include "TArrow.h"
#include "TBox.h"
#include "TVirtualPad.h"
#include "TF1.h"

ClassImp(TGraphBentErrors)

//______________________________________________________________________________
//
//   A TGraphBentErrors is a TGraph with bent, assymetric error bars.
//   The various format options to draw a TGraphBentErrors are explained in
//     TGraphBentErrors::Paint.
//
//  The picture below has been generated by the following macro:
//  FIXME There are 2 bent error bars in the picture, not one! 
//------------------------------------------------------------------------
//{
//   gROOT->Reset();
//
//   Int_t n = 10;
//   Double_t x[n]  = {-0.22, 0.05, 0.25, 0.35, 0.5, 0.61,0.7,0.85,0.89,0.95};
//   Double_t y[n]  = {1,2.9,5.6,7.4,9,9.6,8.7,6.3,4.5,1};
//   Double_t exl[n] = {.05,.1,.07,.07,.04,.05,.06,.07,.08,.05};
//   Double_t eyl[n] = {.8,.7,.6,.5,.4,.4,.5,.6,.7,.8};
//   Double_t exh[n] = {.02,.08,.05,.05,.03,.03,.04,.05,.06,.03};
//   Double_t eyh[n] = {.6,.5,.4,.3,.2,.2,.3,.4,.5,.6};
//   Double_t exld[n] = {.0,.0,.0,.0,.0,.0,.0,.0,.0,.0};
//   Double_t eyld[n] = {.0,.0,.05,.0,.0,.0,.0,.0,.0,.0};
//   Double_t exhd[n] = {.0,.0,.0,.0,.0,.0,.0,.0,.0,.0};
//   Double_t eyhd[n] = {.0,.0,.0,.0,.0,.0,.0,.0,.05,.0};
//   gr = new TGraphBentErrors(n,x,y,exl,exh,eyl,eyh,exld,exhd,eyld,eyhd);
//   gr->SetTitle("TGraphBentErrors Example");
//   gr->SetMarkerColor(4);
//   gr->SetMarkerStyle(21);
//   gr->Draw("ALP");
//}
//Begin_Html
/*
<img src="gif/benterrors.gif">
*/
//End_Html
//

//_____________________________________________________________________________
TGraphBentErrors::TGraphBentErrors(): TGraph()
{
//*-*-*-*-*-*-*-*-*-*-*TGraphBentErrors default constructor*-*-*-*-*-*-*-*-*-*
//*-*                  =====================================
   CtorAllocate();
}



//______________________________________________________________________________
TGraphBentErrors::TGraphBentErrors(const TGraphBentErrors &gr)
       : TGraph(gr)
{
// TGraphBentErrors copy constructor 
   
   if (!CtorAllocate()) return;
   Int_t n = fNpoints*sizeof(Double_t);
   memcpy(fEXlow, gr.fEXlow, n);
   memcpy(fEYlow, gr.fEYlow, n);
   memcpy(fEXhigh, gr.fEXhigh, n);
   memcpy(fEYhigh, gr.fEYhigh, n);
   memcpy(fEXlowd, gr.fEXlowd, n);
   memcpy(fEYlowd, gr.fEYlowd, n);
   memcpy(fEXhighd, gr.fEXhighd, n);
   memcpy(fEYhighd, gr.fEYhighd, n);
}

//_____________________________________________________________________________
TGraphBentErrors::TGraphBentErrors(Int_t n)
       : TGraph(n)
{
//*-*-*-*-*-*-*-*-*-*-*TGraphBentErrors normal constructor*-*-*-*-*-*-*-*-*-*-*
//*-*                  ====================================
//
//  the arrays are preset to zero
   if (!CtorAllocate()) return;
   FillZero(0, fNpoints);
}


//_____________________________________________________________________________
TGraphBentErrors::TGraphBentErrors(Int_t n,
				   const Float_t *x, const Float_t *y,
				   const Float_t *exl, const Float_t *exh,
				   const Float_t *eyl, const Float_t *eyh,
				   const Float_t *exld, const Float_t *exhd,
				   const Float_t *eyld, const Float_t *eyhd)
  : TGraph(n,x,y)
{
//*-*-*-*-*-*-*-*-*-*-*TGraphBentErrors normal constructor*-*-*-*-*-*-*-*-*-*-*
//*-*                  ====================================
//
//  if exl,h or eyl,h are null, the corresponding arrays are preset to zero
   if (!CtorAllocate()) return;

   for (Int_t i=0;i<n;i++) {
      if (exl) fEXlow[i]  = exl[i];
      else     fEXlow[i]  = 0;
      if (exh) fEXhigh[i] = exh[i];
      else     fEXhigh[i] = 0;
      if (eyl) fEYlow[i]  = eyl[i];
      else     fEYlow[i]  = 0;
      if (eyh) fEYhigh[i] = eyh[i];
      else     fEYhigh[i] = 0;

      if (exld) fEXlowd[i]  = exld[i];
      else     fEXlowd[i]  = 0;
      if (exhd) fEXhighd[i] = exhd[i];
      else     fEXhighd[i] = 0;
      if (eyld) fEYlowd[i]  = eyld[i];
      else     fEYlowd[i]  = 0;
      if (eyhd) fEYhighd[i] = eyhd[i];
      else     fEYhighd[i] = 0;
   }
}

//_____________________________________________________________________________
TGraphBentErrors::TGraphBentErrors(Int_t n,
				   const Double_t *x, const Double_t *y,
				   const Double_t *exl, const Double_t *exh,
				   const Double_t *eyl, const Double_t *eyh,
				   const Double_t *exld, const Double_t *exhd,
				   const Double_t *eyld, const Double_t *eyhd)
  : TGraph(n,x,y)
{
//*-*-*-*-*-*-*-*-*-*-*TGraphBentErrors normal constructor*-*-*-*-*-*-*-*-*-*-*
//*-*                  ====================================
//
//  if exl,h or eyl,h are null, the corresponding arrays are preset to zero
   if (!CtorAllocate()) return;
   n = sizeof(Double_t)*fNpoints;

      if (exl) memcpy(fEXlow, exl, n);
      else     memset(fEXlow, 0, n);
      if (exh) memcpy(fEXhigh, exh, n);
      else     memset(fEXhigh, 0, n);
      if (eyl) memcpy(fEYlow, eyl, n);
      else     memset(fEYlow, 0, n);
      if (eyh) memcpy(fEYhigh, eyh, n);
      else     memset(fEYhigh, 0, n);

      if (exld) memcpy(fEXlowd, exld, n);
      else      memset(fEXlowd, 0, n);
      if (exhd) memcpy(fEXhighd, exhd, n);
      else      memset(fEXhighd, 0, n);
      if (eyld) memcpy(fEYlowd,  eyld, n);
      else      memset(fEYlowd, 0, n);
      if (eyhd) memcpy(fEYhighd, eyhd, n);
      else      memset(fEYhighd, 0, n);
}

//_____________________________________________________________________________
TGraphBentErrors::~TGraphBentErrors()
{
//*-*-*-*-*-*-*-*-*-*-*TGraphBentErrors default destructor*-*-*-*-*-*-*-*-*-*-*
//*-*                  ===============================

   delete [] fEXlow;
   delete [] fEXhigh;
   delete [] fEYlow;
   delete [] fEYhigh;

   delete [] fEXlowd;
   delete [] fEXhighd;
   delete [] fEYlowd;
   delete [] fEYhighd;
}

//_____________________________________________________________________________
void TGraphBentErrors::Apply(TF1 *f)
{
  // apply a function to all data points
  // y = f(x,y)
  //
  // Errors are calculated as eyh = f(x,y+eyh)-f(x,y) and
  // eyl = f(x,y)-f(x,y-eyl)
  //
  // Special treatment has to be applied for the functions where the
  // role of "up" and "down" is reversed.
  // function suggested/implemented by Miroslav Helbich <helbich@mail.desy.de>

  Double_t x,y,exl,exh,eyl,eyh,eyl_new,eyh_new,fxy;

  for (Int_t i=0;i<GetN();i++) {
     GetPoint(i,x,y);
     exl=GetEXlow()[i];
     exh=GetEXhigh()[i];
     eyl=GetEYlow()[i];
     eyh=GetEYhigh()[i];

     fxy = f->Eval(x,y);
     SetPoint(i,x,fxy);

     // in the case of the functions like y-> -1*y the roles of the
     // upper and lower error bars is reversed
     if (f->Eval(x,y-eyl)<f->Eval(x,y+eyh)) {
        eyl_new = TMath::Abs(fxy - f->Eval(x,y-eyl));
        eyh_new = TMath::Abs(f->Eval(x,y+eyh) - fxy);
     }
     else {
        eyh_new = TMath::Abs(fxy - f->Eval(x,y-eyl));
        eyl_new = TMath::Abs(f->Eval(x,y+eyh) - fxy);
     }

     //error on x doesn't change
     SetPointError(i,exl,exh,eyl_new,eyh_new);
   }
}

//_____________________________________________________________________________
void TGraphBentErrors::ComputeRange(Double_t &xmin, Double_t &ymin, Double_t &xmax, Double_t &ymax) const
{
  for (Int_t i=0;i<fNpoints;i++) {
     if (fX[i] -fEXlow[i] < xmin) {
        if (gPad && gPad->GetLogx()) {
           if (fEXlow[i] < fX[i]) xmin = fX[i]-fEXlow[i];
           else                   xmin = TMath::Min(xmin,fX[i]/3);
        } else {
          xmin = fX[i]-fEXlow[i];
        }
     }
     if (fX[i] +fEXhigh[i] > xmax) xmax = fX[i]+fEXhigh[i];
     if (fY[i] -fEYlow[i] < ymin) {
        if (gPad && gPad->GetLogy()) {
           if (fEYlow[i] < fY[i]) ymin = fY[i]-fEYlow[i];
           else                   ymin = TMath::Min(ymin,fY[i]/3);
        } else {
          ymin = fY[i]-fEYlow[i];
        }
     }
     if (fY[i] +fEYhigh[i] > ymax) ymax = fY[i]+fEYhigh[i];
  }
}

//______________________________________________________________________________
void TGraphBentErrors::CopyAndRelease(Double_t **newarrays,
                                      Int_t ibegin, Int_t iend, Int_t obegin)
{
   CopyPoints(newarrays, ibegin, iend, obegin);
   if (newarrays) {
      delete[] fEXlow;
      fEXlow = newarrays[0];
      delete[] fEXhigh;
      fEXhigh = newarrays[1];
      delete[] fEYlow;
      fEYlow = newarrays[2];
      delete[] fEYhigh;
      fEYhigh = newarrays[3];
      delete[] fEXlowd;
      fEXlowd = newarrays[4];
      delete[] fEXhighd;
      fEXhighd = newarrays[5];
      delete[] fEYlowd;
      fEYlowd = newarrays[6];
      delete[] fEYhighd;
      fEYhighd = newarrays[7];
      delete[] fX;
      fX = newarrays[8];
      delete[] fY;
      fY = newarrays[9];
      delete[] newarrays;
   }
}

//______________________________________________________________________________
Bool_t TGraphBentErrors::CopyPoints(Double_t **arrays,
                                    Int_t ibegin, Int_t iend, Int_t obegin)
{
// Copy errors from fE*** to arrays[***]
// or to f*** Copy points.
   if (TGraph::CopyPoints(arrays ? arrays+8 : 0, ibegin, iend, obegin)) {
      Int_t n = (iend - ibegin)*sizeof(Double_t);
      if (arrays) {
         memmove(&arrays[0][obegin], &fEXlow[ibegin], n);
         memmove(&arrays[1][obegin], &fEXhigh[ibegin], n);
         memmove(&arrays[2][obegin], &fEYlow[ibegin], n);
         memmove(&arrays[3][obegin], &fEYhigh[ibegin], n);
         memmove(&arrays[4][obegin], &fEXlowd[ibegin], n);
         memmove(&arrays[5][obegin], &fEXhighd[ibegin], n);
         memmove(&arrays[6][obegin], &fEYlowd[ibegin], n);
         memmove(&arrays[7][obegin], &fEYhighd[ibegin], n);
      } else {
         memmove(&fEXlow[obegin], &fEXlow[ibegin], n);
         memmove(&fEXhigh[obegin], &fEXhigh[ibegin], n);
         memmove(&fEYlow[obegin], &fEYlow[ibegin], n);
         memmove(&fEYhigh[obegin], &fEYhigh[ibegin], n);
         memmove(&fEXlowd[obegin], &fEXlowd[ibegin], n);
         memmove(&fEXhighd[obegin], &fEXhighd[ibegin], n);
         memmove(&fEYlowd[obegin], &fEYlowd[ibegin], n);
         memmove(&fEYhighd[obegin], &fEYhighd[ibegin], n);
      }
      return kTRUE;
   } else {
      return kFALSE;
   }
}

//______________________________________________________________________________
Bool_t TGraphBentErrors::CtorAllocate(void)
{
// Should be called from ctors after fNpoints has been set
   if (!fNpoints) {
      fEXlow = fEYlow = fEXhigh = fEYhigh = 0;
      fEXlowd = fEYlowd = fEXhighd = fEYhighd = 0;
      return kFALSE;
   }
   fEXlow = new Double_t[fMaxSize];
   fEYlow = new Double_t[fMaxSize];
   fEXhigh = new Double_t[fMaxSize];
   fEYhigh = new Double_t[fMaxSize];
   fEXlowd = new Double_t[fMaxSize];
   fEYlowd = new Double_t[fMaxSize];
   fEXhighd = new Double_t[fMaxSize];
   fEYhighd = new Double_t[fMaxSize];
   return kTRUE;
}
//_____________________________________________________________________________
Double_t TGraphBentErrors::GetErrorX(Int_t i) const
{
//    This function is called by GraphFitChisquare.
//    It returns the error along X at point i.

   if (i < 0 || i >= fNpoints) return -1;
   if (!fEXlow && !fEXhigh) return -1;
   Double_t elow=0, ehigh=0;
   if (fEXlow)  elow  = fEXlow[i];
   if (fEXhigh) ehigh = fEXhigh[i];
   return TMath::Sqrt(0.5*(elow*elow + ehigh*ehigh));
}

//_____________________________________________________________________________
Double_t TGraphBentErrors::GetErrorY(Int_t i) const
{
//    This function is called by GraphFitChisquare.
//    It returns the error along Y at point i.

   if (i < 0 || i >= fNpoints) return -1;
   if (!fEYlow && !fEYhigh) return -1;
   Double_t elow=0, ehigh=0;
   if (fEYlow)  elow  = fEYlow[i];
   if (fEYhigh) ehigh = fEYhigh[i];
   return TMath::Sqrt(0.5*(elow*elow + ehigh*ehigh));
}

//______________________________________________________________________________
void TGraphBentErrors::FillZero(Int_t begin, Int_t end,
                                 Bool_t from_ctor)
{
// Set zero values for point arrays in the range [begin, end)
   if (!from_ctor) {
      TGraph::FillZero(begin, end, from_ctor);
   }
   Int_t n = (end - begin)*sizeof(Double_t);
   memset(fEXlow + begin, 0, n);
   memset(fEXhigh + begin, 0, n);
   memset(fEYlow + begin, 0, n);
   memset(fEYhigh + begin, 0, n);
   memset(fEXlowd + begin, 0, n);
   memset(fEXhighd + begin, 0, n);
   memset(fEYlowd + begin, 0, n);
   memset(fEYhighd + begin, 0, n);
}

//_____________________________________________________________________________
void TGraphBentErrors::Paint(Option_t *option)
{
   // Paint this TGraphBentErrors with its current attributes
   //
   // by default horizonthal and vertical small lines are drawn at
   // the end of the error bars. if option "z" or "Z" is specified,
   // these lines are not drawn.
   //
   // if option contains ">" an arrow is drawn at the end of the error bars
   // if option contains "|>" a full arrow is drawn at the end of the error bars
   // the size of the arrow is set to 2/3 of the marker size.
   //
   // By default, error bars are drawn. If option "X" is specified,
   // the errors are not drawn (TGraph::Paint equivalent).
   //
   // if option "[]" is specified only the end vertical/horizonthal lines
   // of the error bars are drawn. This option is interesting to superimpose
   // systematic errors on top of a graph with statistical errors.
   // if option "2" is specified error rectangles are drawn.
   // 
   // if option "3" is specified a filled area is drawn through the end points >
   // the vertical error bars.
   // 
   // if option "4" is specified a smoothed filled area is drawn through the end
   // points of the vertical error bars.

   Double_t *xline = 0;
   Double_t *yline = 0;
   Int_t if1 = 0;
   Int_t if2 = 0;

   const Int_t BASEMARKER=8;
   Double_t s2x, s2y, symbolsize, sbase;
   Double_t x, y, xl1, xl2, xr1, xr2, yup1, yup2, ylow1, ylow2, tx, ty;
   Double_t bxl, bxh, byl, byh;
   static Float_t cxx[11] = {1,1,0.6,0.6,1,1,0.6,0.5,1,0.6,0.6};
   static Float_t cyy[11] = {1,1,1,1,1,1,1,1,1,0.5,0.6};

   if (strchr(option,'X') || strchr(option,'x')) {TGraph::Paint(option); return;}
   Bool_t brackets = kFALSE;
   if (strstr(option,"[]")) brackets = kTRUE;
   Bool_t endLines = kTRUE;
   if (strchr(option,'z')) endLines = kFALSE;
   if (strchr(option,'Z')) endLines = kFALSE;
   const char *arrowOpt = 0;
   if (strchr(option,'>'))  arrowOpt = ">";
   if (strstr(option,"|>")) arrowOpt = "|>";

   Bool_t axis = kFALSE;
   if (strchr(option,'a')) axis = kTRUE;
   if (strchr(option,'A')) axis = kTRUE;
   if (axis) TGraph::Paint(option);

   Bool_t option2 = kFALSE;
   Bool_t option3 = kFALSE;
   Bool_t option4 = kFALSE;
   if (strchr(option,'2')) option2 = kTRUE;
   if (strchr(option,'3')) option3 = kTRUE;
   if (strchr(option,'4')) {option3 = kTRUE; option4 = kTRUE;}

   if (option3) {
      xline = new Double_t[2*fNpoints];
      yline = new Double_t[2*fNpoints];
      if (!xline || !yline) {
         Error("Paint", "too many points, out of memory");
         return;
      }
      if1 = 1;
      if2 = 2*fNpoints;
   }

   TAttLine::Modify();

   TArrow arrow;
   arrow.SetLineWidth(GetLineWidth());
   arrow.SetLineColor(GetLineColor());
   arrow.SetFillColor(GetFillColor());

   TBox box;
   box.SetLineWidth(GetLineWidth());
   box.SetLineColor(GetLineColor());
   box.SetFillColor(GetFillColor());

   symbolsize  = GetMarkerSize();
   sbase       = symbolsize*BASEMARKER;
   Int_t mark  = GetMarkerStyle();
   Double_t cx  = 0;
   Double_t cy  = 0;
   if (mark >= 20 && mark < 31) {
      cx = cxx[mark-20];
      cy = cyy[mark-20];
   }

//*-*-      define the offset of the error bars due to the symbol size
   s2x  = gPad->PixeltoX(Int_t(0.5*sbase)) - gPad->PixeltoX(0);
   s2y  =-gPad->PixeltoY(Int_t(0.5*sbase)) + gPad->PixeltoY(0);
   Int_t dxend = Int_t(gStyle->GetEndErrorSize());
   tx   = gPad->PixeltoX(dxend) - gPad->PixeltoX(0);
   ty   =-gPad->PixeltoY(dxend) + gPad->PixeltoY(0);
   Float_t asize = 0.6*symbolsize*BASEMARKER/gPad->GetWh();

   gPad->SetBit(kClipFrame, TestBit(kClipFrame));
   for (Int_t i=0;i<fNpoints;i++) {
      x  = gPad->XtoPad(fX[i]);
      y  = gPad->YtoPad(fY[i]);
      bxl = gPad->YtoPad(fY[i]+fEXlowd[i]);
      bxh = gPad->YtoPad(fY[i]+fEXhighd[i]);
      byl = gPad->XtoPad(fX[i]+fEYlowd[i]);
      byh = gPad->XtoPad(fX[i]+fEYhighd[i]);
      if (x < gPad->GetUxmin()) continue;
      if (x > gPad->GetUxmax()) continue;
      if (y < gPad->GetUymin()) continue;
      if (y > gPad->GetUymax()) continue;

      //  draw the error rectangles
      if (option2) {
         box.PaintBox(gPad->XtoPad(fX[i] - fEXlow[i]),
                      gPad->YtoPad(fY[i] - fEYlow[i]),
                      gPad->XtoPad(fX[i] + fEXhigh[i]),
                      gPad->YtoPad(fY[i] + fEYhigh[i]));
         continue;
      }

      //  keep points for fill area drawing
      if (option3) {
         xline[if1-1] = byh;
         xline[if2-1] = byl;
         yline[if1-1] = gPad->YtoPad(fY[i] + fEYhigh[i]);
         yline[if2-1] = gPad->YtoPad(fY[i] - fEYlow[i]);
         if1++;
         if2--;
         continue;
      }

      xl1 = x - s2x*cx;
      xl2 = gPad->XtoPad(fX[i] - fEXlow[i]);
      if (xl1 > xl2) {
         if (arrowOpt) {
            arrow.PaintArrow(xl1,y,xl2,bxl,asize,arrowOpt);
         } else {
            if (!brackets) gPad->PaintLine(xl1,y,xl2,bxl);
            if (endLines)  gPad->PaintLine(xl2,bxl-ty,xl2,bxl+ty);
         }
      }
      xr1 = x + s2x*cx;
      xr2 = gPad->XtoPad(fX[i] + fEXhigh[i]);
      if (xr1 < xr2) {
         if (arrowOpt) {
            arrow.PaintArrow(xr1,y,xr2,bxh,asize,arrowOpt);
         } else {
            if (!brackets) gPad->PaintLine(xr1,y,xr2,bxh);
            if (endLines)  gPad->PaintLine(xr2,bxh-ty,xr2,bxh+ty);
         }
      }
      yup1 = y + s2y*cy;
      yup2 = gPad->YtoPad(fY[i] + fEYhigh[i]);
      if (yup2 > gPad->GetUymax()) yup2 =  gPad->GetUymax();
      if (yup2 > yup1) {
         if (arrowOpt) {
            arrow.PaintArrow(x,yup1,byh,yup2,asize,arrowOpt);
         } else {
            if (!brackets) gPad->PaintLine(x,yup1,byh,yup2);
            if (endLines)  gPad->PaintLine(byh-tx,yup2,byh+tx,yup2);
         }
      }
      ylow1 = y - s2y*cy;
      ylow2 = gPad->YtoPad(fY[i] - fEYlow[i]);
      if (ylow2 < gPad->GetUymin()) ylow2 =  gPad->GetUymin();
      if (ylow2 < ylow1) {
         if (arrowOpt) {
            arrow.PaintArrow(x,ylow1,byl,ylow2,asize,arrowOpt);
         } else {
            if (!brackets) gPad->PaintLine(x,ylow1,byl,ylow2);
            if (endLines)  gPad->PaintLine(byl-tx,ylow2,byl+tx,ylow2);
         }
      }
   }
   if (!brackets && !axis) TGraph::Paint(option);
   gPad->ResetBit(kClipFrame);

   if (option3) {
      Int_t logx = gPad->GetLogx();
      Int_t logy = gPad->GetLogy();
      gPad->SetLogx(0);
      gPad->SetLogy(0);

      if (option4) PaintGraph(2*fNpoints, xline, yline,"FC");
      else         PaintGraph(2*fNpoints, xline, yline,"F");

      gPad->SetLogx(logx);
      gPad->SetLogy(logy);
      delete [] xline;
      delete [] yline;
   }
}


//______________________________________________________________________________
void TGraphBentErrors::Print(Option_t *) const
{
//*-*-*-*-*-*-*-*-*-*-*Print graph and errors values*-*-*-*-*-*-*-*-*-*-*-*
//*-*                  =============================
//

   for (Int_t i=0;i<fNpoints;i++) {
      printf("x[%d]=%g, y[%d]=%g, exl[%d]=%g, exh[%d]=%g, eyl[%d]=%g, eyh[%d]=%g\n"
         ,i,fX[i],i,fY[i],i,fEXlow[i],i,fEXhigh[i],i,fEYlow[i],i,fEYhigh[i]);
   }
}

//______________________________________________________________________________
void TGraphBentErrors::SavePrimitive(ofstream &out, Option_t *option)
{
    // Save primitive as a C++ statement(s) on output stream out

   char quote = '"';
   out<<"   "<<endl;
   if (gROOT->ClassSaved(TGraphBentErrors::Class())) {
       out<<"   ";
   } else {
       out<<"   TGraphBentErrors *";
   }
   out<<"grae = new TGraphBentErrors("<<fNpoints<<");"<<endl;
   out<<"   grae->SetName("<<quote<<GetName()<<quote<<");"<<endl;
   out<<"   grae->SetTitle("<<quote<<GetTitle()<<quote<<");"<<endl;

   SaveFillAttributes(out,"grae",0,1001);
   SaveLineAttributes(out,"grae",1,1,1);
   SaveMarkerAttributes(out,"grae",1,1,1);

   for (Int_t i=0;i<fNpoints;i++) {
      out<<"   grae->SetPoint("<<i<<","<<fX[i]<<","<<fY[i]<<");"<<endl;
      out<<"   grae->SetPointError("<<i<<","<<fEXlow[i]<<","<<fEXhigh[i]<<","<<fEYlow[i]<<","<<fEYhigh[i]<<");"<<endl;
   }

   static Int_t frameNumber = 0;
   if (fHistogram) {
      frameNumber++;
      TString hname = fHistogram->GetName();
      hname += frameNumber;
      fHistogram->SetName(hname.Data());
      fHistogram->SavePrimitive(out,"nodraw");
      out<<"   grae->SetHistogram("<<fHistogram->GetName()<<");"<<endl;
      out<<"   "<<endl;
   }

   // save list of functions
   TIter next(fFunctions);
   TObject *obj;
   while ((obj=next())) {
      obj->SavePrimitive(out,"nodraw");
      out<<"   grae->GetListOfFunctions()->Add("<<obj->GetName()<<");"<<endl;
      if (obj->InheritsFrom("TPaveStats")) {
         out<<"   ptstats->SetParent(grae->GetListOfFunctions());"<<endl;
      }
   }

   if (strstr(option,"multigraph")) {
      out<<"   multigraph->Add(grae);"<<endl;
      return;
   }
   out<<"   grae->Draw("
      <<quote<<option<<quote<<");"<<endl;
}

//______________________________________________________________________________
void TGraphBentErrors::SetPointError(Double_t exl, Double_t exh, Double_t eyl, Double_t eyh)
{
//*-*-*-*-*-*-*Set ex and ey values for point pointed by the mouse*-*-*-*
//*-*          ===================================================

   Int_t px = gPad->GetEventX();
   Int_t py = gPad->GetEventY();

   //localize point to be deleted
   Int_t ipoint = -2;
   Int_t i;
   // start with a small window (in case the mouse is very close to one point)
   for (i=0;i<fNpoints;i++) {
      Int_t dpx = px - gPad->XtoAbsPixel(gPad->XtoPad(fX[i]));
      Int_t dpy = py - gPad->YtoAbsPixel(gPad->YtoPad(fY[i]));
      if (dpx*dpx+dpy*dpy < 25) {ipoint = i; break;}
   }
   if (ipoint == -2) return;

   fEXlow[ipoint]  = exl;
   fEYlow[ipoint]  = eyl;
   fEXhigh[ipoint] = exh;
   fEYhigh[ipoint] = eyh;
   gPad->Modified();
}

//______________________________________________________________________________
void TGraphBentErrors::SetPointError(Int_t i, Double_t exl, Double_t exh, Double_t eyl, Double_t eyh)
{
//*-*-*-*-*-*-*-*-*-*-*Set ex and ey values for point number i*-*-*-*-*-*-*-*
//*-*                  =======================================

   if (i < 0) return;
   if (i >= fNpoints) {
   // re-allocate the object
      TGraphBentErrors::SetPoint(i,0,0);
   }
   fEXlow[i]  = exl;
   fEYlow[i]  = eyl;
   fEXhigh[i] = exh;
   fEYhigh[i] = eyh;
}

//_____________________________________________________________________________
void TGraphBentErrors::SwapPoints(Int_t pos1, Int_t pos2)
{
  SwapValues(fEXlow,  pos1, pos2);
  SwapValues(fEXhigh, pos1, pos2);
  SwapValues(fEYlow,  pos1, pos2);
  SwapValues(fEYhigh, pos1, pos2);

  SwapValues(fEXlowd,  pos1, pos2);
  SwapValues(fEXhighd, pos1, pos2);
  SwapValues(fEYlowd,  pos1, pos2);
  SwapValues(fEYhighd, pos1, pos2);

  TGraph::SwapPoints(pos1, pos2);
}
