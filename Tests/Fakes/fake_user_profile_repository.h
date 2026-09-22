#ifndef FAKE_USER_PROFILE_REPOSITORY_H
#define FAKE_USER_PROFILE_REPOSITORY_H

#include <optional>
#include "../../Application/Repositories/user_profile_repository.h"

class FakeUserProfileRepository : public IUserProfileRepository {
	public:
		std::optional<UserProfile> profile;
		int saveCount = 0;
		bool saveResult = true;

		std::optional<UserProfile> GetUserProfile() override {
			return this->profile;
		}

		bool SaveUserProfile(const UserProfile& profile) override {
			this->saveCount += 1;
			if (!this->saveResult) {
				return false;
			}
			this->profile = profile;
			return true;
		}
};

#endif
