/*!	@file
	@brief 共通設定ダイアログボックス、「強調キーワード」ページ

	@author Norio Nakatani
*/
/*
	Copyright (C) 1998-2001, Norio Nakatani
	Copyright (C) 2000, MIK
	Copyright (C) 2001, genta, MIK
	Copyright (C) 2002, YAZAKI, MIK, genta
	Copyright (C) 2003, KEITA
	Copyright (C) 2005, genta, Moca
	Copyright (C) 2006, ryoji
	Copyright (C) 2007, ryoji

	This source code is designed for sakura editor.
	Please contact the copyright holders to use this code for other purpose.
*/

#include "stdafx.h"
#include "sakura_rc.h"
#include "prop/CPropCommon.h"
#include "debug/Debug.h"
#include <windows.h>
#include <stdio.h>
#include <commctrl.h>
#include "dlg/CDlgOpenFile.h"
#include "dlg/CDlgInput1.h"
#include "global.h"
#include "io/CTextStream.h"
#include "util/shell.h"
#include "util/file.h"
using namespace std;

//@@@ 2001.02.04 Start by MIK: Popup Help
#if 1	//@@@ 2002.01.03 add MIK
#include "sakura.hh"
static const DWORD p_helpids[] = {	//10800
	IDC_BUTTON_ADDSET,				HIDC_BUTTON_ADDSET,			//キーワードセット追加
	IDC_BUTTON_DELSET,				HIDC_BUTTON_DELSET,			//キーワードセット削除
	IDC_BUTTON_ADDKEYWORD,			HIDC_BUTTON_ADDKEYWORD,		//キーワード追加
	IDC_BUTTON_EDITKEYWORD,			HIDC_BUTTON_EDITKEYWORD,	//キーワード編集
	IDC_BUTTON_DELKEYWORD,			HIDC_BUTTON_DELKEYWORD,		//キーワード削除
	IDC_BUTTON_IMPORT,				HIDC_BUTTON_IMPORT_KEYWORD,	//インポート
	IDC_BUTTON_EXPORT,				HIDC_BUTTON_EXPORT_KEYWORD,	//エクスポート
	IDC_CHECK_KEYWORDCASE,			HIDC_CHECK_KEYWORDCASE,		//キーワードの英大文字小文字区別
	IDC_COMBO_SET,					HIDC_COMBO_SET,				//強調キーワードセット名
	IDC_LIST_KEYWORD,				HIDC_LIST_KEYWORD,			//キーワード一覧
	IDC_BUTTON_KEYCLEAN		,		HIDC_BUTTON_KEYCLEAN,		//キーワード整理	// 2006.08.06 ryoji
	IDC_BUTTON_KEYSETRENAME,		HIDC_BUTTON_KEYSETRENAME,	//セットの名称変更	// 2006.08.06 ryoji
//	IDC_STATIC,						-1,
	0, 0
};
#else
static const DWORD p_helpids[] = {	//10800
	IDC_BUTTON_ADDSET,				10800,	//キーワードセット追加
	IDC_BUTTON_DELSET,				10801,	//キーワードセット削除
	IDC_BUTTON_ADDKEYWORD,			10802,	//キーワード追加
	IDC_BUTTON_EDITKEYWORD,			10803,	//キーワード編集
	IDC_BUTTON_DELKEYWORD,			10804,	//キーワード削除
	IDC_BUTTON_IMPORT,				10805,	//インポート
	IDC_BUTTON_EXPORT,				10806,	//エクスポート
	IDC_CHECK_KEYWORDCASE,			10810,	//キーワードの英大文字小文字区別
	IDC_COMBO_SET,					10830,	//強調キーワードセット名
	IDC_LIST_KEYWORD,				10840,	//キーワード一覧
//	IDC_STATIC,						-1,
	0, 0
};
#endif
//@@@ 2001.02.04 End

//	From Here Jun. 2, 2001 genta
/*!
	@param hwndDlg ダイアログボックスのWindow Handle
	@param uMsg メッセージ
	@param wParam パラメータ1
	@param lParam パラメータ2
*/
INT_PTR CALLBACK CPropCommon::DlgProc_PROP_KEYWORD(
	HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	return DlgProc( &CPropCommon::DispatchEvent_p7, hwndDlg, uMsg, wParam, lParam );
}
//	To Here Jun. 2, 2001 genta

