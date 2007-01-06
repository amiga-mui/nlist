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

/* ansi includes */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

/* system includes */
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

/* system */
#include <datatypes/pictureclass.h>
#include <dos/dosextens.h>
#include <dos/dostags.h>
#include <exec/execbase.h>
#include <exec/lists.h>
#include <exec/nodes.h>
#include <graphics/layers.h>
#include <graphics/blitattr.h>

/* libraries */
#include <libraries/gadtools.h>
#include <libraries/mui.h>
#include <libraries/amigaguide.h>

/* local includes */
#include "NBitmap.h"
#include "private.h"
#include "rev.h"

/* prototypes */
ULONG GetConfigItem(Object *obj, ULONG configitem, ULONG defaultsetting);
VOID InitConfig(Object *obj, struct InstData *data);
VOID FreeConfig(struct InstData *data);
VOID NBitmap_DrawSimpleFrame(Object *obj, struct RastPort *rport, uint32 x, uint32 y, uint32 w, uint32 h);

/* ULONG GetConfigItem() */
ULONG GetConfigItem(Object *obj, ULONG configitem, ULONG defaultsetting)
	{
		ULONG value;
		return( DoMethod(obj, MUIM_GetConfigItem, configitem, &value) ? *(ULONG *)value : defaultsetting);
	}

/* VOID InitConfig() */
VOID InitConfig(Object *obj, struct InstData *data)
	{
		if(obj && data)
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
	}

/* VOID FreeConfig() */
VOID FreeConfig(struct InstData *data)
	{
		if(data)
			{

			}
	}

/* BOOL NBitmap_LoadImage() */
BOOL NBitmap_LoadImage(STRPTR filename, uint32 item, struct IClass *cl, Object *obj)
	{
		BOOL result = FALSE;

		struct InstData *data = NULL;

		if(((data = INST_DATA(cl, obj))!=NULL) && filename)
			{
				data->dt_obj[item] = NewDTObject(filename,
												DTA_GroupID,					GID_PICTURE,
												OBP_Precision,				PRECISION_EXACT,
												PDTA_FreeSourceBitMap,	TRUE,
												PDTA_DestMode,				PMODE_V43,
												PDTA_UseFriendBitMap,		TRUE,
											TAG_DONE);

				if(data->dt_obj[item]) result = TRUE;
			}

		return(result);
	}

/* BOOL NBitmap_NewImage() */
BOOL NBitmap_NewImage(struct IClass *cl, Object *obj)
	{
		BOOL result = FALSE;
		uint32 i, arraysize;

		struct pdtBlitPixelArray pbpa;
		struct FrameInfo fri;
		struct InstData *data = INST_DATA(cl, obj);

		/* need at least the normal image */
		if(((data = INST_DATA(cl, obj))!=NULL) && (data->dt_obj[0]))
			{
				for(i=0;i<3;i++)
					{
						if(data->dt_obj[i])
							{
								SetMem(&fri, 0, sizeof(struct FrameInfo));
								DoMethod(data->dt_obj[0], DTM_FRAMEBOX, NULL, &fri, &fri, sizeof(struct FrameInfo), 0);
								data->depth = fri.fri_Dimensions.Depth;

								if(data->depth>0 && data->depth<=8)
									{
										/* colour lookup bitmap */
										data->fmt = PBPAFMT_LUT8;

										/* bitmap header */
										GetDTAttrs(data->dt_obj[i], PDTA_BitMapHeader, &data->dt_header[i], TAG_DONE);
										data->width =  data->dt_header[0]->bmh_Width;
										data->height =  data->dt_header[0]->bmh_Height;

										result = TRUE;
									}
								else if(data->depth >=16)
									{
										/* true colour bitmap */
										data->fmt = PBPAFMT_ARGB;

										/* bitmap header */
										GetDTAttrs(data->dt_obj[i], PDTA_BitMapHeader, &data->dt_header[i], TAG_DONE);
										data->width =  data->dt_header[0]->bmh_Width;
										data->height =  data->dt_header[0]->bmh_Height;
										data->arraybpp = data->depth/8;
										data->arraybpr = data->arraybpp * data->width;
										arraysize = (data->arraybpr) * data->height;

										/* get array of pixels */
										if((data->arraypixels[i] = AllocVec(arraysize, MEMF_ANY|MEMF_CLEAR))!=NULL)
											{
												SetMem(&pbpa, 0, sizeof(struct pdtBlitPixelArray));
												pbpa.MethodID = PDTM_READPIXELARRAY;
												pbpa.pbpa_PixelData = data->arraypixels[i];
												pbpa.pbpa_PixelFormat = PBPAFMT_ARGB;
												pbpa.pbpa_PixelArrayMod = data->arraybpr;
												pbpa.pbpa_Left = 0;
												pbpa.pbpa_Top = 0;
												pbpa.pbpa_Width = data->width;
												pbpa.pbpa_Height = data->height;

												DoMethodA(data->dt_obj[i], (Msg)(VOID*)&pbpa);
												result = TRUE;
											}
									}
							}
					}
			}

		return(result);
	}

