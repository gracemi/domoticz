#include "stdafx.h"
#include "NotificationPushover.h"
#include "../httpclient/HTTPClient.h"
#include "../main/Logger.h"

CNotificationPushover::CNotificationPushover() : CNotificationBase(std::string("pushover"), OPTIONS_URL_SUBJECT | OPTIONS_URL_BODY | OPTIONS_URL_PARAMS)
{
	SetupConfig(std::string("PushoverEnabled"), &m_IsEnabled);
	SetupConfig(std::string("PushoverAPI"), _apikey);
	SetupConfig(std::string("PushoverUser"), _apiuser);
}

CNotificationPushover::~CNotificationPushover()
{
}

bool CNotificationPushover::SendMessageImplementation(
	const uint64_t Idx,
	const std::string &Name,
	const std::string &Subject,
	const std::string &Text,
	const std::string &ExtraData,
	const int Priority,
	const std::string &Sound,
	const bool bFromNotification)
{
	std::string cSubject = (Subject == Text) ? "Domoticz" : Subject;

	bool bRet;
	std::string sResult;
	std::stringstream sPostData;

	sPostData << "token=" << _apikey << "&user=" << _apiuser << "&priority=" << Priority << "&title=" << cSubject << "&message=" << Text;

	size_t posDevice = ExtraData.find("|Device=");
	if (posDevice != std::string::npos) {
		posDevice += 8;
		std::string sDevice = ExtraData.substr(posDevice, ExtraData.find("|", posDevice) - posDevice);
		if (sDevice != "") {
			sPostData << "&device=" << sDevice;
		}
	}

	if (Sound != "") {
		sPostData << "&sound=" << Sound;
	}

	if (Priority == 2) {
		sPostData << "&retry=300&expire=3600";
	}
	std::vector<std::string> ExtraHeaders;
	HTTPClient::SetSecurityOptions(true, true);
	bRet = HTTPClient::POST("https://api.pushover.net/1/messages.json",sPostData.str(),ExtraHeaders,sResult);
	HTTPClient::SetSecurityOptions(false, false);
	if (!bRet)
		_log.Log(LOG_ERROR, "Pushover: %s", sResult.c_str());
	return bRet;
}

bool CNotificationPushover::IsConfigured()
{
	return _apikey != "" && _apiuser != "";
}
