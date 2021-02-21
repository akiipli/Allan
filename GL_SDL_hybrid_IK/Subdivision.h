/*
The MIT License

Copyright <2018> <Allan Kiipli>
*/

#ifndef SUBDIVISION_H_INCLUDED
#define SUBDIVISION_H_INCLUDED

//#include "Object.h"
//#include "ExtensionFunctions.h"

// all O counts in polylevels are changed in quadlevel function to find [L-1].

// TODO
// When animating, shapes deform.
// For deforming geometry, subdivision needs tune in shape based on transformed (T)
// bounding boxes and coordinates. This for 3d tuning only. UVs stay undeformed.
// Cage is deformed and vertexes update transformed coordinates.

int curve_subdiv = 1;
float edge_divisor = 1.0;

void tune_In_Subdivision_Shape_uvtex(object * O);
int tune_In_Subdivision_Shape_uvtex_(object * O, int L);

void update_bounding_box_for_UV_Edges_(object * O, int L, unsigned char radius);
void update_2d_bounding_box_for_Quads(object * O, int L, unsigned char radius);

void update_bounding_box_for_selected_UV_EdgesQuads(object * O, int L, unsigned char radius);
void generate_cage_for_Subdivision_Quads_UV(object * O, int L);

float min_dist = 0.001;

void clear_edge_Weights_Recursive_(object * O, edge * E, int L, float w)
{
    int idx;
    edge * E0;
    vertex * V;

    int l = L + 1;

    E->weight = w;
    E->weight_init = w;

    idx = E->verts[0];
    V = &O->verts_[L][idx / ARRAYSIZE][idx % ARRAYSIZE];
    V->weight = w;
    V->weight_init = w;
    idx = E->verts[1];
    V = &O->verts_[L][idx / ARRAYSIZE][idx % ARRAYSIZE];
    V->weight = w;
    V->weight_init = w;

    if (E->subdivs)
    {
        idx = E->edges[0];
        E0 = &O->edges_[l][idx / ARRAYSIZE][idx % ARRAYSIZE];

        clear_edge_Weights_Recursive_(O, E0, l, w);

        idx = E->edges[1];
        E0 = &O->edges_[l][idx / ARRAYSIZE][idx % ARRAYSIZE];

        clear_edge_Weights_Recursive_(O, E0, l, w);
    }
}

void assign_edge_Weights_Recursive_(object * O, edge * E, float w, int L)
{
    int idx;
    edge * E0;
    vertex * V;

    int l = L + 1;

    E->weight = E->weight_init + w;

    if (E->weight > 1) E->weight = 1;
    else if (E->weight < -1) E->weight = -1;

    idx = E->verts[0];
    V = &O->verts_[L][idx / ARRAYSIZE][idx % ARRAYSIZE];
    V->weight = V->weight_init + w;
    if (V->weight > 1) V->weight = 1;
    else if (V->weight < -1) V->weight = -1;
    idx = E->verts[1];
    V = &O->verts_[L][idx / ARRAYSIZE][idx % ARRAYSIZE];
    V->weight = V->weight_init + w;
    if (V->weight > 1) V->weight = 1;
    else if (V->weight < -1) V->weight = -1;

    if (E->subdivs)
    {
        idx = E->edges[0];
        E0 = &O->edges_[l][idx / ARRAYSIZE][idx % ARRAYSIZE];

        assign_edge_Weights_Recursive_(O, E0, w, l);

        idx = E->edges[1];
        E0 = &O->edges_[l][idx / ARRAYSIZE][idx % ARRAYSIZE];

        assign_edge_Weights_Recursive_(O, E0, w, l);
    }
}

void assign_edge_Weights_Recursive(object * O, edge * E, float w)
{
    int idx;
    edge * E0;
    vertex * V;

    int l = 0;

    E->weight = E->weight_init + w;
    if (E->weight > 1) E->weight = 1;
    else if (E->weight < -1) E->weight = -1;

    idx = E->verts[0];
    V = &O->verts[idx / ARRAYSIZE][idx % ARRAYSIZE];
    V->weight = V->weight_init + w;
    if (V->weight > 1) V->weight = 1;
    else if (V->weight < -1) V->weight = -1;
    idx = E->verts[1];
    V = &O->verts[idx / ARRAYSIZE][idx % ARRAYSIZE];
    V->weight = V->weight_init + w;
    if (V->weight > 1) V->weight = 1;
    else if (V->weight < -1) V->weight = -1;

    if (E->subdivs)
    {
        idx = E->edges[0];
        E0 = &O->edges_[l][idx / ARRAYSIZE][idx % ARRAYSIZE];

        assign_edge_Weights_Recursive_(O, E0, w, l);

        idx = E->edges[1];
        E0 = &O->edges_[l][idx / ARRAYSIZE][idx % ARRAYSIZE];

        assign_edge_Weights_Recursive_(O, E0, w, l);
    }
}

void clear_edge_Weights_Recursive(object * O, edge * E, float w)
{
    int idx;
    edge * E0;
    vertex * V;

    int l = 0;

    E->weight = w;
    E->weight_init = w;

    idx = E->verts[0];
    V = &O->verts[idx / ARRAYSIZE][idx % ARRAYSIZE];
    V->weight = w;
    V->weight_init = w;
    idx = E->verts[1];
    V = &O->verts[idx / ARRAYSIZE][idx % ARRAYSIZE];
    V->weight = w;
    V->weight_init = w;

    if (E->subdivs)
    {
        idx = E->edges[0];
        E0 = &O->edges_[l][idx / ARRAYSIZE][idx % ARRAYSIZE];

        clear_edge_Weights_Recursive_(O, E0, l, w);

        idx = E->edges[1];
        E0 = &O->edges_[l][idx / ARRAYSIZE][idx % ARRAYSIZE];

        clear_edge_Weights_Recursive_(O, E0, l, w);
    }
}

void clear_Selected_Edges_Weights()
{
    printf("clear Selected Edges Weights\n");
    int l, idx, e, o;

    edge * E;
    object * O;

    for (o = 0; o < selected_object_count; o ++)
    {
        O = objects[selected_objects[o]];

        for (e = 0; e < O->selected_edges_count; e ++)
        {
            idx = O->selected_edges[e];
            E = &O->edges[idx / ARRAYSIZE][idx % ARRAYSIZE];

            clear_edge_Weights_Recursive(O, E, 0);
        }

        if (O->subdlevel > -1)
        {
            tune_In_Subdivision_Shape_uvtex(O);
        }

        for (l = 0; l <= O->subdlevel; l ++)
        {
            update_bounding_box_for_selected_UV_EdgesQuads(O, l, 0);
            //generate_cage_for_Subdivision_Quads_UV(O, l);
            tune_In_Subdivision_Shape_uvtex_(O, l);
        }
    }
}

void adjust_Selected_Edges_Weights(float w)
{
    printf("\radjust %f", w);
    int idx, e, o;

    edge * E;
    object * O;

    for (o = 0; o < selected_object_count; o ++)
    {
        O = objects[selected_objects[o]];

        for (e = 0; e < O->selected_edges_count; e ++)
        {
            idx = O->selected_edges[e];
            E = &O->edges[idx / ARRAYSIZE][idx % ARRAYSIZE];

            assign_edge_Weights_Recursive(O, E, w);
        }
    }
}

void snap_back_Verts_Weights()
{
    printf("\nsnap back Verts Weights\n");
    int l, idx, v, o;

    vertex * V;
    object * O;

    for (o = 0; o < selected_object_count; o ++)
    {
        O = objects[selected_objects[o]];
        for (v = 0; v < O->selected_verts_count; v ++)
        {
            idx = O->selected_verts[v];
            V = &O->verts[idx / ARRAYSIZE][idx % ARRAYSIZE];
            while (V != NULL)
            {
                V->weight = V->weight_init;
                V = V->vert;
            }
        }


        if (O->subdlevel > -1)
        {
            tune_In_Subdivision_Shape_uvtex(O);
        }

        for (l = 0; l <= O->subdlevel; l ++)
        {
            update_bounding_box_for_selected_UV_EdgesQuads(O, l, 0);
            //generate_cage_for_Subdivision_Quads_UV(O, l);
            tune_In_Subdivision_Shape_uvtex_(O, l);
        }
    }
}

void adjust_Selected_Verts_Weights(float w, int L)
{
    printf("\radjust %f", w);
    int l, idx, v, o;

    vertex * V;
    object * O;

    for (o = 0; o < selected_object_count; o ++)
    {
        O = objects[selected_objects[o]];

        for (v = 0; v < O->selected_verts_count; v ++)
        {
            idx = O->selected_verts[v];
            V = &O->verts[idx / ARRAYSIZE][idx % ARRAYSIZE];
            while (V != NULL)
            {
                V->weight = V->weight_init + w;
                if (V->weight < -1)
                    V->weight = -1;
                V = V->vert;
            }
        }

        if (O->subdlevel > -1)
        {
            tune_In_Subdivision_Shape_uvtex(O);
        }

        if (L >= 0 && O->subdlevel >= L)
        {
            for (l = 0; l <= L; l ++)
            {
                update_bounding_box_for_selected_UV_EdgesQuads(O, l, 0);
                //generate_cage_for_Subdivision_Quads_UV(O, l);
                tune_In_Subdivision_Shape_uvtex_(O, l);
            }
        }
    }
}

void  assign_Init_Weight_Recursive_(object * O, edge * E, int L)
{
    int idx;
    edge * E0;
    vertex * V;

    int l = L + 1;

    E->weight = E->weight_init;

    idx = E->verts[0];
    V = &O->verts_[L][idx / ARRAYSIZE][idx % ARRAYSIZE];
    V->weight = V->weight_init;
    idx = E->verts[1];
    V = &O->verts_[L][idx / ARRAYSIZE][idx % ARRAYSIZE];
    V->weight = V->weight_init;

    if (E->subdivs)
    {
        idx = E->edges[0];
        E0 = &O->edges_[l][idx / ARRAYSIZE][idx % ARRAYSIZE];

        assign_Init_Weight_Recursive_(O, E0, l);

        idx = E->edges[1];
        E0 = &O->edges_[l][idx / ARRAYSIZE][idx % ARRAYSIZE];

        assign_Init_Weight_Recursive_(O, E0, l);
    }
}

void assign_Init_Weight_Recursive(object * O, edge * E)
{
    int idx;
    edge * E0;
    vertex * V;

    int l = 0;

    E->weight = E->weight_init;

    idx = E->verts[0];
    V = &O->verts[idx / ARRAYSIZE][idx % ARRAYSIZE];
    V->weight = V->weight_init;
    idx = E->verts[1];
    V = &O->verts[idx / ARRAYSIZE][idx % ARRAYSIZE];
    V->weight = V->weight_init;

    if (E->subdivs)
    {
        idx = E->edges[0];
        E0 = &O->edges_[l][idx / ARRAYSIZE][idx % ARRAYSIZE];

        assign_Init_Weight_Recursive_(O, E0, l);

        idx = E->edges[1];
        E0 = &O->edges_[l][idx / ARRAYSIZE][idx % ARRAYSIZE];

        assign_Init_Weight_Recursive_(O, E0, l);
    }
}

void snap_back_Edges_Weights()
{
    printf("\nsnap back Edges Weights\n");
    int idx, e, o;

    edge * E;
    object * O;

    for (o = 0; o < selected_object_count; o ++)
    {
        O = objects[selected_objects[o]];
        for (e = 0; e < O->selected_edges_count; e ++)
        {
            idx = O->selected_edges[e];
            E = &O->edges[idx / ARRAYSIZE][idx % ARRAYSIZE];

            assign_Init_Weight_Recursive(O, E);
        }
    }
}

void  finish_edge_Weights_Recursive_(object * O, edge * E, int L)
{
    int idx;
    edge * E0;
    vertex * V;

    int l = L + 1;

    E->weight_init = E->weight;

    idx = E->verts[0];
    V = &O->verts_[L][idx / ARRAYSIZE][idx % ARRAYSIZE];
    V->weight_init = V->weight;

    idx = E->verts[1];
    V = &O->verts_[L][idx / ARRAYSIZE][idx % ARRAYSIZE];
    V->weight_init = V->weight;

    if (E->subdivs)
    {
        idx = E->edges[0];
        E0 = &O->edges_[l][idx / ARRAYSIZE][idx % ARRAYSIZE];

        finish_edge_Weights_Recursive_(O, E0, l);

        idx = E->edges[1];
        E0 = &O->edges_[l][idx / ARRAYSIZE][idx % ARRAYSIZE];

        finish_edge_Weights_Recursive_(O, E0, l);
    }
}

void  finish_edge_Weights_Recursive(object * O, edge * E)
{
    int idx;
    edge * E0;
    vertex * V;

    int l = 0;

    E->weight_init = E->weight;

    idx = E->verts[0];
    V = &O->verts[idx / ARRAYSIZE][idx % ARRAYSIZE];
    V->weight_init = V->weight;

    idx = E->verts[1];
    V = &O->verts[idx / ARRAYSIZE][idx % ARRAYSIZE];
    V->weight_init = V->weight;

    if (E->subdivs)
    {
        idx = E->edges[0];
        E0 = &O->edges_[l][idx / ARRAYSIZE][idx % ARRAYSIZE];

        finish_edge_Weights_Recursive_(O, E0, l);

        idx = E->edges[1];
        E0 = &O->edges_[l][idx / ARRAYSIZE][idx % ARRAYSIZE];

        finish_edge_Weights_Recursive_(O, E0, l);
    }
}

void finish_adjusting_Edges_Weights()
{
    printf("\nfinish adjusting Edges Weights\n");
    int idx, e, o;

    edge * E;
    object * O;

    for (o = 0; o < selected_object_count; o ++)
    {
        O = objects[selected_objects[o]];
        for (e = 0; e < O->selected_edges_count; e ++)
        {
            idx = O->selected_edges[e];
            E = &O->edges[idx / ARRAYSIZE][idx % ARRAYSIZE];

            finish_edge_Weights_Recursive(O, E);
        }
    }
}

void finish_adjusting_Verts_Weights()
{
    printf("\nfinish adjusting Verts Weights\n");
    int l, idx, v, o;

    vertex * V;
    object * O;

    for (o = 0; o < selected_object_count; o ++)
    {
        O = objects[selected_objects[o]];
        for (v = 0; v < O->selected_verts_count; v ++)
        {
            idx = O->selected_verts[v];
            V = &O->verts[idx / ARRAYSIZE][idx % ARRAYSIZE];
            while (V != NULL)
            {
                V->weight_init = V->weight;
                V = V->vert;
            }
        }

        if (O->subdlevel > -1)
        {
            tune_In_Subdivision_Shape_uvtex(O);
        }

        for (l = 0; l <= O->subdlevel; l ++)
        {
            update_bounding_box_for_selected_UV_EdgesQuads(O, l, 0);
            //generate_cage_for_Subdivision_Quads_UV(O, l);
            tune_In_Subdivision_Shape_uvtex_(O, l);
        }
    }
}

