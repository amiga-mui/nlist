/***************************************************************************

 NBalance.mcc - New Balance MUI Custom Class
 Copyright (C) 2008 by NList Open Source Team

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

// ansi includes
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

// system
#include <proto/exec.h>
#include <proto/layers.h>
#include <proto/muimaster.h>
#include <libraries/mui.h>
#include <mui/muiundoc.h>

// local includes
#include "NBalance.h"
#include "Pointer.h"
#include "private.h"
#include "rev.h"
#include "Debug.h"

// functions

// methods
ULONG mSetup(struct IClass *cl, Object *obj, struct MUI_RenderInfo *rinfo)
{
  ULONG result = FALSE;
  struct InstData *data = INST_DATA(cl, obj);

  ENTER();

  if(DoSuperMethodA(cl, obj, (Msg)rinfo))
  {
    // find out if the parent object is a horizontal or vertical group
    data->groupType = xget((Object *)_parent(obj), MUIA_Group_Type);

    // setup all pointers
    SetupCustomPointers(data);

    // create/add an event handler to the window the object belongs
    // to
    data->ehnode.ehn_Priority = 0;
    data->ehnode.ehn_Flags    = MUI_EHF_GUIMODE;
    data->ehnode.ehn_Object   = obj;
    data->ehnode.ehn_Class    = cl;
    data->ehnode.ehn_Events   = IDCMP_MOUSEBUTTONS | IDCMP_MOUSEMOVE;
    DoMethod(_win(obj), MUIM_Window_AddEventHandler, &data->ehnode);

    result = TRUE;
  }

  RETURN(result);
  return result;
}

ULONG mCleanup(struct IClass *cl, Object *obj, Msg msg)
{
  ULONG result = 0;
  struct InstData *data = INST_DATA(cl, obj);

  ENTER();

  // cleanup the selection pointer
  CleanupCustomPointers(data);

  DoMethod(_win(obj), MUIM_Window_RemEventHandler, &data->ehnode);

  result = DoSuperMethodA(cl, obj, msg);

  RETURN(result);
  return result;
}

ULONG mHide(struct IClass *cl, Object *obj, Msg msg)
{
  struct InstData *data = INST_DATA(cl, obj);
  ULONG result;

  ENTER();

  if(data->mouseSelectDown == FALSE)
    HideCustomPointer(obj, data);

  result = DoSuperMethodA(cl, obj, msg);

  RETURN(result);
  return result;
}

ULONG mHandleEvent(struct IClass *cl, Object *obj, struct MUIP_HandleEvent *msg)
{
  struct InstData *data;
  ULONG result = 0;

  ENTER();

  if((data = INST_DATA(cl, obj)) != NULL &&
     msg->imsg != NULL)
  {
    struct IntuiMessage *imsg = msg->imsg;

    switch(imsg->Class)
    {
      case IDCMP_MOUSEMOVE:
      {
        BOOL isOverObject = FALSE;

        if(_isinobject(imsg->MouseX, imsg->MouseY))
        {
          struct Layer_Info *li = &(_screen(obj)->LayerInfo);
          struct Layer *layer;

          // get the layer that belongs to the current mouse coordinates
          LockLayerInfo(li);
          layer = WhichLayer(li, _window(obj)->LeftEdge + imsg->MouseX, _window(obj)->TopEdge + imsg->MouseY);
          UnlockLayerInfo(li);

          // if the mouse is currently over the object and over the object's
          // window we go and change the pointer to show the selection pointer
          if(layer != NULL && layer->Window == _window(obj))
            isOverObject = TRUE;
        }

        if(isOverObject == TRUE)
          ShowCustomPointer(obj, data);
        else
        {
          if(data->mouseSelectDown == FALSE)
            HideCustomPointer(obj, data);
        }
      }
      break;

      case IDCMP_MOUSEBUTTONS:
      {
        if(imsg->Code == SELECTUP)
        {
          data->mouseSelectDown = FALSE;
          HideCustomPointer(obj, data);
        }
        else if(imsg->Code == SELECTDOWN)
        {
          if(_isinobject(imsg->MouseX, imsg->MouseY))
            data->mouseSelectDown = TRUE;
        }
      }
      break;
    }
  }

  result = DoSuperMethodA(cl, obj, (Msg)msg);

  RETURN(result);
  return result;
}

