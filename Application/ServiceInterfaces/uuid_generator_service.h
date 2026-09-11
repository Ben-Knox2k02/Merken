#ifndef UUID_GENERATOR_SERVICE_H
#define UUID_GENERATOR_SERVICE_H

#include <string>

class IUuidGeneratorService {
	public:
		virtual ~IUuidGeneratorService() = default;

		virtual std::string GenerateUuid() = 0;
};

#endif
