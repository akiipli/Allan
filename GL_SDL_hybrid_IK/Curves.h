/*
The MIT License

Copyright <2018> <Allan Kiipli>
*/

/*
Since verts have all the grouping and weights already set up,
curves should connect to the verts through cps.
For edges and segments association is made level wise.
Then when subdlevel is raised, vertex tuning is made with
curve tuning.
For animated deformed objects, in every frame all curves
are updated to conform also subdivided shape.
For this to work, curves need to be under objects.
Also cps array needs to be under same object.
Also segments need to be level wise under object,
similar fashion to edges.
*/

#ifndef CURVES_H_INCLUDED
#define CURVES_H_INCLUDED

#define CURVES 10000
#define SEGMENTS 100000
#define CPS 200000

#define OBJECT_CURVES 1000
#define OBJECT_CPS 10000
#define OBJECT_SEGMENTS 10000

#define CP_CONTINUITY 0.66

int selected_curves[CURVES];
int selected_curves_count = 0;
int curvesIndex = 0;
int segmentIndex = 0;
int selected_cps[CPS];
int selected_cps_count = 0;
int cpsIndex = 0;
int currentCurve = 0;
int currentCp = 0;

int cp_Manipulation = 0;
int curve_Manipulation = 0;

float length(float V[3]);
size_t float_3 = sizeof(float[3]);

float cp_continuity[SUBD];

void init_cp_continuity()
{
    int l;
    float v = 0.3; // for subdivided segments it is safe to use A and C points in Segments.
                   // Thus B is not used and we scale tangents down significantly.
    float i = v / SUBD;

    for (l = 0; l < SUBD; l ++)
    {
        cp_continuity[l] = v;
        v -= i;
    }
}

struct curve_segment
{
    int index;
    unsigned address;
    int selected;
    int level;
    int edge; // edge with level
    int subdivided;
    curve * Curve;
    curve_segment * segment[2]; // if subdivided
    float A[3]; // begin
    float B[3]; // center
    float C[3]; // end
};

curve_segment * segments[SEGMENTS];

struct cp
{
    int index;
    unsigned address;
    int selected;
    float pos[3];
    float Pos[3];
    vertex * vert;
    curve_segment ** segments;
    int segment_count;
    float A[3]; // tangent begin
    float B[3]; // tangent center
    float C[3]; // tangent end
};

cp * cps[CPS];

struct curve
{
    int index;
    unsigned address;
    int selected;
    cp ** cps; // index in objects cps array
    float * cps_continuity; // marks pin_points
    float * cps_continuity_init;
    int cps_count;
    curve_segment ** segments; // index in objects segments array
    int segment_count;
    int open;
};

curve * curves[CURVES];

void initialize_Cp(cp * CP)
{
    CP->selected = 0;
    CP->pos[0] = 0;
    CP->pos[1] = 0;
    CP->pos[2] = 0;
    CP->vert = NULL;
    CP->segments = malloc(0 * sizeof(curve_segment*));
    CP->segment_count = 0;
}

void calculate_Curve_Segment_B(curve * C, int index)
{
    float vec[3];
    float continuity0, continuity1;

    cp * CP0, * CP1;
    curve_segment * S = C->segments[index];
    S->level = 0;
    CP0 = C->cps[index];
    CP1 = C->cps[(index + 1) % C->cps_count];
    continuity0 = C->cps_continuity[index];
    continuity1 = C->cps_continuity[(index + 1) % C->cps_count];

    vec[0] = CP0->pos[0] - CP0->B[0];
    vec[1] = CP0->pos[1] - CP0->B[1];
    vec[2] = CP0->pos[2] - CP0->B[2];

    CP0->C[0] += vec[0];
    CP0->C[1] += vec[1];
    CP0->C[2] += vec[2];

    CP0->C[0] -= CP0->pos[0];
    CP0->C[1] -= CP0->pos[1];
    CP0->C[2] -= CP0->pos[2];

    CP0->C[0] *= continuity0;
    CP0->C[1] *= continuity0;
    CP0->C[2] *= continuity0;

    CP0->C[0] += CP0->pos[0];
    CP0->C[1] += CP0->pos[1];
    CP0->C[2] += CP0->pos[2];

    vec[0] = CP1->pos[0] - CP1->B[0];
    vec[1] = CP1->pos[1] - CP1->B[1];
    vec[2] = CP1->pos[2] - CP1->B[2];

    CP1->A[0] += vec[0];
    CP1->A[1] += vec[1];
    CP1->A[2] += vec[2];

    CP1->A[0] -= CP1->pos[0];
    CP1->A[1] -= CP1->pos[1];
    CP1->A[2] -= CP1->pos[2];

    CP1->A[0] *= continuity1;
    CP1->A[1] *= continuity1;
    CP1->A[2] *= continuity1;

    CP1->A[0] += CP1->pos[0];
    CP1->A[1] += CP1->pos[1];
    CP1->A[2] += CP1->pos[2];

    S->B[0] = (CP0->C[0] + CP1->A[0]) / 2.0;
    S->B[1] = (CP0->C[1] + CP1->A[1]) / 2.0;
    S->B[2] = (CP0->C[2] + CP1->A[2]) / 2.0;
}

