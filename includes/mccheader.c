/*****************************************************************************

This code serves as a basis for writing a library-based MUI custom class
(xyz.mcc) and its preferences editor (xyz.mcp).

You need to define a few things in your main source file, then include
this file and then continue with your classes methods. The order is
important, mccheader.c must be the first code-generating module.

Things to be defined before mccheader.c is included:

(1)  UserLibID     - version string for your class. must follow normal $VER: string conventions.
(2)  VERSION       - version number of the class. must match the one given in the $VER: string.
(3)  REVISION      - revision number of the class. must match the one given in the $VER: string.
(4)  CLASS         - Name of your class, ".mcc" or ".mcp" must always be appended.

(5)  SUPERCLASS    - superclass of your class.
(6)  struct Data   - instance data structure.
(7)  _Dispatcher   - your dispatcher function.

(8)  SUPERCLASSP   - Superclass of the preferences class, must be MUIC_Mccprefs or a subclass.
(9)  struct DataP  - instance data structure of preferences class.
(10) _DispatcherP  - dispatcher for the preferences class.

(11) USEDCLASSES   - Other custom classes required by this class (NULL terminated string array)
(12) USEDCLASSESP  - Preferences classes (.mcp) required by this class (NULL terminated string array)
(13) SHORTHELP     - .mcp help text for prefs program's listview.

Items (1) to (4) must always be defined. If you create a stand-alone
custom class (*.mcc) without preferences editor, also define (5), (6)
and (7). Name your class and the resulting ouptut file "Myclass.mcc".

If you create a preferences class (*.mcp) for a custom class, define
(8), (9) and (10) instead of (5), (6) and (7). Name your class and the
resulting output file "Myclass.mcp".

If you create a custom class with included preferences editor, define
all the above. Note that in this case, the name of your class and the
resulting output file is always "Myclass.mcc". MUI will automatically
recognize that there's also a "Myclass.mcp" included. Having a builtin
preferences class reduces the need for a second file but increases the
size and memory consuption of the class.

(11) If your class requires other mcc custom classes, list them in the
static array USEDCLASSES like this: 
const STRPTR USEDCLASSES[] = { "Busy.mcc", "Listtree.mcc", NULL };

(12) If your class has one (or more) preferences classes, list them in
the array USEDCLASSESP like this:
const STRPTR USEDCLASSESP[] = { "Myclass.mcp", "Popxxx.mcp", NULL };

(13) If you want MUI to display additional help text (besides name, 
version and copyright) when the mouse pointer is over your mcp entry 
in the prefs listview:
#define SHORTHELP "ANSI display for terminal programs."

If your class needs custom initialization (e.g. opening other
libraries), you can define
	ClassInit
	ClassExit
to point to custom functions. These functions need to have the prototypes
	BOOL ClassInitFunc(struct Library *base);
	VOID ClassExitFunc(struct Library *base);
and will be called right after the class has been created and right
before the class is being deleted. If your init func returns FALSE,
the custom class will be unloaded immediately.

Define the minimum version of muimaster.libray in MASTERVERSION. If you
don't define MASTERVERSION, it will default to MUIMASTER_VMIN from the
mui.h include file.

---
Items (1) to (4) must always be defined. If you create a stand-alone
custom class (*.mcc) without preferences editor, also define (5), (6)
and (7). Name your class and the resulting ouptut file "Myclass.mcc".

If you create a preferences class (*.mcp) for a custom class, define
(8), (9) and (10) instead of (5), (6) and (7). Name your class and the
resulting output file "Myclass.mcp".

If you create a custom class with included preferences editor, define
all the above. Note that in this case, the name of your class and the
resulting output file is always "Myclass.mcc". MUI will automatically
recognize that there's also a "Myclass.mcp" included. Having a builtin
preferences class reduces the need for a second file but increases the
size and memory consuption of the class.

If your class needs custom initialization (e.g. opening other
libraries), you can define
	PreClassInit
	PostClassExit
	ClassInit
	ClassExit
to point to custom functions. These functions need to have the prototypes
	BOOL ClassInitFunc(struct Library *base);
	VOID ClassExitFunc(struct Library *base);
and will be called right after the class has been created and right
before the class is being deleted. If your init func returns FALSE,
the custom class will be unloaded immediately.

	BOOL PreClassInitFunc(void);
	VOID PostClassExitFunc(void);

These functions will be called BEFORE the class is created and AFTER the
class is deleted, if something depends on it for example. MUIMasterBase
is open then.

Define the minimum version of muimaster.libray in MASTERVERSION. If you
don't define MASTERVERSION, it will default to MUIMASTER_VMIN from the
mui.h include file.

This code automatically defines and initializes the following variables:
	struct Library *MUIMasterBase;
	struct Library *SysBase;
	struct Library *UtilityBase;
	struct DosLibrary *DOSBase;
	struct GfxBase *GfxBase;
	struct IntuitionBase *IntuitionBase;
	struct Library *MUIClassBase;       // your classes library base
	struct MUI_CustomClass *ThisClass;  // your custom class
	struct MUI_CustomClass *ThisClassP; // your preferences class

Example: Myclass.c
	#define CLASS      MUIC_Myclass // name of class, e.g. "Myclass.mcc"
	#define SUPERCLASS MUIC_Area    // name of superclass
	struct Data
	{
		LONG          MyData;
		struct Foobar MyData2;
		// ...
	};
	#define UserLibID "$VER: Myclass.mcc 17.53 (11.11.96)"
	#define VERSION   17
	#define REVISION  53
	#include "mccheader.c"
	ULONG ASM SAVEDS _Dispatcher(REG(a0) struct IClass *cl GNUCREG(a0),
	                             REG(a2) Object *obj GNUCREG(a2),
	                             REG(a1) Msg msg GNUCREG(a1) )
	{
		// ...
	}

Compiling and linking with SAS-C can look like this:
	Myclass.mcc: Myclass.c
		sc $(CFLAGS) $*.c OBJNAME $*.o
		slink to $@ from $*.o lib $(LINKERLIBS) $(LINKERFLAGS)

Note well that we don't use SAS library creation feature here, it simply
sucks too much. It's not much more complicated to do the library
initialziation ourselves and we have better control over everything.

Make sure to read the whole source to get some interesting comments
and some understanding on how libraries are created!

*****************************************************************************/

