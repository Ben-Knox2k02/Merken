#include "gemini_api_service.h"

// dummy placeholder, does not call the real Gemini API yet
std::string GeminiApiService::GenerateResponse(const std::string& prompt, const std::string& apiKey) {
	(void)prompt;
	(void)apiKey;
	(void)this->httpClient;
	return "";
}
