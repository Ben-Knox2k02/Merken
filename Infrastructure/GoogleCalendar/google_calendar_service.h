#ifndef GOOGLE_CALENDAR_SERVICE_REAL_H
#define GOOGLE_CALENDAR_SERVICE_REAL_H

#include "../../Application/Services/calendar_api_service.h"
#include "../../Application/Services/http_client.h"

class GoogleCalendarService : public ICalendarAPIService {
	public:
		explicit GoogleCalendarService(IHttpClient& httpClient) : httpClient(httpClient) {}

		CalendarEvent CreateEvent(const CalendarEvent& event, const std::string& apiKey) override;
		std::optional<CalendarEvent> GetEvent(const std::string& googleEventId, const std::string& apiKey) override;
		std::vector<CalendarEvent> GetEvents(const std::string& apiKey) override;
		bool UpdateEvent(const CalendarEvent& event, const std::string& apiKey) override;

	private:
		IHttpClient& httpClient;
};

#endif
