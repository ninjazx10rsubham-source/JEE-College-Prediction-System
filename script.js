/**
 * ================================================================
 *  JEE MAINS 2026 - COLLEGE PREDICTOR & MARKS-RANK ANALYSER
 *  Language : JavaScript (Node.js)
 *
 *  CSV files required (place in same folder as this script):
 *    jee_marks_percentile_rank_2009_2026.csv
 *    jee_college_cutoffs_nits.csv
 *    jee_category_cutoffs_2009_2026.csv
 *    jee_2026_shift_analysis.csv
 *    jee_2026_subject_analysis.csv
 *    jee_2026_toppers.csv
 *    jee_2026_category_toppers.csv
 *    jee_2026_session1_registration_appearance.csv
 *
 *  Run:  node jee_predictor.js
 * ================================================================
 */

const fs   = require('fs');
const path = require('path');
const readline = require('readline');

// ─── Data Arrays ───────────────────────────────────────────────
let marksRankData    = [];   // { year, marks, percentile, rank, totalCandidates }
let collegeCutoffs   = [];   // { year, institute, branch, category, openingRank, closingRank, quota }
let categoryCutoffs  = [];   // { year, category, cutoffPercentile, cutoffMarks }
let shiftData        = [];   // { examDate, shift, marks999, marks99, marks95, difficulty }
let subjectData      = [];   // { subject, percentile, approxMarks, year }
let toppers          = [];   // { name, state, percentile, category }
let categoryToppers  = [];   // { category, name, state, percentile }
let registrationData = [];   // { year, session, dataType, category, female, male, total }

// ================================================================
//  UTILITY
// ================================================================

function separator(char, len) {
    console.log(char.repeat(len));
}

/** Parse a CSV line into trimmed fields */
function parseCSVLine(line) {
    return line.split(',').map(f => f.trim().replace(/\r/g, ''));
}

/** Read a CSV file synchronously, skip header, return array of field arrays */
function readCSV(filePath) {
    if (!fs.existsSync(filePath)) {
        console.log(`  [WARN] Cannot open: ${filePath}`);
        return [];
    }
    const lines = fs.readFileSync(filePath, 'utf8').split('\n');
    const rows = [];
    for (let i = 1; i < lines.length; i++) {   // skip header
        const line = lines[i].trim();
        if (!line) continue;
        rows.push(parseCSVLine(line));
    }
    return rows;
}

// ================================================================
//  CSV LOADERS
// ================================================================

/** 1. jee_marks_percentile_rank_2009_2026.csv
 *  Columns: Year, Marks, Percentile, Rank, Total_Candidates
 */
function loadMarksRank(filePath) {
    const rows = readCSV(filePath);
    let count = 0;
    for (const [yr, mk, pc, rk, tc] of rows) {
        if (!yr) continue;
        marksRankData.push({
            year            : parseInt(yr),
            marks           : parseInt(mk),
            percentile      : parseFloat(pc),
            rank            : parseInt(rk),
            totalCandidates : parseInt(tc) || 0
        });
        count++;
    }
    console.log(`  [OK] Marks-Rank data       : ${count} rows`);
}

/** 2. jee_college_cutoffs_nits.csv
 *  Columns: Year, Institute, Branch, Category, Opening_Rank, Closing_Rank, Quota
 */
function loadCollegeCutoffs(filePath) {
    const rows = readCSV(filePath);
    let count = 0;
    for (const [yr, ins, br, cat, op, cl, qo] of rows) {
        if (!yr) continue;
        collegeCutoffs.push({
            year        : parseInt(yr),
            institute   : ins || '',
            branch      : br  || '',
            category    : cat || '',
            openingRank : parseInt(op) || 0,
            closingRank : parseInt(cl) || 0,
            quota       : qo  || 'Other State'
        });
        count++;
    }
    console.log(`  [OK] College cutoffs       : ${count} rows`);
}

/** 3. jee_category_cutoffs_2009_2026.csv
 *  Columns: Year, Category, Cutoff_Percentile, Cutoff_Marks
 */
