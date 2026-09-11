# Database due tasks

Your job is **only** to append SQL in `Infrastructure/Persistence/DatabaseContext/db_scripts.cpp`. Do not edit or reorder scripts that already shipped. Do not touch db services or use cases. How migrations run is already in [README.md](README.md#database).

Application already reads/writes these names. Match them exactly.

---

## Already in the first `cards` script — do not ALTER

`interval_days`, `ease_factor`, `repetition_count`, `next_review_date`, `last_reviewed_date` already exist. Empty `next_review_date` (`''`) means never reviewed.

## New table

- [ ] Append a **new** script (do not edit existing `CREATE TABLE` strings) for table `daily_progress`.

| Column | Type | Notes |
|---|---|---|
| `date` | TEXT, primary key | ISO `YYYY-MM-DD`; one row per calendar day |
| `cards_reviewed` | INTEGER | default 0 |
| `cards_correct` | INTEGER | default 0 |

- [ ] No retention column. Retention is computed in the domain, not SQL.
- [ ] No extra indexes required for now.
