/*
 * ClaudeCodeManager 1.1 r0 - probe body fix, credential scoping, console attach
 * Copyright (c) 2026 Fongap
 * SPDX-License-Identifier: MIT
 *
 * No external runtime required. Built with clang/lld in freestanding mode.
 */

typedef unsigned char BYTE;
typedef unsigned short WORD;
typedef unsigned short WCHAR;
typedef WCHAR wchar_t;
typedef unsigned int UINT;
typedef unsigned long DWORD;
typedef long LONG;
typedef long long LONGLONG;
typedef unsigned long long ULONGLONG;
typedef unsigned long long ULONG_PTR;
typedef ULONG_PTR DWORD_PTR;
typedef long long LONG_PTR;
typedef ULONG_PTR WPARAM;
typedef ULONG_PTR UINT_PTR;
typedef LONG_PTR LPARAM;
typedef LONG_PTR LRESULT;
typedef int BOOL;
typedef void VOID;
typedef void *PVOID;
typedef void *HANDLE;
typedef HANDLE HWND;
typedef HANDLE HINSTANCE;
typedef HANDLE HMODULE;
typedef HANDLE HICON;
typedef HANDLE HCURSOR;
typedef HANDLE HBRUSH;
typedef HANDLE HDC;
typedef HANDLE HPEN;
typedef HANDLE HBITMAP;
typedef HANDLE HRGN;
typedef HANDLE HMENU;
typedef HANDLE HFONT;
typedef HANDLE HKEY;
typedef HANDLE HGDIOBJ;
typedef HANDLE HGLOBAL;
typedef HANDLE HDESK;
typedef HANDLE LPITEMIDLIST;
typedef const WCHAR *LPCWSTR;
typedef WCHAR *LPWSTR;
typedef const char *LPCSTR;
typedef char *LPSTR;
typedef void *FARPROC;
typedef DWORD (__stdcall *LPTHREAD_START_ROUTINE)(PVOID);
typedef LONG HRESULT;
typedef unsigned long ULONG;
typedef LRESULT (__stdcall *SUBCLASSPROC)(HWND,UINT,WPARAM,LPARAM,UINT_PTR,DWORD_PTR);
typedef struct _GUID { DWORD Data1; WORD Data2; WORD Data3; BYTE Data4[8]; } GUID;
typedef struct tagTRACKMOUSEEVENT { DWORD cbSize; DWORD dwFlags; HWND hwndTrack; DWORD dwHoverTime; } TRACKMOUSEEVENT;
typedef const GUID *REFIID;
typedef const GUID *REFCLSID;

typedef struct IShellLinkW IShellLinkW;
typedef struct IShellLinkWVtbl {
    HRESULT (__stdcall *QueryInterface)(IShellLinkW*,REFIID,PVOID*);
    ULONG (__stdcall *AddRef)(IShellLinkW*);
    ULONG (__stdcall *Release)(IShellLinkW*);
    HRESULT (__stdcall *GetPath)(IShellLinkW*,LPWSTR,int,PVOID,DWORD);
    HRESULT (__stdcall *GetIDList)(IShellLinkW*,PVOID*);
    HRESULT (__stdcall *SetIDList)(IShellLinkW*,PVOID);
    HRESULT (__stdcall *GetDescription)(IShellLinkW*,LPWSTR,int);
    HRESULT (__stdcall *SetDescription)(IShellLinkW*,LPCWSTR);
    HRESULT (__stdcall *GetWorkingDirectory)(IShellLinkW*,LPWSTR,int);
    HRESULT (__stdcall *SetWorkingDirectory)(IShellLinkW*,LPCWSTR);
    HRESULT (__stdcall *GetArguments)(IShellLinkW*,LPWSTR,int);
    HRESULT (__stdcall *SetArguments)(IShellLinkW*,LPCWSTR);
    HRESULT (__stdcall *GetHotkey)(IShellLinkW*,WORD*);
    HRESULT (__stdcall *SetHotkey)(IShellLinkW*,WORD);
    HRESULT (__stdcall *GetShowCmd)(IShellLinkW*,int*);
    HRESULT (__stdcall *SetShowCmd)(IShellLinkW*,int);
    HRESULT (__stdcall *GetIconLocation)(IShellLinkW*,LPWSTR,int,int*);
    HRESULT (__stdcall *SetIconLocation)(IShellLinkW*,LPCWSTR,int);
    HRESULT (__stdcall *SetRelativePath)(IShellLinkW*,LPCWSTR,DWORD);
    HRESULT (__stdcall *Resolve)(IShellLinkW*,HWND,DWORD);
    HRESULT (__stdcall *SetPath)(IShellLinkW*,LPCWSTR);
} IShellLinkWVtbl;
struct IShellLinkW { IShellLinkWVtbl *lpVtbl; };

typedef struct IPersistFile IPersistFile;
typedef struct IPersistFileVtbl {
    HRESULT (__stdcall *QueryInterface)(IPersistFile*,REFIID,PVOID*);
    ULONG (__stdcall *AddRef)(IPersistFile*);
    ULONG (__stdcall *Release)(IPersistFile*);
    HRESULT (__stdcall *GetClassID)(IPersistFile*,GUID*);
    HRESULT (__stdcall *IsDirty)(IPersistFile*);
    HRESULT (__stdcall *Load)(IPersistFile*,LPCWSTR,DWORD);
    HRESULT (__stdcall *Save)(IPersistFile*,LPCWSTR,BOOL);
    HRESULT (__stdcall *SaveCompleted)(IPersistFile*,LPCWSTR);
    HRESULT (__stdcall *GetCurFile)(IPersistFile*,LPWSTR*);
} IPersistFileVtbl;
struct IPersistFile { IPersistFileVtbl *lpVtbl; };

typedef struct _SECURITY_ATTRIBUTES {
    DWORD nLength;
    PVOID lpSecurityDescriptor;
    BOOL bInheritHandle;
} SECURITY_ATTRIBUTES;

typedef struct _STARTUPINFOW {
    DWORD cb;
    LPWSTR lpReserved;
    LPWSTR lpDesktop;
    LPWSTR lpTitle;
    DWORD dwX;
    DWORD dwY;
    DWORD dwXSize;
    DWORD dwYSize;
    DWORD dwXCountChars;
    DWORD dwYCountChars;
    DWORD dwFillAttribute;
    DWORD dwFlags;
    WORD wShowWindow;
    WORD cbReserved2;
    BYTE *lpReserved2;
    HANDLE hStdInput;
    HANDLE hStdOutput;
    HANDLE hStdError;
} STARTUPINFOW;

typedef struct _PROCESS_INFORMATION {
    HANDLE hProcess;
    HANDLE hThread;
    DWORD dwProcessId;
    DWORD dwThreadId;
} PROCESS_INFORMATION;

typedef struct _WIN32_FIND_DATAW {
    DWORD dwFileAttributes;
    DWORD ftCreationTimeLow;
    DWORD ftCreationTimeHigh;
    DWORD ftLastAccessTimeLow;
    DWORD ftLastAccessTimeHigh;
    DWORD ftLastWriteTimeLow;
    DWORD ftLastWriteTimeHigh;
    DWORD nFileSizeHigh;
    DWORD nFileSizeLow;
    DWORD dwReserved0;
    DWORD dwReserved1;
    WCHAR cFileName[260];
    WCHAR cAlternateFileName[14];
} WIN32_FIND_DATAW;

typedef struct tagWNDCLASSEXW {
    UINT cbSize;
    UINT style;
    LRESULT (__stdcall *lpfnWndProc)(HWND, UINT, WPARAM, LPARAM);
    int cbClsExtra;
    int cbWndExtra;
    HINSTANCE hInstance;
    HICON hIcon;
    HCURSOR hCursor;
    HBRUSH hbrBackground;
    LPCWSTR lpszMenuName;
    LPCWSTR lpszClassName;
    HICON hIconSm;
} WNDCLASSEXW;

typedef struct tagMSG {
    HWND hwnd;
    UINT message;
    WPARAM wParam;
    LPARAM lParam;
    DWORD time;
    LONG pt_x;
    LONG pt_y;
    DWORD lPrivate;
} MSG;

typedef struct tagRECT { LONG left; LONG top; LONG right; LONG bottom; } RECT;
typedef struct tagSIZE { LONG cx; LONG cy; } SIZE;
typedef struct tagPAINTSTRUCT {
    HDC hdc;
    BOOL fErase;
    RECT rcPaint;
    BOOL fRestore;
    BOOL fIncUpdate;
    BYTE rgbReserved[32];
} PAINTSTRUCT;
typedef struct tagDRAWITEMSTRUCT {
    UINT CtlType;
    UINT CtlID;
    UINT itemID;
    UINT itemAction;
    UINT itemState;
    HWND hwndItem;
    HDC hDC;
    RECT rcItem;
    ULONG_PTR itemData;
} DRAWITEMSTRUCT;
typedef struct tagMEASUREITEMSTRUCT {
    UINT CtlType;
    UINT CtlID;
    UINT itemID;
    UINT itemWidth;
    UINT itemHeight;
    ULONG_PTR itemData;
} MEASUREITEMSTRUCT;
typedef struct tagMINMAXINFO {
    LONG ptReserved_x, ptReserved_y;
    LONG ptMaxSize_x, ptMaxSize_y;
    LONG ptMaxPosition_x, ptMaxPosition_y;
    LONG ptMinTrackSize_x, ptMinTrackSize_y;
    LONG ptMaxTrackSize_x, ptMaxTrackSize_y;
} MINMAXINFO;

typedef struct _BROWSEINFOW {
    HWND hwndOwner;
    LPITEMIDLIST pidlRoot;
    LPWSTR pszDisplayName;
    LPCWSTR lpszTitle;
    UINT ulFlags;
    PVOID lpfn;
    LPARAM lParam;
    int iImage;
} BROWSEINFOW;

typedef struct tagOFNW {
    DWORD lStructSize;
    HWND hwndOwner;
    HINSTANCE hInstance;
    LPCWSTR lpstrFilter;
    LPWSTR lpstrCustomFilter;
    DWORD nMaxCustFilter;
    DWORD nFilterIndex;
    LPWSTR lpstrFile;
    DWORD nMaxFile;
    LPWSTR lpstrFileTitle;
    DWORD nMaxFileTitle;
    LPCWSTR lpstrInitialDir;
    LPCWSTR lpstrTitle;
    DWORD Flags;
    WORD nFileOffset;
    WORD nFileExtension;
    LPCWSTR lpstrDefExt;
    LPARAM lCustData;
    PVOID lpfnHook;
    LPCWSTR lpTemplateName;
    PVOID pvReserved;
    DWORD dwReserved;
    DWORD FlagsEx;
} OPENFILENAMEW;


typedef struct _FILETIME_CCM { DWORD dwLowDateTime; DWORD dwHighDateTime; } FILETIME_CCM;
typedef struct _CREDENTIALW_CCM {
    DWORD Flags;
    DWORD Type;
    LPWSTR TargetName;
    LPWSTR Comment;
    FILETIME_CCM LastWritten;
    DWORD CredentialBlobSize;
    BYTE *CredentialBlob;
    DWORD Persist;
    DWORD AttributeCount;
    PVOID Attributes;
    LPWSTR TargetAlias;
    LPWSTR UserName;
} CREDENTIALW_CCM;

typedef HANDLE HINTERNET;
typedef WORD INTERNET_PORT;
typedef struct _URL_COMPONENTS_CCM {
    DWORD dwStructSize;
    LPWSTR lpszScheme;
    DWORD dwSchemeLength;
    int nScheme;
    LPWSTR lpszHostName;
    DWORD dwHostNameLength;
    INTERNET_PORT nPort;
    LPWSTR lpszUserName;
    DWORD dwUserNameLength;
    LPWSTR lpszPassword;
    DWORD dwPasswordLength;
    LPWSTR lpszUrlPath;
    DWORD dwUrlPathLength;
    LPWSTR lpszExtraInfo;
    DWORD dwExtraInfoLength;
} URL_COMPONENTS_CCM;

typedef struct tagINITCOMMONCONTROLSEX {
    DWORD dwSize;
    DWORD dwICC;
} INITCOMMONCONTROLSEX;

#define TRUE 1
#define FALSE 0
#define NULL 0
#define MAX_PATH 260
#define INVALID_HANDLE_VALUE ((HANDLE)(LONG_PTR)-1)
#define INFINITE 0xFFFFFFFF
#define WAIT_OBJECT_0 0
#define STILL_ACTIVE 259
#define ERROR_ALREADY_EXISTS 183
#define COINIT_APARTMENTTHREADED 0x2
#define CLSCTX_INPROC_SERVER 0x1
#define CSIDL_DESKTOPDIRECTORY 0x0010
#define SHGFP_TYPE_CURRENT 0

#define GENERIC_READ  0x80000000UL
#define GENERIC_WRITE 0x40000000UL
#define FILE_SHARE_READ 0x00000001
#define FILE_SHARE_WRITE 0x00000002
#define CREATE_NEW 1
#define CREATE_ALWAYS 2
#define OPEN_EXISTING 3
#define OPEN_ALWAYS 4
#define FILE_ATTRIBUTE_NORMAL 0x00000080
#define FILE_ATTRIBUTE_DIRECTORY 0x00000010
#define FILE_ATTRIBUTE_HIDDEN 0x00000002
#define MOVEFILE_REPLACE_EXISTING 0x00000001
#define MOVEFILE_WRITE_THROUGH 0x00000008
#define INVALID_FILE_ATTRIBUTES 0xFFFFFFFF

#define HANDLE_FLAG_INHERIT 0x00000001
#define STARTF_USESHOWWINDOW 0x00000001
#define STARTF_USESTDHANDLES 0x00000100
#define STD_INPUT_HANDLE ((DWORD)-10)
#define STD_OUTPUT_HANDLE ((DWORD)-11)
#define STD_ERROR_HANDLE ((DWORD)-12)
#define STARTF_USESTDHANDLES 0x00000100
#define CREATE_NO_WINDOW 0x08000000
#define CREATE_NEW_CONSOLE 0x00000010
#define CREATE_UNICODE_ENVIRONMENT 0x00000400
#define CREATE_NEW_PROCESS_GROUP 0x00000200
#define DESKTOP_READOBJECTS 0x0001
#define DESKTOP_CREATEWINDOW 0x0002
#define DESKTOP_ENUMERATE 0x0040
#define DESKTOP_WRITEOBJECTS 0x0080

#define CRED_TYPE_GENERIC 1
#define CRED_PERSIST_LOCAL_MACHINE 2
#define WINHTTP_ACCESS_TYPE_DEFAULT_PROXY 0
#define WINHTTP_ACCESS_TYPE_NO_PROXY 1
#define WINHTTP_ACCESS_TYPE_NAMED_PROXY 3
#define WINHTTP_FLAG_SECURE 0x00800000
#define WINHTTP_QUERY_STATUS_CODE 19
#define WINHTTP_QUERY_FLAG_NUMBER 0x20000000
#define WINHTTP_ADDREQ_FLAG_ADD 0x20000000
#define WINHTTP_ADDREQ_FLAG_REPLACE 0x80000000
#define INTERNET_SCHEME_HTTP 1
#define INTERNET_SCHEME_HTTPS 2
#define CP_ACP 0
#define CP_OEMCP 1
#define CP_UTF8 65001
#define MB_ERR_INVALID_CHARS 0x00000008

#define WM_CREATE 0x0001
#define WM_DESTROY 0x0002
#define WM_SIZE 0x0005
#define SIZE_MINIMIZED 1
#define WM_PAINT 0x000F
#define WM_ERASEBKGND 0x0014
#define WM_CLOSE 0x0010
#define WM_GETMINMAXINFO 0x0024
#define WM_DRAWITEM 0x002B
#define WM_MEASUREITEM 0x002C
#define WM_CTLCOLORMSGBOX 0x0132
#define WM_CTLCOLOREDIT 0x0133
#define WM_CTLCOLORLISTBOX 0x0134
#define WM_CTLCOLORBTN 0x0135
#define WM_CTLCOLORDLG 0x0136
#define WM_CTLCOLORSCROLLBAR 0x0137
#define WM_CTLCOLORSTATIC 0x0138
#define WM_TIMER 0x0113
#define WM_COMMAND 0x0111
#define WM_SETFONT 0x0030
#define WM_DPICHANGED 0x02E0
#define WM_NCDESTROY 0x0082
#define WM_NCPAINT 0x0085
#define WM_MOUSEMOVE 0x0200
#define WM_MOUSELEAVE 0x02A3
#define WM_PRINT 0x0317
#define WM_PRINTCLIENT 0x0318
#define WM_SETTINGCHANGE 0x001A
#define WM_USER 0x0400
#define WM_APP 0x8000
#define WM_DISPLAYCHANGE 0x007E
#define STGM_READ 0x00000000
#define EM_SETSEL 0x00B1
#define EM_REPLACESEL 0x00C2
#define EM_SCROLLCARET 0x00B7
#define EM_SETLIMITTEXT 0x00C5
#define EM_SETMARGINS 0x00D3
#define EC_LEFTMARGIN 0x0001
#define EC_RIGHTMARGIN 0x0002
#define EM_SETCUEBANNER 0x1501
#define CB_ADDSTRING 0x0143
#define CB_RESETCONTENT 0x014B
#define CB_GETCURSEL 0x0147
#define CB_SETCURSEL 0x014E
#define CB_SETITEMHEIGHT 0x0153
#define CBN_SELCHANGE 1
#define CBN_DROPDOWN 7
#define EN_KILLFOCUS 0x0200
#define LB_ADDSTRING 0x0180
#define LB_RESETCONTENT 0x0184
#define LB_GETCURSEL 0x0188
#define LB_GETTEXT 0x0189
#define LB_SETCURSEL 0x0186
#define LBN_DBLCLK 2
#define BN_CLICKED 0
#define PBM_SETMARQUEE (WM_USER + 10)

/* Custom messages for thread-to-GUI communication */
#define WM_APP_NETWORK_TEST_COMPLETE  (WM_APP + 1)
#define WM_APP_MODEL_DISCOVERY_COMPLETE (WM_APP + 2)

#define WS_OVERLAPPED 0x00000000UL
#define WS_CAPTION 0x00C00000UL
#define WS_SYSMENU 0x00080000UL
#define WS_MINIMIZEBOX 0x00020000UL
#define WS_MAXIMIZEBOX 0x00010000UL
#define WS_THICKFRAME 0x00040000UL
#define WS_VISIBLE 0x10000000UL
#define WS_CHILD 0x40000000UL
#define WS_VSCROLL 0x00200000UL
#define WS_TABSTOP 0x00010000UL
#define WS_CLIPCHILDREN 0x02000000UL
#define WS_CLIPSIBLINGS 0x04000000UL
#define WS_BORDER 0x00800000UL
#define ES_AUTOHSCROLL 0x0080
#define ES_MULTILINE 0x0004
#define ES_AUTOVSCROLL 0x0040
#define ES_READONLY 0x0800
#define ES_PASSWORD 0x0020
#define ES_NUMBER 0x2000
#define BS_PUSHBUTTON 0x00000000UL
#define BS_OWNERDRAW 0x0000000BUL
#define LBS_NOTIFY 0x0001
#define LBS_OWNERDRAWFIXED 0x0010
#define LBS_HASSTRINGS 0x0040
#define LBS_NOINTEGRALHEIGHT 0x0100
#define CBS_DROPDOWN 0x0002
#define CBS_DROPDOWNLIST 0x0003
#define CBS_AUTOHSCROLL 0x0040
#define SS_LEFT 0x00000000UL
#define PBS_MARQUEE 0x00000008

#define WS_EX_CLIENTEDGE 0x00000200UL
#define WS_EX_CONTROLPARENT 0x00010000UL

#define SW_HIDE 0
#define SW_SHOWNORMAL 1
#define SW_SHOW 5
#define SWP_NOZORDER 0x0004
#define SWP_NOACTIVATE 0x0010
#define SPI_GETWORKAREA 0x0030
#define TME_LEAVE 0x00000002
#define CW_USEDEFAULT ((int)0x80000000)
#define COLOR_WINDOW 5
#define IDC_ARROW ((LPCWSTR)32512)
#define IDI_APPLICATION ((LPCWSTR)32512)
#define DEFAULT_GUI_FONT 17
#define DC_BRUSH 18
#define DC_PEN 19
#define PS_SOLID 0
#define TRANSPARENT 1
#define OPAQUE 2
#define FW_NORMAL 400
#define FW_MEDIUM 500
#define FW_SEMIBOLD 600
#define FW_BOLD 700
#define DEFAULT_CHARSET 1
#define OUT_DEFAULT_PRECIS 0
#define CLIP_DEFAULT_PRECIS 0
#define CLEARTYPE_QUALITY 5
#define DEFAULT_PITCH 0
#define FF_DONTCARE 0
#define IMAGE_ICON 1
#define LR_DEFAULTCOLOR 0x0000
#define DI_NORMAL 0x0003
#define DT_LEFT 0x0000
#define DT_CENTER 0x0001
#define DT_RIGHT 0x0002
#define DT_VCENTER 0x0004
#define DT_SINGLELINE 0x0020
#define DT_END_ELLIPSIS 0x00008000
#define DT_NOPREFIX 0x00000800
#define ODT_LISTBOX 2
#define ODT_BUTTON 4
#define ODS_SELECTED 0x0001
#define ODS_GRAYED 0x0002
#define ODS_DISABLED 0x0004
#define ODS_FOCUS 0x0010
#define COLOR_BTNFACE 15
#define DWMWA_WINDOW_CORNER_PREFERENCE 33
#define DWMWA_BORDER_COLOR 34
#define DWMWA_CAPTION_COLOR 35
#define DWMWA_TEXT_COLOR 36
#define DWMWCP_ROUND 2
#define ICON_BIG 1
#define ICON_SMALL 0
#define WM_SETICON 0x0080
#define HWND_BROADCAST ((HWND)(LONG_PTR)0xFFFF)
#define SMTO_ABORTIFHUNG 0x0002

#define BIF_RETURNONLYFSDIRS 0x00000001
#define BIF_NEWDIALOGSTYLE 0x00000040
#define BIF_EDITBOX 0x00000010
#define BIF_USENEWUI (BIF_NEWDIALOGSTYLE | BIF_EDITBOX)

#define OFN_PATHMUSTEXIST 0x00000800
#define OFN_FILEMUSTEXIST 0x00001000
#define OFN_EXPLORER 0x00080000

#define ICC_PROGRESS_CLASS 0x00000020

#define REG_SZ 1
#define REG_EXPAND_SZ 2
#define REG_DWORD 4
#define KEY_READ 0x20019
#define KEY_WRITE 0x20006
#define KEY_QUERY_VALUE 0x0001
#define KEY_SET_VALUE 0x0002
#define ERROR_SUCCESS 0
#define ERROR_FILE_NOT_FOUND 2
#define MB_OK 0x00000000
#define MB_OKCANCEL 0x00000001
#define MB_YESNOCANCEL 0x00000003
#define MB_YESNO 0x00000004
#define MB_ICONERROR 0x00000010
#define MB_ICONQUESTION 0x00000020
#define MB_ICONWARNING 0x00000030
#define MB_ICONINFORMATION 0x00000040
#define IDOK 1
#define IDCANCEL 2
#define IDYES 6
#define IDNO 7
#define HKEY_CURRENT_USER ((HKEY)(LONG_PTR)-2147483647LL)

#define IDC_PROXY 1001
#define IDC_URL 1002
#define IDC_WORKSPACE 1003
#define IDC_BROWSE 1004
#define IDC_PROJECTS 1005
#define IDC_REFRESH 1006
#define IDC_INSTALL 1007
#define IDC_LAUNCH 1008
#define IDC_TERMINAL 1009
#define IDC_FOLDER 1010
#define IDC_LOG 1011
#define IDC_STATUS 1012
#define IDC_PROGRESS 1013
#define IDC_SHORTCUT 1014
#define IDC_NETWORK_MODE 1015
#define IDC_TEST_NETWORK 1016
#define IDC_IMPORT_SETTINGS 1017
#define IDC_OPEN_SETTINGS 1018
#define IDC_MODEL_WIZARD 1019
#define IDC_FONGAP_LINK 1020

#define IDC_WIZ_SCOPE 2001
#define IDC_WIZ_PROVIDER 2002
#define IDC_WIZ_BASE_URL 2003
#define IDC_WIZ_SECRET_TYPE 2004
#define IDC_WIZ_SECRET 2005
#define IDC_WIZ_DEFAULT_MODEL 2006
#define IDC_WIZ_OPUS_MODEL 2007
#define IDC_WIZ_SONNET_MODEL 2008
#define IDC_WIZ_HAIKU_MODEL 2009
#define IDC_WIZ_SUBAGENT_MODEL 2010
#define IDC_WIZ_EFFORT 2011
#define IDC_WIZ_RECOMMENDED 2012
#define IDC_WIZ_SAVE 2013
#define IDC_WIZ_CANCEL 2014
#define IDC_WIZ_TEST_MODELS 2015
#define IDC_WIZ_CONTEXT_CAPACITY 2100
#define IDC_WIZ_CONTEXT_BUDGET 2110
#define IDC_WIZ_CONTEXT_WARNING 2120
#define IDC_WIZ_CONTEXT_RESERVE 2130
#define IDC_WIZ_CONTEXT_EFFORT 2140
#define MODEL_ROLE_COUNT 5

/* Kernel32 imports */
__declspec(dllimport) HMODULE __stdcall LoadLibraryW(LPCWSTR);
__declspec(dllimport) FARPROC __stdcall GetProcAddress(HMODULE, LPCSTR);
__declspec(dllimport) VOID __stdcall ExitProcess(UINT);
__declspec(dllimport) HMODULE __stdcall GetModuleHandleW(LPCWSTR);
__declspec(dllimport) LPWSTR __stdcall GetCommandLineW(VOID);
#define ATTACH_PARENT_PROCESS ((DWORD)-1)

__declspec(dllimport) BOOL __stdcall AllocConsole(VOID);
__declspec(dllimport) BOOL __stdcall FreeConsole(VOID);
__declspec(dllimport) BOOL __stdcall AttachConsole(DWORD);
__declspec(dllimport) HWND __stdcall GetConsoleWindow(VOID);
__declspec(dllimport) BOOL __stdcall SetConsoleTitleW(LPCWSTR);
__declspec(dllimport) HANDLE __stdcall GetStdHandle(DWORD);
__declspec(dllimport) BOOL __stdcall SetConsoleCP(UINT);
__declspec(dllimport) BOOL __stdcall SetConsoleOutputCP(UINT);
__declspec(dllimport) DWORD __stdcall GetLastError(VOID);
__declspec(dllimport) BOOL __stdcall CreateProcessW(LPCWSTR, LPWSTR, PVOID, PVOID, BOOL, DWORD, PVOID, LPCWSTR, STARTUPINFOW*, PROCESS_INFORMATION*);
__declspec(dllimport) BOOL __stdcall CreatePipe(HANDLE*, HANDLE*, SECURITY_ATTRIBUTES*, DWORD);
__declspec(dllimport) BOOL __stdcall SetHandleInformation(HANDLE, DWORD, DWORD);
__declspec(dllimport) BOOL __stdcall CloseHandle(HANDLE);
__declspec(dllimport) DWORD __stdcall WaitForSingleObject(HANDLE, DWORD);
__declspec(dllimport) BOOL __stdcall GetExitCodeProcess(HANDLE, DWORD*);
__declspec(dllimport) BOOL __stdcall PeekNamedPipe(HANDLE, PVOID, DWORD, DWORD*, DWORD*, DWORD*);
__declspec(dllimport) BOOL __stdcall ReadFile(HANDLE, PVOID, DWORD, DWORD*, PVOID);
__declspec(dllimport) BOOL __stdcall WriteFile(HANDLE, const VOID*, DWORD, DWORD*, PVOID);
__declspec(dllimport) HANDLE __stdcall CreateFileW(LPCWSTR, DWORD, DWORD, SECURITY_ATTRIBUTES*, DWORD, DWORD, HANDLE);
__declspec(dllimport) BOOL __stdcall DeleteFileW(LPCWSTR);
__declspec(dllimport) BOOL __stdcall CreateDirectoryW(LPCWSTR,SECURITY_ATTRIBUTES*);
__declspec(dllimport) BOOL __stdcall CopyFileW(LPCWSTR,LPCWSTR,BOOL);
__declspec(dllimport) BOOL __stdcall MoveFileExW(LPCWSTR,LPCWSTR,DWORD);
__declspec(dllimport) DWORD __stdcall GetTempPathW(DWORD, LPWSTR);
__declspec(dllimport) DWORD __stdcall GetCurrentProcessId(VOID);
__declspec(dllimport) DWORD __stdcall GetCurrentDirectoryW(DWORD, LPWSTR);
__declspec(dllimport) DWORD __stdcall GetEnvironmentVariableW(LPCWSTR, LPWSTR, DWORD);
__declspec(dllimport) BOOL __stdcall SetEnvironmentVariableW(LPCWSTR, LPCWSTR);
__declspec(dllimport) DWORD __stdcall SearchPathW(LPCWSTR, LPCWSTR, LPCWSTR, DWORD, LPWSTR, LPWSTR*);
__declspec(dllimport) DWORD __stdcall GetFileAttributesW(LPCWSTR);
__declspec(dllimport) HANDLE __stdcall CreateThread(SECURITY_ATTRIBUTES*, ULONG_PTR, LPTHREAD_START_ROUTINE, PVOID, DWORD, DWORD*);
__declspec(dllimport) VOID __stdcall Sleep(DWORD);
__declspec(dllimport) ULONGLONG __stdcall GetTickCount64(VOID);
__declspec(dllimport) int __stdcall MultiByteToWideChar(UINT, DWORD, LPCSTR, int, LPWSTR, int);
__declspec(dllimport) int __stdcall WideCharToMultiByte(UINT,DWORD,LPCWSTR,int,LPSTR,int,LPCSTR,BOOL*);
__declspec(dllimport) HANDLE __stdcall FindFirstFileW(LPCWSTR, WIN32_FIND_DATAW*);
__declspec(dllimport) BOOL __stdcall FindNextFileW(HANDLE, WIN32_FIND_DATAW*);
__declspec(dllimport) BOOL __stdcall FindClose(HANDLE);

/* Dynamic API declarations */
typedef unsigned short ATOM;
typedef ATOM (__stdcall *RegisterClassExW_t)(const WNDCLASSEXW*);
typedef HWND (__stdcall *CreateWindowExW_t)(DWORD,LPCWSTR,LPCWSTR,DWORD,int,int,int,int,HWND,HMENU,HINSTANCE,PVOID);
typedef BOOL (__stdcall *ShowWindow_t)(HWND,int);
typedef BOOL (__stdcall *UpdateWindow_t)(HWND);
typedef int (__stdcall *GetMessageW_t)(MSG*,HWND,UINT,UINT);
typedef BOOL (__stdcall *TranslateMessage_t)(const MSG*);
typedef LRESULT (__stdcall *DispatchMessageW_t)(const MSG*);
typedef LRESULT (__stdcall *DefWindowProcW_t)(HWND,UINT,WPARAM,LPARAM);
typedef VOID (__stdcall *PostQuitMessage_t)(int);
typedef LRESULT (__stdcall *SendMessageW_t)(HWND,UINT,WPARAM,LPARAM);
typedef BOOL (__stdcall *PostMessageW_t)(HWND,UINT,WPARAM,LPARAM);
typedef BOOL (__stdcall *SetWindowTextW_t)(HWND,LPCWSTR);
typedef int (__stdcall *GetWindowTextW_t)(HWND,LPWSTR,int);
typedef int (__stdcall *GetWindowTextLengthW_t)(HWND);
typedef int (__stdcall *MessageBoxW_t)(HWND,LPCWSTR,LPCWSTR,UINT);
typedef BOOL (__stdcall *EnableWindow_t)(HWND,BOOL);
typedef BOOL (__stdcall *MoveWindow_t)(HWND,int,int,int,int,BOOL);
typedef HCURSOR (__stdcall *LoadCursorW_t)(HINSTANCE,LPCWSTR);
typedef HICON (__stdcall *LoadIconW_t)(HINSTANCE,LPCWSTR);
typedef LRESULT (__stdcall *SendMessageTimeoutW_t)(HWND,UINT,WPARAM,LPARAM,UINT,UINT,ULONG_PTR*);
typedef HGDIOBJ (__stdcall *GetStockObject_t)(int);
typedef LPITEMIDLIST (__stdcall *SHBrowseForFolderW_t)(BROWSEINFOW*);
typedef BOOL (__stdcall *SHGetPathFromIDListW_t)(LPITEMIDLIST,LPWSTR);
typedef HANDLE (__stdcall *ShellExecuteW_t)(HWND,LPCWSTR,LPCWSTR,LPCWSTR,LPCWSTR,int);
typedef VOID (__stdcall *CoTaskMemFree_t)(PVOID);
typedef LONG (__stdcall *RegCreateKeyExW_t)(HKEY,LPCWSTR,DWORD,LPWSTR,DWORD,DWORD,PVOID,HKEY*,DWORD*);
typedef LONG (__stdcall *RegOpenKeyExW_t)(HKEY,LPCWSTR,DWORD,DWORD,HKEY*);
typedef LONG (__stdcall *RegQueryValueExW_t)(HKEY,LPCWSTR,DWORD*,DWORD*,BYTE*,DWORD*);
typedef LONG (__stdcall *RegSetValueExW_t)(HKEY,LPCWSTR,DWORD,DWORD,const BYTE*,DWORD);
typedef LONG (__stdcall *RegCloseKey_t)(HKEY);
typedef BOOL (__stdcall *InitCommonControlsEx_t)(const INITCOMMONCONTROLSEX*);
typedef BOOL (__stdcall *SetProcessDpiAwarenessContext_t)(HANDLE);
typedef UINT (__stdcall *GetDpiForWindow_t)(HWND);


