#ifndef HTTP_CLIENT_H
#define HTTP_CLIENT_H

#include <string>
#include <vector>

struct HttpHeader {
	std::string name;
	std::string value;
};

struct HttpResponse {
	int statusCode = 0;
	std::string body;
	std::string error;

	bool Ok() const { return this->statusCode >= 200 && this->statusCode < 300; }
};

class IHttpClient {
	public:
		virtual ~IHttpClient() = default;

		virtual HttpResponse Get(
			const std::string& url,
			const std::vector<HttpHeader>& headers = {}
		) = 0;

		virtual HttpResponse PostJson(
			const std::string& url,
			const std::string& jsonBody,
			const std::vector<HttpHeader>& headers = {}
		) = 0;
};

#endif
