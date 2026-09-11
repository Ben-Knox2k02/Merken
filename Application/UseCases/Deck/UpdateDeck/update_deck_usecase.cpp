#include "update_deck_usecase.h"
#include <optional>

bool UpdateDeckUseCase::Execute(const UpdateDeckRequest& request) {
	std::optional<Deck> existing = this->deckDBService.GetDeck(request.deckId);
	if (!existing.has_value()) { return false; }

	existing->UpdateName(request.name);
	existing->UpdateDescription(request.description);
	return this->deckDBService.UpdateDeck(*existing);
}
