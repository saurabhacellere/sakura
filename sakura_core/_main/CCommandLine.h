﻿/*!	@file
	@brief コマンドラインパーサ ヘッダファイル

	@author aroka
	@date	2002/01/08 作成
*/
/*
	Copyright (C) 1998-2001, Norio Nakatani
	Copyright (C) 2000-2001, genta
	Copyright (C) 2002, aroka CControlTrayより分離
	Copyright (C) 2002, genta
	Copyright (C) 2005, D.S.Koba
	Copyright (C) 2007, ryoji

	This source code is designed for sakura editor.
	Please contact the copyright holder to use this code for other purpose.
*/

#pragma once

#include "global.h"
#include "EditInfo.h"
#include "GrepInfo.h"
#include "util/design_template.h"

/*-----------------------------------------------------------------------
クラスの宣言
-----------------------------------------------------------------------*/

/*!
	@brief コマンドラインパーサ クラス
*/
class CCommandLine  : public TSingleton<CCommandLine> {
	friend class TSingleton<CCommandLine>;
	CCommandLine();

	static int CheckCommandLine(
		LPWSTR	str,		//!< [in] 検証する文字列（先頭の-は含まない）
		WCHAR**	arg,		//!< [out] 引数がある場合はその先頭へのポインタ
		int*	arglen		//!< [out] 引数の長さ
	);

	/*!
		引用符で囲まれている数値を認識するようにする
		@date 2002.12.05 genta
	*/
	static int AtoiOptionInt(const WCHAR* arg){
		return ( arg[0] == L'"' || arg[0] == L'\'' ) ?
			_wtoi( arg + 1 ) : _wtoi( arg );
	}

// member accessor method
public:
	bool IsNoWindow() const {return m_bNoWindow;}
	bool IsWriteQuit() const {return m_bWriteQuit;}	// 2007.05.19 ryoji sakuext用に追加
	bool IsGrepMode() const {return m_bGrepMode;}
	bool IsGrepDlg() const {return m_bGrepDlg;}
	bool IsDebugMode() const {return m_bDebugMode;}
	bool IsViewMode() const {return m_bViewMode;}
	bool GetEditInfo(EditInfo* fi) const { *fi = m_fi; return true; }
	bool GetGrepInfo(GrepInfo* gi) const { *gi = m_gi; return true; }
	int GetGroupId() const {return m_nGroup;}	// 2007.06.26 ryoji
	LPCWSTR GetMacro() const{ return m_cmMacro.GetStringPtr(); }
	LPCWSTR GetMacroType() const{ return m_cmMacroType.GetStringPtr(); }
	LPCWSTR GetProfileName() const{ return m_cmProfile.GetStringPtr(); }
	bool IsSetProfile() const{ return m_bSetProfile; }
	void SetProfileName(LPCWSTR s){
		m_bSetProfile = true;
		m_cmProfile.SetString(s);
	}
	bool IsProfileMgr() { return m_bProfileMgr; }
	int GetFileNum(void) { return m_vFiles.size(); }
	const WCHAR* GetFileName(int i) { return i < GetFileNum() ? m_vFiles[i].c_str() : NULL; }
	void ClearFile(void) { m_vFiles.clear(); }
	void ParseCommandLine( LPCWSTR pszCmdLineSrc, bool bResponse = true );

// member valiables
private:
	bool		m_bGrepMode;		//! [out] TRUE: Grep Mode
	bool		m_bGrepDlg;			//  Grepダイアログ
	bool		m_bDebugMode;		
	bool		m_bNoWindow;		//! [out] TRUE: 編集Windowを開かない
	bool		m_bWriteQuit;		//! [out] TRUE: 設定を保存して終了	// 2007.05.19 ryoji sakuext用に追加
	bool		m_bProfileMgr;
	bool		m_bSetProfile;
	EditInfo	m_fi;				//!
	GrepInfo	m_gi;				//!
	bool		m_bViewMode;		//! [out] TRUE: Read Only
	int			m_nGroup;			//! グループID	// 2007.06.26 ryoji
	CNativeW	m_cmMacro;			//! [out] マクロファイル名／マクロ文
	CNativeW	m_cmMacroType;		//! [out] マクロ種別
	CNativeW	m_cmProfile;		//! プロファイル名
	std::vector<std::wstring> m_vFiles;	//!< ファイル名(複数)
};

///////////////////////////////////////////////////////////////////////
