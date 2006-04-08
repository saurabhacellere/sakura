//	$Id$
/*!	@file
	@brief BREGEXP Library Handler

	Perl5互換正規表現を扱うDLLであるBREGEXP.DLLを利用するためのインターフェース

	@author genta
	@date Jun. 10, 2001
	@date 2002/2/1 hor		ReleaseCompileBufferを適宜追加
	@date Jul. 25, 2002 genta 行頭条件を考慮した検索を行うように．(置換はまだ)
	@date 2003.05.22 かろと 正規な正規表現に近づける
	@date 2005.03.19 かろと リファクタリング。クラス内部を隠蔽。
*/
/*
	Copyright (C) 2001-2002, genta
	Copyright (C) 2002, novice, hor, Azumaiya
	Copyright (C) 2003, かろと
	Copyright (C) 2005, かろと
	Copyright (C) 2006, かろと

	This software is provided 'as-is', without any express or implied
	warranty. In no event will the authors be held liable for any damages
	arising from the use of this software.

	Permission is granted to anyone to use this software for any purpose, 
	including commercial applications, and to alter it and redistribute it 
	freely, subject to the following restrictions:

		1. The origin of this software must not be misrepresented;
		   you must not claim that you wrote the original software.
		   If you use this software in a product, an acknowledgment
		   in the product documentation would be appreciated but is
		   not required.

		2. Altered source versions must be plainly marked as such, 
		   and must not be misrepresented as being the original software.

		3. This notice may not be removed or altered from any source
		   distribution.
*/

#include "stdafx.h"
#include <stdio.h>
#include <string.h>
#include "CBregexp.h"
#include "etc_uty.h"

// Compile時、行頭置換(len=0)の時にダミー文字列(１つに統一) by かろと
const char CBregexp::m_tmpBuf[2] = "\0";

CBregexp::CBregexp() : m_pRegExp( NULL ),
	m_ePatType( PAT_NORMAL )	//	Jul, 25, 2002 genta
{
}

CBregexp::~CBregexp()
{
	//<< 2002/03/27 Azumaiya
	// 一応、クラスの終了時にコンパイルバッファを解放。
	DeinitDll();
	//>> 2002/03/27 Azumaiya
}

//	Jul. 5, 2001 genta 引数追加。ただし、ここでは使わない。
char *
CBregexp::GetDllName( char* str )
{
	return "BREGEXP.DLL";
}
/*!
	DLLの初期化

	関数のアドレスを取得してメンバに保管する．

	@retval 0 成功
	@retval 1 アドレス取得に失敗
*/
int CBregexp::InitDll(void)
{
	//	Apr. 15, 2002 genta
	//	CPPA.cpp を参考に設定を配列化した
	
	const ImportTable table[] = {
		{ &BMatch,		"BMatch" },
		{ &BSubst,		"BSubst" },
		{ &BTrans,		"BTrans" },
		{ &BSplit,		"BSplit" },
		{ &BRegfree, 	"BRegfree" },
		{ &BRegexpVersion,	"BRegexpVersion" },
		{ NULL, 0 }
	};
	
	if( ! RegisterEntries( table )){
		return 1;
	}
	// 2003.11.01 かろと 拡張した関数のアドレス取得
	BMatchEx = (BREGEXP_BMatchEx)GetProcAddress( GetInstance(), "BMatchEx" );
	BSubstEx = (BREGEXP_BSubstEx)GetProcAddress( GetInstance(), "BSubstEx" );
	
	return 0;
}

/*!
	BREGEXP構造体の解放
*/
int CBregexp::DeinitDll( void )
{
	ReleaseCompileBuffer();
	return 0;
}

