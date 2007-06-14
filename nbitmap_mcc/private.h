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

#ifndef NBITMAP_MCC_PRIV_H
#define NBITMAP_MCC_PRIV_H

/* system includes */
#include <dos/exall.h>
#include <exec/types.h>
#include <intuition/classusr.h>
#include <libraries/mui.h>

/* mcc includes */
#include <mcc_common.h>

/* local includes */
#include "Debug.h"
#include "NBitmap.h"

/* private definitions */
#define MUIV_NBitmap_Normal     0
#define MUIV_NBitmap_Ghosted		1
#define MUIV_NBitmap_Selected		2

/* global definitions - as included in developers material */
#define MUIA_NBitmap_Type				 (TAG_USER | (2044078415<<16) | 0x0000)
#define MUIA_NBitmap_Label			    (TAG_USER | (2044078415<<16) | 0x0001)
#define MUIA_NBitmap_Button			 (TAG_USER | (2044078415<<16) | 0x0002)
#define MUIA_NBitmap_Normal			 (TAG_USER | (2044078415<<16) | 0x0003)
#define MUIA_NBitmap_Ghosted		    (TAG_USER | (2044078415<<16) | 0x0004)
#define MUIA_NBitmap_Selected		    (TAG_USER | (2044078415<<16) | 0x0005)
#define MUIA_NBitmap_Width			    (TAG_USER | (2044078415<<16) | 0x0006)
#define MUIA_NBitmap_Height		    (TAG_USER | (2044078415<<16) | 0x0007)

#define MUIV_NBitmap_Type_File	    0
#define MUIV_NBitmap_Type_DTObject	1

/* private structures */
struct PrefsData
{
  uint8 show_label;
	uint8 overlay_type;
	uint8 overlay_r;
	uint8 overlay_g;
	uint8 overlay_b;
	double overlay_shadeover;
	double overlay_shadepress;
	uint8 spacing_horiz;
	uint8 spacing_vert;
};

struct InstData
{
  BOOL button;
	BOOL overlay;
	BOOL pressed;
	STRPTR label;

	uint32 scrdepth;

	uint32 *data[3];
	uint32 fmt, type;
	uint32 width, height, depth;
	uint32 border_horiz, border_vert;
	uint32 arraybpp, arraybpr, arraysize;
	APTR arraypixels[3];
		
	uint32 *dt_colours[3];
	Object *dt_obj[3];
	PLANEPTR *dt_mask[3];

	struct PrefsData prefs;
	struct BitMap *dt_bitmap[3];
	struct BitMapHeader *dt_header[3];
};

/* macros */
#define _id(obj) (muiNotifyData(obj)->mnd_ObjectID)
#define _between(a,x,b) ((x)>=(a) && (x)<=(b))
#define _isinobject(x,y) (_between(_mleft(obj),(x),_mright(obj)) && _between(_mtop(obj),(y),_mbottom(obj)))

#ifndef min
#define min(a, b) (((a) < (b)) ? (a):(b))
#endif

#ifndef max
#define max(a, b) (((a) > (b)) ? (a):(b))
#endif

/* prototypes */
BOOL NBitmap_LoadImage(STRPTR filename, uint32 item, struct IClass *cl, Object *obj);
VOID NBitmap_UpdateImage(uint32 item, STRPTR filename, struct IClass *cl, Object *obj);
BOOL NBitmap_ExamineData(Object *dt_obj, uint32 item, struct IClass *cl, Object *obj);
VOID NBitmap_FreeImage(uint32 item, struct IClass *cl, Object *obj);
BOOL NBitmap_NewImage(struct IClass *cl, Object *obj);
VOID NBitmap_DisposeImage(struct IClass *cl, Object *obj);
VOID NBitmap_SetupImage(struct IClass *cl, Object *obj);
VOID NBitmap_CleanupImage(struct IClass *cl, Object *obj);
BOOL NBitmap_DrawImage(struct IClass *cl, Object *obj);

#endif /* NBITMAP_MCC_PRIV_H */
