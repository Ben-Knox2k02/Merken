# Software Requirements Specification (SRS)

**Product:** Merken  
**Version:** 1.7  
**Date:** 7 October 2026  
**Status:** Draft  
**Based on:** the team SRS through version 1.6, aligned with `main`

| Version | Date | Description |
|---|---|---|
| 1.0 | — | Initial draft (`MerkinSRS.pdf`) |
| 1.1 | 11 Sep 2026 | Expanded to a full SRS: completed requirements, use cases, data model, review/progress behavior, interfaces, and traceability |
| 1.2 | 13 Sep 2026 | Dropped deck import/export (not needed for this course) |
| 1.3 | 13 Sep 2026 | Specified AI study: quiz from the selected deck via `AiPrompt` / `AiStudyUseCase`; not a general chatbot |
| 1.4 | 13 Sep 2026 | AI study session grade: self-grade after reveal, end summary (right/wrong/%); no review dates or daily progress |
| 1.5 | 13 Sep 2026 | AI questions modeled as `AiQuestion` (Sentence or FillIn); one Gemini call returns the list; in-memory `IAiStudyCacheService` singleton |
| 1.6 | 13 Sep 2026 | Sitting grades (correct/wrong/%) live on `IAiStudyCacheService`; `RecordAiStudyGradeUseCase` (not the panel, not `ReviewCard`) |
| 1.7 | 7 Oct 2026 | Shared SRS matched to `main`: local user profile, onboarding, first-run guide, delete deck/card, deck card counts, shipped `daily_progress` and `user_profile` tables, theme kept with app settings, settings stored in the OS user-data directory |

---

## Table of Contents