/******************************************************************************/
/* Includes                                                                   */
/******************************************************************************/

/* MorphOS relevant includes... */
#ifdef __MORPHOS__
#include <emul/emulinterface.h>
#include <emul/emulregs.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#endif


/* a few other includes... */

#include <exec/types.h>
#include <exec/memory.h>
#include <exec/libraries.h>
#include <exec/semaphores.h>
#include <exec/resident.h>
#include <exec/execbase.h>
#include <dos/dos.h>

#include <proto/exec.h>
#include <proto/muimaster.h>
#include <proto/utility.h>
#include <proto/dos.h>
#include <proto/graphics.h>
#include <proto/intuition.h>

/* The name of the class will also become the name of the library. */
/* We need a pointer to this string in our ROMTag (see below). */

static const char UserLibName[] = CLASS;


/* Here's our global data, described above. */

#if defined(__amigaos4__)
struct Library *MUIMasterBase = NULL;
struct Library *SysBase       = NULL;
struct Library *UtilityBase   = NULL;
struct Library *DOSBase       = NULL;
struct Library *GfxBase       = NULL;
struct Library *IntuitionBase = NULL;
struct MUIMasterIFace *IMUIMaster = NULL;
struct ExecIFace *IExec           = NULL;
struct UtilityIFace *IUtility     = NULL;
struct DOSIFace *IDOS             = NULL;
struct GraphicsIFace *IGraphics   = NULL;
struct IntuitionIFace *IIntuition = NULL;
#else
struct Library        *MUIMasterBase = NULL;
struct ExecBase       *SysBase       = NULL;
struct Library        *UtilityBase   = NULL;
struct DosLibrary     *DOSBase       = NULL;
struct GfxBase        *GfxBase       = NULL;
struct IntuitionBase  *IntuitionBase = NULL;
#endif

#ifdef SUPERCLASS
struct MUI_CustomClass *ThisClass;
#endif

#ifdef SUPERCLASSP
struct MUI_CustomClass *ThisClassP;
#endif

#ifdef __SASC

/* these symbols may be referenced by
*	 some functions of sc.lib, but should
*	 never be used inside a shared library
*/