function loadCategoryCutoffs(filePath) {
    const rows = readCSV(filePath);
    let count = 0;
    for (const [yr, cat, pc, mk] of rows) {
        if (!yr) continue;
        categoryCutoffs.push({
            year             : parseInt(yr),
            category         : cat || '',
            cutoffPercentile : parseFloat(pc) || 0,
            cutoffMarks      : parseFloat(mk) || 0
        });
        count++;
    }
    console.log(`  [OK] Category cutoffs      : ${count} rows`);
}

/** 4. jee_2026_shift_analysis.csv
 *  Columns: Exam_Date, Shift, Marks_for_99.9_Percentile,
 *           Marks_for_99_Percentile, Marks_for_95_Percentile, Difficulty_Level
 */
function loadShiftAnalysis(filePath) {
    const rows = readCSV(filePath);
    let count = 0;
    for (const [dt, sh, m9, m99, m95, dif] of rows) {
        if (!dt) continue;
        shiftData.push({
            examDate   : dt,
            shift      : sh  || '',
            marks999   : parseInt(m9)  || 0,
            marks99    : parseInt(m99) || 0,
            marks95    : parseInt(m95) || 0,
            difficulty : dif || 'N/A'
        });
        count++;
    }
    console.log(`  [OK] Shift analysis        : ${count} rows`);
}

/** 5. jee_2026_subject_analysis.csv
 *  Columns: Subject, Percentile, Approx_Marks, Year
 */
function loadSubjectAnalysis(filePath) {
    const rows = readCSV(filePath);
    let count = 0;
    for (const [su, pc, mk, yr] of rows) {
        if (!su) continue;
        subjectData.push({
            subject     : su,
            percentile  : parseFloat(pc) || 0,
            approxMarks : parseInt(mk)   || 0,
            year        : parseInt(yr)   || 0
        });
        count++;
    }
    console.log(`  [OK] Subject analysis      : ${count} rows`);
}

/** 6. jee_2026_toppers.csv
 *  Columns: Name, State, Percentile, Category
 */
function loadToppers(filePath) {
    const rows = readCSV(filePath);
    let count = 0;
    for (const [nm, st, pc, ct] of rows) {
        if (!nm) continue;
        toppers.push({
            name       : nm,
            state      : st  || '',
            percentile : parseFloat(pc) || 0,
            category   : ct  || ''
        });
        count++;
    }
    console.log(`  [OK] Toppers               : ${count} rows`);
}

/** 7. jee_2026_category_toppers.csv
 *  Columns: Category, Name, State, Percentile
 */
function loadCategoryToppers(filePath) {
    const rows = readCSV(filePath);
    let count = 0;
    for (const [ct, nm, st, pc] of rows) {
        if (!ct) continue;
        categoryToppers.push({
            category   : ct,
            name       : nm  || '',
            state      : st  || '',
            percentile : parseFloat(pc) || 0
        });
        count++;
    }
    console.log(`  [OK] Category toppers      : ${count} rows`);
}

/** 8. jee_2026_session1_registration_appearance.csv
 *  Columns: Year, Session, Data_Type, Category, Female, Male, Total
 */
function loadRegistration(filePath) {
    const rows = readCSV(filePath);
    let count = 0;
    for (const [yr, se, dt, ct, fe, ma, to] of rows) {
        if (!yr) continue;
        registrationData.push({
            year     : parseInt(yr),
            session  : parseInt(se) || 0,
            dataType : dt || '',
            category : ct || '',
            female   : parseInt(fe) || 0,
            male     : parseInt(ma) || 0,
            total    : parseInt(to) || 0
        });
        count++;
    }
    console.log(`  [OK] Registration data     : ${count} rows`);
}

// ================================================================
//  INTERPOLATION HELPERS
// ================================================================

