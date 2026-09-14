# Database due tasks

Your job is **only** to append SQL in `Infrastructure/Persistence/DatabaseContext/db_scripts.cpp`. Do not edit or reorder scripts that already shipped. Do not touch repositories or use cases. How migrations run is already in [README.md](README.md#database).

`GetScripts()` is empty. Append one `R"( ... )"` string per table, in this order. Application already reads/writes these names. Match them exactly.

```cpp
R"(
CREATE TABLE IF NOT EXISTS decks (
    ...
);
)",
```

If you already have a `merken.db` from before, delete it (or run uninstall) so the new scripts actually run. `DatabaseContext` skips a script once its version has been recorded.

---

## `decks`

- [ ] Append a script for table `decks`.

| Column | Type | Notes |
|---|---|---|
| `deck_id` | INTEGER, primary key, AUTOINCREMENT | |
| `name` | TEXT, not null | |
| `description` | TEXT, not null | default `''` |
| `created_at` | TEXT, not null | default `''` |

## `cards`

- [ ] Append a **new** script for table `cards`. Do not edit the `decks` string. Foreign key `deck_id` → `decks(deck_id)`.

| Column | Type | Notes |
|---|---|---|
| `card_id` | INTEGER, primary key, AUTOINCREMENT | |
| `deck_id` | INTEGER, not null | FK to `decks` |
| `front` | TEXT, not null | |
| `back` | TEXT, not null | |
| `tags` | TEXT, not null | default `''`; comma-separated |
| `card_type` | INTEGER, not null | default `0` (Recall) |
| `choices` | TEXT, not null | default `''` |
| `interval_days` | INTEGER, not null | default `0` |
| `ease_factor` | REAL, not null | default `2.5` |
| `repetition_count` | INTEGER, not null | default `0` |
| `next_review_date` | TEXT, not null | default `''`; empty means never reviewed |
| `last_reviewed_date` | TEXT, not null | default `''` |

## `daily_progress`

- [ ] Append a **new** script for table `daily_progress`. Do not edit the `decks` or `cards` strings.

| Column | Type | Notes |
|---|---|---|
| `date` | TEXT, primary key | ISO `YYYY-MM-DD`; one row per calendar day |
| `cards_reviewed` | INTEGER, not null | default `0` |
| `cards_correct` | INTEGER, not null | default `0` |

- [ ] No retention column. Retention is computed in the domain, not SQL.
- [ ] No extra indexes required for now.

Progress history / heatmap uses this same `daily_progress` table. No new table or columns.

## Don’t

- Do not edit or reorder a script after it has shipped.
- Do not touch repositories, use cases, or how `DatabaseContext` applies migrations.
- Do not add a calendar table. Events go through Google Calendar, not SQLite.
