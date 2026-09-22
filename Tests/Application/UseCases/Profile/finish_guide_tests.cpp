#include "doctest/doctest.h"
#include "../../../../Application/UseCases/Profile/FinishGuide/finish_guide_usecase.h"
#include "../../../Fakes/fake_user_profile_repository.h"

namespace {
Date TestDate() {
	std::optional<Date> date = Date::Create(2026, 9, 21);
	REQUIRE(date.has_value());
	return *date;
}
}

TEST_CASE("FinishGuideUseCase marks the guide finished") {
	FakeUserProfileRepository profiles;
	profiles.profile = UserProfile(TestDate(), "Ada", "", 20, false, false, true);
	FinishGuideUseCase useCase(profiles);

	CHECK(useCase.Execute());
	REQUIRE(profiles.profile.has_value());
	CHECK(profiles.profile->IsGuideFinished());
	CHECK(profiles.profile->IsOnboardingFinished());
	CHECK(profiles.saveCount == 1);
}

TEST_CASE("FinishGuideUseCase does not save again when the guide is already finished") {
	FakeUserProfileRepository profiles;
	profiles.profile = UserProfile(TestDate(), "Ada", "", std::nullopt, false, true, true);
	FinishGuideUseCase useCase(profiles);

	CHECK(useCase.Execute());
	CHECK(profiles.saveCount == 0);
}

TEST_CASE("FinishGuideUseCase fails when no profile is stored") {
	FakeUserProfileRepository profiles;
	FinishGuideUseCase useCase(profiles);

	CHECK_FALSE(useCase.Execute());
	CHECK(profiles.saveCount == 0);
}