/** Linear interpolation of percentile from marks using 2026 data */
function estimatePercentile(marks) {
    const data2026 = marksRankData.filter(r => r.year === 2026);
    if (data2026.length === 0) return 0;

    let bestMark = -1, secondMark = -1;
    let bestPct  = 0,  secondPct  = 0;
    let bestDist = 99999, secondDist = 99999;

    for (const r of data2026) {
        const d = Math.abs(r.marks - marks);
        if (d < bestDist) {
            secondMark = bestMark; secondPct = bestPct; secondDist = bestDist;
            bestMark   = r.marks;  bestPct   = r.percentile; bestDist = d;
        } else if (d < secondDist && r.marks !== bestMark) {
            secondMark = r.marks; secondPct = r.percentile; secondDist = d;
        }
    }

    if (bestMark === marks || secondMark === -1) return bestPct;
    if (bestMark === secondMark) return bestPct;

    const ratio = (marks - secondMark) / (bestMark - secondMark);
    let pct = secondPct + ratio * (bestPct - secondPct);
    return Math.max(0, Math.min(100, pct));
}

/** Linear interpolation of rank from marks using 2026 data */
function estimateRank(marks) {
    const data2026 = marksRankData.filter(r => r.year === 2026);
    if (data2026.length === 0) return 1;

    let bestMark = -1, secondMark = -1;
    let bestRank = 0,  secondRank = 0;
    let bestDist = 99999, secondDist = 99999;

    for (const r of data2026) {
        const d = Math.abs(r.marks - marks);
        if (d < bestDist) {
            secondMark = bestMark; secondRank = bestRank; secondDist = bestDist;
            bestMark   = r.marks;  bestRank   = r.rank;  bestDist   = d;
        } else if (d < secondDist && r.marks !== bestMark) {
            secondMark = r.marks; secondRank = r.rank; secondDist = d;
        }
    }

    if (bestMark === marks || secondMark === -1) return bestRank > 0 ? bestRank : 1;
    if (bestMark === secondMark) return bestRank > 0 ? bestRank : 1;

    const ratio = (marks - secondMark) / (bestMark - secondMark);
    const rk = Math.round(secondRank + ratio * (bestRank - secondRank));
    return rk > 0 ? rk : 1;
}

// ================================================================
//  MENU 1 – Marks → Rank & Percentile
// ================================================================

function showMarksRankAnalysis(marks) {
    separator('=', 72);
    console.log('  MARKS vs RANK vs PERCENTILE ANALYSIS  (JEE Mains 2026)');
    separator('=', 72);

    if (marks < 0 || marks > 300) {
        console.log('  [ERROR] Marks must be 0-300.'); return;
    }

    const pct  = estimatePercentile(marks);
    const rank = estimateRank(marks);

    separator('-', 72);
    console.log(`  Marks Entered          : ${marks} / 300`);
    console.log(`  Estimated Percentile   : ${pct.toFixed(4)}`);
    console.log(`  Estimated CRL Rank     : ${rank}`);
    console.log(`  Total Candidates 2026  : ~14,00,000`);

    let band;
    if      (pct >= 99.9) band = 'EXCELLENT  – Top 0.1% | JEE Advanced qualifier';
    else if (pct >= 99.0) band = 'VERY GOOD  – Top 1%   | Top NITs reachable';
    else if (pct >= 95.0) band = 'GOOD       – Top 5%   | Good NITs possible';
    else if (pct >= 90.0) band = 'ABOVE AVG  – Top 10%  | Lower NITs / IIITs';
    else if (pct >= 75.0) band = 'AVERAGE    – Top 25%  | State colleges';
    else                  band = 'BELOW AVG             – Consider state quota';

    console.log(`  Performance Band       : ${band}`);

    // Subject-wise benchmarks
    console.log();
    separator('-', 72);
    console.log('  Subject-wise Benchmark at Your Percentile (2026 data)');
    separator('-', 72);
    console.log(`  ${'Subject'.padEnd(14)}  ${'Nearest %ile'.padEnd(12)}  Approx Marks Needed`);
    separator('-', 72);

    for (const subject of ['Physics', 'Chemistry', 'Mathematics']) {
        let bestDiff = 9999, bestMk = 0, bestPctS = 0;
        for (const s of subjectData) {
            if (s.subject !== subject) continue;
            const dp = Math.abs(s.percentile - pct);
            if (dp < bestDiff) {
                bestDiff = dp;
                bestMk   = s.approxMarks;
                bestPctS = s.percentile;
            }
        }
        console.log(`  ${subject.padEnd(14)}  ${bestPctS.toFixed(1).padEnd(12)}  ~${bestMk} marks`);
    }

    // Historical trend
    console.log();
    separator('-', 72);
    console.log('  Historical Context – Nearest marks in past years');
    separator('-', 72);
    console.log(`  ${'Year'.padEnd(6)}  ${'Marks'.padEnd(8)}  ${'Percentile'.padEnd(12)}  ${'Rank'.padEnd(10)}  Candidates`);
    separator('-', 72);

    for (const year of [2022, 2023, 2024, 2025, 2026]) {
        let closest = null, minDist = 99999;
        for (const r of marksRankData) {
            if (r.year !== year) continue;
            const d = Math.abs(r.marks - marks);
            if (d < minDist) { minDist = d; closest = r; }
        }
        if (closest) {
            console.log(
                `  ${String(year).padEnd(6)}  ${String(closest.marks).padEnd(8)}  ` +
                `${closest.percentile.toFixed(4).padEnd(12)}  ${String(closest.rank).padEnd(10)}  ` +
                `${closest.totalCandidates}`
            );
        }
    }
    separator('=', 72);
}