/*! @brief 検索パターンが特定の検索パターンかチェックする
**
** @param[in] szPattern 検索パターン
**
** @retval 検索パターン文字列長
** 
** @date 2005.03.20 かろと 関数に切り出し
*/
int CBregexp::CheckPattern(const char *szPattern)
{
	static const char TOP_MATCH[] = "/^\\(*\\^/k";							//!< 行頭パターンのチェック用パターン
	static const char DOL_MATCH[] = "/\\\\\\$$/k";							//!< \$(行末パターンでない)チェック用パターン
	static const char BOT_MATCH[] = "/\\$\\)*$/k";							//!< 行末パターンのチェック用パターン
	static const char TAB_MATCH[] = "/^\\(*\\^\\$\\)*$/k";					//!< "^$"パターンかをチェック用パターン
	static const char LOOKAHEAD[] = "/\\(\\?[=]/k";							//!< "(?=" 先読み の存在チェックパターン
	BREGEXP*	sReg = NULL;					//!< コンパイル構造体
	char szMsg[80] = "";						//!< エラーメッセージ
	int nLen;									//!< 検索パターンの長さ
	const char *szPatternEnd;					//!< 検索パターンの終端

	m_ePatType = PAT_NORMAL;	//!<　ノーマルは確定
	nLen = strlen( szPattern );
	szPatternEnd = szPattern + nLen;
	// パターン種別の設定
	if( BMatch( TOP_MATCH, szPattern, szPatternEnd, &sReg, szMsg ) > 0 ) {
		// 行頭パターンにマッチした
		m_ePatType |= PAT_TOP;
	}
	BRegfree(sReg);
	sReg = NULL;
	if( BMatch( TAB_MATCH, szPattern, szPatternEnd, &sReg, szMsg ) > 0 ) {
		// 行頭行末パターンにマッチした
		m_ePatType |= PAT_TAB;
	}
	BRegfree(sReg);
	sReg = NULL;
	if( BMatch( DOL_MATCH, szPattern, szPatternEnd, &sReg, szMsg ) > 0 ) {
		// 行末の\$ にマッチした
		// PAT_NORMAL
	} else {
		BRegfree(sReg);
		sReg = NULL;
		if( BMatch( BOT_MATCH, szPattern, szPatternEnd, &sReg, szMsg ) > 0 ) {
			// 行末パターンにマッチした
			m_ePatType |= PAT_BOTTOM;
		} else {
			// その他
			// PAT_NORMAL
		}
	}
	BRegfree(sReg);
	sReg = NULL;
	
	if( BMatch( LOOKAHEAD, szPattern, szPattern + nLen, &sReg, szMsg ) > 0 ) {
		// 先読みパターンにマッチした
		m_ePatType |= PAT_LOOKAHEAD;
	}
	BRegfree(sReg);
	sReg = NULL;
	return (nLen);
}

