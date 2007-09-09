/***************************************************************************

 NList.mcc - New List MUI Custom Class
 Registered MUI class, Serial Number: 1d51 0x9d510030 to 0x9d5100A0
                                           0x9d5100C0 to 0x9d5100FF

 Copyright (C) 1996-2001 by Gilles Masson
 Copyright (C) 2001-2005 by NList Open Source Team

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

#include <string.h>

#include <clib/alib_protos.h>

#include <intuition/pointerclass.h>

#include <proto/utility.h>
#include <proto/intuition.h>
#include <proto/exec.h>
#include <proto/graphics.h>

#include "private.h"

// the meta data of our pointers
#define sizePointerWidth      14
#define sizePointerHeight     12
#define sizePointerXOffset    -7
#define sizePointerYOffset    -6
#define movePointerWidth      14
#define movePointerHeight      6
#define movePointerXOffset    -7
#define movePointerYOffset    -3
#define selectPointerWidth     7
#define selectPointerHeight   16
#define selectPointerXOffset  -3
#define selectPointerYOffset  -8

#if defined(__amigaos4__)
// a 32bit ARGB pointer image where we can define every color of the pixels
// on our own and put an alpha-channel information in it as well.
static const ULONG sizePointer[] =
{
  /* .....#..#..... */ 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0xffffffff, 0x00000000,  0x00000000, 0xffffffff, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
  /* .....#+.#+.... */ 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0xffffffff, 0xff000000,  0x00000000, 0xffffffff, 0xff000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
  /* .....#+.#+.... */ 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0xffffffff, 0xff000000,  0x00000000, 0xffffffff, 0xff000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
  /* ..#..#+.#+.#.. */ 0x00000000, 0x00000000, 0xffffffff, 0x00000000, 0x00000000, 0xffffffff, 0xff000000,  0x00000000, 0xffffffff, 0xff000000, 0x00000000, 0xffffffff, 0x00000000, 0x00000000,
  /* .##+.#+.#+.##. */ 0x00000000, 0xffffffff, 0xffffffff, 0xff000000, 0x00000000, 0xffffffff, 0xff000000,  0x00000000, 0xffffffff, 0xff000000, 0x00000000, 0xffffffff, 0xffffffff, 0x00000000,
  /* ######+.###### */ 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xff000000,  0x00000000, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
  /* .##++#+.#++##+ */ 0x00000000, 0xffffffff, 0xffffffff, 0xff000000, 0xff000000, 0xffffffff, 0xff000000,  0x00000000, 0xffffffff, 0xff000000, 0xff000000, 0xffffffff, 0xffffffff, 0xff000000,
  /* ..#+.#+.#+.#+. */ 0x00000000, 0x00000000, 0xffffffff, 0xff000000, 0x00000000, 0xffffffff, 0xff000000,  0x00000000, 0xffffffff, 0xff000000, 0x00000000, 0xffffffff, 0xff000000, 0x00000000,
  /* ...+.#+.#+.+.. */ 0x00000000, 0x00000000, 0x00000000, 0xff000000, 0x00000000, 0xffffffff, 0xff000000,  0x00000000, 0xffffffff, 0xff000000, 0x00000000, 0xff000000, 0x00000000, 0x00000000,
  /* .....#+.#+.... */ 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0xffffffff, 0xff000000,  0x00000000, 0xffffffff, 0xff000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
  /* .....#+.#+.... */ 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0xffffffff, 0xff000000,  0x00000000, 0xffffffff, 0xff000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
  /* ......+..+.... */ 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0xff000000,  0x00000000, 0x00000000, 0xff000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

