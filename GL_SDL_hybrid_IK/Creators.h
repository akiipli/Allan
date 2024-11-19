/*
The MIT License

Copyright <2018> <Allan Kiipli>
*/

#include "ZData.h"
#include "ZPlane.h"
#include "CubeData.h"
//#include "wchar.h"

typedef void (*O_func_ptr)(object * O, int level);

typedef struct
{
    int index;
    object * O;
    int Level;
    O_func_ptr O_func;
    int progress;
    int cancel;
}
O_arguments;

O_arguments O_Arguments[OBJECTS];

void * O_worker(void * arguments)
{
    O_arguments * Args = (O_arguments *)arguments;
    int Index;
    Index = Args->index;
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    while(!O_Arguments[Index].cancel)
    {
        if (O_Arguments[Index].progress && O_Arguments[Index].O_func != NULL)
        {
            O_Arguments[Index].O_func(O_Arguments[Index].O, O_Arguments[Index].Level); // for different type of function create next signature
            O_Arguments[Index].progress = 0;
        }
    }

    return NULL;
}

int init_object(int index,
    int textcount,
    int vertcount,
    int icolcount,
    int tripcount,
    int edgecount,
    int uvedcount,
    int polycount,
    int put_transformer)
{
    int result = initialize_object(index,
    textcount,
    vertcount,
    icolcount,
    tripcount,
    edgecount,
    uvedcount,
    polycount,
    put_transformer);

    int result_code;
    if (result)
    {
        O_Arguments[index].index = index;
        O_Arguments[index].O = objects[index];
        O_Arguments[index].Level = -1;
        O_Arguments[index].O_func = NULL;
        O_Arguments[index].progress = 0;
        O_Arguments[index].cancel = 0;
        result_code = pthread_create(&O_thread[index], NULL, O_worker, &O_Arguments[index]);
        assert(!result_code);
    }
    return result;
}