ULONG ONBREAK      			= NULL;
ULONG _ONBREAK     			= NULL;
ULONG base         			= NULL;
ULONG _base        			= NULL;
ULONG ProgramName  			= NULL;
ULONG _ProgramName 			= NULL;
ULONG StackPtr     			= NULL;
ULONG _StackPtr    			= NULL;
ULONG oserr        			= NULL;
ULONG _oserr       			= NULL;
ULONG OSERR        			= NULL;
ULONG _OSERR       			= NULL;

void __regargs __chkabort(void) {}	/* a shared library cannot be    */
void __regargs _CXBRK(void)     {}	/* CTRL-C aborted when doing I/O */

#else
	#ifdef __GNUC__
		struct Library *__DOSBase = NULL;     // required by libnix
		struct Library *__UtilityBase = NULL; // required by libnix & clib2
		#ifdef __libnix__
			/* these one are needed copies for libnix.a */
			#ifdef USE_MATHIEEEDOUBBASBASE
			struct Library *__MathIeeeDoubBasBase = NULL;
			#endif
			#ifdef USE_MATHIEEEDOUBTRANSBASE
			struct Library *__MathIeeeDoubTransBase = NULL;
			#endif

			// here we have some "fake" functions to please
			// libnix
			void __chkabort(void) 	{} /* a shared library cannot be    */
			void _CXBRK(void)     	{} /* CTRL-C aborted when doing I/O */
			void exit(int a UNUSED) { abort(); }

			LONG _WBenchMsg = 0;
			LONG __nocommandline = 0;
    #endif
	#endif /* __GNUC__ */
#endif /* __SASC */

/* Our library structure, consisting of a struct Library, a segment pointer */
/* and a semaphore. We need the semaphore to protect init/exit stuff in our */
/* open/close functions */

struct LibraryHeader
{
	struct Library         lh_Library;
	BPTR                   lh_Segment;
	struct SignalSemaphore lh_Semaphore;
	struct StackSwapStruct *lh_Stack;
};

/******************************************************************************/
/* External references                                                        */
/******************************************************************************/

#if defined(__MORPHOS__)

//static BOOL LIBFUNC UserLibInit   (struct Library *dummy);
//static BOOL LIBFUNC UserLibExpunge(struct Library *dummy);
static BOOL LIBFUNC UserLibOpen   (struct Library *dummy);
static BOOL LIBFUNC UserLibClose  (struct Library *dummy);

#else

//static BOOL LIBFUNC UserLibInit   (REG(a6, struct Library *base));
//static BOOL LIBFUNC UserLibExpunge(REG(a6, struct Library *base));
static BOOL LIBFUNC UserLibOpen   (REG(a6, struct Library *base));
static BOOL LIBFUNC UserLibClose  (REG(a6, struct Library *base));

#endif

/******************************************************************************/
/* Local Structures & Prototypes                                              */
/******************************************************************************/

#if defined(__amigaos4__)

struct LibraryHeader *   LIBFUNC LibInit   (struct LibraryHeader *base, BPTR librarySegment, struct ExecIFace *pIExec);
BPTR 									   LIBFUNC LibExpunge(struct LibraryManagerInterface *Self);
struct LibraryHeader *   LIBFUNC LibOpen   (struct LibraryManagerInterface *Self, ULONG version);
BPTR									   LIBFUNC LibClose  (struct LibraryManagerInterface *Self);
LONG                     LIBFUNC LibNull   (void);
ULONG                    LIBFUNC MCC_Query (UNUSED struct Interface *self, REG(d0, LONG which));

#elif defined(__MORPHOS__)

struct LibraryHeader *   LIBFUNC LibInit   (struct Library *lib, BPTR Segment, struct ExecBase *SysBase);
BPTR                     LIBFUNC LibExpunge(void);
struct LibraryHeader *   LIBFUNC LibOpen   (void);
BPTR                     LIBFUNC LibClose  (void);
LONG                     LIBFUNC LibNull   (void);
ULONG                    LIBFUNC MCC_Query (void);

#else

struct LibraryHeader *   LIBFUNC LibInit   (REG(a0, BPTR Segment));
BPTR                     LIBFUNC LibExpunge(REG(a6, struct LibraryHeader *base));
struct LibraryHeader *   LIBFUNC LibOpen   (REG(a6, struct LibraryHeader *base));
BPTR                     LIBFUNC LibClose  (REG(a6, struct LibraryHeader *base));
LONG                     LIBFUNC LibNull   (void);
ULONG                    LIBFUNC MCC_Query (REG(d0, LONG which));

