# ShiYang Plan Subtask Sections Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Add a blank, consistently styled Subtasks table to every milestone worksheet in `ShiYang_Plan.xlsx`.

**Architecture:** Import the desktop workbook with `@oai/artifact-tool`, extend each existing milestone sheet below its current task totals row, and export one modified copy to the thread output folder. Verification will inspect every new range, scan formulas for errors, render all six sheets, and compare the original task ranges with the source workbook.

**Tech Stack:** Node.js, `@oai/artifact-tool` 2.8.58+, XLSX

## Global Constraints

- Preserve all six worksheets and all existing task content, formulas, formats, tables, and navigation.
- Add one Subtasks section to every worksheet with five blank entry rows.
- Use columns `Parent Task`, `Subtask`, `Estimated Hours`, `Actual Hours`, `Status`, `Change`, and `Description`.
- Reuse each worksheet's existing milestone title, table header, body, totals, number-format, and status-validation conventions.
- Do not generate subtask content.
- Leave the desktop source unchanged and save one modified workbook in the workspace output folder.

---

### Task 1: Add and verify Subtask sections

**Files:**
- Read: `C:/Users/Yang Shi/Desktop/ShiYang_Plan.xlsx`
- Create: `C:/p4/Personal/SD/GAS/.codex-temp/shiyang-plan/edit_workbook.mjs`
- Create: `C:/p4/Personal/SD/GAS/outputs/01a07cb8-3200-7633-a522-f0a7e09155a7/ShiYang_Plan_with_Subtasks.xlsx`
- Create: `C:/p4/Personal/SD/GAS/.codex-temp/shiyang-plan/final-previews/Milestone 1.png` through `Milestone 6.png`

**Interfaces:**
- Consumes: the six source worksheets named `Milestone 1` through `Milestone 6`.
- Produces: one XLSX copy containing a `SubtasksTable1` through `SubtasksTable6` table on the matching worksheet.

- [ ] **Step 1: Mark the workbook edit operation**

Run:

```powershell
node container_tools/mark_artifact_operation_started.mjs --operation-kind edit --expected-output-count 1 --output-format xlsx
```

Expected: the operation marker completes successfully exactly once before workbook authoring.

- [ ] **Step 2: Write the workbook transformation**

Create `edit_workbook.mjs` using the documented `FileBlob`, `SpreadsheetFile`, and workbook APIs. For each worksheet, locate its used-range last row, leave one blank spacer row, copy the existing title-row formatting into a merged `A:G` Subtasks heading, copy the existing header formatting into the seven-column subtask header, copy the existing task-body formatting into five blank rows, and copy the existing totals-row formatting into a new totals row. Set the two totals formulas to sum the five entry rows, add an Excel table over the header and five entry rows with the existing task table's style, then export the workbook.

The fixed labels are:

```js
const subtaskHeaders = [
  "Parent Task",
  "Subtask",
  "Estimated Hours",
  "Actual Hours",
  "Status",
  "Change",
  "Description",
];
```

The row layout for a sheet whose source totals row is `N` is:

```text
N + 1  blank spacer
N + 2  merged Subtasks heading
N + 3  table headers
N + 4  blank entry row 1
N + 5  blank entry row 2
N + 6  blank entry row 3
N + 7  blank entry row 4
N + 8  blank entry row 5
N + 9  totals row
```

- [ ] **Step 3: Recalculate and verify workbook structure**

Call `workbook.recalculate()` once after all edits. Inspect each new range and verify the heading, headers, five blank rows, and totals formulas. Scan the workbook for `#REF!`, `#DIV/0!`, `#VALUE!`, `#NAME?`, `#N/A`, `#NUM!`, `#NULL!`, `#SPILL!`, and `#CALC!`.

Expected for every worksheet:

```text
Subtasks heading: present
Blank entry rows: 5
Estimated Hours total: 0
Actual Hours total: 0
Formula errors introduced: 0
```

- [ ] **Step 4: Verify source preservation and visual layout**

Import the exported workbook and compare each original used range cell-for-cell against the source workbook's values and displayed formulas. Render all six worksheets at normal scale and inspect each image for consistent styling, unclipped headings, readable columns, and visible totals.

Expected:

```text
Original task-range differences: 0
Rendered milestone sheets: 6
Clipped or unreadable new cells: 0
```

- [ ] **Step 5: Commit implementation support files**

Run:

```powershell
git add -- .codex-temp/shiyang-plan/edit_workbook.mjs docs/superpowers/plans/2026-09-07-shiyang-plan-subtask-sections.md
git commit -m "feat: add subtask sections to ShiYang plan"
```

Expected: only the plan and workbook transformation support file are committed; the generated workbook and previews remain delivery artifacts outside the commit.
