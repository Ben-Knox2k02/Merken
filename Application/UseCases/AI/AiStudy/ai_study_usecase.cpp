#include "ai_study_usecase.h"
#include "../../../../DomainModels/ai_prompt.h"
#include <optional>
#include <string>

AiStudyResponse AiStudyUseCase::Execute(const AiStudyRequest& request) {
	AiStudyResponse response;
	response.success = false;

	std::optional<Deck> deck = this->deckRepository.GetDeck(request.deckId);
	if (!deck.has_value() || deck->GetCards().empty()) {
		this->aiStudyCacheService.Clear();
		if (deck.has_value()) {
			response.deckName = deck->GetName();
		}
		return response;
	}

	response.deckName = deck->GetName();
	const std::string apiKey = this->appSettingsService.GetSettings().aiApiKey;
	const AiPrompt prompt = AiPrompt::Build(*deck);
	const std::string reply = this->aiApiService.GenerateResponse(prompt.GetText(), apiKey);
	const std::vector<AiQuestion> questions = AiQuestion::ParseMultiple(reply, deck->GetCards());
	if (questions.empty()) {
		this->aiStudyCacheService.Clear();
		return response;
	}

	this->aiStudyCacheService.SetSitting(request.deckId, deck->GetName(), questions);
	for (const AiQuestion& question : questions) {
		response.questions.push_back(AiQuestionResponse{
			.cardId = question.GetCardId(),
			.type = question.GetType() == AiQuestionType::FillIn
				? AiQuestionTypeResponse::FillIn
				: AiQuestionTypeResponse::Sentence,
			.text = question.GetText(),
			.back = question.GetBack()
		});
	}
	response.success = true;
	return response;
}
