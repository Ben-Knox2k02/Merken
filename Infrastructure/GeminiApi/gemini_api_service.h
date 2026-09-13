#ifndef GEMINI_API_SERVICE_REAL_H
#define GEMINI_API_SERVICE_REAL_H

#include "../../Application/Services/ai_api_service.h"
#include "../../Application/Services/http_client.h"

class GeminiApiService : public IAIAPIService {
	public:
		explicit GeminiApiService(IHttpClient& httpClient) : httpClient(httpClient) {}

		std::string GenerateResponse(const std::string& prompt, const std::string& apiKey) override;

	private:
		IHttpClient& httpClient;
};

#endif
