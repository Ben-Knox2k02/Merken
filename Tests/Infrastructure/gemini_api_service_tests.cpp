#include "doctest/doctest.h"
#include <string>
#include <vector>
#include "../Fakes/fake_http_client.h"
#include "../../Infrastructure/GeminiApi/gemini_api_service.h"
#include "../../ThirdParty/nlohmann/json.hpp"
#include "../../DomainModels/ai_prompt.h"
#include "../../DomainModels/ai_question.h"
#include "../../DomainModels/deck.h"

namespace {

const char* kGenerateUrl =
	"https://generativelanguage.googleapis.com/v1beta/models/gemini-3-flash-preview:generateContent";

std::string MakeSuccessBody(const std::string& text) {
	nlohmann::json part;
	part["text"] = text;

	nlohmann::json content;
	content["parts"] = nlohmann::json::array({part});
	content["role"] = "model";

	nlohmann::json candidate;
	candidate["content"] = content;
	candidate["finishReason"] = "STOP";

	nlohmann::json body;
	body["candidates"] = nlohmann::json::array({candidate});
	return body.dump();
}

bool ReadSentPrompt(const std::string& body, std::string& outPrompt) {
	nlohmann::json sent = nlohmann::json::parse(body, nullptr, false);
	if (sent.is_discarded() || !sent.is_object()) {
		return false;
	}
	try {
		outPrompt = sent.at("contents").at(0).at("parts").at(0).at("text").get<std::string>();
	} catch (const std::exception&) {
		return false;
	}
	return true;
}

}

TEST_CASE("GeminiApiService returns the reply text and sends the prompt unchanged") {
	FakeHttpClient http;
	GeminiApiService service(http);
	const std::string text = "What is the \"powerhouse\" of the cell?\n Please answer in 1-2 sentences.";
	http.QueueResponse(200, MakeSuccessBody(text));

	CHECK(service.GenerateResponse(text, "key") == text);

	std::string sent;
	REQUIRE(ReadSentPrompt(http.lastBody, sent));
	CHECK(sent == text);
	CHECK(http.postCallCount == 1);
	CHECK(http.lastUrl == kGenerateUrl);
	CHECK(http.LastHeaderValue("x-goog-api-key") == "key");
}

TEST_CASE("GeminiApiService asks for low thinking") {
	FakeHttpClient http;
	GeminiApiService service(http);
	http.QueueResponse(200, MakeSuccessBody("ok"));

	CHECK(service.GenerateResponse("hi", "key") == "ok");

	nlohmann::json sent = nlohmann::json::parse(http.lastBody, nullptr, false);
	REQUIRE(sent.is_object());
	CHECK(sent["generationConfig"]["thinkingConfig"]["thinkingLevel"] == "low");
}

TEST_CASE("GeminiApiService leaves the current prompt and question parser alone") {
	Deck deck(1, "Spanish");
	deck.AddCard(Card(7, 1, "Hola", "Hello"));
	deck.AddCard(Card(8, 1, "Adios", "Goodbye"));

	const std::string reply =
		"1. fill-in | ___ means hello.\n"
		"2. sentence | What Spanish word means goodbye?\n";

	FakeHttpClient http;
	GeminiApiService service(http);
	http.QueueResponse(200, MakeSuccessBody(reply));

	const std::string prompt = AiPrompt::Build(deck).GetText();
	const std::string response = service.GenerateResponse(prompt, "key");
	const std::vector<AiQuestion> questions = AiQuestion::ParseMultiple(response, deck.GetCards());

	std::string sent;
	REQUIRE(ReadSentPrompt(http.lastBody, sent));
	CHECK(sent == prompt);
	CHECK(sent.find("- Front: Hola | Back: Hello") != std::string::npos);
	CHECK(sent.find("- Front: Adios | Back: Goodbye") != std::string::npos);

	REQUIRE(questions.size() == 2);
	CHECK(questions[0].GetCardId() == 7);
	CHECK(questions[0].GetType() == AiQuestionType::FillIn);
	CHECK(questions[0].GetText() == "___ means hello.");
	CHECK(questions[1].GetCardId() == 8);
	CHECK(questions[1].GetType() == AiQuestionType::Sentence);
	CHECK(questions[1].GetText() == "What Spanish word means goodbye?");
}

TEST_CASE("GeminiApiService does not call Gemini without a key or a prompt") {
	FakeHttpClient http;
	GeminiApiService service(http);
	http.QueueResponse(200, MakeSuccessBody("should not be used"));

	CHECK(service.GenerateResponse("hi", "").empty());
	CHECK(service.GenerateResponse("", "key").empty());
	CHECK(http.postCallCount == 0);
}

TEST_CASE("GeminiApiService returns nothing when Gemini rejects the call") {
	FakeHttpClient http;
	GeminiApiService service(http);
	http.QueueResponse(401, R"({"error":{"message":"Invalid API key"}})");

	CHECK(service.GenerateResponse("hi", "key").empty());
	CHECK(http.postCallCount == 1);
}

TEST_CASE("GeminiApiService returns nothing when the prompt is blocked") {
	FakeHttpClient http;
	GeminiApiService service(http);
	http.QueueResponse(200, R"({"promptFeedback":{"blockReason":"SAFETY"}})");

	CHECK(service.GenerateResponse("hi", "key").empty());
}

TEST_CASE("GeminiApiService returns nothing when the reply is not JSON") {
	FakeHttpClient http;
	GeminiApiService service(http);
	http.QueueResponse(200, "not json");

	CHECK(service.GenerateResponse("hi", "key").empty());
}