static const ULONG movePointer[] =
{
  /* ..#########... */ 0x00000000, 0x00000000, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,  0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0x00000000, 0x00000000, 0x00000000,
  /* .##+++++++##.. */ 0x00000000, 0xffffffff, 0xffffffff, 0xff000000, 0xff000000, 0xff000000, 0xff000000,  0xff000000, 0xff000000, 0xff000000, 0xffffffff, 0xffffffff, 0x00000000, 0x00000000,
  /* ###+......###. */ 0xffffffff, 0xffffffff, 0xffffffff, 0xff000000, 0x00000000, 0x00000000, 0x00000000,  0x00000000, 0x00000000, 0x00000000, 0xffffffff, 0xffffffff, 0xffffffff, 0x00000000,
  /* .##+......##++ */ 0x00000000, 0xffffffff, 0xffffffff, 0xff000000, 0x00000000, 0x00000000, 0x00000000,  0x00000000, 0x00000000, 0x00000000, 0xffffffff, 0xffffffff, 0xff000000, 0xff000000,
  /* ..#########++. */ 0x00000000, 0x00000000, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,  0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xff000000, 0xff000000, 0x00000000,
  /* ...+++++++++.. */ 0x00000000, 0x00000000, 0x00000000, 0xff000000, 0xff000000, 0xff000000, 0xff000000,  0xff000000, 0xff000000, 0xff000000, 0xff000000, 0xff000000, 0x00000000, 0x00000000,
};

static const ULONG selectPointer[] =
{
  0xff6d6d6d, 0xff000000, 0x00000000, 0x00000000, 0xffbbbbbb, 0xff6d6d6d, 0xff000000, // +#  :+#
  0x00000000, 0xffbbbbbb, 0xff000000, 0xffbbbbbb, 0xff000000, 0x00000000, 0x00000000, //  :#:#
  0x00000000, 0x00000000, 0xffbbbbbb, 0xff000000, 0x00000000, 0x00000000, 0x00000000, //   :#
  0x00000000, 0x00000000, 0xffbbbbbb, 0xff000000, 0x00000000, 0x00000000, 0x00000000, //   :#
  0x00000000, 0x00000000, 0xffbbbbbb, 0xff000000, 0x00000000, 0x00000000, 0x00000000, //   :#
  0x00000000, 0x00000000, 0xffbbbbbb, 0xff000000, 0x00000000, 0x00000000, 0x00000000, //   :#
  0x00000000, 0x00000000, 0xffbbbbbb, 0xff000000, 0x00000000, 0x00000000, 0x00000000, //   :#
  0x00000000, 0x00000000, 0xffbbbbbb, 0xff000000, 0x00000000, 0x00000000, 0x00000000, //   :#
  0x00000000, 0xffbbbbbb, 0xff000000, 0xff000000, 0xff000000, 0x00000000, 0x00000000, //  :###
  0x00000000, 0x00000000, 0xffbbbbbb, 0xff000000, 0x00000000, 0x00000000, 0x00000000, //   :#
  0x00000000, 0x00000000, 0xffbbbbbb, 0xff000000, 0x00000000, 0x00000000, 0x00000000, //   :#
  0x00000000, 0x00000000, 0xffbbbbbb, 0xff000000, 0x00000000, 0x00000000, 0x00000000, //   :#
  0x00000000, 0x00000000, 0xffbbbbbb, 0xff000000, 0x00000000, 0x00000000, 0x00000000, //   :#
  0x00000000, 0x00000000, 0xffbbbbbb, 0xff000000, 0x00000000, 0x00000000, 0x00000000, //   :#
  0x00000000, 0xffbbbbbb, 0xff000000, 0xffbbbbbb, 0xff000000, 0x00000000, 0x00000000, //  :#:#
  0xff6d6d6d, 0xff000000, 0x00000000, 0x00000000, 0xffbbbbbb, 0xff6d6d6d, 0xff000000, // +#  :+#
};

#ifndef POINTERA_ImageData
#define POINTERA_ImageData (POINTERA_Dummy + 0x07) // ARGB (width * height * sizeof(ULONG))
#endif
#ifndef POINTERA_Width
#define POINTERA_Width     (POINTERA_Dummy + 0x08) // <= 64
#endif
#ifndef POINTERA_Height
#define POINTERA_Height    (POINTERA_Dummy + 0x09) // <= 64
#endif

// the 32bit pointer image setup seems to require
// to have a fake bitmap.
static struct BitMap fakeSizeBitmap =
{
  2, 12, 0, 2, 0,
  { NULL, NULL, NULL, NULL, NULL, NULL, NULL }
};

static struct BitMap fakeMoveBitmap =
{
  2, 6, 0, 2, 0,
  { NULL, NULL, NULL, NULL, NULL, NULL, NULL }
};

static struct BitMap fakeSelectBitmap =
{
  2, 16, 0, 2, 0,
  { NULL, NULL, NULL, NULL, NULL, NULL, NULL }
};

#else // __amigaos4__

