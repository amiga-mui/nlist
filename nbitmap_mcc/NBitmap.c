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

// ansi includes
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

// system includes
#include <proto/datatypes.h>
#include <proto/dos.h>
#include <proto/exec.h>
#include <proto/gadtools.h>
#include <proto/graphics.h>
#include <proto/intuition.h>
#include <proto/layers.h>
#include <proto/muimaster.h>
#include <proto/Picasso96API.h>
#include <proto/utility.h>

// system
#include <datatypes/pictureclass.h>
#include <dos/dosextens.h>
#include <dos/dostags.h>
#include <exec/execbase.h>
#include <exec/lists.h>
#include <exec/nodes.h>
#include <graphics/layers.h>
#include <graphics/blitattr.h>

// libraries
#include <libraries/gadtools.h>
#include <libraries/mui.h>
#include <libraries/amigaguide.h>

// local includes
#include "NBitmap.h"
#include "private.h"
#include "rev.h"
#include "Debug.h"

// functions
/// GetConfigItem()
//
ULONG GetConfigItem(Object *obj, ULONG configitem, ULONG defaultsetting)
{
  ULONG value;
  ULONG result = defaultsetting;

  ENTER();

  if(DoMethod(obj, MUIM_GetConfigItem, configitem, &value))
    result = *(ULONG *)value;

  RETURN(result);
  return result;
}
///
/// InitConfig()
//
VOID InitConfig(Object *obj, struct InstData *data)
{
  ENTER();

  if(obj != NULL && data != NULL)
  {
    data->prefs.show_label = 0;
    data->prefs.overlay_type = 0;
    data->prefs.overlay_r = 10;
    data->prefs.overlay_g = 36;
    data->prefs.overlay_b = 106;
    data->prefs.overlay_shadeover = 1.5;
    data->prefs.overlay_shadepress = 2.5;
    data->prefs.spacing_horiz = 2;
    data->prefs.spacing_vert = 2;
  }

  LEAVE();
}
///
/// FreeConfig()
//
VOID FreeConfig(struct InstData *data)
{
  ENTER();

  if(data != NULL)
  {
    // nothing yet
  }

  LEAVE();
}
///
/// NBitmap_LoadImage()
//
BOOL NBitmap_LoadImage(STRPTR filename, uint32 item, struct IClass *cl, Object *obj)
{
  BOOL result = FALSE;
  struct InstData *data;

  if((data = INST_DATA(cl, obj)) != NULL && filename != NULL)
  {
    data->dt_obj[item] = NewDTObject(filename,
                                     DTA_GroupID,           GID_PICTURE,
                                     OBP_Precision,         PRECISION_EXACT,
                                     PDTA_FreeSourceBitMap, TRUE,
                                     PDTA_DestMode,         PMODE_V43,
                                     PDTA_UseFriendBitMap,  TRUE,
                                     TAG_DONE);
    SHOWVALUE(DBF_DATATYPE, data->dt_obj[item]);
    if(data->dt_obj[item] != NULL)
      result = TRUE;
  }

  RETURN(result);
  return result;
}
///
/// NBitmap_FreeImage()
//
VOID NBitmap_FreeImage(uint32 item, struct IClass *cl, Object *obj)
{
	struct InstData *data = NULL;

	if(((data = INST_DATA(cl, obj)) != NULL))
	{
		if(data->dt_obj[item] != NULL)
		{
			DisposeDTObject(data->dt_obj[item]);
			data->dt_obj[item] = NULL;

			if(data->arraypixels[item])
			{
				FreeVec(data->arraypixels[item]);
				data->arraypixels[item] = NULL;
			}
		}
	}
}

