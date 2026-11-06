#include "doctest/doctest.h"
#include "../../Infrastructure/GoogleAuth/google_client_config.h"

TEST_CASE("ParseGoogleClientConfig reads the simple format") {
	GoogleClientConfig config = ParseGoogleClientConfig(R"({"clientId":"id","clientSecret":"secret"})");
	CHECK(config.clientId == "id");
	CHECK(config.clientSecret == "secret");
}

TEST_CASE("ParseGoogleClientConfig reads the file downloaded from Google") {
	GoogleClientConfig config = ParseGoogleClientConfig(R"({"installed":{"client_id":"id","client_secret":"secret"}})");
	CHECK(config.clientId == "id");
	CHECK(config.clientSecret == "secret");
}

TEST_CASE("ParseGoogleClientConfig returns empty values for bad input") {
	CHECK(ParseGoogleClientConfig("not json").clientId == "");
	CHECK(ParseGoogleClientConfig("[]").clientId == "");
	CHECK(ParseGoogleClientConfig("{}").clientSecret == "");
}

TEST_CASE("LoadGoogleClientConfig returns empty values for a missing file") {
	CHECK(LoadGoogleClientConfig("this_file_does_not_exist.json").clientId == "");
}