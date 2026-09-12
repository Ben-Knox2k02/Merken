#ifndef DECK_H
#define DECK_H

#include <string>
#include <vector>
#include "card.h"

class Deck {
	public:
		Deck(
			int deckId,
			const std::string& name,
			const std::string& description = "",
			const std::string& createdAt = ""
		) : deckId(deckId),
			name(name),
			description(description),
			createdAt(createdAt) {}

		int GetDeckId() const { return this->deckId; }
		const std::string& GetName() const { return this->name; }
		const std::string& GetDescription() const { return this->description; }
		const std::string& GetCreatedAt() const { return this->createdAt; }
		const std::vector<Card>& GetCards() const { return this->cards; }

		Card* FindCard(int cardId) {
			for (Card& card : this->cards) {
				if (card.GetCardId() == cardId) {
					return &card;
				}
			}
			return nullptr;
		}

		std::vector<Card> GetDueCards(const Date& onDate) const {
			std::vector<Card> dueCards;
			for (const Card& card : this->cards) {
				if (card.IsDue(onDate)) {
					dueCards.push_back(card);
				}
			}
			return dueCards;
		}

		void UpdateName(const std::string& newName) {
			this->name = newName;
		}

		void UpdateDescription(const std::string& newDescription) {
			this->description = newDescription;
		}

		void AddCard(const Card& card) {
			this->cards.push_back(card);
		}

		void RemoveCard(int cardId) {
			for (auto it = this->cards.begin(); it != this->cards.end(); ++it) {
				if (it->GetCardId() == cardId) {
					this->cards.erase(it);
					return;
				}
			}
		}

	private:
		int deckId;
		std::string name;
		std::string description;
		std::string createdAt;
		std::vector<Card> cards;
};

#endif
