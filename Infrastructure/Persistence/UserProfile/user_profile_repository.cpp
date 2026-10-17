#include "user_profile_repository.h"
#include <wx/wxsqlite3.h>

namespace {
std::optional<UserProfile> MapUserProfileRow(wxSQLite3::ResultSet& result) {
	std::optional<Date> startDate = Date::Parse(result.GetString(3).ToStdString());
	if (!startDate.has_value()) {
		return std::nullopt;
	}

	std::optional<int> dailyGoal;
	if (!result.IsNull(4)) {
		dailyGoal = result.GetInt(4);
	}

	return UserProfile(
		*startDate,
		result.GetString(1).ToStdString(),
		result.GetString(2).ToStdString(),
		dailyGoal,
		result.GetInt(5) != 0,
		result.GetInt(6) != 0,
		result.GetInt(7) != 0
	);
}
}

std::optional<UserProfile> UserProfileRepository::GetUserProfile() {
	wxSQLite3::Statement stmt = this->db.GetConnection()->PrepareStatement(
		"SELECT profile_id, display_name, note, start_date, daily_goal, uses_ai_study, guide_finished, onboarding_finished "
		"FROM user_profile WHERE profile_id = 1;"
	);
	wxSQLite3::ResultSet result = stmt.ExecuteQuery();
	if (!result.NextRow()) {
		return std::nullopt;
	}
	return MapUserProfileRow(result);
}

bool UserProfileRepository::SaveUserProfile(const UserProfile& profile) {
	try {
		wxSQLite3::Statement stmt = this->db.GetConnection()->PrepareStatement(
			"INSERT INTO user_profile ("
			"profile_id, display_name, note, start_date, daily_goal, uses_ai_study, guide_finished, onboarding_finished"
			") VALUES (1, ?, ?, ?, ?, ?, ?, ?) "
			"ON CONFLICT(profile_id) DO UPDATE SET "
			"display_name = excluded.display_name, "
			"note = excluded.note, "
			"start_date = excluded.start_date, "
			"daily_goal = excluded.daily_goal, "
			"uses_ai_study = excluded.uses_ai_study, "
			"guide_finished = excluded.guide_finished, "
			"onboarding_finished = excluded.onboarding_finished;"
		);
		stmt.Bind(1, wxString(profile.GetDisplayName()));
		stmt.Bind(2, wxString(profile.GetNote()));
		stmt.Bind(3, wxString(profile.GetStartDate().ToIso()));
		stmt.Bind(4, profile.GetDailyGoal());
		stmt.Bind(5, profile.UsesAiStudy() ? 1 : 0);
		stmt.Bind(6, profile.IsGuideFinished() ? 1 : 0);
		stmt.Bind(7, profile.IsOnboardingFinished() ? 1 : 0);
		return stmt.ExecuteUpdate() > 0;
	} catch (const wxSQLite3::Exception&) {
		return false;
	}
}
