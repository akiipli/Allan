/*
The MIT License

Copyright <2018> <Allan Kiipli>
*/

// Generators generate coordinates and connections for static and transformed geometry.
// Here also OBJ edges are generated via edge pack structure. Also UV texture edges.

typedef struct
{
    int * p_verts;
    int * p_edges;
    int * E_count;
    int * vert_edges;
    int * vert_edge_tiks;
    int * Edges_Index;
    int * Edge_Poly_Index;
    int * Edge_Slot_Index;
    int * Edge_Poly_Tiks;
    int edgecount;
    int p_verts_c;
    int p_edges_c;
    int E_count_c;
    int vert_edges_c;
    int vert_edge_tiks_c;
    int Edges_Index_c;
    int Edge_Poly_Index_c;
    int Edge_Poly_Tiks_c;
}
edgePack;

void print_edgePack(edgePack * E)
{
    printf("edgePack   %d", E->edgecount);
    int i;
    printf("\n p_verts %d", E->p_verts_c);
    for(i = 0; i < E->p_verts_c; i++) printf(" %d", E->p_verts[i]);
    printf("\n p_edges %d", E->p_edges_c);
    for(i = 0; i < E->p_edges_c; i++) printf(" %d", E->p_edges[i]);
    printf("\n E_count %d", E->E_count_c);
    for(i = 0; i < E->E_count_c; i++) printf(" %d", E->E_count[i]);
    printf("\n vert_edges %d", E->vert_edges_c);
    for(i = 0; i < E->vert_edges_c; i++) printf(" %d", E->vert_edges[i]);
    printf("\n vert_edge_tiks %d", E->vert_edge_tiks_c);
    for(i = 0; i < E->vert_edge_tiks_c; i++) printf(" %d", E->vert_edge_tiks[i]);
    printf("\n Edges_Index %d", E->Edges_Index_c);
    for(i = 0; i < E->Edges_Index_c; i++) printf(" %d", E->Edges_Index[i]);
    printf("\n Edge_Poly_Index %d", E->Edge_Poly_Index_c);
    for(i = 0; i < E->Edge_Poly_Index_c; i++) printf(" %d", E->Edge_Poly_Index[i]);
    printf("\n Edge_Poly_Tiks %d", E->Edge_Poly_Tiks_c);
    for(i = 0; i < E->Edge_Poly_Tiks_c; i++) printf(" %d", E->Edge_Poly_Tiks[i]);
    printf("\n");
}

typedef struct
{
    int * p_texts;
    int * p_uveds;
    int * uv_text_edges;
    int * uv_edge_tiks;
    int * UV_Edges_Index;
    int * UV_Edge_Poly_Index;
    int * UV_Edge_Poly_Tiks;
    int uvedcount;
    int p_texts_c;
    int p_uveds_c;
    int uv_text_edges_c;
    int uv_edge_tiks_c;
    int UV_Edges_Index_c;
    int UV_Edge_Poly_Index_c;
    int UV_Edge_Poly_Tiks_c;
}
UV_edgePack;

void print_UV_edgePack(UV_edgePack * E)
{
    printf("UV_edgePack   %d", E->uvedcount);
    int i;
    printf("\n p_texts %d",  E->p_texts_c);
    for(i = 0; i <  E->p_texts_c; i++) printf(" %d", E->p_texts[i]);
    printf("\n p_uveds %d",  E->p_uveds_c);
    for(i = 0; i <  E->p_uveds_c; i++) printf(" %d", E->p_uveds[i]);
    printf("\n uv_text_edges %d",  E->uv_text_edges_c);
    for(i = 0; i <  E->uv_text_edges_c; i++) printf(" %d", E->uv_text_edges[i]);
    printf("\n uv_edge_tiks %d",  E->uv_edge_tiks_c);
    for(i = 0; i <  E->uv_edge_tiks_c; i++) printf(" %d", E->uv_edge_tiks[i]);
    printf("\n UV_Edges_Index %d",  E->UV_Edges_Index_c);
    for(i = 0; i <  E->UV_Edges_Index_c; i++) printf(" %d", E->UV_Edges_Index[i]);
    printf("\n UV_Edge_Poly_Index %d",  E->UV_Edge_Poly_Index_c);
    for(i = 0; i <  E->UV_Edge_Poly_Index_c; i++) printf(" %d", E->UV_Edge_Poly_Index[i]);
    printf("\n UV_Edge_Poly_Tiks %d",  E->UV_Edge_Poly_Tiks_c);
    for(i = 0; i <  E->UV_Edge_Poly_Tiks_c; i++) printf(" %d", E->UV_Edge_Poly_Tiks[i]);
    printf("\n");
}

void update_Box_T(object * O)
{
    vertex * V;
    if (O->vertcount > 1)
    {
        V = &O->verts[0][0];
        float Delta[3];
        Delta[0] = V->Tx - V->x;
        Delta[1] = V->Ty - V->y;
        Delta[2] = V->Tz - V->z;
        O->B.Tx = O->B.x + Delta[0];
        O->B.Ty = O->B.y + Delta[1];
        O->B.Tz = O->B.z + Delta[2];
    }
    else
    {
        O->B.Tx = O->B.x + O->T->pos[0];
        O->B.Ty = O->B.y + O->T->pos[1];
        O->B.Tz = O->B.z + O->T->pos[2];
    }
}

void cross_Product_N_T(normal * v1, float v2[3], float N[3])
{
    N[0] = v1->Ty * v2[2] - v1->Tz * v2[1];
    N[1] = v1->Tz * v2[0] - v1->Tx * v2[2];
    N[2] = v1->Tx * v2[1] - v1->Ty * v2[0];
}

void average_Vert_Tangents_(object * O, int L)
{
    uv_edge * UVE;
    uv * UV;
    int idx, v, e;

    float tx, ty, tz;

    for (v = 0; v < O->textcount_[L]; v ++)
    {
        UV = &O->uvtex_[L][v / ARRAYSIZE][v % ARRAYSIZE];

        tx = 0;
        ty = 0;
        tz = 0;

        for (e = 0; e < UV->uvcount; e ++)
        {
            idx = UV->uv_edges[e];
            UVE = &O->uveds_[L][idx / ARRAYSIZE][idx % ARRAYSIZE];

            tx += UVE->tangent[0];
            ty += UVE->tangent[1];
            tz += UVE->tangent[2];
        }

        tx /= UV->uvcount;
        ty /= UV->uvcount;
        tz /= UV->uvcount;

        UV->tangent[0] = tx;
        UV->tangent[1] = ty;
        UV->tangent[2] = tz;
        UV->tangent[3] = UVE->tangent[3];
    }
}

void average_Edge_Tangents_(object * O, int L)
{
    quadrant * Q;
    uv_edge * UVE;
    int idx, q, e;

    float tx, ty, tz;

    for (e = 0; e < O->uvedcount_[L]; e ++)
    {
        UVE = &O->uveds_[L][e / ARRAYSIZE][e % ARRAYSIZE];

        tx = 0;
        ty = 0;
        tz = 0;

        for (q = 0; q < UVE->polycount; q ++)
        {
            idx = UVE->polys[q];
            Q = &O->quads_[L][idx / ARRAYSIZE][idx % ARRAYSIZE];

            tx += Q->tangent[0];
            ty += Q->tangent[1];
            tz += Q->tangent[2];
        }

        tx /= UVE->polycount;
        ty /= UVE->polycount;
        tz /= UVE->polycount;

        UVE->tangent[0] = tx;
        UVE->tangent[1] = ty;
        UVE->tangent[2] = tz;
        UVE->tangent[3] = Q->tangent[3];
    }
}

void average_Quadrant_Tangents_(object * O, int L)
{
    triangle * T;
    quadrant * Q;
    int idx, t, q;

    float tx, ty, tz;

    for (q = 0; q < O->quadcount_[L]; q ++)
    {
        Q = &O->quads_[L][q / ARRAYSIZE][q % ARRAYSIZE];

        tx = 0;
        ty = 0;
        tz = 0;

        for (t = 0; t < 2; t ++)
        {
            idx = Q->trips[t];
            T = &O->trips_[L][idx / ARRAYSIZE][idx % ARRAYSIZE];

            tx += T->tangent[0];
            ty += T->tangent[1];
            tz += T->tangent[2];
        }

        tx /= 2;
        ty /= 2;
        tz /= 2;

        Q->tangent[0] = tx;
        Q->tangent[1] = ty;
        Q->tangent[2] = tz;
        Q->tangent[3] = T->tangent[3];
    }
}

void average_Vert_Tangents(object * O)
{
    uv_edge * UVE;
    uv * UV;
    int idx, v, e;

    float tx, ty, tz;

    for (v = 0; v < O->textcount; v ++)
    {
        UV = &O->uvtex[v / ARRAYSIZE][v % ARRAYSIZE];

        tx = 0;
        ty = 0;
        tz = 0;

        for (e = 0; e < UV->uvcount; e ++)
        {
            idx = UV->uv_edges[e];
            UVE = &O->uveds[idx / ARRAYSIZE][idx % ARRAYSIZE];

            tx += UVE->tangent[0];
            ty += UVE->tangent[1];
            tz += UVE->tangent[2];
        }

        tx /= UV->uvcount;
        ty /= UV->uvcount;
        tz /= UV->uvcount;

        UV->tangent[0] = tx;
        UV->tangent[1] = ty;
        UV->tangent[2] = tz;
        UV->tangent[3] = UVE->tangent[3];
    }
}

void average_Edge_Tangents(object * O)
{
    polygon * P;
    uv_edge * UVE;
    int idx, p, e;

    float tx, ty, tz;

    for (e = 0; e < O->uvedcount; e ++)
    {
        UVE = &O->uveds[e / ARRAYSIZE][e % ARRAYSIZE];

        tx = 0;
        ty = 0;
        tz = 0;

        for (p = 0; p < UVE->polycount; p ++)
        {
            idx = UVE->polys[p];
            P = &O->polys[idx / ARRAYSIZE][idx % ARRAYSIZE];

            tx += P->tangent[0];
            ty += P->tangent[1];
            tz += P->tangent[2];
        }

        tx /= UVE->polycount;
        ty /= UVE->polycount;
        tz /= UVE->polycount;

        UVE->tangent[0] = tx;
        UVE->tangent[1] = ty;
        UVE->tangent[2] = tz;
        UVE->tangent[3] = P->tangent[3];
    }
}

void average_Polygon_Tangents(object * O)
{
    triangle * T;
    polygon * P;
    int idx, t, p;

    float tx, ty, tz;

    for (p = 0; p < O->polycount; p ++)
    {
        P = &O->polys[p / ARRAYSIZE][p % ARRAYSIZE];

        tx = 0;
        ty = 0;
        tz = 0;

        for (t = 0; t < P->edgecount - 2; t ++)
        {
            idx = P->trips[t];
            T = &O->trips[idx / ARRAYSIZE][idx % ARRAYSIZE];

            tx += T->tangent[0];
            ty += T->tangent[1];
            tz += T->tangent[2];
        }

        tx /= P->edgecount;
        ty /= P->edgecount;
        tz /= P->edgecount;

        P->tangent[0] = tx;
        P->tangent[1] = ty;
        P->tangent[2] = tz;
        P->tangent[3] = T->tangent[3];
    }
}