int tune_In_Subdivision_Shape_uvtex_(object * O, int L)
{
    //O->vertcount_[L] = O->vertcount_[L1] + O->edgecount_[L1] + O->quadcount_[L1];

    if (L < 1 || L > SUBD) return 0;

    int L1 = L - 1;

    //O->vertcount_[L] = O->vertcount + O->edgecount + O->polycount;

    int v, e, idx;

    quadrant * Q0, * Q1;
    uv * UV, * UV0;
    uv_edge * E;

    edge * E0;

    vertex * V0;

    int c_v = O->textcount_[L1];

    float Mu, Mv;

    for (e = 0; e < O->uvedcount_[L1]; e ++) // edge verts average surrounding polys and self position
    {
        E = &O->uveds_[L1][e / ARRAYSIZE][e % ARRAYSIZE];
        UV = &O->uvtex_[L][(e + c_v) / ARRAYSIZE][(e + c_v) % ARRAYSIZE]; // edge vertex

        E0 = &O->edges[E->edge / ARRAYSIZE][E->edge % ARRAYSIZE];

        if (E->polycount > 1)
        {
            idx = E->polys[0];
            Q0 = &O->quads_[L1][idx / ARRAYSIZE][idx % ARRAYSIZE];
            idx = E->polys[1];
            Q1 = &O->quads_[L1][idx / ARRAYSIZE][idx % ARRAYSIZE];

            Mu = (Q0->B2.u + Q1->B2.u) / 2.0;
            Mv = (Q0->B2.v + Q1->B2.v) / 2.0;

            UV->u = (Mu + E->B.u) / 2.0;
            UV->v = (Mv + E->B.v) / 2.0;

            if (E0->weight != 0)
            {
                UV->u = E->B.u * E0->weight + UV->u * (1 - E0->weight);
                UV->v = E->B.v * E0->weight + UV->v * (1 - E0->weight);

                E->Mu = E->B.u * E0->weight + Mu * (1 - E0->weight);
                E->Mv = E->B.v * E0->weight + Mv * (1 - E0->weight);
            }
            else
            {
                E->Mu = Mu;
                E->Mv = Mv;
            }
        }
        else
        {
            E->Mu = E->B.u;
            E->Mv = E->B.v;
        }
    }

    float Fu, Fv;
    float Eu, Ev;
    float U, V;
    float Eu_o, Ev_o;
    float Eu_w, Ev_w;
    float edgecount;
    int openedge, split;
    int edgeweight;

    for (v = 0; v < O->textcount_[L1]; v ++) // cage verts
    {
        UV = &O->uvtex_[L][v / ARRAYSIZE][v % ARRAYSIZE]; // new level 0 cage vertexes
        UV0 = &O->uvtex_[L1][v / ARRAYSIZE][v % ARRAYSIZE]; // old level -1 cage vertexes

        V0 = &O->verts_[L1][UV0->vert / ARRAYSIZE][UV0->vert % ARRAYSIZE];

        // first we scan surroundings and find distance to it as collapsed

        Fu = Fv = 0.0;
        Eu = Ev = 0.0;
        Eu_o = Ev_o = 0.0;
        Eu_w = Ev_w = 0.0;

        openedge = 0;
        split = 0;
        edgeweight = 0;

        // create logic for open edges

        edgecount = UV0->uvcount;

        for(e = 0; e < UV0->uvcount; e ++)
        {
            idx = UV0->uv_edges[e];
            E = &O->uveds_[L1][idx / ARRAYSIZE][idx % ARRAYSIZE];

            E0 = &O->edges[E->edge / ARRAYSIZE][E->edge % ARRAYSIZE];

            if (E->split)
            {
                split ++;
            }
            if (E->polycount == 1)
            {
                openedge ++;
                Eu_o += E->B.u;
                Ev_o += E->B.v;
            }
            else if (E0->weight != 0)
            {
                edgeweight ++;
                Eu_w += E->B.u;
                Ev_w += E->B.v;
            }

            Fu += E->Mu;
            Fv += E->Mv;

            Eu += E->B.u;
            Ev += E->B.v;
        }

        if (split == edgecount ||  split > 3 || (split && V0->edgecount != 4))
        {
            /* smooth split edges that have flow */
        }
        else if (openedge)
        {
            Eu_o /= openedge;
            Ev_o /= openedge;

            UV->u = (Eu_o + UV0->u) / 2.0;
            UV->v = (Ev_o + UV0->v) / 2.0;
        }
        else if (edgeweight)
        {
            Eu_w /= edgeweight;
            Ev_w /= edgeweight;

            UV->u = (Eu_w + UV0->u) / 2.0;
            UV->v = (Ev_w + UV0->v) / 2.0;
        }
        else
        {
            Fu /= edgecount;
            Fv /= edgecount;

            Eu /= edgecount;
            Ev /= edgecount;

            UV->u = (Fu + 2.0 * Eu + (UV->uvcount - 3) * UV0->u) / edgecount;
            UV->v = (Fv + 2.0 * Ev + (UV->uvcount - 3) * UV0->v) / edgecount;
        }
        if (!openedge && (V0->weight != 0) && edgeweight)
        {
            Fu /= edgecount;
            Fv /= edgecount;

            Eu /= edgecount;
            Ev /= edgecount;

            U = (Fu + 2.0 * Eu + (UV->uvcount - 3) * UV0->u) / edgecount;
            V = (Fv + 2.0 * Ev + (UV->uvcount - 3) * UV0->v) / edgecount;

            UV->u = UV->u * V0->weight + U * (1 - V0->weight);
            UV->v = UV->v * V0->weight + V * (1 - V0->weight);
        }
    }

    return 1;
}

void tune_In_Subdivision_Shape_uvtex(object * O)
{

    //O->vertcount_[L] = O->vertcount + O->edgecount + O->polycount;

    int v, e, idx;

    polygon * P0, * P1;
    uv * UV, * UV0;
    uv_edge * E;

    edge * E0;

    vertex * V0;

    int c_v = O->textcount;

    float Mu, Mv;

    for (e = 0; e < O->uvedcount; e ++) // edge verts average surrounding polys and self position
    {
        E = &O->uveds[e / ARRAYSIZE][e % ARRAYSIZE];
        UV = &O->uvtex_[0][(e + c_v) / ARRAYSIZE][(e + c_v) % ARRAYSIZE]; // edge vertex

        E0 = &O->edges[E->edge / ARRAYSIZE][E->edge % ARRAYSIZE];

        if (E->polycount > 1)
        {
            idx = E->polys[0];
            P0 = &O->polys[idx / ARRAYSIZE][idx % ARRAYSIZE];
            idx = E->polys[1];
            P1 = &O->polys[idx / ARRAYSIZE][idx % ARRAYSIZE];

            Mu = (P0->B2.u + P1->B2.u) / 2.0;
            Mv = (P0->B2.v + P1->B2.v) / 2.0;

            UV->u = (Mu + E->B.u) / 2.0;
            UV->v = (Mv + E->B.v) / 2.0;

            if (E0->weight != 0)
            {
                UV->u = E->B.u * E0->weight + UV->u * (1 - E0->weight);
                UV->v = E->B.v * E0->weight + UV->v * (1 - E0->weight);

                E->Mu = E->B.u * E0->weight + Mu * (1 - E0->weight);
                E->Mv = E->B.v * E0->weight + Mv * (1 - E0->weight);
            }
            else
            {
                E->Mu = Mu;
                E->Mv = Mv;
            }
        }
        else
        {
            E->Mu = E->B.u;
            E->Mv = E->B.v;
        }
    }

    float Fu, Fv;
    float Eu, Ev;
    float U, V;
    float Eu_o, Ev_o;
    float Eu_w, Ev_w;
    float edgecount;
    int openedge, split;
    int edgeweight;

    for (v = 0; v < O->textcount; v ++) // cage verts
    {
        UV = &O->uvtex_[0][v / ARRAYSIZE][v % ARRAYSIZE]; // new level 0 cage vertexes
        UV0 = &O->uvtex[v / ARRAYSIZE][v % ARRAYSIZE]; // old level -1 cage vertexes

        V0 = &O->verts[UV0->vert / ARRAYSIZE][UV0->vert % ARRAYSIZE];

        // first we scan surroundings and find distance to it as collapsed

        Fu = Fv = 0.0;
        Eu = Ev = 0.0;
        Eu_o = Ev_o = 0.0;
        Eu_w = Ev_w = 0.0;

        openedge = 0;
        split = 0;
        edgeweight = 0;

        // create logic for open edges

        edgecount = UV0->uvcount;

        for(e = 0; e < UV0->uvcount; e ++)
        {
            idx = UV0->uv_edges[e];
            E = &O->uveds[idx / ARRAYSIZE][idx % ARRAYSIZE];

            E0 = &O->edges[E->edge / ARRAYSIZE][E->edge % ARRAYSIZE];

            if (E->split)
            {
                split ++;
            }
            if (E->polycount == 1)
            {
                openedge ++;
                Eu_o += E->B.u;
                Ev_o += E->B.v;
            }
            else if (E0->weight != 0)
            {
                edgeweight ++;
                Eu_w += E->B.u;
                Ev_w += E->B.v;
            }

            Fu += E->Mu;
            Fv += E->Mv;

            Eu += E->B.u;
            Ev += E->B.v;
        }

        if (split == edgecount || split > 3 || (split && V0->edgecount != 4))
        {
            /* smooth split edges that have flow */
        }
        else if (openedge)
        {
            Eu_o /= openedge;
            Ev_o /= openedge;

            UV->u = (Eu_o + UV0->u) / 2.0;
            UV->v = (Ev_o + UV0->v) / 2.0;
        }
        else if (edgeweight)
        {
            Eu_w /= edgeweight;
            Ev_w /= edgeweight;

            UV->u = (Eu_w + UV0->u) / 2.0;
            UV->v = (Ev_w + UV0->v) / 2.0;
        }
        else
        {
            Fu /= edgecount;
            Fv /= edgecount;

            Eu /= edgecount;
            Ev /= edgecount;

            UV->u = (Fu + 2.0 * Eu + (UV->uvcount - 3) * UV0->u) / edgecount;
            UV->v = (Fv + 2.0 * Ev + (UV->uvcount - 3) * UV0->v) / edgecount;
        }

        if (!openedge && (V0->weight != 0) && edgeweight)
        {
            Fu /= edgecount;
            Fv /= edgecount;

            Eu /= edgecount;
            Ev /= edgecount;

            U = (Fu + 2.0 * Eu + (UV->uvcount - 3) * UV0->u) / edgecount;
            V = (Fv + 2.0 * Ev + (UV->uvcount - 3) * UV0->v) / edgecount;

            UV->u = UV->u * V0->weight + U * (1 - V0->weight);
            UV->v = UV->v * V0->weight + V * (1 - V0->weight);
        }
    }
}

