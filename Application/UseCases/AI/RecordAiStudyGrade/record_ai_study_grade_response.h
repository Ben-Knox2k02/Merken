#ifndef RECORD_AI_STUDY_GRADE_RESPONSE_H
#define RECORD_AI_STUDY_GRADE_RESPONSE_H

struct RecordAiStudyGradeResponse {
	bool success;
	int correct;
	int wrong;
	double percentage;
	bool complete;
};

#endif