void calculate_Curve_Segments(curve * C)
{
    int i;

    for (i = 0; i < C->segment_count; i ++)
    {
        calculate_Curve_Segment_B(C, i);
    }
}

void fill_Curve_Cps_With_Tangents(curve * C)
{
    int s;
    float len0, len1, portion;
    float vec[3], vec0[3], vec1[3];

    cp * CP;
    curve_segment * S0, * S1;

    for (s = 0; s < C->segment_count; s ++)
    {
        S0 = C->segments[s];
        S1 = C->segments[(s + 1) % C->segment_count];

        CP = C->cps[(s + 1) % C->segment_count];

        memcpy(CP->A, S0->B, float_3);
        memcpy(CP->C, S1->B, float_3);

        vec[0] = CP->C[0] - CP->A[0];
        vec[1] = CP->C[1] - CP->A[1];
        vec[2] = CP->C[2] - CP->A[2];

        vec0[0] = CP->pos[0] - CP->A[0];
        vec0[1] = CP->pos[1] - CP->A[1];
        vec0[2] = CP->pos[2] - CP->A[2];

        vec1[0] = CP->pos[0] - CP->C[0];
        vec1[1] = CP->pos[1] - CP->C[1];
        vec1[2] = CP->pos[2] - CP->C[2];

        len0 = length(vec0);
        len1 = length(vec1);

//        if ((len0 + len1) > 0)
            portion = len0 / (len0 + len1);
//        else
//            portion = 0;

        CP->B[0] = CP->A[0] + vec[0] * portion;
        CP->B[1] = CP->A[1] + vec[1] * portion;
        CP->B[2] = CP->A[2] + vec[2] * portion;
    }

    if (C->open)
    {
        S0 = C->segments[0];
        CP = C->cps[0];

        memcpy(CP->A, S0->A, float_3);
        memcpy(CP->B, S0->A, float_3);
        memcpy(CP->C, S0->B, float_3);

        S1 = C->segments[C->segment_count - 2];
        CP = C->cps[C->cps_count - 1];

        memcpy(CP->A, S1->B, float_3);
        memcpy(CP->B, S1->C, float_3);
        memcpy(CP->C, S1->C, float_3);
    }
}

void fill_Curve_Segments_Tips_With_Cp_Coordinates(curve * C)
{
    int c;

    cp * CP0, * CP1;
    curve_segment * S;

    for (c = 0; c < C->cps_count; c ++)
    {
        CP0 = C->cps[c];
        CP1 = C->cps[(c + 1) % C->cps_count];

        S = C->segments[c];

        memcpy(S->A, CP0->pos, float_3);
        memcpy(S->C, CP1->pos, float_3);
//        S->B[0] = (S->A[0] + S->C[0]) / 2;
//        S->B[1] = (S->A[1] + S->C[1]) / 2;
//        S->B[2] = (S->A[2] + S->C[2]) / 2;
    }
}

void fill_Curve_Segments_With_Cp_Coordinates(curve * C)
{
    int c;

    cp * CP0, * CP1;
    curve_segment * S;

    for (c = 0; c < C->cps_count; c ++)
    {
        CP0 = C->cps[c];
        CP1 = C->cps[(c + 1) % C->cps_count];

        S = C->segments[c];

        memcpy(S->A, CP0->pos, float_3);
        memcpy(S->C, CP1->pos, float_3);
        S->B[0] = (S->A[0] + S->C[0]) / 2;
        S->B[1] = (S->A[1] + S->C[1]) / 2;
        S->B[2] = (S->A[2] + S->C[2]) / 2;
    }
}