// ================================================================
//  MENU 2 – College Predictor
// ================================================================

function showCollegePredictor(marks, categoryChoice) {
    separator('=', 72);
    console.log('  NIT COLLEGE PREDICTOR  (JEE Mains 2026 Cutoffs)');
    separator('=', 72);

    if (marks < 0 || marks > 300) { console.log('  [ERROR] Invalid marks.'); return; }
    if (categoryChoice < 1 || categoryChoice > 6) { console.log('  [ERROR] Invalid choice.'); return; }

    const categories = ['', 'General', 'OBC-NCL', 'SC', 'ST', 'EWS', 'PwD'];
    const cat = categories[categoryChoice];

    const estRank = estimateRank(marks);
    const estPct  = estimatePercentile(marks);

    console.log();
    separator('-', 72);
    console.log(`  Marks: ${marks} | Estimated Rank: ${estRank} | Percentile: ${estPct.toFixed(4)} | Category: ${cat}`);
    separator('-', 72);

    let safe = 0, moderate = 0, reach = 0, printed = 0;

    console.log();
    console.log(
        `  ${'Institute'.padEnd(22)}  ${'Branch'.padEnd(34)}  ${'Closing Rk'.padEnd(10)}  Chance`
    );
    separator('-', 72);

    for (const c of collegeCutoffs) {
        if (c.year !== 2026) continue;
        if (c.category !== cat) continue;

        let chance;
        if (estRank <= c.openingRank) {
            chance = 'SAFE';     safe++;
        } else if (estRank <= c.closingRank) {
            chance = 'MODERATE'; moderate++;
        } else if (estRank <= Math.floor(c.closingRank * 1.12)) {
            chance = 'REACH';    reach++;
        } else {
            continue;
        }

        console.log(
            `  ${c.institute.padEnd(22)}  ${c.branch.padEnd(34)}  ` +
            `${String(c.closingRank).padEnd(10)}  ${chance}`
        );
        printed++;
    }

    if (printed === 0) {
        console.log(`  No NIT options found for rank ${estRank} in category ${cat}.`);
        console.log('  Suggestion: Prepare for JEE Mains Session 2,');
        console.log('  consider state-quota colleges or private institutes.');
    } else {
        console.log();
        separator('-', 72);
        console.log(`  Total: ${printed} options  |  SAFE=${safe}  MODERATE=${moderate}  REACH=${reach}`);
        console.log('\n  Legend:');
        console.log('    SAFE     = Your rank < Opening rank (high confidence)');
        console.log('    MODERATE = Opening rank <= Your rank <= Closing rank');
        console.log('    REACH    = Your rank up to 12% above closing rank');
    }
    separator('=', 72);
}

// ================================================================
//  MENU 3 – Category Cutoff Trend 2019-2026
// ================================================================