#endif

/******************************************************************************/
/* Dummy entry point and LibNull() function all in one                        */
/******************************************************************************/

#if defined(__amigaos4__)
int _start(void)
{
	return RETURN_FAIL;
}
#endif

LONG LIBFUNC LibNull(VOID)
{
	return(0);
}

/******************************************************************************/
/* Local data structures                                                      */
/******************************************************************************/

#if defined(__amigaos4__)
/* ------------------- OS4 Manager Interface ------------------------ */
STATIC ULONG LibObtain(struct LibraryManagerInterface *Self)
{
	 return(Self->Data.RefCount++);
}

STATIC ULONG LibRelease(struct LibraryManagerInterface *Self)
{
	 return(Self->Data.RefCount--);
}

STATIC CONST APTR LibManagerVectors[] =
{
   (APTR)LibObtain,
   (APTR)LibRelease,
	 (APTR)NULL,
	 (APTR)NULL,
   (APTR)LibOpen,
   (APTR)LibClose,
   (APTR)LibExpunge,
	 (APTR)NULL,
   (APTR)-1
};

STATIC CONST struct TagItem LibManagerTags[] =
{
   {MIT_Name,             (ULONG)"__library"},
   {MIT_VectorTable,      (ULONG)LibManagerVectors},
   {MIT_Version,          1},
   {TAG_DONE,             0}
};

/* ------------------- Library Interface(s) ------------------------ */

STATIC CONST APTR LibVectors[] =
{
   (APTR)LibObtain,
   (APTR)LibRelease,
	 (APTR)NULL,
	 (APTR)NULL,
	 (APTR)MCC_Query,
   (APTR)-1
};

STATIC CONST struct TagItem MainTags[] =
{
   {MIT_Name,              (ULONG)"main"},
	 {MIT_VectorTable,       (ULONG)LibVectors},
   {MIT_Version,           1},
   {TAG_DONE,              0}
};

STATIC CONST ULONG LibInterfaces[] =
{
   (ULONG)LibManagerTags,
   (ULONG)MainTags,
	 (ULONG)0
};

// Out libraries always have to carry a 68k jump table with it, so
// lets define it here as extern, as we are going to link it to
// our binary here.
#ifndef NO_VECTABLE68K
extern const APTR VecTable68K[];
#endif

STATIC CONST struct TagItem LibCreateTags[] =
{
   {CLT_DataSize,   (ULONG)(sizeof(struct LibraryHeader))},
   {CLT_InitFunc,   (ULONG)LibInit},
   {CLT_Interfaces, (ULONG)LibInterfaces},
	 #ifndef NO_VECTABLE68K
	 {CLT_Vector68K,  (ULONG)VecTable68K},
	 #endif
   {TAG_DONE,       0}
};

#else

static const APTR LibVectors[] =
{
	#ifdef __MORPHOS__
	(APTR)FUNCARRAY_32BIT_NATIVE,
	#endif
	(APTR)LibOpen,
	(APTR)LibClose,
	(APTR)LibExpunge,
	(APTR)LibNull,
	(APTR)MCC_Query,
	(APTR)-1
};

#endif

/* ------------------- ROM Tag ------------------------ */

static const USED_VAR struct Resident ROMTag =
{
	RTC_MATCHWORD,
	(struct Resident *)&ROMTag,
	(struct Resident *)&ROMTag + 1,
	#if defined(__amigaos4__)
	RTF_AUTOINIT|RTF_NATIVE,		  // The Library should be set up according to the given table.
	#elif defined(__MORPHOS__)
	RTF_PPC,
	#else
	0,
	#endif
	VERSION,
	NT_LIBRARY,
	0,
	(char *)UserLibName,
	(char *)UserLibID+6,
	#if defined(__amigaos4__)
	(APTR)LibCreateTags					  // This table is for initializing the Library.
	#else
	(APTR)LibInit
	#endif
};

#if defined(__MORPHOS__)
/*
 * To tell the loader that this is a new emulppc elf and not
 * one for the ppc.library.
 * ** IMPORTANT **
 */
const USED_VAR ULONG __amigappc__=1;
const USED_VAR ULONG __abox__=1;

