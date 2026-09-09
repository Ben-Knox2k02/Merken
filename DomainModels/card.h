#ifndef CARD_H
#define CARD_H

#include <string>

class Card {
	public:
		Card(int id, const std::string& description) : id(id), description(description) {}

		int GetId() const { return this->id; }
		const std::string& GetDescription() const { return this->description; }

		void UpdateDescription(const std::string& newDescription) {
			this->description = newDescription;
		}

	private:
		int id;
		std::string description;
};

#endif
