#include "doctest/doctest.h"
#include "../../../../Application/UseCases/AI/AiStudy/ai_study_usecase.h"
#include "../../../../DomainModels/ai_prompt.h"
#include "../../../Fakes/fake_ai_api_service.h"
#include "../../../Fakes/fake_ai_study_cache_service.h"
#include "../../../Fakes/fake_app_settings_service.h"
#include "../../../Fakes/fake_deck_repository.h"

TEST_CASE("AiStudyUseCase generates the question list in one call and caches it") {
	FakeDeckRepository deckRepository;
	const int deckId = deckRepository.SeedDeck("Spanish");
	deckRepository.SeedCard(deckId, "Hola", "Hello");
	deckRepository.SeedCard(deckId, "Adios", "Goodbye");
	FakeAiApiService aiApi;
	aiApi.reply =
		"1. fill-in | ___ means hello.\n"
		"2. sentence | What Spanish word means goodbye?\n";
	FakeAppSettingsService appSettings;
	appSettings.settings.aiApiKey = "ai-key";
	appSettings.settings.calendarApiKey = "cal-key";
	FakeAiStudyCacheService cache;
	AiStudyUseCase useCase(deckRepository, aiApi, appSettings, cache);

	AiStudyRequest request;
	request.deckId = deckId;

	AiStudyResponse response = useCase.Execute(request);

	CHECK(response.success);
	CHECK(response.deckName == "Spanish");
	REQUIRE(response.questions.size() == 2);
	CHECK(response.questions[0].type == AiQuestionTypeResponse::FillIn);
	CHECK(response.questions[0].text == "___ means hello.");
	CHECK(response.questions[0].back == "Hola");
	CHECK(response.questions[1].type == AiQuestionTypeResponse::Sentence);
	CHECK(response.questions[1].text == "What Spanish word means goodbye?");
	CHECK(response.questions[1].back == "Adios");
	CHECK(aiApi.generateCount == 1);
	CHECK(aiApi.lastApiKey == "ai-key");
	CHECK(aiApi.lastPrompt == AiPrompt::Build(*deckRepository.GetDeck(deckId)).GetText());
	CHECK(cache.setCount == 1);
	CHECK(cache.deckId == deckId);
	CHECK(cache.deckName == "Spanish");
	REQUIRE(cache.questions.size() == 2);
	CHECK(cache.questions[0].GetType() == AiQuestionType::FillIn);
	CHECK(cache.correct == 0);
	CHECK(cache.wrong == 0);
	CHECK(cache.clearCount == 0);
}

TEST_CASE("AiStudyUseCase returns failure when the deck is missing") {
	FakeDeckRepository deckRepository;
	FakeAiApiService aiApi;
	FakeAppSettingsService appSettings;
	FakeAiStudyCacheService cache;
	AiStudyUseCase useCase(deckRepository, aiApi, appSettings, cache);

	AiStudyRequest request;
	request.deckId = 99;

	AiStudyResponse response = useCase.Execute(request);

	CHECK_FALSE(response.success);
	CHECK(response.deckName.empty());
	CHECK(response.questions.empty());
	CHECK(aiApi.generateCount == 0);
	CHECK(cache.setCount == 0);
	CHECK(cache.clearCount == 1);
}

TEST_CASE("AiStudyUseCase returns failure when the deck has no cards") {
	FakeDeckRepository deckRepository;
	const int deckId = deckRepository.SeedDeck("Spanish");
	FakeAiApiService aiApi;
	FakeAppSettingsService appSettings;
	appSettings.settings.aiApiKey = "ai-key";
	FakeAiStudyCacheService cache;
	AiStudyUseCase useCase(deckRepository, aiApi, appSettings, cache);

	AiStudyRequest request;
	request.deckId = deckId;

	AiStudyResponse response = useCase.Execute(request);

	CHECK_FALSE(response.success);
	CHECK(response.deckName == "Spanish");
	CHECK(response.questions.empty());
	CHECK(aiApi.generateCount == 0);
	CHECK(cache.clearCount == 1);
}

TEST_CASE("AiStudyUseCase returns failure when Gemini returns an empty reply") {
	FakeDeckRepository deckRepository;
	const int deckId = deckRepository.SeedDeck("Spanish");
	deckRepository.SeedCard(deckId, "Hola", "Hello");
	FakeAiApiService aiApi;
	aiApi.reply = "";
	FakeAppSettingsService appSettings;
	appSettings.settings.aiApiKey = "ai-key";
	FakeAiStudyCacheService cache;
	AiStudyUseCase useCase(deckRepository, aiApi, appSettings, cache);

	AiStudyRequest request;
	request.deckId = deckId;

	AiStudyResponse response = useCase.Execute(request);

	CHECK_FALSE(response.success);
	CHECK(response.deckName == "Spanish");
	CHECK(response.questions.empty());
	CHECK(aiApi.generateCount == 1);
	CHECK(cache.setCount == 0);
	CHECK(cache.clearCount == 1);
}