///
/// NBitmap_NewImage()
//
BOOL NBitmap_NewImage(struct IClass *cl, Object *obj)
{
  BOOL result = FALSE;
  struct InstData *data;

  ENTER();

  /* need at least the normal image */
  if((data = INST_DATA(cl, obj)) !=NULL && data->dt_obj[0] != NULL)
  {
    uint32 i;

    for(i = 0; i < 3; i++)
    {
      if(data->dt_obj[i] != NULL)
      {
        struct FrameInfo fri;

        SetMem(&fri, 0, sizeof(struct FrameInfo));
        DoMethod(data->dt_obj[0], DTM_FRAMEBOX, NULL, &fri, &fri, sizeof(struct FrameInfo), 0);
        data->depth = fri.fri_Dimensions.Depth;

        if(data->depth > 0 && data->depth <= 8)
        {
          /* colour lookup bitmap */
          data->fmt = PBPAFMT_LUT8;

          /* bitmap header */
          GetDTAttrs(data->dt_obj[i], PDTA_BitMapHeader, &data->dt_header[i], TAG_DONE);
          data->width =  data->dt_header[0]->bmh_Width;
          data->height =  data->dt_header[0]->bmh_Height;

          result = TRUE;
        }
        else if(data->depth > 8)
        {
          uint32 arraysize;

          /* correct read buffer */
          if(data->depth == 24)
            data->fmt = PBPAFMT_RGB;
          else
            data->fmt = PBPAFMT_ARGB;

          /* bitmap header */
          GetDTAttrs(data->dt_obj[i], PDTA_BitMapHeader, &data->dt_header[i], TAG_DONE);
          data->width =  data->dt_header[0]->bmh_Width;
          data->height =  data->dt_header[0]->bmh_Height;
          data->arraybpp = data->depth / 8;
          data->arraybpr = data->arraybpp * data->width;
          arraysize = (data->arraybpr) * data->height;

          /* get array of pixels */
          if((data->arraypixels[i] = AllocVec(arraysize, MEMF_ANY|MEMF_CLEAR)) != NULL)
          {
            DoMethod(data->dt_obj[i], PDTM_READPIXELARRAY, data->arraypixels[i], data->fmt, data->arraybpr, 0, 0, data->width, data->height);

            result = TRUE;
          }
        }
      }
    }
  }

  RETURN(result);
  return result;
}
///
/// NBitmap_DisposeImage()
//
VOID NBitmap_DisposeImage(struct IClass *cl, Object *obj)
{
  struct InstData *data;

  ENTER();

  if((data = INST_DATA(cl, obj))!=NULL)
  {
    uint32 i;

    /* free datatype object */
    if(data->type == MUIV_NBitmap_Type_File)
    {
      for(i =0 ; i < 3; i++)
      {
        SHOWVALUE(DBF_DATATYPE, data->dt_obj[i]);
        if(data->dt_obj[i] != NULL)
        {
          DisposeDTObject(data->dt_obj[i]);
          data->dt_obj[i] = NULL;
        }
      }
    }

    if(data->label != NULL)
    {
      FreeVec(data->label);
      data->label = NULL;
    }

    /* free pixel memory */
    for(i = 0; i < 3; i++)
    {
      if(data->arraypixels[i] != NULL)
      {
        FreeVec(data->arraypixels[i]);
        data->arraypixels[i] = NULL;
      }
    }
  }

  LEAVE();
}
///
/// NBitmap_SetupImage()
//
VOID NBitmap_SetupImage(struct IClass *cl, Object *obj)
{
  struct InstData *data;

  ENTER();

  if((data = INST_DATA(cl, obj)) != NULL)
  {
    uint32 displayid;
    struct Screen *scr = NULL;

    /* stored config */
    InitConfig(obj, data);

    /* screen */
    scr = _screen(obj);
    GetScreenAttr(scr, SA_DisplayID, &displayid, sizeof(uint32));
    data->scrdepth = p96GetModeIDAttr(displayid, P96IDA_DEPTH);

    /* input */
    if(data->button)
      MUI_RequestIDCMP(obj, IDCMP_MOUSEBUTTONS | IDCMP_GADGETUP | IDCMP_GADGETDOWN | IDCMP_MOUSEMOVE);

    /* 8-bit data */
    if(data->fmt == PBPAFMT_LUT8 && data->dt_obj[0] != NULL)
    {
      uint32 i;

      /* layout image */
      for(i = 0; i < 3; i++)
      {
        // set the new screen for this object
        SetDTAttrs(data->dt_obj[i], NULL, NULL, PDTA_Screen, scr, TAG_DONE);
        if(DoMethod(data->dt_obj[i], DTM_PROCLAYOUT, NULL, 1))
        {
          GetDTAttrs(data->dt_obj[i], PDTA_CRegs, &data->dt_colours[i],
                                      PDTA_MaskPlane, &data->dt_mask[i],
                                      PDTA_DestBitMap, &data->dt_bitmap[i],
                                      TAG_DONE);
          if(data->dt_bitmap[i] == NULL)
            GetDTAttrs(data->dt_obj[i], PDTA_BitMap, &data->dt_bitmap[i], TAG_DONE);
          SHOWVALUE(DBF_DATATYPE, data->dt_bitmap[i]);
        }
      }
    }
  }

  LEAVE();
}
///
/// NBitmap_CleanupImage()
//
VOID NBitmap_CleanupImage(struct IClass *cl, Object *obj)
{
  struct InstData *data;

  if((data = INST_DATA(cl, obj)) != NULL)
  {
    // input
    if(data->button)
      MUI_RejectIDCMP(obj, IDCMP_MOUSEBUTTONS | IDCMP_GADGETUP | IDCMP_GADGETDOWN | IDCMP_MOUSEMOVE);

    if(data->fmt == PBPAFMT_LUT8 && data->dt_obj[0] != NULL)
    {
      uint32 i;

      /* layout image */
      for(i = 0; i < 3; i++)
      {
        // reset the screen pointer
        SetDTAttrs(data->dt_obj[i], NULL, NULL, PDTA_Screen, NULL, TAG_DONE);
      }
    }

    // stored config
    FreeConfig(data);
  }
}
///
/// NBitmap_DrawSimpleFrame()
//
VOID NBitmap_DrawSimpleFrame(Object *obj, uint32 x, uint32 y, uint32 w, uint32 h)
{
  ENTER();

  SetAPen(_rp(obj), _pens(obj)[MPEN_SHADOW]);
  Move(_rp(obj), x, y+(h+1));
  Draw(_rp(obj), x, y);
  Draw(_rp(obj), x+(w+1), y);

  SetAPen(_rp(obj), _pens(obj)[MPEN_SHINE]);
  Draw(_rp(obj), x+(w+1), y+(h+1));
  Draw(_rp(obj), x, y+(h+1));

  LEAVE();
}
///
/// NBitmap_DrawImage()
//
BOOL NBitmap_DrawImage(struct IClass *cl, Object *obj)
{
  struct InstData *data;

  ENTER();

  if((data = INST_DATA(cl, obj)) != NULL)
  {
    int32 item;
    uint32 twidth, theight;

    /* coordinates */
    item = 0;

    twidth = (data->width + data->border_horiz) - 2;      /* subtract standard 1 pixel border */
    theight = (data->height + data->border_vert) - 2;

    /* clear */
    if(data->button)
      DoMethod(obj, MUIM_DrawBackground, _left(obj), _top(obj), _width(obj), _height(obj), 0, 0);

    /* draw image */
    if(data->fmt == PBPAFMT_LUT8)
    {
      uint32 error;

      /* select bitmap */
      if(data->button && data->pressed && data->overlay && data->dt_bitmap[2] != NULL)
        item = 2;

      SHOWVALUE(DBF_DRAW, item);
      SHOWVALUE(DBF_DRAW, data->dt_bitmap[item]);
      SHOWVALUE(DBF_DRAW, data->dt_mask[item]);

      error = BltBitMapTags(BLITA_Source, data->dt_bitmap[item],
                            BLITA_Dest, _rp(obj),
                            BLITA_SrcX, 0,
                            BLITA_SrcY, 0,
                            BLITA_DestX, _left(obj) + (data->border_horiz / 2),
                            BLITA_DestY, _top(obj) + (data->border_vert / 2),
                            BLITA_Width, data->width,
                            BLITA_Height, data->height,
                            BLITA_SrcType, BLITT_BITMAP,
                            BLITA_DestType, BLITT_RASTPORT,
                            BLITA_MaskPlane, data->dt_mask[item],
                            TAG_DONE);
      SHOWVALUE(DBF_DRAW, error);
    }
    else
    {
      /* select bitmap */
      if(data->button && data->pressed && data->overlay && data->arraypixels[2] != NULL)
        item = 2;

      SHOWVALUE(DBF_DRAW, item);
      SHOWVALUE(DBF_DRAW, data->arraypixels[item]);

      if(data->arraypixels[item] != NULL)
      {
		  int32 srctype;
        uint32 error;

		  if(data->depth == 24)
			 srctype = BLITT_RGB24;
		  else
			 srctype = BLITT_ARGB32;

        error = BltBitMapTags(BLITA_Source, data->arraypixels[item],
                              BLITA_Dest, _rp(obj),
                              BLITA_SrcX, 0,
                              BLITA_SrcY, 0,
                              BLITA_DestX, _left(obj) + (data->border_horiz / 2),
                              BLITA_DestY, _top(obj) + (data->border_vert / 2),
                              BLITA_Width, data->width,
                              BLITA_Height, data->height,
										BLITA_SrcType, srctype,
                              BLITA_DestType, BLITT_RASTPORT,
                              BLITA_SrcBytesPerRow, data->arraybpr,
                              BLITA_UseSrcAlpha, TRUE,
                              TAG_DONE);
        SHOWVALUE(DBF_DRAW, error);
      }
    }

    /* overlay */
    if(data->button && data->overlay)
    {
      if(data->prefs.overlay_type == 1 || data->scrdepth <= 8)
      {
        /* standard overlay */
        if(data->pressed)
          NBitmap_DrawSimpleFrame(obj, _left(obj), _top(obj), data->width, data->height);
        else
          NBitmap_DrawSimpleFrame(obj, _left(obj), _top(obj), data->width, data->height);
      }
      else
      {
        uint8 r, g, b;
        double shade;
        uint32 size;
        uint8 *array;

        /* shaded overlay */
        r = data->prefs.overlay_r;
        g = data->prefs.overlay_g;
        b = data->prefs.overlay_b;

        if(data->pressed)
          shade = data->prefs.overlay_shadepress;
        else
          shade = data->prefs.overlay_shadeover;

        /* */
        size = (twidth * theight) * 3;
        if((array = AllocVec(size, MEMF_ANY|MEMF_CLEAR)) != NULL)
        {
          struct TrueColorInfo tinfo;
          uint32 h;
          uint8 *p = array;
          uint32 pxl;

          SetMem(&tinfo, 0, sizeof(struct TrueColorInfo));
          tinfo.PixelDistance = 3;
          tinfo.BytesPerRow = twidth * 3;
          tinfo.RedData = array;
          tinfo.GreenData = array + 1;
          tinfo.BlueData = array + 2;

          p96ReadTrueColorData(&tinfo, 0, 0, _rp(obj), _left(obj) + 1, _top(obj) + 1, twidth, theight);

          for(h = 0; h < theight; h++)
          {
            uint32 w;

            for(w = 0; w < twidth; w++)
            {
              *p = (*p + r) / shade;
              p++;
              *p = (*p + g) / shade;
              p++;
              *p = (*p + b) / shade;
              p++;
            }
          }

          pxl = ((uint32)r << 16) | ((uint32)g << 8) | (uint32)b;
          p96RectFill(_rp(obj), _left(obj), _top(obj), _left(obj) + twidth + 1, _top(obj) + theight + 1, pxl);
          p96WriteTrueColorData(&tinfo, 0, 0, _rp(obj), _left(obj) + 1, _top(obj) + 1, twidth, theight);

          FreeVec(array);
        }
      }
    }
  }

  RETURN(FALSE);
  return FALSE;
}
///