int tune_In_Subdivision_Shape_transformed_(object * O, int L)
{

    //O->vertcount_[L] = O->vertcount_[L1] + O->edgecount_[L1] + O->quadcount_[L1];

    if (L < 1 || L > SUBD) return 0;

    int L1 = L - 1;

    //O->vertcount_[L] = O->vertcount + O->edgecount + O->polycount;

    int v, e, idx;

    quadrant * Q0, * Q1;
    vertex * V, * V0;
    edge * E;

    float Mx, My, Mz;

    direction_Pack D;
    float dot;

    int c_v = O->vertcount_[L1]; // cage vertcount

    for (e = 0; e < O->edgecount_[L1]; e ++) // edge verts average surrounding polys and self position
    {
        E = &O->edges_[L1][e / ARRAYSIZE][e % ARRAYSIZE];
        V = &O->verts_[L][(e + c_v) / ARRAYSIZE][(e + c_v) % ARRAYSIZE]; // edge vertex

        if (E->S != NULL && L < curve_subdiv) /* level 1 blocks patches */
        {
            Mx = E->S->B[0];
            My = E->S->B[1];
            Mz = E->S->B[2];

            V->Tx = Mx;
            V->Ty = My;
            V->Tz = Mz;

            E->Mx = Mx;
            E->My = My;
            E->Mz = Mz;
        }
/*
        else if (E->patch && L < curve_subdiv)
        {
            Mx = E->B.Tx;
            My = E->B.Ty;
            Mz = E->B.Tz;

            V->Tx = Mx;
            V->Ty = My;
            V->Tz = Mz;

            E->Mx = Mx;
            E->My = My;
            E->Mz = Mz;
        }
*/
        else if (E->polycount > 1)
        {
            idx = E->polys[0];
            Q0 = &O->quads_[L1][idx / ARRAYSIZE][idx % ARRAYSIZE];
            idx = E->polys[1];
            Q1 = &O->quads_[L1][idx / ARRAYSIZE][idx % ARRAYSIZE];

            Mx = (Q0->B.Tx + Q1->B.Tx) / 2.0;
            My = (Q0->B.Ty + Q1->B.Ty) / 2.0;
            Mz = (Q0->B.Tz + Q1->B.Tz) / 2.0;

            V->Tx = (Mx + V->Tx) / 2.0;
            V->Ty = (My + V->Ty) / 2.0;
            V->Tz = (Mz + V->Tz) / 2.0;

            if (E->weight != 0)
            {
                V->Tx = E->B.Tx * E->weight + V->Tx * (1 - E->weight);
                V->Ty = E->B.Ty * E->weight + V->Ty * (1 - E->weight);
                V->Tz = E->B.Tz * E->weight + V->Tz * (1 - E->weight);

                E->Mx = E->B.Tx * E->weight + Mx * (1 - E->weight);
                E->My = E->B.Ty * E->weight + My * (1 - E->weight);
                E->Mz = E->B.Tz * E->weight + Mz * (1 - E->weight);
            }
            else
            {
                E->Mx = Mx;
                E->My = My;
                E->Mz = Mz;
            }
        }
        else
        {
            E->Mx = E->B.Tx;
            E->My = E->B.Ty;
            E->Mz = E->B.Tz;
        }
    }

    /* create inside axis for every vertex edge involved in patches */

    int start, q, e0;

    edge * EI0,  * EI1,  * EI2, * EI3, * E0, * E1, * E2;
    quadrant * Q;
    polygon * P;
//    vertex * V1;

    float n0[3], n1[3];
    float curvature, pWeight, scaler;

    if (O->curve_count > 0 && L < curve_subdiv) /* level 1 blocks patches */
    {
        if (L == 1)
        {
            start = O->vertcount + O->edgecount;
        }
        else
        {
            start = O->vertcount_[L1 - 1] + O->edgecount_[L1 - 1];
        }

        for (v = start; v < O->vertcount_[L1]; v ++)
        {
            V = &O->verts_[L1][v / ARRAYSIZE][v % ARRAYSIZE];

            idx = v - start;

            if (L > 1)
            {
                Q = &O->quads_[L1 - 1][idx / ARRAYSIZE][idx % ARRAYSIZE];
                pWeight = Q->weight;
            }
            else
            {
                P = &O->polys[idx / ARRAYSIZE][idx % ARRAYSIZE];
                pWeight = V->weight;
            }

            if (V->patch)
            {
                /* find inside edge height with parallel edges */

                if (V->edgecount != 4)
                {
                    for (e = 0; e < V->edgecount; e ++)
                    {
                        idx = V->edges[e];
                        EI0 = &O->edges_[L1][idx / ARRAYSIZE][idx % ARRAYSIZE];

                        E0 = EI0->perimeter;

                        idx = V->edges[(e + V->edgecount - 1) % V->edgecount];
                        EI1 = &O->edges_[L1][idx / ARRAYSIZE][idx % ARRAYSIZE];

                        E1 = EI1->perimeter;

                        idx = V->edges[(e + 1) % V->edgecount];
                        EI2 = &O->edges_[L1][idx / ARRAYSIZE][idx % ARRAYSIZE];

                        E2 = EI2->perimeter;

                        curvature = (E1->height + E2->height) / 2.0;

                        if (L > 1)
                        {
                            curvature /= Q->dist;
                        }
                        else
                        {
                            curvature /= P->dist;
                        }

                        EI0->weight = curvature;
                    }
                }
                else
                {
                    for (e = 0; e < 2; e ++)
                    {
                        idx = V->edges[e];
                        EI0 = &O->edges_[L1][idx / ARRAYSIZE][idx % ARRAYSIZE];

                        E0 = EI0->perimeter;

                        idx = V->edges[(e + V->edgecount - 1) % V->edgecount];
                        EI1 = &O->edges_[L1][idx / ARRAYSIZE][idx % ARRAYSIZE];

                        E1 = EI1->perimeter;

                        idx = V->edges[(e + 1) % V->edgecount];
                        EI2 = &O->edges_[L1][idx / ARRAYSIZE][idx % ARRAYSIZE];

                        E2 = EI2->perimeter;

                        idx = V->edges[(e + 2) % V->edgecount];
                        EI3 = &O->edges_[L1][idx / ARRAYSIZE][idx % ARRAYSIZE];

                        curvature = (E1->height + E2->height) / 2.0;

                        if (L > 1)
                        {
                            curvature /= Q->dist;
                        }
                        else
                        {
                            curvature /= P->dist;
                        }

                        EI0->weight = curvature;
                        EI3->weight = curvature;
                    }
                }
            }
        }

        for (v = start; v < O->vertcount_[L1]; v ++)
        {
            V = &O->verts_[L1][v / ARRAYSIZE][v % ARRAYSIZE];

            idx = v - start;

            if (L > 1)
            {
                Q = &O->quads_[L1 - 1][idx / ARRAYSIZE][idx % ARRAYSIZE];
                pWeight = Q->weight;
            }
            else
            {
                P = &O->polys[idx / ARRAYSIZE][idx % ARRAYSIZE];
                pWeight = V->weight;
            }

            if (V->patch)
            {
/*
                // handle corners

                if (L > 1)
                {
                    for (e = 0; e < 4; e ++)
                    {
                        idx = Q->verts[e];
                        V0 = &O->verts_[L1 - 1][idx / ARRAYSIZE][idx % ARRAYSIZE];
                        V1 = V0->vert;

                        if (V1 == NULL) continue;

                        n0[0] = V1->N.Tx;
                        n0[1] = V1->N.Ty;
                        n0[2] = V1->N.Tz;

                        for (e0 = 0; e0 < V1->edgecount; e0 ++)
                        {
                            idx = V1->edges[e0];
                            E = &O->edges_[L1][idx / ARRAYSIZE][idx % ARRAYSIZE];

                            if (E->smooth)
                                scaler = cp_continuity[L1] * 2.0;
                            else
                                scaler = 0; //cp_continuity[L1] * 2.0 * E->weight * pWeight;

                            // do edge AC

                            D = length_AB_(V1->Tx, V1->Ty, V1->Tz, E->B.Tx, E->B.Ty, E->B.Tz);

                            dot = dot_productFF(n0, D.vec);

                            // corner edges set A, then edge center vertexes in perimeter set C

                            E->A[0] = E->B.Tx + V1->N.Tx * D.distance * -dot * scaler;
                            E->A[1] = E->B.Ty + V1->N.Ty * D.distance * -dot * scaler;
                            E->A[2] = E->B.Tz + V1->N.Tz * D.distance * -dot * scaler;

//                            E->A[0] -= V1->Tx;
//                            E->A[1] -= V1->Ty;
//                            E->A[2] -= V1->Tz;
//
//                            E->A[0] *= scaler;
//                            E->A[1] *= scaler;
//                            E->A[2] *= scaler;
//
//                            E->A[0] += V1->Tx;
//                            E->A[1] += V1->Ty;
//                            E->A[2] += V1->Tz;
                        }
                    }
                }
                else
                {
                    for (e = 0; e < P->edgecount; e ++)
                    {
                        idx = P->verts[e];
                        V0 = &O->verts[idx / ARRAYSIZE][idx % ARRAYSIZE];
                        V1 = V0->vert;

                        if (V1 == NULL) continue;

                        n0[0] = V1->N.Tx;
                        n0[1] = V1->N.Ty;
                        n0[2] = V1->N.Tz;

                        for (e0 = 0; e0 < V1->edgecount; e0 ++)
                        {
                            idx = V1->edges[e0];
                            E = &O->edges_[L1][idx / ARRAYSIZE][idx % ARRAYSIZE];

                            if (E->smooth)
                                scaler = cp_continuity[L1] * 2.0;
                            else
                                scaler = 0; // cp_continuity[L1] * 2.0 * E->weight * pWeight;

                            // do edge AC

                            D = length_AB_(V1->Tx, V1->Ty, V1->Tz, E->B.Tx, E->B.Ty, E->B.Tz);

                            dot = dot_productFF(n0, D.vec);

                            // inside edges set A, then edge center vertexes in perimeter set C

                            E->A[0] = E->B.Tx + V1->N.Tx * D.distance * -dot * scaler;
                            E->A[1] = E->B.Ty + V1->N.Ty * D.distance * -dot * scaler;
                            E->A[2] = E->B.Tz + V1->N.Tz * D.distance * -dot * scaler;

//                            E->A[0] -= V1->Tx;
//                            E->A[1] -= V1->Ty;
//                            E->A[2] -= V1->Tz;
//
//                            E->A[0] *= scaler;
//                            E->A[1] *= scaler;
//                            E->A[2] *= scaler;
//
//                            E->A[0] += V1->Tx;
//                            E->A[1] += V1->Ty;
//                            E->A[2] += V1->Tz;
                        }
                    }
                }

*/

                /* now handle inside edges */

                n0[0] = V->N.Tx;
                n0[1] = V->N.Ty;
                n0[2] = V->N.Tz;

                for (e = 0; e < V->edgecount; e ++)
                {
                    idx = V->edges[e];
                    E = &O->edges_[L1][idx / ARRAYSIZE][idx % ARRAYSIZE];

                    if (E->smooth == 1) // perimeter edge
                    {
                        scaler = cp_continuity[L1] * 2.0; // * E->weight * pWeight;

                        D = length_AB_(V->Tx, V->Ty, V->Tz, E->B.Tx, E->B.Ty, E->B.Tz);

                        dot = dot_productFF(n0, D.vec);

                        /* inside edges set A, then edge center vertexes in perimeter set C */

                        E->A[0] = E->B.Tx + V->N.Tx * D.distance * -dot;
                        E->A[1] = E->B.Ty + V->N.Ty * D.distance * -dot;
                        E->A[2] = E->B.Tz + V->N.Tz * D.distance * -dot;

                        E->A[0] -= V->Tx;
                        E->A[1] -= V->Ty;
                        E->A[2] -= V->Tz;

                        E->A[0] *= scaler;
                        E->A[1] *= scaler;
                        E->A[2] *= scaler;

                        E->A[0] += V->Tx;
                        E->A[1] += V->Ty;
                        E->A[2] += V->Tz;
                    }
                    else if (E->smooth == 2) // inside edge
                    {
                        D = length_AB_(V->Tx, V->Ty, V->Tz, E->B.Tx, E->B.Ty, E->B.Tz);

                        dot = dot_productFF(n0, D.vec);

                        /* inside edges set A, then edge center vertexes in perimeter set C */

                        E->A[0] = E->B.Tx + V->N.Tx * D.distance * -dot;
                        E->A[1] = E->B.Ty + V->N.Ty * D.distance * -dot;
                        E->A[2] = E->B.Tz + V->N.Tz * D.distance * -dot;
                    }
                    else // cross edge is sharp
                    {
                        E->A[0] = E->B.Tx;
                        E->A[1] = E->B.Ty;
                        E->A[2] = E->B.Tz;
                    }

                    /* here address far edges over perimeter vertex */

                    idx = E->verts[1];
                    V0 = &O->verts_[L1][idx / ARRAYSIZE][idx % ARRAYSIZE];

                    n1[0] = V0->N.Tx;
                    n1[1] = V0->N.Ty;
                    n1[2] = V0->N.Tz;

                    for (e0 = 0; e0 < V0->edgecount; e0 ++)
                    {
                        idx = V0->edges[e0];

                        E0 = &O->edges_[L1][idx / ARRAYSIZE][idx % ARRAYSIZE];

                        if (E0->smooth == 1) // perimeter edge
                        {
                            scaler = cp_continuity[L1] * 2.0; // * E->weight * pWeight;

                            D = length_AB_(V0->Tx, V0->Ty, V0->Tz, E0->B.Tx, E0->B.Ty, E0->B.Tz);

                            dot = dot_productFF(n1, D.vec);

                            /* edge center vertexes in perimeter set C */

                            E0->C[0] = E->B.Tx + V0->N.Tx * D.distance * -dot;
                            E0->C[1] = E->B.Ty + V0->N.Ty * D.distance * -dot;
                            E0->C[2] = E->B.Tz + V0->N.Tz * D.distance * -dot;

                            E0->C[0] -= V0->Tx;
                            E0->C[1] -= V0->Ty;
                            E0->C[2] -= V0->Tz;

                            E0->C[0] *= scaler;
                            E0->C[1] *= scaler;
                            E0->C[2] *= scaler;

                            E0->C[0] += V0->Tx;
                            E0->C[1] += V0->Ty;
                            E0->C[2] += V0->Tz;
                        }
                        else if (E0->smooth == 2) // inside edge
                        {
                            scaler = cp_continuity[L1] * 2.0 * E0->weight * pWeight;

                            D = length_AB_(V0->Tx, V0->Ty, V0->Tz, E0->B.Tx, E0->B.Ty, E0->B.Tz);

                            dot = dot_productFF(n1, D.vec);

                            /* edge center vertexes in perimeter set C */

                            E0->C[0] = E0->B.Tx + V0->N.Tx * D.distance * -dot * scaler;
                            E0->C[1] = E0->B.Ty + V0->N.Ty * D.distance * -dot * scaler;
                            E0->C[2] = E0->B.Tz + V0->N.Tz * D.distance * -dot * scaler;
                        }
                        else // cross edge is sharp
                        {
                            E0->C[0] = E0->B.Tx;
                            E0->C[1] = E0->B.Ty;
                            E0->C[2] = E0->B.Tz;
                        }
                    }
                }
            }
        }
    }

    /* */

    int set_and_done;
    //float n0[3], dot;

    //edge * E0;

    if (O->curve_count > 0 && L < curve_subdiv) /* level 1 blocks patches */
    {
        // use polycenter vertexes weight as patch inicator
        // start polycenter verts after cage and edge verts

        if (L == 1)
        {
            start = O->vertcount + O->edgecount;
        }
        else
        {
            start = O->vertcount_[L1 - 1] + O->edgecount_[L1 - 1];
        }

        for (v = start; v < O->vertcount_[L1]; v ++)
        {
            V = &O->verts_[L1][v / ARRAYSIZE][v % ARRAYSIZE];

            if (V->patch)
            {
                for (e = 0; e < V->edgecount; e ++)
                {
                    idx = V->edges[e];
                    E = &O->edges_[L1][idx / ARRAYSIZE][idx % ARRAYSIZE];

                    E->Mx = (E->A[0] + E->C[0]) / 2.0;
                    E->My = (E->A[1] + E->C[1]) / 2.0;
                    E->Mz = (E->A[2] + E->C[2]) / 2.0;
                }

                for (e = 0; e < V->edgecount; e ++)
                {
                    idx = V->edges[e];
                    E = &O->edges_[L1][idx / ARRAYSIZE][idx % ARRAYSIZE];

                    if (E->subdivs)
                    {
                        idx = E->edges[0];

                        E0 = &O->edges_[L][idx / ARRAYSIZE][idx % ARRAYSIZE];

                        set_and_done = 0;

                        if (V->vert != NULL)
                        {
                            if (E0->verts[0] == V->vert->index)
                            {
                                idx = E0->verts[1];
                                set_and_done = 1;
                            }
                            else if (E0->verts[1] == V->vert->index)
                            {
                                idx = E0->verts[0];
                                set_and_done = 1;
                            }
                        }

                        if (!set_and_done)
                        {
                            idx = E->edges[1];

                            E0 = &O->edges_[L][idx / ARRAYSIZE][idx % ARRAYSIZE];

                            set_and_done = 0;

                            if (V->vert != NULL)
                            {
                                if (E0->verts[0] == V->vert->index)
                                {
                                    idx = E0->verts[1];
                                    set_and_done = 1;
                                }
                                else if (E0->verts[1] == V->vert->index)
                                {
                                    idx = E0->verts[0];
                                    set_and_done = 1;
                                }
                            }
                        }

                        if (set_and_done)
                        {
                            V0 = &O->verts_[L][idx / ARRAYSIZE][idx % ARRAYSIZE];

                            V0->Tx = E->Mx;
                            V0->Ty = E->My;
                            V0->Tz = E->Mz;
                        }
                    }
                }
            }
        }
    }

    float Tx, Ty, Tz;
    float dist;

    if (O->curve_count > 0 && L < curve_subdiv) /* level 1 blocks patches */
    {
        if (L == 1)
        {
            start = O->vertcount + O->edgecount;
        }
        else
        {
            start = O->vertcount_[L1 - 1] + O->edgecount_[L1 - 1];
        }

        for (q = 0; q < O->quadcount_[L1]; q ++)
        {
            Q = &O->quads_[L1][q / ARRAYSIZE][q % ARRAYSIZE];

            if (Q->patch)
            {
                Q->center[0] = 0;
                Q->center[1] = 0;
                Q->center[2] = 0;
                Q->dist = 0;

                Q->vec[0] = 0;
                Q->vec[1] = 0;
                Q->vec[2] = 0;

            }
        }

        for (e = 0; e < O->edgecount_[L1]; e ++) // find edge polys and center verts
        {
            E = &O->edges_[L1][e / ARRAYSIZE][e % ARRAYSIZE];

            if (E->patch)
            {
                D = length_AB_(E->B.Tx, E->B.Ty, E->B.Tz, E->Mx, E->My, E->Mz);

                idx = E->polys[0];
                Q0 = &O->quads_[L1][idx / ARRAYSIZE][idx % ARRAYSIZE];

                Q0->center[0] += E->Mx;
                Q0->center[1] += E->My;
                Q0->center[2] += E->Mz;
                Q0->dist += D.distance;

                E->height = D.distance;

                E->vec[0] = E->Mx - E->B.Tx;
                E->vec[1] = E->My - E->B.Ty;
                E->vec[2] = E->Mz - E->B.Tz;

                if (E->polycount > 1)
                {
                    //idx = O->vertcount + O->edgecount + E->polys[1];
                    //V = &O->verts_[0][idx / ARRAYSIZE][idx % ARRAYSIZE]; // polys center;

                    idx = E->polys[1];
                    Q1 = &O->quads_[L1][idx / ARRAYSIZE][idx % ARRAYSIZE];

                    Q1->center[0] += E->Mx;
                    Q1->center[1] += E->My;
                    Q1->center[2] += E->Mz;
                    Q1->dist += D.distance;
                }
            }
        }

        // calculate edge pairs

        for (q = 0; q < O->quadcount_[L1]; q ++)
        {
            Q = &O->quads_[L1][q / ARRAYSIZE][q % ARRAYSIZE];
            idx = O->vertcount_[L1] + O->edgecount_[L1] + q;
            V = &O->verts_[L][idx / ARRAYSIZE][idx % ARRAYSIZE]; // polys center;

            if (V->patch)
            {
                // create paired edge weights
                for (e = 0; e < 2; e ++)
                {
                    idx = Q->edges[e];
                    E = &O->edges_[L1][idx / ARRAYSIZE][idx % ARRAYSIZE];

                    // find opposite edge and make a pair

                    idx = Q->edges[2 + e];
                    E0 = &O->edges_[L1][idx / ARRAYSIZE][idx % ARRAYSIZE];

                    Q->vec[0] += (E0->vec[0] + E->vec[0]) * 0.5;
                    Q->vec[1] += (E0->vec[1] + E->vec[1]) * 0.5;
                    Q->vec[2] += (E0->vec[2] + E->vec[2]) * 0.5;
                }
            }
        }

        /* calculate quad center */

        for (q = 0; q < O->quadcount_[L1]; q ++)
        {
            Q = &O->quads_[L1][q / ARRAYSIZE][q % ARRAYSIZE];
            idx = O->vertcount_[L1] + O->edgecount_[L1] + q;
            V = &O->verts_[L][idx / ARRAYSIZE][idx % ARRAYSIZE]; // polys center;

            if (V->patch)
            {

//                V->Tx = Q->B.Tx + Q->vec[0];
//                V->Ty = Q->B.Ty + Q->vec[1];
//                V->Tz = Q->B.Tz + Q->vec[2];

                Tx = Q->center[0] / 4.0;
                Ty = Q->center[1] / 4.0;
                Tz = Q->center[2] / 4.0;

                Q->dist /= 2.0;
                Q->dist *= edge_divisor;

                if (Q->dist == 0)
                {
                    Q->dist = min_dist;
                }

                D = length_AB_(Q->B.Tx, Q->B.Ty, Q->B.Tz, Tx, Ty, Tz);

                V->weight = D.distance / Q->dist; // lift

                V->weight *= Q->weight;

                dist = (D.distance + Q->dist) * V->weight;

                V->Tx = Tx + V->N.Tx * dist;
                V->Ty = Ty + V->N.Ty * dist;
                V->Tz = Tz + V->N.Tz * dist;

                if (Q->subdivs)
                {
                    for (e = 0; e < 4; e ++)
                    {
                        idx = Q->quads[e];
                        Q0 = &O->quads_[L][idx / ARRAYSIZE][idx % ARRAYSIZE];
                        Q0->weight = V->weight;
                    }
                }
            }
        }
    }


    float Fx, Fy, Fz;
    float Ex, Ey, Ez;
    //float Tx, Ty, Tz;
    float Ex_o, Ey_o, Ez_o;
    float Ex_w, Ey_w, Ez_w;
    float edgecount;
    int openedge;
    int edgeweight;

    for (v = 0; v < O->vertcount_[L1]; v ++) // cage verts
    {
        V = &O->verts_[L][v / ARRAYSIZE][v % ARRAYSIZE]; // new level 0 cage vertexes
        V0 = &O->verts_[L1][v / ARRAYSIZE][v % ARRAYSIZE]; // old level -1 cage vertexes

        if (V0->patch && L < curve_subdiv) /* level 1 blocks patches */
        {

        }
        else
        {
            // first we scan surroundings and find distance to it as collapsed

            Fx = Fy = Fz = 0.0;
            Ex = Ey = Ez = 0.0;
            Ex_o = Ey_o = Ez_o = 0.0;
            Ex_w = Ey_w = Ez_w = 0.0;

            openedge = 0;
            edgeweight = 0;

            // create logic for open edges

            edgecount = V0->edgecount;

            for(e = 0; e < V0->edgecount; e ++)
            {
                idx = V0->edges[e];
                E = &O->edges_[L1][idx / ARRAYSIZE][idx % ARRAYSIZE];

                if (E->polycount == 1)
                {
                    openedge ++;
                    Ex_o += E->B.Tx;
                    Ey_o += E->B.Ty;
                    Ez_o += E->B.Tz;
                }
                else if (E->weight != 0)
                {
                    edgeweight ++;
                    Ex_w += E->B.Tx;
                    Ey_w += E->B.Ty;
                    Ez_w += E->B.Tz;
                }

                Fx += E->Mx;
                Fy += E->My;
                Fz += E->Mz;

                Ex += E->B.Tx;
                Ey += E->B.Ty;
                Ez += E->B.Tz;
            }

            if (openedge)
            {
                Ex_o /= openedge;
                Ey_o /= openedge;
                Ez_o /= openedge;

                Tx = V->Tx;
                Ty = V->Ty;
                Tz = V->Tz;

                V->Tx = (Ex_o + V->Tx) / 2.0;
                V->Ty = (Ey_o + V->Ty) / 2.0;
                V->Tz = (Ez_o + V->Tz) / 2.0;
            }
            else if (edgeweight)
            {
                Ex_w /= edgeweight;
                Ey_w /= edgeweight;
                Ez_w /= edgeweight;

                V->Tx = (Ex_w + V->Tx) / 2.0;
                V->Ty = (Ey_w + V->Ty) / 2.0;
                V->Tz = (Ez_w + V->Tz) / 2.0;
            }
            else
            {
                Fx /= edgecount;
                Fy /= edgecount;
                Fz /= edgecount;

                Ex /= edgecount;
                Ey /= edgecount;
                Ez /= edgecount;

                V->Tx = (Fx + 2.0 * Ex + (V->edgecount - 3) * V->Tx) / edgecount;
                V->Ty = (Fy + 2.0 * Ey + (V->edgecount - 3) * V->Ty) / edgecount;
                V->Tz = (Fz + 2.0 * Ez + (V->edgecount - 3) * V->Tz) / edgecount;
            }
            if (V0->weight != 0 && edgeweight)
            {
                if (openedge)
                {
                    V->Tx = Tx * V0->weight + V->Tx * (1 - V0->weight);
                    V->Ty = Ty * V0->weight + V->Ty * (1 - V0->weight);
                    V->Tz = Tz * V0->weight + V->Tz * (1 - V0->weight);
                }
                else
                {
                    Fx /= edgecount;
                    Fy /= edgecount;
                    Fz /= edgecount;

                    Ex /= edgecount;
                    Ey /= edgecount;
                    Ez /= edgecount;

                    Tx = (Fx + 2.0 * Ex + (V->edgecount - 3) * V->Tx) / edgecount;
                    Ty = (Fy + 2.0 * Ey + (V->edgecount - 3) * V->Ty) / edgecount;
                    Tz = (Fz + 2.0 * Ez + (V->edgecount - 3) * V->Tz) / edgecount;

                    V->Tx = V->Tx * V0->weight + Tx * (1 - V0->weight);
                    V->Ty = V->Ty * V0->weight + Ty * (1 - V0->weight);
                    V->Tz = V->Tz * V0->weight + Tz * (1 - V0->weight);
                }
            }
        }
    }

    float a, b;

    if (O->curve_count > 0)// && L < curve_subdiv) /* level 1 blocks patches */
    {
        start = O->vertcount_[L1];

        for (e = 0; e < O->edgecount_[L1]; e ++) // edge verts average surrounding polys and self position
        {
            E = &O->edges_[L1][e / ARRAYSIZE][e % ARRAYSIZE];

            if (E->S != NULL)
            {
                idx = e + start;
                V0 = &O->verts_[L][idx / ARRAYSIZE][idx % ARRAYSIZE];

                a = E->S->weight;
                b = 1 - a;

                V0->Tx = (E->S->B[0] * a + V0->Tx * b);
                V0->Ty = (E->S->B[1] * a + V0->Ty * b);
                V0->Tz = (E->S->B[2] * a + V0->Tz * b);

                if (E->S->counter_edge)
                    idx = E->verts[1];
                else
                    idx = E->verts[0];

                V = &O->verts_[L1][idx / ARRAYSIZE][idx % ARRAYSIZE];

                if (V->vert != NULL)
                {
                    V->vert->Tx = (E->S->A[0] * a + V->vert->Tx * b);
                    V->vert->Ty = (E->S->A[1] * a + V->vert->Ty * b);
                    V->vert->Tz = (E->S->A[2] * a + V->vert->Tz * b);
                }

                if (E->S->counter_edge)
                    idx = E->verts[0];
                else
                    idx = E->verts[1];

                V = &O->verts_[L1][idx / ARRAYSIZE][idx % ARRAYSIZE];

                if (V->vert != NULL)
                {
                    V->vert->Tx = (E->S->C[0] * a + V->vert->Tx * b);
                    V->vert->Ty = (E->S->C[1] * a + V->vert->Ty * b);
                    V->vert->Tz = (E->S->C[2] * a + V->vert->Tz * b);
                }
            }
        }
    }

    return 1;
}

