/*
The MIT License

Copyright (c) 2010 Bryan Ivory bivory+textureatlas@gmail.com

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

#include <iostream>
#include <string>
#include <list>
#include <vector>

#include "png.hpp"
#include "TexturePacker.h"
#include "tclap/CmdLine.h"


#define VERSION "2010.09.05"

class TextureInfo
   {
   public:
      TextureInfo(int index, std::string path)
         : idx(index), im_path(path)
      {
      im = png::image<png::rgb_pixel>(im_path);
      }

      int index() const { return idx; }
      std::string path() const { return im_path; }
      png::image< png::rgb_pixel > image() const { return im; }

      void writeTo(png::image< png::rgb_pixel > &outimage,
                   const size_t offset_x, const size_t offset_y,
                   const size_t out_width, const size_t out_height,
                   const bool rotate90)
      {
      for(size_t out_y = offset_y; out_y < (offset_y + out_height); ++out_y)
         {
         for(size_t out_x = offset_x; out_x < (offset_x + out_width); ++out_x)
            {
            if (rotate90)
               {
               size_t rot_y = im.get_height() - (out_x - offset_x) - 1;
               size_t rot_x = (out_y - offset_y);
               png::rgb_pixel px = im[rot_y][rot_x];
               outimage[out_y][out_x] = px;
               }
            else
               {
               size_t rot_y = out_y - offset_y;
               size_t rot_x = out_x - offset_x;
               outimage[out_y][out_x] = im[rot_y][rot_x];
               }
            }
         }
      }

   private:
      const int                     idx;
      std::string                   im_path;
      png::image< png::rgb_pixel >  im;
   };


int
main(int argc, char* argv[])
{
std::string                out_atlas_name = "";
size_t                     out_atlas_height = 0;
size_t                     out_atlas_width = 0;
std::vector<std::string>   image_names;

// Read in the command line arguements
try
   {
   TCLAP::CmdLine cmd("Creates a texture atlas from a list of PNG files.",
                      ' ', VERSION, true);

   // Output atlas file name
   TCLAP::ValueArg<std::string> out_atlas_name_arg(
         "o","out_name",
         "Output atlas image's file name.",
         false, "out_atlas", "string");
   cmd.add(out_atlas_name_arg);

   // Output atlas image dimensions
   TCLAP::ValueArg<size_t> out_atlas_width_arg(
         "x","out_width",
         "Maximum output atlas image's width.",
         false, 1024, "pixels");
   cmd.add(out_atlas_width_arg);

   TCLAP::ValueArg<size_t> out_atlas_height_arg(
         "y","out_height",
         "Maximum output atlas image's height.",
         false, 1024, "pixels");
   cmd.add(out_atlas_height_arg);

   // Input image files
   TCLAP::UnlabeledMultiArg<std::string> in_image_names_arg(
         "in_names", "List of the image filenames.", true, "PNG file path");
   cmd.add(in_image_names_arg);

   cmd.parse(argc, argv);
   out_atlas_name = out_atlas_name_arg.getValue();
   out_atlas_width = out_atlas_width_arg.getValue();
   out_atlas_height = out_atlas_height_arg.getValue();
   image_names = in_image_names_arg.getValue();
   }
catch (TCLAP::ArgException &e)
   {
   std::cerr << "Error: " << e.error() << " for arg " << e.argId() << std::endl;
   return 1;
   }


// Create the ist of texture to pack
std::list<TextureInfo> images;
for(size_t idx = 0; idx < image_names.size(); idx++)
   {
   try
      {
      TextureInfo ti(idx, image_names[idx]);
      images.push_back(ti);
      }
   catch(png::std_error &e)
      {
      std::cerr << e.what() << std::endl;
      return 1;
      }
   }

// Pack the textures
TEXTURE_PACKER::TexturePacker *tp = TEXTURE_PACKER::createTexturePacker();
tp->setTextureCount(images.size());
for(
 std::list<TextureInfo>::iterator images_iter = images.begin();
 images_iter != images.end();
 images_iter++)
   {
   png::image<png::rgb_pixel> im = images_iter->image();
   tp->addTexture(im.get_width(), im.get_height());
   }
// Force power of two, no pixel border
int act_out_atlas_width = 0;
int act_out_atlas_height = 0;
size_t unused_area = tp->packTextures(act_out_atlas_width,
      act_out_atlas_height, true, false);

std::cout << "Packed " << image_names.size() << " with "
         << unused_area << " unused area: "
         << "Width (" << act_out_atlas_width
         << ") x Height (" << act_out_atlas_height << ")"
         << std::endl;


// Output the packed textures
png::image<png::rgb_pixel> out_image(act_out_atlas_width, act_out_atlas_height);
for(
 std::list<TextureInfo>::iterator images_iter = images.begin();
 images_iter != images.end();
 images_iter++)
   {
   int x, y, width, height;
   bool rot90;

   rot90 = tp->getTextureLocation(images_iter->index(), x, y, width, height);
   std::cout   << images_iter->path() << " => "
               << "rotated 90: " << rot90 << " "
               << "x: " << x << " "
               << "y: " << y << " "
               << "width: " << width << " "
               << "height: " << height << " "
               << std::endl;

   images_iter->writeTo(out_image, x, y, width, height, rot90);
   }
out_image.write(out_atlas_name + ".png");

return 0;
}

