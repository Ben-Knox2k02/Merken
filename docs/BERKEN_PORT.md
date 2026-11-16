# Three ports onto the current UI

Keep each branch's behavior. Fit it to the current UI. Do not merge the old branches. Where a branch is missing a piece or would drop something that already works, fix that on the port. Do one step, then stop.

The current UI is main's: text buttons, the deck sidebar at 20%, the card canvas, and classic SM-2. The calendar page on `issue-30-port-berken` adds the month at 30% and the day-list at 70%. The day-list is the table of the selected day's events.

`origin/Calendar-API` (`8fd3dcc`) contains `origin/main`. `origin/AIWork` (`24fc8d1`) does not. It is 4 commits behind main. Cut new port branches from the UI they must keep, not from the old branch tips.

## Berken

Berken changes that are already on main, AIWork, or Calendar-API are not part of this port.

The only behavior none of those three have is local `calendar_events` in SQLite. Calendar-API and AIWork have no `event_repository`. Their event list calls Google and does not save a local row. Berken creates the row, reads the list from SQLite, and deletes the row. The Google delete call on Berken is commented out.

That local store is already on `issue-30-port-berken`, on the current calendar page. This branch also stores the Google id after a successful create, and a Google failure still keeps the local row. Berken's table has no `google_event_id` column, and Berken does not write that id back.

The other Berken-only diffs are not functionality to add. They are the older flash-card canvas, the sidebar swap, narrower event-panel columns, and a rename from `GetTitle` to `GetEventTitle`. Leave them.

Nothing Berken-only is left to port.

## Calendar

New branch from `issue-30-port-berken`, so the day-list and the local rows stay.

Bring what Calendar-API has and this branch does not:

- `Delete` on `IHttpClient`, and `GoogleCalendarService::DeleteEvent`. A 404 or 410 counts as success. Tests are in `Tests/Infrastructure/google_calendar_delete_tests.cpp`.
- Description and Reminder on the day-list. Calendar-API's event panel has those columns. This page is one day, so the Date column stays off. Reminder text uses the dialog words (`1 hour`, `1 day`).
- The Google auth, PKCE, client-config, and calendar tests, if step 2 of the old queue is still open. Do not link wxWidgets from `test.bat`.

Fix what Calendar-API does that would drop working behavior:

- Its `GetEventsUseCase` lists only Google events. Keep the day-list on SQLite. A later import reads Google and inserts rows whose Google id is not already stored. It does not replace the list.
- Its `DeleteEventUseCase` deletes only on Google and forgets the local id. Delete the local row first. Google delete is best-effort, and only when the row has a Google id. The confirmation text has to match that.

Do not take the event-panel sidebar, `UI/assets/rip.txt`, menu icons, or Calendar-API's `build.bat`.

## AI

New branch from main. AI study does not need the calendar page.

Main's `GeminiApiService::GenerateResponse` still returns `""`, so AI Study fails before it calls Gemini. AIWork's only commit, `bd98872`, fills that stub with a call to `gemini-3-flash` and scrapes the reply for quotes. Calendar-API and Berken have the same later client: `gemini-3-flash-preview`, JSON parsing, errors for a blocked prompt, and `thinkingLevel` set to `low`. Use that client. AIWork's scraper is the lacking one.

The prompt change is separate. Calendar-API and Berken ask for `<n>. <type> | <question> | <answer>` and grade against the model's answer. AIWork does not. It leaves the card back as the answer. Decide that on the AI port before editing `DomainModels/ai_prompt.h` and `DomainModels/ai_question.h`. The HTTP client can land without the prompt change.

Do not bring Calendar-API's calendar files onto the AI branch.

## Order

1. Berken port stays `issue-30-port-berken` ([issue #30](https://github.com/Ben-Knox2k02/Merken/issues/30)). No further Berken-only work.
2. Calendar port is `issue-32-port-calendar-api`, cut from `55e6aab` ([issue #32](https://github.com/Ben-Knox2k02/Merken/issues/32)). One piece at a time: day-list columns, then Google delete that still removes the local row, then import into SQLite.
3. AI port is `issue-31-port-ai`, cut from `origin/main` at `fb776af` ([issue #31](https://github.com/Ben-Knox2k02/Merken/issues/31)). The working Gemini client comes first. The prompt change is its own step after the decision.

Do not move `main`, `origin/Berken`, `origin/AIWork`, or `origin/Calendar-API`.
