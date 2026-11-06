#include "google_pkce.h"
#include <cstdint>
#include <random>

namespace {
    const uint32_t kRoundConstans[64] = {
        0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
        0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
        0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
        0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
        0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
        0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
        0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
        0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
    };

    uint32_t RotateRight(uint32_t value, int bits){
        return (value >> bits) | (value << (32 - bits));
    }
}

std::string Sha256(const std::string& data){
    uint32_t h[8] = {
        0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a,
        0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19
    };

    std::string message = data;
    const uint64_t bitLength = static_cast<uint64_t>(data.size()) * 8;
    message.push_back(static_cast<char>(0x80));
    while (message.size() % 64 != 56) {
        message.push_back('\0');
    }
    for (int i = 7; i >= 0; --i) {
        message.push_back(static_cast<char>((bitLength >> (i * 8)) & 0xff));
    }

    for(size_t offset = 0; offset < message.size(); offset += 64) {
        uint32_t w[64];
        for(int i = 0; i < 16; ++i) {
            w[i] = (static_cast<uint8_t>(message[offset + i * 4]) << 24) |
                   (static_cast<uint8_t>(message[offset + i * 4 + 1]) << 16) |
                   (static_cast<uint8_t>(message[offset + i * 4 + 2]) << 8) |
                   (static_cast<uint8_t>(message[offset + i * 4 + 3]));
        }
        for(int i = 16; i < 64; ++i) {
            uint32_t s0 = RotateRight(w[i - 15], 7) ^ RotateRight(w[i - 15], 18) ^ (w[i - 15] >> 3);
            uint32_t s1 = RotateRight(w[i - 2], 17) ^ RotateRight(w[i - 2], 19) ^ (w[i - 2] >> 10);
            w[i] = w[i - 16] + s0 + w[i - 7] + s1;
        }
        uint32_t a = h[0], b = h[1], c = h[2], d = h[3], e = h[4], f = h[5], g = h[6], hh = h[7];
        for(int i = 0; i < 64; ++i) {
            uint32_t s1 = RotateRight(e, 6) ^ RotateRight(e, 11) ^ RotateRight(e, 25);
            uint32_t ch = (e & f) ^ ((~e) & g);
            uint32_t temp1 = hh + s1 + ch + kRoundConstans[i] + w[i];
            uint32_t s0 = RotateRight(a, 2) ^ RotateRight(a, 13) ^ RotateRight(a, 22);
            uint32_t maj = (a & b) ^ (a & c) ^ (b & c);
            uint32_t temp2 = s0 + maj;
            hh = g; g = f; f = e; e = d + temp1; d = c; c = b; b = a; a = temp1 + temp2;
        }
        h[0] += a; h[1] += b; h[2] += c; h[3] += d; h[4] += e; h[5] += f; h[6] += g; h[7] += hh;
    }

    std::string digest;
    digest.reserve(32);
    for(int i = 0; i < 8; ++i) {
        digest.push_back(static_cast<char>((h[i] >> 24) & 0xff));
        digest.push_back(static_cast<char>((h[i] >> 16) & 0xff));
        digest.push_back(static_cast<char>((h[i] >> 8) & 0xff));
        digest.push_back(static_cast<char>(h[i] & 0xff));
    }
    return digest;

}

std::string Base64UrlEncode(const std::string& bytes) {
	static const char alphabet[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_";
	std::string out;
	size_t i = 0;
	while (i + 2 < bytes.size()) {
		const uint32_t triple = (static_cast<uint32_t>(static_cast<unsigned char>(bytes[i])) << 16)
			| (static_cast<uint32_t>(static_cast<unsigned char>(bytes[i + 1])) << 8)
			| static_cast<uint32_t>(static_cast<unsigned char>(bytes[i + 2]));
		out.push_back(alphabet[(triple >> 18) & 63]);
		out.push_back(alphabet[(triple >> 12) & 63]);
		out.push_back(alphabet[(triple >> 6) & 63]);
		out.push_back(alphabet[triple & 63]);
		i += 3;
	}
	const size_t remaining = bytes.size() - i;
	if (remaining == 1) {
		const uint32_t value = static_cast<uint32_t>(static_cast<unsigned char>(bytes[i])) << 16;
		out.push_back(alphabet[(value >> 18) & 63]);
		out.push_back(alphabet[(value >> 12) & 63]);
	} else if (remaining == 2) {
		const uint32_t value = (static_cast<uint32_t>(static_cast<unsigned char>(bytes[i])) << 16)
			| (static_cast<uint32_t>(static_cast<unsigned char>(bytes[i + 1])) << 8);
		out.push_back(alphabet[(value >> 18) & 63]);
		out.push_back(alphabet[(value >> 12) & 63]);
		out.push_back(alphabet[(value >> 6) & 63]);
	}
	return out;
}

std::string CodeChallengeS256(const std::string& verifier) {
	return Base64UrlEncode(Sha256(verifier));
}

std::string RandomUrlSafeString(size_t byteCount) {
	std::random_device device;
	std::string bytes;
	bytes.reserve(byteCount);
	for (size_t i = 0; i < byteCount; ++i) {
		bytes.push_back(static_cast<char>(device() & 0xff));
	}
	return Base64UrlEncode(bytes);
}
