/*
@brief		C++ Wrapper Windows Registry C API
@date		2020/3/31 kim,jk
*/
#ifndef __INCLUDE_WINDOWS_REGISTRY_HPP__
#define __INCLUDE_WINDOWS_REGISTRY_HPP__

#include <WinReg.h>
#include "ustring.hpp"

namespace nostd {

class registry {
public:
	registry(HKEY key);
	virtual ~registry();

	HKEY createSubKey(LPCTSTR subKey, const REGSAM samDesired=KEY_WRITE) const;
	HKEY openSubKey(LPCTSTR subKey, const bool writable=false) const;
	HKEY openSubKey(LPCTSTR subKey, const REGSAM samDesired) const;
	bool deleteSubKey(LPCTSTR subKey) const;
	void close();
	HKEY get() const;
	bool isValid() const;

	ustring getString(LPCTSTR name, LPCTSTR def=_T("")) const;	// def : default value
	bool setString(LPCTSTR name, LPCTSTR val) const;

	int getInt(LPCTSTR name, int def=0) const;
	bool setInt(LPCTSTR name, int val) const;

	bool deleteValue(LPCTSTR name) const;

	static registry classesRoot() { return registry(HKEY_CLASSES_ROOT); }
	static registry currentConfig() { return registry(HKEY_CURRENT_CONFIG); }
	static registry currentUser() { return registry(HKEY_CURRENT_USER); }
	static registry dynData() { return registry(HKEY_DYN_DATA); }
	static registry localMachine() { return registry(HKEY_LOCAL_MACHINE); }
	static registry performanceData() { return registry(HKEY_PERFORMANCE_DATA); }
	static registry users() { return registry(HKEY_USERS); }

private:
	bool isReservedKey() const;

	HKEY key_;
};

inline registry::registry(HKEY key):key_(key) {}

inline registry::~registry() {
	close();
}

inline void registry::close() {
	if(key_) {
		if(!isReservedKey())
			::RegCloseKey(key_);
		key_ = nullptr;
	}
}

inline bool registry::isReservedKey() const {
	if (	key_ == HKEY_CLASSES_ROOT
		||	key_ == HKEY_CURRENT_USER
		||	key_ == HKEY_LOCAL_MACHINE
		||	key_ == HKEY_USERS
		||	key_ == HKEY_PERFORMANCE_DATA
		||	key_ == HKEY_PERFORMANCE_TEXT
		||	key_ == HKEY_PERFORMANCE_NLSTEXT
		||	key_ == HKEY_CURRENT_CONFIG
		||	key_ == HKEY_DYN_DATA
		||	key_ == HKEY_CURRENT_USER_LOCAL_SETTINGS)
		return true;
	return false;
}

inline HKEY registry::createSubKey(LPCTSTR subKey, const REGSAM samDesired/*=KEY_WRITE*/) const {
	if(!subKey || !isValid())
		return nullptr;

	HKEY hSubKey = nullptr;
	if(::RegCreateKeyEx(key_, subKey, 0, nullptr, REG_OPTION_NON_VOLATILE, samDesired, nullptr, &hSubKey, nullptr) == ERROR_SUCCESS)
		return hSubKey;
	return nullptr;
}

inline HKEY registry::openSubKey(LPCTSTR subKey, const REGSAM samDesired) const {
	if(!subKey || !isValid())
		return nullptr;

	HKEY hSubKey = nullptr;
	LSTATUS status = ::RegOpenKeyEx(key_, subKey, 0, samDesired, &hSubKey);
	if(status == ERROR_FILE_NOT_FOUND)
		status = ::RegOpenKeyEx(key_, subKey, 0, samDesired|KEY_WOW64_64KEY, &hSubKey);

	if(status == ERROR_SUCCESS)
		return hSubKey;
	return nullptr;
}

inline bool registry::isValid() const {
	return key_ != nullptr;
}

inline HKEY registry::openSubKey(LPCTSTR subKey, const bool writable/*=false*/) const {
	const REGSAM samDesired = KEY_READ|(writable ? KEY_WRITE:0);
	return openSubKey(subKey, samDesired);
}

inline ustring registry::getString(LPCTSTR name, LPCTSTR def/* =_T("") */) const {
	ustring ret = def ? def:_T("");

	if(!name || !isValid())
		return ret;

	TCHAR buf[1024]={0,};
	DWORD buflen = sizeof(buf)/sizeof(TCHAR);
	DWORD type = REG_SZ;

	LSTATUS st = ::RegQueryValueEx(key_, name, nullptr, &type, (BYTE*)buf, &buflen);
	switch(st)
	{
	case ERROR_SUCCESS:	ret = buf;	break;
	case ERROR_MORE_DATA:
		{
			TCHAR* pBuf = new TCHAR[buflen];
			if(!pBuf) break;

			if(::RegQueryValueEx(key_, name, nullptr, &type, (BYTE*)pBuf, &buflen) == ERROR_SUCCESS)
				ret = pBuf;
			delete[] pBuf;
		}break;
	}

	return ret;
}

inline bool registry::setString(LPCTSTR name, LPCTSTR val) const {
	if(!name || !val || !isValid())
		return false;

	return ::RegSetValueEx(key_, name, 0, REG_SZ, (BYTE*)val, (DWORD)((_tcslen(val)+1)*sizeof(TCHAR)) ) == ERROR_SUCCESS;
}

inline int registry::getInt(LPCTSTR name, int def/*=0*/) const {
	if(!name || !isValid())
		return def;

	DWORD dVal = 0;
	DWORD type = REG_DWORD;
	DWORD dwLen = sizeof(dVal);
	if(::RegQueryValueEx(key_, name, nullptr, &type, (BYTE*)&dVal, &dwLen) == ERROR_SUCCESS)
		return (int)dVal;
	return def;
}

inline bool registry::setInt(LPCTSTR name, int val) const {
	if(!name || !isValid()) return false;
	return RegSetValueEx(key_, name, 0, REG_DWORD, (BYTE*)&val, sizeof(val)) == ERROR_SUCCESS;
}

inline HKEY registry::get() const {
	return key_;
}

inline bool registry::deleteSubKey(LPCTSTR subKey) const {
	if(!subKey || !isValid())
		return false;

	const REGSAM samDesired = KEY_ENUMERATE_SUB_KEYS|DELETE;
	registry key(openSubKey(subKey, samDesired));
	if(!key.isValid())
		return false;

	TCHAR szSubkey[1024]={0,};
	DWORD dwSubKeylen = 0; 
	bool bOK = true;
	while(bOK) {
		dwSubKeylen = sizeof(szSubkey)/sizeof(TCHAR);
		DWORD dwRtn = ::RegEnumKeyEx(key.get(), 0, szSubkey, &dwSubKeylen, nullptr, nullptr, nullptr, nullptr);

		if(dwRtn==ERROR_NO_MORE_ITEMS) {
			dwRtn = ::RegDeleteKey(key_, subKey);
			break;
		}
		else if(dwRtn==ERROR_SUCCESS) {
			bOK = key.deleteSubKey(szSubkey);
		}
		else break;
	}
	
	return true;
}

inline bool registry::deleteValue(LPCTSTR name) const {
	if(!name || !isValid())
		return false;
	return ::RegDeleteValue(key_, name) == ERROR_SUCCESS;
}

}	// namespace nostd

#endif // __INCLUDE_WINDOWS_REGISTRY_HPP__