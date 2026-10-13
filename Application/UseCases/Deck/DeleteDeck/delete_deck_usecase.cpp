#include "delete_deck_usecase.h"

bool DeleteDeckUseCase::Execute(const DeleteDeckRequest& request) {
	if (!this->deckRepository.GetDeck(request.deckId).has_value()) {
		return false;
	}
	return this->deckRepository.DeleteDeck(request.deckId);
}
