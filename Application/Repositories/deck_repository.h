#ifndef DECK_REPOSITORY_H
#define DECK_REPOSITORY_H

#include <vector>
#include <optional>
#include <string>
#include "../../DomainModels/deck.h"
#include "../../DomainModels/review_event.h"

class IDeckRepository {
	public:
		virtual ~IDeckRepository() = default;

		virtual std::vector<Deck> GetDecks() = 0;
		virtual std::optional<Deck> GetDeck(int deckId) = 0;
		virtual std::optional<std::string> GetDeckName(int deckId) = 0;
		virtual std::vector<Card> GetDueCards(int deckId, const Date& onDate) = 0;
		virtual std::vector<ReviewEvent> GetReviewHistory(
			const Date& startDate,
			const Date& endDate,
			std::optional<int> deckId = std::nullopt,
			std::optional<int> cardId = std::nullopt
		) = 0;
		virtual int AddDeck(const Deck& deck) = 0;
		virtual bool UpdateDeck(const Deck& deck) = 0;

		virtual int AddCard(const Card& card) = 0;
		virtual bool UpdateCard(const Card& card) = 0;
		virtual bool RecordReview(const Card& card, bool remembered, const Date& reviewedOn) = 0;
};

#endif
