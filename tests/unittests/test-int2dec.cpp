#include <gtest/gtest.h>

#include <limits>
#define NOMINMAX
#include <Windows.h>
#include <tchar.h>
#include "../../sakura_core/basis/primitive.h"
class CNativeA;
class CNativeW;
#include "../../sakura_core/util/string_ex2.h"

template <typename T>
void test_int2dec(T value, int lenAnswer, const wchar_t* strAnswer)
{
	wchar_t buff[64];
	int len = int2dec(value, buff);
	EXPECT_EQ(len, lenAnswer);
	EXPECT_STREQ(buff, strAnswer);
}

TEST(test, zero)
{
	test_int2dec<int32_t>(0, 1, L"0");
	test_int2dec<int64_t>(0, 1, L"0");
}

TEST(test, plus_minus_1)
{
	test_int2dec<int32_t>(1, 1, L"1");
	test_int2dec<int32_t>(-1, 2, L"-1");
	test_int2dec<int64_t>(1, 1, L"1");
	test_int2dec<int64_t>(-1, 2, L"-1");
}

TEST(test, max)
{
	test_int2dec<int32_t>(std::numeric_limits<int32_t>::max(), 10, L"2147483647");
	test_int2dec<int64_t>(std::numeric_limits<int64_t>::max(), 19, L"9223372036854775807");
}

TEST(test, min)
{
	test_int2dec<int32_t>(std::numeric_limits<int32_t>::min(), 11, L"-2147483648");
	test_int2dec<int64_t>(std::numeric_limits<int64_t>::min(), 20, L"-9223372036854775808");
}
