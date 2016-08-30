#include "colorpalettes.h"
#include "TCanvas.h"
#include "TStyle.h"
#include "TH2F.h"
#include "TRandom3.h"
#include "TROOT.h"

int main()
{
   Color::PaletteExamples("~/public_html/palettes/");

   std::vector<Color::Color> colors;
   
   colors.push_back("#f7fcf5");
   colors.push_back("#e5f5e0");
   colors.push_back("#c7e9c0");
   colors.push_back("#a1d99b");
   colors.push_back("#74c476");
   colors.push_back("#41ab5d");
   colors.push_back("#238b45");
   colors.push_back("#006d2c");
   colors.push_back("#00441b");

   //std::vector<double> stops;
   //stops.push_back(0);
   //stops.push_back(1);
   Color::Palette p = MakePalette(colors);

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