int tune_In_Subdivision_Shape_(object * O, int L, float edge_c, float cage_v)
{

    //O->vertcount_[L] = O->vertcount_[L1] + O->edgecount_[L1] + O->quadcount_[L1];

    if (L < 1 || L > SUBD) return 0;

    int L1 = L - 1;

    //O->vertcount_[L] = O->vertcount + O->edgecount + O->polycount;

    int v, e, idx;

    quadrant * Q0, * Q1;
    vertex * V, * V0;
    edge * E;

    float X, Y, Z;

    float edge_c0 = 1.0 - edge_c;
    float cage_v0 = 1.0 - cage_v;

    int c_v = O->vertcount_[L1]; // cage vertcount

    for (e = 0; e < O->edgecount_[L1]; e ++) // edge verts average surrounding polys and self position
    {
        E = &O->edges_[L1][e / ARRAYSIZE][e % ARRAYSIZE];
        V = &O->verts_[L][(e + c_v) / ARRAYSIZE][(e + c_v) % ARRAYSIZE]; // edge vertex

        if (E->polycount > 1)
        {
            idx = E->polys[0];
            Q0 = &O->quads_[L1][idx / ARRAYSIZE][idx % ARRAYSIZE];
            idx = E->polys[1];
            Q1 = &O->quads_[L1][idx / ARRAYSIZE][idx % ARRAYSIZE];

            E->Mx = (Q0->B.x + Q1->B.x) / 2.0;
            E->My = (Q0->B.y + Q1->B.y) / 2.0;
            E->Mz = (Q0->B.z + Q1->B.z) / 2.0;

            X = (E->Mx + V->x) / 2.0;
            Y = (E->My + V->y) / 2.0;
            Z = (E->Mz + V->z) / 2.0;

            V->x = V->x * edge_c0 + X * edge_c;
            V->y = V->y * edge_c0 + Y * edge_c;
            V->z = V->z * edge_c0 + Z * edge_c;

        }
        else
        {
            E->Mx = E->B.x;
            E->My = E->B.y;
            E->Mz = E->B.z;
        }
    }

    float Fx, Fy, Fz;
    float Ex, Ey, Ez;
    float Ex_o, Ey_o, Ez_o;
    float edgecount;
    int openedge;

    for (v = 0; v < O->vertcount_[L1]; v ++) // cage verts
    {
        V = &O->verts_[L][v / ARRAYSIZE][v % ARRAYSIZE]; // new level 0 cage vertexes
        V0 = &O->verts_[L1][v / ARRAYSIZE][v % ARRAYSIZE]; // old level -1 cage vertexes

        // first we scan surroundings and find distance to it as collapsed

        Fx = Fy = Fz = 0.0;
        Ex = Ey = Ez = 0.0;
        Ex_o = Ey_o = Ez_o = 0.0;

        openedge = 0;

        // create logic for open edges

        edgecount = V0->edgecount;

        for(e = 0; e < V0->edgecount; e ++)
        {
            idx = V0->edges[e];
            E = &O->edges_[L1][idx / ARRAYSIZE][idx % ARRAYSIZE];

            if (E->polycount == 1)
            {
                openedge ++;
                Ex_o += E->B.x;
                Ey_o += E->B.y;
                Ez_o += E->B.z;
            }

            Fx += E->Mx;
            Fy += E->My;
            Fz += E->Mz;

            Ex += E->B.x;
            Ey += E->B.y;
            Ez += E->B.z;
        }

        if (openedge)
        {
            Ex_o /= openedge;
            Ey_o /= openedge;
            Ez_o /= openedge;

            V->x = (Ex_o + V->x) / 2.0;
            V->y = (Ey_o + V->y) / 2.0;
            V->z = (Ez_o + V->z) / 2.0;
        }
        else
        {
            Fx /= edgecount;
            Fy /= edgecount;
            Fz /= edgecount;

            Ex /= edgecount;
            Ey /= edgecount;
            Ez /= edgecount;

            X = (Fx + 2.0 * Ex + (V->edgecount - 3) * V->x) / edgecount;
            Y = (Fy + 2.0 * Ey + (V->edgecount - 3) * V->y) / edgecount;
            Z = (Fz + 2.0 * Ez + (V->edgecount - 3) * V->z) / edgecount;

            V->x = V->x * cage_v0 + X * cage_v;
            V->y = V->y * cage_v0 + Y * cage_v;
            V->z = V->z * cage_v0 + Z * cage_v;
        }
    }

    return 1;
}

void tune_In_Subdivision_Shape_transformed(object * O)
{

    //O->vertcount_[L] = O->vertcount + O->edgecount + O->polycount;

    int v, e, idx;

    polygon * P0, * P1;
    vertex * V, * V0;
    edge * E;

    int c_v = O->vertcount;

    float Mx, My, Mz;

    direction_Pack D;

    for (e = 0; e < O->edgecount; e ++) // edge verts average surrounding polys and self position
    {
        E = &O->edges[e / ARRAYSIZE][e % ARRAYSIZE];
        V = &O->verts_[0][(e + c_v) / ARRAYSIZE][(e + c_v) % ARRAYSIZE]; // edge vertex

        if (E->S != NULL) /* edge medians are defined with segments */
        {
            Mx = E->S->B[0];
            My = E->S->B[1];
            Mz = E->S->B[2];

            V->Tx = Mx;
            V->Ty = My;
            V->Tz = Mz;

            E->Mx = Mx;
            E->My = My;
            E->Mz = Mz;
        }
/*
        else if (E->patch)
        {
            Mx = E->B.Tx;
            My = E->B.Ty;
            Mz = E->B.Tz;

            V->Tx = Mx;
            V->Ty = My;
            V->Tz = Mz;

            E->Mx = Mx;
            E->My = My;
            E->Mz = Mz;
        }
*/
        else if (E->polycount > 1)
        {
            idx = E->polys[0];
            P0 = &O->polys[idx / ARRAYSIZE][idx % ARRAYSIZE];
            idx = E->polys[1];
            P1 = &O->polys[idx / ARRAYSIZE][idx % ARRAYSIZE];

            Mx = (P0->B.Tx + P1->B.Tx) / 2.0;
            My = (P0->B.Ty + P1->B.Ty) / 2.0;
            Mz = (P0->B.Tz + P1->B.Tz) / 2.0;

            V->Tx = (Mx + V->Tx) / 2.0;
            V->Ty = (My + V->Ty) / 2.0;
            V->Tz = (Mz + V->Tz) / 2.0;

            if (E->weight != 0)
            {
                V->Tx = E->B.Tx * E->weight + V->Tx * (1 - E->weight);
                V->Ty = E->B.Ty * E->weight + V->Ty * (1 - E->weight);
                V->Tz = E->B.Tz * E->weight + V->Tz * (1 - E->weight);

                E->Mx = E->B.Tx * E->weight + Mx * (1 - E->weight);
                E->My = E->B.Ty * E->weight + My * (1 - E->weight);
                E->Mz = E->B.Tz * E->weight + Mz * (1 - E->weight);
            }
            else
            {
                E->Mx = Mx;
                E->My = My;
                E->Mz = Mz;
            }
        }
        else
        {
            E->Mx = E->B.Tx;
            E->My = E->B.Ty;
            E->Mz = E->B.Tz;
        }
    }

    int p;
    float Tx, Ty, Tz;
//    float dist;

    edge * E0;
    quadrant * Q;
    polygon * P;

    if (O->curve_count > 0)
    {
        /* calculate polygon flatness */

        for (p = 0; p < O->polycount; p ++)
        {
            P = &O->polys[p / ARRAYSIZE][p % ARRAYSIZE];

            if (P->patch)
            {
                P->center[0] = 0;
                P->center[1] = 0;
                P->center[2] = 0;
                P->dist = 0;
                P->vec[0] = 0;
                P->vec[1] = 0;
                P->vec[2] = 0;
            }
        }

        for (e = 0; e < O->edgecount; e ++) // find edge polys and center verts
        {
            E = &O->edges[e / ARRAYSIZE][e % ARRAYSIZE];

            if (E->patch)
            {
                D = length_AB_(E->B.Tx, E->B.Ty, E->B.Tz, E->Mx, E->My, E->Mz); // edge lift

                idx = E->polys[0];
                P0 = &O->polys[idx / ARRAYSIZE][idx % ARRAYSIZE];

                P0->center[0] += E->Mx;
                P0->center[1] += E->My;
                P0->center[2] += E->Mz;
                P0->dist += D.distance;

                E->height = D.distance;

                E->vec[0] = E->Mx - E->B.Tx;
                E->vec[1] = E->My - E->B.Ty;
                E->vec[2] = E->Mz - E->B.Tz;


                if (E->polycount > 1)
                {

                    idx = E->polys[1];
                    P1 = &O->polys[idx / ARRAYSIZE][idx % ARRAYSIZE];

                    P1->center[0] += E->Mx;
                    P1->center[1] += E->My;
                    P1->center[2] += E->Mz;
                    P1->dist += D.distance;
                }

            }
        }

        /* calculate edge pairs */

        for (p = 0; p < O->polycount; p ++)
        {
            P = &O->polys[p / ARRAYSIZE][p % ARRAYSIZE];
            idx = O->vertcount + O->edgecount + p;
            V = &O->verts_[0][idx / ARRAYSIZE][idx % ARRAYSIZE]; // polys center;

            if (V->patch)
            {
                /* create paired edge weights */
                if (P->edgecount % 2) // if uneven
                {
                    for (e = 0; e < P->edgecount; e ++)
                    {
                        /* find composite vector for each vertex edges */

                        idx = P->edges[e];
                        E = &O->edges[e / ARRAYSIZE][e % ARRAYSIZE];

                        idx = P->edges[(e + 1) % P->edgecount];
                        E0 = &O->edges[idx / ARRAYSIZE][idx % ARRAYSIZE];

                        P->vec[0] += (E0->vec[0] + E->vec[0]) * 0.5;
                        P->vec[1] += (E0->vec[1] + E->vec[1]) * 0.5;
                        P->vec[2] += (E0->vec[2] + E->vec[2]) * 0.5;
                    }
                }
                else // if even
                {
                    for (e = 0; e < P->edgecount / 2; e ++)
                    {
                        idx = P->edges[e];
                        E = &O->edges[idx / ARRAYSIZE][idx % ARRAYSIZE];

                        /* find opposite edge and make a pair */

                        idx = P->edges[(P->edgecount / 2) + e];
                        E0 = &O->edges[idx / ARRAYSIZE][idx % ARRAYSIZE];

                        P->vec[0] += (E0->vec[0] + E->vec[0]) * 0.5;
                        P->vec[1] += (E0->vec[1] + E->vec[1]) * 0.5;
                        P->vec[2] += (E0->vec[2] + E->vec[2]) * 0.5;
                    }
                }
            }
        }

        /* calculate polygon center */

        for (p = 0; p < O->polycount; p ++)
        {
            P = &O->polys[p / ARRAYSIZE][p % ARRAYSIZE];
            idx = O->vertcount + O->edgecount + p;
            V = &O->verts_[0][idx / ARRAYSIZE][idx % ARRAYSIZE]; // polys center;

            if (V->patch)
            {
                V->Tx = P->B.Tx + P->vec[0];
                V->Ty = P->B.Ty + P->vec[1];
                V->Tz = P->B.Tz + P->vec[2];

                Tx = P->center[0] / (float)V->edgecount;
                Ty = P->center[1] / (float)V->edgecount;
                Tz = P->center[2] / (float)V->edgecount;

                P->dist /= ((float)V->edgecount / 2.0);
                P->dist *= edge_divisor;

                if (P->dist == 0)
                {
                    P->dist = min_dist;
                }

                D = length_AB_(P->B.Tx, P->B.Ty, P->B.Tz, Tx, Ty, Tz); // median lift

                V->weight = D.distance / P->dist; // lift

//                dist = ((D.distance + P->dist) / 2.0) * V->weight;
//
//                V->Tx = Tx + V->N.Tx * dist;
//                V->Ty = Ty + V->N.Ty * dist;
//                V->Tz = Tz + V->N.Tz * dist;

                if (P->subdivs)
                {
                    for (e = 0; e < P->edgecount; e ++)
                    {
                        idx = P->quads[e];
                        Q = &O->quads_[0][idx / ARRAYSIZE][idx % ARRAYSIZE];
                        Q->weight = V->weight;
                    }
                }
            }
        }
    }

    float Fx, Fy, Fz;
    float Ex, Ey, Ez;
    //float Tx, Ty, Tz;
    float Ex_o, Ey_o, Ez_o;
    float Ex_w, Ey_w, Ez_w;
    float edgecount;
    int openedge;
    int edgeweight;

    for (v = 0; v < O->vertcount; v ++) // cage verts
    {
        V = &O->verts_[0][v / ARRAYSIZE][v % ARRAYSIZE]; // new level 0 cage vertexes
        V0 = &O->verts[v / ARRAYSIZE][v % ARRAYSIZE]; // old level -1 cage vertexes

        if (V0->control_point != NULL)
        {
            V->Tx = V0->control_point->pos[0];
            V->Ty = V0->control_point->pos[1];
            V->Tz = V0->control_point->pos[2];
        }
        else
        {
            // first we scan surroundings and find distance to it as collapsed

            Fx = Fy = Fz = 0.0;
            Ex = Ey = Ez = 0.0;
            Ex_o = Ey_o = Ez_o = 0.0;
            Ex_w = Ey_w = Ez_w = 0.0;

            openedge = 0;
            edgeweight = 0;

            // create logic for open edges

            edgecount = V0->edgecount;

            for(e = 0; e < V0->edgecount; e ++)
            {
                idx = V0->edges[e];
                E = &O->edges[idx / ARRAYSIZE][idx % ARRAYSIZE];

                if (E->polycount == 1)
                {
                    openedge ++;
                    Ex_o += E->B.Tx;
                    Ey_o += E->B.Ty;
                    Ez_o += E->B.Tz;
                }
                else if (E->weight != 0)
                {
                    edgeweight ++;
                    Ex_w += E->B.Tx;
                    Ey_w += E->B.Ty;
                    Ez_w += E->B.Tz;
                }

                Fx += E->Mx;
                Fy += E->My;
                Fz += E->Mz;

                Ex += E->B.Tx;
                Ey += E->B.Ty;
                Ez += E->B.Tz;
            }

            if (openedge)
            {
                Ex_o /= openedge;
                Ey_o /= openedge;
                Ez_o /= openedge;

                Tx = V->Tx;
                Ty = V->Ty;
                Tz = V->Tz;

                V->Tx = (Ex_o + V->Tx) / 2.0;
                V->Ty = (Ey_o + V->Ty) / 2.0;
                V->Tz = (Ez_o + V->Tz) / 2.0;
            }
            else if (edgeweight)
            {
                Ex_w /= edgeweight;
                Ey_w /= edgeweight;
                Ez_w /= edgeweight;

                V->Tx = (Ex_w + V->Tx) / 2.0;
                V->Ty = (Ey_w + V->Ty) / 2.0;
                V->Tz = (Ez_w + V->Tz) / 2.0;
            }
            else
            {
                Fx /= edgecount;
                Fy /= edgecount;
                Fz /= edgecount;

                Ex /= edgecount;
                Ey /= edgecount;
                Ez /= edgecount;

                V->Tx = (Fx + 2.0 * Ex + (V->edgecount - 3) * V->Tx) / edgecount;
                V->Ty = (Fy + 2.0 * Ey + (V->edgecount - 3) * V->Ty) / edgecount;
                V->Tz = (Fz + 2.0 * Ez + (V->edgecount - 3) * V->Tz) / edgecount;
            }
            if (V0->weight != 0 && edgeweight)
            {
                if (openedge)
                {
                    V->Tx = Tx * V0->weight + V->Tx * (1 - V0->weight);
                    V->Ty = Ty * V0->weight + V->Ty * (1 - V0->weight);
                    V->Tz = Tz * V0->weight + V->Tz * (1 - V0->weight);
                }
                else
                {
                    Fx /= edgecount;
                    Fy /= edgecount;
                    Fz /= edgecount;

                    Ex /= edgecount;
                    Ey /= edgecount;
                    Ez /= edgecount;

                    Tx = (Fx + 2.0 * Ex + (V->edgecount - 3) * V->Tx) / edgecount;
                    Ty = (Fy + 2.0 * Ey + (V->edgecount - 3) * V->Ty) / edgecount;
                    Tz = (Fz + 2.0 * Ez + (V->edgecount - 3) * V->Tz) / edgecount;

                    V->Tx = V->Tx * V0->weight + Tx * (1 - V0->weight);
                    V->Ty = V->Ty * V0->weight + Ty * (1 - V0->weight);
                    V->Tz = V->Tz * V0->weight + Tz * (1 - V0->weight);
                }
            }
        }
    }

    float a, b, start;

    if (O->curve_count > 0)// && L < curve_subdiv) /* level 1 blocks patches */
    {
        start = O->vertcount;

        for (e = 0; e < O->edgecount; e ++) // edge verts average surrounding polys and self position
        {
            E = &O->edges[e / ARRAYSIZE][e % ARRAYSIZE];

            if (E->S != NULL)
            {
                idx = e + start;
                V0 = &O->verts_[0][idx / ARRAYSIZE][idx % ARRAYSIZE];

                a = E->S->weight;
                b = 1 - a;

                V0->Tx = (E->S->B[0] * a + V0->Tx * b);
                V0->Ty = (E->S->B[1] * a + V0->Ty * b);
                V0->Tz = (E->S->B[2] * a + V0->Tz * b);
                /*
                if (E->S->counter_edge)
                    idx = E->verts[1];
                else
                    idx = E->verts[0];

                V = &O->verts[idx / ARRAYSIZE][idx % ARRAYSIZE];

                if (V->vert != NULL)
                {
                    V->vert->Tx = (E->S->A[0] * a + V->vert->Tx * b);
                    V->vert->Ty = (E->S->A[1] * a + V->vert->Ty * b);
                    V->vert->Tz = (E->S->A[2] * a + V->vert->Tz * b);
                }

                if (E->S->counter_edge)
                    idx = E->verts[0];
                else
                    idx = E->verts[1];

                V = &O->verts[idx / ARRAYSIZE][idx % ARRAYSIZE];

                if (V->vert != NULL)
                {
                    V->vert->Tx = (E->S->C[0] * a + V->vert->Tx * b);
                    V->vert->Ty = (E->S->C[1] * a + V->vert->Ty * b);
                    V->vert->Tz = (E->S->C[2] * a + V->vert->Tz * b);
                }
                */
            }
        }
    }
}

