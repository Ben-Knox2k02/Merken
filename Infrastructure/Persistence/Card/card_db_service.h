#ifndef CARD_DB_SERVICE_REAL_H
#define CARD_DB_SERVICE_REAL_H

#include "../../../Application/ServiceInterfaces/card_db_service.h"
#include "../DatabaseContext/database_context.h"

class CardDbService : public ICardDBService {
	public:
		CardDbService(DatabaseContext& dbContext) : db(dbContext) {
			this->EnsureSchema();
		}

		std::vector<Card> GetCards(int deckId) override;
		std::optional<Card> GetCard(int cardId) override;
		int AddCard(const Card& card) override;
		bool UpdateCard(const Card& card) override;

	private:
		DatabaseContext& db;
		void EnsureSchema();
};

#endif