void fill_Curve_Segment_With_Coordinates(curve_segment * S, curve_segment * S0, curve_segment * S1,
                    int level, int start_open, int end_open,
                    int start_segment, int end_segment, float start_segment_continuity, float end_segment_continuity)
{
    curve_segment * S_0, * S_1, * S_2, * S_3;

    if (level >= S->level && S->level > 0) // level 0 is filled with Coordinates and Tangents in update Curve, also segments
                                           // level 0 continuity is controlled with curves continuity arrays.
    {
        float len0, len1, portion;
        float vec[3], vec0[3], vec1[3];

        float A[3];
        float B[3];
        float C[3];

        float A1[3];
        float B1[3];
        float C1[3];

        if (start_open)
        {
            C[0] = (S->A[0] + S->C[0]) / 2;
            C[1] = (S->A[1] + S->C[1]) / 2;
            C[2] = (S->A[2] + S->C[2]) / 2;
        }
        else
        {
            memcpy(A, S0->A, float_3);
            memcpy(C, S->C, float_3);

            vec[0] = C[0] - A[0];
            vec[1] = C[1] - A[1];
            vec[2] = C[2] - A[2];

            vec0[0] = S->A[0] - A[0];
            vec0[1] = S->A[1] - A[1];
            vec0[2] = S->A[2] - A[2];

            vec1[0] = S->A[0] - C[0];
            vec1[1] = S->A[1] - C[1];
            vec1[2] = S->A[2] - C[2];

            len0 = length(vec0);
            len1 = length(vec1);

    //        if ((len0 + len1) > 0)
                portion = len0 / (len0 + len1);
    //        else
    //            portion = 0;

            B[0] = A[0] + vec[0] * portion;
            B[1] = A[1] + vec[1] * portion;
            B[2] = A[2] + vec[2] * portion;

            ////

            vec[0] = S->A[0] - B[0];
            vec[1] = S->A[1] - B[1];
            vec[2] = S->A[2] - B[2];

            C[0] += vec[0];
            C[1] += vec[1];
            C[2] += vec[2];

            C[0] -= S->A[0];
            C[1] -= S->A[1];
            C[2] -= S->A[2];

            if (start_segment)
            {
                C[0] *= start_segment_continuity;
                C[1] *= start_segment_continuity;
                C[2] *= start_segment_continuity;
            }
            else
            {
                C[0] *= cp_continuity[S->level];
                C[1] *= cp_continuity[S->level];
                C[2] *= cp_continuity[S->level];
            }


            C[0] += S->A[0];
            C[1] += S->A[1];
            C[2] += S->A[2];
        }

        ////

        if (end_open)
        {
            A1[0] = (S->A[0] + S->C[0]) / 2;
            A1[1] = (S->A[1] + S->C[1]) / 2;
            A1[2] = (S->A[2] + S->C[2]) / 2;
        }
        else
        {
            memcpy(A1, S->A, float_3);
            memcpy(C1, S1->C, float_3);

            vec[0] = C1[0] - A1[0];
            vec[1] = C1[1] - A1[1];
            vec[2] = C1[2] - A1[2];

            vec0[0] = S->C[0] - A1[0];
            vec0[1] = S->C[1] - A1[1];
            vec0[2] = S->C[2] - A1[2];

            vec1[0] = S->C[0] - C1[0];
            vec1[1] = S->C[1] - C1[1];
            vec1[2] = S->C[2] - C1[2];

            len0 = length(vec0);
            len1 = length(vec1);

    //        if ((len0 + len1) > 0)
                portion = len0 / (len0 + len1);
    //        else
    //            portion = 0;

            B1[0] = A1[0] + vec[0] * portion;
            B1[1] = A1[1] + vec[1] * portion;
            B1[2] = A1[2] + vec[2] * portion;

            ////

            vec[0] = S->C[0] - B1[0];
            vec[1] = S->C[1] - B1[1];
            vec[2] = S->C[2] - B1[2];

            A1[0] += vec[0];
            A1[1] += vec[1];
            A1[2] += vec[2];

            A1[0] -= S->C[0];
            A1[1] -= S->C[1];
            A1[2] -= S->C[2];

            if (end_segment)
            {
                A1[0] *= end_segment_continuity;
                A1[1] *= end_segment_continuity;
                A1[2] *= end_segment_continuity;
            }
            else
            {
                A1[0] *= cp_continuity[S->level];
                A1[1] *= cp_continuity[S->level];
                A1[2] *= cp_continuity[S->level];
            }

            A1[0] += S->C[0];
            A1[1] += S->C[1];
            A1[2] += S->C[2];
        }

        ////

        S->B[0] = (C[0] + A1[0]) / 2.0;
        S->B[1] = (C[1] + A1[1]) / 2.0;
        S->B[2] = (C[2] + A1[2]) / 2.0;
    }

    if (S0->subdivided && S->subdivided && S1->subdivided && level >= S->level + 1)
    {
        S_0 = S0->segment[1];
        S_1 = S->segment[0];
        S_2 = S->segment[1];
        S_3 = S1->segment[0];

        memcpy(S_1->A, S->A, float_3);
        memcpy(S_1->C, S->B, float_3);

        memcpy(S_2->A, S->B, float_3);
        memcpy(S_2->C, S->C, float_3);

        if (S->level == 0)
        {
            if (start_open)
                fill_Curve_Segment_With_Coordinates(S_1, S_0, S_2, level, 1, 0, 1, 0, start_segment_continuity, end_segment_continuity);
            else
                fill_Curve_Segment_With_Coordinates(S_1, S_0, S_2, level, 0, 0, 1, 0, start_segment_continuity, end_segment_continuity);
            if (end_open)
                fill_Curve_Segment_With_Coordinates(S_2, S_1, S_3, level, 0, 1, 0, 1, start_segment_continuity, end_segment_continuity);
            else
                fill_Curve_Segment_With_Coordinates(S_2, S_1, S_3, level, 0, 0, 0, 1, start_segment_continuity, end_segment_continuity);
        }
        else if (start_segment)
        {
            if (start_open)
                fill_Curve_Segment_With_Coordinates(S_1, S_0, S_2, level, 1, 0, 1, 0, start_segment_continuity, end_segment_continuity);
            else
                fill_Curve_Segment_With_Coordinates(S_1, S_0, S_2, level, 0, 0, 1, 0, start_segment_continuity, end_segment_continuity);
            if (end_open)
                fill_Curve_Segment_With_Coordinates(S_2, S_1, S_3, level, 0, 1, 0, 0, start_segment_continuity, end_segment_continuity);
            else
                fill_Curve_Segment_With_Coordinates(S_2, S_1, S_3, level, 0, 0, 0, 0, start_segment_continuity, end_segment_continuity);
        }
        else if (end_segment)
        {
            if (start_open)
                fill_Curve_Segment_With_Coordinates(S_1, S_0, S_2, level, 1, 0, 0, 0, start_segment_continuity, end_segment_continuity);
            else
                fill_Curve_Segment_With_Coordinates(S_1, S_0, S_2, level, 0, 0, 0, 0, start_segment_continuity, end_segment_continuity);
            if (end_open)
                fill_Curve_Segment_With_Coordinates(S_2, S_1, S_3, level, 0, 1, 0, 1, start_segment_continuity, end_segment_continuity);
            else
                fill_Curve_Segment_With_Coordinates(S_2, S_1, S_3, level, 0, 0, 0, 1, start_segment_continuity, end_segment_continuity);
        }
        else
        {
            if (start_open)
                fill_Curve_Segment_With_Coordinates(S_1, S_0, S_2, level, 1, 0, 0, 0, start_segment_continuity, end_segment_continuity);
            else
                fill_Curve_Segment_With_Coordinates(S_1, S_0, S_2, level, 0, 0, 0, 0, start_segment_continuity, end_segment_continuity);
            if (end_open)
                fill_Curve_Segment_With_Coordinates(S_2, S_1, S_3, level, 0, 1, 0, 0, start_segment_continuity, end_segment_continuity);
            else
                fill_Curve_Segment_With_Coordinates(S_2, S_1, S_3, level, 0, 0, 0, 0, start_segment_continuity, end_segment_continuity);
        }
    }
}

