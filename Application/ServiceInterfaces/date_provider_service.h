#ifndef DATE_PROVIDER_SERVICE_H
#define DATE_PROVIDER_SERVICE_H

#include "../../DomainModels/date.h"

class IDateProviderService {
	public:
		virtual ~IDateProviderService() = default;

		virtual Date GetCurrentDate() = 0;
};

#endif
