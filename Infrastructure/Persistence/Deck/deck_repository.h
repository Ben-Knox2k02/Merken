#ifndef DECK_REPOSITORY_REAL_H
#define DECK_REPOSITORY_REAL_H

#include "../../../Application/Repositories/deck_repository.h"
#include "../DatabaseContext/database_context.h"

class DeckRepository : public IDeckRepository {
	public:
		DeckRepository(DatabaseContext& dbContext) : db(dbContext) {}

		std::vector<Deck> GetDecks() override;
		std::optional<Deck> GetDeck(int deckId) override;
		std::optional<std::string> GetDeckName(int deckId) override;
		std::vector<Card> GetDueCards(int deckId, const Date& onDate) override;
		std::vector<ReviewEvent> GetReviewHistory(
			const Date& startDate,
			const Date& endDate,
			std::optional<int> deckId = std::nullopt,
			std::optional<int> cardId = std::nullopt
		) override;
		int AddDeck(const Deck& deck) override;
		bool UpdateDeck(const Deck& deck) override;

		int AddCard(const Card& card) override;
		bool UpdateCard(const Card& card) override;
		bool RecordReview(const Card& card, bool remembered, const Date& reviewedOn) override;

	private:
		void LoadCards(Deck& deck);

		DatabaseContext& db;
};

#endif