void calculate_Triangle_Tangents_(object * O, int L)
{
    triangle * T;
    int t;

    uv * UV0;
    uv * UV1;
    uv * UV2;

    vertex * V0;
    vertex * V1;
    vertex * V2;

    float sdir[3], tdir[3], tangent[3], c[3];
    float x1, x2, y1, y2, z1, z2, s1, s2, t1, t2, r;
    float dot;

    for (t = 0; t < O->tripcount_[L]; t++)
    {
        T = &O->trips_[L][t / ARRAYSIZE][t % ARRAYSIZE];

        V0 = &O->verts_[L][T->verts[0] / ARRAYSIZE][T->verts[0] % ARRAYSIZE];
        V1 = &O->verts_[L][T->verts[1] / ARRAYSIZE][T->verts[1] % ARRAYSIZE];
        V2 = &O->verts_[L][T->verts[2] / ARRAYSIZE][T->verts[2] % ARRAYSIZE];

        UV0 = &O->uvtex_[L][T->texts[0] / ARRAYSIZE][T->texts[0] % ARRAYSIZE];
        UV1 = &O->uvtex_[L][T->texts[1] / ARRAYSIZE][T->texts[1] % ARRAYSIZE];
        UV2 = &O->uvtex_[L][T->texts[2] / ARRAYSIZE][T->texts[2] % ARRAYSIZE];

        x1 = V1->Tx - V0->Tx;
        x2 = V2->Tx - V0->Tx;
        y1 = V1->Ty - V0->Ty;
        y2 = V2->Ty - V0->Ty;
        z1 = V1->Tz - V0->Tz;
        z2 = V2->Tz - V0->Tz;

        s1 = UV1->u - UV0->u;
        s2 = UV2->u - UV0->u;
        t1 = UV1->v - UV0->v;
        t2 = UV2->v - UV0->v;

        r = 1.0 / (s1 * t2 - s2 * t1);

        sdir[0] = (t2 * x1 - t1 * x2) * r;
        sdir[1] = (t2 * y1 - t1 * y2) * r;
        sdir[2] = (t2 * z1 - t1 * z2) * r;

        tdir[0] = (s1 * x2 - s2 * x1) * r;
        tdir[1] = (s1 * y2 - s2 * y1) * r;
        tdir[2] = (s1 * z2 - s2 * z1) * r;

        dot = dot_productN_T(&T->N, sdir);

        tangent[0] = sdir[0] - T->N.Tx * dot;
        tangent[1] = sdir[1] - T->N.Ty * dot;
        tangent[2] = sdir[2] - T->N.Tz * dot;

        normalizeF(tangent);

        T->tangent[0] = tangent[0];
        T->tangent[1] = tangent[1];
        T->tangent[2] = tangent[2];

        cross_Product_N_T(&T->N, sdir, c);

        T->tangent[3] = (dot_productFF(c, tdir) < 0) ? -1 : 1;

        //printf("%f %f %f %f\n", T->tangent[0], T->tangent[1], T->tangent[2], T->tangent[3]);
    }
}

void calculate_Triangle_Tangents(object * O)
{
    triangle * T;
    int t;

    uv * UV0;
    uv * UV1;
    uv * UV2;

    vertex * V0;
    vertex * V1;
    vertex * V2;

    float sdir[3], tdir[3], tangent[3], c[3];
    float x1, x2, y1, y2, z1, z2, s1, s2, t1, t2, r;
    float dot;

    for (t = 0; t < O->tripcount; t++)
    {
        T = &O->trips[t / ARRAYSIZE][t % ARRAYSIZE];

        V0 = &O->verts[T->verts[0] / ARRAYSIZE][T->verts[0] % ARRAYSIZE];
        V1 = &O->verts[T->verts[1] / ARRAYSIZE][T->verts[1] % ARRAYSIZE];
        V2 = &O->verts[T->verts[2] / ARRAYSIZE][T->verts[2] % ARRAYSIZE];

        UV0 = &O->uvtex[T->texts[0] / ARRAYSIZE][T->texts[0] % ARRAYSIZE];
        UV1 = &O->uvtex[T->texts[1] / ARRAYSIZE][T->texts[1] % ARRAYSIZE];
        UV2 = &O->uvtex[T->texts[2] / ARRAYSIZE][T->texts[2] % ARRAYSIZE];

        x1 = V1->Tx - V0->Tx;
        x2 = V2->Tx - V0->Tx;
        y1 = V1->Ty - V0->Ty;
        y2 = V2->Ty - V0->Ty;
        z1 = V1->Tz - V0->Tz;
        z2 = V2->Tz - V0->Tz;

        s1 = UV1->u - UV0->u;
        s2 = UV2->u - UV0->u;
        t1 = UV1->v - UV0->v;
        t2 = UV2->v - UV0->v;

        r = 1.0 / (s1 * t2 - s2 * t1);

        sdir[0] = (t2 * x1 - t1 * x2) * r;
        sdir[1] = (t2 * y1 - t1 * y2) * r;
        sdir[2] = (t2 * z1 - t1 * z2) * r;

        tdir[0] = (s1 * x2 - s2 * x1) * r;
        tdir[1] = (s1 * y2 - s2 * y1) * r;
        tdir[2] = (s1 * z2 - s2 * z1) * r;

        dot = dot_productN_T(&T->N, sdir);

        tangent[0] = sdir[0] - T->N.Tx * dot;
        tangent[1] = sdir[1] - T->N.Ty * dot;
        tangent[2] = sdir[2] - T->N.Tz * dot;

        normalizeF(tangent);

        T->tangent[0] = tangent[0];
        T->tangent[1] = tangent[1];
        T->tangent[2] = tangent[2];

        cross_Product_N_T(&T->N, sdir, c);

        T->tangent[3] = (dot_productFF(c, tdir) < 0) ? -1 : 1;

        //printf("%f %f %f %f\n", T->tangent[0], T->tangent[1], T->tangent[2], T->tangent[3]);
    }
}

void update_bounding_box_for_transformed_Trips(object * O, unsigned char radius)
{
    triangle * T;
    int t;
    vertex * V0;
    vertex * V1;
    vertex * V2;
    for (t = 0; t < O->tripcount; t++)
    {
        T = &O->trips[t / ARRAYSIZE][t % ARRAYSIZE];
        V0 = &O->verts[T->verts[0] / ARRAYSIZE][T->verts[0] % ARRAYSIZE];
        V1 = &O->verts[T->verts[1] / ARRAYSIZE][T->verts[1] % ARRAYSIZE];
        V2 = &O->verts[T->verts[2] / ARRAYSIZE][T->verts[2] % ARRAYSIZE];
        boxt_3_T(&T->B, V0, V1, V2, radius);
    }
}

void update_bounding_box_for_transformed_Trips_(object * O, int L, unsigned char radius)
{
    triangle * T;
    int t;
    vertex * V0;
    vertex * V1;
    vertex * V2;
    for (t = 0; t < O->tripcount_[L]; t++)
    {
        T = &O->trips_[L][t / ARRAYSIZE][t % ARRAYSIZE];
        V0 = &O->verts_[L][T->verts[0] / ARRAYSIZE][T->verts[0] % ARRAYSIZE];
        V1 = &O->verts_[L][T->verts[1] / ARRAYSIZE][T->verts[1] % ARRAYSIZE];
        V2 = &O->verts_[L][T->verts[2] / ARRAYSIZE][T->verts[2] % ARRAYSIZE];
        boxt_3_T(&T->B, V0, V1, V2, radius);
    }
}

void update_bounding_box_for_Trips_(object * O, int L, unsigned char radius)
{
    triangle * T;
    int t;
    vertex * V0;
    vertex * V1;
    vertex * V2;
    uv * UV0;
    uv * UV1;
    uv * UV2;
    for (t = 0; t < O->tripcount_[L]; t++)
    {
        T = &O->trips_[L][t / ARRAYSIZE][t % ARRAYSIZE];
        V0 = &O->verts_[L][T->verts[0] / ARRAYSIZE][T->verts[0] % ARRAYSIZE];
        V1 = &O->verts_[L][T->verts[1] / ARRAYSIZE][T->verts[1] % ARRAYSIZE];
        V2 = &O->verts_[L][T->verts[2] / ARRAYSIZE][T->verts[2] % ARRAYSIZE];
        UV0 = &O->uvtex_[L][T->texts[0] / ARRAYSIZE][T->texts[0] % ARRAYSIZE];
        UV1 = &O->uvtex_[L][T->texts[1] / ARRAYSIZE][T->texts[1] % ARRAYSIZE];
        UV2 = &O->uvtex_[L][T->texts[2] / ARRAYSIZE][T->texts[2] % ARRAYSIZE];
        boxt_3(&T->B, V0, V1, V2, radius);
        boxt_3_2d(&T->B2, UV0, UV1, UV2, radius);
    }
}

void update_bounding_box_for_Trips(object * O, unsigned char radius)
{
    triangle * T;
    int t;
    vertex * V0;
    vertex * V1;
    vertex * V2;
    uv * UV0;
    uv * UV1;
    uv * UV2;
    for (t = 0; t < O->tripcount; t++)
    {
        T = &O->trips[t / ARRAYSIZE][t % ARRAYSIZE];
        V0 = &O->verts[T->verts[0] / ARRAYSIZE][T->verts[0] % ARRAYSIZE];
        V1 = &O->verts[T->verts[1] / ARRAYSIZE][T->verts[1] % ARRAYSIZE];
        V2 = &O->verts[T->verts[2] / ARRAYSIZE][T->verts[2] % ARRAYSIZE];
        UV0 = &O->uvtex[T->texts[0] / ARRAYSIZE][T->texts[0] % ARRAYSIZE];
        UV1 = &O->uvtex[T->texts[1] / ARRAYSIZE][T->texts[1] % ARRAYSIZE];
        UV2 = &O->uvtex[T->texts[2] / ARRAYSIZE][T->texts[2] % ARRAYSIZE];
        boxt_3(&T->B, V0, V1, V2, radius);
        boxt_3_2d(&T->B2, UV0, UV1, UV2, radius);
    }
}

void update_bounding_box_for_transformed_Edges_(object * O, int L, unsigned char radius)
{
    edge * E;
    int e;
    vertex * V0;
    vertex * V1;
    for (e = 0; e < O->edgecount_[L]; e++)
    {
        E = &O->edges_[L][e / ARRAYSIZE][e % ARRAYSIZE];
        V0 = &O->verts_[L][E->verts[0] / ARRAYSIZE][E->verts[0] % ARRAYSIZE];
        V1 = &O->verts_[L][E->verts[1] / ARRAYSIZE][E->verts[1] % ARRAYSIZE];
        boxt_2_T(&E->B, V0, V1, radius);
    }
}

