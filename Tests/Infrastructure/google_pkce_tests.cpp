#include "doctest/doctest.h"
#include <string>
#include "../../Infrastructure/GoogleAuth/google_pkce.h"

static std::string ToHex(const std::string& bytes) {
	static const char digits[] = "0123456789abcdef";
	std::string out;
	for (unsigned char c : bytes) {
		out.push_back(digits[c >> 4]);
		out.push_back(digits[c & 15]);
	}
	return out;
}

TEST_CASE("Sha256 matches the known digest of an empty string") {
	CHECK(ToHex(Sha256("")) == "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855");
}

TEST_CASE("Sha256 matches the known digest of abc") {
	CHECK(ToHex(Sha256("abc")) == "ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad");
}

TEST_CASE("Sha256 handles input longer than one block") {
	CHECK(ToHex(Sha256("abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq"))
		== "248d6a61d20638b8e5c026930c3e6039a33ce45964ff2167f6ecedd419db06c1");
}

TEST_CASE("Base64UrlEncode uses the URL-safe alphabet without padding") {
	CHECK(Base64UrlEncode("") == "");
	CHECK(Base64UrlEncode("f") == "Zg");
	CHECK(Base64UrlEncode("fo") == "Zm8");
	CHECK(Base64UrlEncode("foo") == "Zm9v");
	CHECK(Base64UrlEncode(std::string("\xfb\xff", 2)) == "-_8");
}

TEST_CASE("CodeChallengeS256 matches the RFC 7636 example") {
	CHECK(CodeChallengeS256("dBjftJeZ4CVP-mB92K27uhbUJU1p1r_wW1gFWFOEjXk")
		== "E9Melhoa2OwvFrEMTJguCHaoeK1t8URWbuGJSstw-cM");
}

TEST_CASE("RandomUrlSafeString gives 43 URL-safe characters for 32 bytes and differs each time") {
	const std::string first = RandomUrlSafeString(32);
	const std::string second = RandomUrlSafeString(32);
	CHECK(first.size() == 43);
	CHECK(first != second);
	CHECK(first.find_first_not_of("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_") == std::string::npos);
}