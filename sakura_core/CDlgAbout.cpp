//	e$Id$
/*!	@file
	@brief バージョン情報ダイアログ

	@author Norio Nakatani
	@date	1998/3/13 作成
	$Revision$
*/
/*
	Copyright (C) 1998-2001, Norio Nakatani
	Copyright (C) 200, genta, jepro

	This source code is designed for sakura editor.
	Please contact the copyright holder to use this code for other purpose.
*/
#include "CDlgAbout.h"
//#include <windows.h>
//#include <stdio.h>
#include "sakura_rc.h" // 2002/2/10 aroka 復帰
//#include "debug.h"
//#include "etc_uty.h"

// バージョン情報 CDlgAbout.cpp	//@@@ 2002.01.07 add start MIK
#include "sakura.hh"
const DWORD p_helpids[] = {	//12900
	IDC_BUTTON_CONTACT,		HIDC_ABOUT_BUTTON_CONTACT,
	IDOK,					HIDOK_ABOUT,
	IDC_EDIT_ABOUT,			HIDC_ABOUT_EDIT_ABOUT,
//	IDC_STATIC_URL_UR,		12970,
//	IDC_STATIC_URL_ORG,		12971,
//	IDC_STATIC_UPDATE,		12972,
//	IDC_STATIC_VER,			12973,
//	IDC_STATIC,				-1,
	0, 0
};	//@@@ 2002.01.07 add end MIK

//	Nov. 7, 2000 genta URLとして表示する文字列
//const char orgURL[] = "http://hp.vector.co.jp/authors/VA016528/";
//const char urURL[] = "http://sakura-editor.sourceforge.net/";

//	From Here Feb. 7, 2002 genta
#if defined(__BORLANDC__)
#  define COMPILER_TYPE "B"
#else
#  if defined(__GNUG__)
#    define COMPILER_TYPE "G"
#  else
#    if defined(_MSC_VER)
#      define COMPILER_TYPE "V"
#    else
#      define COMPILER_TYPE "U"
#    endif
#  endif
#endif
//	To Here Feb. 7, 2002 genta

//	From Here Nov. 7, 2000 genta
/*!
	標準以外のメッセージを捕捉する
*/
BOOL CDlgAbout::DispatchEvent( HWND hWnd, UINT wMsg, WPARAM wParam, LPARAM lParam )
{
	BOOL result;
	result = CDialog::DispatchEvent( hWnd, wMsg, wParam, lParam );
	switch( wMsg ){
	case WM_SETCURSOR:
		//	カーソルがコントロール上に来た
		if( (HWND)wParam == GetDlgItem(hWnd, IDC_STATIC_URL_UR ) ){
			if( nCursorState != 1 ){
				nCursorState = 1;
				//	再描画させる必要がある
				::InvalidateRect( (HWND)wParam, NULL, TRUE );
			}
		}
		else if( (HWND)wParam == GetDlgItem(hWnd, IDC_STATIC_URL_ORG ) ){
			if( nCursorState != 2 ){
				nCursorState = 2;
				//	再描画させる必要がある
				::InvalidateRect( (HWND)wParam, NULL, TRUE );
			}
		}
		else {
			if( nCursorState != 0 ){
				nCursorState = 0;
				//	再描画させる必要がある
				::InvalidateRect( GetDlgItem(hWnd, IDC_STATIC_URL_UR ), NULL, TRUE );
				::InvalidateRect( GetDlgItem(hWnd, IDC_STATIC_URL_ORG ), NULL, TRUE );
			}
		}
		result = TRUE;
		break;
	case WM_CTLCOLORSTATIC:
		if( nCursorState == 1 &&
			(HWND)lParam == GetDlgItem(hWnd, IDC_STATIC_URL_UR ) ){
			::SetTextColor( (HDC)wParam, RGB(0,0,0xff) );
			result = (BOOL)(HBRUSH)GetStockObject(NULL_BRUSH);
		}
		else if( nCursorState == 2 &&
			(HWND)lParam == GetDlgItem(hWnd, IDC_STATIC_URL_ORG ) ){
			::SetTextColor( (HDC)wParam, RGB(0,0,0xff) );
			result = (BOOL)(HBRUSH)GetStockObject(NULL_BRUSH);
		}
		break;
	case WM_COMMAND:
		//	CDialog標準では拾えない部分
		switch( LOWORD(wParam) ){
		case IDC_STATIC_URL_UR:
		case IDC_STATIC_URL_ORG:
			//	Web Browserの起動
			{
				char buf[512];
				::GetWindowText( ::GetDlgItem( hWnd, LOWORD( wParam ) ), buf, 512 );
				ShellExecute( hWnd, NULL, buf, NULL, NULL, SW_SHOWNORMAL );
			}
			result = TRUE;
		}
		break;
	}
	return result;
}
//	To Here Nov. 7, 2000 genta

