/*
 * ================================================================
 *  JEE MAINS 2026 - COLLEGE PREDICTOR & MARKS-RANK ANALYSER
 *  Language : C (C99)
 *
 *  CSV files read at runtime (place in same folder as executable):
 *    jee_marks_percentile_rank_2009_2026.csv
 *    jee_college_cutoffs_nits.csv
 *    jee_category_cutoffs_2009_2026.csv
 *    jee_2026_shift_analysis.csv
 *    jee_2026_subject_analysis.csv
 *    jee_2026_toppers.csv
 *    jee_2026_category_toppers.csv
 *    jee_2026_session1_registration_appearance.csv
 *
 *  Compile:  gcc -o jee_predictor jee_predictor.c -lm
 *  Run:      ./jee_predictor
 * ================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>

/* ─── limits ─────────────────────────────────────────────── */
#define MAX_MR      2000
#define MAX_COLLEGE  600
#define MAX_CAT_CUT  200
#define MAX_SHIFT     20
#define MAX_SUBJ      50
#define MAX_TOPPER    30
#define MAX_REG       30
#define STR_SM        32
#define STR_MD        72
#define STR_LG        96

/* ─── structures ─────────────────────────────────────────── */
typedef struct {
    int   year;
    int   marks;
    float percentile;
    int   rank;
    int   total_candidates;
} MarksRank;

typedef struct {
    int  year;
    char institute[STR_MD];
    char branch[STR_LG];
    char category[STR_SM];
    int  opening_rank;
    int  closing_rank;
    char quota[STR_SM];
} CollegeCutoff;

typedef struct {
    int   year;
    char  category[STR_SM];
    float cutoff_percentile;
    float cutoff_marks;
} CatCutoff;

typedef struct {
    char  exam_date[STR_SM];
    char  shift[STR_SM];
    int   marks_999;
    int   marks_99;
    int   marks_95;
    char  difficulty[STR_MD];
} ShiftInfo;

typedef struct {
    char  subject[STR_SM];
    float percentile;
    int   approx_marks;
    int   year;
} SubjectAnalysis;

typedef struct {
    char  name[STR_MD];
    char  state[STR_MD];
    float percentile;
    char  category[STR_SM];
} Topper;

typedef struct {
    int   year;
    int   session;
    char  data_type[STR_SM];
    char  category[STR_SM];
    int   female;
    int   male;
    int   total;
} RegData;

/* ─── global arrays ──────────────────────────────────────── */
static MarksRank      mr[MAX_MR];
static CollegeCutoff  cc[MAX_COLLEGE];
static CatCutoff      catcut[MAX_CAT_CUT];
static ShiftInfo      shifts[MAX_SHIFT];
static SubjectAnalysis subj[MAX_SUBJ];
static Topper         toppers[MAX_TOPPER];
static Topper         cat_toppers[MAX_TOPPER];
static RegData        reg[MAX_REG];

static int mr_n=0, cc_n=0, catcut_n=0, shift_n=0;
static int subj_n=0, top_n=0, ctop_n=0, reg_n=0;

/* ================================================================
 *  UTILITY
 * ================================================================ */
static void trim(char *s) {
    int len = (int)strlen(s);
    while (len > 0 && (s[len-1]=='\n'||s[len-1]=='\r'||
                       s[len-1]==' ' ||s[len-1]=='\t'))
        s[--len] = '\0';
    char *p = s;
    while (*p==' '||*p=='\t') p++;
    if (p != s) memmove(s, p, strlen(p)+1);
}

/* Tokenise CSV line – handles empty fields */
static char *next_field(char **line, char delim) {
    char *start = *line;
    if (!start) return NULL;
    char *end = strchr(start, delim);
    if (end) { *end = '\0'; *line = end+1; }
    else      { *line = NULL; }
    trim(start);
    return start;
}

static void separator(char c, int len) {
    for (int i=0;i<len;i++) putchar(c);
    putchar('\n');
}

/* ================================================================
 *  CSV LOADERS
 * ================================================================ */

/* 1. jee_marks_percentile_rank_2009_2026.csv
   Columns: Year,Marks,Percentile,Rank,Total_Candidates            */
