# Working on Merken

Layers do not call across each other except through the interfaces and use cases below.

| You | Own | Do not touch |
|---|---|---|
| UI | `UI/` | `Infrastructure/`, `Application/Services/`, `Application/Repositories/` |
| Application | `Application/`, `DomainModels/`, `Infrastructure/Persistence/` (repositories) | `UI/`, `db_scripts.cpp`, Gemini, Calendar, HTTP |
| Database | `Infrastructure/Persistence/DatabaseContext/db_scripts.cpp` | `UI/`, repositories, Gemini, Calendar |
| APIs | `Infrastructure/GeminiApi/`, `Infrastructure/GoogleCalendar/` | `UI/`, SQL |

New `.cpp` files under `Application/`, `Infrastructure/`, and `UI/src/` are picked up by `build.bat` and CI. New services must also be bound in `DiComposition/composition_root.h`.

---

## Project timeline

The sprint Gantt chart is in [MerkenGanttChart.pdf](MerkenGanttChart.pdf). The editable Excel workbook is on [SharePoint](https://ostatemailokstate-my.sharepoint.com/:x:/g/personal/luke_scoggins_okstate_edu/IQAl9d_6ChIXS7xOtJ6wrFpCAbzqfI_1UORrJufsh7cqG4U?e=KSnxBA).

---

## Due tasks

Open work for each role is a checklist next to this file. Check items off when done.

| Role | Where to look |
|---|---|
| UI | [UI_TODO.md](UI_TODO.md) |
| Database | [DB_TODO.md](DB_TODO.md) |
| Application | no open task list right now — study/review use cases are in |
| APIs | [API_TODO.md](API_TODO.md) |

---

## Build, test, clean, uninstall

Run these from the repo root.

| Step | Windows | macOS / Linux |
|---|---|---|
| Build the app | `build.bat` | `./build.sh` (or double-click `build.command` on Mac) |
| Run unit tests | `test.bat` | `./test.sh` |
| Remove build artifacts | `clean.bat` | `./clean.sh` |
| Remove artifacts, `merken.db`, and stored API keys | `uninstall.bat` | `./uninstall.sh` |

Windows build needs wxWidgets first (`build_wxWidgets.bat`, or set `WXWIN`). Output is `Merken.exe` / `Merken` / `Merken.app`. Tests do not need wxWidgets.

`clean` deletes `obj/`, binaries, and leftover object files. It does **not** delete decks or API keys. `uninstall` runs `clean`, then deletes `merken.db` and the user-data folder (Windows: `%APPDATA%\Merken`, including DPAPI-protected keys). Close the app first.

---

## UI

Call **use cases** from event handlers. Do not talk to SQLite, Gemini, or Google Calendar.

```cpp
#include "app.h"
#include "../../Application/UseCases/Deck/CreateDeck/create_deck_usecase.h"
wxDECLARE_APP(App);

void OnAddDeck(wxCommandEvent&) {
    CreateDeckRequest request;
    request.name = nameCtrl->GetValue().ToStdString();

    auto useCase = wxGetApp().GetInjector().create<CreateDeckUseCase>();
    CreateDeckResponse response = useCase.Execute(request);

    // update the screen from response
}
```

- Resolve with `create<TheUseCase>()`, never `create<IDeckRepository>()`.
- Fill a `*Request`, call `Execute`, read the `*Response`.
- If a screen action has no use case yet, ask Application to add one.

Existing use cases: `CreateDeckUseCase`, `UpdateDeckUseCase`, `GetDecksUseCase`, `StudyDeckUseCase`, `CreateCardUseCase`, `UpdateCardUseCase`, `GetCardsUseCase`, `ReviewCardUseCase`, `GetTodaysProgressUseCase`, `GetProgressHistoryUseCase`, `CreateEventUseCase`, `UpdateEventUseCase`, `SaveAppSettingsUseCase`.

`Deck` is the aggregate: cards are only loaded and changed through a deck. `GetDeck` loads the deck **and its cards**. `GetDecks` (the list) does not. Card add/list/update/review live under `Application/UseCases/Deck/`. Persist with `IDeckRepository.AddCard` / `UpdateCard`. There is no `ICardRepository`.

---

## Application (business logic)

A user action = one use case. Use cases depend on **interfaces**, never on Infrastructure classes.

**Add a use case**

1. Folder: `Application/UseCases/<Area>/<Name>/`
2. Add `*_request.h`, `*_response.h`, `*_usecase.h`, `*_usecase.cpp`
3. Constructor takes interface references (`IDeckRepository&`, `IAIAPIService&`, …)
4. `Execute` uses domain types (`Deck`, `CalendarEvent`, …) and returns a response

```cpp
CreateDeckUseCase(IDeckRepository& deckRepository);

CreateDeckResponse Execute(const CreateDeckRequest& request);
```

- Do **not** bind use cases in DI. UI does `injector.create<CreateDeckUseCase>()`.
- Need a new capability (e.g. list decks)? Add a method to the interface in `Application/Repositories/` (or `Application/Services/` for Gemini/Calendar), then implement it in Infrastructure.
- IDs are `int` (`cardId`, `deckId`, `calendarEventId`). Pass `0` for a new row; SQLite assigns the auto-increment value.

**Add a repository**

Interfaces live in `Application/Repositories/`. Implementations live in `Infrastructure/Persistence/<Entity>/`. They query and write rows. They do not create tables.

1. Domain type in `DomainModels/`.
2. Interface in `Application/Repositories/` (e.g. `IDeckRepository`).
3. Implementation in `Infrastructure/Persistence/<Entity>/`. Inject `DatabaseContext&`.
4. Ask Database to append a SQL string in `db_scripts.cpp` for any new table or column.
5. Register in `DiComposition/composition_root.h`:

```cpp
#include "../Infrastructure/Persistence/Deck/deck_repository.h"

di::bind<IDeckRepository>().to<DeckRepository>().in(di::singleton),
```

---

## Database

One shared SQLite file: `merken.db`. `DatabaseContext` opens it when constructed, creates the file if missing, and applies scripts. Do not open your own connection.

**Your job is only to add scripts.** Append a SQL string in `Infrastructure/Persistence/DatabaseContext/db_scripts.cpp`. Do not edit or reorder scripts that already shipped. Do not touch repositories, use cases, or how migrations run.

```cpp
R"(
ALTER TABLE decks ADD COLUMN color TEXT NOT NULL DEFAULT '';
)",
```

`DatabaseContext` applies new scripts on init and skips ones that already ran. Application owns the repositories that read and write those tables.

---

## Third-party APIs (Gemini + Google Calendar)

Fill in the stubs. Keep method signatures unless Application agrees to change them.

| Interface | Implementation |
|---|---|
| `IAIAPIService` | `Infrastructure/GeminiApi/gemini_api_service.*` |
| `ICalendarAPIService` | `Infrastructure/GoogleCalendar/google_calendar_service.*` |

**HTTP:** do not use curl/wxWebRequest yourself. Injected `IHttpClient& httpClient` is already there (`Get` / `PostJson`). Keys come from `AppSettings` (`aiApiKey`, `calendarApiKey`) via `IAppSettingsService`. Do not hardcode secrets. They are stored in the OS user data directory (Windows: DPAPI-protected `%APPDATA%\Merken\settings.bin`). Do not read `app_settings.json`. UI wiring is [UI_TODO.md](UI_TODO.md#settings-api-keys).

```cpp
HttpResponse response = this->httpClient.PostJson(url, jsonBody, {
    {"x-goog-api-key", settings.aiApiKey}
});
if (!response.Ok()) { return ""; }
```

Use cases already pass `AppSettings` (see `CreateEventUseCase`). New API service: interface in `Application/Services/`, impl in `Infrastructure/`, bind as singleton in `DiComposition`. Do not bind `IHttpClient` again — it is already registered.

---

## DI (all roles)

`DiComposition/composition_root.h` is the only place interfaces are wired to implementations. After adding a service, bind it there or the app will not compile when a use case asks for it.

```text
UI  --create-->  UseCase  --calls-->  ISomething
                       DiComposition binds ISomething -> concrete singleton
```