void fill_Curve_Segments_With_Coordinates(curve * C, int level)
{
    int s;
    curve_segment * S, * S0, * S1;

    float start_segment_continuity;
    float end_segment_continuity;

    for (s = 0; s < C->segment_count; s ++)
    {
        S = C->segments[s];

        start_segment_continuity = C->cps_continuity[s] * 0.5;

        if (s - 1 < 0)
        {
            S0 = C->segments[C->segment_count - 1];
        }
        else
        {
            S0 = C->segments[s - 1];
        }

        if (s + 1 >= C->segment_count)
        {
            S1 = C->segments[0];
            end_segment_continuity = C->cps_continuity[0] * 0.5;
        }
        else
        {
            S1 = C->segments[s + 1];
            end_segment_continuity = C->cps_continuity[s + 1] * 0.5;
        }

        if (level >= S->level)
        {
            if (C->open && s == 0)
            {
                fill_Curve_Segment_With_Coordinates(S, S0, S1, level, 1, 0, 0, 0, start_segment_continuity, end_segment_continuity);
            }
            else if (C->open && s == C->segment_count - 2)
            {
                fill_Curve_Segment_With_Coordinates(S, S0, S1, level, 0, 1, 0, 0, start_segment_continuity, end_segment_continuity);
            }
            else
            {
                fill_Curve_Segment_With_Coordinates(S, S0, S1, level, 0, 0, 0, 0, start_segment_continuity, end_segment_continuity);
            }
        }
    }
}

void subdivide_Curve_Segment(curve_segment * S, int level)
{
    if (!S->subdivided && level > S->level)
    {
        curve_segment * S0, * S1;

        S0 = malloc(sizeof(curve_segment));
        S1 = malloc(sizeof(curve_segment));

        if (S0 && S1)
        {
            S0->index = segmentIndex;
            S0->level = S->level + 1;
            S0->subdivided = 0;
            segments[segmentIndex ++] = S0;

            S1->index = segmentIndex;
            S1->level = S->level + 1;
            S1->subdivided = 0;
            segments[segmentIndex ++] = S1;

            S->subdivided = 1;
            S->segment[0] = S0;
            S->segment[1] = S1;
        }
    }

    if (S->subdivided && level > S->level + 1)
    {
        subdivide_Curve_Segment(S->segment[0], level);
        subdivide_Curve_Segment(S->segment[1], level);
    }
}

