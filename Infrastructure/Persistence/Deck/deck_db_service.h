#ifndef DECK_DB_SERVICE_REAL_H
#define DECK_DB_SERVICE_REAL_H

#include "../../../Application/ServiceInterfaces/deck_db_service.h"
#include "../DatabaseContext/database_context.h"

class DeckDbService : public IDeckDBService {
	public:
		DeckDbService(DatabaseContext& dbContext) : db(dbContext) {}

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
