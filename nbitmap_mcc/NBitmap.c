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

// system
#include <cybergraphx/cybergraphics.h>
#include <datatypes/pictureclass.h>
#include <dos/dosextens.h>
#include <dos/dostags.h>
#include <exec/execbase.h>
#include <exec/lists.h>
#include <exec/nodes.h>
#include <graphics/layers.h>

#if defined(__amigaos4__)
#include <graphics/blitattr.h>
#endif

// system includes
#include <proto/cybergraphics.h>
#include <proto/datatypes.h>
#include <proto/dos.h>
#include <proto/exec.h>
#include <proto/gadtools.h>
#include <proto/graphics.h>
#include <proto/intuition.h>
#include <proto/layers.h>
#include <proto/muimaster.h>
#include <proto/utility.h>

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
/// NBitmap_UpdateImage()
//
VOID NBitmap_UpdateImage(uint32 item, STRPTR filename, struct IClass *cl, Object *obj)
  {
    struct InstData *data = NULL;
    struct Screen *scr = NULL;

    if((data = INST_DATA(cl, obj)) != NULL)
    {
      if(filename != NULL)
      {
        if(data->dt_obj[item] != NULL)
        {
          /* free old image data */
          if(data->fmt == PBPAFMT_LUT8)
            SetDTAttrs(data->dt_obj[item], NULL, NULL, PDTA_Screen, NULL, TAG_DONE);

          DisposeDTObject(data->dt_obj[item]);
          data->dt_obj[item] = NULL;

          if(data->arraypixels[item] != NULL)
          {
            FreeVec(data->arraypixels[item]);
            data->arraypixels[item] = NULL;
          }

          /* load new image */
          if((NBitmap_LoadImage(filename, item, cl, obj)) != FALSE)
          {
            /* setup new image */
            if((NBitmap_ExamineData(data->dt_obj[item], item, cl, obj)) != FALSE)
            {
              /* screen */
              scr = _screen(obj);
              data->scrdepth = GetBitMapAttr(scr->RastPort.BitMap, BMA_DEPTH);

              if(data->fmt == PBPAFMT_LUT8)
              {
                /* layout image */
                SetDTAttrs(data->dt_obj[item], NULL, NULL, PDTA_Screen, scr, TAG_DONE);
                if(DoMethod(data->dt_obj[item], DTM_PROCLAYOUT, NULL, 1))
                {
                  GetDTAttrs(data->dt_obj[item], PDTA_CRegs, &data->dt_colours[item], TAG_DONE);
                  GetDTAttrs(data->dt_obj[item], PDTA_MaskPlane, &data->dt_mask[item], TAG_DONE);
                  GetDTAttrs(data->dt_obj[item], PDTA_DestBitMap, &data->dt_bitmap[item], TAG_DONE);

                  if(data->dt_bitmap[item] == NULL) GetDTAttrs(data->dt_obj[item], PDTA_BitMap, &data->dt_bitmap[item], TAG_DONE);
                }
              }
            }
          }

        }
      }
    }
  }