void subdivide_Curve_Segments(curve * C, int level)
{
    int s;
    curve_segment * S;

    for (s = 0; s < C->segment_count; s ++)
    {
        S = C->segments[s];
        subdivide_Curve_Segment(S, level);
    }
}

void free_Subdivided_Segment(curve_segment * S0)
{
    if (S0->subdivided)
    {
        free_Subdivided_Segment(S0->segment[0]);
        free_Subdivided_Segment(S0->segment[1]);
    }
    free(S0);
}

void remove_CP_From_Curve(curve * C, cp * CP)
{
    int index, c;

    int condition = 0;

    for (c = 0; c < C->cps_count; c ++)
    {
        if (CP == C->cps[c])
        {
            index = c;
            condition = 1;
            break;
        }
    }

    if (condition)
    {
        C->cps_count --;
        for (c = index; c < C->cps_count; c ++)
        {
            C->cps[c] = C->cps[c + 1];
            C->cps_continuity[c] = C->cps_continuity[c + 1];
        }

        // find segments in this curve
        // connecting this CP
        // Merge segments into first segment
        // Assign first segment to next CP
        // replacing the old segment
        // Delete second segment

        int s, s0;
        curve_segment * CS, * CS0;
        curve_segment * S[2], * S0;
        int Idx[2];

        int seg_count = 0;

        for (s0 = 0; s0 < C->segment_count; s0 ++)
        {
            CS0 = C->segments[s0];
            for (s = 0; s < CP->segment_count; s ++)
            {
                CS = CP->segments[s];
                if (CS == CS0)
                {
                    S[seg_count] = CS;
                    Idx[seg_count] = s0;
                    seg_count ++;
                }
            }
            if (seg_count > 1)
            {
                break;
            }
        }

        if (seg_count > 0)
        {
            S0 = S[seg_count - 1];
            C->segment_count --;
            for (s0 = Idx[seg_count - 1]; s0 < C->segment_count; s0 ++)
            {
                C->segments[s0] = C->segments[s0 + 1];
            }

            // free S0 and its subsegments

            if (S0->subdivided)
            {
                free_Subdivided_Segment(S0);
            }
        }
    }
}

int add_Cp_To_Curve(curve * C, cp * CP)
{
    C->cps_count ++;
    C->cps = realloc(C->cps, C->cps_count * sizeof(cp*));
    C->cps_continuity = realloc(C->cps_continuity, C->cps_count * sizeof(float));
    if (C->cps == NULL)
    {
        C->cps_count --;
        return 0;
    }
    C->cps[C->cps_count - 1] = CP;
    C->cps_continuity[C->cps_count - 1] = 0.7;

    if (C->segment_count > 0)
    {
        CP->segment_count ++;
        CP->segments = realloc(CP->segments, CP->segment_count * sizeof(curve_segment*));
        if (CP->segments == NULL)
        {
            C->cps_count --;
            CP->segment_count --;
            return 0;
        }
        CP->segments[CP->segment_count - 1] = C->segments[C->segment_count - 1];

        CP = C->cps[C->cps_count - 2];
        CP->segment_count ++;
        CP->segments = realloc(CP->segments, CP->segment_count * sizeof(curve_segment*));
        if (CP->segments == NULL)
        {
            C->cps_count --;
            CP->segment_count --;
            return 0;
        }
        CP->segments[CP->segment_count - 1] = C->segments[C->segment_count - 1];
    }

    return 1;
}

int add_Cp(object * O)
{
    printf("add Cp\n");

    if (O->cps_count >= OBJECT_CPS || cpsIndex >= CPS)
    {
        return 0;
    }

    cp * CP = malloc(sizeof(cp));
    if (CP == NULL)
    {
        return 0;
    }

    initialize_Cp(CP);

    O->cps[O->cps_count ++] = CP;
    cps[cpsIndex] = CP;
    CP->index = cpsIndex;
    cpsIndex ++;
    return 1;
}

void initialize_Curve(curve * C)
{
    C->cps = malloc(0 * sizeof(cp*)); // index in objects cps array
    C->cps_continuity = malloc(0 * sizeof(float)); // marks pin_points
    C->cps_continuity_init = malloc(0 * sizeof(float)); // marks init pin_points
    C->cps_count = 0;
    C->segments = malloc(0 * sizeof(curve_segment*)); // index in objects segments array
    C->segment_count = 0;
    C->selected = 0;
    C->open = 0;
}

void free_Cp(cp * CP)
{
    free(CP->segments);
    free(CP);
}