int load_marks_rank(const char *path) {
    FILE *f = fopen(path,"r");
    if (!f) { printf("  [WARN] Cannot open: %s\n",path); return 0; }
    char line[256];
    fgets(line,sizeof(line),f);          /* header */
    int count=0;
    while (fgets(line,sizeof(line),f) && mr_n<MAX_MR) {
        char *p=line;
        char *yr=next_field(&p,',');
        char *mk=next_field(&p,',');
        char *pc=next_field(&p,',');
        char *rk=next_field(&p,',');
        char *tc=next_field(&p,',');
        if (!yr||!mk||!pc||!rk||!tc||*yr=='\0') continue;
        mr[mr_n].year             = atoi(yr);
        mr[mr_n].marks            = atoi(mk);
        mr[mr_n].percentile       = (float)atof(pc);
        mr[mr_n].rank             = atoi(rk);
        mr[mr_n].total_candidates = atoi(tc);
        mr_n++; count++;
    }
    fclose(f);
    printf("  [OK] Marks-Rank data       : %d rows\n",count);
    return count;
}

/* 2. jee_college_cutoffs_nits.csv
   Columns: Year,Institute,Branch,Category,Opening_Rank,
            Closing_Rank,Quota                                       */
int load_college_cutoffs(const char *path) {
    FILE *f = fopen(path,"r");
    if (!f) { printf("  [WARN] Cannot open: %s\n",path); return 0; }
    char line[512];
    fgets(line,sizeof(line),f);
    int count=0;
    while (fgets(line,sizeof(line),f) && cc_n<MAX_COLLEGE) {
        char *p=line;
        char *yr =next_field(&p,',');
        char *ins=next_field(&p,',');
        char *br =next_field(&p,',');
        char *cat=next_field(&p,',');
        char *op =next_field(&p,',');
        char *cl =next_field(&p,',');
        char *qo =next_field(&p,',');
        if (!yr||!ins||!br||!cat||!op||!cl||*yr=='\0') continue;
        cc[cc_n].year         = atoi(yr);
        strncpy(cc[cc_n].institute, ins, STR_MD-1);
        strncpy(cc[cc_n].branch,    br,  STR_LG-1);
        strncpy(cc[cc_n].category,  cat, STR_SM-1);
        cc[cc_n].opening_rank = atoi(op);
        cc[cc_n].closing_rank = atoi(cl);
        strncpy(cc[cc_n].quota, qo ? qo : "Other State", STR_SM-1);
        cc_n++; count++;
    }
    fclose(f);
    printf("  [OK] College cutoffs       : %d rows\n",count);
    return count;
}

/* 3. jee_category_cutoffs_2009_2026.csv
   Columns: Year,Category,Cutoff_Percentile,Cutoff_Marks            */
int load_category_cutoffs(const char *path) {
    FILE *f = fopen(path,"r");
    if (!f) { printf("  [WARN] Cannot open: %s\n",path); return 0; }
    char line[256];
    fgets(line,sizeof(line),f);
    int count=0;
    while (fgets(line,sizeof(line),f) && catcut_n<MAX_CAT_CUT) {
        char *p=line;
        char *yr =next_field(&p,',');
        char *cat=next_field(&p,',');
        char *pc =next_field(&p,',');
        char *mk =next_field(&p,',');
        if (!yr||!cat||!pc||*yr=='\0') continue;
        catcut[catcut_n].year              = atoi(yr);
        strncpy(catcut[catcut_n].category,  cat, STR_SM-1);
        catcut[catcut_n].cutoff_percentile = (float)atof(pc);
        catcut[catcut_n].cutoff_marks      = mk ? (float)atof(mk) : 0.0f;
        catcut_n++; count++;
    }
    fclose(f);
    printf("  [OK] Category cutoffs      : %d rows\n",count);
    return count;
}

/* 4. jee_2026_shift_analysis.csv
   Columns: Exam_Date,Shift,Marks_for_99.9_Percentile,
            Marks_for_99_Percentile,Marks_for_95_Percentile,
            Difficulty_Level                                         */
int load_shift_analysis(const char *path) {
    FILE *f = fopen(path,"r");
    if (!f) { printf("  [WARN] Cannot open: %s\n",path); return 0; }
    char line[256];
    fgets(line,sizeof(line),f);
    int count=0;
    while (fgets(line,sizeof(line),f) && shift_n<MAX_SHIFT) {
        char *p=line;
        char *dt =next_field(&p,',');
        char *sh =next_field(&p,',');
        char *m9 =next_field(&p,',');
        char *m99=next_field(&p,',');
        char *m95=next_field(&p,',');
        char *dif=next_field(&p,',');
        if (!dt||!sh||!m9||!m99||!m95||*dt=='\0') continue;
        strncpy(shifts[shift_n].exam_date,  dt,  STR_SM-1);
        strncpy(shifts[shift_n].shift,      sh,  STR_SM-1);
        shifts[shift_n].marks_999 = atoi(m9);
        shifts[shift_n].marks_99  = atoi(m99);
        shifts[shift_n].marks_95  = atoi(m95);
        strncpy(shifts[shift_n].difficulty, dif ? dif : "N/A", STR_MD-1);
        shift_n++; count++;
    }
    fclose(f);
    printf("  [OK] Shift analysis        : %d rows\n",count);
    return count;
}

