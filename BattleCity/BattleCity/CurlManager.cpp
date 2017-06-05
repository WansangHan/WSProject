#include "stdafx.h"
#include "CurlManager.h"

std::unique_ptr<CCurlManager> CCurlManager::m_inst;
std::once_flag CCurlManager::m_once;

void init_string(struct strres *s) {
	s->len = 0;
	s->ptr = (char*)malloc(s->len + 1);
	s->ptr[0] = '\0';
}

size_t writefunc(void *ptr, size_t size, size_t nmemb, struct strres *s)
{
	size_t new_len = s->len + size*nmemb;
	s->ptr = (char*)realloc(s->ptr, new_len + 1);
	memcpy(s->ptr + s->len, ptr, size*nmemb);
	s->ptr[new_len] = '\0';
	s->len = new_len;

	return size*nmemb;
}

char* CCurlManager::SendCurlMessage(const char* _url, std::string _JsonContainer)
{
	struct strres s;
	init_string(&s);
	CURLcode res;
	const char* postString = _JsonContainer.c_str();

	if (m_curl)
	{
		curl_easy_setopt(m_curl, CURLOPT_URL, _url);

		curl_easy_setopt(m_curl, CURLOPT_POSTFIELDS, postString);
		curl_easy_setopt(m_curl, CURLOPT_WRITEFUNCTION, writefunc);
		curl_easy_setopt(m_curl, CURLOPT_WRITEDATA, &s);


		res = curl_easy_perform(m_curl);

		if (res != CURLE_OK)
		{
			CLogManager::getInstance().WriteLogMessage("ERROR", false, "curl_easy_perform Error : %d", res);
		}

	}

	return s.ptr;
}

CCurlManager::CCurlManager()
{
}


CCurlManager::~CCurlManager()
{
	curl_easy_cleanup(m_curl);
}

void CCurlManager::InitCurlManager()
{
	m_curl = curl_easy_init();
}

bool CCurlManager::SendErWnJsonString(const char * _message, char * _level)
{
	Json::Value data;
	data["cate"] = "CLNT";
	data["message"] = _message;
	data["level"] = _level;
	Json::StyledWriter writer;
	SendCurlMessage("http://192.168.127.128/index.php/LogWriter/LogWriter", writer.write(data));

	return true;
}

Json::Value CCurlManager::SendNewAccountJsonString(char * _id, char * _pw, char* _ml)
{
	Json::Value data;
	data["id"] = _id;
	data["pw"] = _pw;
	data["ml"] = _ml;
	Json::StyledWriter writer;
	char* retVal = SendCurlMessage("http://192.168.127.128/index.php/BattleCity/NewAccount", writer.write(data));
	Json::Reader reader;
	Json::Value retjsn;
	reader.parse(retVal, retjsn);

	return retjsn;
}

Json::Value CCurlManager::SendLoginJsonString(char* _id, char* _pw)
{
	Json::Value data;
	data["id"] = _id;
	data["pw"] = _pw;
	Json::StyledWriter writer;
	char* retVal = SendCurlMessage("http://192.168.127.128/index.php/BattleCity/Login", writer.write(data));
	Json::Reader reader;
	Json::Value retjsn;
	reader.parse(retVal, retjsn);
	return retjsn;
}
