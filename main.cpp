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

#include <assert.h>
#include <iostream>
#include <sstream>
#include <string>
#include <list>
#include <vector>

#include "png.hpp"
#include "TexturePacker.h"
#include "tclap/CmdLine.h"


#define VERSION "2010.09.06"

class TextureInfo;

class TextureAtlasInfoVisitor
   {
   public:
      virtual void visit(TextureInfo &im_info,
                         int x, int y, int width, int height, bool rot90) = 0;
   };

class TextureInfo
   {
   public:
      TextureInfo(int index, std::string path)
         : idx(index), im_path(path)
      {
      im = new png::image<png::rgba_pixel>(im_path,
            png::convert_color_space<png::rgba_pixel>());
      }

      void index(size_t i) { idx = i; }
      int index() const { return idx; }
      std::string path() const { return im_path; }
      png::image<png::rgba_pixel>* image() const { return im; }

      void writeTo(png::image< png::rgba_pixel > &outimage,
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
               size_t rot_y = im->get_height() - (out_x - offset_x) - 1;
               size_t rot_x = (out_y - offset_y);
               png::rgba_pixel px = (*im)[rot_y][rot_x];
               outimage[out_y][out_x] = px;
               }
            else
               {
               size_t rot_y = out_y - offset_y;
               size_t rot_x = out_x - offset_x;
               outimage[out_y][out_x] = (*im)[rot_y][rot_x];
               }
            }
         }
      }

   private:
      int                           idx;
      std::string                   im_path;
      png::image<png::rgba_pixel>*   im;
   };


class TextureAtlasInfo
   {
   public:
      TextureAtlasInfo(size_t atlas_width, size_t atlas_height, size_t im_count)
         : atlas_max_width(atlas_width), atlas_max_height(atlas_height),
            atlas_unused_pixels(0), images()
      {
      atlas = TEXTURE_PACKER::createTexturePacker();
      atlas->setTextureCount(im_count);
      }

      ~TextureAtlasInfo(void)
      {
      delete(atlas);
      }

      bool addTexture(TextureInfo* im_info)
      {
      png::image<png::rgba_pixel>* im = im_info->image();
      bool fit = atlas->wouldTextureFit(im->get_width(), im->get_height(),
                                        true, false,
                                        atlas_max_width, atlas_max_height);

      if (!fit)
         {
         return false;
         }

      atlas->addTexture(im->get_width(), im->get_height());
      im_info->index(atlas->getTextureCount() - 1);
      images.push_back(im_info);
      return true;
      }

      void packTextures(void)
      {
      int w, h;
      atlas_unused_pixels = atlas->packTextures(w, h, true, false);
      atlas_max_width = w;
      atlas_max_height = h;
      }

      size_t packedCount(void) { return atlas->getTextureCount(); }
      size_t packedUnusedPixels(void) { return atlas_unused_pixels; }
      size_t packedWidth(void) { return atlas_max_width; }
      size_t packedHeight(void) { return atlas_max_height; }

      void visit(TextureAtlasInfoVisitor &taiv)
      {
      for(std::list<TextureInfo *>::iterator images_iter = images.begin();
          images_iter != images.end();
          images_iter++)
         {
         TextureInfo* im_info = *images_iter;
         int x, y, width, height;
         bool rot90;
         rot90 = atlas->getTextureLocation(im_info->index(),
                                           x, y, width, height);

         taiv.visit(*im_info, x, y, width, height, rot90);
         }

      }

   private:
      size_t                           atlas_unused_pixels;
      size_t                           atlas_max_width;
      size_t                           atlas_max_height;
      TEXTURE_PACKER::TexturePacker*   atlas;
      std::list<TextureInfo *>         images;
   };

class TextureAtlasInfoWriteVisitor : public TextureAtlasInfoVisitor
   {
   public:
   TextureAtlasInfoWriteVisitor(std::string path, size_t w, size_t h) :
      out_image_path(path), out_image(w, h)
      {
      }

   ~TextureAtlasInfoWriteVisitor(void)
      {
      out_image.write(out_image_path + ".png");
      }

   void visit(TextureInfo &im_info,
              int x, int y, int width, int height, bool rot90)
      {
      im_info.writeTo(out_image, x, y, width, height, rot90);
      }

   private:
   png::image<png::rgba_pixel> out_image;
   std::string                 out_image_path;
   };

