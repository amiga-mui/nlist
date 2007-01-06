
/* ansi c */
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/* system */
#include <exec/execbase.h>
#include <exec/lists.h>
#include <exec/nodes.h>
#include <datatypes/pictureclass.h>

/* prototypes */
#include <proto/application.h>
#include <proto/dos.h>
#include <proto/exec.h>
#include <proto/gadtools.h>
#include <proto/intuition.h>
#include <proto/locale.h>
#include <proto/utility.h>
#include <proto/muimaster.h>

#include <mui/Busy_mcc.h>
#include <mui/NList_mcc.h>
#include <mui/NListview_mcc.h>
#include <mui/NListtree_mcc.h>
#include <mui/NBitmap_mcc.h>

/* local prototypes */
int openlibs(void);
int closelibs(void);
int fail(STRPTR str);
int initclasses(void);
void freeclasses(void);
BOOL buildapp(void);
void addnode(void);

ULONG SAVEDS ASM MCC_Main_Despatch(REG(a0,struct IClass *cl), REG(a2,Object *obj), REG(a1,Msg msg));
ULONG SAVEDS MCC_Main_New(struct IClass *cl, Object *obj, struct opSet *msg);

/* local variables */
Object *app, *win;

#define MCC_NBitmap_Test (TAG_USER | (2044078415<<16) | 0x0100)

/* structs */

/* custom classes */
struct MUI_CustomClass *MCC_Main = NULL;

/* hook prototypes */

/* libraries & interfaces */
struct Library			*ApplicationBase = NULL;
struct Library 		*ElfBase = NULL;
struct Library			*GadToolsBase = NULL;
struct Library			*MUIMasterBase	= NULL;
struct Library			*IntuitionBase = NULL;
struct Library			*UtilityBase = NULL;

struct ApplicationIFace *IApplication = NULL;
struct GadToolsIFace	*IGadTools = NULL;
struct ElfIFace 		*IElf = NULL;
struct IntuitionIFace *IIntuition = NULL;
struct MUIMasterIFace *IMUIMaster = NULL;
struct UtilityIFace *IUtility = NULL;

/* startup functions */
int main(int argc, char *argv[])
	{
		ULONG sigs, muisignal;
		
		if(openlibs())
			{
				initclasses();
				if(buildapp())
					{
				
						while(DoMethod(app, MUIM_Application_NewInput, &muisignal) != MUIV_Application_ReturnID_Quit)
							{
								sigs = Wait(muisignal|SIGBREAKF_CTRL_C);
								if(sigs)
									{
										if(sigs & SIGBREAKF_CTRL_C) break;
									}
							}
						
						if(app) MUI_DisposeObject(app);
					}
				else DebugPrintF("buildapp() failed\n");

				freeclasses();
				closelibs();
			}

		return(0);
	}

/* open libraries */
int openlibs(void)
	{
		if((ApplicationBase = (struct Library*)OpenLibrary("application.library", 50))==0)
			fail("couldn't open application.library");

		if((ElfBase = (struct Library*)OpenLibrary("elf.library", 50))==0)
			fail("couldn't open elf.library");

		if((IntuitionBase = (struct Library*)OpenLibrary("intuition.library", 50))==0)
			fail("couldn't open intuition.library");

		if((GadToolsBase = (struct Library*)OpenLibrary("gadtools.library", 50))==0)
			fail("couldn't open gadtools.library");

		if((UtilityBase = (struct Library*)OpenLibrary("utility.library", 50))==0)
			fail("couldn't open utility.library");

		if((MUIMasterBase = (struct Library*)OpenLibrary(MUIMASTER_NAME, MUIMASTER_VMIN))==0)
			fail("couldn't open muimaster.library");

		/* get interfaces */
		if((IApplication  = (struct ApplicationIFace *)GetInterface(ApplicationBase, "application", 1, NULL))==0)
			fail("couldn't obtain main interface from application.library");

		if((IElf = (struct ElfIFace *)GetInterface((struct Library*)ElfBase, "main", 1, NULL))==0)
			fail("couldn't obtain main interface from elf.library");

		if((IIntuition = (struct IntuitionIFace *)GetInterface((struct Library*)IntuitionBase, "main", 1, NULL))==0)
			fail("couldn't obtain main interface from intuition.library");

		if((IGadTools = (struct GadToolsIFace *)GetInterface((struct Library*)GadToolsBase, "main", 1, NULL))==0)
			fail("couldn't obtain main interface from gadtools.library");

		if((IUtility = (struct UtilityIFace *)GetInterface((struct Library*)UtilityBase, "main", 1, NULL))==0)
			fail("couldn't obtain main interface from utility.library");

		if((IMUIMaster = (struct MUIMasterIFace *)GetInterface(MUIMasterBase, "main", 1, NULL))==0)
			fail("couldn't obtain main interface from muimaster.library");
	}

/* close libraries */
int closelibs(void)
	{
		/* drop interfaces */
		if(IApplication) DropInterface((struct Interface *)IApplication);
		if(IElf) DropInterface((struct Interface *)IElf);
		if(IIntuition) DropInterface((struct Interface *)IIntuition);
		if(IGadTools) DropInterface((struct Interface *)IGadTools);
		if(IUtility) DropInterface((struct Interface *)IUtility);
		if(IMUIMaster) DropInterface((struct Interface *)IMUIMaster);

		if(ApplicationBase) CloseLibrary((struct Library*)ApplicationBase);
		if(ElfBase) CloseLibrary((struct Library*)ElfBase);
		if(IntuitionBase) CloseLibrary((struct Library*)IntuitionBase);
		if(GadToolsBase) CloseLibrary((struct Library *)GadToolsBase);
		if(UtilityBase) CloseLibrary((struct Library *)UtilityBase);
		if(MUIMasterBase) CloseLibrary(MUIMasterBase);
	}

