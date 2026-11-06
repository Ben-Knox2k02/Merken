#ifndef USER_PROFILE_REPOSITORY_H
#define USER_PROFILE_REPOSITORY_H

#include <optional>
#include "../../DomainModels/user_profile.h"

class IUserProfileRepository {
	public:
		virtual ~IUserProfileRepository() = default;

		virtual std::optional<UserProfile> GetUserProfile() = 0;
		virtual bool SaveUserProfile(const UserProfile& profile) = 0;
};

#endif
