#ifndef NBITMAP_MCC_H
#define NBITMAP_MCC_H

/***************************************************************************

 NBitmap.mcc - New Bitmap MUI Custom Class
 Copyright (C) 2006 by Daniel Allsopp
 Copyright (C) 2007 by NList Open Source Team

 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.

 NList classes Support Site:  http://www.sf.net/projects/nlist-classes

 $Id: library.c 125 2006-12-17 00:15:23Z damato $

***************************************************************************/

#ifndef EXEC_TYPES_H
#include <exec/types.h>
#endif

#define MUIC_NBitmap                "NBitmap.mcc"
#define NBitmapObject               MUI_NewObject(MUIC_NBitmap

/* attributes */
#define MUIA_NBitmap_Type           (TAG_USER | (2044078415<<16) | 0x0000)
#define MUIA_NBitmap_Label          (TAG_USER | (2044078415<<16) | 0x0001)
#define MUIA_NBitmap_Button         (TAG_USER | (2044078415<<16) | 0x0002)
#define MUIA_NBitmap_Normal         (TAG_USER | (2044078415<<16) | 0x0003)
#define MUIA_NBitmap_Ghosted        (TAG_USER | (2044078415<<16) | 0x0004)
#define MUIA_NBitmap_Selected       (TAG_USER | (2044078415<<16) | 0x0005)

/* source types */
#define MUIV_NBitmap_Type_File      0
#define MUIV_NBitmap_Type_DTObject  1

/* macros */
#define NBitmapFile(filename)       NBitmapObject, \
                                      MUIA_NBitmap_Type, MUIV_NBitmap_Type_File, \
                                      MUIA_NBitmap_Normal, (filename), \
                                    End

#endif /* NBITMAP_MCC_H */