/* p7 メッセージ処理 */
INT_PTR CPropCommon::DispatchEvent_p7(
	HWND	hwndDlg,	// handle to dialog box
	UINT	uMsg,		// message
	WPARAM	wParam,		// first message parameter
	LPARAM	lParam 		// second message parameter
)
{
	WORD				wNotifyCode;
	WORD				wID;
	HWND				hwndCtl;
	NMHDR*				pNMHDR;
	NM_UPDOWN*			pMNUD;
	int					idCtrl;
	int					nIndex1;
	int					i;
	static int			nListItemHeight;
	LV_COLUMN			lvc;
	LV_ITEM*			plvi;
	static HWND			hwndCOMBO_SET;
	static HWND			hwndLIST_KEYWORD;
	RECT				rc;
	CDlgInput1			cDlgInput1;
	wchar_t				szKeyWord[MAX_KEYWORDLEN + 1];
	DWORD				dwStyle;
	LV_DISPINFO*		plvdi;
	LV_KEYDOWN*			pnkd;

	switch( uMsg ){
	case WM_INITDIALOG:
		/* ダイアログデータの設定 p7 */
		SetData_p7( hwndDlg );
		// Modified by KEITA for WIN64 2003.9.6
		::SetWindowLongPtr( hwndDlg, DWLP_USER, lParam );

		/* コントロールのハンドルを取得 */
		hwndCOMBO_SET = ::GetDlgItem( hwndDlg, IDC_COMBO_SET );
		hwndLIST_KEYWORD = ::GetDlgItem( hwndDlg, IDC_LIST_KEYWORD );
		::GetWindowRect( hwndLIST_KEYWORD, &rc );
		lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
		lvc.fmt = LVCFMT_LEFT;
		lvc.cx = rc.right - rc.left;
		lvc.pszText = _T("");
		lvc.iSubItem = 0;
		ListView_InsertColumn( hwndLIST_KEYWORD, 0, &lvc );

		dwStyle = (DWORD)::GetWindowLong( hwndLIST_KEYWORD, GWL_STYLE );
		::SetWindowLong( hwndLIST_KEYWORD, GWL_STYLE, dwStyle | LVS_SHOWSELALWAYS );
//				(dwStyle & ~LVS_TYPEMASK) | dwView);


		/* コントロール更新のタイミング用のタイマーを起動 */
		::SetTimer( hwndDlg, 1, 300, NULL );

		return TRUE;

	case WM_NOTIFY:
		idCtrl = (int)wParam;
		pNMHDR = (NMHDR*)lParam;
		pnkd = (LV_KEYDOWN *)lParam;
		pMNUD  = (NM_UPDOWN*)lParam;
		plvdi = (LV_DISPINFO*)lParam;
		plvi = &plvdi->item;

		if( hwndLIST_KEYWORD == pNMHDR->hwndFrom ){
			switch( pNMHDR->code ){
			case NM_DBLCLK:
//				MYTRACE_A( "NM_DBLCLK     \n" );
				/* p7:リスト中で選択されているキーワードを編集する */
				p7_Edit_List_KeyWord( hwndDlg, hwndLIST_KEYWORD );
				return TRUE;
			case LVN_BEGINLABELEDIT:
#ifdef _DEBUG
				MYTRACE_A( "LVN_BEGINLABELEDIT\n" );
												MYTRACE_A( "	plvi->mask =[%xh]\n", plvi->mask );
												MYTRACE_A( "	plvi->iItem =[%d]\n", plvi->iItem );
												MYTRACE_A( "	plvi->iSubItem =[%d]\n", plvi->iSubItem );
				if (plvi->mask & LVIF_STATE)	MYTRACE_A( "	plvi->state =[%xf]\n", plvi->state );
												MYTRACE_A( "	plvi->stateMask =[%xh]\n", plvi->stateMask );
				if (plvi->mask & LVIF_TEXT)		MYTRACE_A( "	plvi->pszText =[%ls]\n", plvi->pszText );
												MYTRACE_A( "	plvi->cchTextMax=[%d]\n", plvi->cchTextMax );
				if (plvi->mask & LVIF_IMAGE)	MYTRACE_A( "	plvi->iImage=[%d]\n", plvi->iImage );
				if (plvi->mask & LVIF_PARAM)	MYTRACE_A( "	plvi->lParam=[%xh(%d)]\n", plvi->lParam, plvi->lParam );
#endif
				return TRUE;
			case LVN_ENDLABELEDIT:
#ifdef _DEBUG
				MYTRACE_A( "LVN_ENDLABELEDIT\n" );
												MYTRACE_A( "	plvi->mask =[%xh]\n", plvi->mask );
												MYTRACE_A( "	plvi->iItem =[%d]\n", plvi->iItem );
												MYTRACE_A( "	plvi->iSubItem =[%d]\n", plvi->iSubItem );
				if (plvi->mask & LVIF_STATE)	MYTRACE_A( "	plvi->state =[%xf]\n", plvi->state );
												MYTRACE_A( "	plvi->stateMask =[%xh]\n", plvi->stateMask );
				if (plvi->mask & LVIF_TEXT)		MYTRACE_A( "	plvi->pszText =[%ls]\n", plvi->pszText  );
												MYTRACE_A( "	plvi->cchTextMax=[%d]\n", plvi->cchTextMax );
				if (plvi->mask & LVIF_IMAGE)	MYTRACE_A( "	plvi->iImage=[%d]\n", plvi->iImage );
				if (plvi->mask & LVIF_PARAM)	MYTRACE_A( "	plvi->lParam=[%xh(%d)]\n", plvi->lParam, plvi->lParam );
#endif
				if( NULL == plvi->pszText ){
					return TRUE;
				}
				if( 0 < _tcslen( plvi->pszText ) ){
					if( MAX_KEYWORDLEN < _tcslen( plvi->pszText ) ){
						InfoMessage( hwndDlg, _T("キーワードの長さは%dバイトまでです。"), MAX_KEYWORDLEN );
						return TRUE;
					}
					/* ｎ番目のセットにキーワードを編集 */
					m_CKeyWordSetMgr.UpdateKeyWord(
						m_CKeyWordSetMgr.m_nCurrentKeyWordSetIdx,
						plvi->lParam,
						to_wchar(plvi->pszText)
					);
				}else{
					/* ｎ番目のセットのｍ番目のキーワードを削除 */
					m_CKeyWordSetMgr.DelKeyWord( m_CKeyWordSetMgr.m_nCurrentKeyWordSetIdx, plvi->lParam );
				}
				/* ダイアログデータの設定 p7 指定キーワードセットの設定 */
				SetData_p7_KeyWordSet( hwndDlg, m_CKeyWordSetMgr.m_nCurrentKeyWordSetIdx );

				ListView_SetItemState( hwndLIST_KEYWORD, plvi->iItem, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED );

				return TRUE;
			case LVN_KEYDOWN:
//				MYTRACE_A( "LVN_KEYDOWN\n" );
				switch( pnkd->wVKey ){
				case VK_DELETE:
					/* p7:リスト中で選択されているキーワードを削除する */
					p7_Delete_List_KeyWord( hwndDlg, hwndLIST_KEYWORD );
					break;
				case VK_SPACE:
					/* p7:リスト中で選択されているキーワードを編集する */
					p7_Edit_List_KeyWord( hwndDlg, hwndLIST_KEYWORD );
					break;
				}
				return TRUE;
			}
		}else{
			switch( pNMHDR->code ){
			case PSN_HELP:
				OnHelp( hwndDlg, IDD_PROP_KEYWORD );
				return TRUE;
			case PSN_KILLACTIVE:
#ifdef _DEBUG
				MYTRACE_A( "p7 PSN_KILLACTIVE\n" );
#endif
				/* ダイアログデータの取得 p7 */
				GetData_p7( hwndDlg );
				return TRUE;
//@@@ 2002.01.03 YAZAKI 最後に表示していたシートを正しく覚えていないバグ修正
			case PSN_SETACTIVE:
				m_nPageNum = ID_PAGENUM_KEYWORD;
				return TRUE;
			}
		}
		break;
	case WM_COMMAND:
		wNotifyCode = HIWORD(wParam);	/* 通知コード */
		wID = LOWORD(wParam);			/* 項目ID､ コントロールID､ またはアクセラレータID */
		hwndCtl = (HWND) lParam;		/* コントロールのハンドル */
		if( hwndCOMBO_SET == hwndCtl){
			switch( wNotifyCode ){
			case CBN_SELCHANGE:
				nIndex1 = ::SendMessageAny( hwndCOMBO_SET, CB_GETCURSEL, 0, 0 );
				/* ダイアログデータの設定 p7 指定キーワードセットの設定 */
				SetData_p7_KeyWordSet( hwndDlg, nIndex1 );
				return TRUE;
			}
		}else{
			switch( wNotifyCode ){
			/* ボタン／チェックボックスがクリックされた */
			case BN_CLICKED:
				switch( wID ){
				case IDC_BUTTON_ADDSET:	/* セット追加 */
					if( MAX_SETNUM <= m_CKeyWordSetMgr.m_nKeyWordSetNum ){
						InfoMessage( hwndDlg, _T("セットは%d個までしか登録できません。\n"), MAX_SETNUM );
						return TRUE;
					}
					/* モードレスダイアログの表示 */
					wcscpy( szKeyWord, L"" );
					//	Oct. 5, 2002 genta 長さ制限の設定を修正．バッファオーバーランしていた．
					if( !cDlgInput1.DoModal(
						CNormalProcess::Instance()->GetProcessInstance(),
						hwndDlg,
						_T("キーワードのセット追加"),
						_T("セット名を入力してください。"),
						MAX_SETNAMELEN,
						szKeyWord
						)
					){
						return TRUE;
					}
					if( 0 < wcslen( szKeyWord ) ){
						/* セットの追加 */
						m_CKeyWordSetMgr.AddKeyWordSet( szKeyWord, false );

						m_CKeyWordSetMgr.m_nCurrentKeyWordSetIdx = m_CKeyWordSetMgr.m_nKeyWordSetNum - 1;

						/* ダイアログデータの設定 p7 */
						SetData_p7( hwndDlg );
					}
					return TRUE;
				case IDC_BUTTON_DELSET:	/* セット削除 */
					nIndex1 = ::SendMessageAny( hwndCOMBO_SET, CB_GETCURSEL, 0, 0 );
					if( CB_ERR == nIndex1 ){
						return TRUE;
					}
					/* 削除対象のセットを使用しているファイルタイプを列挙 */
					static TCHAR		pszLabel[1024];
					_tcscpy( pszLabel, _T("") );
					for( i = 0; i < MAX_TYPES; ++i ){
						CTypeConfig type(i);
						// 2002/04/25 YAZAKI STypeConfig全体を保持する必要はないし、m_pShareDataを直接見ても問題ない。
						if( nIndex1 == m_Types_nKeyWordSetIdx[i][0]
						||  nIndex1 == m_Types_nKeyWordSetIdx[i][1]
						||  nIndex1 == m_Types_nKeyWordSetIdx[i][2]
						||  nIndex1 == m_Types_nKeyWordSetIdx[i][3]
						||  nIndex1 == m_Types_nKeyWordSetIdx[i][4]
						||  nIndex1 == m_Types_nKeyWordSetIdx[i][5]
						||  nIndex1 == m_Types_nKeyWordSetIdx[i][6]
						||  nIndex1 == m_Types_nKeyWordSetIdx[i][7]
						||  nIndex1 == m_Types_nKeyWordSetIdx[i][8]
						||  nIndex1 == m_Types_nKeyWordSetIdx[i][9] ){
							_tcscat( pszLabel, _T("・") );
							_tcscat( pszLabel, type->m_szTypeName );
							_tcscat( pszLabel, _T("（") );
							_tcscat( pszLabel, type->m_szTypeExts );
							_tcscat( pszLabel, _T("）") );
							_tcscat( pszLabel, _T("\n") );
						}
					}
					if( IDCANCEL == ::MYMESSAGEBOX_A(	hwndDlg, MB_OKCANCEL | MB_ICONQUESTION, GSTR_APPNAME_A,
						"「%ls」のセットを削除します。\nよろしいですか？\n削除しようとするセットは、以下のファイルタイプに割り当てられています。\n削除したセットは無効になります。\n\n%ls",
						m_CKeyWordSetMgr.GetTypeName( nIndex1 ),
						pszLabel
					) ){
						return TRUE;
					}
					/* 削除対象のセットを使用しているファイルタイプのセットをクリア */
					for( i = 0; i < MAX_TYPES; ++i ){
						// 2002/04/25 YAZAKI STypeConfig全体を保持する必要はない。
						for( int j = 0; j < MAX_KEYWORDSET_PER_TYPE; j++ ){
							if( nIndex1 == m_Types_nKeyWordSetIdx[i][j] ){
								m_Types_nKeyWordSetIdx[i][j] = -1;
							}
							else if( nIndex1 < m_Types_nKeyWordSetIdx[i][j] ){
								m_Types_nKeyWordSetIdx[i][j]--;
							}
						}
					}
					/* ｎ番目のセットを削除 */
					m_CKeyWordSetMgr.DelKeyWordSet( m_CKeyWordSetMgr.m_nCurrentKeyWordSetIdx );
					/* ダイアログデータの設定 p7 */
					SetData_p7( hwndDlg );
					return TRUE;
				case IDC_BUTTON_KEYSETRENAME: // キーワードセットの名称変更
					// モードレスダイアログの表示
					wcscpy( szKeyWord, m_CKeyWordSetMgr.GetTypeName( m_CKeyWordSetMgr.m_nCurrentKeyWordSetIdx ) );
					{
						BOOL bDlgInputResult = cDlgInput1.DoModal(
							CNormalProcess::Instance()->GetProcessInstance(),
							hwndDlg,
							_T("セットの名称変更"),
							_T("セット名を入力してください。"),
							MAX_SETNAMELEN,
							szKeyWord
						);
						if( !bDlgInputResult ){
							return TRUE;
						}
					}
					if( 0 < wcslen( szKeyWord ) ){
						m_CKeyWordSetMgr.SetTypeName( m_CKeyWordSetMgr.m_nCurrentKeyWordSetIdx, szKeyWord );

						// ダイアログデータの設定 p7
						SetData_p7( hwndDlg );
					}
					return TRUE;
				case IDC_CHECK_KEYWORDCASE:	/* キーワードの英大文字小文字区別 */
//					m_CKeyWordSetMgr.m_bKEYWORDCASEArr[ m_CKeyWordSetMgr.m_nCurrentKeyWordSetIdx ] = ::IsDlgButtonChecked( hwndDlg, IDC_CHECK_KEYWORDCASE );	//MIK 2000.12.01 case sense
					m_CKeyWordSetMgr.SetKeyWordCase(m_CKeyWordSetMgr.m_nCurrentKeyWordSetIdx, ::IsDlgButtonChecked( hwndDlg, IDC_CHECK_KEYWORDCASE ));			//MIK 2000.12.01 case sense
					return TRUE;
				case IDC_BUTTON_ADDKEYWORD:	/* キーワード追加 */
					/* ｎ番目のセットのキーワードの数を返す */
					if( !m_CKeyWordSetMgr.CanAddKeyWord( m_CKeyWordSetMgr.m_nCurrentKeyWordSetIdx ) ){
						InfoMessage( hwndDlg, _T("登録できるキーワード数が上限に達しています。\n") );
						return TRUE;
					}
					/* モードレスダイアログの表示 */
					wcscpy( szKeyWord, L"" );
					if( !cDlgInput1.DoModal( CNormalProcess::Instance()->GetProcessInstance(), hwndDlg, _T("キーワード追加"), _T("キーワードを入力してください。"), MAX_KEYWORDLEN, szKeyWord ) ){
						return TRUE;
					}
					if( 0 < wcslen( szKeyWord ) ){
						/* ｎ番目のセットにキーワードを追加 */
						if( 0 == m_CKeyWordSetMgr.AddKeyWord( m_CKeyWordSetMgr.m_nCurrentKeyWordSetIdx, szKeyWord ) ){
							// ダイアログデータの設定 p7 指定キーワードセットの設定
							SetData_p7_KeyWordSet( hwndDlg, m_CKeyWordSetMgr.m_nCurrentKeyWordSetIdx );
						}
					}
					return TRUE;
				case IDC_BUTTON_EDITKEYWORD:	/* キーワード編集 */
					/* p7:リスト中で選択されているキーワードを編集する */
					p7_Edit_List_KeyWord( hwndDlg, hwndLIST_KEYWORD );
					return TRUE;
				case IDC_BUTTON_DELKEYWORD:	/* キーワード削除 */
					/* p7:リスト中で選択されているキーワードを削除する */
					p7_Delete_List_KeyWord( hwndDlg, hwndLIST_KEYWORD );
					return TRUE;
				// From Here 2005.01.26 Moca
				case IDC_BUTTON_KEYCLEAN:
					p7_Clean_List_KeyWord( hwndDlg, hwndLIST_KEYWORD );
					return TRUE;
				// To Here 2005.01.26 Moca
				case IDC_BUTTON_IMPORT:	/* インポート */
					/* p7:リスト中のキーワードをインポートする */
					p7_Import_List_KeyWord( hwndDlg, hwndLIST_KEYWORD );
					return TRUE;
				case IDC_BUTTON_EXPORT:	/* エクスポート */
					/* p7:リスト中のキーワードをエクスポートする */
					p7_Export_List_KeyWord( hwndDlg, hwndLIST_KEYWORD );
					return TRUE;
				}
				break;	/* BN_CLICKED */
			}
		}
		break;	/* WM_COMMAND */

	case WM_TIMER:
		nIndex1 = ListView_GetNextItem( hwndLIST_KEYWORD, -1, LVNI_ALL | LVNI_SELECTED );
		if( -1 == nIndex1 ){
			::EnableWindow( ::GetDlgItem( hwndDlg, IDC_BUTTON_EDITKEYWORD ), FALSE );
			::EnableWindow( ::GetDlgItem( hwndDlg, IDC_BUTTON_DELKEYWORD ), FALSE );
		}else{
			::EnableWindow( ::GetDlgItem( hwndDlg, IDC_BUTTON_EDITKEYWORD ), TRUE );
			::EnableWindow( ::GetDlgItem( hwndDlg, IDC_BUTTON_DELKEYWORD ), TRUE );
		}
		break;

	case WM_DESTROY:
		::KillTimer( hwndDlg, 1 );
		break;

//@@@ 2001.02.04 Start by MIK: Popup Help
	case WM_HELP:
		{
			HELPINFO *p = (HELPINFO *)lParam;
			MyWinHelp( (HWND)p->hItemHandle, m_szHelpFile, HELP_WM_HELP, (ULONG_PTR)(LPVOID)p_helpids );	// 2006.10.10 ryoji MyWinHelpに変更に変更
		}
		return TRUE;
		/*NOTREACHED*/
		//break;
//@@@ 2001.02.04 End

//@@@ 2001.12.22 Start by MIK: Context Menu Help
	//Context Menu
	case WM_CONTEXTMENU:
		MyWinHelp( hwndDlg, m_szHelpFile, HELP_CONTEXTMENU, (ULONG_PTR)(LPVOID)p_helpids );	// 2006.10.10 ryoji MyWinHelpに変更に変更
		return TRUE;
//@@@ 2001.12.22 End

	}
	return FALSE;
}

