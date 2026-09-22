#ifndef USER_PROFILE_H
#define USER_PROFILE_H

#include "date.h"
#include <optional>
#include <string>

enum class GuideStep {
	CreateDeck,
	AddCard,
	StudyDeck,
	None
};

class UserProfile {
	public:
		static constexpr const char* kFallbackDisplayName = "Profile";

		UserProfile(
			const Date& startDate,
			const std::string& displayName = "",
			const std::string& note = "",
			const std::string& imagePath = "",
			std::optional<int> dailyGoal = std::nullopt,
			bool usesAiStudy = false,
			bool guideFinished = false
		) : startDate(startDate),
			displayName(displayName),
			note(note),
			imagePath(imagePath),
			dailyGoal(dailyGoal),
			usesAiStudy(usesAiStudy),
			guideFinished(guideFinished) {}

		const Date& GetStartDate() const { return this->startDate; }
		const std::string& GetDisplayName() const { return this->displayName; }
		const std::string& GetNote() const { return this->note; }
		const std::string& GetImagePath() const { return this->imagePath; }
		std::optional<int> GetDailyGoal() const { return this->dailyGoal; }
		bool UsesAiStudy() const { return this->usesAiStudy; }
		bool IsGuideFinished() const { return this->guideFinished; }

		bool HasImage() const { return !this->imagePath.empty(); }
		bool HasDailyGoal() const { return this->dailyGoal.has_value() && *this->dailyGoal > 0; }

		std::string HeaderName() const {
			const std::string trimmed = UserProfile::Trim(this->displayName);
			if (trimmed.empty()) {
				return UserProfile::kFallbackDisplayName;
			}
			return trimmed;
		}

		std::string Initials() const {
			const std::string name = this->HeaderName();
			std::string letters;
			bool inWord = false;
			for (char c : name) {
				const bool space = c == ' ' || c == '\t';
				if (space) {
					inWord = false;
					continue;
				}
				if (!inWord) {
					letters.push_back(UserProfile::Upper(c));
					inWord = true;
					if (letters.size() == 2) {
						break;
					}
				}
			}
			if (letters.empty()) {
				return "P";
			}
			return letters;
		}

		bool IsGoalMet(int cardsReviewed) const {
			if (!this->HasDailyGoal()) {
				return false;
			}
			return cardsReviewed >= *this->dailyGoal;
		}

		GuideStep NextGuideStep(int deckCount, std::optional<int> selectedDeckCardCount) const {
			if (this->guideFinished) {
				return GuideStep::None;
			}
			if (deckCount <= 0) {
				return GuideStep::CreateDeck;
			}
			if (!selectedDeckCardCount.has_value()) {
				return GuideStep::None;
			}
			if (*selectedDeckCardCount <= 0) {
				return GuideStep::AddCard;
			}
			return GuideStep::StudyDeck;
		}

		void UpdateDisplayName(const std::string& newName) { this->displayName = newName; }
		void UpdateNote(const std::string& newNote) { this->note = newNote; }
		void SetImagePath(const std::string& path) { this->imagePath = path; }
		void ClearImage() { this->imagePath.clear(); }
		void SetUsesAiStudy(bool enabled) { this->usesAiStudy = enabled; }
		void MarkGuideFinished() { this->guideFinished = true; }

		void SetDailyGoal(std::optional<int> goal) {
			if (!goal.has_value() || *goal <= 0) {
				this->dailyGoal = std::nullopt;
				return;
			}
			this->dailyGoal = goal;
		}

	private:
		static std::string Trim(const std::string& value) {
			size_t start = 0;
			while (start < value.size() && (value[start] == ' ' || value[start] == '\t')) {
				start += 1;
			}
			size_t end = value.size();
			while (end > start && (value[end - 1] == ' ' || value[end - 1] == '\t')) {
				end -= 1;
			}
			return value.substr(start, end - start);
		}

		static char Upper(char c) {
			if (c >= 'a' && c <= 'z') {
				return static_cast<char>(c - 'a' + 'A');
			}
			return c;
		}

		Date startDate;
		std::string displayName;
		std::string note;
		std::string imagePath;
		std::optional<int> dailyGoal;
		bool usesAiStudy;
		bool guideFinished;
};

#endif
