# ShiYang Plan Subtask Sections

## Scope

Edit the existing `ShiYang_Plan.xlsx` workbook without changing its six worksheets, current task tables, formulas, values, or established visual style.

## Design

Add one blank Subtask section below the existing totals row on every `Milestone 1` through `Milestone 6` worksheet. Leave one spacer row between the current task table and the new section.

Each section will contain:

- A section heading labeled `Subtasks` across the table width, styled consistently with the worksheet's milestone heading.
- A header row with `Parent Task`, `Subtask`, `Estimated Hours`, `Actual Hours`, `Status`, `Change`, and `Description`.
- Five blank entry rows, formatted as an Excel table and ready for manual data entry.
- A totals row that sums `Estimated Hours` and `Actual Hours` for the blank entry rows.

The new table will reuse the source workbook's fonts, colors, borders, widths, status conventions, and numeric formats. No subtasks will be generated automatically.

## Output and Verification

Save a modified copy in the workspace output folder, leaving the desktop source file unchanged. Verify all six worksheets visually, confirm the original task content is preserved, and scan formulas for errors before delivery.
