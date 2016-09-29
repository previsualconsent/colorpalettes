#include "colorpalettes.h"
#include "TCanvas.h"
#include "TStyle.h"
#include "TH2F.h"
#include "TRandom3.h"
#include "TROOT.h"

int main()
{
   Color::PaletteExamples("~/public_html/palettes/");

   //std::vector<Color::Color> colors = {kRed, kYellow, kGreen, kBlue};
   std::vector<Color::Color> colors = { 
      "#f7fcf5",
      "#e5f5e0",
      "#c7e9c0",
      "#a1d99b",
      "#74c476",
      "#41ab5d",
      "#238b45",
      "#006d2c",
      "#00441b"
   };

   //std::vector<double> stops;
   //stops.push_back(0);
   //stops.push_back(1);
   Color::Palette p = MakePalette(colors);

   gStyle->SetOptStat(0);
   TCanvas c{"c","c",500,500};
   TRandom3 rand;
   TH2F h{"Hi","hi",100,0,10,100,0,10};
   for(unsigned int i = 0; i < 1000000; ++i)
   {
      h.Fill(rand.Gaus(5,3), rand.Gaus(5,3));
   }
   h.Draw("colz");
   c.SaveAs("~/public_html/palettes/test.png");

}
