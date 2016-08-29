#include "TColor.h"
#include "TStyle.h"
#include "TROOT.h"
#include "TCanvas.h"
#include "TF3.h"
#include "TH2F.h"
#include "TRandom3.h"
#include "TExec.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <map>
#include <algorithm>

#define MAX_COLORS 256

namespace Color
{
   class Color
   {
      public:
         Color() : m_color()
         {
         }
         Color(EColor color) :  m_color(*(gROOT->GetColor(color)))
         {
         }
         Color(int r, int g, int b) :  m_color(*(gROOT->GetColor(TColor::GetColor(r,g,b))))
         {
         }
         Color(float r, float g, float b) :  m_color(*(gROOT->GetColor(TColor::GetColor(r,g,b))))
         {
         }
         Color(char * hex) :  m_color(*(gROOT->GetColor(TColor::GetColor(hex))))
         {
         }
         void GetRGB(double & r, double & g, double & b)
         {
            float rf,gf,bf;
            m_color.GetRGB(rf,gf,bf);
            r = double(rf);
            g = double(gf);
            b = double(bf);
         }
         void GetRGB(float & r, float & g, float & b)
         {
            m_color.GetRGB(r,g,b);
         }
      private:
         TColor m_color;
   };
   class Palette
   {
      public:
         Palette() : m_ncolors(0)
         {
         }
         Palette(std::string filename, bool reverse = false) : m_ncolors(0)
         {

            //Read file
            std::cout << "Opening " << filename << std::endl;
            std::ifstream ifs(filename.c_str(), std::ifstream::in);

            float r,g,b;
            while(ifs.good() && m_ncolors < MAX_COLORS )
            {
               std::string line;
               char c;
               getline(ifs, line);

               std::istringstream ss(line);
               ss >> r >> c >> g >> c >> b;
               m_palette[m_ncolors] = TColor::GetColor(r,g,b);

               m_ncolors++;
            }
            if(reverse)
               std::reverse(m_palette, m_palette + m_ncolors);
            gStyle->SetPalette(m_ncolors, m_palette);
            gStyle->SetNumberContours(m_ncolors);
         }
         void SetColors(size_t ncolors, int palette[]) 
         {
            std::copy( palette, palette + ncolors, m_palette);
            m_ncolors = ncolors;
         }
         void Activate()
         {
            gStyle->SetPalette(m_ncolors, m_palette);
            gStyle->SetNumberContours(m_ncolors);
         }

      private:
         int m_palette[MAX_COLORS];
         size_t m_ncolors;
   };

   typedef std::map<std::string, std::string> PaletteFileMap;
   PaletteFileMap PaletteFileDB;

   typedef Palette (*PaletteFunction)(bool);
   typedef std::map<std::string, PaletteFunction> PaletteFunctionMap;
   PaletteFunctionMap PaletteFunctionDB;

   Palette MakePalette(std::vector<Color::Color> colors, std::vector<double> stops, bool reverse = false, size_t ncolors = MAX_COLORS)
   {
      double r[MAX_COLORS];
      double g[MAX_COLORS];
      double b[MAX_COLORS];
      for(size_t i = 0; i < colors.size(); ++i)
      {
         colors[i].GetRGB(r[i], g[i], b[i]);
      }
      if(reverse)
      {
         std::cout << "doing revers\n";
         std::reverse(r,r+colors.size());
         std::reverse(g,g+colors.size());
         std::reverse(b,b+colors.size());
      }
      int FI = TColor::CreateGradientColorTable(stops.size(), stops.data(), r, g, b, ncolors);
      gStyle->SetNumberContours(ncolors);

      int palette[MAX_COLORS];
      for (size_t i = 0; i < ncolors; ++i) palette[i] = FI+i;
      
      Palette newPalette;
      newPalette.SetColors(ncolors, palette);
      return newPalette;
   }

   Palette MakePalette(std::vector<Color::Color> colors, bool reverse = false, size_t ncolors = MAX_COLORS)
   {
      std::vector<double> stops;
      float stepsize = 1.0f/(colors.size() - 1.0f);
      for(size_t i = 0; i < colors.size(); ++i)
         stops.push_back(stepsize*i);

      return MakePalette(colors, stops, reverse, ncolors);
   }