int create_OBJ(int index, char * name, OBJ_In * OBJ_File)
{
    if (OBJ_File->Vertcoord_c == 0)
    {
        return 0;
    }
    if (OBJ_File->Textcoord_c == 0)
    {
        int t, f;
        for (t = 0; t < OBJ_File->Vertcoord_c; t += 3)
        {
            OBJ_File->Textcoords[OBJ_File->Textcoord_c++] = OBJ_File->Vertcoords[t];
            OBJ_File->Textcoords[OBJ_File->Textcoord_c++] = OBJ_File->Vertcoords[t + 1];
        }
        Face * F;
        for (f = 0; f < OBJ_File->Face_c; f ++)
        {
            F = &OBJ_File->Faces[f];
            for (t = 0; t < F->vert_c; t ++)
            {
                F->text[t] = F->vert[t];
            }
        }
    }
    if (OBJ_File->Normcoord_c == 0)
    {
        OBJ_File->Normcoords[OBJ_File->Normcoord_c++] = 0;
        OBJ_File->Normcoords[OBJ_File->Normcoord_c++] = 1;
        OBJ_File->Normcoords[OBJ_File->Normcoord_c++] = 0;
    }

    int textcount = OBJ_File->Textcoord_c / 2;
    int vertcount = OBJ_File->Vertcoord_c / 3;
    int tripcount = 0;
    int polycount = OBJ_File->Face_c;
    int icolcount = polycount;
    int t, f, vert_c;

    for (f = 0; f < polycount; f++)
    {
        vert_c = OBJ_File->Faces[f].vert_c;
        tripcount += (vert_c - 2);
    }

    int O_p_trips[tripcount];

    for (t = 0; t < tripcount; t ++)
    {
        O_p_trips[t] = t;
    }

    edgePack Eds = generate_Edges_OBJ(OBJ_File);
    UV_edgePack UVEds = generate_UVEdges_OBJ(OBJ_File);

    printf("edgecount uvedcount %d, %d\n", Eds.edgecount, UVEds.uvedcount);

    int edgecount = Eds.edgecount;
    int uvedcount = UVEds.uvedcount;

//    print_UV_edgePack(&UVEds);
//    print_edgePack(&Eds);

    printf("OBJ init\n");
    printf("%d, %d, %d, %d, %d, %d, %d\n", textcount, vertcount, icolcount, tripcount, edgecount, uvedcount, polycount);

    int result = init_object(index, textcount, vertcount, icolcount, tripcount, edgecount, uvedcount, polycount, 1);

    if (result == 0) return 0;

    char * n = strdup(basename(name));

    int N = strlen(n);

    if (N > STRLEN - 1) N = STRLEN - 1; // if elements exceed
    n[N] = '\0';
    memcpy(objects[index]->Name, n, N + 1);

    //objects[index].Name = basename(name);

    // unicode filenames dont work
    // find a way to make these filenames object names.

    object * O = objects[index];

    //O->tune_in_uvtex = !UVEds.overlaps;

    int i;
    for (i = 0; i < SUBD; i ++)
    {
        O->tune_in_uvtex[i] = 1;
    }

    int s = 0; // start

    init_object_uvs(O, s, OBJ_File->Textcoords, UVEds.uv_text_edges, UVEds.uv_text_edges_c, UVEds.uv_edge_tiks, textcount);
    init_object_verts(O, s, OBJ_File->Vertcoords, Eds.vert_edges, Eds.vert_edges_c, Eds.vert_edge_tiks, vertcount);
//
//    init_object_triangles(O, s, O_Trips_Vert_index, O_Trips_UV_index, tripcount);

    init_object_edges(O, s, Eds.Edges_Index, Eds.Edge_Poly_Index, Eds.Edge_Slot_Index, Eds.Edge_Poly_Index_c, Eds.Edge_Poly_Tiks, edgecount);
    init_object_uv_edges(O, s, UVEds.UV_Edges_Index, UVEds.UV_Edge_Poly_Index, UVEds.UV_Edge_Poly_Index_c, UVEds.UV_Edge_Poly_Tiks, uvedcount);
    init_object_colors(O, s);
    init_object_polys(O, s, O_p_trips, tripcount, Eds.p_edges, Eds.p_verts, UVEds.p_texts, UVEds.p_texts_c, UVEds.p_uveds, Eds.E_count, polycount);


    free(Eds.p_verts);
    free(Eds.p_edges);
    free(Eds.E_count);
    free(Eds.vert_edges);
    free(Eds.vert_edge_tiks);
    free(Eds.Edges_Index);
    free(Eds.Edge_Poly_Index);
    free(Eds.Edge_Slot_Index);
    free(Eds.Edge_Poly_Tiks);

    free(UVEds.p_texts);
    free(UVEds.p_uveds);
    free(UVEds.uv_text_edges);
    free(UVEds.uv_edge_tiks);
    free(UVEds.UV_Edges_Index);
    free(UVEds.UV_Edge_Poly_Index);
    free(UVEds.UV_Edge_Poly_Tiks);

    return 1;
}

int create_ZPlane(int index, char * name)
{
    int textcount = 4;
    int vertcount = 4;
    int tripcount = 2;
    int edgecount = 4;
    int uvedcount = 4;
    int polycount = 1;
    int icolcount = polycount;

    int result = init_object(index, textcount, vertcount, icolcount, tripcount, edgecount, uvedcount, polycount, 1);

    if (result == 0) return 0;

    objects[index]->Name = name;

    object * O = objects[index];

    int s = 0; // start

    int int_s = sizeof(int);

    init_object_uvs(O, s, ZP_uv_coords, ZP_uv_text_edges, sizeof ZP_uv_text_edges / int_s, ZP_uv_edge_tiks, textcount);
    init_object_verts(O, s, ZP_vert_coords, ZP_vert_edges, sizeof ZP_vert_edges / int_s, ZP_vert_edge_tiks, vertcount);

    init_object_triangles(O, s, ZP_Trips_Vert_index, ZP_Trips_UV_index, tripcount);
    init_object_edges(O, s, ZP_Edges_Index, ZP_Edge_Poly_Index, ZP_Edge_Poly_Index, sizeof ZP_Edge_Poly_Index / int_s, ZP_Edge_Poly_Tiks, edgecount);
    init_object_uv_edges(O, s, ZP_UV_Edges_Index, ZP_UV_Edge_Poly_Index, sizeof ZP_UV_Edge_Poly_Index / int_s, ZP_UV_Edge_Poly_Tiks, uvedcount);
    init_object_colors(O, s);
    init_object_polys(O, s, ZP_p_trips, sizeof ZP_p_trips / int_s, ZP_p_edges, ZP_p_verts, ZP_p_texts, sizeof ZP_p_texts / int_s, ZP_p_uveds, ZP_E_count, polycount);
    return 1;
}

