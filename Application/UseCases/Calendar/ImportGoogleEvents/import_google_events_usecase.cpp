#include "import_google_events_usecase.h"
#include <exception>
#include <string>
#include <vector>
#include <set>

namespace {

const CalendarEvent* FindByGoogleEventId(const std::vector<CalendarEvent>& events, const std::string& googleEventId) {
	for (const CalendarEvent& event : events) {
		if (event.GetGoogleEventId() == googleEventId) {
			return &event;
		}
	}
	return nullptr;
}

bool SameContent(const CalendarEvent& a, const CalendarEvent& b) {
	return a.GetTitle() == b.GetTitle()
		&& a.GetDate().ToIso() == b.GetDate().ToIso()
		&& a.GetStartTime() == b.GetStartTime()
		&& a.GetEndTime() == b.GetEndTime()
		&& a.GetLocation() == b.GetLocation()
		&& a.GetDescription() == b.GetDescription()
		&& a.GetReminderMinutes() == b.GetReminderMinutes();
}

CalendarEvent WithLocalId(const CalendarEvent& remote, int localId) {
	CalendarEvent merged(localId, remote.GetTitle(), remote.GetDate(), remote.GetStartTime(),
		remote.GetEndTime(), remote.GetLocation(), remote.GetDescription());
	merged.UpdateReminderMinutes(remote.GetReminderMinutes());
	merged.SetGoogleEventId(remote.GetGoogleEventId());
	return merged;
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
		const std::vector<CalendarEvent> storedEvents = this->eventRepository.GetEvents();
		std::set<std::string> handledIds;
		for (const CalendarEvent& remoteEvent : remoteEvents) {
			const std::string googleEventId = remoteEvent.GetGoogleEventId();
			if (googleEventId.empty()) {
				continue;
			}
			if (!handledIds.insert(googleEventId).second) { continue;}

			const CalendarEvent* stored = FindByGoogleEventId(storedEvents, googleEventId);
			if (stored == nullptr) {
				//new on Google
				if (this->eventRepository.AddEvent(remoteEvent) > 0) {
					response.imported += 1;
				}
			} else if (!SameContent(*stored, remoteEvent)) {
				//edited on Google
				if (this->eventRepository.UpdateEvent(WithLocalId(remoteEvent, stored->GetCalendarEventId()))) {
					response.updated += 1;
				}
			}
		}
	} catch (const std::exception&) {
		response.googleFailed = true;
	}
	return response;
}