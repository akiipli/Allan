/*
The MIT License

Copyright <2018><2021> <Allan Kiipli>
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

Deleting a curve is not deleting segments from segments array,
but rather replacing them with null pointer.

Because cps can connect to multiple curves,
i do not delete them now.
I could not determine the connection.
*/

#ifndef CURVES_H_INCLUDED
#define CURVES_H_INCLUDED

#define CURVES 10000
#define SEGMENTS 100000
#define CPS 200000
#define SELEDGES 1000

#define OBJECT_CURVES 1000
#define OBJECT_CPS 10000
#define OBJECT_SEGMENTS 10000

#define CP_CONTINUITY 0.5

int selected_curves[CURVES];
int selected_curves_count = 0;
int selected_curves0[CURVES];
int selected_curves_count0 = 0;
int connected_curves[CURVES];
int connected_curves_count = 0;
int connected_objects[OBJECTS];
int connected_objects_count = 0;
int curvesIndex = 0;
int segmentIndex = 0;
int selected_cps[CPS];
int selected_cps_count = 0;
int selected_cps0[CPS];
int selected_cps_count0 = 0;

int selected_segments[SEGMENTS];
int selected_segments_count = 0;
int selected_segments0[SEGMENTS];
int selected_segments_count0 = 0;

int cpsIndex = 0;
int currentCurve = 0;
int currentCp = 0;
int currentSegment = 0;

vertex * selected_verts[CPS];
int selected_verts_count = 0;
vertex * selected_verts0[CPS];
int selected_verts_count0 = 0;
vertex * currentVert;

edge * selected_edges[SELEDGES];
int selected_edges_count = 0;
edge * selected_edges0[SELEDGES];
int selected_edges_count0 = 0;
edge * currentEdge;

int cp_Manipulation = 0;
int curve_Manipulation = 0;
int rotate_Curves_With_Object = 0;
int update_Curve_Objects = 0;

float length(float V[3]);
size_t float_3 = sizeof(float[3]);

float cp_continuity[SUBD];