int create_Z(int index, char * name)
{
    int textcount = 10;
    int vertcount = 10;
    int tripcount = 6;
    int edgecount = 10;
    int uvedcount = 10;
    int polycount = 2;
    int icolcount = polycount;

    int result = init_object(index, textcount, vertcount, icolcount, tripcount, edgecount, uvedcount, polycount, 1);

    if (result == 0) return 0;

    objects[index]->Name = name;

    object * O = objects[index];

    int s = 0; // start

    int int_s = sizeof(int);

    init_object_uvs(O, s, Z_uv_coords, Z_uv_text_edges, sizeof Z_uv_text_edges / int_s, Z_uv_edge_tiks, textcount);
    init_object_verts(O, s, Z_vert_coords, Z_vert_edges, sizeof Z_vert_edges / int_s, Z_vert_edge_tiks, vertcount);

    init_object_triangles(O, s, Z_Trips_Vert_index, Z_Trips_UV_index, tripcount);
    init_object_edges(O, s, Z_Edges_Index, Z_Edge_Poly_Index, Z_Edge_Poly_Index, sizeof Z_Edge_Poly_Index / int_s, Z_Edge_Poly_Tiks, edgecount);
    init_object_uv_edges(O, s, Z_UV_Edges_Index, Z_UV_Edge_Poly_Index, sizeof Z_UV_Edge_Poly_Index / int_s, Z_UV_Edge_Poly_Tiks, uvedcount);
    init_object_colors(O, s);
    init_object_polys(O, s, Z_p_trips, sizeof Z_p_trips / int_s, Z_p_edges, Z_p_verts, Z_p_texts, sizeof Z_p_texts / int_s, Z_p_uveds, Z_E_count, polycount);
    return 1;
}

int create_cube(int index, char * name)
{
    int textcount = 14;
    int vertcount = 8;
    int tripcount = 2 * 6;
    int edgecount = 3 * 4;
    int uvedcount = 19;
    int polycount = 6;
    int icolcount = polycount;

    int result = init_object(index, textcount, vertcount, icolcount, tripcount, edgecount, uvedcount, polycount, 1);

    if (result == 0) return 0;

    object * O = objects[index];

    O->Name = malloc(STRLEN);

    sprintf(O->Name, "%s", name);

    int s = 0; // start

    int int_s = sizeof(int);

    init_object_uvs(O, s, CUBE_uv_coords, CUBE_uv_text_edges, sizeof CUBE_uv_text_edges / int_s, CUBE_uv_edge_tiks, textcount);
    init_object_verts(O, s, CUBE_vert_coords, CUBE_vert_edges, sizeof CUBE_vert_edges / int_s, CUBE_vert_edge_tiks, vertcount);

    init_object_triangles(O, s, CUBE_Trips_Vert_index, CUBE_Trips_UV_index, tripcount);
    init_object_edges(O, s, CUBE_Edges_Index, CUBE_Edge_Poly_Index, CUBE_Edge_Poly_Index, sizeof CUBE_Edge_Poly_Index / int_s, CUBE_Edge_Poly_Tiks, edgecount);
    init_object_uv_edges(O, s, CUBE_UV_Edges_Index, CUBE_UV_Edge_Poly_Index, sizeof CUBE_UV_Edge_Poly_Index / int_s, CUBE_UV_Edge_Poly_Tiks, uvedcount);
    init_object_colors(O, s);
    init_object_polys(O, s, CUBE_p_trips, sizeof CUBE_p_trips / int_s, CUBE_p_edges, CUBE_p_verts, CUBE_p_texts, sizeof CUBE_p_texts / int_s, CUBE_p_uveds, CUBE_E_count, polycount);

    O->polys[0][0].surface = 0;
    O->polys[0][1].surface = 1;
    O->polys[0][2].surface = 2;
    O->polys[0][3].surface = 3;
    return 1;
}