/*! @brief ライブラリに渡すための検索・置換パターンを作成する
**
** @note szPattern2: == NULL:検索 != NULL:置換
** 
** @param[in] szPattern 検索パターン
** @param[in] szPattern2 置換パターン(NULLなら検索)
** @param[in] szAdd2	置換パターンの後ろに付け加えるパターン($1など) 
** @param[in] nOption	検索オプション
**
** @retval ライブラリに渡す検索パターンへのポインタを返す
** @note 返すポインタは、呼び出し側で delete すること
** 
** @date 2003.05.03 かろと 関数に切り出し
*/
char* CBregexp::MakePatternSub( const char* szPattern, const char* szPattern2, const char* szAdd2, int nOption ) 
{
	static const char DELIMITER = '\xFF';		//!< デリミタ
	int nLen;									//!< szPatternの長さ
	int nLen2;									//!< szPattern2 + szAdd2 の長さ

	// 検索パターン作成
	char *szNPattern;		//!< ライブラリ渡し用の検索パターン文字列
	char *pPat;				//!< パターン文字列操作用のポインタ

	nLen = strlen(szPattern);
	if (szPattern2 == NULL) {
		// 検索(BMatch)時
		szNPattern = new char[ nLen + 15 ];	//	15：「s///option」が余裕ではいるように。
		pPat = szNPattern;
		*pPat++ = 'm';
	} else {
		// 置換(BSubst)時
		nLen2 = strlen(szPattern2) + strlen(szAdd2);
		szNPattern = new char[ nLen + nLen2 + 15 ];
		pPat = szNPattern;
		*pPat++ = 's';
	}
	*pPat++ = DELIMITER;
	while (*szPattern != '\0') { *pPat++ = *szPattern++; }
	*pPat++ = DELIMITER;
	if (szPattern2 != NULL) {
		while (*szPattern2 != '\0') { *pPat++ = *szPattern2++; }
		while (*szAdd2 != '\0') { *pPat++ = *szAdd2++; }
		*pPat++ = DELIMITER;
	}
	*pPat++ = 'k';			// 漢字対応
	*pPat++ = 'm';			// 複数行対応(但し、呼び出し側が複数行対応でない)
	// 2006.01.22 かろと 論理逆なので bIgnoreCase -> optCaseSensitiveに変更
	if( !(nOption & optCaseSensitive) ) {		// 2002/2/1 hor IgnoreCase オプション追加 マージ：aroka
		*pPat++ = 'i';		// 大文字小文字を同一視(無視)する
	}
	// 2006.01.22 かろと 行単位置換のために、全域オプション追加
	if( (nOption & optGlobal) ) {
		*pPat++ = 'g';			// 全域(global)オプション、行単位の置換をする時に使用する
	}
	*pPat = '\0';
	return szNPattern;
}


