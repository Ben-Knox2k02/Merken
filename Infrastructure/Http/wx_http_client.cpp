#include "wx_http_client.h"

WxHttpClient::WxHttpClient() {
	this->Bind(wxEVT_WEBREQUEST_STATE, &WxHttpClient::OnRequestState, this);
}

HttpResponse WxHttpClient::Get(const std::string& url, const std::vector<HttpHeader>& headers) {
	return this->Execute("GET", url, nullptr, headers);
}

HttpResponse WxHttpClient::PostJson(
	const std::string& url,
	const std::string& jsonBody,
	const std::vector<HttpHeader>& headers
) {
	return this->Execute("POST", url, &jsonBody, headers);
}

HttpResponse WxHttpClient::Execute(
	const wxString& method,
	const std::string& url,
	const std::string* jsonBody,
	const std::vector<HttpHeader>& headers
) {
	std::lock_guard<std::mutex> lock(this->mutex);

	this->finished = false;
	this->result = HttpResponse{};

	wxWebRequest request = wxWebSession::GetDefault().CreateRequest(this, wxString::FromUTF8(url));
	if (!request.IsOk()) {
		this->result.error = "Failed to create HTTP request";
		return this->result;
	}

	request.SetMethod(method);
	request.SetStorage(wxWebRequest::Storage_Memory);
	for (const HttpHeader& header : headers) {
		request.SetHeader(wxString::FromUTF8(header.name), wxString::FromUTF8(header.value));
	}
	if (jsonBody != nullptr) {
		request.SetData(wxString::FromUTF8(*jsonBody), "application/json");
	}

	request.Start();
	while (!this->finished) {
		if (wxTheApp == nullptr || !wxTheApp->Yield(true)) {
			wxMilliSleep(10);
		}
	}

	return this->result;
}

void WxHttpClient::OnRequestState(wxWebRequestEvent& event) {
	switch (event.GetState()) {
		case wxWebRequest::State_Completed: {
			const wxWebResponse response = event.GetResponse();
			if (response.IsOk()) {
				this->result.statusCode = response.GetStatus();
				this->result.body = std::string(response.AsString().utf8_str());
			} else {
				this->result.error = "Empty HTTP response";
			}
			this->finished = true;
			break;
		}
		case wxWebRequest::State_Failed:
		case wxWebRequest::State_Cancelled:
			this->result.error = event.GetErrorDescription().ToStdString();
			this->finished = true;
			break;
		case wxWebRequest::State_Unauthorized:
			this->result.statusCode = event.GetResponse().IsOk() ? event.GetResponse().GetStatus() : 401;
			this->result.error = event.GetErrorDescription().ToStdString();
			this->finished = true;
			break;
		default:
			break;
	}
}
