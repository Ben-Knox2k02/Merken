#ifndef AI_API_SERVICE_H
#define AI_API_SERVICE_H

#include <string>

class IAIAPIService {
	public:
		virtual ~IAIAPIService() = default;

		virtual std::string GenerateResponse(const std::string& prompt, const std::string& apiKey) = 0;
};

#endif