/* Conventional imports for Windows UI and registry APIs. */
__declspec(dllimport) ATOM __stdcall RegisterClassExW(const WNDCLASSEXW*);
__declspec(dllimport) HWND __stdcall CreateWindowExW(DWORD,LPCWSTR,LPCWSTR,DWORD,int,int,int,int,HWND,HMENU,HINSTANCE,PVOID);
__declspec(dllimport) BOOL __stdcall ShowWindow(HWND,int);
__declspec(dllimport) BOOL __stdcall UpdateWindow(HWND);
__declspec(dllimport) int __stdcall GetMessageW(MSG*,HWND,UINT,UINT);
__declspec(dllimport) BOOL __stdcall TranslateMessage(const MSG*);
__declspec(dllimport) LRESULT __stdcall DispatchMessageW(const MSG*);
__declspec(dllimport) BOOL __stdcall IsDialogMessageW(HWND,MSG*);
__declspec(dllimport) LRESULT __stdcall DefWindowProcW(HWND,UINT,WPARAM,LPARAM);
__declspec(dllimport) VOID __stdcall PostQuitMessage(int);
__declspec(dllimport) LRESULT __stdcall SendMessageW(HWND,UINT,WPARAM,LPARAM);
__declspec(dllimport) BOOL __stdcall PostMessageW(HWND,UINT,WPARAM,LPARAM);
__declspec(dllimport) BOOL __stdcall SetWindowTextW(HWND,LPCWSTR);
__declspec(dllimport) int __stdcall GetWindowTextW(HWND,LPWSTR,int);
__declspec(dllimport) int __stdcall GetWindowTextLengthW(HWND);
__declspec(dllimport) int __stdcall MessageBoxW(HWND,LPCWSTR,LPCWSTR,UINT);
__declspec(dllimport) BOOL __stdcall EnableWindow(HWND,BOOL);
__declspec(dllimport) BOOL __stdcall MoveWindow(HWND,int,int,int,int,BOOL);
__declspec(dllimport) BOOL __stdcall GetWindowRect(HWND,RECT*);
__declspec(dllimport) BOOL __stdcall SetWindowPos(HWND,HWND,int,int,int,int,UINT);
__declspec(dllimport) HCURSOR __stdcall LoadCursorW(HINSTANCE,LPCWSTR);
__declspec(dllimport) HICON __stdcall LoadIconW(HINSTANCE,LPCWSTR);
__declspec(dllimport) LRESULT __stdcall SendMessageTimeoutW(HWND,UINT,WPARAM,LPARAM,UINT,UINT,ULONG_PTR*);
__declspec(dllimport) HGDIOBJ __stdcall GetStockObject(int);
__declspec(dllimport) LPITEMIDLIST __stdcall SHBrowseForFolderW(BROWSEINFOW*);
__declspec(dllimport) BOOL __stdcall SHGetPathFromIDListW(LPITEMIDLIST,LPWSTR);
__declspec(dllimport) HANDLE __stdcall ShellExecuteW(HWND,LPCWSTR,LPCWSTR,LPCWSTR,LPCWSTR,int);
__declspec(dllimport) HRESULT __stdcall SetCurrentProcessExplicitAppUserModelID(LPCWSTR);
__declspec(dllimport) HRESULT __stdcall SHGetFolderPathW(HWND,int,HANDLE,DWORD,LPWSTR);
__declspec(dllimport) int __stdcall SHCreateDirectoryExW(HWND,LPCWSTR,SECURITY_ATTRIBUTES*);
__declspec(dllimport) VOID __stdcall CoTaskMemFree(PVOID);
__declspec(dllimport) LONG __stdcall CoInitializeEx(PVOID,DWORD);
__declspec(dllimport) HRESULT __stdcall CoCreateInstance(REFCLSID,PVOID,DWORD,REFIID,PVOID*);
__declspec(dllimport) VOID __stdcall CoUninitialize(VOID);
__declspec(dllimport) LONG __stdcall RegCreateKeyExW(HKEY,LPCWSTR,DWORD,LPWSTR,DWORD,DWORD,PVOID,HKEY*,DWORD*);
__declspec(dllimport) LONG __stdcall RegOpenKeyExW(HKEY,LPCWSTR,DWORD,DWORD,HKEY*);
__declspec(dllimport) LONG __stdcall RegQueryValueExW(HKEY,LPCWSTR,DWORD*,DWORD*,BYTE*,DWORD*);
__declspec(dllimport) LONG __stdcall RegSetValueExW(HKEY,LPCWSTR,DWORD,DWORD,const BYTE*,DWORD);
__declspec(dllimport) LONG __stdcall RegCloseKey(HKEY);
__declspec(dllimport) BOOL __stdcall CredReadW(LPCWSTR,DWORD,DWORD,CREDENTIALW_CCM**);
__declspec(dllimport) BOOL __stdcall CredWriteW(CREDENTIALW_CCM*,DWORD);
__declspec(dllimport) VOID __stdcall CredFree(PVOID);
__declspec(dllimport) HINTERNET __stdcall WinHttpOpen(LPCWSTR,DWORD,LPCWSTR,LPCWSTR,DWORD);
__declspec(dllimport) HINTERNET __stdcall WinHttpConnect(HINTERNET,LPCWSTR,INTERNET_PORT,DWORD);
__declspec(dllimport) HINTERNET __stdcall WinHttpOpenRequest(HINTERNET,LPCWSTR,LPCWSTR,LPCWSTR,LPCWSTR,LPCWSTR*,DWORD);
__declspec(dllimport) BOOL __stdcall WinHttpAddRequestHeaders(HINTERNET,LPCWSTR,DWORD,DWORD);
__declspec(dllimport) BOOL __stdcall WinHttpSendRequest(HINTERNET,LPCWSTR,DWORD,PVOID,DWORD,DWORD,ULONG_PTR);
__declspec(dllimport) BOOL __stdcall WinHttpReceiveResponse(HINTERNET,PVOID);
__declspec(dllimport) BOOL __stdcall WinHttpQueryHeaders(HINTERNET,DWORD,LPCWSTR,PVOID,DWORD*,DWORD*);
__declspec(dllimport) BOOL __stdcall WinHttpQueryDataAvailable(HINTERNET,DWORD*);
__declspec(dllimport) BOOL __stdcall WinHttpReadData(HINTERNET,PVOID,DWORD,DWORD*);
__declspec(dllimport) BOOL __stdcall WinHttpSetTimeouts(HINTERNET,int,int,int,int);
__declspec(dllimport) BOOL __stdcall WinHttpCloseHandle(HINTERNET);
__declspec(dllimport) BOOL __stdcall WinHttpCrackUrl(LPCWSTR,DWORD,DWORD,URL_COMPONENTS_CCM*);
__declspec(dllimport) BOOL __stdcall InitCommonControlsEx(const INITCOMMONCONTROLSEX*);
__declspec(dllimport) BOOL __stdcall GetOpenFileNameW(OPENFILENAMEW*);

__declspec(dllimport) BOOL __stdcall InvalidateRect(HWND,const RECT*,BOOL);
__declspec(dllimport) HDC __stdcall BeginPaint(HWND,PAINTSTRUCT*);
__declspec(dllimport) BOOL __stdcall EndPaint(HWND,const PAINTSTRUCT*);
__declspec(dllimport) HDC __stdcall GetWindowDC(HWND);
__declspec(dllimport) int __stdcall ReleaseDC(HWND,HDC);
__declspec(dllimport) BOOL __stdcall GetClientRect(HWND,RECT*);
__declspec(dllimport) BOOL __stdcall AdjustWindowRectEx(RECT*,DWORD,BOOL,DWORD);
__declspec(dllimport) BOOL __stdcall SystemParametersInfoW(UINT,UINT,PVOID,UINT);
__declspec(dllimport) BOOL __stdcall TrackMouseEvent(TRACKMOUSEEVENT*);
__declspec(dllimport) BOOL __stdcall SetWindowSubclass(HWND,SUBCLASSPROC,UINT_PTR,DWORD_PTR);
__declspec(dllimport) LRESULT __stdcall DefSubclassProc(HWND,UINT,WPARAM,LPARAM);
__declspec(dllimport) BOOL __stdcall RemoveWindowSubclass(HWND,SUBCLASSPROC,UINT_PTR);
__declspec(dllimport) int __stdcall FillRect(HDC,const RECT*,HBRUSH);
__declspec(dllimport) UINT_PTR __stdcall SetTimer(HWND,UINT_PTR,UINT,PVOID);
__declspec(dllimport) BOOL __stdcall KillTimer(HWND,UINT_PTR);
__declspec(dllimport) HICON __stdcall LoadImageW(HINSTANCE,LPCWSTR,UINT,int,int,UINT);
__declspec(dllimport) BOOL __stdcall DrawIconEx(HDC,int,int,HICON,int,int,UINT,HBRUSH,UINT);
__declspec(dllimport) int __stdcall GetDlgCtrlID(HWND);
__declspec(dllimport) UINT __stdcall GetDpiForSystem(VOID);
__declspec(dllimport) HDESK __stdcall CreateDesktopW(LPCWSTR,LPCWSTR,PVOID,DWORD,DWORD,SECURITY_ATTRIBUTES*);
__declspec(dllimport) BOOL __stdcall CloseDesktop(HDESK);
__declspec(dllimport) DWORD __stdcall GetModuleFileNameW(HMODULE,LPWSTR,DWORD);
__declspec(dllimport) int __stdcall MulDiv(int,int,int);

__declspec(dllimport) HBRUSH __stdcall CreateSolidBrush(DWORD);
__declspec(dllimport) HPEN __stdcall CreatePen(int,int,DWORD);
__declspec(dllimport) BOOL __stdcall DeleteObject(HGDIOBJ);
__declspec(dllimport) HGDIOBJ __stdcall SelectObject(HDC,HGDIOBJ);
__declspec(dllimport) int __stdcall SetBkMode(HDC,int);
__declspec(dllimport) DWORD __stdcall SetTextColor(HDC,DWORD);
__declspec(dllimport) DWORD __stdcall SetBkColor(HDC,DWORD);
__declspec(dllimport) int __stdcall DrawTextW(HDC,LPCWSTR,int,RECT*,UINT);
__declspec(dllimport) BOOL __stdcall RoundRect(HDC,int,int,int,int,int,int);
__declspec(dllimport) BOOL __stdcall DrawFocusRect(HDC,const RECT*);
__declspec(dllimport) BOOL __stdcall Rectangle(HDC,int,int,int,int);
__declspec(dllimport) BOOL __stdcall MoveToEx(HDC,int,int,PVOID);
__declspec(dllimport) BOOL __stdcall LineTo(HDC,int,int);
__declspec(dllimport) HFONT __stdcall CreateFontW(int,int,int,int,int,DWORD,DWORD,DWORD,DWORD,DWORD,DWORD,DWORD,DWORD,LPCWSTR);
__declspec(dllimport) DWORD __stdcall SetDCBrushColor(HDC,DWORD);
__declspec(dllimport) DWORD __stdcall SetDCPenColor(HDC,DWORD);

__declspec(dllimport) LONG __stdcall DwmSetWindowAttribute(HWND,DWORD,const VOID*,DWORD);


static RegisterClassExW_t pRegisterClassExW;
static CreateWindowExW_t pCreateWindowExW;
static ShowWindow_t pShowWindow;
static UpdateWindow_t pUpdateWindow;
static GetMessageW_t pGetMessageW;
static TranslateMessage_t pTranslateMessage;
static DispatchMessageW_t pDispatchMessageW;
static DefWindowProcW_t pDefWindowProcW;
static PostQuitMessage_t pPostQuitMessage;
static SendMessageW_t pSendMessageW;
static PostMessageW_t pPostMessageW;
static SetWindowTextW_t pSetWindowTextW;
static GetWindowTextW_t pGetWindowTextW;
static GetWindowTextLengthW_t pGetWindowTextLengthW;
static MessageBoxW_t pMessageBoxW;
static EnableWindow_t pEnableWindow;
static MoveWindow_t pMoveWindow;
static LoadCursorW_t pLoadCursorW;
static LoadIconW_t pLoadIconW;
static SendMessageTimeoutW_t pSendMessageTimeoutW;
static GetStockObject_t pGetStockObject;
static SHBrowseForFolderW_t pSHBrowseForFolderW;
static SHGetPathFromIDListW_t pSHGetPathFromIDListW;
static ShellExecuteW_t pShellExecuteW;
static CoTaskMemFree_t pCoTaskMemFree;
static RegCreateKeyExW_t pRegCreateKeyExW;
static RegOpenKeyExW_t pRegOpenKeyExW;
static RegQueryValueExW_t pRegQueryValueExW;
static RegSetValueExW_t pRegSetValueExW;
static RegCloseKey_t pRegCloseKey;
static InitCommonControlsEx_t pInitCommonControlsEx;
static SetProcessDpiAwarenessContext_t pSetProcessDpiAwarenessContext;
static GetDpiForWindow_t pGetDpiForWindow;

static HINSTANCE g_instance;
static HWND g_main;
static HWND g_proxy;
static HWND g_url;
static HWND g_workspace;
static HWND g_projects;
static HWND g_install;
static HWND g_launch;
static HWND g_terminal;
static HWND g_folder;
static HWND g_refresh;
static HWND g_browse;
static HWND g_log;
static HWND g_shortcut;
static HWND g_network_mode;
static HWND g_test_network;
static HWND g_import_settings;
static HWND g_open_settings;
static HWND g_model_wizard;
static HWND g_fongap_link;
static HWND g_wizard;
static HWND g_wiz_scope;
static HWND g_wiz_provider;
static HWND g_wiz_base_url;
static HWND g_wiz_secret_type; /* legacy placeholder; no longer shown */
static HWND g_wiz_secret;
static HWND g_wiz_default_model;
static HWND g_wiz_opus_model;
static HWND g_wiz_sonnet_model;
static HWND g_wiz_haiku_model;
static HWND g_wiz_subagent_model;
static HWND g_wiz_effort;
static HWND g_wiz_recommended;
static HWND g_wiz_save;
static HWND g_wiz_cancel;
static HWND g_wiz_test_models;
static HWND g_wiz_context_capacity[MODEL_ROLE_COUNT];
static HWND g_wiz_context_budget[MODEL_ROLE_COUNT];
static HWND g_wiz_context_warning[MODEL_ROLE_COUNT];
static HWND g_wiz_context_reserve[MODEL_ROLE_COUNT];
static HWND g_wiz_context_effort[MODEL_ROLE_COUNT];
static HWND g_hover_button;
static HFONT g_font;
static HFONT g_font_title;
static HFONT g_font_subtitle;
static HFONT g_font_heading;
static HFONT g_font_body;
static HFONT g_font_button;
static HFONT g_font_mono;
static HFONT g_font_link;
static HFONT g_wiz_font_title;
static HFONT g_wiz_font_subtitle;
static HFONT g_wiz_font_body;
static HFONT g_wiz_font_button;
static HBRUSH g_br_bg;
static HBRUSH g_br_card;
static HBRUSH g_br_input;
static HBRUSH g_br_log;
static HICON g_app_icon;
static volatile LONG g_installing;
static volatile LONG g_shortcut_busy;
static volatile LONG g_model_fetch_busy;
static volatile LONG g_model_gateway_protocol_state; /* 0 unknown, 1 compatible, 2 incompatible */
static volatile LONG g_model_discovery_pending;    /* coalesce input events into a delayed probe */
static ULONG_PTR g_model_discovery_timer;          /* Win32 timer used to debounce the probe */
static WCHAR g_model_status[512];
static WCHAR g_model_fetch_base[2048];
static WCHAR g_model_fetch_secret[4096];
static WCHAR g_model_result_text[131072];
static BYTE g_http_bytes[524288];

typedef struct _CCM_JSON_TOKEN { int type; int start; int end; int size; } CCM_JSON_TOKEN;
#define CCM_JSON_UNDEFINED 0
#define CCM_JSON_OBJECT 1
#define CCM_JSON_ARRAY 2
#define CCM_JSON_STRING 3
#define CCM_JSON_PRIMITIVE 4
#define CCM_JSON_MAX_TOKENS 32768
static WCHAR g_json_source_text[262144];
static WCHAR g_json_target_text[262144];
static WCHAR g_json_output_text[524288];
static BYTE g_json_io_bytes[1048576];
static CCM_JSON_TOKEN g_json_source_tokens[CCM_JSON_MAX_TOKENS];
static CCM_JSON_TOKEN g_json_target_tokens[CCM_JSON_MAX_TOKENS];
static int g_json_source_count;
static int g_json_target_count;
static BOOL g_import_secret_error;
static WCHAR g_loaded_credential_names[4096];
static WCHAR g_models[128][256];
static DWORD g_model_context_capacity[128];
static int g_model_count;
static volatile LONG g_network_busy;
static volatile LONG g_import_busy;
static int g_dpi = 96;
static int g_main_dpi = 96;
static int g_wizard_dpi = 96;
static int g_client_w = 1000;
static int g_client_h = 720;
static int g_project_count = 0;
static int g_progress_value = 0;
static int g_progress_phase = 0;
static ULONGLONG g_install_start = 0;
static BOOL g_claude_installed = FALSE;
static WCHAR g_status_text[512];
static WCHAR g_phase_text[256];
static WCHAR g_claude_path_display[4096];
static WCHAR g_module_path[4096];
static WCHAR g_cfg_shortcut[16];
static WCHAR g_cfg_network_mode[32];
static WCHAR g_cfg_last_auto_proxy[1024];
static DWORD g_cfg_main_client_width=900;
static DWORD g_cfg_main_client_height=640;
static WCHAR g_network_status[512];
static WCHAR g_import_source[4096];
static WCHAR g_import_target[4096];
static WCHAR g_import_mode[32];
static BOOL g_import_delete_source = FALSE;
static WCHAR g_extra_secret_name[128];
static WCHAR g_extra_secret_value[4096];
static WCHAR g_extra_secret2_name[128];
static WCHAR g_extra_secret2_value[4096];
static WCHAR g_pending_active_credential_names[512];

static WCHAR g_cfg_proxy[1024];
static WCHAR g_cfg_url[2048];
static WCHAR g_cfg_workspace[4096];
static WCHAR g_claude_exe[4096];
static WCHAR g_temp_installer[4096];
static WCHAR g_effective_proxy[1024];
static WCHAR g_cmdline[32768];
static WCHAR g_path_buffer[32768];
static WCHAR g_path_buffer2[32768];
static WCHAR g_output_wide[16384];
static char g_output_pending[8196];
static int g_output_pending_len;
static WCHAR g_model_json[32768];
static WCHAR g_model_temp[4096];
static char g_output_bytes[8192];
static HANDLE g_startup_log = INVALID_HANDLE_VALUE;

static const WCHAR CLASS_NAME[] = L"ClaudeCodeManagerV1";
static const WCHAR WIZARD_CLASS_NAME[] = L"ClaudeCodeManagerModelWizardV1";
static const WCHAR REG_KEY[] = L"Software\\Fongap\\ClaudeCodeManager";
static const WCHAR CURRENT_FOLDER_ITEM[] = L"当前文件夹（Claude 默认）";
static const LPCWSTR MODEL_ROLE_LABELS[MODEL_ROLE_COUNT]={L"主力",L"高阶 · Opus",L"通用 · Sonnet",L"快速 · Haiku",L"分工 · Subagent"};
static const LPCWSTR MODEL_META_VALUE_NAMES[MODEL_ROLE_COUNT]={L"ContextMetaMain",L"ContextMetaOpus",L"ContextMetaSonnet",L"ContextMetaHaiku",L"ContextMetaSubagent"};

void *memset(void *dst, int c, unsigned long long n) {
    BYTE *p = (BYTE*)dst;
    while (n--) *p++ = (BYTE)c;
    return dst;
}

void *memcpy(void *dst, const void *src, unsigned long long n) {
    BYTE *d = (BYTE*)dst;
    const BYTE *s = (const BYTE*)src;
    while (n--) *d++ = *s++;
    return dst;
}

static unsigned int wlen(LPCWSTR s) {
    unsigned int n = 0;
    if (!s) return 0;
    while (s[n]) n++;
    return n;
}

static BOOL wblank(LPCWSTR s) {
    unsigned int i = 0;
    if (!s) return TRUE;
    while (s[i]) {
        if (s[i] != L' ' && s[i] != L'\t' && s[i] != L'\r' && s[i] != L'\n') return FALSE;
        i++;
    }
    return TRUE;
}

static void wcopy(LPWSTR dst, unsigned int cap, LPCWSTR src) {
    unsigned int i = 0;
    if (!cap) return;
    if (src) {
        while (src[i] && i + 1 < cap) { dst[i] = src[i]; i++; }
    }
    dst[i] = 0;
}

static void wcat(LPWSTR dst, unsigned int cap, LPCWSTR src) {
    unsigned int i = wlen(dst), j = 0;
    if (!cap || i >= cap) return;
    while (src && src[j] && i + 1 < cap) dst[i++] = src[j++];
    dst[i] = 0;
}

static WCHAR lower_ascii(WCHAR c);

static BOOL wstarts_ci(LPCWSTR s, LPCWSTR prefix) {
    unsigned int i=0;
    if(!s || !prefix) return FALSE;
    while(prefix[i]) { if(lower_ascii(s[i]) != lower_ascii(prefix[i])) return FALSE; i++; }
    return TRUE;
}

static BOOL weq(LPCWSTR a, LPCWSTR b) {
    unsigned int i=0;
    if(!a||!b) return FALSE;
    while(a[i]&&b[i]){if(a[i]!=b[i])return FALSE;i++;}
    return a[i]==0&&b[i]==0;
}

static BOOL weq_ci(LPCWSTR a, LPCWSTR b) {
    unsigned int i=0;
    if(!a||!b) return FALSE;
    while(a[i]&&b[i]){if(lower_ascii(a[i])!=lower_ascii(b[i]))return FALSE;i++;}
    return a[i]==0&&b[i]==0;
}

static int wfind_ci(LPCWSTR s, LPCWSTR needle) {
    unsigned int i,j,ls=wlen(s),ln=wlen(needle);
    if(!ln || ln>ls) return -1;
    for(i=0;i+ln<=ls;i++) { for(j=0;j<ln;j++) if(lower_ascii(s[i+j])!=lower_ascii(needle[j])) break; if(j==ln) return (int)i; }
    return -1;
}

static void wcopy_range(LPWSTR out, unsigned int cap, LPCWSTR s, unsigned int start, unsigned int end) {
    unsigned int i=0;
    if(!cap) return;
    while(start<end && s[start] && i+1<cap) out[i++]=s[start++];
    out[i]=0;
}

static void secure_zero_w(LPWSTR s, unsigned int cap) { volatile WCHAR *p=(volatile WCHAR*)s; while(cap--) *p++=0; }

static BOOL write_bytes_file(LPCWSTR path, const BYTE *data, DWORD len) {
    HANDLE h; DWORD wrote=0,total=0;
    h=CreateFileW(path,GENERIC_WRITE,FILE_SHARE_READ,0,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,0);
    if(h==INVALID_HANDLE_VALUE) return FALSE;
    while(total<len) {
        DWORD chunk=len-total;
        if(!WriteFile(h,data+total,chunk,&wrote,0) || !wrote) { CloseHandle(h); return FALSE; }
        total+=wrote;
    }
    CloseHandle(h); return TRUE;
}


static WCHAR lower_ascii(WCHAR c) {
    if (c >= L'A' && c <= L'Z') return (WCHAR)(c + 32);
    return c;
}

static BOOL path_equal(LPCWSTR a, LPCWSTR b) {
    unsigned int la = wlen(a), lb = wlen(b), i;
    while (la && (a[la-1] == L'\\' || a[la-1] == L'/')) la--;
    while (lb && (b[lb-1] == L'\\' || b[lb-1] == L'/')) lb--;
    if (la != lb) return FALSE;
    for (i = 0; i < la; i++) {
        WCHAR ca = a[i] == L'/' ? L'\\' : lower_ascii(a[i]);
        WCHAR cb = b[i] == L'/' ? L'\\' : lower_ascii(b[i]);
        if (ca != cb) return FALSE;
    }
    return TRUE;
}

static BOOL is_dot_dir(LPCWSTR s) {
    return (s[0] == L'.' && s[1] == 0) || (s[0] == L'.' && s[1] == L'.' && s[2] == 0);
}

static void uint_to_wstr(unsigned int value, LPWSTR out, unsigned int cap) {
    WCHAR temp[16];
    unsigned int n = 0, i;
    if (!cap) return;
    if (value == 0) { out[0] = L'0'; out[1] = 0; return; }
    while (value && n < 15) { temp[n++] = (WCHAR)(L'0' + (value % 10)); value /= 10; }
    i = 0;
    while (n && i + 1 < cap) out[i++] = temp[--n];
    out[i] = 0;
}

static void elapsed_text(ULONGLONG ms, LPWSTR out, unsigned int cap) {
    unsigned int total = (unsigned int)(ms / 1000);
    unsigned int h = total / 3600;
    unsigned int m = (total % 3600) / 60;
    unsigned int s = total % 60;
    WCHAR t[16];
    out[0] = 0;
    if (h < 10) wcat(out, cap, L"0"); uint_to_wstr(h, t, 16); wcat(out, cap, t); wcat(out, cap, L":");
    if (m < 10) wcat(out, cap, L"0"); uint_to_wstr(m, t, 16); wcat(out, cap, t); wcat(out, cap, L":");
    if (s < 10) wcat(out, cap, L"0"); uint_to_wstr(s, t, 16); wcat(out, cap, t);
}