///
/// NBitmap_ExamineData()
//
BOOL NBitmap_ExamineData(Object *dt_obj, uint32 item, struct IClass *cl, Object *obj)
  {
    BOOL result = FALSE;
    ULONG arraysize;

    struct pdtBlitPixelArray pbpa;
	 //struct FrameInfo fri;
    struct InstData *data = INST_DATA(cl, obj);

    if(dt_obj != NULL)
    {
      /* bitmap header */
		GetDTAttrs(dt_obj, PDTA_BitMapHeader, &data->dt_header[item], TAG_DONE);
		D(DBF_DATATYPE, "examine: BMHD dimensions %ldx%ldx%ld", data->dt_header[item]->bmh_Width, data->dt_header[item]->bmh_Height, data->dt_header[item]->bmh_Depth);
		data->depth = data->dt_header[item]->bmh_Depth;

		//memset(&fri, 0, sizeof(struct FrameInfo));
		//DoMethod(dt_obj, DTM_FRAMEBOX, NULL, &fri, &fri, sizeof(struct FrameInfo), 0);
		//data->depth = fri.fri_Dimensions.Depth;

      if(data->depth>0 && data->depth<=8)
      {
        /* colour lookup bitmap */
        data->fmt = PBPAFMT_LUT8;

        /* bitmap header */
        data->width =  data->dt_header[0]->bmh_Width;
        data->height =  data->dt_header[0]->bmh_Height;

        result = TRUE;
        D(DBF_DATATYPE, "examine: using LUT8 bitmaps");
      }
      else if(data->depth >=24)
      {
        /* true colour bitmap */
        if(data->depth == 24)
        {
          data->fmt = PBPAFMT_RGB;
          D(DBF_DATATYPE, "examine: using 24bit RGB data");
        }
        else if(data->depth == 32)
        {
          data->fmt = PBPAFMT_ARGB;
          D(DBF_DATATYPE, "examine: using 32bit ARGB data");
        }

        data->width =  data->dt_header[0]->bmh_Width;
        data->height =  data->dt_header[0]->bmh_Height;
        data->arraybpp = data->depth/8;
        data->arraybpr = data->arraybpp * data->width;
        arraysize = (data->arraybpr) * data->height;

        /* get array of pixels */
        if((data->arraypixels[item] = AllocVec(arraysize, MEMF_ANY|MEMF_CLEAR)) != NULL)
        {
          ULONG error;

          memset(&pbpa, 0, sizeof(struct pdtBlitPixelArray));

          pbpa.MethodID = PDTM_READPIXELARRAY;
          pbpa.pbpa_PixelData = data->arraypixels[item];
          pbpa.pbpa_PixelFormat = data->fmt;
          pbpa.pbpa_PixelArrayMod = data->arraybpr;
          pbpa.pbpa_Left = 0;
          pbpa.pbpa_Top = 0;
          pbpa.pbpa_Width = data->width;
          pbpa.pbpa_Height = data->height;

          error = DoMethodA(dt_obj, (Msg)(VOID*)&pbpa);
          D(DBF_DATATYPE, "examine: READPIXELARRAY returned %ld", error);

          result = TRUE;
        }
      }
    }

    return(result);
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

      if(data->arraypixels[item] != NULL)
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

  if(((data = INST_DATA(cl, obj))!=NULL) && (data->dt_obj[0]))
  {
	 ULONG i;

    for(i=0;i<3;i++)
	 {
		if(data->dt_obj[i] != NULL) result = NBitmap_ExamineData(data->dt_obj[i], i, cl, obj);
    }
  }

  RETURN(result);
  return result;
}