/* VOID NBitmap_DisposeImage() */
VOID NBitmap_DisposeImage(struct IClass *cl, Object *obj)
	{
		uint32 i;
		struct InstData *data = NULL;

      if((data = INST_DATA(cl, obj))!=NULL)
			{
				/* free datatype object */
				if(data->type == MUIV_NBitmap_Type_File)
					{
						for(i=0;i<3;i++)
							{
								if(data->dt_obj[i])
									{
										DisposeDTObject(data->dt_obj[i]);
										data->dt_obj[i] = NULL;
									}
							}
					}
				
				if(data->label)
					{
						FreeVec(data->label);
						data->label = NULL;
					}

				/* free pixel memory */
				for(i=0;i<3;i++)
					{
						if(data->arraypixels[i])
							{
								FreeVec(data->arraypixels[i]);
								data->arraypixels[i] = NULL;
							}
					}
			}
	}

/* VOID NBitmap_SetupImage() */
VOID NBitmap_SetupImage(struct IClass *cl, Object *obj)
	{
		uint32 i, displayid;

      struct InstData *data = NULL;
		struct Screen *scr = NULL;

      if((data = INST_DATA(cl, obj))!=NULL)
			{
				/* stored config */
				InitConfig(obj, data);

				/* screen */
				scr = muiRenderInfo(obj)->mri_Screen;
				GetScreenAttr(scr, SA_DisplayID, &displayid, sizeof(uint32));
				data->scrdepth = p96GetModeIDAttr(displayid, P96IDA_DEPTH);

				/* input */
				if(data->button) MUI_RequestIDCMP(obj, IDCMP_MOUSEBUTTONS|IDCMP_GADGETUP|IDCMP_GADGETDOWN|IDCMP_MOUSEMOVE);
				
				/* 8-bit data */
				if(data->fmt == PBPAFMT_LUT8)
					{
						/* layout image */
						for(i=0;i<3;i++)
							{
								SetDTAttrs(data->dt_obj[i], NULL, NULL, PDTA_Screen, scr, TAG_DONE);
								if(DoMethod(data->dt_obj[i], DTM_PROCLAYOUT, NULL, 1))
									{
										GetDTAttrs(data->dt_obj[i], PDTA_CRegs, &data->dt_colours[i], TAG_DONE);
										GetDTAttrs(data->dt_obj[i], PDTA_MaskPlane, &data->dt_mask[i], TAG_DONE);
										GetDTAttrs(data->dt_obj[i], PDTA_DestBitMap, &data->dt_bitmap[i], TAG_DONE);
										if(data->dt_bitmap[i] == NULL) GetDTAttrs(data->dt_obj[i], PDTA_BitMap, &data->dt_bitmap[i], TAG_DONE);
									}
							}
					}
			}
	}

/* VOID NBitmap_CleanupImage() */
VOID NBitmap_CleanupImage(struct IClass *cl, Object *obj)
	{
      struct InstData *data = NULL;

      if((data = INST_DATA(cl, obj))!=NULL)
			{
				/* input */
				if(data->button) MUI_RejectIDCMP(obj, IDCMP_MOUSEBUTTONS|IDCMP_GADGETUP|IDCMP_GADGETDOWN|IDCMP_MOUSEMOVE);

				/* stored config */
				FreeConfig(data);
			}
	}

