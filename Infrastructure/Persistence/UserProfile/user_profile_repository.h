#ifndef USER_PROFILE_REPOSITORY_REAL_H
#define USER_PROFILE_REPOSITORY_REAL_H

#include "../../../Application/Repositories/user_profile_repository.h"
#include "../DatabaseContext/database_context.h"

class UserProfileRepository : public IUserProfileRepository {
	public:
		explicit UserProfileRepository(DatabaseContext& dbContext) : db(dbContext) {}

		std::optional<UserProfile> GetUserProfile() override;
		bool SaveUserProfile(const UserProfile& profile) override;

	private:
		DatabaseContext& db;
};

#endif