1. [Introduction](#1-introduction)
2. [Overall Description](#2-overall-description)
3. [External Interface Requirements](#3-external-interface-requirements)
4. [Functional Requirements](#4-functional-requirements) (including [User profile](#49-user-profile-and-first-run-guide))
5. [Use Cases](#5-use-cases)
6. [Non-Functional Requirements](#6-non-functional-requirements)
7. [Data Requirements](#7-data-requirements)
8. [Other Requirements](#8-other-requirements)
9. [Appendices](#9-appendices)

---

## 1. Introduction

### 1.1 Purpose

This Software Requirements Specification describes the functional and non-functional requirements for **Merken**, a desktop flashcard application (referred to as “Merkin” in the initial draft).

Many people find studying time-consuming and difficult to get around to. Merken aims to reduce that friction by giving the user an environment where they can study on their own time, at their own pace, with flashcards.

This document is the agreement among the development team, testers, and course stakeholders about what the system shall do. It is the basis for design, implementation, and acceptance.

### 1.2 Scope

Merken is a cross-platform desktop application with a native user interface. In scope:

- Creating, listing, updating, and deleting flashcard decks
- Creating, viewing, editing, and deleting cards (front, back, tags)
- Reviewing cards on a spaced-repetition schedule
- Tracking daily study progress (counts, retention, heatmap, remaining-for-today)
- One local learner profile: display name, note, start date, daily review goal, onboarding, and a first-run guide
- An AI testing feature that quizzes the user from known cards in a selected deck (not a general chatbot)
- Study events that sync with Google Calendar
- Local persistence in SQLite
- No login required for core use

Out of scope for the core release (stretch goals, see §2.7):

- Cloud database (Supabase) and multi-device sync
- User accounts / authentication
- Profile photos or shared profiles
- Multiple-choice flashcards (domain support exists; UI/behavior is optional)

The system does **not** replace a general calendar client or a general-purpose chatbot. Calendar and AI are study aids around the flashcard core.

### 1.3 Definitions, Acronyms, and Abbreviations

| Term | Definition |
|---|---|
| **Deck** | A named collection of flashcards. |
| **Card** | A flashcard with a front (prompt), a back (answer), optional tags, and spaced-repetition scheduling fields. |
| **Recall card** | A “think of the answer” card. The user sees the front, then reveals the back, then grades themselves. |
| **Multiple-choice card** | A card that presents a prompt and a list of choices (stretch). |
| **Spaced repetition** | A review method that sets the next interval for a card based on whether the user remembered it. |
| **Review** | One grading of one card: remembered or not. Updates that card’s schedule and that day’s progress. |
| **Daily progress** | Per-day aggregate: cards reviewed, cards correct, and retention rate. |
| **Retention rate** | `cardsCorrect / cardsReviewed` for a given day; `0` if nothing was reviewed. |
| **Due card** | A card whose `nextReviewDate` is today or earlier, or a new card that has never been reviewed. |
| **SRS** | Software Requirements Specification (this document), or spaced-repetition system, depending on context. |
| **SQLite** | A lightweight, file-based SQL database engine used for local data storage. |
| **wxWidgets** | A cross-platform C++ GUI framework used to build the desktop client. |
| **Gemini** | Google’s LLM API used for the AI testing feature. |
| **AI study** | An optional quiz from a deck’s known cards via Gemini. Separate from spaced-repetition **review**. Session grades (got it / missed it) do not change intervals. |
| **AI sitting** | One start-test through end-summary. `IAiStudyCacheService` holds the question list **and** sitting grades (correct/wrong) in memory. Not SQLite. Replaced on a new start-test; cleared on failed start. |
| **User profile** | The one local learner row: display name, note, start date, optional daily goal, AI-study flag, guide-finished flag, and onboarding-finished flag. Not an account. |
| **Daily goal** | Optional positive count of cards to review today. Unset means no goal. Met when today’s `cardsReviewed` reaches it. |
| **Header name** | Trimmed display name, or `"Profile"` when the trimmed name is empty. |
| **Initials** | First letter of the first two words of the header name, uppercased. `"P"` when the header name is the fallback. |
| **Guide step** | `CreateDeck`, `AddCard`, `StudyDeck`, or `None`. Derived by `UserProfile::NextGuideStep`. |
| **Onboarding** | The one-time capture of display name and daily goal. `CompleteOnboardingUseCase` marks it finished. |
| **AiQuestion** | One generated quiz item: `cardId`, `type` (`Sentence` or `FillIn`), `text`, `back`. Not persisted. |
| **AiQuestionType** | `Sentence` — a question in words (typically `?`), no cloze blank. `FillIn` — cloze with `___` where the answer goes. Not `CardType`. |
| **AiPrompt** | Domain object. `Build(deck)` asks Gemini for one tagged question per card. |
| **AiQuestion::Parse** | Parses one reply line into an `AiQuestion` for a card. |
| **AiQuestion::ParseMultiple** | Parses a full Gemini reply into a `vector<AiQuestion>` in card order. |
| **Supabase** | *(stretch)* Backend-as-a-service providing hosted Postgres, auth, and storage. |
| **PostgREST** | *(stretch)* The REST API layer Supabase uses. |
| **API** | Application Programming Interface. |
| **NFR** | Non-functional requirement. |
| **FR** | Functional requirement. |
| **UC** | Use case. |

### 1.4 References

1. wxWidgets documentation — https://docs.wxwidgets.org/
2. C++ reference — https://cplusplus.com/doc/tutorial/
3. IEEE Std 830-1998, *IEEE Recommended Practice for Software Requirements Specifications*
4. Google Gemini API documentation
5. Google Calendar API documentation
6. Merken source repository (this project), including `DomainModels/`, `Application/UseCases/`, and `docs/README.md`

### 1.5 Overview of This Document

- **Section 2** describes the product in context: users, environment, constraints, assumptions, and what is core vs stretch.
- **Section 3** specifies external interfaces (UI, SQLite, Gemini, Calendar).
- **Section 4** lists functional requirements by feature, with priority.
- **Section 5** specifies use cases (one user action each).
- **Section 6** lists non-functional requirements.
- **Section 7** specifies the data the system stores.
- **Section 8** covers remaining constraints (threading, security of keys, offline).
- **Section 9** holds glossary, UI mockups, diagrams, the review algorithm, and a traceability matrix.

### 1.6 Document Conventions

| Keyword | Meaning |
|---|---|
| **shall** | Mandatory for the core release (Must). |
| **should** | Expected and planned, but not release-blocking (Should). |
| **may** | Optional / stretch (Could). |

Requirement IDs are stable (`FR-D1`, `NFR-1`, `UC-07`, …). Priority uses MoSCoW: **Must / Should / Could / Won’t**.

### 1.7 Intended Audience

- The Merken development team (UI, Application, Database, APIs)
- Course instructor and graders
- Testers writing acceptance tests from use cases

---

## 2. Overall Description

### 2.1 Product Perspective

Merken is a **new standalone desktop application**. It is not a plugin or a web app.

It owns a local SQLite file (`merken.db`) on the user’s machine. Optionally, when the user supplies API keys and a network connection is available, it talks to:

- **Google Gemini** — AI testing from known cards
- **Google Calendar** — create/view study events

```
                    ┌─────────────────────────────────┐
                    │            Merken               │
   Student ────────▶│  wxWidgets UI                   │
                    │  Use cases                      │
                    │  Domain (Deck, Card,            │
                    │          DailyProgress,         │
                    │          UserProfile, …)        │
                    │           │                     │
                    │           ▼                     │
                    │      SQLite (merken.db)         │
                    └───────────┬─────────────────────┘
                                │  (optional, online)
                     ┌──────────┴──────────┐
                     ▼                     ▼
              Gemini API           Google Calendar API
                     │
                     ▼  (stretch)
                 Supabase
```

Core study (decks, cards, review, daily progress) works with **no internet and no account**.

### 2.2 Product Functions

At a high level the product:

1. Lets the user create, list, update, and delete **decks**. The list includes each deck’s card count.
2. Lets the user create, list, update, and delete **cards** inside a deck (front, back, tags).
3. Lets the user **review** due cards: show front, reveal back, mark remembered or not, then apply spaced repetition and show the next due card.
4. **Tracks daily progress**: cards reviewed, cards correct, retention rate; shows a heatmap of previous days and a progress bar of cards remaining today.
5. Keeps one **local profile**: display name, note, start date, optional daily goal, onboarding, and a first-run guide.
6. Lets the user **schedule study sessions** and sync them with Google Calendar.
7. Lets the user **test recall with an AI** on a selected deck: one start-test call, walk the question list, self-grade after revealing the back, see right/wrong/% at the end (does not change review dates).
8. Lets the user **enter API keys** (Gemini, Calendar) in settings, and keeps a saved theme when those keys are saved.
9. **Persists** decks, cards, daily progress, and the user profile locally in SQLite.
10. *(Could)* Multiple-choice cards and cloud sync.

### 2.3 User Classes and Characteristics

| Class | Characteristics | Privileges |
|---|---|---|
| **Student / learner** | Primary user. Studying material that needs memorization (language, math, history, etc.). Assumed to be familiar with flashcards. Uses a personal desktop or laptop. | All features on the local database. |

There is no administrator, no multi-user login, and no role model in the core release. One local database per installation.

### 2.4 Operating Environment

| Item | Requirement |
|---|---|
| OS | Windows 10+, macOS 12+, Arch-based Linux (e.g. CachyOS) |
| Hardware | Low; no GPU-intensive work |
| Runtime | Native C++ desktop process using wxWidgets |
| Local store | SQLite file `merken.db` |
| Network | Required only for Google Calendar and AI study. **Not** required for deck management, card management, review, daily progress, or the local profile. |

### 2.5 Design and Implementation Constraints

- Frontend and application logic shall be C++.
- UI shall be wxWidgets.
- Local data shall be SQLite.
- GUI shall run on the **main thread**. wxWidgets has no built-in async UI model.
- Google Calendar and AI API calls shall run on **worker threads** and post results back to the UI thread.
- Cross-platform differences in native appearance and behavior are accepted; custom rendering requires manual work.
- Layers shall not call across each other except through use cases and service interfaces (see project `docs/README.md`).
- Secrets (API keys) shall not be hardcoded; they come from `AppSettings`.
- Identifiers for decks, cards, and calendar events are integers. `0` means “new row”; SQLite assigns the auto-increment value.
- The user profile is a single row (`profile_id = 1`), not an auto-increment account id.
- Builds are the repo scripts (`build.bat`, `./build.sh`), documented in `docs/SETUP.md`. CMake, vcpkg, and Conan are not required.
- Supabase is a stretch goal only, not a core dependency.

### 2.6 Assumptions and Dependencies

**Assumptions**

- The user is on a personal desktop or laptop.
- Internet is available when using Calendar or AI, but not for core study.
- Card content is primarily text. Small media may be considered later; it is not required for core.
- The user is familiar with flashcard apps (front/back, flip, grade).
- “Cards remaining in the day” means due cards for today that have not yet been reviewed today.
- API keys and the theme string are stored in the OS user-data directory, not in `merken.db` and not in a repository `app_settings.json`. Windows stores a DPAPI-protected `settings.bin`. Other platforms store `settings.json` with user-only file permissions.
- Dates used by review, daily progress, and the profile start date are calendar dates in the local timezone, stored as strings.
- One review of one card updates both that `Card` and that day’s `DailyProgress`.
- There is one profile row per database. The first `GetUserProfileUseCase` creates it.

**Dependencies**

| Dependency | Used for |
|---|---|
| wxWidgets | Native UI |
| SQLite (wxSQLite3) | Local persistence |
| Google Calendar API | External calendar sync |
| Google Gemini (or compatible LLM API) | AI testing |
| Boost.DI | Composition root / injection |
| Repo build scripts (`build.bat`, `./build.sh`) | Cross-platform builds. Setup is `docs/SETUP.md`. |
| Supabase *(stretch)* | Cloud sync |

### 2.7 Apportioning of Requirements

| Bucket | Contents |
|---|---|
| **Must (core)** | Decks, recall cards, local SQLite, spaced-repetition review, daily progress record + today view, local profile (name, note, start date, daily goal, onboarding, first-run guide), Calendar create event, AI study quiz from selected-deck cards (session grade + end summary), API key input, offline core |
| **Should** | Progress heatmap, remaining-today progress bar, progress history, view calendar events, delete deck/card, an `usesAiStudy` flag stored on the profile |
| **Could (stretch)** | Multiple-choice cards, Supabase sync, user accounts |
| **Won’t (this course)** | Import/export decks, mobile apps, shared/class decks, social features, profile photos, full calendar client, general-purpose AI assistant |

---

## 3. External Interface Requirements

### 3.1 User Interfaces

The main window (`MainFrame`) has a menu bar (**File, Edit, Study, Calendar, AI, Help**), a left **DeckPanel**, a right **active panel** that swaps (card list, study, calendar, AI), and a status bar.

Detailed wireframes are in [Appendix B](#appendix-b--ui-wireframes).

UI rules:

- The UI shall call **use cases** from event handlers. It shall not talk to SQLite, Gemini, Google Calendar, or repositories directly.
- The deck list shall show each deck’s name and card count from `GetDecksUseCase`.
- The profile header shall show `headerName` and `initials` from `GetUserProfileUseCase`. An empty display name still shows `"Profile"` / `"P"`.
- Empty states shall be shown when there are no decks or no cards (centered message).
- Errors from network APIs shall be shown to the user without crashing (status bar or dialog).
- The UI should stay simple and avoid expensive widgets so the experience stays responsive (NFR-2).

### 3.2 Hardware Interfaces

None beyond a standard desktop/laptop (keyboard, mouse/trackpad, display). No special hardware.

### 3.3 Software Interfaces

| Interface | Direction | Data | Notes |
|---|---|---|---|
| **SQLite (`merken.db`)** | Read/write | Decks, cards, daily progress, one user profile | Created if missing. Schema applied via ordered SQL scripts in `db_scripts.cpp`. Shipped scripts are not edited or reordered. |
| **`IUserProfileRepository`** | Read/write | The single `user_profile` row | `GetUserProfile` / `SaveUserProfile`. Bound in `DiComposition`. |
| **`IAppSettingsService`** | Read/write | `aiApiKey`, `calendarApiKey`, `theme` | OS user-data directory. Windows: DPAPI `settings.bin`. Elsewhere: `settings.json`. |
| **`IAIAPIService.GenerateResponse(prompt, apiKey)`** | Outbound HTTPS | Prompt in, text out | Gemini. Requires `aiApiKey` and network. Empty string on missing key or failure. One call per start-test. |
| **`IAiStudyCacheService`** | In-memory | Sitting: `AiQuestion` list + `correct` / `wrong` | Singleton. Not SQLite. `SetSitting` resets grades. `RecordGrade(gotIt)` updates counters. |
| **`ICalendarAPIService.CreateEvent` / `GetEvents` / `UpdateEvent`** | Outbound HTTPS | `CalendarEvent` | Google Calendar. Requires `calendarApiKey` and network. |
| **`IHttpClient`** | GET / PostJson | JSON bodies, headers | Shared HTTP used by Gemini and Calendar. Keys sent as headers, not hardcoded. |

### 3.4 Communications Interfaces

- HTTPS to Google APIs when Calendar or AI is used.
- No server of our own in the core release.
- Timeout/failure: the use case shall return a failure the UI can display; core study remains available.

---

## 4. Functional Requirements

Priorities: **Must** = shall, **Should** = should, **Could** = may.

### 4.1 Deck Management

| ID | Requirement | Priority |
|---|---|---|
| **FR-D1** | The system shall allow the user to create a deck with a name and an optional description. | Must |
| **FR-D2** | The system shall list existing decks (id, name, description, created-at, and card count). | Must |
| **FR-D3** | The system shall allow the user to update a deck’s name and description. | Must |
| **FR-D4** | Selecting a deck shall show that deck’s cards in the active panel. | Must |
| **FR-D5** | The system should allow the user to delete a deck (and its cards). | Should |

### 4.2 Card Management

| ID | Requirement | Priority |
|---|---|---|
| **FR-C1** | The system shall allow the user to create a card in a selected deck with front, back, and optional comma-separated tags. | Must |
| **FR-C2** | The system shall list the cards in a selected deck (id, front, back, tags). | Must |
| **FR-C3** | The system shall allow the user to update a card’s front, back, and tags. | Must |
| **FR-C4** | New cards shall default to **Recall** type (“think of the answer”). | Must |
| **FR-C5** | The system may support **multiple-choice** cards with a list of choices. | Could |
| **FR-C6** | The system should allow the user to delete a card. | Should |

### 4.3 Card Review (Spaced Repetition)

Review is **scheduling**, not editing. Algorithm details are in [Appendix D](#appendix-d--spaced-repetition-algorithm).

| ID | Requirement | Priority |
|---|---|---|
| **FR-R1** | The system shall let the user start a study session for a selected deck. | Must |
| **FR-R2** | The session shall present **due** cards: `nextReviewDate` ≤ today, plus cards never reviewed. | Must |
| **FR-R3** | For a recall card, the system shall show the front, then reveal the back on user action (click / “Show Answer”). | Must |
| **FR-R4** | After the back is shown, the user shall mark the card **remembered** or **not remembered**. | Must |
| **FR-R5** | On grade, the system shall call `Card::RecordReview(remembered, reviewedOnDate)` and then set `nextReviewDate` from `intervalDays` + `reviewedOnDate`. | Must |
| **FR-R6** | The updated card shall be persisted. | Must |
| **FR-R7** | After grading, the system shall show the next due card, or an end-of-session state if none remain. | Must |
| **FR-R8** | The study UI should show session progress (e.g. 4 / 15 due today). | Should |
| **FR-R9** | The user should be able to edit the current card from the study screen and return to study afterward. | Should |
| **FR-R10** | Review shall work fully **offline**. | Must |

`RecordReview` does **not** change front, back, tags, or card type. It updates `lastReviewedDate`, `repetitionCount`, `intervalDays`, and `easeFactor` only.

### 4.4 Daily Progress

`DailyProgress` is keyed by **date**. The user never “creates a progress day.” The first review of a day creates it; later reviews of that day call `RecordCard`.

| ID | Requirement | Priority |
|---|---|---|
| **FR-P1** | Each card review shall also record into that date’s `DailyProgress` via `RecordCard(remembered)`: increment `cardsReviewed`; if remembered, increment `cardsCorrect`. | Must |
| **FR-P2** | The system shall display today’s progress: date, cards reviewed, cards correct, and retention rate. | Must |
| **FR-P3** | Retention rate shall be `cardsCorrect / cardsReviewed`, or `0` when reviewed is `0`. | Must |
| **FR-P4** | The system should display a **heatmap** of cards reviewed on previous days. | Should |
| **FR-P5** | The system should display a **progress bar** of cards remaining today (due today, not yet reviewed today). | Should |
| **FR-P6** | The system should return progress for a date range (history / heatmap source). | Should |

There is no user-facing Create/Update/Delete Daily Progress. Those are persistence details inside review and get.

### 4.5 Calendar Integration

| ID | Requirement | Priority |
|---|---|---|
| **FR-CAL1** | The system shall allow the user to create a study event with title, description, start time, end time, and reminder minutes (default **60**). | Must |
| **FR-CAL2** | On submit, the system shall sync the event to Google Calendar via the Calendar API. | Must |
| **FR-CAL3** | The system should allow the user to view events on their Google Calendar. | Should |
| **FR-CAL4** | Calendar features shall require a configured calendar API key and a network connection; on failure the UI shall report the error and leave local study unaffected. | Must |

### 4.6 AI Testing

AI study is a **quiz on the selected deck**, not a general-purpose chatbot. Original FR-4: use known cards in sentences to test the user.

The UI shall call `AiStudyUseCase` and `RecordAiStudyGradeUseCase`. It shall not call `IAIAPIService`, Gemini, or `IAiStudyCacheService` directly. Gemini I/O shall run on a worker thread (NFR-4).

| ID | Requirement | Priority |
|---|---|---|
| **FR-AI1** | The system shall provide an AI testing feature that quizzes the user from **known cards** (front/back) of the **selected deck** (sentences or fill-ins built from those cards). | Must |
| **FR-AI2** | **Start test** shall make **one** Gemini call for the **whole selected deck**. The use case shall parse the reply into `AiQuestion` items, store them in `IAiStudyCacheService`, and return that list. **Next**, reveal, and self-grade shall not call Gemini. | Must |
| **FR-AI3** | AI calls shall use the user-supplied Gemini API key (`aiApiKey`). | Must |
| **FR-AI4** | If the key is missing, the network fails, or Gemini returns an empty reply, the system shall show an error and shall not block core study. | Must |
| **FR-AI5** | AI testing shall not replace spaced-repetition review. It shall not call `ReviewCardUseCase`, `Card::RecordReview`, or `DailyProgress::RecordCard`. It shall not change `nextReviewDate` / intervals or write the heatmap. | Must |
| **FR-AI6** | The prompt shall be built by `AiPrompt::Build(deck)`. Each item is an `AiQuestion`: `cardId`, `type` (`Sentence` or `FillIn`), `text`, `back`. Fill-in uses a `___` blank; sentence is a question with no cloze. | Must |
| **FR-AI7** | If the deck is missing or has no cards, the system shall fail without calling Gemini. | Must |
| **FR-AI8** | After the back is revealed, the user shall self-grade **Got it** or **Missed it** via `RecordAiStudyGradeUseCase`. That shall update sitting counters on `IAiStudyCacheService`. Typed answers shall not be sent to Gemini or string-matched as the grader. | Must |
| **FR-AI9** | At the end of the sitting (`complete`, or the user stops), the system shall show questions correct, questions wrong, and percentage (`correct / answered`) from the grade response. Counters live on the in-memory sitting cache (not SQLite) and shall be discarded when Start test / change deck runs again or the sitting is cleared. | Must |

### 4.7 Settings and Persistence

| ID | Requirement | Priority |
|---|---|---|
| **FR-S1** | The system shall allow the user to input their Gemini API key. | Must |
| **FR-S2** | The system should allow the user to input their Google Calendar API key. | Should |
| **FR-S3** | Settings shall persist across launches in the OS user-data directory. | Must |
| **FR-S4** | The system shall persist a theme string with app settings. Saving API keys shall keep the theme already stored. | Must |
| **FR-DB1** | The system shall save decks and cards to a local SQLite database on the user’s device. | Must |
| **FR-DB2** | The system shall persist daily progress in the shipped `daily_progress` table so heatmap/history survive restart. | Must |
| **FR-DB3** | No login shall be required for core use. | Must |
| **FR-DB4** | The system shall persist the single user profile in the shipped `user_profile` table. | Must |

### 4.8 Stretch

| ID | Requirement | Priority |
|---|---|---|
| **FR-ST1** | The system may sync a single user’s data to Supabase. | Could |
| **FR-ST2** | The system may implement user accounts. | Could |
| **FR-ST3** | The system may present multiple-choice cards in study (see FR-C5). | Could |

### 4.9 User profile and first-run guide

There is one learner, stored as one `UserProfile`. There is no account and no photo. The UI shall call the profile use cases. It shall not write the `user_profile` table itself.

`GetUserProfileUseCase` creates the row on first read, using today’s date as `startDate`. Later reads do not create another row and do not change `startDate`.

| ID | Requirement | Priority |
|---|---|---|
| **FR-UP1** | The system shall keep one local user profile. The first read shall create it when none is stored, with today’s date as the start date. If that save fails, the read shall fail and shall not pretend a profile exists. | Must |
| **FR-UP2** | The user shall be able to change the display name, note, and daily goal. The start date, guide-finished flag, and onboarding-finished flag shall stay as they are on that update. | Must |
| **FR-UP3** | `HeaderName` shall be the display name with leading and trailing spaces and tabs removed, or `"Profile"` when that is empty. `Initials` shall be the uppercase first letter of the first two words, or `"P"` for the fallback. | Must |
| **FR-UP4** | A daily goal shall be stored only when it is a positive integer. Zero, a negative number, or an empty goal shall clear it. | Must |
| **FR-UP5** | `IsGoalMet` shall be true only when a daily goal is set and the reviewed count is at least that goal. | Must |
| **FR-UP6** | Completing onboarding shall store the display name and daily goal and mark onboarding finished. It shall fail if no profile row exists yet. | Must |
| **FR-UP7** | `NextGuideStep` shall return `CreateDeck` when there are no decks, `AddCard` when a selected deck has no cards, `StudyDeck` when that deck has cards, and `None` when no deck is selected or the guide is already finished. | Must |
| **FR-UP8** | `FinishGuideUseCase` shall mark the guide finished. Calling it again shall succeed and leave the flag set. It shall fail if no profile row exists yet. | Must |
| **FR-UP9** | The profile shall not store a photo. The shipped `image_path` column is dropped and shall not be re-added. | Must |
| **FR-UP10** | The profile should store whether the learner uses AI study. That flag shall not start an AI sitting and shall not replace `AiStudyUseCase`. | Should |

Deleting a deck removes its cards, then the deck. Deleting a card removes that card only when it belongs to the given deck. Both fail when the deck or card is missing. See UC-15 and UC-16.

---

## 5. Use Cases

A **user action = one use case**, matching the application layer. Persistence of a new `DailyProgress` row is **not** its own use case.

| ID | Name | Actor | Priority | Related FRs |
|---|---|---|---|---|
| UC-01 | Create deck | User | Must | FR-D1 |
| UC-02 | List decks | User | Must | FR-D2 |
| UC-03 | Update deck | User | Must | FR-D3 |
| UC-04 | Create card | User | Must | FR-C1 |
| UC-05 | List cards | User | Must | FR-C2, FR-D4 |
| UC-06 | Edit card | User | Must | FR-C3, FR-R9 |
| UC-07 | Review card (study session) | User | Must | FR-R1–R7, FR-R10, FR-P1 |
| UC-08 | View today’s progress | User | Must | FR-P2, FR-P3 |
| UC-09 | View progress history / heatmap | User | Should | FR-P4, FR-P6 |
| UC-10 | Create calendar event | User | Must | FR-CAL1, FR-CAL2, FR-CAL4 |
| UC-11 | View calendar events | User | Should | FR-CAL3 |
| UC-12 | AI study test | User | Must | FR-AI1–AI7, FR-AI9 |
| UC-13 | Enter API keys | User | Must | FR-S1–S4 |
| UC-14 | Record AI sitting grade | User | Must | FR-AI8, FR-AI9 |
| UC-15 | Delete deck | User | Should | FR-D5 |
| UC-16 | Delete card | User | Should | FR-C6 |
| UC-17 | Get user profile | User | Must | FR-UP1, FR-UP3, FR-DB4 |
| UC-18 | Update user profile | User | Must | FR-UP2, FR-UP4, FR-UP10 |
| UC-19 | Complete onboarding | User | Must | FR-UP4, FR-UP6 |
| UC-20 | Finish guide | User | Must | FR-UP7, FR-UP8 |

### UC-01 Create Deck

**Goal:** Add a new deck.  
**Preconditions:** App is running.  
**Main flow:**

1. User chooses to add a deck (DeckPanel “Add deck” or equivalent).
2. User enters a name and optional description.
3. User confirms.
4. System creates the deck in SQLite and refreshes the deck list.

**Postcondition:** A new deck exists and is visible.  
**Exceptions:** Empty name — system shall reject and keep the previous list.

### UC-02 List Decks

**Goal:** See all decks.  
**Main flow:** On launch (and after create, update, or delete), `GetDecksUseCase` loads decks and shows them. Each `DeckResponse` includes `deckId`, `name`, `description`, `createdAt`, and `cardCount`. `cardCount` is the number of cards in that deck. The list response does not ask the UI to edit those cards.

### UC-03 Update Deck

**Goal:** Rename or re-describe a deck.  
**Preconditions:** At least one deck exists.  
**Main flow:** User selects a deck, opens edit, changes name/description, saves. System persists and refreshes the list.

### UC-04 Create Card

**Actors:** User  
**Preconditions:** A deck is selected.

**Main flow:**

1. User selects the deck that will own the card.
2. User clicks **Create Card** / **Add card**.
3. User fills in the front and back (and optional tags).
4. User saves the card to the deck.

**Postcondition:** The card is stored with Recall type, `intervalDays = 0`, `easeFactor = 2.5`, `repetitionCount = 0`, empty review dates. It appears in the card list and is due for study.  
**Exceptions:** Missing front or back — system shall reject.

### UC-05 List Cards

**Preconditions:** A deck is selected (or a default deck).  
**Main flow:** System loads cards for `deckId` and shows them in CardListPanel (front → back). If the deck has no cards, show an empty state.

### UC-06 Edit Card

**Actors:** User  
**Preconditions:** A card exists.

**Main flow (from list):**

1. User selects a card in the list (or opens edit from study).
2. User changes front, back, and/or tags.
3. User saves.
4. System persists content/tags only (scheduling fields are unchanged).

**Alternate — from study (original draft):**

1. User is on the study screen for a card.
2. User opens edit (cog).
3. User corrects information and hits save.
4. User is brought back to the study screen of that card.

### UC-07 Review Card (Study Session)

This is the primary write path for both `Card` scheduling and `DailyProgress`.

**Actors:** User  
**Preconditions:** A deck with at least one due card. Works **offline**.

**Main flow:**

1. User chooses a deck.
2. User clicks **Study Cards** / **Study Deck**.
3. System loads due cards for that deck and shows the first card’s **front**.
4. User reveals the back (“Show Answer” / click).
5. User marks **remembered** or **not remembered**.
6. System:
   1. Loads the card.
   2. `card.RecordReview(remembered, today)`.
   3. Sets `nextReviewDate` from `intervalDays`.
   4. Persists the card.
   5. Loads or creates `DailyProgress(today)` and calls `RecordCard(remembered)`.
   6. Persists daily progress.
7. System shows the next due card, or an end-of-session summary (today’s reviewed / correct / retention).

**Extensions:**

- No due cards: show “Nothing due today.”
- User edits the card mid-session (UC-06), then returns to study.
- User leaves the session early: already-graded cards stay saved; remaining due cards stay due.

**Postconditions:** Graded cards have updated SRS fields; today’s `DailyProgress` reflects those grades.

### UC-08 View Today’s Progress

**Goal:** See today’s study stats.  
**Main flow:** User opens the progress/status view (or end-of-session summary). System returns `date`, `cardsReviewed`, `cardsCorrect`, and `RetentionRate()` for today. If no reviews yet, counts are 0 and retention is 0.

### UC-09 View Progress History / Heatmap

**Goal:** See past days’ volume (and optionally retention).  
**Main flow:** User opens the heatmap / history. System returns daily rows for a date range. UI maps `cardsReviewed` to heatmap intensity. Progress bar for **today** uses due-remaining, not the heatmap.

### UC-10 Create Calendar Event

**Actors:** User  
**Preconditions:** Calendar API key configured; network available.

**Main flow:**

1. User opens Calendar.
2. User clicks a date (or **Create Event**), opening the event UI.
3. User fills title, description, start, end, reminder.
4. User submits.
5. The app creates the event and syncs it with Google Calendar.

**Exceptions:** Missing key or network failure — show error; no local study data is changed.

### UC-11 View Calendar Events

**Goal:** Look at events on the user’s Google Calendar (original FR-2).  
**Preconditions:** Same as UC-10.  
**Main flow:** User opens Calendar; system fetches and displays events. *(Should)*

### UC-12 AI Study Test

**Goal:** Quiz the user from the selected deck’s known cards, then show a session score. Not spaced-repetition study.  
**Actors:** User  
**Preconditions:** A deck with at least one card is selected. Gemini API key configured; network available.

**Main flow:**

1. User selects a deck and opens **AI**.
2. User starts a test. UI calls `AiStudyUseCase` **once** (`deckId`).
3. System loads the deck, builds `AiPrompt::Build(deck)`, calls Gemini **once**, parses with `AiQuestion::ParseMultiple` (same order as the cards), stores them on `IAiStudyCacheService`, and returns the list.
4. UI walks that list (like `dueCards` on the study panel). Sitting grades on the cache start at 0.
5. For each item:
   1. Show `text` (`FillIn` has `___`; `Sentence` has no cloze).
   2. User may think or type; do **not** send that text to Gemini.
   3. Reveal `back`.
   4. User taps **Got it** or **Missed it**. UI calls `RecordAiStudyGradeUseCase` (`gotIt`). Do **not** call `ReviewCardUseCase`.
   5. Next item is the next index (no Gemini).
6. When `complete` is true, or the user stops, show the **end summary** from the grade response: correct, wrong, percentage.

**Request / response (start test):** `deckId` → `success`, `deckName`, `questions[]` (`cardId`, `type` as `AiQuestionTypeResponse`, `text`, `back`).  
**Request / response (grade):** `gotIt` → `success`, `correct`, `wrong`, `percentage`, `complete`.

**Exceptions:**

- No deck selected, missing deck, or deck with no cards: `success = false`; Gemini is not called. Show an error.
- Missing key, network failure, or empty Gemini reply: `success = false`. Show an error; review remains available.

**Postconditions:** No card schedules, intervals, or daily progress are changed. A new start-test (`SetSitting`) or `Clear` discards the list and the score. No AI transcript is stored.

### UC-13 Enter API Keys

**Main flow:** User opens settings (or the first-time prompt for AI/Calendar), enters Gemini and/or Calendar keys, saves. `SaveAppSettingsUseCase` writes those keys and keeps the theme already stored in `AppSettings`. Subsequent AI/Calendar calls read `AppSettings` from the OS user-data directory.

### UC-14 Record AI Sitting Grade

**Goal:** Record Got it / Missed it for the current AI sitting without changing review dates.  
**Actors:** User  
**Preconditions:** A sitting is in `IAiStudyCacheService` (UC-12 succeeded) and not every item is already graded.

**Main flow:**

1. After the back is revealed, the user taps **Got it** or **Missed it**.
2. UI calls `RecordAiStudyGradeUseCase` with `gotIt` true or false. It shall not call `IAiStudyCacheService` or `ReviewCardUseCase`.
3. System calls `RecordGrade(gotIt)` on the sitting cache.
4. Response: `success`, `correct`, `wrong`, `percentage` (`correct / answered`, or `0` if none), `complete` (answered ≥ question count).

**Exceptions:**

- No sitting, or every question already graded: `success = false`. Counters in the response stay as they are (`complete` is true if the sitting was already finished).

**Postconditions:** Only in-memory sitting counters change. No `RecordReview`, no `DailyProgress`, no heatmap.

### UC-15 Delete Deck

**Goal:** Remove a deck and the cards that belong to it.  
**Preconditions:** The deck exists.  
**Main flow:** User chooses delete. `DeleteDeckUseCase` checks the deck exists, deletes its cards, then deletes the deck.  
**Exceptions:** Unknown deck — return failure and leave other decks unchanged.  
**Postcondition:** That deck and its cards are gone. Other decks are unchanged.

### UC-16 Delete Card

**Goal:** Remove one card from a deck.  
**Preconditions:** The deck exists and contains the card.  
**Main flow:** User chooses delete on a card. `DeleteCardUseCase` checks the deck and the card, then deletes that card.  
**Exceptions:** Unknown deck or unknown card — return failure.  
**Postcondition:** The card is gone. The deck and its other cards remain.

### UC-17 Get User Profile

**Goal:** Load the local learner profile, creating it the first time.  
**Preconditions:** App is running. Works offline.  
**Main flow:**

1. UI calls `GetUserProfileUseCase`.
2. If a row exists, the system returns it.
3. If none exists, the system creates `UserProfile` with today’s date as `startDate` and empty name, note, and goal, then saves it.
4. Response: `ok`, `displayName`, `headerName`, `initials`, `note`, `startDate`, `dailyGoal`, `usesAiStudy`, `guideFinished`, `onboardingFinished`.

**Exceptions:** The new row cannot be saved — `ok` is false and no profile is stored.  
**Postcondition:** Exactly one profile row exists after a successful first read. A later read does not save again.

### UC-18 Update User Profile

**Goal:** Change the editable profile fields.  
**Preconditions:** A profile row exists (UC-17 succeeded).  
**Main flow:** UI calls `UpdateUserProfileUseCase` with `displayName`, `note`, `dailyGoal`, and `usesAiStudy`. The system updates those fields and saves. `startDate`, `guideFinished`, and `onboardingFinished` stay as they were.  
**Exceptions:** No stored profile — return failure. A non-positive `dailyGoal` clears the goal (FR-UP4).  
**Postcondition:** The saved profile has the new editable fields and the original start date.

### UC-19 Complete Onboarding

**Goal:** Record the name and daily goal collected at first run, and mark onboarding finished.  
**Preconditions:** A profile row exists.  
**Main flow:** UI calls `CompleteOnboardingUseCase` with `displayName` and `dailyGoal`. The system stores both and sets onboarding finished.  
**Exceptions:** No stored profile — return failure. Skipping the name leaves the display name empty, so the header stays `"Profile"`. Skipping the goal clears it.  
**Postcondition:** `onboardingFinished` is true. The start date is unchanged.

### UC-20 Finish Guide

**Goal:** Mark the first-run guide finished.  
**Preconditions:** A profile row exists.  
**Main flow:** UI calls `FinishGuideUseCase`. The system sets `guideFinished`.  
**Exceptions:** No stored profile — return failure. If the guide is already finished, the call succeeds and does not write again.  
**Postcondition:** `NextGuideStep` returns `None` for every deck and card count.

The guide step itself is not a stored number. `UserProfile::NextGuideStep(deckCount, selectedDeckCardCount)` derives it (FR-UP7). Steps after opening study (reveal, grade, back) are not separate domain steps.

---

## 6. Non-Functional Requirements

| ID | Requirement | Priority |
|---|---|---|
| **NFR-1** | Core flashcard features (review, deck management, daily progress) shall function fully **offline**. | Must |
| **NFR-2** | The system should avoid expensive UI elements and stay simple so the UI remains highly responsive. | Should |
| **NFR-3** | *(stretch)* The system may sync a single user’s data to Supabase within a reasonable time (on the order of **2 seconds** under normal network conditions). | Could |
| **NFR-4** | The GUI shall run on the main thread. Calendar and AI I/O shall run on worker threads and post back to the UI thread. | Must |
| **NFR-5** | The app shall run on Windows 10+, macOS 12+, and Arch-based Linux. | Must |
| **NFR-6** | Hardware demand shall remain low (no GPU-intensive processing). | Must |
| **NFR-7** | API keys shall not appear in source code. | Must |
| **NFR-8** | Local data shall survive application restart (SQLite `merken.db` plus app settings in the OS user-data directory). | Must |
| **NFR-9** | Cross-platform look-and-feel differences are acceptable; behavior of use cases shall be the same. | Must |
| **NFR-10** | Failure of Calendar or AI shall not corrupt or block the local database. | Must |

---

## 7. Data Requirements

These entities already exist in `DomainModels/` unless noted.

### 7.1 Deck

| Field | Type | Notes |
|---|---|---|
| `deckId` | int | 0 = new; SQLite assigns |
| `name` | string | Required |
| `description` | string | Default empty |
| `createdAt` | string | Set on create |
| `cards` | list of Card | Loaded with the deck when a single deck is opened |

`GetDecksUseCase` also returns `cardCount` for each deck. That count is the number of card rows. It is not its own column on `decks`.

### 7.2 Card

| Field | Type | Notes |
|---|---|---|
| `cardId` | int | 0 = new |
| `deckId` | int | Owner deck |
| `front` | string | Prompt |
| `back` | string | Answer |
| `tags` | list of Tag | Stored as comma-separated text |
| `cardType` | Recall \| MultipleChoice | Default Recall |
| `choices` | list of string | Used only for MultipleChoice |
| `intervalDays` | int | Default 0 |
| `easeFactor` | double | Default 2.5 |
| `repetitionCount` | int | Default 0 |
| `nextReviewDate` | string | Empty = never scheduled / new |
| `lastReviewedDate` | string | Empty = never reviewed |

### 7.3 DailyProgress

Keyed by **date**. No separate numeric id required.

| Field | Type | Notes |
|---|---|---|
| `date` | string | Calendar date |
| `cardsReviewed` | int | Starts at 0 |
| `cardsCorrect` | int | Starts at 0 |

**Behavior**

- `RecordCard(remembered)`: `cardsReviewed += 1`; if remembered, `cardsCorrect += 1`.
- `RetentionRate()`: `cardsCorrect / cardsReviewed`, or `0` if reviewed is 0.

**Persistence:** Table `daily_progress` is already in `db_scripts.cpp`. Columns: `date` (TEXT primary key), `cards_reviewed` (INTEGER, default 0), `cards_correct` (INTEGER, default 0). No retention column. Retention is `DailyProgress::RetentionRate()`.

### 7.4 CalendarEvent

| Field | Type | Notes |
|---|---|---|
| `calendarEventId` | int | Local id |
| `title` | string | Required |
| `description` | string | Optional |
| `startTime` | string | Required |
| `endTime` | string | Required |
| `reminderMinutes` | int | Default 60 |
| `googleEventId` | string | Set after Calendar API create |

### 7.5 AppSettings

Not in `merken.db`. `AppSettingsService` reads and writes the OS user-data directory. Windows uses DPAPI-protected `settings.bin`. Other platforms use `settings.json` with user-only permissions.

| Field | Type | Notes |
|---|---|---|
| `calendarApiKey` | string | Sent only to Calendar calls |
| `aiApiKey` | string | Sent only to Gemini calls |
| `theme` | string | Kept when `SaveAppSettingsUseCase` writes the keys |

`SaveAppSettingsUseCase` does not take a theme argument. It loads the current settings, replaces the two keys, and saves, so a theme already stored is not cleared.

### 7.6 Tag

A named label. Multiple tags on a card are split/joined on commas.

### 7.7 AiQuestion

Not persisted. One generated quiz item for the current sitting. Not a `Card` and not `CardType`.

| Field | Type | Notes |
|---|---|---|
| `cardId` | int | Card this item was generated from (same order as the deck) |
| `type` | `AiQuestionType` | `Sentence` or `FillIn` |
| `text` | string | Prompt shown to the user |
| `back` | string | Card back, for reveal |

| Type | What the user sees | Mark |
|---|---|---|
| **FillIn** | Cloze, e.g. `___ means hello.` | `___` = write the word |
| **Sentence** | Question in words, e.g. `What Spanish word do you use to greet someone?` | no cloze; `?` is punctuation |

### 7.8 AiPrompt

Not persisted. Built once per start-test from the whole `Deck`.

| Field / API | Type | Notes |
|---|---|---|
| `Build(deck)` | static | Returns an `AiPrompt` asking for one tagged line per card |
| `GetText()` | string | Full prompt sent to Gemini |

**`Build` text**

1. Instruction: one question per card, same order; each line `<n>. <type> \| <question>` where type is `sentence` or `fill-in`; sentence has no blank; fill-in uses `___`; do not invent facts; do not include answers.
2. `Deck: <name>`
3. `Cards:` then every card `- Front: … \| Back: …`

**`AiQuestion::Parse(line, card)`** — one line. **`ParseMultiple(reply, cards)`** — full reply, calls `Parse` per line.

- `n. fill-in \| …` / `n. sentence \| …` (also `fillin`, `fill_in`). Unknown type → `Sentence`.
- A line with `___` and no type tag → `FillIn`.
- Extra lines past the card count are ignored. `cardId` and `back` come from the matching card.
- Unnumbered non-empty reply → one `Sentence` on the first card.
- Empty reply → no questions.

Each start-test is one Gemini call. There is no conversation object in the domain.

### 7.9 AI sitting cache (`IAiStudyCacheService`)

In-memory singleton (`InMemoryAiStudyCacheService`). Not SQLite.

| API | Notes |
|---|---|
| `SetSitting(deckId, deckName, questions)` | Replaces the current sitting; **resets** correct/wrong to 0 |
| `GetQuestions()` / `GetDeckId()` / `GetDeckName()` | Current sitting |
| `RecordGrade(gotIt)` | Increments correct or wrong. `false` if no sitting or every item is already graded |
| `GetCorrect()` / `GetWrong()` | Sitting counters |
| `Clear()` | Drops questions and scores |

The UI shall not resolve this service. `AiStudyUseCase` writes the list; `RecordAiStudyGradeUseCase` writes grades.

**Session score (in-memory, not persisted)**

Percentage is `correct / answered` (`answered = correct + wrong`), or `0` if nothing was graded. Same idea as `DailyProgress::RetentionRate`, but it must not be written to `daily_progress` or used on the heatmap.

### 7.10 UserProfile

One row in `user_profile`, `profile_id` fixed at 1. Domain type: `DomainModels/user_profile.h`. Repository: `IUserProfileRepository`, implemented by `UserProfileRepository`.

| Field | Type | Notes |
|---|---|---|
| `profileId` | int | Always 1. Not an account id. |
| `displayName` | string | May be empty. Header uses `HeaderName()`. |
| `note` | string | Optional course or term. Default empty. |
| `startDate` | `Date` | Set on first create from the date provider. Not edited later. |
| `dailyGoal` | optional int | Stored only when `> 0`. Otherwise null. |
| `usesAiStudy` | bool | Default false. Does not run AI study. |
| `guideFinished` | bool | Default false. Set by `FinishGuideUseCase`. |
| `onboardingFinished` | bool | Default false. Set by `CompleteOnboardingUseCase`. |

**Shipped scripts, in order, after `daily_progress`:**

1. `CREATE TABLE user_profile` including a temporary `image_path` column.
2. `ALTER TABLE user_profile DROP COLUMN image_path`.
3. `ALTER TABLE user_profile ADD COLUMN onboarding_finished`.

Do not edit those strings. Do not add a photo column back.

**Behavior**

- `HeaderName()` trims spaces and tabs. Empty result returns `"Profile"`.
- `Initials()` takes the first letter of the first two words of that header name. `"Ada Lovelace"` → `"AL"`. Blank name → `"P"`.
- `SetDailyGoal` clears the goal for a missing, zero, or negative value.
- `IsGoalMet(cardsReviewed)` compares that count with the goal. No goal → false.
- `NextGuideStep(deckCount, selectedDeckCardCount)` follows FR-UP7.

---

## 8. Other Requirements

### 8.1 Security / Privacy

- No account in core: decks, cards, progress, and the profile are local to the machine.
- API keys are local secrets in the OS user-data directory; do not log them and do not put them in `merken.db`.
- The profile has no photo and no password.
- Stretch cloud sync must not be enabled without an explicit user action.

### 8.2 Maintainability

- New user actions are new use cases under `Application/UseCases/<Area>/<Name>/`.
- New persistence needs an interface, a db service, a domain type, and an appended SQL script (never edit shipped scripts).
- New services are bound in `DiComposition/composition_root.h`.

### 8.3 Internationalization

Not required for this course. UI strings may be English-only.

---

## 9. Appendices

### Appendix A — Glossary

Same terms as §1.3. Repeated here for the printed/exported document.

- **Deck:** a collection of flashcards.
- **Spaced repetition:** review that sets intervals based on how well the user did on that card.
- **AI study:** optional quiz from a deck’s known cards via Gemini; not review and not a general chatbot. Sitting grades live on `IAiStudyCacheService` via `RecordAiStudyGradeUseCase` and do not change intervals.
- **AiQuestion:** Sentence (question, no cloze) or FillIn (`___` blank); `cardId`, `text`, `back`.
- **AiPrompt:** `Build(deck)`; one Gemini call for the whole deck. `AiQuestion::Parse` / `ParseMultiple` turn the reply into items.
- **User profile:** one local learner row. Header name, initials, daily goal, onboarding, and the first-run guide. Not an account.
- **Daily goal:** optional positive review count for today. Met when `cardsReviewed` reaches it.
- **Retention rate:** the fraction of cards the user recalled correctly that day.
- **SQLite:** lightweight file-based SQL engine for local storage.
- **Supabase / PostgREST:** stretch cloud backend.
- **wxWidgets:** cross-platform C++ GUI framework for the desktop client.

---

### Appendix B — UI Wireframes

From the initial draft, kept as the target layout.

#### MainFrame

```
File | Edit | Study | Calendar | AI | Help
+----------------------+---------------------------+
| [AL] Ada Lovelace    | [ActivePanel]             |
| [DeckPanel]          | CardListPanel             |
| Spanish          12  | Card 1: front -> back     |
| Linear Algebra    4  | Card 2: front -> back     |
| World History     0  | Card 3: front -> back     |
| [Add deck]           | [Add card] [Study Deck]   |
| [Delete deck]        | [Delete card]             |
+----------------------+---------------------------+
Status Bar: Ready
```

The header is `initials` and `headerName` from `GetUserProfileUseCase` (UC-17). The number on each deck is `cardCount` from `GetDecksUseCase` (UC-02). Delete deck and delete card call UC-15 and UC-16.

#### StudyPanel

```
Spanish
[Hola]
[Show Answer]
[Remembered] [Forgot]     (grade; then auto-advance)
[Edit Card]
Progress: 4 / 15
[Back] [Start Over] [Change Deck]
```

The initial mockup also showed Previous / Next. **Grading then auto-advance is the required study path (UC-07).** Previous/Next may exist as browse-only and shall not by themselves change SRS fields or daily progress.

#### EditPanel

```
Edit Card
Front: [Hola]
Back:  [Hello]
Tags:  [Greeting]
[OK] [Cancel]
```

#### AIPanel

Quiz on the selected deck (not a Copilot chat). Self-grade after reveal; score is session-only.

```
Spanish
___ means hello.          (FillIn)
[Show answer]
Hello
[Got it] [Missed it]
3 / 8

What Spanish word means goodbye?   (Sentence)

(end)
Correct: 6
Wrong: 2
75%
```

**Start test** = one `AiStudyUseCase` call (Gemini once; list stored on `IAiStudyCacheService`). Keep `questions` on the panel. **Show answer** reveals `back`. **Got it** / **Missed it** = `RecordAiStudyGradeUseCase`. End summary from that response. Do not call `ReviewCardUseCase`.

#### CalendarPanel

```
Create Event
Title: Team Meeting
Date: September 4, 2026 | Time: 09:30 - 10:30
Location: NRD 207          (optional; not in current domain)
Description: Weekly discussion of software project
Reminder: 1 hour before
[Create Event] [Cancel]
```

---

### Appendix C — Context / Flow

Core path: **Start → profile (create if missing) → onboarding → MainFrame → (create/select deck) → study due cards → record progress**. Optional paths: Calendar, AI, (stretch) cloud sync.

```
Start
  → GetUserProfileUseCase (create the row on first read)
  → CompleteOnboardingUseCase (name, daily goal)
  → UI (MainFrame)
       → header: headerName + initials
       → NextGuideStep: CreateDeck / AddCard / StudyDeck / None
       → FinishGuideUseCase when the guide is done
       → DeckPanel: list (with cardCount) / add / edit / delete decks
       → CardListPanel: list / add / edit / delete cards
       → StudyPanel: review due cards
            → Card.RecordReview + DailyProgress.RecordCard
            → progress bar + heatmap (today / history)
            → IsGoalMet(cardsReviewed)
       → CalendarPanel: create event → Google Calendar API
       → AIPanel: AiStudyUseCase (once) → IAiStudyCacheService (AiQuestion list)
            → reveal back → RecordAiStudyGradeUseCase (Got it / Missed it)
            → end summary: correct, wrong, %
            (no RecordReview / DailyProgress / nextReviewDate)
  Local data: SQLite (decks, cards, daily_progress, user_profile)
  Settings: OS user-data directory (keys + theme)
  Stretch: User sync with cloud (Supabase)
```

---

### Appendix D — Spaced-Repetition Algorithm

Implemented by `Card::RecordReview(bool remembered, reviewedOnDate)`. Always sets `lastReviewedDate = reviewedOnDate`. Then:

| Outcome | `repetitionCount` | `intervalDays` | `easeFactor` |
|---|---|---|---|
| **Forgot** | set to 0 | set to 1 | `-= 0.2` if still `> 1.3` |
| **Remembered, 1st success** | 1 | 1 | `+= 0.1` |
| **Remembered, 2nd success** | 2 | 6 | `+= 0.1` |
| **Remembered, later** | `+= 1` | `intervalDays * easeFactor` (truncated to int) | `+= 0.1` |

`SetNextReviewDate` is a **separate** step: the use case adds `intervalDays` to `reviewedOnDate` and stores that date. `RecordReview` does not write `nextReviewDate` itself.

This is a simplified SM-2 ladder. Ease has a floor near 1.3 on failure and no documented upper cap on success.

---

### Appendix E — Requirements Traceability

| Requirement | Use case | Domain / notes |
|---|---|---|
| FR-D1–D3 | UC-01–03 | `Deck`, `CreateDeckUseCase` / `GetDecksUseCase` / `UpdateDeckUseCase` |
| FR-D2 card count | UC-02 | `DeckResponse.cardCount` |
| FR-D5 | UC-15 | `DeleteDeckUseCase` deletes the deck’s cards, then the deck |
| FR-C1–C3 | UC-04–06 | `Card`, `CreateCardUseCase` / `GetCardsUseCase` / `UpdateCardUseCase` |
| FR-C6 | UC-16 | `DeleteCardUseCase` |
| FR-R1–R7, FR-P1 | UC-07 | `StudyDeckUseCase`, `ReviewCardUseCase`, `Card::RecordReview`, `DailyProgress::RecordCard` |
| FR-P2–P3 | UC-08 | `GetTodaysProgressUseCase`, `DailyProgress::RetentionRate` |
| FR-P4–P6 | UC-09 | `GetProgressHistoryUseCase`. Empty dates default to the last 12 weeks ending today (today minus 83 days through today). |
| FR-CAL1–2 | UC-10 | `CalendarEvent`, `CreateEventUseCase` |
| FR-CAL3 | UC-11 | `GetEventsUseCase` |
| FR-AI1–7 | UC-12 | `AiQuestion` (Sentence / FillIn), `AiPrompt::Build`, `AiQuestion::ParseMultiple`, `AiStudyUseCase` (one Gemini call) → `IAiStudyCacheService` |
| FR-AI8–9 | UC-14 | `RecordAiStudyGradeUseCase` → `IAiStudyCacheService.RecordGrade`; no `ReviewCardUseCase` |
| FR-S1–S4 | UC-13 | `SaveAppSettingsUseCase`. Theme is preserved. |
| FR-UP1–UP10, FR-DB4 | UC-17–20 | `UserProfile`, `GetUserProfileUseCase`, `UpdateUserProfileUseCase`, `CompleteOnboardingUseCase`, `FinishGuideUseCase` |
| FR-DB1 | UC-01–07, UC-15–16 | SQLite `decks`, `cards` |
| FR-DB2 | UC-07–09 | Shipped `daily_progress` table |
| NFR-1, FR-R10 | UC-01–09, UC-15–20 | No network in those paths |
| FR-C5, FR-ST1–3 | — | Stretch |

Use cases on `main`: `CreateDeckUseCase`, `UpdateDeckUseCase`, `DeleteDeckUseCase`, `GetDecksUseCase`, `StudyDeckUseCase`, `CreateCardUseCase`, `UpdateCardUseCase`, `DeleteCardUseCase`, `GetCardsUseCase`, `ReviewCardUseCase`, `GetTodaysProgressUseCase`, `GetProgressHistoryUseCase`, `CreateEventUseCase`, `GetEventsUseCase`, `UpdateEventUseCase`, `AiStudyUseCase`, `RecordAiStudyGradeUseCase`, `SaveAppSettingsUseCase`, `GetUserProfileUseCase`, `UpdateUserProfileUseCase`, `CompleteOnboardingUseCase`, `FinishGuideUseCase`. Deck import/export is out of scope (Won’t). `UpdateEventUseCase` exists for Calendar event edits; a full edit-event use case is not specified here beyond the service method.

---

### Appendix F — Open Issues

1. **Delete deck/card** — `DeleteDeckUseCase` and `DeleteCardUseCase` are on `main`. Still Should, not a core-release blocker. Deck delete removes that deck’s cards first.
2. **Calendar “Location”** — on the CalendarPanel mockup; not on `CalendarEvent`. Treat as Could unless the domain is extended.
3. **Google Calendar auth** — code uses `calendarApiKey`; OAuth may be required by Google for real accounts. Implementation detail to confirm with the APIs role.
4. **Heatmap range** — `GetProgressHistoryUseCase` uses today minus 83 days through today when the request dates are empty (12 weeks).
5. **Daily remaining count** — defined here as due-today minus reviewed-today. New (never reviewed) cards count toward the daily bar.
6. **Product name** — codebase and database use **Merken**; the initial PDF used **Merkin**. This SRS uses Merken.
7. **Theme** — `AppSettings.theme` is persisted. `SaveAppSettingsUseCase` keeps the stored value and does not accept a new theme. A later theme control must write `theme` without clearing the API keys.
8. **Guide after Study Deck** — `NextGuideStep` stops at `StudyDeck`. Reveal, grade, and Back are not separate domain steps.
9. **Profile photo** — `image_path` was appended and then dropped. Do not add it back (FR-UP9).
10. **`usesAiStudy`** — still a column and an `UpdateUserProfileUseCase` field. AI study itself stays `AiStudyUseCase`, not this flag.

---

*End of SRS v1.7*
