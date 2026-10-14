#include "deck_repository.h"
#include "card_row.h"
#include <wx/wxsqlite3.h>

namespace {
std::optional<ReviewEvent> MapReviewEventRow(wxSQLite3::ResultSet& result) {
	std::optional<Date> reviewedOn = Date::Parse(result.GetString(3).ToStdString());
	if (!reviewedOn.has_value()) {
		return std::nullopt;
	}
	std::optional<Date> nextReviewDate = Date::Parse(result.GetString(8).ToStdString());
	return ReviewEvent(
		result.GetInt(0),
		result.GetInt(1),
		result.GetInt(2),
		*reviewedOn,
		result.GetInt(4) != 0,
		result.GetInt(5),
		result.GetDouble(6),
		result.GetInt(7),
		nextReviewDate
	);
}

std::vector<ReviewEvent> LoadReviewEvents(wxSQLite3::Statement& stmt) {
	std::vector<ReviewEvent> events;
	wxSQLite3::ResultSet result = stmt.ExecuteQuery();
	while (result.NextRow()) {
		std::optional<ReviewEvent> event = MapReviewEventRow(result);
		if (event.has_value()) {
			events.push_back(*event);
		}
	}
	return events;
}
}

std::vector<Deck> DeckRepository::GetDecks() {
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

std::optional<Deck> DeckRepository::GetDeck(int deckId) {
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

std::optional<std::string> DeckRepository::GetDeckName(int deckId) {
	wxSQLite3::Statement stmt = this->db.GetConnection()->PrepareStatement(
		"SELECT name FROM decks WHERE deck_id = ?;"
	);
	stmt.Bind(1, deckId);

	wxSQLite3::ResultSet result = stmt.ExecuteQuery();
	if (!result.NextRow()) { return std::nullopt; }
	return result.GetString(0).ToStdString();
}

std::vector<Card> DeckRepository::GetDueCards(int deckId, const Date& onDate) {
	std::vector<Card> cards;
	wxSQLite3::Statement stmt = this->db.GetConnection()->PrepareStatement(
		"SELECT card_id, deck_id, front, back, tags, card_type, choices, "
		"next_review_date, last_reviewed_date, interval_days, ease_factor, repetition_count "
		"FROM cards WHERE deck_id = ? AND next_review_date <= ? "
		"ORDER BY next_review_date, card_id;"
	);
	stmt.Bind(1, deckId);
	stmt.Bind(2, wxString(onDate.ToIso()));

	wxSQLite3::ResultSet result = stmt.ExecuteQuery();
	while (result.NextRow()) {
		cards.push_back(MapCardRow(result));
	}
	return cards;
}

std::vector<ReviewEvent> DeckRepository::GetReviewHistory(
	const Date& startDate,
	const Date& endDate,
	std::optional<int> deckId,
	std::optional<int> cardId
) {
	std::string sql =
		"SELECT review_id, deck_id, card_id, reviewed_on, remembered, "
		"interval_days, ease_factor, repetition_count, next_review_date "
		"FROM review_events WHERE reviewed_on >= ? AND reviewed_on <= ?";
	if (deckId.has_value()) {
		sql += " AND deck_id = ?";
	}
	if (cardId.has_value()) {
		sql += " AND card_id = ?";
	}
	sql += " ORDER BY reviewed_on DESC, review_id DESC;";

	wxSQLite3::Statement stmt = this->db.GetConnection()->PrepareStatement(sql.c_str());
	int bindIndex = 1;
	stmt.Bind(bindIndex++, wxString(startDate.ToIso()));
	stmt.Bind(bindIndex++, wxString(endDate.ToIso()));
	if (deckId.has_value()) {
		stmt.Bind(bindIndex++, *deckId);
	}
	if (cardId.has_value()) {
		stmt.Bind(bindIndex, *cardId);
	}
	return LoadReviewEvents(stmt);
}

void DeckRepository::LoadCards(Deck& deck) {
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

int DeckRepository::AddDeck(const Deck& deck) {
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

bool DeckRepository::UpdateDeck(const Deck& deck) {
	wxSQLite3::Statement stmt = this->db.GetConnection()->PrepareStatement(
		"UPDATE decks SET name = ?, description = ? WHERE deck_id = ?;"
	);
	stmt.Bind(1, wxString(deck.GetName()));
	stmt.Bind(2, wxString(deck.GetDescription()));
	stmt.Bind(3, deck.GetDeckId());
	return stmt.ExecuteUpdate() > 0;
}

int DeckRepository::AddCard(const Card& card) {
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

bool DeckRepository::UpdateCard(const Card& card) {
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

bool DeckRepository::RecordReview(const Card& card, bool remembered, const Date& reviewedOn) {
	try {
		wxSQLite3::Database* connection = this->db.GetConnection();
		wxSQLite3::Transaction transaction(connection);
		if (!transaction.IsActive()) {
			return false;
		}
		wxSQLite3::Statement update = connection->PrepareStatement(
			"UPDATE cards SET interval_days = ?, ease_factor = ?, repetition_count = ?, "
			"next_review_date = ?, last_reviewed_date = ? "
			"WHERE deck_id = ? AND card_id = ?;"
		);
		update.Bind(1, card.GetIntervalDays());
		update.Bind(2, card.GetEaseFactor());
		update.Bind(3, card.GetRepetitionCount());
		update.Bind(4, wxString(card.GetNextReviewDate().has_value() ? card.GetNextReviewDate()->ToIso() : ""));
		update.Bind(5, wxString(reviewedOn.ToIso()));
		update.Bind(6, card.GetDeckId());
		update.Bind(7, card.GetCardId());
		if (update.ExecuteUpdate() <= 0) {
			return false;
		}

		wxSQLite3::Statement insert = connection->PrepareStatement(
			"INSERT INTO review_events "
			"(deck_id, card_id, reviewed_on, remembered, interval_days, ease_factor, repetition_count, next_review_date) "
			"VALUES (?, ?, ?, ?, ?, ?, ?, ?);"
		);
		insert.Bind(1, card.GetDeckId());
		insert.Bind(2, card.GetCardId());
		insert.Bind(3, wxString(reviewedOn.ToIso()));
		insert.Bind(4, remembered ? 1 : 0);
		insert.Bind(5, card.GetIntervalDays());
		insert.Bind(6, card.GetEaseFactor());
		insert.Bind(7, card.GetRepetitionCount());
		insert.Bind(8, wxString(card.GetNextReviewDate().has_value() ? card.GetNextReviewDate()->ToIso() : ""));
		if (insert.ExecuteUpdate() <= 0) {
			return false;
		}

		transaction.Commit();
		return true;
	} catch (const wxSQLite3::Exception&) {
		return false;
	}
}
