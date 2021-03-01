#ifndef __USTRING_H__
#define __USTRING_H__
#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <WTypes.h>
#include <tchar.h>
#include <memory>
#include <stdarg.h>
#include <algorithm>

#ifdef UNICODE
typedef std::wstring ustring;
typedef std::wistringstream uistringstream;
typedef std::wstringstream ustringstream;
#else
typedef std::string ustring;
typedef std::istringstream uistringstream;
typedef std::stringstream ustringstream;
#endif

typedef std::vector<ustring> ustrings;

class ustring_helper
{
	ustring& m_rOb;
public:
	ustring_helper(ustring& obj):m_rOb(obj){}
	
	/*	
	@brief		문자열 나누기
	@param		str		: 나눌 문자열
	@param		c		: 구분자
	@return		ustrings
	@date		2018/3/24 kim,jk
	*/
	static ustrings split(const TCHAR* str, TCHAR c=_T(' '))
	{
		ustrings result;
		do{
			const TCHAR* begin = str;
			while(*str != c && *str)
				str++;

			result.push_back(ustring(begin, str));
		} while (0 != *str++);
		return result;
	}

	static ustrings split(const ustring& str, TCHAR c=_T(' ')) {
		return split(str.c_str(), c);
	}

	// left trim
	ustring_helper& ltrim() {
		m_rOb.erase(0, m_rOb.find_first_not_of(_T(" \n\r\t")));
		return *this;
	}

	// right trim
	ustring_helper& rtrim(){
		m_rOb.erase(m_rOb.find_last_not_of(_T(" \n\r\t"))+1);		
		return *this;
	}
		
	ustring_helper& trim(){
		return ltrim().rtrim();
	}

	static ustring& trim(ustring& str){
		ustring_helper(str).trim();
		return str;
	}

	static void replace_all(ustring& str, const ustring& from, const ustring& to) {
		if(from.empty())
			return;
		size_t start_pos = 0;
		while((start_pos = str.find(from, start_pos)) != ustring::npos) {
			str.replace(start_pos, from.length(), to);
			start_pos += to.length();
		}
	}

	static ustring format(const ustring fmt, ...) {
		int final_n, n = ((int)fmt.size()) * 2;
		std::unique_ptr<TCHAR[]> formatted;
		va_list ap;
		while(true) {
			formatted.reset(new TCHAR[n]);
			_tcscpy_s(&formatted[0], sizeof(TCHAR)*n, fmt.c_str());
			va_start(ap, fmt);
			final_n = _vsntprintf_s(&formatted[0], n, _TRUNCATE, fmt.c_str(), ap);
			va_end(ap);

			if(final_n < 0 || final_n >= n)
				n += abs(final_n - n + 1);
			else
				break;
		}
		return ustring(formatted.get());
	}

	// 문자열비교
	static bool equals(const ustring& s1, const ustring& s2) {
		const ustring::size_type sz = s1.size();

		if(sz != s2.size())
			return false;
		for(ustring::size_type i=0; i<sz; i++) {
			if(s1[i] != s2[i])
				return false;
		}
		return true;
	}

	// 문자열 비교(ignore case)
	static bool iequals(const ustring& s1, const ustring& s2) {
		const ustring::size_type sz = s1.size();

		if(sz != s2.size())
			return false;
		for(ustring::size_type i=0; i<sz; i++) {
			if(toupper(s1[i]) != toupper(s2[i]))
				return false;
		}
		return true;
	}

	static void to_upper(ustring& s) {
		std::transform(s.begin(), s.end(), s.begin(), ::toupper);
	}

	static void to_lower(ustring& s) {
		std::transform(s.begin(), s.end(), s.begin(), ::tolower);
	}
};

typedef ustring_helper ustr_;

#endif /* __USTRING_H__ */