#include "gemini_api_service.h"

// dummy placeholder, does not call the real Gemini API yet
std::string GeminiApiService::GenerateResponse(const std::string& prompt, const AppSettings& settings) {
	(void)prompt;
	(void)settings;
	(void)this->httpClient;
	return "";
}