static void startup_log_open(void) {
    WCHAR path[1024];
    DWORD n = GetTempPathW(900, path);
    if (!n || n >= 900) return;
    wcat(path, 1024, L"ClaudeCodeManager-startup.log");
    g_startup_log = CreateFileW(path, GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
}

static void startup_log_write(LPCSTR text) {
    DWORD written = 0;
    unsigned int n = 0;
    if (g_startup_log == INVALID_HANDLE_VALUE || !text) return;
    while (text[n]) n++;
    WriteFile(g_startup_log, text, n, &written, 0);
}

static void startup_log_close(void) {
    if (g_startup_log != INVALID_HANDLE_VALUE) {
        CloseHandle(g_startup_log);
        g_startup_log = INVALID_HANDLE_VALUE;
    }
}

static DWORD rgb(BYTE r, BYTE g, BYTE b) { return (DWORD)r | ((DWORD)g << 8) | ((DWORD)b << 16); }
static int sc(int v) { return MulDiv(v, g_dpi, 96); }
static RECT mkrect(int l, int t, int r, int b) { RECT x; x.left=l; x.top=t; x.right=r; x.bottom=b; return x; }

static void delete_fonts(void) {
    if(g_font_title)DeleteObject(g_font_title); g_font_title=0;
    if(g_font_subtitle)DeleteObject(g_font_subtitle); g_font_subtitle=0;
    if(g_font_heading)DeleteObject(g_font_heading); g_font_heading=0;
    if(g_font_body)DeleteObject(g_font_body); g_font_body=0;
    if(g_font_button)DeleteObject(g_font_button); g_font_button=0;
    if(g_font_mono)DeleteObject(g_font_mono); g_font_mono=0;
    if(g_font_link)DeleteObject(g_font_link); g_font_link=0;
}

static void create_fonts(void) {
    g_dpi=g_main_dpi;
    delete_fonts();
    g_font_title=CreateFontW(-sc(22),0,0,0,FW_SEMIBOLD,0,0,0,DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,CLEARTYPE_QUALITY,DEFAULT_PITCH|FF_DONTCARE,L"Microsoft YaHei UI");
    g_font_subtitle=CreateFontW(-sc(11),0,0,0,FW_NORMAL,0,0,0,DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,CLEARTYPE_QUALITY,DEFAULT_PITCH|FF_DONTCARE,L"Microsoft YaHei UI");
    g_font_heading=CreateFontW(-sc(15),0,0,0,FW_MEDIUM,0,0,0,DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,CLEARTYPE_QUALITY,DEFAULT_PITCH|FF_DONTCARE,L"Microsoft YaHei UI");
    g_font_body=CreateFontW(-sc(12),0,0,0,FW_NORMAL,0,0,0,DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,CLEARTYPE_QUALITY,DEFAULT_PITCH|FF_DONTCARE,L"Microsoft YaHei UI");
    g_font_button=CreateFontW(-sc(12),0,0,0,FW_MEDIUM,0,0,0,DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,CLEARTYPE_QUALITY,DEFAULT_PITCH|FF_DONTCARE,L"Microsoft YaHei UI");
    /* A CJK UI font keeps Chinese log lines consistent instead of falling back
       from Cascadia Mono to a visually unrelated serif font. */
    g_font_mono=CreateFontW(-sc(11),0,0,0,FW_NORMAL,0,0,0,DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,CLEARTYPE_QUALITY,DEFAULT_PITCH|FF_DONTCARE,L"Microsoft YaHei UI");
    g_font_link=CreateFontW(-sc(10),0,0,0,FW_NORMAL,0,1,0,DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,CLEARTYPE_QUALITY,DEFAULT_PITCH|FF_DONTCARE,L"Microsoft YaHei UI");
    g_font=g_font_body;
}

static void delete_wizard_fonts(void) {
    if(g_wiz_font_title)DeleteObject(g_wiz_font_title);g_wiz_font_title=0;
    if(g_wiz_font_subtitle)DeleteObject(g_wiz_font_subtitle);g_wiz_font_subtitle=0;
    if(g_wiz_font_body)DeleteObject(g_wiz_font_body);g_wiz_font_body=0;
    if(g_wiz_font_button)DeleteObject(g_wiz_font_button);g_wiz_font_button=0;
}

static void create_wizard_fonts(void) {
    g_dpi=g_wizard_dpi;delete_wizard_fonts();
    g_wiz_font_title=CreateFontW(-sc(22),0,0,0,FW_SEMIBOLD,0,0,0,DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,CLEARTYPE_QUALITY,DEFAULT_PITCH|FF_DONTCARE,L"Microsoft YaHei UI");
    g_wiz_font_subtitle=CreateFontW(-sc(11),0,0,0,FW_NORMAL,0,0,0,DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,CLEARTYPE_QUALITY,DEFAULT_PITCH|FF_DONTCARE,L"Microsoft YaHei UI");
    g_wiz_font_body=CreateFontW(-sc(12),0,0,0,FW_NORMAL,0,0,0,DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,CLEARTYPE_QUALITY,DEFAULT_PITCH|FF_DONTCARE,L"Microsoft YaHei UI");
    g_wiz_font_button=CreateFontW(-sc(12),0,0,0,FW_MEDIUM,0,0,0,DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,CLEARTYPE_QUALITY,DEFAULT_PITCH|FF_DONTCARE,L"Microsoft YaHei UI");
}

static void apply_main_control_fonts(void) {
    HWND controls[] = {g_workspace,g_browse,g_projects,g_refresh,g_launch,g_terminal,g_folder,g_network_mode,g_test_network,g_proxy,g_url,g_install,g_import_settings,g_open_settings,g_model_wizard,g_shortcut,g_log};
    unsigned int i;
    for(i=0;i<sizeof(controls)/sizeof(controls[0]);i++) if(controls[i]) pSendMessageW(controls[i],WM_SETFONT,(WPARAM)((controls[i]==g_log)?g_font_mono:g_font_body),TRUE);
    if(g_network_mode){pSendMessageW(g_network_mode,CB_SETITEMHEIGHT,(WPARAM)-1,sc(20));pSendMessageW(g_network_mode,CB_SETITEMHEIGHT,0,sc(22));}
    if(g_fongap_link) pSendMessageW(g_fongap_link,WM_SETFONT,(WPARAM)g_font_link,TRUE);
}

static void apply_wizard_control_fonts(void) {
    unsigned int i;
    { HWND wiz[] = {g_wiz_scope,g_wiz_provider,g_wiz_base_url,g_wiz_secret,g_wiz_default_model,g_wiz_opus_model,g_wiz_sonnet_model,g_wiz_haiku_model,g_wiz_subagent_model,g_wiz_effort,g_wiz_test_models,g_wiz_recommended,g_wiz_save,g_wiz_cancel};
      for(i=0;i<sizeof(wiz)/sizeof(wiz[0]);i++) if(wiz[i]) pSendMessageW(wiz[i],WM_SETFONT,(WPARAM)g_wiz_font_body,TRUE); }
    if(g_wiz_scope){pSendMessageW(g_wiz_scope,CB_SETITEMHEIGHT,(WPARAM)-1,sc(24));pSendMessageW(g_wiz_scope,CB_SETITEMHEIGHT,0,sc(24));}
    if(g_wiz_provider){pSendMessageW(g_wiz_provider,CB_SETITEMHEIGHT,(WPARAM)-1,sc(24));pSendMessageW(g_wiz_provider,CB_SETITEMHEIGHT,0,sc(24));}
    for(i=0;i<MODEL_ROLE_COUNT;i++) {
        if(g_wiz_context_capacity[i])pSendMessageW(g_wiz_context_capacity[i],WM_SETFONT,(WPARAM)g_wiz_font_body,TRUE);
        if(g_wiz_context_budget[i])pSendMessageW(g_wiz_context_budget[i],WM_SETFONT,(WPARAM)g_wiz_font_body,TRUE);
        if(g_wiz_context_warning[i])pSendMessageW(g_wiz_context_warning[i],WM_SETFONT,(WPARAM)g_wiz_font_body,TRUE);
        if(g_wiz_context_reserve[i])pSendMessageW(g_wiz_context_reserve[i],WM_SETFONT,(WPARAM)g_wiz_font_body,TRUE);
        if(g_wiz_context_effort[i])pSendMessageW(g_wiz_context_effort[i],WM_SETFONT,(WPARAM)g_wiz_font_body,TRUE);
        {HWND models[]={g_wiz_default_model,g_wiz_opus_model,g_wiz_sonnet_model,g_wiz_haiku_model,g_wiz_subagent_model};if(models[i]){pSendMessageW(models[i],CB_SETITEMHEIGHT,(WPARAM)-1,sc(22));pSendMessageW(models[i],CB_SETITEMHEIGHT,0,sc(22));}}
        if(g_wiz_context_effort[i]){pSendMessageW(g_wiz_context_effort[i],CB_SETITEMHEIGHT,(WPARAM)-1,sc(22));pSendMessageW(g_wiz_context_effort[i],CB_SETITEMHEIGHT,0,sc(22));}
    }
}

static void apply_main_edit_margins(void) {
    int margin;LPARAM packed;g_dpi=g_main_dpi;margin=sc(6);packed=(LPARAM)((margin&0xFFFF)|((margin&0xFFFF)<<16));
    if(g_workspace)pSendMessageW(g_workspace,EM_SETMARGINS,EC_LEFTMARGIN|EC_RIGHTMARGIN,packed);
    if(g_proxy)pSendMessageW(g_proxy,EM_SETMARGINS,EC_LEFTMARGIN|EC_RIGHTMARGIN,packed);
    if(g_url)pSendMessageW(g_url,EM_SETMARGINS,EC_LEFTMARGIN|EC_RIGHTMARGIN,packed);
}

static void apply_wizard_edit_margins(void) {
    int i,margin;LPARAM packed;g_dpi=g_wizard_dpi;margin=sc(6);packed=(LPARAM)((margin&0xFFFF)|((margin&0xFFFF)<<16));
    if(g_wiz_base_url)pSendMessageW(g_wiz_base_url,EM_SETMARGINS,EC_LEFTMARGIN|EC_RIGHTMARGIN,packed);
    if(g_wiz_secret)pSendMessageW(g_wiz_secret,EM_SETMARGINS,EC_LEFTMARGIN|EC_RIGHTMARGIN,packed);
    for(i=0;i<MODEL_ROLE_COUNT;i++){
        if(g_wiz_context_capacity[i])pSendMessageW(g_wiz_context_capacity[i],EM_SETMARGINS,EC_LEFTMARGIN|EC_RIGHTMARGIN,packed);
        if(g_wiz_context_budget[i])pSendMessageW(g_wiz_context_budget[i],EM_SETMARGINS,EC_LEFTMARGIN|EC_RIGHTMARGIN,packed);
        if(g_wiz_context_warning[i])pSendMessageW(g_wiz_context_warning[i],EM_SETMARGINS,EC_LEFTMARGIN|EC_RIGHTMARGIN,packed);
        if(g_wiz_context_reserve[i])pSendMessageW(g_wiz_context_reserve[i],EM_SETMARGINS,EC_LEFTMARGIN|EC_RIGHTMARGIN,packed);
    }
}

static LRESULT __stdcall button_subclass_proc(HWND hwnd,UINT msg,WPARAM wp,LPARAM lp,UINT_PTR id,DWORD_PTR data) {
    (void)id;(void)data;
    if(msg==WM_MOUSEMOVE){
        TRACKMOUSEEVENT tme;
        if(g_hover_button!=hwnd){if(g_hover_button)InvalidateRect(g_hover_button,0,FALSE);g_hover_button=hwnd;InvalidateRect(hwnd,0,FALSE);}
        tme.cbSize=sizeof(tme);tme.dwFlags=TME_LEAVE;tme.hwndTrack=hwnd;tme.dwHoverTime=0;TrackMouseEvent(&tme);
    }else if(msg==WM_MOUSELEAVE){
        if(g_hover_button==hwnd)g_hover_button=0;InvalidateRect(hwnd,0,FALSE);
    }else if(msg==WM_NCDESTROY){
        if(g_hover_button==hwnd)g_hover_button=0;RemoveWindowSubclass(hwnd,button_subclass_proc,1);
    }
    return DefSubclassProc(hwnd,msg,wp,lp);
}

static void fill_round(HDC dc, RECT r, int radius, DWORD fill, DWORD border) {
    HBRUSH br = CreateSolidBrush(fill);
    HPEN pen = CreatePen(PS_SOLID, 1, border);
    HGDIOBJ oldb = SelectObject(dc, br);
    HGDIOBJ oldp = SelectObject(dc, pen);
    RoundRect(dc, r.left, r.top, r.right, r.bottom, radius, radius);
    SelectObject(dc, oldp); SelectObject(dc, oldb);
    DeleteObject(pen); DeleteObject(br);
}

static void erase_combo_frame(HWND combo,HDC dc) {
    RECT wr,edge,arrow;int width,height,thickness,arrow_w,cx,cy,chevron;HPEN pen;HGDIOBJ oldpen;
    if(!combo||!dc||!GetWindowRect(combo,&wr)||!g_br_input)return;
    width=(int)(wr.right-wr.left);height=(int)(wr.bottom-wr.top);thickness=MulDiv(1,g_wizard_dpi,96);if(thickness<1)thickness=1;
    edge=mkrect(0,0,width,thickness);FillRect(dc,&edge,g_br_input);
    edge=mkrect(0,height-thickness,width,height);FillRect(dc,&edge,g_br_input);
    edge=mkrect(0,0,thickness,height);FillRect(dc,&edge,g_br_input);
    edge=mkrect(width-thickness,0,width,height);FillRect(dc,&edge,g_br_input);
    arrow_w=MulDiv(20,g_wizard_dpi,96);if(arrow_w<12)arrow_w=12;
    arrow=mkrect(width-arrow_w,thickness,width-thickness,height-thickness);FillRect(dc,&arrow,g_br_input);
    cx=width-arrow_w/2;cy=height/2;chevron=MulDiv(3,g_wizard_dpi,96);if(chevron<2)chevron=2;
    pen=CreatePen(PS_SOLID,1,rgb(100,100,105));oldpen=SelectObject(dc,pen);
    MoveToEx(dc,cx-chevron,cy-chevron/2,0);LineTo(dc,cx,cy+chevron/2);LineTo(dc,cx+chevron,cy-chevron/2);
    SelectObject(dc,oldpen);DeleteObject(pen);
}

static LRESULT __stdcall combo_subclass_proc(HWND hwnd,UINT msg,WPARAM wp,LPARAM lp,UINT_PTR id,DWORD_PTR data) {
    LRESULT result;(void)id;(void)data;
    if(msg==WM_NCDESTROY){RemoveWindowSubclass(hwnd,combo_subclass_proc,2);return DefSubclassProc(hwnd,msg,wp,lp);}
    result=DefSubclassProc(hwnd,msg,wp,lp);
    if(msg==WM_PRINT||msg==WM_PRINTCLIENT)erase_combo_frame(hwnd,(HDC)wp);
    else if(msg==WM_PAINT||msg==WM_NCPAINT){HDC dc=GetWindowDC(hwnd);if(dc){erase_combo_frame(hwnd,dc);ReleaseDC(hwnd,dc);}}
    return result;
}

static void text_out(HDC dc, LPCWSTR text, RECT r, HFONT font, DWORD color, UINT flags) {
    HGDIOBJ old = SelectObject(dc, font ? font : g_font_body);
    SetBkMode(dc, TRANSPARENT);
    SetTextColor(dc, color);
    DrawTextW(dc, text, -1, &r, flags | DT_NOPREFIX);
    SelectObject(dc, old);
}

static void set_phase(int phase, int value, LPCWSTR text) {
    g_progress_phase = phase;
    g_progress_value = value;
    if (text) wcopy(g_phase_text, 256, text); else g_phase_text[0] = 0;
    if (g_main) InvalidateRect(g_main, 0, FALSE);
}

static WCHAR lower_ascii(WCHAR c);
static BOOL reg_read_string(HKEY key, LPCWSTR name, LPWSTR out, DWORD cap_chars);
static BOOL reg_read_dword(HKEY key,LPCWSTR name,DWORD *value);
static BOOL reg_read_dword(HKEY key, LPCWSTR name, DWORD *value);
static int network_mode_index(void);
static void update_claude_state(void);
static BOOL create_desktop_shortcut(BOOL notify);
static DWORD run_process_capture(LPWSTR command, LPCWSTR workdir, BOOL heartbeat, LPCWSTR heartbeat_name);
static DWORD __stdcall install_thread(PVOID unused);
static void set_proxy_environment(LPCWSTR proxy);
static int model_add_unique(LPCWSTR id,DWORD context_capacity);
static HWND model_combo_for_role(int role);
static void populate_model_combos(void);
static void append_context_budget_summary(void);
static LRESULT __stdcall wizard_wndproc(HWND,UINT,WPARAM,LPARAM);
static void show_model_wizard(void);
static void draw_button(const DRAWITEMSTRUCT *di);

static BOOL contains_quote(LPCWSTR s) {
    while (s && *s) { if (*s == L'"') return TRUE; s++; }
    return FALSE;
}

static void append_log_raw(LPCWSTR text) {
    if (!g_log || !text) return;
    pSendMessageW(g_log, EM_SETSEL, (WPARAM)-1, (LPARAM)-1);
    pSendMessageW(g_log, EM_REPLACESEL, FALSE, (LPARAM)text);
    pSendMessageW(g_log, EM_SCROLLCARET, 0, 0);
}

static void append_log(LPCWSTR text) {
    append_log_raw(text);
    append_log_raw(L"\r\n");
}

static void set_status(LPCWSTR text) {
    wcopy(g_status_text, 512, text ? text : L"");
    if (g_main) InvalidateRect(g_main, 0, FALSE);
}

static void mask_proxy(LPCWSTR src, LPWSTR out, unsigned int cap) {
    int scheme,at,colon; unsigned int i;
    wcopy(out,cap,src?src:L"");
    scheme=wfind_ci(src,L"://"); at=wfind_ci(src,L"@"); colon=-1;
    if(scheme>=0 && at>scheme+3) { for(i=(unsigned int)scheme+3;i<(unsigned int)at;i++) if(src[i]==L':') colon=(int)i; }
    if(colon>0 && at>colon) { out[0]=0; wcopy_range(out,cap,src,0,(unsigned int)colon+1); wcat(out,cap,L"******"); wcat(out,cap,src+at); }
}

static BOOL get_system_proxy(LPWSTR out, unsigned int cap) {
    HKEY key=0; DWORD enabled=0; WCHAR raw[2048]; int pos; unsigned int start,end;
    out[0]=0; raw[0]=0;
    if(pRegOpenKeyExW(HKEY_CURRENT_USER,L"Software\\Microsoft\\Windows\\CurrentVersion\\Internet Settings",0,KEY_READ,&key)!=ERROR_SUCCESS) return FALSE;
    if(!reg_read_dword(key,L"ProxyEnable",&enabled) || !enabled || !reg_read_string(key,L"ProxyServer",raw,2048)) { pRegCloseKey(key); return FALSE; }
    pRegCloseKey(key);
    pos=wfind_ci(raw,L"https="); if(pos<0) pos=wfind_ci(raw,L"http=");
    if(pos>=0) { start=(unsigned int)pos; while(raw[start] && raw[start]!=L'=') start++; if(raw[start]==L'=') start++; end=start; while(raw[end] && raw[end]!=L';') end++; wcopy_range(out,cap,raw,start,end); }
    else wcopy(out,cap,raw);
    if(!out[0]) return FALSE;
    if(wfind_ci(out,L"://")<0) { wcopy(raw,2048,out); wcopy(out,cap,L"http://"); wcat(out,cap,raw); }
    return TRUE;
}

static void update_network_controls(void) {
    int mode=network_mode_index();
    if(g_proxy) pEnableWindow(g_proxy, (!g_installing && mode==3));
    if(g_main) InvalidateRect(g_main,0,FALSE);
}

static void resolve_effective_proxy(LPWSTR out, unsigned int cap, LPWSTR desc, unsigned int dcap) {
    int mode=network_mode_index(); WCHAR sys[1024];
    out[0]=0; sys[0]=0;
    if(mode==2) { wcopy(desc,dcap,L"直连"); return; }
    if(mode==1) { if(get_system_proxy(out,cap)) wcopy(desc,dcap,L"跟随 Windows 系统代理"); else wcopy(desc,dcap,L"系统未启用代理，使用直连"); return; }
    if(mode==3) { wcopy(out,cap,g_cfg_proxy); wcopy(desc,dcap,out[0]?L"自定义代理":L"自定义代理为空，使用直连"); return; }
    if(get_system_proxy(out,cap)) { wcopy(desc,dcap,L"自动检测：Windows 系统代理"); return; }
    if(g_cfg_last_auto_proxy[0]) { wcopy(out,cap,g_cfg_last_auto_proxy); wcopy(desc,dcap,L"自动检测：上次可用代理"); return; }
    if(g_cfg_proxy[0]) { wcopy(out,cap,g_cfg_proxy); wcopy(desc,dcap,L"自动检测：自定义候选代理"); return; }
    wcopy(desc,dcap,L"自动检测：未发现代理，使用直连");
}

static void set_busy(BOOL busy) {
    g_installing = busy ? 1 : 0;
    pEnableWindow(g_install, !busy);
    pEnableWindow(g_launch, !busy);
    pEnableWindow(g_terminal, !busy);
    pEnableWindow(g_folder, !busy);
    pEnableWindow(g_refresh, !busy);
    pEnableWindow(g_browse, !busy);
    pEnableWindow(g_proxy, !busy);
    pEnableWindow(g_url, !busy);
    pEnableWindow(g_workspace, !busy);
    pEnableWindow(g_projects, !busy);
    if(g_network_mode) pEnableWindow(g_network_mode,!busy);
    if(g_test_network) pEnableWindow(g_test_network,!busy);
    if(g_import_settings) pEnableWindow(g_import_settings,!busy);
    if(g_open_settings) pEnableWindow(g_open_settings,!busy);
    if(g_model_wizard) pEnableWindow(g_model_wizard,!busy);
    if(g_shortcut) pEnableWindow(g_shortcut,!busy);
    if(!busy) update_network_controls();
    if (busy) {
        g_install_start = GetTickCount64();
        if(g_main) SetTimer(g_main,1,1000,0);
    } else if(g_main) KillTimer(g_main,1);
    InvalidateRect(g_main, 0, FALSE);
}

static BOOL load_apis(void) {
    pRegisterClassExW = RegisterClassExW;
    pCreateWindowExW = CreateWindowExW;
    pShowWindow = ShowWindow;
    pUpdateWindow = UpdateWindow;
    pGetMessageW = GetMessageW;
    pTranslateMessage = TranslateMessage;
    pDispatchMessageW = DispatchMessageW;
    pDefWindowProcW = DefWindowProcW;
    pPostQuitMessage = PostQuitMessage;
    pSendMessageW = SendMessageW;
    pPostMessageW = PostMessageW;
    pSetWindowTextW = SetWindowTextW;
    pGetWindowTextW = GetWindowTextW;
    pGetWindowTextLengthW = GetWindowTextLengthW;
    pMessageBoxW = MessageBoxW;
    pEnableWindow = EnableWindow;
    pMoveWindow = MoveWindow;
    pLoadCursorW = LoadCursorW;
    pLoadIconW = LoadIconW;
    pSendMessageTimeoutW = SendMessageTimeoutW;
    pGetStockObject = GetStockObject;
    pSHBrowseForFolderW = SHBrowseForFolderW;
    pSHGetPathFromIDListW = SHGetPathFromIDListW;
    pShellExecuteW = ShellExecuteW;
    pCoTaskMemFree = CoTaskMemFree;
    pRegCreateKeyExW = RegCreateKeyExW;
    pRegOpenKeyExW = RegOpenKeyExW;
    pRegQueryValueExW = RegQueryValueExW;
    pRegSetValueExW = RegSetValueExW;
    pRegCloseKey = RegCloseKey;
    pInitCommonControlsEx = InitCommonControlsEx;
    {
        HMODULE user32 = GetModuleHandleW(L"user32.dll");
        if (user32) {
            pSetProcessDpiAwarenessContext = (SetProcessDpiAwarenessContext_t)GetProcAddress(user32, "SetProcessDpiAwarenessContext");
            pGetDpiForWindow = (GetDpiForWindow_t)GetProcAddress(user32, "GetDpiForWindow");
        }
    }
    return TRUE;
}

static void set_default_config(void) {
    wcopy(g_cfg_proxy, 1024, L"http://127.0.0.1:7890");
    wcopy(g_cfg_url, 2048, L"https://claude.ai/install.cmd");
    g_cfg_workspace[0] = 0;
    if (!GetCurrentDirectoryW(4096, g_cfg_workspace) || !g_cfg_workspace[0]) {
        WCHAR profile[4096]; profile[0]=0;
        GetEnvironmentVariableW(L"USERPROFILE", profile, 4096);
        wcopy(g_cfg_workspace, 4096, profile);
    }
    wcopy(g_cfg_network_mode, 32, L"Auto");
    g_cfg_last_auto_proxy[0] = 0;
    wcopy(g_network_status, 512, L"自动检测：系统代理 → 自定义代理 → 直连");
    g_cfg_shortcut[0] = 0;
    g_cfg_main_client_width=900;
    g_cfg_main_client_height=640;
}

static BOOL reg_read_string(HKEY key, LPCWSTR name, LPWSTR out, DWORD cap_chars) {
    DWORD type = 0;
    DWORD bytes = cap_chars * 2;
    if (pRegQueryValueExW(key, name, 0, &type, (BYTE*)out, &bytes) != ERROR_SUCCESS || type != REG_SZ) return FALSE;
    out[cap_chars - 1] = 0;
    return TRUE;
}

static void load_config(void) {
    HKEY key = 0;
    set_default_config();
    if (pRegOpenKeyExW(HKEY_CURRENT_USER, REG_KEY, 0, KEY_READ, &key) == ERROR_SUCCESS) {
        reg_read_string(key, L"Proxy", g_cfg_proxy, 1024);
        reg_read_string(key, L"InstallUrl", g_cfg_url, 2048);
        reg_read_string(key, L"WorkspaceRoot", g_cfg_workspace, 4096);
        reg_read_string(key, L"DesktopShortcutCCM1", g_cfg_shortcut, 16);
        reg_read_string(key, L"NetworkMode", g_cfg_network_mode, 32);
        reg_read_string(key, L"LastAutoProxy", g_cfg_last_auto_proxy, 1024);
        reg_read_dword(key,L"MainClientWidth",&g_cfg_main_client_width);
        reg_read_dword(key,L"MainClientHeight",&g_cfg_main_client_height);
        pRegCloseKey(key);
    }
    /* An older build could persist an empty installer URL. Restore the official
       default so first-time and upgraded users always see a usable value. */
    if (wblank(g_cfg_url)) wcopy(g_cfg_url, 2048, L"https://claude.ai/install.cmd");
    if(g_cfg_main_client_width<720||g_cfg_main_client_width>2400)g_cfg_main_client_width=900;
    if(g_cfg_main_client_height<520||g_cfg_main_client_height>1600)g_cfg_main_client_height=640;
}

static void reg_write_string(HKEY key, LPCWSTR name, LPCWSTR value) {
    DWORD bytes = (wlen(value) + 1) * 2;
    pRegSetValueExW(key, name, 0, REG_SZ, (const BYTE*)value, bytes);
}

static int network_mode_index(void) {
    LRESULT idx;
    if(g_network_mode) { idx=pSendMessageW(g_network_mode,CB_GETCURSEL,0,0); if(idx>=0 && idx<=3) return (int)idx; }
    if(wstarts_ci(g_cfg_network_mode,L"System")) return 1;
    if(wstarts_ci(g_cfg_network_mode,L"Direct")) return 2;
    if(wstarts_ci(g_cfg_network_mode,L"Custom")) return 3;
    return 0;
}

static void network_mode_name(int idx, LPWSTR out, unsigned int cap) {
    if(idx==1) wcopy(out,cap,L"System"); else if(idx==2) wcopy(out,cap,L"Direct"); else if(idx==3) wcopy(out,cap,L"Custom"); else wcopy(out,cap,L"Auto");
}

static BOOL reg_read_dword(HKEY key, LPCWSTR name, DWORD *value) {
    DWORD type=0, bytes=sizeof(DWORD);
    return pRegQueryValueExW(key,name,0,&type,(BYTE*)value,&bytes)==ERROR_SUCCESS && type==REG_DWORD;
}

static void save_config(void) {
    HKEY key = 0;
    DWORD disp = 0;
    if (g_proxy) pGetWindowTextW(g_proxy, g_cfg_proxy, 1024);
    if (g_url) pGetWindowTextW(g_url, g_cfg_url, 2048);
    if (wblank(g_cfg_url)) {
        wcopy(g_cfg_url, 2048, L"https://claude.ai/install.cmd");
        if (g_url) pSetWindowTextW(g_url, g_cfg_url);
    }
    if (g_workspace) pGetWindowTextW(g_workspace, g_cfg_workspace, 4096);
    network_mode_name(network_mode_index(), g_cfg_network_mode, 32);
    if (pRegCreateKeyExW(HKEY_CURRENT_USER, REG_KEY, 0, 0, 0, KEY_WRITE, 0, &key, &disp) == ERROR_SUCCESS) {
        reg_write_string(key, L"Proxy", g_cfg_proxy);
        reg_write_string(key, L"InstallUrl", g_cfg_url);
        reg_write_string(key, L"WorkspaceRoot", g_cfg_workspace);
        reg_write_string(key, L"DesktopShortcutCCM1", g_cfg_shortcut);
        reg_write_string(key, L"NetworkMode", g_cfg_network_mode);
        reg_write_string(key, L"LastAutoProxy", g_cfg_last_auto_proxy);
        pRegSetValueExW(key,L"MainClientWidth",0,REG_DWORD,(const BYTE*)&g_cfg_main_client_width,sizeof(DWORD));
        pRegSetValueExW(key,L"MainClientHeight",0,REG_DWORD,(const BYTE*)&g_cfg_main_client_height,sizeof(DWORD));
        pRegCloseKey(key);
    }
}

static HWND create_control(DWORD ex, LPCWSTR cls, LPCWSTR text, DWORD style, int x, int y, int w, int h, int id) {
    HWND hwnd = pCreateWindowExW(ex, cls, text, style | WS_CHILD | WS_VISIBLE,
                                 x,y,w,h,g_main,(HMENU)(ULONG_PTR)id,g_instance,0);
    if (hwnd && (g_font_body || g_font)) pSendMessageW(hwnd, WM_SETFONT, (WPARAM)(g_font_body ? g_font_body : g_font), TRUE);
    if(hwnd&&weq(cls,L"BUTTON")&&((style&0x0F)==BS_OWNERDRAW))SetWindowSubclass(hwnd,button_subclass_proc,1,0);
    return hwnd;
}

static HWND create_child_control(HWND parent, DWORD ex, LPCWSTR cls, LPCWSTR text, DWORD style, int x, int y, int w, int h, int id) {
    HWND hwnd = pCreateWindowExW(ex, cls, text, style | WS_CHILD | WS_VISIBLE,
                                 x,y,w,h,parent,(HMENU)(ULONG_PTR)id,g_instance,0);
    if(hwnd){HFONT font=(parent==g_wizard&&g_wiz_font_body)?g_wiz_font_body:(g_font_body?g_font_body:g_font);if(font)pSendMessageW(hwnd,WM_SETFONT,(WPARAM)font,TRUE);}
    if(hwnd&&weq(cls,L"BUTTON")&&((style&0x0F)==BS_OWNERDRAW))SetWindowSubclass(hwnd,button_subclass_proc,1,0);
    if(hwnd&&parent==g_wizard&&weq(cls,L"COMBOBOX"))SetWindowSubclass(hwnd,combo_subclass_proc,2,0);
    return hwnd;
}

static void refresh_projects(void) {
    WIN32_FIND_DATAW fd;
    HANDLE h;
    WCHAR pattern[4096];
    DWORD attrs;
    int count = 0;
    g_project_count = 0;
    pGetWindowTextW(g_workspace, g_cfg_workspace, 4096);
    pSendMessageW(g_projects, LB_RESETCONTENT, 0, 0);
    attrs = GetFileAttributesW(g_cfg_workspace);
    if (attrs == INVALID_FILE_ATTRIBUTES || !(attrs & FILE_ATTRIBUTE_DIRECTORY)) {
        set_status(L"\u5de5\u4f5c\u533a\u76ee\u5f55\u4e0d\u5b58\u5728");
        return;
    }
    pSendMessageW(g_projects, LB_ADDSTRING, 0, (LPARAM)CURRENT_FOLDER_ITEM);
    count = 1;
    wcopy(pattern, 4096, g_cfg_workspace);
    if (wlen(pattern) && pattern[wlen(pattern)-1] != L'\\') wcat(pattern, 4096, L"\\");
    wcat(pattern, 4096, L"*");
    memset(&fd, 0, sizeof(fd));
    h = FindFirstFileW(pattern, &fd);
    if (h == INVALID_HANDLE_VALUE) {
        g_project_count = 1;
        pSendMessageW(g_projects, LB_SETCURSEL, 0, 0);
        set_status(L"已使用当前文件夹作为 Claude 默认项目");
        InvalidateRect(g_main,0,FALSE);
        return;
    }
    do {
        if ((fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) && !is_dot_dir(fd.cFileName) && !(fd.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN)) {
            pSendMessageW(g_projects, LB_ADDSTRING, 0, (LPARAM)fd.cFileName);
            count++;
        }
    } while (FindNextFileW(h, &fd));
    FindClose(h);
    g_project_count = count;
    InvalidateRect(g_main, 0, FALSE);
    pSendMessageW(g_projects, LB_SETCURSEL, 0, 0);
    if (count > 1) set_status(L"项目列表已刷新；当前文件夹可直接启动 Claude");
    else set_status(L"已使用当前文件夹作为 Claude 默认项目");
}

static BOOL get_selected_project(LPWSTR out, unsigned int cap) {
    LRESULT idx = pSendMessageW(g_projects, LB_GETCURSEL, 0, 0);
    WCHAR name[1024];
    if (idx < 0) {
        pMessageBoxW(g_main, L"\u8bf7\u5148\u9009\u62e9\u4e00\u4e2a\u9879\u76ee\u3002", L"ClaudeCodeManager", 0x30);
        return FALSE;
    }
    name[0] = 0;
    pSendMessageW(g_projects, LB_GETTEXT, (WPARAM)idx, (LPARAM)name);
    pGetWindowTextW(g_workspace, g_cfg_workspace, 4096);
    wcopy(out, cap, g_cfg_workspace);
    if(weq(name,CURRENT_FOLDER_ITEM)) return TRUE;
    if (wlen(out) && out[wlen(out)-1] != L'\\') wcat(out, cap, L"\\");
    wcat(out, cap, name);
    return TRUE;
}

static BOOL locate_claude(LPWSTR out, unsigned int cap) {
    WCHAR profile[4096];
    DWORD attrs;
    DWORD n = GetEnvironmentVariableW(L"USERPROFILE", profile, 4096);
    if (n && n < 4096) {
        wcopy(out, cap, profile);
        wcat(out, cap, L"\\.local\\bin\\claude.exe");
        attrs=GetFileAttributesW(out);
        if (attrs != INVALID_FILE_ATTRIBUTES && !(attrs & FILE_ATTRIBUTE_DIRECTORY)) return TRUE;
    }
    if (SearchPathW(0, L"claude.exe", 0, cap, out, 0)) {
        attrs=GetFileAttributesW(out);
        if(attrs!=INVALID_FILE_ATTRIBUTES && !(attrs&FILE_ATTRIBUTE_DIRECTORY))return TRUE;
    }
    out[0] = 0;
    return FALSE;
}


static void update_claude_state(void) {
    WCHAR profile[4096];
    if (locate_claude(g_claude_path_display, 4096)) {
        g_claude_installed = TRUE;
    } else {
        g_claude_installed = FALSE;
        profile[0] = 0;
        GetEnvironmentVariableW(L"USERPROFILE", profile, 4096);
        wcopy(g_claude_path_display, 4096, profile);
        wcat(g_claude_path_display, 4096, L"\\.local\\bin\\claude.exe");
    }
    if (g_install) pSetWindowTextW(g_install, g_claude_installed ? L"检查并更新" : L"安装 Claude Code");
    if (g_main) InvalidateRect(g_main, 0, FALSE);
}

static const GUID CLSID_ShellLink_CCM = {0x00021401,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}};
static const GUID IID_IShellLinkW_CCM = {0x000214F9,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}};
static const GUID IID_IPersistFile_CCM = {0x0000010B,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}};

static BOOL desktop_path_resolve(LPWSTR out, unsigned int cap) {
    WCHAR profile[4096], onedrive[4096], candidate[4096];
    HRESULT hr;
    out[0]=0;
    hr=SHGetFolderPathW(0,CSIDL_DESKTOPDIRECTORY,0,SHGFP_TYPE_CURRENT,out);
    if(hr>=0 && out[0] && GetFileAttributesW(out)!=INVALID_FILE_ATTRIBUTES) return TRUE;
    profile[0]=0; GetEnvironmentVariableW(L"USERPROFILE",profile,4096);
    if(profile[0]) {
        wcopy(candidate,4096,profile); if(candidate[wlen(candidate)-1]!=L'\\') wcat(candidate,4096,L"\\"); wcat(candidate,4096,L"Desktop");
        if(GetFileAttributesW(candidate)!=INVALID_FILE_ATTRIBUTES){wcopy(out,cap,candidate);return TRUE;}
    }
    onedrive[0]=0; GetEnvironmentVariableW(L"OneDrive",onedrive,4096);
    if(onedrive[0]) {
        wcopy(candidate,4096,onedrive); if(candidate[wlen(candidate)-1]!=L'\\') wcat(candidate,4096,L"\\"); wcat(candidate,4096,L"Desktop");
        if(GetFileAttributesW(candidate)!=INVALID_FILE_ATTRIBUTES){wcopy(out,cap,candidate);return TRUE;}
    }
    return FALSE;
}

static BOOL write_url_shortcut(LPCWSTR desktop, LPCWSTR exe) {
    WCHAR path[4096], text[12288], url[8192]; unsigned int i,j=0; HANDLE h; DWORD wrote=0; char utf8[24576]; int bytes;
    wcopy(path,4096,desktop); if(path[wlen(path)-1]!=L'\\') wcat(path,4096,L"\\"); wcat(path,4096,L"ClaudeCodeManager.url");
    url[0]=0; wcat(url,8192,L"file:///");
    for(i=0;exe[i] && j+4<8192;i++) {
        WCHAR c=exe[i];
        if(c==L'\\') { unsigned int n=wlen(url); url[n]=L'/'; url[n+1]=0; }
        else if(c==L' ') wcat(url,8192,L"%20");
        else { unsigned int n=wlen(url); url[n]=c; url[n+1]=0; }
    }
    text[0]=0; wcat(text,12288,L"[InternetShortcut]\r\nURL="); wcat(text,12288,url); wcat(text,12288,L"\r\nIconFile="); wcat(text,12288,exe); wcat(text,12288,L"\r\nIconIndex=0\r\n");
    bytes=WideCharToMultiByte(CP_UTF8,0,text,-1,utf8,sizeof(utf8),0,0); if(bytes<=1) return FALSE;
    h=CreateFileW(path,GENERIC_WRITE,FILE_SHARE_READ,0,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,0); if(h==INVALID_HANDLE_VALUE)return FALSE;
    if(!WriteFile(h,utf8,(DWORD)(bytes-1),&wrote,0)||wrote!=(DWORD)(bytes-1)){CloseHandle(h);return FALSE;} CloseHandle(h); return TRUE;
}

static BOOL shortcut_points_to_our_exe(LPCWSTR shortcut_path) {
    IShellLinkW *link=0;
    IPersistFile *persist=0;
    WCHAR target[MAX_PATH];
    BOOL ok=FALSE;
    HRESULT hr=CoInitializeEx(0,COINIT_APARTMENTTHREADED);
    if(hr<0) return FALSE;
    hr=CoCreateInstance(&CLSID_ShellLink_CCM,0,CLSCTX_INPROC_SERVER,&IID_IShellLinkW_CCM,(PVOID*)&link);
    if(hr<0 || !link) goto done;
    hr=link->lpVtbl->QueryInterface(link,&IID_IPersistFile_CCM,(PVOID*)&persist);
    if(hr<0 || !persist) goto done;
    hr=persist->lpVtbl->Load(persist,shortcut_path,STGM_READ);
    if(hr<0) goto done;
    hr=link->lpVtbl->GetPath(link,target,MAX_PATH,0,0);
    if(hr<0) goto done;
    if(weq_ci(target,g_module_path)) ok=TRUE;
done:
    if(persist) persist->lpVtbl->Release(persist);
    if(link) link->lpVtbl->Release(link);
    CoUninitialize();
    return ok;
}

static BOOL create_desktop_shortcut(BOOL notify) {
    HRESULT hr=0;
    IShellLinkW *link=0;
    IPersistFile *persist=0;
    WCHAR desktop[4096], shortcut[4096], old_shortcut[4096], workdir[4096];
    BOOL com_initialized=FALSE, ok=FALSE, fallback=FALSE;
    if (g_shortcut_busy) return FALSE;
    g_shortcut_busy = 1;
    GetModuleFileNameW(0, g_module_path, 4096);
    wcopy(workdir,4096,g_module_path);
    { int i=(int)wlen(workdir)-1; while(i>=0 && workdir[i]!=L'\\')i--; if(i>=0)workdir[i]=0; }
    if(!desktop_path_resolve(desktop,4096)) { hr=(HRESULT)0x80070003; goto done; }
    wcopy(shortcut,4096,desktop); if(shortcut[wlen(shortcut)-1]!=L'\\') wcat(shortcut,4096,L"\\"); wcat(shortcut,4096,L"ClaudeCodeManager.lnk");
    wcopy(old_shortcut,4096,desktop); if(old_shortcut[wlen(old_shortcut)-1]!=L'\\') wcat(old_shortcut,4096,L"\\"); wcat(old_shortcut,4096,L"Fongap Claude Code.lnk");
    /* Only delete the old shortcut if it was created by this application */
    if(GetFileAttributesW(old_shortcut)!=INVALID_FILE_ATTRIBUTES && shortcut_points_to_our_exe(old_shortcut)) {
        DeleteFileW(old_shortcut);
    }
    hr=CoInitializeEx(0,COINIT_APARTMENTTHREADED);
    if(hr>=0) com_initialized=TRUE;
    hr=CoCreateInstance(&CLSID_ShellLink_CCM,0,CLSCTX_INPROC_SERVER,&IID_IShellLinkW_CCM,(PVOID*)&link);
    if(hr<0 || !link) goto fallback_url;
    hr=link->lpVtbl->SetPath(link,g_module_path); if(hr<0) goto fallback_url;
    link->lpVtbl->SetWorkingDirectory(link,workdir);
    link->lpVtbl->SetDescription(link,L"Claude Code Manager");
    link->lpVtbl->SetIconLocation(link,g_module_path,0);
    link->lpVtbl->SetShowCmd(link,SW_SHOWNORMAL);
    hr=link->lpVtbl->QueryInterface(link,&IID_IPersistFile_CCM,(PVOID*)&persist); if(hr<0 || !persist) goto fallback_url;
    hr=persist->lpVtbl->Save(persist,shortcut,TRUE); if(hr<0) goto fallback_url;
    ok=TRUE; goto done;
fallback_url:
    if(write_url_shortcut(desktop,g_module_path)){ok=TRUE;fallback=TRUE;}
done:
    if(persist) persist->lpVtbl->Release(persist);
    if(link) link->lpVtbl->Release(link);
    if(com_initialized) CoUninitialize();
    if(ok) {
        wcopy(g_cfg_shortcut,16,L"1"); save_config(); set_status(L"桌面快捷方式已创建");
        append_log(fallback?L"[OK] 已创建桌面启动快捷方式：ClaudeCodeManager.url":L"[OK] 已创建桌面快捷方式：ClaudeCodeManager.lnk");
        if(notify) pMessageBoxW(g_main,fallback?L"已在桌面创建 ClaudeCodeManager 启动快捷方式。\r\n\r\n当前系统未能创建 .lnk，已自动使用兼容的 .url 快捷方式。":L"已在当前用户桌面创建 ClaudeCodeManager 快捷方式。",L"ClaudeCodeManager",MB_ICONINFORMATION);
    } else {
        WCHAR code[32]; uint_to_wstr((unsigned int)hr,code,32); set_status(L"桌面快捷方式创建失败");
        append_log_raw(L"[ERROR] 创建桌面快捷方式失败，HRESULT："); append_log(code);
        if(notify) pMessageBoxW(g_main,L"无法创建桌面快捷方式。\r\n\r\n请确认当前账户可以写入桌面目录。",L"ClaudeCodeManager",MB_ICONERROR);
    }
    g_shortcut_busy=0; return ok;
}

static DWORD __stdcall shortcut_thread(PVOID param) {
    create_desktop_shortcut(param ? TRUE : FALSE);
    return 0;
}

static void start_shortcut(BOOL notify) {
    HANDLE th; DWORD tid;
    if (g_shortcut_busy) return;
    th = CreateThread(0,0,shortcut_thread,notify ? (PVOID)1 : 0,0,&tid);
    if (th) CloseHandle(th);
}

static void clear_loaded_credentials(void) {
    WCHAR name[256]; unsigned int i=0,j=0;
    while(g_loaded_credential_names[i]) {
        j=0; while(g_loaded_credential_names[i] && g_loaded_credential_names[i]!=L';' && j+1<256) name[j++]=g_loaded_credential_names[i++]; name[j]=0;
        if(g_loaded_credential_names[i]==L';')i++;
        if(name[0]) SetEnvironmentVariableW(name,0);
    }
    secure_zero_w(g_loaded_credential_names,4096);
}

static int load_credentials_to_environment(void) {
    HKEY key=0; WCHAR names[4096], name[256], target[512]; unsigned int i=0,j; int count=0; CREDENTIALW_CCM *cred=0;
    names[0]=0; g_loaded_credential_names[0]=0;
    if(pRegOpenKeyExW(HKEY_CURRENT_USER,REG_KEY,0,KEY_READ,&key)!=ERROR_SUCCESS)return 0;
    if(!reg_read_string(key,L"ActiveCredentialNames",names,4096))reg_read_string(key,L"CredentialNames",names,4096); pRegCloseKey(key);
    if(weq_ci(names,L"-")){secure_zero_w(names,4096);return 0;}
    while(names[i]) {
        j=0; while(names[i] && names[i]!=L';' && j+1<256)name[j++]=names[i++]; name[j]=0; if(names[i]==L';')i++;
        if(!name[0])continue;
        target[0]=0; wcat(target,512,L"ClaudeCodeManager/"); wcat(target,512,name);
        cred=0;
        if(CredReadW(target,CRED_TYPE_GENERIC,0,&cred) && cred && cred->CredentialBlob && cred->CredentialBlobSize>=2) {
            WCHAR value[4096]; unsigned int chars=cred->CredentialBlobSize/2; if(chars>=4096)chars=4095;
            memcpy(value,cred->CredentialBlob,chars*2); value[chars]=0;
            if(SetEnvironmentVariableW(name,value)) { if(g_loaded_credential_names[0])wcat(g_loaded_credential_names,4096,L";");wcat(g_loaded_credential_names,4096,name);count++; }
            secure_zero_w(value,4096); CredFree(cred);
        } else if(cred) {
            /* If a credential name is registered but the underlying secret is
               missing (e.g. the user cleaned Credential Manager), surface that
               rather than silently skipping — otherwise stale env names hide
               the real cause of "credential not found" failures. */
            append_log_raw(L"[WARN] 已注册凭据缺失，请重新保存："); append_log(name);
            CredFree(cred);
        }
    }
    secure_zero_w(names,4096); return count;
}

