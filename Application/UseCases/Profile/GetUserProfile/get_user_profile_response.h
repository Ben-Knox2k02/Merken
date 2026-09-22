#ifndef GET_USER_PROFILE_RESPONSE_H
#define GET_USER_PROFILE_RESPONSE_H

#include <optional>
#include <string>

struct GetUserProfileResponse {
	bool ok;
	std::string displayName;
	std::string headerName;
	std::string initials;
	std::string note;
	std::string startDate;
	std::optional<int> dailyGoal;
	bool usesAiStudy;
	bool guideFinished;
};

#endif
