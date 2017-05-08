#include "stdafx.h"
#include "CurlManager.h"



CCurlManager::CCurlManager()
{
}


CCurlManager::~CCurlManager()
{
	curl_easy_cleanup(m_curl);
}

void CCurlManager::MakeJsonString(const char * _message, char * _level, std::string& JsonContainer)
{
	Json::Value data;
	data["cate"] = "CLNT";
	data["message"] = _message;
	data["level"] = _level;
	Json::StyledWriter writer;
	JsonContainer = writer.write(data);
}

void CCurlManager::InitCurlManager()
{
	m_curl = curl_easy_init();
}

bool CCurlManager::SendLogMessage(const char * _message, char * _level)
{
	std::string JsonContainer;
	MakeJsonString(_message, _level, JsonContainer);
	const char* postString = JsonContainer.c_str();

	if (m_curl)
	{
		curl_easy_setopt(m_curl, CURLOPT_URL, "http://192.168.127.128/index.php/LogWriter/LogWriter");

		curl_easy_setopt(m_curl, CURLOPT_POSTFIELDS, postString);


		res = curl_easy_perform(m_curl);

		if (res != CURLE_OK)
		{
			CLogManager::getInstance().WriteLogMessage("ERROR", false, "curl_easy_perform Error : %d", res);
		}

	}

	return true;
}