/* 5. jee_2026_subject_analysis.csv
   Columns: Subject,Percentile,Approx_Marks,Year                    */
int load_subject_analysis(const char *path) {
    FILE *f = fopen(path,"r");
    if (!f) { printf("  [WARN] Cannot open: %s\n",path); return 0; }
    char line[256];
    fgets(line,sizeof(line),f);
    int count=0;
    while (fgets(line,sizeof(line),f) && subj_n<MAX_SUBJ) {
        char *p=line;
        char *su=next_field(&p,',');
        char *pc=next_field(&p,',');
        char *mk=next_field(&p,',');
        char *yr=next_field(&p,',');
        if (!su||!pc||!mk||!yr||*su=='\0') continue;
        strncpy(subj[subj_n].subject, su, STR_SM-1);
        subj[subj_n].percentile   = (float)atof(pc);
        subj[subj_n].approx_marks = atoi(mk);
        subj[subj_n].year         = atoi(yr);
        subj_n++; count++;
    }
    fclose(f);
    printf("  [OK] Subject analysis      : %d rows\n",count);
    return count;
}

/* 6. jee_2026_toppers.csv
   Columns: Name,State,Percentile,Category                          */
int load_toppers(const char *path) {
    FILE *f = fopen(path,"r");
    if (!f) { printf("  [WARN] Cannot open: %s\n",path); return 0; }
    char line[256];
    fgets(line,sizeof(line),f);
    int count=0;
    while (fgets(line,sizeof(line),f) && top_n<MAX_TOPPER) {
        char *p=line;
        char *nm=next_field(&p,',');
        char *st=next_field(&p,',');
        char *pc=next_field(&p,',');
        char *ct=next_field(&p,',');
        if (!nm||!st||!pc||!ct||*nm=='\0') continue;
        strncpy(toppers[top_n].name,     nm, STR_MD-1);
        strncpy(toppers[top_n].state,    st, STR_MD-1);
        toppers[top_n].percentile = (float)atof(pc);
        strncpy(toppers[top_n].category, ct, STR_SM-1);
        top_n++; count++;
    }
    fclose(f);
    printf("  [OK] Toppers               : %d rows\n",count);
    return count;
}

/* 7. jee_2026_category_toppers.csv
   Columns: Category,Name,State,Percentile                          */
int load_cat_toppers(const char *path) {
    FILE *f = fopen(path,"r");
    if (!f) { printf("  [WARN] Cannot open: %s\n",path); return 0; }
    char line[256];
    fgets(line,sizeof(line),f);
    int count=0;
    while (fgets(line,sizeof(line),f) && ctop_n<MAX_TOPPER) {
        char *p=line;
        char *ct=next_field(&p,',');
        char *nm=next_field(&p,',');
        char *st=next_field(&p,',');
        char *pc=next_field(&p,',');
        if (!ct||!nm||!st||!pc||*ct=='\0') continue;
        strncpy(cat_toppers[ctop_n].category, ct, STR_SM-1);
        strncpy(cat_toppers[ctop_n].name,     nm, STR_MD-1);
        strncpy(cat_toppers[ctop_n].state,    st, STR_MD-1);
        cat_toppers[ctop_n].percentile = (float)atof(pc);
        ctop_n++; count++;
    }
    fclose(f);
    printf("  [OK] Category toppers      : %d rows\n",count);
    return count;
}

/* 8. jee_2026_session1_registration_appearance.csv
   Columns: Year,Session,Data_Type,Category,Female,Male,Total       */
