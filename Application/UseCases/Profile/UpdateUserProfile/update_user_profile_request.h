#ifndef UPDATE_USER_PROFILE_REQUEST_H
#define UPDATE_USER_PROFILE_REQUEST_H

#include <optional>
#include <string>

struct UpdateUserProfileRequest {
	std::string displayName;
	std::string note;
	std::string imagePath;
	std::optional<int> dailyGoal;
	bool usesAiStudy;
};

#endif
