#ifndef FAKE_DECK_DB_SERVICE_H
#define FAKE_DECK_DB_SERVICE_H

#include <vector>
#include <optional>
#include "../../Application/ServiceInterfaces/deck_db_service.h"

class FakeDeckDbService : public IDeckDBService {
	public:
		int nextDeckId = 1;
		std::vector<Deck> addedDecks;

		std::vector<Deck> GetDecks() override {
			return this->addedDecks;
		}

		std::optional<Deck> GetDeck(int deckId) override {
			for (const Deck& deck : this->addedDecks) {
				if (deck.GetDeckId() == deckId) {
					return deck;
				}
			}
			return std::nullopt;
		}

		int AddDeck(const Deck& deck) override {
			this->addedDecks.push_back(deck);
			const int deckId = this->nextDeckId;
			this->nextDeckId += 1;
			return deckId;
		}

		bool UpdateDeck(const Deck&) override {
			return false;
		}

		int AddCard(const Card&) override {
			return 0;
		}

		bool UpdateCard(const Card&) override {
			return false;
		}
};

#endif
