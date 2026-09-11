#ifndef DATE_PROVIDER_SERVICE_REAL_H
#define DATE_PROVIDER_SERVICE_REAL_H

#include "../../Application/ServiceInterfaces/date_provider_service.h"

class DateProviderService : public IDateProviderService {
	public:
		Date GetCurrentDate() override;
};

#endif
