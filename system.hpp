/*
@brief		
@date		2020/05/04 kim,jk
*/
#ifndef __NOSTD_SYSTEM_HPP__
#define __NOSTD_SYSTEM_HPP__

#include <WinNT.h>
#include <process.h>
#include "ustring.hpp"

namespace nostd {

class process {
public:
	process(){};
	virtual ~process(){};

	enum PIL { // Process Integrity Level
		PIL_ERROR = 0,
		PIL_LOW,		// Low Integrity
		PIL_MEDIUM,		// Medium Integrity
		PIL_HIGH		// High Integrity
	};

	/*
	@brief		지정된 프로세스의 무결성수준(Integrity Level) 확인
	@param		pid	: 확인할 프로세스의 ID
	@returns	PIL값, PIL_ERROR 리턴시에는 GetLastError()로 오류코드 확인
	@date		2020/05/04 kim,jk
	*/
	static PIL integrityLevel(DWORD pid);

	/*
	@brief		현재 프로세스의 무결성수준(Integrity Level) 확인
	@returns	PIL값, PIL_ERROR 리턴시에는 GetLastError()로 오류코드 확인
	@date		2020/05/04 kim,jk
	*/
	static PIL curIntegrityLevel();
};

inline process::PIL process::integrityLevel(DWORD pid) {
	HANDLE hToken = nullptr;
	LPWSTR pStrSid = nullptr;
	DWORD dwIL;	// TokenIntegrityLevel

	PIL level = PIL_ERROR;

	HANDLE hProc = OpenProcess(PROCESS_QUERY_INFORMATION|PROCESS_VM_READ, FALSE, pid);
	if(!hProc)
		goto _cleanup;

	if(!OpenProcessToken(hProc, TOKEN_QUERY|TOKEN_QUERY_SOURCE, &hToken))
		goto _cleanup;

	DWORD dwLen = 0;

	// 무결성 수준을 가져온다.
	if(!GetTokenInformation(hToken, TokenIntegrityLevel, nullptr, 0, &dwLen)) {
		DWORD err = GetLastError();
		if(err == ERROR_INSUFFICIENT_BUFFER) {
			if(PTOKEN_MANDATORY_LABEL pTML = (PTOKEN_MANDATORY_LABEL)LocalAlloc(0, dwLen)) {
				if(GetTokenInformation(hToken, TokenIntegrityLevel, pTML, dwLen, &dwLen)) {
					const DWORD nSubAuth = (DWORD)(UCHAR)(*GetSidSubAuthorityCount(pTML->Label.Sid)-1);
					dwIL = *GetSidSubAuthority(pTML->Label.Sid, nSubAuth);

					if(dwIL < SECURITY_MANDATORY_MEDIUM_RID)
						level = PIL_LOW;
					else if(dwIL >= SECURITY_MANDATORY_MEDIUM_RID && dwIL < SECURITY_MANDATORY_HIGH_RID)
						level = PIL_MEDIUM;
					else if(dwIL >= SECURITY_MANDATORY_HIGH_RID)
						level = PIL_HIGH;
				}
				LocalFree(pTML);
			}
		}
	}

_cleanup:

	if(hToken)
		CloseHandle(hToken);

	if(hProc)
		CloseHandle(hProc);

	return level;
}

inline process::PIL process::curIntegrityLevel() {
	
	return integrityLevel((DWORD)_getpid());
}

}	// namespace nostd

#endif // __NOSTD_SYSTEM_HPP__