int load_registration(const char *path) {
    FILE *f = fopen(path,"r");
    if (!f) { printf("  [WARN] Cannot open: %s\n",path); return 0; }
    char line[256];
    fgets(line,sizeof(line),f);
    int count=0;
    while (fgets(line,sizeof(line),f) && reg_n<MAX_REG) {
        char *p=line;
        char *yr=next_field(&p,',');
        char *se=next_field(&p,',');
        char *dt=next_field(&p,',');
        char *ct=next_field(&p,',');
        char *fe=next_field(&p,',');
        char *ma=next_field(&p,',');
        char *to=next_field(&p,',');
        if (!yr||!se||!dt||!ct||!fe||!ma||!to||*yr=='\0') continue;
        reg[reg_n].year    = atoi(yr);
        reg[reg_n].session = atoi(se);
        strncpy(reg[reg_n].data_type, dt, STR_SM-1);
        strncpy(reg[reg_n].category,  ct, STR_SM-1);
        reg[reg_n].female  = atoi(fe);
        reg[reg_n].male    = atoi(ma);
        reg[reg_n].total   = atoi(to);
        reg_n++; count++;
    }
    fclose(f);
    printf("  [OK] Registration data     : %d rows\n",count);
    return count;
}

/* ================================================================
 *  INTERPOLATION HELPERS
 * ================================================================ */

/* Linear interpolation of percentile from marks using 2026 data */
float estimate_percentile(int marks) {
    int   bm=-1, sm=-1;
    float bp=0,  sp=0;
    int   bd=99999, sd=99999;

    for (int i=0;i<mr_n;i++) {
        if (mr[i].year!=2026) continue;
        int d = abs(mr[i].marks - marks);
        if (d < bd) {
            sm=bm; sp=bp; sd=bd;
            bm=mr[i].marks; bp=mr[i].percentile; bd=d;
        } else if (d < sd && mr[i].marks!=bm) {
            sm=mr[i].marks; sp=mr[i].percentile; sd=d;
        }
    }
    if (bm==marks || sm==-1) return bp;
    if (bm==sm) return bp;
    float ratio = (float)(marks-sm)/(bm-sm);
    float pct   = sp + ratio*(bp-sp);
    if (pct<0)   pct=0;
    if (pct>100) pct=100;
    return pct;
}

/* Linear interpolation of rank from marks using 2026 data */
int estimate_rank(int marks) {
    int   bm=-1, sm=-1;
    float bp=0,  sp=0;
    int   br=0,  sr=0;
    int   bd=99999, sd=99999;

    for (int i=0;i<mr_n;i++) {
        if (mr[i].year!=2026) continue;
        int d = abs(mr[i].marks - marks);
        if (d < bd) {
            sm=bm; sp=bp; sr=br; sd=bd;
            bm=mr[i].marks; bp=mr[i].percentile; br=mr[i].rank; bd=d;
        } else if (d < sd && mr[i].marks!=bm) {
            sm=mr[i].marks; sp=mr[i].percentile; sr=mr[i].rank; sd=d;
        }
    }
    if (bm==marks || sm==-1) return br>0?br:1;
    if (bm==sm) return br>0?br:1;
    float ratio = (float)(marks-sm)/(bm-sm);
    int rk = (int)(sr + ratio*(br-sr));
    return rk>0 ? rk : 1;
}

/* ================================================================
 *  MENU 1 – Marks → Rank & Percentile
 * ================================================================ */
