#include "import_google_events_usecase.h"
#include <exception>
#include <string>
#include <vector>

namespace {

bool ContainsGoogleEventId(const std::vector<std::string>& googleEventIds, const std::string& googleEventId) {
	for (const std::string& storedId : googleEventIds) {
		if (storedId == googleEventId) {
			return true;
		}
	}
	return false;
}

}

ImportGoogleEventsResponse ImportGoogleEventsUseCase::Execute() {
	ImportGoogleEventsResponse response;
	try {
		const std::string accessToken = this->authService.GetAccessToken();
		if (accessToken.empty()) {
			return response;
		}

		const std::vector<CalendarEvent> remoteEvents = this->calendarAPIService.GetEvents(accessToken);
		std::vector<std::string> knownIds;
		for (const CalendarEvent& stored : this->eventRepository.GetEvents()) {
			if (!stored.GetGoogleEventId().empty()) {
				knownIds.push_back(stored.GetGoogleEventId());
			}
		}

		for (const CalendarEvent& remoteEvent : remoteEvents) {
			const std::string googleEventId = remoteEvent.GetGoogleEventId();
			if (googleEventId.empty() || ContainsGoogleEventId(knownIds, googleEventId)) {
				continue;
			}
			if (this->eventRepository.AddEvent(remoteEvent) <= 0) {
				continue;
			}
			knownIds.push_back(googleEventId);
			response.imported += 1;
		}
	} catch (const std::exception&) {
		response.googleFailed = true;
	}
	return response;
}