void init_cp_continuity()
{
    int l;
    float v = 0.2; // for subdivided segments it is safe to use A and C points in Segments.
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
    edge * E; // edge with level
    int subdivided;
    curve * Curve;
    curve_segment * segment[2]; // if subdivided
    float A[3]; // begin
    float B[3]; // center
    float C[3]; // end
    int counter_edge;
    float weight;
    float weight_init;
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
    object * O;
    int visible;
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
    S->level = -1;
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

    //printf("\r %d %d\t", level, S->level);

    if (level > S->level && S->level > -1) // level 0 is filled with Coordinates and Tangents in update Curve, also segments
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

    //printf("\r %d %d %d %d %d\t", S0->subdivided, S->subdivided, S1->subdivided, level, S->level);

    if (S0->subdivided && S->subdivided && S1->subdivided && level >= S->level)
    {
        S_0 = S0->segment[1];
        S_1 = S->segment[0];
        S_2 = S->segment[1];
        S_3 = S1->segment[0];

        memcpy(S_1->A, S->A, float_3);
        memcpy(S_1->C, S->B, float_3);

        memcpy(S_2->A, S->B, float_3);
        memcpy(S_2->C, S->C, float_3);

        if (S->level == -1)
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

        //printf("\r%d %d\t", level, S->level);

        if (level > S->level)
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

void subdivide_Curve_Segment(curve_segment * S, int level, int counter, object * O, edge * E)
{
    //printf("\r%d %d %d\t", S->subdivided, level, S->level);

    if (!S->subdivided && level > S->level)
    {
        int idx;
        edge * E0, * E1;
        curve_segment * S0, * S1;

        S0 = malloc(sizeof(curve_segment));
        S1 = malloc(sizeof(curve_segment));

        if (S0 && S1)
        {
            S0->index = segmentIndex;
            S0->level = S->level + 1;
            S0->subdivided = 0;
            S0->selected = S->selected;
            segments[segmentIndex ++] = S0;
            S0->E = NULL;
            S0->Curve = S->Curve;
            S0->weight = S->weight;

            S1->index = segmentIndex;
            S1->level = S->level + 1;
            S1->subdivided = 0;
            S1->selected = S->selected;
            segments[segmentIndex ++] = S1;
            S1->E = NULL;
            S1->Curve = S->Curve;
            S1->weight = S->weight;

            S->subdivided = 1;
            S->segment[0] = S0;
            S->segment[1] = S1;

            S0->counter_edge = 0;
            S1->counter_edge = 0;

//            object * O2 = O;
//            edge * E2 = E;
//            edge * E3 = S->E;
//            int SS = E->subdivs;

            if (O != NULL && E != NULL && S->E != NULL && E->subdivs)
            {
                /* assign subdivided edges to subdivided segments */
                /* detect if edge runs against curve */

                idx = E->edges[0];
                E0 = &O->edges_[S->level + 1][idx / ARRAYSIZE][idx % ARRAYSIZE];
                idx = E->edges[1];
                E1 = &O->edges_[S->level + 1][idx / ARRAYSIZE][idx % ARRAYSIZE];

                if (counter)
                {
                    S0->counter_edge = 1;
                    S1->counter_edge = 1;
                    S0->E = E1;
                    E1->S = S0;
                    S1->E = E0;
                    E0->S = S1;
                }
                else
                {
                    S0->counter_edge = 0;
                    S1->counter_edge = 0;
                    S0->E = E0;
                    E0->S = S0;
                    S1->E = E1;
                    E1->S = S1;
                }
            }
        }
    }

    if (S->subdivided && level > S->level + 1)
    {
        subdivide_Curve_Segment(S->segment[0], level, counter, O, S->segment[0]->E);
        subdivide_Curve_Segment(S->segment[1], level, counter, O, S->segment[1]->E);
    }
}

void subdivide_Curve_Segments(curve * C, int level)
{
    int s, idx, counter;

    cp * CP;
    edge * E = NULL;
    vertex * V;
    curve_segment * S;

    object * O = C->O;

    if (O != NULL && level > O->subdlevel)
    {
        level = O->subdlevel;
    }

    for (s = 0; s < C->segment_count; s ++)
    {
        S = C->segments[s];
        CP = C->cps[s];

        counter = 0;

//        object * O1 = C->O;
//        edge * E1 = S->E;
//        cp * CP1 = CP->vert;

        if (C->O != NULL && S->E != NULL && CP->vert != NULL)
        {
            E = S->E;

            if (E->subdivs)
            {
                idx = E->verts[0];
                V = &C->O->verts[idx / ARRAYSIZE][idx % ARRAYSIZE];
                if (V == CP->vert)
                {
                    S->counter_edge = 0;
                    counter = 0;
                }
                else
                {
                    S->counter_edge = 1;
                    counter = 1;
                }
            }
        }
        else
        {
            E = NULL;
        }
        subdivide_Curve_Segment(S, level, counter, C->O, E);
    }
}

void scan_for_Object_Patches(object * O, int level)
{
    int q, l, v, p, e, idx;

    edge * E;
    polygon * P;
    quadrant * Q, * Q0;
    vertex * V, * V0;

    if (level > O->subdlevel)
    {
        level = O->subdlevel;
    }

    if (level >= 0)
    {
        for (l = 0; l <= level; l ++)
        {
            for (v = 0; v < O->vertcount_[l]; v ++)
            {
                V = &O->verts_[l][v / ARRAYSIZE][v % ARRAYSIZE];
                V->patch = 0;
            }
        }

        for (l = 0; l <= level; l ++)
        {
            for (e = 0; e < O->edgecount_[l]; e ++)
            {
                E = &O->edges_[l][e / ARRAYSIZE][e % ARRAYSIZE];
                E->patch = 0;
            }
        }

        int v_start = O->vertcount + O->edgecount;
        int v_start0 = O->vertcount_[0] + O->edgecount_[0];

        for (p = 0; p < O->polycount; p ++)
        {
            P = &O->polys[p / ARRAYSIZE][p % ARRAYSIZE];
            V = &O->verts_[0][(p + v_start) / ARRAYSIZE][(p + v_start) % ARRAYSIZE];

            P->patch = 0;

            for (e = 0; e < P->edgecount; e ++)
            {
                idx = P->edges[e];
                E = &O->edges[idx / ARRAYSIZE][idx % ARRAYSIZE];

                if (E->S != NULL)
                {
                    V->patch ++;
                    //break;
                }
            }

            if (V->patch >= P->edgecount) // / 2)
            {
                V->patch = 1;
            }
            else
            {
                V->patch = 0;
            }

            if (V->patch)
            {
                P->patch = 1;

                for (e = 0; e < P->edgecount; e ++)
                {
                    idx = P->quads[e];

                    Q = &O->quads_[0][idx / ARRAYSIZE][idx % ARRAYSIZE];

                    if (Q->subdivs)
                    {
                        idx = v_start0 + Q->index;
                        V0 = &O->verts_[1][idx / ARRAYSIZE][idx % ARRAYSIZE];
                        V0->patch = 1;
                    }

                    idx = P->edges[e];

                    E = &O->edges[idx / ARRAYSIZE][idx % ARRAYSIZE];
                    E->patch = 1;

                    idx = V->edges[e];

                    E = &O->edges_[0][idx / ARRAYSIZE][idx % ARRAYSIZE];
                    E->patch = 1;
                }
            }
        }

        for (l = 0; l < level; l ++)
        {
            v_start = O->vertcount_[l] + O->edgecount_[l];
            v_start0 = O->vertcount_[l + 1] + O->edgecount_[l + 1];

            for (q = 0; q < O->quadcount_[l]; q ++)
            {
                Q = &O->quads_[l][q / ARRAYSIZE][q % ARRAYSIZE];
                V = &O->verts_[l + 1][(q + v_start) / ARRAYSIZE][(q + v_start) % ARRAYSIZE];

                Q->patch = 0;

                if (V->patch)
                {
                    Q->patch = 1;

                    for (e = 0; e < 4; e ++)
                    {
                        idx = Q->quads[e];

                        Q0 = &O->quads_[l + 1][idx / ARRAYSIZE][idx % ARRAYSIZE];

                        if (Q0->subdivs)
                        {
                            idx = v_start0 + Q0->index;
                            V0 = &O->verts_[l + 2][idx / ARRAYSIZE][idx % ARRAYSIZE];
                            V0->patch = 1;
                        }

                        idx = Q->edges[e];

                        E = &O->edges_[l][idx / ARRAYSIZE][idx % ARRAYSIZE];
                        E->patch = 1;

                        idx = V->edges[e];

                        E = &O->edges_[l + 1][idx / ARRAYSIZE][idx % ARRAYSIZE];
                        E->patch = 1;
                    }
                }
            }
        }
    }
}

void scan_for_Objects_Patches(int level)
{
    int o;

    object * O;

    for (o = 0; o < objectIndex; o ++)
    {
        O = objects[o];

        if (O->curve_count > 0)
            scan_for_Object_Patches(O, level);
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
    C->cps_continuity[C->cps_count - 1] = CP_CONTINUITY;

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
    C->O = NULL;
}

void free_Cp(cp * CP)
{
    free(CP->segments);
    free(CP);
}

int add_Curve_Segment_To_Verts(curve * C, vertex * V, object * O)
{
    if (cpsIndex >= CPS)
    {
        return 0;
    }

    if (segmentIndex >= SEGMENTS)
    {
        return 0;
    }

    cp * CP;

    if (V->control_point != NULL)
    {
        CP = V->control_point;
    }
    else
    {
        CP = malloc(sizeof(cp));

        if (CP == NULL) return 0;

        CP->index = cpsIndex;
        cps[cpsIndex ++] = CP;
        CP->segments = NULL;

        V->control_point = CP;
        CP->vert = V;

        CP->pos[0] = V->Tx;
        CP->pos[1] = V->Ty;
        CP->pos[2] = V->Tz;
    }

    curve_segment * S;

    if (V->control_point != NULL)
    {
        S = segments[segmentIndex - 1];
    }
    else
    {
        if (segmentIndex >= SEGMENTS)
        {
            return 0;
        }

        S = malloc(sizeof(curve_segment));

        if (S == NULL) return 0;

        S->level = -1;
        S->subdivided = 0;
        S->selected = 0;
        S->E = NULL;
        S->index = segmentIndex;
        segments[segmentIndex ++] = S;
        S->weight = 0.0;
        S->weight_init = 0.0;
    }

    C->cps = realloc(C->cps, (C->cps_count + 1) * sizeof(cp*));

    if (C->cps == NULL) return 0;

    C->cps[C->cps_count ++] = CP;

    C->cps_continuity = realloc(C->cps_continuity, (C->cps_count) * sizeof(float));

    if (C->cps_continuity == NULL) return 0;

    C->cps_continuity[C->cps_count - 1] = CP_CONTINUITY;

    C->segments = realloc(C->segments, (C->segment_count + 1) * sizeof(curve_segment*));

    if (C->segments == NULL) return 0;

    C->segments[C->segment_count ++] = S;
    S->Curve = C;

    //memcpy(CP->pos, (float[3]){1.0, 1.0, 1.0}, float_3);

    if (C->cps[C->cps_count - 2]->vert == NULL)
    {
        C->cps[C->cps_count - 2]->segments[1] = S;
        C->cps[C->cps_count - 2]->segment_count = 2;
    }

    if (V->control_point != NULL)
    {
        if (CP->segments == NULL)
            CP->segments = malloc(V->edgecount * sizeof(curve_segment*));
        else
            CP->segments = realloc(CP->segments, V->edgecount * sizeof(curve_segment*));
        CP->segment_count = V->edgecount;

        int e, idx;
        edge * E;

        for (e = 0; e < V->edgecount; e ++)
        {
            idx = V->edges[e];
            E = &O->edges[idx / ARRAYSIZE][idx % ARRAYSIZE];
            CP->segments[e] = E->S;
        }
    }
    else
    {
        CP->segments = malloc(2 * sizeof(curve_segment *));
        CP->segment_count = 1;
        CP->segments[0] = S;
    }

    CP->selected = 0;

    return 1;
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
    CP->segments = NULL;
    CP->vert = NULL;

    curve_segment * S = malloc(sizeof(curve_segment));

    if (S == NULL) return 0;

    S->level = -1;
    S->subdivided = 0;
    S->selected = 0;
    S->E = NULL;
    S->index = segmentIndex;
    segments[segmentIndex ++] = S;
    S->weight = 0.0;
    S->weight_init = 0.0;

    C->cps = realloc(C->cps, (C->cps_count + 1) * sizeof(cp*));

    if (C->cps == NULL) return 0;

    C->cps[C->cps_count ++] = CP;

    C->cps_continuity = realloc(C->cps_continuity, (C->cps_count) * sizeof(float));

    if (C->cps_continuity == NULL) return 0;

    C->cps_continuity[C->cps_count - 1] = CP_CONTINUITY;

    C->segments = realloc(C->segments, (C->segment_count + 1) * sizeof(curve_segment*));

    if (C->segments == NULL) return 0;

    C->segments[C->segment_count ++] = S;
    S->Curve = C;

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

int add_New_Curve_To_Verts(float pos[3], int open, vertex * V, object * O)
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
    C->visible = 1;

    if (O->curve_count < OBJECT_CURVES)
    {
        O->curves[O->curve_count ++] = C;
    }

    C->O = O;

    if (cpsIndex >= CPS)
    {
        return 0;
    }

    cp * CP;

    if (V->control_point != NULL)
    {
        CP = V->control_point;
    }
    else
    {
        CP = malloc(sizeof(cp));

        if (CP == NULL) return 0;

        CP->index = cpsIndex;
        cps[cpsIndex ++] = CP;
        CP->segments = NULL;

        memcpy(CP->pos, pos, float_3);

        V->control_point = CP;
        CP->vert = V;
    }

    CP->selected = 0;

    C->cps = realloc(C->cps, (C->cps_count + 1) * sizeof(cp*));

    if (C->cps == NULL) return 0;

    C->cps[C->cps_count ++] = CP;

    C->cps_continuity = realloc(C->cps_continuity, (C->cps_count) * sizeof(float));

    if (C->cps_continuity == NULL) return 0;

    C->cps_continuity[C->cps_count - 1] = CP_CONTINUITY;

    C->open = open;

    curve_segment * S;

    if (V->control_point != NULL)
    {
        S = segments[segmentIndex - 1];
    }
    else
    {
        if (segmentIndex >= SEGMENTS)
        {
            return 0;
        }

        S = malloc(sizeof(curve_segment));

        if (S == NULL) return 0;

        S->level = -1;
        S->subdivided = 0;
        S->selected = 0;
        S->E = NULL;
        S->index = segmentIndex;
        segments[segmentIndex ++] = S;
        S->weight = 0.0;
        S->weight_init = 0.0;
    }

    C->segments = realloc(C->segments, (C->segment_count + 1) * sizeof(curve_segment*));
    if (C->segments == NULL) return 0;

    C->segments[C->segment_count ++] = S;
    S->Curve = C;

    if (V->control_point != NULL)
    {
        if (CP->segments == NULL)
            CP->segments = malloc(V->edgecount * sizeof(curve_segment*));
        else
            CP->segments = realloc(CP->segments, V->edgecount * sizeof(curve_segment*));
        CP->segment_count = V->edgecount;

        int e, idx;
        edge * E;

        for (e = 0; e < V->edgecount; e ++)
        {
            idx = V->edges[e];
            E = &O->edges[idx / ARRAYSIZE][idx % ARRAYSIZE];
            CP->segments[e] = E->S;
        }
    }
    else
    {
        CP->segments = malloc(2 * sizeof(curve_segment *));
        CP->segment_count = 1;
        CP->segments[0] = S;
    }

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
    C->visible = 1;

    if (cpsIndex >= CPS)
    {
        return 0;
    }

    cp * CP = malloc(sizeof(cp));

    if (CP == NULL) return 0;

    CP->index = cpsIndex;
    cps[cpsIndex ++] = CP;
    CP->segments = NULL;
    CP->vert = NULL;

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

    S->level = -1;
    S->subdivided = 0;
    S->selected = 0;
    S->E = NULL;
    S->index = segmentIndex;
    segments[segmentIndex ++] = S;
    S->weight = 0.0;
    S->weight_init = 0.0;

    C->segments = realloc(C->segments, (C->segment_count + 1) * sizeof(curve_segment*));
    if (C->segments == NULL) return 0;

    C->segments[C->segment_count ++] = S;
    S->Curve = C;

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
    C->O = O;
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
        if (S != NULL)
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
        if (CP != NULL)
        {
            if (CP->segments != NULL)
                free(CP->segments);
            free_Cp(CP);
        }
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

void find_connected_Curves()
{
    int s, c, p, t, condition;

    cp * CP;
    curve * C, * C0;
    curve_segment * S;

    connected_curves_count = 0;

    for (c = 0; c < selected_curves_count; c ++)
    {
        C = curves[selected_curves[c]];

        for (p = 0; p < C->cps_count; p ++)
        {
            CP = C->cps[p];

            for (s = 0; s < CP->segment_count; s ++)
            {
                S = CP->segments[s];

                condition = 1;

                if (S == NULL)
                {

                }
                else if (S->Curve != NULL && S->Curve->selected == 0)
                {
                    for (t = 0; t < connected_curves_count; t ++)
                    {
                        C0 = curves[connected_curves[t]];
                        if (C0 == S->Curve)
                        {
                            condition = 0;
                            break;
                        }
                    }
                    if (condition)
                        connected_curves[connected_curves_count ++] = S->Curve->index;
                }
            }
        }
    }
}

void find_connected_Objects()
{
    int o, c, condition;

    curve * C;
    object * O;

    connected_objects_count = 0;

    for (c = 0; c < selected_curves_count; c ++)
    {
        C = curves[selected_curves[c]];

        if (C->O != NULL)
        {
            condition = 1;

            for (o = 0; o < connected_objects_count; o ++)
            {
                O = objects[connected_objects[o]];
                if (C->O == O)
                {
                    condition = 0;
                    break;
                }
            }
            if (condition)
                connected_objects[connected_objects_count ++] = C->O->index;
        }
    }
}

void update_connected_Curves(int level)
{
    int c;

    curve * C;

    for (c = 0; c < connected_curves_count; c ++)
    {
        C = curves[connected_curves[c]];
        update_Curve(C, level);
    }
}

void update_object_Curves(object * O, int level)
{
    int c;
    curve * C;

    for (c = 0; c < O->curve_count; c ++)
    {
        C = O->curves[c];
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

void assign_Segment_Weight_Recursive(curve_segment * S)
{
    curve_segment * S0, * S1;

    if (S->subdivided)
    {
        S0 = S->segment[0];
        S0->weight = S->weight;
        S1 = S->segment[1];
        S1->weight = S->weight;
        assign_Segment_Weight_Recursive(S0);
        assign_Segment_Weight_Recursive(S1);
    }
}

void snap_back_Curve_Weights()
{
    printf("\nsnap back Curve Weights\n");

    int c, s;

    curve_segment * S;
    curve * C;

    for (c = 0; c < selected_curves_count; c ++)
    {
        C = curves[selected_curves[c]];
        for (s = 0; s < C->segment_count; s ++)
        {
            S = C->segments[s];
            S->weight = S->weight_init;
            if (S->subdivided)
                assign_Segment_Weight_Recursive(S);
        }
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

void finish_adjusting_Curve_Weights()
{
    printf("\nfinish adjusting Curve Weights\n");

    int c, s;

    curve_segment * S;
    curve * C;

    for (c = 0; c < selected_curves_count; c ++)
    {
        C = curves[selected_curves[c]];
        for (s = 0; s < C->segment_count; s ++)
        {
            S = C->segments[s];
            S->weight_init = S->weight;
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

void adjust_Selected_Curve_Weights(float w)
{
    int c, s;

    curve_segment * S;
    curve * C;

    for (c = 0; c < selected_curves_count; c ++)
    {
        C = curves[selected_curves[c]];
        for (s = 0; s < C->segment_count; s ++)
        {
            S = C->segments[s];
            S->weight = S->weight_init + w;

            if (S->weight < 0)
            {
                S->weight = 0.0;
            }
            else if (S->weight > 1.0)
            {
                S->weight = 1.0;
            }

            assign_Segment_Weight_Recursive(S);
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

void clear_Selected_Segments_Weights()
{
    int c, s;

    curve_segment * S;
    curve * C;

    for (c = 0; c < selected_curves_count; c ++)
    {
        C = curves[selected_curves[c]];
        for (s = 0; s < C->segment_count; s ++)
        {
            S = C->segments[s];

            if (S->selected)
            {
                S->weight = 0.0;
                S->weight_init = 0.0;
                assign_Segment_Weight_Recursive(S);
            }
        }
    }
}

void clear_Selected_Curves_Weights()
{
    int c, s;

    curve_segment * S;
    curve * C;

    for (c = 0; c < selected_curves_count; c ++)
    {
        C = curves[selected_curves[c]];
        for (s = 0; s < C->segment_count; s ++)
        {
            S = C->segments[s];
            S->weight = 0.0;
            S->weight_init = 0.0;
            assign_Segment_Weight_Recursive(S);
        }
    }
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

void transfer_Delta_To_Object_Cps(object * O, float Delta[3])
{
    int c, p;

    cp * CP;
    curve * C;

    for (c = 0; c < O->curve_count; c ++)
    {
        C = O->curves[c];
        for (p = 0; p < C->cps_count; p ++)
        {
            CP = C->cps[p];
            CP->pos[0] = CP->Pos[0] + Delta[0];
            CP->pos[1] = CP->Pos[1] + Delta[1];
            CP->pos[2] = CP->Pos[2] + Delta[2];
        }
    }
}

void remember_Object_Curves_pos(object * O)
{
    int c, p;

    cp * CP;
    curve * C;

    for (c = 0; c < O->curve_count; c ++)
    {
        C = O->curves[c];
        for (p = 0; p < C->cps_count; p ++)
        {
            CP = C->cps[p];
            CP->Pos[0] = CP->pos[0];
            CP->Pos[1] = CP->pos[1];
            CP->Pos[2] = CP->pos[2];
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

void snap_back_Object_Cps_To_Pos(object * O)
{
    int c, p;

    cp * CP;
    curve * C;

    for (c = 0; c < O->curve_count; c ++)
    {
        C = O->curves[c];
        for (p = 0; p < C->cps_count; p ++)
        {
            CP = C->cps[p];

            CP->pos[0] = CP->Pos[0];
            CP->pos[1] = CP->Pos[1];
            CP->pos[2] = CP->Pos[2];

//            if (CP->vert != NULL)
//            {
//                CP->vert->Rx = CP->pos[0];
//                CP->vert->Ry = CP->pos[1];
//                CP->vert->Rz = CP->pos[2];
//            }
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

void update_Object_Curves_Cps_Positions(object * O)
{
    int c, p;

    cp * CP;
    curve * C;

    for (c = 0; c < O->curve_count; c ++)
    {
        C = O->curves[c];
        for (p = 0; p < C->cps_count; p ++)
        {
            CP = C->cps[p];
            //rotate_center(CP->Pos, Action_Center->rotVec, Action_Center->pos, CP->pos);
            if (CP->vert != NULL)
            {
                CP->pos[0] = CP->vert->Tx;
                CP->pos[1] = CP->vert->Ty;
                CP->pos[2] = CP->vert->Tz;
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

int create_Object_Curve(object * O)
{
    printf("create Object Curve\n");

    /* collect selected verts and connecting E0 edges */

    int v, e, idx, n, s, r;

    r = 0;

    edge * E, * E0;
    vertex * V, * V0, * V1;

    vertex ** V_candidates = malloc(selected_verts_count * sizeof(vertex*));
    edge ** E_candidates = malloc(selected_verts_count * sizeof(edge*));

    if (V_candidates == NULL || E_candidates == NULL)
    {
        return 0;
    }

    int canditates_count = 0;
    int curve_closed = 0;

    V1 = selected_verts[0];

    for (v = 0; v < selected_verts_count; v ++)
    {
        V0 = selected_verts[v];
        E0 = NULL;

        if (V0->O == O)
        {
            for (e = 0; e < V0->edgecount; e ++)
            {
                idx = V0->edges[e];
                E = &O->edges[idx / ARRAYSIZE][idx % ARRAYSIZE];

                if (O->selected_edges_count > 0)
                {
                    if (E->selected && (E != E_candidates[0]))
                    {
                        if (E->verts[0] == V0->index)
                        {
                            n = E->verts[1];
                        }
                        else if (E->verts[1] == V0->index)
                        {
                            n = E->verts[0];
                        }
                        if (n == selected_verts[(v + 1) % selected_verts_count]->index)
                        {
                            E0 = E;
                            break;
                        }
                    }
                }
                else
                {
                    if (E != E_candidates[0])
                    {
                        if (E->verts[0] == V0->index)
                        {
                            n = E->verts[1];
                        }
                        else if (E->verts[1] == V0->index)
                        {
                            n = E->verts[0];
                        }
                        if (n == selected_verts[(v + 1) % selected_verts_count]->index)
                        {
                            E0 = E;
                            break;
                        }
                    }
                }
            }
        }

        if (E0 == NULL) // one selected edge in row
        {
            for (e = 0; e < V0->edgecount; e ++)
            {
                idx = V0->edges[e];
                E = &O->edges[idx / ARRAYSIZE][idx % ARRAYSIZE];

                if (E->verts[0] == V0->index)
                {
                    n = E->verts[1];
                }
                else if (E->verts[1] == V0->index)
                {
                    n = E->verts[0];
                }
                if (n == V1->index)
                {
                    E0 = E;
                    break;
                }
            }
        }

        if (E0 == NULL || E0->S != NULL)
        {
            break;
        }
        else
        {
            V_candidates[canditates_count] = V0;
            E_candidates[canditates_count ++] = E0;
            if (v == selected_verts_count - 1 || n == V1->index)
            {
                curve_closed = 1;
                printf("Curve closed\n");
                break;
            }
        }
    }

    if (canditates_count > 1)
    {
        printf("Curve validates %d\n", curve_closed);

        float pos[3];

        V = V_candidates[0];
        E = E_candidates[0];

        pos[0] = V->Tx;
        pos[1] = V->Ty;
        pos[2] = V->Tz;

        for (e = 0; e < V->edgecount; e ++)
        {
            idx = V->edges[e];
            E0 = &O->edges[idx / ARRAYSIZE][idx % ARRAYSIZE];
            if (E == E0)
            {
                if (segmentIndex >= SEGMENTS)
                {
                    return 0;
                }

                curve_segment * S = malloc(sizeof(curve_segment));

                if (S == NULL) return 0;

                S->level = -1;
                S->subdivided = 0;
                S->selected = 0;
                S->index = segmentIndex;
                segments[segmentIndex ++] = S;
                S->weight = 0.0;
                S->weight_init = 0.0;

                E->S = S;
                S->E = E;
            }
        }

        r = add_New_Curve_To_Verts(pos, !curve_closed, V_candidates[0], O);

        if (r)
        {
            V_candidates[0]->selected = 0; // drop selection

            curve * C = curves[curvesIndex - 1];

            for (s = 1; s < canditates_count; s ++)
            {
                V = V_candidates[s];
                V->selected = 0; // drop selection
                E = E_candidates[s];

                for (e = 0; e < V->edgecount; e ++)
                {
                    idx = V->edges[e];
                    E0 = &O->edges[idx / ARRAYSIZE][idx % ARRAYSIZE];
                    if (E == E0)
                    {
                        if (segmentIndex >= SEGMENTS)
                        {
                            return 0;
                        }

                        curve_segment * S = malloc(sizeof(curve_segment));

                        if (S == NULL) return 0;

                        S->level = -1;
                        S->subdivided = 0;
                        S->selected = 0;
                        S->index = segmentIndex;
                        segments[segmentIndex ++] = S;
                        S->weight = 0.0;
                        S->weight_init = 0.0;

                        E->S = S;
                        S->E = E;
                    }
                }

                r = add_Curve_Segment_To_Verts(C, V, O);
            }

            if (!curve_closed)
            {
                V = V0;
                V->selected = 0; // drop selection

                if (segmentIndex >= SEGMENTS)
                {
                    return 0;
                }

                curve_segment * S = malloc(sizeof(curve_segment));

                S->E = NULL; // find this edge

                if (S == NULL) return 0;

                S->level = -1;
                S->subdivided = 0;
                S->selected = 0;
                S->index = segmentIndex;
                segments[segmentIndex ++] = S;
                S->weight = 0.0;
                S->weight_init = 0.0;

                r = add_Curve_Segment_To_Verts(C, V, O);
                if (r)
                {
                    pos[0] = V->Tx;
                    pos[1] = V->Ty;
                    pos[2] = V->Tz;

                    memcpy(C->cps[C->cps_count - 1]->pos, pos, sizeof(float[3]));
                }
            }

            printf("canditates count %d\n", canditates_count);
            printf("curve cps count %d\n", C->cps_count);

            if (canditates_count == 2)
            {
                C->open = 1;
                printf("Curve is open\n");
            }

            /* drop candidate vertexes */
            /* associate segments with edges */
            /* for each vertex solve potential cp problem if it has another curve there */
        }
    }

    free(V_candidates);
    free(E_candidates);

    return r;
}

void select_CP_Segments_Edges(cp * CP, int select)
{
    int s;

    curve_segment * S;

    for (s = 0; s < CP->segment_count; s ++)
    {
        S = CP->segments[s];

        if (S != NULL && S->E != NULL)
        {
            S->E->selected = select;
        }
    }
}

direction_Pack length_AB(float A[3], float B[3]);

int add_Next_Edge_To_Selected(object * O)
{
    printf("add Next Edge To Selected\n");

    int e, condition, idx, idx0, idx1;

    edge * E, * E0, * E1, * E2;

    vertex * V0, * V1, * V2;

    float pos0[3];
    float pos1[3];

    direction_Pack P0, P1;

    float angle, angle_max;

    angle_max = -1;

    if (selected_edges_count < 2)
    {
        return 0;
    }

    E0 = selected_edges[selected_edges_count - 2];
    E1 = selected_edges[selected_edges_count - 1];

    if (E0->O != O || E1->O != O)
    {
        return 0;
    }

    /* find the utter vertex in E1 */

    E2 = NULL;
    condition = 1;

    while (condition)
    {
        condition = 0;

        if (E0->verts[1] == E1->verts[0] || E0->verts[0] == E1->verts[0])
        {
            idx0 = E1->verts[0];
            idx1 = E1->verts[1];
            condition = 1;
        }
        else if (E0->verts[1] == E1->verts[1] || E0->verts[0] == E1->verts[1])
        {
            idx0 = E1->verts[1];
            idx1 = E1->verts[0];
            condition = 1;
        }

        if (condition)
        {
            condition = 0;
            V0 = &O->verts[idx0 / ARRAYSIZE][idx0 % ARRAYSIZE];
            V1 = &O->verts[idx1 / ARRAYSIZE][idx1 % ARRAYSIZE];

            /* find best angle */

            pos0[0] = V0->x;
            pos0[1] = V0->y;
            pos0[2] = V0->z;

            pos1[0] = V1->x;
            pos1[1] = V1->y;
            pos1[2] = V1->z;

            P0 = length_AB(pos0, pos1);

            for (e = 0; e < V1->edgecount; e ++)
            {
                idx = V1->edges[e];
                E = &O->edges[idx / ARRAYSIZE][idx % ARRAYSIZE];

                if (E->polys[0] == E1->polys[0]
                 || E->polys[0] == E1->polys[1]
                 || E->polys[1] == E1->polys[0]
                 || E->polys[1] == E1->polys[1])
                {
                    continue;
                }

                if (E->verts[0] == V1->index)
                {
                    idx = E->verts[1];
                }
                else
                {
                    idx = E->verts[0];
                }

                V2 = &O->verts[idx / ARRAYSIZE][idx % ARRAYSIZE];

                pos0[0] = V1->x;
                pos0[1] = V1->y;
                pos0[2] = V1->z;

                pos1[0] = V2->x;
                pos1[1] = V2->y;
                pos1[2] = V2->z;

                P1 = length_AB(pos0, pos1);

                angle = dot_productFF(P0.vec, P1.vec);
                if (angle >= angle_max)
                {
                    E2 = E;
                    angle_max = angle;
                }
            }

            if (E2 != NULL && !E2->selected)
            {
                E2->selected = 1;
                if (selected_edges_count < SELEDGES)
                    selected_edges[selected_edges_count ++] = E2;
            }
        }
    }

    return 1;
}

int add_Next_Vertex_To_Selected(object * O)
{
    int r = 0;

    int e, idx;

    vertex * V0, * V1, * N0, * N1, * N2;
    edge * E, * E0;

    direction_Pack P0, P1;

    float pos0[3];
    float pos1[3];

    V0 = selected_verts[selected_verts_count - 2];
    V1 = selected_verts[selected_verts_count - 1];

    N0 = NULL;
    N2 = NULL;

    float angle, angle_max;

    angle_max = -1;

    for (e = 0; e < V0->edgecount; e ++)
    {
        idx = V0->edges[e];
        E = &O->edges[idx / ARRAYSIZE][idx % ARRAYSIZE];

        if (E->verts[0] == V0->index)
        {
            if (E->verts[1] == V1->index)
            {
                pos0[0] = V0->x;
                pos0[1] = V0->y;
                pos0[2] = V0->z;

                pos1[0] = V1->x;
                pos1[1] = V1->y;
                pos1[2] = V1->z;

                P0 = length_AB(pos0, pos1);

                N0 = V1;
                E0 = E;

                break;
            }
        }
        else if (E->verts[1] == V0->index)
        {
            if (E->verts[0] == V1->index)
            {
                pos0[0] = V0->x;
                pos0[1] = V0->y;
                pos0[2] = V0->z;

                pos1[0] = V1->x;
                pos1[1] = V1->y;
                pos1[2] = V1->z;

                P0 = length_AB(pos0, pos1);

                N0 = V1;
                E0 = E;

                break;
            }
        }
    }

    if (N0 != NULL)
    {
        for (e = 0; e < N0->edgecount; e ++)
        {
            idx = N0->edges[e];
            E = &O->edges[idx / ARRAYSIZE][idx % ARRAYSIZE];

            if (E != E0)
            {
                if (E->verts[0] == N0->index)
                {
                    idx = E->verts[1];
                }
                else
                {
                    idx = E->verts[0];
                }

                N1 = &O->verts[idx / ARRAYSIZE][idx % ARRAYSIZE];

                pos0[0] = N0->x;
                pos0[1] = N0->y;
                pos0[2] = N0->z;

                pos1[0] = N1->x;
                pos1[1] = N1->y;
                pos1[2] = N1->z;

                P1 = length_AB(pos0, pos1);

                angle = dot_productFF(P0.vec, P1.vec);
                if (angle >= angle_max)
                {
                    N2 = N1;
                    angle_max = angle;
                }
            }
        }

        if (N2 != NULL)
        {
            N2->selected = 1;
            selected_verts[selected_verts_count ++] = N2;

            r = 1;
        }
    }

    return r;
}

void clean_Edge_Segment_Recursive(object * O, edge * E, int L, int l)
{
    if (L > l)
    {

    }
    else
    {
        if (E->S != NULL)
        {
            E->S->E = NULL;
            E->S = NULL;
        }

        L ++;
        if (L >= SUBD)
        {

        }
        else
        {
            int idx;

            edge * E0;

            if (E->subdivs)
            {
                idx = E->edges[0];
                E0 = &O->edges_[L][idx / ARRAYSIZE][idx % ARRAYSIZE];
                clean_Edge_Segment_Recursive(O, E0, L, l);
                idx = E->edges[1];
                E0 = &O->edges_[L][idx / ARRAYSIZE][idx % ARRAYSIZE];
                clean_Edge_Segment_Recursive(O, E0, L, l);
            }
        }
    }
}

void disconnect_Curve_Form_Object(curve * C)
{
    int p, s;

    curve_segment * S;
    cp * CP;

    for (p = 0; p < C->cps_count; p ++)
    {
        CP = C->cps[p];
        CP->vert = NULL;
    }

    for (s = 0; s < C->segment_count; s ++)
    {
        S = C->segments[s];
        S->E = NULL;
    }
}

void clean_Object_Curves(object * O)
{
    printf("clean Object Curves\n");

    O->curve_count = 0;

    int e, v, c;

    curve * C;
    edge * E;
    vertex * V;

    for (e = 0; e < O->edgecount; e ++)
    {
        E = &O->edges[e / ARRAYSIZE][e % ARRAYSIZE];
        if (E->S != NULL)
        {
            clean_Edge_Segment_Recursive(O, E, 0, O->subdlevel);
        }
    }

    for (v = 0; v < O->vertcount; v ++)
    {
        V = &O->verts[v / ARRAYSIZE][v % ARRAYSIZE];
        V->control_point = NULL;
    }

    for (c = 0; c < O->curve_count; c ++)
    {
        C = O->curves[c];
        disconnect_Curve_Form_Object(C);
    }
}

void delete_Segment_Recursive(curve_segment * S)
{
    if (S->subdivided)
    {
        if (S->segment[0])
            delete_Segment_Recursive(S->segment[0]);
        if (S->segment[1])
            delete_Segment_Recursive(S->segment[1]);
    }

    segments[S->index] = NULL;
    free(S);
}

void erase_Vertex_Patch_Indicators(object * O)
{
    int l, v;

    vertex * V;

    for (l = 0; l <= O->subdlevel; l ++)
    {
        for (v = 0; v < O->vertcount_[l]; v ++)
        {
            V = &O->verts_[l][v / ARRAYSIZE][v % ARRAYSIZE];
            V->patch = 0;
        }
    }
}

void delete_Curve(curve * C)
{
    int c, s, p;

    object * O = C->O;
    curve_segment * S;
    cp * CP;

    int condition, index;

    if (O != NULL)
    {
        for (s = 0; s < C->segment_count; s ++)
        {
            S = C->segments[s];

            if (S->E != NULL)
            {
                clean_Edge_Segment_Recursive(O, S->E, -1, O->subdlevel);
            }
        }

        for (p = 0; p < C->cps_count; p ++)
        {
            CP = C->cps[p];

            if (CP->vert != NULL)
            {
                condition = 1;

                for (s = 0; s < CP->segment_count; s ++)
                {
                    S = CP->segments[s];
                    if (S != NULL && S->E != NULL)
                    {
                        condition = 0;
                        break;
                    }
                }

                if (condition)
                {
                    CP->vert->control_point = NULL;
                }
            }
        }

        condition = 0;

        for (c = 0; c < O->curve_count; c ++)
        {
            if (O->curves[c] == C)
            {
                index = c;
                condition = 1;
                break;
            }
        }

        if (condition)
        {
            O->curve_count --;

            for (c = index; c < O->curve_count; c ++)
            {
                O->curves[c] = O->curves[c + 1];
            }
        }

        //erase_Vertex_Patch_Indicators(O);
    }

    /* segmentIndex is not changed */

    for (s = 0; s < C->segment_count; s ++)
    {
        S = C->segments[s];

        delete_Segment_Recursive(S);
    }

    condition = 0;

    for (c = 0; c < curvesIndex; c ++)
    {
        if (curves[c] == C)
        {
            index = c;
            condition = 1;
            break;
        }
    }

    if (condition)
    {
        int c0;

        for (c = selected_curves_count - 1; c >= 0; c --)
        {
            if (selected_curves[c] == index)
            {
                selected_curves_count --;

                for (c0 = c; c0 < selected_curves_count; c0 ++)
                {
                    selected_curves[c0] = selected_curves[c0 + 1];
                }
                break;
            }
        }

        curvesIndex --;

        for (c = index; c < curvesIndex; c ++)
        {
            curves[c] = curves[c + 1];
            curves[c]->index = c;
        }

        free_Curve(C);
    }
}

void print_Curve_subdLevel(curve * C)
{
    printf("starting segments ");
    curve_segment * S = C->segments[0];

    while(S->subdivided)
    {
        printf("%d ", S->level);
        S = S->segment[0];
    }
    printf("\n");
}

void update_Objects_Curves_To_Original_Coordinates(object * O)
{
    int c, p;
    curve * C;
    cp * CP;

    for (c = 0; c < O->curve_count; c ++)
    {
        C = O->curves[c];

        for (p = 0; p < C->cps_count; p ++)
        {
            CP = C->cps[p];
            if (CP->vert != NULL)
            {
                CP->pos[0] = CP->vert->x;
                CP->pos[1] = CP->vert->y;
                CP->pos[2] = CP->vert->z;
            }
        }
    }
}

void update_Objects_Curves_Coordinates(object * O)
{
    int c, p;
    curve * C;
    cp * CP;

    for (c = 0; c < O->curve_count; c ++)
    {
        C = O->curves[c];

        for (p = 0; p < C->cps_count; p ++)
        {
            CP = C->cps[p];
            if (CP->vert != NULL)
            {
                CP->pos[0] = CP->vert->Tx;
                CP->pos[1] = CP->vert->Ty;
                CP->pos[2] = CP->vert->Tz;
            }
        }
    }
}

void hide_Object_Curves(object * O)
{
    int c;
    curve * C;

    for (c = 0; c < O->curve_count; c ++)
    {
        C = O->curves[c];
        C->visible = 0;
    }
}

void unhide_Object_Curves(object * O)
{
    int c;
    curve * C;

    for (c = 0; c < O->curve_count; c ++)
    {
        C = O->curves[c];
        C->visible = 1;
    }
}

void transfer_Curve_Cps_To_Vertex_Coordinates(object * O)
{
    int c, p;
    curve * C;
    cp * CP;

    for (c = 0; c < O->curve_count; c ++)
    {
        C = O->curves[c];

        for (p = 0; p < C->cps_count; p ++)
        {
            CP = C->cps[p];

            if (CP->vert != NULL)
            {
                CP->vert->x = CP->pos[0];
                CP->vert->y = CP->pos[1];
                CP->vert->z = CP->pos[2];

                CP->vert->Rx = CP->pos[0];
                CP->vert->Ry = CP->pos[1];
                CP->vert->Rz = CP->pos[2];
            }
        }
    }
}

void transfer_Curve_Cps_To_Vertex_Coordinates_ROT(object * O)
{
    int c, p;
    curve * C;
    cp * CP;

    direction D;

    float x, y, z;

    for (c = 0; c < O->curve_count; c ++)
    {
        C = O->curves[c];

        for (p = 0; p < C->cps_count; p ++)
        {
            CP = C->cps[p];

            if (CP->vert != NULL)
            {
                x = CP->pos[0] - O->T->pos[0];
                y = CP->pos[1] - O->T->pos[1];
                z = CP->pos[2] - O->T->pos[2];

                rotate_Vector_I(O->T, x, y, z, &D);

                CP->vert->x = D.x;
                CP->vert->y = D.y;
                CP->vert->z = D.z;

                CP->vert->Rx = CP->vert->x;
                CP->vert->Ry = CP->vert->y;
                CP->vert->Rz = CP->vert->z;

                //*/

                CP->vert->Tx = CP->pos[0];
                CP->vert->Ty = CP->pos[1];
                CP->vert->Tz = CP->pos[2];
            }
        }
    }
}

void transfer_Curve_Cps_To_Vertex_TCoordinates(object * O)
{
    int c, p;
    curve * C;
    cp * CP;

    for (c = 0; c < O->curve_count; c ++)
    {
        C = O->curves[c];

        for (p = 0; p < C->cps_count; p ++)
        {
            CP = C->cps[p];


            if (CP->vert != NULL)
            {
                CP->vert->x = CP->pos[0] - O->T->pos[0];
                CP->vert->y = CP->pos[1] - O->T->pos[1];
                CP->vert->z = CP->pos[2] - O->T->pos[2];

                ///* Morph coordinates

                CP->vert->Rx = CP->vert->x;
                CP->vert->Ry = CP->vert->y;
                CP->vert->Rz = CP->vert->z;

                //*/

                CP->vert->Tx = CP->vert->x;
                CP->vert->Ty = CP->vert->y;
                CP->vert->Tz = CP->vert->z;
            }
        }
    }
}

void update_connected_Objects()
{
    int o;

    object * O;

    for (o = 0; o < connected_objects_count; o ++)
    {
        O = objects[connected_objects[o]];
        if (1)//(!O->binding)
        {
            if (O->T->rot[0] == 0 && O->T->rot[1] == 0 && O->T->rot[2] == 0)
            {
                transfer_Curve_Cps_To_Vertex_TCoordinates(O);
            }
        }
    }
}

void update_connected_Objects_ROT()
{
    int o;

    object * O;

    for (o = 0; o < connected_objects_count; o ++)
    {
        O = objects[connected_objects[o]];

        transfer_Curve_Cps_To_Vertex_Coordinates_ROT(O);
    }
}

void delete_Object_Curves(object * O)
{
    int c;
    curve * C;

    for (c = O->curve_count - 1; c >= 0; c --)
    {
        C = O->curves[c];
        delete_Curve(C);
    }
}

void convert_To_Cp_Selection(curve * C)
{
    int p;

    cp * CP;

    for (p = 0; p < C->cps_count; p ++)
    {
        CP = C->cps[p];
        if(CP->selected == 0)
        {
            CP->selected = 1;
            if (CP->vert != NULL)
            {
                CP->vert->selected = 1;
            }
            if (selected_cps_count < CPS)
            {
                selected_cps[selected_cps_count ++] = CP->index;
            }
        }
    }
}

void convert_Segments_To_Cp_Selection()
{
    int s, idx;

    object * O;
    curve_segment * S;
    edge * E;
    vertex * V;
    cp * CP;

    selected_cps_count = 0;

    for (s = 0; s < selected_segments_count; s ++)
    {
        S = segments[selected_segments[s]];
        if (S->E != NULL)
        {
            E = S->E;
            O = E->O;
            idx = E->verts[0];
            V = &O->verts[idx / ARRAYSIZE][idx % ARRAYSIZE];
            if (V->control_point != NULL)
            {
                CP = V->control_point;
                if(CP->selected == 0)
                {
                    CP->selected = 1;
                    if (selected_cps_count < CPS)
                    {
                        selected_cps[selected_cps_count ++] = CP->index;
                    }
                }
            }
            idx = E->verts[1];
            V = &O->verts[idx / ARRAYSIZE][idx % ARRAYSIZE];
            if (V->control_point != NULL)
            {
                CP = V->control_point;
                if(CP->selected == 0)
                {
                    CP->selected = 1;
                    if (selected_cps_count < CPS)
                    {
                        selected_cps[selected_cps_count ++] = CP->index;
                    }
                }
            }
        }
    }
}

void convert_Curves_To_Cp_Selection()
{
    int c;

    curve * C;

    selected_cps_count = 0;

    for (c = 0; c < curvesIndex; c ++)
    {
        C = curves[c];
        if (C->selected)
        {
            convert_To_Cp_Selection(C);
        }
    }
}

void toggle_Selected_Edges_Patch_Mode(object * O)
{
    int e;

    edge * E;

    for (e = 0; e < O->edgecount; e ++)
    {
        E = &O->edges[e / ARRAYSIZE][e % ARRAYSIZE];

        if (E->selected)
        {
            E->patch = !E->patch;
        }
    }
}

void clear_Selected_Edges_Patch_Mode(object * O)
{
    int e;

    edge * E;

    for (e = 0; e < O->edgecount; e ++)
    {
        E = &O->edges[e / ARRAYSIZE][e % ARRAYSIZE];

        if (E->selected)
        {
            E->patch = 0;
        }
    }
}

void assign_Segment_Selection_Recursive(curve_segment * S, int selected)
{
    if (S->subdivided)
    {
        assign_Segment_Selection_Recursive(S->segment[0], selected);
        assign_Segment_Selection_Recursive(S->segment[1], selected);
    }
}

void assign_Selection_To_Subsegments()
{
    int s;
    curve_segment * S;

    for (s = 0; s < selected_segments_count; s ++)
    {
        S = segments[selected_segments[s]];

        assign_Segment_Selection_Recursive(S, S->selected);
    }
}

#endif // CURVES_H_INCLUDED
