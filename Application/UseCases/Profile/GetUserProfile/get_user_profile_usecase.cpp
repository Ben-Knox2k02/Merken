#include "get_user_profile_usecase.h"

namespace {
GetUserProfileResponse MapProfile(const UserProfile& profile) {
	return GetUserProfileResponse{
		true,
		profile.GetDisplayName(),
		profile.HeaderName(),
		profile.Initials(),
		profile.GetNote(),
		profile.GetStartDate().ToIso(),
		profile.GetDailyGoal(),
		profile.UsesAiStudy(),
		profile.IsGuideFinished(),
		profile.IsOnboardingFinished()
	};
}
}

GetUserProfileResponse GetUserProfileUseCase::Execute() {
	std::optional<UserProfile> existing = this->userProfileRepository.GetUserProfile();
	if (existing.has_value()) {
		return MapProfile(*existing);
	}

	UserProfile created(this->dateProviderService.GetCurrentDate());
	if (!this->userProfileRepository.SaveUserProfile(created)) {
		return GetUserProfileResponse{};
	}
	return MapProfile(created);
}
