#include "card_db_service.h"
#include <wx/wxsqlite3.h>
#include <sstream>

namespace {
std::string JoinTags(const std::vector<std::string>& tags) {
	std::string joined;
	for (size_t i = 0; i < tags.size(); ++i) {
		if (i > 0) { joined += ", "; }
		joined += tags[i];
	}
	return joined;
}

std::vector<std::string> SplitTags(const std::string& tags) {
	std::vector<std::string> result;
	std::stringstream stream(tags);
	std::string item;
	while (std::getline(stream, item, ',')) {
		const auto start = item.find_first_not_of(" \t");
		if (start == std::string::npos) { continue; }
		const auto end = item.find_last_not_of(" \t");
		result.push_back(item.substr(start, end - start + 1));
	}
	return result;
}

Card MapCardRow(wxSQLite3::ResultSet& result) {
	Card card(
		result.GetInt(0),
		result.GetInt(1),
		result.GetString(2).ToStdString(),
		result.GetString(3).ToStdString()
	);
	card.UpdateTags(SplitTags(result.GetString(4).ToStdString()));
	if (result.GetInt(5) == static_cast<int>(CardType::MultipleChoice)) {
		card.SetMultipleChoice(SplitTags(result.GetString(6).ToStdString()));
	}
	return card;
}
}

void CardDbService::EnsureSchema() {
	this->db.GetConnection()->ExecuteUpdate(
		"CREATE TABLE IF NOT EXISTS cards ("
		"card_id INTEGER PRIMARY KEY AUTOINCREMENT,"
		"deck_id INTEGER NOT NULL,"
		"front TEXT NOT NULL,"
		"back TEXT NOT NULL,"
		"tags TEXT NOT NULL DEFAULT '',"
		"card_type INTEGER NOT NULL DEFAULT 0,"
		"choices TEXT NOT NULL DEFAULT '',"
		"interval_days INTEGER NOT NULL DEFAULT 0,"
		"ease_factor REAL NOT NULL DEFAULT 2.5,"
		"repetition_count INTEGER NOT NULL DEFAULT 0,"
		"next_review_date TEXT NOT NULL DEFAULT '',"
		"last_reviewed_date TEXT NOT NULL DEFAULT '',"
		"FOREIGN KEY (deck_id) REFERENCES decks(deck_id)"
		");"
	);
}

std::vector<Card> CardDbService::GetCards(int deckId) {
	std::vector<Card> cards;
	wxSQLite3::Statement stmt = this->db.GetConnection()->PrepareStatement(
		"SELECT card_id, deck_id, front, back, tags, card_type, choices "
		"FROM cards WHERE deck_id = ? ORDER BY card_id;"
	);
	stmt.Bind(1, deckId);

	wxSQLite3::ResultSet result = stmt.ExecuteQuery();
	while (result.NextRow()) {
		cards.push_back(MapCardRow(result));
	}
	return cards;
}

std::optional<Card> CardDbService::GetCard(int cardId) {
	wxSQLite3::Statement stmt = this->db.GetConnection()->PrepareStatement(
		"SELECT card_id, deck_id, front, back, tags, card_type, choices "
		"FROM cards WHERE card_id = ?;"
	);
	stmt.Bind(1, cardId);

	wxSQLite3::ResultSet result = stmt.ExecuteQuery();
	if (!result.NextRow()) { return std::nullopt; }
	return MapCardRow(result);
}

int CardDbService::AddCard(const Card& card) {
	try {
		wxSQLite3::Statement stmt = this->db.GetConnection()->PrepareStatement(
			"INSERT INTO cards (deck_id, front, back, tags, card_type, choices) "
			"VALUES (?, ?, ?, ?, ?, ?);"
		);
		stmt.Bind(1, card.GetDeckId());
		stmt.Bind(2, wxString(card.GetFront()));
		stmt.Bind(3, wxString(card.GetBack()));
		stmt.Bind(4, wxString(JoinTags(card.GetTags())));
		stmt.Bind(5, static_cast<int>(card.GetCardType()));
		stmt.Bind(6, wxString(JoinTags(card.GetChoices())));
		if (stmt.ExecuteUpdate() <= 0) { return 0; }
		return static_cast<int>(this->db.GetConnection()->GetLastRowId().GetValue());
	} catch (const wxSQLite3::Exception&) {
		return 0;
	}
}

bool CardDbService::UpdateCard(const Card& card) {
	wxSQLite3::Statement stmt = this->db.GetConnection()->PrepareStatement(
		"UPDATE cards SET front = ?, back = ?, tags = ?, card_type = ?, choices = ? "
		"WHERE card_id = ?;"
	);
	stmt.Bind(1, wxString(card.GetFront()));
	stmt.Bind(2, wxString(card.GetBack()));
	stmt.Bind(3, wxString(JoinTags(card.GetTags())));
	stmt.Bind(4, static_cast<int>(card.GetCardType()));
	stmt.Bind(5, wxString(JoinTags(card.GetChoices())));
	stmt.Bind(6, card.GetCardId());
	return stmt.ExecuteUpdate() > 0;
}
