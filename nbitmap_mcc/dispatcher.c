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
#include <string.h>

/* system includes */
#include <clib/alib_protos.h>
#include <proto/exec.h>
#include <proto/intuition.h>
#include <proto/muimaster.h>
#include <proto/graphics.h>
#include <proto/utility.h>
#include <proto/locale.h>

/* local includes */
#include "NBitmap.h"
#include "private.h"

/* Object *DoSuperNew() */
#if !defined(__MORPHOS__)
static Object * STDARGS VARARGS68K DoSuperNew(struct IClass *cl, Object *obj, ...)
{
  Object *rc;
  VA_LIST args;

  ENTER();

  VA_START(args, obj);
  rc = (Object *)DoSuperMethod(cl, obj, OM_NEW, VA_ARG(args, ULONG), NULL);
  VA_END(args);

  RETURN(rc);
  return rc;
}
#endif

/* VOID setstr() */
static VOID setstr(STRPTR *dest, STRPTR str)
{
  ENTER();

  if(*dest != NULL)
  {
    FreeVec(*dest);
    *dest = NULL;
  }

  if(str != NULL)
  {
    ULONG len;

    len = strlen(str) + 1;
    if((*dest = AllocVec(len, MEMF_ANY)) != NULL)
      strlcpy(*dest, str, len);
  }

  LEAVE();
}

/* ULONG NBitmap_New() */
ULONG NBitmap_New(struct IClass *cl, Object *obj, struct opSet *msg)
{
      uint32 i;
      struct InstData *data = NULL;
      struct TagItem *tags = NULL, *tag = NULL;

      if((obj = (Object *)DoSuperNew(cl, obj,
        MUIA_CycleChain, TRUE,
      TAG_MORE, msg->ops_AttrList))!=NULL)
        {
          if((data = INST_DATA(cl, obj))!=NULL)
            {
              SetMem(data, 0, sizeof(struct InstData));

              /* passed tags */
              for(tags=((struct opSet *)msg)->ops_AttrList;(tag = NextTagItem(&tags)); )
                  {
                    switch(tag->ti_Tag)
                      {
                        case MUIA_NBitmap_Type:
                          data->type = tag->ti_Data;
                        break;

                        case MUIA_NBitmap_Normal:
                          data->data[0] = (uint32*)tag->ti_Data;
                        break;

                        case MUIA_NBitmap_Ghosted:
                          data->data[1] = (uint32*)tag->ti_Data;
                        break;

                        case MUIA_NBitmap_Selected:
                          data->data[2] = (uint32*)tag->ti_Data;
                        break;

                        case MUIA_NBitmap_Button:
                          data->button = (BOOL)tag->ti_Data;
                        break;

                        case MUIA_NBitmap_Label:
                          setstr(&data->label, (STRPTR)tag->ti_Data);
                        break;
                      }
                  }

              /* load files */
              if(data->type == MUIV_NBitmap_Type_File)
                  {
                    for(i=0;i<3;i++)
                      if(data->data[i]) NBitmap_LoadImage((STRPTR)data->data[i], i, cl, obj);
                  }
              else
                  {
                    for(i=0;i<3;i++)
                      if(data->data[i]) data->dt_obj[i] = (Object *)data->data[i];
                  }

              /* setup images */
              if(NBitmap_NewImage(cl, obj)) return((ULONG)obj);
            }
        }

      CoerceMethod(cl, obj, OM_DISPOSE);
      return(FALSE);
  }

/* ULONG NBitmap_Get() */
ULONG NBitmap_Get(struct IClass *cl, Object *obj, Msg msg)
{
  ULONG result;

  ENTER();

  result = DoSuperMethodA(cl, obj, msg);

  RETURN(result);
  return result;
}

/* ULONG NBitmap_Set() */
ULONG NBitmap_Set(struct IClass *cl,Object *obj, Msg msg)
{
  ULONG result;

  ENTER();

  result = DoSuperMethodA(cl, obj, msg);

  RETURN(result);
  return result;
}

/* ULONG NBitmap_Dispose() */
ULONG NBitmap_Dispose(struct IClass *cl, Object *obj, Msg msg)
{
  ULONG result;

  ENTER();

  NBitmap_DisposeImage(cl, obj);

  result = DoSuperMethodA(cl, obj, msg);

  RETURN(result);
  return result;
}

/* ULONG NBitmap_Setup() */
ULONG NBitmap_Setup(struct IClass *cl, Object *obj, struct MUI_RenderInfo *rinfo)
{
  ULONG result = 0;

  ENTER();

  if(DoSuperMethodA(cl, obj, (Msg)rinfo))
  {
    NBitmap_SetupImage(cl, obj);
    result = 1;
  }

  RETURN(result);
  return result;
}

/* ULONG NBitmap_Cleanup() */
ULONG NBitmap_Cleanup(struct IClass *cl, Object *obj, Msg msg)
{
  ULONG result;

  ENTER();

  NBitmap_CleanupImage(cl, obj);

  result = DoSuperMethodA(cl, obj, msg);

  RETURN(result);
  return(result);
}

/* ULONG NBitmap_Show() */
ULONG NBitmap_Show(struct IClass *cl, Object *obj, Msg msg)
{
  ULONG result;

  ENTER();

  result = DoSuperMethodA(cl, obj, msg);

  RETURN(result);
  return result;
}

/* ULONG NBitmap_Hide() */
ULONG NBitmap_Hide(struct IClass *cl, Object *obj, Msg msg)
{
  ULONG result;

  ENTER();

  result = DoSuperMethodA(cl, obj, msg);

  RETURN(result);
  return result;
}

