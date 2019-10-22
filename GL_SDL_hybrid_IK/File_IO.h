/*
The MIT License

Copyright <2018> <Allan Kiipli>
*/

#ifndef FILE_IO_H_INCLUDED
#define FILE_IO_H_INCLUDED

char outfiles[] = "C:/Allan/outfiles/";

int write_OBJ_to_Disk_level_(object * O, int L)
{
    char dirfile[STRLEN];
    char index[5];
    sprintf(index, "_%d", O->index);
    char level[] = "_0";
    sprintf(level, "_%d", L);
    dirfile[0] = '\0';
    strcat(dirfile, outfiles);
    strcat(dirfile, O->Name);
    strcat(dirfile, index);
    strcat(dirfile, level);
    strcat(dirfile, ".obj");

    FILE * F;
    F = fopen(dirfile, "w");
    if (F == NULL) return 0;
    fprintf(F, "o %s%s\n", O->Name, level);

    int e, i;

    uv * UV;
    vertex * V;
    quadrant * Q;

    for(i = 0; i < O->vertcount_[L]; i ++)
    {
        V = &O->verts_[L][i / ARRAYSIZE][i % ARRAYSIZE];
        fprintf(F, "v %f %f %f\n", V->Tx, V->Ty, V->Tz);
    }
    for(i = 0; i < O->quadcount_[L]; i ++)
    {
        Q = &O->quads_[L][i / ARRAYSIZE][i % ARRAYSIZE];
        fprintf(F, "vn %f %f %f\n", Q->N.Tx, Q->N.Ty, Q->N.Tz);
    }
    for(i = 0; i < O->textcount_[L]; i ++)
    {
        UV = &O->uvtex_[L][i / ARRAYSIZE][i % ARRAYSIZE];
        fprintf(F, "vt %f %f\n", UV->u, UV->v);
    }
    for(i = 0; i < O->quadcount_[L]; i ++)
    {
        Q = &O->quads_[L][i / ARRAYSIZE][i % ARRAYSIZE];
        fprintf(F, "f");
        for (e = 0; e < 4; e ++)
        {
            fprintf(F, " %d/%d/%d", Q->verts[e] + 1, Q->texts[e] + 1, i + 1);
        }
        fprintf(F, "\n");
    }

    fclose(F);
    printf("%s%s written to %s\n", O->Name, level, outfiles);
    return 1;
}

int write_OBJ_to_Disk(object * O)
{
    char dirfile[STRLEN];
    dirfile[0] = '\0';
    char index[5];
    sprintf(index, "_%d", O->index);
    strcat(dirfile, outfiles);
    strcat(dirfile, O->Name);
    strcat(dirfile, index);
    strcat(dirfile, ".obj");

    FILE * F;
    F = fopen(dirfile, "w");
    if (F == NULL) return 0;
    fprintf(F, "o %s\n", O->Name);

    int e, i;

    uv * UV;
    vertex * V;
    polygon * P;

    for(i = 0; i < O->vertcount; i ++)
    {
        V = &O->verts[i / ARRAYSIZE][i % ARRAYSIZE];
        fprintf(F, "v %f %f %f\n", V->Tx, V->Ty, V->Tz);
    }
    for(i = 0; i < O->polycount; i ++)
    {
        P = &O->polys[i / ARRAYSIZE][i % ARRAYSIZE];
        fprintf(F, "vn %f %f %f\n", P->N.Tx, P->N.Ty, P->N.Tz);
    }
    for(i = 0; i < O->textcount; i ++)
    {
        UV = &O->uvtex[i / ARRAYSIZE][i % ARRAYSIZE];
        fprintf(F, "vt %f %f\n", UV->u, UV->v);
    }
    for(i = 0; i < O->polycount; i ++)
    {
        P = &O->polys[i / ARRAYSIZE][i % ARRAYSIZE];
        fprintf(F, "f");
        for (e = 0; e < P->edgecount; e ++)
        {
            fprintf(F, " %d/%d/%d", P->verts[e] + 1, P->texts[e] + 1, i + 1);
        }
        fprintf(F, "\n");
    }

    fclose(F);
    printf("%s written to %s\n", O->Name, outfiles);
    return 1;
}