function showCategoryCutoffs() {
    separator('=', 72);
    console.log('  CATEGORY CUTOFF TREND  (2019 – 2026)');
    separator('=', 72);
    console.log(
        `  ${'Year'.padEnd(6)}  ${'Category'.padEnd(12)}  ${'Cutoff %ile'.padEnd(14)}  Cutoff Marks`
    );
    separator('-', 72);

    const years = [2019, 2020, 2021, 2022, 2023, 2024, 2025, 2026];
    const cats  = ['General', 'OBC-NCL', 'SC', 'ST', 'EWS', 'PwD'];

    for (const year of years) {
        let found = false;
        for (const cat of cats) {
            for (const c of categoryCutoffs) {
                if (c.year !== year || c.category !== cat) continue;
                if (c.cutoffMarks > 0.01) {
                    console.log(
                        `  ${String(year).padEnd(6)}  ${cat.padEnd(12)}  ` +
                        `${c.cutoffPercentile.toFixed(2).padEnd(14)}  ${c.cutoffMarks.toFixed(1)}`
                    );
                } else {
                    console.log(
                        `  ${String(year).padEnd(6)}  ${cat.padEnd(12)}  ` +
                        `${c.cutoffPercentile.toFixed(2).padEnd(14)}  N/A (percentile-only year)`
                    );
                }
                found = true;
            }
        }
        if (found) console.log();
    }
    separator('=', 72);
}

// ================================================================
//  MENU 4 – Shift Analysis
// ================================================================

function showShiftAnalysis() {
    separator('=', 72);
    console.log('  JEE MAINS 2026 – SESSION 1 SHIFT-WISE PAPER ANALYSIS');
    separator('=', 72);
    console.log(
        `  ${'Date'.padEnd(16)}  ${'Shift'.padEnd(9)}  ${'Marks@99.9'.padEnd(11)}  ` +
        `${'Marks@99'.padEnd(10)}  ${'Marks@95'.padEnd(10)}  Difficulty`
    );
    separator('-', 72);

    let avg999 = 0, avg99 = 0, avg95 = 0;
    for (const s of shiftData) {
        console.log(
            `  ${s.examDate.padEnd(16)}  ${s.shift.padEnd(9)}  ` +
            `${String(s.marks999).padEnd(11)}  ${String(s.marks99).padEnd(10)}  ` +
            `${String(s.marks95).padEnd(10)}  ${s.difficulty}`
        );
        avg999 += s.marks999;
        avg99  += s.marks99;
        avg95  += s.marks95;
    }

    if (shiftData.length > 0) {
        const n = shiftData.length;
        separator('-', 72);
        console.log(
            `  ${'AVERAGE'.padEnd(16)}  ${'All Shifts'.padEnd(9)}  ` +
            `${(avg999/n).toFixed(1).padEnd(11)}  ${(avg99/n).toFixed(1).padEnd(10)}  ` +
            `${(avg95/n).toFixed(1).padEnd(10)}  Average`
        );
    }
    separator('=', 72);
}

// ================================================================
//  MENU 5 – Toppers
// ================================================================

function showToppers() {
    separator('=', 72);
    console.log('  JEE MAINS 2026 – TOPPERS');
    separator('=', 72);

    console.log('\n  100-Percentile Achievers:');
    separator('-', 72);
    console.log(
        `  ${'No.'.padEnd(4)}  ${'Name'.padEnd(28)}  ${'State'.padEnd(18)}  ${'Percentile'.padEnd(12)}  Category`
    );
    separator('-', 72);
    toppers.forEach((t, i) => {
        console.log(
            `  ${String(i+1).padEnd(4)}  ${t.name.padEnd(28)}  ` +
            `${t.state.padEnd(18)}  ${t.percentile.toFixed(7).padEnd(12)}  ${t.category}`
        );
    });

    console.log('\n  Category-Wise Toppers:');
    separator('-', 72);
    console.log(
        `  ${'Category'.padEnd(20)}  ${'Name'.padEnd(28)}  ${'State'.padEnd(18)}  Percentile`
    );
    separator('-', 72);
    for (const t of categoryToppers) {
        console.log(
            `  ${t.category.padEnd(20)}  ${t.name.padEnd(28)}  ` +
            `${t.state.padEnd(18)}  ${t.percentile.toFixed(7)}`
        );
    }
    separator('=', 72);
}

// ================================================================
//  MENU 6 – Registration Statistics
// ================================================================

