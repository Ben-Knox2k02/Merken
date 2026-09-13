#include "record_ai_study_grade_usecase.h"

RecordAiStudyGradeResponse RecordAiStudyGradeUseCase::Execute(
	const RecordAiStudyGradeRequest& request
) {
	RecordAiStudyGradeResponse response;
	response.success = this->aiStudyCacheService.RecordGrade(request.gotIt);

	const int correct = this->aiStudyCacheService.GetCorrect();
	const int wrong = this->aiStudyCacheService.GetWrong();
	const int answered = correct + wrong;
	response.correct = correct;
	response.wrong = wrong;
	response.percentage = answered == 0 ? 0.0 : static_cast<double>(correct) / answered;
	response.complete = !this->aiStudyCacheService.GetQuestions().empty()
		&& answered >= static_cast<int>(this->aiStudyCacheService.GetQuestions().size());
	return response;
}