void print_uv_f(FILE * F, uv * UV)
{
    fprintf(F, "text %d %d %d\n",
    UV->index,
    UV->uvcount,
    UV->vert);
    fprintf(F, " coord %f %f", UV->u, UV->v);
    int i;
    fprintf(F, "\n uveds");
    for(i = 0; i < UV->uvcount; i++) fprintf(F, " %d", UV->uv_edges[i]);
    fprintf(F, "\n");

//    int index;
//    int visible;
//    int selected;
////    int color;
//    float u;
//    float v;
//    int * uv_edges;
//    int uvcount;
//    int uvcount_fill; // help with subdivision
//    int vert; // 3d vertex
}

void print_vertex_f(FILE * F, vertex * V)
{
    fprintf(F, "vert %d %d %d\n",
    V->index,
    V->edgecount,
    V->uv_vertcount);
    fprintf(F, " weight %f\n", V->weight);
    fprintf(F, " coord %f %f %f\n", V->x, V->y, V->z);
    fprintf(F, " coordT %f %f %f\n", V->Tx, V->Ty, V->Tz);
    fprintf(F, " coordNT %f %f %f\n", V->N.Tx, V->N.Ty, V->N.Tz);
    int i;
    fprintf(F, " edges");
    for(i = 0; i < V->edgecount; i++) fprintf(F, " %d", V->edges[i]);
    fprintf(F, "\n");
    fprintf(F, " uvvrs");
    for(i = 0; i < V->uv_vertcount; i++) fprintf(F, " %d", V->uv_verts[i]);
    fprintf(F, "\n");

//    int index;
//    int visible;
//    int selected;
//    float weight;
//    float weight_init;
////    int color;
//    float x;
//    float y;
//    float z;
//    float Tx; // transformed verts
//    float Ty;
//    float Tz;
//    vertex * vert;
////    float Rx; // radial coordinates, z is depth
////    float Ry;
////    float Rz;
//    int * edges;
//    int edgecount;
//    int * uv_verts;
//    int uv_vertcount;
//    normal N;
}

void print_triangle_f(FILE * F, triangle * T)
{
    fprintf(F, "trip %d %d %d",
    T->index,
    T->poly,
    T->surface);
    int i;
    fprintf(F, "\n verts");
    for(i = 0; i < 3; i++) fprintf(F, " %d", T->verts[i]);
    fprintf(F, "\n texts");
    for(i = 0; i < 3; i++) fprintf(F, " %d", T->texts[i]);
    fprintf(F, "\n");

//    int index;
//    int poly;
//    int visible;
//    int surface;
//    int selected;
//    int verts[3];
////    int color[3];
//    int texts[3];
//    normal N;
//    box_3d B;
//    box_2d B2;
}

void print_edge_f(FILE * F, edge * E)
{
    fprintf(F, "edge %d %d %d\n",
    E->index,
    E->uv_edcount,
    E->polycount);
    fprintf(F, " weight %f\n", E->weight);
    int i;
    fprintf(F, " verts");
    for(i = 0; i < 2; i++) fprintf(F, " %d", E->verts[i]);
    fprintf(F, "\n polys");
    for(i = 0; i < E->polycount; i++) fprintf(F, " %d", E->polys[i]);
    fprintf(F, "\n slots");
    for(i = 0; i < E->polycount; i++) fprintf(F, " %d", E->slots[i]);
    fprintf(F, "\n uveds");
    for(i = 0; i < E->uv_edcount; i++) fprintf(F, " %d", E->uv_edges[i]);
    fprintf(F, "\n");

//    int index;
//    int visible;
//    int subdivs;
//    int selected;
//    float weight;
//    float weight_init;
//    int verts[2];
////    int color[2];
//    int edges[2];
//    int uv_edges[2]; // split edges, same level
//    int uv_edcount;
//    int polys[2];
//    int slots[2];
//    int polycount;
//    normal N;
//    box_3d B;
//    float Mx; // polys mean center
//    float My;
//    float Mz;
}

