#include "deck_db_service.h"
#include "card_row.h"
#include <wx/wxsqlite3.h>

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

	Deck deck(
		result.GetInt(0),
		result.GetString(1).ToStdString(),
		result.GetString(2).ToStdString(),
		result.GetString(3).ToStdString()
	);
	this->LoadCards(deck);
	return deck;
}

void DeckDbService::LoadCards(Deck& deck) {
	wxSQLite3::Statement stmt = this->db.GetConnection()->PrepareStatement(
		"SELECT card_id, deck_id, front, back, tags, card_type, choices, "
		"next_review_date, last_reviewed_date, interval_days, ease_factor, repetition_count "
		"FROM cards WHERE deck_id = ? ORDER BY card_id;"
	);
	stmt.Bind(1, deck.GetDeckId());

	wxSQLite3::ResultSet result = stmt.ExecuteQuery();
	while (result.NextRow()) {
		deck.AddCard(MapCardRow(result));
	}
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

int DeckDbService::AddCard(const Card& card) {
	try {
		wxSQLite3::Statement stmt = this->db.GetConnection()->PrepareStatement(
			"INSERT INTO cards (deck_id, front, back, tags, card_type, choices) "
			"VALUES (?, ?, ?, ?, ?, ?);"
		);
		stmt.Bind(1, card.GetDeckId());
		stmt.Bind(2, wxString(card.GetFront()));
		stmt.Bind(3, wxString(card.GetBack()));
		stmt.Bind(4, wxString(Tag::Join(card.GetTags())));
		stmt.Bind(5, static_cast<int>(card.GetCardType()));
		stmt.Bind(6, wxString(JoinCsv(card.GetChoices())));
		if (stmt.ExecuteUpdate() <= 0) { return 0; }
		return static_cast<int>(this->db.GetConnection()->GetLastRowId().GetValue());
	} catch (const wxSQLite3::Exception&) {
		return 0;
	}
}

bool DeckDbService::UpdateCard(const Card& card) {
	wxSQLite3::Statement stmt = this->db.GetConnection()->PrepareStatement(
		"UPDATE cards SET front = ?, back = ?, tags = ?, card_type = ?, choices = ?, "
		"interval_days = ?, ease_factor = ?, repetition_count = ?, "
		"next_review_date = ?, last_reviewed_date = ? "
		"WHERE deck_id = ? AND card_id = ?;"
	);
	stmt.Bind(1, wxString(card.GetFront()));
	stmt.Bind(2, wxString(card.GetBack()));
	stmt.Bind(3, wxString(Tag::Join(card.GetTags())));
	stmt.Bind(4, static_cast<int>(card.GetCardType()));
	stmt.Bind(5, wxString(JoinCsv(card.GetChoices())));
	stmt.Bind(6, card.GetIntervalDays());
	stmt.Bind(7, card.GetEaseFactor());
	stmt.Bind(8, card.GetRepetitionCount());
	stmt.Bind(9, wxString(card.GetNextReviewDate().has_value() ? card.GetNextReviewDate()->ToIso() : ""));
	stmt.Bind(10, wxString(card.GetLastReviewedDate().has_value() ? card.GetLastReviewedDate()->ToIso() : ""));
	stmt.Bind(11, card.GetDeckId());
	stmt.Bind(12, card.GetCardId());
	return stmt.ExecuteUpdate() > 0;
}
