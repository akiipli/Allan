#ifndef FILE_IO_H_INCLUDED
#define FILE_IO_H_INCLUDED

char outfiles[] = "C:/Allan/outfiles/";

void print_uv_f(FILE * F, uv * UV)
{
    fprintf(F, "text   %d %d %d\n",
    UV->index,
    UV->visible,
    UV->uvcount);
    fprintf(F, " coord %f %f", UV->u, UV->v);
    int i;
    fprintf(F, "\n uveds");
    for(i = 0; i < UV->uvcount; i++) fprintf(F, " %d", UV->uv_edges[i]);
    fprintf(F, "\n");
}

void print_vertex_f(FILE * F, vertex * V)
{
    fprintf(F, "vert   %d %d %d\n",
    V->index,
    V->visible,
    V->edgecount);
    fprintf(F, " coord %f %f %f\n", V->x, V->y, V->z);
    fprintf(F, " norm  %f %f %f\n", V->N.x, V->N.y, V->N.z);
    int i;
    fprintf(F, " edges");
    for(i = 0; i < V->edgecount; i++) fprintf(F, " %d", V->edges[i]);
    fprintf(F, "\n");
}

void print_triangle_f(FILE * F, triangle * T)
{
    fprintf(F, "trip   %d %d %d",
    T->index,
    T->poly,
    T->visible);
    int i;
    fprintf(F, "\n verts");
    for(i = 0; i < 3; i++) fprintf(F, " %d", T->verts[i]);
    fprintf(F, "\n texts");
    for(i = 0; i < 3; i++) fprintf(F, " %d", T->texts[i]);
    fprintf(F, "\n");
    fprintf(F, " norm  %f %f %f\n", T->N.x, T->N.y, T->N.z);
    fprintf(F, " bx3d  %f %f %f %f\n", T->B.x, T->B.y, T->B.z, T->B.radius);
}

void print_edge_f(FILE * F, edge * E)
{
    fprintf(F, "edge   %d %d %d %d",
    E->index,
    E->visible,
    E->subdivs,
    E->polycount);
    int i;
    fprintf(F, "\n verts");
    for(i = 0; i < 2; i++) fprintf(F, " %d", E->verts[i]);
    fprintf(F, "\n edges");
    for(i = 0; i < 2; i++) fprintf(F, " %d", E->edges[i]);
    fprintf(F, "\n polys");
    for(i = 0; i < E->polycount; i++) fprintf(F, " %d", E->polys[i]);
    fprintf(F, "\n uv edges");
    for(i = 0; i < E->uv_edcount; i++) fprintf(F, " %d", E->uv_edges[i]);
    fprintf(F, "\n");
    fprintf(F, " norm  %f %f %f\n", E->N.x, E->N.y, E->N.z);
    fprintf(F, " boxt  %f %f %f %f\n", E->B.x, E->B.y, E->B.z, E->B.radius);
}

void print_uv_edge_f(FILE * F, uv_edge * E)
{
    fprintf(F, "uved   %d %d %d %d %d",
    E->index,
    E->visible,
    E->subdivs,
    E->polycount,
    E->split);
    int i;
    fprintf(F, "\n uvvrs");
    for(i = 0; i < 2; i++) fprintf(F, " %d", E->uv_verts[i]);
    fprintf(F, "\n uveds");
    for(i = 0; i < 2; i++) fprintf(F, " %d", E->uv_edges[i]);
    fprintf(F, "\n polys");
    for(i = 0; i < E->polycount; i++) fprintf(F, " %d", E->polys[i]);
    fprintf(F, "\n");
    fprintf(F, " bx2d  %f %f %f\n", E->B.u, E->B.v, E->B.radius);
}

void print_polygon_f(FILE * F, polygon * P)
{
    fprintf(F, "poly   %d %d %d %d %d %d",
    P->index,
    P->subdivs,
    P->visible,
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
//    fprintf(F, "\n uvdir");
//    for(i = 0; i < P->edgecount; i++) fprintf(F, " %d", P->uvdir[i]);
    fprintf(F, "\n");
    fprintf(F, " norm  %f %f %f\n", P->N.x, P->N.y, P->N.z);
    fprintf(F, " boxt  %f %f %f %f\n", P->B.x, P->B.y, P->B.z, P->B.radius);
    fprintf(F, " bx2d  %f %f %f\n", P->B2.u, P->B2.v, P->B2.radius);
}

void print_quadrant_f(FILE * F, quadrant * Q)
{
    fprintf(F, "quad   %d %d %d %d",
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
