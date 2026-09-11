#ifndef GEMINI_API_SERVICE_REAL_H
#define GEMINI_API_SERVICE_REAL_H

#include "../../Application/ServiceInterfaces/ai_api_service.h"

// dummy placeholder, does not call the real Gemini API yet
class GeminiApiService : public IAIAPIService {
	public:
		std::string GenerateResponse(const std::string& prompt, const AppSettings& settings) override;
};

#endif