void show_marks_rank_analysis(void) {
    separator('=',72);
    printf("  MARKS vs RANK vs PERCENTILE ANALYSIS  (JEE Mains 2026)\n");
    separator('=',72);

    int marks;
    printf("  Enter your JEE Mains marks (out of 300): ");
    if (scanf("%d",&marks)!=1 || marks<0 || marks>300) {
        printf("  [ERROR] Marks must be 0-300.\n"); return;
    }

    float pct  = estimate_percentile(marks);
    int   rank = estimate_rank(marks);

    printf("\n");
    separator('-',72);
    printf("  Marks Entered          : %d / 300\n", marks);
    printf("  Estimated Percentile   : %.4f\n", pct);
    printf("  Estimated CRL Rank     : %d\n",   rank);
    printf("  Total Candidates 2026  : ~14,00,000\n");

    const char *band;
    if      (pct>=99.9) band="EXCELLENT  – Top 0.1% | JEE Advanced qualifier";
    else if (pct>=99.0) band="VERY GOOD  – Top 1%   | Top NITs reachable";
    else if (pct>=95.0) band="GOOD       – Top 5%   | Good NITs possible";
    else if (pct>=90.0) band="ABOVE AVG  – Top 10%  | Lower NITs / IIITs";
    else if (pct>=75.0) band="AVERAGE    – Top 25%  | State colleges";
    else                band="BELOW AVG             – Consider state quota";

    printf("  Performance Band       : %s\n", band);

    /* Subject-wise benchmarks */
    printf("\n");
    separator('-',72);
    printf("  Subject-wise Benchmark at Your Percentile (2026 data)\n");
    separator('-',72);
    printf("  %-14s  %-12s  %-20s\n","Subject","Nearest %ile","Approx Marks Needed");
    separator('-',72);

    const char *subjects[]={"Physics","Chemistry","Mathematics"};
    for (int s=0;s<3;s++) {
        float best_diff=9999;
        int   best_mk=0;
        float best_pct_s=0;
        for (int i=0;i<subj_n;i++) {
            if (strcmp(subj[i].subject,subjects[s])!=0) continue;
            float dp = fabsf(subj[i].percentile - pct);
            if (dp < best_diff) {
                best_diff  = dp;
                best_mk    = subj[i].approx_marks;
                best_pct_s = subj[i].percentile;
            }
        }
        printf("  %-14s  %-12.1f  ~%d marks\n",
               subjects[s], best_pct_s, best_mk);
    }

    /* Historical trend */
    printf("\n");
    separator('-',72);
    printf("  Historical Context – Nearest marks in past years\n");
    separator('-',72);
    printf("  %-6s  %-8s  %-12s  %-10s  %-12s\n",
           "Year","Marks","Percentile","Rank","Candidates");
    separator('-',72);

    int years[]={2022,2023,2024,2025,2026};
    for (int y=0;y<5;y++) {
        int   cmk=-1,crk=-1,ctc=0,mnd=99999;
        float cpc=0;
        for (int i=0;i<mr_n;i++) {
            if (mr[i].year!=years[y]) continue;
            int d=abs(mr[i].marks-marks);
            if (d<mnd) {
                mnd=d; cmk=mr[i].marks;
                crk=mr[i].rank; cpc=mr[i].percentile;
                ctc=mr[i].total_candidates;
            }
        }
        if (cmk!=-1)
            printf("  %-6d  %-8d  %-12.4f  %-10d  %-12d\n",
                   years[y],cmk,cpc,crk,ctc);
    }
    separator('=',72);
}

/* ================================================================
 *  MENU 2 – College Predictor
 * ================================================================ */
void show_college_predictor(void) {
    separator('=',72);
    printf("  NIT COLLEGE PREDICTOR  (JEE Mains 2026 Cutoffs)\n");
    separator('=',72);

    int marks;
    printf("  Enter your JEE Mains marks (out of 300): ");
    if (scanf("%d",&marks)!=1||marks<0||marks>300) {
        printf("  [ERROR] Invalid marks.\n"); return;
    }

    printf("\n  Select your category:\n");
    printf("    1. General\n    2. OBC-NCL\n    3. SC\n");
    printf("    4. ST\n    5. EWS\n    6. PwD\n");
    printf("  Choice: ");
    int cat_ch;
    if (scanf("%d",&cat_ch)!=1||cat_ch<1||cat_ch>6) {
        printf("  [ERROR] Invalid choice.\n"); return;
    }
    const char *categories[]={"","General","OBC-NCL","SC","ST","EWS","PwD"};
    const char *cat = categories[cat_ch];

    int   est_rank = estimate_rank(marks);
    float est_pct  = estimate_percentile(marks);

    printf("\n");
    separator('-',72);
    printf("  Marks: %d | Estimated Rank: %d | Percentile: %.4f | Category: %s\n",
           marks, est_rank, est_pct, cat);
    separator('-',72);

    int safe=0, moderate=0, reach=0, printed=0;

    printf("\n  %-22s  %-34s  %-10s  %-10s\n",
           "Institute","Branch","Closing Rk","Chance");
    separator('-',72);

    for (int i=0;i<cc_n;i++) {
        if (cc[i].year!=2026) continue;
        if (strcmp(cc[i].category,cat)!=0) continue;

        const char *chance;
        if (est_rank <= cc[i].opening_rank) {
            chance = "SAFE";    safe++;
        } else if (est_rank <= cc[i].closing_rank) {
            chance = "MODERATE"; moderate++;
        } else if (est_rank <= (int)(cc[i].closing_rank * 1.12f)) {
            chance = "REACH";   reach++;
        } else {
            continue;
        }

        printf("  %-22s  %-34s  %-10d  %s\n",
               cc[i].institute, cc[i].branch,
               cc[i].closing_rank, chance);
        printed++;
    }

    if (printed==0) {
        printf("  No NIT options found for rank %d in category %s.\n",
               est_rank, cat);
        printf("  Suggestion: Prepare for JEE Mains Session 2,\n");
        printf("  consider state-quota colleges or private institutes.\n");
    } else {
        printf("\n");
        separator('-',72);
        printf("  Total: %d options  |  SAFE=%d  MODERATE=%d  REACH=%d\n",
               printed, safe, moderate, reach);
        printf("\n  Legend:\n");
        printf("    SAFE     = Your rank < Opening rank (high confidence)\n");
        printf("    MODERATE = Opening rank <= Your rank <= Closing rank\n");
        printf("    REACH    = Your rank up to 12%% above closing rank\n");
    }
    separator('=',72);
}

