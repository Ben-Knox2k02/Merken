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

HttpResponse WxHttpClient::PatchJson(
	const std::string& url,
	const std::string& jsonBody,
	const std::vector<HttpHeader>& headers
) {
	return this->Execute("PATCH", url, &jsonBody, headers);
}

HttpResponse WxHttpClient::PostForm(
	const std::string& url,
	const std::string& formBody,
	const std::vector<HttpHeader>& headers
) {
	return this->Execute("POST", url, &formBody, headers, "application/x-www-form-urlencoded");
}

void WxHttpClient::StartRequest(
	const wxString& method,
	const std::string& url,
	const std::string* body,
	const std::vector<HttpHeader>& headers,
	const wxString& contentType
) {
	wxWebRequest request = wxWebSession::GetDefault().CreateRequest(this, wxString::FromUTF8(url));
	if (!request.IsOk()) {
		this->result.error = "Failed to create HTTP request";
		this->MarkFinished();
		return;
	}

	request.SetMethod(method);
	request.SetStorage(wxWebRequest::Storage_Memory);
	for (const HttpHeader& header : headers) {
		request.SetHeader(wxString::FromUTF8(header.name), wxString::FromUTF8(header.value));
	}
	if (body != nullptr) {
		request.SetData(wxString::FromUTF8(*body), contentType);
	}

	this->activeRequest = request;
	request.Start();
}

void WxHttpClient::MarkFinished() {
	{
		std::lock_guard<std::mutex> state(this->stateMutex);
		this->finished = true;
	}
	this->condition.notify_all();
}

HttpResponse WxHttpClient::Execute(
	const wxString& method,
	const std::string& url,
	const std::string* body,
	const std::vector<HttpHeader>& headers,
	const wxString& contentType
) {
	std::lock_guard<std::mutex> lock(this->mutex);

	{
		std::lock_guard<std::mutex> state(this->stateMutex);
		this->finished = false;
		this->result = HttpResponse{};
	}

	if (wxIsMainThread()) {
		this->StartRequest(method, url, body, headers, contentType);
		while (!this->finished) {
			if (wxTheApp == nullptr || !wxTheApp->Yield(true)) {
				wxMilliSleep(10);
			}
		}
		return this->result;
	}

	const wxString methodCopy = method;
	const std::string urlCopy = url;
	const bool hasBody = body != nullptr;
	const std::string bodyCopy = hasBody ? *body : std::string();
	const std::vector<HttpHeader> headersCopy = headers;
	const wxString contentTypeCopy = contentType;

	wxTheApp->CallAfter([this, methodCopy, urlCopy, hasBody, bodyCopy, headersCopy, contentTypeCopy]() {
		this->StartRequest(
			methodCopy,
			urlCopy,
			hasBody ? &bodyCopy : nullptr,
			headersCopy,
			contentTypeCopy
		);
	});

	std::unique_lock<std::mutex> waitLock(this->stateMutex);
	this->condition.wait(waitLock, [this]() { return this->finished.load(); });
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
			this->MarkFinished();
			break;
		}
		case wxWebRequest::State_Failed:
		case wxWebRequest::State_Cancelled: {
			const wxWebResponse failed = event.GetResponse();
			if (failed.IsOk()) {
				this->result.statusCode = failed.GetStatus();
				this->result.body = std::string(failed.AsString().utf8_str());
			} else {
				this->result.error = event.GetErrorDescription().ToStdString();
			}
			this->MarkFinished();
			break;
		}
		case wxWebRequest::State_Unauthorized:
			this->result.statusCode = event.GetResponse().IsOk() ? event.GetResponse().GetStatus() : 401;
			if (event.GetResponse().IsOk()) {
				this->result.body = std::string(event.GetResponse().AsString().utf8_str());
			}
			this->result.error = event.GetErrorDescription().ToStdString();
			this->MarkFinished();
			break;
		default:
			break;
	}
}
