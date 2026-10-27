//{{{}}}
// Copyright 1994,5,6 Leyfield Systems Ltd
// helper.cpp: misc helper functions

//{{{  include

#include "stdafx.h"
#include "foldit.h"
#include "inifile.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif
//}}}

//{{{  new()

//void __far *operator new(size_t size)
//{
//   //AfxMessageBox ("my new");
//   //__far *m;
//   return ::new char[size];
//   //if (m==NULL)
//   //  MemoryProblem();
//   //return m;
//}

//}}}
//{{{  App()

CFolditApp *App ()
{
    return ((CFolditApp *)AfxGetApp());
}
//}}}
//{{{  NotImplementedYet()

void NotImplementedYet ()
{
    AfxMessageBox ("Sorry!, this feature is not implemented yet");
}

//}}}
//{{{  MemoryProblem()

void MemoryProblem()
{
   AfxMessageBox ("Save your work NOW!");
}

//}}}
//{{{  OneTimeMessage() (DISABLED)

//HACK BOOL OneTimeMessage (const char *Key, const char *Message)
//{
//    int NotShownYet = 0;
//    CIniFile::ReadInt ("OneTimeMessages", Key, NotShownYet);
//
//    if (NotShownYet)
//    {
//        ::MessageBox (NULL, (CString)Message + "\n\nYou will not see this message again!",
//                        "One-Time Message", MB_OK);
  //    CIniFile::WriteInt ("OneTimeMessages", Key, 0);
//        return TRUE;
//    }
//
//    return FALSE;
//}

//}}}
//{{{  WarningBox()

void WarningBox (const char *Message)
{
    ::MessageBox (NULL, Message, "FoldIt Message", MB_OK | MB_ICONEXCLAMATION);
}

//}}}
//{{{  ImplementationRestriction()

void ImplementationRestriction (const char *Message)
{
    ::MessageBox (NULL, Message, "Implementation Restriction", MB_OK | MB_ICONEXCLAMATION);
}

//}}}
//{{{  EnvironmentRestr

void EnvironmentRestriction (const char *Message)
{
    ::MessageBox (NULL, Message, "Environment Restriction", MB_OK | MB_ICONEXCLAMATION);
}
//}}}

