#include "doctest/doctest.h"
#include "../../../../Application/UseCases/AI/RecordAiStudyGrade/record_ai_study_grade_usecase.h"
#include "../../../Fakes/fake_ai_study_cache_service.h"
#include "../../../../DomainModels/ai_question.h"

TEST_CASE("RecordAiStudyGradeUseCase records got-it and missed-it on the cache") {
	FakeAiStudyCacheService cache;
	cache.SetSitting(1, "Spanish", {
		AiQuestion(7, AiQuestionType::FillIn, "___ means hello.", "Hello"),
		AiQuestion(8, AiQuestionType::Sentence, "What means goodbye?", "Goodbye")
	});
	RecordAiStudyGradeUseCase useCase(cache);

	RecordAiStudyGradeRequest gotIt;
	gotIt.gotIt = true;
	RecordAiStudyGradeResponse first = useCase.Execute(gotIt);

	CHECK(first.success);
	CHECK(first.correct == 1);
	CHECK(first.wrong == 0);
	CHECK(first.percentage == doctest::Approx(1.0));
	CHECK_FALSE(first.complete);
	CHECK(cache.correct == 1);
	CHECK(cache.wrong == 0);

	RecordAiStudyGradeRequest missed;
	missed.gotIt = false;
	RecordAiStudyGradeResponse second = useCase.Execute(missed);

	CHECK(second.success);
	CHECK(second.correct == 1);
	CHECK(second.wrong == 1);
	CHECK(second.percentage == doctest::Approx(0.5));
	CHECK(second.complete);
	CHECK(cache.recordCount == 2);
}

TEST_CASE("RecordAiStudyGradeUseCase fails when there is no sitting") {
	FakeAiStudyCacheService cache;
	RecordAiStudyGradeUseCase useCase(cache);

	RecordAiStudyGradeRequest request;
	request.gotIt = true;

	RecordAiStudyGradeResponse response = useCase.Execute(request);

	CHECK_FALSE(response.success);
	CHECK(response.correct == 0);
	CHECK(response.wrong == 0);
	CHECK(response.percentage == doctest::Approx(0.0));
	CHECK_FALSE(response.complete);
}

TEST_CASE("RecordAiStudyGradeUseCase fails after every question is graded") {
	FakeAiStudyCacheService cache;
	cache.SetSitting(1, "Spanish", {
		AiQuestion(7, AiQuestionType::FillIn, "___ means hello.", "Hello")
	});
	RecordAiStudyGradeUseCase useCase(cache);

	RecordAiStudyGradeRequest request;
	request.gotIt = true;
	CHECK(useCase.Execute(request).complete);

	RecordAiStudyGradeResponse extra = useCase.Execute(request);
	CHECK_FALSE(extra.success);
	CHECK(extra.correct == 1);
	CHECK(extra.wrong == 0);
	CHECK(extra.complete);
	CHECK(cache.correct == 1);
}
