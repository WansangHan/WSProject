#ifndef CURLMANAGER_H
#define CURLMANAGER_H
#pragma comment(lib, "json_vc71_libmtd")
#include <json/json.h>
#define CURL_STATICLIB
#include <curl/curl.h>
#pragma comment(lib, "wldap32.lib")
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "libcurld.lib")
#include "LogManager.h"
#include <memory>
#include <thread>
#include <mutex>
struct strres {
	char *ptr;
	size_t len;
};
class CCurlManager
{
	static std::unique_ptr<CCurlManager> m_inst;
	static std::once_flag m_once;

	CURL *m_curl;

	char* SendCurlMessage(const char* _url, std::string _JsonContainer);
	CCurlManager();
public:
	static CCurlManager& getInstance()
	{
		std::call_once(m_once, []() { m_inst.reset(new CCurlManager()); });
		return *m_inst;
	}
	~CCurlManager();

	void InitCurlManager();
	bool SendErWnJsonString(const char * _message, char * _level);
	Json::Value SendNewAccountJsonString(char* _id, char* _pw, char* _ml);
	Json::Value SendLoginJsonString(char* _id, char* _pw);
};
#endif