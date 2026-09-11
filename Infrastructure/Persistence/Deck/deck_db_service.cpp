#include "deck_db_service.h"
#include <wx/wxsqlite3.h>

void DeckDbService::EnsureSchema() {
	this->db.GetConnection()->ExecuteUpdate(
		"CREATE TABLE IF NOT EXISTS decks ("
		"deck_id INTEGER PRIMARY KEY AUTOINCREMENT,"
		"name TEXT NOT NULL,"
		"description TEXT NOT NULL DEFAULT '',"
		"created_at TEXT NOT NULL DEFAULT ''"
		");"
	);
}

std::vector<Deck> DeckDbService::GetDecks() {
	std::vector<Deck> decks;
	wxSQLite3::ResultSet result = this->db.GetConnection()->ExecuteQuery(
		"SELECT deck_id, name, description, created_at FROM decks ORDER BY deck_id;"
	);

	while (result.NextRow()) {
		decks.emplace_back(
			result.GetInt(0),
			result.GetString(1).ToStdString(),
			result.GetString(2).ToStdString(),
			result.GetString(3).ToStdString()
		);
	}

	return decks;
}

std::optional<Deck> DeckDbService::GetDeck(int deckId) {
	wxSQLite3::Statement stmt = this->db.GetConnection()->PrepareStatement(
		"SELECT deck_id, name, description, created_at FROM decks WHERE deck_id = ?;"
	);
	stmt.Bind(1, deckId);

	wxSQLite3::ResultSet result = stmt.ExecuteQuery();
	if (!result.NextRow()) { return std::nullopt; }

	return Deck(
		result.GetInt(0),
		result.GetString(1).ToStdString(),
		result.GetString(2).ToStdString(),
		result.GetString(3).ToStdString()
	);
}

int DeckDbService::AddDeck(const Deck& deck) {
	try {
		wxSQLite3::Statement stmt = this->db.GetConnection()->PrepareStatement(
			"INSERT INTO decks (name, description, created_at) VALUES (?, ?, ?);"
		);
		stmt.Bind(1, wxString(deck.GetName()));
		stmt.Bind(2, wxString(deck.GetDescription()));
		stmt.Bind(3, wxString(deck.GetCreatedAt()));
		if (stmt.ExecuteUpdate() <= 0) { return 0; }
		return static_cast<int>(this->db.GetConnection()->GetLastRowId().GetValue());
	} catch (const wxSQLite3::Exception&) {
		return 0;
	}
}

bool DeckDbService::UpdateDeck(const Deck& deck) {
	wxSQLite3::Statement stmt = this->db.GetConnection()->PrepareStatement(
		"UPDATE decks SET name = ?, description = ? WHERE deck_id = ?;"
	);
	stmt.Bind(1, wxString(deck.GetName()));
	stmt.Bind(2, wxString(deck.GetDescription()));
	stmt.Bind(3, deck.GetDeckId());
	return stmt.ExecuteUpdate() > 0;
}