void tune_In_Subdivision_Shape(object * O)
{

    //O->vertcount_[L] = O->vertcount + O->edgecount + O->polycount;

    int v, e, idx;

    polygon * P0, * P1;
    vertex * V, * V0;
    edge * E;

    int c_v = O->vertcount;

    for (e = 0; e < O->edgecount; e ++) // edge verts average surrounding polys and self position
    {
        E = &O->edges[e / ARRAYSIZE][e % ARRAYSIZE];
        V = &O->verts_[0][(e + c_v) / ARRAYSIZE][(e + c_v) % ARRAYSIZE]; // edge vertex

        if (E->polycount > 1)
        {
            idx = E->polys[0];
            P0 = &O->polys[idx / ARRAYSIZE][idx % ARRAYSIZE];
            idx = E->polys[1];
            P1 = &O->polys[idx / ARRAYSIZE][idx % ARRAYSIZE];

            E->Mx = (P0->B.x + P1->B.x) / 2.0;
            E->My = (P0->B.y + P1->B.y) / 2.0;
            E->Mz = (P0->B.z + P1->B.z) / 2.0;

            V->x = (E->Mx + V->x) / 2.0;
            V->y = (E->My + V->y) / 2.0;
            V->z = (E->Mz + V->z) / 2.0;

        }
        else
        {
            E->Mx = E->B.x;
            E->My = E->B.y;
            E->Mz = E->B.z;
        }
    }

    float Fx, Fy, Fz;
    float Ex, Ey, Ez;
    float Ex_o, Ey_o, Ez_o;
    float edgecount;
    int openedge;

    for (v = 0; v < O->vertcount; v ++) // cage verts
    {
        V = &O->verts_[0][v / ARRAYSIZE][v % ARRAYSIZE]; // new level 0 cage vertexes
        V0 = &O->verts[v / ARRAYSIZE][v % ARRAYSIZE]; // old level -1 cage vertexes

        // first we scan surroundings and find distance to it as collapsed

        Fx = Fy = Fz = 0.0;
        Ex = Ey = Ez = 0.0;
        Ex_o = Ey_o = Ez_o = 0.0;

        openedge = 0;

        // create logic for open edges

        edgecount = V0->edgecount;

        for(e = 0; e < V0->edgecount; e ++)
        {
            idx = V0->edges[e];
            E = &O->edges[idx / ARRAYSIZE][idx % ARRAYSIZE];

            if (E->polycount == 1)
            {
                openedge ++;
                Ex_o += E->B.x;
                Ey_o += E->B.y;
                Ez_o += E->B.z;
            }
            Fx += E->Mx;
            Fy += E->My;
            Fz += E->Mz;

            Ex += E->B.x;
            Ey += E->B.y;
            Ez += E->B.z;
        }

        if (openedge)
        {
            Ex_o /= openedge;
            Ey_o /= openedge;
            Ez_o /= openedge;

            V->x = (Ex_o + V->x) / 2.0;
            V->y = (Ey_o + V->y) / 2.0;
            V->z = (Ez_o + V->z) / 2.0;
        }
        else
        {
            Fx /= edgecount;
            Fy /= edgecount;
            Fz /= edgecount;

            Ex /= edgecount;
            Ey /= edgecount;
            Ez /= edgecount;

            V->x = (Fx + 2.0 * Ex + (V->edgecount - 3) * V->x) / edgecount;
            V->y = (Fy + 2.0 * Ey + (V->edgecount - 3) * V->y) / edgecount;
            V->z = (Fz + 2.0 * Ez + (V->edgecount - 3) * V->z) / edgecount;
        }
    }
}