/* ================================================================
 *  MENU 3 – Category Cutoff Trend 2019-2026
 * ================================================================ */
void show_category_cutoffs(void) {
    separator('=',72);
    printf("  CATEGORY CUTOFF TREND  (2019 – 2026)\n");
    separator('=',72);
    printf("  %-6s  %-12s  %-14s  %-12s\n",
           "Year","Category","Cutoff %ile","Cutoff Marks");
    separator('-',72);

    int years[]={2019,2020,2021,2022,2023,2024,2025,2026};
    const char *cats[]={"General","OBC-NCL","SC","ST","EWS","PwD"};

    for (int y=0;y<8;y++) {
        int found=0;
        for (int c=0;c<6;c++) {
            for (int i=0;i<catcut_n;i++) {
                if (catcut[i].year!=years[y]) continue;
                if (strcmp(catcut[i].category,cats[c])!=0) continue;
                if (catcut[i].cutoff_marks>0.01f)
                    printf("  %-6d  %-12s  %-14.2f  %.1f\n",
                           years[y],cats[c],
                           catcut[i].cutoff_percentile,
                           catcut[i].cutoff_marks);
                else
                    printf("  %-6d  %-12s  %-14.2f  N/A (percentile-only year)\n",
                           years[y],cats[c],
                           catcut[i].cutoff_percentile);
                found=1;
            }
        }
        if (found) printf("\n");
    }
    separator('=',72);
}

/* ================================================================
 *  MENU 4 – Shift Analysis
 * ================================================================ */
void show_shift_analysis(void) {
    separator('=',72);
    printf("  JEE MAINS 2026 – SESSION 1 SHIFT-WISE PAPER ANALYSIS\n");
    separator('=',72);
    printf("  %-16s  %-9s  %-11s  %-10s  %-10s  %-22s\n",
           "Date","Shift","Marks@99.9","Marks@99","Marks@95","Difficulty");
    separator('-',72);

    float avg999=0,avg99=0,avg95=0;
    for (int i=0;i<shift_n;i++) {
        printf("  %-16s  %-9s  %-11d  %-10d  %-10d  %s\n",
               shifts[i].exam_date, shifts[i].shift,
               shifts[i].marks_999, shifts[i].marks_99,
               shifts[i].marks_95,  shifts[i].difficulty);
        avg999 += shifts[i].marks_999;
        avg99  += shifts[i].marks_99;
        avg95  += shifts[i].marks_95;
    }

    if (shift_n>0) {
        separator('-',72);
        printf("  %-16s  %-9s  %-11.1f  %-10.1f  %-10.1f  Average\n",
               "AVERAGE","All Shifts",
               avg999/shift_n, avg99/shift_n, avg95/shift_n);
    }
    separator('=',72);
}

/* ================================================================
 *  MENU 5 – Toppers
 * ================================================================ */
void show_toppers(void) {
    separator('=',72);
    printf("  JEE MAINS 2026 – TOPPERS\n");
    separator('=',72);

    printf("\n  100-Percentile Achievers:\n");
    separator('-',72);
    printf("  %-4s  %-28s  %-18s  %-12s  %s\n",
           "No.","Name","State","Percentile","Category");
    separator('-',72);
    for (int i=0;i<top_n;i++)
        printf("  %-4d  %-28s  %-18s  %-12.7f  %s\n",
               i+1, toppers[i].name, toppers[i].state,
               toppers[i].percentile, toppers[i].category);

    printf("\n  Category-Wise Toppers:\n");
    separator('-',72);
    printf("  %-20s  %-28s  %-18s  %s\n",
           "Category","Name","State","Percentile");
    separator('-',72);
    for (int i=0;i<ctop_n;i++)
        printf("  %-20s  %-28s  %-18s  %.7f\n",
               cat_toppers[i].category, cat_toppers[i].name,
               cat_toppers[i].state,   cat_toppers[i].percentile);

    separator('=',72);
}

