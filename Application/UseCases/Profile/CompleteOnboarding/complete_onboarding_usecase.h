#ifndef COMPLETE_ONBOARDING_USE_CASE_H
#define COMPLETE_ONBOARDING_USE_CASE_H

#include "complete_onboarding_request.h"
#include "../../../Repositories/user_profile_repository.h"

class CompleteOnboardingUseCase {
	public:
		explicit CompleteOnboardingUseCase(IUserProfileRepository& userProfileRepository)
			: userProfileRepository(userProfileRepository) {}

		bool Execute(const CompleteOnboardingRequest& request);

	private:
		IUserProfileRepository& userProfileRepository;
};

#endif