/* p7:リスト中で選択されているキーワードを編集する */
void CPropCommon::p7_Edit_List_KeyWord( HWND hwndDlg, HWND hwndLIST_KEYWORD )
{
	int			nIndex1;
	LV_ITEM	lvi;
	wchar_t		szKeyWord[MAX_KEYWORDLEN + 1];
	CDlgInput1	cDlgInput1;

	nIndex1 = ListView_GetNextItem( hwndLIST_KEYWORD, -1, LVNI_ALL | LVNI_SELECTED );
	if( -1 == nIndex1 ){
		return;
	}
	lvi.mask = LVIF_PARAM;
	lvi.iItem = nIndex1;
	lvi.iSubItem = 0;
	ListView_GetItem( hwndLIST_KEYWORD, &lvi );

	/* ｎ番目のセットのｍ番目のキーワードを返す */
	wcscpy( szKeyWord, m_CKeyWordSetMgr.GetKeyWord( m_CKeyWordSetMgr.m_nCurrentKeyWordSetIdx, lvi.lParam ) );

	/* モードレスダイアログの表示 */
	if( !cDlgInput1.DoModal( CNormalProcess::Instance()->GetProcessInstance(), hwndDlg, _T("キーワード編集"), _T("キーワードを編集してください。"), MAX_KEYWORDLEN, szKeyWord ) ){
		return;
	}
	if( 0 < wcslen( szKeyWord ) ){
		/* ｎ番目のセットにキーワードを編集 */
		m_CKeyWordSetMgr.UpdateKeyWord(
			m_CKeyWordSetMgr.m_nCurrentKeyWordSetIdx,
			lvi.lParam,
			szKeyWord
		);
	}else{
		/* ｎ番目のセットのｍ番目のキーワードを削除 */
		m_CKeyWordSetMgr.DelKeyWord( m_CKeyWordSetMgr.m_nCurrentKeyWordSetIdx, lvi.lParam );
	}
	/* ダイアログデータの設定 p7 指定キーワードセットの設定 */
	SetData_p7_KeyWordSet( hwndDlg, m_CKeyWordSetMgr.m_nCurrentKeyWordSetIdx );

	ListView_SetItemState( hwndLIST_KEYWORD, nIndex1, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED );
	return;
}



