//{{{}}}
// inifile.cpp
//

#include "stdafx.h"
#include "foldit.h"
#include "inifile.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CIniFile

//static members

//{{{  CString CIniFile::m_IniName;
CString CIniFile::m_IniName;

void CIniFile::Read (const char *Sec, const char *Ent, CString &Val)
{
        CString buf;

        ASSERT ((const char *)Val);

        if (GetPrivateProfileString (Sec, Ent, Val, buf.GetBuffer(U_FILEBUFFER), U_FILEBUFFER, m_IniName))
        {
          buf.ReleaseBuffer();
          Val = buf;
          TRACE1 ("Read |%s| from .ini file\n", (const char *)buf);
        }
        else
          buf.ReleaseBuffer();
}

//}}}
//{{{  void CIniFile::ReadInt (const char *Sec, const char *Ent, int &i)
void CIniFile::ReadInt (const char *Sec, const char *Ent, int &i)
{
        i = GetPrivateProfileInt (Sec, Ent, i, m_IniName);
}

//}}}
//{{{  void CIniFile::ReadUInt (const char *Sec, const char *Ent, UINT &i)
void CIniFile::ReadUInt (const char *Sec, const char *Ent, UINT &i)
{
        i = GetPrivateProfileInt (Sec, Ent, i, m_IniName);
}

//}}}
//{{{  void CIniFile::Write (const char *Sec, const char *Ent, const char *Val)
void CIniFile::Write (const char *Sec, const char *Ent, const char *Val)
{
        char buf[255];
        if (strchr (Val, ' '))
        {
               sprintf (buf, "\"%s\"", Val);
               WritePrivateProfileString (Sec, Ent, buf, m_IniName);
        }
        else
               WritePrivateProfileString (Sec, Ent, Val, m_IniName);
}

//}}}
//{{{  void CIniFile::WriteInt (const char *Sec, const char *Ent, int i)
void CIniFile::WriteInt (const char *Sec, const char *Ent, int i)
{
        char buf[32];
        sprintf (buf, "%d", i);
        Write (Sec, Ent, buf);
}

//}}}
//{{{  void CIniFile::WriteUInt (const char *Sec, const char *Ent, UINT i)
void CIniFile::WriteUInt (const char *Sec, const char *Ent, UINT i)
{
        char buf[32];
        sprintf (buf, "%ud", i);
        Write (Sec, Ent, buf);
}

//}}}
//{{{  void CIniFile::ReadSection (const char *Sec, CStringArray *pVal)
void CIniFile::ReadSection (const char *Sec, CStringArray *pVal)
{
        const int maxbuf = 1024; //HACK
        char buf[maxbuf];

        //force buf to null string
        buf[0] = '\0';

        //clear string array
        for (int i = 0; i < pVal->GetSize(); i++)
        {
               delete pVal->GetAt(i);
        }
        pVal->RemoveAll();
		pVal->FreeExtra();
        ASSERT (pVal->GetSize() == 0);

        //get section from .ini file in buf
        GetPrivateProfileString (Sec, NULL, buf, buf, maxbuf, m_IniName);

        if (!lstrlen(buf))
        {
               TRACE0 ("CIniFile::ReadSection: No styles in .ini file\n");
               TRACE1 ("                       .ini file name is |%s|\n",
                       (const char *)m_IniName);
               return;
        }
        //unpack into CStringArray

        char *nextstr = buf;

        //The strings from the ini file string are separated with null
        //the last one is terminated with a second null
        while (lstrlen (nextstr))
        {
               pVal->Add ((const char *)nextstr);
               nextstr += lstrlen (nextstr);
               nextstr++;

               TRACE2 ("CIniFile::ReadSection: Adding %s to section list (size is %d)\n",
                         (const char *)(pVal->GetAt(pVal->GetSize()-1)),
                         (int)(pVal->GetSize()));
        }
}

//}}}