int add_Curve_Segment(curve * C)
{
    if (cpsIndex >= CPS)
    {
        return 0;
    }

    if (segmentIndex >= SEGMENTS)
    {
        return 0;
    }

    cp * CP = malloc(sizeof(cp));

    if (CP == NULL) return 0;

    CP->index = cpsIndex;
    cps[cpsIndex ++] = CP;

    curve_segment * S = malloc(sizeof(curve_segment));

    if (S == NULL) return 0;

    S->level = 0;
    S->subdivided = 0;

    segments[segmentIndex ++] = S;

    C->cps = realloc(C->cps, (C->cps_count + 1) * sizeof(cp*));

    if (C->cps == NULL) return 0;

    C->cps[C->cps_count ++] = CP;

    C->cps_continuity = realloc(C->cps_continuity, (C->cps_count) * sizeof(float));

    if (C->cps_continuity == NULL) return 0;

    C->cps_continuity[C->cps_count - 1] = 0.7;

    C->segments = realloc(C->segments, (C->segment_count + 1) * sizeof(curve_segment*));

    if (C->segments == NULL) return 0;

    S->index = C->segment_count; // Segment index is Curve based, not global segments

    C->segments[C->segment_count ++] = S;

    //memcpy(CP->pos, (float[3]){1.0, 1.0, 1.0}, float_3);

    memcpy(CP->pos, C->cps[C->cps_count - 2]->pos, float_3);

    C->cps[C->cps_count - 2]->segments[1] = S;
    C->cps[C->cps_count - 2]->segment_count = 2;

    CP->segments = malloc(2 * sizeof(curve_segment *));
    CP->segment_count = 1;
    CP->segments[0] = S;
    CP->selected = 0;

    return 1;
}

int add_New_Curve(float pos[3], int open)
{
    printf("add New Curve\n");

    if (curvesIndex >= CURVES)
    {
        return 0;
    }

    curve * C = malloc(sizeof(curve));
    if (C == NULL)
    {
        return 0;
    }

    initialize_Curve(C);
    curves[curvesIndex] = C;
    C->index = curvesIndex;
    curvesIndex ++;

    if (cpsIndex >= CPS)
    {
        return 0;
    }

    cp * CP = malloc(sizeof(cp));

    if (CP == NULL) return 0;

    CP->index = cpsIndex;
    cps[cpsIndex ++] = CP;

    memcpy(CP->pos, pos, float_3);
    CP->selected = 0;

    C->cps = realloc(C->cps, (C->cps_count + 1) * sizeof(cp*));

    if (C->cps == NULL) return 0;

    C->cps[C->cps_count ++] = CP;

    C->cps_continuity = realloc(C->cps_continuity, (C->cps_count) * sizeof(float));

    if (C->cps_continuity == NULL) return 0;

    C->cps_continuity[C->cps_count - 1] = CP_CONTINUITY;

    C->open = open;

    if (segmentIndex >= SEGMENTS)
    {
        return 0;
    }

    curve_segment * S = malloc(sizeof(curve_segment));

    if (S == NULL) return 0;

    S->level = 0;
    S->subdivided = 0;

    segments[segmentIndex ++] = S;

    C->segments = realloc(C->segments, (C->segment_count + 1) * sizeof(curve_segment*));
    if (C->segments == NULL) return 0;

    S->index = C->segment_count; // Segment index is Curve based, not global segments

    C->segments[C->segment_count ++] = S;

    CP->segments = malloc(2 * sizeof(curve_segment *));
    CP->segment_count = 1;
    CP->segments[0] = S;

    return 1;
}

int add_Curve(object * O)
{
    printf("add Curve\n");

    if (O->curve_count >= OBJECT_CURVES || curvesIndex >= CURVES)
    {
        return 0;
    }

    curve * C = malloc(sizeof(curve));
    if (C == NULL)
    {
        return 0;
    }

    initialize_Curve(C);

    O->curves[O->curve_count ++] = C;
    curves[curvesIndex] = C;
    C->index = curvesIndex;
    curvesIndex ++;
    return 1;
}

void free_Curve(curve * C)
{
    free(C->cps); // index in objects cps array
    free(C->cps_continuity); // marks pin_points
    free(C->segments); // index in objects segments array
    free(C);
}

void free_Segments()
{
    int s;

    curve_segment * S;

    for (s = 0; s < segmentIndex; s ++)
    {
        S = segments[s];
        free(S);
    }
}

void free_Cps()
{
    int c;

    cp * CP;

    for (c = 0; c < cpsIndex; c ++)
    {
        CP = cps[c];
        free_Cp(CP);
    }
}

void free_Curves()
{
    int c;

    curve * C;

    for (c = 0; c < curvesIndex; c ++)
    {
        C = curves[c];
        free_Curve(C);
    }
}

void print_Object_Cps(object * O)
{
    int o, c;

    object * O0;

    for (o = 0; o < objectIndex; o ++)
    {
        O0 = objects[o];

        if (O0 == O)
        {
            printf("--");
        }
        printf("%s ", O0->Name);

        for (c = 0; c < O0->cps_count; c ++)
        {
            printf("%d ", O0->cps[c]->index);
        }
        printf("\n");
    }
}

