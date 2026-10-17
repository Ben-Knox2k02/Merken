#include "doctest/doctest.h"
#include "../../DomainModels/user_profile.h"

namespace {
Date MustCreate(int year, int month, int day) {
	std::optional<Date> date = Date::Create(year, month, day);
	REQUIRE(date.has_value());
	return *date;
}

UserProfile MakeProfile(
	const std::string& displayName = "",
	std::optional<int> dailyGoal = std::nullopt,
	bool guideFinished = false
) {
	return UserProfile(
		MustCreate(2026, 9, 21),
		displayName,
		"",
		dailyGoal,
		false,
		guideFinished,
		false
	);
}
}

TEST_CASE("UserProfile stores the supplied fields") {
	UserProfile profile(
		MustCreate(2026, 9, 21),
		"Ada Lovelace",
		"Term",
		20,
		true,
		false,
		true
	);

	CHECK(profile.GetStartDate() == MustCreate(2026, 9, 21));
	CHECK(profile.GetDisplayName() == "Ada Lovelace");
	CHECK(profile.GetNote() == "Term");
	CHECK(profile.GetDailyGoal() == 20);
	CHECK(profile.UsesAiStudy());
	CHECK_FALSE(profile.IsGuideFinished());
	CHECK(profile.IsOnboardingFinished());
}

TEST_CASE("UserProfile defaults optional fields") {
	UserProfile profile(MustCreate(2026, 9, 21));

	CHECK(profile.GetDisplayName().empty());
	CHECK(profile.GetNote().empty());
	CHECK_FALSE(profile.GetDailyGoal().has_value());
	CHECK_FALSE(profile.HasDailyGoal());
	CHECK_FALSE(profile.UsesAiStudy());
	CHECK_FALSE(profile.IsGuideFinished());
	CHECK_FALSE(profile.IsOnboardingFinished());
	CHECK(profile.HeaderName() == "Profile");
	CHECK(profile.Initials() == "P");
	CHECK_FALSE(profile.IsGoalMet(100));
}

TEST_CASE("UserProfile::HeaderName trims the display name") {
	CHECK(MakeProfile("  Ada Lovelace  ").HeaderName() == "Ada Lovelace");
	CHECK(MakeProfile("\tAda\t").HeaderName() == "Ada");
	CHECK(MakeProfile("   ").HeaderName() == "Profile");
	CHECK(MakeProfile("\t").HeaderName() == UserProfile::kFallbackDisplayName);
}

TEST_CASE("UserProfile::Initials uses the first letter of the first two words") {
	CHECK(MakeProfile("Ada Lovelace").Initials() == "AL");
	CHECK(MakeProfile("ada lovelace king").Initials() == "AL");
	CHECK(MakeProfile("  grace\thopper ").Initials() == "GH");
	CHECK(MakeProfile("Ada").Initials() == "A");
	CHECK(MakeProfile("").Initials() == "P");
}

TEST_CASE("UserProfile::SetDailyGoal keeps only a positive goal") {
	UserProfile profile = MakeProfile();

	profile.SetDailyGoal(20);
	CHECK(profile.GetDailyGoal() == 20);
	CHECK(profile.HasDailyGoal());

	profile.SetDailyGoal(0);
	CHECK_FALSE(profile.GetDailyGoal().has_value());

	profile.SetDailyGoal(10);
	profile.SetDailyGoal(-3);
	CHECK_FALSE(profile.HasDailyGoal());

	profile.SetDailyGoal(15);
	profile.SetDailyGoal(std::nullopt);
	CHECK_FALSE(profile.GetDailyGoal().has_value());
}

TEST_CASE("UserProfile::IsGoalMet compares the reviewed count with the daily goal") {
	UserProfile profile = MakeProfile("", 20);
	CHECK_FALSE(profile.IsGoalMet(19));
	CHECK(profile.IsGoalMet(20));
	CHECK(profile.IsGoalMet(21));

	profile.SetDailyGoal(std::nullopt);
	CHECK_FALSE(profile.IsGoalMet(21));
}

TEST_CASE("UserProfile::NextGuideStep follows deck and card counts") {
	UserProfile profile = MakeProfile();

	CHECK(profile.NextGuideStep(0, std::nullopt) == GuideStep::CreateDeck);
	CHECK(profile.NextGuideStep(0, 3) == GuideStep::CreateDeck);
	CHECK(profile.NextGuideStep(1, std::nullopt) == GuideStep::None);
	CHECK(profile.NextGuideStep(2, 0) == GuideStep::AddCard);
	CHECK(profile.NextGuideStep(2, 4) == GuideStep::StudyDeck);

	profile.MarkGuideFinished();
	CHECK(profile.IsGuideFinished());
	CHECK(profile.NextGuideStep(0, std::nullopt) == GuideStep::None);
	CHECK(profile.NextGuideStep(2, 0) == GuideStep::None);
	CHECK(profile.NextGuideStep(2, 4) == GuideStep::None);
}

TEST_CASE("UserProfile updates editable fields and leaves the start date") {
	UserProfile profile = MakeProfile("Ada");
	profile.UpdateDisplayName("Grace Hopper");
	profile.UpdateNote("Navy");
	profile.SetUsesAiStudy(true);
	profile.MarkOnboardingFinished();

	CHECK(profile.GetDisplayName() == "Grace Hopper");
	CHECK(profile.GetNote() == "Navy");
	CHECK(profile.UsesAiStudy());
	CHECK(profile.IsOnboardingFinished());
	CHECK(profile.GetStartDate() == MustCreate(2026, 9, 21));
}
