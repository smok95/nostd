#ifndef NOSTD_PATH_HPP__
#define NOSTD_PATH_HPP__

#include "ustring.hpp"

namespace nostd {

class path {
public:
    /*
    @brief  지정된 경로 문자열에서 확장자가 포함된 파일명 반환
    @param  path 경로 문자열
    @return 확장자가 포함된 파일명
    */
    static ustring getFileName(const ustring &path);

    /// <summary>
    /// 지정된 경로 문자열에서 확장자가 제외된 파일명 반환
    /// </summary>
    /// <param name="path">경로 문자열</param>
    /// <returns></returns>
    static ustring getFileNameWithoutExtension(const ustring& path);

    /// <summary>
    /// 지정된 경로 문자열에서 파일명을 제외한 경로만 반환
    /// </summary>
    /// <param name="path">경로 문자열</param>
    /// <returns>경로</returns>
    static ustring getDirectoryName(const ustring& path);
};

inline ustring path::getFileName(const ustring &path) {

    // 경로에서 파일명만 추출	
	size_t found = path.find_last_of(_T("/\\"));
	if(found != ustring::npos)
        return path.substr(found+1);

    // 구분자가 없는 경우 그대로 리턴
    return path;
}

inline ustring path::getFileNameWithoutExtension(const ustring& path) {
    ustring filename = getFileName(path);

    // 파일명에서 확장자 제거
    size_t found = filename.find_last_of(_T("."));
    if (found != ustring::npos)
        return filename.substr(0, found);

    // 확장자 구분문자가 없는 겨우 그대로 리턴
    return filename;
}

inline ustring path::getDirectoryName(const ustring& path) {
    // 경로에서 파일명 제거
    size_t found = path.find_last_of(_T("/\\"));
    if (found != ustring::npos)
        return path.substr(0, found);
    return _T("");
}

} // namespace nostd

#endif // NOSTD_PATH_HPP__