# Working on Merken

wxWidgets desktop app. Layers do not call across each other except through the interfaces and use cases below.

| You | Own | Do not touch |
|---|---|---|
| UI | `UI/` | `Infrastructure/`, `Application/ServiceInterfaces/` |
| Application | `Application/`, `DomainModels/` | `UI/`, SQL, HTTP |
| Database | `Infrastructure/Persistence/` | `UI/`, Gemini, Calendar |
| APIs | `Infrastructure/GeminiApi/`, `Infrastructure/GoogleCalendar/` | `UI/`, SQL |

New `.cpp` files under `Application/`, `Infrastructure/`, and `UI/src/` are picked up by `build.bat` and CI. New services must also be bound in `DiComposition/composition_root.h`.

---

## UI

Call **use cases** from event handlers. Do not talk to SQLite, Gemini, or Google Calendar.

```cpp
#include "app.h"
#include "../../Application/UseCases/Course/CreateCourse/create_course_usecase.h"
wxDECLARE_APP(App);

void OnAddCourse(wxCommandEvent&) {
    CreateCourseRequest request;
    request.name = nameCtrl->GetValue().ToStdString();

    auto useCase = wxGetApp().GetInjector().create<CreateCourseUseCase>();
    CreateCourseResponse response = useCase.Execute(request);

    // update the screen from response
}
```

- Resolve with `create<TheUseCase>()`, never `create<ICourseDBService>()`.
- Fill a `*Request`, call `Execute`, read the `*Response`.
- If a screen action has no use case yet, ask Application to add one.

Existing use cases: `CreateCourseUseCase`, `UpdateCourseUseCase`, `CreateEventUseCase`.

---

## Application (business logic)

A user action = one use case. Use cases depend on **interfaces**, never on Infrastructure classes.

**Add a use case**

1. Folder: `Application/UseCases/<Area>/<Name>/`
2. Add `*_request.h`, `*_response.h`, `*_usecase.h`, `*_usecase.cpp`
3. Constructor takes interface references (`ICourseDBService&`, `IAIAPIService&`, …)
4. `Execute` uses domain types (`Course`, `CalendarEvent`, …) and returns a response

```cpp
CreateCourseUseCase(ICourseDBService& courseDBService);

CreateCourseResponse Execute(const CreateCourseRequest& request);
```

- Do **not** bind use cases in DI. UI does `injector.create<CreateCourseUseCase>()`.
- Need a new capability (e.g. list decks)? Add a method to the interface in `Application/ServiceInterfaces/`. Database or API then implements it.
- IDs are `int` (`cardId`, `deckId`, `calendarEventId`, `courseId`). Pass `0` for a new row; SQLite assigns the auto-increment value.

---

## Database

One shared SQLite file: `merken.db`. `DatabaseContext` opens it on first use and creates the file if it is missing. Do not open your own connection.

**New table / entity**

1. Domain type in `DomainModels/` (coordinate with Application).
2. Interface in `Application/ServiceInterfaces/` (e.g. `IDeckDBService`).
3. Implementation in `Infrastructure/Persistence/<Entity>/`.
4. Inject `DatabaseContext&`. Create the table in the ctor with `IF NOT EXISTS`:

```cpp
DeckDbService(DatabaseContext& dbContext) : db(dbContext) {
    this->EnsureSchema();
}

void DeckDbService::EnsureSchema() {
    this->db.GetConnection()->ExecuteUpdate(
        "CREATE TABLE IF NOT EXISTS decks ("
        "deck_id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "name TEXT NOT NULL"
        ");"
    );
}
```

5. Register in `DiComposition/composition_root.h`:

```cpp
#include "../Infrastructure/Persistence/Deck/deck_db_service.h"

di::bind<IDeckDBService>().to<DeckDbService>().in(di::singleton),
```

`GetConnection()` creates `merken.db` if needed. Schema is created when that db service is first constructed (first use case that needs it).

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