void update_bounding_box_for_transformed_Edges(object * O, unsigned char radius)
{
    edge * E;
    int e;
    vertex * V0;
    vertex * V1;
    for (e = 0; e < O->edgecount; e++)
    {
        E = &O->edges[e / ARRAYSIZE][e % ARRAYSIZE];
        V0 = &O->verts[E->verts[0] / ARRAYSIZE][E->verts[0] % ARRAYSIZE];
        V1 = &O->verts[E->verts[1] / ARRAYSIZE][E->verts[1] % ARRAYSIZE];
        boxt_2_T(&E->B, V0, V1, radius);
    }
}

void update_bounding_box_for_Edges_(object * O, int L, unsigned char radius)
{
    edge * E;
    int e;
    vertex * V0;
    vertex * V1;
    for (e = 0; e < O->edgecount_[L]; e++)
    {
        E = &O->edges_[L][e / ARRAYSIZE][e % ARRAYSIZE];
        V0 = &O->verts_[L][E->verts[0] / ARRAYSIZE][E->verts[0] % ARRAYSIZE];
        V1 = &O->verts_[L][E->verts[1] / ARRAYSIZE][E->verts[1] % ARRAYSIZE];
        boxt_2(&E->B, V0, V1, radius);
    }
}

void assign_Quad_Box_And_Cage_Recursive(object * O, quadrant * Q, int l, int L, unsigned char radius)
{
    int i, idx, q;
    uv * V0, * V_0;
    uv * V1, * V_1;
    uv * V2;
    uv * V3;
    quadrant * Q0;
    uv_edge * UVE;
    uv * UV;

    int vert_c;

    if (l == L)
    {
        V0 = &O->uvtex_[l][Q->texts[0] / ARRAYSIZE][Q->texts[0] % ARRAYSIZE];
        V1 = &O->uvtex_[l][Q->texts[1] / ARRAYSIZE][Q->texts[1] % ARRAYSIZE];
        V2 = &O->uvtex_[l][Q->texts[2] / ARRAYSIZE][Q->texts[2] % ARRAYSIZE];
        V3 = &O->uvtex_[l][Q->texts[3] / ARRAYSIZE][Q->texts[3] % ARRAYSIZE];
        boxt_4_2d(&Q->B2, V0, V1, V2, V3, radius);

        if (l + 1 <= O->subdlevel)
        {
            UV = &O->uvtex_[l + 1][Q->texts[0] / ARRAYSIZE][Q->texts[0] % ARRAYSIZE];
            UV->u = V0->u;
            UV->v = V0->v;
            UV = &O->uvtex_[l + 1][Q->texts[1] / ARRAYSIZE][Q->texts[1] % ARRAYSIZE];
            UV->u = V1->u;
            UV->v = V1->v;
            UV = &O->uvtex_[l + 1][Q->texts[2] / ARRAYSIZE][Q->texts[2] % ARRAYSIZE];
            UV->u = V2->u;
            UV->v = V2->v;
            UV = &O->uvtex_[l + 1][Q->texts[3] / ARRAYSIZE][Q->texts[3] % ARRAYSIZE];
            UV->u = V3->u;
            UV->v = V3->v;

            vert_c = O->textcount_[l] + O->uvedcount_[L] + Q->index; // text polygon vertexes start after cage and edges
            UV = &O->uvtex_[l + 1][vert_c / ARRAYSIZE][vert_c % ARRAYSIZE];
            UV->u = Q->B2.u;
            UV->v = Q->B2.v;
        }

        for (i = 0; i < 4; i ++)
        {
            idx = Q->uveds[i];
            UVE = &O->uveds_[l][idx / ARRAYSIZE][idx % ARRAYSIZE];
            V_0 = &O->uvtex_[l][UVE->uv_verts[0] / ARRAYSIZE][UVE->uv_verts[0] % ARRAYSIZE];
            V_1 = &O->uvtex_[l][UVE->uv_verts[1] / ARRAYSIZE][UVE->uv_verts[1] % ARRAYSIZE];
            boxt_0(&UVE->B, V_0, V_1, radius);

            if (l + 1 <= O->subdlevel)
            {

                idx = O->textcount_[l] + idx; // edges start after cage

                UV = &O->uvtex_[l + 1][idx / ARRAYSIZE][idx % ARRAYSIZE];

                UV->u = UVE->B.u;
                UV->v = UVE->B.v;
            }
        }
    }

    if (l < L)
    {
        for (q = 0; q < 4; q ++)
        {
            idx = Q->quads[q];
            Q0 = &O->quads_[l + 1][idx / ARRAYSIZE][idx % ARRAYSIZE];
            assign_Quad_Box_And_Cage_Recursive(O, Q0, l + 1, L, radius);
        }
    }
}

void update_bounding_box_for_selected_UV_EdgesQuads(object * O, int L, unsigned char radius)
{
    int a, idx, q, p, e;

    quadrant * Q;
    polygon * P;
    uv_edge * E;

    int condition;
    int assigned_poly_count = 0;
    int * assigned_poly = malloc(O->polycount * sizeof(int));

    for (e = 0; e < O->uvedcount; e++)
    {
        E = &O->uveds[e / ARRAYSIZE][e % ARRAYSIZE];
        if (E->selected)
        {
            for (p = 0; p < E->polycount; p ++)
            {
                condition = 1;

                idx = E->polys[p];
                for (a = 0; a < assigned_poly_count; a ++)
                {
                    if (idx == assigned_poly[a])
                    {
                        condition = 0;
                    }
                }

                if (condition)
                {
                    P = &O->polys[idx / ARRAYSIZE][idx % ARRAYSIZE];

                    assigned_poly[assigned_poly_count ++] = idx;

                    for (q = 0; q < P->edgecount; q ++)
                    {
                        idx = P->quads[q];
                        Q = &O->quads_[0][idx / ARRAYSIZE][idx % ARRAYSIZE];
                        assign_Quad_Box_And_Cage_Recursive(O, Q, 0, L, radius);
                    }
                }
            }
        }
    }

    free(assigned_poly);
}

void update_bounding_box_for_UV_Edges_(object * O, int L, unsigned char radius)
{
    uv_edge * E;
    int e;
    uv * V0;
    uv * V1;
    for (e = 0; e < O->uvedcount_[L]; e++)
    {
        E = &O->uveds_[L][e / ARRAYSIZE][e % ARRAYSIZE];
        V0 = &O->uvtex_[L][E->uv_verts[0] / ARRAYSIZE][E->uv_verts[0] % ARRAYSIZE];
        V1 = &O->uvtex_[L][E->uv_verts[1] / ARRAYSIZE][E->uv_verts[1] % ARRAYSIZE];
        boxt_0(&E->B, V0, V1, radius);
    }
}

void update_bounding_box_for_Edges(object * O, unsigned char radius)
{
    edge * E;
    int e;
    vertex * V0;
    vertex * V1;
    for (e = 0; e < O->edgecount; e++)
    {
        E = &O->edges[e / ARRAYSIZE][e % ARRAYSIZE];
        V0 = &O->verts[E->verts[0] / ARRAYSIZE][E->verts[0] % ARRAYSIZE];
        V1 = &O->verts[E->verts[1] / ARRAYSIZE][E->verts[1] % ARRAYSIZE];
        boxt_2(&E->B, V0, V1, radius);
    }
}

void update_bounding_box_for_UV_Edges(object * O, unsigned char radius)
{
    uv_edge * E;
    int e;
    uv * V0;
    uv * V1;
    for (e = 0; e < O->uvedcount; e++)
    {
        E = &O->uveds[e / ARRAYSIZE][e % ARRAYSIZE];
        V0 = &O->uvtex[E->uv_verts[0] / ARRAYSIZE][E->uv_verts[0] % ARRAYSIZE];
        V1 = &O->uvtex[E->uv_verts[1] / ARRAYSIZE][E->uv_verts[1] % ARRAYSIZE];
        boxt_0(&E->B, V0, V1, radius);
    }
}

void update_2d_bounding_box_for_Quads(object * O, int L, unsigned char radius)
{
    quadrant * Q;
//    polygon
    int q;
    uv * V0;
    uv * V1;
    uv * V2;
    uv * V3;
    for (q = 0; q < O->quadcount_[L]; q++)
    {
        Q = &O->quads_[L][q / ARRAYSIZE][q % ARRAYSIZE];
        V0 = &O->uvtex_[L][Q->texts[0] / ARRAYSIZE][Q->texts[0] % ARRAYSIZE];
        V1 = &O->uvtex_[L][Q->texts[1] / ARRAYSIZE][Q->texts[1] % ARRAYSIZE];
        V2 = &O->uvtex_[L][Q->texts[2] / ARRAYSIZE][Q->texts[2] % ARRAYSIZE];
        V3 = &O->uvtex_[L][Q->texts[3] / ARRAYSIZE][Q->texts[3] % ARRAYSIZE];
        boxt_4_2d(&Q->B2, V0, V1, V2, V3, radius);
    }
}

void update_bounding_box_for_Quads(object * O, int L, unsigned char radius)
{
    quadrant * Q;
//    polygon
    int q;
    vertex * V0;
    vertex * V1;
    vertex * V2;
    vertex * V3;
    for (q = 0; q < O->quadcount_[L]; q++)
    {
        Q = &O->quads_[L][q / ARRAYSIZE][q % ARRAYSIZE];
        V0 = &O->verts_[L][Q->verts[0] / ARRAYSIZE][Q->verts[0] % ARRAYSIZE];
        V1 = &O->verts_[L][Q->verts[1] / ARRAYSIZE][Q->verts[1] % ARRAYSIZE];
        V2 = &O->verts_[L][Q->verts[2] / ARRAYSIZE][Q->verts[2] % ARRAYSIZE];
        V3 = &O->verts_[L][Q->verts[3] / ARRAYSIZE][Q->verts[3] % ARRAYSIZE];
        boxt_4(&Q->B, V0, V1, V2, V3, radius);
    }
}

void update_bounding_box_for_transformed_Quads(object * O, int L, unsigned char radius)
{
    quadrant * Q;
//    polygon
    int q;
    vertex * V0;
    vertex * V1;
    vertex * V2;
    vertex * V3;
    for (q = 0; q < O->quadcount_[L]; q++)
    {
        Q = &O->quads_[L][q / ARRAYSIZE][q % ARRAYSIZE];
        V0 = &O->verts_[L][Q->verts[0] / ARRAYSIZE][Q->verts[0] % ARRAYSIZE];
        V1 = &O->verts_[L][Q->verts[1] / ARRAYSIZE][Q->verts[1] % ARRAYSIZE];
        V2 = &O->verts_[L][Q->verts[2] / ARRAYSIZE][Q->verts[2] % ARRAYSIZE];
        V3 = &O->verts_[L][Q->verts[3] / ARRAYSIZE][Q->verts[3] % ARRAYSIZE];
        boxt_4_T(&Q->B, V0, V1, V2, V3, radius);
    }
}

