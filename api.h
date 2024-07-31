typedef HINTERNET(WINAPI* InternetOpenA_imported)(
    LPCSTR lpszAgent,
    DWORD  dwAccessType,
    LPCSTR lpszProxy,
    LPCSTR lpszProxyBypass,
    DWORD  dwFlags
);


typedef BOOL (WINAPI* HttpSendRequestA_imported)(
    HINTERNET hRequest,
    LPCSTR    lpszHeaders,
    DWORD     dwHeadersLength,
    LPVOID    lpOptional,
    DWORD     dwOptionalLength
);

typedef HINTERNET(WINAPI* HttpOpenRequestA_imported)(
    HINTERNET hConnect,
    LPCSTR    lpszVerb,
    LPCSTR    lpszObjectName,
    LPCSTR    lpszVersion,
    LPCSTR    lpszReferrer,
    LPCSTR    *lplpszAcceptTypes,
    DWORD     dwFlags,
    DWORD_PTR dwContext
);

typedef BOOL (WINAPI* HttpAddRequestHeadersA_imported)(
    HINTERNET hRequest,
    LPCSTR    lpszHeaders,
    DWORD     dwHeadersLength,
    DWORD     dwModifiers
);


typedef HINTERNET(WINAPI* InternetOpenUrlA_imported)(
    HINTERNET hInternet,
    LPCSTR    lpszUrl,
    LPCSTR    lpszHeaders,
    DWORD     dwHeadersLength,
    DWORD     dwFlags,
    DWORD_PTR dwContext
);


typedef BOOL(WINAPI* InternetReadFile_imported)(
    HINTERNET hFile,
    LPVOID    lpBuffer,
    DWORD     dwNumberOfBytesToRead,
    LPDWORD   lpdwNumberOfBytesRead
);

typedef HINTERNET(WINAPI* InternetConnectA_imported)(
    HINTERNET     hInternet,
    LPCSTR        lpszServerName,
    INTERNET_PORT nServerPort,
    LPCSTR        lpszUserName,
    LPCSTR        lpszPassword,
    DWORD         dwService,
    DWORD         dwFlags,
    DWORD_PTR     dwContext
);

typedef BOOL (WINAPI* InternetCloseHandle_imported)(
    HINTERNET hInternet
);
