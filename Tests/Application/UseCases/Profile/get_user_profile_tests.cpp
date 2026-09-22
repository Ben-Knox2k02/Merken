#include "doctest/doctest.h"
#include "../../../../Application/UseCases/Profile/GetUserProfile/get_user_profile_usecase.h"
#include "../../../Fakes/fake_user_profile_repository.h"
#include "../../../Fakes/fake_date_provider_service.h"

namespace {
Date TestDate() {
	std::optional<Date> date = Date::Create(2026, 9, 21);
	REQUIRE(date.has_value());
	return *date;
}
}

TEST_CASE("GetUserProfileUseCase creates a profile for today when none is stored") {
	FakeUserProfileRepository profiles;
	FakeDateProviderService dates(TestDate());
	GetUserProfileUseCase useCase(profiles, dates);

	GetUserProfileResponse response = useCase.Execute();

	CHECK(response.ok);
	CHECK(response.displayName.empty());
	CHECK(response.headerName == "Profile");
	CHECK(response.initials == "P");
	CHECK(response.startDate == "2026-09-21");
	CHECK_FALSE(response.dailyGoal.has_value());
	CHECK_FALSE(response.usesAiStudy);
	CHECK_FALSE(response.guideFinished);
	CHECK_FALSE(response.hasImage);
	CHECK(profiles.saveCount == 1);
	REQUIRE(profiles.profile.has_value());
	CHECK(profiles.profile->GetStartDate() == TestDate());
}

TEST_CASE("GetUserProfileUseCase returns the stored profile without saving again") {
	FakeUserProfileRepository profiles;
	profiles.profile = UserProfile(TestDate(), "Ada Lovelace", "Term", "/tmp/ada.png", 20, true, false);
	FakeDateProviderService dates(TestDate());
	GetUserProfileUseCase useCase(profiles, dates);

	GetUserProfileResponse response = useCase.Execute();

	CHECK(response.ok);
	CHECK(response.displayName == "Ada Lovelace");
	CHECK(response.headerName == "Ada Lovelace");
	CHECK(response.initials == "AL");
	CHECK(response.note == "Term");
	CHECK(response.imagePath == "/tmp/ada.png");
	CHECK(response.hasImage);
	CHECK(response.dailyGoal == 20);
	CHECK(response.usesAiStudy);
	CHECK(profiles.saveCount == 0);
}

TEST_CASE("GetUserProfileUseCase reports failure when the new profile cannot be saved") {
	FakeUserProfileRepository profiles;
	profiles.saveResult = false;
	FakeDateProviderService dates(TestDate());
	GetUserProfileUseCase useCase(profiles, dates);

	GetUserProfileResponse response = useCase.Execute();

	CHECK_FALSE(response.ok);
	CHECK(profiles.saveCount == 1);
	CHECK_FALSE(profiles.profile.has_value());
}