class TextureAtlasInfoDebugWriteVisitor : public TextureAtlasInfoVisitor
   {
   public:
   TextureAtlasInfoDebugWriteVisitor(std::string path,
                                     size_t w, size_t h,
                                     size_t num_packed, size_t unused) :
      out_image_path(path), out_image_width(w), out_image_height(h)
      {
      std::cout << "Packed " << num_packed << " images to " << path << ".png"
         << " with " << unused << " unused area "
         << "Width (" << w << ") x Height (" << h  << ")"
         << std::endl;
      }

   ~TextureAtlasInfoDebugWriteVisitor(void)
      {
      std::cout << std::endl;
      }

   void visit(TextureInfo &im_info,
              int x, int y, int width, int height, bool rot90)
      {
      std::cout << im_info.path() << " => ";
      if (rot90) std::cout << "rotated 90 ";
      std::cout  << "x: " << x << " "
         << "y: " << y << " "
         << "width: " << width << " "
         << "height: " << height << " "
         << std::endl;
      }

   private:
   std::string                   out_image_path;
   size_t                        out_image_width;
   size_t                        out_image_height;
   };

class TextureAtlasInfoCSVWriteVisitor : public TextureAtlasInfoVisitor
   {
   public:
   TextureAtlasInfoCSVWriteVisitor(std::string path, size_t w, size_t h) :
      out_image_path(path), out_image_width(w), out_image_height(h)
      {
      }

   ~TextureAtlasInfoCSVWriteVisitor(void)
      {
      }

   void visit(TextureInfo &im_info,
              int x, int y, int width, int height, bool rot90)
      {
      }

   private:
   std::string                   out_image_path;
   size_t                        out_image_width;
   size_t                        out_image_height;
   };

int
main(int argc, char* argv[])
{
std::string                                  out_atlas_name = "";
size_t                                       out_atlas_height = 0;
size_t                                       out_atlas_width = 0;
std::vector<std::string>                     image_names;
std::list<TextureAtlasInfo *>                atlases;

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

// Create the initial texture atlas
atlases.push_back(new TextureAtlasInfo(out_atlas_width, out_atlas_height,
                                       image_names.size()));

// Load the textures
for(size_t idx = 0; idx < image_names.size(); idx++)
   {
   try
      {
      // Load the image
      TextureInfo* ti = new TextureInfo(idx, image_names[idx]);
      if (ti->image()->get_width() > out_atlas_width ||
          ti->image()->get_height() > out_atlas_height)
         {
         std::cerr << "Error: " << ti->path() << " is too big!" << std::endl;
         std::cerr << "Image dimensions: " << ti->image()->get_width()
            << " x " << ti->image()->get_height() << std::endl;
         std::cerr << "Atlas dimensions: " << out_atlas_width
            << " x " << out_atlas_height << std::endl;
         return 1;
         }

      // Add to the atlas
      TextureAtlasInfo* tai = atlases.back();
      if (!tai->addTexture(ti))
         {
         // Create a new atlas
         TextureAtlasInfo* tai_next = new TextureAtlasInfo(out_atlas_width,
                                                           out_atlas_height,
                                                           image_names.size());
         tai_next->addTexture(ti);
         atlases.push_back(tai_next);
         }
      }
   catch(png::std_error &e)
      {
      std::cerr << e.what() << std::endl;
      return 1;
      }
   }

// Pack and write out the atlases
int idx = 0;
for(std::list<TextureAtlasInfo *>::iterator atlases_iter = atlases.begin();
    atlases_iter != atlases.end();
    atlases_iter++, idx++)
   {
   std::ostringstream oss;
   oss << out_atlas_name << "_" << idx;
   std::string atlas_name(oss.str());

   TextureAtlasInfo* tai = *atlases_iter;
   tai->packTextures();

   TextureAtlasInfoWriteVisitor tai_writer(atlas_name,
                                           tai->packedWidth(),
                                           tai->packedHeight());
   TextureAtlasInfoDebugWriteVisitor tai_debug(atlas_name,
                                               tai->packedWidth(),
                                               tai->packedHeight(),
                                               tai->packedCount(),
                                               tai->packedUnusedPixels());
   tai->visit(tai_writer);
   tai->visit(tai_debug);
   }

return 0;
}

