#include "finish_guide_usecase.h"

bool FinishGuideUseCase::Execute() {
	std::optional<UserProfile> existing = this->userProfileRepository.GetUserProfile();
	if (!existing.has_value()) {
		return false;
	}
	if (existing->IsGuideFinished()) {
		return true;
	}
	existing->MarkGuideFinished();
	return this->userProfileRepository.SaveUserProfile(*existing);
}
