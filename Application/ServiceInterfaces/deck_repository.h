#ifndef DECK_REPOSITORY_H
#define DECK_REPOSITORY_H

#include <vector>
#include <optional>
#include "../../DomainModels/deck.h"

class IDeckRepository {
	public:
		virtual ~IDeckRepository() = default;

		virtual std::vector<Deck> GetDecks() = 0;
		virtual std::optional<Deck> GetDeck(int deckId) = 0;
		virtual int AddDeck(const Deck& deck) = 0;
		virtual bool UpdateDeck(const Deck& deck) = 0;

		virtual int AddCard(const Card& card) = 0;
		virtual bool UpdateCard(const Card& card) = 0;
};

#endif
