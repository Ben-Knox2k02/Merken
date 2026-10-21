#include "doctest/doctest.h"
#include "../../../../Application/UseCases/Profile/CompleteOnboarding/complete_onboarding_usecase.h"
#include "../../../Fakes/fake_user_profile_repository.h"

namespace {
Date TestDate() {
	std::optional<Date> date = Date::Create(2026, 9, 21);
	REQUIRE(date.has_value());
	return *date;
}
}

TEST_CASE("CompleteOnboardingUseCase saves the name and goal and marks onboarding finished") {
	FakeUserProfileRepository profiles;
	profiles.profile = UserProfile(TestDate(), "", "Keep", std::nullopt, true, false, false);
	CompleteOnboardingUseCase useCase(profiles);

	bool saved = useCase.Execute(CompleteOnboardingRequest{"Ada", 20});

	CHECK(saved);
	REQUIRE(profiles.profile.has_value());
	CHECK(profiles.profile->GetDisplayName() == "Ada");
	CHECK(profiles.profile->GetNote() == "Keep");
	CHECK(profiles.profile->GetDailyGoal() == 20);
	CHECK(profiles.profile->UsesAiStudy());
	CHECK(profiles.profile->IsOnboardingFinished());
	CHECK_FALSE(profiles.profile->IsGuideFinished());
	CHECK(profiles.profile->GetStartDate() == TestDate());
}

TEST_CASE("CompleteOnboardingUseCase can finish with no name and no goal") {
	FakeUserProfileRepository profiles;
	profiles.profile = UserProfile(TestDate());
	CompleteOnboardingUseCase useCase(profiles);

	bool saved = useCase.Execute(CompleteOnboardingRequest{});

	CHECK(saved);
	REQUIRE(profiles.profile.has_value());
	CHECK(profiles.profile->HeaderName() == "Profile");
	CHECK_FALSE(profiles.profile->HasDailyGoal());
	CHECK(profiles.profile->IsOnboardingFinished());
}

TEST_CASE("CompleteOnboardingUseCase fails when no profile is stored") {
	FakeUserProfileRepository profiles;
	CompleteOnboardingUseCase useCase(profiles);

	CHECK_FALSE(useCase.Execute(CompleteOnboardingRequest{}));
	CHECK(profiles.saveCount == 0);
}
