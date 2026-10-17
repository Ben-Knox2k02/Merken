#ifndef AI_STUDY_USE_CASE_H
#define AI_STUDY_USE_CASE_H

#include "ai_study_request.h"
#include "ai_study_response.h"
#include "../../../Repositories/deck_repository.h"
#include "../../../Services/ai_api_service.h"
#include "../../../Services/ai_study_cache_service.h"
#include "../../../Services/app_settings_service.h"

class AiStudyUseCase {
	public:
		AiStudyUseCase(
			IDeckRepository& deckRepository,
			IAIAPIService& aiApiService,
			IAppSettingsService& appSettingsService,
			IAiStudyCacheService& aiStudyCacheService
		) : deckRepository(deckRepository),
			aiApiService(aiApiService),
			appSettingsService(appSettingsService),
			aiStudyCacheService(aiStudyCacheService) {}

		AiStudyResponse Execute(const AiStudyRequest& request);

	private:
		IDeckRepository& deckRepository;
		IAIAPIService& aiApiService;
		IAppSettingsService& appSettingsService;
		IAiStudyCacheService& aiStudyCacheService;
};

#endif
