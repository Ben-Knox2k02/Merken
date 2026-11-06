#ifndef UPDATE_USER_PROFILE_USE_CASE_H
#define UPDATE_USER_PROFILE_USE_CASE_H

#include "update_user_profile_request.h"
#include "../../../Repositories/user_profile_repository.h"

class UpdateUserProfileUseCase {
	public:
		explicit UpdateUserProfileUseCase(IUserProfileRepository& userProfileRepository)
			: userProfileRepository(userProfileRepository) {}

		bool Execute(const UpdateUserProfileRequest& request);

	private:
		IUserProfileRepository& userProfileRepository;
};

#endif