// the size pointer
static const UWORD sizePointer_bp0[] =
{
  0x0480, // .....#..#.......
  0x0480, // .....#..#.......
  0x0480, // .....#..#.......
  0x2490, // ..#..#..#..#....
  0x6498, // .##..#..#..##...
  0xfcfc, // ######..######..
  0x6498, // .##..#..#..##...
  0x2490, // ..#..#..#..#....
  0x0480, // .....#..#.......
  0x0480, // .....#..#.......
  0x0480, // .....#..#.......
  0x0000  // ................
};

static const UWORD sizePointer_bp1[] =
{
  0x0000, // ................
  0x0240, // ......#..#......
  0x0240, // ......#..#......
  0x0240, // ......#..#......
  0x1240, // ...#..#..#......
  0x0200, // ......#.........
  0x1a64, // ...##.#..##..#..
  0x1248, // ...#..#..#..#...
  0x1250, // ...#..#..#.#....
  0x0240, // ......#..#......
  0x0240, // ......#..#......
  0x0240  // ......#..#......
};

static UWORD sizePointer_bp2[] =
{
  0x0000,
  0x0000,
  0x0000,
  0x0000,
  0x0000,
  0x0000,
  0x0000,
  0x0000,
  0x0000,
  0x0000,
  0x0000,
  0x0000
};

static const UWORD sizePointer[] =
{
//plane1    plane2
  0x0000,   0x0000,

  0x0480,   0x0000,
  0x0480,   0x0240,
  0x0480,   0x0240,
  0x2490,   0x0240,
  0x6498,   0x1240,
  0xfcfc,   0x0200,
  0x6498,   0x1a64,
  0x2490,   0x1248,
  0x0480,   0x1250,
  0x0480,   0x0240,
  0x0480,   0x0240,
  0x0000,   0x0240,

  0x0000,   0x0000
};

static struct BitMap sizePointerBitmap =
{
  2, 12, 0, 2, 0,
  { (PLANEPTR)sizePointer_bp0, (PLANEPTR)sizePointer_bp1, NULL, NULL, NULL, NULL, NULL, NULL }
};

// the move pointer
static const UWORD movePointer_bp0[] =
{
  0x3fe0, // ..#########...
  0x6030, // .##.......##..
  0xe038, // ###.......###.
  0x6030, // .##.......##..
  0x3fe0, // ..#########...
  0x0000  // ..............
};

static const UWORD movePointer_bp1[] =
{
  0x0000, // ..............
  0x1fc0, // ...#######....
  0x1000, // ...#..........
  0x100c, // ...#........##
  0x0018, // ...........##.
  0x1ff0  // ...#########..
};

static UWORD movePointer_bp2[] =
{
  0x0000,
  0x0000,
  0x0000,
  0x0000,
  0x0000,
  0x0000
};

static const UWORD movePointer[] =
{
//plane1    plane2
  0x0000,   0x0000,

  0x3fe0,   0x0000,
  0x6030,   0x1fc0,
  0xe038,   0x1000,
  0x6030,   0x100c,
  0x3fe0,   0x0018,
  0x0000,   0x1ff0,

  0x0000,   0x0000
};

static struct BitMap movePointerBitmap =
{
  2,6,0,2,0,
  { (PLANEPTR)movePointer_bp0,(PLANEPTR) movePointer_bp1,NULL,NULL,NULL,NULL,NULL,NULL }
};


// the selection pointer
static const UWORD selectPointer_bp0[] =
{
  0x8800,    // #...#..
  0x5000,    // .#.#...
  0x2000,    // ..#....
  0x2000,    // ..#....
  0x2000,    // ..#....
  0x2000,    // ..#....
  0x2000,    // ..#....
  0x2000,    // ..#....
  0x4000,    // .#.....
  0x2000,    // ..#....
  0x2000,    // ..#....
  0x2000,    // ..#....
  0x2000,    // ..#....
  0x2000,    // ..#....
  0x5000,    // .#.#...
  0x8800,    // #...#..
};

static const UWORD selectPointer_bp1[] =
{
  0x4600,    // .#...##
  0x2800,    // ..#.#..
  0x1000,    // ...#...
  0x1000,    // ...#...
  0x1000,    // ...#...
  0x1000,    // ...#...
  0x1000,    // ...#...
  0x1000,    // ...#...
  0x3800,    // ..###..
  0x1000,    // ...#...
  0x1000,    // ...#...
  0x1000,    // ...#...
  0x1000,    // ...#...
  0x1000,    // ...#...
  0x2800,    // ..#.#..
  0x4600,    // .#...##
};