static BOOL extract_launcher_argument(LPCWSTR flag, LPWSTR out, unsigned int cap) {
    LPCWSTR cmd=GetCommandLineW(); unsigned int i=0,j=0,flen=wlen(flag); BOOL quoted=FALSE;
    if(!cmd||!flag||!out||cap<2)return FALSE; out[0]=0;
    while(cmd[i]) {
        unsigned int k=0;
        while(k<flen && cmd[i+k] && cmd[i+k]==flag[k])k++;
        if(k==flen && (i==0 || cmd[i-1]==L' ' || cmd[i-1]==L'\t')) {
            i+=flen; while(cmd[i]==L' '||cmd[i]==L'\t')i++;
            if(cmd[i]==L'"'){quoted=TRUE;i++;}
            while(cmd[i] && j+1<cap) {
                if(quoted){if(cmd[i]==L'"')break;} else if(cmd[i]==L' '||cmd[i]==L'\t')break;
                out[j++]=cmd[i++];
            }
            out[j]=0; return out[0]!=0;
        }
        i++;
    }
    return FALSE;
}

static DWORD run_unified_console_launcher(BOOL with_claude, LPCWSTR project) {
    STARTUPINFOW si; PROCESS_INFORMATION pi; WCHAR app[4096], cmd[32768], desc[256]; DWORD code=1; HWND console;
    BOOL allocated_console=FALSE;
    if(!project||!project[0])return 2;
    if(!load_apis())return 3;
    g_instance=(HINSTANCE)GetModuleHandleW(0); load_config();
    SetCurrentProcessExplicitAppUserModelID(with_claude?L"Fongap.ClaudeCodeManager.ClaudeRuntime":L"Fongap.ClaudeCodeManager.Terminal");
    /* Attach to the parent console when one already exists (most common:
       launched from a terminal by hand) so we don't allocate a fresh window
       that the user never asked for. Only allocate a new console when the
       parent really has none. */
    if(!AttachConsole(ATTACH_PARENT_PROCESS)) {
        if(!AllocConsole())return 4;
        allocated_console=TRUE;
    }
    SetConsoleCP(CP_UTF8); SetConsoleOutputCP(CP_UTF8);
    SetConsoleTitleW(with_claude?L"Claude Code":L"Claude Code Terminal");
    console=GetConsoleWindow();
    g_app_icon=LoadIconW(g_instance,(LPCWSTR)(ULONG_PTR)1);
    if(console && g_app_icon){SendMessageW(console,WM_SETICON,ICON_BIG,(LPARAM)g_app_icon);SendMessageW(console,WM_SETICON,ICON_SMALL,(LPARAM)g_app_icon);}
    memset(&si,0,sizeof(si));memset(&pi,0,sizeof(pi));si.cb=sizeof(si);si.dwFlags=STARTF_USESTDHANDLES;
    si.hStdInput=GetStdHandle(STD_INPUT_HANDLE);si.hStdOutput=GetStdHandle(STD_OUTPUT_HANDLE);si.hStdError=GetStdHandle(STD_ERROR_HANDLE);
    app[0]=0;cmd[0]=0;
    if(with_claude) {
        if(!locate_claude(app,4096)){MessageBoxW(0,L"未找到 claude.exe，请先通过 Claude Code Manager 安装。",L"Claude Code",MB_ICONERROR);if(allocated_console)FreeConsole();return 5;}
        wcat(cmd,32768,L"\"");wcat(cmd,32768,app);wcat(cmd,32768,L"\"");
        resolve_effective_proxy(g_effective_proxy,1024,desc,256);set_proxy_environment(g_effective_proxy);load_credentials_to_environment();
    } else {
        GetEnvironmentVariableW(L"ComSpec",app,4096);if(!app[0])wcopy(app,4096,L"C:\\Windows\\System32\\cmd.exe");
        wcat(cmd,32768,L"\"");wcat(cmd,32768,app);wcat(cmd,32768,L"\" /d /k");
    }
    /* CREATE_NEW_PROCESS_GROUP so Ctrl-C inside the console is routed to the
       child (claude / cmd) rather than the launcher, which would leave the
       launcher stuck in WaitForSingleObject. */
    if(CreateProcessW(app,cmd,0,0,TRUE,CREATE_NEW_PROCESS_GROUP|CREATE_UNICODE_ENVIRONMENT,0,project,&si,&pi)) {
        CloseHandle(pi.hThread);WaitForSingleObject(pi.hProcess,INFINITE);GetExitCodeProcess(pi.hProcess,&code);CloseHandle(pi.hProcess);
    } else {code=GetLastError();MessageBoxW(0,with_claude?L"无法启动 Claude Code。":L"无法打开终端。",L"Claude Code Manager",MB_ICONERROR);}
    if(with_claude){clear_loaded_credentials();set_proxy_environment(0);secure_zero_w(g_effective_proxy,1024);}
    /* FreeConsole detaches us from the attached parent terminal too. Only
       release consoles we actually allocated, so we never tear down the
       terminal that launched us. */
    if(allocated_console) FreeConsole();
    return code;
}

static void launch_in_console(BOOL with_claude) {
    WCHAR project[4096], exe[4096]; STARTUPINFOW si; PROCESS_INFORMATION pi;
    if(!get_selected_project(project,4096))return;
    if(with_claude)append_context_budget_summary();
    GetModuleFileNameW(0,exe,4096);memset(&si,0,sizeof(si));memset(&pi,0,sizeof(pi));si.cb=sizeof(si);
    g_cmdline[0]=0;wcat(g_cmdline,32768,L"\"");wcat(g_cmdline,32768,exe);wcat(g_cmdline,32768,with_claude?L"\" --claude-launcher \"":L"\" --terminal-launcher \"");wcat(g_cmdline,32768,project);wcat(g_cmdline,32768,L"\"");
    if(!CreateProcessW(exe,g_cmdline,0,0,FALSE,CREATE_UNICODE_ENVIRONMENT,0,project,&si,&pi)) {
        WCHAR err[32];uint_to_wstr((unsigned int)GetLastError(),err,32);append_log_raw(with_claude?L"[ERROR] 无法启动 Claude Code，系统错误：":L"[ERROR] 无法打开终端，系统错误：");append_log(err);
        pMessageBoxW(g_main,with_claude?L"无法启动 Claude Code。":L"无法打开终端。",L"Claude Code Manager",MB_ICONERROR);return;
    }
    CloseHandle(pi.hThread);CloseHandle(pi.hProcess);
    append_log_raw(with_claude?L"[OK] 已使用统一图标在所选文件夹启动 Claude Code：":L"[OK] 已使用统一图标打开终端：");append_log(project);
    set_status(with_claude?L"Claude Code 已启动":L"终端已打开");
}

static void open_project_folder(void) {
    WCHAR project[4096];
    if (!get_selected_project(project, 4096)) return;
    pShellExecuteW(g_main, L"open", project, 0, 0, SW_SHOWNORMAL);
}

static void browse_workspace(void) {
    BROWSEINFOW bi;
    WCHAR display[MAX_PATH];
    WCHAR selected[4096];
    LPITEMIDLIST pidl;
    memset(&bi, 0, sizeof(bi));
    display[0] = 0;
    selected[0] = 0;
    bi.hwndOwner = g_main;
    bi.pszDisplayName = display;
    bi.lpszTitle = L"选择存放 AI 项目的总文件夹";
    bi.ulFlags = BIF_RETURNONLYFSDIRS | BIF_USENEWUI;
    pidl = pSHBrowseForFolderW(&bi);
    if (pidl) {
        if (pSHGetPathFromIDListW(pidl, selected)) {
            pSetWindowTextW(g_workspace, selected);
            save_config();
            refresh_projects();
        }
        pCoTaskMemFree(pidl);
    }
}

static BOOL path_contains_entry(LPCWSTR path, LPCWSTR target) {
    WCHAR part[4096];
    unsigned int i = 0, j = 0;
    while (1) {
        WCHAR c = path[i++];
        if (c == L';' || c == 0) {
            while (j && (part[j-1] == L' ' || part[j-1] == L'\\')) j--;
            part[j] = 0;
            if (j && path_equal(part, target)) return TRUE;
            j = 0;
            if (c == 0) break;
        } else if (j + 1 < 4096) {
            if (!(j == 0 && c == L' ')) part[j++] = c;
        }
    }
    return FALSE;
}

static BOOL ensure_user_path(void) {
    HKEY key = 0;
    DWORD disp = 0, type = REG_SZ, bytes = sizeof(g_path_buffer);
    LONG query_result;
    WCHAR profile[4096];
    WCHAR target[4096];
    ULONG_PTR result = 0;
    profile[0] = 0;
    if (!GetEnvironmentVariableW(L"USERPROFILE", profile, 4096)) return FALSE;
    wcopy(target, 4096, profile);
    wcat(target, 4096, L"\\.local\\bin");
    g_path_buffer[0] = 0;
    if (pRegCreateKeyExW(HKEY_CURRENT_USER, L"Environment", 0, 0, 0, KEY_READ | KEY_WRITE, 0, &key, &disp) != ERROR_SUCCESS) return FALSE;
    query_result=pRegQueryValueExW(key,L"Path",0,&type,(BYTE*)g_path_buffer,&bytes);
    /* User PATH is frequently REG_EXPAND_SZ. Treating that type as an empty
       value would overwrite the user's entire PATH. Only start from empty when
       the value genuinely does not exist; otherwise preserve both data/type. */
    if(query_result==ERROR_FILE_NOT_FOUND) { g_path_buffer[0]=0; type=REG_EXPAND_SZ; }
    else if(query_result!=ERROR_SUCCESS || (type!=REG_SZ && type!=REG_EXPAND_SZ)) {
        pRegCloseKey(key);
        return FALSE;
    }
    g_path_buffer[(sizeof(g_path_buffer)/2)-1] = 0;
    if (!path_contains_entry(g_path_buffer, target)) {
        if (g_path_buffer[0] && g_path_buffer[wlen(g_path_buffer)-1] != L';') wcat(g_path_buffer, sizeof(g_path_buffer)/2, L";");
        wcat(g_path_buffer, sizeof(g_path_buffer)/2, target);
        if (pRegSetValueExW(key, L"Path", 0, type, (const BYTE*)g_path_buffer, (wlen(g_path_buffer)+1)*2) != ERROR_SUCCESS) {
            pRegCloseKey(key);
            return FALSE;
        }
    }
    pRegCloseKey(key);
    if (pSendMessageTimeoutW) pSendMessageTimeoutW(HWND_BROADCAST, WM_SETTINGCHANGE, 0, (LPARAM)L"Environment", SMTO_ABORTIFHUNG, 3000, &result);
    g_path_buffer2[0] = 0;
    GetEnvironmentVariableW(L"PATH", g_path_buffer2, sizeof(g_path_buffer2)/2);
    if (!path_contains_entry(g_path_buffer2, target)) {
        if (g_path_buffer2[0] && g_path_buffer2[wlen(g_path_buffer2)-1] != L';') wcat(g_path_buffer2, sizeof(g_path_buffer2)/2, L";");
        wcat(g_path_buffer2, sizeof(g_path_buffer2)/2, target);
        SetEnvironmentVariableW(L"PATH", g_path_buffer2);
    }
    return TRUE;
}

static void append_normalized_wide(const WCHAR *text,int n) {
    int i,o=0;
    g_path_buffer[0] = 0;
    for (i = 0; i < n && o + 3 < (int)(sizeof(g_path_buffer)/2); i++) {
        WCHAR c = text[i];
        if (c == L'\r') {
            g_path_buffer[o++] = L'\r';
            if (i + 1 >= n || text[i+1] != L'\n') g_path_buffer[o++] = L'\n';
        } else if (c == L'\n') {
            g_path_buffer[o++] = L'\r';
            g_path_buffer[o++] = L'\n';
        } else {
            g_path_buffer[o++] = c;
        }
    }
    g_path_buffer[o] = 0;
    append_log_raw(g_path_buffer);
}

static void normalize_and_append_output(const char *bytes, int len) {
    int n=0,trim,total;
    if(len<=0)return;
    if(len>(int)sizeof(g_output_pending)-g_output_pending_len)len=(int)sizeof(g_output_pending)-g_output_pending_len;
    memcpy(g_output_pending+g_output_pending_len,bytes,(unsigned long long)len);
    total=g_output_pending_len+len;
    /* ReadFile may split a UTF-8 code point. Keep up to three trailing bytes
       for the next chunk instead of decoding the whole chunk as OEM text. */
    for(trim=0;trim<=3 && total-trim>0;trim++) {
        n=MultiByteToWideChar(CP_UTF8,MB_ERR_INVALID_CHARS,g_output_pending,total-trim,g_output_wide,16000);
        if(n>0)break;
    }
    if(n>0) {
        if(trim>0)memcpy(g_output_pending,g_output_pending+total-trim,(unsigned long long)trim);
        g_output_pending_len=trim;
    } else {
        n=MultiByteToWideChar(CP_OEMCP,0,g_output_pending,total,g_output_wide,16000);
        g_output_pending_len=0;
    }
    if(n>0)append_normalized_wide(g_output_wide,n);
}

static void flush_process_output(void) {
    int n;
    if(g_output_pending_len<=0)return;
    n=MultiByteToWideChar(CP_UTF8,0,g_output_pending,g_output_pending_len,g_output_wide,16000);
    if(n<=0)n=MultiByteToWideChar(CP_OEMCP,0,g_output_pending,g_output_pending_len,g_output_wide,16000);
    g_output_pending_len=0;
    if(n>0)append_normalized_wide(g_output_wide,n);
}

