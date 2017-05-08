#pragma once
#pragma comment(lib, "json_vc71_libmtd")
#include <json/json.h>
#define CURL_STATICLIB
#include <curl/curl.h>
#pragma comment(lib, "wldap32.lib")
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "libcurld.lib")
#include "LogManager.h"
class CCurlManager
{
	CURL *m_curl;
	CURLcode res;

	void MakeJsonString(const char * _message, char * _level, std::string& JsonContainer);
public:
	CCurlManager();
	~CCurlManager();

	void InitCurlManager();
	bool SendLogMessage(const char* _message, char* _level);
};

