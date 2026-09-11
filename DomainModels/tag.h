#ifndef TAG_H
#define TAG_H

#include <string>
#include <sstream>
#include <vector>

class Tag {
	public:
		explicit Tag(const std::string& name) : name(name) {}

		const std::string& GetName() const { return this->name; }

		static std::vector<Tag> Split(const std::string& tags) {
			std::vector<Tag> result;
			std::stringstream stream(tags);
			std::string item;
			while (std::getline(stream, item, ',')) {
				const auto start = item.find_first_not_of(" \t");
				if (start == std::string::npos) { continue; }
				const auto end = item.find_last_not_of(" \t");
				result.emplace_back(item.substr(start, end - start + 1));
			}
			return result;
		}

		static std::string Join(const std::vector<Tag>& tags) {
			std::string joined;
			for (size_t i = 0; i < tags.size(); ++i) {
				if (i > 0) { joined += ", "; }
				joined += tags[i].GetName();
			}
			return joined;
		}

	private:
		std::string name;
};

#endif