static DWORD run_process_capture(LPWSTR command, LPCWSTR workdir, BOOL heartbeat, LPCWSTR heartbeat_name) {
    SECURITY_ATTRIBUTES sa;
    STARTUPINFOW si;
    PROCESS_INFORMATION pi;
    HANDLE read_pipe = 0, write_pipe = 0, input_null = 0;
    DWORD exit_code = 9001;
    DWORD avail = 0, got = 0;
    ULONGLONG start, lastbeat, now;
    BOOL running = TRUE;
    WCHAR elapsed[32];
    WCHAR line[256];
    g_output_pending_len=0;
    memset(&sa, 0, sizeof(sa));
    memset(&si, 0, sizeof(si));
    memset(&pi, 0, sizeof(pi));
    sa.nLength = sizeof(sa);
    sa.bInheritHandle = TRUE;
    if (!CreatePipe(&read_pipe, &write_pipe, &sa, 0)) return 9002;
    SetHandleInformation(read_pipe, HANDLE_FLAG_INHERIT, 0);
    si.cb = sizeof(si);
    si.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
    si.wShowWindow = SW_HIDE;
    si.hStdOutput = write_pipe;
    si.hStdError = write_pipe;
    input_null = CreateFileW(L"NUL", GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
    si.hStdInput = (input_null == INVALID_HANDLE_VALUE) ? 0 : input_null;
    if (!CreateProcessW(0, command, 0, 0, TRUE, CREATE_NO_WINDOW|CREATE_NEW_PROCESS_GROUP, 0, workdir, &si, &pi)) {
        CloseHandle(read_pipe);
        CloseHandle(write_pipe);
        if (input_null && input_null != INVALID_HANDLE_VALUE) CloseHandle(input_null);
        { WCHAR errnum[32]; uint_to_wstr((unsigned int)GetLastError(),errnum,32); append_log_raw(L"[ERROR] 无法启动隐藏子进程，系统错误："); append_log(errnum); }
        return 9003;
    }
    if (input_null && input_null != INVALID_HANDLE_VALUE) CloseHandle(input_null);
    CloseHandle(write_pipe);
    write_pipe = 0;
    start = lastbeat = GetTickCount64();
    while (running) {
        while (PeekNamedPipe(read_pipe, 0, 0, 0, &avail, 0) && avail) {
            DWORD take = avail > (sizeof(g_output_bytes)-1) ? (sizeof(g_output_bytes)-1) : avail;
            if (!ReadFile(read_pipe, g_output_bytes, take, &got, 0) || !got) break;
            g_output_bytes[got] = 0;
            normalize_and_append_output(g_output_bytes, (int)got);
        }
        if (WaitForSingleObject(pi.hProcess, 150) == WAIT_OBJECT_0) running = FALSE;
        now = GetTickCount64();
        if (heartbeat && now - lastbeat >= 5000) {
            elapsed_text(now - start, elapsed, 32);
            line[0] = 0;
            wcat(line, 256, L"[WAIT] ");
            wcat(line, 256, heartbeat_name);
            wcat(line, 256, L" \u4ecd\u5728\u8fd0\u884c\uff0c\u5df2\u7528\u65f6 ");
            wcat(line, 256, elapsed);
            append_log(line);
            lastbeat = now;
        }
    }
    while (PeekNamedPipe(read_pipe, 0, 0, 0, &avail, 0) && avail) {
        DWORD take = avail > (sizeof(g_output_bytes)-1) ? (sizeof(g_output_bytes)-1) : avail;
        if (!ReadFile(read_pipe, g_output_bytes, take, &got, 0) || !got) break;
        g_output_bytes[got] = 0;
        normalize_and_append_output(g_output_bytes, (int)got);
    }
    flush_process_output();
    GetExitCodeProcess(pi.hProcess, &exit_code);
    CloseHandle(pi.hThread);
    CloseHandle(pi.hProcess);
    CloseHandle(read_pipe);
    return exit_code;
}

/* Native JSON import. No PowerShell, script host, or hidden helper process is used. */
typedef struct _CCM_JSON_OUT { LPWSTR data; unsigned int cap; unsigned int pos; BOOL ok; } CCM_JSON_OUT;

static BOOL read_text_file_w(LPCWSTR path, LPWSTR out, unsigned int cap) {
    HANDLE h; DWORD got=0,total=0; int chars; unsigned int i,start=0;
    if(!out||cap<2) return FALSE; out[0]=0;
    h=CreateFileW(path,GENERIC_READ,FILE_SHARE_READ|FILE_SHARE_WRITE,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0);
    if(h==INVALID_HANDLE_VALUE) return FALSE;
    while(total+4<sizeof(g_json_io_bytes)) {
        DWORD take=(DWORD)(sizeof(g_json_io_bytes)-total-4); if(take>32768)take=32768;
        if(!ReadFile(h,g_json_io_bytes+total,take,&got,0)||!got)break; total+=got;
    }
    CloseHandle(h);
    if(total+4>=sizeof(g_json_io_bytes)) return FALSE;
    if(total>=2 && g_json_io_bytes[0]==0xFF && g_json_io_bytes[1]==0xFE) {
        start=2; chars=(int)((total-start)/2); if(chars>=(int)cap)chars=(int)cap-1;
        for(i=0;i<(unsigned int)chars;i++)out[i]=(WCHAR)(g_json_io_bytes[start+i*2]|((WORD)g_json_io_bytes[start+i*2+1]<<8));
        out[chars]=0; return TRUE;
    }
    if(total>=2 && g_json_io_bytes[0]==0xFE && g_json_io_bytes[1]==0xFF) return FALSE;
    if(total>=3 && g_json_io_bytes[0]==0xEF && g_json_io_bytes[1]==0xBB && g_json_io_bytes[2]==0xBF)start=3;
    chars=MultiByteToWideChar(CP_UTF8,MB_ERR_INVALID_CHARS,(LPCSTR)(g_json_io_bytes+start),(int)(total-start),out,(int)cap-1);
    if(chars<=0)chars=MultiByteToWideChar(CP_ACP,0,(LPCSTR)(g_json_io_bytes+start),(int)(total-start),out,(int)cap-1);
    if(chars<=0)return FALSE; out[chars]=0; return TRUE;
}

static BOOL write_utf8_text_file(LPCWSTR path,LPCWSTR text) {
    HANDLE h; DWORD wrote=0,total=0; int bytes;
    bytes=WideCharToMultiByte(CP_UTF8,0,text,-1,(LPSTR)g_json_io_bytes,(int)sizeof(g_json_io_bytes),0,0);
    if(bytes<=0 || bytes>(int)sizeof(g_json_io_bytes))return FALSE;
    bytes--; /* do not write trailing NUL */
    h=CreateFileW(path,GENERIC_WRITE,FILE_SHARE_READ,0,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,0);
    if(h==INVALID_HANDLE_VALUE)return FALSE;
    while(total<(DWORD)bytes){DWORD chunk=(DWORD)bytes-total;if(!WriteFile(h,g_json_io_bytes+total,chunk,&wrote,0)||!wrote){CloseHandle(h);return FALSE;}total+=wrote;}
    CloseHandle(h);return TRUE;
}

static void json_skip_ws(LPCWSTR text,int *pos){while(text[*pos]&&(text[*pos]==L' '||text[*pos]==L'\t'||text[*pos]==L'\r'||text[*pos]==L'\n'))(*pos)++;}
static int json_new_token(CCM_JSON_TOKEN *tokens,int *count,int type,int start){int idx;if(*count>=CCM_JSON_MAX_TOKENS)return -2;idx=(*count)++;tokens[idx].type=type;tokens[idx].start=start;tokens[idx].end=-1;tokens[idx].size=0;return idx;}

static int json_parse_value_native(LPCWSTR text,int *pos,CCM_JSON_TOKEN *tokens,int *count,int depth);

static int json_parse_string_native(LPCWSTR text,int *pos,CCM_JSON_TOKEN *tokens,int *count) {
    int idx,start=*pos; WCHAR c;
    if(text[*pos]!=L'\"')return -1; idx=json_new_token(tokens,count,CCM_JSON_STRING,start);if(idx<0)return -1;(*pos)++;
    while((c=text[*pos])!=0){
        if(c==L'\"'){(*pos)++;tokens[idx].end=*pos;return idx;}
        if(c<L' ')return -1;
        if(c==L'\\'){
            (*pos)++;c=text[*pos];if(!c)return -1;
            if(c==L'u'){int k;(*pos)++;for(k=0;k<4;k++){WCHAR h=text[*pos];if(!((h>=L'0'&&h<=L'9')||(h>=L'a'&&h<=L'f')||(h>=L'A'&&h<=L'F')))return -1;(*pos)++;}continue;}
            if(c!=L'"'&&c!=L'\\'&&c!=L'/'&&c!=L'b'&&c!=L'f'&&c!=L'n'&&c!=L'r'&&c!=L't')return -1;
            (*pos)++;continue;
        }
        (*pos)++;
    }
    return -1;
}

static BOOL json_range_equals(LPCWSTR text,int start,int end,LPCWSTR expected) {
    int i=start,j=0;
    while(i<end&&expected[j]&&text[i]==expected[j]){i++;j++;}
    return i==end&&expected[j]==0;
}

static BOOL json_primitive_valid(LPCWSTR text,int start,int end) {
    int i=start;
    if(json_range_equals(text,start,end,L"true")||json_range_equals(text,start,end,L"false")||json_range_equals(text,start,end,L"null"))return TRUE;
    if(i<end&&text[i]==L'-')i++;
    if(i>=end)return FALSE;
    if(text[i]==L'0')i++;
    else {
        if(text[i]<L'1'||text[i]>L'9')return FALSE;
        while(i<end&&text[i]>=L'0'&&text[i]<=L'9')i++;
    }
    if(i<end&&text[i]==L'.') {
        i++;if(i>=end||text[i]<L'0'||text[i]>L'9')return FALSE;
        while(i<end&&text[i]>=L'0'&&text[i]<=L'9')i++;
    }
    if(i<end&&(text[i]==L'e'||text[i]==L'E')) {
        i++;if(i<end&&(text[i]==L'+'||text[i]==L'-'))i++;
        if(i>=end||text[i]<L'0'||text[i]>L'9')return FALSE;
        while(i<end&&text[i]>=L'0'&&text[i]<=L'9')i++;
    }
    return i==end;
}

static int json_parse_primitive_native(LPCWSTR text,int *pos,CCM_JSON_TOKEN *tokens,int *count) {
    int idx,start=*pos; WCHAR c;
    while((c=text[*pos])!=0 && c!=L',' && c!=L']' && c!=L'}' && c!=L' ' && c!=L'\t' && c!=L'\r' && c!=L'\n') {
        if(c<L' '||c==L'\"'||c==L'\\')return -1;(*pos)++;
    }
    if(*pos==start||!json_primitive_valid(text,start,*pos))return -1;idx=json_new_token(tokens,count,CCM_JSON_PRIMITIVE,start);if(idx<0)return -1;tokens[idx].end=*pos;return idx;
}

static int json_parse_value_native(LPCWSTR text,int *pos,CCM_JSON_TOKEN *tokens,int *count,int depth) {
    int idx,key,val; WCHAR c; if(depth>128)return -1;json_skip_ws(text,pos);c=text[*pos];
    if(c==L'{'){
        idx=json_new_token(tokens,count,CCM_JSON_OBJECT,*pos);if(idx<0)return -1;(*pos)++;json_skip_ws(text,pos);
        if(text[*pos]==L'}'){(*pos)++;tokens[idx].end=*pos;return idx;}
        while(text[*pos]){
            key=json_parse_string_native(text,pos,tokens,count);if(key<0)return -1;json_skip_ws(text,pos);if(text[*pos]!=L':')return -1;(*pos)++;
            val=json_parse_value_native(text,pos,tokens,count,depth+1);if(val<0)return -1;tokens[idx].size++;json_skip_ws(text,pos);
            if(text[*pos]==L','){(*pos)++;json_skip_ws(text,pos);continue;}if(text[*pos]==L'}'){(*pos)++;tokens[idx].end=*pos;return idx;}return -1;
        }return -1;
    }
    if(c==L'['){
        idx=json_new_token(tokens,count,CCM_JSON_ARRAY,*pos);if(idx<0)return -1;(*pos)++;json_skip_ws(text,pos);
        if(text[*pos]==L']'){(*pos)++;tokens[idx].end=*pos;return idx;}
        while(text[*pos]){val=json_parse_value_native(text,pos,tokens,count,depth+1);if(val<0)return -1;tokens[idx].size++;json_skip_ws(text,pos);if(text[*pos]==L','){(*pos)++;json_skip_ws(text,pos);continue;}if(text[*pos]==L']'){(*pos)++;tokens[idx].end=*pos;return idx;}return -1;}return -1;
    }
    if(c==L'\"')return json_parse_string_native(text,pos,tokens,count);
    if(c==0)return -1;return json_parse_primitive_native(text,pos,tokens,count);
}

static int json_parse_document_native(LPCWSTR text,CCM_JSON_TOKEN *tokens,int *count) {
    int pos=0,root;*count=0;root=json_parse_value_native(text,&pos,tokens,count,0);if(root<0)return -1;json_skip_ws(text,&pos);if(text[pos])return -1;return root;
}

static int json_token_after(CCM_JSON_TOKEN *tokens,int idx) {
    int i,j=idx+1;if(idx<0)return idx;
    if(tokens[idx].type==CCM_JSON_OBJECT){for(i=0;i<tokens[idx].size;i++){j=json_token_after(tokens,j);j=json_token_after(tokens,j);}}
    else if(tokens[idx].type==CCM_JSON_ARRAY){for(i=0;i<tokens[idx].size;i++)j=json_token_after(tokens,j);}
    return j;
}

static int hex_value(WCHAR c){if(c>=L'0'&&c<=L'9')return c-L'0';if(c>=L'a'&&c<=L'f')return c-L'a'+10;if(c>=L'A'&&c<=L'F')return c-L'A'+10;return -1;}
static BOOL json_decode_string_token(LPCWSTR text,CCM_JSON_TOKEN *t,LPWSTR out,unsigned int cap) {
    int i=t->start+1,end=t->end-1;unsigned int o=0;if(t->type!=CCM_JSON_STRING||t->end<=t->start+1||!cap)return FALSE;
    while(i<end&&o+1<cap){WCHAR c=text[i++];if(c==L'\\'&&i<end){WCHAR e=text[i++];if(e==L'\"'||e==L'\\'||e==L'/')c=e;else if(e==L'b')c=8;else if(e==L'f')c=12;else if(e==L'n')c=L'\n';else if(e==L'r')c=L'\r';else if(e==L't')c=L'\t';else if(e==L'u'&&i+3<end){int a=hex_value(text[i]),b=hex_value(text[i+1]),d=hex_value(text[i+2]),f=hex_value(text[i+3]);if(a<0||b<0||d<0||f<0)return FALSE;c=(WCHAR)((a<<12)|(b<<8)|(d<<4)|f);i+=4;}else return FALSE;}out[o++]=c;}out[o]=0;return TRUE;
}

static BOOL wends_ci(LPCWSTR s,LPCWSTR suffix){unsigned int ls=wlen(s),lf=wlen(suffix),i;if(lf>ls)return FALSE;for(i=0;i<lf;i++)if(lower_ascii(s[ls-lf+i])!=lower_ascii(suffix[i]))return FALSE;return TRUE;}
static BOOL is_unsupported_manager_field_native(LPCWSTR name){return weq_ci(name,L"CLAUDE_CODE_MAX_CONTEXT_TOKENS")||weq_ci(name,L"contextWindow");}
static BOOL is_sensitive_field_native(LPCWSTR name) {
    static const LPCWSTR exact[]={L"GATEWAY_ACCESS_KEY",L"ANTHROPIC_API_KEY",L"ANTHROPIC_AUTH_TOKEN",L"CLAUDE_CODE_OAUTH_TOKEN",L"OPENAI_API_KEY",L"DEEPSEEK_API_KEY",L"GEMINI_API_KEY",L"GOOGLE_API_KEY",L"AZURE_OPENAI_API_KEY",L"NVIDIA_API_KEY",L"NIM_API_KEY",L"OPENROUTER_API_KEY",L"MINIMAX_API_KEY",L"MISTRAL_API_KEY",L"COHERE_API_KEY",L"AWS_ACCESS_KEY_ID",L"AWS_SECRET_ACCESS_KEY"};
    static const LPCWSTR suffix[]={L"_API_KEY",L"_ACCESS_KEY",L"_AUTH_TOKEN",L"_TOKEN",L"_SECRET",L"_PASSWORD",L"_PRIVATE_KEY",L"_CREDENTIAL",L"_CLIENT_SECRET"};unsigned int i;
    for(i=0;i<sizeof(exact)/sizeof(exact[0]);i++)if(weq_ci(name,exact[i]))return TRUE;for(i=0;i<sizeof(suffix)/sizeof(suffix[0]);i++)if(wends_ci(name,suffix[i]))return TRUE;return FALSE;
}

static BOOL credential_list_has(LPCWSTR list,LPCWSTR name){WCHAR part[256];unsigned int i=0,j;while(list[i]){j=0;while(list[i]&&list[i]!=L';'&&j+1<256)part[j++]=list[i++];part[j]=0;if(list[i]==L';')i++;if(weq_ci(part,name))return TRUE;}return FALSE;}
static BOOL register_credential_name_native(LPCWSTR name) {
    HKEY key=0;DWORD disp=0,bytes;WCHAR names[4096];BOOL ok=TRUE;names[0]=0;
    if(pRegCreateKeyExW(HKEY_CURRENT_USER,REG_KEY,0,0,0,KEY_READ|KEY_WRITE,0,&key,&disp)!=ERROR_SUCCESS)return FALSE;
    reg_read_string(key,L"CredentialNames",names,4096);
    if(!credential_list_has(names,name)){
        if(wlen(names)+wlen(name)+2>=4096)ok=FALSE;
        else {if(names[0])wcat(names,4096,L";");wcat(names,4096,name);bytes=(wlen(names)+1)*2;ok=pRegSetValueExW(key,L"CredentialNames",0,REG_SZ,(const BYTE*)names,bytes)==ERROR_SUCCESS;}
    }
    pRegCloseKey(key);secure_zero_w(names,4096);return ok;
}
static BOOL save_credential_native(LPCWSTR name,LPCWSTR value) {
    CREDENTIALW_CCM c;WCHAR target[512];if(!name||!name[0]||!value||!value[0])return TRUE;memset(&c,0,sizeof(c));target[0]=0;wcat(target,512,L"ClaudeCodeManager/");wcat(target,512,name);
    c.Type=CRED_TYPE_GENERIC;c.TargetName=target;c.Comment=L"Stored by ClaudeCodeManager";c.CredentialBlobSize=wlen(value)*2;c.CredentialBlob=(BYTE*)value;c.Persist=CRED_PERSIST_LOCAL_MACHINE;c.UserName=L"ClaudeCodeManager";
    if(!CredWriteW(&c,0))return FALSE;return register_credential_name_native(name);
}

static BOOL set_active_credential_names_native(LPCWSTR names) {
    HKEY key=0;DWORD disp=0,bytes;BOOL ok;
    if(!names||!names[0])return TRUE;
    if(pRegCreateKeyExW(HKEY_CURRENT_USER,REG_KEY,0,0,0,KEY_READ|KEY_WRITE,0,&key,&disp)!=ERROR_SUCCESS)return FALSE;
    bytes=(wlen(names)+1)*2;ok=pRegSetValueExW(key,L"ActiveCredentialNames",0,REG_SZ,(const BYTE*)names,bytes)==ERROR_SUCCESS;
    pRegCloseKey(key);return ok;
}

static void jout_init(CCM_JSON_OUT *o,LPWSTR data,unsigned int cap){o->data=data;o->cap=cap;o->pos=0;o->ok=TRUE;if(cap)data[0]=0;}
static void jout_char(CCM_JSON_OUT *o,WCHAR c){if(!o->ok)return;if(o->pos+1>=o->cap){o->ok=FALSE;return;}o->data[o->pos++]=c;o->data[o->pos]=0;}
static void jout_text(CCM_JSON_OUT *o,LPCWSTR s){unsigned int i=0;while(s&&s[i])jout_char(o,s[i++]);}
static void jout_raw(CCM_JSON_OUT *o,LPCWSTR text,int start,int end){int i;for(i=start;i<end;i++)jout_char(o,text[i]);}
static void jout_indent(CCM_JSON_OUT *o,int depth){int i;for(i=0;i<depth*2;i++)jout_char(o,L' ');}

static BOOL token_value_to_w(LPCWSTR text,CCM_JSON_TOKEN *token,LPWSTR out,unsigned int cap){if(token->type==CCM_JSON_STRING)return json_decode_string_token(text,token,out,cap);if(token->type==CCM_JSON_PRIMITIVE){wcopy_range(out,cap,text,(unsigned int)token->start,(unsigned int)token->end);return TRUE;}out[0]=0;return FALSE;}
static void import_store_token_secret(LPCWSTR name,LPCWSTR text,CCM_JSON_TOKEN *token){WCHAR value[4096];value[0]=0;if(!token_value_to_w(text,token,value,4096)){append_log_raw(L"[WARN] 敏感字段值不是简单文本，已从配置中移除但未保存：");append_log(name);return;}if(value[0]){if(save_credential_native(name,value)){append_log_raw(L"[SECURE] 已保存到 Windows 凭据管理器：");append_log(name);}else{g_import_secret_error=TRUE;append_log_raw(L"[ERROR] 无法保存敏感凭据：");append_log(name);}}secure_zero_w(value,4096);}

static BOOL json_object_find_key(LPCWSTR text,CCM_JSON_TOKEN *tokens,int object_idx,LPCWSTR wanted,int *key_idx,int *value_idx){int i,j=object_idx+1;WCHAR key[256];if(tokens[object_idx].type!=CCM_JSON_OBJECT)return FALSE;for(i=0;i<tokens[object_idx].size;i++){int k=j,v=k+1;key[0]=0;json_decode_string_token(text,&tokens[k],key,256);if(weq_ci(key,wanted)){if(key_idx)*key_idx=k;if(value_idx)*value_idx=v;return TRUE;}j=json_token_after(tokens,v);}return FALSE;}

static void json_write_clean(CCM_JSON_OUT *o,LPCWSTR text,CCM_JSON_TOKEN *tokens,int idx,int depth);
static void json_write_clean_object(CCM_JSON_OUT *o,LPCWSTR text,CCM_JSON_TOKEN *tokens,int idx,int depth){int i,j=idx+1,written=0;WCHAR key[256];jout_char(o,L'{');for(i=0;i<tokens[idx].size;i++){int k=j,v=k+1;key[0]=0;json_decode_string_token(text,&tokens[k],key,256);if(is_sensitive_field_native(key)){import_store_token_secret(key,text,&tokens[v]);}else if(is_unsupported_manager_field_native(key)){}else{if(written)jout_char(o,L',');jout_text(o,L"\r\n");jout_indent(o,depth+1);jout_raw(o,text,tokens[k].start,tokens[k].end);jout_text(o,L": ");json_write_clean(o,text,tokens,v,depth+1);written++;}j=json_token_after(tokens,v);}if(written){jout_text(o,L"\r\n");jout_indent(o,depth);}jout_char(o,L'}');}
static void json_write_clean(CCM_JSON_OUT *o,LPCWSTR text,CCM_JSON_TOKEN *tokens,int idx,int depth){int i,j;if(tokens[idx].type==CCM_JSON_OBJECT){json_write_clean_object(o,text,tokens,idx,depth);return;}if(tokens[idx].type==CCM_JSON_ARRAY){jout_char(o,L'[');j=idx+1;for(i=0;i<tokens[idx].size;i++){if(i)jout_text(o,L", ");json_write_clean(o,text,tokens,j,depth+1);j=json_token_after(tokens,j);}jout_char(o,L']');return;}jout_raw(o,text,tokens[idx].start,tokens[idx].end);}

static void json_write_merged(CCM_JSON_OUT *o,LPCWSTR oldtext,CCM_JSON_TOKEN *oldt,int oldidx,LPCWSTR newtext,CCM_JSON_TOKEN *newt,int newidx,int depth) {
    int i,j,written=0;WCHAR key[256];
    if(oldidx<0||oldt[oldidx].type!=CCM_JSON_OBJECT||newidx<0||newt[newidx].type!=CCM_JSON_OBJECT){json_write_clean(o,newtext,newt,newidx,depth);return;}
    jout_char(o,L'{');j=oldidx+1;
    for(i=0;i<oldt[oldidx].size;i++){
        int ok=j,ov=ok+1,nk=-1,nv=-1;key[0]=0;json_decode_string_token(oldtext,&oldt[ok],key,256);
        if(is_sensitive_field_native(key)){import_store_token_secret(key,oldtext,&oldt[ov]);if(json_object_find_key(newtext,newt,newidx,key,&nk,&nv))import_store_token_secret(key,newtext,&newt[nv]);}
        else if(is_unsupported_manager_field_native(key)){}
        else{
            if(written)jout_char(o,L',');jout_text(o,L"\r\n");jout_indent(o,depth+1);
            if(json_object_find_key(newtext,newt,newidx,key,&nk,&nv)){jout_raw(o,newtext,newt[nk].start,newt[nk].end);jout_text(o,L": ");if(oldt[ov].type==CCM_JSON_OBJECT&&newt[nv].type==CCM_JSON_OBJECT)json_write_merged(o,oldtext,oldt,ov,newtext,newt,nv,depth+1);else json_write_clean(o,newtext,newt,nv,depth+1);}
            else{jout_raw(o,oldtext,oldt[ok].start,oldt[ok].end);jout_text(o,L": ");json_write_clean(o,oldtext,oldt,ov,depth+1);}written++;
        }
        j=json_token_after(oldt,ov);
    }
    j=newidx+1;
    for(i=0;i<newt[newidx].size;i++){
        int nk=j,nv=nk+1,foundk=-1,foundv=-1;key[0]=0;json_decode_string_token(newtext,&newt[nk],key,256);
        if(is_sensitive_field_native(key)){if(!json_object_find_key(oldtext,oldt,oldidx,key,&foundk,&foundv))import_store_token_secret(key,newtext,&newt[nv]);}
        else if(is_unsupported_manager_field_native(key)){}
        else if(!json_object_find_key(oldtext,oldt,oldidx,key,&foundk,&foundv)){if(written)jout_char(o,L',');jout_text(o,L"\r\n");jout_indent(o,depth+1);jout_raw(o,newtext,newt[nk].start,newt[nk].end);jout_text(o,L": ");json_write_clean(o,newtext,newt,nv,depth+1);written++;}
        j=json_token_after(newt,nv);
    }
    if(written){jout_text(o,L"\r\n");jout_indent(o,depth);}jout_char(o,L'}');
}

static BOOL ensure_parent_directory_native(LPCWSTR target){WCHAR dir[4096];int i;DWORD err;wcopy(dir,4096,target);i=(int)wlen(dir)-1;while(i>=0&&dir[i]!=L'\\')i--;if(i<=0)return FALSE;dir[i]=0;err=(DWORD)SHCreateDirectoryExW(g_main,dir,0);return err==0||err==ERROR_ALREADY_EXISTS||GetFileAttributesW(dir)!=INVALID_FILE_ATTRIBUTES;}

static BOOL choose_settings_target(LPWSTR out,unsigned int cap,BOOL importing) {
    int r;WCHAR project[4096],profile[4096];LPCWSTR title=importing?L"选择导入位置":L"选择要打开的配置";
    r=pMessageBoxW(g_main,importing?L"请选择配置要导入到哪里：\r\n\r\n“是”＝所有项目（全局配置）\r\n“否”＝当前项目\r\n“取消”＝返回":L"请选择要打开哪一份配置：\r\n\r\n“是”＝所有项目（全局配置）\r\n“否”＝当前项目\r\n“取消”＝返回",title,MB_YESNOCANCEL|MB_ICONQUESTION);
    if(r==IDCANCEL)return FALSE;if(r==IDYES){profile[0]=0;GetEnvironmentVariableW(L"USERPROFILE",profile,4096);if(!profile[0])return FALSE;wcopy(out,cap,profile);wcat(out,cap,L"\\.claude\\settings.json");return TRUE;}
    if(!get_selected_project(project,4096))return FALSE;
    r=pMessageBoxW(g_main,importing?L"请选择当前项目配置类型：\r\n\r\n“是”＝团队共享 settings.json\r\n“否”＝仅本机 settings.local.json":L"请选择要打开的当前项目配置：\r\n\r\n“是”＝团队共享 settings.json\r\n“否”＝仅本机 settings.local.json",title,MB_YESNOCANCEL|MB_ICONQUESTION);
    if(r==IDCANCEL)return FALSE;wcopy(out,cap,project);wcat(out,cap,r==IDYES?L"\\.claude\\settings.json":L"\\.claude\\settings.local.json");return TRUE;
}

static BOOL select_json_file(LPWSTR out,unsigned int cap){OPENFILENAMEW ofn;static const WCHAR filter[]=L"JSON files (*.json)\0*.json\0All files (*.*)\0*.*\0\0";memset(&ofn,0,sizeof(ofn));out[0]=0;ofn.lStructSize=sizeof(ofn);ofn.hwndOwner=g_main;ofn.lpstrFilter=filter;ofn.lpstrFile=out;ofn.nMaxFile=cap;ofn.lpstrTitle=L"选择要导入的 settings.json";ofn.Flags=OFN_EXPLORER|OFN_FILEMUSTEXIST|OFN_PATHMUSTEXIST;ofn.lpstrDefExt=L"json";return GetOpenFileNameW(&ofn);}

static DWORD __stdcall import_thread(PVOID unused) {
    int srcroot=-1,tgtroot=-1;BOOL target_exists,merge;CCM_JSON_OUT out;WCHAR temp[4096],backup[4096],tick[32];DWORD code=0;(void)unused;
    g_import_busy=1;g_import_secret_error=FALSE;set_busy(TRUE);set_status(L"正在导入 settings.json...");append_log(L"");append_log(L"========================================");append_log(L"settings.json 原生安全导入");append_log(L"========================================");
    if(!read_text_file_w(g_import_source,g_json_source_text,sizeof(g_json_source_text)/2)){append_log(L"[ERROR] 无法读取导入文件，或文件过大。");code=10;goto done;}
    srcroot=json_parse_document_native(g_json_source_text,g_json_source_tokens,&g_json_source_count);if(srcroot==-2||(srcroot<0&&srcroot!=-2)){append_log(srcroot==-2?L"[ERROR] 导入文件过大，JSON token 数量超限。":L"[ERROR] 导入文件不是有效的 JSON 对象。");code=11;goto done;}
    target_exists=GetFileAttributesW(g_import_target)!=INVALID_FILE_ATTRIBUTES;merge=weq_ci(g_import_mode,L"Merge");
    if(target_exists){if(!read_text_file_w(g_import_target,g_json_target_text,sizeof(g_json_target_text)/2)){append_log(L"[ERROR] 无法读取现有目标配置。");code=12;goto done;}tgtroot=json_parse_document_native(g_json_target_text,g_json_target_tokens,&g_json_target_count);if(tgtroot==-2||(tgtroot<0&&tgtroot!=-2)){append_log(tgtroot==-2?L"[ERROR] 现有配置过大，JSON token 数量超限。":L"[ERROR] 现有目标配置不是有效的 JSON 对象，未进行覆盖。");code=13;goto done;}}
    if(!ensure_parent_directory_native(g_import_target)){append_log(L"[ERROR] 无法创建目标 .claude 文件夹。");code=14;goto done;}
    if(target_exists){jout_init(&out,g_json_output_text,sizeof(g_json_output_text)/2);json_write_clean(&out,g_json_target_text,g_json_target_tokens,tgtroot,0);jout_text(&out,L"\r\n");if(!out.ok){append_log(L"[ERROR] 现有配置过大，无法生成安全备份。");code=15;goto done;}wcopy(backup,4096,g_import_target);wcat(backup,4096,L".backup-");uint_to_wstr((unsigned int)GetTickCount64(),tick,32);wcat(backup,4096,tick);if(!write_utf8_text_file(backup,g_json_output_text)){append_log(L"[ERROR] 无法创建导入前备份。");code=16;goto done;}append_log_raw(L"[INFO] 已创建脱敏备份：");append_log(backup);}
    jout_init(&out,g_json_output_text,sizeof(g_json_output_text)/2);if(merge&&target_exists)json_write_merged(&out,g_json_target_text,g_json_target_tokens,tgtroot,g_json_source_text,g_json_source_tokens,srcroot,0);else json_write_clean(&out,g_json_source_text,g_json_source_tokens,srcroot,0);jout_text(&out,L"\r\n");if(!out.ok){append_log(L"[ERROR] 合并后的配置过大。");code=17;goto done;}if(g_import_secret_error){append_log(L"[ERROR] 敏感凭据未能全部保存，配置文件未写入。");code=18;goto done;}
    if(g_extra_secret_name[0]&&g_extra_secret_value[0]){if(!save_credential_native(g_extra_secret_name,g_extra_secret_value)){append_log(L"[ERROR] 无法保存模型向导中的访问密钥。");code=18;goto done;}append_log_raw(L"[SECURE] 已保存到 Windows 凭据管理器：");append_log(g_extra_secret_name);}
    if(g_extra_secret2_name[0]&&g_extra_secret2_value[0]){if(!save_credential_native(g_extra_secret2_name,g_extra_secret2_value)){append_log(L"[ERROR] 无法保存网关访问密钥。");code=18;goto done;}append_log_raw(L"[SECURE] 已保存到 Windows 凭据管理器：");append_log(g_extra_secret2_name);}
    wcopy(temp,4096,g_import_target);wcat(temp,4096,L".ccm.tmp");if(!write_utf8_text_file(temp,g_json_output_text)){append_log(L"[ERROR] 无法写入临时配置文件。");code=19;goto done;}if(!MoveFileExW(temp,g_import_target,MOVEFILE_REPLACE_EXISTING|MOVEFILE_WRITE_THROUGH)){DeleteFileW(temp);append_log(L"[ERROR] 无法原子替换目标配置文件。");code=20;goto done;}
    if(g_pending_active_credential_names[0]&&!set_active_credential_names_native(g_pending_active_credential_names))append_log(L"[WARN] 配置已保存，但活动凭据范围未能更新。");
    append_log(L"[OK] 配置导入完成；敏感字段未写入 settings.json。");append_log_raw(L"[OK] 目标文件：");append_log(g_import_target);set_status(L"settings.json 导入完成");
done:
    if(code){WCHAR n[32];uint_to_wstr(code,n,32);append_log_raw(L"[ERROR] 导入失败，错误代码：");append_log(n);set_status(L"settings.json 导入失败");}
    if(g_import_delete_source&&g_import_source[0])DeleteFileW(g_import_source);g_import_delete_source=FALSE;secure_zero_w(g_import_source,4096);secure_zero_w(g_extra_secret_name,128);secure_zero_w(g_extra_secret_value,4096);secure_zero_w(g_extra_secret2_name,128);secure_zero_w(g_extra_secret2_value,4096);secure_zero_w(g_pending_active_credential_names,512);secure_zero_w(g_json_source_text,sizeof(g_json_source_text)/2);secure_zero_w(g_json_target_text,sizeof(g_json_target_text)/2);g_import_busy=0;set_busy(FALSE);return code;
}

static void start_import_settings(void){
    HANDLE th;DWORD tid;int r;
    if(g_installing||g_import_busy)return;
    if(!select_json_file(g_import_source,4096))return;
    secure_zero_w(g_extra_secret_name,128);secure_zero_w(g_extra_secret_value,4096);secure_zero_w(g_extra_secret2_name,128);secure_zero_w(g_extra_secret2_value,4096);secure_zero_w(g_pending_active_credential_names,512);g_import_delete_source=FALSE;
    if(!choose_settings_target(g_import_target,4096,TRUE)){secure_zero_w(g_import_source,4096);return;}
    r=pMessageBoxW(g_main,L"导入时会自动移除 Token、API Key、Password 等敏感字段，并保存到当前用户的 Windows 凭据管理器。\r\n\r\n原始导入文件不会被修改。是否继续？",L"导入配置",MB_YESNO|MB_ICONINFORMATION);if(r!=IDYES){secure_zero_w(g_import_source,4096);return;}
    r=pMessageBoxW(g_main,L"选择导入方式：\r\n\r\n“是”＝智能合并（推荐）\r\n“否”＝完全替换\r\n“取消”＝返回",L"导入方式",MB_YESNOCANCEL|MB_ICONQUESTION);if(r==IDCANCEL){secure_zero_w(g_import_source,4096);return;}
    wcopy(g_import_mode,32,r==IDYES?L"Merge":L"Replace");g_import_busy=1;set_busy(TRUE);
    th=CreateThread(0,0,import_thread,0,0,&tid);
    if(th)CloseHandle(th);else{g_import_busy=0;set_busy(FALSE);secure_zero_w(g_import_source,4096);pMessageBoxW(g_main,L"无法启动配置导入线程。",L"ClaudeCodeManager",MB_ICONERROR);}
}


static BOOL ensure_settings_file(LPCWSTR target) {
    WCHAR dir[4096]; HANDLE h; DWORD wrote=0, err;
    int i;
    wcopy(dir,4096,target);
    i=(int)wlen(dir)-1; while(i>=0 && dir[i]!=L'\\') i--;
    if(i<=0) return FALSE;
    dir[i]=0;
    err=(DWORD)SHCreateDirectoryExW(g_main,dir,0);
    if(err!=0 && err!=ERROR_ALREADY_EXISTS && GetFileAttributesW(dir)==INVALID_FILE_ATTRIBUTES) return FALSE;
    h=CreateFileW(target,GENERIC_WRITE,FILE_SHARE_READ,0,OPEN_ALWAYS,FILE_ATTRIBUTE_NORMAL,0);
    if(h==INVALID_HANDLE_VALUE) return FALSE;
    err=GetLastError();
    if(err!=ERROR_ALREADY_EXISTS) {
        const BYTE json_bytes[2]={(BYTE)'{',(BYTE)'}'};
        if(!WriteFile(h,json_bytes,2,&wrote,0) || wrote!=2) { CloseHandle(h); return FALSE; }
    }
    CloseHandle(h);
    return TRUE;
}

static void open_settings_file(void) {
    WCHAR target[4096], notepad[4096], params[8192], workdir[4096]; STARTUPINFOW si; PROCESS_INFORMATION pi;
    if(!choose_settings_target(target,4096,FALSE)) return;
    if(!ensure_settings_file(target)) {
        append_log(L"[ERROR] 无法创建或准备配置文件。");
        pMessageBoxW(g_main,L"无法创建或准备配置文件。\r\n\r\n请检查当前账户是否有权写入 .claude 文件夹。",L"ClaudeCodeManager",MB_ICONERROR); return;
    }
    notepad[0]=0; if(!SearchPathW(0,L"notepad.exe",0,4096,notepad,0))wcopy(notepad,4096,L"C:\\Windows\\System32\\notepad.exe");
    params[0]=0;wcat(params,8192,L"\"");wcat(params,8192,notepad);wcat(params,8192,L"\" \"");wcat(params,8192,target);wcat(params,8192,L"\"");
    wcopy(workdir,4096,target);{int i=(int)wlen(workdir)-1;while(i>=0&&workdir[i]!=L'\\')i--;if(i>=0)workdir[i]=0;}
    memset(&si,0,sizeof(si));memset(&pi,0,sizeof(pi));si.cb=sizeof(si);si.dwFlags=STARTF_USESHOWWINDOW;si.wShowWindow=SW_SHOWNORMAL;
    if(!CreateProcessW(notepad,params,0,0,FALSE,0,0,workdir,&si,&pi)) {
        LONG_PTR r=(LONG_PTR)pShellExecuteW(g_main,L"open",target,0,workdir,SW_SHOWNORMAL);
        if(r<=32){WCHAR err[32];uint_to_wstr((unsigned int)GetLastError(),err,32);append_log_raw(L"[ERROR] 无法打开配置文件，系统错误：");append_log(err);pMessageBoxW(g_main,L"配置文件已经创建，但无法启动记事本或默认编辑器。",L"ClaudeCodeManager",MB_ICONERROR);return;}
    } else {CloseHandle(pi.hThread);CloseHandle(pi.hProcess);}
    append_log_raw(L"[OK] 已打开配置文件：");append_log(target);set_status(L"配置文件已打开");
}

static BOOL url_parse_basic(LPCWSTR url, LPWSTR host, unsigned int hcap, LPWSTR path, unsigned int pcap, INTERNET_PORT *port, BOOL *secure) {
    unsigned int i=0,start,end; unsigned int value=0; BOOL hasport=FALSE;
    host[0]=0;path[0]=0;*secure=FALSE;*port=80;
    if(wstarts_ci(url,L"https://")){*secure=TRUE;*port=443;i=8;}else if(wstarts_ci(url,L"http://")){i=7;}else return FALSE;
    start=i;while(url[i]&&url[i]!=L'/'&&url[i]!=L':'&&url[i]!=L'?'&&url[i]!=L'#'){
        if(url[i]==L' '||url[i]==L'\t'||url[i]==L'@')return FALSE;i++;
    }
    end=i;if(end<=start)return FALSE;wcopy_range(host,hcap,url,start,end);
    if(url[i]==L':'){
        i++;value=0;while(url[i]>=L'0'&&url[i]<=L'9'){hasport=TRUE;value=value*10+(url[i]-L'0');if(value>65535)return FALSE;i++;}
        if(!hasport||value==0)return FALSE;*port=(INTERNET_PORT)value;
        if(url[i]&&url[i]!=L'/'&&url[i]!=L'?'&&url[i]!=L'#')return FALSE;
    }
    if(url[i]==L'/')wcopy(path,pcap,url+i);
    else if(url[i]==L'?'){wcopy(path,pcap,L"/");wcat(path,pcap,url+i);}
    else wcopy(path,pcap,L"/");
    {int fragment=wfind_ci(path,L"#");if(fragment>=0)path[fragment]=0;}
    return host[0]!=0;
}

static void normalize_proxy_for_winhttp(LPCWSTR in, LPWSTR out, unsigned int cap) {
    if(!in){out[0]=0;return;} if(wstarts_ci(in,L"http://"))wcopy(out,cap,in+7);else if(wstarts_ci(in,L"https://"))wcopy(out,cap,in+8);else wcopy(out,cap,in);
}

static BOOL http_get_bytes(LPCWSTR url, LPCWSTR token, LPCWSTR proxy, BYTE *out, DWORD cap, DWORD *outlen, DWORD *status, DWORD *error) {
    WCHAR host[1024],path[4096],proxy_norm[1024],headers[12288]; INTERNET_PORT port; BOOL secure; HINTERNET session=0,connect=0,request=0; DWORD avail=0,got=0,total=0,sz=sizeof(DWORD),idx=0; BOOL ok=FALSE;
    *outlen=0;*status=0;*error=0;if(!url_parse_basic(url,host,1024,path,4096,&port,&secure)){*error=87;return FALSE;}
    normalize_proxy_for_winhttp(proxy,proxy_norm,1024);
    session=WinHttpOpen(L"ClaudeCodeManager/1.0",proxy_norm[0]?WINHTTP_ACCESS_TYPE_NAMED_PROXY:WINHTTP_ACCESS_TYPE_NO_PROXY,proxy_norm[0]?proxy_norm:0,0,0);
    if(!session){*error=GetLastError();goto done;} WinHttpSetTimeouts(session,8000,8000,15000,20000);
    connect=WinHttpConnect(session,host,port,0);if(!connect){*error=GetLastError();goto done;}
    request=WinHttpOpenRequest(connect,L"GET",path,0,0,0,secure?WINHTTP_FLAG_SECURE:0);if(!request){*error=GetLastError();goto done;}
    if(token&&token[0]){
        headers[0]=0;
        wcat(headers,12288,L"Authorization: Bearer "); wcat(headers,12288,token);
        wcat(headers,12288,L"\r\nx-api-key: "); wcat(headers,12288,token);
        wcat(headers,12288,L"\r\nx-gateway-access-key: "); wcat(headers,12288,token);
        wcat(headers,12288,L"\r\nanthropic-version: 2023-06-01\r\n");
        WinHttpAddRequestHeaders(request,headers,(DWORD)-1,WINHTTP_ADDREQ_FLAG_ADD|WINHTTP_ADDREQ_FLAG_REPLACE);
        secure_zero_w(headers,12288);
    }
    if(!WinHttpSendRequest(request,0,0,0,0,0,0)){*error=GetLastError();goto done;}if(!WinHttpReceiveResponse(request,0)){*error=GetLastError();goto done;}
    if(!WinHttpQueryHeaders(request,WINHTTP_QUERY_STATUS_CODE|WINHTTP_QUERY_FLAG_NUMBER,0,status,&sz,&idx))*status=0;
    while(total+1<cap){avail=0;if(!WinHttpQueryDataAvailable(request,&avail)){*error=GetLastError();goto done;}if(!avail)break;if(avail>cap-total-1)avail=cap-total-1;if(!WinHttpReadData(request,out+total,avail,&got)){*error=GetLastError();goto done;}if(!got)break;total+=got;}
    out[total]=0;*outlen=total;ok=TRUE;
done:
    if(request)WinHttpCloseHandle(request);if(connect)WinHttpCloseHandle(connect);if(session)WinHttpCloseHandle(session);secure_zero_w(proxy_norm,1024);return ok;
}

static void build_anthropic_messages_url(LPCWSTR base,LPWSTR out,unsigned int cap) {
    wcopy(out,cap,base);while(wlen(out)>0&&out[wlen(out)-1]==L'/')out[wlen(out)-1]=0;
    if(wends_ci(out,L"/v1"))wcat(out,cap,L"/messages");else wcat(out,cap,L"/v1/messages");
}

/*
 * Anthropic Messages 兼容性探活：发送合法的最小请求体，避免 OpenAI 风格网关
 * 对 "{}" 返回 401 误判为鉴权失败。max_tokens=1 + 单字符 prompt 是 Anthropic
 * 官方推荐的轻量级探测方式，不会触发任何实质性推理。
 */
static const char ccm_probe_body[] =
    "{\"max_tokens\":1,\"messages\":[{\"role\":\"user\",\"content\":\"ping\"}]}";
#define CCM_PROBE_BODY_LEN (int)(sizeof(ccm_probe_body) - 1)

static BOOL http_probe_anthropic_messages(LPCWSTR url,LPCWSTR token,LPCWSTR proxy,DWORD *status,DWORD *error) {
    WCHAR host[1024],path[4096],proxy_norm[1024],headers[12288];INTERNET_PORT port;BOOL secure;HINTERNET session=0,connect=0,request=0;DWORD sz=sizeof(DWORD),idx=0;BOOL ok=FALSE;
    *status=0;*error=0;if(!url_parse_basic(url,host,1024,path,4096,&port,&secure)){*error=87;return FALSE;}
    normalize_proxy_for_winhttp(proxy,proxy_norm,1024);
    session=WinHttpOpen(L"ClaudeCodeManager/1.0",proxy_norm[0]?WINHTTP_ACCESS_TYPE_NAMED_PROXY:WINHTTP_ACCESS_TYPE_NO_PROXY,proxy_norm[0]?proxy_norm:0,0,0);
    if(!session){*error=GetLastError();goto done;}WinHttpSetTimeouts(session,8000,8000,15000,20000);
    connect=WinHttpConnect(session,host,port,0);if(!connect){*error=GetLastError();goto done;}
    request=WinHttpOpenRequest(connect,L"POST",path,0,0,0,secure?WINHTTP_FLAG_SECURE:0);if(!request){*error=GetLastError();goto done;}
    headers[0]=0;wcat(headers,12288,L"Content-Type: application/json\r\nanthropic-version: 2023-06-01\r\n");
    if(token&&token[0]){wcat(headers,12288,L"Authorization: Bearer ");wcat(headers,12288,token);wcat(headers,12288,L"\r\nx-api-key: ");wcat(headers,12288,token);wcat(headers,12288,L"\r\nx-gateway-access-key: ");wcat(headers,12288,token);wcat(headers,12288,L"\r\n");}
    WinHttpAddRequestHeaders(request,headers,(DWORD)-1,WINHTTP_ADDREQ_FLAG_ADD|WINHTTP_ADDREQ_FLAG_REPLACE);secure_zero_w(headers,12288);
    if(!WinHttpSendRequest(request,0,(DWORD)-1,(PVOID)ccm_probe_body,CCM_PROBE_BODY_LEN,CCM_PROBE_BODY_LEN,0)){*error=GetLastError();goto done;}if(!WinHttpReceiveResponse(request,0)){*error=GetLastError();goto done;}
    if(!WinHttpQueryHeaders(request,WINHTTP_QUERY_STATUS_CODE|WINHTTP_QUERY_FLAG_NUMBER,0,status,&sz,&idx)){*error=GetLastError();goto done;}ok=TRUE;
done:
    if(request)WinHttpCloseHandle(request);if(connect)WinHttpCloseHandle(connect);if(session)WinHttpCloseHandle(session);secure_zero_w(proxy_norm,1024);return ok;
}

static BOOL http_status_success(DWORD status){return status>=200&&status<400;}

static void json_unescape_simple(LPCWSTR src, unsigned int start, unsigned int end, LPWSTR out, unsigned int cap) {
    unsigned int i=start,o=0;while(i<end&&o+1<cap){if(!src[i])break;WCHAR c=src[i++];if(c==L'\\'&&i<end){if(!src[i])break;WCHAR e=src[i++];if(e==L'n')c=L'\n';else if(e==L'r')c=L'\r';else if(e==L't')c=L'\t';else c=e;}out[o++]=c;}out[o]=0;
}

static BOOL json_key_match_at(LPCWSTR text,unsigned int pos,LPCWSTR key,unsigned int *value_start,unsigned int *value_end) {
    unsigned int i=pos,k=0; if(!text[i])return FALSE; if(text[i]!=L'\"')return FALSE;i++;while(key[k]&&text[i]&&text[i]==key[k]){i++;k++;}if(key[k]||!text[i]||text[i]!=L'\"')return FALSE;i++;while(text[i]&&(text[i]==L' '||text[i]==L'\t'||text[i]==L'\r'||text[i]==L'\n'))i++;if(!text[i]||text[i++]!=L':')return FALSE;while(text[i]&&(text[i]==L' '||text[i]==L'\t'||text[i]==L'\r'||text[i]==L'\n'))i++;if(!text[i]||text[i++]!=L'\"')return FALSE;*value_start=i;while(text[i]){if(text[i]==L'\\'&&text[i+1]){i+=2;continue;}if(text[i]==L'\"'){*value_end=i;return TRUE;}i++;}return FALSE;
}

static BOOL json_unsigned_key_at(LPCWSTR text,unsigned int pos,unsigned int limit,LPCWSTR key,DWORD *value) {
    unsigned int i=pos,k=0;unsigned long long n=0;BOOL quoted=FALSE,has_digit=FALSE;
    if(text[i]!=L'\"')return FALSE;i++;while(key[k]&&i<limit&&text[i]==key[k]){i++;k++;}if(key[k]||i>=limit||text[i]!=L'\"')return FALSE;
    i++;while(i<limit&&(text[i]==L' '||text[i]==L'\t'||text[i]==L'\r'||text[i]==L'\n'))i++;if(i>=limit||text[i++]!=L':')return FALSE;
    while(i<limit&&(text[i]==L' '||text[i]==L'\t'||text[i]==L'\r'||text[i]==L'\n'))i++;if(i<limit&&text[i]==L'\"'){quoted=TRUE;i++;}
    while(i<limit&&text[i]>=L'0'&&text[i]<=L'9'){has_digit=TRUE;n=n*10+(text[i++]-L'0');if(n>100000000)return FALSE;}
    if(!has_digit||(quoted&&(i>=limit||text[i]!=L'\"')))return FALSE;*value=(DWORD)n;return TRUE;
}

static DWORD json_context_capacity_for_object(LPCWSTR text,unsigned int member_pos) {
    unsigned int stack[64],i,start=0,end=0;int depth=0;BOOL in_string=FALSE,escaped=FALSE;DWORD value=0;
    for(i=0;text[i]&&i<=member_pos;i++){
        WCHAR c=text[i];if(in_string){if(escaped)escaped=FALSE;else if(c==L'\\')escaped=TRUE;else if(c==L'\"')in_string=FALSE;continue;}if(c==L'\"'){in_string=TRUE;continue;}if(c==L'{'){if(depth<64)stack[depth]=(unsigned int)i;depth++;}else if(c==L'}'&&depth>0)depth--;
    }
    if(depth<=0||depth>64)return 0;start=stack[depth-1];in_string=escaped=FALSE;depth=0;
    for(i=start;text[i];i++){
        WCHAR c=text[i];if(in_string){if(escaped)escaped=FALSE;else if(c==L'\\')escaped=TRUE;else if(c==L'\"')in_string=FALSE;continue;}if(c==L'\"'){in_string=TRUE;continue;}if(c==L'{')depth++;else if(c==L'}'&&--depth==0){end=i+1;break;}
    }
    if(!end)return 0;
    for(i=start;i<end;i++)if(json_unsigned_key_at(text,i,end,L"context_window",&value)||json_unsigned_key_at(text,i,end,L"context_length",&value)||json_unsigned_key_at(text,i,end,L"max_context_length",&value)||json_unsigned_key_at(text,i,end,L"input_token_limit",&value))return value;
    return 0;
}

static void parse_models_from_http(BYTE *bytes,DWORD len) {
    int chars; unsigned int i,vs,ve; WCHAR id[512];DWORD capacity;
    g_model_count=0;memset(g_model_context_capacity,0,sizeof(g_model_context_capacity));chars=MultiByteToWideChar(CP_UTF8,0,(LPCSTR)bytes,(int)len,g_model_result_text,131071);if(chars<=0)chars=MultiByteToWideChar(CP_ACP,0,(LPCSTR)bytes,(int)len,g_model_result_text,131071);if(chars<=0){g_model_result_text[0]=0;return;}g_model_result_text[chars]=0;
    for(i=0;g_model_result_text[i];i++){
        if(json_key_match_at(g_model_result_text,i,L"id",&vs,&ve)||json_key_match_at(g_model_result_text,i,L"name",&vs,&ve)){json_unescape_simple(g_model_result_text,vs,ve,id,512);capacity=json_context_capacity_for_object(g_model_result_text,i);if(id[0])model_add_unique(id,capacity);i=ve;}
    }
    if(g_model_count==0){i=0;while(g_model_result_text[i]&&(g_model_result_text[i]==L' '||g_model_result_text[i]==L'\r'||g_model_result_text[i]==L'\n'||g_model_result_text[i]==L'\t'))i++;if(g_model_result_text[i]==L'['){i++;while(g_model_result_text[i]){while(g_model_result_text[i]&&g_model_result_text[i]!=L'\"'&&g_model_result_text[i]!=L']')i++;if(!g_model_result_text[i])break;if(g_model_result_text[i]==L']')break;vs=++i;while(g_model_result_text[i]){if(g_model_result_text[i]==L'\\'&&g_model_result_text[i+1]){i+=2;continue;}if(g_model_result_text[i]==L'\"')break;i++;}ve=i;json_unescape_simple(g_model_result_text,vs,ve,id,512);if(id[0])model_add_unique(id,0);if(g_model_result_text[i])i++;}}}
}

static DWORD __stdcall network_test_thread(PVOID unused) {
    WCHAR desc[256],masked[1024],errtxt[32];DWORD status=0,error=0,len=0;(void)unused;
    resolve_effective_proxy(g_effective_proxy,1024,desc,256);
    append_log(L"");append_log(L"[INFO] 正在检测网络连接...");mask_proxy(g_effective_proxy,masked,1024);append_log_raw(L"[INFO] 网络模式：");append_log(desc);if(masked[0]){append_log_raw(L"[INFO] 代理：");append_log(masked);}else append_log(L"[INFO] 连接方式：直连");
    if(http_get_bytes(g_cfg_url,0,g_effective_proxy,g_http_bytes,sizeof(g_http_bytes),&len,&status,&error)&&http_status_success(status)){
        wcopy(g_network_status,512,L"连接正常 · ");wcat(g_network_status,512,desc);if(network_mode_index()==0&&g_effective_proxy[0]){wcopy(g_cfg_last_auto_proxy,1024,g_effective_proxy);save_config();}append_log(L"[OK] 网络连接正常。");set_status(L"网络连接正常");error=0;
    }else{
        wcopy(g_network_status,512,L"连接失败 · ");wcat(g_network_status,512,desc);if(status){uint_to_wstr(status,errtxt,32);append_log_raw(L"[ERROR] HTTP 状态码：");append_log(errtxt);}else{uint_to_wstr(error,errtxt,32);append_log_raw(L"[ERROR] 网络错误：");append_log(errtxt);}set_status(L"网络连接失败");
    }
    secure_zero_w(g_effective_proxy,1024);
    g_network_busy=0;
    /* Post completion message to main thread for UI updates */
    if(g_main) pPostMessageW(g_main, WM_APP_NETWORK_TEST_COMPLETE, (WPARAM)(error?error:status), 0);
    return error?error:status;
}

static void start_network_test(void){
    HANDLE th;DWORD tid;
    if(g_installing||g_network_busy)return;
    pGetWindowTextW(g_url,g_cfg_url,2048);pGetWindowTextW(g_proxy,g_cfg_proxy,1024);save_config();
    g_network_busy=1;if(g_test_network)pEnableWindow(g_test_network,FALSE);wcopy(g_network_status,512,L"正在检测连接……");set_status(L"正在检测网络连接");
    th=CreateThread(0,0,network_test_thread,0,0,&tid);
    if(th)CloseHandle(th);else{g_network_busy=0;if(g_test_network)pEnableWindow(g_test_network,TRUE);wcopy(g_network_status,512,L"无法启动网络检测");set_status(L"网络检测未启动");pMessageBoxW(g_main,L"无法启动网络检测线程。",L"ClaudeCodeManager",MB_ICONERROR);}
}

static void set_proxy_environment(LPCWSTR proxy) {
    LPCWSTR value = (proxy && proxy[0]) ? proxy : 0;
    SetEnvironmentVariableW(L"http_proxy", value);
    SetEnvironmentVariableW(L"https_proxy", value);
    SetEnvironmentVariableW(L"HTTP_PROXY", value);
    SetEnvironmentVariableW(L"HTTPS_PROXY", value);
    SetEnvironmentVariableW(L"no_proxy", L"localhost,127.0.0.1,::1");
    SetEnvironmentVariableW(L"NO_PROXY", L"localhost,127.0.0.1,::1");
}


static BOOL file_exists_regular(LPCWSTR path) {
    DWORD attr;
    if(!path || !path[0]) return FALSE;
    attr=GetFileAttributesW(path);
    return attr!=0xFFFFFFFFu && !(attr & FILE_ATTRIBUTE_DIRECTORY);
}

static void dirname_from_path(LPCWSTR path, LPWSTR out, unsigned int cap) {
    int i;
    wcopy(out,cap,path);
    i=(int)wlen(out)-1;
    while(i>=0 && out[i]!=L'\\' && out[i]!=L'/') i--;
    if(i>=0) out[i]=0; else out[0]=0;
}

static BOOL ensure_executable_dir_in_path(LPCWSTR exe_path) {
    WCHAR dir[4096];
    DWORD n;
    dirname_from_path(exe_path,dir,4096);
    if(!dir[0]) return FALSE;
    g_path_buffer2[0]=0;
    n=GetEnvironmentVariableW(L"PATH",g_path_buffer2,sizeof(g_path_buffer2)/2);
    if(n>=sizeof(g_path_buffer2)/2) return FALSE;
    if(path_contains_entry(g_path_buffer2,dir)) return TRUE;
    g_path_buffer[0]=0;
    wcat(g_path_buffer,sizeof(g_path_buffer)/2,dir);
    if(g_path_buffer2[0]) { wcat(g_path_buffer,sizeof(g_path_buffer)/2,L";"); wcat(g_path_buffer,sizeof(g_path_buffer)/2,g_path_buffer2); }
    return SetEnvironmentVariableW(L"PATH",g_path_buffer);
}

static BOOL locate_curl(LPWSTR out, unsigned int cap) {
    WCHAR root[4096];
    out[0]=0;
    if(SearchPathW(0,L"curl.exe",0,cap,out,0) && file_exists_regular(out)) {
        ensure_executable_dir_in_path(out);
        return TRUE;
    }
    root[0]=0;
    GetEnvironmentVariableW(L"SystemRoot",root,4096);
    if(!root[0]) wcopy(root,4096,L"C:\\Windows");
    wcopy(out,cap,root);
    if(out[wlen(out)-1]!=L'\\') wcat(out,cap,L"\\");
    wcat(out,cap,L"System32\\curl.exe");
    if(file_exists_regular(out)) {
        ensure_executable_dir_in_path(out);
        return TRUE;
    }
    out[0]=0;
    return FALSE;
}

static BOOL locate_powershell(LPWSTR out, unsigned int cap) {
    WCHAR root[4096];
    out[0]=0;
    if(SearchPathW(0,L"powershell.exe",0,cap,out,0) && file_exists_regular(out)) return TRUE;
    root[0]=0;
    GetEnvironmentVariableW(L"SystemRoot",root,4096);
    if(!root[0]) wcopy(root,4096,L"C:\\Windows");
    wcopy(out,cap,root);
    if(out[wlen(out)-1]!=L'\\') wcat(out,cap,L"\\");
    wcat(out,cap,L"System32\\WindowsPowerShell\\v1.0\\powershell.exe");
    if(file_exists_regular(out)) return TRUE;
    out[0]=0;
    return FALSE;
}

static DWORD __stdcall update_thread(PVOID unused) {
    DWORD code;
    WCHAR command[8192], version_command[8192], desc[256], masked[1024];
    (void)unused;
    set_busy(TRUE);
    set_phase(1,12,L"检查当前安装");
    set_status(L"正在检查 Claude Code 更新...");
    pSetWindowTextW(g_log,L"");
    append_log(L"========================================");
    append_log(L"Claude Code 检查并更新");
    append_log(L"========================================");
    append_log(L"");
    if(!locate_claude(g_claude_exe,4096)) {
        append_log(L"[WARN] 未检测到 Claude Code，将切换到首次安装流程。");
        set_busy(FALSE);
        set_proxy_environment(0); /* clean any env from a partial resolve */
        secure_zero_w(g_effective_proxy,1024);
        return install_thread(0);
    }
    pGetWindowTextW(g_proxy,g_cfg_proxy,1024);
    resolve_effective_proxy(g_effective_proxy,1024,desc,256);
    set_proxy_environment(g_effective_proxy);
    mask_proxy(g_effective_proxy,masked,1024);
    append_log_raw(L"[INFO] 网络模式："); append_log(desc);
    if(masked[0]) { append_log_raw(L"[INFO] Proxy: "); append_log(masked); }
    else append_log(L"[INFO] 使用直连。");
    append_log(L"");
    set_phase(2,25,L"读取当前版本");
    append_log(L"[1/3] 当前版本：");
    version_command[0]=0; wcat(version_command,8192,L"\""); wcat(version_command,8192,g_claude_exe); wcat(version_command,8192,L"\" --version");
    run_process_capture(version_command,0,FALSE,L"Claude");
    set_phase(3,55,L"检查并安装更新");
    append_log(L""); append_log(L"[2/3] 正在调用 Claude Code 官方更新命令...");
    command[0]=0; wcat(command,8192,L"\""); wcat(command,8192,g_claude_exe); wcat(command,8192,L"\" update");
    code=run_process_capture(command,0,FALSE,L"Claude 更新程序");
    if(code!=0) {
        WCHAR n[32]; uint_to_wstr(code,n,32);
        append_log_raw(L"[ERROR] 更新命令返回代码："); append_log(n);
        append_log(L"[INFO] 如安装文件损坏，可使用安装来源重新安装进行修复。");
        set_status(L"Claude Code 更新失败");
        set_proxy_environment(0); secure_zero_w(g_effective_proxy,1024); set_busy(FALSE); update_claude_state();
        return code;
    }
    set_phase(4,88,L"验证更新结果");
    append_log(L""); append_log(L"[3/3] 更新后的版本：");
    version_command[0]=0; wcat(version_command,8192,L"\""); wcat(version_command,8192,g_claude_exe); wcat(version_command,8192,L"\" --version");
    code=run_process_capture(version_command,0,FALSE,L"Claude");
    if(code==0) {
        append_log(L""); append_log(L"========================================"); append_log(L"Claude Code 检查并更新完成"); append_log(L"========================================");
        set_phase(5,100,L"更新完成"); set_status(L"Claude Code 已检查 / 更新完成");
    } else {
        append_log(L"[WARN] 更新命令已结束，但版本验证未通过。"); set_status(L"更新完成，但版本验证异常");
    }
    set_proxy_environment(0); secure_zero_w(g_effective_proxy,1024); set_busy(FALSE); update_claude_state();
    return code;
}

static DWORD __stdcall install_thread(PVOID unused) {
    DWORD code;
    DWORD temp_len;
    DWORD http_len=0, http_status=0, http_error=0;
    WCHAR pid[32];
    WCHAR version_cmd[8192];
    WCHAR comspec[4096];
    WCHAR curl_path[4096];
    WCHAR powershell_path[4096];
    WCHAR installer_url[2048];
    BOOL use_cmd_installer=FALSE;
    (void)unused;
    set_busy(TRUE);
    set_phase(1, 6, L"检查运行环境");
    set_status(L"正在安装 Claude Code...");
    pSetWindowTextW(g_log, L"");
    append_log(L"========================================");
    append_log(L"Claude Code 首次安装");
    append_log(L"========================================");
    append_log(L"");

    if (contains_quote(g_cfg_proxy) || contains_quote(g_cfg_url)) {
        append_log(L"[ERROR] 代理地址或安装地址不能包含双引号。");
        set_status(L"配置无效");
        set_busy(FALSE);
        return 1;
    }

    set_phase(1, 8, L"检查安装组件");
    append_log(L"[1/5] 检查安装组件...");
    append_log(L"[OK] WinHTTP 可用，将由管理器下载安装入口脚本。");
    curl_path[0]=0; powershell_path[0]=0;
    if(locate_curl(curl_path,4096)) {
        use_cmd_installer=TRUE;
        append_log_raw(L"[OK] 已找到 curl.exe："); append_log(curl_path);
        append_log(L"[INFO] 将使用 Claude Code 官方 Windows CMD 安装方式。");
    } else if(locate_powershell(powershell_path,4096)) {
        use_cmd_installer=FALSE;
        append_log(L"[WARN] 未找到 curl.exe，已自动切换为 Claude Code 官方 PowerShell 安装方式。");
    } else {
        append_log(L"[ERROR] 系统中既未找到 curl.exe，也未找到 Windows PowerShell。");
        append_log(L"[INFO] 请安装 Windows 10 1809 或更高版本，或恢复系统 PowerShell 组件。");
        set_status(L"安装失败：缺少安装组件");
        set_busy(FALSE);
        return 10;
    }
    append_log(L"");

    set_phase(2, 18, L"配置网络环境");
    append_log(L"[2/5] 配置本次进程网络环境...");
    {
        WCHAR desc[256], masked[1024];
        resolve_effective_proxy(g_effective_proxy,1024,desc,256);
        set_proxy_environment(g_effective_proxy);
        mask_proxy(g_effective_proxy,masked,1024);
        append_log_raw(L"[INFO] 网络模式："); append_log(desc);
        if(masked[0]) { append_log_raw(L"[INFO] Proxy: "); append_log(masked); }
        else append_log(L"[INFO] 使用直连。");
    }
    append_log(L"");

    temp_len = GetTempPathW(4096, g_temp_installer);
    if (!temp_len || temp_len >= 4000) {
        append_log(L"[ERROR] 无法获取临时目录。");
        set_status(L"安装失败：临时目录异常");
        set_proxy_environment(0); secure_zero_w(g_effective_proxy,1024); set_busy(FALSE);
        return 20;
    }
    uint_to_wstr(GetCurrentProcessId(), pid, 32);
    wcat(g_temp_installer, 4096, L"claude-install-");
    wcat(g_temp_installer, 4096, pid);
    wcat(g_temp_installer, 4096, use_cmd_installer ? L".cmd" : L".ps1");

    if(use_cmd_installer) {
        wcopy(installer_url,2048,g_cfg_url);
    } else {
        if(wends_ci(g_cfg_url,L".cmd")) {
            unsigned int n=wlen(g_cfg_url)-4;
            wcopy_range(installer_url,2048,g_cfg_url,0,n);
            wcat(installer_url,2048,L".ps1");
        } else {
            wcopy(installer_url,2048,L"https://claude.ai/install.ps1");
        }
    }

    set_phase(3, 30, L"下载官方安装脚本");
    append_log(L"[3/5] 使用 Windows 原生网络组件下载安装脚本...");
    append_log_raw(L"[INFO] URL: "); append_log(installer_url);
    if(!http_get_bytes(installer_url,0,g_effective_proxy,g_http_bytes,sizeof(g_http_bytes),&http_len,&http_status,&http_error) || !http_status_success(http_status)) {
        WCHAR num[32];
        if(http_status){uint_to_wstr(http_status,num,32);append_log_raw(L"[ERROR] 下载失败，HTTP 状态码：");append_log(num);} else {uint_to_wstr(http_error,num,32);append_log_raw(L"[ERROR] 下载失败，系统错误：");append_log(num);}
        DeleteFileW(g_temp_installer);
        set_status(L"安装失败：下载安装脚本异常");
        set_proxy_environment(0); secure_zero_w(g_effective_proxy,1024); set_busy(FALSE);
        return 21;
    }
    if(http_len<100 || !write_bytes_file(g_temp_installer,g_http_bytes,http_len)) {
        append_log(L"[ERROR] 安装脚本内容异常或无法写入临时文件。");
        DeleteFileW(g_temp_installer);
        set_status(L"安装失败：安装脚本异常");
        set_proxy_environment(0); secure_zero_w(g_effective_proxy,1024); set_busy(FALSE);
        return 22;
    }
    set_phase(3, 45, L"安装脚本下载完成");
    append_log(L"[OK] 安装脚本下载完成。");
    append_log(L"");

    set_phase(4, 55, L"执行官方安装器");
    append_log(L"[4/5] 执行官方安装器...");
    append_log(L"[INFO] 下载、校验或解压期间可能暂时无输出。");
    append_log(L"[INFO] 安装耗时显示在界面状态栏，日志仅保留关键输出。");
    g_cmdline[0] = 0;
    if(use_cmd_installer) {
        comspec[0]=0; GetEnvironmentVariableW(L"ComSpec",comspec,4096); if(!comspec[0]) wcopy(comspec,4096,L"C:\\Windows\\System32\\cmd.exe");
        wcat(g_cmdline, 32768, L"\""); wcat(g_cmdline,32768,comspec); wcat(g_cmdline,32768,L"\" /d /q /c call \"");
        wcat(g_cmdline, 32768, g_temp_installer);
        wcat(g_cmdline, 32768, L"\"");
    } else {
        wcat(g_cmdline,32768,L"\""); wcat(g_cmdline,32768,powershell_path); wcat(g_cmdline,32768,L"\" -NoLogo -NoProfile -NonInteractive -ExecutionPolicy Bypass -File \"");
        wcat(g_cmdline,32768,g_temp_installer); wcat(g_cmdline,32768,L"\"");
    }
    code = run_process_capture(g_cmdline, 0, FALSE, L"Claude 安装器");
    DeleteFileW(g_temp_installer);
    if (code != 0) {
        WCHAR num[32]; uint_to_wstr(code, num, 32);
        append_log_raw(L"[ERROR] 安装器返回代码: "); append_log(num);
        if(use_cmd_installer && code==1) append_log(L"[INFO] 若日志仍提示 curl 不可用，请确认系统 curl.exe 未被安全软件隔离。");
        set_status(L"安装失败：官方安装器报错");
        set_proxy_environment(0); secure_zero_w(g_effective_proxy,1024); set_busy(FALSE);
        return 30;
    }
    append_log(L"[OK] 官方安装器执行完成。");
    append_log(L"");

    set_phase(5, 88, L"验证安装并更新 PATH");
    append_log(L"[5/5] 验证 Claude Code 并更新 PATH...");
    if (!locate_claude(g_claude_exe, 4096)) {
        append_log(L"[ERROR] 安装完成后仍未找到 claude.exe。");
        set_status(L"安装失败：未找到 claude.exe");
        set_proxy_environment(0); secure_zero_w(g_effective_proxy,1024); set_busy(FALSE);
        return 31;
    }
    if (ensure_user_path()) append_log(L"[OK] 用户 PATH 已检查 / 更新。");
    else append_log(L"[WARN] 无法更新用户 PATH，但安装文件已存在。");
    version_cmd[0] = 0;
    wcat(version_cmd, 8192, L"\""); wcat(version_cmd, 8192, g_claude_exe); wcat(version_cmd, 8192, L"\" --version");
    code = run_process_capture(version_cmd, 0, FALSE, L"Claude");
    if (code != 0) {
        append_log(L"[WARN] claude.exe 已存在，但版本验证未通过。");
        set_status(L"安装完成，但版本验证异常");
    } else {
        append_log(L"");
        append_log(L"========================================");
        append_log(L"Claude Code 安装完成");
        append_log(L"========================================");
        update_claude_state();
        set_phase(6, 100, L"安装完成");
        set_status(L"Claude Code 已安装");
    }
    set_proxy_environment(0);
    secure_zero_w(g_effective_proxy,1024);
    set_busy(FALSE);
    return code;
}

static void start_install(void) {
    HANDLE thread;
    DWORD tid;
    LPTHREAD_START_ROUTINE worker;
    WCHAR parsed_host[1024],parsed_path[4096];INTERNET_PORT parsed_port;BOOL parsed_secure;
    if (g_installing) return;
    pGetWindowTextW(g_proxy,g_cfg_proxy,1024);
    pGetWindowTextW(g_url,g_cfg_url,2048);
    pGetWindowTextW(g_workspace,g_cfg_workspace,4096);
    update_claude_state();
    if(!g_claude_installed && wblank(g_cfg_url)) {
        wcopy(g_cfg_url,2048,L"https://claude.ai/install.cmd");
        pSetWindowTextW(g_url,g_cfg_url);
        append_log(L"[INFO] 安装来源为空，已恢复为 Claude 官方地址。");
    }
    if(!g_claude_installed&&!url_parse_basic(g_cfg_url,parsed_host,1024,parsed_path,4096,&parsed_port,&parsed_secure)) {
        pMessageBoxW(g_main,L"安装来源无效。请填写以 http:// 或 https:// 开头的完整地址。",L"地址格式不正确",MB_ICONWARNING);
        set_status(L"安装来源格式不正确");return;
    }
    save_config();
    worker=g_claude_installed?update_thread:install_thread;
    set_busy(TRUE);
    thread=CreateThread(0,0,worker,0,0,&tid);
    if(!thread) {
        set_busy(FALSE);
        pMessageBoxW(g_main,g_claude_installed?L"无法启动更新线程。":L"无法启动安装线程。",L"ClaudeCodeManager",MB_ICONERROR);
        return;
    }
    CloseHandle(thread);
}


static void json_append_quoted(LPWSTR dst, unsigned int cap, LPCWSTR src) {
    WCHAR one[3]; unsigned int i=0; one[1]=0; one[2]=0;
    wcat(dst,cap,L"\"");
    while(src && src[i]) {
        WCHAR c=src[i++];
        if(c==L'\"' || c==L'\\') { one[0]=L'\\'; wcat(dst,cap,one); one[0]=c; wcat(dst,cap,one); }
        else if(c==L'\r') wcat(dst,cap,L"\\r");
        else if(c==L'\n') wcat(dst,cap,L"\\n");
        else if(c==L'\t') wcat(dst,cap,L"\\t");
        else { one[0]=c; wcat(dst,cap,one); }
    }
    wcat(dst,cap,L"\"");
}

static void json_add_pair(LPWSTR dst, unsigned int cap, LPCWSTR name, LPCWSTR value, BOOL *first, int indent) {
    int i;
    if(!value || !value[0]) return;
    if(!*first) wcat(dst,cap,L",\r\n");
    for(i=0;i<indent;i++) wcat(dst,cap,L" ");
    json_append_quoted(dst,cap,name); wcat(dst,cap,L": "); json_append_quoted(dst,cap,value);
    *first=FALSE;
}

static int model_add_unique(LPCWSTR id,DWORD context_capacity) {
    int i;
    if(!id||!id[0]) return -1;
    for(i=0;i<g_model_count;i++)if(weq_ci(g_models[i],id)){if(!g_model_context_capacity[i]&&context_capacity)g_model_context_capacity[i]=context_capacity;return i;}
    if(g_model_count>=128)return -1;
    wcopy(g_models[g_model_count],256,id);g_model_context_capacity[g_model_count]=context_capacity;return g_model_count++;
}

static void apply_discovered_context_capacity(int role) {
    WCHAR model[1024],current[64],number[32];int i;HWND combo=model_combo_for_role(role);
    if(role<0||role>=MODEL_ROLE_COUNT||!combo||!g_wiz_context_capacity[role])return;
    current[0]=0;pGetWindowTextW(g_wiz_context_capacity[role],current,64);if(!wblank(current))return;
    model[0]=0;pGetWindowTextW(combo,model,1024);if(!model[0])return;
    for(i=0;i<g_model_count;i++)if(weq_ci(model,g_models[i])&&g_model_context_capacity[i]){uint_to_wstr(g_model_context_capacity[i],number,32);pSetWindowTextW(g_wiz_context_capacity[role],number);return;}
}

static void populate_one_model_combo(HWND combo) {
    WCHAR current[1024]; int i;
    if(!combo) return;
    current[0]=0; pGetWindowTextW(combo,current,1024);
    pSendMessageW(combo,CB_RESETCONTENT,0,0);
    for(i=0;i<g_model_count;i++) pSendMessageW(combo,CB_ADDSTRING,0,(LPARAM)g_models[i]);
    if(current[0]) pSetWindowTextW(combo,current);
}

static LPCWSTR find_model_role(LPCWSTR keyword) {
    int i; for(i=0;i<g_model_count;i++) if(wfind_ci(g_models[i],keyword)>=0) return g_models[i];
    return 0;
}

static void populate_model_combos(void) {
    WCHAR text[1024];int role;LPCWSTR opus=find_model_role(L"opus"),sonnet=find_model_role(L"sonnet"),haiku=find_model_role(L"haiku");
    populate_one_model_combo(g_wiz_default_model); populate_one_model_combo(g_wiz_opus_model); populate_one_model_combo(g_wiz_sonnet_model); populate_one_model_combo(g_wiz_haiku_model); populate_one_model_combo(g_wiz_subagent_model);
    text[0]=0;pGetWindowTextW(g_wiz_default_model,text,1024);if(!text[0]&&g_model_count>0)pSetWindowTextW(g_wiz_default_model,sonnet?sonnet:g_models[0]);
    text[0]=0;pGetWindowTextW(g_wiz_opus_model,text,1024);if(!text[0]&&opus)pSetWindowTextW(g_wiz_opus_model,opus);
    text[0]=0;pGetWindowTextW(g_wiz_sonnet_model,text,1024);if(!text[0]&&sonnet)pSetWindowTextW(g_wiz_sonnet_model,sonnet);
    text[0]=0;pGetWindowTextW(g_wiz_haiku_model,text,1024);if(!text[0]&&haiku)pSetWindowTextW(g_wiz_haiku_model,haiku);
    text[0]=0;pGetWindowTextW(g_wiz_subagent_model,text,1024);if(!text[0]&&haiku)pSetWindowTextW(g_wiz_subagent_model,haiku);
    for(role=0;role<MODEL_ROLE_COUNT;role++)apply_discovered_context_capacity(role);
}

static HWND model_combo_for_role(int role) {
    if(role==0)return g_wiz_default_model;if(role==1)return g_wiz_opus_model;if(role==2)return g_wiz_sonnet_model;if(role==3)return g_wiz_haiku_model;return g_wiz_subagent_model;
}

static void metadata_field(LPCWSTR text,int wanted,LPWSTR out,unsigned int cap) {
    int field=0;unsigned int i=0,o=0;out[0]=0;if(!text)return;
    while(text[i]&&field<wanted){if(text[i++]==L'|')field++;}
    if(field!=wanted)return;
    while(text[i]&&text[i]!=L'|'&&o+1<cap)out[o++]=text[i++];out[o]=0;
}

static int effort_index_from_code(LPCWSTR code) {
    if(weq_ci(code,L"low"))return 0;if(weq_ci(code,L"medium"))return 1;if(weq_ci(code,L"xhigh"))return 3;return 2;
}

static LPCWSTR effort_code_from_index(int index) {
    if(index==0)return L"low";if(index==1)return L"medium";if(index==3)return L"xhigh";return L"high";
}

static void set_context_metadata_defaults(void) {
    int i;
    for(i=0;i<MODEL_ROLE_COUNT;i++){
        pSetWindowTextW(g_wiz_context_capacity[i],L"");pSetWindowTextW(g_wiz_context_budget[i],L"");pSetWindowTextW(g_wiz_context_warning[i],L"80");pSetWindowTextW(g_wiz_context_reserve[i],L"8192");
        pSendMessageW(g_wiz_context_effort[i],CB_SETCURSEL,(WPARAM)((i>=3)?1:2),0);
    }
}

static void load_context_metadata_controls(void) {
    HKEY key=0;WCHAR data[512],field[64];int i;
    set_context_metadata_defaults();
    if(pRegOpenKeyExW(HKEY_CURRENT_USER,REG_KEY,0,KEY_READ,&key)!=ERROR_SUCCESS)return;
    for(i=0;i<MODEL_ROLE_COUNT;i++){
        data[0]=0;if(!reg_read_string(key,MODEL_META_VALUE_NAMES[i],data,512))continue;
        metadata_field(data,0,field,64);pSetWindowTextW(g_wiz_context_capacity[i],field);
        metadata_field(data,1,field,64);pSetWindowTextW(g_wiz_context_budget[i],field);
        metadata_field(data,2,field,64);if(field[0])pSetWindowTextW(g_wiz_context_warning[i],field);
        metadata_field(data,3,field,64);if(field[0])pSetWindowTextW(g_wiz_context_reserve[i],field);
        metadata_field(data,4,field,64);pSendMessageW(g_wiz_context_effort[i],CB_SETCURSEL,(WPARAM)effort_index_from_code(field),0);
    }
    pRegCloseKey(key);
}

static BOOL parse_context_number(LPCWSTR text,BOOL optional,DWORD *value,BOOL *present) {
    unsigned long long n=0;unsigned int i=0;if(present)*present=FALSE;if(value)*value=0;
    if(wblank(text))return optional;
    while(text[i]){if(text[i]<L'0'||text[i]>L'9')return FALSE;n=n*10+(text[i]-L'0');if(n>100000000)return FALSE;i++;}
    if(present)*present=TRUE;if(value)*value=(DWORD)n;return TRUE;
}

static BOOL save_context_metadata_controls(void) {
    HKEY key=0;DWORD disp=0,capacity,budget,warning,reserve,bytes;BOOL has_capacity,has_budget;WCHAR c[64],b[64],w[64],r[64],data[512],message[256];int i,effort;BOOL ok=TRUE;
    for(i=0;i<MODEL_ROLE_COUNT;i++){
        pGetWindowTextW(g_wiz_context_capacity[i],c,64);pGetWindowTextW(g_wiz_context_budget[i],b,64);pGetWindowTextW(g_wiz_context_warning[i],w,64);pGetWindowTextW(g_wiz_context_reserve[i],r,64);
        if(!parse_context_number(c,TRUE,&capacity,&has_capacity)||!parse_context_number(b,TRUE,&budget,&has_budget)||!parse_context_number(w,FALSE,&warning,0)||!parse_context_number(r,FALSE,&reserve,0)||warning<50||warning>95){
            message[0]=0;wcat(message,256,MODEL_ROLE_LABELS[i]);wcat(message,256,L" 的上下文参数无效。容量/预算可留空；预警阈值应为 50–95，其他字段请输入数字。");pMessageBoxW(g_wizard,message,L"上下文参数不正确",MB_ICONWARNING);return FALSE;
        }
        if(has_capacity&&((has_budget&&(ULONGLONG)budget+reserve>capacity)||reserve>=capacity)){
            message[0]=0;wcat(message,256,MODEL_ROLE_LABELS[i]);wcat(message,256,L" 的建议预算与输出预留超过了标注容量。请降低预算/预留，或修正容量。");pMessageBoxW(g_wizard,message,L"软预算超出容量",MB_ICONWARNING);return FALSE;
        }
    }
    if(pRegCreateKeyExW(HKEY_CURRENT_USER,REG_KEY,0,0,0,KEY_READ|KEY_WRITE,0,&key,&disp)!=ERROR_SUCCESS){pMessageBoxW(g_wizard,L"无法打开 Manager 本地配置存储。",L"保存失败",MB_ICONERROR);return FALSE;}
    for(i=0;i<MODEL_ROLE_COUNT;i++){
        c[0]=b[0]=w[0]=r[0]=0;pGetWindowTextW(g_wiz_context_capacity[i],c,64);pGetWindowTextW(g_wiz_context_budget[i],b,64);pGetWindowTextW(g_wiz_context_warning[i],w,64);pGetWindowTextW(g_wiz_context_reserve[i],r,64);effort=(int)pSendMessageW(g_wiz_context_effort[i],CB_GETCURSEL,0,0);
        data[0]=0;wcat(data,512,c);wcat(data,512,L"|");wcat(data,512,b);wcat(data,512,L"|");wcat(data,512,w);wcat(data,512,L"|");wcat(data,512,r);wcat(data,512,L"|");wcat(data,512,effort_code_from_index(effort));bytes=(wlen(data)+1)*2;
        if(pRegSetValueExW(key,MODEL_META_VALUE_NAMES[i],0,REG_SZ,(const BYTE*)data,bytes)!=ERROR_SUCCESS)ok=FALSE;
    }
    pRegCloseKey(key);if(!ok)pMessageBoxW(g_wizard,L"部分上下文预算元数据未能保存。settings.json 尚未修改。",L"保存失败",MB_ICONERROR);return ok;
}

static void append_context_budget_summary(void) {
    HKEY key=0;WCHAR data[512],capacity[64],budget[64],warning[64],reserve[64],line[512];
    data[0]=0;if(pRegOpenKeyExW(HKEY_CURRENT_USER,REG_KEY,0,KEY_READ,&key)!=ERROR_SUCCESS)return;
    reg_read_string(key,MODEL_META_VALUE_NAMES[0],data,512);pRegCloseKey(key);if(!data[0])return;
    metadata_field(data,0,capacity,64);metadata_field(data,1,budget,64);metadata_field(data,2,warning,64);metadata_field(data,3,reserve,64);
    line[0]=0;wcat(line,512,L"[INFO] 主力模型软预算（仅提示，不改写 CLI 请求）：容量 ");wcat(line,512,capacity[0]?capacity:L"自动/未知");wcat(line,512,L"，建议预算 ");wcat(line,512,budget[0]?budget:L"未设置");wcat(line,512,L"，预警 ");wcat(line,512,warning[0]?warning:L"80");wcat(line,512,L"%，输出预留 ");wcat(line,512,reserve[0]?reserve:L"8192");append_log(line);
}

static DWORD __stdcall model_discovery_thread(PVOID unused) {
    WCHAR base[2048],messages_url[4096],url1[4096],url2[4096],proxy[1024],desc[256],num[32];DWORD status=0,error=0,len=0;int attempt;BOOL reachable=FALSE,authfail=FALSE,compatible=FALSE;(void)unused;
    g_model_gateway_protocol_state=0;
    wcopy(base,2048,g_model_fetch_base);while(wlen(base)>0&&base[wlen(base)-1]==L'/')base[wlen(base)-1]=0;
    if(wlen(base)>=3&&weq_ci(base+wlen(base)-3,L"/v1")){wcopy(url1,4096,base);wcat(url1,4096,L"/models");url2[0]=0;}else{wcopy(url1,4096,base);wcat(url1,4096,L"/v1/models");wcopy(url2,4096,base);wcat(url2,4096,L"/models");}
    pGetWindowTextW(g_proxy,g_cfg_proxy,1024);resolve_effective_proxy(proxy,1024,desc,256);
    build_anthropic_messages_url(base,messages_url,4096);
    if(!http_probe_anthropic_messages(messages_url,g_model_fetch_secret,proxy,&status,&error)){
        wcopy(g_model_status,512,L"无法检测 Claude Code 所需的 Messages 接口，请检查网络和代理。");uint_to_wstr(error,num,32);append_log_raw(L"[ERROR] Anthropic Messages 兼容性检测失败，网络错误：");append_log(num);goto done;
    }
    reachable=TRUE;
    if(status==401||status==403){authfail=TRUE;wcopy(g_model_status,512,L"Messages 接口已响应，但访问密钥未通过验证。");append_log(L"[ERROR] Anthropic Messages 接口拒绝了访问密钥。");goto done;}
    if(status==404||status==405){g_model_gateway_protocol_state=2;wcopy(g_model_status,512,L"不兼容 Claude Code：服务器缺少 Anthropic /v1/messages 接口。");append_log(L"[ERROR] 服务器可列模型但不提供 Anthropic Messages 接口，不能用于原生 Claude Code。");goto done;}
    if(status>=200&&status<300){compatible=TRUE;g_model_gateway_protocol_state=1;}
    else{g_model_gateway_protocol_state=1;append_log_raw(L"[WARN] Messages 接口返回非预期状态码，已继续读取模型列表：");uint_to_wstr(status,num,32);append_log(num);}
    for(attempt=0;attempt<2;attempt++){
        LPCWSTR url=attempt==0?url1:url2;if(!url[0])continue;status=error=len=0;
        if(http_get_bytes(url,g_model_fetch_secret,proxy,g_http_bytes,sizeof(g_http_bytes),&len,&status,&error)){reachable=TRUE;if(status==401||status==403){authfail=TRUE;continue;}if(status>=200&&status<300){parse_models_from_http(g_http_bytes,len);if(g_model_count>0){populate_model_combos();uint_to_wstr((unsigned int)g_model_count,num,32);wcopy(g_model_status,512,compatible?L"Messages 兼容 · 已获取 ":L"已获取 ");wcat(g_model_status,512,num);wcat(g_model_status,512,L" 个模型。");append_log_raw(L"[OK] ");append_log_raw(compatible?L"Anthropic Messages 接口兼容，已获取 ":L"已获取 ");append_log_raw(num);append_log(L" 个模型。");goto done;}}}
    }
    if(authfail){wcopy(g_model_status,512,L"服务器已响应，但访问密钥未通过验证。");append_log(L"[ERROR] 模型连接测试失败：访问密钥未通过验证。");}
    else if(compatible){wcopy(g_model_status,512,L"Messages 接口兼容，但没有返回模型列表；可手动填写模型 ID。");append_log(L"[WARN] Anthropic Messages 接口兼容，但未获取到模型列表。");}
    else if(reachable){wcopy(g_model_status,512,L"服务器可以连接，但没有返回可识别的模型列表；可手动填写模型 ID。");append_log(L"[WARN] 服务器可连接，但未获取到模型列表。");}
    else{wcopy(g_model_status,512,L"无法连接服务器，请检查地址、网络环境和代理设置。");uint_to_wstr(error,num,32);append_log_raw(L"[ERROR] 模型连接测试失败，网络错误：");append_log(num);}
done:
    secure_zero_w(g_model_fetch_secret,4096);secure_zero_w(proxy,1024);g_model_fetch_busy=0;
    /* Post completion message to wizard thread for UI updates */
    if(g_wizard) {
        WPARAM wparam = (compatible && !authfail) ? 0 : (authfail ? 3 : (reachable ? 2 : (error ? error : 4)));
        pPostMessageW(g_wizard, WM_APP_MODEL_DISCOVERY_COMPLETE, wparam, 0);
    }
    return (compatible&&!authfail)?0:(authfail?3:(reachable?2:(error?error:4)));
}

static void start_model_discovery(BOOL notify_missing) {
    if(g_model_fetch_busy||!g_wizard) return;
    /* Coalesce all but the explicit button press into a 400 ms debounce window
       so that input changes do not fire a probe for every keystroke / focus move. */
    if(!notify_missing) {
        g_model_discovery_pending = 1;
        if(g_model_discovery_timer) return; /* already armed, wait for the tick */
        g_model_discovery_timer = SetTimer(g_wizard, (UINT_PTR)0xC0DE, 400, 0);
        if(!g_model_discovery_timer) {
            g_model_discovery_pending = 0; /* timer unavailable, fall through to immediate */
        } else {
            return;
        }
    }
    {
        HANDLE th; DWORD tid; int provider;
        g_model_discovery_pending = 0;
        if(g_model_discovery_timer) { KillTimer(g_wizard, g_model_discovery_timer); g_model_discovery_timer = 0; }
        provider=(int)pSendMessageW(g_wiz_provider,CB_GETCURSEL,0,0);
        if(provider!=1) {wcopy(g_model_status,512,L"自动获取模型仅用于第三方或自建网关。");InvalidateRect(g_wizard,0,FALSE);return;}
        g_model_gateway_protocol_state=0;g_model_fetch_base[0]=g_model_fetch_secret[0]=0;pGetWindowTextW(g_wiz_base_url,g_model_fetch_base,2048);pGetWindowTextW(g_wiz_secret,g_model_fetch_secret,4096);
        if(!g_model_fetch_base[0]||!g_model_fetch_secret[0]) {wcopy(g_model_status,512,L"请先填写服务器地址和访问密钥。");InvalidateRect(g_wizard,0,FALSE);if(notify_missing)pMessageBoxW(g_wizard,L"请先填写服务器地址和访问密钥，再测试连接。",L"还差一步",MB_ICONWARNING);secure_zero_w(g_model_fetch_secret,4096);return;}
        g_model_fetch_busy=1;if(g_wiz_test_models)pEnableWindow(g_wiz_test_models,FALSE);wcopy(g_model_status,512,L"正在测试连接并刷新模型列表……");InvalidateRect(g_wizard,0,FALSE);append_log(L"[INFO] 正在测试第三方模型接口并获取模型列表……");
        th=CreateThread(0,0,model_discovery_thread,0,0,&tid);if(th)CloseHandle(th);else{g_model_fetch_busy=0;if(g_wiz_test_models)pEnableWindow(g_wiz_test_models,TRUE);secure_zero_w(g_model_fetch_secret,4096);wcopy(g_model_status,512,L"无法启动模型刷新线程。");InvalidateRect(g_wizard,0,FALSE);}
    }
}

static BOOL wizard_target_from_scope(int scope, LPWSTR out, unsigned int cap) {
    WCHAR project[4096], profile[4096];
    if(scope==0) {
        profile[0]=0; GetEnvironmentVariableW(L"USERPROFILE",profile,4096);
        if(!profile[0]) return FALSE;
        wcopy(out,cap,profile); wcat(out,cap,L"\\.claude\\settings.json"); return TRUE;
    }
    if(!get_selected_project(project,4096)) return FALSE;
    wcopy(out,cap,project);
    wcat(out,cap,scope==1?L"\\.claude\\settings.json":L"\\.claude\\settings.local.json");
    return TRUE;
}

static void wizard_update_provider(void) {
    int provider=(int)pSendMessageW(g_wiz_provider,CB_GETCURSEL,0,0);
    BOOL gateway=(provider==1);g_model_gateway_protocol_state=0;
    pEnableWindow(g_wiz_base_url,gateway);
    pEnableWindow(g_wiz_secret,gateway||provider==0);
    if(g_wiz_test_models) pEnableWindow(g_wiz_test_models,gateway&&!g_model_fetch_busy);
    if(provider==1){pSendMessageW(g_wiz_base_url,EM_SETCUEBANNER,0,(LPARAM)L"https://gateway.example.com");pSendMessageW(g_wiz_secret,EM_SETCUEBANNER,0,(LPARAM)L"访问密钥");wcopy(g_model_status,512,L"填写服务器地址和访问密钥后，将自动测试并获取模型。");}
    else if(provider==0){pSendMessageW(g_wiz_base_url,EM_SETCUEBANNER,0,(LPARAM)L"使用官方接口，无需填写");pSendMessageW(g_wiz_secret,EM_SETCUEBANNER,0,(LPARAM)L"可留空，使用账号登录或官方 API");wcopy(g_model_status,512,L"官方 Claude 通常无需手动获取模型列表；保持推荐值即可。");}
    else{pSendMessageW(g_wiz_base_url,EM_SETCUEBANNER,0,(LPARAM)L"保持现有接口配置");pSendMessageW(g_wiz_secret,EM_SETCUEBANNER,0,(LPARAM)L"无需填写");wcopy(g_model_status,512,L"仅配置模型时，可直接手动填写模型 ID。");}
    InvalidateRect(g_wizard,0,FALSE);
}

static void wizard_apply_recommended(void) {
    int provider=(int)pSendMessageW(g_wiz_provider,CB_GETCURSEL,0,0);
    set_context_metadata_defaults();
    g_model_count=0;
    if(provider==0) {
        pSetWindowTextW(g_wiz_base_url,L""); pSetWindowTextW(g_wiz_secret,L"");
        pSetWindowTextW(g_wiz_default_model,L"default"); pSetWindowTextW(g_wiz_opus_model,L""); pSetWindowTextW(g_wiz_sonnet_model,L""); pSetWindowTextW(g_wiz_haiku_model,L""); pSetWindowTextW(g_wiz_subagent_model,L"");
    } else if(provider==1) {
        pSetWindowTextW(g_wiz_base_url,L""); pSetWindowTextW(g_wiz_secret,L"");
        pSetWindowTextW(g_wiz_default_model,L""); pSetWindowTextW(g_wiz_opus_model,L""); pSetWindowTextW(g_wiz_sonnet_model,L""); pSetWindowTextW(g_wiz_haiku_model,L""); pSetWindowTextW(g_wiz_subagent_model,L"");
    } else {
        pSetWindowTextW(g_wiz_base_url,L""); pSetWindowTextW(g_wiz_secret,L"");
        pSetWindowTextW(g_wiz_default_model,L""); pSetWindowTextW(g_wiz_opus_model,L""); pSetWindowTextW(g_wiz_sonnet_model,L""); pSetWindowTextW(g_wiz_haiku_model,L""); pSetWindowTextW(g_wiz_subagent_model,L"");
    }
    wizard_update_provider();
}

static void wizard_hide(void) {
    /* Cancel any pending debounce so the wizard does not fire a probe after
       the user closed it (avoids surprising network activity). */
    if(g_model_discovery_timer && g_wizard) {
        KillTimer(g_wizard, g_model_discovery_timer);
        g_model_discovery_timer = 0;
    }
    g_model_discovery_pending = 0;
    if(g_wiz_secret) pSetWindowTextW(g_wiz_secret,L"");
    if(g_wizard) pShowWindow(g_wizard,SW_HIDE);
    if(g_main) pEnableWindow(g_main,TRUE);
}

static void wizard_save(void) {
    WCHAR base[2048],secret[4096],defm[1024],opus[1024],sonnet[1024],haiku[1024],subagent[1024],effort[64],secret_name[128];
    WCHAR parsed_host[1024],parsed_path[4096];INTERNET_PORT parsed_port;BOOL parsed_secure;
    WCHAR pid[32]; DWORD tlen; BOOL first=TRUE, envfirst=TRUE; int scope,provider,eff;
    HANDLE th; DWORD tid;
    if(g_installing||g_import_busy) return;
    scope=(int)pSendMessageW(g_wiz_scope,CB_GETCURSEL,0,0); provider=(int)pSendMessageW(g_wiz_provider,CB_GETCURSEL,0,0); eff=(int)pSendMessageW(g_wiz_effort,CB_GETCURSEL,0,0);
    if(scope<0) scope=0; if(provider<0) provider=1; if(eff<0) eff=2;
    base[0]=secret[0]=defm[0]=opus[0]=sonnet[0]=haiku[0]=subagent[0]=0;
    pGetWindowTextW(g_wiz_base_url,base,2048); pGetWindowTextW(g_wiz_secret,secret,4096); pGetWindowTextW(g_wiz_default_model,defm,1024); pGetWindowTextW(g_wiz_opus_model,opus,1024); pGetWindowTextW(g_wiz_sonnet_model,sonnet,1024); pGetWindowTextW(g_wiz_haiku_model,haiku,1024); pGetWindowTextW(g_wiz_subagent_model,subagent,1024);
    if(provider==1 && !base[0]) { pMessageBoxW(g_wizard,L"请填写网关或第三方接口地址。\r\n\r\n示例：https://gateway.example.com",L"还差一步",MB_ICONWARNING); return; }
    if(provider==1 && !url_parse_basic(base,parsed_host,1024,parsed_path,4096,&parsed_port,&parsed_secure)) { pMessageBoxW(g_wizard,L"服务器地址无效。请填写以 http:// 或 https:// 开头的完整地址。",L"地址格式不正确",MB_ICONWARNING); return; }
    if(provider==1 && wfind_ci(base, L"@") >= 0 &&
       wfind_ci(base, L"@") > wfind_ci(base, L"://")) {  // '@' appears after scheme://
        pMessageBoxW(g_wizard, L"服务器地址不应包含用户名或密码片段。\r\n\r\n请只填写 https://host[:port] 形式，访问密钥在下方单独填写。",
                     L"地址格式不正确", MB_ICONWARNING);
        return;
    }
    if(provider==1 && g_model_gateway_protocol_state==2) { pMessageBoxW(g_wizard,L"该服务器可以列出模型，但缺少 Claude Code 必需的 Anthropic /v1/messages 接口。\r\n\r\n请改用 Anthropic Messages 兼容网关或自托管 NIM。",L"接口不兼容",MB_ICONERROR); return; }
    if(provider==1 && g_model_gateway_protocol_state==0 && pMessageBoxW(g_wizard,L"尚未确认服务器是否兼容 Anthropic Messages。\r\n\r\n建议先点击“测试并获取模型”。仍要保存吗？",L"兼容性尚未验证",MB_YESNO|MB_ICONWARNING)!=IDYES) return;
    if(!defm[0] && !opus[0] && !sonnet[0] && !haiku[0] && !subagent[0]) { pMessageBoxW(g_wizard,L"请至少选择或填写一个模型。\r\n\r\n通常只设置“主力模型”即可。",L"还差一步",MB_ICONWARNING); return; }
    if(!save_context_metadata_controls())return;
    if(provider==1 && !secret[0]) {
        if(pMessageBoxW(g_wizard,L"尚未填写访问密钥。可以先保存模型配置，但启动时可能无法连接。\r\n\r\n仍要继续吗？",L"未填写访问密钥",MB_YESNO|MB_ICONWARNING)!=IDYES) return;
    }
    if(!wizard_target_from_scope(scope,g_import_target,4096)) return;
    secret_name[0]=0;g_pending_active_credential_names[0]=0;
    if(provider==1 && secret[0]) wcopy(secret_name,128,L"ANTHROPIC_AUTH_TOKEN");
    else if(provider==0 && secret[0]) wcopy(secret_name,128,L"ANTHROPIC_API_KEY");
    /* The provider-2 ("only configure models") path must clear any active
       credentials so that previous gateway bearer tokens do not leak into
       a Claude Code session that uses the official endpoint. */
    if(provider==2) wcopy(g_pending_active_credential_names,512,L"-");
    else wcopy(g_pending_active_credential_names,512,secret_name[0]?secret_name:L"-");
    effort[0]=0; if(eff==0)wcopy(effort,64,L"low"); else if(eff==1)wcopy(effort,64,L"medium"); else if(eff==3)wcopy(effort,64,L"xhigh"); else wcopy(effort,64,L"high");
    g_model_json[0]=0; wcat(g_model_json,32768,L"{\r\n");
    json_add_pair(g_model_json,32768,L"$schema",L"https://json.schemastore.org/claude-code-settings.json",&first,2);
    json_add_pair(g_model_json,32768,L"model",defm,&first,2);
    json_add_pair(g_model_json,32768,L"effortLevel",effort,&first,2);
    if(base[0] || defm[0] || opus[0] || sonnet[0] || haiku[0] || subagent[0]) {
        if(!first) wcat(g_model_json,32768,L",\r\n"); wcat(g_model_json,32768,L"  \"env\": {\r\n");
        json_add_pair(g_model_json,32768,L"ANTHROPIC_BASE_URL",base,&envfirst,4);
        /* ANTHROPIC_MODEL has higher priority than the top-level model field.
           Keep both aligned so an older env value cannot override the wizard. */
        json_add_pair(g_model_json,32768,L"ANTHROPIC_MODEL",defm,&envfirst,4);
        json_add_pair(g_model_json,32768,L"ANTHROPIC_DEFAULT_OPUS_MODEL",opus,&envfirst,4);
        json_add_pair(g_model_json,32768,L"ANTHROPIC_DEFAULT_SONNET_MODEL",sonnet,&envfirst,4);
        json_add_pair(g_model_json,32768,L"ANTHROPIC_DEFAULT_HAIKU_MODEL",haiku,&envfirst,4);
        json_add_pair(g_model_json,32768,L"CLAUDE_CODE_SUBAGENT_MODEL",subagent,&envfirst,4);
        wcat(g_model_json,32768,L"\r\n  }"); first=FALSE;
    }
    wcat(g_model_json,32768,L"\r\n}\r\n");
    tlen=GetTempPathW(4096,g_model_temp); if(!tlen||tlen>=4000){pMessageBoxW(g_wizard,L"无法创建临时配置文件。",L"保存失败",MB_ICONERROR);secure_zero_w(secret,4096);return;}
    uint_to_wstr(GetCurrentProcessId(),pid,32); wcat(g_model_temp,4096,L"fongap-model-");wcat(g_model_temp,4096,pid);wcat(g_model_temp,4096,L".json");
    if(!write_utf8_text_file(g_model_temp,g_model_json)){pMessageBoxW(g_wizard,L"无法写入临时配置文件。",L"保存失败",MB_ICONERROR);secure_zero_w(secret,4096);secure_zero_w(g_model_json,32768);return;}
    secure_zero_w(g_model_json,32768);
    wcopy(g_extra_secret_name,128,(secret_name[0]&&secret[0])?secret_name:L""); wcopy(g_extra_secret_value,4096,(secret_name[0]&&secret[0])?secret:L"");
    secure_zero_w(g_extra_secret2_name,128);secure_zero_w(g_extra_secret2_value,4096);
    wcopy(g_import_source,4096,g_model_temp); wcopy(g_import_mode,32,L"Merge"); g_import_delete_source=TRUE;
    append_log(L""); append_log(L"[INFO] 模型配置向导正在保存配置；主力模型将同步覆盖 model 与 ANTHROPIC_MODEL。");
    if(secret_name[0]&&secret[0]) { append_log_raw(L"[SECURE] "); append_log_raw(secret_name); append_log(L" 将保存到 Windows 凭据库，不写入 settings.json。"); }
    secure_zero_w(secret,4096); pSetWindowTextW(g_wiz_secret,L""); wizard_hide();g_import_busy=1;set_busy(TRUE);
    th=CreateThread(0,0,import_thread,0,0,&tid);
    if(th) CloseHandle(th);
    else {
        DeleteFileW(g_model_temp);g_import_delete_source=FALSE;g_import_busy=0;set_busy(FALSE);
        secure_zero_w(g_import_source,4096);secure_zero_w(g_extra_secret_name,128);secure_zero_w(g_extra_secret_value,4096);secure_zero_w(g_extra_secret2_name,128);secure_zero_w(g_extra_secret2_value,4096);secure_zero_w(g_pending_active_credential_names,512);
        pMessageBoxW(g_main,L"无法启动模型配置保存任务。",L"保存失败",MB_ICONERROR);
    }
}

static void layout_wizard_controls(int width, int height) {
    int field_x=sc(168), field_right=width-sc(24), field_w=field_right-field_x;
    int inner_x=field_x+sc(4), inner_w=field_w-sc(8);
    int test_w=sc(124), row_h=sc(34), edit_h=sc(22);
    int secret_inner_x=field_x+sc(4), secret_inner_w=field_w-test_w-sc(16);
    int footer_y=height-sc(50), save_w=sc(108), cancel_w=sc(82), footer_gap=sc(8);
    int table_left=sc(40),role_w=sc(120),model_x=table_left+role_w;
    int gap=sc(7),cap_w=sc(82),budget_w=sc(88),warning_w=sc(72),reserve_w=sc(84),effort_w=sc(84);
    int model_w=width-sc(24)-model_x-cap_w-budget_w-warning_w-reserve_w-effort_w-gap*5;
    int cap_x=model_x+model_w+gap,budget_x=cap_x+cap_w+gap,warning_x=budget_x+budget_w+gap,reserve_x=warning_x+warning_w+gap,effort_x=reserve_x+reserve_w+gap;
    int i,y;
    if(field_w<sc(320)) field_w=sc(320);
    pMoveWindow(g_wiz_scope,inner_x,sc(93),inner_w,sc(140),TRUE);
    pMoveWindow(g_wiz_provider,inner_x,sc(135),inner_w,sc(140),TRUE);
    pMoveWindow(g_wiz_base_url,field_x+sc(4),sc(176),field_w-sc(8),sc(24),TRUE);
    pMoveWindow(g_wiz_secret,secret_inner_x,sc(218),secret_inner_w,sc(24),TRUE);
    pMoveWindow(g_wiz_test_models,field_right-test_w,sc(212),test_w,row_h,TRUE);
    for(i=0;i<MODEL_ROLE_COUNT;i++){
        y=sc(320+i*42);
        {HWND model=model_combo_for_role(i);pMoveWindow(model,model_x+sc(4),y+sc(7),model_w-sc(8),sc(160),TRUE);}
        pMoveWindow(g_wiz_context_capacity[i],cap_x+sc(4),y+sc(8),cap_w-sc(8),edit_h,TRUE);
        pMoveWindow(g_wiz_context_budget[i],budget_x+sc(4),y+sc(8),budget_w-sc(8),edit_h,TRUE);
        pMoveWindow(g_wiz_context_warning[i],warning_x+sc(4),y+sc(8),warning_w-sc(8),edit_h,TRUE);
        pMoveWindow(g_wiz_context_reserve[i],reserve_x+sc(4),y+sc(8),reserve_w-sc(8),edit_h,TRUE);
        pMoveWindow(g_wiz_context_effort[i],effort_x+sc(4),y+sc(7),effort_w-sc(8),sc(120),TRUE);
    }
    pMoveWindow(g_wiz_recommended,sc(24),footer_y,sc(138),sc(34),TRUE);
    pMoveWindow(g_wiz_save,width-sc(24)-save_w,footer_y,save_w,sc(34),TRUE);
    pMoveWindow(g_wiz_cancel,width-sc(24)-save_w-footer_gap-cancel_w,footer_y,cancel_w,sc(34),TRUE);
}

static void paint_wizard(HWND hwnd) {
    PAINTSTRUCT ps; HDC dc=BeginPaint(hwnd,&ps); RECT c,r; HBRUSH bg;
    int field_x=sc(168), field_right, label_x=sc(40), label_right=sc(160);
    int table_left=sc(40),role_w=sc(120),model_x=table_left+role_w;
    int gap=sc(7),cap_w=sc(82),budget_w=sc(88),warning_w=sc(72),reserve_w=sc(84),effort_w=sc(84);
    int model_w,cap_x,budget_x,warning_x,reserve_x,effort_x,i,y;
    GetClientRect(hwnd,&c); field_right=c.right-sc(24);
    model_w=c.right-sc(24)-model_x-cap_w-budget_w-warning_w-reserve_w-effort_w-gap*5;
    cap_x=model_x+model_w+gap;budget_x=cap_x+cap_w+gap;warning_x=budget_x+budget_w+gap;reserve_x=warning_x+warning_w+gap;effort_x=reserve_x+reserve_w+gap;
    bg=CreateSolidBrush(rgb(245,245,247)); FillRect(dc,&c,bg); DeleteObject(bg);
    r=mkrect(sc(24),sc(14),c.right-sc(24),sc(48)); text_out(dc,L"模型配置",r,g_wiz_font_title,rgb(29,29,31),DT_LEFT|DT_VCENTER|DT_SINGLELINE);
    r=mkrect(sc(26),sc(46),c.right-sc(24),sc(70)); text_out(dc,L"设置原生 Claude Code 接口与模型；容量和预算仅用于本地规划。",r,g_wiz_font_subtitle,rgb(105,105,110),DT_LEFT|DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS);

    r=mkrect(sc(24),sc(78),c.right-sc(24),sc(278)); fill_round(dc,r,sc(15),rgb(255,255,255),rgb(232,232,235));
    r=mkrect(sc(24),sc(288),c.right-sc(24),sc(554)); fill_round(dc,r,sc(15),rgb(255,255,255),rgb(232,232,235));

    /* Tonal fields avoid a second border around the native edit/combo control. */
    r=mkrect(field_x,sc(86),field_right,sc(120)); fill_round(dc,r,sc(10),rgb(247,247,249),rgb(247,247,249));
    r=mkrect(field_x,sc(128),field_right,sc(162)); fill_round(dc,r,sc(10),rgb(247,247,249),rgb(247,247,249));
    r=mkrect(field_x,sc(170),field_right,sc(204)); fill_round(dc,r,sc(10),rgb(247,247,249),rgb(247,247,249));
    r=mkrect(field_x,sc(212),field_right-sc(132),sc(246)); fill_round(dc,r,sc(10),rgb(247,247,249),rgb(247,247,249));

    r=mkrect(label_x,sc(86),label_right,sc(120)); text_out(dc,L"保存范围",r,g_wiz_font_button,rgb(45,45,48),DT_LEFT|DT_VCENTER|DT_SINGLELINE);
    r=mkrect(label_x,sc(128),label_right,sc(162)); text_out(dc,L"接口类型",r,g_wiz_font_button,rgb(45,45,48),DT_LEFT|DT_VCENTER|DT_SINGLELINE);
    r=mkrect(label_x,sc(170),label_right,sc(204)); text_out(dc,L"服务器地址",r,g_wiz_font_button,rgb(45,45,48),DT_LEFT|DT_VCENTER|DT_SINGLELINE);
    r=mkrect(label_x,sc(212),label_right,sc(246)); text_out(dc,L"访问密钥",r,g_wiz_font_button,rgb(45,45,48),DT_LEFT|DT_VCENTER|DT_SINGLELINE);
    r=mkrect(label_x,sc(248),c.right-sc(40),sc(276)); text_out(dc,g_model_status,r,g_wiz_font_subtitle,g_model_fetch_busy?rgb(216,119,86):rgb(105,105,110),DT_LEFT|DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS);

    r=mkrect(table_left,sc(292),model_x,sc(316));text_out(dc,L"角色",r,g_wiz_font_subtitle,rgb(105,105,110),DT_LEFT|DT_VCENTER|DT_SINGLELINE);
    r=mkrect(model_x,sc(292),model_x+model_w,sc(316));text_out(dc,L"模型 ID",r,g_wiz_font_subtitle,rgb(105,105,110),DT_LEFT|DT_VCENTER|DT_SINGLELINE);
    r=mkrect(cap_x,sc(292),cap_x+cap_w,sc(316));text_out(dc,L"容量",r,g_wiz_font_subtitle,rgb(105,105,110),DT_CENTER|DT_VCENTER|DT_SINGLELINE);
    r=mkrect(budget_x,sc(292),budget_x+budget_w,sc(316));text_out(dc,L"建议预算",r,g_wiz_font_subtitle,rgb(105,105,110),DT_CENTER|DT_VCENTER|DT_SINGLELINE);
    r=mkrect(warning_x,sc(292),warning_x+warning_w,sc(316));text_out(dc,L"预警 %",r,g_wiz_font_subtitle,rgb(105,105,110),DT_CENTER|DT_VCENTER|DT_SINGLELINE);
    r=mkrect(reserve_x,sc(292),reserve_x+reserve_w,sc(316));text_out(dc,L"输出预留",r,g_wiz_font_subtitle,rgb(105,105,110),DT_CENTER|DT_VCENTER|DT_SINGLELINE);
    r=mkrect(effort_x,sc(292),effort_x+effort_w,sc(316));text_out(dc,L"思考",r,g_wiz_font_subtitle,rgb(105,105,110),DT_CENTER|DT_VCENTER|DT_SINGLELINE);
    for(i=0;i<MODEL_ROLE_COUNT;i++){
        y=sc(320+i*42);r=mkrect(model_x,y,field_right,y+sc(34));fill_round(dc,r,sc(9),rgb(249,249,250),rgb(249,249,250));
        r=mkrect(table_left,y,model_x-sc(7),y+sc(34));text_out(dc,MODEL_ROLE_LABELS[i],r,g_wiz_font_button,rgb(45,45,48),DT_LEFT|DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS);
    }
    r=mkrect(table_left,sc(530),c.right-sc(40),sc(552));text_out(dc,L"容量可自动识别或手动标注；本地预算不改变 Claude Code 或网关限制。",r,g_wiz_font_subtitle,rgb(105,105,110),DT_LEFT|DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS);
    EndPaint(hwnd,&ps);
}

static LRESULT __stdcall wizard_wndproc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    switch(msg) {
        case WM_CREATE:
            g_wizard=hwnd;
            g_wizard_dpi=g_main_dpi;if(pGetDpiForWindow){UINT dpi=pGetDpiForWindow(hwnd);if(dpi>=96&&dpi<=480)g_wizard_dpi=(int)dpi;}g_dpi=g_wizard_dpi;create_wizard_fonts();
            wcopy(g_model_status,512,L"填写服务器地址和访问密钥后，将自动测试并获取模型。");
            g_wiz_scope=create_child_control(hwnd,0,L"COMBOBOX",L"",WS_TABSTOP|WS_VSCROLL|CBS_DROPDOWNLIST,0,0,0,0,IDC_WIZ_SCOPE);
            pSendMessageW(g_wiz_scope,CB_ADDSTRING,0,(LPARAM)L"所有项目（全局，推荐）"); pSendMessageW(g_wiz_scope,CB_ADDSTRING,0,(LPARAM)L"当前项目（团队共享）"); pSendMessageW(g_wiz_scope,CB_ADDSTRING,0,(LPARAM)L"当前项目（仅本机）"); pSendMessageW(g_wiz_scope,CB_SETCURSEL,0,0);
            g_wiz_provider=create_child_control(hwnd,0,L"COMBOBOX",L"",WS_TABSTOP|WS_VSCROLL|CBS_DROPDOWNLIST,0,0,0,0,IDC_WIZ_PROVIDER);
            pSendMessageW(g_wiz_provider,CB_ADDSTRING,0,(LPARAM)L"官方 Claude（账号登录或官方 API）"); pSendMessageW(g_wiz_provider,CB_ADDSTRING,0,(LPARAM)L"第三方 / 自建网关（推荐）"); pSendMessageW(g_wiz_provider,CB_ADDSTRING,0,(LPARAM)L"只配置模型，不修改接口"); pSendMessageW(g_wiz_provider,CB_SETCURSEL,1,0);
            g_wiz_base_url=create_child_control(hwnd,0,L"EDIT",L"",WS_TABSTOP|ES_AUTOHSCROLL,0,0,0,0,IDC_WIZ_BASE_URL);
            g_wiz_secret_type=0;
            g_wiz_secret=create_child_control(hwnd,0,L"EDIT",L"",WS_TABSTOP|ES_AUTOHSCROLL|ES_PASSWORD,0,0,0,0,IDC_WIZ_SECRET);
            g_wiz_test_models=create_child_control(hwnd,0,L"BUTTON",L"测试并获取模型",WS_TABSTOP|BS_OWNERDRAW,0,0,0,0,IDC_WIZ_TEST_MODELS);
            g_wiz_default_model=create_child_control(hwnd,0,L"COMBOBOX",L"",WS_TABSTOP|WS_VSCROLL|CBS_DROPDOWN|CBS_AUTOHSCROLL,0,0,0,0,IDC_WIZ_DEFAULT_MODEL);
            g_wiz_opus_model=create_child_control(hwnd,0,L"COMBOBOX",L"",WS_TABSTOP|WS_VSCROLL|CBS_DROPDOWN|CBS_AUTOHSCROLL,0,0,0,0,IDC_WIZ_OPUS_MODEL);
            g_wiz_sonnet_model=create_child_control(hwnd,0,L"COMBOBOX",L"",WS_TABSTOP|WS_VSCROLL|CBS_DROPDOWN|CBS_AUTOHSCROLL,0,0,0,0,IDC_WIZ_SONNET_MODEL);
            g_wiz_haiku_model=create_child_control(hwnd,0,L"COMBOBOX",L"",WS_TABSTOP|WS_VSCROLL|CBS_DROPDOWN|CBS_AUTOHSCROLL,0,0,0,0,IDC_WIZ_HAIKU_MODEL);
            g_wiz_subagent_model=create_child_control(hwnd,0,L"COMBOBOX",L"",WS_TABSTOP|WS_VSCROLL|CBS_DROPDOWN|CBS_AUTOHSCROLL,0,0,0,0,IDC_WIZ_SUBAGENT_MODEL);
            {int i;for(i=0;i<MODEL_ROLE_COUNT;i++){
                g_wiz_context_capacity[i]=create_child_control(hwnd,0,L"EDIT",L"",WS_TABSTOP|ES_AUTOHSCROLL|ES_NUMBER,0,0,0,0,IDC_WIZ_CONTEXT_CAPACITY+i);
                g_wiz_context_budget[i]=create_child_control(hwnd,0,L"EDIT",L"",WS_TABSTOP|ES_AUTOHSCROLL|ES_NUMBER,0,0,0,0,IDC_WIZ_CONTEXT_BUDGET+i);
                g_wiz_context_warning[i]=create_child_control(hwnd,0,L"EDIT",L"80",WS_TABSTOP|ES_AUTOHSCROLL|ES_NUMBER,0,0,0,0,IDC_WIZ_CONTEXT_WARNING+i);
                g_wiz_context_reserve[i]=create_child_control(hwnd,0,L"EDIT",L"8192",WS_TABSTOP|ES_AUTOHSCROLL|ES_NUMBER,0,0,0,0,IDC_WIZ_CONTEXT_RESERVE+i);
                g_wiz_context_effort[i]=create_child_control(hwnd,0,L"COMBOBOX",L"",WS_TABSTOP|WS_VSCROLL|CBS_DROPDOWNLIST,0,0,0,0,IDC_WIZ_CONTEXT_EFFORT+i);
                pSendMessageW(g_wiz_context_capacity[i],EM_SETCUEBANNER,0,(LPARAM)L"自动");pSendMessageW(g_wiz_context_budget[i],EM_SETCUEBANNER,0,(LPARAM)L"未设置");
                pSendMessageW(g_wiz_context_effort[i],CB_ADDSTRING,0,(LPARAM)L"低");pSendMessageW(g_wiz_context_effort[i],CB_ADDSTRING,0,(LPARAM)L"中");pSendMessageW(g_wiz_context_effort[i],CB_ADDSTRING,0,(LPARAM)L"高");pSendMessageW(g_wiz_context_effort[i],CB_ADDSTRING,0,(LPARAM)L"极高");
            }}
            g_wiz_effort=g_wiz_context_effort[0];
            g_wiz_recommended=create_child_control(hwnd,0,L"BUTTON",L"恢复推荐值",WS_TABSTOP|BS_OWNERDRAW,0,0,0,0,IDC_WIZ_RECOMMENDED);
            g_wiz_cancel=create_child_control(hwnd,0,L"BUTTON",L"取消",WS_TABSTOP|BS_OWNERDRAW,0,0,0,0,IDCANCEL);
            g_wiz_save=create_child_control(hwnd,0,L"BUTTON",L"保存配置",WS_TABSTOP|BS_OWNERDRAW,0,0,0,0,IDC_WIZ_SAVE);
            apply_wizard_control_fonts();apply_wizard_edit_margins();load_context_metadata_controls();{RECT cr;GetClientRect(hwnd,&cr);g_dpi=g_wizard_dpi;layout_wizard_controls(cr.right-cr.left,cr.bottom-cr.top);} wizard_update_provider(); return 0;
        case WM_SIZE: { int width=(int)(lp&0xFFFF),height=(int)((lp>>16)&0xFFFF);g_dpi=g_wizard_dpi;if(g_wiz_scope)layout_wizard_controls(width,height); InvalidateRect(hwnd,0,FALSE); return 0; }
        case WM_GETMINMAXINFO: {MINMAXINFO *mmi=(MINMAXINFO*)lp;g_dpi=g_wizard_dpi;if(mmi){mmi->ptMinTrackSize_x=sc(880);mmi->ptMinTrackSize_y=sc(660);return 0;}break;}
        case WM_PAINT: g_dpi=g_wizard_dpi;paint_wizard(hwnd); return 0;
        case WM_DPICHANGED: {
            RECT *suggested=(RECT*)lp; int newdpi=(int)(wp & 0xFFFF);
            if(newdpi>=96&&newdpi<=480&&newdpi!=g_wizard_dpi){g_wizard_dpi=newdpi;g_dpi=g_wizard_dpi;create_wizard_fonts();apply_wizard_control_fonts();apply_wizard_edit_margins();if(suggested)SetWindowPos(hwnd,0,suggested->left,suggested->top,suggested->right-suggested->left,suggested->bottom-suggested->top,SWP_NOZORDER|SWP_NOACTIVATE);{RECT cr;GetClientRect(hwnd,&cr);layout_wizard_controls(cr.right-cr.left,cr.bottom-cr.top);}InvalidateRect(hwnd,0,TRUE);}
            return 0;
        }
        case WM_ERASEBKGND: return 1;
        case WM_TIMER:
            /* Debounce timer for input-driven model discovery. Only fire while
               a probe is pending and not already in flight. */
            if(g_model_discovery_timer && wp == g_model_discovery_timer) {
                KillTimer(hwnd, g_model_discovery_timer);
                g_model_discovery_timer = 0;
                if(g_model_discovery_pending && !g_model_fetch_busy) {
                    g_model_discovery_pending = 0;
                    start_model_discovery(TRUE);
                }
                return 0;
            }
            break;
        case WM_APP_MODEL_DISCOVERY_COMPLETE: {
            /* Re-enable test button and repaint on GUI thread */
            if(g_wiz_test_models) pEnableWindow(g_wiz_test_models, TRUE);
            InvalidateRect(hwnd, 0, FALSE);
            return 0;
        }
        case WM_DRAWITEM: { DRAWITEMSTRUCT *di=(DRAWITEMSTRUCT*)lp; if(di&&di->CtlType==ODT_BUTTON){draw_button(di);return TRUE;} return FALSE; }
        case WM_CTLCOLOREDIT: { HDC dc=(HDC)wp; SetTextColor(dc,rgb(35,35,38)); SetBkColor(dc,rgb(247,247,249)); return (LRESULT)g_br_input; }
        case WM_CTLCOLORLISTBOX: { HDC dc=(HDC)wp; SetTextColor(dc,rgb(35,35,38)); SetBkColor(dc,rgb(255,255,255)); return (LRESULT)g_br_card; }
        case WM_COMMAND: {
            int id=(int)(wp&0xFFFF), code=(int)((wp>>16)&0xFFFF);
            if(id==IDC_WIZ_PROVIDER&&code==CBN_SELCHANGE){wizard_update_provider();return 0;}
            if((id==IDC_WIZ_BASE_URL||id==IDC_WIZ_SECRET)&&code==EN_KILLFOCUS){start_model_discovery(FALSE);return 0;}
            if((id==IDC_WIZ_BASE_URL||id==IDC_WIZ_SECRET)&&code==0x0300){g_model_count=0;g_model_gateway_protocol_state=0;return 0;}
            if((id==IDC_WIZ_DEFAULT_MODEL||id==IDC_WIZ_OPUS_MODEL||id==IDC_WIZ_SONNET_MODEL||id==IDC_WIZ_HAIKU_MODEL||id==IDC_WIZ_SUBAGENT_MODEL)&&code==CBN_DROPDOWN){if(g_model_count==0)start_model_discovery(FALSE);return 0;}
            if((id==IDC_WIZ_DEFAULT_MODEL||id==IDC_WIZ_OPUS_MODEL||id==IDC_WIZ_SONNET_MODEL||id==IDC_WIZ_HAIKU_MODEL||id==IDC_WIZ_SUBAGENT_MODEL)&&code==CBN_SELCHANGE){int role=id==IDC_WIZ_DEFAULT_MODEL?0:id==IDC_WIZ_OPUS_MODEL?1:id==IDC_WIZ_SONNET_MODEL?2:id==IDC_WIZ_HAIKU_MODEL?3:4;apply_discovered_context_capacity(role);return 0;}
            if(id==IDC_WIZ_TEST_MODELS&&code==BN_CLICKED){start_model_discovery(TRUE);return 0;}
            if(id==IDC_WIZ_RECOMMENDED&&code==BN_CLICKED){wizard_apply_recommended();return 0;}
            if(id==IDC_WIZ_SAVE&&code==BN_CLICKED){wizard_save();return 0;}
            if((id==IDC_WIZ_CANCEL||id==IDCANCEL)&&code==BN_CLICKED){wizard_hide();return 0;}
            return 0;
        }
        case WM_CLOSE: wizard_hide(); return 0;
    }
    return pDefWindowProcW(hwnd,msg,wp,lp);
}