int object_Subdivide_Quads(object * O, int L)
{
    if (L < 1 || L > SUBD) return 0;

    int L1 = L - 1;

    int index = O->index;

    O->vertcount_[L] = O->vertcount_[L1] + O->edgecount_[L1] + O->quadcount_[L1];
    O->textcount_[L] = O->textcount_[L1] + O->uvedcount_[L1] + O->quadcount_[L1];

    O->quadcount_[L] = O->quadcount_[L1] * 4;

    O->edgecount_[L] = O->edgecount_[L1] * 2 + O->quadcount_[L];
    O->uvedcount_[L] = O->uvedcount_[L1] * 2 + O->quadcount_[L];
    O->tripcount_[L] = O->quadcount_[L] * 2;

    uvtex_[L][index] = (uv**)      malloc(OBJECTARRAY * sizeof(uv*));
    if (uvtex_[L][index] == NULL) return 0;
    verts_[L][index] = (vertex**)  malloc(OBJECTARRAY * sizeof(vertex*));
    if (verts_[L][index] == NULL) return 0;
    trips_[L][index] = (triangle**)malloc(OBJECTARRAY * sizeof(triangle*));
    if (trips_[L][index] == NULL) return 0;
    edges_[L][index] = (edge**)    malloc(OBJECTARRAY * sizeof(edge*));
    if (edges_[L][index] == NULL) return 0;
    uveds_[L][index] = (uv_edge**) malloc(OBJECTARRAY * sizeof(uv_edge*));
    if (uveds_[L][index] == NULL) return 0;
    quads_[L][index] = (quadrant**)malloc(OBJECTARRAY * sizeof(quadrant*));
    if (quads_[L][index] == NULL) return 0;

    int i;

    i = 0;
    while(i <= O->textcount_[L] / ARRAYSIZE)
    {
        uvtex_[L][index][i] = (uv*) calloc(ARRAYSIZE, sizeof(uv));
        if (uvtex_[L][index][i] == NULL)
        {
            O->textcount_[L] = i * ARRAYSIZE;
            return 0;
        }
        i ++;
    }

    i = 0;
    while(i <= O->vertcount_[L] / ARRAYSIZE)
    {
        verts_[L][index][i] = (vertex*) calloc(ARRAYSIZE, sizeof(vertex));
        if (verts_[L][index][i] == NULL)
        {
            O->vertcount_[L] = i * ARRAYSIZE;
            return 0;
        }
        i ++;
    }

    i = 0;
    while(i <= O->tripcount_[L] / ARRAYSIZE)
    {
        trips_[L][index][i] = (triangle*) calloc(ARRAYSIZE, sizeof(triangle));
        if (trips_[L][index][i] == NULL)
        {
            O->tripcount_[L] = i * ARRAYSIZE;
            return 0;
        }
        i ++;
    }

    i = 0;
    while(i <= O->edgecount_[L] / ARRAYSIZE)
    {
        edges_[L][index][i] = (edge*) calloc(ARRAYSIZE, sizeof(edge));
        if (edges_[L][index][i] == NULL)
        {
            O->edgecount_[L] = i * ARRAYSIZE;
            return 0;
        }
        i ++;
    }

    i = 0;
    while(i <= O->uvedcount_[L] / ARRAYSIZE)
    {
        uveds_[L][index][i] = (uv_edge*) calloc(ARRAYSIZE, sizeof(uv_edge));
        if (uveds_[L][index][i] == NULL)
        {
            O->uvedcount_[L] = i * ARRAYSIZE;
            return 0;
        }
        i ++;
    }

    i = 0;
    while(i <= O->quadcount_[L] / ARRAYSIZE)
    {
        quads_[L][index][i] = (quadrant*) calloc(ARRAYSIZE, sizeof(quadrant));
        if (quads_[L][index][i] == NULL)
        {
            O->quadcount_[L] = i * ARRAYSIZE;
            return 0;
        }
        i ++;
    }

    O->uvtex_[L] = uvtex_[L][index];
    O->verts_[L] = verts_[L][index];
    O->trips_[L] = trips_[L][index];
    O->edges_[L] = edges_[L][index];
    O->uveds_[L] = uveds_[L][index];
    O->quads_[L] = quads_[L][index];

    // initializing begins

    int v, v0, v1, t, e, q, idx, idx0, idx1, quad_c, quad_e, vert_c, edge_c, quad_i;
    edge * E, * E0, * E1;
    vertex * V, * V0;
    triangle * T;
    uv * UV, * UV0;
    uv_edge * UVE, * UVE0, * UVE1;
    quadrant * Q, * Q0;

    for (v = 0; v < O->vertcount_[L]; v ++) // all new vertexes
    {
        V = &O->verts_[L][v / ARRAYSIZE][v % ARRAYSIZE];
        V->index = v;
        V->visible = 0;
        V->weight = 0;
        V->weight_init = 0;
        V->control_point = NULL;
    }

    for (t = 0; t < O->textcount_[L]; t ++)
    {
        UV = &O->uvtex_[L][t / ARRAYSIZE][t % ARRAYSIZE];
        UV->index = t;
        UV->visible = 0;
        UV->uv_vert = NULL;
    }

    for (t = 0; t < O->tripcount_[L]; t ++)
    {
        T = &O->trips_[L][t / ARRAYSIZE][t % ARRAYSIZE];
        T->index = t;
        T->visible = 0;
    }

    for (e = 0; e < O->edgecount_[L]; e ++)
    {
        E = &O->edges_[L][e / ARRAYSIZE][e % ARRAYSIZE];
        E->index = e;
        E->visible = 0;
        E->subdivs = 0;
        E->edges[0] = -1;
        E->edges[1] = -1;
        E->S = NULL;
    }

    for (e = 0; e < O->uvedcount_[L]; e ++)
    {
        UVE = &O->uveds_[L][e / ARRAYSIZE][e % ARRAYSIZE];
        UVE->index = e;
        UVE->visible = 0;
        UVE->subdivs = 0;
        UVE->uv_edges[0] = -1;
        UVE->uv_edges[1] = -1;
    }

    for (v = 0; v < O->vertcount_[L1]; v ++) // vertex cage
    {
        V = &O->verts_[L1][v / ARRAYSIZE][v % ARRAYSIZE];
        V0 = &O->verts_[L][v / ARRAYSIZE][v % ARRAYSIZE];
        V0->edgecount = 0;
        V0->edges = malloc(V->edgecount * sizeof(int));
        if (V0->edges == NULL) return 0;

        V0->x = V->x;
        V0->y = V->y;
        V0->z = V->z;

        V0->N.x = V->N.x;
        V0->N.y = V->N.y;
        V0->N.z = V->N.z;

        V->vert = V0;

        V0->weight = V->weight;
    }

    for (t = 0; t < O->textcount_[L1]; t ++) // text cage
    {
        UV = &O->uvtex_[L1][t / ARRAYSIZE][t % ARRAYSIZE];
        UV0 = &O->uvtex_[L][t / ARRAYSIZE][t % ARRAYSIZE];
        UV0->uvcount = 0;
        UV0->uvcount_fill = UV->uvcount;
        UV0->uv_edges = malloc(UV->uvcount * sizeof(int));
        if (UV0->uv_edges == NULL) return 0;

        UV0->u = UV->u;
        UV0->v = UV->v;
        UV0->vert = UV->vert;
        UV->uv_vert = UV0;
    }

    vert_c = O->textcount_[L1];

    for (e = 0; e < O->uvedcount_[L1]; e ++) // text edge vertexes
    {
        UVE = &O->uveds_[L1][e / ARRAYSIZE][e % ARRAYSIZE];
        UV = &O->uvtex_[L][vert_c / ARRAYSIZE][vert_c % ARRAYSIZE];
        vert_c ++;
        UV->uvcount = 0;
        if (UVE->polycount > 1) // it is 3, if edge is open
            UV->uvcount_fill = 4;
        else
            UV->uvcount_fill = 3;
        UV->uv_edges = malloc(UV->uvcount_fill * sizeof(int));
        if (UV->uv_edges == NULL) return 0;
    }

    edge_c = 0;

    for (e = 0; e < O->uvedcount_[L1]; e ++) // cage text edges
    {
        UVE = &O->uveds_[L1][e / ARRAYSIZE][e % ARRAYSIZE]; // cage text edge
        UVE0 = &O->uveds_[L][edge_c / ARRAYSIZE][edge_c % ARRAYSIZE]; // edge index floats up
        UVE0->index = edge_c;
        UVE->uv_edges[0] = edge_c++;
        UVE1 = &O->uveds_[L][edge_c / ARRAYSIZE][edge_c % ARRAYSIZE];
        UVE1->index = edge_c;
        UVE->uv_edges[1] = edge_c++;

        idx = O->textcount_[L1] + e; // edges start after cage

        UVE0->uv_verts[0] = UVE->uv_verts[0];
        UVE0->uv_verts[1] = idx;
        UVE1->uv_verts[0] = idx;
        UVE1->uv_verts[1] = UVE->uv_verts[1];

        UV = &O->uvtex_[L][idx / ARRAYSIZE][idx % ARRAYSIZE];

        UV->u = UVE->B.u;
        UV->v = UVE->B.v;

        UVE0->split = UVE->split;
        UVE1->split = UVE->split;

        UVE0->polycount = 0;
        UVE1->polycount = 0;

        UVE0->edge = UVE->edge;
        UVE1->edge = UVE->edge;

//        UVE0->polys = malloc(UVE->polycount * sizeof(int));
//        if (UVE0->polys == NULL) return 0;
//        UVE1->polys = malloc(UVE->polycount * sizeof(int));
//        if (UVE1->polys == NULL) return 0;

        UVE->subdivs = 1;
    }

    for (v = O->vertcount_[L1]; v < O->vertcount_[L1] + O->edgecount_[L1]; v ++) // new edge vertexes
    {
        V = &O->verts_[L][v / ARRAYSIZE][v % ARRAYSIZE];
        V->edgecount = 0;
        V->edges = malloc(4 * sizeof(int));
        if (V->edges == NULL) return 0;
    }

    edge_c = 0;

    for (e = 0; e < O->edgecount_[L1]; e ++) // edge vertexes
    {
        E = &O->edges_[L1][e / ARRAYSIZE][e % ARRAYSIZE];
        E0 = &O->edges_[L][edge_c / ARRAYSIZE][edge_c % ARRAYSIZE];
        E0->index = edge_c;
        E->edges[0] = edge_c++;
        E1 = &O->edges_[L][edge_c / ARRAYSIZE][edge_c % ARRAYSIZE];
        E1->index = edge_c;
        E->edges[1] = edge_c++;

        idx = O->vertcount_[L1] + e;

        E0->verts[0] = E->verts[0];
        E0->verts[1] = idx;
        E1->verts[0] = idx;
        E1->verts[1] = E->verts[1];

        V = &O->verts_[L][idx / ARRAYSIZE][idx % ARRAYSIZE];

        V->x = E->B.x;
        V->y = E->B.y;
        V->z = E->B.z;

        V->N.x = E->N.x;
        V->N.y = E->N.y;
        V->N.z = E->N.z;

        E0->polycount = 0;
        E1->polycount = 0;
//        E0->polys = malloc(E->polycount * sizeof(int));
//        if (E0->polys == NULL) return 0;
//        E1->polys = malloc(E->polycount * sizeof(int));
//        if (E1->polys == NULL) return 0;

        E->subdivs = 1;

        E0->weight = E->weight;
        E1->weight = E->weight;
    }

    quad_c = 0;
    vert_c = O->vertcount_[L1] + O->edgecount_[L1];
    edge_c = O->edgecount_[L1] * 2;

    edge * EI;
    for (q = 0; q < O->quadcount_[L1]; q ++)
    {
        Q = &O->quads_[L1][q / ARRAYSIZE][q % ARRAYSIZE];
        Q->subdivs = 1;
        V = &O->verts_[L][vert_c / ARRAYSIZE][vert_c % ARRAYSIZE];
        V->index = vert_c;
        V->edgecount = 4;
        V->edges = malloc(4 * sizeof(int));
        if (V->edges == NULL) return 0;

        V->x = Q->B.x;
        V->y = Q->B.y;
        V->z = Q->B.z;

        V->N.x = Q->N.x;
        V->N.y = Q->N.y;
        V->N.z = Q->N.z;

        quad_i = quad_c;
        quad_e = edge_c;
        for (e = 0; e < 4; e++)
        {
            idx = Q->edges[e];
            E = &O->edges_[L1][idx / ARRAYSIZE][idx % ARRAYSIZE];
            Q->quads[e] = quad_c;
            Q0 = &O->quads_[L][quad_c / ARRAYSIZE][quad_c % ARRAYSIZE];
            Q0->index = quad_c;
            Q0->poly = q;
            Q0->color = Q->color;
            Q0->visible = 0;
            Q0->subdivs = 0;
            Q0->surface = Q->surface;
            Q0->selected = Q->selected;
            Q0->filler = 1;
            Q0->verts[0] = vert_c;
            Q0->verts[1] = O->vertcount_[L1] + Q->edges[(e + 3) % 4];
            Q0->verts[2] = Q->verts[e];
            Q0->verts[3] = O->vertcount_[L1] + Q->edges[e];
            V->edges[e] = edge_c;
            EI = &O->edges_[L][edge_c / ARRAYSIZE][edge_c % ARRAYSIZE];
            EI->index = edge_c;
            EI->verts[0] = Q0->verts[0];
            EI->verts[1] = Q0->verts[3];

            EI->perimeter = E;

            idx = EI->verts[1]; // perimeter vertex needs EI as its edge
            V0 = &O->verts_[L][idx / ARRAYSIZE][idx % ARRAYSIZE];
            V0->edges[V0->edgecount++] = edge_c; // perimeter
            EI->polycount = 2;
//            EI->polys = malloc(2 * sizeof(int));
//            if (EI->polys == NULL) return 0;
            EI->polys[0] = quad_c;
            EI->polys[1] = quad_i + ((e + 1) % 4);
            idx = E->edges[0];
            E0 = &O->edges_[L][idx / ARRAYSIZE][idx % ARRAYSIZE];
            idx = E->edges[1];
            E1 = &O->edges_[L][idx / ARRAYSIZE][idx % ARRAYSIZE];
            //reverse if major edge direction is clockwise

            if (E0->polycount < E->polycount)
            {
                if (Q->verts[e] != E0->verts[0])
                {
                    E0->polys[E0->polycount] = EI->polys[1];
                    E1->polys[E1->polycount] = EI->polys[0];
                }
                else
                {
                    E0->polys[E0->polycount] = EI->polys[0];
                    E1->polys[E1->polycount] = EI->polys[1];
                }
                E0->polycount++;
                E1->polycount++;
            }

            //need Q edges surrounding

            Q0->edges[0] = quad_e + (e + 3) % 4;
            Q0->edges[3] = edge_c;

            edge_c++;
            quad_c++;
        }
        vert_c ++;
    }

    int condition;

    for(e = 0; e < O->edgecount_[L1] * 2; e ++)
    {
        E = &O->edges_[L][e / ARRAYSIZE][e % ARRAYSIZE];
        idx = E->verts[0];
        V = &O->verts_[L][idx / ARRAYSIZE][idx % ARRAYSIZE];
        condition = 1;
        for(i = 0; i < V->edgecount; i ++)
        {
            if (V->edges[i] == e)
            {
                condition = 0;
                break;
            }
        }
        if (condition == 1) V->edges[V->edgecount++] = e;
        idx = E->verts[1];
        V = &O->verts_[L][idx / ARRAYSIZE][idx % ARRAYSIZE];
        condition = 1;
        for(i = 0; i < V->edgecount; i ++)
        {
            if (V->edges[i] == e)
            {
                condition = 0;
                break;
            }
        }
        if (condition == 1) V->edges[V->edgecount++] = e;

        for (q = 0; q < E->polycount; q ++)
        {
            idx = E->polys[q];
            Q = &O->quads_[L][idx / ARRAYSIZE][idx % ARRAYSIZE];
            Q->edges[Q->filler++] = e;
        }
    }

    quad_c = 0; // cycling quads
    vert_c = O->textcount_[L1] + O->uvedcount_[L1]; // text polygon vertexes start after cage and edges
    edge_c = O->uvedcount_[L1] * 2; // inside edges start after split cage edges

    uv_edge * UVEI;
    for (q = 0; q < O->quadcount_[L1]; q ++)
    {
        Q = &O->quads_[L1][q / ARRAYSIZE][q % ARRAYSIZE];
        UV = &O->uvtex_[L][vert_c / ARRAYSIZE][vert_c % ARRAYSIZE];
        UV->index = vert_c;
        UV->uvcount = 4;
        UV->uv_edges = malloc(UV->uvcount * sizeof(int));
        if (UV->uv_edges == NULL) return 0;

        UV->u = Q->B2.u;
        UV->v = Q->B2.v;

        quad_i = quad_c;
        quad_e = edge_c;
        for (e = 0; e < 4; e++)
        {
            idx = Q->uveds[e];
            UVE = &O->uveds_[L1][idx / ARRAYSIZE][idx % ARRAYSIZE];
            Q0 = &O->quads_[L][quad_c / ARRAYSIZE][quad_c % ARRAYSIZE];
            Q0->texts[0] = vert_c;
            Q0->texts[1] = O->textcount_[L1] + Q->uveds[(e + 3) % 4];
            Q0->texts[2] = Q->texts[e];
            Q0->texts[3] = O->textcount_[L1] + Q->uveds[e];
            Q0->filler = 1;
            UV->uv_edges[e] = edge_c;
            UVEI = &O->uveds_[L][edge_c / ARRAYSIZE][edge_c % ARRAYSIZE];
            UVEI->index = edge_c;
            UVEI->uv_verts[0] = Q0->texts[0];
            UVEI->uv_verts[1] = Q0->texts[3];
            idx = UVEI->uv_verts[1];  // perimeter vertex needs UVEI as its edge
            UV0 = &O->uvtex_[L][idx / ARRAYSIZE][idx % ARRAYSIZE];
            UV0->uv_edges[UV0->uvcount++] = edge_c; // perimeter
            UVEI->polycount = 2;
//            UVEI->polys = malloc(2 * sizeof(int));
//            if (UVEI->polys == NULL) return 0;
            UVEI->polys[0] = quad_c; // inside texture edge polygon 0
            UVEI->polys[1] = quad_i + ((e + 1) % 4);
            idx = UVE->uv_edges[0];
            UVE0 = &O->uveds_[L][idx / ARRAYSIZE][idx % ARRAYSIZE];
            idx = UVE->uv_edges[1];
            UVE1 = &O->uveds_[L][idx / ARRAYSIZE][idx % ARRAYSIZE];
            //reverse if major edge direction is clockwise

            if (UVE0->polycount < UVE->polycount)
            {
                if (Q->texts[e] != UVE0->uv_verts[0])
                {
                    UVE0->polys[UVE0->polycount] = UVEI->polys[1];
                    UVE1->polys[UVE1->polycount] = UVEI->polys[0];
                }
                else
                {
                    UVE0->polys[UVE0->polycount] = UVEI->polys[0];
                    UVE1->polys[UVE1->polycount] = UVEI->polys[1];
                }

                UVE0->polycount++;
                UVE1->polycount++;
            }
            //need Q edges surrounding

            Q0->uveds[0] = quad_e + (e + 3) % 4;
            Q0->uveds[3] = edge_c;

            edge_c++;
            quad_c++;
        }
        vert_c ++;
    }

    for(e = 0; e < O->uvedcount_[L1] * 2; e ++)
    {
        UVE = &O->uveds_[L][e / ARRAYSIZE][e % ARRAYSIZE];
        idx = UVE->uv_verts[0];
        UV = &O->uvtex_[L][idx / ARRAYSIZE][idx % ARRAYSIZE];
        if (UV->uvcount < UV->uvcount_fill)
        {
            condition = 1;
            for(i = 0; i < UV->uvcount; i ++)
            {
                if (UV->uv_edges[i] == e)
                {
                    condition = 0;
                    break;
                }
            }
            if (condition == 1) UV->uv_edges[UV->uvcount++] = e;
        }
        idx = UVE->uv_verts[1];
        UV = &O->uvtex_[L][idx / ARRAYSIZE][idx % ARRAYSIZE];
        if (UV->uvcount < UV->uvcount_fill)
        {
            condition = 1;
            for(i = 0; i < UV->uvcount; i ++)
            {
                if (UV->uv_edges[i] == e)
                {
                    condition = 0;
                    break;
                }
            }
            if (condition == 1) UV->uv_edges[UV->uvcount++] = e;
        }
        for (q = 0; q < UVE->polycount; q ++)
        {
            idx = UVE->polys[q];
            Q = &O->quads_[L][idx / ARRAYSIZE][idx % ARRAYSIZE];
            Q->uveds[Q->filler++] = e;
        }
    }

    for (q = 0; q < O->quadcount_[L]; q ++) // quad edge swap
    {
        Q = &O->quads_[L][q / ARRAYSIZE][q % ARRAYSIZE];
        idx = Q->edges[1];
        E = &O->edges_[L][idx / ARRAYSIZE][idx % ARRAYSIZE];
        idx0 = E->verts[0];
        idx1 = E->verts[1];
        v0 = Q->verts[1];
        v1 = Q->verts[2];
        condition = 1;
        if ((v0 == idx0 && v1 == idx1) || (v1 == idx0 && v0 == idx1))
        {
            condition = 0;
        }
        if (condition)
        {
            Q->edges[1] = Q->edges[2];
            Q->edges[2] = idx;
        }

        idx = Q->uveds[1];
        UVE = &O->uveds_[L][idx / ARRAYSIZE][idx % ARRAYSIZE];
        idx0 = UVE->uv_verts[0];
        idx1 = UVE->uv_verts[1];
        v0 = Q->texts[1];
        v1 = Q->texts[2];
        condition = 1;
        if ((v0 == idx0 && v1 == idx1) || (v1 == idx0 && v0 == idx1))
        {
            condition = 0;
        }
        if (condition)
        {
            Q->uveds[1] = Q->uveds[2];
            Q->uveds[2] = idx;
        }
    }

    return 1;

}

////////////////

