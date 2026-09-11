#ifndef UUID_GENERATOR_SERVICE_REAL_H
#define UUID_GENERATOR_SERVICE_REAL_H

#include "../../Application/ServiceInterfaces/uuid_generator_service.h"

class UuidGeneratorService : public IUuidGeneratorService {
	public:
		std::string GenerateUuid() override;
};

#endif