static void show_model_wizard(void) {
    if(g_installing||g_import_busy) return;
    g_dpi=g_wizard?g_wizard_dpi:g_main_dpi;
    if(!g_wizard) {
        g_wizard=pCreateWindowExW(WS_EX_CONTROLPARENT,WIZARD_CLASS_NAME,L"Claude Code 模型配置",WS_OVERLAPPED|WS_CAPTION|WS_SYSMENU|WS_MAXIMIZEBOX|WS_THICKFRAME|WS_CLIPCHILDREN|WS_CLIPSIBLINGS,
            CW_USEDEFAULT,CW_USEDEFAULT,sc(1000),sc(680),g_main,0,g_instance,0);
        if(!g_wizard){pMessageBoxW(g_main,L"无法打开模型配置向导。",L"ClaudeCodeManager",MB_ICONERROR);return;}
        pSendMessageW(g_wizard,WM_SETICON,ICON_BIG,(LPARAM)g_app_icon); pSendMessageW(g_wizard,WM_SETICON,ICON_SMALL,(LPARAM)g_app_icon);
    }
    pEnableWindow(g_main,FALSE); pShowWindow(g_wizard,SW_SHOW); pUpdateWindow(g_wizard);
}


static int main_right_width(int width) {
    int result=width*39/100;if(result<sc(330))result=sc(330);if(result>sc(390))result=sc(390);return result;
}