static UWORD selectPointer_bp2[] =
{
  0x0000,
  0x0000,
  0x0000,
  0x0000,
  0x0000,
  0x0000,
  0x0000,
  0x0000,
  0x0000,
  0x0000,
  0x0000,
  0x0000,
  0x0000,
  0x0000,
  0x0000,
  0x0000,
};

static const UWORD selectPointer[] =
{
//plane1    plane2
  0x0000,   0x0000,

  0x8800,   0x4600,
  0x5000,   0x2800,
  0x2000,   0x1000,
  0x2000,   0x1000,
  0x2000,   0x1000,
  0x2000,   0x1000,
  0x2000,   0x1000,
  0x2000,   0x1000,
  0x4000,   0x3800,
  0x2000,   0x1000,
  0x2000,   0x1000,
  0x2000,   0x1000,
  0x2000,   0x1000,
  0x2000,   0x1000,
  0x5000,   0x2800,
  0x8800,   0x4600,

  0x0000,   0x0000
};

static struct BitMap selectPointerBitmap =
{
  2, 16, 0, 2, 0,
  { (PLANEPTR)selectPointer_bp0, (PLANEPTR)selectPointer_bp1, NULL, NULL, NULL, NULL, NULL }
};

static void IdentifyPointerColors(Object *obj)
{
  int i;
  ULONG colors[3*3];
  LONG blackDiff[3];
  LONG whiteDiff[3];
  LONG blackIndex;
  LONG whiteIndex;

  ENTER();

  for(i=0; i<16; i++)
  {
    sizePointer_bp2[i] = sizePointer_bp0[i] | sizePointer_bp1[i];
    movePointer_bp2[i] = movePointer_bp0[i] | movePointer_bp1[i];
    selectPointer_bp2[i] = selectPointer_bp0[i] | selectPointer_bp1[i];
  }

  // get the current screen's pointer colors (17 to 19)
  GetRGB32(_window(obj)->WScreen->ViewPort.ColorMap, 17, 3, colors);

  for(i=0; i < 3; i++)
  {
    LONG dr;
    LONG dg;
    LONG db;

    // normalize the colors to 8 bit per gun as GetRGB32() returns
    // 32bit left aligned values
    colors[i*3+0] >>= 24;
    colors[i*3+1] >>= 24;
    colors[i*3+2] >>= 24;

    // calculate the geometric difference to the color black (=0x00000000)
    dr = 0x00000000 - colors[i*3+0];
    dg = 0x00000000 - colors[i*3+1];
    db = 0x00000000 - colors[i*3+2];
    blackDiff[i] = dr * dr + dg * dg + db * db;

    // calculate the geometric difference to the color white (=0x000000ff)
    dr = 0x000000ff - colors[i*3+0];
    dg = 0x000000ff - colors[i*3+1];
    db = 0x000000ff - colors[i*3+2];
    whiteDiff[i] = dr * dr + dg * dg + db * db;
  }

  // the smallest difference defines the color which is closest to black or
  // equal to black
  if(blackDiff[0] > blackDiff[1])
  {
    if(blackDiff[1] > blackDiff[2])
      blackIndex = 19;
    else
      blackIndex = 18;
  }
  else if(blackDiff[0] > blackDiff[2])
    blackIndex = 19;
  else
    blackIndex = 17;

  // the smallest difference defines the color which is closest to white or
  // equal to white
  if(whiteDiff[0] > whiteDiff[1])
  {
    if(whiteDiff[1] > whiteDiff[2])
      whiteIndex = 19;
    else
      whiteIndex = 18;
  }
  else if(whiteDiff[0] > whiteDiff[2])
    whiteIndex = 19;
  else
    whiteIndex = 17;

  // Here we expect the user to have set up quite "different" colors. That
  // means the color closest to white will never be close to black and vice
  // versa. According to these differences we spread the required bitplanes.
  if(whiteIndex == 17)
  {
    if(blackIndex == 18)
    {
      sizePointerBitmap.Planes[0] = (PLANEPTR)sizePointer_bp0;
      sizePointerBitmap.Planes[1] = (PLANEPTR)sizePointer_bp1;
      movePointerBitmap.Planes[0] = (PLANEPTR)movePointer_bp0;
      movePointerBitmap.Planes[1] = (PLANEPTR)movePointer_bp1;
      selectPointerBitmap.Planes[0] = (PLANEPTR)selectPointer_bp0;
      selectPointerBitmap.Planes[1] = (PLANEPTR)selectPointer_bp1;
    }
    else // blackIndex == 19
    {
      sizePointerBitmap.Planes[0] = (PLANEPTR)sizePointer_bp2;
      sizePointerBitmap.Planes[1] = (PLANEPTR)sizePointer_bp1;
      movePointerBitmap.Planes[0] = (PLANEPTR)movePointer_bp2;
      movePointerBitmap.Planes[1] = (PLANEPTR)movePointer_bp1;
      selectPointerBitmap.Planes[0] = (PLANEPTR)selectPointer_bp2;
      selectPointerBitmap.Planes[1] = (PLANEPTR)selectPointer_bp1;
    }
  }
  else if(whiteIndex == 18)
  {
    if(blackIndex == 17)
    {
      sizePointerBitmap.Planes[0] = (PLANEPTR)sizePointer_bp1;
      sizePointerBitmap.Planes[1] = (PLANEPTR)sizePointer_bp0;
      movePointerBitmap.Planes[0] = (PLANEPTR)movePointer_bp1;
      movePointerBitmap.Planes[1] = (PLANEPTR)movePointer_bp0;
      selectPointerBitmap.Planes[0] = (PLANEPTR)selectPointer_bp1;
      selectPointerBitmap.Planes[1] = (PLANEPTR)selectPointer_bp0;
    }
    else // blackIndex == 19
    {
      sizePointerBitmap.Planes[0] = (PLANEPTR)sizePointer_bp1;
      sizePointerBitmap.Planes[1] = (PLANEPTR)sizePointer_bp2;
      movePointerBitmap.Planes[0] = (PLANEPTR)movePointer_bp1;
      movePointerBitmap.Planes[1] = (PLANEPTR)movePointer_bp2;
      selectPointerBitmap.Planes[0] = (PLANEPTR)selectPointer_bp1;
      selectPointerBitmap.Planes[1] = (PLANEPTR)selectPointer_bp2;
    }
  }
  else // whiteIndex == 19
  {
    if(blackIndex == 17)
    {
      sizePointerBitmap.Planes[0] = (PLANEPTR)sizePointer_bp2;
      sizePointerBitmap.Planes[1] = (PLANEPTR)sizePointer_bp0;
      movePointerBitmap.Planes[0] = (PLANEPTR)movePointer_bp2;
      movePointerBitmap.Planes[1] = (PLANEPTR)movePointer_bp0;
      selectPointerBitmap.Planes[0] = (PLANEPTR)selectPointer_bp2;
      selectPointerBitmap.Planes[1] = (PLANEPTR)selectPointer_bp0;
    }
    else // blackIndex == 18
    {
      sizePointerBitmap.Planes[0] = (PLANEPTR)sizePointer_bp2;
      sizePointerBitmap.Planes[1] = (PLANEPTR)sizePointer_bp0;
      movePointerBitmap.Planes[0] = (PLANEPTR)movePointer_bp2;
      movePointerBitmap.Planes[1] = (PLANEPTR)movePointer_bp0;
      selectPointerBitmap.Planes[0] = (PLANEPTR)selectPointer_bp0;
      selectPointerBitmap.Planes[1] = (PLANEPTR)selectPointer_bp2;
    }
  }

  LEAVE();
}
#endif

