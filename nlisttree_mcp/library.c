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

/******************************************************************************/
/*                                                                            */
/* includes                                                                   */
/*                                                                            */
/******************************************************************************/

#include <proto/exec.h>
#include <proto/intuition.h>

/******************************************************************************/
/*                                                                            */
/* MCC/MCP name and version                                                   */
/*                                                                            */
/* ATTENTION:  The FIRST LETTER of NAME MUST be UPPERCASE                     */
/*                                                                            */
/******************************************************************************/

#include "private.h"
#include "version.h"

#define	VERSION				LIB_VERSION
#define	REVISION			LIB_REVISION

#define CLASS				  MUIC_NListtreeP
#define SUPERCLASSP		MUIC_Mccprefs

#define	INSTDATAP     NListtreeP_Data

#define USERLIBID     CLASS " " LIB_REV_STRING " [" SYSTEMSHORT "/" CPU "] (" LIB_DATE ") " LIB_COPYRIGHT
#define MASTERVERSION	19

#define	CLASSINIT
#define	CLASSEXPUNGE

#define MIN_STACKSIZE 8192

#include "locale.h"

struct Library *LocaleBase = NULL;

#if defined(__amigaos4__)
struct LocaleIFace *ILocale = NULL;
#endif

/******************************************************************************/
/* define the functions used by the startup code ahead of including mccinit.c */
/******************************************************************************/
static BOOL ClassInit(UNUSED struct Library *base);
static VOID ClassExpunge(UNUSED struct Library *base);

/******************************************************************************/
/* include the lib startup code for the mcc/mcp  (and muimaster inlines)      */
/******************************************************************************/
#define USE_IM_PREFS_BODY   1
#define USE_IM_PREFS_COLORS 1
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
    MUIA_Bitmap_RawData,        icon32,\
    MUIA_Bitmap_RawDataFormat,  MUIV_Bitmap_RawDataFormat_ARGB32,\
  End
#include "icon.bh"
#include "icon32.h"
#include "mccinit.c"

/******************************************************************************/
/* define all implementations of our user functions                           */
/******************************************************************************/

static BOOL ClassInit(UNUSED struct Library *base)
{
  if((LocaleBase = OpenLibrary("locale.library", 38)) &&
    GETINTERFACE(ILocale, struct LocaleIFace *, LocaleBase))
  {
    // open the NListtree_mcp catalog
    OpenCat();

    return(TRUE);
  }

	return(FALSE);
}


static VOID ClassExpunge(UNUSED struct Library *base)
{
  // close the catalog
	CloseCat();

  if(LocaleBase)
	{
    DROPINTERFACE(ILocale);
		CloseLibrary(LocaleBase);
		LocaleBase	= NULL;
	}
}