/*! 
** 行末文字の意味がライブラリでは \n固定なので、
** これをごまかすために、ライブラリに渡すための検索・置換パターンを工夫する
**
** 行末文字($)が検索パターンの最後にあり、その直前が [\r\n] でない場合に、
** 行末文字($)の手前に ([\r\n]+)を補って、置換パターンに $(nParen+1)を補う
** というアルゴリズムを用いて、ごまかす。
**
** @note szPattern2: == NULL:検索 != NULL:置換
** 
** @param[in] szPattern 検索パターン
** @param[in] szPattern2 置換パターン(NULLなら検索)
** @param[in] nOption 検索オプション
**
** @retval ライブラリに渡す検索パターンへのポインタを返す
** @note 返すポインタは、呼び出し側で delete すること
**
** @date 2003.05.03 かろと 関数に切り出し
*/
char* CBregexp::MakePattern( const char* szPattern, const char* szPattern2, int nOption ) 
{
	static const char CRLF[] = "\r\n";			//!< 復帰・改行
	static const char CR[] = "\r";				//!< 復帰
	static const char LF[] = "\n";				//!< 改行
	static const char LFCR[] = "\n\r";			//!< 改行・復帰
	static const char BOT_SUBST[] = "s/\\$(\\)*)$/([\\\\r\\\\n]+)\\$$1/k";	//!< 行末パターンの置換用パターン
	int nLen;									//!< szPatternの長さ
	BREGEXP*	sReg = NULL;					//!< コンパイル構造体
	char szMsg[80] = "";						//!< エラーメッセージ
	char szAdd2[5] = "";						//!< 行末あり置換の $数字 格納用
	int nParens = 0;							//!< 検索パターン(szPattern)中の括弧の数(行末時に使用)
	char *szNPattern;							//!< 検索パターン

	nLen = CheckPattern( szPattern );
	if( (m_ePatType & PAT_BOTTOM) != 0 ) {
		bool bJustDollar = false;			// 行末指定の$のみであるフラグ($の前に \r\nが指定されていない)
		szNPattern = MakePatternSub(szPattern, NULL, NULL, nOption);
		int matched = BMatch( szNPattern, CRLF, CRLF+sizeof(CRLF)-1, &sReg, szMsg );
		if( matched >= 0 ) {
			// szNPatternが不正なパターン等のエラーでなかった
			// エラー時には sRegがNULLのままなので、sReg->nparensへのアクセスは不正
			nParens = sReg->nparens;			// オリジナルの検索文字列中の()の数を記憶
			if( matched > 0 ) {
				if( sReg->startp[0] == &CRLF[1] && sReg->endp[0] == &CRLF[1] ) {
					if( BMatch( NULL, CR, CR+sizeof(CR)-1, &sReg, szMsg ) > 0 && sReg->startp[0] == &CR[1] && sReg->endp[0] == &CR[1] ) {
						if( BMatch( NULL, LF, LF+sizeof(LF)-1, &sReg, szMsg ) > 0 && sReg->startp[0] == &LF[0] && sReg->endp[0] == &LF[0] ) {
							if( BMatch( NULL, LFCR, LFCR+sizeof(LFCR)-1, &sReg, szMsg ) > 0 && sReg->startp[0] == &LFCR[0] && sReg->endp[0] == &LFCR[0] ) {
								// 検索文字列は 行末($)のみだった
								bJustDollar = true;
							}
						}
					}
				}
			} else {
				if( BMatch( NULL, CR, CR+sizeof(CR)-1, &sReg, szMsg ) <= 0 ) {
					if( BMatch( NULL, LF, LF+sizeof(LF)-1, &sReg, szMsg ) <= 0 ) {
						if( BMatch( NULL, LFCR, LFCR+sizeof(LFCR)-1, &sReg, szMsg ) <= 0 ) {
							// 検索文字列は、文字＋行末($)だった
							bJustDollar = true;
						}
					}
				}
			}
			BRegfree(sReg);
			sReg = NULL;
		}
		delete [] szNPattern;

		if( bJustDollar == true || (m_ePatType & PAT_TAB) != 0 ) {
			// 行末指定の$ or 行頭行末指定 なので、検索文字列を置換
			if( BSubst( BOT_SUBST, szPattern, szPattern + nLen, &sReg, szMsg ) > 0 ) {
				szPattern = sReg->outp;
				if( szPattern2 != NULL ) {
					// 置換パターンもあるので、置換パターンの最後に $(nParens+1)を追加
					wsprintf( szAdd2, "$%d", nParens + 1 );
				}
			}
			// sReg->outp のポインタを参照しているので、sRegを解放するのは最後に
		}
	}

	szNPattern = MakePatternSub( szPattern, szPattern2, szAdd2, nOption );
	if( sReg != NULL ) {
		BRegfree(sReg);
	}
	return szNPattern;
}


/*!
	JRE32のエミュレーション関数．空の文字列に対して検索・置換を行うことにより
	BREGEXP構造体の生成のみを行う．

	@param[in] szPattern0	検索or置換パターン
	@param[in] szPattern1	置換後文字列パターン(検索時はNULL)
	@param[in] nOption		検索・置換オプション

	@retval true 成功
	@retval false 失敗
*/
bool CBregexp::Compile( const char *szPattern0, const char *szPattern1, int nOption )
{

	//	DLLが利用可能でないときはエラー終了
	if( !IsAvailable() )
		return false;

	//	BREGEXP構造体の解放
	ReleaseCompileBuffer();

	// ライブラリに渡す検索パターンを作成
	// 別関数で共通処理に変更 2003.05.03 by かろと
	char *szNPattern = MakePattern( szPattern0, szPattern1, nOption );
	m_szMsg[0] = '\0';		//!< エラー解除
	if (szPattern1 == NULL) {
		// 検索実行
		BMatch( szNPattern, m_tmpBuf, m_tmpBuf+1, &m_pRegExp, m_szMsg );
	} else {
		// 置換実行
		BSubst( szNPattern, m_tmpBuf, m_tmpBuf+1, &m_pRegExp, m_szMsg );
	}
	delete [] szNPattern;

	//	メッセージが空文字列でなければ何らかのエラー発生。
	//	サンプルソース参照
	if( m_szMsg[0] ){
		ReleaseCompileBuffer();
		return false;
	}
	
	// 行頭条件チェックは、MakePatternに取り込み 2003.05.03 by かろと

	return true;
}

