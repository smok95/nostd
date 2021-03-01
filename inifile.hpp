/*
@brief		Win32 ini function wrapper
@date		2020/4/2  kim,jk
*/
#ifndef __INI_FILE_WRAPPER_HPP__
#define __INI_FILE_WRAPPER_HPP__

#include <WinBase.h>
#include "ustring.hpp"
#include <cassert>

namespace nostd {

class inifile {
public:
	inifile(const char* file) { setFile(file); }
	inifile(const ustring& file) { setFile(file); }
	virtual ~inifile(){}

	void setFile(LPCTSTR file);
	void setFile(const ustring& file);
	
	const ustring& getFile() const;

	bool setString(LPCTSTR section, LPCTSTR key, LPCTSTR v) const;
	ustring getString(LPCTSTR section, LPCTSTR key, LPCTSTR def=_T("")) const;

	bool setInt(LPCTSTR section, LPCTSTR key, int v) const;
	int getInt(LPCTSTR section, LPCTSTR key, int def=0) const;

	bool eraseSection(LPCTSTR section) const;
	bool eraseKey(LPCTSTR section, LPCTSTR key) const;
	class section {
	public:
		section(const inifile& obj, LPCTSTR name):obj_(obj), name_(name) {}
		section(const inifile& obj, const ustring& name):obj_(obj), name_(name) {}

		const section& setString(LPCTSTR key, LPCTSTR v) const;
		ustring getString(LPCTSTR key, LPCTSTR def=_T("")) const;

		const section& setInt(LPCTSTR key, int v) const;
		int getInt(LPCTSTR key, int def=0) const;

		const section& erase() const;
		const section& eraseKey() const;
	private:
		ustring name_;
		const inifile& obj_;
	};

	const section getSection(LPCTSTR name) const;
	const section getSection(const ustring& name) const;

private:
	ustring file_;
};

inline void inifile::setFile(LPCTSTR file) {
	assert(file != nullptr);
	file_ = file ? file:"";
	assert(file_.length() > 0);
}

inline void inifile::setFile(const ustring& file) {
	setFile(file.c_str());
}

inline const ustring& inifile::getFile() const {
	return file_;
}

inline bool inifile::setString(LPCTSTR section, LPCTSTR key, LPCTSTR v) const {
	assert(section != nullptr);
	return WritePrivateProfileString(section, key, v, file_.c_str()) == TRUE;
}

inline ustring inifile::getString(LPCTSTR section, LPCTSTR key, LPCTSTR def/*=_T("")*/) const {
	assert(section != nullptr && key != nullptr);
	TCHAR buf[1024]={0,};
	GetPrivateProfileString(section, key, def, buf, sizeof(buf)/sizeof(TCHAR), file_.c_str());
	return buf[0]==0 ? _T(""):buf;
}

inline bool inifile::setInt(LPCTSTR section, LPCTSTR key, int v) const {
	TCHAR buf[100]={0,};
	_stprintf_s(buf, sizeof(buf)/sizeof(TCHAR), _T("%d"), v);
	return WritePrivateProfileString(section, key, buf, file_.c_str()) == TRUE;
}
inline int inifile::getInt(LPCTSTR section, LPCTSTR key, int def/*=0*/) const {
	return (int)GetPrivateProfileInt(section, key, def, file_.c_str());
}

inline bool inifile::eraseSection(LPCTSTR section) const {
	return setString(section, nullptr, nullptr);
}
inline bool inifile::eraseKey(LPCTSTR section, LPCTSTR key) const {
	return setString(section, key, nullptr);
}

typedef inifile::section _inisec;

inline const _inisec& _inisec::setString(LPCTSTR key, LPCTSTR v) const {
	obj_.setString(name_.c_str(), key, v);
	return *this;
}

inline ustring _inisec::getString(LPCTSTR key, LPCTSTR def/*=_T("")*/) const {
	return obj_.getString(name_.c_str(), key, def);
}

inline const _inisec& _inisec::setInt(LPCTSTR key, int v) const {
	obj_.setInt(name_.c_str(), key, v);
	return *this;
}

inline int _inisec::getInt(LPCTSTR key, int def/*=0*/) const {
	return obj_.getInt(name_.c_str(), key, def);
}

inline const _inisec inifile::getSection(LPCTSTR name) const { 
	return section(*this, name); 
}

inline const _inisec inifile::getSection(const ustring& name) const { 
	return section(*this, name); 
}

} // namespace nostd
#endif // __INI_FILE_WRAPPER_HPP__