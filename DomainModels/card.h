#ifndef CARD_H
#define CARD_H

#include <string>

class Card {
	public:
		Card(std::string id, const std::string& description) : id(id), description(description) {}

		const std::string& GetId() const { return this->id; }
		const std::string& GetDescription() const { return this->description; }

		void UpdateDescription(const std::string& newDescription) {
			this->description = newDescription;
		}

	private:
		std::string id;
		std::string description;
};

#endif
