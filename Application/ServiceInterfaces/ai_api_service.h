#ifndef AI_API_SERVICE_H
#define AI_API_SERVICE_H

#include <string>
#include "../../DomainModels/app_settings.h"

class IAIAPIService {
	public:
		virtual ~IAIAPIService() = default;

		virtual std::string GenerateResponse(const std::string& prompt, const AppSettings& settings) = 0;
};

#endif
