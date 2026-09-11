#ifndef DECK_H
#define DECK_H

#include <string>
#include <vector>
#include <wx/datetime.h>
#include "card.h"

class Deck {
	public:
		Deck(
			const std::string& name,
			const std::string& description,
			const wxDateTime& creationDate
		) : name(name),
			description(description),
			creationDate(creationDate) {}

		const std::string& GetName() const { return this->name; }
		const std::string& GetDescription() const { return this->description; }
		const wxDateTime& GetCreationDate() const { return this->creationDate; }
		const std::vector<Card>& GetCards() const { return this->cards; }

		void UpdateName(const std::string& newName) {
			this->name = newName;
		}

		void UpdateDescription(const std::string& newDescription) {
			this->description = newDescription;
		}

		void AddCard(const Card& card) {
			this->cards.push_back(card);
		}

	private:
		std::string name;
		std::string description;
		wxDateTime creationDate;
		std::vector<Card> cards;
};

#endif