void update_2d_bounding_box_for_Polys(object * O, unsigned char radius)
{
    polygon * P;
    int p;
    for (p = 0; p < O->polycount; p++)
    {
        P = &O->polys[p / ARRAYSIZE][p % ARRAYSIZE];
        boxt_2d_P(O, P, radius);
    }
}

void update_transformed_bounding_box_for_Trip_Quads_(object * O, int L)
{
    quadrant * Q;
    triangle * T;
    int q, t, idx;
    for (q = 0; q < O->quadcount_[L]; q++)
    {
        Q = &O->quads_[L][q / ARRAYSIZE][q % ARRAYSIZE];
        Q->B.Tx = 0.0;
        Q->B.Ty = 0.0;
        Q->B.Tz = 0.0;
        for (t = 0; t < 2; t ++)
        {
            idx = Q->trips[t];
            T = &O->trips_[L][idx / ARRAYSIZE][idx % ARRAYSIZE];
            Q->B.Tx += T->B.Tx;
            Q->B.Ty += T->B.Tx;
            Q->B.Tz += T->B.Tx;
        }
        Q->B.Tx /= 2;
        Q->B.Ty /= 2;
        Q->B.Tz /= 2;
    }
}

void update_transformed_bounding_box_for_Trip_Polys(object * O)
{
    polygon * P;
    triangle * T;
    int p, t, idx;
    for (p = 0; p < O->polycount; p++)
    {
        P = &O->polys[p / ARRAYSIZE][p % ARRAYSIZE];
        P->B.Tx = 0.0;
        P->B.Ty = 0.0;
        P->B.Tz = 0.0;
        for (t = 0; t < P->tripcount; t ++)
        {
            idx = P->trips[t];
            T = &O->trips[idx / ARRAYSIZE][idx % ARRAYSIZE];
            P->B.Tx += T->B.Tx;
            P->B.Ty += T->B.Tx;
            P->B.Tz += T->B.Tx;
        }
        P->B.Tx /= P->tripcount;
        P->B.Ty /= P->tripcount;
        P->B.Tz /= P->tripcount;
    }
}

void update_bounding_box_for_transformed_Polys(object * O, unsigned char radius)
{
    polygon * P;
    int p;
    for (p = 0; p < O->polycount; p++)
    {
        P = &O->polys[p / ARRAYSIZE][p % ARRAYSIZE];
        boxt_P_T(O, P, radius);
    }
}

void update_bounding_box_for_Polys(object * O, unsigned char radius)
{
    polygon * P;
    int p;
    for (p = 0; p < O->polycount; p++)
    {
        P = &O->polys[p / ARRAYSIZE][p % ARRAYSIZE];
        boxt_P(O, P, radius);
    }
}

void generate_transformed_vertexNormals(object * O)
{
    int v, e, idx, E_count;
    float x, y, z;
    float d;
    edge * E;
    vertex * V;
    normal * N;
    for (v = 0; v < O->vertcount; v++)
    {
        V = &O->verts[v / ARRAYSIZE][v % ARRAYSIZE];
        N = &V->N;

        x = y = z = 0.0;

        E_count = V->edgecount;
        for(e = 0; e < V->edgecount; e ++)
        {
            idx = V->edges[e];
            E = &O->edges[idx / ARRAYSIZE][idx % ARRAYSIZE];
            x += E->N.Tx;
            y += E->N.Ty;
            z += E->N.Tz;
        }
        x /= E_count;
        y /= E_count;
        z /= E_count;
        d = sqrt(x * x + y * y + z * z);
        x /= d;
        y /= d;
        z /= d;
        N->Tx = x;
        N->Ty = y;
        N->Tz = z;
    }
}

void generate_vertexNormals_transformed_(object * O, int L)
{
    int v, e, idx, E_count;
    float x, y, z;
    float d;
    edge * E;
    vertex * V;
    normal * N;
    for (v = 0; v < O->vertcount_[L]; v++)
    {
        V = &O->verts_[L][v / ARRAYSIZE][v % ARRAYSIZE];
        N = &V->N;

        x = y = z = 0.0;

        E_count = V->edgecount;
        for(e = 0; e < V->edgecount; e ++)
        {
            idx = V->edges[e];
            E = &O->edges_[L][idx / ARRAYSIZE][idx % ARRAYSIZE];
            x += E->N.Tx;
            y += E->N.Ty;
            z += E->N.Tz;
        }
        x /= E_count;
        y /= E_count;
        z /= E_count;
        d = sqrt(x * x + y * y + z * z);
        x /= d;
        y /= d;
        z /= d;
        N->Tx = x;
        N->Ty = y;
        N->Tz = z;
    }
}

void generate_vertexNormals_(object * O, int L)
{
    int v, e, idx, E_count;
    float x, y, z;
    float d;
    edge * E;
    vertex * V;
    normal * N;
    for (v = 0; v < O->vertcount_[L]; v++)
    {
        V = &O->verts_[L][v / ARRAYSIZE][v % ARRAYSIZE];
        N = &V->N;

        x = y = z = 0.0;

        E_count = V->edgecount;
        for(e = 0; e < V->edgecount; e ++)
        {
            idx = V->edges[e];
            E = &O->edges_[L][idx / ARRAYSIZE][idx % ARRAYSIZE];
            x += E->N.x;
            y += E->N.y;
            z += E->N.z;
        }
        x /= E_count;
        y /= E_count;
        z /= E_count;
        d = sqrt(x * x + y * y + z * z);
        x /= d;
        y /= d;
        z /= d;
        N->x = x;
        N->y = y;
        N->z = z;
    }
}

void generate_vertexNormals(object * O)
{
    int v, e, idx, E_count;
    float x, y, z;
    float d;
    edge * E;
    vertex * V;
    normal * N;
    for (v = 0; v < O->vertcount; v++)
    {
        V = &O->verts[v / ARRAYSIZE][v % ARRAYSIZE];
        N = &V->N;
        x = 0.0;
        y = 0.0;
        z = 0.0;
        E_count = V->edgecount;
        for(e = 0; e < V->edgecount; e ++)
        {
            idx = V->edges[e];
            E = &O->edges[idx / ARRAYSIZE][idx % ARRAYSIZE];
            x += E->N.x;
            y += E->N.y;
            z += E->N.z;
        }
        x /= E_count;
        y /= E_count;
        z /= E_count;
        d = sqrt(x * x + y * y + z * z);
        x /= d;
        y /= d;
        z /= d;
        N->x = x;
        N->y = y;
        N->z = z;
    }
}

void generate_transformed_edgeNormals(object * O)
{
    int e, k, idx, P_count;
    float x, y, z;
    float d;
    polygon * P;
    edge * E;
    normal * N;
    for (e = 0; e < O->edgecount; e++)
    {
        E = &O->edges[e / ARRAYSIZE][e % ARRAYSIZE];
        N = &E->N;
        x = 0.0;
        y = 0.0;
        z = 0.0;
        P_count = E->polycount;
        for (k = 0; k < E->polycount; k++)
        {
            idx = E->polys[k];
            P = &O->polys[idx / ARRAYSIZE][idx % ARRAYSIZE];
            x += P->N.Tx;
            y += P->N.Ty;
            z += P->N.Tz;
        }
        x /= P_count;
        y /= P_count;
        z /= P_count;
        d = sqrt(x * x + y * y + z * z);
        x /= d;
        y /= d;
        z /= d;
        N->Tx = x;
        N->Ty = y;
        N->Tz = z;
    }
}

void generate_transformed_edgeNormals_(object * O, int L)
{
    int e, k, idx, Q_count;
    float x, y, z;
    float d;
    quadrant * Q;
    edge * E;
    normal * N;
    for (e = 0; e < O->edgecount_[L]; e++)
    {
        E = &O->edges_[L][e / ARRAYSIZE][e % ARRAYSIZE];
        N = &E->N;
        x = 0.0;
        y = 0.0;
        z = 0.0;
        Q_count = E->polycount;
        for (k = 0; k < E->polycount; k++)
        {
            idx = E->polys[k];
            Q = &O->quads_[L][idx / ARRAYSIZE][idx % ARRAYSIZE];
            x += Q->N.Tx;
            y += Q->N.Ty;
            z += Q->N.Tz;
        }
        x /= Q_count;
        y /= Q_count;
        z /= Q_count;
        d = sqrt(x * x + y * y + z * z);
        x /= d;
        y /= d;
        z /= d;
        N->Tx = x;
        N->Ty = y;
        N->Tz = z;
    }
}

void generate_edgeNormals_level_(object * O, int L)
{
    int e, k, idx, Q_count;
    float x, y, z;
    float d;
    quadrant * Q;
    edge * E;
    normal * N;
    for (e = 0; e < O->edgecount_[L]; e++)
    {
        E = &O->edges_[L][e / ARRAYSIZE][e % ARRAYSIZE];
        N = &E->N;
        x = 0.0;
        y = 0.0;
        z = 0.0;
        Q_count = E->polycount;
        for (k = 0; k < E->polycount; k++)
        {
            idx = E->polys[k];
            Q = &O->quads_[L][idx / ARRAYSIZE][idx % ARRAYSIZE];
            x += Q->N.x;
            y += Q->N.y;
            z += Q->N.z;
        }
        x /= Q_count;
        y /= Q_count;
        z /= Q_count;
        d = sqrt(x * x + y * y + z * z);
        x /= d;
        y /= d;
        z /= d;
        N->x = x;
        N->y = y;
        N->z = z;
    }
}

void generate_edgeNormals(object * O)
{
    int e, k, idx, P_count;
    float x, y, z;
    float d;
    polygon * P;
    edge * E;
    normal * N;
    for (e = 0; e < O->edgecount; e++)
    {
        E = &O->edges[e / ARRAYSIZE][e % ARRAYSIZE];
        N = &E->N;
        x = 0.0;
        y = 0.0;
        z = 0.0;
        P_count = E->polycount;
        for (k = 0; k < E->polycount; k++)
        {
            idx = E->polys[k];
            P = &O->polys[idx / ARRAYSIZE][idx % ARRAYSIZE];
            x += P->N.x;
            y += P->N.y;
            z += P->N.z;
        }
        x /= P_count;
        y /= P_count;
        z /= P_count;
        d = sqrt(x * x + y * y + z * z);
        x /= d;
        y /= d;
        z /= d;
        N->x = x;
        N->y = y;
        N->z = z;
    }
}