/* BOOL NBitmap_DrawImage() */
BOOL NBitmap_DrawImage(struct IClass *cl, Object *obj)
	{
		int32 i, item, error;
		uint8	r, g, b, *array = NULL;
		uint32 x, y, w, h, size, pxl;
		uint32 twidth, theight;
		double shade;

		struct TrueColorInfo tinfo;

		struct InstData *data = NULL;
		struct RastPort *rport = NULL;
		struct MUI_AreaData *areadata = NULL;

		if((data = INST_DATA(cl, obj))!=NULL)
			{
				rport = muiRenderInfo(obj)->mri_RastPort;
				areadata = muiAreaData(obj);
            
				/* coordinates */
				i = 0;
				item = 0;
				x = areadata->mad_Box.Left;
				y = areadata->mad_Box.Top;

				twidth = (data->width + data->border_horiz)-2; 		/* subtract standard 1 pixel border */
				theight = (data->height + data->border_vert)-2;

				/* clear */
				if(data->button) DoMethod(obj, MUIM_DrawBackground, areadata->mad_Box.Left, areadata->mad_Box.Top, areadata->mad_Box.Width, areadata->mad_Box.Height, 0, 0);
				
				/* draw image */
				if(data->fmt == PBPAFMT_LUT8)
					{
						/* select bitmap */
						if(data->button && data->pressed && data->overlay && data->dt_bitmap[2]) item = 2;

						error = BltBitMapTags(BLITA_Source, data->dt_bitmap[item],
													BLITA_Dest, rport,
													BLITA_SrcX, 0,
													BLITA_SrcY, 0,
													BLITA_DestX, x+(data->border_horiz/2),
													BLITA_DestY, y+(data->border_vert/2),
													BLITA_Width, data->width,
													BLITA_Height, data->height,
													BLITA_SrcType, BLITT_BITMAP,
													BLITA_DestType, BLITT_RASTPORT,
													BLITA_MaskPlane, data->dt_mask[item],
												TAG_DONE);
					}
				else
					{
						/* select bitmap */
						if(data->button && data->pressed && data->overlay && data->arraypixels[2]) item = 2;

						if(data->arraypixels[item])
							{
								error = BltBitMapTags(BLITA_Source, data->arraypixels[item],
													BLITA_Dest, rport,
													BLITA_SrcX, 0,
													BLITA_SrcY, 0,
													BLITA_DestX, x+(data->border_horiz/2),
													BLITA_DestY, y+(data->border_vert/2),
													BLITA_Width, data->width,
													BLITA_Height, data->height,
													BLITA_SrcType, BLITT_ARGB32,
													BLITA_DestType, BLITT_RASTPORT,
													BLITA_SrcBytesPerRow, data->arraybpr,
													BLITA_UseSrcAlpha, TRUE,
												TAG_DONE);
							}
					}

				/* overlay */
				if(data->button && data->overlay)
					{
						if(data->prefs.overlay_type == 1 || data->scrdepth<=8)
							{
								/* standard overlay */
								if(data->pressed)
									NBitmap_DrawSimpleFrame(obj, rport, x, y, data->width, data->height);
								else
									NBitmap_DrawSimpleFrame(obj, rport, x, y, data->width, data->height);
							}
						else
							{
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
								if((array = AllocVec(size, MEMF_ANY|MEMF_CLEAR))!=NULL)
									{
										SetMem(&tinfo, 0, sizeof(struct TrueColorInfo));
										tinfo.PixelDistance = 3;
										tinfo.BytesPerRow = twidth * 3;
										tinfo.RedData = array;
										tinfo.GreenData = array+1;
										tinfo.BlueData = array+2;

										p96ReadTrueColorData(&tinfo, 0, 0, rport, x+1, y+1, twidth, theight);

										for(h=0;h<theight;h++)
											{
												for(w=0;w<twidth;w++)
													{
														array[i] = (array[i] + r)/shade;
														array[i+1] = (array[i+1] + g)/shade;
														array[i+2] = (array[i+2] + b)/shade;

														i+=3;
													}
											}
														
										pxl = (uint32)((r<<16) + (g<<8) + b);
										p96RectFill(rport, x, y, x+(twidth+1), y+(theight+1), pxl);
										p96WriteTrueColorData(&tinfo, 0, 0, rport, x+1, y+1, twidth, theight);

										FreeVec(array);
										array = NULL;
									}
							}
					}
			}

		return(FALSE);
	}

/* VOID NBitmap_DrawSimpleFrame() */
VOID NBitmap_DrawSimpleFrame(Object *obj, struct RastPort *rport, uint32 x, uint32 y, uint32 w, uint32 h)
	{
		if(rport)
			{
				SetAPen(rport, _pens(obj)[MPEN_SHADOW]);
				Move(rport, x, y+(h+1));
				Draw(rport, x, y);
				Draw(rport, x+(w+1), y);

				SetAPen(rport, _pens(obj)[MPEN_SHINE]);
				Draw(rport, x+(w+1), y+(h+1));
				Draw(rport, x, y+(h+1));
			}
	}

