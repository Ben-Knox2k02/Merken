#ifndef FAKE_AI_API_SERVICE_H
#define FAKE_AI_API_SERVICE_H

#include <string>
#include "../../Application/Services/ai_api_service.h"

class FakeAiApiService : public IAIAPIService {
	public:
		std::string reply = "Fill in: ___ means hello.";
		std::string lastPrompt;
		std::string lastApiKey;
		int generateCount = 0;

		std::string GenerateResponse(const std::string& prompt, const std::string& apiKey) override {
			this->lastPrompt = prompt;
			this->lastApiKey = apiKey;
			this->generateCount += 1;
			return this->reply;
		}
};

#endif
