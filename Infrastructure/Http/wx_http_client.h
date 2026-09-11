#ifndef WX_HTTP_CLIENT_H
#define WX_HTTP_CLIENT_H

#include "../../Application/ServiceInterfaces/http_client.h"
#include <wx/wx.h>
#include <wx/webrequest.h>
#include <mutex>
#include <atomic>

class WxHttpClient : public IHttpClient, public wxEvtHandler {
	public:
		WxHttpClient();

		HttpResponse Get(
			const std::string& url,
			const std::vector<HttpHeader>& headers = {}
		) override;

		HttpResponse PostJson(
			const std::string& url,
			const std::string& jsonBody,
			const std::vector<HttpHeader>& headers = {}
		) override;

	private:
		HttpResponse Execute(
			const wxString& method,
			const std::string& url,
			const std::string* jsonBody,
			const std::vector<HttpHeader>& headers
		);
		void OnRequestState(wxWebRequestEvent& event);

		std::mutex mutex;
		std::atomic<bool> finished{false};
		HttpResponse result;
};

#endif
