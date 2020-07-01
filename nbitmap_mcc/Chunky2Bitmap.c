/***************************************************************************

 NBitmap.mcc - New Bitmap MUI Custom Class
 Copyright (C) 2006 by Daniel Allsopp
 Copyright (C) 2007-2020 NList Open Source Team

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

#include "Chunky2Bitmap.h"
#include "DitherImage.h"

#include "private.h"

struct BitMap *Chunky2Bitmap(APTR chunky, ULONG width, ULONG height, ULONG depth, struct BitMap *friend)
{
  struct BitMap *bm = NULL;

  ENTER();

  if(chunky != NULL && width > 0 && height > 0)
  {
    if((bm = AllocBitMap(width, height, min(8, depth), BMF_MINPLANES, friend)) != NULL)
    {
      struct BitMap *tempBM;

      if((tempBM = AllocBitMap(PADWIDTH(width), 1, min(8, depth), BMF_MINPLANES, friend)) != NULL)
      {
        struct RastPort remapRP;
        struct RastPort tempRP;

        InitRastPort(&remapRP);
        remapRP.BitMap = bm;

        InitRastPort(&tempRP);
        tempRP.BitMap = tempBM;

        WritePixelArray8(&remapRP, 0, 0, width-1, height-1, chunky, &tempRP);

        FreeBitMap(tempBM);
      }
      else
      {
        FreeBitMap(bm);
        bm = NULL;
      }
    }
  }

  RETURN(bm);
  return bm;
}