BOOL NBitmap_OldNewImage(struct IClass *cl, Object *obj)
{
  BOOL result = FALSE;
  struct InstData *data;
  
  /* need at least the normal image */
  if((data = INST_DATA(cl, obj)) !=NULL && data->dt_obj[0] != NULL)
  {
    ULONG i;

    for(i = 0; i < 3; i++)
    {
      if(data->dt_obj[i] != NULL)
      {
        struct FrameInfo fri;

        memset(&fri, 0, sizeof(struct FrameInfo));
        DoMethod(data->dt_obj[0], DTM_FRAMEBOX, NULL, &fri, &fri, sizeof(struct FrameInfo), 0);
        data->depth = fri.fri_Dimensions.Depth;
        D(DBF_DATATYPE, "new: framebox dimensions %ldx%ldx%ld", fri.fri_Dimensions.Width, fri.fri_Dimensions.Height, fri.fri_Dimensions.Depth);

        if(data->maxwidth == 0 || (data->maxwidth <= data->dt_header[i]->bmh_Width))
        {
          if(data->maxheight == 0 || (data->maxheight <= data->dt_header[i]->bmh_Height))
          {
            if(data->depth > 0 && data->depth <= 8)
            {
              /* colour lookup bitmap */
              data->fmt = PBPAFMT_LUT8;

              /* bitmap header */
              GetDTAttrs(data->dt_obj[i], PDTA_BitMapHeader, &data->dt_header[i], TAG_DONE);
              data->width =  data->dt_header[0]->bmh_Width;
              data->height =  data->dt_header[0]->bmh_Height;
              D(DBF_DATATYPE, "new: using LUT8 bitmaps");

              result = TRUE;
            }
            else if(data->depth > 8)
            {
              ULONG arraysize;

              /* correct read buffer */
              if(data->depth == 24)
              {
                data->fmt = PBPAFMT_RGB;
                D(DBF_DATATYPE, "new: using 24bit RGB data");
              }
              else
              {
                data->fmt = PBPAFMT_ARGB;
                D(DBF_DATATYPE, "new: using 32bit ARGB data");
              }

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
                ULONG error;

                error = DoMethod(data->dt_obj[i], PDTM_READPIXELARRAY, data->arraypixels[i], data->fmt, data->arraybpr, 0, 0, data->width, data->height);
                D(DBF_DATATYPE, "new: READPIXELARRAY returned %ld", error);

                result = TRUE;
              }
            }
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

  if((data = INST_DATA(cl, obj)) != NULL)
  {
    ULONG i;

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
    struct Screen *scr = NULL;

    /* stored config */
    InitConfig(obj, data);

    /* screen */
    scr = _screen(obj);
    data->scrdepth = GetBitMapAttr(scr->RastPort.BitMap, BMA_DEPTH);

    /* input */
    if(data->button)
    {
      data->ehnode.ehn_Priority = 0;
      data->ehnode.ehn_Flags = MUI_EHF_GUIMODE;
      data->ehnode.ehn_Object = obj;
      data->ehnode.ehn_Class = cl;
      data->ehnode.ehn_Events = IDCMP_MOUSEBUTTONS | IDCMP_MOUSEMOVE;

      DoMethod(_win(obj), MUIM_Window_AddEventHandler, &data->ehnode);
    }

    /* 8-bit data */
    if(data->fmt == PBPAFMT_LUT8 && data->dt_obj[0] != NULL)
    {
      ULONG i;

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
      DoMethod(_win(obj), MUIM_Window_RemEventHandler, &data->ehnode);

    if(data->fmt == PBPAFMT_LUT8 && data->dt_obj[0] != NULL)
    {
      ULONG i;

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
  struct MUI_AreaData *areadata = NULL;

  ENTER();

  if((data = INST_DATA(cl, obj)) != NULL)
  {
    LONG item;
	 ULONG x, y, twidth, theight;

    areadata = muiAreaData(obj);

    /* coordinates */
    item = 0;
	 x = areadata->mad_Box.Left;
	 y = areadata->mad_Box.Top;
	 twidth = (data->width + data->border_horiz) - 2;      /* subtract standard 1 pixel border */
    theight = (data->height + data->border_vert) - 2;

	 /* clear */
	 if(data->button) DoMethod(obj, MUIM_DrawBackground, areadata->mad_Box.Left, areadata->mad_Box.Top, areadata->mad_Box.Width, areadata->mad_Box.Height, 0, 0);

	 /* label */
	 if(data->label != NULL && data->button != FALSE)
	 {
	   uint32 labelx;

	   SetFont(_rp(obj), _font(obj));
	   SetAPen(_rp(obj), 1);

	   labelx = (twidth/2) - (data->labelte.te_Width/2);

	   Move(_rp(obj), x + labelx, _bottom(obj) - 3);
	   Text(_rp(obj), data->label, strlen(data->label));
	 }

    /* draw image */
    if(data->fmt == PBPAFMT_LUT8)
    {
      #if defined(__amigaos4__)
      uint32 error;
      #endif

      /* select bitmap */
		if(data->button && data->pressed && data->overlay && data->dt_bitmap[2])
        item = 2;

      SHOWVALUE(DBF_DRAW, item);
      SHOWVALUE(DBF_DRAW, data->dt_bitmap[item]);
      SHOWVALUE(DBF_DRAW, data->dt_mask[item]);

      #if defined(__amigaos4__)
      error = BltBitMapTags(BLITA_Source, data->dt_bitmap[item],
									 BLITA_Dest, _rp(obj),
                            BLITA_SrcX, 0,
                            BLITA_SrcY, 0,
									 BLITA_DestX, x + (data->border_horiz / 2),
									 BLITA_DestY, y + ((data->border_vert / 2) - (data->label_vert/2)),
                            BLITA_Width, data->width,
                            BLITA_Height, data->height,
                            BLITA_SrcType, BLITT_BITMAP,
                            BLITA_DestType, BLITT_RASTPORT,
                            BLITA_MaskPlane, data->dt_mask[item],
                            TAG_DONE);
      SHOWVALUE(DBF_DRAW, error);

      #else

      if(data->dt_mask[item] != NULL)
      {
        BltMaskBitMapRastPort(data->dt_bitmap[item], 0, 0, _rp(obj),
          _left(obj) + (data->border_horiz / 2),
          _top(obj) + (data->border_vert / 2),
          data->width,
          data->height,
          0xc0,
          (APTR)data->dt_mask[item]);
      }
      else
      {
        BltBitMapRastPort(data->dt_bitmap[item], 0, 0, _rp(obj),
          _left(obj) + (data->border_horiz / 2),
          _top(obj) + (data->border_vert / 2),
          data->width,
          data->height,
          0xc0);
      }
      #endif
    }
    else
    {
      /* select bitmap */
		if(data->button && data->pressed && data->overlay && data->arraypixels[2])
        item = 2;

      SHOWVALUE(DBF_DRAW, item);
      SHOWVALUE(DBF_DRAW, data->arraypixels[item]);

      if(data->arraypixels[item] != NULL)
      {
        #if defined(__amigaos4__)
        int32 srctype;
        uint32 error;

        if(data->depth == 24)
         srctype = BLITT_RGB24;
        else
         srctype = BLITT_ARGB32;

		  /* graphic */
        error = BltBitMapTags(BLITA_Source, data->arraypixels[item],
										BLITA_Dest, _rp(obj),
                              BLITA_SrcX, 0,
                              BLITA_SrcY, 0,
										BLITA_DestX, x + (data->border_horiz / 2),
										BLITA_DestY, y + ((data->border_vert / 2) - (data->label_vert/2)),
                              BLITA_Width, data->width,
                              BLITA_Height, data->height,
                              BLITA_SrcType, srctype,
                              BLITA_DestType, BLITT_RASTPORT,
                              BLITA_SrcBytesPerRow, data->arraybpr,
                              BLITA_UseSrcAlpha, TRUE,
                              TAG_DONE);

        SHOWVALUE(DBF_DRAW, error);

        #elif defined (__MORPHOS__)
        if(data->depth == 24)
        {
			 WritePixelArray(data->arraypixels[item], 0, 0, data->arraybpr, _rp(obj), _left(obj) + (data->border_horiz / 2), _top(obj) + (data->border_vert / 2), data->width, data->height, RECTFMT_RGB);
        }
        else
        {
          WritePixelArrayAlpha(data->arraypixels[item], 0, 0, data->arraybpr, _rp(obj), _left(obj) + (data->border_horiz / 2), _top(obj) + (data->border_vert / 2), data->width, data->height, 0xffffffff);
        }
        #else
        WritePixelArray(data->arraypixels[item], 0, 0, data->arraybpr, _rp(obj), _left(obj) + (data->border_horiz / 2), _top(obj) + (data->border_vert / 2), data->width, data->height, data->depth == 24 ? RECTFMT_RGB : RECTFMT_ARGB);
        #endif
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
        UBYTE r, g, b;
        DOUBLE shade;
        ULONG size;
        UBYTE *array;

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
          ULONG h;
          UBYTE *p = array;
          ULONG pxl;

			 ReadPixelArray(array, 0, 0, twidth * 3, _rp(obj), x+1, y+1, twidth, theight, RECTFMT_RGB);

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

			 FillPixelArray(_rp(obj), x, y, twidth+2, theight+2, pxl);
			 WritePixelArray(array, 0, 0, twidth * 3, _rp(obj), x+1, y+1, twidth, theight, RECTFMT_RGB);

          FreeVec(array);
        }
      }
    }
  }

  RETURN(FALSE);
  return FALSE;
}
///

