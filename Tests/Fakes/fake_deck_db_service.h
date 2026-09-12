#ifndef FAKE_DECK_DB_SERVICE_H
#define FAKE_DECK_DB_SERVICE_H

#include <vector>
#include <optional>
#include "../../Application/ServiceInterfaces/deck_db_service.h"

class FakeDeckDbService : public IDeckDBService {
	public:
		int nextDeckId = 1;
		int nextCardId = 1;
		bool updateDeckSucceeds = true;
		bool updateCardSucceeds = true;
		std::vector<Deck> addedDecks;
		std::vector<Card> addedCards;
		std::vector<Deck> decks;

		int SeedDeck(const std::string& name, const std::string& description = "", const std::string& createdAt = "") {
			const int deckId = this->nextDeckId;
			this->nextDeckId += 1;
			this->decks.push_back(Deck(deckId, name, description, createdAt));
			return deckId;
		}

		int SeedCard(int deckId, const std::string& front, const std::string& back, const std::string& tags = "") {
			Deck* deck = this->FindDeck(deckId);
			if (deck == nullptr) {
				return 0;
			}
			const int cardId = this->nextCardId;
			this->nextCardId += 1;
			Card card(cardId, deckId, front, back);
			card.UpdateTags(Tag::Split(tags));
			deck->AddCard(card);
			return cardId;
		}

		Card* FindStoredCard(int deckId, int cardId) {
			Deck* deck = this->FindDeck(deckId);
			if (deck == nullptr) {
				return nullptr;
			}
			return deck->FindCard(cardId);
		}

		std::vector<Deck> GetDecks() override {
			return this->decks;
		}

		std::optional<Deck> GetDeck(int deckId) override {
			Deck* deck = this->FindDeck(deckId);
			if (deck == nullptr) {
				return std::nullopt;
			}
			return *deck;
		}

		int AddDeck(const Deck& deck) override {
			this->addedDecks.push_back(deck);
			const int deckId = this->nextDeckId;
			this->nextDeckId += 1;
			this->decks.push_back(Deck(deckId, deck.GetName(), deck.GetDescription(), deck.GetCreatedAt()));
			return deckId;
		}

		bool UpdateDeck(const Deck& deck) override {
			if (!this->updateDeckSucceeds) {
				return false;
			}
			Deck* stored = this->FindDeck(deck.GetDeckId());
			if (stored == nullptr) {
				return false;
			}
			*stored = deck;
			return true;
		}

		int AddCard(const Card& card) override {
			this->addedCards.push_back(card);
			Deck* deck = this->FindDeck(card.GetDeckId());
			if (deck == nullptr) {
				return 0;
			}
			const int cardId = this->nextCardId;
			this->nextCardId += 1;
			Card stored(cardId, card.GetDeckId(), card.GetFront(), card.GetBack());
			stored.UpdateTags(card.GetTags());
			deck->AddCard(stored);
			return cardId;
		}

		bool UpdateCard(const Card& card) override {
			if (!this->updateCardSucceeds) {
				return false;
			}
			Card* stored = this->FindStoredCard(card.GetDeckId(), card.GetCardId());
			if (stored == nullptr) {
				return false;
			}
			*stored = card;
			return true;
		}

	private:
		Deck* FindDeck(int deckId) {
			for (Deck& deck : this->decks) {
				if (deck.GetDeckId() == deckId) {
					return &deck;
				}
			}
			return nullptr;
		}
};

#endif