/* ULONG NBitmap_HandleInput() */
ULONG NBitmap_HandleInput(struct IClass *cl, Object *obj, struct MUIP_HandleInput *msg)
{
  struct InstData *data;
  ULONG result;

  ENTER();

  if((data = INST_DATA(cl, obj)) != NULL)
  {
    if(msg->imsg != NULL)
    {
      switch(msg->imsg->Class)
      {
        case IDCMP_MOUSEBUTTONS:
          if(_isinobject(msg->imsg->MouseX, msg->imsg->MouseY))
          {
            if(msg->imsg->Code == SELECTDOWN)
            {
              if(data->pressed == FALSE)
              {
                data->pressed = TRUE;

                MUI_Redraw(obj, MADF_DRAWUPDATE);
                SetAttrs(obj, MUIA_Pressed, TRUE, TAG_DONE);
              }
            }
            else if(msg->imsg->Code == SELECTUP)
            {
              if(data->overlay && data->pressed)
              {
                data->pressed = FALSE;
                data->overlay = FALSE;

                MUI_Redraw(obj, MADF_DRAWUPDATE);
                SetAttrs(obj, MUIA_Pressed, FALSE, TAG_DONE);
              }
            }
          }
          else
          {
            if(msg->imsg->Code==SELECTUP)
            {
              data->pressed = FALSE;
            }
          }
        break;

        case IDCMP_MOUSEMOVE:
          if(_isinobject(msg->imsg->MouseX, msg->imsg->MouseY))
          {
            if(data->overlay == FALSE)
            {
              data->overlay = TRUE;

              MUI_Redraw(obj, MADF_DRAWUPDATE);
            }
          }
          else
          {
            if(data->overlay)
            {
              data->overlay = FALSE;

              MUI_Redraw(obj, MADF_DRAWUPDATE);
            }
          }
        break;
      }
    }
  }

  result = DoSuperMethodA(cl, obj, (Msg)msg);

  RETURN(result);
  return result;
}

/* ULONG  NBitmap_AskMinMax() */
ULONG NBitmap_AskMinMax(struct IClass *cl, Object *obj, struct MUIP_AskMinMax *msg)
{
  struct InstData *data;

  ENTER();

  DoSuperMethodA(cl, obj, (Msg)msg);

  if((data = INST_DATA(cl, obj)) != NULL)
  {
    // spacing
    data->border_horiz = 0;
    data->border_vert = 0;

    if(data->button)
    {
      data->border_horiz += 4;
      data->border_horiz += data->prefs.spacing_horiz * 2;

      data->border_vert += 4;
      data->border_vert += data->prefs.spacing_vert * 2;
    }

    // standard width & height
    msg->MinMaxInfo->MinWidth = data->width + data->border_horiz;
    msg->MinMaxInfo->DefWidth = data->width + data->border_horiz;
    msg->MinMaxInfo->MaxWidth = data->width + data->border_horiz;

    msg->MinMaxInfo->MinHeight = data->height + data->border_vert;
    msg->MinMaxInfo->DefHeight = data->height + data->border_vert;
    msg->MinMaxInfo->MaxHeight = data->height + data->border_vert;
  }

  RETURN(0);
  return 0;
}

/* ULONG NBitmap_Draw() */
ULONG NBitmap_Draw(struct IClass *cl, Object *obj, struct MUIP_Draw *msg)
{
  ULONG result;

  ENTER();

  result = DoSuperMethodA(cl, obj, (Msg)msg);

  if((msg->flags & MADF_DRAWUPDATE) || (msg->flags & MADF_DRAWOBJECT))
  {
    NBitmap_DrawImage(cl, obj);
  }

  RETURN(result);
  return result;
}

/* DISPATCHER() */
DISPATCHER(_Dispatcher)
{
  ULONG result = TRUE;

  ENTER();

  switch(msg->MethodID)
  {
    case OM_NEW:
      result = NBitmap_New(cl, obj, (struct opSet *)msg);
    break;

    case OM_SET:
      result = NBitmap_Set(cl, obj, msg);
    break;

    case OM_GET:
      result = NBitmap_Get(cl, obj, msg);
    break;

    case OM_DISPOSE:
      result = NBitmap_Dispose(cl, obj, msg);
    break;

    case OM_ADDMEMBER:
      result = DoSuperMethodA(cl, obj, msg);
    break;

    case OM_REMMEMBER:
      result = DoSuperMethodA(cl, obj, msg);
    break;

    case MUIM_Setup:
      result = NBitmap_Setup(cl, obj, (struct MUI_RenderInfo *)msg);
    break;

    case MUIM_Show:
      result = NBitmap_Show(cl, obj, msg);
    break;

    case MUIM_Hide:
      result = NBitmap_Hide(cl, obj, msg);
    break;

    case MUIM_HandleInput:
      result = NBitmap_HandleInput(cl, obj, (APTR)msg);
    break;

    case MUIM_Cleanup:
      result = NBitmap_Cleanup(cl, obj, msg);
    break;

    case MUIM_AskMinMax:
      result = NBitmap_AskMinMax(cl, obj, (struct MUIP_AskMinMax *)msg);
    break;

    case MUIM_Draw:
      result = NBitmap_Draw(cl, obj, (struct MUIP_Draw *)msg);
    break;

    default:
      result = DoSuperMethodA(cl, obj, msg);
    break;
  }

  RETURN(result);
  return result;
}