int object_Subdivide(object * O, int L)
{
    int index = O->index;
    O->vertcount_[L] = O->vertcount + O->edgecount + O->polycount;
    O->textcount_[L] = O->textcount + O->uvedcount + O->polycount;

    O->quadcount_[L] = 0;
    int p;
    polygon * P;
    for (p = 0; p < O->polycount; p ++)
    {
        P = &O->polys[p / ARRAYSIZE][p % ARRAYSIZE];
        O->quadcount_[L] += P->edgecount;
    }

    O->edgecount_[L] = O->edgecount * 2 + O->quadcount_[L];
    O->uvedcount_[L] = O->uvedcount * 2 + O->quadcount_[L];
    O->tripcount_[L] = O->quadcount_[L] * 2;

    uvtex_[L][index] = (uv**)      malloc(OBJECTARRAY * sizeof(uv*));
    if (uvtex_[L][index] == NULL) return 0;
    verts_[L][index] = (vertex**)  malloc(OBJECTARRAY * sizeof(vertex*));
    if (verts_[L][index] == NULL) return 0;
    trips_[L][index] = (triangle**)malloc(OBJECTARRAY * sizeof(triangle*));
    if (trips_[L][index] == NULL) return 0;
    edges_[L][index] = (edge**)    malloc(OBJECTARRAY * sizeof(edge*));
    if (edges_[L][index] == NULL) return 0;
    uveds_[L][index] = (uv_edge**) malloc(OBJECTARRAY * sizeof(uv_edge*));
    if (uveds_[L][index] == NULL) return 0;
    quads_[L][index] = (quadrant**)malloc(OBJECTARRAY * sizeof(quadrant*));
    if (quads_[L][index] == NULL) return 0;

    int i;

    i = 0;
    while(i <= O->textcount_[L] / ARRAYSIZE)
    {
        uvtex_[L][index][i] = (uv*) calloc(ARRAYSIZE, sizeof(uv));
        if (uvtex_[L][index][i] == NULL)
        {
            O->textcount_[L] = i * ARRAYSIZE;
            return 0;
        }
        i ++;
    }

    i = 0;
    while(i <= O->vertcount_[L] / ARRAYSIZE)
    {
        verts_[L][index][i] = (vertex*) calloc(ARRAYSIZE, sizeof(vertex));
        if (verts_[L][index][i] == NULL)
        {
            O->vertcount_[L] = i * ARRAYSIZE;
            return 0;
        }
        i ++;
    }

    i = 0;
    while(i <= O->tripcount_[L] / ARRAYSIZE)
    {
        trips_[L][index][i] = (triangle*) calloc(ARRAYSIZE, sizeof(triangle));
        if (trips_[L][index][i] == NULL)
        {
            O->tripcount_[L] = i * ARRAYSIZE;
            return 0;
        }
        i ++;
    }

    i = 0;
    while(i <= O->edgecount_[L] / ARRAYSIZE)
    {
        edges_[L][index][i] = (edge*) calloc(ARRAYSIZE, sizeof(edge));
        if (edges_[L][index][i] == NULL)
        {
            O->edgecount_[L] = i * ARRAYSIZE;
            return 0;
        }
        i ++;
    }

    i = 0;
    while(i <= O->uvedcount_[L] / ARRAYSIZE)
    {
        uveds_[L][index][i] = (uv_edge*) calloc(ARRAYSIZE, sizeof(uv_edge));
        if (uveds_[L][index][i] == NULL)
        {
            O->uvedcount_[L] = i * ARRAYSIZE;
            return 0;
        }
        i ++;
    }

    i = 0;
    while(i <= O->quadcount_[L] / ARRAYSIZE)
    {
        quads_[L][index][i] = (quadrant*) calloc(ARRAYSIZE, sizeof(quadrant));
        if (quads_[L][index][i] == NULL)
        {
            O->quadcount_[L] = i * ARRAYSIZE;
            return 0;
        }
        i ++;
    }

    O->uvtex_[L] = uvtex_[L][index];
    O->verts_[L] = verts_[L][index];
    O->trips_[L] = trips_[L][index];
    O->edges_[L] = edges_[L][index];
    O->uveds_[L] = uveds_[L][index];
    O->quads_[L] = quads_[L][index];

    int v, v0, v1, t, e, q, idx, idx0, idx1, quad_c, quad_e, vert_c, edge_c, quad_i;
    edge * E, * E0, * E1;
    vertex * V, * V0;
    triangle * T;
    uv * UV, * UV0;
    uv_edge * UVE, * UVE0, * UVE1;
    quadrant * Q;

    for (v = 0; v < O->vertcount_[L]; v ++)
    {
        V = &O->verts_[L][v / ARRAYSIZE][v % ARRAYSIZE];
        V->index = v;
        V->visible = 0;
        V->weight = 0;
        V->weight_init = 0;
        V->control_point = NULL;
    }

    for (t = 0; t < O->textcount_[L]; t ++)
    {
        UV = &O->uvtex_[L][t / ARRAYSIZE][t % ARRAYSIZE];
        UV->index = t;
        UV->visible = 0;
        UV->uv_vert = NULL;
    }

    for (t = 0; t < O->tripcount_[L]; t ++)
    {
        T = &O->trips_[L][t / ARRAYSIZE][t % ARRAYSIZE];
        T->index = t;
        T->visible = 0;
    }

    for (e = 0; e < O->edgecount_[L]; e ++)
    {
        E = &O->edges_[L][e / ARRAYSIZE][e % ARRAYSIZE];
        E->index = e;
        E->visible = 0;
        E->subdivs = 0;
        E->edges[0] = -1;
        E->edges[1] = -1;
        E->S = NULL;
    }

    for (e = 0; e < O->uvedcount_[L]; e ++)
    {
        UVE = &O->uveds_[L][e / ARRAYSIZE][e % ARRAYSIZE];
        UVE->index = e;
        UVE->visible = 0;
        UVE->subdivs = 0;
        UVE->uv_edges[0] = -1;
        UVE->uv_edges[1] = -1;
    }

    for (v = 0; v < O->vertcount; v ++)
    {
        V = &O->verts[v / ARRAYSIZE][v % ARRAYSIZE];
        V0 = &O->verts_[L][v / ARRAYSIZE][v % ARRAYSIZE];
        V0->edgecount = 0;
        V0->edges = malloc(V->edgecount * sizeof(int));
        if (V0->edges == NULL) return 0;

        V0->x = V->x;
        V0->y = V->y;
        V0->z = V->z;

        V0->N.x = V->N.x;
        V0->N.y = V->N.y;
        V0->N.z = V->N.z;

        V->vert = V0;

        V0->weight = V->weight;
    }

    for (t = 0; t < O->textcount; t ++)
    {
        UV = &O->uvtex[t / ARRAYSIZE][t % ARRAYSIZE];
        UV0 = &O->uvtex_[L][t / ARRAYSIZE][t % ARRAYSIZE];
        UV0->uvcount = 0;
        UV0->uvcount_fill = UV->uvcount;
        UV0->uv_edges = malloc(UV->uvcount * sizeof(int));
        if (UV0->uv_edges == NULL) return 0;

        UV0->u = UV->u;
        UV0->v = UV->v;
        UV0->vert = UV->vert;
        UV->uv_vert = UV0;
    }

    vert_c = O->textcount;

    for (e = 0; e < O->uvedcount; e ++)
    {
        UVE = &O->uveds[e / ARRAYSIZE][e % ARRAYSIZE];
        UV = &O->uvtex_[L][vert_c / ARRAYSIZE][vert_c % ARRAYSIZE];
        vert_c ++;
        UV->uvcount = 0;
        if (UVE->polycount > 1)
            UV->uvcount_fill = 4;
        else
            UV->uvcount_fill = 3;
        UV->uv_edges = malloc(UV->uvcount_fill * sizeof(int)); // it may be also 3, if edge is open, but not more
        if (UV->uv_edges == NULL) return 0;
    }

    edge_c = 0;

    for (e = 0; e < O->uvedcount; e ++)
    {
        UVE = &O->uveds[e / ARRAYSIZE][e % ARRAYSIZE];
        UVE0 = &O->uveds_[L][edge_c / ARRAYSIZE][edge_c % ARRAYSIZE];
        UVE0->index = edge_c;
        UVE->uv_edges[0] = edge_c++;
        UVE1 = &O->uveds_[L][edge_c / ARRAYSIZE][edge_c % ARRAYSIZE];
        UVE1->index = edge_c;
        UVE->uv_edges[1] = edge_c++;

        idx = O->textcount + e;

        UVE0->uv_verts[0] = UVE->uv_verts[0];
        UVE0->uv_verts[1] = idx;
        UVE1->uv_verts[0] = idx;
        UVE1->uv_verts[1] = UVE->uv_verts[1];

        UV = &O->uvtex_[L][idx / ARRAYSIZE][idx % ARRAYSIZE];

        UV->u = UVE->B.u;
        UV->v = UVE->B.v;

        UVE0->split = UVE->split;
        UVE1->split = UVE->split;

        UVE0->polycount = 0;
        UVE1->polycount = 0;

        UVE0->edge = UVE->edge;
        UVE1->edge = UVE->edge;

//        UVE0->polys = malloc(UVE->polycount * sizeof(int));
//        if (UVE0->polys == NULL) return 0;
//        UVE1->polys = malloc(UVE->polycount * sizeof(int));
//        if (UVE1->polys == NULL) return 0;

        UVE->subdivs = 1;
    }

    for (v = O->vertcount; v < O->vertcount + O->edgecount; v ++) // new edge vertexes
    {
        V = &O->verts_[L][v / ARRAYSIZE][v % ARRAYSIZE];
        V->edgecount = 0;
        V->edges = malloc(4 * sizeof(int));
        if (V->edges == NULL) return 0;
    }

    edge_c = 0;

    for (e = 0; e < O->edgecount; e ++)
    {
        E = &O->edges[e / ARRAYSIZE][e % ARRAYSIZE];
        E0 = &O->edges_[L][edge_c / ARRAYSIZE][edge_c % ARRAYSIZE];
        E0->index = edge_c;
        E->edges[0] = edge_c++;
        E1 = &O->edges_[L][edge_c / ARRAYSIZE][edge_c % ARRAYSIZE];
        E1->index = edge_c;
        E->edges[1] = edge_c++;

        idx = O->vertcount + e;

        E0->verts[0] = E->verts[0];
        E0->verts[1] = idx;
        E1->verts[0] = idx;
        E1->verts[1] = E->verts[1];

        V = &O->verts_[L][idx / ARRAYSIZE][idx % ARRAYSIZE];

        V->x = E->B.x;
        V->y = E->B.y;
        V->z = E->B.z;

        V->N.x = E->N.x;
        V->N.y = E->N.y;
        V->N.z = E->N.z;

        E0->polycount = 0;
        E1->polycount = 0;
//        E0->polys = malloc(E->polycount * sizeof(int));
//        if (E0->polys == NULL) return 0;
//        E1->polys = malloc(E->polycount * sizeof(int));
//        if (E1->polys == NULL) return 0;

        E->subdivs = 1;

        E0->weight = E->weight;
        E1->weight = E->weight;
    }

    quad_c = 0;
    vert_c = O->vertcount + O->edgecount;
    edge_c = O->edgecount * 2;

    edge * EI;
    for (p = 0; p < O->polycount; p ++)
    {
        P = &O->polys[p / ARRAYSIZE][p % ARRAYSIZE];
        P->subdivs = 1;
        V = &O->verts_[L][vert_c / ARRAYSIZE][vert_c % ARRAYSIZE];
        V->index = vert_c;
        V->edgecount = P->edgecount;
        V->edges = malloc(V->edgecount * sizeof(int));
        if (V->edges == NULL) return 0;

        V->x = P->B.x;
        V->y = P->B.y;
        V->z = P->B.z;

        V->N.x = P->N.x;
        V->N.y = P->N.y;
        V->N.z = P->N.z;

        quad_i = quad_c;
        quad_e = edge_c;
        for (e = 0; e < P->edgecount; e++)
        {
            idx = P->edges[e];
            E = &O->edges[idx / ARRAYSIZE][idx % ARRAYSIZE];
            P->quads[e] = quad_c;
            Q = &O->quads_[L][quad_c / ARRAYSIZE][quad_c % ARRAYSIZE];
            Q->index = quad_c;
            Q->poly = p;
            Q->color = P->color;
            Q->visible = 0;
            Q->subdivs = 0;
            Q->surface = P->surface;
            Q->selected = P->selected;
            Q->filler = 1;
            Q->verts[0] = vert_c;
            Q->verts[1] = O->vertcount + P->edges[(e + P->edgecount - 1) % P->edgecount];
            Q->verts[2] = P->verts[e];
            Q->verts[3] = O->vertcount + P->edges[e];
            V->edges[e] = edge_c;
            EI = &O->edges_[L][edge_c / ARRAYSIZE][edge_c % ARRAYSIZE];
            EI->index = edge_c;
            EI->verts[0] = Q->verts[0];
            EI->verts[1] = Q->verts[3];

            EI->perimeter = E;

            idx = EI->verts[1]; // perimeter vertex needs EI as its edge
            V0 = &O->verts_[L][idx / ARRAYSIZE][idx % ARRAYSIZE];
            V0->edges[V0->edgecount++] = edge_c; // perimeter
            EI->polycount = 2;
//            EI->polys = malloc(2 * sizeof(int));
//            if (EI->polys == NULL) return 0;
            EI->polys[0] = quad_c;
            EI->polys[1] = quad_i + ((e + 1) % P->edgecount);
            idx = E->edges[0];
            E0 = &O->edges_[L][idx / ARRAYSIZE][idx % ARRAYSIZE];
            idx = E->edges[1];
            E1 = &O->edges_[L][idx / ARRAYSIZE][idx % ARRAYSIZE];
            //reverse if major edge direction is clockwise

            if (E0->polycount < E->polycount)
            {
                if (P->verts[e] != E0->verts[0])
                {
                    E0->polys[E0->polycount] = EI->polys[1];
                    E1->polys[E1->polycount] = EI->polys[0];
                }
                else
                {
                    E0->polys[E0->polycount] = EI->polys[0];
                    E1->polys[E1->polycount] = EI->polys[1];
                }
                E0->polycount++;
                E1->polycount++;
            }

            //need Q edges surrounding

            Q->edges[0] = quad_e + (e + P->edgecount - 1) % P->edgecount;
            Q->edges[3] = edge_c;

            edge_c++;
            quad_c++;
        }
        vert_c ++;
    }

    int condition;

    for(e = 0; e < O->edgecount * 2; e ++) // edge vertex gets edge connections
    {
        E = &O->edges_[L][e / ARRAYSIZE][e % ARRAYSIZE];
        idx = E->verts[0];
        V = &O->verts_[L][idx / ARRAYSIZE][idx % ARRAYSIZE];
        condition = 1;
        for(i = 0; i < V->edgecount; i ++)
        {
            if (V->edges[i] == e)
            {
                condition = 0;
                break;
            }
        }
        if (condition == 1) V->edges[V->edgecount++] = e;
        idx = E->verts[1];
        V = &O->verts_[L][idx / ARRAYSIZE][idx % ARRAYSIZE];
        condition = 1;
        for(i = 0; i < V->edgecount; i ++)
        {
            if (V->edges[i] == e)
            {
                condition = 0;
                break;
            }
        }
        if (condition == 1) V->edges[V->edgecount++] = e;

        for (p = 0; p < E->polycount; p ++) // quads get edges
        {
            idx = E->polys[p];
            Q = &O->quads_[L][idx / ARRAYSIZE][idx % ARRAYSIZE];
            Q->edges[Q->filler++] = e;
        }
    }

    quad_c = 0;
    vert_c = O->textcount + O->uvedcount; // polygon center text vertexes start after cage and edges vertexes
    edge_c = O->uvedcount * 2; // starting after subdivided edges in edge array

    uv_edge * UVEI;
    for (p = 0; p < O->polycount; p ++)
    {
        P = &O->polys[p / ARRAYSIZE][p % ARRAYSIZE];
        UV = &O->uvtex_[L][vert_c / ARRAYSIZE][vert_c % ARRAYSIZE];
        UV->index = vert_c;
        UV->uvcount = P->edgecount;
        UV->uv_edges = malloc(UV->uvcount * sizeof(int));
        if (UV->uv_edges == NULL) return 0;

        UV->u = P->B2.u;
        UV->v = P->B2.v;

        quad_i = quad_c;
        quad_e = edge_c;
        for (e = 0; e < P->edgecount; e++)
        {
            idx = P->uveds[e];
            UVE = &O->uveds[idx / ARRAYSIZE][idx % ARRAYSIZE];
            Q = &O->quads_[L][quad_c / ARRAYSIZE][quad_c % ARRAYSIZE];
            Q->texts[0] = vert_c;
            Q->texts[1] = O->textcount + P->uveds[(e + P->edgecount - 1) % P->edgecount];
            Q->texts[2] = P->texts[e];
            Q->texts[3] = O->textcount + P->uveds[e];
            Q->filler = 1;
            UV->uv_edges[e] = edge_c;
            UVEI = &O->uveds_[L][edge_c / ARRAYSIZE][edge_c % ARRAYSIZE];
            UVEI->index = edge_c;
            UVEI->uv_verts[0] = Q->texts[0];
            UVEI->uv_verts[1] = Q->texts[3];
            idx = UVEI->uv_verts[1]; // perimeter vertex needs UVEI as its edge
            UV0 = &O->uvtex_[L][idx / ARRAYSIZE][idx % ARRAYSIZE];
            UV0->uv_edges[UV0->uvcount++] = edge_c; // perimeter
            UVEI->polycount = 2;
//            UVEI->polys = malloc(2 * sizeof(int));
//            if (UVEI->polys == NULL) return 0;
            UVEI->polys[0] = quad_c;
            UVEI->polys[1] = quad_i + ((e + 1) % P->edgecount);
            idx = UVE->uv_edges[0];
            UVE0 = &O->uveds_[L][idx / ARRAYSIZE][idx % ARRAYSIZE];
            idx = UVE->uv_edges[1];
            UVE1 = &O->uveds_[L][idx / ARRAYSIZE][idx % ARRAYSIZE];
            //reverse if major edge direction is clockwise

            if (UVE0->polycount < UVE->polycount)
            {
                if (P->texts[e] != UVE0->uv_verts[0])
                {
                    UVE0->polys[UVE0->polycount] = UVEI->polys[1];
                    UVE1->polys[UVE1->polycount] = UVEI->polys[0];
                }
                else
                {
                    UVE0->polys[UVE0->polycount] = UVEI->polys[0];
                    UVE1->polys[UVE1->polycount] = UVEI->polys[1];
                }

                UVE0->polycount++;
                UVE1->polycount++;
            }
            //need Q edges surrounding

            Q->uveds[0] = quad_e + (e + P->edgecount - 1) % P->edgecount;
            Q->uveds[3] = edge_c;

            edge_c++;
            quad_c++;
        }
        vert_c ++;
    }

    for(e = 0; e < O->uvedcount * 2; e ++) // edge verts get filled
    {
        UVE = &O->uveds_[L][e / ARRAYSIZE][e % ARRAYSIZE];
        idx = UVE->uv_verts[0];
        UV = &O->uvtex_[L][idx / ARRAYSIZE][idx % ARRAYSIZE];
        if (UV->uvcount < UV->uvcount_fill)
        {
            condition = 1;
            for(i = 0; i < UV->uvcount; i ++)
            {
                if (UV->uv_edges[i] == e)
                {
                    condition = 0;
                    break;
                }
            }
            if (condition == 1) UV->uv_edges[UV->uvcount++] = e;
        }
        idx = UVE->uv_verts[1];
        UV = &O->uvtex_[L][idx / ARRAYSIZE][idx % ARRAYSIZE];
        if (UV->uvcount < UV->uvcount_fill)
        {
            condition = 1;
            for(i = 0; i < UV->uvcount; i ++)
            {
                if (UV->uv_edges[i] == e)
                {
                    condition = 0;
                    break;
                }
            }
            if (condition == 1) UV->uv_edges[UV->uvcount++] = e;
        }
        for (p = 0; p < UVE->polycount; p ++)
        {
            idx = UVE->polys[p];
            Q = &O->quads_[L][idx / ARRAYSIZE][idx % ARRAYSIZE];
            Q->uveds[Q->filler++] = e;
        }
    }

    for (q = 0; q < O->quadcount_[L]; q ++) // quad edge swap, rim edge
    {
        Q = &O->quads_[L][q / ARRAYSIZE][q % ARRAYSIZE];
        idx = Q->edges[1];
        E = &O->edges_[L][idx / ARRAYSIZE][idx % ARRAYSIZE];
        idx0 = E->verts[0];
        idx1 = E->verts[1];
        v0 = Q->verts[1];
        v1 = Q->verts[2];
        condition = 1;
        if ((v0 == idx0 && v1 == idx1) || (v1 == idx0 && v0 == idx1))
        {
            condition = 0;
        }
        if (condition)
        {
            Q->edges[1] = Q->edges[2];
            Q->edges[2] = idx;
        }

        idx = Q->uveds[1];
        UVE = &O->uveds_[L][idx / ARRAYSIZE][idx % ARRAYSIZE];
        idx0 = UVE->uv_verts[0];
        idx1 = UVE->uv_verts[1];
        v0 = Q->texts[1];
        v1 = Q->texts[2];
        condition = 1;
        if ((v0 == idx0 && v1 == idx1) || (v1 == idx0 && v0 == idx1))
        {
            condition = 0;
        }
        if (condition)
        {
            Q->uveds[1] = Q->uveds[2];
            Q->uveds[2] = idx;
        }
    }

    /*TODO:
    As result, edge count new polys are created.
    Topologically they reside in bounds of the initial polyface.
    They are connected in the center to new center vertex.
    And to the new edge verts. Poly edges are also subdivided.
    If they are already subdivided, the center vertex is used.
    To calculate vertex normals correctly cage position must be used
    and remembered.
    */
    // calculate center
    // add edge mean vertex
    return 1;

}

