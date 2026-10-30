#include "doctest/doctest.h"
#include <string>
#include <vector>
#include "../Fakes/fake_http_client.h"
#include "../../Infrastructure/GeminiApi/gemini_api_service.h"
#include "../../ThirdParty/nlohmann/json.hpp"
#include "../../DomainModels/ai_prompt.h"
#include "../../DomainModels/ai_question.h"
#include "../../DomainModels/deck.h"


// build gemini success body, sicne nolohmann escapes quotes and newlines, we can just use the text as is
static std::string MakeSuccessBody(const std::string& text){
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

// parse the request body the service sent and extract the prompt text, returns false if the body is not valid json or does not contain the prompt
static bool ReadSentPrompt(const std::string& body, std::string& outPrompt){
    nlohmann::json sent = nlohmann::json::parse(body, nullptr, false);
    if(sent.is_discarded() || !sent.is_object()){
        return false;
    }
    try{
        outPrompt = sent.at("contents").at(0).at("parts").at(0).at("text").get<std::string>();
    } catch (const std::exception&){
        return false;
    }
    return true;
}

TEST_CASE("GeminiApiService::GenerateResponse checks quotes and newlines in prompt"){
    FakeHttpClient http;
    GeminiApiService service(http);
    const std::string text = "What is the \"powerhouse\" of the cell?\n Please answer in 1-2 sentences.";
    http.QueueResponse(200, MakeSuccessBody(text));

    CHECK(service.GenerateResponse(text, "key") == text); // response parsing

    std::string sent;
    REQUIRE(ReadSentPrompt(http.lastBody, sent)); // check if body is valid json
    CHECK(sent == text); // prompt escaping
}

TEST_CASE("AiPrompt, GeminiApiService and AiQuestion integration test"){
    Deck deck(1, "Spanish");
    deck.AddCard(Card(7, 1, "Hola", "Hello"));
    deck.AddCard(Card(8, 1, "Adios", "Goodbye"));

    // what the pretend Gemini will send back
    const std::string reply = 
    "1. fill-in | ___ means hello.\n"
    "2. sentence | What Spanish word means goodbye?\n";

    FakeHttpClient http;
    GeminiApiService service(http);
    http.QueueResponse(200, MakeSuccessBody(reply));

    // build the real prompt
    const std::string prompt = AiPrompt::Build(deck).GetText();
    // send the prompt to the real service
    const std::string response = service.GenerateResponse(prompt, "key");
    // parse the response
    const std::vector<AiQuestion> questions = AiQuestion::ParseMultiple(response, deck.GetCards());

    // check the prompt reached the request body unchanged
    std::string sent;
    REQUIRE(ReadSentPrompt(http.lastBody, sent));
    CHECK(sent == prompt);
    CHECK(sent.find("- Front: Hola | Back: Hello") != std::string::npos);
    CHECK(sent.find("- Front: Adios | Back: Goodbye") != std::string::npos);

    // check the response became one question per card
    REQUIRE(questions.size() == 2);
    CHECK(questions[0].GetCardId() == 7);
    CHECK(questions[0].GetType() == AiQuestionType::FillIn);
    CHECK(questions[0].GetText() == "___ means hello.");
    CHECK(questions[1].GetCardId() == 8);
    CHECK(questions[1].GetType() == AiQuestionType::Sentence);
    CHECK(questions[1].GetText() == "What Spanish word means goodbye?");
}
