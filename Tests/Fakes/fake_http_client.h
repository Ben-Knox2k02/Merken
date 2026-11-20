#ifndef FAKE_HTTP_CLIENT_H
#define FAKE_HTTP_CLIENT_H

#include <deque>
#include <string>
#include <vector>
#include "../../Application/Services/http_client.h"

class FakeHttpClient : public IHttpClient {
	public:
		std::deque<HttpResponse> responses;
		std::string lastUrl;
		std::string lastBody;
		std::vector<HttpHeader> lastHeaders;
		int postCallCount = 0;

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
			return this->Record(url, "", headers);
		}

		HttpResponse PostJson(
			const std::string& url,
			const std::string& jsonBody,
			const std::vector<HttpHeader>& headers
		) override {
			this->postCallCount += 1;
			return this->Record(url, jsonBody, headers);
		}

		HttpResponse PatchJson(
			const std::string& url,
			const std::string& jsonBody,
			const std::vector<HttpHeader>& headers
		) override {
			return this->Record(url, jsonBody, headers);
		}

		HttpResponse PostForm(
			const std::string& url,
			const std::string& formBody,
			const std::vector<HttpHeader>& headers
		) override {
			return this->Record(url, formBody, headers);
		}

	private:
		HttpResponse Record(
			const std::string& url,
			const std::string& body,
			const std::vector<HttpHeader>& headers
		) {
			this->lastUrl = url;
			this->lastBody = body;
			this->lastHeaders = headers;
			if (this->responses.empty()) {
				return HttpResponse();
			}
			HttpResponse response = this->responses.front();
			this->responses.pop_front();
			return response;
		}
};

#endif