/* p7:リスト中で選択されているキーワードを削除する */
void CPropCommon::p7_Delete_List_KeyWord( HWND hwndDlg, HWND hwndLIST_KEYWORD )
{
	int			nIndex1;
	LV_ITEM		lvi;

	nIndex1 = ListView_GetNextItem( hwndLIST_KEYWORD, -1, LVNI_ALL | LVNI_SELECTED );
	if( -1 == nIndex1 ){
		return;
	}
	lvi.mask = LVIF_PARAM;
	lvi.iItem = nIndex1;
	lvi.iSubItem = 0;
	ListView_GetItem( hwndLIST_KEYWORD, &lvi );
	/* ｎ番目のセットのｍ番目のキーワードを削除 */
	m_CKeyWordSetMgr.DelKeyWord( m_CKeyWordSetMgr.m_nCurrentKeyWordSetIdx, lvi.lParam );
	/* ダイアログデータの設定 p7 指定キーワードセットの設定 */
	SetData_p7_KeyWordSet( hwndDlg, m_CKeyWordSetMgr.m_nCurrentKeyWordSetIdx );
	ListView_SetItemState( hwndLIST_KEYWORD, nIndex1, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED );

	//キーワード数を表示する。
	DispKeywordCount( hwndDlg );

	return;
}