void SetupCustomPointers(struct NLData *data)
{
  ENTER();

  if(data->SizePointerObj == NULL)
  {
    #if defined(__amigaos4__)
    data->SizePointerObj = (Object *)NewObject(NULL, "pointerclass",
      POINTERA_ImageData,   sizePointer,
      POINTERA_Width,       sizePointerWidth,
      POINTERA_Height,      sizePointerHeight,
      POINTERA_BitMap,      &fakeSizeBitmap,
      POINTERA_XResolution, (ULONG)POINTERXRESN_SCREENRES,
      POINTERA_YResolution, (ULONG)POINTERYRESN_SCREENRESASPECT,
      POINTERA_XOffset,     (LONG)sizePointerXOffset,
      POINTERA_YOffset,     (LONG)sizePointerYOffset,
      TAG_DONE);
    #else
    if(((struct Library *)IntuitionBase)->lib_Version >= 39)
    {
      data->SizePointerObj = (Object *)NewObject(NULL, "pointerclass",
        POINTERA_BitMap,      (LONG)&sizePointerBitmap,
        POINTERA_WordWidth,   (ULONG)1,
        POINTERA_XResolution, (ULONG)POINTERXRESN_SCREENRES,
        POINTERA_YResolution, (ULONG)POINTERYRESN_SCREENRESASPECT,
        POINTERA_XOffset,     (LONG)sizePointerXOffset,
        POINTERA_YOffset,     (LONG)sizePointerYOffset,
        TAG_DONE);
    }
    else
    {
      if((data->SizePointerObj = (Object *)AllocVec(sizeof(sizePointer), MEMF_CHIP|MEMF_PUBLIC)) != NULL)
        memcpy(data->SizePointerObj, sizePointer, sizeof(sizePointer));
    }
    #endif
  }

  if(data->MovePointerObj == NULL)
  {
    #if defined(__amigaos4__)
    data->MovePointerObj = (Object *)NewObject(NULL, "pointerclass",
      POINTERA_ImageData,   movePointer,
      POINTERA_Width,       movePointerWidth,
      POINTERA_Height,      movePointerHeight,
      POINTERA_BitMap,      &fakeMoveBitmap,
      POINTERA_XResolution, (ULONG)POINTERXRESN_SCREENRES,
      POINTERA_YResolution, (ULONG)POINTERYRESN_SCREENRESASPECT,
      POINTERA_XOffset,     (LONG)movePointerXOffset,
      POINTERA_YOffset,     (LONG)movePointerYOffset,
      TAG_DONE);
    #else
    if(((struct Library *)IntuitionBase)->lib_Version >= 39)
    {
      data->MovePointerObj = (Object *)NewObject(NULL, "pointerclass",
        POINTERA_BitMap,      (LONG)&movePointerBitmap,
        POINTERA_WordWidth,   (ULONG)1,
        POINTERA_XResolution, (ULONG)POINTERXRESN_SCREENRES,
        POINTERA_YResolution, (ULONG)POINTERYRESN_SCREENRESASPECT,
        POINTERA_XOffset,     (LONG)movePointerXOffset,
        POINTERA_YOffset,     (LONG)movePointerYOffset,
        TAG_DONE);
    }
    else
    {
      if((data->MovePointerObj = (Object *)AllocVec(sizeof(movePointer), MEMF_CHIP|MEMF_PUBLIC)) != NULL)
        memcpy(data->MovePointerObj, movePointer, sizeof(movePointer));
    }
    #endif
  }

  if(data->SelectPointerObj == NULL)
  {
    #if defined(__amigaos4__)
    data->SelectPointerObj = (Object *)NewObject(NULL, "pointerclass",
      POINTERA_ImageData,   selectPointer,
      POINTERA_Width,       selectPointerWidth,
      POINTERA_Height,      selectPointerHeight,
      POINTERA_BitMap,      &fakeSelectBitmap,
      POINTERA_XResolution, (ULONG)POINTERXRESN_SCREENRES,
      POINTERA_YResolution, (ULONG)POINTERYRESN_SCREENRESASPECT,
      POINTERA_XOffset,     (LONG)selectPointerXOffset,
      POINTERA_YOffset,     (LONG)selectPointerYOffset,
      TAG_DONE);
    #else
    if(((struct Library *)IntuitionBase)->lib_Version >= 39)
    {
      data->SelectPointerObj = (Object *)NewObject(NULL, "pointerclass",
        POINTERA_BitMap,      (LONG)&selectPointerBitmap,
        POINTERA_WordWidth,   (ULONG)1,
        POINTERA_XResolution, (ULONG)POINTERXRESN_SCREENRES,
        POINTERA_YResolution, (ULONG)POINTERYRESN_SCREENRESASPECT,
        POINTERA_XOffset,     (LONG)selectPointerXOffset,
        POINTERA_YOffset,     (LONG)selectPointerYOffset,
        TAG_DONE);
    }
    else
    {
      if((data->SelectPointerObj = (Object *)AllocVec(sizeof(selectPointer), MEMF_CHIP|MEMF_PUBLIC)) != NULL)
        memcpy(data->SelectPointerObj, selectPointer, sizeof(selectPointer));
    }
    #endif
  }

  LEAVE();
}

