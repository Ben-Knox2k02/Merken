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

#include "../Application/Repositories/deck_repository.h"
#include "../Application/Repositories/daily_progress_repository.h"
#include "../Application/Services/calendar_api_service.h"
#include "../Application/Services/ai_api_service.h"
#include "../Application/Services/app_settings_service.h"
#include "../Application/Services/date_provider_service.h"
#include "../Application/Services/http_client.h"

#include "../Infrastructure/Persistence/DatabaseContext/database_context.h"
#include "../Infrastructure/Persistence/Deck/deck_repository.h"
#include "../Infrastructure/Persistence/DailyProgress/daily_progress_repository.h"
#include "../Infrastructure/GoogleCalendar/google_calendar_service.h"
#include "../Infrastructure/GeminiApi/gemini_api_service.h"
#include "../Infrastructure/AppSettings/app_settings_service.h"
#include "../Infrastructure/DateProvider/date_provider_service.h"
#include "../Infrastructure/Http/wx_http_client.h"

namespace di = boost::di;

inline constexpr const char* kDatabaseFileName = "merken.db";

// wires service interfaces to their Infrastructure implementations as app-lifetime singletons
inline auto MakeInjector() {
	return di::make_injector(
		di::bind<DatabaseContext>().in(di::singleton).to<DatabaseContext>(std::string{kDatabaseFileName}),
		di::bind<IDeckRepository>().to<DeckRepository>().in(di::singleton),
		di::bind<IDailyProgressRepository>().to<DailyProgressRepository>().in(di::singleton),
		di::bind<IHttpClient>().to<WxHttpClient>().in(di::singleton),
		di::bind<ICalendarAPIService>().to<GoogleCalendarService>().in(di::singleton),
		di::bind<IAIAPIService>().to<GeminiApiService>().in(di::singleton),
		di::bind<IAppSettingsService>().to<AppSettingsService>().in(di::singleton),
		di::bind<IDateProviderService>().to<DateProviderService>().in(di::singleton)
	);
}

#endif
