#ifndef COMPLETE_ONBOARDING_REQUEST_H
#define COMPLETE_ONBOARDING_REQUEST_H

#include <optional>
#include <string>

struct CompleteOnboardingRequest {
	std::string displayName;
	std::optional<int> dailyGoal;
};

#endif
