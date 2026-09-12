#ifndef FAKE_DATE_PROVIDER_SERVICE_H
#define FAKE_DATE_PROVIDER_SERVICE_H

#include "../../Application/ServiceInterfaces/date_provider_service.h"

class FakeDateProviderService : public IDateProviderService {
	public:
		explicit FakeDateProviderService(const Date& currentDate)
			: currentDate(currentDate) {}

		Date GetCurrentDate() override {
			return this->currentDate;
		}

	private:
		Date currentDate;
};

#endif
