# API due tasks

Fill in the Gemini and Google Calendar **stubs**. HTTP is already injected. Pattern is in [README.md](README.md#third-party-apis-gemini--google-calendar).

Do **not** use curl or `wxWebRequest` yourself. Call `this->httpClient` (`Get` / `PostJson`). Do **not** bind `IHttpClient` again — it is already a singleton. Do not hardcode keys; they come from `AppSettings` (`aiApiKey`, `calendarApiKey`). Keep method signatures unless Application agrees to change them.

Constructors already take `IHttpClient&`. Implementations live here:

| Interface | File | Status |
|---|---|---|
| `IAIAPIService` | `Infrastructure/GeminiApi/gemini_api_service.cpp` | stub: always `""` |
| `ICalendarAPIService` | `Infrastructure/GoogleCalendar/google_calendar_service.cpp` | stub: echoes the event / returns `true` |

```cpp
HttpResponse response = this->httpClient.PostJson(url, jsonBody, {
    {"x-goog-api-key", settings.aiApiKey}
});
if (!response.Ok()) { return ""; }
```

There is no JSON library. Build request bodies as strings. Pull fields out of `response.body` the same way `AppSettingsService` reads `app_settings.json` (search for `"key"`). Escape quotes in user text.

---

## Gemini (`GenerateResponse`)

- [ ] `POST` Gemini `generateContent` with `settings.aiApiKey` (header `x-goog-api-key`). Do not put the key in source.
- [ ] Request body: the `prompt` as the user text. Response: the model’s text (`candidates[0].content.parts[0].text`).
- [ ] On missing key, failed HTTP, or missing text in the body, return `""` (same as today’s stub). Do not throw.

`IAIAPIService` is not called by a use case yet. Still implement it so Application can wire a use case later.

## Google Calendar (`CreateEvent` / `UpdateEvent`)

`CreateEventUseCase` already loads `AppSettings` and calls `CreateEvent`. Put the Google event id on the domain object (`SetGoogleEventId`) and return that `CalendarEvent`. Use cases map `googleEventId` onto the response.

- [ ] `CreateEvent`: `PostJson` to Calendar `calendars/primary/events` with `settings.calendarApiKey`. Map `title` → `summary`, `description`, `startTime` / `endTime` as `dateTime`, `reminderMinutes` as a popup reminder.
- [ ] On success, `SetGoogleEventId` from the response `"id"` and return that event. On failure, return the event **without** a Google id (do not invent one).
- [ ] `UpdateEvent`: send the same fields for an existing `GetGoogleEventId()`. Return `true` only when HTTP succeeds. Empty Google id or failed HTTP → `false`.

`IHttpClient` has `Get` and `PostJson` only. Calendar **update** is PATCH/PUT. Do **not** add methods on `IHttpClient` yourself — ask Application for `PatchJson` (or similar), then implement `UpdateEvent`. Until that exists, `UpdateEvent` can keep returning `false` on a real call you cannot make.

## Don’t

- Do not `create<IHttpClient>()` from UI or use cases. Services receive it in the constructor.
- Do not open your own HTTP session, sockets, or curl.
- Do not persist to SQLite or call `I*Repository`. Calendar/Gemini only talk to the network.
- Do not change `IAIAPIService` / `ICalendarAPIService` signatures, `AppSettings` field names, or `DiComposition` bindings for HTTP/Gemini/Calendar.
- Do not commit real API keys. `app_settings.json` stays dummy values in git.
