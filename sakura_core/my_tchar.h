/*!	@file
	@brief my_icmp/my_stringのShift_JISとUnicodeを切り替えるための定義

	@author 
	@date 2003/2/17 新規登録
*/
/*
	This source code is designed for sakura editor.
	Please contact the copyright holder to use this code for other purpose.
*/
/*
	my_icmp/my_stringのShift_JISとUnicodeを切り替えるための定義
*/

#ifndef _MY_TCHAR_H__
#define _MY_TCHAR_H__
 
/* include files */
#include "my_icmp.h"
// #include "my_string.h"

	
/* ************ _UNICODE ************ */
#ifdef _UNICODE

 /* my_icmp */
#define  my_tmemicmp wmemicmp
#define  my_tcsicmp  wcsicmp
#define  my_tcsnicmp wcsnicmp

 /* CRT */
#ifndef _tmemcpy
#define _tmemcpy     wmemcpy
#endif

 
/* ********** MBCS or CBCS ********** */
#else

 /*  my_icmp */
#define  my_tmemicmp auto_memicmp
#define  my_tcsicmp  my_stricmp
#define  my_tcsnicmp my_strnicmp

 /* CRT */
#ifndef _tmemcpy
#define _tmemcpy     memcpy
#endif

#endif

#endif /* _MY_TCHAR_H__ */
/*[EOF]*/