void print_uv_edge_f(FILE * F, uv_edge * E)
{
    fprintf(F, "uved %d %d %d %d",
    E->index,
    E->polycount,
    E->split,
    E->edge);
    int i;
    fprintf(F, "\n uvvrs");
    for(i = 0; i < 2; i++) fprintf(F, " %d", E->uv_verts[i]);
    fprintf(F, "\n uveds");
    for(i = 0; i < 2; i++) fprintf(F, " %d", E->uv_edges[i]);
    fprintf(F, "\n polys");
    for(i = 0; i < E->polycount; i++) fprintf(F, " %d", E->polys[i]);
    fprintf(F, "\n slots");
    for(i = 0; i < E->polycount; i++) fprintf(F, " %d", E->slots[i]);
    fprintf(F, "\n");

//    int index;
//    int visible;
//    int subdivs;
//    int selected;
//    int uv_verts[2];
//    int uv_color[2];
//    int uv_edges[2];
//    int split;
//    int edge;
//    int polys[2];
//    int slots[2];
//    int polycount;
//    box_2d B;
//    float Mu; // polys mean center
//    float Mv;
}

void print_polygon_f(FILE * F, polygon * P)
{
    fprintf(F, "poly %d %d %d %d %d",
    P->index,
    P->color,
    P->surface,
    P->tripcount,
    P->edgecount);
    int i;
    fprintf(F, "\n trips");
    for(i = 0; i < P->tripcount; i++) fprintf(F, " %d", P->trips[i]);
    fprintf(F, "\n quads");
    for(i = 0; i < P->edgecount; i++) fprintf(F, " %d", P->quads[i]);
    fprintf(F, "\n edges");
    for(i = 0; i < P->edgecount; i++) fprintf(F, " %d", P->edges[i]);
    fprintf(F, "\n verts");
    for(i = 0; i < P->edgecount; i++) fprintf(F, " %d", P->verts[i]);
    fprintf(F, "\n texts");
    for(i = 0; i < P->edgecount; i++) fprintf(F, " %d", P->texts[i]);
    fprintf(F, "\n uveds");
    for(i = 0; i < P->edgecount; i++) fprintf(F, " %d", P->uveds[i]);
    fprintf(F, "\n");

//    int index;
//    int color;
//    int subdivs;
//    int visible;
//    int surface;
//    int selected;
//    int * trips;
//    int * quads;
//    int * edges;
//    int * verts;
//    int * texts;
//    int * uveds;
//    //int * uvdir;
//    int tripcount;
//    int edgecount;
//    normal N;
//    box_3d B;
//    box_2d B2;
}

void print_quadrant_f(FILE * F, quadrant * Q)
{
    fprintf(F, "quad %d %d %d %d",
    Q->index,
    Q->poly,
    Q->subdivs,
    Q->visible);
    int i;
    fprintf(F, "\n verts");
    for(i = 0; i < 4; i++) fprintf(F, " %d", Q->verts[i]);
    fprintf(F, "\n texts");
    for(i = 0; i < 4; i++) fprintf(F, " %d", Q->texts[i]);
    fprintf(F, "\n trips");
    for(i = 0; i < 2; i++) fprintf(F, " %d", Q->trips[i]);
    fprintf(F, "\n quads");
    for(i = 0; i < 4; i++) fprintf(F, " %d", Q->quads[i]);
    fprintf(F, "\n edges");
    for(i = 0; i < 4; i++) fprintf(F, " %d", Q->edges[i]);
    fprintf(F, "\n uveds");
    for(i = 0; i < 4; i++) fprintf(F, " %d", Q->uveds[i]);
    fprintf(F, "\n");
    fprintf(F, " norm  %f %f %f\n", Q->N.x, Q->N.y, Q->N.z);
    fprintf(F, " boxt  %f %f %f %f\n", Q->B.x, Q->B.y, Q->B.z, Q->B.radius);
    fprintf(F, " bx2d  %f %f %f\n", Q->B2.u, Q->B2.v, Q->B2.radius);
}