#endif /* __MORPHOS */


/******************************************************************************/
/* Standard Library Functions, all of them are called in Forbid() state.      */
/******************************************************************************/

#if defined(__amigaos4__)
struct LibraryHeader * ASM SAVEDS LibInit(struct LibraryHeader *base, BPTR librarySegment, struct ExecIFace *pIExec)
{
	struct ExecBase *sb = (struct ExecBase *)pIExec->Data.LibBase;
	IExec = pIExec;
	SysBase = (struct Library *)sb;

	D(bug("start... (segment=%08lx)\n",librarySegment));

	base->lh_Library.lib_Node.ln_Type = NT_LIBRARY;
	base->lh_Library.lib_Node.ln_Pri  = 0;
	base->lh_Library.lib_Node.ln_Name = (char *)UserLibName;
	base->lh_Library.lib_Flags        = LIBF_CHANGED | LIBF_SUMUSED;
	base->lh_Library.lib_Version      = VERSION;
	base->lh_Library.lib_Revision     = REVISION;
	base->lh_Library.lib_IdString     = (char *)UserLibID;

	base->lh_Segment = librarySegment;

	InitSemaphore(&base->lh_Semaphore);

  /*
  if(!UserLibInit((struct Library *)base))
	{
  	DeleteLibrary((struct Library *)base);
	  return(NULL);
  }
  */

	return(base);
}

#else

#ifdef __MORPHOS__
struct LibraryHeader *LibInit(struct Library *lib, BPTR Segment, struct ExecBase *sb)
{
#else
struct LibraryHeader * ASM SAVEDS LibInit(REG(a0, BPTR Segment))
{
	struct ExecBase *sb = *((struct ExecBase **)4L);
#endif

	struct LibraryHeader *base;
  #if defined(CLASS_STACKSWAP) && !defined(__MORPHOS__) && !defined(__amigaos4__)
	static struct StackSwapStruct *stack = NULL;
  #endif

	SysBase = sb;
  #if defined(__amigaos4__)
  IExec = (struct ExecIFace *)((*(struct ExecBase **)4L)->MainInterface);
  #endif

	D(bug( "Start...\n" ) );

  #if defined(CLASS_STACKSWAP) && !defined(__MORPHOS__) && !defined(__amigaos4__)
	if ( !( stack = AllocMem( sizeof( struct StackSwapStruct ) + 8192, MEMF_PUBLIC | MEMF_CLEAR ) ) )
		return( NULL );

	stack->stk_Lower	= (APTR)( (ULONG)stack + sizeof( struct StackSwapStruct ) );
	stack->stk_Upper	= (ULONG)( (ULONG)stack->stk_Lower + 8192 );
	stack->stk_Pointer	= (APTR)stack->stk_Upper;

	D(bug( "Before StackSwap()\n" ) );
	StackSwap( stack );
  #endif

	// make sure that this is really a 68020+ machine if optimized for 020+
	#if _M68060 || _M68040 || _M68030 || _M68020 || __mc68020 || __mc68030 || __mc68040 || __mc68060
	if(!(SysBase->AttnFlags & AFF_68020))
		return(NULL);
	#endif

	if((base = (struct LibraryHeader *)MakeLibrary((APTR)LibVectors,NULL,NULL,sizeof(struct LibraryHeader),NULL)))
	{
		D(bug( "After MakeLibrary()\n" ) );

		base->lh_Library.lib_Node.ln_Type = NT_LIBRARY;
		base->lh_Library.lib_Node.ln_Name = (char *)UserLibName;
		base->lh_Library.lib_Flags        = LIBF_CHANGED | LIBF_SUMUSED;
		base->lh_Library.lib_Version      = VERSION;
		base->lh_Library.lib_Revision     = REVISION;
		base->lh_Library.lib_IdString     = (char *)UserLibID;

		base->lh_Segment	= Segment;

    #if defined( CLASS_STACKSWAP ) && !defined(__MORPHOS__) && !defined(__amigaos4__)
		base->lh_Stack		= stack;
    #endif

    #ifndef __MORPHOS__
		InitSemaphore(&base->lh_Semaphore);
    #endif

		//if(UserLibInit((struct Library *)base))
		{
  		D(bug( "AddLibrary()\n") );
	  	AddLibrary((struct Library *)base);
    }
    //else base = NULL;
	}
	else
	{
		D(bug("\7MakeLibrary() failed\n") );
	}

