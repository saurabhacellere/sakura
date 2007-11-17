#include "stdafx.h"
#include "CViewParser.h"
#include "CLayout.h"
#include "CEditView.h"
#include "CEditDoc.h"
#include "charcode.h"

/*
	カーソル直前の単語を取得 単語の長さを返します
	単語区切り
*/
int CViewParser::GetLeftWord( CNativeW* pcmemWord, int nMaxWordLen ) const
{
	const wchar_t*	pLine;
	CLogicInt		nLineLen;
	CLogicInt		nIdx;
	CLogicInt		nIdxTo;

	CNativeW	cmemWord;
	CLayoutInt	nCurLine;
	int			nCharChars;
	const CLayout* pcLayout;

	nCurLine = m_pEditView->GetCaret().GetCaretLayoutPos().GetY2();
	pLine = m_pEditView->m_pcEditDoc->m_cLayoutMgr.GetLineStr( nCurLine, &nLineLen, &pcLayout );
	if( NULL == pLine ){
//		return 0;
		nIdxTo = CLogicInt(0);
	}else{
		/* 指定された桁に対応する行のデータ内の位置を調べる Ver1 */
		nIdxTo = m_pEditView->LineColmnToIndex( pcLayout, m_pEditView->GetCaret().GetCaretLayoutPos().GetX2() );
	}
	if( 0 == nIdxTo || NULL == pLine ){
		if( nCurLine <= 0 ){
			return 0;
		}
		nCurLine--;
		pLine = m_pEditView->m_pcEditDoc->m_cLayoutMgr.GetLineStr( nCurLine, &nLineLen );
		if( NULL == pLine ){
			return 0;
		}
		if( WCODE::isLineDelimiter(pLine[nLineLen - 1]) ){
			return 0;
		}

		nCharChars = &pLine[nLineLen] - CNativeW::GetCharPrev( pLine, nLineLen, &pLine[nLineLen] );
		if( 0 == nCharChars ){
			return 0;
		}
		nIdxTo = nLineLen;
		nIdx = nIdxTo - CLogicInt(nCharChars);
	}
	else{
		nCharChars = &pLine[nIdxTo] - CNativeW::GetCharPrev( pLine, nLineLen, &pLine[nIdxTo] );
		if( 0 == nCharChars ){
			return 0;
		}
		nIdx = nIdxTo - CLogicInt(nCharChars);
	}

	if( 1 == nCharChars ){
		if( WCODE::isWordDelimiter(pLine[nIdx]) ){
			return 0;
		}
	}

	/* 現在位置の単語の範囲を調べる */
	CLayoutRange sRange;
	int nResult=m_pEditView->m_pcEditDoc->m_cLayoutMgr.WhereCurrentWord(
		nCurLine,
		nIdx,
		&sRange,
		&cmemWord,
		pcmemWord
	);
	if( nResult ){
		pcmemWord->AppendString( &pLine[nIdx], nCharChars );

		return pcmemWord->GetStringLength();
	}else{
		return 0;
	}
}


/*!
	キャレット位置の単語を取得
	単語区切り

	@param[out] pcmemWord キャレット位置の単語
	@return true: 成功，false: 失敗
	
	@date 2006.03.24 fon (CEditView::Command_SELECTWORDを流用)
*/
BOOL CViewParser::GetCurrentWord(
		CNativeW* pcmemWord
) const
{
	const CLayout*	pcLayout = m_pEditView->m_pcEditDoc->m_cLayoutMgr.SearchLineByLayoutY( m_pEditView->GetCaret().GetCaretLayoutPos().GetY2() );
	if( NULL == pcLayout ){
		return false;	/* 単語選択に失敗 */
	}

	/* 指定された桁に対応する行のデータ内の位置を調べる */
	CLogicInt		nIdx = m_pEditView->LineColmnToIndex( pcLayout, m_pEditView->GetCaret().GetCaretLayoutPos().GetX2() );

	/* 現在位置の単語の範囲を調べる */
	CLayoutRange sRange;
	int nResult=m_pEditView->m_pcEditDoc->m_cLayoutMgr.WhereCurrentWord(
		m_pEditView->GetCaret().GetCaretLayoutPos().GetY2(),
		nIdx,
		&sRange,
		pcmemWord,
		NULL
	);
	if( nResult ){
		return true;	/* 単語選択に成功 */
	}
	else {
		return false;	/* 単語選択に失敗 */
	}
}

