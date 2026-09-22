#include "user_profile_repository.h"
#include <wx/wxsqlite3.h>

namespace {
std::optional<UserProfile> MapUserProfileRow(wxSQLite3::ResultSet& result) {
	std::optional<Date> startDate = Date::Parse(result.GetString(4).ToStdString());
	if (!startDate.has_value()) {
		return std::nullopt;
	}

	std::optional<int> dailyGoal;
	if (!result.IsNull(5)) {
		dailyGoal = result.GetInt(5);
	}

	return UserProfile(
		*startDate,
		result.GetString(1).ToStdString(),
		result.GetString(2).ToStdString(),
		result.GetString(3).ToStdString(),
		dailyGoal,
		result.GetInt(6) != 0,
		result.GetInt(7) != 0
	);
}
}

std::optional<UserProfile> UserProfileRepository::GetUserProfile() {
	wxSQLite3::Statement stmt = this->db.GetConnection()->PrepareStatement(
		"SELECT profile_id, display_name, note, image_path, start_date, daily_goal, uses_ai_study, guide_finished "
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
			"profile_id, display_name, note, image_path, start_date, daily_goal, uses_ai_study, guide_finished"
			") VALUES (1, ?, ?, ?, ?, ?, ?, ?) "
			"ON CONFLICT(profile_id) DO UPDATE SET "
			"display_name = excluded.display_name, "
			"note = excluded.note, "
			"image_path = excluded.image_path, "
			"start_date = excluded.start_date, "
			"daily_goal = excluded.daily_goal, "
			"uses_ai_study = excluded.uses_ai_study, "
			"guide_finished = excluded.guide_finished;"
		);
		stmt.Bind(1, wxString(profile.GetDisplayName()));
		stmt.Bind(2, wxString(profile.GetNote()));
		stmt.Bind(3, wxString(profile.GetImagePath()));
		stmt.Bind(4, wxString(profile.GetStartDate().ToIso()));
		stmt.Bind(5, profile.GetDailyGoal());
		stmt.Bind(6, profile.UsesAiStudy() ? 1 : 0);
		stmt.Bind(7, profile.IsGuideFinished() ? 1 : 0);
		return stmt.ExecuteUpdate() > 0;
	} catch (const wxSQLite3::Exception&) {
		return false;
	}
}
