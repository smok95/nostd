/*
@brief		filesystem library ( paths, files, directiries )
@date		2020/4/23  kim,jk
*/
#ifndef __NOSTD_FILESYSTEM_HPP__
#define __NOSTD_FILESYSTEM_HPP__

#include <WinBase.h>
#include <cassert>
#include "process_module.hpp"
#include "ustring.hpp"

namespace nostd {

class fs {
public:
	virtual ~fs(){}

	/*
	@brief	지정된 모듈의 경로
			ex. C:/Windows/system32/test.dll 이라면 C:/Windows/system32 리턴
	@param	hm	: 대상모듈handle, nullptr 지정시 excutable file of the current process
	@date	2020/04/23 kim,jk
	*/
	static ustring modulePathWithHandle(HMODULE hm);

	// 현재 모듈의 경로
	static ustring modulePath();

	// 현재 프로세스의 경로
	static ustring processPath();
};

inline ustring fs::modulePathWithHandle(HMODULE hm) {		
	process_module pm = process_module(hm);
	return path::getDirectoryName(pm.fileName());
}

inline ustring fs::processPath() {
	ustring filename = process_module::currentProcess().fileName();
	return path::getDirectoryName(filename);
}

inline ustring fs::modulePath() {
	ustring filename = process_module::currentModule().fileName();
	return path::getDirectoryName(filename);
}

} // namespace nostd
#endif // __NOSTD_FILESYSTEM_HPP__