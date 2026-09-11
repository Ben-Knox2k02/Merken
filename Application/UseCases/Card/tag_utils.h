#ifndef TAG_UTILS_H
#define TAG_UTILS_H

#include <string>
#include <sstream>
#include <vector>

inline std::vector<std::string> SplitTags(const std::string& tags) {
	std::vector<std::string> result;
	std::stringstream stream(tags);
	std::string item;
	while (std::getline(stream, item, ',')) {
		const auto start = item.find_first_not_of(" \t");
		if (start == std::string::npos) { continue; }
		const auto end = item.find_last_not_of(" \t");
		result.push_back(item.substr(start, end - start + 1));
	}
	return result;
}

inline std::string JoinTags(const std::vector<std::string>& tags) {
	std::string joined;
	for (size_t i = 0; i < tags.size(); ++i) {
		if (i > 0) { joined += ", "; }
		joined += tags[i];
	}
	return joined;
}

#endif
