=== About ===

This is a simple command line tool that takes a list of PNG image files and creates a somewhat optimized texture atlas. It might rotate images if that will allow the texture atlas to be smaller.



=== Usage ===

USAGE:
   ./texture-atlas  [-y <pixels>] [-x <pixels>] [-o <string>] [--]
                    [--version] [-h] <PNG file path> ...

Where:

   -y <pixels>,  --out_height <pixels>
     Maximum output atlas image's height.

   -x <pixels>,  --out_width <pixels>
     Maximum output atlas image's width.

   -o <string>,  --out_name <string>
     Output atlas image's file name.

   --,  --ignore_rest
     Ignores the rest of the labeled arguments following this flag.

   --version
     Displays version information and exits.

   -h,  --help
     Displays usage information and exits.

   <PNG file path>  (accepted multiple times)
     (required)  List of the image filenames.


   Creates a texture atlas from a list of PNG files.


Ex:
./texture-atlas image/128x64.png  image/64x128.png



=== License and Acknowledgements ===

The files in png++ are from the png++ project hosted at http://savannah.nongnu.org/projects/pngpp/ and licensed under a modified BSD license. See the file png++/COPYING for more details.


The files in the texture-atlas directory are from John Ratcliff's Texture-Atlas library: http://code.google.com/p/texture-atlas/ and is licensed under the MIT License. This library is based on this paper: http://www.antisphere.com/Research/Tiles.php


The files found in the tclap directory are from the Templatized C++ Command Line Parser Library hosted at http://tclap.sourceforge.net/ and is licensed under the MIT License.


The code not found in those two directories is written by Bryan Ivory and licensed under the MIT License: http://www.opensource.org/licenses/mit-license.php