/* p7:リスト中のキーワードをインポートする */
/* p7:リスト中のキーワードをインポートする */
void CPropCommon::p7_Import_List_KeyWord( HWND hwndDlg, HWND hwndLIST_KEYWORD )
{
	CDlgOpenFile	cDlgOpenFile;
	TCHAR			szPath[_MAX_PATH + 1];
	TCHAR			szInitDir[_MAX_PATH + 1];
	bool			bAddError = false;

	// 2007.05.19 ryoji 他画面と同じようにインポート用フォルダ設定を使うようにした
	_tcscpy( szPath, _T("") );
	_tcscpy( szInitDir, m_pShareData->m_szIMPORTFOLDER );	/* インポート用フォルダ */
	/* ファイルオープンダイアログの初期化 */
	cDlgOpenFile.Create(
		CNormalProcess::Instance()->GetProcessInstance(),
		hwndDlg,
		_T("*.kwd"),
		szInitDir
	);
	if( !cDlgOpenFile.DoModal_GetOpenFileName( szPath ) ){
		return;
	}
	/* ファイルのフルパスを、フォルダとファイル名に分割 */
	/* [c:\work\test\aaa.txt] → [c:\work\test] + [aaa.txt] */
	::SplitPath_FolderAndFile( szPath, m_pShareData->m_szIMPORTFOLDER, NULL );
	_tcscat( m_pShareData->m_szIMPORTFOLDER, _T("\\") );

	CTextInputStream in(szPath);
	if(!in){
		ErrorMessage( hwndDlg, _T("ファイルを開けませんでした。\n\n%ts"), szPath );
		return;
	}
	while( in ){
		wstring szLine = in.ReadLineW();

		//コメント無視
		if( 2 < szLine.length() && 0 == auto_memcmp( szLine.c_str(), L"//", 2 )  )continue;
		
		//解析
		if( 0 < szLine.length() ){
			/* ｎ番目のセットにキーワードを追加 */
			int nRetValue = m_CKeyWordSetMgr.AddKeyWord( m_CKeyWordSetMgr.m_nCurrentKeyWordSetIdx, szLine.c_str() );
			if( 2 == nRetValue ){
				bAddError = true;
				break;
			}
		}
	}
	in.Close();

	if( bAddError ){
		ErrorMessage( hwndDlg, _T("キーワードの数が上限に達したため、いくつかのキーワードを追加できませんでした。") );
	}
	/* ダイアログデータの設定 p7 指定キーワードセットの設定 */
	SetData_p7_KeyWordSet( hwndDlg, m_CKeyWordSetMgr.m_nCurrentKeyWordSetIdx );
	return;
}


