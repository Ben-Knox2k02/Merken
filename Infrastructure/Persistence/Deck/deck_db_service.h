#ifndef DECK_DB_SERVICE_REAL_H
#define DECK_DB_SERVICE_REAL_H

#include "../../../Application/ServiceInterfaces/deck_db_service.h"
#include "../DatabaseContext/database_context.h"

class DeckDbService : public IDeckDBService {
	public:
		DeckDbService(DatabaseContext& dbContext) : db(dbContext) {
			this->EnsureSchema();
		}

		std::vector<Deck> GetDecks() override;
		std::optional<Deck> GetDeck(int deckId) override;
		int AddDeck(const Deck& deck) override;
		bool UpdateDeck(const Deck& deck) override;

	private:
		DatabaseContext& db;
		void EnsureSchema();
};

#endif