void generate_polyNormals(object * O)
{
    int p, t, idx, T_count;
    float x, y, z;
    float d;
    polygon * P;
    triangle * T;
    normal * N;
    for (p = 0; p < O->polycount; p++)
    {
        P = &O->polys[p / ARRAYSIZE][p % ARRAYSIZE];
        N = &P->N;
        x = 0.0;
        y = 0.0;
        z = 0.0;
        T_count = P->tripcount;
        for(t = 0; t < P->tripcount; t ++)
        {
            idx = P->trips[t];
            T = &O->trips[idx / ARRAYSIZE][idx % ARRAYSIZE];
            x += T->N.x;
            y += T->N.y;
            z += T->N.z;
        }
        x /= T_count;
        y /= T_count;
        z /= T_count;
        d = sqrt(x * x + y * y + z * z);
        x /= d;
        y /= d;
        z /= d;
        N->x = x;
        N->y = y;
        N->z = z;
    }
}

void generate_transformed_polyNormals(object * O)
{
    int p, t, idx, T_count;
    float x, y, z;
    float d;
    polygon * P;
    triangle * T;
    normal * N;
    for (p = 0; p < O->polycount; p++)
    {
        P = &O->polys[p / ARRAYSIZE][p % ARRAYSIZE];
        N = &P->N;
        x = 0.0;
        y = 0.0;
        z = 0.0;
        T_count = P->tripcount;
        for(t = 0; t < P->tripcount; t ++)
        {
            idx = P->trips[t];
            T = &O->trips[idx / ARRAYSIZE][idx % ARRAYSIZE];
            x += T->N.Tx;
            y += T->N.Ty;
            z += T->N.Tz;
        }
        x /= T_count;
        y /= T_count;
        z /= T_count;
        d = sqrt(x * x + y * y + z * z);
        x /= d;
        y /= d;
        z /= d;
        N->Tx = x;
        N->Ty = y;
        N->Tz = z;
    }
}

void update_transformed_Triangles(object * O)
{
    int t;
    triangle * T;
    vertex * V0, * V1, * V2;

    for (t = 0; t < O->tripcount; t++)
    {
        T = &O->trips[t / ARRAYSIZE][t % ARRAYSIZE];

        V0 = &O->verts[T->verts[0] / ARRAYSIZE][T->verts[0] % ARRAYSIZE];
        V1 = &O->verts[T->verts[1] / ARRAYSIZE][T->verts[1] % ARRAYSIZE];
        V2 = &O->verts[T->verts[2] / ARRAYSIZE][T->verts[2] % ARRAYSIZE];
        cross_T(&T->N, V0, V1, V2);
    }
}

void update_transformed_Triangles_level_(object * O, int L)
{
    int t0, t1, q;
    triangle * T0, * T1;
    vertex * V0, * V1, * V2;
    quadrant * Q;

    t0 = 0;
    t1 = 1;
    for (q = 0; q < O->quadcount_[L]; q++)
    {
        T0 = &O->trips_[L][t0 / ARRAYSIZE][t0 % ARRAYSIZE];

        V0 = &O->verts_[L][T0->verts[0] / ARRAYSIZE][T0->verts[0] % ARRAYSIZE];
        V1 = &O->verts_[L][T0->verts[1] / ARRAYSIZE][T0->verts[1] % ARRAYSIZE];
        V2 = &O->verts_[L][T0->verts[2] / ARRAYSIZE][T0->verts[2] % ARRAYSIZE];
        cross_T(&T0->N, V0, V1, V2);

        T1 = &O->trips_[L][t1 / ARRAYSIZE][t1 % ARRAYSIZE];

        V0 = &O->verts_[L][T1->verts[0] / ARRAYSIZE][T1->verts[0] % ARRAYSIZE];
        V1 = &O->verts_[L][T1->verts[1] / ARRAYSIZE][T1->verts[1] % ARRAYSIZE];
        V2 = &O->verts_[L][T1->verts[2] / ARRAYSIZE][T1->verts[2] % ARRAYSIZE];
        cross_T(&T1->N, V0, V1, V2);

        Q = &O->quads_[L][q / ARRAYSIZE][q % ARRAYSIZE];

        Q->N.Tx = (T0->N.Tx + T1->N.Tx) / 2.0;
        Q->N.Ty = (T0->N.Ty + T1->N.Ty) / 2.0;
        Q->N.Tz = (T0->N.Tz + T1->N.Tz) / 2.0;

        normalize_T(&Q->N);

        t0 += 2;
        t1 += 2;
    }
}

void update_Triangles_level_(object * O, int L)
{
    int t0, t1, q;
    quadrant * Q;
    triangle * T0, * T1;
    vertex * V0, * V1, * V2;

    t0 = 0;
    t1 = 1;
    for (q = 0; q < O->quadcount_[L]; q++)
    {
        Q = &O->quads_[L][q / ARRAYSIZE][q % ARRAYSIZE];

        T0 = &O->trips_[L][t0 / ARRAYSIZE][t0 % ARRAYSIZE];

        V0 = &O->verts_[L][T0->verts[0] / ARRAYSIZE][T0->verts[0] % ARRAYSIZE];
        V1 = &O->verts_[L][T0->verts[1] / ARRAYSIZE][T0->verts[1] % ARRAYSIZE];
        V2 = &O->verts_[L][T0->verts[2] / ARRAYSIZE][T0->verts[2] % ARRAYSIZE];
        cross(&T0->N, V0, V1, V2);

        T1 = &O->trips_[L][t1 / ARRAYSIZE][t1 % ARRAYSIZE];

        V0 = &O->verts_[L][T1->verts[0] / ARRAYSIZE][T1->verts[0] % ARRAYSIZE];
        V1 = &O->verts_[L][T1->verts[1] / ARRAYSIZE][T1->verts[1] % ARRAYSIZE];
        V2 = &O->verts_[L][T1->verts[2] / ARRAYSIZE][T1->verts[2] % ARRAYSIZE];
        cross(&T1->N, V0, V1, V2);

        Q->N.x = (T0->N.x + T1->N.x) / 2.0;
        Q->N.y = (T0->N.y + T1->N.y) / 2.0;
        Q->N.z = (T0->N.z + T1->N.z) / 2.0;

        normalize(&Q->N);

        t0 += 2;
        t1 += 2;
    }
}

void generate_Triangles_level_(object * O, int L)
{
    int t0, t1, q;
    quadrant * Q;
    triangle * T0, * T1;
    vertex * V0, * V1, * V2;

    t0 = 0;
    t1 = 1;
    for (q = 0; q < O->quadcount_[L]; q++)
    {
        Q = &O->quads_[L][q / ARRAYSIZE][q % ARRAYSIZE];

        T0 = &O->trips_[L][t0 / ARRAYSIZE][t0 % ARRAYSIZE];
        T0->index = t0;
        T0->poly = Q->index;
        T0->surface = Q->surface;
        T0->selected = Q->selected;
        T0->visible = Q->visible;
        T0->verts[0] = Q->verts[0];
        T0->verts[1] = Q->verts[1];
        T0->verts[2] = Q->verts[2];
        T0->texts[0] = Q->texts[0];
        T0->texts[1] = Q->texts[1];
        T0->texts[2] = Q->texts[2];
        V0 = &O->verts_[L][T0->verts[0] / ARRAYSIZE][T0->verts[0] % ARRAYSIZE];
        V1 = &O->verts_[L][T0->verts[1] / ARRAYSIZE][T0->verts[1] % ARRAYSIZE];
        V2 = &O->verts_[L][T0->verts[2] / ARRAYSIZE][T0->verts[2] % ARRAYSIZE];
        cross(&T0->N, V0, V1, V2);
        Q->trips[0] = t0;

        T1 = &O->trips_[L][t1 / ARRAYSIZE][t1 % ARRAYSIZE];
        T1->index = t1;
        T1->poly = Q->index;
        T1->surface = Q->surface;
        T1->selected = Q->selected;
        T1->visible = Q->visible;
        T1->verts[0] = Q->verts[0];
        T1->verts[1] = Q->verts[2];
        T1->verts[2] = Q->verts[3];
        T1->texts[0] = Q->texts[0];
        T1->texts[1] = Q->texts[2];
        T1->texts[2] = Q->texts[3];
        V0 = &O->verts_[L][T1->verts[0] / ARRAYSIZE][T1->verts[0] % ARRAYSIZE];
        V1 = &O->verts_[L][T1->verts[1] / ARRAYSIZE][T1->verts[1] % ARRAYSIZE];
        V2 = &O->verts_[L][T1->verts[2] / ARRAYSIZE][T1->verts[2] % ARRAYSIZE];
        cross(&T1->N, V0, V1, V2);
        Q->trips[1] = t1;

        Q->N.x = (T0->N.x + T1->N.x) / 2.0;
        Q->N.y = (T0->N.y + T1->N.y) / 2.0;
        Q->N.z = (T0->N.z + T1->N.z) / 2.0;

        normalize(&Q->N);

        t0 += 2;
        t1 += 2;
    }
}

void generate_Triangles(object * O)
{
    int t, p, v, t_count;
    int vert_index0, vert_index1, vert_index2;
    int text_index0, text_index1, text_index2;
    int tripcount = 0;
    polygon * P;
    triangle * T;
    vertex * V0, * V1, * V2;

    for (p = 0; p < O->polycount; p++)
    {
        P = &O->polys[p / ARRAYSIZE][p % ARRAYSIZE];
        tripcount += (P->edgecount - 2);
    }

    t = 0;
    for (p = 0; p < O->polycount; p++)
    {
        P = &O->polys[p / ARRAYSIZE][p % ARRAYSIZE];
        if (P->tripcount < P->edgecount - 2)
            P->trips = realloc(P->trips, (P->edgecount - 2) * sizeof(int));
        P->tripcount = P->edgecount - 2;
        t_count = 0;
        vert_index0 = P->verts[0];
        text_index0 = P->texts[0];
        for (v = 1; v < P->edgecount - 1; v ++)
        {
            vert_index1 = P->verts[v];
            text_index1 = P->texts[v];
            vert_index2 = P->verts[v + 1];
            text_index2 = P->texts[v + 1];
            T = &O->trips[t / ARRAYSIZE][t % ARRAYSIZE];
            T->index = t;
            T->poly = P->index;
            T->surface = P->surface;
            T->selected = P->selected;
            T->visible = P->visible;
            T->verts[0] = vert_index0;
            T->verts[1] = vert_index1;
            T->verts[2] = vert_index2;
            T->texts[0] = text_index0;
            T->texts[1] = text_index1;
            T->texts[2] = text_index2;
            V0 = &O->verts[T->verts[0] / ARRAYSIZE][T->verts[0] % ARRAYSIZE];
            V1 = &O->verts[T->verts[1] / ARRAYSIZE][T->verts[1] % ARRAYSIZE];
            V2 = &O->verts[T->verts[2] / ARRAYSIZE][T->verts[2] % ARRAYSIZE];
            cross(&T->N, V0, V1, V2);
            P->trips[t_count++] = t;
            t++;
        }
    }
}

