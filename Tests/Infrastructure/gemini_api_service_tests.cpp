#include "doctest/doctest.h"
#include <string>
#include "../Fakes/fake_http_client.h"
#include "../../Infrastructure/GeminiApi/gemini_api_service.h"
#include "../../ThirdParty/nlohmann/json.hpp"

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
