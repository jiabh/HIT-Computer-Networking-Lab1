//
// Created by jiabh on 2021/10/26.
//

#include <cstring>
#include "util.h"

void Next(const char *T, size_t len_T, int *next)
{
	int i = 1;
	next[1] = 0;
	int j = 0;
	while (i < len_T)
	{
		if (j == 0 || T[i - 1] == T[j - 1])
		{
			i++;
			j++;
			next[i] = j;
		} else
		{
			j = next[j];
		}
	}
}

ssize_t KMP(const char *S, size_t len_S, const char *T, size_t len_T)
{
	/**
	 * 参考链接：KMP算法（快速模式匹配算法）C语言详解
	 * http://data.biancheng.net/view/180.html
	 */
	int next[len_T];
	Next(T, len_T, next);//根据模式串T,初始化next数组
	size_t i = 1;
	size_t j = 1;
	while (i <= len_S && j <= len_T)
	{
		//j==0:代表模式串的第一个字符就和当前测试的字符不相等；S[i-1]==T[j-1],如果对应位置字符相等，两种情况下，指向当前测试的两个指针下标i和j都向后移
		if (j == 0 || S[i - 1] == T[j - 1])
		{
			i++;
			j++;
		} else
		{
			j = next[j];//如果测试的两个字符不相等，i不动，j变为当前测试字符串的next值
		}
	}
	if (j > len_T)
	{//如果条件为真，说明匹配成功
		return (ssize_t) (i - len_T - 1);
	}
	return -1;
}

std::pair<size_t, std::vector<size_t>> split_str(char *src, const char *delimiter, const char *eos)
{
	std::vector<size_t> result;
	result.push_back(0);

	const size_t len_deli = strlen(delimiter);
	const size_t len_eos = strlen(eos);

	ssize_t len_src = KMP(src, -1, eos, len_eos);
	if (len_src < 0)
		return std::make_pair(0, result);


	size_t i_last = 0; // 最后一次找到的下标
	ssize_t delta_i; // 下一个分隔符距上一个分隔符的位置
	while ((delta_i = KMP(src + i_last, len_src - i_last, delimiter, len_deli)) != -1)
	{
		i_last += (size_t) delta_i + len_deli;
		result.push_back(i_last);
	}

	return std::make_pair(len_src, result);
}


std::string &trim(std::string &s, const std::string &pattern)
{
	/**
	 * 参考链接：C++ string的trim, split方法
	 * https://blog.csdn.net/Butterfly_Dreaming/article/details/10142443
	 */
	if (s.empty())
		return s;

	s.erase(0, s.find_first_not_of(pattern));
	s.erase(s.find_last_not_of(pattern) + 1);
	return s;
}
