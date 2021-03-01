#ifndef NOSTD_PROCESS_MODULE_HPP_
#define NOSTD_PROCESS_MODULE_HPP_

#include <WinBase.h>
#include <exception>

#include "ustring.hpp"
#include "path.hpp"

namespace nostd {
class process_module {
public:
    process_module(HMODULE handle):handle_(handle){}
    virtual ~process_module(){}

    // 현재 프로세스
    static process_module currentProcess();

    // 현재 모듈
    static process_module currentModule();

    // 현재 모듈의 전체경로
    const ustring& fileName();

    // 현재 모듈명
    const ustring& moduleName();

    // 현재 모듈의 경로
    const ustring directoryName();
private:
    HMODULE handle_;
    ustring filename_;
    ustring moduleName_;
};

inline process_module process_module::currentProcess() {
    return process_module(nullptr);
}

inline process_module process_module::currentModule() {
    HMODULE hm = nullptr;
    if (GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
        (LPCTSTR)&currentModule, &hm) == 0) {
        throw std::exception(_T("Failed to GetModulehandleEx"));
    }        

    return process_module(hm);
}

inline const ustring& process_module::fileName() {
    if (filename_.empty()) {
        TCHAR buf[MAX_PATH] = { 0, };
        GetModuleFileName(handle_, buf, sizeof(buf));
        filename_ = buf;
    }
    return filename_;
}

inline  const ustring& process_module::moduleName() {
    if (moduleName_.empty()) {
        moduleName_ = path::getFileName(fileName());
    }
    return moduleName_;
}

inline const ustring process_module::directoryName() {
    const ustring& filename = fileName();
    return path::getDirectoryName(filename);
}

}   // namespace nostd

#endif // NOSTD_PROCESS_MODULE_HPP_