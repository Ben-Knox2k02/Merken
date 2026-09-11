#ifndef COMPOSITION_ROOT_H
#define COMPOSITION_ROOT_H

#include <boost/di.hpp>

#include "../Application/ServiceInterfaces/course_db_service.h"
#include "../Application/ServiceInterfaces/calendar_api_service.h"
#include "../Application/ServiceInterfaces/ai_api_service.h"
#include "../Application/ServiceInterfaces/app_settings_service.h"
#include "../Application/ServiceInterfaces/uuid_generator_service.h"

#include "../Infrastructure/Persistence/Course/course_db_service.h"
#include "../Infrastructure/GoogleCalendar/google_calendar_service.h"
#include "../Infrastructure/GeminiApi/gemini_api_service.h"
#include "../Infrastructure/AppSettings/app_settings_service.h"
#include "../Infrastructure/Uuid/uuid_generator_service.h"

namespace di = boost::di;

// wires service interfaces to their Infrastructure implementations as app-lifetime singletons
inline auto MakeInjector() {
	return di::make_injector(
		di::bind<ICourseDBService>().to<CourseDbService>().in(di::singleton),
		di::bind<ICalendarAPIService>().to<GoogleCalendarService>().in(di::singleton),
		di::bind<IAIAPIService>().to<GeminiApiService>().in(di::singleton),
		di::bind<IAppSettingsService>().to<AppSettingsService>().in(di::singleton),
		di::bind<IUuidGeneratorService>().to<UuidGeneratorService>().in(di::singleton)
	);
}

#endif
