#ifndef GOOGLE_CLIENT_CONFIG_H
#define GOOGLE_CLIENT_CONFIG_H

#include <string>

struct GoogleClientConfig{
    std::string clientId;
    std::string clientSecret;
};

GoogleClientConfig ParseGoogleClientConfig(const std::string& text);

GoogleClientConfig LoadGoogleClientConfig(const std::string& path = "google_client.json");

#endif