#ifndef GEMINI_API_SERVICE_REAL_H
#define GEMINI_API_SERVICE_REAL_H

#include "../../Application/ServiceInterfaces/ai_api_service.h"
#include "../../Application/ServiceInterfaces/http_client.h"

class GeminiApiService : public IAIAPIService {
	public:
		explicit GeminiApiService(IHttpClient& httpClient) : httpClient(httpClient) {}

		std::string GenerateResponse(const std::string& prompt, const AppSettings& settings) override;

	private:
		IHttpClient& httpClient;
};

#endif