void generate_cage_for_Subdivision_transformed(object * O, int L)
{
    int p;
    polygon * P;
    int v, e, idx, vert_c;
    edge * E;
    vertex * V, * V0;

    for (v = 0; v < O->vertcount; v ++)
    {
        V = &O->verts[v / ARRAYSIZE][v % ARRAYSIZE];
        V0 = &O->verts_[L][v / ARRAYSIZE][v % ARRAYSIZE];

        V0->Tx = V->Tx;
        V0->Ty = V->Ty;
        V0->Tz = V->Tz;

        V0->N.Tx = V->N.Tx;
        V0->N.Ty = V->N.Ty;
        V0->N.Tz = V->N.Tz;
    }

    for (e = 0; e < O->edgecount; e ++)
    {
        E = &O->edges[e / ARRAYSIZE][e % ARRAYSIZE];

        idx = O->vertcount + e;

        V = &O->verts_[L][idx / ARRAYSIZE][idx % ARRAYSIZE];

        V->Tx = E->B.Tx;
        V->Ty = E->B.Ty;
        V->Tz = E->B.Tz;

        V->N.Tx = E->N.Tx;
        V->N.Ty = E->N.Ty;
        V->N.Tz = E->N.Tz;
    }

    vert_c = O->vertcount + O->edgecount;

    for (p = 0; p < O->polycount; p ++)
    {
        P = &O->polys[p / ARRAYSIZE][p % ARRAYSIZE];
        V = &O->verts_[L][vert_c / ARRAYSIZE][vert_c % ARRAYSIZE];

        V->Tx = P->B.Tx;
        V->Ty = P->B.Ty;
        V->Tz = P->B.Tz;

        V->N.Tx = P->N.Tx;
        V->N.Ty = P->N.Ty;
        V->N.Tz = P->N.Tz;

        vert_c ++;
    }
}

void generate_cage_for_Subdivision(object * O, int L)
{
    int p;
    polygon * P;
    int v, t, e, idx, vert_c;
    edge * E;
    vertex * V, * V0;
    uv * UV, * UV0;
    uv_edge * UVE;

    for (v = 0; v < O->vertcount; v ++)
    {
        V = &O->verts[v / ARRAYSIZE][v % ARRAYSIZE];
        V0 = &O->verts_[L][v / ARRAYSIZE][v % ARRAYSIZE];

        V0->x = V->x;
        V0->y = V->y;
        V0->z = V->z;

        V0->N.x = V->N.x;
        V0->N.y = V->N.y;
        V0->N.z = V->N.z;
    }

    for (t = 0; t < O->textcount; t ++)
    {
        UV = &O->uvtex[t / ARRAYSIZE][t % ARRAYSIZE];
        UV0 = &O->uvtex_[L][t / ARRAYSIZE][t % ARRAYSIZE];

        UV0->u = UV->u;
        UV0->v = UV->v;
    }

    for (e = 0; e < O->uvedcount; e ++)
    {
        UVE = &O->uveds[e / ARRAYSIZE][e % ARRAYSIZE];

        idx = O->textcount + e;

        UV = &O->uvtex_[L][idx / ARRAYSIZE][idx % ARRAYSIZE];

        UV->u = UVE->B.u;
        UV->v = UVE->B.v;
    }

    for (e = 0; e < O->edgecount; e ++)
    {
        E = &O->edges[e / ARRAYSIZE][e % ARRAYSIZE];

        idx = O->vertcount + e;

        V = &O->verts_[L][idx / ARRAYSIZE][idx % ARRAYSIZE];

        V->x = E->B.x;
        V->y = E->B.y;
        V->z = E->B.z;

        V->N.x = E->N.x;
        V->N.y = E->N.y;
        V->N.z = E->N.z;
    }

    vert_c = O->vertcount + O->edgecount;

    for (p = 0; p < O->polycount; p ++)
    {
        P = &O->polys[p / ARRAYSIZE][p % ARRAYSIZE];
        V = &O->verts_[L][vert_c / ARRAYSIZE][vert_c % ARRAYSIZE];

        V->x = P->B.x;
        V->y = P->B.y;
        V->z = P->B.z;

        V->N.x = P->N.x;
        V->N.y = P->N.y;
        V->N.z = P->N.z;

        vert_c ++;
    }

    vert_c = O->textcount + O->uvedcount; // polygon center text vertexes start after cage and edges vertexes

    for (p = 0; p < O->polycount; p ++)
    {
        P = &O->polys[p / ARRAYSIZE][p % ARRAYSIZE];
        UV = &O->uvtex_[L][vert_c / ARRAYSIZE][vert_c % ARRAYSIZE];

        UV->u = P->B2.u;
        UV->v = P->B2.v;

        vert_c ++;
    }
}

void generate_cage_for_Subdivision_Quads_transformed(object * O, int L)
{
    int L1 = L - 1;

    int v, e, q, idx, vert_c;
    edge * E;
    vertex * V, * V0;
    quadrant * Q;

    for (v = 0; v < O->vertcount_[L1]; v ++) // vertex cage
    {
        V = &O->verts_[L1][v / ARRAYSIZE][v % ARRAYSIZE];
        V0 = &O->verts_[L][v / ARRAYSIZE][v % ARRAYSIZE];

        V0->Tx = V->Tx;
        V0->Ty = V->Ty;
        V0->Tz = V->Tz;

        V0->N.Tx = V->N.Tx;
        V0->N.Ty = V->N.Ty;
        V0->N.Tz = V->N.Tz;
    }

    for (e = 0; e < O->edgecount_[L1]; e ++) // edge vertexes
    {
        E = &O->edges_[L1][e / ARRAYSIZE][e % ARRAYSIZE];

        idx = O->vertcount_[L1] + e;

        V = &O->verts_[L][idx / ARRAYSIZE][idx % ARRAYSIZE];

        V->Tx = E->B.Tx;
        V->Ty = E->B.Ty;
        V->Tz = E->B.Tz;

        V->N.Tx = E->N.Tx;
        V->N.Ty = E->N.Ty;
        V->N.Tz = E->N.Tz;
    }

    vert_c = O->vertcount_[L1] + O->edgecount_[L1];

    for (q = 0; q < O->quadcount_[L1]; q ++)
    {
        Q = &O->quads_[L1][q / ARRAYSIZE][q % ARRAYSIZE];
        V = &O->verts_[L][vert_c / ARRAYSIZE][vert_c % ARRAYSIZE];

        V->Tx = Q->B.Tx;
        V->Ty = Q->B.Ty;
        V->Tz = Q->B.Tz;

        V->N.Tx = Q->N.Tx;
        V->N.Ty = Q->N.Ty;
        V->N.Tz = Q->N.Tz;

        vert_c ++;
    }
}

void generate_cage_for_Subdivision_Quads(object * O, int L)
{
    int L1 = L - 1;

    int v, t, e, q, idx, vert_c;
    edge * E;
    vertex * V, * V0;
    uv * UV, * UV0;
    uv_edge * UVE;
    quadrant * Q;

    for (v = 0; v < O->vertcount_[L1]; v ++) // vertex cage
    {
        V = &O->verts_[L1][v / ARRAYSIZE][v % ARRAYSIZE];
        V0 = &O->verts_[L][v / ARRAYSIZE][v % ARRAYSIZE];

        V0->x = V->x;
        V0->y = V->y;
        V0->z = V->z;

        V0->N.x = V->N.x;
        V0->N.y = V->N.y;
        V0->N.z = V->N.z;
    }

    for (t = 0; t < O->textcount_[L1]; t ++) // text cage
    {
        UV = &O->uvtex_[L1][t / ARRAYSIZE][t % ARRAYSIZE];
        UV0 = &O->uvtex_[L][t / ARRAYSIZE][t % ARRAYSIZE];

        UV0->u = UV->u;
        UV0->v = UV->v;
    }

    for (e = 0; e < O->uvedcount_[L1]; e ++) // cage text edges
    {
        UVE = &O->uveds_[L1][e / ARRAYSIZE][e % ARRAYSIZE]; // cage text edge

        idx = O->textcount_[L1] + e; // edges start after cage

        UV = &O->uvtex_[L][idx / ARRAYSIZE][idx % ARRAYSIZE];

        UV->u = UVE->B.u;
        UV->v = UVE->B.v;
    }

    for (e = 0; e < O->edgecount_[L1]; e ++) // edge vertexes
    {
        E = &O->edges_[L1][e / ARRAYSIZE][e % ARRAYSIZE];

        idx = O->vertcount_[L1] + e;

        V = &O->verts_[L][idx / ARRAYSIZE][idx % ARRAYSIZE];

        V->x = E->B.x;
        V->y = E->B.y;
        V->z = E->B.z;

        V->N.x = E->N.x;
        V->N.y = E->N.y;
        V->N.z = E->N.z;
    }

    vert_c = O->vertcount_[L1] + O->edgecount_[L1];

    for (q = 0; q < O->quadcount_[L1]; q ++)
    {
        Q = &O->quads_[L1][q / ARRAYSIZE][q % ARRAYSIZE];
        V = &O->verts_[L][vert_c / ARRAYSIZE][vert_c % ARRAYSIZE];

        V->x = Q->B.x;
        V->y = Q->B.y;
        V->z = Q->B.z;

        V->N.x = Q->N.x;
        V->N.y = Q->N.y;
        V->N.z = Q->N.z;

        vert_c ++;
    }

    vert_c = O->textcount_[L1] + O->uvedcount_[L1]; // text polygon vertexes start after cage and edges

    for (q = 0; q < O->quadcount_[L1]; q ++)
    {
        Q = &O->quads_[L1][q / ARRAYSIZE][q % ARRAYSIZE];
        UV = &O->uvtex_[L][vert_c / ARRAYSIZE][vert_c % ARRAYSIZE];

        UV->u = Q->B2.u;
        UV->v = Q->B2.v;

        vert_c ++;
    }
}

void generate_cage_for_Subdivision_Quads_UV(object * O, int L)
{
    if (L > 0 && L <= O->subdlevel)
    {
        int L1 = L - 1;

        int t, e, q, idx, vert_c;
        uv * UV, * UV0;
        uv_edge * UVE;
        quadrant * Q;

        for (t = 0; t < O->textcount_[L1]; t ++) // text cage
        {
            UV = &O->uvtex_[L1][t / ARRAYSIZE][t % ARRAYSIZE];
            UV0 = &O->uvtex_[L][t / ARRAYSIZE][t % ARRAYSIZE];

            UV0->u = UV->u;
            UV0->v = UV->v;
        }

        for (e = 0; e < O->uvedcount_[L1]; e ++) // cage text edges
        {
            UVE = &O->uveds_[L1][e / ARRAYSIZE][e % ARRAYSIZE]; // cage text edge

            idx = O->textcount_[L1] + e; // edges start after cage

            UV = &O->uvtex_[L][idx / ARRAYSIZE][idx % ARRAYSIZE];

            UV->u = UVE->B.u;
            UV->v = UVE->B.v;
        }

        vert_c = O->textcount_[L1] + O->uvedcount_[L1]; // text polygon vertexes start after cage and edges

        for (q = 0; q < O->quadcount_[L1]; q ++)
        {
            Q = &O->quads_[L1][q / ARRAYSIZE][q % ARRAYSIZE];
            UV = &O->uvtex_[L][vert_c / ARRAYSIZE][vert_c % ARRAYSIZE];

            UV->u = Q->B2.u;
            UV->v = Q->B2.v;

            vert_c ++;
        }
    }
}

#endif // SUBDIVISION_H_INCLUDED
