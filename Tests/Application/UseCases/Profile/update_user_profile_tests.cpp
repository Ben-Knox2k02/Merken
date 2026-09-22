#include "doctest/doctest.h"
#include "../../../../Application/UseCases/Profile/UpdateUserProfile/update_user_profile_usecase.h"
#include "../../../Fakes/fake_user_profile_repository.h"

namespace {
Date TestDate() {
	std::optional<Date> date = Date::Create(2026, 9, 21);
	REQUIRE(date.has_value());
	return *date;
}
}

TEST_CASE("UpdateUserProfileUseCase saves editable fields and keeps the start date and guide flag") {
	FakeUserProfileRepository profiles;
	profiles.profile = UserProfile(TestDate(), "Ada", "Old", "", 10, false, true);
	UpdateUserProfileUseCase useCase(profiles);

	bool saved = useCase.Execute(UpdateUserProfileRequest{
		"Ada Lovelace",
		"Term",
		"/tmp/ada.png",
		30,
		true
	});

	CHECK(saved);
	CHECK(profiles.saveCount == 1);
	REQUIRE(profiles.profile.has_value());
	CHECK(profiles.profile->GetDisplayName() == "Ada Lovelace");
	CHECK(profiles.profile->GetNote() == "Term");
	CHECK(profiles.profile->GetImagePath() == "/tmp/ada.png");
	CHECK(profiles.profile->GetDailyGoal() == 30);
	CHECK(profiles.profile->UsesAiStudy());
	CHECK(profiles.profile->GetStartDate() == TestDate());
	CHECK(profiles.profile->IsGuideFinished());
}

TEST_CASE("UpdateUserProfileUseCase clears the daily goal when it is unset") {
	FakeUserProfileRepository profiles;
	profiles.profile = UserProfile(TestDate(), "Ada", "", "", 20, false, false);
	UpdateUserProfileUseCase useCase(profiles);

	bool saved = useCase.Execute(UpdateUserProfileRequest{
		"Ada",
		"",
		"",
		std::nullopt,
		false
	});

	CHECK(saved);
	REQUIRE(profiles.profile.has_value());
	CHECK_FALSE(profiles.profile->HasDailyGoal());
}

TEST_CASE("UpdateUserProfileUseCase fails when no profile is stored") {
	FakeUserProfileRepository profiles;
	UpdateUserProfileUseCase useCase(profiles);

	bool saved = useCase.Execute(UpdateUserProfileRequest{});

	CHECK_FALSE(saved);
	CHECK(profiles.saveCount == 0);
}
