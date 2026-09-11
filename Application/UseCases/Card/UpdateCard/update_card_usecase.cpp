#include "update_card_usecase.h"
#include "../tag_utils.h"
#include <optional>

bool UpdateCardUseCase::Execute(const UpdateCardRequest& request) {
	std::optional<Card> existing = this->cardDBService.GetCard(request.cardId);
	if (!existing.has_value()) { return false; }

	existing->UpdateContent(request.front, request.back);
	existing->UpdateTags(SplitTags(request.tags));
	return this->cardDBService.UpdateCard(*existing);
}
