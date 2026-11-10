#ifndef WX_HTTP_CLIENT_H
#define WX_HTTP_CLIENT_H

#include "../../Application/Services/http_client.h"
#include <wx/wx.h>
#include <wx/webrequest.h>
#include <condition_variable>
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

		HttpResponse PatchJson(
			const std::string& url,
			const std::string& jsonBody,
			const std::vector<HttpHeader>& headers = {}
		) override;

		HttpResponse PostForm(
			const std::string& url,
			const std::string& formBody,
			const std::vector<HttpHeader>& headers = {}
		) override;

	private:
		HttpResponse Execute(
			const wxString& method,
			const std::string& url,
			const std::string* body,
			const std::vector<HttpHeader>& headers,
			const wxString& contentType = "application/json"
		);
		void StartRequest(
			const wxString& method,
			const std::string& url,
			const std::string* body,
			const std::vector<HttpHeader>& headers,
			const wxString& contentType
		);
		void MarkFinished();
		void OnRequestState(wxWebRequestEvent& event);

		std::mutex mutex;
		std::mutex stateMutex;
		std::condition_variable condition;
		std::atomic<bool> finished{false};
		wxWebRequest activeRequest;
		HttpResponse result;
};

#endif