edgePack generate_Edges_OBJ(OBJ_In * OBJ_File)
{
    int i, e, p, v, V, idx, k, condition;
    int vertcount = OBJ_File->Vertcoord_c / 3;
    int polycount = OBJ_File->Face_c;

    typedef struct
    {
        int index;
        int edges[VERT_EDGES];
        int edgecount;
    }
    vertex_;

    vertex_ ** Vert = malloc(OBJECTARRAY * sizeof(vertex_*)); // Two dimensional indexing.

    i = 0;
    while(i <= vertcount / ARRAYSIZE)
    {
        Vert[i] = (vertex_*) calloc(ARRAYSIZE, sizeof(vertex_));
        if (Vert[i] == NULL)
        {
            vertcount = i * ARRAYSIZE;
            break;
        }
        i ++;
    }

    //vertex_ Vert[vertcount];

    typedef struct
    {
        int index;
        int v0;
        int v1;
        int polys[2]; // Assuming max polycount is 2 per edge.
        int slots[2];
        int p_count;
    }
    edge_;

    int edgecount = 0;

    typedef struct
    {
        int index;
        int edges[POLY_VERTS]; // Now polys get packed into chunks this size.
        int verts[POLY_VERTS]; // It helps speed up loading and makes it reasonable.
        int edgecount;
    }
    polygon_;

    polygon_ ** polys_ = malloc(OBJECTARRAY * sizeof(polygon_*)); // We use scalable array and global indexing method.

    i = 0;
    while(i <= polycount / ARRAYSIZE)
    {
        polys_[i] = (polygon_*) calloc(ARRAYSIZE, sizeof(polygon_));
        if (polys_[i] == NULL)
        {
            polycount = i * ARRAYSIZE;
            break;
        }
        i ++;
    }

    //polygon_ polys_[polycount];

    Face * F;
    polygon_ * P;

    for (p = 0; p < polycount; p ++)
    {
        F = &OBJ_File->Faces[p];
        P = &polys_[p / ARRAYSIZE][p % ARRAYSIZE];
        P->index = p;
        P->edgecount = F->vert_c;
        for (i = 0; i < F->vert_c; i ++)
        {
            P->verts[i] = F->vert[i];
        }
        //memcpy(P->verts, F->vert, sizeof F->vert);
    }

    for (p = 0; p < polycount; p++)
    {
        P = &polys_[p / ARRAYSIZE][p % ARRAYSIZE];
        edgecount += P->edgecount;
        for (v = 0; v < P->edgecount; v++)
        {
            idx = P->verts[v];
            Vert[idx / ARRAYSIZE][idx % ARRAYSIZE].edgecount = 0;
        }
    }

    edge_ ** edges_ = malloc(OBJECTARRAY * sizeof(edge_*)); // Same global indexing.

    i = 0;
    while(i <= edgecount / ARRAYSIZE)
    {
        edges_[i] = (edge_*) calloc(ARRAYSIZE, sizeof(edge_));
        if (edges_[i] == NULL)
        {
            edgecount = i * ARRAYSIZE;
            break;
        }
        i ++;
    }

    //edge_ edges_[edgecount];
    int vert_edgecount = 0;

    e = 0;

    edge_ * E0;
    vertex_ * V0;

    for (p = 0; p < polycount; p++)
    {
        P = &polys_[p / ARRAYSIZE][p % ARRAYSIZE];
        V = P->edgecount;
        for (v = 0; v < V; v++)
        {
            edge_ E = {e, P->verts[v], P->verts[(v + 1) % V], {P->index, -1}, {v, 0}, 1};
            condition = 1;
            for (k = 0; k < e; k ++)
            {
                E0 = &edges_[k / ARRAYSIZE][k % ARRAYSIZE];
                if (E0->p_count < 2 && ((E0->v1 == E.v0 && E0->v0 == E.v1) || (E0->v0 == E.v0 && E0->v1 == E.v1)))
                {
                    condition = 0;
                    E0->polys[1] = P->index;
                    E0->slots[1] = v;
                    E0->p_count = 2;
                    P->edges[v] = E0->index;
                    break;
                }
            }

            if (condition)
            {
                idx = P->verts[v];
                V0 = &Vert[idx / ARRAYSIZE][idx % ARRAYSIZE];
                // here we test against vert edges limit
                if (V0->edgecount < VERT_EDGES)
                {
                    V0->edges[V0->edgecount++] = e;
                    vert_edgecount ++;
                }

                idx = P->verts[(v + 1) % V];
                V0 = &Vert[idx / ARRAYSIZE][idx % ARRAYSIZE];

                if (V0->edgecount < VERT_EDGES)
                {
                    V0->edges[V0->edgecount++] = e;
                    vert_edgecount ++;
                }

                P->edges[v] = e;
                edges_[e / ARRAYSIZE][e % ARRAYSIZE] = E;
                e ++;
            }
        }
    }

    printf("-----\n");

    edgePack Eds;

    Eds.p_verts_c = edgecount;
    Eds.p_edges_c = edgecount;

    Eds.p_verts = malloc(edgecount * sizeof(int));
    Eds.p_edges = malloc(edgecount * sizeof(int));

    Eds.E_count_c = polycount;

    Eds.E_count = malloc(polycount * sizeof(int));

    int p_verts_c = 0;
    int p_edges_c = 0;

    for (p = 0; p < polycount; p++)
    {
        P = &polys_[p / ARRAYSIZE][p % ARRAYSIZE];
        Eds.E_count[p] = P->edgecount;
        for (v = 0; v < P->edgecount; v++)
        {
            Eds.p_verts[p_verts_c ++] = P->verts[v];
            Eds.p_edges[p_edges_c ++] = P->edges[v];
        }
    }

    Eds.vert_edges_c = vert_edgecount;
    Eds.vert_edge_tiks_c = vertcount;

    Eds.vert_edges = malloc(vert_edgecount * sizeof(int));
    Eds.vert_edge_tiks = malloc(vertcount * sizeof(int));

    int vert_edges_Index = 0;

    for (v = 0; v < vertcount; v++)
    {
        V0 = &Vert[v / ARRAYSIZE][v % ARRAYSIZE];
        for (k = 0; k < V0->edgecount; k ++)
        {
            Eds.vert_edges[vert_edges_Index % vert_edgecount] = V0->edges[k]; // triangulated mesh may have lots of edges per vertex
            vert_edges_Index ++;
        }
        Eds.vert_edge_tiks[v] = V0->edgecount;
    }

    Eds.Edge_Poly_Index_c = edgecount;
    Eds.Edge_Poly_Tiks_c = e;
    Eds.Edges_Index_c = e * 2;

    Eds.Edge_Poly_Index = malloc(edgecount * sizeof(int));
    Eds.Edge_Slot_Index = malloc(edgecount * sizeof(int));
    Eds.Edge_Poly_Tiks = malloc(e * sizeof(int));
    Eds.Edges_Index = malloc(e * 2 * sizeof(int));

    int Edge_Poly_Index = 0;
    int Edge_Slot_Index = 0;
    int Edges_Index_c = 0;

    for (v = 0; v < e; v++)
    {
        E0 = &edges_[v / ARRAYSIZE][v % ARRAYSIZE];
        Eds.Edges_Index[Edges_Index_c ++] = E0->v0;
        Eds.Edges_Index[Edges_Index_c ++] = E0->v1;
        for (p = 0; p < E0->p_count; p ++)
        {
            Eds.Edge_Poly_Index[Edge_Poly_Index++] = E0->polys[p];
            Eds.Edge_Slot_Index[Edge_Slot_Index++] = E0->slots[p];
        }
        Eds.Edge_Poly_Tiks[v] = E0->p_count;
    }

    Eds.edgecount = e;

    // FREE

    i = 0;
    while(i <= vertcount / ARRAYSIZE)
    {
        free(Vert[i]);
        i ++;
    }

    free(Vert);

    i = 0;
    while(i <= polycount / ARRAYSIZE)
    {
        free(polys_[i]);
        i ++;
    }

    free(polys_);

    i = 0;
    while(i <= edgecount / ARRAYSIZE)
    {
        free(edges_[i]);
        i ++;
    }

    free(edges_);

    // FREE

    return Eds;
}