/* p7:リスト中のキーワードをエクスポートする */
void CPropCommon::p7_Export_List_KeyWord( HWND hwndDlg, HWND hwndLIST_KEYWORD )
{
	CDlgOpenFile	cDlgOpenFile;
	TCHAR			szPath[_MAX_PATH + 1];
	TCHAR			szInitDir[_MAX_PATH + 1];
	int				i;
	int				nKeyWordNum;

	// 2007.05.19 ryoji 他画面と同じようにインポート用フォルダ設定を使うようにした
	_tcscpy( szPath, _T("") );
	_tcscpy( szInitDir, m_pShareData->m_szIMPORTFOLDER );	/* インポート用フォルダ */
	/* ファイルオープンダイアログの初期化 */
	cDlgOpenFile.Create(
		CNormalProcess::Instance()->GetProcessInstance(),
		hwndDlg,
		_T("*.kwd"),
		szInitDir
	);
	if( !cDlgOpenFile.DoModal_GetSaveFileName( szPath ) ){
		return;
	}
//	MYTRACE_A( "%ls\n", szPath );
	/* ファイルのフルパスを、フォルダとファイル名に分割 */
	/* [c:\work\test\aaa.txt] → [c:\work\test] + [aaa.txt] */
	::SplitPath_FolderAndFile( szPath, m_pShareData->m_szIMPORTFOLDER, NULL );
	_tcscat( m_pShareData->m_szIMPORTFOLDER, _T("\\") );

	CTextOutputStream out(szPath);
	if(!out){
		ErrorMessage( hwndDlg, _T("ファイルを開けませんでした。\n\n%ts"), szPath );
		return;
	}
	out.WriteF( L"// " );
	out.WriteF( m_CKeyWordSetMgr.GetTypeName( m_CKeyWordSetMgr.m_nCurrentKeyWordSetIdx ) );
	out.WriteF( L"  キーワード定義ファイル" );
	out.WriteF( L"\n" );
	out.WriteF( L"\n" );

	m_CKeyWordSetMgr.SortKeyWord(m_CKeyWordSetMgr.m_nCurrentKeyWordSetIdx);	//MIK 2000.12.01 sort keyword

	/* ｎ番目のセットのキーワードの数を返す */
	nKeyWordNum = m_CKeyWordSetMgr.GetKeyWordNum( m_CKeyWordSetMgr.m_nCurrentKeyWordSetIdx );
	for( i = 0; i < nKeyWordNum; ++i ){
		/* ｎ番目のセットのｍ番目のキーワードを返す */
		m_CKeyWordSetMgr.GetKeyWord( m_CKeyWordSetMgr.m_nCurrentKeyWordSetIdx, i );
		out.WriteF( m_CKeyWordSetMgr.GetKeyWord( m_CKeyWordSetMgr.m_nCurrentKeyWordSetIdx, i ) );
		out.WriteF( L"\n" );
	}
	out.Close();

	/* ダイアログデータの設定 p7 指定キーワードセットの設定 */
	SetData_p7_KeyWordSet( hwndDlg, m_CKeyWordSetMgr.m_nCurrentKeyWordSetIdx );

	InfoMessage( hwndDlg, _T("ファイルへエクスポートしました。\n\n%ls"), szPath );

	return;
}