void CleanupCustomPointers(Object *obj, struct NLData *data)
{
  ENTER();

  // restore the original default WB pointer
  if(((struct Library *)IntuitionBase)->lib_Version >= 39)
    SetWindowPointer(_window(obj), TAG_DONE);
  else
    ClearPointer(_window(obj));

  // dispose the different pointer objects
  if(data->SizePointerObj != NULL)
  {
    if(((struct Library *)IntuitionBase)->lib_Version >= 39)
      DisposeObject(data->SizePointerObj);
    else
      FreeVec(data->SizePointerObj);

    data->SizePointerObj = NULL;
  }

  if(data->MovePointerObj != NULL)
  {
    if(((struct Library *)IntuitionBase)->lib_Version >= 39)
      DisposeObject(data->MovePointerObj);
    else
      FreeVec(data->MovePointerObj);

    data->MovePointerObj = NULL;
  }

  if(data->SelectPointerObj != NULL)
  {
    if(((struct Library *)IntuitionBase)->lib_Version >= 39)
      DisposeObject(data->SelectPointerObj);
    else
      FreeVec(data->SelectPointerObj);

    data->SelectPointerObj = NULL;
  }

  data->activeCustomPointer = PT_NONE;

  LEAVE();
}

void ShowCustomPointer(Object *obj, struct NLData *data, enum PointerType type)
{
  ENTER();

  // even if it seems to be a waste of performance, but
  // we unfortunately have to always set the window pointer
  // regardless of the point if it was previously set or not.
  // This is required as any other gadget or process might
  // reset the window pointer and as such we would end up
  // with no custom pointer as well. So we only check the window
  // sleep status here :(
  if(xget(_win(obj), MUIA_Window_Sleep) == FALSE)
  {
    Object *ptrObject = NULL;

    switch(type)
    {
      case PT_SIZE:
        ptrObject = data->SizePointerObj;
      break;

      case PT_MOVE:
        ptrObject = data->MovePointerObj;
      break;

      case PT_SELECT:
        ptrObject = data->SelectPointerObj;
      break;

      case PT_NONE:
        ptrObject = NULL;
      break;
    }

    if(ptrObject != NULL)
    {
      #if defined(__amigaos4__)
      SetWindowPointer(_window(obj), WA_Pointer, ptrObject, TAG_DONE);
      #else
      if(((struct Library *)IntuitionBase)->lib_Version >= 39)
      {
        // try to identify the black/white colors
        // of the current screen colormap
        IdentifyPointerColors(obj);

        SetWindowPointer(_window(obj), WA_Pointer, ptrObject, TAG_DONE);
      }
      else
      {
        ULONG height = 0;
        ULONG width = 0;
        ULONG xoffset = 0;
        ULONG yoffset = 0;

        switch(type)
        {
          case PT_SIZE:
          {
            height = sizePointerHeight;
            width = sizePointerWidth;
            xoffset = sizePointerXOffset;
            yoffset = sizePointerYOffset;
          }
          break;

          case PT_MOVE:
          {
            height = movePointerHeight;
            width = movePointerWidth;
            xoffset = movePointerXOffset;
            yoffset = movePointerYOffset;
          }
          break;

          case PT_SELECT:
          {
            height = selectPointerHeight;
            width = selectPointerWidth;
            xoffset = selectPointerXOffset;
            yoffset = selectPointerYOffset;
          }
          break;

          case PT_NONE:
            // nothing
          break;
        }

        SetPointer(_window(obj), (APTR)ptrObject, height, width, xoffset, yoffset);
      }
      #endif

      data->activeCustomPointer = type;
    }
    else
      HideCustomPointer(obj, data);
  }

  LEAVE();
}

void HideCustomPointer(Object *obj, struct NLData *data)
{
  ENTER();

  if(data->activeCustomPointer != PT_NONE)
  {
    if(((struct Library *)IntuitionBase)->lib_Version >= 39)
      SetWindowPointer(_window(obj), TAG_DONE);
    else
      ClearPointer(_window(obj));

    data->activeCustomPointer = PT_NONE;
  }

  LEAVE();
}
