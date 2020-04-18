#ifndef EXTENSIONFUNCTIONS_H_INCLUDED
#define EXTENSIONFUNCTIONS_H_INCLUDED

#include "Object.h"

void extend_object_texts(object * O, int index, int textcount, int Edges_count, float * uv_coords, int * E_tiks)
{
    printf("extend object texts\n");
    int i;
    int c;

    c = textcount / ARRAYSIZE;
    if (textcount > O->textcount)
    {
        i = O->textcount / ARRAYSIZE;
        i ++;
        while(i <= c)
        {
            uvtex[index][i] = (uv*) malloc(ARRAYSIZE * sizeof(uv));
            if (uvtex[index][i] == NULL)
            {
                textcount = i * ARRAYSIZE;
                break;
            }
            i ++;
        }
    }
    int * Edges = calloc(Edges_count, sizeof(int));
    int start = O->textcount;
    O->textcount = textcount;
    int t_count = textcount - start;
    init_object_uvs(O, start, uv_coords, Edges, Edges_count, E_tiks, t_count);
    free(Edges);
}

void extend_object_verts(object * O, int index, int vertcount, int Edges_count, float * vert_coords, int * E_tiks)
{
    int i;
    int c;

    c = vertcount / ARRAYSIZE;
    if (vertcount > O->vertcount)
    {
        i = O->vertcount / ARRAYSIZE;
        i ++;
        while(i <= c)
        {
            verts[index][i] = (vertex*) malloc(ARRAYSIZE * sizeof(vertex));
            if (verts[index][i] == NULL)
            {
                vertcount = i * ARRAYSIZE;
                break;
            }
            i ++;
        }
    }
    int * Edges = calloc(Edges_count, sizeof(int));
    int start = O->vertcount;
    O->vertcount = vertcount;
    int v_count = vertcount - start;
    init_object_verts(O, start, vert_coords, Edges, Edges_count, E_tiks, v_count);
    free(Edges);
}

void extend_object_trips(object * O, int index, int tripcount, int * Trips_Vert_index, int * Trips_UV_index)
{
    int i;
    int c;

    c = tripcount / ARRAYSIZE;
    if (tripcount > O->tripcount)
    {
        i = O->tripcount / ARRAYSIZE;
        i ++;
        while(i <= c)
        {
            trips[index][i] = (triangle*) malloc(ARRAYSIZE * sizeof(triangle));
            if (trips[index][i] == NULL)
            {
                tripcount = i * ARRAYSIZE;
                break;
            }
            i ++;
        }
    }
    int start = O->tripcount;
    O->tripcount = tripcount;
    init_object_triangles(O, start, Trips_Vert_index, Trips_UV_index, 3);
}

void extend_object_edges(object * O, int index, int edgecount, int * Edges_Index, int * Poly_Tiks)
{
    printf("extend object edges\n");
    int i;
    int c;

    c = edgecount / ARRAYSIZE;
    if (edgecount > O->edgecount)
    {
        i = O->edgecount / ARRAYSIZE;
        i ++;
        while(i <= c)
        {
            edges[index][i] = (edge*) malloc(ARRAYSIZE * sizeof(edge));
            if (edges[index][i] == NULL)
            {
                edgecount = i * ARRAYSIZE;
                break;
            }
            i ++;
        }
    }
    int start = O->edgecount;
    O->edgecount = edgecount;
    int e_count = edgecount - start;
    int * e_polys = calloc(e_count * 2, sizeof(int));
    int * e_slots = calloc(e_count * 2, sizeof(int));
    init_object_edges(O, start, Edges_Index, e_polys, e_slots, e_count * 2, Poly_Tiks, e_count);
    free(e_polys);
    free(e_slots);
}

void extend_object_uv_edges(object * O, int index, int uvedcount, int * Edges_Index, int * Poly_Tiks)
{
    printf("extend object uv edges\n");
    int i;
    int c;

    c = uvedcount / ARRAYSIZE;
    if (uvedcount > O->uvedcount)
    {
        i = O->uvedcount / ARRAYSIZE;
        i ++;
        while(i <= c)
        {
            uveds[index][i] = (uv_edge*) malloc(ARRAYSIZE * sizeof(uv_edge));
            if (uveds[index][i] == NULL)
            {
                uvedcount = i * ARRAYSIZE;
                break;
            }
            i ++;
        }
    }
    int start = O->uvedcount;
    O->uvedcount = uvedcount;
    int e_count = uvedcount - start;
    int * e_polys = calloc(e_count * 2, sizeof(int));
    init_object_uv_edges(O, start, Edges_Index, e_polys, e_count * 2, Poly_Tiks, uvedcount);
    free(e_polys);
}

void extend_object_polys(object * O, int index, int polycount, int * E_tiks, int T_len, int E_len)
{
    int i;
    int c;

    c = polycount / ARRAYSIZE;
    if (polycount > O->polycount)
    {
        i = O->polycount / ARRAYSIZE;
        i ++;
        while(i <= c)
        {
            polys[index][i] = (polygon*) malloc(ARRAYSIZE * sizeof(polygon));
            if (polys[index][i] == NULL)
            {
                polycount = i * ARRAYSIZE;
                break;
            }
            i ++;
        }
    }
    int * p_trips = malloc(T_len * sizeof(int));
    int * p_edges = malloc(E_len * sizeof(int));
    int * p_verts = malloc(E_len * sizeof(int));
    int * p_texts = malloc(E_len * sizeof(int));
    int * p_uveds = malloc(E_len * sizeof(int));
    int start = O->polycount;
    O->polycount = polycount;
    int p_count = polycount - start;
    init_object_polys(O, start, p_trips, T_len, p_edges, p_verts, p_texts, E_len, p_uveds, E_tiks, p_count);
    free(p_trips);
    free(p_edges);
    free(p_verts);
    free(p_texts);
    free(p_uveds);
}


#endif // EXTENSIONFUNCTIONS_H_INCLUDED
