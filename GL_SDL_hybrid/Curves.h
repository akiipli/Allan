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

typedef struct curve curve;
typedef struct curve_segment curve_segment;

struct curve_segment
{
    int index;
    int level;
    int edge; // edge with level
    int subdivided;
    curve * Curve;
    curve_segment * segment[2]; // if subdivided
    float A[3]; // begin
    float B[3]; // end
    float C[3]; // center
};

curve_segment * Segments[SEGMENTS];

struct cp
{
    int index;
    int selected;
    float pos[3];
    vertex * vert;
    curve_segment ** segments;
    int segment_count;
};

cp * Cps[CPS];

struct curve
{
    int index;
    cp ** cps; // index in objects cps array
    int * cps_continuity; // marks pin_points
    int cps_count;
    curve_segment ** segments; // index in objects segments array
    int segment_count;
    int open;
};

curve * Curves[CURVES];

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

void fill_Curve_Segments_With_Cp_Coordinates(curve * C)
{
    int c;

    cp * CP0, * CP1;
    curve_segment * S;

    size_t float_3 = sizeof(float[3]);

    for (c = 1; c < C->cps_count; c ++)
    {
        CP0 = C->cps[c - 1];
        CP1 = C->cps[c];

        S = C->segments[c - 1];

        memcpy(S->A, CP0->pos, float_3);
        memcpy(S->B, CP1->pos, float_3);
        S->C[0] = (S->A[0] + S->B[0]) / 2;
        S->C[1] = (S->A[1] + S->B[1]) / 2;
        S->C[2] = (S->A[2] + S->B[2]) / 2;
    }

    if (C->open == 0)
    {
        CP0 = C->cps[C->cps_count - 1];
        CP1 = C->cps[0];

        S = C->segments[C->segment_count - 1];

        memcpy(S->A, CP0->pos, float_3);
        memcpy(S->B, CP1->pos, float_3);
        S->C[0] = (S->A[0] + S->B[0]) / 2;
        S->C[1] = (S->A[1] + S->B[1]) / 2;
        S->C[2] = (S->A[2] + S->B[2]) / 2;
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
    C->cps_continuity = realloc(C->cps_continuity, C->cps_count * sizeof(int));
    if (C->cps == NULL)
    {
        C->cps_count --;
        return 0;
    }
    C->cps[C->cps_count - 1] = CP;
    C->cps_continuity[C->cps_count - 1] = 1;

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

    O->cps[O->cps_count ++] = cpsIndex;
    Cps[cpsIndex] = CP;
    CP->index = cpsIndex;
    cpsIndex ++;
    return 1;
}

void initialize_Curve(curve * C)
{
    C->cps = malloc(0 * sizeof(cp*)); // index in objects cps array
    C->cps_continuity = malloc(0 * sizeof(int*)); // marks pin_points
    C->cps_count = 0;
    C->segments = malloc(0 * sizeof(curve_segment*)); // index in objects segments array
    C->segment_count = 0;
    C->open = 0;
}

void free_Cp(cp * CP)
{
    free(CP->segments);
    free(CP);
}

void remove_Cp(object * O, int index)
{
    printf("remove Cp\n");

    int o, c, idx;
    int condition = 0;

    object * O0;
    curve * C;

    if (index < cpsIndex)
    {
        cp * CP = Cps[index];

        for (c = 0; c < O->cps_count; c ++)
        {
            if (O->cps[c] == index)
            {
                idx = c;
                condition = 1;
                break;
            }
        }

        if (condition)
        {
            O->cps_count --;

            for (c = idx; c < O->cps_count; c ++)
            {
                O->cps[c] = O->cps[c + 1];
            }

            for (o = 0; o < objectIndex; o ++)
            {
                O0 = objects[o];

                for (c = 0; c < O0->curve_count; c ++)
                {
                    C = Curves[O0->curves[c]];
                    remove_CP_From_Curve(C, CP);
                }

                for (c = 0; c < O0->cps_count; c ++)
                {
                    if (O0->cps[c] >= index)
                    {
                        O0->cps[c] --;
                    }
                }
            }

            cpsIndex --;

            for (c = index; c < cpsIndex; c ++)
            {
                Cps[c] = Cps[c + 1];
                Cps[c]->index = c;
            }

            free(CP); // make free_Cp
        }
    }
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

    O->curves[O->curve_count ++] = curvesIndex;
    Curves[curvesIndex] = C;
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

void remove_Curve(object * O, int index)
{
    printf("remove Curve\n");

    int o, c, idx;
    int condition = 0;

    object * O0;

    if (index < curvesIndex)
    {
        curve * C = Curves[index];

        for (c = 0; c < O->curve_count; c ++)
        {
            if (O->curves[c] == index)
            {
                idx = c;
                condition = 1;
                break;
            }
        }

        if (condition)
        {
            O->curve_count --;

            for (c = idx; c < O->curve_count; c ++)
            {
                O->curves[c] = O->curves[c + 1];
            }

            for (o = 0; o < objectIndex; o ++)
            {
                O0 = objects[o];
                for (c = 0; c < O0->curve_count; c ++)
                {
                    if (O0->curves[c] >= index)
                    {
                        O0->curves[c] --;
                    }
                }
            }

            curvesIndex --;

            for (c = index; c < curvesIndex; c ++)
            {
                Curves[c] = Curves[c + 1];
                Curves[c]->index = c;
            }

            free_Curve(C);
        }
    }
}

void free_Segments()
{
    int s;

    curve_segment * S;

    for (s = 0; s < segmentIndex; s ++)
    {
        S = Segments[s];
        free(S);
    }
}

void free_Cps()
{
    int c;

    cp * CP;

    for (c = 0; c < cpsIndex; c ++)
    {
        CP = Cps[c];
        free_Cp(CP);
    }
}

void free_Curves()
{
    int c;

    curve * C;

    for (c = 0; c < curvesIndex; c ++)
    {
        C = Curves[c];
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
            printf("%d ", Cps[O0->cps[c]]->index);
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
            printf("%d ", Curves[O0->curves[c]]->index);
        }
        printf("\n");
    }
}

#endif // CURVES_H_INCLUDED
