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


        void QueueResponse(int statusCode, const std::string& body, const std::string& error = ""){
            HttpResponse response;
            response.statusCode = statusCode;
            response.body = body;
            response.error = error;
            this->responses.push_back(response);
        }


        std::string LastHeaderValue(const std::string& name) const{
            for (const HttpHeader& header : this->lastHeaders) {
                if (header.name == name) {
                    return header.value;
                }
            }
            return "";
        }

        HttpResponse Get(const std::string& url, const std::vector<HttpHeader>& headers) override {
            this->getCallCount++;
            this->Record("GET", url, "", headers);
            return this->NextResponse();
        }

        HttpResponse PostJson(const std::string& url, const std::string& jsonBody, const std::vector<HttpHeader>& headers) override{
            this->postCallCount++;
            this->Record("POST", url, jsonBody, headers);
            return this->NextResponse();
        }

    private:
        void Record(const std::string& method, const std::string& url, const std::string& body, const std::vector<HttpHeader>& headers) {
            this->lastUrl = url;
            this->lastBody = body;
            this->lastHeaders = headers;
            this->requestedUrls.push_back(url);
            this->requestedMethods.push_back(method);
        }

        HttpResponse NextResponse() {
            if (!this->responses.empty()) {
                HttpResponse response = this->responses.front();
                this->responses.pop_front();
                return response;
            }
            HttpResponse emptyResponse;
            return emptyResponse;
        }
};

#endif // FAKE_HTTP_CLIENT_H