/* モーダルダイアログの表示 */
int CDlgAbout::DoModal( HINSTANCE hInstance, HWND hwndParent )
{
	return CDialog::DoModal( hInstance, hwndParent, IDD_ABOUT, NULL );
}

BOOL CDlgAbout::OnInitDialog( HWND hwndDlg, WPARAM wParam, LPARAM lParam )
{
	m_hWnd = hwndDlg;

	char			szMsg[2048];
	char			szFile[_MAX_PATH];
	WIN32_FIND_DATA	wfd;
	SYSTEMTIME		systimeL;

	/* この実行ファイルの情報 */
	::GetModuleFileName( ::GetModuleHandle( NULL ), szFile, sizeof( szFile ) );
	::FindFirstFile( szFile, &wfd );
	::FileTimeToLocalFileTime( &wfd.ftLastWriteTime, &wfd.ftLastWriteTime );
	::FileTimeToSystemTime( &wfd.ftLastWriteTime, &systimeL );
	/* バージョン情報 */
	//	Nov. 6, 2000 genta	Unofficial Releaseのバージョンとして設定
	//	Jun. 8, 2001 genta	GPL化に伴い、OfficialなReleaseとしての道を歩み始める
	//	Feb. 7, 2002 genta コンパイラ情報追加
	wsprintf( szMsg, "Ver. %d.%d.%d.%d " COMPILER_TYPE,
		HIWORD( m_pShareData->m_dwProductVersionMS ),
		LOWORD( m_pShareData->m_dwProductVersionMS ),
		HIWORD( m_pShareData->m_dwProductVersionLS ),
		LOWORD( m_pShareData->m_dwProductVersionLS )
	);
	::SetDlgItemText( m_hWnd, IDC_STATIC_VER, szMsg );

	/* 更新日情報 */
	wsprintf( szMsg, "Last Update: %d/%d/%d %02d:%02d:%02d",
		systimeL.wYear,
		systimeL.wMonth,
		systimeL.wDay,
		systimeL.wHour,
		systimeL.wMinute,
		systimeL.wSecond
	);
	::SetDlgItemText( m_hWnd, IDC_STATIC_UPDATE, szMsg );

	//	Nov. 7, 2000 genta カーソル位置の情報を保持
	nCursorState = 0;

	//	From Here Jun. 8, 2001 genta
	//	Edit Boxにメッセージを追加する．
	int desclen = ::LoadString( m_hInstance, IDS_ABOUT_DESCRIPTION, szMsg, sizeof( szMsg ) );
	if( desclen > 0 ){
		::SetDlgItemText( m_hWnd, IDC_EDIT_ABOUT, szMsg );
	}

	//	To Here Jun. 8, 2001 genta

	/* 基底クラスメンバ */
	return CDialog::OnInitDialog( m_hWnd, wParam, lParam );
}


BOOL CDlgAbout::OnBnClicked( int wID )
{
	switch( wID ){
	case IDC_BUTTON_CONTACT:
		/* 「原作者連絡先」のヘルプ */	//Jan. 12, 2001 jepro `作者'の前に`原'を付けた
		::WinHelp( m_hWnd, m_szHelpFile, HELP_CONTEXT, 8 );
		return TRUE;
//Jan. 12, 2001 JEPRO UR1.2.20.2 (Nov. 7, 2000) から以下のボタンは削除されているのでコメントアウトした
//	case IDC_BUTTON_DOWNLOAD:
//		/* 「最新バージョンのダウンロード」のヘルプ  */
//		::WinHelp( m_hWnd, m_szHelpFile, HELP_CONTEXT, 112 );
//		return TRUE;
	}
	/* 基底クラスメンバ */
	return CDialog::OnBnClicked( wID );
}

//@@@ 2002.01.18 add start
LPVOID CDlgAbout::GetHelpIdTable(void)
{
	return (LPVOID)p_helpids;
}
//@@@ 2002.01.18 add end

/*[EOF]*/