void print_Object_Curves(object * O)
{
    int o, c;

    object * O0;

    for (o = 0; o < objectIndex; o ++)
    {
        O0 = objects[o];

        if (O0 == O)
        {
            printf("--");
        }
        printf("%s ", O0->Name);

        for (c = 0; c < O0->curve_count; c ++)
        {
            printf("%d ", O0->curves[c]->index);
        }
        printf("\n");
    }
}

void update_Curve(curve * C, int level)
{
    fill_Curve_Segments_With_Cp_Coordinates(C);
    fill_Curve_Cps_With_Tangents(C);
    calculate_Curve_Segments(C);

    //fill_Curve_Segments_Tips_With_Cp_Coordinates(C);
    fill_Curve_Segments_With_Coordinates(C, level);
}

void subdivide_Curves(int level)
{
    int c;
    curve * C;

    for (c = 0; c < curvesIndex; c ++)
    {
        C = curves[c];
        subdivide_Curve_Segments(C, level);
    }
}

void update_Curves(int level)
{
    int c;
    curve * C;

    for (c = 0; c < curvesIndex; c ++)
    {
        C = curves[c];
        update_Curve(C, level);
    }
}

void update_selected_Curves(int level)
{
    int c;
    curve * C;

    for (c = 0; c < selected_curves_count; c ++)
    {
        C = curves[selected_curves[c]];
        update_Curve(C, level);
    }
}

void snap_back_Cp_Weights()
{
    printf("\nsnap back Cp Weights\n");

    int c, p;

    cp * CP;
    curve * C;

    for (c = 0; c < selected_curves_count; c ++)
    {
        C = curves[selected_curves[c]];
        for (p = 0; p < C->cps_count; p ++)
        {
            CP = C->cps[p];
            if (CP->selected)
            {
                C->cps_continuity[p] = C->cps_continuity_init[p];
            }
        }
    }
}

void finish_adjusting_Cp_Weights()
{
    printf("\nfinish adjusting Cp Weights\n");

    int c, p;

    cp * CP;
    curve * C;

    for (c = 0; c < selected_curves_count; c ++)
    {
        C = curves[selected_curves[c]];
        for (p = 0; p < C->cps_count; p ++)
        {
            CP = C->cps[p];
            if (CP->selected)
            {
                C->cps_continuity_init[p] = C->cps_continuity[p];
            }
        }
    }
}

void adjust_Selected_Cp_Weights(float w)
{
    int c, p;

    cp * CP;
    curve * C;

    for (c = 0; c < selected_curves_count; c ++)
    {
        C = curves[selected_curves[c]];
        for (p = 0; p < C->cps_count; p ++)
        {
            CP = C->cps[p];
            if (CP->selected)
            {
                C->cps_continuity[p] = C->cps_continuity_init[p] + w;
                if (C->cps_continuity[p] < 0)
                {
                    C->cps_continuity[p] = 0.0;
                }
                else if (C->cps_continuity[p] > 1.0)
                {
                    C->cps_continuity[p] = 1.0;
                }
            }
        }
    }
}

int init_continuity_For_Selected_Curves()
{
    int r = 1;
    int c;

    curve * C;

    for (c = 0; c < selected_curves_count; c ++)
    {
        C = curves[selected_curves[c]];
        C->cps_continuity_init = realloc(C->cps_continuity_init, C->cps_count * sizeof(float));

        if (C->cps_continuity_init != NULL)
        {
            memcpy(C->cps_continuity_init, C->cps_continuity, C->cps_count * sizeof(float));
        }
        else
        {
            r = 0;
            break;
        }
    }
    return r;
}

void clear_Selected_Cp_Weights()
{
    int c, p;

    cp * CP;
    curve * C;

    for (c = 0; c < selected_curves_count; c ++)
    {
        C = curves[selected_curves[c]];
        for (p = 0; p < C->cps_count; p ++)
        {
            CP = C->cps[p];
            if (CP->selected)
            {
                C->cps_continuity[p] = CP_CONTINUITY;
            }
        }
    }
}

void remember_Curves_Cp_pos()
{
    int c, p;

    cp * CP;
    curve * C;

    for (c = 0; c < selected_curves_count; c ++)
    {
        C = curves[selected_curves[c]];
        for (p = 0; p < C->cps_count; p ++)
        {
            CP = C->cps[p];
            CP->Pos[0] = CP->pos[0];
            CP->Pos[1] = CP->pos[1];
            CP->Pos[2] = CP->pos[2];
        }
    }
}

void transfer_pos_To_Cp_Pos()
{
    int c, p;

    cp * CP;
    curve * C;

    for (c = 0; c < selected_curves_count; c ++)
    {
        C = curves[selected_curves[c]];
        for (p = 0; p < C->cps_count; p ++)
        {
            CP = C->cps[p];
            if (CP->selected)
            {
                CP->Pos[0] = CP->pos[0];
                CP->Pos[1] = CP->pos[1];
                CP->Pos[2] = CP->pos[2];
            }
        }
    }
}

