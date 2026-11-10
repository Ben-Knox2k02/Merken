#ifndef GOOGLE_PKCE_H
#define GOOGLE_PKCE_H

#include <cstddef>
#include <string>

//SHA256 of the input
std::string Sha256(const std::string& data);
std::string Base64UrlEncode(const std::string& bytes);
std::string CodeChallengeS256(const std::string& verifier);
std::string RandomUrlSafeString(size_t byteCount);

#endif