function showRegistration() {
    separator('=', 72);
    console.log('  JEE MAINS 2026 SESSION 1 – REGISTRATION & APPEARANCE');
    separator('=', 72);

    for (const type of ['Registered', 'Appeared']) {
        console.log(`\n  ${type}:`);
        console.log(
            `  ${'Category'.padEnd(12)}  ${'Female'.padEnd(10)}  ${'Male'.padEnd(10)}  Total`
        );
        separator('-', 50);
        let tf = 0, tm = 0, tt = 0;
        for (const r of registrationData) {
            if (r.dataType !== type) continue;
            console.log(
                `  ${r.category.padEnd(12)}  ${String(r.female).padEnd(10)}  ` +
                `${String(r.male).padEnd(10)}  ${r.total}`
            );
            tf += r.female; tm += r.male; tt += r.total;
        }
        console.log(
            `  ${'TOTAL'.padEnd(12)}  ${String(tf).padEnd(10)}  ${String(tm).padEnd(10)}  ${tt}`
        );
    }

    let regTot = 0, appTot = 0;
    for (const r of registrationData) {
        if (r.dataType === 'Registered') regTot += r.total;
        if (r.dataType === 'Appeared')   appTot += r.total;
    }
    if (regTot > 0) {
        console.log(`\n  Appearance Rate: ${(100 * appTot / regTot).toFixed(2)}%`);
    }
    separator('=', 72);
}

// ================================================================
//  MENU 7 – Export Predictions to CSV
// ================================================================

function exportResults(marks, categoryChoice, outfile) {
    separator('=', 72);
    console.log('  EXPORT COLLEGE PREDICTIONS TO CSV FILE');
    separator('=', 72);

    if (marks < 0 || marks > 300) { console.log('  [ERROR] Invalid marks.'); return; }
    if (categoryChoice < 1 || categoryChoice > 6) { console.log('  [ERROR] Invalid category.'); return; }

    const categories = ['', 'General', 'OBC-NCL', 'SC', 'ST', 'EWS', 'PwD'];
    const cat = categories[categoryChoice];
    const filename = outfile || 'my_predictions.csv';

    const estRank = estimateRank(marks);
    const estPct  = estimatePercentile(marks);

    let lines = [];
    lines.push('JEE Mains 2026 College Predictions');
    lines.push(`Marks,${marks}`);
    lines.push(`Estimated Percentile,${estPct.toFixed(4)}`);
    lines.push(`Estimated Rank,${estRank}`);
    lines.push(`Category,${cat}`);
    lines.push('');
    lines.push('Institute,Branch,Category,Opening Rank,Closing Rank,Chance');

    let count = 0;
    for (const c of collegeCutoffs) {
        if (c.year !== 2026) continue;
        if (c.category !== cat) continue;

        let chance;
        if      (estRank <= c.openingRank)                     chance = 'SAFE';
        else if (estRank <= c.closingRank)                     chance = 'MODERATE';
        else if (estRank <= Math.floor(c.closingRank * 1.12))  chance = 'REACH';
        else continue;

        lines.push(`${c.institute},${c.branch},${c.category},${c.openingRank},${c.closingRank},${chance}`);
        count++;
    }

    lines.push('');
    lines.push(`Total Options,${count}`);

    try {
        fs.writeFileSync(filename, lines.join('\n'), 'utf8');
        console.log(`  [OK] ${count} options written to '${filename}'`);
    } catch (e) {
        console.log(`  [ERROR] Cannot create file: ${e.message}`);
    }
}

// ================================================================
//  MENU 8 – Full Marks-Percentile Table 2026
// ================================================================

function showPercentileTable() {
    separator('=', 72);
    console.log('  JEE MAINS 2026 – MARKS vs PERCENTILE vs RANK TABLE');
    separator('=', 72);
    console.log(`  ${'Marks'.padEnd(8)}  ${'Percentile'.padEnd(12)}  Est. Rank`);
    separator('-', 40);

    const data2026 = marksRankData
        .filter(r => r.year === 2026)
        .sort((a, b) => b.marks - a.marks);   // descending

    const seen = new Set();
    for (const r of data2026) {
        if (seen.has(r.marks)) continue;
        seen.add(r.marks);
        console.log(
            `  ${String(r.marks).padEnd(8)}  ${r.percentile.toFixed(4).padEnd(12)}  ${r.rank}`
        );
    }
    separator('=', 72);
}