void move_Curve_Cps_To_Delta(float Delta[3])
{
    int c, p;

    cp * CP;
    curve * C;

    for (c = 0; c < selected_curves_count; c ++)
    {
        C = curves[selected_curves[c]];
        for (p = 0; p < C->cps_count; p ++)
        {
            CP = C->cps[p];

            CP->pos[0] = CP->Pos[0] + Delta[0];
            CP->pos[1] = CP->Pos[1] + Delta[1];
            CP->pos[2] = CP->Pos[2] + Delta[2];
        }
    }
}

void move_Cps_To_Delta(float Delta[3])
{
    int c, p;

    cp * CP;
    curve * C;

    for (c = 0; c < selected_curves_count; c ++)
    {
        C = curves[selected_curves[c]];
        for (p = 0; p < C->cps_count; p ++)
        {
            CP = C->cps[p];
            if (CP->selected)
            {
                CP->pos[0] = CP->Pos[0] + Delta[0];
                CP->pos[1] = CP->Pos[1] + Delta[1];
                CP->pos[2] = CP->Pos[2] + Delta[2];
            }
        }
    }
}

void snap_back_Curve_Cps_To_Pos()
{
    int c, p;

    cp * CP;
    curve * C;

    for (c = 0; c < selected_curves_count; c ++)
    {
        C = curves[selected_curves[c]];
        for (p = 0; p < C->cps_count; p ++)
        {
            CP = C->cps[p];
            CP->pos[0] = CP->Pos[0];
            CP->pos[1] = CP->Pos[1];
            CP->pos[2] = CP->Pos[2];
        }
    }
}

void snap_back_Cps_To_Pos()
{
    int c, p;

    cp * CP;
    curve * C;

    for (c = 0; c < selected_curves_count; c ++)
    {
        C = curves[selected_curves[c]];
        for (p = 0; p < C->cps_count; p ++)
        {
            CP = C->cps[p];
            if (CP->selected)
            {
                CP->pos[0] = CP->Pos[0];
                CP->pos[1] = CP->Pos[1];
                CP->pos[2] = CP->Pos[2];
            }
        }
    }
}

void find_Cps_action_Center()
{
    int c, p;

    cp * CP;
    curve * C;

    Action_Center->pos[0] = 0;
    Action_Center->pos[1] = 0;
    Action_Center->pos[2] = 0;

    int counter = 0;

    for (c = 0; c < selected_curves_count; c ++)
    {
        C = curves[selected_curves[c]];
        for (p = 0; p < C->cps_count; p ++)
        {
            CP = C->cps[p];
            if (CP->selected)
            {
                counter ++;
                Action_Center->pos[0] += CP->pos[0];
                Action_Center->pos[1] += CP->pos[1];
                Action_Center->pos[2] += CP->pos[2];
            }
        }
    }

    if (counter > 0)
    {
        Action_Center->pos[0] /= counter;
        Action_Center->pos[1] /= counter;
        Action_Center->pos[2] /= counter;
    }
}

void find_Curves_action_Center()
{
    int c, p;

    cp * CP;
    curve * C;

    Action_Center->pos[0] = 0;
    Action_Center->pos[1] = 0;
    Action_Center->pos[2] = 0;

    int counter = 0;

    for (c = 0; c < selected_curves_count; c ++)
    {
        C = curves[selected_curves[c]];
        for (p = 0; p < C->cps_count; p ++)
        {
            CP = C->cps[p];
            counter ++;
            Action_Center->pos[0] += CP->pos[0];
            Action_Center->pos[1] += CP->pos[1];
            Action_Center->pos[2] += CP->pos[2];
        }
    }

    if (counter > 0)
    {
        Action_Center->pos[0] /= counter;
        Action_Center->pos[1] /= counter;
        Action_Center->pos[2] /= counter;
    }
}

void update_Selected_Cps_Positions()
{
    int c, p;

    cp * CP;
    curve * C;

    for (c = 0; c < selected_curves_count; c ++)
    {
        C = curves[selected_curves[c]];
        for (p = 0; p < C->cps_count; p ++)
        {
            CP = C->cps[p];
            if (CP->selected)
            {
                rotate_center(CP->Pos, Action_Center->rotVec, Action_Center->pos, CP->pos);
            }
        }
    }
}

void update_Selected_Curves_Cps_Positions()
{
    int c, p;

    cp * CP;
    curve * C;

    for (c = 0; c < selected_curves_count; c ++)
    {
        C = curves[selected_curves[c]];
        for (p = 0; p < C->cps_count; p ++)
        {
            CP = C->cps[p];
            rotate_center(CP->Pos, Action_Center->rotVec, Action_Center->pos, CP->pos);
        }
    }
}

#endif // CURVES_H_INCLUDED
