#ifndef CARD_ROW_H
#define CARD_ROW_H

#include <string>
#include <vector>
#include <sstream>
#include <optional>
#include <wx/wxsqlite3.h>
#include "../../../DomainModels/card.h"

inline std::string JoinCsv(const std::vector<std::string>& values) {
	std::string joined;
	for (size_t i = 0; i < values.size(); ++i) {
		if (i > 0) { joined += ", "; }
		joined += values[i];
	}
	return joined;
}

inline std::vector<std::string> SplitCsv(const std::string& values) {
	std::vector<std::string> result;
	std::stringstream stream(values);
	std::string item;
	while (std::getline(stream, item, ',')) {
		const auto start = item.find_first_not_of(" \t");
		if (start == std::string::npos) { continue; }
		const auto end = item.find_last_not_of(" \t");
		result.push_back(item.substr(start, end - start + 1));
	}
	return result;
}

inline Card MapCardRow(wxSQLite3::ResultSet& result) {
	Card card(
		result.GetInt(0),
		result.GetInt(1),
		result.GetString(2).ToStdString(),
		result.GetString(3).ToStdString()
	);
	card.UpdateTags(Tag::Split(result.GetString(4).ToStdString()));
	if (result.GetInt(5) == static_cast<int>(CardType::MultipleChoice)) {
		card.SetMultipleChoice(SplitCsv(result.GetString(6).ToStdString()));
	}
	card.SetReviewState(result.GetInt(9), result.GetDouble(10), result.GetInt(11));
	std::optional<Date> nextReviewDate = Date::Parse(result.GetString(7).ToStdString());
	if (nextReviewDate.has_value()) {
		card.SetNextReviewDate(*nextReviewDate);
	}
	std::optional<Date> lastReviewedDate = Date::Parse(result.GetString(8).ToStdString());
	if (lastReviewedDate.has_value()) {
		card.SetLastReviewedDate(*lastReviewedDate);
	}
	return card;
}

#endif