  #if defined(CLASS_STACKSWAP) && !defined(__MORPHOS__) && !defined(__amigaos4__)
	StackSwap( base->lh_Stack );
	FreeMem( base->lh_Stack, sizeof( struct StackSwapStruct ) + 8192 );
	D(bug( "After second StackSwap()\n" ) );
  #endif

	return(base);
}
#endif

/*****************************************************************************************************/
/*****************************************************************************************************/

#ifdef __amigaos4__
BPTR LibExpunge(struct LibraryManagerInterface *Self)
{
	struct LibraryHeader *base = (struct LibraryHeader *)Self->Data.LibBase;
#elif __MORPHOS__
BPTR LibExpunge(void)
{
	struct LibraryHeader *base = (void *)REG_A6;
#else
BPTR ASM SAVEDS LibExpunge(REG(a6, struct LibraryHeader *base))
{
#endif
	BPTR rc;

	#ifdef __amigaos4__
	SysBase = *((struct Library **)4L);
	IExec = (struct ExecIFace *)((*(struct ExecBase **)4L)->MainInterface);
	#else
	SysBase = *((struct ExecBase **)4L);
	#endif

	D(bug( "OpenCount = %ld\n", base->lh_Library.lib_OpenCnt ) );

	if(base->lh_Library.lib_OpenCnt > 0)
	{
		base->lh_Library.lib_Flags |= LIBF_DELEXP;
		D(bug("Setting LIBF_DELEXP\n") );
		return((BPTR)NULL);
	}

  /*
	if(!UserLibExpunge((struct Library *)base))
	{
		D(bug("UserLibExpunge() failed, setting LIBF_DELEXP\n"));
		base->lh_Library.lib_Flags |= LIBF_DELEXP;
		return((BPTR)NULL);
	}
  */

	Remove((struct Node *)base);
	rc = base->lh_Segment;

	#ifdef __amigaos4__
	DeleteLibrary((struct Library *)base);
	#else
	FreeMem((BYTE *)base - base->lh_Library.lib_NegSize,base->lh_Library.lib_NegSize + base->lh_Library.lib_PosSize);
	#endif

	return(rc);
}

/*****************************************************************************************************/
/*****************************************************************************************************/

#ifdef __amigaos4__
struct LibraryHeader *LibOpen(struct LibraryManagerInterface *Self, UNUSED ULONG version)
{
	struct LibraryHeader *base = (struct LibraryHeader *)Self->Data.LibBase;
#elif __MORPHOS__
struct LibraryHeader *LibOpen( void )
{
	struct LibraryHeader *base = (void *)REG_A6;
#else
struct LibraryHeader * ASM SAVEDS LibOpen(REG(a6, struct LibraryHeader *base))
{
#endif
	struct LibraryHeader *rc;

	#ifdef __amigaos4__
	SysBase = *((struct Library **)4L);
	IExec = (struct ExecIFace *)((*(struct ExecBase **)4L)->MainInterface);
	#else
	SysBase = *((struct ExecBase **)4L);
	#endif

  /* Kill the Delayed Expunge flag since we are opened again */
	base->lh_Library.lib_Flags &= ~LIBF_DELEXP;

  #ifndef __MORPHOS__
	ObtainSemaphore(&base->lh_Semaphore);
  #endif

	base->lh_Library.lib_OpenCnt++;

	D(bug( "OpenCount = %ld\n", base->lh_Library.lib_OpenCnt ) );

	if (UserLibOpen((struct Library *)base))
	{
		#ifdef CLASS_VERSIONFAKE
		base->lh_Library.lib_Version	= (UWORD)MUIMasterBase->lib_Version;
		base->lh_Library.lib_Revision	= (UWORD)MUIMasterBase->lib_Revision;
		#endif

		rc = base;
	}
	else
	{
		rc = NULL;
		base->lh_Library.lib_OpenCnt--;
		D(bug("\7UserLibOpen() failed\n") );
	}

  #ifndef __MORPHOS__
	ReleaseSemaphore(&base->lh_Semaphore);
  #endif

	return(rc);
}

/*****************************************************************************************************/
/*****************************************************************************************************/

#ifdef __amigaos4__
BPTR LibClose(struct LibraryManagerInterface *Self)
{
	struct LibraryHeader *base = (struct LibraryHeader *)Self->Data.LibBase;
#elif __MORPHOS__
BPTR LibClose(void)
{
	struct LibraryHeader *base = (struct LibraryHeader *)REG_A6;
#else
BPTR ASM SAVEDS LibClose(REG(a6, struct LibraryHeader *base))
{
#endif
	BPTR rc = (BPTR)NULL;

	#ifdef __amigaos4__
	SysBase = *((struct Library **)4L);
	IExec = (struct ExecIFace *)((*(struct ExecBase **)4L)->MainInterface);
	#else
	SysBase = *((struct ExecBase **)4L);
	#endif

  #ifndef __MORPHOS__
	ObtainSemaphore(&base->lh_Semaphore);
  #endif

	D(bug( "OpenCount = %ld %s\n", base->lh_Library.lib_OpenCnt, base->lh_Library.lib_OpenCnt == 0 ? "\7ERROR" : "" ) );

	UserLibClose((struct Library *)base);

	base->lh_Library.lib_OpenCnt--;

  #ifndef __MORPHOS__
	ReleaseSemaphore(&base->lh_Semaphore);
  #endif

	if(base->lh_Library.lib_OpenCnt == 0)
	{
		if (base->lh_Library.lib_Flags & LIBF_DELEXP)
		{
			#ifdef __amigaos4__
			rc = (BPTR)LibExpunge(Self);
			#elif __MORPHOS__
			rc = (BPTR)LibExpunge();
      #else
			rc = (BPTR)LibExpunge(base);
			#endif
		}
	}

	return(rc);
}

/*****************************************************************************************************/
/*****************************************************************************************************/

#ifdef SUPERCLASS
  DISPATCHERPROTO(_Dispatcher);
#endif

#ifdef SUPERCLASSP
  DISPATCHERPROTO(_DispatcherP);
#endif

#ifdef __MORPHOS__
BOOL UserLibOpen(struct Library *dummy)
{
	struct Library *base = (void *)REG_A6;
#else
BOOL ASM SAVEDS UserLibOpen(REG(a6, struct Library *base))
{
#endif
	BOOL PreClassInitFunc(void);
	BOOL ClassInitFunc(struct Library *base);

	D(bug( "OpenCount = %ld\n", base->lib_OpenCnt ) );

	if (base->lib_OpenCnt!=1)
		return(TRUE);

	#ifndef MASTERVERSION
	#define MASTERVERSION MUIMASTER_VMIN
	#endif

	if((MUIMasterBase = OpenLibrary("muimaster.library", MASTERVERSION)) &&
     GETINTERFACE(IMUIMaster, MUIMasterBase))
	{
		#ifdef PreClassInit
		PreClassInitFunc();
		#endif

		#ifdef SUPERCLASS
		ThisClass = MUI_CreateCustomClass(base, SUPERCLASS, NULL, sizeof(struct INSTDATA), ENTRY(_Dispatcher));
		if(ThisClass)
		#endif
		{
			#ifdef SUPERCLASSP
			if((ThisClassP = MUI_CreateCustomClass(base, SUPERCLASSP, NULL, sizeof(struct INSTDATAP), ENTRY(_DispatcherP))))
			#endif
			{
				#ifdef SUPERCLASS
				#define THISCLASS ThisClass
				#else
				#define THISCLASS ThisClassP
				#endif

        #ifdef __amigaos4__
				UtilityBase   = (struct Library *)THISCLASS->mcc_UtilityBase;
				DOSBase       = (struct Library *)THISCLASS->mcc_DOSBase;
				GfxBase       = (struct Library *)THISCLASS->mcc_GfxBase;
				IntuitionBase = (struct Library *)THISCLASS->mcc_IntuitionBase;
        #else
				UtilityBase   = (struct Library *)      THISCLASS->mcc_UtilityBase;
				DOSBase       = (struct DosLibrary *)   THISCLASS->mcc_DOSBase;
				GfxBase       = (struct GfxBase *)      THISCLASS->mcc_GfxBase;
				IntuitionBase = (struct IntuitionBase *)THISCLASS->mcc_IntuitionBase;
        #endif

				#if defined(__GNUC__)
        __DOSBase		  = (APTR)DOSBase;
        __UtilityBase	= (APTR)UtilityBase;
				#endif /* __GNUC__ */

        if(UtilityBase && DOSBase && GfxBase && IntuitionBase &&
           GETINTERFACE(IUtility, UtilityBase) &&
           GETINTERFACE(IDOS, DOSBase) &&
           GETINTERFACE(IGraphics, GfxBase) &&
           GETINTERFACE(IIntuition, IntuitionBase))
        {
  				#ifndef ClassInit
	  			return(TRUE);
		  		#else
			  	if(ClassInitFunc(base))
				  {
					  return(TRUE);
  				}

				  #ifdef SUPERCLASSP
				  MUI_DeleteCustomClass(ThisClassP);
				  ThisClassP = NULL;
				  #endif
				  #endif
        }

        DROPINTERFACE(IIntuition);
        DROPINTERFACE(IGraphics);
        DROPINTERFACE(IDOS);
        DROPINTERFACE(IUtility);
			}

			#if defined(SUPERCLASSP) && defined(SUPERCLASS)
			MUI_DeleteCustomClass(ThisClass);
			ThisClass = NULL;
			#endif
		}

    DROPINTERFACE(IMUIMaster);
		CloseLibrary(MUIMasterBase);
		MUIMasterBase = NULL;
	}

	D(bug("fail.: %08lx %s\n",base,base->lib_Node.ln_Name) );

	return(FALSE);
}

/*****************************************************************************************************/
/*****************************************************************************************************/

#ifdef __MORPHOS__
BOOL UserLibClose(struct Library *dummy)
{
	struct Library *base = (void *)REG_A6;
#else
BOOL ASM SAVEDS UserLibClose(REG(a6, struct Library *base))
{
#endif

	VOID PostClassExitFunc(void);
	VOID ClassExitFunc(struct Library *base);

	D(bug( "OpenCount = %ld\n", base->lib_OpenCnt ) );

	if (base->lib_OpenCnt==1)
	{
		#ifdef ClassExit
		ClassExitFunc(base);
		#endif

		#ifdef SUPERCLASSP
		if (ThisClassP)
		{
			MUI_DeleteCustomClass(ThisClassP);
			ThisClassP = NULL;
		}
		#endif

		#ifdef SUPERCLASS
		if (ThisClass)
		{
			MUI_DeleteCustomClass(ThisClass);
			ThisClass = NULL;
		}
		#endif

		#ifdef PostClassExit
		PostClassExitFunc();
		#endif

    DROPINTERFACE(IIntuition);
    DROPINTERFACE(IGraphics);
    DROPINTERFACE(IDOS);
    DROPINTERFACE(IUtility);

		if (MUIMasterBase)
		{
      DROPINTERFACE(IMUIMaster);
			CloseLibrary(MUIMasterBase);
			MUIMasterBase = NULL;
		}
	}

	return(TRUE);
}

/*****************************************************************************************************/
/*****************************************************************************************************/

#if defined(__amigaos4__)
ULONG LIBFUNC MCC_Query(UNUSED struct Interface *self, REG(d0, LONG which))
{
#elif defined(__MORPHOS__)
ULONG MCC_Query(void)
{
	LONG which = (LONG)REG_D0;
#else
ULONG LIBFUNC MCC_Query(REG(d0, LONG which))
{
#endif

	switch (which)
	{
		#ifdef SUPERCLASS
		case 0: return((ULONG)ThisClass);
		#endif

		#ifdef SUPERCLASSP
		case 1: return((ULONG)ThisClassP);
		#endif

		#ifdef PREFSIMAGEOBJECT
		case 2:
		{
			Object *obj = PREFSIMAGEOBJECT;
			return((ULONG)obj);
		}
		#endif

		#ifdef ONLYGLOBAL
		case 3:
		{
			return(TRUE);
		}
		#endif

		#ifdef INFOCLASS
		case 4:
		{
			return(TRUE);
		}
		#endif

		#ifdef USEDCLASSES
		case 5:
		{
			return((ULONG)USEDCLASSES);
		}
		#endif

		#ifdef USEDCLASSESP
		case 6:
		{
			return((ULONG)USEDCLASSESP);
		}
		#endif

		#ifdef SHORTHELP
		case 7:
		{
			return((ULONG)SHORTHELP);
		}
		#endif
	}

	return(0);
}
