/***************************************************************************

 NList Custom Classes
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

#include "SDI_compiler.h"
#include "SDI_hook.h"
#include "SDI_lib.h"
#include "SDI_stdarg.h"

#ifdef __amigaos4__
/* redefine some defines to allow complexer macro use later on */
#define DoMethod				IDoMethod
#define DoMethodA 			IDoMethodA
#define DoSuperMethod 	IDoSuperMethod
#define DoSuperMethodA 	IDoSuperMethodA
#define SetSuperAttrs 	ISetSuperAttrs
#define CoerceMethod 		ICoerceMethod
#define CoerceMethodA 	ICoerceMethodA

#ifdef OpenWindowTags
#undef OpenWindowTags
#define OpenWindowTags IIntuition->OpenWindowTags
#endif

#ifdef NewObject
#undef NewObject
#define NewObject IIntuition->NewObject
#endif

#define GETINTERFACE(iface, base)	(iface = (APTR)GetInterface((struct Library *)(base), "main", 1L, NULL))
#define DROPINTERFACE(iface)			(DropInterface((struct Interface *)iface), iface = NULL)

#else

#define GETINTERFACE(iface, base)	TRUE
#define DROPINTERFACE(iface)

#endif /* __amigaos4__ */

