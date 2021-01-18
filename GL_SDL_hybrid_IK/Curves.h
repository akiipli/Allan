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

int curvesIndex = 0;
int segmentIndex = 0;
int cpsIndex = 0;
int currentCurve = 0;

float length(float V[3]);
size_t float_3 = sizeof(float[3]);

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

    S->index = segmentIndex;
    segments[segmentIndex ++] = S;

    C->cps = realloc(C->cps, (C->cps_count + 1) * sizeof(cp*));

    if (C->cps == NULL) return 0;

    C->cps[C->cps_count ++] = CP;

    C->cps_continuity = realloc(C->cps_continuity, (C->cps_count) * sizeof(float));

    if (C->cps_continuity == NULL) return 0;

    C->cps_continuity[C->cps_count - 1] = 0.7;

    C->segments = realloc(C->segments, (C->segment_count + 1) * sizeof(curve_segment*));

    if (C->segments == NULL) return 0;

    C->segments[C->segment_count ++] = S;

    //memcpy(CP->pos, (float[3]){1.0, 1.0, 1.0}, float_3);

    memcpy(CP->pos, C->cps[C->cps_count - 2]->pos, float_3);

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

    C->cps = realloc(C->cps, (C->cps_count + 1) * sizeof(cp*));

    if (C->cps == NULL) return 0;

    C->cps[C->cps_count ++] = CP;

    C->cps_continuity = realloc(C->cps_continuity, (C->cps_count) * sizeof(float));

    if (C->cps_continuity == NULL) return 0;

    C->cps_continuity[C->cps_count - 1] = 0.7;

    C->open = open;

    if (segmentIndex >= SEGMENTS)
    {
        return 0;
    }

    curve_segment * S = malloc(sizeof(curve_segment));

    if (S == NULL) return 0;

    S->level = 0;
    S->subdivided = 0;

    S->index = segmentIndex;
    segments[segmentIndex ++] = S;

    C->segments = realloc(C->segments, (C->segment_count + 1) * sizeof(curve_segment*));
    if (C->segments == NULL) return 0;

    C->segments[C->segment_count ++] = S;

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

void update_Curve(curve * C)
{
    fill_Curve_Segments_With_Cp_Coordinates(C);
    fill_Curve_Cps_With_Tangents(C);
    calculate_Curve_Segments(C);
}

#endif // CURVES_H_INCLUDED