UV_edgePack generate_UVEdges_OBJ(OBJ_In * OBJ_File) // When generating uv edge polyconnections,
{                                                   // test for real edge, because uvs may overlap.
    int i, e, p, v, V, idx, k;
    int vertcount = OBJ_File->Textcoord_c / 2;
    int polycount = OBJ_File->Face_c;

    typedef struct
    {
        int index;
        int edges[UV_VERT_EDGES];
        int edgecount;
    }
    vertex_;

    vertex_ ** Vert = malloc(OBJECTARRAY * sizeof(vertex_*));

    i = 0;
    while(i <= vertcount / ARRAYSIZE)
    {
        Vert[i] = (vertex_*) calloc(ARRAYSIZE, sizeof(vertex_));
        if (Vert[i] == NULL)
        {
            vertcount = i * ARRAYSIZE;
            break;
        }
        i ++;
    }

    //vertex_ Vert[vertcount];

    typedef struct
    {
        int index;
        int v0;
        int v1;
        int polys[2];
        int p_count;
    }
    edge_;

    int edgecount = 0;

    typedef struct
    {
        int index;
        int edges[POLY_VERTS];
        int verts[POLY_VERTS];
        int edgecount;
    }
    polygon_;

    polygon_ ** polys_ = malloc(OBJECTARRAY * sizeof(polygon_*));

    i = 0;
    while(i <= polycount / ARRAYSIZE)
    {
        polys_[i] = (polygon_*) calloc(ARRAYSIZE, sizeof(polygon_));
        if (polys_[i] == NULL)
        {
            polycount = i * ARRAYSIZE;
            break;
        }
        i ++;
    }

    //polygon_ polys_[polycount];

    Face * F;
    polygon_ * P;

    for (p = 0; p < polycount; p ++)
    {
        F = &OBJ_File->Faces[p];
        P = &polys_[p / ARRAYSIZE][p % ARRAYSIZE];
        P->index = p;
        P->edgecount = F->vert_c;
        for (i = 0; i < F->vert_c; i ++)
        {
            P->verts[i] = F->text[i];
        }
        //memcpy(P->verts, F->text, sizeof F->text);
    }

    for (p = 0; p < polycount; p++)
    {
        P = &polys_[p / ARRAYSIZE][p % ARRAYSIZE];
        edgecount += P->edgecount;
        for (v = 0; v < P->edgecount; v++)
        {
            idx = P->verts[v];
            Vert[idx / ARRAYSIZE][idx % ARRAYSIZE].edgecount = 0;
        }
    }

    edge_ ** edges_ = malloc(OBJECTARRAY * sizeof(edge_*));

    i = 0;
    while(i <= edgecount / ARRAYSIZE)
    {
        edges_[i] = (edge_*) calloc(ARRAYSIZE, sizeof(edge_));
        if (edges_[i] == NULL)
        {
            edgecount = i * ARRAYSIZE;
            break;
        }
        i ++;
    }

    //edge_ edges_[edgecount];
    int vert_edgecount = 0;

    e = 0;

    edge_ * E0;
    vertex_ * V0;

    for (p = 0; p < polycount; p++) // for each UV edge in every polygon.
    {
        P = &polys_[p / ARRAYSIZE][p % ARRAYSIZE];
        V = P->edgecount;
        for (v = 0; v < V; v++)
        {
            edge_ E = {e, P->verts[v], P->verts[(v + 1) % V], {P->index, -1}, 1};

            idx = P->verts[v];
            V0 = &Vert[idx / ARRAYSIZE][idx % ARRAYSIZE];

            if (V0->edgecount < UV_VERT_EDGES)
            {
                V0->edges[V0->edgecount++] = e;
                vert_edgecount ++;
            }

            idx = P->verts[(v + 1) % V];
            V0 = &Vert[idx / ARRAYSIZE][idx % ARRAYSIZE];
            if (V0->edgecount < UV_VERT_EDGES)
            {
                V0->edges[V0->edgecount++] = e;
                vert_edgecount ++;
            }

            P->edges[v] = e;
            edges_[e / ARRAYSIZE][e % ARRAYSIZE] = E;
            e ++;
        }
    }

    printf("-----\n");

    UV_edgePack Eds;

    Eds.p_texts_c = edgecount;
    Eds.p_uveds_c = edgecount;

    Eds.p_texts = malloc(edgecount * sizeof(int));
    Eds.p_uveds = malloc(edgecount * sizeof(int)); // edge indexes polygon by polygon

    int p_texts_c = 0;
    int p_uveds_c = 0;

    for (p = 0; p < polycount; p++)
    {
        P = &polys_[p / ARRAYSIZE][p % ARRAYSIZE];
        for (v = 0; v < P->edgecount; v++)
        {
            Eds.p_texts[p_texts_c ++] = P->verts[v];
            Eds.p_uveds[p_uveds_c ++] = P->edges[v];
        }
    }

    Eds.uv_text_edges_c = vert_edgecount;
    Eds.uv_edge_tiks_c = vertcount;

    Eds.uv_text_edges = malloc(vert_edgecount * sizeof(int));
    Eds.uv_edge_tiks = malloc(vertcount * sizeof(int)); // need to fill

    int vert_edges_Index = 0;

    for (v = 0; v < vertcount; v++)
    {
        V0 = &Vert[v / ARRAYSIZE][v % ARRAYSIZE];
        for (k = 0; k < V0->edgecount; k ++)
        {
            Eds.uv_text_edges[vert_edges_Index % vert_edgecount] = V0->edges[k]; // triangulated mesh may have lots of edges per vertex
            vert_edges_Index++;
        }
        Eds.uv_edge_tiks[v] = V0->edgecount;
    }

    Eds.UV_Edge_Poly_Index_c = edgecount;
    Eds.UV_Edge_Poly_Tiks_c = e;
    Eds.UV_Edges_Index_c = e * 2;

    Eds.UV_Edge_Poly_Index = malloc(edgecount * sizeof(int));
    Eds.UV_Edge_Poly_Tiks = malloc(e * sizeof(int));
    Eds.UV_Edges_Index = malloc(e * 2 * sizeof(int));

    int Edge_Poly_Index = 0;
    int Edges_Index_c = 0;

    for (k = 0; k < e; k++)
    {
        E0 = &edges_[k / ARRAYSIZE][k % ARRAYSIZE];
        Eds.UV_Edges_Index[Edges_Index_c ++] = E0->v0;
        Eds.UV_Edges_Index[Edges_Index_c ++] = E0->v1;
        for (p = 0; p < E0->p_count; p ++)
        {
            Eds.UV_Edge_Poly_Index[Edge_Poly_Index ++] = E0->polys[p];
        }
        Eds.UV_Edge_Poly_Tiks[k] = E0->p_count;
    }

    Eds.uvedcount = e;

    // FREE

    i = 0;
    while(i <= vertcount / ARRAYSIZE)
    {
        free(Vert[i]);
        i ++;
    }

    free(Vert);

    i = 0;
    while(i <= polycount / ARRAYSIZE)
    {
        free(polys_[i]);
        i ++;
    }

    free(polys_);

    i = 0;
    while(i <= edgecount / ARRAYSIZE)
    {
        free(edges_[i]);
        i ++;
    }

    free(edges_);

    // FREE

    return Eds;
}

int generate_Edges(object * O)
{
    int e, p, v, V, idx, k, condition;
    vertex * Vert;
    typedef struct
    {
        int index;
        int v0;
        int v1;
        int polys[2];
        int slots[2];
        int p_count;
    }edge_;

    int edgecount = 0;

    polygon * P;

    for (p = 0; p < O->polycount; p++)
    {
        P = &O->polys[p / ARRAYSIZE][p % ARRAYSIZE];
        edgecount += P->edgecount;
    }

    for (v = 0; v < O->vertcount; v++)
    {
        Vert = &O->verts[v / ARRAYSIZE][v % ARRAYSIZE];
        Vert->edgecount = 0;
    }

    edge_ * edges_ = calloc(edgecount, sizeof(edge_));
    if (edges_ == NULL) return 0;
    e = 0;
    for (p = 0; p < O->polycount; p++)
    {
        P = &O->polys[p / ARRAYSIZE][p % ARRAYSIZE];
        V = P->edgecount;
        for (v = 0; v < V; v++)
        {
            edge_ E = {e, P->verts[v], P->verts[(v + 1) % V], {P->index, -1}, {v, 0}, 1};
            condition = 1;
            for (k = 0; k < e; k ++)
            {
                if (edges_[k].p_count < 2 && ((edges_[k].v1 == E.v0 && edges_[k].v0 == E.v1) || (edges_[k].v0 == E.v0 && edges_[k].v1 == E.v1)))
                {
                    condition = 0;
                    edges_[k].polys[1] = P->index;
                    edges_[k].slots[1] = v;
                    edges_[k].p_count = 2;
                    P->edges[v] = edges_[k].index;
                    break;
                }
            }

            if (condition)
            {
                idx = P->verts[v];
                Vert = &O->verts[idx / ARRAYSIZE][idx % ARRAYSIZE];
                Vert->edges[Vert->edgecount++] = e;
                idx = P->verts[(v + 1) % V];
                Vert = &O->verts[idx / ARRAYSIZE][idx % ARRAYSIZE];
                Vert->edges[Vert->edgecount++] = e;
                P->edges[v] = e;
                edges_[e++] = E;
            }
        }
    }
    int i, j;
    printf("-----\n");
    edge * E_;
    if (e > O->edgecount)
    {
        edgecount = e - O->edgecount;
        int Edges_Index[edgecount];
        int Poly_Count[edgecount];
        for(i = 0; i < edgecount; i ++) Poly_Count[i] = 2;
        extend_object_edges(O, O->index, edgecount, Edges_Index, Poly_Count);
        e = edgecount;
    }
    for (j = 0; j < e; j++)
    {
        E_ = &O->edges[j / ARRAYSIZE][j % ARRAYSIZE];
        E_->verts[0] = edges_[j].v0;
        E_->verts[1] = edges_[j].v1;
        E_->polys[0] = edges_[j].polys[0];
        E_->slots[0] = edges_[j].slots[0];
        if (edges_[j].p_count > 1)
        {
            E_->polys[1] = edges_[j].polys[1];
            E_->slots[1] = edges_[j].slots[1];
        }
        E_->polycount = edges_[j].p_count;
        E_->index = j;
    }
    O->edgecount = e;
    free(edges_);
    return 1;
}

int generate_UV_Verts(object * O)
{

    /* for every 3d vertex there needs to be one or more uv verts */
    /* loop through all polygons and place uv verts into new array */
    /* vertex is unique if 3d and 2d index combined are unique. */

    int i, text, vert, u, v, V, p, condition;

    typedef struct
    {
        int index;
        int text; // old index
        int vert; // 3d vertex
        int uvcount;
        float u;
        float v;
    }
    uv_;

    polygon * P;
    uv * UV;

    /* sum up max possible uvs */

    int uv_count = 0, textcount;

    for (p = 0; p < O->polycount; p++)
    {
        P = &O->polys[p / ARRAYSIZE][p % ARRAYSIZE];
        uv_count += P->edgecount;
    }

    uv_ * used_uv = calloc(uv_count, sizeof(uv_));
    if (used_uv == NULL) return 0;

    int used_c = 0;

    for (p = 0; p < O->polycount; p ++)
    {
        P = &O->polys[p / ARRAYSIZE][p % ARRAYSIZE];
        V = P->edgecount;
        for (v = 0; v < V; v++)
        {
            vert = P->verts[v];
            text = P->texts[v];
            condition = 1;

            /* if same vertex does not exist */

            for (u = 0; u < used_c; u ++)
            {
                if (used_uv[u].vert == vert && used_uv[u].text == text)
                {
                    condition = 0;
                    P->texts[v] = used_uv[u].index;
                    break;
                }
            }
            if (condition)
            {
                UV = &O->uvtex[text / ARRAYSIZE][text % ARRAYSIZE];
                uv_ uv = {used_c, text, vert, UV->uvcount, UV->u, UV->v};
                used_uv[used_c] = uv;
                P->texts[v] = used_c;
                used_c ++;
            }
        }
    }

    /* if generated uvs exceed existing uvs */

    if (used_c > O->textcount)
    {
        int Edges_count = 0;
        textcount = used_c - O->textcount;
        for(i = O->textcount; i < used_c; i ++)
        {
            Edges_count += used_uv[i].uvcount;
        }
        float * uv_coords = calloc(textcount * 2, sizeof(float));
        int * E_tiks = calloc(textcount, sizeof(int));
        for(i = 0; i < textcount; i ++)
        {
            E_tiks[i] = used_uv[i + O->textcount].uvcount;
        }
        extend_object_texts(O, O->index, used_c, Edges_count, uv_coords, E_tiks);
        free(uv_coords);
        free(E_tiks);
    }

    /* transfer used_uv to global uv arrays */

    for (u = 0; u < used_c; u ++)
    {
        UV = &O->uvtex[u / ARRAYSIZE][u % ARRAYSIZE];
        UV->index = u;
        if (UV->uvcount < used_uv[u].uvcount)
        {
            UV->uv_edges = realloc(UV->uv_edges, used_uv[u].uvcount * sizeof(int));
        }
        UV->uvcount = used_uv[u].uvcount;
        UV->u = used_uv[u].u;
        UV->v = used_uv[u].v;
        UV->vert = used_uv[u].vert;
    }
    free(used_uv);
    return 1;
}