static void layout_controls(int width, int height) {
    int m=sc(22),gap=sc(14),top=sc(16),cardh=sc(400),rightw=main_right_width(width);
    int leftw=width-m*2-gap-rightw,lx=m,rx=lx+leftw+gap;
    int logy=top+cardh+sc(12),logh=height-logy-sc(16),compact_left=leftw<sc(430);
    int inner=sc(20),btn_gap=sc(10),left_btn_area=leftw-inner*2;
    int primary_w=(left_btn_area-btn_gap*2)*46/100;
    int secondary_w=(left_btn_area-btn_gap*2-primary_w)/2;
    int right_inner_w=rightw-inner*2,test_w=sc(92),network_gap=sc(8),network_w=right_inner_w-test_w-network_gap;
    int pair_w=(right_inner_w-sc(10))/2;
    g_client_w=width;g_client_h=height;

    /* Left card */
    pMoveWindow(g_workspace,lx+sc(32),top+sc(78),leftw-sc(148),sc(22),TRUE);
    pMoveWindow(g_browse,lx+leftw-sc(108),top+sc(71),sc(88),sc(34),TRUE);
    pMoveWindow(g_refresh,lx+leftw-sc(108),top+sc(109),sc(88),sc(34),TRUE);
    pMoveWindow(g_projects,lx+sc(22),top+sc(146),leftw-sc(44),compact_left?sc(138):sc(170),TRUE);
    if(compact_left){
        pMoveWindow(g_launch,lx+inner,top+sc(296),left_btn_area,sc(36),TRUE);
        pMoveWindow(g_terminal,lx+inner,top+sc(340),(left_btn_area-btn_gap)/2,sc(32),TRUE);
        pMoveWindow(g_folder,lx+inner+(left_btn_area-btn_gap)/2+btn_gap,top+sc(340),left_btn_area-(left_btn_area-btn_gap)/2-btn_gap,sc(32),TRUE);
    }else{
        pMoveWindow(g_launch,lx+inner,top+sc(336),primary_w,sc(40),TRUE);
        pMoveWindow(g_terminal,lx+inner+primary_w+btn_gap,top+sc(336),secondary_w,sc(40),TRUE);
        pMoveWindow(g_folder,lx+inner+primary_w+btn_gap+secondary_w+btn_gap,top+sc(336),left_btn_area-primary_w-secondary_w-btn_gap*2,sc(40),TRUE);
    }

    /* Right card */
    pMoveWindow(g_network_mode,rx+inner,top+sc(114),network_w,sc(120),TRUE);
    {RECT combo_rect;int combo_h=sc(24);if(GetWindowRect(g_network_mode,&combo_rect)){int actual=(int)(combo_rect.bottom-combo_rect.top);if(actual>=sc(20)&&actual<=sc(32))combo_h=actual;}pMoveWindow(g_test_network,rx+inner+network_w+network_gap,top+sc(114),test_w,combo_h,TRUE);}
    pMoveWindow(g_proxy,rx+sc(32),top+sc(177),rightw-sc(64),sc(20),TRUE);
    pMoveWindow(g_url,rx+sc(32),top+sc(229),rightw-sc(64),sc(20),TRUE);
    pMoveWindow(g_install,rx+inner,top+sc(262),right_inner_w,sc(36),TRUE);
    pMoveWindow(g_import_settings,rx+inner,top+sc(304),pair_w,sc(32),TRUE);
    pMoveWindow(g_open_settings,rx+inner+pair_w+sc(10),top+sc(304),right_inner_w-pair_w-sc(10),sc(32),TRUE);
    pMoveWindow(g_model_wizard,rx+inner,top+sc(340),pair_w,sc(32),TRUE);
    pMoveWindow(g_shortcut,rx+inner+pair_w+sc(10),top+sc(340),right_inner_w-pair_w-sc(10),sc(32),TRUE);

    pMoveWindow(g_fongap_link,width-m-sc(220),logy+sc(7),sc(198),sc(28),TRUE);
    pMoveWindow(g_log,m+sc(22),logy+sc(40),width-m*2-sc(44),logh-sc(52),TRUE);
}

