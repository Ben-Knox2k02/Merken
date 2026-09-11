# Working on Merken

Layers do not call across each other except through the interfaces and use cases below.

| You | Own | Do not touch |
|---|---|---|
| UI | `UI/` | `Infrastructure/`, `Application/ServiceInterfaces/` |
| Application | `Application/`, `DomainModels/`, `Infrastructure/Persistence/` (db services) | `UI/`, `db_scripts.cpp`, Gemini, Calendar, HTTP |
| Database | `Infrastructure/Persistence/DatabaseContext/db_scripts.cpp` | `UI/`, db services, Gemini, Calendar |
| APIs | `Infrastructure/GeminiApi/`, `Infrastructure/GoogleCalendar/` | `UI/`, SQL |

New `.cpp` files under `Application/`, `Infrastructure/`, and `UI/src/` are picked up by `build.bat` and CI. New services must also be bound in `DiComposition/composition_root.h`.

---

## Due tasks

Open work for each role is a checklist next to this file. Check items off when done.

| Role | Where to look |
|---|---|
| UI | [UI_TODO.md](UI_TODO.md) |
| Database | [DB_TODO.md](DB_TODO.md) |
| Application | no open task list right now — study/review use cases are in |
| APIs | still the Gemini / Calendar stubs below |

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

- Resolve with `create<TheUseCase>()`, never `create<IDeckDBService>()`.
- Fill a `*Request`, call `Execute`, read the `*Response`.
- If a screen action has no use case yet, ask Application to add one.

Existing use cases: `CreateDeckUseCase`, `UpdateDeckUseCase`, `GetDecksUseCase`, `StudyDeckUseCase`, `CreateCardUseCase`, `UpdateCardUseCase`, `GetCardsUseCase`, `ReviewCardUseCase`, `CreateEventUseCase`.

---

## Application (business logic)

A user action = one use case. Use cases depend on **interfaces**, never on Infrastructure classes.

**Add a use case**

1. Folder: `Application/UseCases/<Area>/<Name>/`
2. Add `*_request.h`, `*_response.h`, `*_usecase.h`, `*_usecase.cpp`
3. Constructor takes interface references (`IDeckDBService&`, `IAIAPIService&`, …)
4. `Execute` uses domain types (`Deck`, `CalendarEvent`, …) and returns a response

```cpp
CreateDeckUseCase(IDeckDBService& deckDBService);

CreateDeckResponse Execute(const CreateDeckRequest& request);
```

- Do **not** bind use cases in DI. UI does `injector.create<CreateDeckUseCase>()`.
- Need a new capability (e.g. list decks)? Add a method to the interface in `Application/ServiceInterfaces/`, then implement it in the db service (or ask APIs if it is Gemini/Calendar).
- IDs are `int` (`cardId`, `deckId`, `calendarEventId`). Pass `0` for a new row; SQLite assigns the auto-increment value.

**Add a db service**

Db services live in `Infrastructure/Persistence/<Entity>/`. They query and write rows. They do not create tables.

1. Domain type in `DomainModels/`.
2. Interface in `Application/ServiceInterfaces/` (e.g. `IDeckDBService`).
3. Implementation in `Infrastructure/Persistence/<Entity>/`. Inject `DatabaseContext&`.
4. Ask Database to append a SQL string in `db_scripts.cpp` for any new table or column.
5. Register in `DiComposition/composition_root.h`:

```cpp
#include "../Infrastructure/Persistence/Deck/deck_db_service.h"

di::bind<IDeckDBService>().to<DeckDbService>().in(di::singleton),
```

---

## Database

One shared SQLite file: `merken.db`. `DatabaseContext` opens it when constructed, creates the file if missing, and applies scripts. Do not open your own connection.

**Your job is only to add scripts.** Append a SQL string in `Infrastructure/Persistence/DatabaseContext/db_scripts.cpp`. Do not edit or reorder scripts that already shipped. Do not touch db services, use cases, or how migrations run.

```cpp
R"(
ALTER TABLE decks ADD COLUMN color TEXT NOT NULL DEFAULT '';
)",
```

`DatabaseContext` applies new scripts on init and skips ones that already ran. Application owns the db services that read and write those tables.

---

## Third-party APIs (Gemini + Google Calendar)

Fill in the stubs. Keep method signatures unless Application agrees to change them.

| Interface | Implementation |
|---|---|
| `IAIAPIService` | `Infrastructure/GeminiApi/gemini_api_service.*` |
| `ICalendarAPIService` | `Infrastructure/GoogleCalendar/google_calendar_service.*` |

**HTTP:** do not use curl/wxWebRequest yourself. Injected `IHttpClient& httpClient` is already there (`Get` / `PostJson`). Keys come from `AppSettings` (`aiApiKey`, `calendarApiKey`). Do not hardcode secrets.

```cpp
HttpResponse response = this->httpClient.PostJson(url, jsonBody, {
    {"x-goog-api-key", settings.aiApiKey}
});
if (!response.Ok()) { return ""; }
```

Use cases already pass `AppSettings` (see `CreateEventUseCase`). New API service: interface in `Application/ServiceInterfaces/`, impl in `Infrastructure/`, bind as singleton in `DiComposition`. Do not bind `IHttpClient` again — it is already registered.

---

## DI (all roles)

`DiComposition/composition_root.h` is the only place interfaces are wired to implementations. After adding a service, bind it there or the app will not compile when a use case asks for it.

```text
UI  --create-->  UseCase  --calls-->  ISomething
                       DiComposition binds ISomething -> concrete singleton
```