int write_Object_to_Disk(object * O)
{
    char dirfile[STRLEN];
    dirfile[0] = '\0';
    strcat(dirfile, outfiles);
    strcat(dirfile, O->Name);
    strcat(dirfile, ".txt");

    FILE * F;
    F = fopen(dirfile, "w");
    if (F == NULL) return 0;
    fprintf(F, "%s\n", O->Name);
    int i;
    uv * UV;
    vertex * V;
    triangle * T;
    edge * E;
    uv_edge * UVE;
    polygon * P;
    for(i = 0; i < O->textcount; i ++)
    {
        UV = &O->uvtex[i / ARRAYSIZE][i % ARRAYSIZE];
        print_uv_f(F, UV);
    }
    for(i = 0; i < O->vertcount; i ++)
    {
        V = &O->verts[i / ARRAYSIZE][i % ARRAYSIZE];
        print_vertex_f(F, V);
    }
    for(i = 0; i < O->tripcount; i ++)
    {
        T = &O->trips[i / ARRAYSIZE][i % ARRAYSIZE];
        print_triangle_f(F, T);
    }
    for(i = 0; i < O->edgecount; i ++)
    {
        E = &O->edges[i / ARRAYSIZE][i % ARRAYSIZE];
        print_edge_f(F, E);
    }
    for(i = 0; i < O->uvedcount; i ++)
    {
        UVE = &O->uveds[i / ARRAYSIZE][i % ARRAYSIZE];
        print_uv_edge_f(F, UVE);
    }
    for(i = 0; i < O->polycount; i ++)
    {
        P = &O->polys[i / ARRAYSIZE][i % ARRAYSIZE];
        print_polygon_f(F, P);
    }

    fclose(F);
    printf("%s written\n", O->Name);
    return 1;
}

int write_Object_to_Disk_level_(object * O, int L)
{
    char dirfile[STRLEN];
    char level[] = "_0";
    sprintf(level, "_%d", L);
    dirfile[0] = '\0';
    strcat(dirfile, outfiles);
    strcat(dirfile, O->Name);
    strcat(dirfile, level);
    strcat(dirfile, ".txt");

    FILE * F;
    F = fopen(dirfile, "w");
    if (F == NULL) return 0;
    fprintf(F, "%s%s\n", O->Name, level);
    int i;
    uv * UV;
    vertex * V;
    triangle * T;
    edge * E;
    uv_edge * UVE;
    quadrant * Q;
    for(i = 0; i < O->textcount_[L]; i ++)
    {
        UV = &O->uvtex_[L][i / ARRAYSIZE][i % ARRAYSIZE];
        print_uv_f(F, UV);
    }
    for(i = 0; i < O->vertcount_[L]; i ++)
    {
        V = &O->verts_[L][i / ARRAYSIZE][i % ARRAYSIZE];
        print_vertex_f(F, V);
    }
    for(i = 0; i < O->tripcount_[L]; i ++)
    {
        T = &O->trips_[L][i / ARRAYSIZE][i % ARRAYSIZE];
        print_triangle_f(F, T);
    }
    for(i = 0; i < O->edgecount_[L]; i ++)
    {
        E = &O->edges_[L][i / ARRAYSIZE][i % ARRAYSIZE];
        print_edge_f(F, E);
    }
    for(i = 0; i < O->uvedcount_[L]; i ++)
    {
        UVE = &O->uveds_[L][i / ARRAYSIZE][i % ARRAYSIZE];
        print_uv_edge_f(F, UVE);
    }
    for(i = 0; i < O->quadcount_[L]; i ++)
    {
        Q = &O->quads_[L][i / ARRAYSIZE][i % ARRAYSIZE];
        print_quadrant_f(F, Q);
    }

    fclose(F);
    printf("%s%s written\n", O->Name, level);
    return 1;
}

#endif // FILE_IO_H_INCLUDED
