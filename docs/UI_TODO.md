# UI due tasks

Call **use cases** from event handlers. Do not talk to SQLite or `I*Repository`. Pattern is in [README.md](README.md#ui).

New study use cases (not wired yet):

| Use case | Request | What you get back |
|---|---|---|
| `StudyDeckUseCase` | `deckId` | `deckName`, `dueCards` (`CardResponse`: id, front, back, tags) |
| `ReviewCardUseCase` | `deckId`, `cardId`, `remembered` | `success`, `nextReviewDate`, today’s `cardsReviewed` / `cardsCorrect` / `retentionRate` |

```cpp
#include "../../Application/UseCases/Deck/StudyDeck/study_deck_usecase.h"
#include "../../Application/UseCases/Deck/ReviewCard/review_card_usecase.h"
```

Progress use cases (not wired yet). Do **not** query `daily_progress` or `IDailyProgressRepository`. Empty history dates default to the last 12 weeks ending today (computed in the use case).

| Use case | Request | What you get back |
|---|---|---|
| `GetTodaysProgressUseCase` | none (`Execute()`) | `date`, `cardsReviewed`, `cardsCorrect`, `retentionRate` |
| `GetProgressHistoryUseCase` | `startDate`, `endDate` (ISO; empty → last 12 weeks ending today) | days: `date`, `cardsReviewed`, `cardsCorrect`, `retentionRate` (one row per day in range, zeros if no reviews) |

```cpp
#include "../../Application/UseCases/Progress/GetTodaysProgress/get_todays_progress_usecase.h"
#include "../../Application/UseCases/Progress/GetProgressHistory/get_progress_history_usecase.h"
```

API keys (not wired yet). Do **not** write `app_settings.json` or call `IAppSettingsService`. Gemini/Calendar already load keys with `GetSettings()`. Storage is already implemented.

| Use case | Request | What you get back |
|---|---|---|
| `SaveAppSettingsUseCase` | `calendarApiKey`, `aiApiKey` | `true` if saved |

```cpp
#include "../../Application/UseCases/SaveAppSettings/save_app_settings_usecase.h"
```

Calendar events (not wired yet). Do **not** call `ICalendarAPIService` or Google Calendar. `GetEventsUseCase` loads the calendar API key.

| Use case | Request | What you get back |
|---|---|---|
| `GetEventsUseCase` | none (`Execute()`) | events: `calendarEventId`, `title`, `description`, `startTime`, `endTime`, `reminderMinutes`, `googleEventId` |

```cpp
#include "../../Application/UseCases/Calendar/GetEvents/get_events_usecase.h"
```

---

## Study deck

- [ ] **Study Deck** (menu and/or card-list button) runs `StudyDeckUseCase` with the selected `deckId`. Do **not** use `GetCardsUseCase` for study — that is every card, including ones not due.
- [ ] If `dueCards` is empty, show “Nothing due today” and stay out of the flip/grade loop.
- [ ] Study screen shows **front** first; user reveals **back**; then **Remembered** / **Forgot** (not Previous/Next as the grade).
- [ ] Keep the `dueCards` list in the panel. After a successful review, drop that card and show the next one.

## Review (grade)

- [ ] Remembered / Forgot calls `ReviewCardUseCase` (`remembered = true/false`). If `success` is false, show an error and do not advance.
- [ ] Do **not** wait until the user leaves study to save. Each grade is already persisted by the use case (card schedule + today’s progress).
- [ ] When `dueCards` is empty after a grade, show an end summary from the last response: cards reviewed, cards correct, retention rate. Back/Change Deck does not need a separate “end session” use case.

## Progress on screen

- [ ] Session progress bar: remaining / starting size of `dueCards` (e.g. `4 / 15`).
- [ ] Optional: show today’s `cardsReviewed` / `retentionRate` from `ReviewCardResponse` on the study screen.
- [ ] Wire Today's Progress: call `GetTodaysProgressUseCase` (`Execute()`, no request). Do not query `daily_progress` / `IDailyProgressRepository`.
- [ ] Heatmap of previous days: call `GetProgressHistoryUseCase` (empty dates → last 12 weeks ending today). Map `cardsReviewed` to intensity; missing days are already zeros (intensity 0). Do not query `daily_progress` / `IDailyProgressRepository`.

## Settings (API keys)

- [ ] **File → Settings** opens a dialog. On OK, save with `SaveAppSettingsUseCase`. Leave the fields empty; do not load existing keys (Gemini/Calendar read them via `IAppSettingsService.GetSettings()`).
- [ ] Fields: Gemini API key (`aiApiKey`) and Calendar API key (`calendarApiKey`). Use password-style inputs. If save returns `false`, show an error and keep the dialog’s values.
- [ ] Do **not** write `app_settings.json`, `merken.db`, or `%APPDATA%\Merken\settings.bin` yourself. The use case persists keys (Windows: DPAPI in the user data dir).

## Calendar (view events)

- [ ] Opening **Calendar** runs `GetEventsUseCase` (`Execute()`, no request) and shows the returned events. An empty list is valid (no events, missing key, or API failure).
- [ ] Do **not** call `ICalendarAPIService`, Google Calendar, or `IAppSettingsService`. The use case loads `calendarApiKey`.

## Don’t

- Do not `create<IDeckRepository>()`, `IDailyProgressRepository`, `IDateProviderService`, `IAppSettingsService`, or `ICalendarAPIService`.
- Do not compute due-ness or next-review dates in the UI. `StudyDeck` already filtered; `ReviewCard` already scheduled.
- Do not write `app_settings.json`. API keys go through File → Settings (`SaveAppSettingsUseCase`).
