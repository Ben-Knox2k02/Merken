#include "update_user_profile_usecase.h"

bool UpdateUserProfileUseCase::Execute(const UpdateUserProfileRequest& request) {
	std::optional<UserProfile> existing = this->userProfileRepository.GetUserProfile();
	if (!existing.has_value()) {
		return false;
	}

	existing->UpdateDisplayName(request.displayName);
	existing->UpdateNote(request.note);
	existing->SetImagePath(request.imagePath);
	existing->SetDailyGoal(request.dailyGoal);
	existing->SetUsesAiStudy(request.usesAiStudy);
	return this->userProfileRepository.SaveUserProfile(*existing);
}
