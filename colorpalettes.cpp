#include "colorpalettes.h"
#include "TCanvas.h"
#include "TStyle.h"
#include "TH2F.h"
#include "TRandom3.h"
#include "TROOT.h"

int main()
{
   Color::PaletteExamples("~/public_html/palettes/");

   Color::Palette p = Color::GetPalette("YlGnBu");

   gStyle->SetOptStat(0);
   TCanvas c("c","c",500,500);
   TRandom3 rand;
   TH2F h("Hi","hi",100,0,10,100,0,10);
   for(unsigned int i = 0; i < 1000000; ++i)
   {
      h.Fill(rand.Gaus(5,3), rand.Gaus(5,3));
   }
   h.Draw("colz");
   c.SaveAs("~/public_html/palettes/test.png");

}