/* ================================================================
 *  MENU 6 – Registration Statistics
 * ================================================================ */
void show_registration(void) {
    separator('=',72);
    printf("  JEE MAINS 2026 SESSION 1 – REGISTRATION & APPEARANCE\n");
    separator('=',72);

    const char *types[]={"Registered","Appeared"};
    for (int t=0;t<2;t++) {
        printf("\n  %s:\n", types[t]);
        printf("  %-12s  %-10s  %-10s  %-10s\n",
               "Category","Female","Male","Total");
        separator('-',50);
        int tf=0,tm=0,tt=0;
        for (int i=0;i<reg_n;i++) {
            if (strcmp(reg[i].data_type,types[t])!=0) continue;
            printf("  %-12s  %-10d  %-10d  %-10d\n",
                   reg[i].category, reg[i].female, reg[i].male, reg[i].total);
            tf+=reg[i].female; tm+=reg[i].male; tt+=reg[i].total;
        }
        printf("  %-12s  %-10d  %-10d  %-10d\n","TOTAL",tf,tm,tt);
    }

    /* appearance rate */
    int reg_tot=0, app_tot=0;
    for (int i=0;i<reg_n;i++) {
        if (strcmp(reg[i].data_type,"Registered")==0) reg_tot+=reg[i].total;
        if (strcmp(reg[i].data_type,"Appeared")==0)   app_tot+=reg[i].total;
    }
    if (reg_tot>0)
        printf("\n  Appearance Rate: %.2f%%\n",
               100.0f*app_tot/reg_tot);
    separator('=',72);
}

/* ================================================================
 *  MENU 7 – Export predictions to CSV
 * ================================================================ */
void export_results(void) {
    separator('=',72);
    printf("  EXPORT COLLEGE PREDICTIONS TO CSV FILE\n");
    separator('=',72);

    int marks;
    printf("  Enter marks (0-300): ");
    if (scanf("%d",&marks)!=1||marks<0||marks>300) {
        printf("  [ERROR] Invalid.\n"); return;
    }

    printf("  Category (1=Gen 2=OBC 3=SC 4=ST 5=EWS 6=PwD): ");
    int cat_ch;
    if (scanf("%d",&cat_ch)!=1||cat_ch<1||cat_ch>6) {
        printf("  [ERROR] Invalid.\n"); return;
    }
    const char *categories[]={"","General","OBC-NCL","SC","ST","EWS","PwD"};
    const char *cat = categories[cat_ch];

    char outfile[128]="my_predictions.csv";
    printf("  Output filename [my_predictions.csv]: ");
    scanf("%s", outfile);

    FILE *f = fopen(outfile,"w");
    if (!f) { printf("  [ERROR] Cannot create file.\n"); return; }

    int   est_rank = estimate_rank(marks);
    float est_pct  = estimate_percentile(marks);

    fprintf(f,"JEE Mains 2026 College Predictions\n");
    fprintf(f,"Marks,%d\n",marks);
    fprintf(f,"Estimated Percentile,%.4f\n",est_pct);
    fprintf(f,"Estimated Rank,%d\n",est_rank);
    fprintf(f,"Category,%s\n\n",cat);
    fprintf(f,"Institute,Branch,Category,Opening Rank,Closing Rank,Chance\n");

    int count=0;
    for (int i=0;i<cc_n;i++) {
        if (cc[i].year!=2026) continue;
        if (strcmp(cc[i].category,cat)!=0) continue;

        const char *chance;
        if      (est_rank <= cc[i].opening_rank) chance="SAFE";
        else if (est_rank <= cc[i].closing_rank) chance="MODERATE";
        else if (est_rank <= (int)(cc[i].closing_rank*1.12f)) chance="REACH";
        else continue;

        fprintf(f,"%s,%s,%s,%d,%d,%s\n",
                cc[i].institute,cc[i].branch,cc[i].category,
                cc[i].opening_rank,cc[i].closing_rank,chance);
        count++;
    }
    fprintf(f,"\nTotal Options,%d\n",count);
    fclose(f);
    printf("  [OK] %d options written to '%s'\n",count,outfile);
}

/* ================================================================
 *  MENU 8 – Full Marks-Percentile Table 2026
 * ================================================================ */
