#ifndef FAKE_HTTP_CLIENT_H
#define FAKE_HTTP_CLIENT_H

#include <deque>
#include <string>
#include <vector>
#include "../../Application/Services/http_client.h"

class FakeHttpClient : public IHttpClient {
	public:
		std::deque<HttpResponse> responses;
		std::vector<std::string> requestedUrls;
		std::vector<std::string> requestedMethods;
		std::string lastUrl;
		std::string lastBody;
		std::vector<HttpHeader> lastHeaders;
		int getCallCount = 0;
		int postCallCount = 0;
		int patchCallCount = 0;
		int postFormCallCount = 0;
		int deleteCallCount = 0;

		void QueueResponse(int statusCode, const std::string& body, const std::string& error = "") {
			HttpResponse response;
			response.statusCode = statusCode;
			response.body = body;
			response.error = error;
			this->responses.push_back(response);
		}

		std::string LastHeaderValue(const std::string& name) const {
			for (const HttpHeader& header : this->lastHeaders) {
				if (header.name == name) {
					return header.value;
				}
			}
			return "";
		}

		HttpResponse Get(const std::string& url, const std::vector<HttpHeader>& headers) override {
			this->getCallCount += 1;
			return this->Record("GET", url, "", headers);
		}

		HttpResponse PostJson(
			const std::string& url,
			const std::string& jsonBody,
			const std::vector<HttpHeader>& headers
		) override {
			this->postCallCount += 1;
			return this->Record("POST", url, jsonBody, headers);
		}

		HttpResponse PatchJson(
			const std::string& url,
			const std::string& jsonBody,
			const std::vector<HttpHeader>& headers
		) override {
			this->patchCallCount += 1;
			return this->Record("PATCH", url, jsonBody, headers);
		}

		HttpResponse PostForm(
			const std::string& url,
			const std::string& formBody,
			const std::vector<HttpHeader>& headers
		) override {
			this->postFormCallCount += 1;
			return this->Record("POST_FORM", url, formBody, headers);
		}

		HttpResponse Delete(const std::string& url, const std::vector<HttpHeader>& headers) override {
			this->deleteCallCount += 1;
			return this->Record("DELETE", url, "", headers);
		}

	private:
		HttpResponse Record(
			const std::string& method,
			const std::string& url,
			const std::string& body,
			const std::vector<HttpHeader>& headers
		) {
			this->lastUrl = url;
			this->lastBody = body;
			this->lastHeaders = headers;
			this->requestedUrls.push_back(url);
			this->requestedMethods.push_back(method);
			return this->NextResponse();
		}

		HttpResponse NextResponse() {
			if (this->responses.empty()) {
				return HttpResponse();
			}
			HttpResponse response = this->responses.front();
			this->responses.pop_front();
			return response;
		}
};

#endif
