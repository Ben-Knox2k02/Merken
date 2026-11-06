#include "complete_onboarding_usecase.h"

bool CompleteOnboardingUseCase::Execute(const CompleteOnboardingRequest& request) {
	std::optional<UserProfile> existing = this->userProfileRepository.GetUserProfile();
	if (!existing.has_value()) {
		return false;
	}

	existing->UpdateDisplayName(request.displayName);
	existing->SetDailyGoal(request.dailyGoal);
	existing->MarkOnboardingFinished();
	return this->userProfileRepository.SaveUserProfile(*existing);
}