void show_percentile_table(void) {
    separator('=',72);
    printf("  JEE MAINS 2026 – MARKS vs PERCENTILE vs RANK TABLE\n");
    separator('=',72);
    printf("  %-8s  %-12s  %-12s\n","Marks","Percentile","Est. Rank");
    separator('-',40);

    /* collect 2026 data */
    int   mks[MAX_MR]; float pcs[MAX_MR]; int rks[MAX_MR];
    int   cnt=0;
    for (int i=0;i<mr_n;i++) {
        if (mr[i].year!=2026) {
            mks[cnt]=mr[i].marks; pcs[cnt]=mr[i].percentile;
            rks[cnt]=mr[i].rank;  cnt++;
        }
    }
    /* only 2026 */
    cnt=0;
    for (int i=0;i<mr_n;i++) {
        if (mr[i].year==2026) {
            mks[cnt]=mr[i].marks; pcs[cnt]=mr[i].percentile;
            rks[cnt]=mr[i].rank;  cnt++;
        }
    }

    /* sort descending by marks (simple insertion sort) */
    for (int a=1;a<cnt;a++) {
        int km=mks[a]; float kp=pcs[a]; int kr=rks[a];
        int b=a-1;
        while (b>=0 && mks[b]<km) {
            mks[b+1]=mks[b]; pcs[b+1]=pcs[b]; rks[b+1]=rks[b]; b--;
        }
        mks[b+1]=km; pcs[b+1]=kp; rks[b+1]=kr;
    }

    int last_m=-1;
    for (int i=0;i<cnt;i++) {
        if (mks[i]==last_m) continue;
        last_m=mks[i];
        printf("  %-8d  %-12.4f  %-12d\n", mks[i], pcs[i], rks[i]);
    }
    separator('=',72);
}

/* ================================================================
 *  MAIN
 * ================================================================ */
static void print_banner(void) {
    separator('*',72);
    printf("*%-70s*\n","");
    printf("*   JEE MAINS 2026 – COLLEGE PREDICTOR & MARKS-RANK ANALYSER       *\n");
    printf("*   Built in C | Reads all 8 JEE 2026 CSV data files               *\n");
    printf("*%-70s*\n","");
    separator('*',72);
}

static void print_menu(void) {
    printf("\n  MAIN MENU\n");
    separator('-',55);
    printf("  1. Marks  →  Rank & Percentile Analysis\n");
    printf("  2. College Predictor  (NIT Branch-wise Cutoffs)\n");
    printf("  3. Category Cutoff Trend  (2019 – 2026)\n");
    printf("  4. Shift-wise Paper Difficulty Analysis\n");
    printf("  5. Toppers & Hall of Fame\n");
    printf("  6. Registration & Appearance Statistics\n");
    printf("  7. Export My Predictions to CSV\n");
    printf("  8. Full Marks-Percentile Table  (2026)\n");
    printf("  0. Exit\n");
    separator('-',55);
    printf("  Enter choice: ");
}

int main(void) {
    print_banner();
    printf("\n  Loading data from CSV files...\n\n");

    load_marks_rank        ("jee_marks_percentile_rank_2009_2026.csv");
    load_college_cutoffs   ("jee_college_cutoffs_nits.csv");
    load_category_cutoffs  ("jee_category_cutoffs_2009_2026.csv");
    load_shift_analysis    ("jee_2026_shift_analysis.csv");
    load_subject_analysis  ("jee_2026_subject_analysis.csv");
    load_toppers           ("jee_2026_toppers.csv");
    load_cat_toppers       ("jee_2026_category_toppers.csv");
    load_registration      ("jee_2026_session1_registration_appearance.csv");

    printf("\n  All CSV files processed. System ready!\n");

    int choice;
    do {
        print_menu();
        if (scanf("%d",&choice)!=1) {
            choice=-1;
            while (getchar()!='\n');
        }
        printf("\n");
        switch(choice) {
            case 1: show_marks_rank_analysis(); break;
            case 2: show_college_predictor();   break;
            case 3: show_category_cutoffs();    break;
            case 4: show_shift_analysis();      break;
            case 5: show_toppers();             break;
            case 6: show_registration();        break;
            case 7: export_results();           break;
            case 8: show_percentile_table();    break;
            case 0: printf("  Goodbye! Best of luck for JEE!\n\n"); break;
            default: printf("  Invalid choice. Please try again.\n"); break;
        }
    } while (choice!=0);

    return 0;
}
