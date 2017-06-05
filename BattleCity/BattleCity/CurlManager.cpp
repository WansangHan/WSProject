#include "stdafx.h"
#include "CurlManager.h"

std::unique_ptr<CCurlManager> CCurlManager::m_inst;
std::once_flag CCurlManager::m_once;

CURLcode CCurlManager::SendCurlMessage(const char* _url, std::string _JsonContainer)
{
	CURLcode res;
	const char* postString = _JsonContainer.c_str();

	if (m_curl)
	{
		curl_easy_setopt(m_curl, CURLOPT_URL, _url);

		curl_easy_setopt(m_curl, CURLOPT_POSTFIELDS, postString);


		res = curl_easy_perform(m_curl);

		if (res != CURLE_OK)
		{
			CLogManager::getInstance().WriteLogMessage("ERROR", false, "curl_easy_perform Error : %d", res);
		}

	}

	return res;
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

bool CCurlManager::SendNewAccountJsonString(char * _id, char * _pw, char* _ml)
{
	Json::Value data;
	data["id"] = _id;
	data["pw"] = _pw;
	data["ml"] = _ml;
	Json::StyledWriter writer;
	CURLcode retVal = SendCurlMessage("http://192.168.127.128/index.php/BattleCity/NewAccount", writer.write(data));

	return true;
}

bool CCurlManager::SendLoginJsonString(char* _id, char* _pw)
{
	Json::Value data;
	data["id"] = _id;
	data["pw"] = _pw;
	Json::StyledWriter writer;
	CURLcode retVal = SendCurlMessage("http://192.168.127.128/index.php/BattleCity/Login", writer.write(data));

	return true;
}
