// Copyright (c) 2014 Brad J. Tarratt

// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include <stdio.h>
#include <math.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>


#include "util.h"

#define PI (3.141592653585)

int floadwavetable(int f)
{
  unsigned char *table;

  table = wavetable();

  if (table) {
    return read(f, table, TABLELEN);
  } else {
    return -2;
    }
}

static int loadsine()
{
  int i;
  double f;
  unsigned char *table;

  table = wavetable();
  if (table) {
    for (i=0; i<TABLELEN; i++) {
      f = sin(2*PI*i/TABLELEN);
      f = ((DDSMAX/2)-1)*f+(DDSMAX/2);
      table[i] = (unsigned int)f;
      }
    return 0;
  } else {
    return 1;
    }
}
  
static int loadsquare()
{
  int i;
  unsigned char *table;

  table = wavetable();

  if (table) {
    for (i=0; i<TABLELEN; i++) {
      if (i&(TABLELEN/2)) {
        table[i] = (DDSMAX-1);
      } else {
        table[i] = 0;
        }
      }
    return 0;
  } else {
    return 1;
    }
}

static int loadtriangle()
{
  int i;
  double f;
  unsigned char *table;

  table = wavetable();

  if (table) {
    for (i=0; i<TABLELEN; i++) {
      if (i&(TABLELEN/2)) {
        f = DDSMAX - 1;
        f = TABLELEN/f;
        f = (2*i)/f;
        f -= DDSMAX;
        f = DDSMAX - 1 - f;
        table[i] = (unsigned char)f;
        // table[i] = DDSMAX - 1 - ((2* i / (TABLELEN/(DDSMAX-1)))-DDSMAX);
      } else {
        f = DDSMAX - 1;
        f = TABLELEN/f;
        f = (2*i)/f;
        table[i] = (unsigned char)f;
        // table[i] = 2* i / (TABLELEN/(DDSMAX-1)) ;
        }
      }
    return 0;
  } else {
    return 1;
    }
}
  
static int loadramp()
{
  int i;
  double f;
  unsigned char *table;

  table = wavetable();

  if (table) {
    for (i=0; i<TABLELEN; i++) {
      f = DDSMAX-1;
      f = TABLELEN/f;
      f = i/f;
      table[i] = (unsigned char)f;
// i / (TABLELEN/(DDSMAX-1)) ;
      }
    return 0;
  } else {
    return 1;
    }
}

  
int main (int argc, char **argv)
{
  int f, ret = -2;
  pruinit(&argc, argv, MAINPRU);

  if (argc != 2) {
    fprintf(stderr, "\nUsage: %s <waveshape>\n", argv[0]);
    fprintf(stderr, "  <waveshape> can be a filename or one of sine, square, triangle, ramp\n");
    fprintf(stderr, "  If the file exists it takes precedence over the built-in waveshape.\n");
    fprintf(stderr, "  Files must contain raw 8-bit unsigned samples.  You can convert wave\n");
    fprintf(stderr, "  and other such files with the excellent but seperate sox package.  BE\n");
    fprintf(stderr, "  AWARE that the calling semantics (and expected sample format) for this\n");
    fprintf(stderr, "  program may change in future releases.  If that occurs, this message\n");
    fprintf(stderr, "  will be updated.\n\n");
    return 0;
    }

  f = open(argv[1], O_RDONLY);

  if (f != -1) {
    ret = floadwavetable(f);
    if (ret < 0) {
      fprintf(stderr, "Error loading wavetable from file %s\n", argv[1]);
      return 1;
    } else {
      printf ("Loaded %d octets from file %s into %d-octet wavetable\n", 
              ret, argv[1], TABLELEN);
      return 0;
      }
    }

  if (!strcmp(argv[1], "sine")) {
    printf("Loading built-in sine table\n");
    ret = loadsine();
  } else if (!strcmp(argv[1], "square")) {
    printf("Loading built-in square table\n");
    ret = loadsquare();
  } else if (!strcmp(argv[1], "triangle")) {
    printf("Loading built-in triangle table\n");
    ret = loadtriangle();
  } else if (!strcmp(argv[1], "ramp")) {
    printf("Loading built-in ramp table\n");
    ret = loadramp();
    }

#ifdef AMPLITUDE
//  wavetable2signed();
// This has to happen in the DDS
#endif

  if (ret == -1) {
    fprintf(stderr, "Unable to load file %s, and no such built-in waveshape\n", argv[1]);
  } else if (ret) {
    fprintf(stderr, "Error loading built-in %s wavetable\n", argv[1]);
  } else {
    }

  return(0);
}

