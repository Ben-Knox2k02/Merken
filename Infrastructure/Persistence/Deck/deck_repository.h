#ifndef DECK_REPOSITORY_REAL_H
#define DECK_REPOSITORY_REAL_H

#include "../../../Application/ServiceInterfaces/deck_repository.h"
#include "../DatabaseContext/database_context.h"

class DeckRepository : public IDeckRepository {
	public:
		DeckRepository(DatabaseContext& dbContext) : db(dbContext) {}

		std::vector<Deck> GetDecks() override;
		std::optional<Deck> GetDeck(int deckId) override;
		int AddDeck(const Deck& deck) override;
		bool UpdateDeck(const Deck& deck) override;

		int AddCard(const Card& card) override;
		bool UpdateCard(const Card& card) override;

	private:
		void LoadCards(Deck& deck);

		DatabaseContext& db;
};

#endif
