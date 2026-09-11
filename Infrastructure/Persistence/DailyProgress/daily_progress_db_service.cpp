#include "daily_progress_db_service.h"
#include <wx/wxsqlite3.h>

namespace {
std::optional<DailyProgress> MapDailyProgressRow(wxSQLite3::ResultSet& result) {
	std::optional<Date> date = Date::Parse(result.GetString(0).ToStdString());
	if (!date.has_value()) {
		return std::nullopt;
	}
	return DailyProgress(*date, result.GetInt(1), result.GetInt(2));
}
}

std::optional<DailyProgress> DailyProgressDbService::GetDailyProgress(const Date& date) {
	wxSQLite3::Statement stmt = this->db.GetConnection()->PrepareStatement(
		"SELECT date, cards_reviewed, cards_correct FROM daily_progress WHERE date = ?;"
	);
	stmt.Bind(1, wxString(date.ToIso()));

	wxSQLite3::ResultSet result = stmt.ExecuteQuery();
	if (!result.NextRow()) { return std::nullopt; }
	return MapDailyProgressRow(result);
}

std::vector<DailyProgress> DailyProgressDbService::GetDailyProgressRange(const Date& startDate, const Date& endDate) {
	std::vector<DailyProgress> progressList;
	wxSQLite3::Statement stmt = this->db.GetConnection()->PrepareStatement(
		"SELECT date, cards_reviewed, cards_correct FROM daily_progress "
		"WHERE date >= ? AND date <= ? ORDER BY date;"
	);
	stmt.Bind(1, wxString(startDate.ToIso()));
	stmt.Bind(2, wxString(endDate.ToIso()));

	wxSQLite3::ResultSet result = stmt.ExecuteQuery();
	while (result.NextRow()) {
		std::optional<DailyProgress> progress = MapDailyProgressRow(result);
		if (progress.has_value()) {
			progressList.push_back(*progress);
		}
	}
	return progressList;
}

bool DailyProgressDbService::AddDailyProgress(const DailyProgress& progress) {
	try {
		wxSQLite3::Statement stmt = this->db.GetConnection()->PrepareStatement(
			"INSERT INTO daily_progress (date, cards_reviewed, cards_correct) VALUES (?, ?, ?);"
		);
		stmt.Bind(1, wxString(progress.GetDate().ToIso()));
		stmt.Bind(2, progress.GetCardsReviewed());
		stmt.Bind(3, progress.GetCardsCorrect());
		return stmt.ExecuteUpdate() > 0;
	} catch (const wxSQLite3::Exception&) {
		return false;
	}
}

bool DailyProgressDbService::UpdateDailyProgress(const DailyProgress& progress) {
	wxSQLite3::Statement stmt = this->db.GetConnection()->PrepareStatement(
		"UPDATE daily_progress SET cards_reviewed = ?, cards_correct = ? WHERE date = ?;"
	);
	stmt.Bind(1, progress.GetCardsReviewed());
	stmt.Bind(2, progress.GetCardsCorrect());
	stmt.Bind(3, wxString(progress.GetDate().ToIso()));
	return stmt.ExecuteUpdate() > 0;
}