void generate_Edge_UV_Assoc(object * O)
{
    int e, p, idx, idx0;
    polygon * P;
    edge * E;
    uv_edge * UVE;

    for (p = 0; p < O->polycount; p ++)
    {
        P = &O->polys[p / ARRAYSIZE][p % ARRAYSIZE];
        for (e = 0; e < P->edgecount; e ++)
        {
            idx = P->edges[e];
            E = &O->edges[idx / ARRAYSIZE][idx % ARRAYSIZE];
            idx0 = P->uveds[e];
            UVE = &O->uveds[idx0 / ARRAYSIZE][idx0 % ARRAYSIZE];
            E->uv_edges[E->uv_edcount++] = idx0;
            UVE->edge = idx;
        }
    }
    for (e = 0; e < O->edgecount; e ++)
    {
        E = &O->edges[e / ARRAYSIZE][e % ARRAYSIZE];
        if (E->uv_edcount > 1)
        {
            if (E->uv_edges[0] == E->uv_edges[1])
            {
                E->uv_edcount = 1;
            }
            else
            {
                idx = E->uv_edges[0];
                UVE = &O->uveds[idx / ARRAYSIZE][idx % ARRAYSIZE];
                UVE->split = 1;
                idx = E->uv_edges[1];
                UVE = &O->uveds[idx / ARRAYSIZE][idx % ARRAYSIZE];
                UVE->split = 1;
            }
        }
    }
}

int generate_Vert_UV_Assoc(object * O)
{
    int v, e, p, vert, text, condition;
    polygon * P;
    vertex * V;
    uv * UV;

    for (v = 0; v < O->vertcount; v ++)
    {
        V = &O->verts[v / ARRAYSIZE][v % ARRAYSIZE];
        V->uv_verts = calloc(V->edgecount, sizeof(int));
        if (V->uv_verts == NULL)
            return 0;
    }

    for (p = 0; p < O->polycount; p ++)
    {
        P = &O->polys[p / ARRAYSIZE][p % ARRAYSIZE];
        for (e = 0; e < P->edgecount; e ++)
        {
            vert = P->verts[e];
            text = P->texts[e];
            V = &O->verts[vert / ARRAYSIZE][vert % ARRAYSIZE];
            condition = 1;
            for (v = 0; v < V->uv_vertcount; v ++)
            {
                if (V->uv_verts[v] == text)
                {
                    condition = 0;
                    break;
                }
            }
            if (condition)
            {
                V->uv_verts[V->uv_vertcount++] = text;
                UV = &O->uvtex[text / ARRAYSIZE][text % ARRAYSIZE];
                UV->vert = vert;
            }
        }
    }
    return 1;
}

int generate_Vert_UV_Assoc_(object * O, int L)
{
    int v, e, q, vert, text, condition;
    quadrant * Q;
    vertex * V;
    uv * UV;

    for (v = 0; v < O->vertcount_[L]; v ++)
    {
        V = &O->verts_[L][v / ARRAYSIZE][v % ARRAYSIZE];
        V->uv_verts = calloc(V->edgecount, sizeof(int));
        if (V->uv_verts == NULL)
            return 0;
    }

    for (q = 0; q < O->quadcount_[L]; q ++)
    {
        Q = &O->quads_[L][q / ARRAYSIZE][q % ARRAYSIZE];
        for (e = 0; e < 4; e ++)
        {
            vert = Q->verts[e];
            text = Q->texts[e];
            V = &O->verts_[L][vert / ARRAYSIZE][vert % ARRAYSIZE];
            condition = 1;
            for (v = 0; v < V->uv_vertcount; v ++)
            {
                if (V->uv_verts[v] == text)
                {
                    condition = 0;
                    break;
                }
            }
            if (condition)
            {
                V->uv_verts[V->uv_vertcount++] = text;
                UV = &O->uvtex_[L][text / ARRAYSIZE][text % ARRAYSIZE];
                UV->vert = vert;
            }
        }
    }
    return 1;
}

int generate_Vert_UV_Verts(object * O)
{
    int u, v, idx, condition;

    vertex * V;
    uv * UV;

    for (v = 0; v < O->vertcount; v ++)
    {
        V = &O->verts[v / ARRAYSIZE][v % ARRAYSIZE];
        V->uv_verts = calloc(V->edgecount, sizeof(int));
        if (V->uv_verts == NULL)
            return 0;
    }

    for (v = 0; v < O->textcount; v ++)
    {
        UV = &O->uvtex[v / ARRAYSIZE][v % ARRAYSIZE];
        idx = UV->vert;
        V = &O->verts[idx / ARRAYSIZE][idx % ARRAYSIZE];
        condition = 1;
        for (u = 0; u < V->uv_vertcount; u ++)
        {
            if (V->uv_verts[u] == UV->index)
            {
                condition = 0;
                break;
            }
        }

        if (condition)
        {
            V->uv_verts[V->uv_vertcount++] = UV->index;
        }
    }
    return 1;
}

int generate_UVEdges(object * O)
{
    int e, p, v, V, idx, k, condition, edgecount, slot;
    uv * UVert;
    typedef struct
    {
        int index;
        int v0;
        int v1;
        int p0;
        int p1;
        int p_count;
        int edge;
        int open;
        int split;
        int slots[2];
    }edge_;

    edgecount = 0;

    polygon * P;
    edge * E0;

    int EP;

    for (p = 0; p < O->polycount; p++)
    {
        P = &O->polys[p / ARRAYSIZE][p % ARRAYSIZE];
        edgecount += P->edgecount;
    }

    for (v = 0; v < O->textcount; v++)
    {
        UVert = &O->uvtex[v / ARRAYSIZE][v % ARRAYSIZE];
        UVert->uvcount = 0;
    }

    //edge_ edges_[edgecount];
    edge_ * edges_ = calloc(edgecount, sizeof(edge_));
    if (edges_ == NULL) return 0;
    e = 0;
    for (p = 0; p < O->polycount; p++)
    {
        P = &O->polys[p / ARRAYSIZE][p % ARRAYSIZE];
        V = P->edgecount;
        for (v = 0; v < V; v++)
        {
            edge_ E = {e, P->texts[v], P->texts[(v + 1) % V], P->index, -1, 1, -1, 0, 0, {v, 0}};
            condition = 1;
            idx = P->edges[v];
            E0 = &O->edges[idx / ARRAYSIZE][idx % ARRAYSIZE];
            // find also uv edges belonging
            // to this polygon edge
            if (E0->polycount > 1)
            {
                EP = E0->polys[0];
                slot = E0->slots[0];
                if (EP == P->index)
                {
                    EP = E0->polys[1];
                    slot = E0->slots[1];
                }
                for (k = 0; k < e; k ++)
                {
                    /* if endpoints match */
                    if (edges_[k].p_count < 2 && ((edges_[k].v1 == E.v0 && edges_[k].v0 == E.v1) || (edges_[k].v0 == E.v0 && edges_[k].v1 == E.v1)))
                    {
                        if (edges_[k].open == 0 && edges_[k].p0 == EP && edges_[k].slots[0] == slot)
                        {
                        /* if polygons are adjacent in 3d */
                            condition = 0;
                            edges_[k].p1 = P->index;
                            edges_[k].p_count = 2;
                            edges_[k].slots[1] = v;
                            P->uveds[v] = edges_[k].index;
//                            P->uvdir[v] = 1; /* assume not flipped */
                            break;
                        }
                        else
                        {
                            condition = 2;
                            //break;
                        }
                        /* polygons do not meet, edges stack */
                    }
                }
            }
            /* else it is open in 3d and also in 2d */
            else
            {
                E.open = 1;
            }

            if (condition)
            {
                /* avoid geometry with collapsed and stacked edges */

                /* creating uv texts edge info */
                idx = P->texts[v];
                UVert = &O->uvtex[idx / ARRAYSIZE][idx % ARRAYSIZE];
                UVert->uv_edges[UVert->uvcount++] = e;
                idx = P->texts[(v + 1) % V];
                UVert = &O->uvtex[idx / ARRAYSIZE][idx % ARRAYSIZE];
                UVert->uv_edges[UVert->uvcount++] = e;

                /* adding edge to polygon */
                P->uveds[v] = e;

                // was set to zero in initializing functions
                /* 3d edge collects uv edges below
                split edge means two edges */
                E0->uv_edges[E0->uv_edcount ++] = e;
                E.edge = E0->index;
                edges_[e] = E;
                e ++;
            }
        }
    }
    int i, j;
    printf("generate UVEdges -----\n");
    uv_edge * E_;
    if (e > O->uvedcount)
    {
        edgecount = e - O->uvedcount;
        int Edges_Index[edgecount];
        int Poly_Count[edgecount];
        for(i = 0; i < edgecount; i ++) Poly_Count[i] = 2;
        extend_object_uv_edges(O, O->index, edgecount, Edges_Index, Poly_Count);
        e = edgecount;
    }
    for (j = 0; j < e; j++)
    {
        E_ = &O->uveds[j / ARRAYSIZE][j % ARRAYSIZE];
        E_->uv_verts[0] = edges_[j].v0;
        E_->uv_verts[1] = edges_[j].v1;
        E_->polys[0] = edges_[j].p0;
        if (edges_[j].p_count > 1)
        E_->polys[1] = edges_[j].p1;
        E_->polycount = edges_[j].p_count;
        E_->index = j;
        E_->edge = edges_[j].edge;
        E_->slots[0] = edges_[j].slots[0];
        E_->slots[1] = edges_[j].slots[1];
    }
    O->uvedcount = e;
    free(edges_);
    return 1;
}

void generate_Edge_Split(object * O)
{
    int e, idx;
    edge * E;
    uv_edge * UVE;
    for (e = 0; e < O->edgecount; e ++)
    {
        E = &O->edges[e / ARRAYSIZE][e % ARRAYSIZE];
        if (E->uv_edcount > 1)
        {
            idx = E->uv_edges[0];
            UVE = &O->uveds[idx / ARRAYSIZE][idx % ARRAYSIZE];
            UVE->split = 1;
            idx = E->uv_edges[1];
            UVE = &O->uveds[idx / ARRAYSIZE][idx % ARRAYSIZE];
            UVE->split = 1;
        }
    }
}

void generate_UV_Verts_Edges(object * O)
{
    int e, v, idx;
    uv * UVert;
    uv_edge * UVE;

    for (v = 0; v < O->textcount; v++)
    {
        UVert = &O->uvtex[v / ARRAYSIZE][v % ARRAYSIZE];
        UVert->uvcount = 0;
    }

    for (e = 0; e < O->uvedcount; e ++)
    {
        UVE = &O->uveds[e / ARRAYSIZE][e % ARRAYSIZE];
        /* if endpoints match */
        idx = UVE->uv_verts[0];
        UVert = &O->uvtex[idx / ARRAYSIZE][idx % ARRAYSIZE];
        UVert->uv_edges[UVert->uvcount++] = e;
        idx = UVE->uv_verts[1];
        UVert = &O->uvtex[idx / ARRAYSIZE][idx % ARRAYSIZE];
        UVert->uv_edges[UVert->uvcount++] = e;
    }
}
