#ifndef COMPOSITION_ROOT_H
#define COMPOSITION_ROOT_H

#include <string>

// wxWidgets defines `_` for translations; Boost.DI uses `_` as a type.
#ifdef _
#pragma push_macro("_")
#undef _
#define MERKEN_RESTORE_WX_UNDERSCORE
#endif
#include <boost/di.hpp>
#ifdef MERKEN_RESTORE_WX_UNDERSCORE
#pragma pop_macro("_")
#undef MERKEN_RESTORE_WX_UNDERSCORE
#endif

#include "../Application/ServiceInterfaces/course_db_service.h"
#include "../Application/ServiceInterfaces/calendar_api_service.h"
#include "../Application/ServiceInterfaces/ai_api_service.h"
#include "../Application/ServiceInterfaces/app_settings_service.h"

#include "../Infrastructure/Persistence/DatabaseContext/database_context.h"
#include "../Infrastructure/Persistence/Course/course_db_service.h"
#include "../Infrastructure/GoogleCalendar/google_calendar_service.h"
#include "../Infrastructure/GeminiApi/gemini_api_service.h"
#include "../Infrastructure/AppSettings/app_settings_service.h"

namespace di = boost::di;

inline constexpr const char* kDatabaseFileName = "merken.db";

// wires service interfaces to their Infrastructure implementations as app-lifetime singletons
inline auto MakeInjector() {
	return di::make_injector(
		di::bind<DatabaseContext>().in(di::singleton).to<DatabaseContext>(std::string{kDatabaseFileName}),
		di::bind<ICourseDBService>().to<CourseDbService>().in(di::singleton),
		di::bind<ICalendarAPIService>().to<GoogleCalendarService>().in(di::singleton),
		di::bind<IAIAPIService>().to<GeminiApiService>().in(di::singleton),
		di::bind<IAppSettingsService>().to<AppSettingsService>().in(di::singleton)
	);
}

#endif