/* fail */
int fail(STRPTR str)
	{
		if(str) DebugPrintF("NBitmap-Demo Failed: %s\n");

		/* close libraries & exit */
		closelibs();
		exit(0);
	}

/* initialise classes */
int initclasses(void)
	{
		if((MCC_Main = MUI_CreateCustomClass(NULL, MUIC_Window, 	NULL, sizeof(0), MCC_Main_Despatch))==0) return(FALSE);
		
		return(TRUE);
	}

/* free classes */
void freeclasses(void)
	{
		if(MCC_Main) MUI_DeleteCustomClass(MCC_Main);
	}

/* mui support functions */
ULONG DoSuperNew(struct IClass *cl, Object *obj, ...)
	{
		ULONG mav[50];
		ULONG ret;
		va_list tags;
		struct opSet myopSet;
		int i=0;

		va_start(tags, obj);
		while(i<50)
			{
				mav[i] = va_arg(tags, ULONG);
				if(mav[i] == TAG_DONE) break;
				mav[i+1] = va_arg(tags, ULONG);
				if(mav[i] == TAG_MORE) break;
				i += 2;
			}
		va_end(tags);

		myopSet.MethodID = OM_NEW;
		myopSet.ops_AttrList = (struct TagItem *) &mav;
		myopSet.ops_GInfo = NULL;
		ret = DoSuperMethodA(cl, obj, (APTR)&myopSet);

		return(ret);
	}

/* build app */
BOOL buildapp(void)
	{
		app = ApplicationObject,
			MUIA_Application_Title,				"NBitmap-Demo",
			MUIA_Application_Version,			"1",
			MUIA_Application_Description,		"NBitmap-Demo",
			MUIA_Application_Base,				"NBITMAP",

			SubWindow, win = (Object*)NewObject(MCC_Main->mcc_Class, NULL, TAG_DONE),
		End;

		if(app)
			{
				DoMethod(win, MUIM_Notify, MUIA_Window_CloseRequest, TRUE, app, 2, MUIM_Application_ReturnID, MUIV_Application_ReturnID_Quit);
				DoMethod(win, MUIM_Set, MUIA_Window_Open, TRUE);

				return(TRUE);
			}

		return(FALSE);
	}

ULONG SAVEDS ASM MCC_Main_Despatch(REG(a0,struct IClass *cl), REG(a2,Object *obj), REG(a1,Msg msg))
	{
		switch(msg->MethodID)
			{
				case OM_NEW:
					return MCC_Main_New(cl,obj,(APTR)msg);

				case MCC_NBitmap_Test:
					Printf("Hello\n");
				break;
			}

		return DoSuperMethodA(cl,obj,msg);
	}

/* */
ULONG SAVEDS MCC_Main_New(struct IClass *cl, Object *obj, struct opSet *msg)
	{
		Object *btn, *btn2;

		/* create class */
		obj = (Object *)DoSuperNew(cl, obj,
				MUIA_Window_ID,		MAKE_ID('M','A','I','N'),
				MUIA_Window_Title,	"NBitmap Demo Application",

				WindowContents, VGroup,
					Child, HGroup,
						Child, NBitmapFile("TBImages:cd"),
						Child, NBitmapFile("Development:Images/MasonIcons/appMI/AppMI/Images/Mail+News/AddressBook"),

                  Child, RectangleObject, End,
					End,

					Child, HGroup,
						MUIA_Frame, MUIV_Frame_ImageButton,

						Child, NBitmapObject,
							MUIA_NBitmap_Type, MUIV_NBitmap_Type_File,
							MUIA_NBitmap_Normal, "TBImages:userfind",
							MUIA_NBitmap_Ghosted, "TBImages:userfind_g",
							MUIA_NBitmap_Selected, "TBImages:userfind_s",
							MUIA_NBitmap_Label, "Find User",
							MUIA_NBitmap_Button, TRUE,
						End,

						Child, btn = NBitmapObject,
							MUIA_NBitmap_Type, MUIV_NBitmap_Type_File,
							MUIA_NBitmap_Normal, "TBImages:pdf",
							MUIA_NBitmap_Ghosted, "TBImages:pdf_g",
							MUIA_NBitmap_Selected, "TBImages:pdf_s",
							MUIA_NBitmap_Label, "PDF",
							MUIA_NBitmap_Button, TRUE,
						End,

						Child, btn2 = NBitmapObject,
							MUIA_NBitmap_Type, MUIV_NBitmap_Type_File,
							MUIA_NBitmap_Normal, "Development:Images/MasonIcons/appMI/AppMI/Images/Mail+News/DisplayAttachment",
							MUIA_NBitmap_Label, "Display",
							MUIA_NBitmap_Button, TRUE,
						End,

                  Child, RectangleObject, End,
					End,

					Child, RectangleObject, End,
					Child, SimpleButton("Quit"),
				End,
			TAG_MORE, msg->ops_AttrList);

		if(obj)
			{
				DoMethod(btn, MUIM_Notify, MUIA_Pressed, FALSE, obj, 1, MCC_NBitmap_Test);
				DoMethod(btn2, MUIM_Notify, MUIA_Pressed, FALSE, obj, 1, MCC_NBitmap_Test);

				return((ULONG)obj);
			}

		return(0);
	}

