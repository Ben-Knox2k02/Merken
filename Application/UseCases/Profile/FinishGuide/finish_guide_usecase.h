#ifndef FINISH_GUIDE_USE_CASE_H
#define FINISH_GUIDE_USE_CASE_H

#include "../../../Repositories/user_profile_repository.h"

class FinishGuideUseCase {
	public:
		explicit FinishGuideUseCase(IUserProfileRepository& userProfileRepository)
			: userProfileRepository(userProfileRepository) {}

		bool Execute();

	private:
		IUserProfileRepository& userProfileRepository;
};

#endif
