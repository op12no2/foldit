//{{{}}}
// inifile.h : header file
//

#ifndef __INIFILE_H__
#define __INIFILE_H__

///////////////////////////////////////////////////////////
// CIniFile

class CIniFile
{
public: //members
        static CString     m_IniName;     //name of init file

public: //general .ini file access functions
        static void Read             (const char *Sec, const char *Ent, CString &Val);
        static void ReadInt          (const char *Sec, const char *Ent, int &i);
        static void ReadUInt         (const char *Sec, const char *Ent, UINT &i);
        static void Write            (const char *Sec, const char *Ent, const char *Val);
        static void WriteInt         (const char *Sec, const char *Ent, int i);
        static void WriteUInt        (const char *Sec, const char *Ent, UINT i);
    static void ReadSection      (const char *Sec, CStringArray *pVal);
};

#endif
