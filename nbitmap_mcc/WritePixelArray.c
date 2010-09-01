/***************************************************************************

 NBitmap.mcc - New Bitmap MUI Custom Class
 Copyright (C) 2006 by Daniel Allsopp
 Copyright (C) 2007-2008 by NList Open Source Team

 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.

 NList classes Support Site:  http://www.sf.net/projects/nlist-classes

 $Id$

***************************************************************************/

#include <proto/graphics.h>
#include <cybergraphx/cybergraphics.h>

#include "SDI_compiler.h"

#include "private.h"

void _WPA(APTR src, UWORD srcx, UWORD srcy, UWORD srcmod, struct RastPort *rp, UWORD destx, UWORD desty, UWORD width, UWORD height, ULONG fmt)
{
  if(width > 0 && height > 0 && fmt == RECTFMT_LUT8)
  {
    UBYTE *_src = (UBYTE *)src + srcmod * srcy + srcx;
    UWORD y;

    for(y = 0; y < height; y++)
    {
      UWORD x;

      // copy one line pixel by pixel
      // yes, I know, this is a very dumb approach, but it works
      for(x = 0; x < width; x++)
      {
        SetAPen(rp, _src[x]);
        WritePixel(rp, destx+x, desty+y);
      }
    }
  }
}
