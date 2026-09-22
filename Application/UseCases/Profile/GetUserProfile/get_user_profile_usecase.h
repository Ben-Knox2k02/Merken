#ifndef GET_USER_PROFILE_USE_CASE_H
#define GET_USER_PROFILE_USE_CASE_H

#include "get_user_profile_response.h"
#include "../../../Repositories/user_profile_repository.h"
#include "../../../Services/date_provider_service.h"

class GetUserProfileUseCase {
	public:
		GetUserProfileUseCase(
			IUserProfileRepository& userProfileRepository,
			IDateProviderService& dateProviderService
		) : userProfileRepository(userProfileRepository),
			dateProviderService(dateProviderService) {}

		GetUserProfileResponse Execute();

	private:
		IUserProfileRepository& userProfileRepository;
		IDateProviderService& dateProviderService;
};

#endif