static void draw_button(const DRAWITEMSTRUCT *di) {
    int id=(int)di->CtlID;
    BOOL wizard_button=(id>=2000||id==IDCANCEL);HFONT button_font;
    g_dpi=wizard_button?g_wizard_dpi:g_main_dpi;button_font=wizard_button?g_wiz_font_button:g_font_button;
    BOOL primary=(id==IDC_LAUNCH || id==IDC_WIZ_SAVE || (id==IDC_INSTALL&&!g_claude_installed));
    BOOL disabled=(di->itemState & ODS_DISABLED) != 0;
    BOOL pressed=(di->itemState & ODS_SELECTED) != 0;
    BOOL focused=(di->itemState & ODS_FOCUS) != 0;
    BOOL hovered=(di->hwndItem==g_hover_button)&&!disabled;
    DWORD fill, border, text;
    WCHAR label[128];
    RECT r=di->rcItem;
    if(id==IDC_FONGAP_LINK) {
        HBRUSH br=CreateSolidBrush(rgb(28,28,30));
        FillRect(di->hDC,&r,br); DeleteObject(br);
        label[0]=0; pGetWindowTextW(di->hwndItem,label,128);
        text_out(di->hDC,label,r,g_font_link,hovered?rgb(246,166,133):rgb(232,139,103),DT_RIGHT|DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS);
        return;
    }
    if (primary) {
        fill = disabled ? rgb(210,210,214) : (pressed ? rgb(190,92,63) : (hovered?rgb(224,128,94):rgb(216,119,86)));
        border=fill; text=rgb(255,255,255);
    } else {
        fill = pressed ? rgb(232,232,235) : (hovered?rgb(241,241,244):rgb(247,247,249));
        border = rgb(226,226,230);
        text = disabled ? rgb(138,138,143) : rgb(45,45,48);
    }
    fill_round(di->hDC,r,sc(12),fill,border);
    label[0]=0; pGetWindowTextW(di->hwndItem,label,128);
    {int pad_y=(id==IDC_TEST_NETWORK)?sc(1):sc(4);RECT tr=mkrect(r.left+sc(10),r.top+pad_y,r.right-sc(10),r.bottom-pad_y);text_out(di->hDC,label,tr,button_font,text,DT_CENTER|DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS);}
    if(focused&&!disabled){RECT fr=mkrect(r.left+sc(5),r.top+sc(5),r.right-sc(5),r.bottom-sc(5));DrawFocusRect(di->hDC,&fr);}
}

static void draw_list_item(const DRAWITEMSTRUCT *di) {
    WCHAR name[1024]; RECT r=di->rcItem, tr, ir, tab;
    DWORD bg = (di->itemState & ODS_SELECTED) ? rgb(255,241,236) : rgb(255,255,255);
    HBRUSH br=CreateSolidBrush(bg);
    FillRect(di->hDC,&r,br); DeleteObject(br);
    if (di->itemID == (UINT)-1) return;
    name[0]=0; pSendMessageW(di->hwndItem,LB_GETTEXT,di->itemID,(LPARAM)name);
    tab=mkrect(r.left+sc(12),r.top+sc(9),r.left+sc(24),r.top+sc(14));fill_round(di->hDC,tab,sc(3),rgb(236,154,121),rgb(236,154,121));
    ir=mkrect(r.left+sc(12),r.top+sc(12),r.left+sc(35),r.top+sc(28));
    fill_round(di->hDC,ir,sc(4),rgb(255,224,212),rgb(242,184,161));
    tr=mkrect(r.left+sc(46),r.top,r.right-sc(14),r.bottom);
    text_out(di->hDC,name,tr,g_font_body,rgb(35,35,38),DT_LEFT|DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS);
    if(di->itemState&ODS_FOCUS){RECT fr=mkrect(r.left+sc(3),r.top+sc(2),r.right-sc(3),r.bottom-sc(2));DrawFocusRect(di->hDC,&fr);}
}

static void paint_main(HWND hwnd) {
    PAINTSTRUCT ps;HDC dc=BeginPaint(hwnd,&ps);RECT c;
    int m=sc(22),gap=sc(14),top=sc(16),cardh=sc(400),rightw=main_right_width(g_client_w);
    int leftw=g_client_w-m*2-gap-rightw,lx=m,rx=lx+leftw+gap;
    int logy=top+cardh+sc(12),logh=g_client_h-logy-sc(16),compact_left=leftw<sc(430);
    RECT r,t;WCHAR count[64],elapsed[32],status[128];HBRUSH bg;
    GetClientRect(hwnd,&c);bg=CreateSolidBrush(rgb(245,245,247));FillRect(dc,&c,bg);DeleteObject(bg);

    r=mkrect(lx,top,lx+leftw,top+cardh);fill_round(dc,r,sc(18),rgb(255,255,255),rgb(232,232,235));
    r=mkrect(rx,top,rx+rightw,top+cardh);fill_round(dc,r,sc(18),rgb(255,255,255),rgb(232,232,235));
    r=mkrect(m,logy,g_client_w-m,logy+logh);fill_round(dc,r,sc(18),rgb(28,28,30),rgb(28,28,30));

    /* Left card: consistent 20 px content inset and an 8/10 px rhythm. */
    r=mkrect(lx+sc(20),top+sc(14),lx+leftw-sc(20),top+sc(40));text_out(dc,L"项目文件夹",r,g_font_heading,rgb(29,29,31),DT_LEFT|DT_VCENTER|DT_SINGLELINE);
    r=mkrect(lx+sc(20),top+sc(40),lx+leftw-sc(20),top+sc(62));text_out(dc,L"选择项目目录，然后用原生 Claude Code 启动。",r,g_font_subtitle,rgb(120,120,125),DT_LEFT|DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS);
    r=mkrect(lx+sc(20),top+sc(68),lx+leftw-sc(20),top+sc(108));fill_round(dc,r,sc(11),rgb(247,247,249),rgb(232,232,235));
    r=mkrect(lx+sc(20),top+sc(108),lx+leftw-sc(20),top+sc(144));text_out(dc,L"项目",r,g_font_button,rgb(78,78,82),DT_LEFT|DT_VCENTER|DT_SINGLELINE);
    count[0]=0;uint_to_wstr((unsigned int)g_project_count,count,64);wcat(count,64,L" 个目录");
    r=mkrect(lx+sc(78),top+sc(108),lx+sc(170),top+sc(144));text_out(dc,count,r,g_font_subtitle,rgb(145,145,150),DT_LEFT|DT_VCENTER|DT_SINGLELINE);
    r=mkrect(lx+sc(20),top+sc(144),lx+leftw-sc(20),top+(compact_left?sc(286):sc(318)));fill_round(dc,r,sc(11),rgb(255,255,255),rgb(235,235,238));

    /* Right card uses the same inset, control heights and column gaps. */
    r=mkrect(rx+sc(20),top+sc(14),rx+rightw-sc(164),top+sc(42));text_out(dc,L"安装与连接",r,g_font_heading,rgb(29,29,31),DT_LEFT|DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS);
    r=mkrect(rx+rightw-sc(154),top+sc(16),rx+rightw-sc(20),top+sc(42));fill_round(dc,r,sc(13),g_installing?rgb(255,241,236):(g_claude_installed?rgb(235,247,238):rgb(240,240,243)),rgb(232,232,235));
    t=mkrect(r.left+sc(10),r.top,r.left+sc(27),r.bottom);text_out(dc,L"●",t,g_font_subtitle,g_installing?rgb(216,119,86):(g_claude_installed?rgb(53,145,78):rgb(150,150,155)),DT_CENTER|DT_VCENTER|DT_SINGLELINE);
    t=mkrect(r.left+sc(30),r.top,r.right-sc(8),r.bottom);text_out(dc,g_installing?L"正在处理":(g_claude_installed?L"已安装":L"尚未安装"),t,g_font_subtitle,rgb(65,65,68),DT_LEFT|DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS);

    r=mkrect(rx+sc(20),top+sc(46),rx+rightw-sc(20),top+sc(64));text_out(dc,L"Claude 程序位置",r,g_font_subtitle,rgb(120,120,125),DT_LEFT|DT_VCENTER|DT_SINGLELINE);
    r=mkrect(rx+sc(20),top+sc(66),rx+rightw-sc(20),top+sc(98));fill_round(dc,r,sc(10),rgb(247,247,249),rgb(235,235,238));
    t=mkrect(r.left+sc(12),r.top,r.right-sc(12),r.bottom);text_out(dc,g_claude_path_display,t,g_font_subtitle,g_claude_installed?rgb(55,55,58):rgb(140,140,145),DT_LEFT|DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS);

    r=mkrect(rx+sc(20),top+sc(98),rx+rightw-sc(20),top+sc(114));text_out(dc,L"网络环境",r,g_font_subtitle,rgb(100,100,105),DT_LEFT|DT_VCENTER|DT_SINGLELINE);
    r=mkrect(rx+sc(20),top+sc(138),rx+rightw-sc(20),top+sc(154));text_out(dc,g_network_status,r,g_font_subtitle,g_network_busy?rgb(216,119,86):rgb(125,125,130),DT_LEFT|DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS);
    r=mkrect(rx+sc(20),top+sc(154),rx+rightw-sc(20),top+sc(172));text_out(dc,L"自定义代理地址（仅自定义模式可编辑）",r,g_font_subtitle,rgb(120,120,125),DT_LEFT|DT_VCENTER|DT_SINGLELINE);
    r=mkrect(rx+sc(20),top+sc(172),rx+rightw-sc(20),top+sc(202));fill_round(dc,r,sc(9),rgb(247,247,249),rgb(232,232,235));
    r=mkrect(rx+sc(20),top+sc(206),rx+rightw-sc(20),top+sc(224));text_out(dc,L"安装来源（通常无需修改）",r,g_font_subtitle,rgb(120,120,125),DT_LEFT|DT_VCENTER|DT_SINGLELINE);
    r=mkrect(rx+sc(20),top+sc(224),rx+rightw-sc(20),top+sc(254));fill_round(dc,r,sc(9),rgb(247,247,249),rgb(232,232,235));

    if(g_installing){
        r=mkrect(rx+sc(20),top+sc(298),rx+rightw-sc(20),top+sc(302));fill_round(dc,r,sc(2),rgb(235,235,238),rgb(235,235,238));
        if(g_progress_value>0){int pw=(rightw-sc(40))*g_progress_value/100;RECT pr=mkrect(r.left,r.top,r.left+pw,r.bottom);fill_round(dc,pr,sc(2),rgb(216,119,86),rgb(216,119,86));}
        elapsed_text(GetTickCount64()-g_install_start,elapsed,32);status[0]=0;wcat(status,128,g_phase_text);wcat(status,128,L" · ");wcat(status,128,elapsed);
    } else wcopy(status,128,g_status_text);
    r=mkrect(rx+sc(20),top+sc(374),rx+sc(38),top+sc(394));text_out(dc,L"●",r,g_font_subtitle,(g_installing||g_network_busy)?rgb(216,119,86):rgb(145,145,150),DT_LEFT|DT_VCENTER|DT_SINGLELINE);
    r=mkrect(rx+sc(38),top+sc(374),rx+rightw-sc(20),top+sc(394));text_out(dc,status,r,g_font_subtitle,rgb(105,105,110),DT_LEFT|DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS);

    r=mkrect(m+sc(22),logy+sc(8),g_client_w-m-sc(242),logy+sc(36));text_out(dc,L"运行日志",r,g_font_heading,rgb(245,245,247),DT_LEFT|DT_VCENTER|DT_SINGLELINE);
    EndPaint(hwnd,&ps);
}

static LRESULT __stdcall wndproc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    switch (msg) {
        case WM_CREATE: {
            startup_log_write("WM_CREATE begin\r\n");
            g_main=hwnd;
            if(pGetDpiForWindow){UINT dpi=pGetDpiForWindow(hwnd);if(dpi>=96&&dpi<=480)g_main_dpi=(int)dpi;}g_dpi=g_main_dpi;
            /* Defer child control creation until after the window is sized,
               so first paint happens with real WM_SIZE values and avoids a
               flash of unsized/zero-size controls at spawn. */
            create_fonts();
            g_br_bg=CreateSolidBrush(rgb(245,245,247)); g_br_card=CreateSolidBrush(rgb(255,255,255)); g_br_input=CreateSolidBrush(rgb(247,247,249)); g_br_log=CreateSolidBrush(rgb(28,28,30));
            startup_log_write("WM_CREATE fonts and brushes ready\r\n");

            g_workspace=create_control(0,L"EDIT",g_cfg_workspace,WS_TABSTOP|ES_AUTOHSCROLL,0,0,0,0,IDC_WORKSPACE);
            g_browse=create_control(0,L"BUTTON",L"浏览…",WS_TABSTOP|BS_OWNERDRAW,0,0,0,0,IDC_BROWSE);
            g_projects=create_control(0,L"LISTBOX",L"",WS_TABSTOP|WS_VSCROLL|LBS_NOTIFY|LBS_NOINTEGRALHEIGHT|LBS_OWNERDRAWFIXED|LBS_HASSTRINGS,0,0,0,0,IDC_PROJECTS);
            g_refresh=create_control(0,L"BUTTON",L"刷新",WS_TABSTOP|BS_OWNERDRAW,0,0,0,0,IDC_REFRESH);
            g_launch=create_control(0,L"BUTTON",L"启动 Claude Code",WS_TABSTOP|BS_OWNERDRAW,0,0,0,0,IDC_LAUNCH);
            g_terminal=create_control(0,L"BUTTON",L"打开终端",WS_TABSTOP|BS_OWNERDRAW,0,0,0,0,IDC_TERMINAL);
            g_folder=create_control(0,L"BUTTON",L"打开文件夹",WS_TABSTOP|BS_OWNERDRAW,0,0,0,0,IDC_FOLDER);
            g_network_mode=create_control(0,L"COMBOBOX",L"",WS_TABSTOP|WS_VSCROLL|CBS_DROPDOWNLIST,0,0,0,0,IDC_NETWORK_MODE);
            pSendMessageW(g_network_mode,CB_ADDSTRING,0,(LPARAM)L"自动检测");pSendMessageW(g_network_mode,CB_ADDSTRING,0,(LPARAM)L"跟随系统");pSendMessageW(g_network_mode,CB_ADDSTRING,0,(LPARAM)L"直连");pSendMessageW(g_network_mode,CB_ADDSTRING,0,(LPARAM)L"自定义");pSendMessageW(g_network_mode,CB_SETCURSEL,(WPARAM)network_mode_index(),0);
            g_test_network=create_control(0,L"BUTTON",L"检测连接",WS_TABSTOP|BS_OWNERDRAW,0,0,0,0,IDC_TEST_NETWORK);
            g_proxy=create_control(0,L"EDIT",g_cfg_proxy,WS_TABSTOP|ES_AUTOHSCROLL,0,0,0,0,IDC_PROXY);
            g_url=create_control(0,L"EDIT",g_cfg_url,WS_TABSTOP|ES_AUTOHSCROLL,0,0,0,0,IDC_URL);
            g_install=create_control(0,L"BUTTON",L"安装 Claude Code",WS_TABSTOP|BS_OWNERDRAW,0,0,0,0,IDC_INSTALL);
            g_import_settings=create_control(0,L"BUTTON",L"导入配置…",WS_TABSTOP|BS_OWNERDRAW,0,0,0,0,IDC_IMPORT_SETTINGS);
            g_open_settings=create_control(0,L"BUTTON",L"打开配置…",WS_TABSTOP|BS_OWNERDRAW,0,0,0,0,IDC_OPEN_SETTINGS);
            g_model_wizard=create_control(0,L"BUTTON",L"模型配置…",WS_TABSTOP|BS_OWNERDRAW,0,0,0,0,IDC_MODEL_WIZARD);
            g_shortcut=create_control(0,L"BUTTON",L"创建桌面快捷方式",WS_TABSTOP|BS_OWNERDRAW,0,0,0,0,IDC_SHORTCUT);
            g_fongap_link=create_control(0,L"BUTTON",L"Fongap · www.fongap.com",WS_TABSTOP|BS_OWNERDRAW,0,0,0,0,IDC_FONGAP_LINK);
            g_log=create_control(0,L"EDIT",L"",WS_VSCROLL|ES_MULTILINE|ES_AUTOVSCROLL|ES_READONLY,0,0,0,0,IDC_LOG);
            startup_log_write("WM_CREATE controls created\r\n");
            apply_main_control_fonts();apply_main_edit_margins();
            pSendMessageW(g_log,EM_SETLIMITTEXT,2097152,0);
            update_claude_state(); startup_log_write("WM_CREATE Claude state checked\r\n"); set_status(L"就绪"); refresh_projects(); startup_log_write("WM_CREATE projects refreshed\r\n");
            update_network_controls(); set_phase(0,0,L"");
            append_log(L"欢迎使用 Claude Code Manager 1.0");
            append_log(g_claude_installed?L"Claude Code 已安装；可选择项目直接启动，或打开模型配置向导。":L"首次使用：先安装 Claude Code，再打开“模型配置向导”，最后选择项目启动。");
            append_log(L"不清楚的网络选项保持“自动检测”即可。");
            /* Desktop shortcut creation is now opt-in (button in the main
               window). Creating it silently on first launch can trip Windows
               SmartScreen prompts and confuse users about where the icon came
               from. */
            startup_log_write("WM_CREATE complete\r\n");
            return 0;
        }
        case WM_GETMINMAXINFO: {
            MINMAXINFO *mmi=(MINMAXINFO*)lp;
            g_dpi=g_main_dpi;
            if(mmi){
                mmi->ptMinTrackSize_x=sc(760);
                mmi->ptMinTrackSize_y=sc(600);
                return 0;
            }
            break;
        }
        case WM_DPICHANGED: {
            RECT *suggested=(RECT*)lp; int newdpi=(int)(wp & 0xFFFF);
            if(newdpi>=96 && newdpi<=480 && newdpi!=g_main_dpi){
                g_main_dpi=newdpi;g_dpi=g_main_dpi;create_fonts();apply_main_control_fonts();apply_main_edit_margins();
                if(suggested) SetWindowPos(hwnd,0,suggested->left,suggested->top,suggested->right-suggested->left,suggested->bottom-suggested->top,SWP_NOZORDER|SWP_NOACTIVATE);
                {RECT cr;GetClientRect(hwnd,&cr);layout_controls(cr.right-cr.left,cr.bottom-cr.top);}
                InvalidateRect(hwnd,0,TRUE);
            }
            return 0;
        }
        case WM_SIZE: {
            int width=(int)(lp & 0xFFFF), height=(int)((lp>>16)&0xFFFF);
            g_dpi=g_main_dpi;if(wp!=SIZE_MINIMIZED&&width>0&&height>0){g_cfg_main_client_width=(DWORD)MulDiv(width,96,g_main_dpi);g_cfg_main_client_height=(DWORD)MulDiv(height,96,g_main_dpi);}
            if(g_log) layout_controls(width,height);
            InvalidateRect(hwnd,0,FALSE); return 0;
        }
        case WM_PAINT: g_dpi=g_main_dpi; paint_main(hwnd); return 0;
        case WM_ERASEBKGND: return 1; /* suppress background erase to kill flicker */
        case WM_PRINTCLIENT: { /* respond cleanly to redraw requests (e.g. on DPI change) */
            if(!g_log) { RECT c; GetClientRect(hwnd,&c); layout_controls(c.right-c.left,c.bottom-c.top); }
            break;
        }
        case WM_TIMER:
            if(wp==1&&g_installing)InvalidateRect(hwnd,0,FALSE);
            return 0;
        case WM_APP_NETWORK_TEST_COMPLETE: {
            /* Re-enable test button and repaint on GUI thread */
            if(g_test_network) pEnableWindow(g_test_network, TRUE);
            InvalidateRect(hwnd, 0, FALSE);
            return 0;
        }
        case WM_MEASUREITEM: {
            MEASUREITEMSTRUCT *mi=(MEASUREITEMSTRUCT*)lp;
            g_dpi=g_main_dpi;if(mi && mi->CtlID==IDC_PROJECTS){mi->itemHeight=sc(34); return TRUE;} return FALSE;
        }
        case WM_DRAWITEM: {
            DRAWITEMSTRUCT *di=(DRAWITEMSTRUCT*)lp;
            if(!di) return FALSE;
            if(di->CtlType==ODT_BUTTON){draw_button(di); return TRUE;}
            if(di->CtlType==ODT_LISTBOX && di->CtlID==IDC_PROJECTS){g_dpi=g_main_dpi;draw_list_item(di);return TRUE;}
            return FALSE;
        }
        case WM_CTLCOLOREDIT: {
            HDC dc=(HDC)wp; HWND ctl=(HWND)lp;
            SetBkMode(dc,OPAQUE);
            if(ctl==g_log){SetTextColor(dc,rgb(220,220,224)); SetBkColor(dc,rgb(28,28,30)); return (LRESULT)g_br_log;}
            SetTextColor(dc,rgb(35,35,38)); SetBkColor(dc,rgb(247,247,249)); return (LRESULT)g_br_input;
        }
        case WM_CTLCOLORSTATIC: {
            HDC dc=(HDC)wp; HWND ctl=(HWND)lp;
            if(ctl==g_log){SetTextColor(dc,rgb(220,220,224)); SetBkColor(dc,rgb(28,28,30)); return (LRESULT)g_br_log;}
            break;
        }
        case WM_CTLCOLORLISTBOX: {
            HDC dc=(HDC)wp; SetTextColor(dc,rgb(35,35,38)); SetBkColor(dc,rgb(255,255,255)); return (LRESULT)g_br_card;
        }
        case WM_COMMAND: {
            int id=(int)(wp & 0xFFFF), code=(int)((wp>>16)&0xFFFF);
            if(id==IDC_BROWSE && code==BN_CLICKED) browse_workspace();
            else if(id==IDC_REFRESH && code==BN_CLICKED){save_config();refresh_projects();}
            else if(id==IDC_INSTALL && code==BN_CLICKED) start_install();
            else if(id==IDC_LAUNCH && code==BN_CLICKED){save_config();launch_in_console(TRUE);}
            else if(id==IDC_TERMINAL && code==BN_CLICKED){save_config();launch_in_console(FALSE);}
            else if(id==IDC_FOLDER && code==BN_CLICKED){save_config();open_project_folder();}
            else if(id==IDC_SHORTCUT && code==BN_CLICKED){save_config();start_shortcut(TRUE);}
            else if(id==IDC_TEST_NETWORK && code==BN_CLICKED) start_network_test();
            else if(id==IDC_IMPORT_SETTINGS && code==BN_CLICKED) start_import_settings();
            else if(id==IDC_OPEN_SETTINGS && code==BN_CLICKED) open_settings_file();
            else if(id==IDC_MODEL_WIZARD && code==BN_CLICKED) show_model_wizard();
            else if(id==IDC_FONGAP_LINK && code==BN_CLICKED) pShellExecuteW(g_main,L"open",L"https://www.fongap.com",0,0,SW_SHOWNORMAL);
            else if(id==IDC_NETWORK_MODE && code==CBN_SELCHANGE){network_mode_name(network_mode_index(),g_cfg_network_mode,32);update_network_controls();save_config();}
            else if(id==IDC_PROJECTS && code==LBN_DBLCLK){save_config();launch_in_console(TRUE);}
            return 0;
        }
        case WM_CLOSE:
            if(g_installing){pMessageBoxW(hwnd,L"任务仍在进行，请任务结束后再关闭。",L"ClaudeCodeManager",0x30);return 0;}
            save_config(); break;
        case WM_DESTROY:
            delete_fonts();delete_wizard_fonts();
            if(g_br_bg)DeleteObject(g_br_bg); if(g_br_card)DeleteObject(g_br_card); if(g_br_input)DeleteObject(g_br_input); if(g_br_log)DeleteObject(g_br_log);
            pPostQuitMessage(0); return 0;
    }
    return pDefWindowProcW(hwnd,msg,wp,lp);
}

void mainCRTStartup(void) {
    WNDCLASSEXW wc; HWND hwnd; MSG msg; int corner=DWMWCP_ROUND; DWORD cap=0x00FFFFFF, txt=0x001D1D1F, border=0x00E8E8EB;
    DWORD main_style=WS_OVERLAPPED|WS_CAPTION|WS_SYSMENU|WS_MINIMIZEBOX|WS_MAXIMIZEBOX|WS_THICKFRAME|WS_CLIPCHILDREN|WS_CLIPSIBLINGS;
    RECT initial,workarea;int initial_width,initial_height,max_width,max_height;
    WCHAR launcher_project[4096];
    if(extract_launcher_argument(L"--claude-launcher",launcher_project,4096)) ExitProcess(run_unified_console_launcher(TRUE,launcher_project));
    if(extract_launcher_argument(L"--terminal-launcher",launcher_project,4096)) ExitProcess(run_unified_console_launcher(FALSE,launcher_project));
    startup_log_open();
    startup_log_write("mainCRTStartup entered\r\n");
    if(!load_apis()) { startup_log_write("load_apis failed\r\n"); MessageBoxW(0,L"Failed to initialize Windows APIs.",L"ClaudeCodeManager",0x10); startup_log_close(); ExitProcess(2); }
    startup_log_write("Windows APIs ready\r\n");
    if(pSetProcessDpiAwarenessContext) pSetProcessDpiAwarenessContext((HANDLE)(LONG_PTR)-4);
    g_instance=(HINSTANCE)GetModuleHandleW(0); CoInitializeEx(0,2); load_config();
    g_main_dpi=(int)GetDpiForSystem();if(g_main_dpi<96||g_main_dpi>480)g_main_dpi=96;g_wizard_dpi=g_main_dpi;g_dpi=g_main_dpi;
    g_app_icon=LoadIconW(g_instance,(LPCWSTR)(ULONG_PTR)1);
    if(!g_app_icon)g_app_icon=pLoadIconW(0,IDI_APPLICATION);
    memset(&wc,0,sizeof(wc)); wc.cbSize=sizeof(wc); wc.lpfnWndProc=wndproc; wc.hInstance=g_instance; wc.hCursor=pLoadCursorW(0,IDC_ARROW);
    wc.hIcon=g_app_icon; wc.hIconSm=g_app_icon; wc.hbrBackground=g_br_bg; wc.lpszClassName=CLASS_NAME;
    startup_log_write("Registering window class\r\n");
    if(!pRegisterClassExW(&wc)) { startup_log_write("RegisterClassExW failed\r\n"); MessageBoxW(0,L"Unable to register the application window.",L"ClaudeCodeManager",0x10); startup_log_close(); ExitProcess(3); }
    wc.lpfnWndProc=wizard_wndproc; wc.lpszClassName=WIZARD_CLASS_NAME; wc.hbrBackground=0;
    if(!pRegisterClassExW(&wc)) { startup_log_write("Wizard RegisterClassExW failed\r\n"); MessageBoxW(0,L"Unable to register the model wizard window.",L"ClaudeCodeManager",0x10); startup_log_close(); ExitProcess(3); }
    initial=mkrect(0,0,sc((int)g_cfg_main_client_width),sc((int)g_cfg_main_client_height));AdjustWindowRectEx(&initial,main_style,FALSE,WS_EX_CONTROLPARENT);initial_width=initial.right-initial.left;initial_height=initial.bottom-initial.top;
    if(initial_width<sc(760))initial_width=sc(760);if(initial_height<sc(600))initial_height=sc(600);
    if(SystemParametersInfoW(SPI_GETWORKAREA,0,&workarea,0)){max_width=workarea.right-workarea.left-sc(24);max_height=workarea.bottom-workarea.top-sc(24);if(initial_width>max_width)initial_width=max_width;if(initial_height>max_height)initial_height=max_height;}
    hwnd=pCreateWindowExW(WS_EX_CONTROLPARENT,CLASS_NAME,L"Claude Code Manager 1.0",main_style,
        CW_USEDEFAULT,CW_USEDEFAULT,initial_width,initial_height,0,0,g_instance,0);
    if(!hwnd) { startup_log_write("CreateWindowExW failed\r\n"); MessageBoxW(0,L"Unable to create the application window. See %TEMP%\\ClaudeCodeManager-startup.log.",L"ClaudeCodeManager",0x10); startup_log_close(); ExitProcess(4); }
    startup_log_write("Main window created\r\n");
    pSendMessageW(hwnd,WM_SETICON,ICON_BIG,(LPARAM)g_app_icon); pSendMessageW(hwnd,WM_SETICON,ICON_SMALL,(LPARAM)g_app_icon);
    DwmSetWindowAttribute(hwnd,DWMWA_WINDOW_CORNER_PREFERENCE,&corner,sizeof(corner));
    DwmSetWindowAttribute(hwnd,DWMWA_CAPTION_COLOR,&cap,sizeof(cap)); DwmSetWindowAttribute(hwnd,DWMWA_TEXT_COLOR,&txt,sizeof(txt)); DwmSetWindowAttribute(hwnd,DWMWA_BORDER_COLOR,&border,sizeof(border));
    pShowWindow(hwnd,SW_SHOW); pUpdateWindow(hwnd);
    startup_log_write("Main window shown\r\n");
    startup_log_close();
    while(pGetMessageW(&msg,0,0,0)>0){if(g_wizard&&IsDialogMessageW(g_wizard,&msg))continue;pTranslateMessage(&msg);pDispatchMessageW(&msg);} CoUninitialize(); ExitProcess(0);
}