// ================================================================
//  INTERACTIVE MENU (readline)
// ================================================================

function printBanner() {
    separator('*', 72);
    console.log('*' + ''.padEnd(70) + '*');
    console.log('*   JEE MAINS 2026 – COLLEGE PREDICTOR & MARKS-RANK ANALYSER       *');
    console.log('*   Built in JavaScript (Node.js) | Reads all 8 JEE 2026 CSV files *');
    console.log('*' + ''.padEnd(70) + '*');
    separator('*', 72);
}

function printMenu() {
    console.log('\n  MAIN MENU');
    separator('-', 55);
    console.log('  1. Marks  →  Rank & Percentile Analysis');
    console.log('  2. College Predictor  (NIT Branch-wise Cutoffs)');
    console.log('  3. Category Cutoff Trend  (2019 – 2026)');
    console.log('  4. Shift-wise Paper Difficulty Analysis');
    console.log('  5. Toppers & Hall of Fame');
    console.log('  6. Registration & Appearance Statistics');
    console.log('  7. Export My Predictions to CSV');
    console.log('  8. Full Marks-Percentile Table  (2026)');
    console.log('  0. Exit');
    separator('-', 55);
}

async function question(rl, prompt) {
    return new Promise(resolve => rl.question(prompt, resolve));
}

async function main() {
    printBanner();
    console.log('\n  Loading data from CSV files...\n');

    loadMarksRank       ('jee_marks_percentile_rank_2009_2026.csv');
    loadCollegeCutoffs  ('jee_college_cutoffs_nits.csv');
    loadCategoryCutoffs ('jee_category_cutoffs_2009_2026.csv');
    loadShiftAnalysis   ('jee_2026_shift_analysis.csv');
    loadSubjectAnalysis ('jee_2026_subject_analysis.csv');
    loadToppers         ('jee_2026_toppers.csv');
    loadCategoryToppers ('jee_2026_category_toppers.csv');
    loadRegistration    ('jee_2026_session1_registration_appearance.csv');

    console.log('\n  All CSV files processed. System ready!');

    const rl = readline.createInterface({ input: process.stdin, output: process.stdout });

    let running = true;
    while (running) {
        printMenu();
        const choice = parseInt(await question(rl, '  Enter choice: '));
        console.log();

        switch (choice) {
            case 1: {
                const marks = parseInt(await question(rl, '  Enter your JEE Mains marks (out of 300): '));
                showMarksRankAnalysis(marks);
                break;
            }
            case 2: {
                const marks = parseInt(await question(rl, '  Enter your JEE Mains marks (out of 300): '));
                console.log('  Select your category:\n    1. General\n    2. OBC-NCL\n    3. SC\n    4. ST\n    5. EWS\n    6. PwD');
                const catCh = parseInt(await question(rl, '  Choice: '));
                showCollegePredictor(marks, catCh);
                break;
            }
            case 3:
                showCategoryCutoffs();
                break;
            case 4:
                showShiftAnalysis();
                break;
            case 5:
                showToppers();
                break;
            case 6:
                showRegistration();
                break;
            case 7: {
                const marks = parseInt(await question(rl, '  Enter marks (0-300): '));
                console.log('  Category (1=Gen 2=OBC 3=SC 4=ST 5=EWS 6=PwD): ');
                const catCh   = parseInt(await question(rl, '  Choice: '));
                const outfile = (await question(rl, '  Output filename [my_predictions.csv]: ')) || 'my_predictions.csv';
                exportResults(marks, catCh, outfile);
                break;
            }
            case 8:
                showPercentileTable();
                break;
            case 0:
                console.log('  Goodbye! Best of luck for JEE!\n');
                running = false;
                break;
            default:
                console.log('  Invalid choice. Please try again.');
        }
    }

    rl.close();
}

main().catch(err => {
    console.error('Fatal error:', err);
    process.exit(1);
});
