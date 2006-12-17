#ifndef MUI_NLISTTREE_priv_MCP_H
#define MUI_NLISTTREE_priv_MCP_H

/***************************************************************************

 NListtree.mcc - New Listtree MUI Custom Class
 Copyright (C) 1999-2001 by Carsten Scholling
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

#include "amiga-align.h"
#include "../nlisttree_mcc/private.h"
#include "default-align.h"
#include "Debug.h"

#include <mcc_common.h>

#define PREFSIMAGEOBJECT \
  BodychunkObject,\
    MUIA_FixWidth,              IM_PREFS_WIDTH,\
    MUIA_FixHeight,             IM_PREFS_HEIGHT,\
    MUIA_Bitmap_Width,          IM_PREFS_WIDTH ,\
    MUIA_Bitmap_Height,         IM_PREFS_HEIGHT,\
    MUIA_Bodychunk_Depth,       IM_PREFS_DEPTH,\
    MUIA_Bodychunk_Body,        (UBYTE *)Im_Prefs_body,\
    MUIA_Bodychunk_Compression, IM_PREFS_COMPRESSION,\
    MUIA_Bodychunk_Masking,     IM_PREFS_MASKING,\
    MUIA_Bitmap_SourceColors,   (ULONG *)Im_Prefs_colors,\
    MUIA_Bitmap_Transparent,    0,\
  End

#define MCPMAXRAWBUF 64

struct NListtreeP_Data
{
	Object
			*WI_Sample,
				*NLT_Sample,
				*BT_Sample_Expand,
				*BT_Sample_Collapse,
				*BT_Sample_Close,

			*WI_Copyright,
				*BT_Copyright_Close,

			*GR_Prefs,
				*PI_ImageClosed,
				*PI_ImageOpen,
				*PI_ImageSpecial,

				*PP_LinePen,
				*PP_ShadowPen,
				*PP_DrawPen,
				*PP_Draw2Pen,

				*CY_Style,
				*SL_Space,
				*CH_RememberStatus,
				*CH_OpenAutoScroll,

			*BT_OpenSample,
			*BT_OpenCopyright;
	//Object	*BT_Test;

	BOOL	SampleWasOpen,
			CopyrightWasOpen;
};

#endif /* MUI_NLISTTREE_priv_MCP_H */
