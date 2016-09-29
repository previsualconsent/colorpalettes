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
         Color(int color) :  m_color(*(gROOT->GetColor(color)))
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
         Color(char const * hex) :  m_color(*(gROOT->GetColor(TColor::GetColor(hex))))
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
            std::ifstream ifs{filename.c_str(), std::ifstream::in};

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
      std::vector<Color::Color> colors = {
         "#ffffd9",
         "#edf8b1",
         "#c7e9b4",
         "#7fcdbb",
         "#41b6c4",
         "#1d91c0",
         "#225ea8",
         "#253494",
         "#081d58",
      };
      return  MakePalette(colors, reverse);
   }
   Palette PrGn(bool reverse = false)
   {
      std::vector<Color::Color> colors = {
         "#40004b",
         "#762a83",
         "#9970ab",
         "#c2a5cf",
         "#e7d4e8",
         "#f7f7f7",
         "#d9f0d3",
         "#a6dba0",
         "#5aae61",
         "#1b7837",
         "#00441b",
      };
      return  MakePalette(colors, reverse);
   }

   Palette YlOrRd(bool reverse = false)
   {
      std::vector<Color::Color> colors = {
         "#ffffcc",
         "#ffeda0",
         "#fed976",
         "#feb24c",
         "#fd8d3c",
         "#fc4e2a",
         "#e31a1c",
         "#bd0026",
         "#800026",
      };
      return MakePalette(colors, reverse);
   }

   Palette Ugly(bool reverse = false)
   {
      std::vector<Color::Color> colors = {
         {"#0099ff"},
         {1.0f,1.0f,1.0f},
         {255, 127, 0},
      };
      return MakePalette(colors, reverse);
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
         PaletteFunctionDB["YlOrRd"] = YlOrRd;
         PaletteFunctionDB["Ugly"] = Ugly;
      }

      PaletteFileMap::iterator palette = PaletteFileDB.find(pal);
      if( palette != PaletteFileDB.end())
         return Palette{palette->second, reverse};
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
      TH2F h{"Hi","hi",100,0,10,100,0,10};
      for(unsigned int i = 0; i < 1000000; ++i)
      {
         h.Fill(rand.Gaus(5,3), rand.Gaus(5,3));
      }
      
      TCanvas * c = new TCanvas{"c","c",600, 600};
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
