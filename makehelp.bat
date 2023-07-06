
REM -- First make map file from App Studio generated resource.h
echo // MAKEHELP.BAT generated Help Map file.  Used by FOLDIT.HPJ. >hlp\foldit.hm
echo. >>hlp\foldit.hm
echo // Commands (ID_* and IDM_*) >>hlp\foldit.hm
makehm ID_,HID_,0x10000 IDM_,HIDM_,0x10000 resource.h >>hlp\foldit.hm
echo. >>hlp\foldit.hm
echo // Prompts (IDP_*) >>hlp\foldit.hm
makehm IDP_,HIDP_,0x30000 resource.h >>hlp\foldit.hm
echo. >>hlp\foldit.hm
echo // Resources (IDR_*) >>hlp\foldit.hm
makehm IDR_,HIDR_,0x20000 resource.h >>hlp\foldit.hm
echo. >>hlp\foldit.hm
echo // Dialogs (IDD_*) >>hlp\foldit.hm
makehm IDD_,HIDD_,0x20000 resource.h >>hlp\foldit.hm
echo. >>hlp\foldit.hm
echo // Frame Controls (IDW_*) >>hlp\foldit.hm
makehm IDW_,HIDW_,0x50000 resource.h >>hlp\foldit.hm
REM -- Make help for Project FOLDIT
call hc31 foldit.hpj
echo.
