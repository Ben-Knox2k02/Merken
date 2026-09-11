#ifndef CARD_DB_SERVICE_H
#define CARD_DB_SERVICE_H

#include <vector>
#include <optional>
#include "../../DomainModels/card.h"

class ICardDBService {
	public:
		virtual ~ICardDBService() = default;

		virtual std::vector<Card> GetCards(int deckId) = 0;
		virtual std::optional<Card> GetCard(int cardId) = 0;
		virtual int AddCard(const Card& card) = 0;
		virtual bool UpdateCard(const Card& card) = 0;
};

#endif
