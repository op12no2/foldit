// Copyright 1994,5,6 Leyfield Systems Ltd
// linelist.cpp: CLineList class

//{{{  history
///////////////////////////////////////////////////////////////////////////
//
// 20/3/96  CRJ Folded.
//
///////////////////////////////////////////////////////////////////////////

//}}}

//{{{  includes

#include "stdafx.h"
#include "foldit.h"
#include "linelist.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

//}}}
//{{{  ~CLineList

CLineList::~CLineList()
{
    CLine *pLine;
    POSITION p = GetHeadPosition();

    while (p)
    {
        pLine = (CLine *)GetNext(p);
        delete pLine;
    }

    RemoveAll();
}

//}}}

