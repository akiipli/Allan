
void validate_poly_Edges(object * O, polygon * P)
{
    int i;
    for(i = 0; i < P->edgecount; i ++)
    {
        int idx = P->verts[i];
        vertex * V = &O->verts[idx / ARRAYSIZE][idx % ARRAYSIZE];
        printf("%d ", V->index);
    }
    printf("\n");
    for(i = 0; i < P->edgecount; i ++)
    {
        int idx = P->edges[i];
        edge * E = &O->edges[idx / ARRAYSIZE][idx % ARRAYSIZE];
        printf("%d %d\n", E->verts[0], E->verts[1]);
    }
}

void validate_poly_UVEdges(object * O, polygon * P)
{
    int i;
    for(i = 0; i < P->edgecount; i ++)
    {
        int idx = P->texts[i];
        uv * UV = &O->uvtex[idx / ARRAYSIZE][idx % ARRAYSIZE];
        printf("%d ", UV->index);
    }
    printf("\n");
    for(i = 0; i < P->edgecount; i ++)
    {
        int idx = P->uveds[i];
        uv_edge * E = &O->uveds[idx / ARRAYSIZE][idx % ARRAYSIZE];
        printf("%d %d\n", E->uv_verts[0], E->uv_verts[1]);
    }
}

