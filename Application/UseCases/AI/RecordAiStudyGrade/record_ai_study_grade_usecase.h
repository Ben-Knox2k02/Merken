#ifndef RECORD_AI_STUDY_GRADE_USE_CASE_H
#define RECORD_AI_STUDY_GRADE_USE_CASE_H

#include "record_ai_study_grade_request.h"
#include "record_ai_study_grade_response.h"
#include "../../../Services/ai_study_cache_service.h"

class RecordAiStudyGradeUseCase {
	public:
		explicit RecordAiStudyGradeUseCase(IAiStudyCacheService& aiStudyCacheService)
			: aiStudyCacheService(aiStudyCacheService) {}

		RecordAiStudyGradeResponse Execute(const RecordAiStudyGradeRequest& request);

	private:
		IAiStudyCacheService& aiStudyCacheService;
};

#endif