   Palette YlGnBu(bool reverse = false)
   {
      std::vector<Color::Color> colors;
      colors.push_back("#ffffd9");
      colors.push_back("#edf8b1");
      colors.push_back("#c7e9b4");
      colors.push_back("#7fcdbb");
      colors.push_back("#41b6c4");
      colors.push_back("#1d91c0");
      colors.push_back("#225ea8");
      colors.push_back("#253494");
      colors.push_back("#081d58");
      return  MakePalette(colors, reverse);
   }
   Palette PrGn(bool reverse = false)
   {
      std::vector<Color::Color> colors;
      colors.push_back("#40004b");
      colors.push_back("#762a83");
      colors.push_back("#9970ab");
      colors.push_back("#c2a5cf");
      colors.push_back("#e7d4e8");
      colors.push_back("#f7f7f7");
      colors.push_back("#d9f0d3");
      colors.push_back("#a6dba0");
      colors.push_back("#5aae61");
      colors.push_back("#1b7837");
      colors.push_back("#00441b");
      return  MakePalette(colors, reverse);
   }

   Palette GetPalette(std::string pal, bool reverse = false)
   {
      if(PaletteFileDB.size() == 0)
      {
         PaletteFileDB["CubeYF"] = "cubeYF_0-1.csv";
         PaletteFileDB["Cube1"] = "cube1_0-1.csv";
         PaletteFileDB["Linear_L"] = "Linear_L_0-1.csv";
      }

      if(PaletteFunctionDB.size() == 0)
      {
         PaletteFunctionDB["YlGnBu"] = YlGnBu;
         PaletteFunctionDB["PrGn"] = PrGn;
      }

      PaletteFileMap::iterator palette = PaletteFileDB.find(pal);
      if( palette != PaletteFileDB.end())
         return Palette(palette->second, reverse);
      else {
         PaletteFunctionMap::iterator palfunc = PaletteFunctionDB.find(pal);
         if( palfunc != PaletteFunctionDB.end())
            return (palfunc->second)(reverse);
         else
            std::cerr << "Palette " << pal << " not found." << std::endl;
      }
   }

   void PaletteExamples(std::string name)
   {
      GetPalette("");
      TRandom3 rand;
      TH2F h("Hi","hi",100,0,10,100,0,10);
      for(unsigned int i = 0; i < 1000000; ++i)
      {
         h.Fill(rand.Gaus(5,3), rand.Gaus(5,3));
      }
      
      TCanvas * c = new TCanvas("c","c",300, 300);
      gStyle->SetOptStat(0);
      for(PaletteFileMap::iterator i = PaletteFileDB.begin();
            i != PaletteFileDB.end();
            ++i)
      {
         std::cout << i->first << std::endl;
         Palette pal = GetPalette(i->first);

         h.SetTitle(i->first.c_str());
         h.Draw("colz");

         c->SaveAs((name + i->first + ".png").c_str());
      }
      for(PaletteFunctionMap::iterator i = PaletteFunctionDB.begin();
            i != PaletteFunctionDB.end();
            ++i)
      {
         std::cout << i->first << std::endl;
         Palette pal = GetPalette(i->first);
         h.SetTitle(i->first.c_str());
         h.Draw("colz");
         c->SaveAs((name + i->first + ".png").c_str());
      }
      for(PaletteFileMap::iterator i = PaletteFileDB.begin();
            i != PaletteFileDB.end();
            ++i)
      {
         std::cout << i->first << std::endl;

         Palette pal = GetPalette(i->first, true);

         h.SetTitle((i->first + "-rev").c_str());
         h.Draw("colz");

         c->SaveAs((name + i->first + "-rev.png").c_str());
      }
      for(PaletteFunctionMap::iterator i = PaletteFunctionDB.begin();
            i != PaletteFunctionDB.end();
            ++i)
      {
         std::cout << i->first << std::endl;
         Palette pal = GetPalette(i->first, true);
         h.SetTitle((i->first + "-rev").c_str());
         h.Draw("colz");
         c->SaveAs((name + i->first + "-rev.png").c_str());
      }
   }
}
