#include "gemini_api_service.h"
#include "../../ThirdParty/nlohmann/json.hpp"
#include <iostream>

// rewriting to use nlohmann like the google calendar service 

// gemini api endpoint
static const std::string kGenerateUrl = "https://generativelanguage.googleapis.com/v1beta/models/gemini-3-flash:generateContent";

// error logging
static void LogGeminiError(const std::string& operation, int statusCode, const std::string& detail){
	std::cerr << "[GeminiApi] " << operation << " failed with status code " << statusCode << ": " << detail << std::endl;
}

// pull message from the error body or fall back to the body start
static std::string ExtractGeminiErrorMessage(const std::string& body){
	nlohmann::json j = nlohmann::json::parse(body, nullptr, false);
	if(j.is_object() && j.contains("error") && j["error"].is_object()){
		return j["error"].value("message", "Unknown error");
	}
	return body.substr(0, 200);
}

// desrcibe failure for logging
static std::string DescribeFailure(const HttpResponse& response){
	if (!response.error.empty()){
		return response.error;
	}
	return ExtractGeminiErrorMessage(response.body);
}

// get the first candidate text from the response body (since there can be multiple correct answers) or return empty string if not found
static std::string ExtractText(const nlohmann::json& parsed){
	if(!parsed.is_object()){
		return "";
	}

	auto candidates = parsed.find("candidates");
	if(candidates == parsed.end() || !candidates->is_array() || candidates->empty()){
		return "";
	}

	const nlohmann::json& firstCandidate = (*candidates)[0];
	if (!firstCandidate.is_object()){
		return "";
	}

	auto content = firstCandidate.find("content");
	if(content == firstCandidate.end() || !content->is_object()){
		return "";
	}

	auto parts = content->find("parts");
	if(parts == content->end() || !parts->is_array() || parts->empty()){
		return "";
	}

	std::string text;
	for(const nlohmann::json& part : *parts){
		if(!part.is_object()){
			continue;
		}
		auto textField = part.find("text");
		if(textField != part.end() && textField->is_string()){
			text += textField->get<std::string>();
		}
	}
	return text;
}

// explain why a response had no text (for logging)
static std::string ExplainNoText(const nlohmann::json& parsed){
	if(!parsed.is_object()){
		return "Response was not a JSON object";
	}

	if(parsed.contains("promptFeedback") && parsed["promptFeedback"].is_object()){
		std::string blockReason = parsed["promptFeedback"].value("blockReason", "");
		if(!blockReason.empty()){
			return "Prompt was blocked: " + blockReason;
		}
	}

	if(parsed.contains("candidates") && parsed["candidates"].is_array() && !parsed["candidates"].empty()){
		const nlohmann::json& firstCandidate = parsed["candidates"][0];
		if(firstCandidate.is_object()){
			std::string finishReason = firstCandidate.value("finishReason", "");
			if(!finishReason.empty()){
				return "No text returned. Finish reason: " + finishReason;
			}
		}
	}
	return "Response contained no text.";
}

std::string GeminiApiService::GenerateResponse(const std::string& prompt, const std::string& apiKey) {
//if key or prompt missing
	if (apiKey.empty() || prompt.empty()){
		LogGeminiError("GenerateResponse", 0, "API key or prompt is empty");
		return "";
	}

	try{
		// escape quotes, newlines, and backslashes in the prompt
		nlohmann::json requestBody = {
			{"contents", nlohmann::json::array({
				{
					{"parts", nlohmann::json::array({
						{{"text", prompt}}
					})}
				}
			})}
		};
		const std::string jsonBody = requestBody.dump(-1, ' ', false, nlohmann::json::error_handler_t::replace);

		//send post request
		HttpResponse response = this->httpClient.PostJson(kGenerateUrl, jsonBody, {
			{"x-goog-api-key", apiKey}
		});

		if (!response.Ok()) {
			LogGeminiError("GenerateResponse", response.statusCode, DescribeFailure(response));
			 return ""; 
		}

		nlohmann::json parsed = nlohmann::json::parse(response.body, nullptr, false);
		if (parsed.is_discarded()) {
			LogGeminiError("GenerateResponse", response.statusCode, "Response was not valid JSON");
			return "";
		}

		std::string text = ExtractText(parsed);
		if (text.empty()) {
			LogGeminiError("GenerateResponse", response.statusCode, ExplainNoText(parsed));
		}
		return text;

	} catch (const std::exception& e){
		LogGeminiError("GenerateResponse", 0, e.what());
	}
	return "";
}