/*!
	JRE32のエミュレーション関数．既にあるコンパイル構造体を利用して検索（1行）を
	行う．

	@param[in] target 検索対象領域先頭アドレス
	@param[in] len 検索対象領域サイズ
	@param[in] nStart 検索開始位置．(先頭は0)

	@retval true Match
	@retval false No Match または エラー。エラーは GetLastMessage()により判定可能。

*/
bool CBregexp::Match( const char* target, int len, int nStart )
{
	int matched;		//!< 検索一致したか? >0:Match, 0:NoMatch, <0:Error

	//	DLLが利用可能でないとき、または構造体が未設定の時はエラー終了
	if( (!IsAvailable()) || m_pRegExp == NULL ){
		return false;
	}

	m_szMsg[0] = '\0';		//!< エラー解除
	// 拡張関数がない場合は、行の先頭("^")の検索時の特別処理 by かろと
	if (BMatchEx == NULL) {
		/*
		** 行頭(^)とマッチするのは、nStart=0の時だけなので、それ以外は false
		*/
		if( (m_ePatType & PAT_TOP) != 0 && nStart != 0 ) {
			// nStart!=0でも、BMatch()にとっては行頭になるので、ここでfalseにする必要がある
			return false;
		}
		//	検索文字列＝NULLを指定すると前回と同一の文字列と見なされる
		matched = BMatch( NULL, target + nStart, target + len, &m_pRegExp, m_szMsg );
	} else {
		//	検索文字列＝NULLを指定すると前回と同一の文字列と見なされる
		matched = BMatchEx( NULL, target, target + nStart, target + len, &m_pRegExp, m_szMsg );
	}
	m_szTarget = target;
			
	if ( matched < 0 || m_szMsg[0] ) {
		// BMatchエラー
		// エラー処理をしていなかったので、nStart>=lenのような場合に、マッチ扱いになり
		// 無限置換等の不具合になっていた 2003.05.03 by かろと
		return false;
	} else if ( matched == 0 ) {
		// 一致しなかった
		return false;
	}

	return true;
}


//<< 2002/03/27 Azumaiya
/*!
	正規表現による文字列置換
	既にあるコンパイル構造体を利用して置換（1行）を
	行う．

	@param[in] szTarget 置換対象データ
	@param[in] nLen 置換対象データ長
	@param[in] nStart 置換開始位置(0からnLen未満)

	@retval true 成功
	@retval false 失敗
*/
bool CBregexp::Replace(const char *szTarget, int nLen, int nStart)
{
	int result;
	//	DLLが利用可能でないとき、または構造体が未設定の時はエラー終了
	if( !IsAvailable() || m_pRegExp == NULL )
	{
		return false;
	}

	//	From Here 2003.05.03 かろと
	// nLenが０だと、BSubst()が置換に失敗してしまうので、代用データ(m_tmpBuf)を使う
	if( nLen == 0 ) {
		szTarget = m_tmpBuf;
		nLen = 1;
	}
	//	To Here 2003.05.03 かろと

	m_szMsg[0] = '\0';		//!< エラー解除
	if (BSubstEx == NULL) {
		result = BSubst( NULL, szTarget + nStart, szTarget + nLen, &m_pRegExp, m_szMsg );
	} else {
		result = BSubstEx( NULL, szTarget, szTarget + nStart, szTarget + nLen, &m_pRegExp, m_szMsg );
	}
	m_szTarget = szTarget;

	//	メッセージが空文字列でなければ何らかのエラー発生。
	//	サンプルソース参照
	if( m_szMsg[0] ) {
		return false;
	}

	if( !result ) {
		// 置換するものがなかった
		return false;
	}
	return true;
}
//>> 2002/03/27 Azumaiya

/*[EOF]*/