//! キーワードを整頓する
void CPropCommon::p7_Clean_List_KeyWord( HWND hwndDlg, HWND hwndLIST_KEYWORD )
{
	if( IDYES == ::MessageBoxA( hwndDlg, "現在の設定では強調キーワードとして表示できないキーワードを削除しますか？",
			GSTR_APPNAME_A, MB_YESNO | MB_ICONSTOP ) ){
		if( m_CKeyWordSetMgr.CleanKeyWords( m_CKeyWordSetMgr.m_nCurrentKeyWordSetIdx ) ){
		}
		SetData_p7_KeyWordSet( hwndDlg, m_CKeyWordSetMgr.m_nCurrentKeyWordSetIdx );
	}
}

/* ダイアログデータの設定 p7 */
void CPropCommon::SetData_p7( HWND hwndDlg )
{
	int		i;
//	LV_ITEM	lvi;
	HWND	hwndWork;
//	int		nIdx;
//	char*	pszWork;


	/* セット名コンボボックスの値セット */
	hwndWork = ::GetDlgItem( hwndDlg, IDC_COMBO_SET );
	::SendMessageAny( hwndWork, CB_RESETCONTENT, 0, 0 );  /* コンボボックスを空にする */
	if( 0 < m_CKeyWordSetMgr.m_nKeyWordSetNum ){
		for( i = 0; i < m_CKeyWordSetMgr.m_nKeyWordSetNum; ++i ){
			Combo_AddString( hwndWork, m_CKeyWordSetMgr.GetTypeName( i ) );
		}
		/* セット名コンボボックスのデフォルト選択 */
		::SendMessageAny( hwndWork, CB_SETCURSEL, (WPARAM)m_CKeyWordSetMgr.m_nCurrentKeyWordSetIdx, 0 );

		/* ダイアログデータの設定 p7 指定キーワードセットの設定 */
		SetData_p7_KeyWordSet( hwndDlg, m_CKeyWordSetMgr.m_nCurrentKeyWordSetIdx );
	}else{
		/* ダイアログデータの設定 p7 指定キーワードセットの設定 */
		SetData_p7_KeyWordSet( hwndDlg, -1 );
	}

	return;
}


