#ifndef MUI_NLISTVIEWS_priv_MCP_H
#define MUI_NLISTVIEWS_priv_MCP_H

/***************************************************************************

 NListviews.mcp - New Listview MUI Custom Class Preferences
 Registered MUI class, Serial Number: 1d51 (0x9d510001 to 0x9d51001F
                                            and 0x9d510101 to 0x9d51013F)

 Copyright (C) 1996-2004 by Gilles Masson,
                            Carsten Scholling <aphaso@aphaso.de>,
                            Przemyslaw Grunchala,
                            Sebastian Bauer <sebauer@t-online.de>,
                            Jens Langner <Jens.Langner@light-speed.de>

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

extern UWORD LIBVER;
#define LIBVER(lib)          ((struct Library *)lib)->lib_Version

#ifndef MUI_MUI_H
  #include "mui.h"
#endif

#if defined(__PPC__)
  #pragma pack(2)
  #include "NListviews_mcp.h"
  #pragma pack()
#else
  #include "NListviews_mcp.h"
#endif

#include <mcc_common.h>
#include <mcc_debug.h>

#define PREFSIMAGEOBJECT \
  BodychunkObject,\
    MUIA_FixWidth,              LIST_WIDTH,\
    MUIA_FixHeight,             LIST_HEIGHT,\
    MUIA_Bitmap_Width,          LIST_WIDTH ,\
    MUIA_Bitmap_Height,         LIST_HEIGHT,\
    MUIA_Bodychunk_Depth,       LIST_DEPTH,\
    MUIA_Bodychunk_Body,        (UBYTE *)list_body,\
    MUIA_Bodychunk_Compression, LIST_COMPRESSION,\
    MUIA_Bodychunk_Masking,     LIST_MASKING,\
    MUIA_Bitmap_SourceColors,   (ULONG *)list_colors,\
    MUIA_Bitmap_Transparent,    0,\
  End

#define MCPMAXRAWBUF 64

struct NListviews_MCP_Data
{
  APTR mcp_group;

  APTR mcp_list1;
  APTR mcp_list2;

  APTR mcp_PenTitle;
  APTR mcp_PenList;
  APTR mcp_PenSelect;
  APTR mcp_PenCursor;
  APTR mcp_PenUnselCur;
  APTR mcp_BG_Title;
  APTR mcp_BG_List;
  APTR mcp_BG_Select;
  APTR mcp_BG_Cursor;
  APTR mcp_BG_UnselCur;

  APTR mcp_R_Multi;
  APTR mcp_B_MultiMMB;
  APTR mcp_R_Drag;
  APTR mcp_ST_DragQualifier;
  APTR mcp_SL_VertInc;
  APTR mcp_R_HSB;
  APTR mcp_R_VSB;
  APTR mcp_B_Smooth;

  APTR mcp_Font;
  APTR mcp_Font_Little;
  APTR mcp_Font_Fixed;

  APTR mcp_ForcePen;
  APTR mcp_StackCheck;
  APTR mcp_ColWidthDrag;
  APTR mcp_PartialCol;
  APTR mcp_List_Select;
  APTR mcp_NList_Menu;
  APTR mcp_PartialChar;
  APTR mcp_PointerColor;
  APTR mcp_SerMouseFix;
  APTR mcp_DragLines;
  APTR mcp_VerticalCenteredLines;

  APTR mcp_WheelStep;
  APTR mcp_WheelFast;
  APTR mcp_WheelMMB;

  APTR mcp_listkeys;
  APTR mcp_stringkey;
  APTR mcp_snoopkey;
  APTR mcp_insertkey;
  APTR mcp_removekey;
  APTR mcp_updatekeys;
  APTR mcp_defaultkeys;
  APTR mcp_txtfct;
  APTR mcp_popstrfct;
  APTR mcp_poplistfct;

  struct KeyBinding *nlkeys;
  LONG nlkeys_size;
  char QualifierString[192];

  char rawtext[MCPMAXRAWBUF];

  struct InputEvent ievent;

  WORD pad1,pad2;
};

#endif /* MUI_NLISTVIEWS_priv_MCP_H */