/* ダイアログデータの設定 p7 指定キーワードセットの設定 */
void CPropCommon::SetData_p7_KeyWordSet( HWND hwndDlg, int nIdx )
{
	int		i;
	int		nNum;
	HWND	hwndList;
	LV_ITEM	lvi;

	ListView_DeleteAllItems( ::GetDlgItem( hwndDlg, IDC_LIST_KEYWORD ) );
	if( 0 <= nIdx ){
		::EnableWindow( ::GetDlgItem( hwndDlg, IDC_BUTTON_DELSET ), TRUE );
		::EnableWindow( ::GetDlgItem( hwndDlg, IDC_CHECK_KEYWORDCASE ), TRUE );
		::EnableWindow( ::GetDlgItem( hwndDlg, IDC_LIST_KEYWORD ), TRUE );
		::EnableWindow( ::GetDlgItem( hwndDlg, IDC_BUTTON_ADDKEYWORD ), TRUE );
		//	Jan. 29, 2005 genta キーワードセット切り替え直後はキーワードは未選択
		//	そのため有効にしてすぐにタイマーで無効になる．
		//	なのでここで無効にしておく．
		::EnableWindow( ::GetDlgItem( hwndDlg, IDC_BUTTON_EDITKEYWORD ), FALSE );
		::EnableWindow( ::GetDlgItem( hwndDlg, IDC_BUTTON_DELKEYWORD ), FALSE );
		::EnableWindow( ::GetDlgItem( hwndDlg, IDC_BUTTON_IMPORT ), TRUE );
		::EnableWindow( ::GetDlgItem( hwndDlg, IDC_BUTTON_EXPORT ), TRUE );
	}else{
		::CheckDlgButton( hwndDlg, IDC_CHECK_KEYWORDCASE, FALSE );

		::EnableWindow( ::GetDlgItem( hwndDlg, IDC_BUTTON_DELSET ), FALSE );
		::EnableWindow( ::GetDlgItem( hwndDlg, IDC_CHECK_KEYWORDCASE ), FALSE );
		::EnableWindow( ::GetDlgItem( hwndDlg, IDC_LIST_KEYWORD ), FALSE );
		::EnableWindow( ::GetDlgItem( hwndDlg, IDC_BUTTON_ADDKEYWORD ), FALSE );
		::EnableWindow( ::GetDlgItem( hwndDlg, IDC_BUTTON_EDITKEYWORD ), FALSE );
		::EnableWindow( ::GetDlgItem( hwndDlg, IDC_BUTTON_DELKEYWORD ), FALSE );
		::EnableWindow( ::GetDlgItem( hwndDlg, IDC_BUTTON_IMPORT ), FALSE );
		::EnableWindow( ::GetDlgItem( hwndDlg, IDC_BUTTON_EXPORT ), FALSE );
		return;
	}

	/* キーワードの英大文字小文字区別 */
	if( TRUE == m_CKeyWordSetMgr.GetKeyWordCase(nIdx) ){		//MIK 2000.12.01 case sense
		::CheckDlgButton( hwndDlg, IDC_CHECK_KEYWORDCASE, TRUE );
	}else{
		::CheckDlgButton( hwndDlg, IDC_CHECK_KEYWORDCASE, FALSE );
	}

	/* ｎ番目のセットのキーワードの数を返す */
	nNum = m_CKeyWordSetMgr.GetKeyWordNum( nIdx );
	hwndList = ::GetDlgItem( hwndDlg, IDC_LIST_KEYWORD );

	// 2005.01.25 Moca/genta リスト追加中は再描画を抑制してすばやく表示
	::SendMessageAny( hwndList, WM_SETREDRAW, FALSE, 0 );

	for( i = 0; i < nNum; ++i ){
		/* ｎ番目のセットのｍ番目のキーワードを返す */
		const TCHAR* pszKeyWord = to_tchar(m_CKeyWordSetMgr.GetKeyWord( nIdx, i ));

		lvi.mask = LVIF_TEXT | LVIF_PARAM;
		lvi.pszText = const_cast<TCHAR*>(pszKeyWord);
		lvi.iItem = i;
		lvi.iSubItem = 0;
		lvi.lParam	= i;
		ListView_InsertItem( hwndList, &lvi );

	}
	m_CKeyWordSetMgr.m_nCurrentKeyWordSetIdx = nIdx;

	// 2005.01.25 Moca/genta リスト追加完了のため再描画許可
	::SendMessageAny( hwndList, WM_SETREDRAW, TRUE, 0 );

	//キーワード数を表示する。
	DispKeywordCount( hwndDlg );

	return;
}



/* ダイアログデータの取得 p7 */
int CPropCommon::GetData_p7( HWND hwndDlg )
{
//	HWND	hwndResList;
//	int		i;
//	int		j;
//	int		k;

//@@@ 2002.01.03 YAZAKI 最後に表示していたシートを正しく覚えていないバグ修正
//	m_nPageNum = ID_PAGENUM_KEYWORD;


	return TRUE;
}

/* ダイアログデータの取得 p7 指定キーワードセットの取得 */
void CPropCommon::GetData_p7_KeyWordSet( HWND hwndDlg, int nIdx )
{
}

//キーワード数を表示する。
void CPropCommon::DispKeywordCount( HWND hwndDlg )
{
	HWND	hwndList;
	int		n;
	TCHAR szCount[ 256 ];

	hwndList = ::GetDlgItem( hwndDlg, IDC_LIST_KEYWORD );
	n = ListView_GetItemCount( hwndList );
	if( n < 0 ) n = 0;

	int		nAlloc;
	nAlloc = m_CKeyWordSetMgr.GetAllocSize( m_CKeyWordSetMgr.m_nCurrentKeyWordSetIdx );
	nAlloc -= m_CKeyWordSetMgr.GetKeyWordNum( m_CKeyWordSetMgr.m_nCurrentKeyWordSetIdx );
	nAlloc += m_CKeyWordSetMgr.GetFreeSize();
	
	auto_sprintf( szCount, _T("(最大 %d 文字, 登録数 %d, 空き %d 個)"), MAX_KEYWORDLEN, n, nAlloc );
	::SetWindowText( ::GetDlgItem( hwndDlg, IDC_STATIC_KEYWORD_COUNT ), szCount );
}


