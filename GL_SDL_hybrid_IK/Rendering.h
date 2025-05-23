/*
The MIT License

Copyright <2018> <2022> <Allan Kiipli>
*/

#define TOLERANCE 10

#define POLYGON_ID_COLORS 2
#define OBJECT_ID_COLORS 1
#define OBJECT_COLORS 0

#define OBJES 0
#define POLYS 1
#define EDGES 2
#define VERTS 3
#define BONEZ 4
#define CURVE 5
#define CURVE_CP 6
#define CURVE_SEGMENT 7

// to do: migrate pixels to GL
// make ray traced rendering progressive
// develop lights, shadows, alpha for pixel textures

int PLANAR = 1;
int VAO = 0;

typedef struct
{
    float R;
    float G;
    float B;
    float A;
}
RGBA;

float light_vec[3] = {-1.0, -1.0, -1.0};

Uint8 wire_r = 100;
Uint8 wire_g = 100;
Uint8 wire_b = 100;
Uint8 wire_a = 255;

GLubyte dark_wire_color[4] = {10, 10, 50, 155};
GLubyte wire_color[4] = {100, 100, 150, 155};
GLubyte dark_deform_color[4] = {50, 50, 155, 155};
GLubyte deform_color[4] = {150, 150, 255, 155};
GLubyte selected_color[4] = {200, 250, 150, 155};
GLubyte selected_edge_color[4] = {255, 255, 255, 155};
GLubyte selected_deform_color[4] = {200, 200, 100, 155};
GLubyte current_color[4] = {255, 155, 150, 155};
GLubyte current_deform_color[4] = {255, 200, 250, 155};
GLubyte vert_color[4] = {0, 0, 0, 255};
GLubyte selected_vert_color[4] = {255, 255, 255, 255};
GLubyte * line_color = NULL;
GLubyte segment_color[4] = {255, 0, 0, 255};
GLubyte Line_Color[4] = {0, 0, 0, 0};
GLubyte line_red[4] = {255, 0, 0, 255};
GLubyte line_cyan[4] = {0, 255, 255, 255};
GLubyte line_green[4] = {0, 255, 0, 255};
GLubyte line_blue[4] = {0, 0, 255, 255};
GLubyte line_gray[4] = {100, 100, 100, 255};
GLubyte line_white[4] = {255, 255, 255, 255};
GLubyte line_yellow[4] = {255, 255, 0, 255};

typedef struct
{
    int result;
    HexG ** G;
    int g_idx;
}
HexPack;

typedef struct
{
    float uv[2];
}
uvPack;

typedef struct
{
    float uv[2];
    float normal[3];
}
texelPack;

typedef struct
{
    float x;
    float y;
    float z;
    float nx;
    float ny;
    float nz;
}
polyplane;

typedef struct
{
    float R[PIXEL_VOLUME], G[PIXEL_VOLUME], B[PIXEL_VOLUME], A[PIXEL_VOLUME], D[PIXEL_VOLUME];
    float X[PIXEL_VOLUME], Y[PIXEL_VOLUME], Z[PIXEL_VOLUME]; // pixel location
    float NX[PIXEL_VOLUME], NY[PIXEL_VOLUME], NZ[PIXEL_VOLUME]; // shading normal negative
    triangle * T[PIXEL_VOLUME];
    quadrant * Q[PIXEL_VOLUME];
    int level[PIXEL_VOLUME]; // level of triangle
    object * O[PIXEL_VOLUME];
}
pixel;

typedef struct
{
    float S[PIXEL_VOLUME]; // shadow
    //float A[PIXEL_VOLUME]; // alpha
    float D[PIXEL_VOLUME]; // depth
}
light_pixel;

/*inline*/ float length(float V[3])
{
    float l = sqrt(V[0] * V[0] + V[1] * V[1] + V[2] * V[2]);
    return l;
}

/*inline*/ aim vector3dD(GLdouble A[3], float B[3])
{
    aim a;
    a.vec[0] = A[0] - B[0];
    a.vec[1] = A[1] - B[1];
    a.vec[2] = A[2] - B[2];
    a.dist = length(a.vec);
    if (a.dist > 0)
    {
        a.vec[0] /= a.dist;
        a.vec[1] /= a.dist;
        a.vec[2] /= a.dist;
    }
    return a;
}

/*inline*/ aim vector3dF(float A[3], float B[3])
{
    aim a;
    a.vec[0] = A[0] - B[0];
    a.vec[1] = A[1] - B[1];
    a.vec[2] = A[2] - B[2];
    a.dist = length(a.vec);
    if (a.dist > 0)
    {
        a.vec[0] /= a.dist;
        a.vec[1] /= a.dist;
        a.vec[2] /= a.dist;
    }
    return a;
}

aim vector3d_T(vertex * V, float B[3])
{
    aim a;
    a.vec[0] = V->Tx - B[0];
    a.vec[1] = V->Ty - B[1];
    a.vec[2] = V->Tz - B[2];
    a.dist = length(a.vec);
    if (a.dist > 0)
    {
        a.vec[0] /= a.dist;
        a.vec[1] /= a.dist;
        a.vec[2] /= a.dist;
    }
    return a;
}

/*inline*/ aim vector3d(box_3d A, float B[3])
{
    aim a;
    a.vec[0] = A.Tx - B[0];
    a.vec[1] = A.Ty - B[1];
    a.vec[2] = A.Tz - B[2];
    a.dist = length(a.vec);
    if (a.dist > 0)
    {
        a.vec[0] /= a.dist;
        a.vec[1] /= a.dist;
        a.vec[2] /= a.dist;
    }
    return a;
}

HexPack collect_Hexas_light(HexG * H, direction D, HexG ** G, int g_idx, int result)
{
    HexG * H_0;
    HexPack HP;
    HP.result = result;
    HP.G = G;
    HP.g_idx = g_idx;

    int h;
    float deviation;

    deviation = acos(dot_product((normal *)&H->D_light0, (normal *)&D));

    if (deviation < H->Radius)
    {
        if (H->subdivided)
        {
            for (h = 0; h < 7; h ++)
            {
                H_0 = H->subH[h];
                HP = collect_Hexas_light(H_0, D, HP.G, HP.g_idx, HP.result);
            }
        }

        if (g_idx < HEXAS && H->lightpacks > 0)
        {
            HP.G[HP.g_idx ++] = H;
            HP.result ++;
        }
    }
    return HP;
}

HexPack collect_Hexa_Groups_light(direction D, HexG ** G, int g_idx)
{
    int h;

    HexG * H;

    HexPack HP;
    HP.G = G;
    HP.g_idx = g_idx;

    for (h = 0; h < 7; h ++)
    {
        H = Hex_Group[h];
        HP.result = 0;
        HP = collect_Hexas_light(H, D, HP.G, HP.g_idx, HP.result);
    }

    return HP;
}

HexPack collect_Hexas(HexG * H, direction D, HexG ** G, int g_idx, int result)
{
    HexG * H_0;
    HexPack HP;
    HP.result = result;
    HP.G = G;
    HP.g_idx = g_idx;

    int h;
    float deviation;

    deviation = acos(dot_product((normal *)&H->D, (normal *)&D));

    if (deviation < H->Radius)
    {
        if (H->subdivided)
        {
            for (h = 0; h < 7; h ++)
            {
                H_0 = H->subH[h];
                HP = collect_Hexas(H_0, D, HP.G, HP.g_idx, HP.result);
            }
        }

        if (g_idx < HEXAS && H->polypacks > 0)
        {
            HP.G[HP.g_idx ++] = H;
            HP.result ++;
        }
    }
    return HP;
}

HexPack collect_Hexa_Groups(direction D, HexG ** G, int g_idx)
{
    int h;

    HexG * H;

    HexPack HP;
    HP.G = G;
    HP.g_idx = g_idx;

    for (h = 0; h < 7; h ++)
    {
        H = Hex_Group[h];
        HP.result = 0;
        HP = collect_Hexas(H, D, HP.G, HP.g_idx, HP.result);
    }

    return HP;
}

/*inline*/ polyplane init_plane(float points[3], normal * N)
{
    polyplane p = {points[0], points[1], points[2], N->x, N->y, N->z};
    return p;
}

/*inline*/ float nearest(float point[3], polyplane  plane)
{
    float dist;
    float h_norm[3] = {plane.x - point[0], plane.y - point[1], plane.z - point[2]};
    float h = length(h_norm);
    if (h > 0)
    {
        h_norm[0] /= h;
        h_norm[1] /= h;
        h_norm[2] /= h;
        float dot = h_norm[0] * plane.nx + h_norm[1] * plane.ny + h_norm[2] * plane.nz;
        dist = dot * h;
    }
    else
    {
        dist = 0;
    }
    return dist;
}

void populate_box_3d_Aim_And_Deviation(camera * C, int level)
{
    int o, p, q, q0, idx, t, v, L;

    vertex * V;
    object * O;
    polygon * P;
    quadrant * Q, * Q0;
    //edge * E;
    triangle * T;

    aim polyAim, vertexAim;

    float dot, Dot;

    normal polynormal;

    polyplane plane;

    float polypoints[3];

    float BACKFACE_QUALIFIER = 0.0; //-0.5; // 0.0

/*
    normal D;
    D.x = C->T->rotVec_[2][0];
    D.y = C->T->rotVec_[2][1];
    D.z = C->T->rotVec_[2][2];
*/

    int l, l0;

    if (level > 3) level = 3;

    for (o = 0; o < C->object_count; o ++)
    {
        O = objects[C->objects[o]];

        if (level > O->subdlevel)
        {
            L = O->subdlevel;
        }
        else
        {
            L = level;
        }

        if (O->subdlevel == -1)
        {
            for (v = 0; v < O->vertcount; v ++)
            {
                V = &O->verts[v / ARRAYSIZE][v % ARRAYSIZE];

                vertexAim = vector3d_T(V, C->T->pos);

                V->aim_vec[0] = vertexAim.vec[0];
                V->aim_vec[1] = vertexAim.vec[1];
                V->aim_vec[2] = vertexAim.vec[2];
            }

            for (p = 0; p < O->polycount; p ++)
            {
                P = &O->polys[p / ARRAYSIZE][p % ARRAYSIZE];

                polyAim = vector3d(P->B, C->T->pos);

                Dot = 1.0;

                for (v = 0; v < P->edgecount; v ++)
                {
                    idx = P->verts[v];

                    V = &O->verts[idx / ARRAYSIZE][idx % ARRAYSIZE];

                    dot = dot_productFF(V->aim_vec, polyAim.vec);

                    if (dot < Dot)
                    {
                        Dot = dot;
                    }
                }

                P->B.deviation = acos(Dot);
                P->B.Aim.dist = polyAim.dist + P->B.deviation;
                P->B.Aim.vec[0] = polyAim.vec[0];
                P->B.Aim.vec[1] = polyAim.vec[1];
                P->B.Aim.vec[2] = polyAim.vec[2];

                P->B.backface = 1;

                for (t = 0; t < P->tripcount; t ++)
                {
                    idx = P->trips[t];

                    T = &O->trips[idx / ARRAYSIZE][idx % ARRAYSIZE];

                    polyAim = vector3d(T->B, C->T->pos);

                    idx = T->verts[1];

                    V = &O->verts[idx / ARRAYSIZE][idx % ARRAYSIZE];

                    polynormal.x = -T->N.Tx;
                    polynormal.y = -T->N.Ty;
                    polynormal.z = -T->N.Tz;

                    polypoints[0] = V->Tx;
                    polypoints[1] = V->Ty;
                    polypoints[2] = V->Tz;

                    plane = init_plane(polypoints, &polynormal);

                    T->B.Aim.dist = nearest(C->T->pos, plane); // polyAim.dist;

                    dot = dot_productN(&polynormal, polyAim.vec);

                    if (dot > BACKFACE_QUALIFIER)
                    {
                        T->B.backface = 0;
                        P->B.backface = 0;
                    }
                    else
                    {
                        T->B.backface = 1;
                    }
                }
            }
        }
        else
        {
            for (l = L; l >= 0; l --)
            {
                if (O->subdlevel < l)
                {
                    continue;
                }

                for (v = 0; v < O->vertcount_[l]; v ++)
                {
                    V = &O->verts_[l][v / ARRAYSIZE][v % ARRAYSIZE];

                    vertexAim = vector3d_T(V, C->T->pos);

                    V->aim_vec[0] = vertexAim.vec[0];
                    V->aim_vec[1] = vertexAim.vec[1];
                    V->aim_vec[2] = vertexAim.vec[2];
                }

                for (q = 0; q < O->quadcount_[l]; q ++)
                {
                    Q = &O->quads_[l][q / ARRAYSIZE][q % ARRAYSIZE];

                    if (Q->subdivs && l < L)
                    {
                        idx = O->vertcount_[l] + O->edgecount_[l] + q;

                        V = &O->verts_[l + 1][idx / ARRAYSIZE][idx % ARRAYSIZE];

                        l0 = l + 1;

                        while (l0 < L) // delegates to current subdivision
                        {
                            l0 ++;
                            V = V->vert;
                        }

                        polyAim = vector3d_T(V, C->T->pos);

                        Dot = 1.0;

                        for (v = 0; v < 4; v ++)
                        {
                            idx = Q->verts[v];

                            V = &O->verts_[l][idx / ARRAYSIZE][idx % ARRAYSIZE];

                            l0 = l;

                            while (l0 < L) // delegates to current subdivision
                            {
                                l0 ++;
                                V = V->vert;
                            }

                            dot = dot_productFF(V->aim_vec, polyAim.vec);

                            if (dot < Dot)
                            {
                                Dot = dot;
                            }
                        }

                        Q->B.deviation = acos(Dot) + C->Pixel_Size_In_Radians; // adding slight radius
                        Q->B.Aim.dist = polyAim.dist;
                        Q->B.Aim.vec[0] = polyAim.vec[0];
                        Q->B.Aim.vec[1] = polyAim.vec[1];
                        Q->B.Aim.vec[2] = polyAim.vec[2];

                        /* if deviation is less than View Radius in Radians divided by Resolution Radius
                        we have a one pixel size */

                        Q->B.backface = 1;

                        for (q0 = 0; q0 < 4; q0 ++)
                        {
                            idx = Q->quads[q0];

                            Q0 = &O->quads_[l + 1][idx / ARRAYSIZE][idx % ARRAYSIZE];

                            if (Q0->B.backface == 0)
                            {
                                Q->B.backface = 0;
                                break;
                            }
                        }
                    }
                    else
                    {
                        polyAim = vector3d(Q->B, C->T->pos);

                        Dot = 1.0;

                        for (v = 0; v < 4; v ++)
                        {
                            idx = Q->verts[v];

                            V = &O->verts_[l][idx / ARRAYSIZE][idx % ARRAYSIZE];

                            dot = dot_productFF(V->aim_vec, polyAim.vec);

                            if (dot < Dot)
                            {
                                Dot = dot;
                            }
                        }

                        Q->B.deviation = acos(Dot) + C->Pixel_Size_In_Radians; // adding slight radius
                        Q->B.Aim.dist = polyAim.dist;
                        Q->B.Aim.vec[0] = polyAim.vec[0];
                        Q->B.Aim.vec[1] = polyAim.vec[1];
                        Q->B.Aim.vec[2] = polyAim.vec[2];

                        Q->B.backface = 1;

                        for (t = 0; t < 2; t ++)
                        {
                            idx = Q->trips[t];

                            T = &O->trips_[l][idx / ARRAYSIZE][idx % ARRAYSIZE];

                            polyAim = vector3d(T->B, C->T->pos);

                            idx = T->verts[1];

                            V = &O->verts_[l][idx / ARRAYSIZE][idx % ARRAYSIZE];

                            polynormal.x = -T->N.Tx;
                            polynormal.y = -T->N.Ty;
                            polynormal.z = -T->N.Tz;

                            polypoints[0] = V->Tx;
                            polypoints[1] = V->Ty;
                            polypoints[2] = V->Tz;

                            plane = init_plane(polypoints, &polynormal);

                            T->B.Aim.dist = nearest(C->T->pos, plane); // polyAim.dist;

                            dot = dot_productN(&polynormal, polyAim.vec);

                            if (dot > BACKFACE_QUALIFIER)
                            {
                                T->B.backface = 0;
                                Q->B.backface = 0;
                            }
                            else
                            {
                                T->B.backface = 1;
                            }
                        }
                    }
                }
            }

            /* level -1 */

            for (p = 0; p < O->polycount; p ++)
            {
                P = &O->polys[p / ARRAYSIZE][p % ARRAYSIZE];

                idx = O->vertcount + O->edgecount + p;

                V = &O->verts_[0][idx / ARRAYSIZE][idx % ARRAYSIZE];

                l = 1;

                while (l < L) // delegates to current subdivision
                {
                    l ++;
                    V = V->vert;
                }

                polyAim = vector3d_T(V, C->T->pos);

                Dot = 1.0;

                for (v = 0; v < P->edgecount; v ++)
                {
                    idx = O->vertcount + P->edges[v];

                    V = &O->verts_[0][idx / ARRAYSIZE][idx % ARRAYSIZE];

                    l = 0;

                    while (l < L) // delegates to current subdivision
                    {
                        l ++;
                        V = V->vert;
                    }

                    dot = dot_productFF(V->aim_vec, polyAim.vec);

                    if (dot < Dot)
                    {
                        Dot = dot;
                    }

                    /////

                    idx = P->verts[v];

                    V = &O->verts[idx / ARRAYSIZE][idx % ARRAYSIZE];

                    l = -1;

                    while (l < L) // delegates to current subdivision
                    {
                        l ++;
                        V = V->vert;
                    }

                    dot = dot_productFF(V->aim_vec, polyAim.vec);

                    if (dot < Dot)
                    {
                        Dot = dot;
                    }
                }

                P->B.deviation = acos(Dot);
                P->B.Aim.dist = polyAim.dist + P->B.deviation;;
                P->B.Aim.vec[0] = polyAim.vec[0];
                P->B.Aim.vec[1] = polyAim.vec[1];
                P->B.Aim.vec[2] = polyAim.vec[2];

                P->B.backface = 1;

                for (q = 0; q < P->edgecount; q ++)
                {
                    idx = P->quads[q];

                    Q = &O->quads_[0][idx / ARRAYSIZE][idx % ARRAYSIZE];

                    if (Q->B.backface == 0)
                    {
                        P->B.backface = 0;
                        break;
                    }
                }
            }
        }
    }
}

void populate_box_3d_Aim_And_Deviation_light(camera * C, int level)
{
    int o, p, q, q0, idx, t, v, L;

    vertex * V;
    object * O;
    polygon * P;
    quadrant * Q, * Q0;
    //edge * E;
    triangle * T;

    aim polyAim, vertexAim;

    float dot, Dot;

    normal polynormal;

    polyplane plane;

    float polypoints[3];

    float BACKFACE_QUALIFIER = 0.0; // -0.5; // 0.0 // could be frontface qualifier when dot term is reversed and negated

/*
    normal D;
    D.x = C->T->rotVec_[2][0];
    D.y = C->T->rotVec_[2][1];
    D.z = C->T->rotVec_[2][2];
*/

    int l, l0;

    if (level > 3) level = 3;

    for (o = 0; o < C->object_count; o ++)
    {
        O = objects[C->objects[o]];

        if (level > O->subdlevel)
        {
            L = O->subdlevel;
        }
        else
        {
            L = level;
        }

        if (O->subdlevel == -1)
        {
            for (v = 0; v < O->vertcount; v ++)
            {
                V = &O->verts[v / ARRAYSIZE][v % ARRAYSIZE];

                vertexAim = vector3d_T(V, C->T->pos);

                V->aim_vec[0] = vertexAim.vec[0];
                V->aim_vec[1] = vertexAim.vec[1];
                V->aim_vec[2] = vertexAim.vec[2];
            }

            for (p = 0; p < O->polycount; p ++)
            {
                P = &O->polys[p / ARRAYSIZE][p % ARRAYSIZE];

                polyAim = vector3d(P->B, C->T->pos);

                Dot = 1.0;

                for (v = 0; v < P->edgecount; v ++)
                {
                    idx = P->verts[v];

                    V = &O->verts[idx / ARRAYSIZE][idx % ARRAYSIZE];

                    dot = dot_productFF(V->aim_vec, polyAim.vec);

                    if (dot < Dot)
                    {
                        Dot = dot;
                    }
                }

                P->B_light.deviation = acos(Dot);
                P->B_light.Aim.dist = polyAim.dist + P->B_light.deviation;
                P->B_light.Aim.vec[0] = polyAim.vec[0];
                P->B_light.Aim.vec[1] = polyAim.vec[1];
                P->B_light.Aim.vec[2] = polyAim.vec[2];

                P->B_light.backface = 1;

                for (t = 0; t < P->tripcount; t ++)
                {
                    idx = P->trips[t];

                    T = &O->trips[idx / ARRAYSIZE][idx % ARRAYSIZE];

                    polyAim = vector3d(T->B, C->T->pos);

                    idx = T->verts[1];

                    V = &O->verts[idx / ARRAYSIZE][idx % ARRAYSIZE];

                    polynormal.x = -T->N.Tx;
                    polynormal.y = -T->N.Ty;
                    polynormal.z = -T->N.Tz;

                    polypoints[0] = V->Tx;
                    polypoints[1] = V->Ty;
                    polypoints[2] = V->Tz;

                    plane = init_plane(polypoints, &polynormal);

                    T->B_light.Aim.dist = nearest(C->T->pos, plane); // polyAim.dist;

                    dot = dot_productN(&polynormal, polyAim.vec);

                    if (dot < BACKFACE_QUALIFIER) // reversed
                    {
                        T->B_light.backface = 0;
                        P->B_light.backface = 0;
                    }
                    else
                    {
                        T->B_light.backface = 1;
                    }
                }
            }
        }
        else
        {
            for (l = L; l >= 0; l --)
            {
                if (O->subdlevel < l)
                {
                    continue;
                }

                for (v = 0; v < O->vertcount_[l]; v ++)
                {
                    V = &O->verts_[l][v / ARRAYSIZE][v % ARRAYSIZE];

                    vertexAim = vector3d_T(V, C->T->pos);

                    V->aim_vec[0] = vertexAim.vec[0];
                    V->aim_vec[1] = vertexAim.vec[1];
                    V->aim_vec[2] = vertexAim.vec[2];
                }

                for (q = 0; q < O->quadcount_[l]; q ++)
                {
                    Q = &O->quads_[l][q / ARRAYSIZE][q % ARRAYSIZE];

                    if (Q->subdivs && l < L)
                    {
                        idx = O->vertcount_[l] + O->edgecount_[l] + q;

                        V = &O->verts_[l + 1][idx / ARRAYSIZE][idx % ARRAYSIZE];

                        l0 = l + 1;

                        while (l0 < L) // delegates to current subdivision
                        {
                            l0 ++;
                            V = V->vert;
                        }

                        polyAim = vector3d_T(V, C->T->pos);

                        Dot = 1.0;

                        for (v = 0; v < 4; v ++)
                        {
                            idx = Q->verts[v];

                            V = &O->verts_[l][idx / ARRAYSIZE][idx % ARRAYSIZE];

                            l0 = l;

                            while (l0 < L) // delegates to current subdivision
                            {
                                l0 ++;
                                V = V->vert;
                            }

                            dot = dot_productFF(V->aim_vec, polyAim.vec);

                            if (dot < Dot)
                            {
                                Dot = dot;
                            }
                        }

                        Q->B_light.deviation = acos(Dot) + C->Pixel_Size_In_Radians; // adding slight radius
                        Q->B_light.Aim.dist = polyAim.dist;
                        Q->B_light.Aim.vec[0] = polyAim.vec[0];
                        Q->B_light.Aim.vec[1] = polyAim.vec[1];
                        Q->B_light.Aim.vec[2] = polyAim.vec[2];

                        /* if deviation is less than View Radius in Radians divided by Resolution Radius
                        we have a one pixel size */

                        Q->B_light.backface = 1;

                        for (q0 = 0; q0 < 4; q0 ++)
                        {
                            idx = Q->quads[q0];

                            Q0 = &O->quads_[l + 1][idx / ARRAYSIZE][idx % ARRAYSIZE];

                            if (Q0->B_light.backface == 0)
                            {
                                Q->B_light.backface = 0;
                                break;
                            }
                        }
                    }
                    else
                    {
                        polyAim = vector3d(Q->B, C->T->pos);

                        Dot = 1.0;

                        for (v = 0; v < 4; v ++)
                        {
                            idx = Q->verts[v];

                            V = &O->verts_[l][idx / ARRAYSIZE][idx % ARRAYSIZE];

                            dot = dot_productFF(V->aim_vec, polyAim.vec);

                            if (dot < Dot)
                            {
                                Dot = dot;
                            }
                        }

                        Q->B_light.deviation = acos(Dot) + C->Pixel_Size_In_Radians; // adding slight radius
                        Q->B_light.Aim.dist = polyAim.dist;
                        Q->B_light.Aim.vec[0] = polyAim.vec[0];
                        Q->B_light.Aim.vec[1] = polyAim.vec[1];
                        Q->B_light.Aim.vec[2] = polyAim.vec[2];

                        Q->B_light.backface = 1;

                        for (t = 0; t < 2; t ++)
                        {
                            idx = Q->trips[t];

                            T = &O->trips_[l][idx / ARRAYSIZE][idx % ARRAYSIZE];

                            polyAim = vector3d(T->B, C->T->pos);

                            idx = T->verts[1];

                            V = &O->verts_[l][idx / ARRAYSIZE][idx % ARRAYSIZE];

                            polynormal.x = -T->N.Tx;
                            polynormal.y = -T->N.Ty;
                            polynormal.z = -T->N.Tz;

                            polypoints[0] = V->Tx;
                            polypoints[1] = V->Ty;
                            polypoints[2] = V->Tz;

                            plane = init_plane(polypoints, &polynormal);

                            T->B_light.Aim.dist = nearest(C->T->pos, plane); // polyAim.dist;

                            dot = dot_productN(&polynormal, polyAim.vec);

                            if (dot < BACKFACE_QUALIFIER) // reversed
                            {
                                T->B_light.backface = 0;
                                Q->B_light.backface = 0;
                            }
                            else
                            {
                                T->B_light.backface = 1;
                            }
                        }
                    }
                }
            }

            /* level -1 */

            for (p = 0; p < O->polycount; p ++)
            {
                P = &O->polys[p / ARRAYSIZE][p % ARRAYSIZE];

                idx = O->vertcount + O->edgecount + p;

                V = &O->verts_[0][idx / ARRAYSIZE][idx % ARRAYSIZE];

                l = 1;

                while (l < L) // delegates to current subdivision
                {
                    l ++;
                    V = V->vert;
                }

                polyAim = vector3d_T(V, C->T->pos);

                Dot = 1.0;

                for (v = 0; v < P->edgecount; v ++)
                {
                    idx = O->vertcount + P->edges[v];

                    V = &O->verts_[0][idx / ARRAYSIZE][idx % ARRAYSIZE];

                    l = 0;

                    while (l < L) // delegates to current subdivision
                    {
                        l ++;
                        V = V->vert;
                    }

                    dot = dot_productFF(V->aim_vec, polyAim.vec);

                    if (dot < Dot)
                    {
                        Dot = dot;
                    }

                    /////

                    idx = P->verts[v];

                    V = &O->verts[idx / ARRAYSIZE][idx % ARRAYSIZE];

                    l = -1;

                    while (l < L) // delegates to current subdivision
                    {
                        l ++;
                        V = V->vert;
                    }

                    dot = dot_productFF(V->aim_vec, polyAim.vec);

                    if (dot < Dot)
                    {
                        Dot = dot;
                    }
                }

                P->B_light.deviation = acos(Dot);
                P->B_light.Aim.dist = polyAim.dist + P->B_light.deviation;
                P->B_light.Aim.vec[0] = polyAim.vec[0];
                P->B_light.Aim.vec[1] = polyAim.vec[1];
                P->B_light.Aim.vec[2] = polyAim.vec[2];

                P->B_light.backface = 1;

                for (q = 0; q < P->edgecount; q ++)
                {
                    idx = P->quads[q];

                    Q = &O->quads_[0][idx / ARRAYSIZE][idx % ARRAYSIZE];

                    if (Q->B_light.backface == 0)
                    {
                        P->B_light.backface = 0;
                        break;
                    }
                }
            }
        }
    }
}

/*inline*/ int cull(float point[3], float p_points[3][3])
{
    float angle = 0;
    float dot;
    //int on_edge = 0;


    aim A, B;

    int i; // , C, C0;

    A = vector3dF(p_points[2], point);

    for (i = 0; i < 3; i ++)
    {
        B = vector3dF(p_points[i], point);
        dot = dot_productFF(A.vec, B.vec);
        A.vec[0] = B.vec[0];
        A.vec[1] = B.vec[1];
        A.vec[2] = B.vec[2];
        //if (dot < -0.98) on_edge = 1;
        angle += acos(dot);
    }

//    C = angle * TOLERANCE;
//    C0 = pi2 * TOLERANCE;

    if (angle >= pi2_TOLERANCE)
    {
        //if (on_edge) return 2; else return 1;
        return 1;
    }
    else
    {
        return 0;
    }
}

/*inline*/ void cross_product(float * N, float A[3], float B[3], float C[3])
{
    float v1[3] = {A[0] - B[0], A[1] - B[1], A[2] - B[2]};
    float v2[3] = {C[0] - B[0], C[1] - B[1], C[2] - B[2]};
    N[0] = v1[1] * v2[2] - v1[2] * v2[1];
    N[1] = v1[2] * v2[0] - v1[0] * v2[2];
    N[2] = v1[0] * v2[1] - v1[1] * v2[0];
}

uvPack uv_value_mean(float polytexts[4][2])
{
    float I_TextUV[2] = {0.0, 0.0};

    float M = 1.0 / 2.0;

    int i;

    for (i = 0; i < 2; i++)
    {
        I_TextUV[0] += polytexts[i][0] * M;
        I_TextUV[1] += polytexts[i][1] * M;
    }

    uvPack texel_UV = {{I_TextUV[0], I_TextUV[1]}};
    return texel_UV;
}

texelPack uv_value(float i_point[3], float polypoints[3][3], float polynormals[3][3], float polytexts[3][2])
{
    float L[3], N[3];

    float I_TextUV[2] = {0.0, 0.0};
    float I_Normal[3] = {0.0, 0.0, 0.0};

    float A, M;

    int i, n, s;
    A = 0.0;
    for (i = 0; i < 3; i++)
    {
        n = (i + 1) % 3;
        s = (i + 2) % 3;
        cross_product((float *)&N, polypoints[n], i_point, polypoints[s]);
        L[i] = length(N);
        A += L[i];
    }

    for (i = 0; i < 3; i++)
    {
        M = L[i] / A;
        I_Normal[0] += polynormals[i][0] * M;
        I_Normal[1] += polynormals[i][1] * M;
        I_Normal[2] += polynormals[i][2] * M;

        I_TextUV[0] += polytexts[i][0] * M;
        I_TextUV[1] += polytexts[i][1] * M;
    }

    // texts are flipped
    // vertex normals at intersection point are reversed

    texelPack texel_uvNormal = {{I_TextUV[0], I_TextUV[1]}, {-I_Normal[0], -I_Normal[1], -I_Normal[2]}};
    return texel_uvNormal;
}

void normal_value(float i_point[3], float polypoints[3][3], float polynormals[3][3], float * N0)
{
    float L[3], N[3];

    N0[0] = 0.0;
    N0[1] = 0.0;
    N0[2] = 0.0;

    float A, M;

    int i, n, s;
    A = 0.0;
    for (i = 0; i < 3; i++)
    {
        n = (i + 1) % 3;
        s = (i + 2) % 3;
        cross_product((float *)&N, polypoints[n], i_point, polypoints[s]);
        L[i] = length(N);
        A += L[i];
    }

    for (i = 0; i < 3; i++)
    {
        M = L[i] / A;
        N0[0] -= polynormals[i][0] * M;
        N0[1] -= polynormals[i][1] * M;
        N0[2] -= polynormals[i][2] * M;
    }
}

typedef struct
{
    int preak;
    int cancel;
    int volume_counter;
}
trianges_cancel;

trianges_cancel render_Pixel(pixel * P, camera * C, normal * D, int L, object * O, quadrant * Q, int volume_counter, float shading_normal[3])
{
    trianges_cancel Cancel;
    Cancel.preak = 0;
    Cancel.cancel = 0;

    Uint32 pix;
    Uint8 r, g, b, a;
    unsigned int R, G, B, A;
    float a0, b0, mean;

    int idx, x, y;
    float dot_light;
    uv * UV;

    float polytexts[2][2];

    uvPack Q_UV;
    float Q_Normal[3];
    SDL_Surface * texture;
    surface_Material Material;

    //P->R[volume_counter] = 255;

    idx = Q->surface;
    Material = Materials[idx];
    texture = Surf_Text[Material.texture];

    if (texture != NULL)//(Material.use_texture && texture != NULL)
    {
        idx = Q->texts[0];
        UV = &O->uvtex_[L][idx / ARRAYSIZE][idx % ARRAYSIZE];
        polytexts[0][0] = UV->u;
        polytexts[0][1] = UV->v;
        idx = Q->texts[2];
        UV = &O->uvtex_[L][idx / ARRAYSIZE][idx % ARRAYSIZE];
        polytexts[1][0] = UV->u;
        polytexts[1][1] = UV->v;

        Q_UV = uv_value_mean(polytexts);
        x = abs((int)(texture->w * Q_UV.uv[0])) % texture->w;
        y = abs((int)(texture->h * Q_UV.uv[1])) % texture->h;
        pix = get_pixel32(texture, x, y);
        SDL_GetRGBA(pix, texture->format, &r, &g, &b, &a);

        a0 = (float)a / (float)255;
        b0 = (float)Material.RGBA.A / (float)255;

        a0 *= 2;
        mean = a0 + b0;

        R = r * a0 + Material.RGBA.R * b0;
        G = g * a0 + Material.RGBA.G * b0;
        B = b * a0 + Material.RGBA.B * b0;
        A = a * a0 + Material.RGBA.A * b0;

        r = R / mean;
        g = G / mean;
        b = B / mean;
        a = A / mean;
    }
    else
    {
        r = Material.RGBA.R;
        g = Material.RGBA.G;
        b = Material.RGBA.B;
        a = Material.RGBA.A;
    }

    Q_Normal[0] = -Q->N.Tx;
    Q_Normal[1] = -Q->N.Ty;
    Q_Normal[2] = -Q->N.Tz;

    if (Material.smooth)
    {
        dot_light = dot_productFF(Q_Normal, light_vec);
        P->NX[volume_counter] = Q_Normal[0];
        P->NY[volume_counter] = Q_Normal[1];
        P->NZ[volume_counter] = Q_Normal[2];
    }
    else
    {
        dot_light = dot_productFF(shading_normal, light_vec);
        P->NX[volume_counter] = shading_normal[0];
        P->NY[volume_counter] = shading_normal[1];
        P->NZ[volume_counter] = shading_normal[2];
    }

    if (dot_light < 0)
        dot_light = -dot_light; // 0;

    P->D[volume_counter] = Q->B.Aim.dist;
    P->T[volume_counter] = NULL;
    P->Q[volume_counter] = Q;
    P->level[volume_counter] = L;
    P->O[volume_counter] = O;
    P->R[volume_counter] = r * dot_light;
    P->G[volume_counter] = g * dot_light;
    P->B[volume_counter] = b * dot_light;
    P->A[volume_counter] = a;
    P->X[volume_counter] = Q->B.Tx;
    P->Y[volume_counter] = Q->B.Ty;
    P->Z[volume_counter] = Q->B.Tz;
    volume_counter ++;

    Cancel.cancel = 1;

    if (volume_counter == PIXEL_VOLUME)
        Cancel.preak = 1;

    Cancel.volume_counter = volume_counter;
    return Cancel;
}

trianges_cancel render_Pixel_light(float Dist, camera * Light0, normal * D, int L, object * O,
            object * Pixel_Object, quadrant * Q, float * shadow, light_pixel * LP, int volume_counter)
{
    trianges_cancel Cancel;
    Cancel.preak = 0;
    Cancel.cancel = 0;

    Uint32 pix;
    Uint8 r, g, b, a;
    int idx, x, y;
    float A;

    float dist, Dot;

    uv * UV;

    float polytexts[2][2];

    uvPack Q_UV;
    float Q_Normal[3];
    SDL_Surface * texture;
    surface_Material Material;

    float shadow_intensity = 1.0;

    Q_Normal[0] = -Q->N.Tx;
    Q_Normal[1] = -Q->N.Ty;
    Q_Normal[2] = -Q->N.Tz;

    Dot = dot_productFF(Q_Normal, light_vec);

    dist = Q->B_light.Aim.dist;

    if (dist < Dist - SHADOW_DIST)
    {
        idx = Q->surface;
        Material = Materials[idx];
        texture = Surf_Text[Material.texture];

        if (texture != NULL)//(Material.use_texture && texture != NULL)
        {
            idx = Q->texts[0];
            UV = &O->uvtex_[L][idx / ARRAYSIZE][idx % ARRAYSIZE];
            polytexts[0][0] = UV->u;
            polytexts[0][1] = UV->v;
            idx = Q->texts[2];
            UV = &O->uvtex_[L][idx / ARRAYSIZE][idx % ARRAYSIZE];
            polytexts[1][0] = UV->u;
            polytexts[1][1] = UV->v;

            Q_UV = uv_value_mean(polytexts);
            x = abs((int)(texture->w * Q_UV.uv[0])) % texture->w;
            y = abs((int)(texture->h * Q_UV.uv[1])) % texture->h;
            pix = get_pixel32(texture, x, y);
            SDL_GetRGBA(pix, texture->format, &r, &g, &b, &a);
        }
        else
        {
            a = Material.RGBA.A;
        }

        if (Dot < 0.0)
        {
            Dot = - Dot;
        }

        shadow_intensity = dist / Dist;
        shadow_intensity = pow(shadow_intensity, 4.0);  // fade to distance

        if (O == Pixel_Object) // self shadow, no obsticle between light and incident
        {
            shadow_intensity *= Dot;
        }

        A = (float)a / 255.0;

        shadow[0] = SHADOW * shadow_intensity * A; // * dot

        LP->S[volume_counter] = shadow[0];
        // LP->A[volume_counter] = A;
        LP->D[volume_counter] = -Q->B_light.Aim.dist; // negate to reverse
        volume_counter ++;

        Cancel.cancel = 1;

        if (volume_counter == PIXEL_VOLUME)
            Cancel.preak = 1;
    }

    Cancel.volume_counter = volume_counter;
    return Cancel;
}

trianges_cancel render_Triangles_light(float Dist, camera * Light0, normal * D, object * O, object * Pixel_Object,
                                       triangle * T, float * shadow, light_pixel * LP, int volume_counter)
{
    trianges_cancel Cancel;
    Cancel.preak = 0;
    Cancel.cancel = 0;

    Uint32 pix;
    Uint8 r, g, b, a;

    float A;

    int idx, c, x, y;
    float dot, dist, Dot;
    normal polynormal;
    vertex * V;
    uv * UV;
    //polyplane plane;
    float polypoints[3][3];
    float polynormals[3][3];
    float polytexts[3][2];
    float intersection_Point[3];
    texelPack T_uvNormal;
    SDL_Surface * texture;
    surface_Material Material;

    float shadow_intensity = 1.0;

    polynormal.x = -T->N.Tx;
    polynormal.y = -T->N.Ty;
    polynormal.z = -T->N.Tz;

    dot = dot_product(&polynormal, D);

    //plane = init_plane(polypoints[1], &polynormal);
    dist = T->B_light.Aim.dist; //nearest(Light0->T->pos, plane);

    dist /= dot;

    if (dist < Dist - SHADOW_DIST)
    {
        idx = T->verts[0];
        V = &O->verts[idx / ARRAYSIZE][idx % ARRAYSIZE];
        polypoints[0][0] = V->Tx;
        polypoints[0][1] = V->Ty;
        polypoints[0][2] = V->Tz;
        polynormals[0][0] = V->N.Tx;
        polynormals[0][1] = V->N.Ty;
        polynormals[0][2] = V->N.Tz;
        idx = T->verts[1];
        V = &O->verts[idx / ARRAYSIZE][idx % ARRAYSIZE];
        polypoints[1][0] = V->Tx;
        polypoints[1][1] = V->Ty;
        polypoints[1][2] = V->Tz;
        polynormals[1][0] = V->N.Tx;
        polynormals[1][1] = V->N.Ty;
        polynormals[1][2] = V->N.Tz;
        idx = T->verts[2];
        V = &O->verts[idx / ARRAYSIZE][idx % ARRAYSIZE];
        polypoints[2][0] = V->Tx;
        polypoints[2][1] = V->Ty;
        polypoints[2][2] = V->Tz;
        polynormals[2][0] = V->N.Tx;
        polynormals[2][1] = V->N.Ty;
        polynormals[2][2] = V->N.Tz;

        intersection_Point[0] = Light0->T->pos[0] + D->x * dist;
        intersection_Point[1] = Light0->T->pos[1] + D->y * dist;
        intersection_Point[2] = Light0->T->pos[2] + D->z * dist;

        c = cull(intersection_Point, polypoints);

        if (c > 0)
        {
            idx = T->surface;
            Material = Materials[idx];
            texture = Surf_Text[Material.texture];

            if (texture != NULL)//(Material.use_texture && texture != NULL)
            {
                idx = T->texts[0];
                UV = &O->uvtex[idx / ARRAYSIZE][idx % ARRAYSIZE];
                polytexts[0][0] = UV->u;
                polytexts[0][1] = UV->v;
                idx = T->texts[1];
                UV = &O->uvtex[idx / ARRAYSIZE][idx % ARRAYSIZE];
                polytexts[1][0] = UV->u;
                polytexts[1][1] = UV->v;
                idx = T->texts[2];
                UV = &O->uvtex[idx / ARRAYSIZE][idx % ARRAYSIZE];
                polytexts[2][0] = UV->u;
                polytexts[2][1] = UV->v;

                T_uvNormal = uv_value(intersection_Point, polypoints, polynormals, polytexts);
                x = abs((int)(texture->w * T_uvNormal.uv[0])) % texture->w;
                y = abs((int)(texture->h * T_uvNormal.uv[1])) % texture->h;
                pix = get_pixel32(texture, x, y);
                SDL_GetRGBA(pix, texture->format, &r, &g, &b, &a);
            }
            else
            {
                a = Material.RGBA.A;
                normal_value(intersection_Point, polypoints, polynormals, T_uvNormal.normal);
            }

            Dot = dot_productF(T_uvNormal.normal, D);

            if (Dot < 0.0)
            {
                Dot = - Dot;
            }

            shadow_intensity = dist / Dist;
            shadow_intensity = pow(shadow_intensity, 4.0);  // fade to distance

            if (O == Pixel_Object) // self shadow, no obsticle between light and incident
            {
                shadow_intensity *= Dot;
            }

            A = (float)a / 255.0;

            shadow[0] = SHADOW * shadow_intensity * A; // * dot

            LP->S[volume_counter] = shadow[0];
            // LP->A[volume_counter] = A;
            LP->D[volume_counter] = -dist; // negate to reverse
            volume_counter ++;

            Cancel.cancel = 1;

            if (volume_counter == PIXEL_VOLUME)
                Cancel.preak = 1;
            //Cancel.preak = 1;
        }
        /*
        else
        {
            shadow[0] = SHADOW / 4; // poly radius
        }
        */
    }

    Cancel.volume_counter = volume_counter;
    return Cancel;
}

trianges_cancel render_Triangles_light_(float Dist, camera * Light0, normal * D, int L, object * O, object * Pixel_Object,
                                        triangle * T, float * shadow, light_pixel * LP, int volume_counter)
{
    trianges_cancel Cancel;
    Cancel.preak = 0;
    Cancel.cancel = 0;

    Uint32 pix;
    Uint8 r, g, b, a;

    float A;

    int idx, c, x, y;
    float dot, dist, Dot;
    normal polynormal;
    vertex * V;
    uv * UV;
    //polyplane plane;
    float polypoints[3][3];
    float polynormals[3][3];
    float polytexts[3][2];
    float intersection_Point[3];
    texelPack T_uvNormal;
    SDL_Surface * texture;
    surface_Material Material;

    float shadow_intensity = 1.0;

    polynormal.x = -T->N.Tx;
    polynormal.y = -T->N.Ty;
    polynormal.z = -T->N.Tz;

    dot = dot_product(&polynormal, D);

    //plane = init_plane(polypoints[1], &polynormal);
    dist = T->B_light.Aim.dist; // nearest(Light0->T->pos, plane);

    dist /= dot;

    if (dist < Dist - SHADOW_DIST) // + sign change
    {
        idx = T->verts[0];
        V = &O->verts_[L][idx / ARRAYSIZE][idx % ARRAYSIZE];
        polypoints[0][0] = V->Tx;
        polypoints[0][1] = V->Ty;
        polypoints[0][2] = V->Tz;
        polynormals[0][0] = V->N.Tx;
        polynormals[0][1] = V->N.Ty;
        polynormals[0][2] = V->N.Tz;
        idx = T->verts[1];
        V = &O->verts_[L][idx / ARRAYSIZE][idx % ARRAYSIZE];
        polypoints[1][0] = V->Tx;
        polypoints[1][1] = V->Ty;
        polypoints[1][2] = V->Tz;
        polynormals[1][0] = V->N.Tx;
        polynormals[1][1] = V->N.Ty;
        polynormals[1][2] = V->N.Tz;
        idx = T->verts[2];
        V = &O->verts_[L][idx / ARRAYSIZE][idx % ARRAYSIZE];
        polypoints[2][0] = V->Tx;
        polypoints[2][1] = V->Ty;
        polypoints[2][2] = V->Tz;
        polynormals[2][0] = V->N.Tx;
        polynormals[2][1] = V->N.Ty;
        polynormals[2][2] = V->N.Tz;

        intersection_Point[0] = Light0->T->pos[0] + D->x * dist;
        intersection_Point[1] = Light0->T->pos[1] + D->y * dist;
        intersection_Point[2] = Light0->T->pos[2] + D->z * dist;

        c = cull(intersection_Point, polypoints);

        if (c > 0)
        {
            idx = T->surface;
            Material = Materials[idx];
            texture = Surf_Text[Material.texture];

            if (texture != NULL)//(Material.use_texture && texture != NULL)
            {
                idx = T->texts[0];
                UV = &O->uvtex_[L][idx / ARRAYSIZE][idx % ARRAYSIZE];
                polytexts[0][0] = UV->u;
                polytexts[0][1] = UV->v;
                idx = T->texts[1];
                UV = &O->uvtex_[L][idx / ARRAYSIZE][idx % ARRAYSIZE];
                polytexts[1][0] = UV->u;
                polytexts[1][1] = UV->v;
                idx = T->texts[2];
                UV = &O->uvtex_[L][idx / ARRAYSIZE][idx % ARRAYSIZE];
                polytexts[2][0] = UV->u;
                polytexts[2][1] = UV->v;

                T_uvNormal = uv_value(intersection_Point, polypoints, polynormals, polytexts);
                x = abs((int)(texture->w * T_uvNormal.uv[0])) % texture->w;
                y = abs((int)(texture->h * T_uvNormal.uv[1])) % texture->h;
                pix = get_pixel32(texture, x, y);
                SDL_GetRGBA(pix, texture->format, &r, &g, &b, &a);
            }
            else
            {
                a = Material.RGBA.A;
                normal_value(intersection_Point, polypoints, polynormals, T_uvNormal.normal);
            }

            Dot = dot_productF(T_uvNormal.normal, D);

            if (Dot < 0.0)
            {
                Dot = -Dot;
            }

            //if (dot < 0.0)
            //{
            //    dot = - dot;
            //}

            shadow_intensity = dist / Dist;
            shadow_intensity = pow(shadow_intensity, 4.0); // fade to distance

            if (O == Pixel_Object) // self shadow
            {
                shadow_intensity *= Dot;
            }

            A = (float)a / 255.0;

            shadow[0] = SHADOW * shadow_intensity * A; // * dot;

            LP->S[volume_counter] = shadow[0];
            // LP->A[volume_counter] = A;
            LP->D[volume_counter] = -dist; // negate to reverse
            volume_counter ++;

            Cancel.cancel = 1;

            if (volume_counter == PIXEL_VOLUME)
                Cancel.preak = 1;
        }
        /*
        else
        {
            shadow[0] = SHADOW / 4; // poly radius
        }
        */
    }

    Cancel.volume_counter = volume_counter;
    return Cancel;
}

float cast_ray_from_Light0(float P_pos[3], camera * Light0, HexG ** G, int g_idx, int L, triangle * Pixel_Triangle, object * Pixel_Object)
{
    trianges_cancel Cancel;
    Cancel.preak = 0;
    Cancel.cancel = 0;

    float shadow = 0.0;
    float Shadow = 0.0;

    int t, p, h, q, q0, q1, q2;
    HexG * H;

    object * O;
    polygon * P0;
    triangle * T;
    quadrant * Q, * Q0, * Q1, * Q2;

    light_pixel LP;

    int i, k, s, d;
    int volume_index[PIXEL_VOLUME];
    float d_index[PIXEL_VOLUME];

    int volume_counter = 0;

    float aim_deviation;

    int Preak, idx;

    Preak = 0;
    polyPack * PP;

    direction_Pack D;

    D = length_AB(Light0->T->pos, P_pos);

    for (h = 0; h < g_idx; h ++)
    {
        if (Preak)
            break;

        H = G[h];

        for (p = 0; p < H->lightpacks; p ++) // (p = H->lightpacks - 1; p >= 0; p --) // distant first
        {
            if (Preak)
                break;

            PP = &H->Lights[p];

            O = PP->O;

            /*
            if (O == Pixel_Object) // self shadow
            {
                continue;
            }
            */

            idx = PP->idx;

            P0 = &O->polys[idx / ARRAYSIZE][idx % ARRAYSIZE];
            aim_deviation = acos(dot_productF(D.vec, (normal *)P0->B_light.Aim.vec));

            if (aim_deviation > P0->B_light.deviation) // P0->B.deviation)
            {
                continue;
            }

            if (P0->subdivs && O->subdlevel > -1)
            {
                for (q = 0; q < P0->edgecount; q ++)
                {
                    if (Preak)
                        break;

                    idx = P0->quads[q];

                    Q = &O->quads_[0][idx / ARRAYSIZE][idx % ARRAYSIZE];

                    if (Q->B_light.backface)
                    {
                        continue;
                    }

                    aim_deviation = acos(dot_productF(D.vec, (normal *)Q->B_light.Aim.vec));

                    if (aim_deviation > Q->B_light.deviation)
                    {
                        continue;
                    }
/*
                    if (Q->B_light.deviation < Light0->Pixel_Size_In_Radians)
                    {
                        Cancel = render_Pixel_light(D.distance, Light0, (normal *)D.vec, 0, O, Pixel_Object, Q, &shadow, &LP, volume_counter);
                        volume_counter = Cancel.volume_counter;
                        Preak = Cancel.preak;
                        continue;
                    }
*/
                    if (Q->subdivs && L >= 1)
                    {
                        for (q0 = 0; q0 < 4; q0 ++)
                        {
                            if (Preak)
                                break;

                            idx = Q->quads[q0];
                            Q0 = &O->quads_[1][idx / ARRAYSIZE][idx % ARRAYSIZE];

                            if (Q0->B_light.backface)
                            {
                                continue;
                            }

                            aim_deviation = acos(dot_productF(D.vec, (normal *)Q0->B_light.Aim.vec));

                            if (aim_deviation > Q0->B_light.deviation)
                            {
                                continue;
                            }
/*
                            if (Q0->B_light.deviation < Light0->Pixel_Size_In_Radians)
                            {
                                Cancel = render_Pixel_light(D.distance, Light0, (normal *)D.vec, 1, O, Pixel_Object, Q0, &shadow, &LP, volume_counter);
                                volume_counter = Cancel.volume_counter;
                                Preak = Cancel.preak;
                                continue;
                            }
*/
                            if (Q0->subdivs && L >= 2)
                            {
                                for (q1 = 0; q1 < 4; q1 ++)
                                {
                                    if (Preak)
                                        break;

                                    idx = Q0->quads[q1];
                                    Q1 = &O->quads_[2][idx / ARRAYSIZE][idx % ARRAYSIZE];

                                    if (Q1->B_light.backface)
                                    {
                                        continue;
                                    }

                                    aim_deviation = acos(dot_productF(D.vec, (normal *)Q1->B_light.Aim.vec));

                                    if (aim_deviation > Q1->B_light.deviation)
                                    {
                                        continue;
                                    }
/*
                                    if (Q1->B_light.deviation < Light0->Pixel_Size_In_Radians)
                                    {
                                        Cancel = render_Pixel_light(D.distance, Light0, (normal *)D.vec, 2, O, Pixel_Object, Q1, &shadow, &LP, volume_counter);
                                        volume_counter = Cancel.volume_counter;
                                        Preak = Cancel.preak;
                                        continue;
                                    }
*/
                                    if (Q1->subdivs && L >= 3)
                                    {
                                        for (q2 = 0; q2 < 4; q2 ++)
                                        {
                                            if (Preak)
                                                break;

                                            idx = Q1->quads[q2];
                                            Q2 = &O->quads_[3][idx / ARRAYSIZE][idx % ARRAYSIZE];

                                            if (Q2->B_light.backface)
                                            {
                                                continue;
                                            }

                                            aim_deviation = acos(dot_productF(D.vec, (normal *)Q2->B_light.Aim.vec));

                                            if (aim_deviation > Q2->B_light.deviation)
                                            {
                                                continue;
                                            }
/*
                                            if (Q2->B_light.deviation < Light0->Pixel_Size_In_Radians)
                                            {
                                                Cancel = render_Pixel_light(D.distance, Light0, (normal *)D.vec, 3, O, Pixel_Object, Q2, &shadow, &LP, volume_counter);
                                                volume_counter = Cancel.volume_counter;
                                                Preak = Cancel.preak;
                                                continue;
                                            }
*/
                                            for (t = 0; t < 2; t ++)
                                            {
                                                if (Preak)
                                                    break;

                                                idx = Q2->trips[t];

                                                T = &O->trips_[3][idx / ARRAYSIZE][idx % ARRAYSIZE];

                                                if (T == Pixel_Triangle)
                                                {
                                                    continue;
                                                }

                                                if (!T->B_light.backface)
                                                {
                                                    Cancel = render_Triangles_light_(D.distance, Light0, (normal *)D.vec, 3, O, Pixel_Object, T, &shadow, &LP, volume_counter);

                                                    volume_counter = Cancel.volume_counter;

                                                    Preak = Cancel.preak;

                                                    if (Cancel.cancel)
                                                        break;
                                                }
                                            }
                                        }
                                    }
                                    else
                                    {
                                        for (t = 0; t < 2; t ++)
                                        {
                                            if (Preak)
                                                break;

                                            idx = Q1->trips[t];

                                            T = &O->trips_[2][idx / ARRAYSIZE][idx % ARRAYSIZE];

                                            if (T == Pixel_Triangle)
                                            {
                                                continue;
                                            }

                                            if (!T->B_light.backface)
                                            {
                                                Cancel = render_Triangles_light_(D.distance, Light0, (normal *)D.vec, 2, O, Pixel_Object, T, &shadow, &LP, volume_counter);

                                                volume_counter = Cancel.volume_counter;

                                                Preak = Cancel.preak;

                                                if (Cancel.cancel)
                                                    break;
                                            }
                                        }
                                    }
                                }
                            }
                            else
                            {
                                for (t = 0; t < 2; t ++)
                                {
                                    if (Preak)
                                        break;

                                    idx = Q0->trips[t];

                                    T = &O->trips_[1][idx / ARRAYSIZE][idx % ARRAYSIZE];

                                    if (T == Pixel_Triangle)
                                    {
                                        continue;
                                    }

                                    if (!T->B_light.backface)
                                    {
                                        Cancel = render_Triangles_light_(D.distance, Light0, (normal *)D.vec, 1, O, Pixel_Object, T, &shadow, &LP, volume_counter);

                                        volume_counter = Cancel.volume_counter;

                                        Preak = Cancel.preak;

                                        if (Cancel.cancel)
                                            break;
                                    }
                                }
                            }
                        }
                    }
                    else
                    {
                        for (t = 0; t < 2; t ++)
                        {
                            if (Preak)
                                break;

                            idx = Q->trips[t];

                            T = &O->trips_[0][idx / ARRAYSIZE][idx % ARRAYSIZE];

                            if (T == Pixel_Triangle)
                            {
                                continue;
                            }

                            if (!T->B_light.backface)
                            {
                                Cancel = render_Triangles_light_(D.distance, Light0, (normal *)D.vec, 0, O, Pixel_Object, T, &shadow, &LP, volume_counter);

                                volume_counter = Cancel.volume_counter;

                                Preak = Cancel.preak;

                                if (Cancel.cancel)
                                    break;
                            }
                        }
                    }
                }
            }
            else
            {
                for (t = 0; t < P0->tripcount; t ++)
                {
                    if (Preak)
                        break;

                    idx = P0->trips[t];

                    T = &O->trips[idx / ARRAYSIZE][idx % ARRAYSIZE];

                    if (T == Pixel_Triangle)
                    {
                        continue;
                    }

                    if (!T->B_light.backface) // Light0 overrides it
                    {
                        Cancel = render_Triangles_light(D.distance, Light0, (normal *)D.vec, O, Pixel_Object, T, &shadow, &LP, volume_counter);

                        volume_counter = Cancel.volume_counter;

                        Preak = Cancel.preak;

                        if (Cancel.cancel)
                            break;
                    }
                }
            }
        }
    }

    if (volume_counter == 0)
    {
        return shadow;
    }

    for (i = 0; i < volume_counter; i ++)
    {
        volume_index[i] = i;
        d_index[i] = LP.D[i];
    }

    for (i = 1; i < volume_counter; i ++)
    {
        for (k = 0; k < volume_counter - i; k ++)
        {
            if (d_index[k] > d_index[k + 1]) // to reverse use other comparison
            {
                s = d_index[k + 1];
                d_index[k + 1] = d_index[k];
                d_index[k] = s;
                d = volume_index[k + 1];
                volume_index[k + 1] = volume_index[k];
                volume_index[k] = d;
            }
        }
    }

    /* combining shadows */

    for (k = volume_counter - 1; k >= 0; k --)
    {
        idx = volume_index[k];
        if (LP.S[idx] > Shadow)
        {
            Shadow = LP.S[idx];
        }
    }

    shadow = Shadow; // LP.S[volume_index[0]]; // 0 is closest light pixel

    return shadow;
}

trianges_cancel render_Triangles(pixel * P, camera * C, normal * D, object * O, triangle * T, int volume_counter, int t, float shading_normal[3])
{
    trianges_cancel Cancel;
    Cancel.preak = 0;
    Cancel.cancel = 0;

    Uint32 pix;
    Uint8 r, g, b, a;
    unsigned int R, G, B, A;
    float a0, b0, mean;

    int idx, c, x, y;
    float dot, dist, dot_light;
    normal polynormal;
    vertex * V;
    uv * UV;
//    polyplane plane;
    float polypoints[3][3];
    float polynormals[3][3];
    float polytexts[3][2];
    float intersection_Point[3];
    texelPack T_uvNormal;
    SDL_Surface * texture;
    surface_Material Material;

    polynormal.x = -T->N.Tx;
    polynormal.y = -T->N.Ty;
    polynormal.z = -T->N.Tz;

    dot = dot_product(&polynormal, D);

    idx = T->verts[0];
    V = &O->verts[idx / ARRAYSIZE][idx % ARRAYSIZE];
    polypoints[0][0] = V->Tx;
    polypoints[0][1] = V->Ty;
    polypoints[0][2] = V->Tz;
    polynormals[0][0] = V->N.Tx;
    polynormals[0][1] = V->N.Ty;
    polynormals[0][2] = V->N.Tz;
    idx = T->verts[1];
    V = &O->verts[idx / ARRAYSIZE][idx % ARRAYSIZE];
    polypoints[1][0] = V->Tx;
    polypoints[1][1] = V->Ty;
    polypoints[1][2] = V->Tz;
    polynormals[1][0] = V->N.Tx;
    polynormals[1][1] = V->N.Ty;
    polynormals[1][2] = V->N.Tz;
    idx = T->verts[2];
    V = &O->verts[idx / ARRAYSIZE][idx % ARRAYSIZE];
    polypoints[2][0] = V->Tx;
    polypoints[2][1] = V->Ty;
    polypoints[2][2] = V->Tz;
    polynormals[2][0] = V->N.Tx;
    polynormals[2][1] = V->N.Ty;
    polynormals[2][2] = V->N.Tz;

    //P->R[volume_counter] = 255;

    // plane = init_plane(polypoints[1], &polynormal);
    dist = T->B.Aim.dist; // nearest(C->T->pos, plane);

    dist /= dot;

    intersection_Point[0] = C->T->pos[0] + D->x * dist;
    intersection_Point[1] = C->T->pos[1] + D->y * dist;
    intersection_Point[2] = C->T->pos[2] + D->z * dist;

    c = cull(intersection_Point, polypoints);

//                printf("c %d\n", c);

    if (c > 0)
    {
        idx = T->surface;
        Material = Materials[idx];
        texture = Surf_Text[Material.texture];

        if (texture != NULL)//(Material.use_texture && texture != NULL)
        {
            idx = T->texts[0];
            UV = &O->uvtex[idx / ARRAYSIZE][idx % ARRAYSIZE];
            polytexts[0][0] = UV->u;
            polytexts[0][1] = UV->v;
            idx = T->texts[1];
            UV = &O->uvtex[idx / ARRAYSIZE][idx % ARRAYSIZE];
            polytexts[1][0] = UV->u;
            polytexts[1][1] = UV->v;
            idx = T->texts[2];
            UV = &O->uvtex[idx / ARRAYSIZE][idx % ARRAYSIZE];
            polytexts[2][0] = UV->u;
            polytexts[2][1] = UV->v;

            T_uvNormal = uv_value(intersection_Point, polypoints, polynormals, polytexts);
            x = abs((int)(texture->w * T_uvNormal.uv[0])) % texture->w;
            y = abs((int)(texture->h * T_uvNormal.uv[1])) % texture->h;
            pix = get_pixel32(texture, x, y);
            SDL_GetRGBA(pix, texture->format, &r, &g, &b, &a);

            a0 = (float)a / (float)255;
            b0 = (float)Material.RGBA.A / (float)255;

            a0 *= 2;
            mean = a0 + b0;

            R = r * a0 + Material.RGBA.R * b0;
            G = g * a0 + Material.RGBA.G * b0;
            B = b * a0 + Material.RGBA.B * b0;
            A = a * a0 + Material.RGBA.A * b0;

            r = R / mean;
            g = G / mean;
            b = B / mean;
            a = A / mean;
        }
        else
        {
            r = Material.RGBA.R;
            g = Material.RGBA.G;
            b = Material.RGBA.B;
            a = Material.RGBA.A;
            normal_value(intersection_Point, polypoints, polynormals, T_uvNormal.normal);
        }
        if (Material.smooth)
        {
            dot_light = dot_productFF(T_uvNormal.normal, light_vec);
            P->NX[volume_counter] = T_uvNormal.normal[0];
            P->NY[volume_counter] = T_uvNormal.normal[1];
            P->NZ[volume_counter] = T_uvNormal.normal[2];
        }
        else
        {
            dot_light = dot_productFF(shading_normal, light_vec);
            P->NX[volume_counter] = shading_normal[0];
            P->NY[volume_counter] = shading_normal[1];
            P->NZ[volume_counter] = shading_normal[2];
        }

        if (dot_light < 0)
            dot_light = -dot_light; // 0;

        P->D[volume_counter] = dist;
        P->T[volume_counter] = T;
        P->Q[volume_counter] = NULL;
        P->level[volume_counter] = -1;
        P->O[volume_counter] = O;
        P->R[volume_counter] = r * dot_light;
        P->G[volume_counter] = g * dot_light;
        P->B[volume_counter] = b * dot_light;
        P->A[volume_counter] = a;
        P->X[volume_counter] = intersection_Point[0];
        P->Y[volume_counter] = intersection_Point[1];
        P->Z[volume_counter] = intersection_Point[2];
        volume_counter ++;

        Cancel.cancel = 1;

        if (volume_counter == PIXEL_VOLUME)
            Cancel.preak = 1;

    }

    Cancel.volume_counter = volume_counter;
    return Cancel;
}

trianges_cancel render_Triangles_(pixel * P, camera * C, normal * D, int L, object * O, triangle * T, int volume_counter, int t, float shading_normal[3])
{
    trianges_cancel Cancel;
    Cancel.preak = 0;
    Cancel.cancel = 0;

    Uint32 pix;
    Uint8 r, g, b, a;
    unsigned int R, G, B, A;
    float a0, b0, mean;

    int idx, c, x, y;
    float dot, dist, dot_light;
    normal polynormal;
    vertex * V;
    uv * UV;
//    polyplane plane;
    float polypoints[3][3];
    float polynormals[3][3];
    float polytexts[3][2];
    float intersection_Point[3];
    texelPack T_uvNormal;
    SDL_Surface * texture;
    surface_Material Material;

    polynormal.x = -T->N.Tx;
    polynormal.y = -T->N.Ty;
    polynormal.z = -T->N.Tz;

    dot = dot_product(&polynormal, D);

    idx = T->verts[0];
    V = &O->verts_[L][idx / ARRAYSIZE][idx % ARRAYSIZE];
    polypoints[0][0] = V->Tx;
    polypoints[0][1] = V->Ty;
    polypoints[0][2] = V->Tz;
    polynormals[0][0] = V->N.Tx;
    polynormals[0][1] = V->N.Ty;
    polynormals[0][2] = V->N.Tz;
    idx = T->verts[1];
    V = &O->verts_[L][idx / ARRAYSIZE][idx % ARRAYSIZE];
    polypoints[1][0] = V->Tx;
    polypoints[1][1] = V->Ty;
    polypoints[1][2] = V->Tz;
    polynormals[1][0] = V->N.Tx;
    polynormals[1][1] = V->N.Ty;
    polynormals[1][2] = V->N.Tz;
    idx = T->verts[2];
    V = &O->verts_[L][idx / ARRAYSIZE][idx % ARRAYSIZE];
    polypoints[2][0] = V->Tx;
    polypoints[2][1] = V->Ty;
    polypoints[2][2] = V->Tz;
    polynormals[2][0] = V->N.Tx;
    polynormals[2][1] = V->N.Ty;
    polynormals[2][2] = V->N.Tz;

    //P->R[volume_counter] = 255;

    //plane = init_plane(polypoints[1], &polynormal);
    dist = T->B.Aim.dist; // nearest(C->T->pos, plane);

    dist /= dot;

    intersection_Point[0] = C->T->pos[0] + D->x * dist;
    intersection_Point[1] = C->T->pos[1] + D->y * dist;
    intersection_Point[2] = C->T->pos[2] + D->z * dist;

    c = cull(intersection_Point, polypoints);

//                printf("c %d\n", c);

    if (c > 0)
    {
        idx = T->surface;
        Material = Materials[idx];
        texture = Surf_Text[Material.texture];

        if (texture != NULL)//(Material.use_texture && texture != NULL)
        {
            idx = T->texts[0];
            UV = &O->uvtex_[L][idx / ARRAYSIZE][idx % ARRAYSIZE];
            polytexts[0][0] = UV->u;
            polytexts[0][1] = UV->v;
            idx = T->texts[1];
            UV = &O->uvtex_[L][idx / ARRAYSIZE][idx % ARRAYSIZE];
            polytexts[1][0] = UV->u;
            polytexts[1][1] = UV->v;
            idx = T->texts[2];
            UV = &O->uvtex_[L][idx / ARRAYSIZE][idx % ARRAYSIZE];
            polytexts[2][0] = UV->u;
            polytexts[2][1] = UV->v;

            T_uvNormal = uv_value(intersection_Point, polypoints, polynormals, polytexts);
            x = abs((int)(texture->w * T_uvNormal.uv[0])) % texture->w;
            y = abs((int)(texture->h * T_uvNormal.uv[1])) % texture->h;
            pix = get_pixel32(texture, x, y);
            SDL_GetRGBA(pix, texture->format, &r, &g, &b, &a);

            a0 = (float)a / (float)255;
            b0 = (float)Material.RGBA.A / (float)255;

            a0 *= 2;
            mean = a0 + b0;

            R = r * a0 + Material.RGBA.R * b0;
            G = g * a0 + Material.RGBA.G * b0;
            B = b * a0 + Material.RGBA.B * b0;
            A = a * a0 + Material.RGBA.A * b0;

            r = R / mean;
            g = G / mean;
            b = B / mean;
            a = A / mean;
        }
        else
        {
            r = Material.RGBA.R;
            g = Material.RGBA.G;
            b = Material.RGBA.B;
            a = Material.RGBA.A;
            normal_value(intersection_Point, polypoints, polynormals, T_uvNormal.normal);
        }
        if (Material.smooth)
        {
            dot_light = dot_productFF(T_uvNormal.normal, light_vec);
            P->NX[volume_counter] = T_uvNormal.normal[0];
            P->NY[volume_counter] = T_uvNormal.normal[1];
            P->NZ[volume_counter] = T_uvNormal.normal[2];
        }
        else
        {
            dot_light = dot_productFF(shading_normal, light_vec);
            P->NX[volume_counter] = shading_normal[0];
            P->NY[volume_counter] = shading_normal[1];
            P->NZ[volume_counter] = shading_normal[2];
        }

        if (dot_light < 0)
            dot_light = -dot_light; // 0;

        P->D[volume_counter] = dist;
        P->T[volume_counter] = T;
        P->Q[volume_counter] = NULL;
        P->level[volume_counter] = L;
        P->O[volume_counter] = O;
        P->R[volume_counter] = r * dot_light;
        P->G[volume_counter] = g * dot_light;
        P->B[volume_counter] = b * dot_light;
        P->A[volume_counter] = a;
        P->X[volume_counter] = intersection_Point[0];
        P->Y[volume_counter] = intersection_Point[1];
        P->Z[volume_counter] = intersection_Point[2];
        volume_counter ++;

        Cancel.cancel = 1;

        if (volume_counter == PIXEL_VOLUME)
            Cancel.preak = 1;

    }

    Cancel.volume_counter = volume_counter;
    return Cancel;
}

RGBA come_With_Pixel_(pixel * P, camera * C, normal * D, int L, HexG ** G, int g_idx)
{
    trianges_cancel Cancel;

    int i, idx, k, q, q0, q1, q2; // o
    //edge * E;
    polygon * P0;
    quadrant * Q, * Q0, * Q1, * Q2;
    triangle * T;
    object * O;

    float aim_deviation;

    int volume_index[PIXEL_VOLUME];
    float d_index[PIXEL_VOLUME];

    int volume_counter = 0;

    P->R[volume_counter] = 100;
    P->G[volume_counter] = 100;
    P->B[volume_counter] = 100;
    P->A[volume_counter] = 100;

    RGBA rgba;
    float a, b;

    rgba.R = 100;
    rgba.G = 100;
    rgba.B = 100;
    rgba.A = 0;

    int d;
    float s;

    int p, t, Preak;

    Preak = 0;

    polyPack * PP;

    float shading_normal[3];

    int h;
    HexG * H;

    if (L > 3) L = 3;

    for (h = 0; h < g_idx; h ++)
    {
        if (Preak)
            break;

        H = G[h];

        for (p = 0; p < H->polypacks; p ++)
        {
            if (Preak)
                break;

            PP = &H->Polygons[p];

            O = PP->O;

            idx = PP->idx;

            P0 = &O->polys[idx / ARRAYSIZE][idx % ARRAYSIZE];

            aim_deviation = acos(dot_productN(D, P0->B.Aim.vec)); // P0->B.Aim.vec));

            if (aim_deviation > P0->B.deviation) // P0->B.deviation)
            {
                continue;
            }

            if (P0->subdivs && O->subdlevel > -1)
            {
                shading_normal[0] = -P0->N.Tx;
                shading_normal[1] = -P0->N.Ty;
                shading_normal[2] = -P0->N.Tz;

                for (q = 0; q < P0->edgecount; q ++)
                {
                    if (Preak)
                        break;

                    idx = P0->quads[q];

                    Q = &O->quads_[0][idx / ARRAYSIZE][idx % ARRAYSIZE];

                    if (Q->B.backface)
                    {
                        continue;
                    }

                    aim_deviation = acos(dot_productN(D, Q->B.Aim.vec));

                    if (aim_deviation > Q->B.deviation)
                    {
                        continue;
                    }

                    if (Q->B.deviation < C->Pixel_Size_In_Radians) /* Size is one pixel */
                    {
                        Cancel = render_Pixel(P, C, D, 0, O, Q, volume_counter, shading_normal);
                        volume_counter = Cancel.volume_counter;
                        Preak = Cancel.preak;
                        continue;
                    }

                    if (Q->subdivs && L >= 1)
                    {
                        for (q0 = 0; q0 < 4; q0 ++)
                        {
                            if (Preak)
                                break;

                            idx = Q->quads[q0];
                            Q0 = &O->quads_[1][idx / ARRAYSIZE][idx % ARRAYSIZE];

                            if (Q0->B.backface)
                            {
                                continue;
                            }

                            aim_deviation = acos(dot_productN(D, Q0->B.Aim.vec));

                            if (aim_deviation > Q0->B.deviation)
                            {
                                continue;
                            }

                            if (Q0->B.deviation < C->Pixel_Size_In_Radians) /* Size is one pixel */
                            {
                                Cancel = render_Pixel(P, C, D, 1, O, Q0, volume_counter, shading_normal);
                                volume_counter = Cancel.volume_counter;
                                Preak = Cancel.preak;
                                continue;
                            }

                            if (Q0->subdivs && L >= 2)
                            {
                                for (q1 = 0; q1 < 4; q1 ++)
                                {
                                    if (Preak)
                                        break;

                                    idx = Q0->quads[q1];
                                    Q1 = &O->quads_[2][idx / ARRAYSIZE][idx % ARRAYSIZE];

                                    if (Q1->B.backface)
                                    {
                                        continue;
                                    }

                                    aim_deviation = acos(dot_productN(D, Q1->B.Aim.vec));

                                    if (aim_deviation > Q1->B.deviation)
                                    {
                                        continue;
                                    }

                                    if (Q1->B.deviation < C->Pixel_Size_In_Radians) /* Size is one pixel */
                                    {
                                        Cancel = render_Pixel(P, C, D, 2, O, Q1, volume_counter, shading_normal);
                                        volume_counter = Cancel.volume_counter;
                                        Preak = Cancel.preak;
                                        continue;
                                    }

                                    if (Q1->subdivs && L >= 3)
                                    {
                                        for (q2 = 0; q2 < 4; q2 ++)
                                        {
                                            if (Preak)
                                                break;

                                            idx = Q1->quads[q2];
                                            Q2 = &O->quads_[3][idx / ARRAYSIZE][idx % ARRAYSIZE];

                                            if (Q2->B.backface)
                                            {
                                                continue;
                                            }

                                            aim_deviation = acos(dot_productN(D, Q2->B.Aim.vec));

                                            if (aim_deviation > Q2->B.deviation)
                                            {
                                                continue;
                                            }

                                            if (Q2->B.deviation < C->Pixel_Size_In_Radians) /* Size is one pixel */
                                            {
                                                Cancel = render_Pixel(P, C, D, 3, O, Q2, volume_counter, shading_normal);
                                                volume_counter = Cancel.volume_counter;
                                                Preak = Cancel.preak;
                                                continue;
                                            }

                                            for (t = 0; t < 2; t ++)
                                            {
                                                if (Preak)
                                                    break;

                                                idx = Q2->trips[t];

                                                T = &O->trips_[3][idx / ARRAYSIZE][idx % ARRAYSIZE];

                                                if (!T->B.backface)
                                                {
                                                    Cancel = render_Triangles_(P, C, D, 3, O, T, volume_counter, t, shading_normal);

                                                    volume_counter = Cancel.volume_counter;

                                                    Preak = Cancel.preak;

                                                    if (Cancel.cancel)
                                                        break;
                                                }
                                            }
                                        }
                                    }
                                    else
                                    {
                                        for (t = 0; t < 2; t ++)
                                        {
                                            if (Preak)
                                                break;

                                            idx = Q1->trips[t];

                                            T = &O->trips_[2][idx / ARRAYSIZE][idx % ARRAYSIZE];

                                            if (!T->B.backface)
                                            {
                                                Cancel = render_Triangles_(P, C, D, 2, O, T, volume_counter, t, shading_normal);

                                                volume_counter = Cancel.volume_counter;

                                                Preak = Cancel.preak;

                                                if (Cancel.cancel)
                                                    break;
                                            }
                                        }
                                    }
                                }
                            }
                            else
                            {
                                for (t = 0; t < 2; t ++)
                                {
                                    if (Preak)
                                        break;

                                    idx = Q0->trips[t];

                                    T = &O->trips_[1][idx / ARRAYSIZE][idx % ARRAYSIZE];

                                    if (!T->B.backface)
                                    {
                                        Cancel = render_Triangles_(P, C, D, 1, O, T, volume_counter, t, shading_normal);

                                        volume_counter = Cancel.volume_counter;

                                        Preak = Cancel.preak;

                                        if (Cancel.cancel)
                                            break;
                                    }
                                }
                            }
                        }
                    }
                    else
                    {
                        for (t = 0; t < 2; t ++)
                        {
                            if (Preak)
                                break;

                            idx = Q->trips[t];

                            T = &O->trips_[0][idx / ARRAYSIZE][idx % ARRAYSIZE];

                            if (!T->B.backface)
                            {
                                Cancel = render_Triangles_(P, C, D, 0, O, T, volume_counter, t, shading_normal);

                                volume_counter = Cancel.volume_counter;

                                Preak = Cancel.preak;

                                if (Cancel.cancel)
                                    break;
                            }
                        }
                    }
                }
            }
            else
            {
                shading_normal[0] = -P0->N.Tx;
                shading_normal[1] = -P0->N.Ty;
                shading_normal[2] = -P0->N.Tz;

                for (t = 0; t < P0->tripcount; t ++)
                {
                    if (Preak)
                        break;

                    idx = P0->trips[t];

                    T = &O->trips[idx / ARRAYSIZE][idx % ARRAYSIZE];

                    if (!T->B.backface) // Light0 overrides it
                    {
                        Cancel = render_Triangles(P, C, D, O, T, volume_counter, t, shading_normal);

                        volume_counter = Cancel.volume_counter;

                        Preak = Cancel.preak;

                        if (Cancel.cancel)
                            break;
                    }
                }
            }
        }
    }
    if (volume_counter == 0)
    {
        return rgba;
    }

    for (i = 0; i < volume_counter; i ++)
    {
        volume_index[i] = i;
        d_index[i] = P->D[i];
    }

    for (i = 1; i < volume_counter; i ++)
    {
        for (k = 0; k < volume_counter - i; k ++)
        {
            if (d_index[k] > d_index[k + 1]) // to reverse use other comparison
            {
                s = d_index[k + 1];
                d_index[k + 1] = d_index[k];
                d_index[k] = s;
                d = volume_index[k + 1];
                volume_index[k + 1] = volume_index[k];
                volume_index[k] = d;
            }
        }
    }

    // shadow light

    if (RENDER_LIGHT0)
    {
        union Dir D_0 = {{0.0, 0.0, -1.0}};
        float shadow = SHADOW;
        HexG * G0[HEXAS];
        int g_idx_0;
        HexPack HP;
        g_idx_0 = 0;

        object * Pixel_Object = NULL;
        triangle * Pixel_Triangle = NULL;

        /* scan transparent pixel closer */

        for (k = 0; k < volume_counter; k ++)
        {
            idx = volume_index[k];

            //idx = volume_index[0];

            if (P->O[idx] != NULL)
            {
                Pixel_Object = P->O[idx];
            }

            if (P->T[idx] != NULL)
            {
                Pixel_Triangle = P->T[idx];
            }

            float P_pos[3];

            P_pos[0] = P->X[idx];
            P_pos[1] = P->Y[idx];
            P_pos[2] = P->Z[idx];

            direction_Pack DP;

            DP = length_AB(Light0->T->pos, P_pos);

            D_0.D.x = DP.vec[0];
            D_0.D.y = DP.vec[1];
            D_0.D.z = DP.vec[2];

            aim_deviation = acos(dot_productN((normal *)DP.vec, Light0->T->aim));

            if (aim_deviation < light_cone)
            {
                HP = collect_Hexa_Groups_light(D_0.D, G0, g_idx_0);
                shadow = cast_ray_from_Light0(P_pos, Light0, HP.G, HP.g_idx, L, Pixel_Triangle, Pixel_Object); // index 0
            }

            P->R[idx] -= shadow;
            P->G[idx] -= shadow;
            P->B[idx] -= shadow;

            if (P->R[idx] < 0) P->R[idx] = 0;
            if (P->G[idx] < 0) P->G[idx] = 0;
            if (P->B[idx] < 0) P->B[idx] = 0;

            if (P->A[idx] == 255)
            {
                break;
            }
        }
    }

    //composite_Pixels(P, volume_counter);

    for (k = volume_counter - 1; k >= 0; k --)
    {
        idx = volume_index[k];

        a = P->A[idx] / 255.0;
        b = 1 - a;

        rgba.R = rgba.R * b + P->R[idx] * a;
        rgba.G = rgba.G * b + P->G[idx] * a;
        rgba.B = rgba.B * b + P->B[idx] * a;
        rgba.A = rgba.A * b + P->A[idx] * a;
    }

    //rgba.A /= volume_counter;

    return rgba;
}

void project_Selected_Locators(camera * C, object * O, int * selected_transformers, int selected_transformer_count)
{
    printf("Object %s %d\n", O->Name, O->polycount);
    printf("Camera %s %d\n", C->Name, C->ortho);

    float dot;
    float angle;
    float i_point[3];
    direction_Pack D, D0;

    int s, t, p, p_count;

    polygon * P;
    transformer * T;

    if (C->ortho)
    {
        printf("ortho");

        direction T_Dir, P_Dir;

        float rotVec_I[3][3];
        float x, y, z;
        float X, Y, Z;
        float dist, a, b;

        invert_Rotation(C->T, rotVec_I);

        for (s = 0; s < selected_transformer_count; s ++)
        {
            t = selected_transformers[s];
            T = transformers[t];

            if (T->Bone == NULL)
            {
                continue;
            }
            else if (T == T->Bone->A)
            {
                continue;
            }

            x = T->pos[0] - C->T->pos[0];
            y = T->pos[1] - C->T->pos[1];
            z = T->pos[2] - C->T->pos[2];

            rotate_Vertex_I(rotVec_I, x, y, z, &T_Dir);

            p_count = 0;
            i_point[0] = T_Dir.x;
            i_point[1] = T_Dir.y;
            i_point[2] = 0.0;

            for (p = 0; p < O->polycount; p ++)
            {
                P = &O->polys[p / ARRAYSIZE][p % ARRAYSIZE];

                X = P->B.Tx - C->T->pos[0];
                Y = P->B.Ty - C->T->pos[1];
                Z = P->B.Tz - C->T->pos[2];

                rotate_Vertex_I(rotVec_I, X, Y, Z, &P_Dir);

                if (P_Dir.z > 0)
                {
                    a = T_Dir.x - P_Dir.x;
                    b = T_Dir.y - P_Dir.y;
                    dist = sqrt(a * a + b * b);

                    if (dist < P->B.Tradius)
                    {
                        i_point[2] += P_Dir.z;

                        p_count ++;
                    }
                }
            }

            if (p_count > 0)
            {
                i_point[2] /= p_count;

                rotate_Vertex_I(C->T->rotVec_, i_point[0], i_point[1], i_point[2], &T_Dir);

                T->pos[0] = C->T->pos[0] + T_Dir.x;
                T->pos[1] = C->T->pos[1] + T_Dir.y;
                T->pos[2] = C->T->pos[2] + T_Dir.z;
            }
        }
    }
    else
    {
        for (s = 0; s < selected_transformer_count; s ++)
        {
            t = selected_transformers[s];
            T = transformers[t];

            if (T->Bone == NULL)
            {
                continue;
            }
            else if (T == T->Bone->A)
            {
                continue;
            }

            D = length_AB_(C->T->pos[0], C->T->pos[1], C->T->pos[2], T->pos[0], T->pos[1], T->pos[2]);

            p_count = 0;
            i_point[0] = 0.0;
            i_point[1] = 0.0;
            i_point[2] = 0.0;

            for (p = 0; p < O->polycount; p ++)
            {
                P = &O->polys[p / ARRAYSIZE][p % ARRAYSIZE];

                D0 = length_AB_(C->T->pos[0], C->T->pos[1], C->T->pos[2], P->B.Tx, P->B.Ty, P->B.Tz);

                if (D0.distance > 0)
                {
                    angle = atan2(P->B.Tradius, D0.distance);

                    dot = dot_productFF(D0.vec, D.vec);

                    if (acos(dot) < angle)
                    {
                        i_point[0] += (D.vec[0] * D0.distance);
                        i_point[1] += (D.vec[1] * D0.distance);
                        i_point[2] += (D.vec[2] * D0.distance);

                        p_count ++;
                    }
                }
            }

            if (p_count > 0)
            {
                i_point[0] /= p_count;
                i_point[1] /= p_count;
                i_point[2] /= p_count;

                T->pos[0] = C->T->pos[0] + i_point[0];
                T->pos[1] = C->T->pos[1] + i_point[1];
                T->pos[2] = C->T->pos[2] + i_point[2];
            }
        }
    }

    for (s = 0; s < selected_transformer_count; s ++)
    {
        t = selected_transformers[s];
        T = transformers[t];

        if (T->Bone == NULL)
        {
            continue;
        }
        else if (T == T->Bone->A)
        {
            T->pos[0] = T->parent->pos[0];
            T->pos[1] = T->parent->pos[1];
            T->pos[2] = T->parent->pos[2];
        }
    }

    bone * B;

    for (s = 0; s < selected_transformer_count; s ++)
    {
        t = selected_transformers[s];
        T = transformers[t];

        if (T->Bone == NULL || T->Bone == B)
        {
            continue;
        }

        synthesize_Bone_Alignement(T->Bone);

        B = T->Bone;
    }
}

RGBA come_With_Pixel(pixel * P, camera * C, normal * D, HexG ** G, int g_idx)
{
    trianges_cancel Cancel;
    int i, idx, k; // o;

    //edge * E;
    polygon *P0;
    triangle * T;
    object * O;

    float aim_deviation;
    int volume_index[PIXEL_VOLUME];
    float d_index[PIXEL_VOLUME];

    int volume_counter = 0;

    P->R[volume_counter] = 100;
    P->G[volume_counter] = 100;
    P->B[volume_counter] = 100;
    P->A[volume_counter] = 100;

    RGBA rgba;
    float a, b;

    rgba.R = 100;
    rgba.G = 100;
    rgba.B = 100;
    rgba.A = 0;

    int d;
    float s;

    int p, t, Preak;

    Preak = 0;
    polyPack * PP;

    float shading_normal[3];

    int h;
    HexG * H;

    for (h = 0; h < g_idx; h ++)
    {
        if (Preak)
            break;

        H = G[h];

        for (p = 0; p < H->polypacks; p ++)
        {
            if (Preak)
                break;

            PP = &H->Polygons[p];

            O = PP->O; //printf("%s", O->Name);

            idx = PP->idx;

            P0 = &O->polys[idx / ARRAYSIZE][idx % ARRAYSIZE];

            aim_deviation = acos(dot_productN(D, P0->B.Aim.vec)); // P0->B.Aim.vec));

            if (aim_deviation > P0->B.deviation) // P0->B.deviation)
            {
                continue;
            }

            shading_normal[0] = -P0->N.Tx;
            shading_normal[1] = -P0->N.Ty;
            shading_normal[2] = -P0->N.Tz;

            for (t = 0; t < P0->tripcount; t ++)
            {
                if (Preak)
                    break;

                idx = P0->trips[t];

                T = &O->trips[idx / ARRAYSIZE][idx % ARRAYSIZE];

                if (!T->B.backface) // Light0 overrides it
                {
                        Cancel = render_Triangles(P, C, D, O, T, volume_counter, t, shading_normal);

                        volume_counter = Cancel.volume_counter;

                        Preak = Cancel.preak;

                        if (Cancel.cancel)
                            break;
                }
            }
        }
    }
    if (volume_counter == 0)
    {
        return rgba;
    }

    for (i = 0; i < volume_counter; i ++)
    {
        volume_index[i] = i;
        d_index[i] = P->D[i];
    }

    for (i = 1; i < volume_counter; i ++)
    {
        for (k = 0; k < volume_counter - i; k ++)
        {
            if (d_index[k] > d_index[k + 1]) // to reverse use other comparison
            {
                s = d_index[k + 1];
                d_index[k + 1] = d_index[k];
                d_index[k] = s;
                d = volume_index[k + 1];
                volume_index[k + 1] = volume_index[k];
                volume_index[k] = d;
            }
        }
    }

    // shadow light

    if (RENDER_LIGHT0)
    {
        union Dir D_0 = {{0.0, 0.0, -1.0}};
        float shadow = SHADOW;
        HexG * G0[HEXAS];
        int g_idx_0;
        HexPack HP;
        g_idx_0 = 0;

        object * Pixel_Object = NULL;
        triangle * Pixel_Triangle = NULL;

        /* scan transparent pixel closer */

        for (k = 0; k < volume_counter; k ++)
        {
            idx = volume_index[k];

            //idx = volume_index[0];

            if (P->O[idx] != NULL)
            {
                Pixel_Object = P->O[idx];
            }

            if (P->T[idx] != NULL)
            {
                Pixel_Triangle = P->T[idx];
            }

            float P_pos[3];

            P_pos[0] = P->X[idx];
            P_pos[1] = P->Y[idx];
            P_pos[2] = P->Z[idx];

            direction_Pack DP;

            DP = length_AB(Light0->T->pos, P_pos);

            D_0.D.x = DP.vec[0];
            D_0.D.y = DP.vec[1];
            D_0.D.z = DP.vec[2];

            aim_deviation = acos(dot_productN((normal *)DP.vec, Light0->T->aim));

            if (aim_deviation < light_cone)
            {
                HP = collect_Hexa_Groups_light(D_0.D, G0, g_idx_0);
                shadow = cast_ray_from_Light0(P_pos, Light0, HP.G, HP.g_idx, -1, Pixel_Triangle, Pixel_Object); // index 0
            }

            P->R[idx] -= shadow;
            P->G[idx] -= shadow;
            P->B[idx] -= shadow;

            if (P->R[idx] < 0) P->R[idx] = 0;
            if (P->G[idx] < 0) P->G[idx] = 0;
            if (P->B[idx] < 0) P->B[idx] = 0;

            if (P->A[idx] == 255)
            {
                break;
            }
        }
    }

    //composite_Pixels(P, volume_counter);

    for (k = volume_counter - 1; k >= 0; k --)
    {
        idx = volume_index[k];

        a = P->A[idx] / 255.0;
        b = 1 - a;

        rgba.R = rgba.R * b + P->R[idx] * a;
        rgba.G = rgba.G * b + P->G[idx] * a;
        rgba.B = rgba.B * b + P->B[idx] * a;
        rgba.A = rgba.A * b + P->A[idx] * a; // 255;
    }

    //rgba.A /= volume_counter;

    return rgba;
}

void all_objects_in_frame(camera * C)
{
    int o;
    C->object_count = 0;

    int object_c = 0;

    for (o = 0; o < objectIndex; o ++)
    {
        if (loaded_objects[o] == 0)
            continue;
        C->objects[object_c++] = o;
        C->object_count++;
    }
}

void find_objects_in_frame(camera * C)
{
    //all_objects_in_frame(C);

    object * O;
    int o;
    C->object_count = 0;
    float deviation, aim_deviation, objectradius;
    aim objectAim;

    int object_c = 0;

    float d_index[objectIndex], s;

    for (o = 0; o < objectIndex; o ++)
    {
        if (loaded_objects[o] == 0)
            continue;
        O = objects[o];
        update_Box_T(O);
        objectAim = vector3d(O->B, C->T->pos);

        objectradius = atan2(O->B.radius, objectAim.dist * 2.0);

        deviation = C->view_major - objectradius;

        aim_deviation = acos(dot_productFF(C->T->aim, objectAim.vec));

        if (aim_deviation > deviation)
        {
            continue;
        }

        C->objects[object_c] = o;
        d_index[object_c++] = objectAim.dist;
        C->object_count ++;
    }

    int i, k, d;

    for (i = 1; i < C->object_count; i ++)
    {
        for (k = 0; k < C->object_count - i; k ++)
        {
            if (d_index[k] > d_index[k + 1]) // to reverse use other comparison
            {
                s = d_index[k + 1];
                d_index[k + 1] = d_index[k];
                d_index[k] = s;
                d = C->objects[k + 1];
                C->objects[k + 1] = C->objects[k];
                C->objects[k] = d;
            }
        }
    }
//    printf("C objects\n");
//    for (i = 0; i < C->object_count; i ++)
//    {
//        printf("%d ", C->objects[i]);
//    }
//    printf("\n");

}

direction unproject_screen_point(GLfloat winX, GLfloat winY, GLfloat winZ)
{
    direction D;

    GLdouble coords[3];
    GLint viewport[4];
    GLdouble modelview[16];
    GLdouble projection[16];

    glGetDoublev(GL_MODELVIEW_MATRIX, modelview);

    glGetDoublev(GL_PROJECTION_MATRIX, projection);
    glGetIntegerv(GL_VIEWPORT, viewport);

    gluUnProject(winX, winY, winZ, modelview, projection, viewport, &coords[0], &coords[1], &coords[2]);

    D.x = coords[0];
    D.y = coords[1];
    D.z = coords[2];

    //printf("UNPROJECT %f %f %f winZ %f\n", D.x, D.y, D.z, winZ);

    return D;
}

direction screen_point_to_vector(int x, int y, int hres, int vres, float h_view, float v_view)
{
    float angle_v = atan(v_view / 2.0);
    float R_v = sqrt((v_view / 2.0) + 1.0);
    float v_angle = angle_v * 2.0 * R_v;

    v_angle = (v_angle + v_view) / 2.0;

//    float angle_h = atan(h_view / 2.0);
//    float R_h = sqrt((h_view / 2.0) + 1.0);
//    float h_angle = angle_h * 2.0 * R_h;
//
//    h_angle = (h_angle + h_view) / 2.0;

    direction D;

    float H_Mark = h_view / 2.0;
    float V_Mark = v_angle / 2.0;
    float H_step = h_view / (float)hres;
    float V_step = v_angle / (float)vres;
    H_Mark -= H_step / 2.0;
    V_Mark -= V_step / 2.0;
    H_Mark += pi;
    V_Mark += pi_2;

    H_Mark -= x * H_step;
    V_Mark -= y * V_step;

    float R = sin(V_Mark);
    D.y = -cos(V_Mark);
    D.x = sin(H_Mark) * R;
    D.z = cos(H_Mark) * R;

    return D;
}

int point_on_screen_GLU(GLdouble point[3], GLdouble coords[3])
{
    GLint viewport[4];
    GLdouble modelview[16];
    GLdouble projection[16];

    glGetDoublev(GL_MODELVIEW_MATRIX, modelview);

    glGetDoublev(GL_PROJECTION_MATRIX, projection);
    glGetIntegerv(GL_VIEWPORT, viewport);

    int result = gluProject(point[0], point[1], point[2], modelview, projection, viewport, &coords[0], &coords[1], &coords[2]);
    return result;
}

int point_on_screen(camera * C, normal * N, float point[3], float coords[3], int width, int height, float rotVec_I[3][3])
{
    aim A;
    direction D;
    float R, Y_turn, T, X_turn;
    A = vector3dF(point, C->T->pos);
//    if (acos(dot_productFF(A.vec, C->T.aim)) >= C->view / 2.0)
//        return 0;
//    if (dot_productN(N, C->T.aim) > 0)
//        return 0;
    rotate_Vertex_I(rotVec_I, A.vec[0], A.vec[1], A.vec[2], &D);

    float h_view_2 = C->h_view / 2.0;
    float v_view_2 = C->v_view / 2.0;

    Y_turn = atan2(D.x, D.z);
    T = h_view_2 - Y_turn;
    R = T / C->h_view;
    coords[0] = width * R;

    X_turn = asin(D.y);
    T = v_view_2 - X_turn;
    R = T / C->v_view;
    coords[1] = height * R;

    coords[2] = A.dist;

    return 1;
}

void assignSelToQuads(object * O, quadrant * Q, int L, int sel)
{
    int idx;
    triangle * T;
    quadrant * Q0;
    int t, q;
    for (t = 0; t < 2; t ++)
    {
        idx = Q->trips[t];
        T = &O->trips_[L - 1][idx / ARRAYSIZE][idx % ARRAYSIZE];
        T->selected = sel;
    }
    if (Q->subdivs && L < SUBD)
    {
        for (q = 0; q < 4; q ++)
        {
            idx = Q->quads[q];
            Q0 = &O->quads_[L][idx / ARRAYSIZE][idx % ARRAYSIZE];
            Q0->selected = sel;
            assignSelToQuads(O, Q0, L + 1, sel);
        }
    }
}

void assignSelectionToQuads(object * O, polygon * P, int sel)
{
    int idx;
    triangle * T;
    quadrant * Q;
    int t, q;
    int L = 0;
    P->selected = sel;
    for (t = 0; t < P->tripcount; t ++)
    {
        idx = P->trips[t];
        T = &O->trips[idx / ARRAYSIZE][idx % ARRAYSIZE];
        T->selected = sel;
    }
    if (P->subdivs)
    {
        for (q = 0; q < P->edgecount; q ++)
        {
            idx = P->quads[q];
            Q = &O->quads_[L][idx / ARRAYSIZE][idx % ARRAYSIZE];
            Q->selected = sel;
            assignSelToQuads(O, Q, L + 1, sel);
        }
    }
}

void assign_Quad_Color(object * O, quadrant * Q, id_color * I, int L)
{
    quadrant * Q0;
    int idx = Q->index;
    int c_c = idx * 4 * 4;
    int c_c1;
    int v, e, q;
    if (O->vertex_arrays[L])
    {
        for (v = 0; v < 4; v ++)
        {
            idx = Q->texts[v];
            c_c1 = idx * 4;
            O->cols_array_[L][1][c_c1 ++] = I->R;
            O->cols_array_[L][1][c_c1 ++] = I->G;
            O->cols_array_[L][1][c_c1 ++] = I->B;
            O->cols_array_[L][1][c_c1 ++] = I->A;
        }
        for (v = 0; v < 4; v ++)
        {
            O->cols_array_[L][0][c_c ++] = I->R;
            O->cols_array_[L][0][c_c ++] = I->G;
            O->cols_array_[L][0][c_c ++] = I->B;
            O->cols_array_[L][0][c_c ++] = I->A;
        }
    }

    L++;
    if (L <= O->subdlevel)
    {
        for(e = 0; e < 4; e++)
        {
            q = Q->quads[e];
            Q0 = &O->quads_[L][q / ARRAYSIZE][q % ARRAYSIZE];
            assign_Quad_Color(O, Q0, I, L);
        }
    }
}

void assign_Surface_To_Geometry(object * O, int m)
{
    int p, l, q, t, idx;

    triangle * T;
    polygon * P;
    quadrant * Q;

    for (p = 0; p < O->polycount; p ++)
    {
        P = &O->polys[p / ARRAYSIZE][p % ARRAYSIZE];
        P->surface = m;

        for (t = 0; t < P->tripcount; t ++)
        {
            idx = P->trips[t];
            T = &O->trips[idx / ARRAYSIZE][idx % ARRAYSIZE];
            T->surface = m;
        }
    }

    for (l = 0; l <= O->subdlevel; l ++)
    {
        for (q = 0; q < O->quadcount_[l]; q ++)
        {
            Q = &O->quads_[l][q / ARRAYSIZE][q % ARRAYSIZE];
            Q->surface = m;

            for (t = 0; t < 2; t ++)
            {
                idx = Q->trips[t];
                T = &O->trips_[l][idx / ARRAYSIZE][idx % ARRAYSIZE];
                T->surface = m;
            }
        }
    }
}

void assign_Surface_To_Selected_Geometry(object * O, int m)
{
    int p, l, q, t, idx;

    triangle * T;
    polygon * P;
    quadrant * Q;

    for (p = 0; p < O->polycount; p ++)
    {
        P = &O->polys[p / ARRAYSIZE][p % ARRAYSIZE];
        if (P->selected)
        {
            P->surface = m;

            for (t = 0; t < P->tripcount; t ++)
            {
                idx = P->trips[t];
                T = &O->trips[idx / ARRAYSIZE][idx % ARRAYSIZE];
                T->surface = m;
            }
        }
    }

    for (l = 0; l <= O->subdlevel; l ++)
    {
        for (q = 0; q < O->quadcount_[l]; q ++)
        {
            Q = &O->quads_[l][q / ARRAYSIZE][q % ARRAYSIZE];
            if (Q->selected)
            {
                Q->surface = m;
                for (t = 0; t < 2; t ++)
                {
                    idx = Q->trips[t];
                    T = &O->trips_[l][idx / ARRAYSIZE][idx % ARRAYSIZE];
                    T->surface = m;
                }
            }
        }
    }
}

void load_m_colors_polygon(object * O, polygon * P)
{
    int L = 0;

    int c_c = 0;
    int c_c1 = 0;

    id_color * I;
    id_color I0;

    triangle * T;
    quadrant * Q;

    int idx, p, e, v, t, q;

    I = &I0;

    I0.R = Materials[P->surface].RGBA.R / 255;
    I0.G = Materials[P->surface].RGBA.G / 255;
    I0.B = Materials[P->surface].RGBA.B / 255;
    I0.A = Materials[P->surface].RGBA.A / 255;

    I = &I0;

    if (O->vertex_array)
    {
        for (t = 0; t < P->tripcount; t ++)
        {
            idx = P->trips[t];
            T = &O->trips[idx / ARRAYSIZE][idx % ARRAYSIZE];

            for (v = 0; v < 3; v ++)
            {
                idx = T->texts[v];
                c_c1 = idx * 4;
                O->cols_array[1][c_c1 ++] = I->R;
                O->cols_array[1][c_c1 ++] = I->G;
                O->cols_array[1][c_c1 ++] = I->B;
                O->cols_array[1][c_c1 ++] = I->A;
            }
        }

        polygon * P0;
        for (p = 0; p < O->polycount; p++)
        {
            if (p > P->index)
            {
                break;
            }
            P0 = &O->polys[p / ARRAYSIZE][p % ARRAYSIZE];

            for (e = 0; e < P0->edgecount; e ++)
            {
                if (p == P->index)
                {
                    O->cols_array[0][c_c ++] = I->R;
                    O->cols_array[0][c_c ++] = I->G;
                    O->cols_array[0][c_c ++] = I->B;
                    O->cols_array[0][c_c ++] = I->A;
                }
                else
                {
                    c_c += 4;
                }
            }
        }
    }

    if (O->vertex_arrays[L])
    {
        for(e = 0; e < P->edgecount; e ++)
        {
            q = P->quads[e];
            Q = &O->quads_[L][q / ARRAYSIZE][q % ARRAYSIZE];
            assign_Quad_Color(O, Q, I, L);
        }
    }
}

void load_m_colors_selected_polys(object * O)
{
    int p;
    polygon * P;

    for (p = 0; p < O->polycount; p ++)
    {
        P = &O->polys[p / ARRAYSIZE][p % ARRAYSIZE];
        if (P->selected)
            load_m_colors_polygon(O, P);
    }
}

void load_m_colors_object(object * O)
{
    int l = 0;

    int c;
    id_color * I;
    id_color I0;

    I0.R = Materials[O->surface].RGBA.R / 255;
    I0.G = Materials[O->surface].RGBA.G / 255;
    I0.B = Materials[O->surface].RGBA.B / 255;
    I0.A = Materials[O->surface].RGBA.A / 255;

    I = &I0;

    int f_size = sizeof(GLfloat);

    if (O->vertex_array)
    {
        for (c = 0; c < O->cols_array_size[0] / f_size; c += 4)
        {
            O->cols_array[0][c]     = I->R;
            O->cols_array[0][c + 1] = I->G;
            O->cols_array[0][c + 2] = I->B;
            O->cols_array[0][c + 3] = I->A;
        }

        for (c = 0; c < O->cols_array_size[1] / f_size; c += 4)
        {
            O->cols_array[1][c]     = I->R;
            O->cols_array[1][c + 1] = I->G;
            O->cols_array[1][c + 2] = I->B;
            O->cols_array[1][c + 3] = I->A;
        }
    }

    for(l = 0; l <= O->subdlevel; l ++)
    {
        if (O->vertex_arrays[l])
        {
            for (c = 0; c < O->cols_array_size_[l][0] / f_size; c += 4)
            {
                O->cols_array_[l][0][c]     = I->R;
                O->cols_array_[l][0][c + 1] = I->G;
                O->cols_array_[l][0][c + 2] = I->B;
                O->cols_array_[l][0][c + 3] = I->A;
            }

            for (c = 0; c < O->cols_array_size_[l][1] / f_size; c += 4)
            {
                O->cols_array_[l][1][c]     = I->R;
                O->cols_array_[l][1][c + 1] = I->G;
                O->cols_array_[l][1][c + 2] = I->B;
                O->cols_array_[l][1][c + 3] = I->A;
            }
        }
    }
}

void load_id_colors_polygon(object * O, polygon * P, int mode)
{
    int L = 0;
    int c_c = 0;
    int c_c1 = 0;

    triangle * T;
    quadrant * Q;

    int idx, p, e, v, t, q;
    id_color * I;
    id_color I0;
    if (mode == POLYGON_ID_COLORS)
    {
        idx = P->color;
        I = &O->icols[idx / ARRAYSIZE][idx % ARRAYSIZE];
    }
    else if (mode == OBJECT_COLORS)
    {
        if (P->selected == 1)
        {
            I0.R = 0;
            I0.G = 0;
            I0.B = 1;
            I0.A = 1;
        }
        else
        {
            I0.R = Materials[P->surface].RGBA.R / 255;
            I0.G = Materials[P->surface].RGBA.G / 255;
            I0.B = Materials[P->surface].RGBA.B / 255;
            I0.A = Materials[P->surface].RGBA.A / 255;
        }

        I = &I0;
    }
    if (O->vertex_array)
    {
//        if (E)
//        {
        for (t = 0; t < P->tripcount; t ++)
        {
            idx = P->trips[t];
            T = &O->trips[idx / ARRAYSIZE][idx % ARRAYSIZE];

            for (v = 0; v < 3; v ++)
            {
                idx = T->texts[v];
                c_c1 = idx * 4;
                O->cols_array[1][c_c1 ++] = I->R;
                O->cols_array[1][c_c1 ++] = I->G;
                O->cols_array[1][c_c1 ++] = I->B;
                O->cols_array[1][c_c1 ++] = I->A;
            }
        }
//        }
//        else
//        {
        polygon * P0;
        for (p = 0; p < O->polycount; p++)
        {
            if (p > P->index)
            {
                break;
            }
            P0 = &O->polys[p / ARRAYSIZE][p % ARRAYSIZE];

            for (e = 0; e < P0->edgecount; e ++)
            {
                if (p == P->index)
                {
                    O->cols_array[0][c_c ++] = I->R;
                    O->cols_array[0][c_c ++] = I->G;
                    O->cols_array[0][c_c ++] = I->B;
                    O->cols_array[0][c_c ++] = I->A;
                }
                else
                {
                    c_c += 4;
                }
            }
//            }
        }
    }
    if (O->vertex_arrays[L])
    {
        for(e = 0; e < P->edgecount; e ++)
        {
            q = P->quads[e];
            Q = &O->quads_[L][q / ARRAYSIZE][q % ARRAYSIZE];
            assign_Quad_Color(O, Q, I, L);
        }
    }
}

void put_In_Rectangle_Selection(camera * C, object * O, int * Objects, int Objects_count, int sel_add, int sel_type, dragrectangle * R, int splitview, float CamDist,
                                int draw_uv_view, int cull_Selection, int draw_locators, int L, int E)
{
    GLdouble point[3];
    GLdouble coords[3];

    int result;

    if (splitview)
    {
        if (C->ID == CAMERA_TOP)
        {
            glViewport(SIDEBAR, C->height/2 + BOTTOM_LINE, C->width/2, C->height/2);
            update_camera_top(&Camera_Top, CamDist);
        }
        else if (C->ID == CAMERA_PERSPECTIVE)
        {
            glViewport(C->width/2 + SIDEBAR, C->height/2 + BOTTOM_LINE, C->width/2, C->height/2);
            if (draw_uv_view)
            {
                glMatrixMode(GL_PROJECTION);
                glLoadIdentity();
                glOrtho(0, C->h_v_ratio, 0, 1, 1, -1);
            }
            else
            {
                update_camera_persp(&Camera_Persp, CamDist);
            }
        }
        else if (C->ID == CAMERA_FRONT)
        {
            glViewport(SIDEBAR, BOTTOM_LINE, C->width/2, C->height/2);
            update_camera_front(&Camera_Front, CamDist);
        }
        else if (C->ID == CAMERA_LEFT)
        {
            glViewport(C->width/2 + SIDEBAR, BOTTOM_LINE, C->width/2, C->height/2);
            update_camera_left(&Camera_Left, CamDist);
        }
    }
    else
    {
        glViewport(SIDEBAR, BOTTOM_LINE, C->width, C->height);
        if (draw_uv_view)
        {
            glMatrixMode(GL_PROJECTION);
            glLoadIdentity();
            glOrtho(0, C->h_v_ratio, 0, 1, 1, -1);
        }
        else
        {
            update_camera(Camera, CamDist);
        }
    }

    if (draw_uv_view && C->ID == CAMERA_PERSPECTIVE)
    {
        glDisable(GL_DEPTH_TEST);
        if (sel_type == VERTS)
        {
            int level, v, idx;
            uv * UV, * UV0;
            vertex * V;

            for (v = 0; v < O->textcount; v ++)
            {
                UV0 = &O->uvtex[v / ARRAYSIZE][v % ARRAYSIZE];
                UV = UV0;

                level = -1;

                while (UV0 != NULL && level < L)
                {
                    UV0 = UV0->uv_vert;
                    level ++;
                }

                point[0] = UV0->u;
                point[1] = UV0->v;
                point[2] = 0;

                result = point_on_screen_GLU(point, coords);
                if (result)
                {
                    if (coords[0] > R->x
                        && coords[0] < R->x + R->w
                        && coords[1] > R->y
                        && coords[1] < R->y + R->h)
                    {
                        idx = UV->vert;
                        if (idx > -1 && idx < O->vertcount)
                        {
                            V = &O->verts[idx / ARRAYSIZE][idx % ARRAYSIZE];
                            if (sel_add)
                            {
                                UV->selected = 1;
                                V->selected = 1;
                                if (selected_verts_count < CPS)
                                    selected_verts[selected_verts_count ++] = V;
                            }
                            else
                            {
                                UV->selected = 0;
                                V->selected = 0;
                            }
                        }
                    }
                }
            }
        }
        else if (sel_type == POLYS)
        {
            int p;
            polygon * P;

            for (p = 0; p < O->polycount; p ++)
            {
                P = &O->polys[p / ARRAYSIZE][p % ARRAYSIZE];
                point[0] = P->B2.u;
                point[1] = P->B2.v;
                point[2] = 0;
                result = point_on_screen_GLU(point, coords);
                if (result)
                {
                    if (coords[0] > R->x
                        && coords[0] < R->x + R->w
                        && coords[1] > R->y
                        && coords[1] < R->y + R->h)
                    {
                        if (sel_add)
                        {
                            P->selected = 1;
                            assignSelectionToQuads(O, P, 1);
                        }
                        else
                        {
                            P->selected = 0;
                            assignSelectionToQuads(O, P, 0);
                        }
                        //load_id_colors_polygon(O, P, OBJECT_COLORS);
                    }
                }
            }
        }
        else if (sel_type == EDGES)
        {
            int e, idx;
            uv_edge * UVE;
            edge * E;

            for (e = 0; e < O->uvedcount; e ++)
            {
                UVE = &O->uveds[e / ARRAYSIZE][e % ARRAYSIZE];
                point[0] = UVE->B.u;
                point[1] = UVE->B.v;
                point[2] = 0;
                result = point_on_screen_GLU(point, coords);
                if (result)
                {
                    if (coords[0] > R->x
                        && coords[0] < R->x + R->w
                        && coords[1] > R->y
                        && coords[1] < R->y + R->h)
                    {
                        idx = UVE->edge;
                        if (idx > -1 && idx < O->edgecount)
                        {
                            E = &O->edges[idx / ARRAYSIZE][idx % ARRAYSIZE];
                            if (sel_add)
                            {
                                UVE->selected = 1;
                                E->selected = 1;
                                if (selected_edges_count < SELEDGES)
                                    selected_edges[selected_edges_count ++] = E;
                            }
                            else
                            {
                                UVE->selected = 0;
                                E->selected = 0;
                            }
                        }
                    }
                }
            }
        }
        glEnable(GL_DEPTH_TEST);
    }
    else
    {
        if (sel_type == CURVE)
        {
            int c;
            curve * C;
            for (c = 0; c < curvesIndex; c ++)
            {
                C = curves[c];
                if (C->cps_count > 1)
                {
                    point[0] = (C->cps[0]->pos[0] + C->cps[C->cps_count - 1]->pos[0]) / 2.0;
                    point[1] = (C->cps[0]->pos[1] + C->cps[C->cps_count - 1]->pos[1]) / 2.0;
                    point[2] = (C->cps[0]->pos[2] + C->cps[C->cps_count - 1]->pos[2]) / 2.0;
                    result = point_on_screen_GLU(point, coords);
                    if (result)
                    {
                        if (coords[0] > R->x
                            && coords[0] < R->x + R->w
                            && coords[1] > R->y
                            && coords[1] < R->y + R->h)
                        {
                            if (sel_add)
                            {
                                C->selected = 1;
                                if (selected_curves_count < CURVES)
                                    selected_curves[selected_curves_count ++] = C->index;
                            }
                            else
                            {
                                C->selected = 0;
                            }
                        }
                    }
                }
            }
        }
        else if (sel_type == CURVE_CP)
        {
            int c;
            cp * CP;
            for (c = 0; c < cpsIndex; c ++)
            {
                CP = cps[c];
                point[0] = CP->pos[0];
                point[1] = CP->pos[1];
                point[2] = CP->pos[2];
                result = point_on_screen_GLU(point, coords);
                if (result)
                {
                    if (coords[0] > R->x
                        && coords[0] < R->x + R->w
                        && coords[1] > R->y
                        && coords[1] < R->y + R->h)
                    {
                        if (sel_add)
                        {
                            CP->selected = 1;
                            if (selected_cps_count < CPS)
                                selected_cps[selected_cps_count ++] = CP->index;
                        }
                        else
                        {
                            CP->selected = 0;
                        }
                    }
                }
            }
        }
        else if (sel_type == CURVE_SEGMENT)
        {
            int c, s;
            curve * C;
            curve_segment * S;

            for (c = 0; c < curvesIndex; c ++)
            {
                C = curves[c];

                for (s = 0; s < C->segment_count; s ++)
                {
                    S = C->segments[s];

                    point[0] = S->B[0];
                    point[1] = S->B[1];
                    point[2] = S->B[2];
                    result = point_on_screen_GLU(point, coords);
                    if (result)
                    {
                        if (coords[0] > R->x
                            && coords[0] < R->x + R->w
                            && coords[1] > R->y
                            && coords[1] < R->y + R->h)
                        {
                            if (sel_add)
                            {
                                S->selected = 1;
                                if (selected_segments_count < SEGMENTS)
                                    selected_segments[selected_segments_count ++] = S->index;
                            }
                            else
                            {
                                S->selected = 0;
                            }
                        }
                    }
                }
            }
        }
        else if (sel_type == BONEZ)
        {
            if (draw_locators)
            {
                int b;
                bone * B;
                for (b = 0; b < bonesIndex; b ++)
                {
                    B = bones[b];
                    point[0] = (B->A->pos[0] + B->B->pos[0]) / 2.0;
                    point[1] = (B->A->pos[1] + B->B->pos[1]) / 2.0;
                    point[2] = (B->A->pos[2] + B->B->pos[2]) / 2.0;
                    result = point_on_screen_GLU(point, coords);
                    if (result)
                    {
                        if (coords[0] > R->x
                            && coords[0] < R->x + R->w
                            && coords[1] > R->y
                            && coords[1] < R->y + R->h)
                        {
                            if (sel_add)
                                B->selected = 1;
                            else
                                B->selected = 0;
                        }
                    }
                }
            }
        }
        else if (sel_type == OBJES)
        {
            if (draw_locators)
            {
                int t;
                transformer * T;
                for (t = 1; t < transformerIndex; t ++)
                {
                    T = transformers[t];
                    point[0] = T->pos[0];
                    point[1] = T->pos[1];
                    point[2] = T->pos[2];
                    result = point_on_screen_GLU(point, coords);
                    if (result)
                    {
                        if (coords[0] > R->x
                            && coords[0] < R->x + R->w
                            && coords[1] > R->y
                            && coords[1] < R->y + R->h)
                        {
                            if (sel_add)
                                T->selected = 1;
                            else
                                T->selected = 0;
                        }
                    }
                }
            }
            else
            {
                int o;
                object * O0;

                for (o = 0; o < objectIndex; o ++)
                {
                    if (loaded_objects[o] == 0)
                        continue;

                    O0 = objects[o];

                    update_Box_T(O0);

                    point[0] = O0->B.Tx;
                    point[1] = O0->B.Ty;
                    point[2] = O0->B.Tz;

                    result = point_on_screen_GLU(point, coords);
                    if (result)
                    {
                        if (coords[0] > R->x
                            && coords[0] < R->x + R->w
                            && coords[1] > R->y
                            && coords[1] < R->y + R->h)
                        {
                            if (sel_add)
                                O0->selected = 1;
                            else
                                O0->selected = 0;
                        }
                    }
                }
            }
        }
        else if (sel_type == VERTS)
        {
            int level, o, u, v, idx;
            vertex * V, * V0;
            uv * UV;

            object * O0;

            aim vertAim;
            normal vertnormal;

            for (o = 0; o < Objects_count; o ++)
            {
                O0 = objects[Objects[o]];
                for (v = 0; v < O0->vertcount; v ++)
                {
                    V = &O0->verts[v / ARRAYSIZE][v % ARRAYSIZE];

                    V0 = V;

                    level = -1;

                    while (V->vert != NULL && level < L)
                    {
                        V = V->vert;
                        level ++;
                    }

                    point[0] = V->Tx;
                    point[1] = V->Ty;
                    point[2] = V->Tz;

                    if (cull_Selection)
                    {
                        vertnormal.x = -V->N.Tx;
                        vertnormal.y = -V->N.Ty;
                        vertnormal.z = -V->N.Tz;

                        if (C->ortho)
                        {
                            vertAim.vec[0] = C->T->aim[0];
                            vertAim.vec[1] = C->T->aim[1];
                            vertAim.vec[2] = C->T->aim[2];
                        }
                        else
                        {
                            vertAim = vector3dD(point, C->T->pos);
                        }

                        if (dot_productN(&vertnormal, vertAim.vec) < 0)
                            result = 0;
                        else
                            result = point_on_screen_GLU(point, coords);
                    }
                    else
                    {
                        result = point_on_screen_GLU(point, coords);
                    }

                    if (result)
                    {
                        if (coords[0] > R->x
                            && coords[0] < R->x + R->w
                            && coords[1] > R->y
                            && coords[1] < R->y + R->h)
                        {
                            if (sel_add)
                            {
                                V0->selected = 1;
                                if (selected_verts_count < CPS && O == O0)
                                    selected_verts[selected_verts_count ++] = V0;
                            }
                            else
                            {
                                V0->selected = 0;
                            }
                            for (u = 0; u < V0->uv_vertcount; u ++)
                            {
                                idx = V0->uv_verts[u];
                                if (idx > -1 && idx < O0->textcount)
                                {
                                    UV = &O0->uvtex[idx / ARRAYSIZE][idx % ARRAYSIZE];
                                    UV->selected = V0->selected;
                                }
                            }
                        }
                    }
                }
            }
        }
        else if (sel_type == POLYS)
        {
            int o, p;
            polygon * P;

            object * O;

            aim polyAim;
            normal polynormal;

            for (o = 0; o < Objects_count; o ++)
            {
                O = objects[Objects[o]];
                for (p = 0; p < O->polycount; p ++)
                {
                    P = &O->polys[p / ARRAYSIZE][p % ARRAYSIZE];
                    point[0] = P->B.Tx;
                    point[1] = P->B.Ty;
                    point[2] = P->B.Tz;
                    if (cull_Selection)
                    {
                        polynormal.x = -P->N.Tx;
                        polynormal.y = -P->N.Ty;
                        polynormal.z = -P->N.Tz;

                        if (C->ortho)
                        {
                            polyAim.vec[0] = C->T->aim[0];
                            polyAim.vec[1] = C->T->aim[1];
                            polyAim.vec[2] = C->T->aim[2];
                        }
                        else
                        {
                            polyAim = vector3dD(point, C->T->pos);
                        }

                        if (dot_productN(&polynormal, polyAim.vec) < 0)
                            result = 0;
                        else
                            result = point_on_screen_GLU(point, coords);
                    }
                    else
                    {
                        result = point_on_screen_GLU(point, coords);
                    }
                    if (result)
                    {
                        if (coords[0] > R->x
                            && coords[0] < R->x + R->w
                            && coords[1] > R->y
                            && coords[1] < R->y + R->h)
                        {
                            if (sel_add)
                            {
                                P->selected = 1;
                                assignSelectionToQuads(O, P, 1);
                            }
                            else
                            {
                                P->selected = 0;
                                assignSelectionToQuads(O, P, 0);
                            }
                            //load_id_colors_polygon(O, P, OBJECT_COLORS);
                        }
                    }
                }
            }
        }
        else if (sel_type == EDGES)
        {
            int o, u, e, idx;
            edge * E;
            uv_edge * UVE;

            object * O0;

            aim edgeAim;
            normal edgenormal;

            for (o = 0; o < Objects_count; o ++)
            {
                O0 = objects[Objects[o]];
                for (e = 0; e < O0->edgecount; e ++)
                {
                    E = &O0->edges[e / ARRAYSIZE][e % ARRAYSIZE];
                    point[0] = E->B.Tx;
                    point[1] = E->B.Ty;
                    point[2] = E->B.Tz;
                    if (cull_Selection)
                    {
                        edgenormal.x = -E->N.Tx;
                        edgenormal.y = -E->N.Ty;
                        edgenormal.z = -E->N.Tz;

                        if (C->ortho)
                        {
                            edgeAim.vec[0] = C->T->aim[0];
                            edgeAim.vec[1] = C->T->aim[1];
                            edgeAim.vec[2] = C->T->aim[2];
                        }
                        else
                        {
                            edgeAim = vector3dD(point, C->T->pos);
                        }

                        if (dot_productN(&edgenormal, edgeAim.vec) < 0)
                            result = 0;
                        else
                            result = point_on_screen_GLU(point, coords);
                    }
                    else
                    {
                        result = point_on_screen_GLU(point, coords);
                    }
                    if (result)
                    {
                        if (coords[0] > R->x
                            && coords[0] < R->x + R->w
                            && coords[1] > R->y
                            && coords[1] < R->y + R->h)
                        {
                            if (sel_add)
                            {
                                E->selected = 1;
                                if (selected_edges_count < SELEDGES && O == O0)
                                    selected_edges[selected_edges_count ++] = E;
                            }
                            else
                                E->selected = 0;
                            for (u = 0; u < E->uv_edcount; u ++)
                            {
                                idx = E->uv_edges[u];
                                if (idx > -1 && idx < O0->uvedcount)
                                {
                                    UVE = &O0->uveds[idx / ARRAYSIZE][idx % ARRAYSIZE];
                                    UVE->selected = E->selected;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

int draw_UV_Edge_ID_recursive(camera * C, object * O, uv_edge * UVE, int L, int l)
{
    if (L > l) return 0;

    int idx;

    if (UVE->subdivs == 0 || L == l)
    {
        uv * UV0, * UV1;

        idx = UVE->uv_verts[0];
        UV0 = &O->uvtex_[L][idx / ARRAYSIZE][idx % ARRAYSIZE];

        idx = UVE->uv_verts[1];
        UV1 = &O->uvtex_[L][idx / ARRAYSIZE][idx % ARRAYSIZE];

        //glBegin(GL_LINES);
        glVertex2f(UV0->u, UV0->v);
        glVertex2f(UV1->u, UV1->v);
        //glEnd();

        return 1;
    }
    else
    {
        L ++;
        if (L >= SUBD) return 0;

        uv_edge * UVE0;

        idx = UVE->uv_edges[0];
        UVE0 = &O->uveds_[L][idx / ARRAYSIZE][idx % ARRAYSIZE];
        draw_UV_Edge_ID_recursive(C, O, UVE0, L, l);
        idx = UVE->uv_edges[1];
        UVE0 = &O->uveds_[L][idx / ARRAYSIZE][idx % ARRAYSIZE];
        draw_UV_Edge_ID_recursive(C, O, UVE0, L, l);
    }
    return 0;
}

int draw_Segment_ID_recursive(curve_segment * S, int L, int l)
{
    if (L > l) return 0;

    if (S->subdivided == 0 || L == l)
    {
        glVertex3f(S->A[0], S->A[1], S->A[2]);
        glVertex3f(S->C[0], S->C[1], S->C[2]);

        return 1;
    }
    else
    {
        L ++;
        if (L >= SUBD) return 0;

        draw_Segment_ID_recursive(S->segment[0], L, l);
        draw_Segment_ID_recursive(S->segment[1], L, l);
    }

    return 0;
}

int draw_Edge_ID_recursive(camera * C, object * O, edge * E, int L, int l)
{
    if (L > l) return 0;

    int idx;

    float factor;
    float dot;
    float view_minor;

    if (C->view_minor > 1)
        view_minor = C->view_minor;
    else
        view_minor = 1;

    if (E->subdivs == 0 || L == l)
    {
        aim Aim;
        normal Normal;

        Normal.x = -E->N.Tx;
        Normal.y = -E->N.Ty;
        Normal.z = -E->N.Tz;

        Aim = vector3d(E->B, C->T->pos);

        if (C->ortho)
        {
            dot = dot_productN(&Normal, C->T->aim);
        }
        else
        {
            dot = dot_productN(&Normal, Aim.vec);
        }

        if (dot < 0)
            return 0;

        vertex * V0, * V1;

        idx = E->verts[0];
        V0 = &O->verts_[L][idx / ARRAYSIZE][idx % ARRAYSIZE];

        idx = E->verts[1];
        V1 = &O->verts_[L][idx / ARRAYSIZE][idx % ARRAYSIZE];

        Normal.x = -V0->N.Tx;
        Normal.y = -V0->N.Ty;
        Normal.z = -V0->N.Tz;

        Aim = vector3d_T(V0, C->T->pos);

        if (C->ortho)
        {
            dot = dot_productN(&Normal, C->T->aim);
        }
        else
        {
            dot = dot_productN(&Normal, Aim.vec);
        }

        if (C->ortho)
        {
            factor = Aim.dist * view_minor * 0.002 * dot;
            glVertex3f(V0->Tx + V0->N.Tx * factor, V0->Ty + V0->N.Ty * factor, V0->Tz + V0->N.Tz * factor);
        }
        else
        {
            factor = Aim.dist * view_minor * 0.3 * O->mean_Edge * dot;
            if (factor > Aim.dist / 2)
            {
                factor = Aim.dist / 2;
            }
            glVertex3f(V0->Tx - Aim.vec[0] * factor, V0->Ty - Aim.vec[1] * factor, V0->Tz - Aim.vec[2] * factor);
        }

        Normal.x = -V1->N.Tx;
        Normal.y = -V1->N.Ty;
        Normal.z = -V1->N.Tz;

        Aim = vector3d_T(V1, C->T->pos);

        if (C->ortho)
        {
            dot = dot_productN(&Normal, C->T->aim);
        }
        else
        {
            dot = dot_productN(&Normal, Aim.vec);
        }

        if (C->ortho)
        {
            factor = Aim.dist * view_minor * 0.002 * dot;
            glVertex3f(V1->Tx + V1->N.Tx * factor, V1->Ty + V1->N.Ty * factor, V1->Tz + V1->N.Tz * factor);
        }
        else
        {
            factor = Aim.dist * view_minor * 0.3 * O->mean_Edge * dot;
            if (factor > Aim.dist / 2)
            {
                factor = Aim.dist / 2;
            }
            glVertex3f(V1->Tx - Aim.vec[0] * factor, V1->Ty - Aim.vec[1] * factor, V1->Tz - Aim.vec[2] * factor);
        }

        return 1;
    }
    else
    {
        L ++;
        if (L >= SUBD) return 0;

        edge * E0;

        idx = E->edges[0];
        E0 = &O->edges_[L][idx / ARRAYSIZE][idx % ARRAYSIZE];
        draw_Edge_ID_recursive(C, O, E0, L, l);
        idx = E->edges[1];
        E0 = &O->edges_[L][idx / ARRAYSIZE][idx % ARRAYSIZE];
        draw_Edge_ID_recursive(C, O, E0, L, l);
    }
    return 0;
}

int draw_Edge_recursive(object * O, edge * E, int L, int l, camera * C, int wireframe, float edge_length)
{
    if (L > l) return 0;

    int idx;

    aim Aim;

    normal Normal;

    float factor;
    float dot;
    float view_minor;

    if (C->view_minor > 1)
        view_minor = C->view_minor;
    else
        view_minor = 1;

    if (E->subdivs == 0 || L == l)
    {
        vertex * V0, * V1;

        Aim = vector3d(E->B, C->T->pos);

        Normal.x = -E->N.Tx;
        Normal.y = -E->N.Ty;
        Normal.z = -E->N.Tz;

        if (C->ortho)
        {
            dot = dot_productN(&Normal, C->T->aim);
        }
        else
        {
            dot = dot_productN(&Normal, Aim.vec);
        }

        if (!wireframe)
        {
            if (dot < 0)
                return 0;
        }

        idx = E->verts[0];
        V0 = &O->verts_[L][idx / ARRAYSIZE][idx % ARRAYSIZE];

        idx = E->verts[1];
        V1 = &O->verts_[L][idx / ARRAYSIZE][idx % ARRAYSIZE];

        glBegin(GL_LINES);
        glColor4ubv(line_color);
        glNormal3f(E->N.Tx, E->N.Ty, E->N.Tz);

        Normal.x = -V0->N.Tx;
        Normal.y = -V0->N.Ty;
        Normal.z = -V0->N.Tz;

        Aim = vector3d_T(V0, C->T->pos);

        if (C->ortho)
        {
            dot = dot_productN(&Normal, C->T->aim);
        }
        else
        {
            dot = dot_productN(&Normal, Aim.vec);
        }

        if (C->ortho)
        {
            factor = Aim.dist * view_minor * 0.002 * dot;
            glVertex3f(V0->Tx + V0->N.Tx * factor, V0->Ty + V0->N.Ty * factor, V0->Tz + V0->N.Tz * factor);
        }
        else
        {
            factor = Aim.dist * view_minor * 0.3 * edge_length * dot;
            if (factor > Aim.dist / 2)
            {
                factor = Aim.dist / 2;
            }
            glVertex3f(V0->Tx - Aim.vec[0] * factor, V0->Ty - Aim.vec[1] * factor, V0->Tz - Aim.vec[2] * factor);
        }

        Normal.x = -V1->N.Tx;
        Normal.y = -V1->N.Ty;
        Normal.z = -V1->N.Tz;

        Aim = vector3d_T(V1, C->T->pos);

        if (C->ortho)
        {
            dot = dot_productN(&Normal, C->T->aim);
        }
        else
        {
            dot = dot_productN(&Normal, Aim.vec);
        }

        if (C->ortho)
        {
            factor = Aim.dist * view_minor * 0.002 * dot;
            glVertex3f(V1->Tx + V1->N.Tx * factor, V1->Ty + V1->N.Ty * factor, V1->Tz + V1->N.Tz * factor);
        }
        else
        {
            factor = Aim.dist * view_minor * 0.3 * edge_length * dot;
            if (factor > Aim.dist / 2)
            {
                factor = Aim.dist / 2;
            }
            glVertex3f(V1->Tx - Aim.vec[0] * factor, V1->Ty - Aim.vec[1] * factor, V1->Tz - Aim.vec[2] * factor);
        }
        glEnd();

        return 1;
    }
    else
    {
        L ++;
        if (L >= SUBD) return 0;

        edge * E0;

        idx = E->edges[0];
        E0 = &O->edges_[L][idx / ARRAYSIZE][idx % ARRAYSIZE];
        draw_Edge_recursive(O, E0, L, l, C, wireframe, edge_length);
        idx = E->edges[1];
        E0 = &O->edges_[L][idx / ARRAYSIZE][idx % ARRAYSIZE];
        draw_Edge_recursive(O, E0, L, l, C, wireframe, edge_length);
    }
    return 0;
}

int draw_UVEdge_recursive(object * O, uv_edge * E, int L, int l)
{
    if (L > l) return 0;

    int idx;

    if (E->subdivs == 0 || L == l)
    {
        E->visible = 1;
        uv * UV0, * UV1;

        idx = E->uv_verts[0];
        UV0 = &O->uvtex_[L][idx / ARRAYSIZE][idx % ARRAYSIZE];

        idx = E->uv_verts[1];
        UV1 = &O->uvtex_[L][idx / ARRAYSIZE][idx % ARRAYSIZE];

        glVertex2f(UV0->u, UV0->v);
        glVertex2f(UV1->u, UV1->v);

        return 1;
    }
    else
    {
        L ++;
        if (L >= SUBD) return 0;

        uv_edge * E0;

        idx = E->uv_edges[0];
        E0 = &O->uveds_[L][idx / ARRAYSIZE][idx % ARRAYSIZE];
        draw_UVEdge_recursive(O, E0, L, l);
        idx = E->uv_edges[1];
        E0 = &O->uveds_[L][idx / ARRAYSIZE][idx % ARRAYSIZE];
        draw_UVEdge_recursive(O, E0, L, l);
    }
    return 0;
}

void render_Outline_quads(object * O, camera * C, int L)
{
    int e, idx;
    glColor4ubv(line_color);
    int frontfacing0, frontfacing1;
    aim polyAim;
    float polyNormal[3];
    edge * E;
    vertex * V0, * V1;
    quadrant * Q;

    float factor = CamDist * 0.0007;

    if (C->ortho)
    {
        polyAim.vec[0] = C->T->aim[0];
        polyAim.vec[1] = C->T->aim[1];
        polyAim.vec[2] = C->T->aim[2];
    }

    for (e = 0; e < O->edgecount_[L]; e++)
    {
        frontfacing0 = 0;
        frontfacing1 = 0;

        E = &O->edges_[L][e / ARRAYSIZE][e % ARRAYSIZE];
        if (E->polycount > 1)
        {
            idx = E->polys[0];
            Q = &O->quads_[L][idx / ARRAYSIZE][idx % ARRAYSIZE];
            if (!C->ortho)
            {
                polyAim = vector3d(Q->B, C->T->pos);
            }

            polyNormal[0] = -Q->N.Tx;
            polyNormal[1] = -Q->N.Ty;
            polyNormal[2] = -Q->N.Tz;
            if (dot_productFF(polyNormal, polyAim.vec) > 0)
            {
                frontfacing0 = 1;
            }
            idx = E->polys[1];
            Q = &O->quads_[L][idx / ARRAYSIZE][idx % ARRAYSIZE];
            if (!C->ortho)
            {
                polyAim = vector3d(Q->B, C->T->pos);
            }
            polyNormal[0] = -Q->N.Tx;
            polyNormal[1] = -Q->N.Ty;
            polyNormal[2] = -Q->N.Tz;
            if (dot_productFF(polyNormal, polyAim.vec) > 0)
            {
                frontfacing1 = 1;
            }
        }
        else
        {
            idx = E->polys[0];
            Q = &O->quads_[L][idx / ARRAYSIZE][idx % ARRAYSIZE];
            if (!C->ortho)
            {
                polyAim = vector3d(Q->B, C->T->pos);
            }
            polyNormal[0] = -Q->N.Tx;
            polyNormal[1] = -Q->N.Ty;
            polyNormal[2] = -Q->N.Tz;
            if (dot_productFF(polyNormal, polyAim.vec) > 0)
            {
                frontfacing0 = 1;
            }
        }
        if ((frontfacing0 == 1 && frontfacing1 == 0) || (frontfacing0 == 0 && frontfacing1 == 1))
        {
            idx = E->verts[0];
            V0 = &O->verts_[L][idx / ARRAYSIZE][idx % ARRAYSIZE];

            idx = E->verts[1];
            V1 = &O->verts_[L][idx / ARRAYSIZE][idx % ARRAYSIZE];

            glBegin(GL_LINES);
            glNormal3f(E->N.Tx, E->N.Ty, E->N.Tz);

            glVertex3f(V0->Tx + V0->N.Tx * factor, V0->Ty + V0->N.Ty * factor, V0->Tz + V0->N.Tz * factor);
            glVertex3f(V1->Tx + V1->N.Tx * factor, V1->Ty + V1->N.Ty * factor, V1->Tz + V1->N.Tz * factor);
            glEnd();
        }
    }
}

void render_Outline_polys(object * O, camera * C)
{
    int e, idx;
    glColor4ubv(line_color);
    int frontfacing0, frontfacing1;
    aim polyAim;
    float polyNormal[3];
    edge * E;
    polygon * P;
    vertex * V0, * V1;

    float factor = CamDist * 0.0007;

    if (C->ortho)
    {
        polyAim.vec[0] = C->T->aim[0];
        polyAim.vec[1] = C->T->aim[1];
        polyAim.vec[2] = C->T->aim[2];
    }

    for (e = 0; e < O->edgecount; e++)
    {
        frontfacing0 = 0;
        frontfacing1 = 0;

        E = &O->edges[e / ARRAYSIZE][e % ARRAYSIZE];
        if (E->polycount > 1)
        {
            idx = E->polys[0];
            P = &O->polys[idx / ARRAYSIZE][idx % ARRAYSIZE];
            if (!C->ortho)
            {
                polyAim = vector3d(P->B, C->T->pos);
            }

            polyNormal[0] = -P->N.Tx;
            polyNormal[1] = -P->N.Ty;
            polyNormal[2] = -P->N.Tz;
            if (dot_productFF(polyNormal, polyAim.vec) > 0)
            {
                frontfacing0 = 1;
            }
            idx = E->polys[1];
            P = &O->polys[idx / ARRAYSIZE][idx % ARRAYSIZE];
            if (!C->ortho)
            {
                polyAim = vector3d(P->B, C->T->pos);
            }
            polyNormal[0] = -P->N.Tx;
            polyNormal[1] = -P->N.Ty;
            polyNormal[2] = -P->N.Tz;
            if (dot_productFF(polyNormal, polyAim.vec) > 0)
            {
                frontfacing1 = 1;
            }
        }
        else
        {
            idx = E->polys[0];
            P = &O->polys[idx / ARRAYSIZE][idx % ARRAYSIZE];
            if (!C->ortho)
            {
                polyAim = vector3d(P->B, C->T->pos);
            }
            polyNormal[0] = -P->N.Tx;
            polyNormal[1] = -P->N.Ty;
            polyNormal[2] = -P->N.Tz;
            if (dot_productFF(polyNormal, polyAim.vec) > 0)
            {
                frontfacing0 = 1;
            }
        }
        if ((frontfacing0 == 1 && frontfacing1 == 0) || (frontfacing0 == 0 && frontfacing1 == 1))
        {
            idx = E->verts[0];
            V0 = &O->verts[idx / ARRAYSIZE][idx % ARRAYSIZE];

            idx = E->verts[1];
            V1 = &O->verts[idx / ARRAYSIZE][idx % ARRAYSIZE];

            glBegin(GL_LINES);
            glNormal3f(E->N.Tx, E->N.Ty, E->N.Tz);

            glVertex3f(V0->Tx + V0->N.Tx * factor, V0->Ty + V0->N.Ty * factor, V0->Tz + V0->N.Tz * factor);
            glVertex3f(V1->Tx + V1->N.Tx * factor, V1->Ty + V1->N.Ty * factor, V1->Tz + V1->N.Tz * factor);
            glEnd();
        }
    }
}

int draw_Edge(object * O, edge * E, camera * C, int wireframe, float edge_length)
{
    int idx;

    vertex * V0, * V1;

    float factor;
    float dot;
    float view_minor;

    aim Aim;
    normal Normal;

    Aim = vector3d(E->B, C->T->pos);

    Normal.x = -E->N.Tx;
    Normal.y = -E->N.Ty;
    Normal.z = -E->N.Tz;

    if (C->ortho)
    {
        dot = dot_productN(&Normal, C->T->aim);
    }
    else
    {
        dot = dot_productN(&Normal, Aim.vec);
    }

    if (!wireframe)
    {
        if (dot < 0)
            return 0;
    }

    idx = E->verts[0];
    V0 = &O->verts[idx / ARRAYSIZE][idx % ARRAYSIZE];

    idx = E->verts[1];
    V1 = &O->verts[idx / ARRAYSIZE][idx % ARRAYSIZE];

    glBegin(GL_LINES);
    glColor4ubv(line_color);
    glNormal3f(E->N.Tx, E->N.Ty, E->N.Tz);
    //glTexCoord2f(0.0, 0.0);
    Normal.x = -V0->N.Tx;
    Normal.y = -V0->N.Ty;
    Normal.z = -V0->N.Tz;

    Aim = vector3d_T(V0, C->T->pos);

    if (C->ortho)
    {
        dot = dot_productN(&Normal, C->T->aim);
    }
    else
    {
        dot = dot_productN(&Normal, Aim.vec);
    }

    if (C->view_minor > 1)
        view_minor = C->view_minor;
    else
        view_minor = 1;

    if (C->ortho)
    {
        factor = Aim.dist * view_minor * 0.002 * dot;
        glVertex3f(V0->Tx + V0->N.Tx * factor, V0->Ty + V0->N.Ty * factor, V0->Tz + V0->N.Tz * factor);
    }
    else
    {
        factor = Aim.dist * view_minor * 0.3 * edge_length * dot;
        if (factor > Aim.dist / 2)
        {
            factor = Aim.dist / 2;
        }
        glVertex3f(V0->Tx - Aim.vec[0] * factor, V0->Ty - Aim.vec[1] * factor, V0->Tz - Aim.vec[2] * factor);
    }

    Normal.x = -V1->N.Tx;
    Normal.y = -V1->N.Ty;
    Normal.z = -V1->N.Tz;

    Aim = vector3d_T(V1, C->T->pos);

    if (C->ortho)
    {
        dot = dot_productN(&Normal, C->T->aim);
    }
    else
    {
        dot = dot_productN(&Normal, Aim.vec);
    }

    if (C->ortho)
    {
        factor = Aim.dist * view_minor * 0.002 * dot;
        glVertex3f(V1->Tx + V1->N.Tx * factor, V1->Ty + V1->N.Ty * factor, V1->Tz + V1->N.Tz * factor);
    }
    else
    {
        factor = Aim.dist * view_minor * 0.3 * edge_length * dot;
        if (factor > Aim.dist / 2)
        {
            factor = Aim.dist / 2;
        }
        glVertex3f(V1->Tx - Aim.vec[0] * factor, V1->Ty - Aim.vec[1] * factor, V1->Tz - Aim.vec[2] * factor);
    }
    glEnd();

    return 1;
}

void render_trips_OnScreen_(camera * C, int wireframe, int edgedraw, int vertdraw, int l, int currentObject, int Selection_Mode)
{
    int v, o, t, e, idx, x_, y_, L;
    object * O;
    triangle * T;
    edge * E, * E0;
    vertex * V;

    normal polynormal;

    surface_Material Material;

    float dot_light;

    Uint32 pix;

    static GLubyte color[4];

    Uint8 r, g, b, a;

    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_LIGHTING);

    for (o = 0; o < C->object_count; o++)
    {
        idx = C->objects[o];
        O = objects[idx];

        if (l > O->subdlevel)
        {
            L = O->subdlevel;
        }
        else
        {
            L = l;
        }

        if (L == -1)
        {
            for (t = 0; t < O->tripcount; t++)
            {
                T = &O->trips[t / ARRAYSIZE][t % ARRAYSIZE];
                polynormal.x = -T->N.Tx;
                polynormal.y = -T->N.Ty;
                polynormal.z = -T->N.Tz;

                idx = T->surface;
                Material = Materials[idx];

                texture = Surf_Text[Material.texture];

                if (Material.use_texture && texture != NULL)
                {
                    x_ = abs((int)(texture->w * T->B2.u)) % texture->w;
                    y_ = abs((int)(texture->h * T->B2.v)) % texture->h;
                    pix = get_pixel32(texture, x_, y_);
                    SDL_GetRGBA(pix, texture->format, &r, &g, &b, &a);
                }
                else if (T->selected)
                {
                    r = 0;
                    g = 0;
                    b = 255;
                    a = 255;
                }
                else
                {
                    r = Material.RGBA.R;
                    g = Material.RGBA.G;
                    b = Material.RGBA.B;
                    a = Material.RGBA.A;
                }

                dot_light = dot_productN(&polynormal, light_vec);

                if (dot_light < 0) dot_light = 0;

                r *= dot_light;
                g *= dot_light;
                b *= dot_light;

                color[0] = r;
                color[1] = g;
                color[2] = b;
                color[3] = a;

                glColor4ubv(color);

                if (wireframe)
                {
                    glBegin(GL_LINE_LOOP);
                    for (e = 0; e < 3; e ++)
                    {
                        idx = T->verts[e];
                        V = &O->verts[idx / ARRAYSIZE][idx % ARRAYSIZE];
                        glVertex3f(V->Tx, V->Ty, V->Tz);
                    }
                    glEnd();
                }
                else
                {
                    glBegin(GL_TRIANGLES);
                    for (e = 0; e < 3; e ++)
                    {
                        idx = T->verts[e];
                        V = &O->verts[idx / ARRAYSIZE][idx % ARRAYSIZE];
                        glVertex3f(V->Tx, V->Ty, V->Tz);
                    }
                    glEnd();
                }
            }
            if (edgedraw) // assuming some objects have no geometry on all levels.
            {
                glDisable(GL_BLEND);
                glEnable(GL_LINE_STIPPLE);
                if (C->objects[o] == currentObject && O->selected)
                {
                    if (Selection_Mode == POLYS)
                    glLineStipple(1, 0xEEEE);
                    else if (Selection_Mode == VERTS)
                    glLineWidth(1);
                    else if (Selection_Mode == EDGES)
                    glLineWidth(1);
                    else
                    glLineWidth(2);
                    if (O->deforms)
                    line_color = &current_deform_color[0];
                    else
                    line_color = &current_color[0];
                }
                else if (O->selected)
                {
                    if (Selection_Mode == POLYS)
                    glLineStipple(1, 0xEEEE);
                    else if (Selection_Mode == VERTS)
                    glLineWidth(1);
                    else
                    glLineWidth(1);
                    if (O->deforms)
                    line_color = &selected_deform_color[0];
                    else
                    line_color = &selected_color[0];
                }
                else
                {
                    glLineStipple(1, 0xFFFF);
                    glLineWidth(1);
                    if (O->deforms)
                    line_color = &deform_color[0];
                    else
                    line_color = &wire_color[0];
                }
                if (Selection_Mode != OBJES)
                {
                    memcpy(Line_Color, line_color, sizeof Line_Color);
                    for (e = 0; e < O->edgecount; e++)
                    {
                        E = &O->edges[e / ARRAYSIZE][e % ARRAYSIZE];
                        if (E->selected)
                        {
                            line_color = &selected_edge_color[0];
                        }
                        else if (!glIsEnabled(GL_LIGHTING))
                        {
                            line_color = Line_Color;
                        }
                        draw_Edge(O, E, C, wireframe, E->B.Tradius);
                    }
                }
                else
                {
                    if (C->objects[o] == currentObject || O->selected)
                    {
                        render_Outline_polys(O, C);
                    }
                    else
                    {
                        for (e = 0; e < O->edgecount; e++)
                        {
                            E = &O->edges[e / ARRAYSIZE][e % ARRAYSIZE];
                            draw_Edge(O, E, C, wireframe, E->B.Tradius);
                        }
                    }
                }
                glEnable(GL_BLEND);
                glLineWidth(1);
                glDisable(GL_LINE_STIPPLE);
            }
            if (vertdraw)
            {
                if (C->objects[o] == currentObject)
                glPointSize(3);
                else
                glPointSize(2);
                glBegin(GL_POINTS);
                glColor4ubv(vert_color);

                float factor;
                float dot;
                float view_minor;

                aim Aim;
                normal Normal;

                if (C->view_minor > 1)
                    view_minor = C->view_minor;
                else
                    view_minor = 1;

                for (v = 0; v < O->vertcount; v ++)
                {
                    V = &O->verts[v / ARRAYSIZE][v % ARRAYSIZE];

                    Normal.x = -V->N.Tx;
                    Normal.y = -V->N.Ty;
                    Normal.z = -V->N.Tz;

                    Aim = vector3d_T(V, C->T->pos);

                    if (C->ortho)
                    {
                        dot = dot_productN(&Normal, C->T->aim);
                    }
                    else
                    {
                        dot = dot_productN(&Normal, Aim.vec);
                    }

                    if (dot < 0)
                        continue;

                    if (C->ortho)
                    {
                        factor = Aim.dist * view_minor * 0.002 * dot;
                        glVertex3f(V->Tx + V->N.Tx * factor, V->Ty + V->N.Ty * factor, V->Tz + V->N.Tz * factor);
                    }
                    else
                    {
                        factor = Aim.dist * view_minor * 0.3 * O->mean_Edge * dot;
                        if (factor > Aim.dist / 2)
                        {
                            factor = Aim.dist / 2;
                        }
                        glVertex3f(V->Tx - Aim.vec[0] * factor, V->Ty - Aim.vec[1] * factor, V->Tz - Aim.vec[2] * factor);
                    }
                }
                glEnd();
            }
        }
        else
        {
            for (t = 0; t < O->tripcount_[L]; t++)
            {
                T = &O->trips_[L][t / ARRAYSIZE][t % ARRAYSIZE];
                polynormal.x = -T->N.Tx;
                polynormal.y = -T->N.Ty;
                polynormal.z = -T->N.Tz;

                idx = T->surface;
                Material = Materials[idx];

                texture = Surf_Text[Material.texture];

                if (Material.use_texture && texture != NULL)
                {
                    x_ = abs((int)(texture->w * T->B2.u)) % texture->w;
                    y_ = abs((int)(texture->h * T->B2.v)) % texture->h;
                    pix = get_pixel32(texture, x_, y_);
                    SDL_GetRGBA(pix, texture->format, &r, &g, &b, &a);
                }
                else if (T->selected)
                {
                    r = 0;
                    g = 0;
                    b = 255;
                    a = 255;
                }
                else
                {
                    r = Material.RGBA.R;
                    g = Material.RGBA.G;
                    b = Material.RGBA.B;
                    a = Material.RGBA.A;
                }

                dot_light = dot_productN(&polynormal, light_vec);

                if (dot_light < 0) dot_light = 0;

                r *= dot_light;
                g *= dot_light;
                b *= dot_light;

                color[0] = r;
                color[1] = g;
                color[2] = b;
                color[3] = a;

                glColor4ubv(color);

                if (wireframe)
                {
                    glBegin(GL_LINE_LOOP);
                    for (e = 0; e < 3; e ++)
                    {
                        idx = T->verts[e];
                        V = &O->verts_[L][idx / ARRAYSIZE][idx % ARRAYSIZE];
                        glVertex3f(V->Tx, V->Ty, V->Tz);
                    }
                    glEnd();
                }
                else
                {
                    glBegin(GL_TRIANGLES);
                    for (e = 0; e < 3; e ++)
                    {
                        idx = T->verts[e];
                        V = &O->verts_[L][idx / ARRAYSIZE][idx % ARRAYSIZE];
                        glVertex3f(V->Tx, V->Ty, V->Tz);
                    }
                    glEnd();
                }
            }

            if (edgedraw && O->subdlevel >= 0) // assuming some objects have no geometry on all levels.
            {
                glDisable(GL_BLEND);
                glEnable(GL_LINE_STIPPLE);
                if (C->objects[o] == currentObject && O->selected)
                {
                    if (Selection_Mode == POLYS)
                    glLineStipple(1, 0xEEEE);
                    else if (Selection_Mode == VERTS)
                    glLineWidth(1);
                    else if (Selection_Mode == EDGES)
                    glLineWidth(1);
                    else
                    glLineWidth(2);
                    if (O->deforms)
                    line_color = &current_deform_color[0];
                    else
                    line_color = &current_color[0];
                }
                else if (O->selected)
                {
                    if (Selection_Mode == POLYS)
                    glLineStipple(1, 0xEEEE);
                    else if (Selection_Mode == VERTS)
                    glLineWidth(1);
                    else
                    glLineWidth(1);
                    if (O->deforms)
                    line_color = &selected_deform_color[0];
                    else
                    line_color = &selected_color[0];
                }
                else
                {
                    glLineStipple(1, 0xFFFF);
                    glLineWidth(1);
                    if (O->deforms)
                    line_color = &deform_color[0];
                    else
                    line_color = &wire_color[0];
                }
                if (Selection_Mode != OBJES)
                {
                    memcpy(Line_Color, line_color, sizeof Line_Color);
                    for (e = 0; e < O->edgecount; e++)
                    {
                        E = &O->edges[e / ARRAYSIZE][e % ARRAYSIZE];
                        if (E->selected)
                        {
                            line_color = &selected_edge_color[0];
                        }
                        else if (!glIsEnabled(GL_LIGHTING))
                        {
                            line_color = Line_Color;
                        }
                        idx = E->edges[0];
                        E0 = &O->edges_[0][idx / ARRAYSIZE][idx % ARRAYSIZE];
                        draw_Edge_recursive(O, E0, 0, L, C, wireframe, E->B.Tradius);
                        idx = E->edges[1];
                        E0 = &O->edges_[0][idx / ARRAYSIZE][idx % ARRAYSIZE];
                        draw_Edge_recursive(O, E0, 0, L, C, wireframe, E->B.Tradius);
                    }
                }
                else
                {
                    if (C->objects[o] == currentObject || O->selected)
                    {
                        render_Outline_quads(O, C, L);
                    }
                    else
                    {
                        for (e = 0; e < O->edgecount; e++)
                        {
                            E = &O->edges[e / ARRAYSIZE][e % ARRAYSIZE];
                            idx = E->edges[0];
                            E0 = &O->edges_[0][idx / ARRAYSIZE][idx % ARRAYSIZE];
                            draw_Edge_recursive(O, E0, 0, L, C, wireframe, E->B.Tradius);
                            idx = E->edges[1];
                            E0 = &O->edges_[0][idx / ARRAYSIZE][idx % ARRAYSIZE];
                            draw_Edge_recursive(O, E0, 0, L, C, wireframe, E->B.Tradius);
                        }
                    }
                }
                glEnable(GL_BLEND);
                glDisable(GL_LINE_STIPPLE);
                glLineWidth(1);
            }
            if (vertdraw)
            {
                if (C->objects[o] == currentObject)
                glPointSize(3);
                else
                glPointSize(2);
                glBegin(GL_POINTS);
                glColor4ubv(vert_color);

                float factor;
                float dot;
                float view_minor;

                aim Aim;
                normal Normal;

                if (C->view_minor > 1)
                    view_minor = C->view_minor;
                else
                    view_minor = 1;

                for (v = 0; v < O->vertcount; v ++)
                {
                    V = &O->verts[v / ARRAYSIZE][v % ARRAYSIZE];

                    Normal.x = -V->N.Tx;
                    Normal.y = -V->N.Ty;
                    Normal.z = -V->N.Tz;

                    Aim = vector3d_T(V, C->T->pos);

                    if (C->ortho)
                    {
                        dot = dot_productN(&Normal, C->T->aim);
                    }
                    else
                    {
                        dot = dot_productN(&Normal, Aim.vec);
                    }

                    if (dot < 0)
                        continue;

                    if (C->ortho)
                    {
                        factor = Aim.dist * view_minor * 0.002 * dot;
                        glVertex3f(V->Tx + V->N.Tx * factor, V->Ty + V->N.Ty * factor, V->Tz + V->N.Tz * factor);
                    }
                    else
                    {
                        factor = Aim.dist * view_minor * 0.3 * O->mean_Edge * dot;
                        if (factor > Aim.dist / 2)
                        {
                            factor = Aim.dist / 2;
                        }
                        glVertex3f(V->Tx - Aim.vec[0] * factor, V->Ty - Aim.vec[1] * factor, V->Tz - Aim.vec[2] * factor);
                    }
                }
                glEnd();
            }
        }
    }
    glEnable(GL_LIGHTING);
}

void render_trips_OnScreen(camera * C, int wireframe, int edgedraw, int vertdraw, int currentObject, int Selection_Mode)
{
    int v, o, t, e, idx, x_, y_;
    object * O;
    triangle * T;
    vertex * V;
    edge * E;

    normal polynormal;

    surface_Material Material;

    float dot_light;

    Uint32 pix;

    static GLubyte color[4];

    Uint8 r, g, b, a;

    glDisable(GL_LIGHTING);
    glBindTexture(GL_TEXTURE_2D, 0);

    for (o = 0; o < C->object_count; o++)
    {
        idx = C->objects[o];
        O = objects[idx];

        for (t = 0; t < O->tripcount; t++)
        {
            T = &O->trips[t / ARRAYSIZE][t % ARRAYSIZE];
            polynormal.x = -T->N.Tx;
            polynormal.y = -T->N.Ty;
            polynormal.z = -T->N.Tz;

            idx = T->surface;
            Material = Materials[idx];

            texture = Surf_Text[Material.texture];

            if (Material.use_texture && texture != NULL)
            {
                x_ = abs((int)(texture->w * T->B2.u)) % texture->w;
                y_ = abs((int)(texture->h * T->B2.v)) % texture->h;
                pix = get_pixel32(texture, x_, y_);
                SDL_GetRGBA(pix, texture->format, &r, &g, &b, &a);
            }
            else if (T->selected)
            {
                r = 0;
                g = 0;
                b = 255;
                a = 255;
            }
            else
            {
                r = Material.RGBA.R;
                g = Material.RGBA.G;
                b = Material.RGBA.B;
                a = Material.RGBA.A;
            }

            dot_light = dot_productN(&polynormal, light_vec);

            if (dot_light < 0) dot_light = 0;

            r *= dot_light;
            g *= dot_light;
            b *= dot_light;

            color[0] = r;
            color[1] = g;
            color[2] = b;
            color[3] = a;

            glColor4ubv(color);

            if (wireframe)
            {
                glBegin(GL_LINE_LOOP);
                for (e = 0; e < 3; e ++)
                {
                    idx = T->verts[e];
                    V = &O->verts[idx / ARRAYSIZE][idx % ARRAYSIZE];
                    glVertex3f(V->Tx, V->Ty, V->Tz);
                }
                glEnd();
            }
            else
            {
                glBegin(GL_TRIANGLES);
                for (e = 0; e < 3; e ++)
                {
                    idx = T->verts[e];
                    V = &O->verts[idx / ARRAYSIZE][idx % ARRAYSIZE];
                    glVertex3f(V->Tx, V->Ty, V->Tz);
                }
                glEnd();
            }
        }
        if (edgedraw) // assuming some objects have no geometry on all levels.
        {
            glDisable(GL_BLEND);
            glEnable(GL_LINE_STIPPLE);
            if (C->objects[o] == currentObject && O->selected)
            {
                if (Selection_Mode == POLYS)
                glLineStipple(1, 0xEEEE);
                else if (Selection_Mode == VERTS)
                glLineWidth(1);
                else if (Selection_Mode == EDGES)
                glLineWidth(1);
                else
                glLineWidth(2);
                if (O->deforms)
                line_color = &current_deform_color[0];
                else
                line_color = &current_color[0];
            }
            else if (O->selected)
            {
                if (Selection_Mode == POLYS)
                glLineStipple(1, 0xEEEE);
                else if (Selection_Mode == VERTS)
                glLineWidth(1);
                else
                glLineWidth(1);
                if (O->deforms)
                line_color = &selected_deform_color[0];
                else
                line_color = &selected_color[0];
            }
            else
            {
                glLineStipple(1, 0xFFFF);
                glLineWidth(1);
                if (O->deforms)
                line_color = &deform_color[0];
                else
                line_color = &wire_color[0];
            }
            if (Selection_Mode != OBJES)
            {
                memcpy(Line_Color, line_color, sizeof Line_Color);
                for (e = 0; e < O->edgecount; e++)
                {
                    E = &O->edges[e / ARRAYSIZE][e % ARRAYSIZE];
                    if (E->selected)
                    {
                        line_color = &selected_edge_color[0];
                    }
                    else if (!glIsEnabled(GL_LIGHTING))
                    {
                        line_color = Line_Color;
                    }
                    draw_Edge(O, E, C, wireframe, E->B.Tradius);
                }
            }
            else
            {
                if (C->objects[o] == currentObject || O->selected)
                {
                    render_Outline_polys(O, C);
                }
                else
                {
                    for (e = 0; e < O->edgecount; e++)
                    {
                        E = &O->edges[e / ARRAYSIZE][e % ARRAYSIZE];
                        draw_Edge(O, E, C, wireframe, E->B.Tradius);
                    }
                }
            }
            glEnable(GL_BLEND);
            glLineWidth(1);
            glDisable(GL_LINE_STIPPLE);
        }
        if (vertdraw)
        {
            if (C->objects[o] == currentObject)
            glPointSize(3);
            else
            glPointSize(2);
            glBegin(GL_POINTS);
            glColor4ubv(vert_color);

            float factor;
            float dot;
            float view_minor;

            aim Aim;
            normal Normal;

            if (C->view_minor > 1)
                view_minor = C->view_minor;
            else
                view_minor = 1;

            for (v = 0; v < O->vertcount; v ++)
            {
                V = &O->verts[v / ARRAYSIZE][v % ARRAYSIZE];

                Normal.x = -V->N.Tx;
                Normal.y = -V->N.Ty;
                Normal.z = -V->N.Tz;

                Aim = vector3d_T(V, C->T->pos);

                if (C->ortho)
                {
                    dot = dot_productN(&Normal, C->T->aim);
                }
                else
                {
                    dot = dot_productN(&Normal, Aim.vec);
                }

                if (dot < 0)
                    continue;

                if (C->ortho)
                {
                    factor = Aim.dist * view_minor * 0.002 * dot;
                    glVertex3f(V->Tx + V->N.Tx * factor, V->Ty + V->N.Ty * factor, V->Tz + V->N.Tz * factor);
                }
                else
                {
                    factor = Aim.dist * view_minor * 0.3 * O->mean_Edge * dot;
                    if (factor > Aim.dist / 2)
                    {
                        factor = Aim.dist / 2;
                    }
                    glVertex3f(V->Tx - Aim.vec[0] * factor, V->Ty - Aim.vec[1] * factor, V->Tz - Aim.vec[2] * factor);
                }
            }
            glEnd();
        }
    }
    glEnable(GL_LIGHTING);
}

void render_UV_Edges_ID_(camera * C, object * O, int l)
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, C->h_v_ratio, 0, 1, 1, -1);

    int L;
    if (l > O->subdlevel)
    {
        L = O->subdlevel;
    }
    else
    {
        L = l;
    }

    int e, idx, idx0, idx1, idx2;
    uv_edge * UVE, * UVE0;
    uv * UV0, * UV1;

    int M0 = 255 * 255 * 255;
    int M1 = 255 * 255;

    float R, G, B, A;

    glDisable(GL_LIGHTING);
    glDisable(GL_DITHER);
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);
    glDisable(GL_MULTISAMPLE);

    glLineWidth(6);

    glBegin(GL_LINES);

    for (e = 0; e < O->uvedcount; e++)
    {
        UVE = &O->uveds[e / ARRAYSIZE][e % ARRAYSIZE];

        R = (float)(e / M0) / (float)255;
        idx0 = e % M0;
        G = (float)(idx0 / M1) / (float)255;
        idx1 = idx0 % M1;
        B = (float)(idx1 / 255) / (float)255;
        idx2 = idx1 % 255;
        A = (float)(idx2) / (float)255;

        glColor4f(R, G, B, A);

        if (L == -1)
        {
            idx = UVE->uv_verts[0];
            UV0 = &O->uvtex[idx / ARRAYSIZE][idx % ARRAYSIZE];

            idx = UVE->uv_verts[1];
            UV1 = &O->uvtex[idx / ARRAYSIZE][idx % ARRAYSIZE];

            glVertex2f(UV0->u, UV0->v);
            glVertex2f(UV1->u, UV1->v);
        }
        else if (O->subdlevel >= L)
        {
            idx = UVE->uv_edges[0];
            UVE0 = &O->uveds_[0][idx / ARRAYSIZE][idx % ARRAYSIZE];
            draw_UV_Edge_ID_recursive(C, O, UVE0, 0, L);
            idx = UVE->uv_edges[1];
            UVE0 = &O->uveds_[0][idx / ARRAYSIZE][idx % ARRAYSIZE];
            draw_UV_Edge_ID_recursive(C, O, UVE0, 0, L);
        }
    }

    glEnd();

    glLineWidth(1);

    glEnable(GL_MULTISAMPLE);
    glEnable(GL_BLEND);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_DITHER);
    glEnable(GL_LIGHTING);
}

int render_Quads_ID_recursive(object * O, quadrant * Q, int L, int l)
{
    if (L > l) return 0;

    int e, idx;

    if (Q->subdivs == 0 || L == l)
    {
        int e;
        vertex * V;
        for(e = 0; e < 4; e ++)
        {
            idx = Q->verts[e];
            V = &O->verts_[L][idx / ARRAYSIZE][idx % ARRAYSIZE];
            glVertex3f(V->Tx, V->Ty, V->Tz);
        }
        return 1;
    }
    else
    {
        L ++;
        if (L >= SUBD) return 0;

        quadrant * Q0;

        for (e = 0; e < 4; e ++)
        {
            idx = Q->quads[e];
            Q0 = &O->quads_[L][idx / ARRAYSIZE][idx % ARRAYSIZE];
            render_Quads_ID_recursive(O, Q0, L, l);
        }
    }
    return 0;
}

void render_Quads_ID(camera * C, object * O, int l)
{
    int e, p, idx, idx0, idx1, idx2;
    polygon * P;
    quadrant * Q;

    int L;

    if (l > O->subdlevel)
    {
        L = O->subdlevel;
    }
    else
    {
        L = l;
    }

    int M0 = 255 * 255 * 255;
    int M1 = 255 * 255;

    float R, G, B, A;

    glDisable(GL_LIGHTING);
    glDisable(GL_DITHER);
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);
    glDisable(GL_MULTISAMPLE);
    glDisable(GL_CULL_FACE);

    for (p = 0; p < O->polycount; p++)
    {
        P = &O->polys[p / ARRAYSIZE][p % ARRAYSIZE];

        R = (float)(p / M0) / (float)255;
        idx0 = p % M0;
        G = (float)(idx0 / M1) / (float)255;
        idx1 = idx0 % M1;
        B = (float)(idx1 / 255) / (float)255;
        idx2 = idx1 % 255;
        A = (float)(idx2) / (float)255;

        glColor4f(R, G, B, A);

        glBegin(GL_QUADS);
        for (e = 0; e < P->edgecount; e ++)
        {
            idx = P->quads[e];
            Q = &O->quads_[0][idx / ARRAYSIZE][idx % ARRAYSIZE];
            render_Quads_ID_recursive(O, Q, 0, L);
        }
        glEnd();
    }

    glEnable(GL_CULL_FACE);
    glEnable(GL_MULTISAMPLE);
    glEnable(GL_BLEND);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_DITHER);
    glEnable(GL_LIGHTING);
}

void render_Segments_ID_(object * O, int l)
{
    int L;
    if (l > O->subdlevel)
    {
        L = O->subdlevel;
    }
    else
    {
        L = l;
    }

    int c, p, s, q, e, idx, idx0, idx1, idx2;
    quadrant * Q;
    polygon * P;
    vertex * V;
    curve * C;
    curve_segment * S;

    int M0 = 255 * 255 * 255;
    int M1 = 255 * 255;

    float R, G, B, A;

    glDisable(GL_LIGHTING);
    glDisable(GL_DITHER);
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);
    glDisable(GL_MULTISAMPLE);

    if (L == -1)
    {
        for (p = 0; p < O->polycount; p++)
        {
            P = &O->polys[p / ARRAYSIZE][p % ARRAYSIZE];

            glBegin(GL_TRIANGLE_FAN);
            for (e = 0; e < P->edgecount; e ++)
            {
                idx = P->verts[e];
                V = &O->verts[idx / ARRAYSIZE][idx % ARRAYSIZE];
                glVertex3f(V->Tx, V->Ty, V->Tz);
            }
            glEnd();
        }

        glLineWidth(6);
        glColor4f(1, 1, 1, 1);
        glBegin(GL_LINES);

        for (c = 0; c < curvesIndex; c ++)
        {
            C = curves[c];
            if (C->selected)
            {
                for (s = 0; s < C->segment_count; s ++)
                {
                    S = C->segments[s];
                    idx = S->index;

                    R = (float)(idx / M0) / (float)255;
                    idx0 = idx % M0;
                    G = (float)(idx0 / M1) / (float)255;
                    idx1 = idx0 % M1;
                    B = (float)(idx1 / 255) / (float)255;
                    idx2 = idx1 % 255;
                    A = (float)(idx2) / (float)255;

                    glColor4f(R, G, B, A);
                    glVertex3fv(S->A);
                    glVertex3fv(S->C);
                }
            }
        }
    }
    else if (O->subdlevel >= L)
    {
        for (q = 0; q < O->quadcount_[L]; q ++)
        {
            Q = &O->quads_[L][q / ARRAYSIZE][q % ARRAYSIZE];

            glColor4f(1, 1, 1, 1);

            glBegin(GL_QUADS);
            for (e = 0; e < 4; e ++)
            {
                idx = Q->verts[e];
                V = &O->verts_[L][idx / ARRAYSIZE][idx % ARRAYSIZE];
                glVertex3f(V->Tx, V->Ty, V->Tz);
            }
            glEnd();
        }

        glLineWidth(6);

        glBegin(GL_LINES);

        for (c = 0; c < curvesIndex; c ++)
        {
            C = curves[c];

            for (s = 0; s < C->segment_count; s ++)
            {
                S = C->segments[s];

                idx = S->index;

                R = (float)(idx / M0) / (float)255;
                idx0 = idx % M0;
                G = (float)(idx0 / M1) / (float)255;
                idx1 = idx0 % M1;
                B = (float)(idx1 / 255) / (float)255;
                idx2 = idx1 % 255;
                A = (float)(idx2) / (float)255;

                glColor4f(R, G, B, A);

                if (S->subdivided)
                {
                    draw_Segment_ID_recursive(S->segment[0], 0, L);
                    draw_Segment_ID_recursive(S->segment[1], 0, L);
                }
            }
        }

        glEnd();
    }

    glLineWidth(1);

    glEnable(GL_MULTISAMPLE);
    glEnable(GL_BLEND);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_DITHER);
    glEnable(GL_LIGHTING);
}

void render_poly_quads_Edges_ID(camera * C, object * O, int l)
{
    int L;
    if (l > O->subdlevel)
    {
        L = O->subdlevel;
    }
    else
    {
        L = l;
    }
    int p, q, e, idx, idx0, idx1, idx2;
    quadrant * Q;
    polygon * P;
    edge * E, * E0;
    vertex * V, * V0, * V1;

    int M0 = 255 * 255 * 255;
    int M1 = 255 * 255;

    float R, G, B, A;

    glDisable(GL_LIGHTING);
    glDisable(GL_DITHER);
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);
    glDisable(GL_MULTISAMPLE);

    if (L == -1)
    {
        for (p = 0; p < O->polycount; p++)
        {
            P = &O->polys[p / ARRAYSIZE][p % ARRAYSIZE];

            glColor4f(1, 1, 1, 1);

            glBegin(GL_TRIANGLE_FAN);
            for (e = 0; e < P->edgecount; e ++)
            {
                idx = P->verts[e];
                V = &O->verts[idx / ARRAYSIZE][idx % ARRAYSIZE];
                glVertex3f(V->Tx, V->Ty, V->Tz);
            }
            glEnd();
        }

        glLineWidth(6);

        glBegin(GL_LINES);

        float factor;
        float dot;
        float view_minor;

        aim Aim;
        normal Normal;

        if (C->view_minor > 1)
            view_minor = C->view_minor;
        else
            view_minor = 1;

        for (e = 0; e < O->edgecount; e++)
        {
            E = &O->edges[e / ARRAYSIZE][e % ARRAYSIZE];

            Normal.x = -E->N.Tx;
            Normal.y = -E->N.Ty;
            Normal.z = -E->N.Tz;

            Aim = vector3d(E->B, C->T->pos);

            if (C->ortho)
            {
                dot = dot_productN(&Normal, C->T->aim);
            }
            else
            {
                dot = dot_productN(&Normal, Aim.vec);
            }

            if (dot < 0)
                continue;

            idx = E->verts[0];
            V0 = &O->verts[idx / ARRAYSIZE][idx % ARRAYSIZE];

            idx = E->verts[1];
            V1 = &O->verts[idx / ARRAYSIZE][idx % ARRAYSIZE];

            R = (float)(e / M0) / (float)255;
            idx0 = e % M0;
            G = (float)(idx0 / M1) / (float)255;
            idx1 = idx0 % M1;
            B = (float)(idx1 / 255) / (float)255;
            idx2 = idx1 % 255;
            A = (float)(idx2) / (float)255;

            glColor4f(R, G, B, A);

            Normal.x = -V0->N.Tx;
            Normal.y = -V0->N.Ty;
            Normal.z = -V0->N.Tz;

            Aim = vector3d_T(V0, C->T->pos);

            if (C->ortho)
            {
                dot = dot_productN(&Normal, C->T->aim);
            }
            else
            {
                dot = dot_productN(&Normal, Aim.vec);
            }

            if (C->ortho)
            {
                factor = Aim.dist * view_minor * 0.002 * dot;
                glVertex3f(V0->Tx + V0->N.Tx * factor, V0->Ty + V0->N.Ty * factor, V0->Tz + V0->N.Tz * factor);
            }
            else
            {
                factor = Aim.dist * view_minor * 0.3 * O->mean_Edge * dot;
                if (factor > Aim.dist / 2)
                {
                    factor = Aim.dist / 2;
                }
                glVertex3f(V0->Tx - Aim.vec[0] * factor, V0->Ty - Aim.vec[1] * factor, V0->Tz - Aim.vec[2] * factor);
            }

            Normal.x = -V1->N.Tx;
            Normal.y = -V1->N.Ty;
            Normal.z = -V1->N.Tz;

            Aim = vector3d_T(V1, C->T->pos);

            if (C->ortho)
            {
                dot = dot_productN(&Normal, C->T->aim);
            }
            else
            {
                dot = dot_productN(&Normal, Aim.vec);
            }

            if (C->ortho)
            {
                factor = Aim.dist * view_minor * 0.002 * dot;
                glVertex3f(V1->Tx + V1->N.Tx * factor, V1->Ty + V1->N.Ty * factor, V1->Tz + V1->N.Tz * factor);
            }
            else
            {
                factor = Aim.dist * view_minor * 0.3 * O->mean_Edge * dot;
                if (factor > Aim.dist / 2)
                {
                    factor = Aim.dist / 2;
                }
                glVertex3f(V1->Tx - Aim.vec[0] * factor, V1->Ty - Aim.vec[1] * factor, V1->Tz - Aim.vec[2] * factor);
            }
        }

        glEnd();
    }
    else if (O->subdlevel >= L)
    {
        for (q = 0; q < O->quadcount_[L]; q ++)
        {
            Q = &O->quads_[L][q / ARRAYSIZE][q % ARRAYSIZE];

            glColor4f(1, 1, 1, 1);

            glBegin(GL_QUADS);
            for (e = 0; e < 4; e ++)
            {
                idx = Q->verts[e];
                V = &O->verts_[L][idx / ARRAYSIZE][idx % ARRAYSIZE];
                glVertex3f(V->Tx, V->Ty, V->Tz);
            }
            glEnd();
        }

        glLineWidth(6);

        glBegin(GL_LINES);

        for (e = 0; e < O->edgecount; e++)
        {
            E = &O->edges[e / ARRAYSIZE][e % ARRAYSIZE];

            R = (float)(e / M0) / (float)255;
            idx0 = e % M0;
            G = (float)(idx0 / M1) / (float)255;
            idx1 = idx0 % M1;
            B = (float)(idx1 / 255) / (float)255;
            idx2 = idx1 % 255;
            A = (float)(idx2) / (float)255;

            glColor4f(R, G, B, A);

            idx = E->edges[0];
            E0 = &O->edges_[0][idx / ARRAYSIZE][idx % ARRAYSIZE];
            draw_Edge_ID_recursive(C, O, E0, 0, L);
            idx = E->edges[1];
            E0 = &O->edges_[0][idx / ARRAYSIZE][idx % ARRAYSIZE];
            draw_Edge_ID_recursive(C, O, E0, 0, L);
        }

        glEnd();
    }

    glLineWidth(1);

    glEnable(GL_MULTISAMPLE);
    glEnable(GL_BLEND);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_DITHER);
    glEnable(GL_LIGHTING);
}

void render_UV_Verts_ID_(camera * C, object * O, int l)
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, C->h_v_ratio, 0, 1, 1, -1);

    int v, idx0, idx1, idx2;
    uv * UV;

    int M0 = 255 * 255 * 255;
    int M1 = 255 * 255;

    float R, G, B, A;

    glDisable(GL_LIGHTING);
    glDisable(GL_DITHER);
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);
    glDisable(GL_MULTISAMPLE);

    int L, l0;

    if (l > O->subdlevel)
    {
        L = O->subdlevel;
    }
    else
    {
        L = l;
    }

    glPointSize(6);
    glBegin(GL_POINTS);

    for (v = 0; v < O->textcount; v ++)
    {
        UV = &O->uvtex[v / ARRAYSIZE][v % ARRAYSIZE];

        R = (float)(v / M0) / (float)255;
        idx0 = v % M0;
        G = (float)(idx0 / M1) / (float)255;
        idx1 = idx0 % M1;
        B = (float)(idx1 / 255) / (float)255;
        idx2 = idx1 % 255;
        A = (float)(idx2) / (float)255;

        glColor4f(R, G, B, A);

        l0 = 0;

        while (UV != NULL && l0 <= L)
        {
            UV = UV->uv_vert;
            l0 ++;
        }

        glVertex2f(UV->u, UV->v);
    }

    glEnd();

    glEnable(GL_MULTISAMPLE);
    glEnable(GL_BLEND);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_DITHER);
    glEnable(GL_LIGHTING);
}

void render_poly_quad_Verts_ID(camera * C, object * O, int l)
{
    int level, e, p, q, v, idx, idx0, idx1, idx2;
    quadrant * Q;
    polygon * P;
    vertex * V;

    int M0 = 255 * 255 * 255;
    int M1 = 255 * 255;

    float R, G, B, A;

    glDisable(GL_LIGHTING);
    glDisable(GL_DITHER);
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);
    glDisable(GL_MULTISAMPLE);

    float factor;
    float dot;
    float view_minor;

    aim Aim;
    normal Normal;

    int L;

    if (l > O->subdlevel)
    {
        L = O->subdlevel;
    }
    else
    {
        L = l;
    }

    glColor4f(1, 1, 1, 1);

    if (L == -1)
    {
        for (p = 0; p < O->polycount; p++)
        {
            P = &O->polys[p / ARRAYSIZE][p % ARRAYSIZE];

            glBegin(GL_TRIANGLE_FAN);
            for (e = 0; e < P->edgecount; e ++)
            {
                idx = P->verts[e];
                V = &O->verts[idx / ARRAYSIZE][idx % ARRAYSIZE];
                glVertex3f(V->Tx, V->Ty, V->Tz);
            }
            glEnd();
        }

        glPointSize(6);
        glBegin(GL_POINTS);

        if (C->view_minor > 1)
            view_minor = C->view_minor;
        else
            view_minor = 1;

        for (v = 0; v < O->vertcount; v ++)
        {
            V = &O->verts[v / ARRAYSIZE][v % ARRAYSIZE];

            R = (float)(v / M0) / (float)255;
            idx0 = v % M0;
            G = (float)(idx0 / M1) / (float)255;
            idx1 = idx0 % M1;
            B = (float)(idx1 / 255) / (float)255;
            idx2 = idx1 % 255;
            A = (float)(idx2) / (float)255;

            glColor4f(R, G, B, A);

            Normal.x = -V->N.Tx;
            Normal.y = -V->N.Ty;
            Normal.z = -V->N.Tz;

            Aim = vector3d_T(V, C->T->pos);

            if (C->ortho)
            {
                dot = dot_productN(&Normal, C->T->aim);
            }
            else
            {
                dot = dot_productN(&Normal, Aim.vec);
            }

            if (dot < 0)
                continue;

            if (C->ortho)
            {
                factor = Aim.dist * view_minor * 0.002 * dot;
                glVertex3f(V->Tx + V->N.Tx * factor, V->Ty + V->N.Ty * factor, V->Tz + V->N.Tz * factor);
            }
            else
            {
                factor = Aim.dist * view_minor * 0.3 * O->mean_Edge * dot;
                if (factor > Aim.dist / 2)
                {
                    factor = Aim.dist / 2;
                }
                glVertex3f(V->Tx - Aim.vec[0] * factor, V->Ty - Aim.vec[1] * factor, V->Tz - Aim.vec[2] * factor);
            }
        }
        glEnd();
    }
    else
    {
        glBegin(GL_QUADS);

        for (q = 0; q < O->quadcount_[L]; q ++)
        {
            Q = &O->quads_[L][q / ARRAYSIZE][q % ARRAYSIZE];
            for (e = 0; e < 4; e ++)
            {
                idx = Q->verts[e];
                V = &O->verts_[L][idx / ARRAYSIZE][idx % ARRAYSIZE];
                glVertex3f(V->Tx, V->Ty, V->Tz);
            }
        }
        glEnd();

        glPointSize(6);
        glBegin(GL_POINTS);

        if (C->view_minor > 1)
            view_minor = C->view_minor;
        else
            view_minor = 1;

        for (v = 0; v < O->vertcount; v ++)
        {
            V = &O->verts[v / ARRAYSIZE][v % ARRAYSIZE];

            level = -1;

            while (V->vert != NULL && level < L)
            {
                V = V->vert;
                level ++;
            }

            R = (float)(v / M0) / (float)255;
            idx0 = v % M0;
            G = (float)(idx0 / M1) / (float)255;
            idx1 = idx0 % M1;
            B = (float)(idx1 / 255) / (float)255;
            idx2 = idx1 % 255;
            A = (float)(idx2) / (float)255;

            glColor4f(R, G, B, A);

            Normal.x = -V->N.Tx;
            Normal.y = -V->N.Ty;
            Normal.z = -V->N.Tz;

            Aim = vector3d_T(V, C->T->pos);

            if (C->ortho)
            {
                dot = dot_productN(&Normal, C->T->aim);
            }
            else
            {
                dot = dot_productN(&Normal, Aim.vec);
            }

            if (dot < 0)
                continue;

            if (C->ortho)
            {
                factor = Aim.dist * view_minor * 0.002 * dot;
                glVertex3f(V->Tx + V->N.Tx * factor, V->Ty + V->N.Ty * factor, V->Tz + V->N.Tz * factor);
            }
            else
            {
                factor = Aim.dist * view_minor * 0.3 * O->mean_Edge * dot;
                if (factor > Aim.dist / 2)
                {
                    factor = Aim.dist / 2;
                }
                glVertex3f(V->Tx - Aim.vec[0] * factor, V->Ty - Aim.vec[1] * factor, V->Tz - Aim.vec[2] * factor);
            }
        }
        glEnd();
    }

    glEnable(GL_MULTISAMPLE);
    glEnable(GL_BLEND);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_DITHER);
    glEnable(GL_LIGHTING);
}

void render_UV_Verts_ID(camera * C, object * O)
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, C->h_v_ratio, 0, 1, 1, -1);

    int v, idx0, idx1, idx2;
    uv * UV;

    int M0 = 255 * 255 * 255;
    int M1 = 255 * 255;

    float R, G, B, A;

    glDisable(GL_LIGHTING);
    glDisable(GL_DITHER);
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);
    glDisable(GL_MULTISAMPLE);

    glPointSize(6);
    glBegin(GL_POINTS);

    for (v = 0; v < O->textcount; v ++)
    {
        UV = &O->uvtex[v / ARRAYSIZE][v % ARRAYSIZE];

        R = (float)(v / M0) / (float)255;
        idx0 = v % M0;
        G = (float)(idx0 / M1) / (float)255;
        idx1 = idx0 % M1;
        B = (float)(idx1 / 255) / (float)255;
        idx2 = idx1 % 255;
        A = (float)(idx2) / (float)255;

        glColor4f(R, G, B, A);

        glVertex2f(UV->u, UV->v);
    }

    glEnd();

    glEnable(GL_MULTISAMPLE);
    glEnable(GL_BLEND);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_DITHER);
    glEnable(GL_LIGHTING);
}

int render_UV_Quads_ID_recursive(object * O, quadrant * Q, int L, int l)
{
    if (L > l) return 0;

    int e, idx;

    if (Q->subdivs == 0 || L == l)
    {
        int e;
        uv * UV;
        for(e = 0; e < 4; e ++)
        {
            idx = Q->texts[e];
            UV = &O->uvtex_[L][idx / ARRAYSIZE][idx % ARRAYSIZE];
            glVertex2f(UV->u, UV->v);
        }
        return 1;
    }
    else
    {
        L ++;
        if (L >= SUBD) return 0;

        quadrant * Q0;

        for (e = 0; e < 4; e ++)
        {
            idx = Q->quads[e];
            Q0 = &O->quads_[L][idx / ARRAYSIZE][idx % ARRAYSIZE];
            render_UV_Quads_ID_recursive(O, Q0, L, l);
        }
    }
    return 0;
}

void render_UV_Polys_Quads_ID(camera * C, object * O, int l)
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, C->h_v_ratio, 0, 1, 1, -1);

    int e, p, idx, idx0, idx1, idx2;
    polygon * P;
    uv * UV;
    quadrant * Q;

    int L;

    if (l > O->subdlevel)
    {
        L = O->subdlevel;
    }
    else
    {
        L = l;
    }

    int M0 = 255 * 255 * 255;
    int M1 = 255 * 255;

    float R, G, B, A;

    glDisable(GL_LIGHTING);
    glDisable(GL_DITHER);
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);
    glDisable(GL_MULTISAMPLE);
    glDisable(GL_CULL_FACE);

    for (p = 0; p < O->polycount; p++)
    {
        P = &O->polys[p / ARRAYSIZE][p % ARRAYSIZE];

        R = (float)(p / M0) / (float)255;
        idx0 = p % M0;
        G = (float)(idx0 / M1) / (float)255;
        idx1 = idx0 % M1;
        B = (float)(idx1 / 255) / (float)255;
        idx2 = idx1 % 255;
        A = (float)(idx2) / (float)255;

        glColor4f(R, G, B, A);

        if (L == -1)
        {
            glBegin(GL_TRIANGLE_FAN);
            for (e = 0; e < P->edgecount; e ++)
            {
                idx = P->texts[e];
                UV = &O->uvtex[idx / ARRAYSIZE][idx % ARRAYSIZE];
                glVertex2f(UV->u, UV->v);
            }
            glEnd();
        }
        else
        {
            glBegin(GL_QUADS);
            for (e = 0; e < P->edgecount; e ++)
            {
                idx = P->quads[e];
                Q = &O->quads_[0][idx / ARRAYSIZE][idx % ARRAYSIZE];
                render_UV_Quads_ID_recursive(O, Q, 0, L);
            }
            glEnd();
        }
    }

    glEnable(GL_CULL_FACE);
    glEnable(GL_MULTISAMPLE);
    glEnable(GL_BLEND);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_DITHER);
    glEnable(GL_LIGHTING);
}

void render_UV_Polys_ID(camera * C, object * O)
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, C->h_v_ratio, 0, 1, 1, -1);

    int e, p, idx, idx0, idx1, idx2;
    polygon * P;
    uv * UV;

    int M0 = 255 * 255 * 255;
    int M1 = 255 * 255;

    float R, G, B, A;

    glDisable(GL_LIGHTING);
    glDisable(GL_DITHER);
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);
    glDisable(GL_MULTISAMPLE);
    glDisable(GL_CULL_FACE);

    for (p = 0; p < O->polycount; p++)
    {
        P = &O->polys[p / ARRAYSIZE][p % ARRAYSIZE];

        R = (float)(p / M0) / (float)255;
        idx0 = p % M0;
        G = (float)(idx0 / M1) / (float)255;
        idx1 = idx0 % M1;
        B = (float)(idx1 / 255) / (float)255;
        idx2 = idx1 % 255;
        A = (float)(idx2) / (float)255;

        glColor4f(R, G, B, A);

        glBegin(GL_TRIANGLE_FAN);
        for (e = 0; e < P->edgecount; e ++)
        {
            idx = P->texts[e];
            UV = &O->uvtex[idx / ARRAYSIZE][idx % ARRAYSIZE];
            glVertex2f(UV->u, UV->v);
        }
        glEnd();
    }

    glEnable(GL_CULL_FACE);
    glEnable(GL_MULTISAMPLE);
    glEnable(GL_BLEND);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_DITHER);
    glEnable(GL_LIGHTING);
}

void render_poly_Verts_ID(camera * C, object * O)
{
    int e, p, v, idx, idx0, idx1, idx2;
    polygon * P;
    vertex * V;

    int M0 = 255 * 255 * 255;
    int M1 = 255 * 255;

    float R, G, B, A;

    glDisable(GL_LIGHTING);
    glDisable(GL_DITHER);
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);
    glDisable(GL_MULTISAMPLE);

    glColor4f(1, 1, 1, 1);

    float factor;
    float dot;
    float view_minor;

    aim Aim;
    normal Normal;

    for (p = 0; p < O->polycount; p++)
    {
        P = &O->polys[p / ARRAYSIZE][p % ARRAYSIZE];

        glBegin(GL_TRIANGLE_FAN);
        for (e = 0; e < P->edgecount; e ++)
        {
            idx = P->verts[e];
            V = &O->verts[idx / ARRAYSIZE][idx % ARRAYSIZE];
            glVertex3f(V->Tx, V->Ty, V->Tz);
        }
        glEnd();
    }

    glPointSize(6);
    glBegin(GL_POINTS);

    if (C->view_minor > 1)
        view_minor = C->view_minor;
    else
        view_minor = 1;

    for (v = 0; v < O->vertcount; v ++)
    {
        V = &O->verts[v / ARRAYSIZE][v % ARRAYSIZE];

        R = (float)(v / M0) / (float)255;
        idx0 = v % M0;
        G = (float)(idx0 / M1) / (float)255;
        idx1 = idx0 % M1;
        B = (float)(idx1 / 255) / (float)255;
        idx2 = idx1 % 255;
        A = (float)(idx2) / (float)255;

        glColor4f(R, G, B, A);

        Normal.x = -V->N.Tx;
        Normal.y = -V->N.Ty;
        Normal.z = -V->N.Tz;

        Aim = vector3d_T(V, C->T->pos);

        if (C->ortho)
        {
            dot = dot_productN(&Normal, C->T->aim);
        }
        else
        {
            dot = dot_productN(&Normal, Aim.vec);
        }

        if (dot < 0)
            continue;

        if (C->ortho)
        {
            factor = Aim.dist * view_minor * 0.002 * dot;
            glVertex3f(V->Tx + V->N.Tx * factor, V->Ty + V->N.Ty * factor, V->Tz + V->N.Tz * factor);
        }
        else
        {
            factor = Aim.dist * view_minor * 0.3 * O->mean_Edge * dot;
            if (factor > Aim.dist / 2)
            {
                factor = Aim.dist / 2;
            }
            glVertex3f(V->Tx - Aim.vec[0] * factor, V->Ty - Aim.vec[1] * factor, V->Tz - Aim.vec[2] * factor);
        }
    }

    glEnd();

    glEnable(GL_MULTISAMPLE);
    glEnable(GL_BLEND);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_DITHER);
    glEnable(GL_LIGHTING);
}

void render_UV_Edges_ID(camera * C, object * O)
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, C->h_v_ratio, 0, 1, 1, -1);

    int e, idx, idx0, idx1, idx2;
    uv_edge * UVE;
    uv * UV0, * UV1;

    int M0 = 255 * 255 * 255;
    int M1 = 255 * 255;

    float R, G, B, A;

    glDisable(GL_LIGHTING);
    glDisable(GL_DITHER);
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);
    glDisable(GL_MULTISAMPLE);

    glLineWidth(6);

    glBegin(GL_LINES);

    for (e = 0; e < O->uvedcount; e++)
    {
        UVE = &O->uveds[e / ARRAYSIZE][e % ARRAYSIZE];

        idx = UVE->uv_verts[0];
        UV0 = &O->uvtex[idx / ARRAYSIZE][idx % ARRAYSIZE];

        idx = UVE->uv_verts[1];
        UV1 = &O->uvtex[idx / ARRAYSIZE][idx % ARRAYSIZE];

        R = (float)(e / M0) / (float)255;
        idx0 = e % M0;
        G = (float)(idx0 / M1) / (float)255;
        idx1 = idx0 % M1;
        B = (float)(idx1 / 255) / (float)255;
        idx2 = idx1 % 255;
        A = (float)(idx2) / (float)255;

        glColor4f(R, G, B, A);

        glVertex2f(UV0->u, UV0->v);
        glVertex2f(UV1->u, UV1->v);
    }

    glEnd();

    glLineWidth(1);

    glEnable(GL_MULTISAMPLE);
    glEnable(GL_BLEND);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_DITHER);
    glEnable(GL_LIGHTING);
}

void render_poly_ID(camera * C, object * O)
{
    int p, e, idx, idx0, idx1, idx2;
    polygon * P;
    vertex * V;

    int M0 = 255 * 255 * 255;
    int M1 = 255 * 255;

    float R, G, B, A;

    glDisable(GL_LIGHTING);
    glDisable(GL_DITHER);
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);
    glDisable(GL_MULTISAMPLE);

    for (p = 0; p < O->polycount; p++)
    {
        P = &O->polys[p / ARRAYSIZE][p % ARRAYSIZE];

        R = (float)(p / M0) / (float)255;
        idx0 = p % M0;
        G = (float)(idx0 / M1) / (float)255;
        idx1 = idx0 % M1;
        B = (float)(idx1 / 255) / (float)255;
        idx2 = idx1 % 255;
        A = (float)(idx2) / (float)255;

        glColor4f(R, G, B, A);

        glBegin(GL_TRIANGLE_FAN);
        for (e = 0; e < P->edgecount; e ++)
        {
            idx = P->verts[e];
            V = &O->verts[idx / ARRAYSIZE][idx % ARRAYSIZE];
            glVertex3f(V->Tx, V->Ty, V->Tz);
        }
        glEnd();
    }

    glEnable(GL_MULTISAMPLE);
    glEnable(GL_BLEND);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_DITHER);
    glEnable(GL_LIGHTING);
}

void render_poly_Edges_ID(camera * C, object * O)
{
    int p, e, idx, idx0, idx1, idx2;
    polygon * P;
    edge * E;
    vertex * V, * V0, * V1;

    int M0 = 255 * 255 * 255;
    int M1 = 255 * 255;

    float R, G, B, A;

    glDisable(GL_LIGHTING);
    glDisable(GL_DITHER);
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);
    glDisable(GL_MULTISAMPLE);

    for (p = 0; p < O->polycount; p++)
    {
        P = &O->polys[p / ARRAYSIZE][p % ARRAYSIZE];

        glColor4f(1, 1, 1, 1);

        glBegin(GL_TRIANGLE_FAN);
        for (e = 0; e < P->edgecount; e ++)
        {
            idx = P->verts[e];
            V = &O->verts[idx / ARRAYSIZE][idx % ARRAYSIZE];
            glVertex3f(V->Tx, V->Ty, V->Tz);
        }
        glEnd();
    }

    glLineWidth(6);

    glBegin(GL_LINES);

    float factor;
    float dot;
    float view_minor;

    aim Aim;
    normal Normal;

    if (C->view_minor > 1)
        view_minor = C->view_minor;
    else
        view_minor = 1;

    for (e = 0; e < O->edgecount; e++)
    {
        E = &O->edges[e / ARRAYSIZE][e % ARRAYSIZE];

        Normal.x = -E->N.Tx;
        Normal.y = -E->N.Ty;
        Normal.z = -E->N.Tz;

        Aim = vector3d(E->B, C->T->pos);

        if (C->ortho)
        {
            dot = dot_productN(&Normal, C->T->aim);
        }
        else
        {
            dot = dot_productN(&Normal, Aim.vec);
        }

        if (dot < 0)
            continue;

        idx = E->verts[0];
        V0 = &O->verts[idx / ARRAYSIZE][idx % ARRAYSIZE];

        idx = E->verts[1];
        V1 = &O->verts[idx / ARRAYSIZE][idx % ARRAYSIZE];

        R = (float)(e / M0) / (float)255;
        idx0 = e % M0;
        G = (float)(idx0 / M1) / (float)255;
        idx1 = idx0 % M1;
        B = (float)(idx1 / 255) / (float)255;
        idx2 = idx1 % 255;
        A = (float)(idx2) / (float)255;

        glColor4f(R, G, B, A);

        Normal.x = -V0->N.Tx;
        Normal.y = -V0->N.Ty;
        Normal.z = -V0->N.Tz;

        Aim = vector3d_T(V0, C->T->pos);

        if (C->ortho)
        {
            dot = dot_productN(&Normal, C->T->aim);
        }
        else
        {
            dot = dot_productN(&Normal, Aim.vec);
        }

        if (C->ortho)
        {
            factor = Aim.dist * view_minor * 0.002 * dot;
            glVertex3f(V0->Tx + V0->N.Tx * factor, V0->Ty + V0->N.Ty * factor, V0->Tz + V0->N.Tz * factor);
        }
        else
        {
            factor = Aim.dist * view_minor * 0.3 * O->mean_Edge * dot;
            if (factor > Aim.dist / 2)
            {
                factor = Aim.dist / 2;
            }
            glVertex3f(V0->Tx - Aim.vec[0] * factor, V0->Ty - Aim.vec[1] * factor, V0->Tz - Aim.vec[2] * factor);
        }

        Normal.x = -V1->N.Tx;
        Normal.y = -V1->N.Ty;
        Normal.z = -V1->N.Tz;

        Aim = vector3d_T(V1, C->T->pos);

        if (C->ortho)
        {
            dot = dot_productN(&Normal, C->T->aim);
        }
        else
        {
            dot = dot_productN(&Normal, Aim.vec);
        }

        if (C->ortho)
        {
            factor = Aim.dist * view_minor * 0.002 * dot;
            glVertex3f(V1->Tx + V1->N.Tx * factor, V1->Ty + V1->N.Ty * factor, V1->Tz + V1->N.Tz * factor);
        }
        else
        {
            factor = Aim.dist * view_minor * 0.3 * O->mean_Edge * dot;
            if (factor > Aim.dist / 2)
            {
                factor = Aim.dist / 2;
            }
            glVertex3f(V1->Tx - Aim.vec[0] * factor, V1->Ty - Aim.vec[1] * factor, V1->Tz - Aim.vec[2] * factor);
        }
    }

    glEnd();

    glLineWidth(1);

    glEnable(GL_MULTISAMPLE);
    glEnable(GL_BLEND);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_DITHER);
    glEnable(GL_LIGHTING);
}

void draw_Rhombic(transformer * T, float rotVec_[3][3])
{
    float tip[6][3];
    float tip1[6][3];
    float d[3];
    d[0] = 1;
    d[1] = sqrt(2);
    d[2] = 1;
    tip1[0][0] = -d[0]; tip1[0][1] = 0;     tip1[0][2] =  d[2];
    tip1[1][0] =  d[0]; tip1[1][1] = 0;     tip1[1][2] =  d[2];
    tip1[2][0] =  d[0]; tip1[2][1] = 0;     tip1[2][2] = -d[2];
    tip1[3][0] = -d[0]; tip1[3][1] = 0;     tip1[3][2] = -d[2];
    tip1[4][0] = 0;     tip1[4][1] =  d[1]; tip1[4][2] = 0;
    tip1[5][0] = 0;     tip1[5][1] = -d[1]; tip1[5][2] = 0;

    rotate_vector(rotVec_, tip1[0], tip[0]);
    rotate_vector(rotVec_, tip1[1], tip[1]);
    rotate_vector(rotVec_, tip1[2], tip[2]);
    rotate_vector(rotVec_, tip1[3], tip[3]);
    rotate_vector(rotVec_, tip1[4], tip[4]);
    rotate_vector(rotVec_, tip1[5], tip[5]);

    tip[0][0] += T->pos[0]; tip[0][1] += T->pos[1]; tip[0][2] += T->pos[2];
    tip[1][0] += T->pos[0]; tip[1][1] += T->pos[1]; tip[1][2] += T->pos[2];
    tip[2][0] += T->pos[0]; tip[2][1] += T->pos[1]; tip[2][2] += T->pos[2];
    tip[3][0] += T->pos[0]; tip[3][1] += T->pos[1]; tip[3][2] += T->pos[2];
    tip[4][0] += T->pos[0]; tip[4][1] += T->pos[1]; tip[4][2] += T->pos[2];
    tip[5][0] += T->pos[0]; tip[5][1] += T->pos[1]; tip[5][2] += T->pos[2];

    glVertex3f(tip[0][0], tip[0][1], tip[0][2]);
    glVertex3f(tip[1][0], tip[1][1], tip[1][2]);

    glVertex3f(tip[1][0], tip[1][1], tip[1][2]);
    glVertex3f(tip[2][0], tip[2][1], tip[2][2]);

    glVertex3f(tip[2][0], tip[2][1], tip[2][2]);
    glVertex3f(tip[3][0], tip[3][1], tip[3][2]);

    glVertex3f(tip[3][0], tip[3][1], tip[3][2]);
    glVertex3f(tip[0][0], tip[0][1], tip[0][2]);

    //

    glVertex3f(tip[0][0], tip[0][1], tip[0][2]);
    glVertex3f(tip[4][0], tip[4][1], tip[4][2]);

    glVertex3f(tip[1][0], tip[1][1], tip[1][2]);
    glVertex3f(tip[4][0], tip[4][1], tip[4][2]);

    glVertex3f(tip[2][0], tip[2][1], tip[2][2]);
    glVertex3f(tip[4][0], tip[4][1], tip[4][2]);

    glVertex3f(tip[3][0], tip[3][1], tip[3][2]);
    glVertex3f(tip[4][0], tip[4][1], tip[4][2]);

    //

    glVertex3f(tip[0][0], tip[0][1], tip[0][2]);
    glVertex3f(tip[5][0], tip[5][1], tip[5][2]);

    glVertex3f(tip[1][0], tip[1][1], tip[1][2]);
    glVertex3f(tip[5][0], tip[5][1], tip[5][2]);

    glVertex3f(tip[2][0], tip[2][1], tip[2][2]);
    glVertex3f(tip[5][0], tip[5][1], tip[5][2]);

    glVertex3f(tip[3][0], tip[3][1], tip[3][2]);
    glVertex3f(tip[5][0], tip[5][1], tip[5][2]);
}

void draw_Cam_Loop(transformer * T, float rotVec_[3][3])
{
    float corner[8][3];
    float corner1[8][3];

    float d_01 = 1.0;
    float d_15 = 1.5;
    float d_05 = 0.5;

    corner1[0][0] = 0; corner1[0][1] = -d_01; corner1[0][2] =  d_01;
    corner1[1][0] = 0; corner1[1][1] = -d_05; corner1[1][2] =  d_01;
    corner1[2][0] = 0; corner1[2][1] = -d_05; corner1[2][2] =  d_15;
    corner1[3][0] = 0; corner1[3][1] =  d_05; corner1[3][2] =  d_15;
    corner1[4][0] = 0; corner1[4][1] =  d_05; corner1[4][2] =  d_01;
    corner1[5][0] = 0; corner1[5][1] =  d_01; corner1[5][2] =  d_01;
    corner1[6][0] = 0; corner1[6][1] =  d_01; corner1[6][2] = -d_01;
    corner1[7][0] = 0; corner1[7][1] = -d_01; corner1[7][2] = -d_01;

    rotate_vector(rotVec_, corner1[0], corner[0]);
    rotate_vector(rotVec_, corner1[1], corner[1]);
    rotate_vector(rotVec_, corner1[2], corner[2]);
    rotate_vector(rotVec_, corner1[3], corner[3]);
    rotate_vector(rotVec_, corner1[4], corner[4]);
    rotate_vector(rotVec_, corner1[5], corner[5]);
    rotate_vector(rotVec_, corner1[6], corner[6]);
    rotate_vector(rotVec_, corner1[7], corner[7]);

    corner[0][0] += T->pos[0]; corner[0][1] += T->pos[1]; corner[0][2] += T->pos[2];
    corner[1][0] += T->pos[0]; corner[1][1] += T->pos[1]; corner[1][2] += T->pos[2];
    corner[2][0] += T->pos[0]; corner[2][1] += T->pos[1]; corner[2][2] += T->pos[2];
    corner[3][0] += T->pos[0]; corner[3][1] += T->pos[1]; corner[3][2] += T->pos[2];
    corner[4][0] += T->pos[0]; corner[4][1] += T->pos[1]; corner[4][2] += T->pos[2];
    corner[5][0] += T->pos[0]; corner[5][1] += T->pos[1]; corner[5][2] += T->pos[2];
    corner[6][0] += T->pos[0]; corner[6][1] += T->pos[1]; corner[6][2] += T->pos[2];
    corner[7][0] += T->pos[0]; corner[7][1] += T->pos[1]; corner[7][2] += T->pos[2];

    glVertex3f(corner[0][0], corner[0][1], corner[0][2]);
    glVertex3f(corner[1][0], corner[1][1], corner[1][2]);

    glVertex3f(corner[2][0], corner[2][1], corner[2][2]);
    glVertex3f(corner[3][0], corner[3][1], corner[3][2]);

    glVertex3f(corner[4][0], corner[4][1], corner[4][2]);
    glVertex3f(corner[5][0], corner[5][1], corner[5][2]);

    glVertex3f(corner[6][0], corner[6][1], corner[6][2]);
    glVertex3f(corner[7][0], corner[7][1], corner[7][2]);
}

void draw_Box(transformer * T, float rotVec_[3][3])
{
    float corner[8][3];
    float corner1[8][3];
    float d[3];
    d[0] = 1;
    d[1] = 1;
    d[2] = 1;
    corner1[0][0] = -d[0]; corner1[0][1] =  d[1]; corner1[0][2] =  d[2];
    corner1[1][0] =  d[0]; corner1[1][1] =  d[1]; corner1[1][2] =  d[2];
    corner1[2][0] =  d[0]; corner1[2][1] =  d[1]; corner1[2][2] = -d[2];
    corner1[3][0] = -d[0]; corner1[3][1] =  d[1]; corner1[3][2] = -d[2];
    corner1[4][0] = -d[0]; corner1[4][1] = -d[1]; corner1[4][2] =  d[2];
    corner1[5][0] =  d[0]; corner1[5][1] = -d[1]; corner1[5][2] =  d[2];
    corner1[6][0] =  d[0]; corner1[6][1] = -d[1]; corner1[6][2] = -d[2];
    corner1[7][0] = -d[0]; corner1[7][1] = -d[1]; corner1[7][2] = -d[2];

    rotate_vector(rotVec_, corner1[0], corner[0]);
    rotate_vector(rotVec_, corner1[1], corner[1]);
    rotate_vector(rotVec_, corner1[2], corner[2]);
    rotate_vector(rotVec_, corner1[3], corner[3]);
    rotate_vector(rotVec_, corner1[4], corner[4]);
    rotate_vector(rotVec_, corner1[5], corner[5]);
    rotate_vector(rotVec_, corner1[6], corner[6]);
    rotate_vector(rotVec_, corner1[7], corner[7]);

    corner[0][0] += T->pos[0]; corner[0][1] += T->pos[1]; corner[0][2] += T->pos[2];
    corner[1][0] += T->pos[0]; corner[1][1] += T->pos[1]; corner[1][2] += T->pos[2];
    corner[2][0] += T->pos[0]; corner[2][1] += T->pos[1]; corner[2][2] += T->pos[2];
    corner[3][0] += T->pos[0]; corner[3][1] += T->pos[1]; corner[3][2] += T->pos[2];
    corner[4][0] += T->pos[0]; corner[4][1] += T->pos[1]; corner[4][2] += T->pos[2];
    corner[5][0] += T->pos[0]; corner[5][1] += T->pos[1]; corner[5][2] += T->pos[2];
    corner[6][0] += T->pos[0]; corner[6][1] += T->pos[1]; corner[6][2] += T->pos[2];
    corner[7][0] += T->pos[0]; corner[7][1] += T->pos[1]; corner[7][2] += T->pos[2];

    glVertex3f(corner[0][0], corner[0][1], corner[0][2]);
    glVertex3f(corner[1][0], corner[1][1], corner[1][2]);

    glVertex3f(corner[1][0], corner[1][1], corner[1][2]);
    glVertex3f(corner[2][0], corner[2][1], corner[2][2]);

    glVertex3f(corner[2][0], corner[2][1], corner[2][2]);
    glVertex3f(corner[3][0], corner[3][1], corner[3][2]);

    glVertex3f(corner[3][0], corner[3][1], corner[3][2]);
    glVertex3f(corner[0][0], corner[0][1], corner[0][2]);

    //

    glVertex3f(corner[4][0], corner[4][1], corner[4][2]);
    glVertex3f(corner[5][0], corner[5][1], corner[5][2]);

    glVertex3f(corner[5][0], corner[5][1], corner[5][2]);
    glVertex3f(corner[6][0], corner[6][1], corner[6][2]);

    glVertex3f(corner[6][0], corner[6][1], corner[6][2]);
    glVertex3f(corner[7][0], corner[7][1], corner[7][2]);

    glVertex3f(corner[7][0], corner[7][1], corner[7][2]);
    glVertex3f(corner[4][0], corner[4][1], corner[4][2]);

    //

    glVertex3f(corner[0][0], corner[0][1], corner[0][2]);
    glVertex3f(corner[4][0], corner[4][1], corner[4][2]);

    glVertex3f(corner[1][0], corner[1][1], corner[1][2]);
    glVertex3f(corner[5][0], corner[5][1], corner[5][2]);

    glVertex3f(corner[2][0], corner[2][1], corner[2][2]);
    glVertex3f(corner[6][0], corner[6][1], corner[6][2]);

    glVertex3f(corner[3][0], corner[3][1], corner[3][2]);
    glVertex3f(corner[7][0], corner[7][1], corner[7][2]);
}

void render_Bones_ID()
{
    int b, idx0, idx1, idx2;
    bone * Bone;

    transformer * T0, * T1;

    int M0 = 255 * 255 * 255;
    int M1 = 255 * 255;

    float R, G, B, A;

    glDisable(GL_LIGHTING);
    glDisable(GL_DITHER);
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);
    glDisable(GL_MULTISAMPLE);

    glBegin(GL_LINES);

    for (b = 0; b < bonesIndex; b++)
    {
        Bone = bones[b];

        R = (float)(b / M0) / (float)255;
        idx0 = b % M0;
        G = (float)(idx0 / M1) / (float)255;
        idx1 = idx0 % M1;
        B = (float)(idx1 / 255) / (float)255;
        idx2 = idx1 % 255;
        A = (float)(idx2) / (float)255;

        glColor4f(R, G, B, A);

        T0 = Bone->A;
        T1 = Bone->B;

        glVertex3f(T0->pos[0], T0->pos[1], T0->pos[2]);
        glVertex3f(T1->pos[0], T1->pos[1], T1->pos[2]);
    }

    glEnd();

    glEnable(GL_MULTISAMPLE);
    glEnable(GL_BLEND);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_DITHER);
    glEnable(GL_LIGHTING);
}

void render_Camera_Icons_ID(int id_offset)
{
    int c, c_o;

    camera * C;
    transformer * T;

    float rotVec_[3][3];
    float L_size;

    int idx0, idx1, idx2;

    int M0 = 255 * 255 * 255;
    int M1 = 255 * 255;

    float R, G, B, A;

    glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);
    glDisable(GL_DITHER);
    glDisable(GL_MULTISAMPLE);

    glLineWidth(6);

    for (c = CAMERAS; c < camIndex; c ++)
    {
        C = cameras[c];

        if (C->T != NULL)
        {
            T = C->T;

            L_size = T->LocatorSize;

            memcpy(rotVec_, T->rotVec_, sizeof(rotVec_));

            rotVec_[0][0] *= L_size;
            rotVec_[0][1] *= L_size;
            rotVec_[0][2] *= L_size;
            rotVec_[1][0] *= L_size;
            rotVec_[1][1] *= L_size;
            rotVec_[1][2] *= L_size;
            rotVec_[2][0] *= L_size;
            rotVec_[2][1] *= L_size;
            rotVec_[2][2] *= L_size;

            c_o = c + id_offset;

            R = (float)(c_o / M0) / (float)255;
            idx0 = c_o % M0;
            G = (float)(idx0 / M1) / (float)255;
            idx1 = idx0 % M1;
            B = (float)(idx1 / 255) / (float)255;
            idx2 = idx1 % 255;
            A = (float)(idx2) / (float)255;

            glColor4f(R, G, B, A);

            glBegin(GL_LINE_LOOP);

            draw_Cam_Loop(T, rotVec_);

            glEnd();

//            glBegin(GL_LINES);
//
//            glVertex3f(T->pos[0], T->pos[1], T->pos[2]);
//            glVertex3f(T->target[0], T->target[1], T->target[2]);
//
//            glEnd();
        }
    }

    glEnable(GL_LIGHTING);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glEnable(GL_DITHER);
    glEnable(GL_MULTISAMPLE);
}

void render_Transformers_ID(int bind_Pose)
{
    int t;
    transformer * T;

    float rotVec_[3][3];
    float L_size;

    int idx0, idx1, idx2;

    int M0 = 255 * 255 * 255;
    int M1 = 255 * 255;

    float R, G, B, A;

    glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);
    glDisable(GL_DITHER);
    glDisable(GL_MULTISAMPLE);

    glLineWidth(6);

    glBegin(GL_LINES);

    for (t = CUBEINDEX - 1; t < transformerIndex; t ++)
    {
        if (t >= TRANSFORMERS - 1)
            break;

        T = transformers[t];

        if (T->collapsed)
            continue;

        if (T->Bone != NULL && T == T->Bone->B)
        {
            L_size = T->Bone->A->LocatorSize / 4;
        }
        else
        {
            L_size = T->LocatorSize;
        }

        memcpy(rotVec_, T->rotVec, sizeof(rotVec_));

        rotVec_[0][0] *= L_size;
        rotVec_[0][1] *= L_size;
        rotVec_[0][2] *= L_size;
        rotVec_[1][0] *= L_size;
        rotVec_[1][1] *= L_size;
        rotVec_[1][2] *= L_size;
        rotVec_[2][0] *= L_size;
        rotVec_[2][1] *= L_size;
        rotVec_[2][2] *= L_size;

        R = (float)(t / M0) / (float)255;
        idx0 = t % M0;
        G = (float)(idx0 / M1) / (float)255;
        idx1 = idx0 % M1;
        B = (float)(idx1 / 255) / (float)255;
        idx2 = idx1 % 255;
        A = (float)(idx2) / (float)255;

        glColor4f(R, G, B, A);

        if (T->Object != NULL)
        {
            if (bind_Pose)
            {
                glVertex3f(T->pos[0] + rotVec_[0][0] / 4, T->pos[1] + rotVec_[0][1] / 4, T->pos[2] + rotVec_[0][2] / 4);
                glVertex3f(T->pos[0] + rotVec_[2][0] / 4, T->pos[1] + rotVec_[2][1] / 4, T->pos[2] + rotVec_[2][2] / 4);

                glVertex3f(T->pos[0] + rotVec_[2][0] / 4, T->pos[1] + rotVec_[2][1] / 4, T->pos[2] + rotVec_[2][2] / 4);
                glVertex3f(T->pos[0] - rotVec_[0][0] / 4, T->pos[1] - rotVec_[0][1] / 4, T->pos[2] - rotVec_[0][2] / 4);

                glVertex3f(T->pos[0] - rotVec_[0][0] / 4, T->pos[1] - rotVec_[0][1] / 4, T->pos[2] - rotVec_[0][2] / 4);
                glVertex3f(T->pos[0] - rotVec_[2][0] / 4, T->pos[1] - rotVec_[2][1] / 4, T->pos[2] - rotVec_[2][2] / 4);

                glVertex3f(T->pos[0] - rotVec_[2][0] / 4, T->pos[1] - rotVec_[2][1] / 4, T->pos[2] - rotVec_[2][2] / 4);
                glVertex3f(T->pos[0] + rotVec_[0][0] / 4, T->pos[1] + rotVec_[0][1] / 4, T->pos[2] + rotVec_[0][2] / 4);

                glVertex3f(T->pos[0] + rotVec_[1][0] / 4, T->pos[1] + rotVec_[1][1] / 4, T->pos[2] + rotVec_[1][2] / 4);
                glVertex3f(T->pos[0] - rotVec_[1][0] / 4, T->pos[1] - rotVec_[1][1] / 4, T->pos[2] - rotVec_[1][2] / 4);
            }
            else
            {
                glVertex3f(T->pos[0] + rotVec_[0][0] / 2, T->pos[1] + rotVec_[0][1] / 2, T->pos[2] + rotVec_[0][2] / 2);
                glVertex3f(T->pos[0] + rotVec_[2][0] / 2, T->pos[1] + rotVec_[2][1] / 2, T->pos[2] + rotVec_[2][2] / 2);

                glVertex3f(T->pos[0] + rotVec_[2][0] / 2, T->pos[1] + rotVec_[2][1] / 2, T->pos[2] + rotVec_[2][2] / 2);
                glVertex3f(T->pos[0] - rotVec_[0][0] / 2, T->pos[1] - rotVec_[0][1] / 2, T->pos[2] - rotVec_[0][2] / 2);

                glVertex3f(T->pos[0] - rotVec_[0][0] / 2, T->pos[1] - rotVec_[0][1] / 2, T->pos[2] - rotVec_[0][2] / 2);
                glVertex3f(T->pos[0] - rotVec_[2][0] / 2, T->pos[1] - rotVec_[2][1] / 2, T->pos[2] - rotVec_[2][2] / 2);

                glVertex3f(T->pos[0] - rotVec_[2][0] / 2, T->pos[1] - rotVec_[2][1] / 2, T->pos[2] - rotVec_[2][2] / 2);
                glVertex3f(T->pos[0] + rotVec_[0][0] / 2, T->pos[1] + rotVec_[0][1] / 2, T->pos[2] + rotVec_[0][2] / 2);

                glVertex3f(T->pos[0] + rotVec_[1][0] / 2, T->pos[1] + rotVec_[1][1] / 2, T->pos[2] + rotVec_[1][2] / 2);
                glVertex3f(T->pos[0] - rotVec_[1][0] / 2, T->pos[1] - rotVec_[1][1] / 2, T->pos[2] - rotVec_[1][2] / 2);
            }
        }
        else if (T->style == ik_goal || T->style == ik_fixed)
        {
            draw_Box(T, rotVec_);
        }
        else if (T->style == root_node)
        {
            draw_Rhombic(T, rotVec_);
        }
        else
        {
            glVertex3f(T->pos[0], T->pos[1], T->pos[2]);
            glVertex3f(T->pos[0] + rotVec_[0][0], T->pos[1] + rotVec_[0][1], T->pos[2] + rotVec_[0][2]);

            glVertex3f(T->pos[0], T->pos[1], T->pos[2]);
            glVertex3f(T->pos[0] + rotVec_[1][0], T->pos[1] + rotVec_[1][1], T->pos[2] + rotVec_[1][2]);

            glVertex3f(T->pos[0], T->pos[1], T->pos[2]);
            glVertex3f(T->pos[0] + rotVec_[2][0], T->pos[1] + rotVec_[2][1], T->pos[2] + rotVec_[2][2]);

            glVertex3f(T->pos[0], T->pos[1], T->pos[2]);
            glVertex3f(T->pos[0] - rotVec_[0][0], T->pos[1] - rotVec_[0][1], T->pos[2] - rotVec_[0][2]);

            glVertex3f(T->pos[0], T->pos[1], T->pos[2]);
            glVertex3f(T->pos[0] - rotVec_[1][0], T->pos[1] - rotVec_[1][1], T->pos[2] - rotVec_[1][2]);

            glVertex3f(T->pos[0], T->pos[1], T->pos[2]);
            glVertex3f(T->pos[0] - rotVec_[2][0], T->pos[1] - rotVec_[2][1], T->pos[2] - rotVec_[2][2]);
        }
    }

    glEnd();

    glEnable(GL_LIGHTING);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glEnable(GL_DITHER);
    glEnable(GL_MULTISAMPLE);
}

void line_To_Childs(transformer * T)
{
    int c;
    transformer * C;

    for (c = 0; c < T->childcount; c ++)
    {
        C = T->childs[c];

        glColor4ubv(line_red);

        glVertex3f(T->pos[0], T->pos[1], T->pos[2]);
        float Mid[3] =
           {(C->pos[0] - T->pos[0]) / 2 + T->pos[0],
            (C->pos[1] - T->pos[1]) / 2 + T->pos[1],
            (C->pos[2] - T->pos[2]) / 2 + T->pos[2]};
        glVertex3f(Mid[0], Mid[1], Mid[2]);

        glColor4ubv(line_green);

        glVertex3f(Mid[0], Mid[1], Mid[2]);
        glVertex3f(C->pos[0], C->pos[1], C->pos[2]);

        line_To_Childs(C);
    }
}

void draw_IK_Chain(ikChain * I)
{
    glVertex3f(I->A->pos[0], I->A->pos[1], I->A->pos[2]);
    glVertex3f(I->B->pos[0], I->B->pos[1], I->B->pos[2]);
}

void render_IK_Chains()
{
    glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);

    glLineWidth(1);

    glEnable(GL_LINE_STIPPLE);
    glLineStipple(1, 0xEEEE);
    glBegin(GL_LINES);

    glColor4ubv(line_gray);

    int i;
    ikChain * I;

    for (i = 0; i < iksIndex; i ++)
    {
        I = ikChains[i];
        draw_IK_Chain(I);
    }

    glEnd();

    glEnable(GL_LIGHTING);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);

    glDisable(GL_LINE_STIPPLE);
}

void render_HighLighted_Bones()
{
    glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);

    glLineWidth(1);
    glLineStipple(1, 0xEEEE);
    glBegin(GL_LINES);

    glColor4ubv(line_white);

    int b;

    bone * B;
    transformer * T0, * T1;

    for(b = 0; b < bonesIndex; b ++)
    {
        B = bones[b];

        if (B->selected)
        {
            T0 = B->A;
            T1 = B->B;

            glVertex3f(T0->pos[0], T0->pos[1], T0->pos[2]);
            glVertex3f(T1->pos[0], T1->pos[1], T1->pos[2]);
        }
    }

    glEnd();

    glEnable(GL_LIGHTING);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
}

void render_Parent_Lines()
{
    glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);

    glLineWidth(1);

    glEnable(GL_LINE_STIPPLE);
    glLineStipple(1, 0xEEEE);
    glBegin(GL_LINES);

    int c;

    transformer * C, * T;

    T = transformers[0];

    for (c = 0; c < T->childcount; c ++)
    {
        C = T->childs[c];
        line_To_Childs(C);
    }

    glEnd();

    glEnable(GL_LIGHTING);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);

    glDisable(GL_LINE_STIPPLE);
}

void draw_Axis(int Axis_lock, float object_Pos[3])
{
    glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);

    glLineWidth(1);

    glBegin(GL_LINES);

    if (Axis_lock == 1)
    {
        glColor4ubv(line_white);
        glVertex3f(object_Pos[0] - 1, object_Pos[1], object_Pos[2]);
        glVertex3f(object_Pos[0], object_Pos[1], object_Pos[2]);
        glColor4ubv(line_red);
        glVertex3f(object_Pos[0], object_Pos[1], object_Pos[2]);
        glVertex3f(object_Pos[0] + 1, object_Pos[1], object_Pos[2]);
    }
    else if (Axis_lock == 2)
    {
        glColor4ubv(line_white);
        glVertex3f(object_Pos[0], object_Pos[1] - 1, object_Pos[2]);
        glVertex3f(object_Pos[0], object_Pos[1], object_Pos[2]);
        glColor4ubv(line_cyan);
        glVertex3f(object_Pos[0], object_Pos[1], object_Pos[2]);
        glVertex3f(object_Pos[0], object_Pos[1] + 1, object_Pos[2]);
    }
    else if (Axis_lock == 3)
    {
        glColor4ubv(line_white);
        glVertex3f(object_Pos[0], object_Pos[1], object_Pos[2] - 1);
        glVertex3f(object_Pos[0], object_Pos[1], object_Pos[2]);
        glColor4ubv(line_blue);
        glVertex3f(object_Pos[0], object_Pos[1], object_Pos[2]);
        glVertex3f(object_Pos[0], object_Pos[1], object_Pos[2] + 1);
    }

    glEnd();

    glEnable(GL_LIGHTING);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
}

void render_Corner_Edge_Smooth_Labels(int width, int height, object * O, int l)
{
    int idx, e, e0, q, p, v_start;

    quadrant * Q;
    polygon * P;
    vertex * V, * V0, * V1;
    edge * E, * E0;

    label * L;

    label_count = 0;

    GLdouble point[3];
    GLdouble coords[3];

    int result;

    if (l < 0)
    {
        v_start = O->vertcount + O->edgecount;

        for (p = 0; p < O->polycount; p ++)
        {
            P = &O->polys[p / ARRAYSIZE][p % ARRAYSIZE];

            if (P->subdivs && P->selected)
            {
                V = &O->verts_[0][(p + v_start) / ARRAYSIZE][(p + v_start) % ARRAYSIZE];

                if (V->patch)
                {
                    for (e = 0; e < V->edgecount; e ++)
                    {
                        idx = V->edges[e];
                        E = &O->edges_[0][idx / ARRAYSIZE][idx % ARRAYSIZE];

                        idx = E->verts[1];
                        V0 = &O->verts_[1][idx / ARRAYSIZE][idx % ARRAYSIZE];

                        if (V0->vert != NULL)
                        {
                            V1 = V0->vert;

                            for (e0 = 0; e0 < V1->edgecount; e0 ++)
                            {
                                idx = V1->edges[e0];
                                E0 = &O->edges_[1][idx / ARRAYSIZE][idx % ARRAYSIZE];

                                point[0] = E0->B.Tx;
                                point[1] = E0->B.Ty;
                                point[2] = E0->B.Tz;

                                result = point_on_screen_GLU(point, coords);

                                if (result && label_count < LABELS)
                                {
                                    L = labels[label_count ++];
//                                    sprintf(L->text, "%d", E0->smooth);
                                    L->x = coords[0] - 100;
                                    L->y = height - coords[1] + 20;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    else
    {
        v_start = O->vertcount_[l] + O->edgecount_[l];

        for (q = 0; q < O->quadcount_[l]; q ++)
        {
            Q = &O->quads_[l][q / ARRAYSIZE][q % ARRAYSIZE];

            if (Q->subdivs && Q->selected)
            {
                V = &O->verts_[l + 1][(q + v_start) / ARRAYSIZE][(q + v_start) % ARRAYSIZE];

                if (V->patch)
                {
                    for (e = 0; e < V->edgecount; e ++)
                    {
                        idx = V->edges[e];
                        E = &O->edges_[l + 1][idx / ARRAYSIZE][idx % ARRAYSIZE];

                        idx = E->verts[1];
                        V0 = &O->verts_[l + 1][idx / ARRAYSIZE][idx % ARRAYSIZE];

                        if (V0->vert != NULL)
                        {
                            V1 = V0->vert;

                            for (e0 = 0; e0 < V1->edgecount; e0 ++)
                            {
                                idx = V1->edges[e0];
                                E0 = &O->edges_[l + 2][idx / ARRAYSIZE][idx % ARRAYSIZE];

                                point[0] = E0->B.Tx;
                                point[1] = E0->B.Ty;
                                point[2] = E0->B.Tz;

                                result = point_on_screen_GLU(point, coords);

                                if (result && label_count < LABELS)
                                {
                                    L = labels[label_count ++];
//                                    sprintf(L->text, "%d", E0->smooth);
                                    L->x = coords[0] - 100;
                                    L->y = height - coords[1] + 20;
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    display_labels(width, height);
}

void render_Edge_Smooth_Labels(int width, int height, object * O, int l)
{
    int idx, e, q, p, v_start;

    quadrant * Q;
    polygon * P;
    vertex * V;
    edge * E;

    label * L;

    label_count = 0;

    GLdouble point[3];
    GLdouble coords[3];

    int result;

    if (l < 0)
    {
        v_start = O->vertcount + O->edgecount;

        for (p = 0; p < O->polycount; p ++)
        {
            P = &O->polys[p / ARRAYSIZE][p % ARRAYSIZE];

            if (P->subdivs && P->selected)
            {
                V = &O->verts_[0][(p + v_start) / ARRAYSIZE][(p + v_start) % ARRAYSIZE];

                if (V->patch)
                {
                    for (e = 0; e < V->edgecount; e ++)
                    {
                        idx = V->edges[e];
                        E = &O->edges_[0][idx / ARRAYSIZE][idx % ARRAYSIZE];

                        point[0] = E->B.Tx;
                        point[1] = E->B.Ty;
                        point[2] = E->B.Tz;

                        result = point_on_screen_GLU(point, coords);

                        if (result && label_count < LABELS)
                        {
                            L = labels[label_count ++];
//                            sprintf(L->text, "%d", E->perimeter->smooth);
                            L->x = coords[0] - 100;
                            L->y = height - coords[1] + 20;
                        }
                    }
                }
            }
        }
    }
    else
    {
        v_start = O->vertcount_[l] + O->edgecount_[l];

        for (q = 0; q < O->quadcount_[l]; q ++)
        {
            Q = &O->quads_[l][q / ARRAYSIZE][q % ARRAYSIZE];

            if (Q->subdivs && Q->selected)
            {
                V = &O->verts_[l + 1][(q + v_start) / ARRAYSIZE][(q + v_start) % ARRAYSIZE];

                if (V->patch)
                {
                    for (e = 0; e < V->edgecount; e ++)
                    {
                        idx = V->edges[e];
                        E = &O->edges_[l + 1][idx / ARRAYSIZE][idx % ARRAYSIZE];

                        point[0] = E->B.Tx;
                        point[1] = E->B.Ty;
                        point[2] = E->B.Tz;

                        result = point_on_screen_GLU(point, coords);

                        if (result && label_count < LABELS)
                        {
                            L = labels[label_count ++];
//                            sprintf(L->text, "%d", E->perimeter->smooth);
                            L->x = coords[0] - 100;
                            L->y = height - coords[1] + 20;
                        }
                    }
                }
            }
        }
    }

    display_labels(width, height);
}

void render_Patch_Labels(int width, int height, object * O, int l)
{
    int q, p, v_start;

    quadrant * Q;
    polygon * P;
    vertex * V;

    label * L;

    label_count = 0;

    GLdouble point[3];
    GLdouble coords[3];

    int result;

    if (l < 0)
    {
        v_start = O->vertcount + O->edgecount;

        for (p = 0; p < O->polycount; p ++)
        {
            P = &O->polys[p / ARRAYSIZE][p % ARRAYSIZE];

            if (P->subdivs && P->selected)
            {
                V = &O->verts_[0][(p + v_start) / ARRAYSIZE][(p + v_start) % ARRAYSIZE];

                if (V->patch)
                {
                    point[0] = V->Tx;
                    point[1] = V->Ty;
                    point[2] = V->Tz;

                    result = point_on_screen_GLU(point, coords);

                    if (result && label_count < LABELS)
                    {
                        L = labels[label_count ++];
                        L->text[0] = '1';
                        L->text[1] = '\0';
                        L->x = coords[0] - 100;
                        L->y = height - coords[1] + 20;
                    }
                }
            }
        }
    }
    else
    {
        v_start = O->vertcount_[l] + O->edgecount_[l];

        for (q = 0; q < O->quadcount_[l]; q ++)
        {
            Q = &O->quads_[l][q / ARRAYSIZE][q % ARRAYSIZE];

            if (Q->subdivs && Q->selected)
            {
                V = &O->verts_[l + 1][(q + v_start) / ARRAYSIZE][(q + v_start) % ARRAYSIZE];

                if (V->patch)
                {
                    point[0] = V->Tx;
                    point[1] = V->Ty;
                    point[2] = V->Tz;

                    result = point_on_screen_GLU(point, coords);

                    if (result && label_count < LABELS)
                    {
                        L = labels[label_count ++];
                        L->text[0] = '1';
                        L->text[1] = '\0';
                        L->x = coords[0] - 100;
                        L->y = height - coords[1] + 20;
                    }
                }
            }
        }
    }

    display_labels(width, height);
}

void label_Hexas(HexG * H, camera * C, int width, int height)
{
    int h;
    HexG * H_0;
    label * L;

    int x_2 = width / 2;
    int y_2 = height / 2;

    float h_view = C->h_view * 2;
    float v_view = C->v_view * 2;

    if (label_count < LABELS)
    {
        L = labels[label_count ++];
        sprintf(L->text, "%d", H->index);
        L->x = (H->Center[0] / h_view) * width + x_2;
        L->y = (-H->Center[1] / v_view) * height + y_2;
    }

    if (H->subdivided)
    {
        for (h = 0; h < 6; h ++) // 6
        {
            H_0 = H->subH[h];

            if (H_0 != NULL)
            {
                label_Hexas(H_0, C, width, height);
            }
        }
    }
}

void render_Hexagon_Labels(camera * C, int width, int height)
{
    int h;

    HexG * H;

    label_count = 0;

    for (h = 0; h < 7; h ++)
    {
        H = Hex_Group[h];

        label_Hexas(H, C, width, height);
    }

    display_labels(width, height);
}

void render_patch_edge_polys_Labels(int width, int height, object * O, int l)
{
    int p, e, v_start, idx;

    quadrant * Q;
    polygon * P;
    vertex * V;
    edge * E, * E0;

    label * L;

    label_count = 0;

    GLdouble point[3];
    GLdouble coords[3];

    int result;

    v_start = O->vertcount + O->edgecount;

    for (p = 0; p < O->polycount; p ++)
    {
        P = &O->polys[p / ARRAYSIZE][p % ARRAYSIZE];

        if (P->subdivs && P->selected)
        {
            V = &O->verts_[0][(p + v_start) / ARRAYSIZE][(p + v_start) % ARRAYSIZE];

            for (e = 0; e < V->edgecount; e ++)
            {
                idx = V->edges[e];
                E = &O->edges_[0][idx / ARRAYSIZE][idx % ARRAYSIZE];

                idx = P->edges[e];
                E0 = &O->edges[idx / ARRAYSIZE][idx % ARRAYSIZE];

                idx = E->polys[0];
                Q = &O->quads_[0][idx / ARRAYSIZE][idx % ARRAYSIZE];

                if (E->perimeter == E0)
                {
                    point[0] = E->B.Tx;
                    point[1] = E->B.Ty;
                    point[2] = E->B.Tz;

                    result = point_on_screen_GLU(point, coords);

                    if (result && label_count < LABELS)
                    {
                        L = labels[label_count ++];
                        L->text[0] = e + 48;
                        L->text[1] = ' ';
                        L->text[2] = E->slots[0] + 48;
                        L->text[3] = '\0';
                        L->x = coords[0] - 100;
                        L->y = height - coords[1] + 20;
                    }

                    point[0] = Q->B.Tx;
                    point[1] = Q->B.Ty;
                    point[2] = Q->B.Tz;

                    result = point_on_screen_GLU(point, coords);

                    if (result && label_count < LABELS)
                    {
                        L = labels[label_count ++];
                        L->text[0] = e + 48;
                        L->text[1] = ' ';
                        L->text[2] = E->slots[0] + 48;
                        L->text[3] = '\0';
                        L->x = coords[0] - 100;
                        L->y = height - coords[1] + 20;
                    }
                }
            }
        }
    }

    display_labels(width, height);
}

void render_patch_edge_Labels(int width, int height, object * O, int l)
{
    int p, e, v_start, idx;

    polygon * P;
    vertex * V;
    edge * E;

    label * L;

    label_count = 0;

    GLdouble point[3];
    GLdouble coords[3];

    int result;

    v_start = O->vertcount + O->edgecount;

    for (p = 0; p < O->polycount; p ++)
    {
        P = &O->polys[p / ARRAYSIZE][p % ARRAYSIZE];

        if (P->subdivs && P->selected)
        {
            V = &O->verts_[0][(p + v_start) / ARRAYSIZE][(p + v_start) % ARRAYSIZE];

            for (e = 0; e < V->edgecount; e ++)
            {
                idx = V->edges[e];
                E = &O->edges_[0][idx / ARRAYSIZE][idx % ARRAYSIZE];

                point[0] = E->B.Tx;
                point[1] = E->B.Ty;
                point[2] = E->B.Tz;

                result = point_on_screen_GLU(point, coords);

                if (result && label_count < LABELS)
                {
                    L = labels[label_count ++];
                    L->text[0] = e + 48;
                    L->text[1] = '\0';
                    L->x = coords[0] - 100;
                    L->y = height - coords[1] + 20;
                }
            }
        }
    }

    display_labels(width, height);
}

void render_poly_winding_Labels(int width, int height, object * O, int l)
{
    int p, e, idx;

    polygon * P;
    quadrant * Q;

    label * L;

    label_count = 0;

    GLdouble point[3];
    GLdouble coords[3];

    int result;

    for (p = 0; p < O->polycount; p ++)
    {
        P = &O->polys[p / ARRAYSIZE][p % ARRAYSIZE];

        if (P->subdivs && P->selected)
        {
            for (e = 0; e < P->edgecount; e ++)
            {
                idx = P->quads[e];
                Q = &O->quads_[0][idx / ARRAYSIZE][idx % ARRAYSIZE];

                if (Q->selected)
                {
                    point[0] = Q->B.Tx;
                    point[1] = Q->B.Ty;
                    point[2] = Q->B.Tz;

                    result = point_on_screen_GLU(point, coords);

                    if (result && label_count < LABELS)
                    {
                        L = labels[label_count ++];
                        L->text[0] = e + 48;
                        L->text[1] = '\0';
                        L->x = coords[0] - 100;
                        L->y = height - coords[1] + 20;
                    }
                }
            }
        }
    }

    display_labels(width, height);
}

void render_IK_Labels(int width, int height)
{
    int t;
    transformer * T;

    label * L;

    label_count = 0;
    int label_cursor;

    GLdouble point[3];
    GLdouble coords[3];

    int result;

    for (t = CUBEINDEX - 1; t < transformerIndex; t ++)
    {
        if (t >= TRANSFORMERS - 1)
            break;

        T = transformers[t];

        if (T->collapsed)
            continue;

        label_cursor = 0;

        if (T->style == ik_fixed)
        {
            point[0] = T->pos[0];
            point[1] = T->pos[1];
            point[2] = T->pos[2];

            result = point_on_screen_GLU(point, coords);

            if (result && label_count < LABELS)
            {
                L = labels[label_count ++];
                L->x = coords[0] - 120;
                L->y = height - (coords[1] + 10);

                if (T->style == ik_fixed)
                {
                    L->text[label_cursor] = 'F';
                    label_cursor ++;
                }

                if (T->IK != NULL && T->IK->update)
                {
                    L->text[label_cursor] = 'U';
                    label_cursor ++;
                }

                if (T->pin)
                {
                    L->text[label_cursor] = (char)(T->pin + 119); // 48
                    label_cursor ++;
                }

                L->text[label_cursor] = '\0';
            }
        }
    }

    display_labels(width, height);
}

void render_Camera_Icons()
{
    int c;

    camera * C;
    transformer * T;

    float rotVec_[3][3];
    float L_size;

    glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);

    glLineWidth(1);

    for (c = CAMERAS; c < camIndex; c ++)
    {
        C = cameras[c];

        if (C->T != NULL)
        {
            T = C->T;

            L_size = T->LocatorSize;

            memcpy(rotVec_, T->rotVec_, sizeof(rotVec_));

            rotVec_[0][0] *= L_size;
            rotVec_[0][1] *= L_size;
            rotVec_[0][2] *= L_size;
            rotVec_[1][0] *= L_size;
            rotVec_[1][1] *= L_size;
            rotVec_[1][2] *= L_size;
            rotVec_[2][0] *= L_size;
            rotVec_[2][1] *= L_size;
            rotVec_[2][2] *= L_size;

            if (C->selected)
            {
                glColor4ubv(line_white);
            }
            else
            {
                glColor4ubv(line_gray);
            }

            glBegin(GL_LINE_LOOP);

            draw_Cam_Loop(T, rotVec_);

            glEnd();

            glBegin(GL_LINES);

            glVertex3f(T->pos[0], T->pos[1], T->pos[2]);
            glVertex3f(T->target[0], T->target[1], T->target[2]);

            glEnd();
        }
    }

    glEnable(GL_LIGHTING);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
}

void render_Hexagons(camera * C)
{
    float width = C->h_view;
    float height = C->v_view;

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(-width, width, -height, height, 1, -1);

    glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_DEPTH_TEST);
    //glDisable(GL_BLEND);

//    glLineWidth(1);
//    glColor4ubv(line_gray);

    int h;

    HexG * H;

    for (h = 0; h < hexaIndex; h ++)
    {
        H = Hexas[h];

        glColor4fv(hexasColor[h % 7]);

        glBegin(GL_POLYGON);

        glVertex2f(H->Verts[0][0], H->Verts[0][1]);
        glVertex2f(H->Verts[1][0], H->Verts[1][1]);
        glVertex2f(H->Verts[2][0], H->Verts[2][1]);
        glVertex2f(H->Verts[3][0], H->Verts[3][1]);
        glVertex2f(H->Verts[4][0], H->Verts[4][1]);
        glVertex2f(H->Verts[5][0], H->Verts[5][1]);

        glEnd();
    }

    glEnable(GL_LIGHTING);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_DEPTH_TEST);
    //glEnable(GL_BLEND);

    glPopMatrix();
}

void render_Transformers(int currentLocator, int bind_Pose)
{
    int t;
    transformer * T;

    float rotVec_[3][3];
    float L_size;

    glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);

    glLineWidth(1);

    glBegin(GL_LINES);

    for (t = CUBEINDEX - 1; t < transformerIndex; t ++)
    {
        if (t >= TRANSFORMERS - 1)
            break;

        T = transformers[t];

        if (T->collapsed)
            continue;

        if (T->Bone != NULL && T == T->Bone->B)
        {
            L_size = T->Bone->A->LocatorSize / 4;
        }
        else
        {
            L_size = T->LocatorSize;
        }

        memcpy(rotVec_, T->rotVec, sizeof(rotVec_));

        rotVec_[0][0] *= L_size;
        rotVec_[0][1] *= L_size;
        rotVec_[0][2] *= L_size;
        rotVec_[1][0] *= L_size;
        rotVec_[1][1] *= L_size;
        rotVec_[1][2] *= L_size;
        rotVec_[2][0] *= L_size;
        rotVec_[2][1] *= L_size;
        rotVec_[2][2] *= L_size;

        if (T->Object != NULL)
        {
            if (t == currentLocator)
            {
                glColor4ubv(line_yellow);
            }
            else if (T->selected)
            {
                glColor4ubv(line_white);
            }
            else
            {
                glColor4ubv(line_gray);
            }

            if (bind_Pose)
            {
                glVertex3f(T->pos[0] + rotVec_[0][0] / 4, T->pos[1] + rotVec_[0][1] / 4, T->pos[2] + rotVec_[0][2] / 4);
                glVertex3f(T->pos[0] + rotVec_[2][0] / 4, T->pos[1] + rotVec_[2][1] / 4, T->pos[2] + rotVec_[2][2] / 4);

                glVertex3f(T->pos[0] + rotVec_[2][0] / 4, T->pos[1] + rotVec_[2][1] / 4, T->pos[2] + rotVec_[2][2] / 4);
                glVertex3f(T->pos[0] - rotVec_[0][0] / 4, T->pos[1] - rotVec_[0][1] / 4, T->pos[2] - rotVec_[0][2] / 4);

                glVertex3f(T->pos[0] - rotVec_[0][0] / 4, T->pos[1] - rotVec_[0][1] / 4, T->pos[2] - rotVec_[0][2] / 4);
                glVertex3f(T->pos[0] - rotVec_[2][0] / 4, T->pos[1] - rotVec_[2][1] / 4, T->pos[2] - rotVec_[2][2] / 4);

                glVertex3f(T->pos[0] - rotVec_[2][0] / 4, T->pos[1] - rotVec_[2][1] / 4, T->pos[2] - rotVec_[2][2] / 4);
                glVertex3f(T->pos[0] + rotVec_[0][0] / 4, T->pos[1] + rotVec_[0][1] / 4, T->pos[2] + rotVec_[0][2] / 4);

                glVertex3f(T->pos[0] + rotVec_[1][0] / 4, T->pos[1] + rotVec_[1][1] / 4, T->pos[2] + rotVec_[1][2] / 4);
                glVertex3f(T->pos[0] - rotVec_[1][0] / 4, T->pos[1] - rotVec_[1][1] / 4, T->pos[2] - rotVec_[1][2] / 4);
            }
            else
            {
                glVertex3f(T->pos[0] + rotVec_[0][0] / 2, T->pos[1] + rotVec_[0][1] / 2, T->pos[2] + rotVec_[0][2] / 2);
                glVertex3f(T->pos[0] + rotVec_[2][0] / 2, T->pos[1] + rotVec_[2][1] / 2, T->pos[2] + rotVec_[2][2] / 2);

                glVertex3f(T->pos[0] + rotVec_[2][0] / 2, T->pos[1] + rotVec_[2][1] / 2, T->pos[2] + rotVec_[2][2] / 2);
                glVertex3f(T->pos[0] - rotVec_[0][0] / 2, T->pos[1] - rotVec_[0][1] / 2, T->pos[2] - rotVec_[0][2] / 2);

                glVertex3f(T->pos[0] - rotVec_[0][0] / 2, T->pos[1] - rotVec_[0][1] / 2, T->pos[2] - rotVec_[0][2] / 2);
                glVertex3f(T->pos[0] - rotVec_[2][0] / 2, T->pos[1] - rotVec_[2][1] / 2, T->pos[2] - rotVec_[2][2] / 2);

                glVertex3f(T->pos[0] - rotVec_[2][0] / 2, T->pos[1] - rotVec_[2][1] / 2, T->pos[2] - rotVec_[2][2] / 2);
                glVertex3f(T->pos[0] + rotVec_[0][0] / 2, T->pos[1] + rotVec_[0][1] / 2, T->pos[2] + rotVec_[0][2] / 2);

                glVertex3f(T->pos[0] + rotVec_[1][0] / 2, T->pos[1] + rotVec_[1][1] / 2, T->pos[2] + rotVec_[1][2] / 2);
                glVertex3f(T->pos[0] - rotVec_[1][0] / 2, T->pos[1] - rotVec_[1][1] / 2, T->pos[2] - rotVec_[1][2] / 2);
            }
        }
        else if (T->style == ik_goal || T->style == ik_fixed)
        {
            if (t == currentLocator)
            {
                glColor4ubv(line_yellow);
            }
            else if (T->selected)
            {
                glColor4ubv(line_white);
            }
            else
            {
                glColor4ubv(line_gray);
            }
            draw_Box(T, rotVec_);
        }
        else if (T->style == root_node)
        {
            if (t == currentLocator)
            {
                glColor4ubv(line_yellow);
            }
            else if (T->selected)
            {
                glColor4ubv(line_white);
            }
            else
            {
                glColor4ubv(line_gray);
            }
            draw_Rhombic(T, rotVec_);
        }
        else
        {
            glColor4ubv(line_red);

            glVertex3f(T->pos[0], T->pos[1], T->pos[2]);
            glVertex3f(T->pos[0] + rotVec_[0][0], T->pos[1] + rotVec_[0][1], T->pos[2] + rotVec_[0][2]);

            glColor4ubv(line_cyan);

            glVertex3f(T->pos[0], T->pos[1], T->pos[2]);
            glVertex3f(T->pos[0] + rotVec_[1][0], T->pos[1] + rotVec_[1][1], T->pos[2] + rotVec_[1][2]);

            glColor4ubv(line_blue);

            glVertex3f(T->pos[0], T->pos[1], T->pos[2]);
            glVertex3f(T->pos[0] + rotVec_[2][0], T->pos[1] + rotVec_[2][1], T->pos[2] + rotVec_[2][2]);

            if (t == currentLocator)
            {
                glColor4ubv(line_yellow);
            }
            else if (T->selected)
            {
                glColor4ubv(line_white);
            }
            else
            {
                glColor4ubv(line_gray);
            }

            glVertex3f(T->pos[0], T->pos[1], T->pos[2]);
            glVertex3f(T->pos[0] - rotVec_[0][0], T->pos[1] - rotVec_[0][1], T->pos[2] - rotVec_[0][2]);

            glVertex3f(T->pos[0], T->pos[1], T->pos[2]);
            glVertex3f(T->pos[0] - rotVec_[1][0], T->pos[1] - rotVec_[1][1], T->pos[2] - rotVec_[1][2]);

            glVertex3f(T->pos[0], T->pos[1], T->pos[2]);
            glVertex3f(T->pos[0] - rotVec_[2][0], T->pos[1] - rotVec_[2][1], T->pos[2] - rotVec_[2][2]);
        }
    }

    glEnd();

    glEnable(GL_LIGHTING);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
}

void render_polys_quads_OnScreen_Shadows(camera * C, int l, int currentObject, int mode, int Selection_Mode, int update_colors, int update_uv, int elem)
{
    int o, idx, L;
    object * O;

    glEnable(GL_DEPTH_TEST);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, 0);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, 0);
    glActiveTexture(GL_TEXTURE0);

    for (o = 0; o < C->object_count; o++)
    {
        idx = C->objects[o];
        O = objects[idx];

        glCallList(displayLists[Materials[O->surface].Texture_idx % Textures_c]);

        if (l > O->subdlevel)
        {
            L = O->subdlevel;
        }
        else
        {
            L = l;
        }

        if (O->shadow)
        {
            if (VAO && SHADERS)
            {
                if (SHADOWS)
                {
                    glUseProgram(T_program[4]);
                    glUniform1i(uniform_tex2, 0);
                    glUniformMatrix3fv(uniform_cam4, 1, GL_TRUE, (GLfloat*)C->T->rotVec_);
                    glUniformMatrix4fv(uniform_proj4, 1, GL_FALSE, projectionMatrix);
                    glUniformMatrix4fv(uniform_sha4, 1, GL_FALSE, shadowMatrix);
                    if (L == -1)
                    {
                        if (Fan_Arrays_Shader)
                        {
                            draw_Fan_Arrays_Shader(O, update_colors, update_uv, elem);
                        }
                    }
                    else
                    {
                        draw_Arrays_Shader(O, L, update_colors, update_uv, elem);
                    }
                    glUseProgram(0);
                }
            }
        }
    }
}

void render_polys_OnScreen_Shadows(camera * C, int currentObject, int mode, int Selection_Mode, int update_colors, int update_uv, int elem)
{
    int o, idx;
    object * O;

    glEnable(GL_DEPTH_TEST);
//    glActiveTexture(GL_TEXTURE0);
//    glBindTexture(GL_TEXTURE_2D, 0);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, 0);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, 0);
    glActiveTexture(GL_TEXTURE0);

    for (o = 0; o < C->object_count; o++)
    {
        idx = C->objects[o];
        O = objects[idx];

        glCallList(displayLists[Materials[O->surface].Texture_idx % Textures_c]);

        if (O->shadow)
        {
            if (VAO && SHADERS && Fan_Arrays_Shader)
            {
                if (SHADOWS)
                {
                    glUseProgram(T_program[4]);
                    glUniform1i(uniform_tex2, 0);
                    glUniformMatrix3fv(uniform_cam4, 1, GL_TRUE, (GLfloat*)C->T->rotVec_);
                    glUniformMatrix4fv(uniform_proj4, 1, GL_FALSE, projectionMatrix);
                    glUniformMatrix4fv(uniform_sha4, 1, GL_FALSE, shadowMatrix);
                    draw_Fan_Arrays_Shader(O, update_colors, update_uv, elem);
                    glUseProgram(0);
                }
            }
        }
    }
}

void render_Depth_Drag_Plane()
{
//    glEnable(GL_DEPTH_TEST);
//    glActiveTexture(GL_TEXTURE0);
//    glBindTexture(GL_TEXTURE_2D, 0);
//    glActiveTexture(GL_TEXTURE1);
//    glBindTexture(GL_TEXTURE_2D, 0);
//    glActiveTexture(GL_TEXTURE2);
//    glBindTexture(GL_TEXTURE_2D, 0);
//    glActiveTexture(GL_TEXTURE0);
//    glBindTexture(GL_TEXTURE_2D, 0);

    glBegin(GL_QUADS);

    glVertex3fv(Drag_Plane.A);
    glVertex3fv(Drag_Plane.B);
    glVertex3fv(Drag_Plane.C);
    glVertex3fv(Drag_Plane.D);

    glEnd();
}

void render_polys_OnScreen(camera * C, int wireframe, int edgedraw, int vertdraw, int currentObject, int rendermode, int Selection_Mode, int update_colors, int update_uv, int elem)
{
    int v, o, p, e, idx;
    object * O;
    id_color * I;
    polygon * P;
    vertex * V;
    edge * E;
    uv * UV;

    surface_Material Material;

    Uint8 r, g, b, a;

    for (o = 0; o < C->object_count; o++)
    {
        idx = C->objects[o];
        O = objects[idx];

        glActiveTexture(GL_TEXTURE0);
        glCallList(displayLists[Materials[O->surface].Texture_idx % Textures_c]);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, depthTex);

        glActiveTexture(GL_TEXTURE2);
        glCallList(normalsLists[Materials[O->surface].Normal_idx % Normals_c]);

        glActiveTexture(GL_TEXTURE3);
        glCallList(bumpsLists[Materials[O->surface].Bump_idx % Bumps_c]);

        glActiveTexture(GL_TEXTURE0);

        if (O->vertex_array)
        {
            if (VAO && SHADERS && Fan_Arrays_Shader)
            {
                if (rendermode == ID_RENDER)
                {
                    glUseProgram(T_program[3]);
                    glUniformMatrix4fv(uniform_proj3, 1, GL_FALSE, projectionMatrix);

                    draw_Fan_Arrays_Shader_ID(O, update_colors, update_uv, elem);
                    glUseProgram(0);
                }
                else if (SHADOWS)
                {
                    glUseProgram(T_program[4]);

                    glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 1, &pass2Index);

                    glUniform1i(uniform_tex2, 0); // 0th texture
                    glUniform1i(uniform_smap4, 1); // 0th texture
                    glUniform1i(uniform_nmap4, 2); // 0th texture
                    glUniform1i(uniform_bmap4, 3); // 0th texture
                    glBindBufferBase(GL_UNIFORM_BUFFER, 0, lightHandle);
                    glBindBufferBase(GL_UNIFORM_BUFFER, 1, fogHandle);
                    glUniformMatrix3fv(uniform_cam4, 1, GL_TRUE, (GLfloat*)C->T->rotVec_);
                    glUniformMatrix4fv(uniform_proj4, 1, GL_FALSE, projectionMatrix);
                    glUniformMatrix4fv(uniform_sha4, 1, GL_FALSE, shadowMatrix);
                    glUniform1i(uniform_night4, NIGHT);
                    glUniform1f(uniform_refls4, Reflect);
                    glUniform1f(uniform_shine4, Materials[O->surface].Shininess);
                    glUniform1f(uniform_displ4, Materials[O->surface].Displacement);
                    draw_Fan_Arrays_Shader(O, update_colors, update_uv, elem);
                    glUseProgram(0);
                }
                else
                {
                    glUseProgram(T_program[2]);
                    glUniform1i(uniform_tex2, 0); // 0th texture
                    glBindBufferBase(GL_UNIFORM_BUFFER, 0, lightHandle);
                    glBindBufferBase(GL_UNIFORM_BUFFER, 1, fogHandle);
                    glUniformMatrix3fv(uniform_cam2, 1, GL_TRUE, (GLfloat*)C->T->rotVec_);
                    glUniformMatrix4fv(uniform_proj2, 1, GL_FALSE, projectionMatrix);
                    glUniform1i(uniform_night2, NIGHT);
                    glUniform1f(uniform_refls2, Reflect);
                    draw_Fan_Arrays_Shader(O, update_colors, update_uv, elem);
                    glUseProgram(0);
                }
            }
            else
            {
                draw_Fan_Arrays(O, rendermode, update_colors, update_uv, elem);
            }
        }
        else
        {
            for (p = 0; p < O->polycount; p++)
            {
                P = &O->polys[p / ARRAYSIZE][p % ARRAYSIZE];

                idx = P->surface;
                Material = Materials[idx];

                r = Material.RGBA.R;
                g = Material.RGBA.G;
                b = Material.RGBA.B;
                a = Material.RGBA.A;

                if (rendermode == ID_RENDER)
                {
                    idx = P->color;
                    I = &O->icols[idx / ARRAYSIZE][idx % ARRAYSIZE];
                    glColor4f(I->R, I->G, I->B, I->A);
                }
                else
                {
                    glColor4ub(r, g, b, a);
                }

                if (wireframe)
                {
                    glBegin(GL_LINE_LOOP);
                    for (e = 0; e < P->edgecount; e ++)
                    {
                        idx = P->verts[e];
                        V = &O->verts[idx / ARRAYSIZE][idx % ARRAYSIZE];
                        glVertex3f(V->Tx, V->Ty, V->Tz);
                    }
                    glEnd();
                }
                else
                {
                    glBegin(GL_TRIANGLE_FAN);
                    for (e = 0; e < P->edgecount; e ++)
                    {
                        idx = P->verts[e];
                        V = &O->verts[idx / ARRAYSIZE][idx % ARRAYSIZE];
                        idx = P->texts[e];
                        UV = &O->uvtex[idx / ARRAYSIZE][idx % ARRAYSIZE];
                        glNormal3f(V->N.Tx, V->N.Ty, V->N.Tz);
                        glTexCoord2f(UV->u, UV->v);
                        glVertex3f(V->Tx, V->Ty, V->Tz);
                    }
                    glEnd();
                }
            }
        }

        if (edgedraw) // assuming some objects have no geometry on all levels.
        {
            glDisable(GL_TEXTURE_2D);
            //glDisable(GL_MULTISAMPLE);
            if (Selection_Mode == POLYS)
                glEnable(GL_LINE_STIPPLE);
            if (C->objects[o] == currentObject && O->selected)
            {
                if (Selection_Mode == POLYS)
                glLineStipple(1, 0xEEEE);
                else if (Selection_Mode == VERTS)
                glLineWidth(1);
                else if (Selection_Mode == EDGES)
                glLineWidth(1);
                else
                glLineWidth(2);
                if (O->deforms)
                line_color = &current_deform_color[0];
                else
                line_color = &current_color[0];
            }
            else if (O->selected)
            {
                if (Selection_Mode == POLYS)
                glLineStipple(1, 0xEEEE);
                else if (Selection_Mode == VERTS)
                glLineWidth(1);
                else
                glLineWidth(1);
                if (O->deforms)
                line_color = &selected_deform_color[0];
                else
                line_color = &selected_color[0];
            }
            else
            {
                glLineStipple(1, 0xFFFF);
                glLineWidth(1);
                if (O->deforms)
                line_color = &dark_deform_color[0];
                else
                line_color = &dark_wire_color[0];
            }
            if (Selection_Mode != OBJES)
            {
                memcpy(Line_Color, line_color, sizeof Line_Color);
                for (e = 0; e < O->edgecount; e++)
                {
                    E = &O->edges[e / ARRAYSIZE][e % ARRAYSIZE];
                    if (E->selected)
                    {
                        glDisable(GL_LIGHTING);
//                        if (O->selected)
//                            glLineWidth(2);
//                        else
//                            glLineWidth(1);
                        line_color = &selected_edge_color[0];
                    }
                    else if (!glIsEnabled(GL_LIGHTING))
                    {
                        glEnable(GL_LIGHTING);
//                        glLineWidth(1);
                        line_color = Line_Color;
                    }
                    draw_Edge(O, E, C, wireframe, E->B.Tradius);
                }
            }
            else
            {
                //if (C->objects[o] == currentObject || O->selected)
                //{
                    render_Outline_polys(O, C);
                //}
//                else
//                {
//                    for (e = 0; e < O->edgecount; e ++)
//                    {
//                        E = &O->edges[e / ARRAYSIZE][e % ARRAYSIZE];
//                        draw_Edge(O, E, C, wireframe, E->B.Tradius);
//                    }
//                }
            }
            glEnable(GL_TEXTURE_2D);
            //glEnable(GL_MULTISAMPLE);
            glEnable(GL_LIGHTING);
            glDisable(GL_LINE_STIPPLE);
            glLineWidth(1);
        }

        if (vertdraw)
        {
            if (C->objects[o] == currentObject)
            glPointSize(3);
            else
            glPointSize(2);
            glDisable(GL_TEXTURE_2D);
            glDisable(GL_LIGHTING);
            glBegin(GL_POINTS);
            glColor4ubv(vert_color);

            float factor;
            float dot;
            float view_minor;

            aim Aim;
            normal Normal;

            if (C->view_minor > 1)
                view_minor = C->view_minor;
            else
                view_minor = 1;

            for (v = 0; v < O->vertcount; v ++)
            {
                V = &O->verts[v / ARRAYSIZE][v % ARRAYSIZE];

                if (V->selected)
                {
                    glColor4ubv(selected_vert_color);
                }
                else
                {
                    glColor4ubv(vert_color);
                }

                Normal.x = -V->N.Tx;
                Normal.y = -V->N.Ty;
                Normal.z = -V->N.Tz;

                Aim = vector3d_T(V, C->T->pos);

                if (C->ortho)
                {
                    dot = dot_productN(&Normal, C->T->aim);
                }
                else
                {
                    dot = dot_productN(&Normal, Aim.vec);
                }

                if (dot < 0)
                    continue;

                if (C->ortho)
                {
                    factor = Aim.dist * view_minor * 0.002 * dot;
                    glVertex3f(V->Tx + V->N.Tx * factor, V->Ty + V->N.Ty * factor, V->Tz + V->N.Tz * factor);
                }
                else
                {
                    factor = Aim.dist * view_minor * 0.3 * O->mean_Edge * dot;
                    if (factor > Aim.dist / 2)
                    {
                        factor = Aim.dist / 2;
                    }
                    glVertex3f(V->Tx - Aim.vec[0] * factor, V->Ty - Aim.vec[1] * factor, V->Tz - Aim.vec[2] * factor);
                }
            }
            glEnd();
            glEnable(GL_LIGHTING);
            glEnable(GL_TEXTURE_2D);
        }
    }
}

void load_id_colors_Fan_current(object * O, int mode)
{
    int e, v, p, t, idx, idx0, idx1, idx2;
    triangle * T;
    polygon * P;
    id_color * I;

    float R, G, B, A;

    R = 1;
    G = 1;
    B = 1;
    A = 1;

    int M0 = 255 * 255 * 255;
    int M1 = 255 * 255;

    int c_c = 0;
    int c_c1 = 0;

    idx = O->index;
    if (O->vertex_array)
    {
        if (mode == OBJECT_ID_COLORS)
        {
            R = (float)(idx / M0) / (float)255;
            idx0 = idx % M0;
            G = (float)(idx0 / M1) / (float)255;
            idx1 = idx0 % M1;
            B = (float)(idx1 / 255) / (float)255;
            idx2 = idx1 % 255;
            A = (float)(idx2) / (float)255;
        }

        for (p = 0; p < O->polycount; p++)
        {
            P = &O->polys[p / ARRAYSIZE][p % ARRAYSIZE];
            if (mode == POLYGON_ID_COLORS)
            {
                idx = P->color;
                I = &O->icols[idx / ARRAYSIZE][idx % ARRAYSIZE];
                R = I->R;
                G = I->G;
                B = I->B;
                A = I->A;
            }
            else if (mode == OBJECT_COLORS)
            {
                if (P->selected == 1)
                {
                    R = 0;
                    G = 0;
                    B = 1;
                    A = 1;
                }
                else
                {
                    R = Materials[P->surface].RGBA.R / 255;
                    G = Materials[P->surface].RGBA.G / 255;
                    B = Materials[P->surface].RGBA.B / 255;
                    A = Materials[P->surface].RGBA.A / 255;
                }
            }

//            if (E)
//            {
            for (t = 0; t < P->tripcount; t ++)
            {
                idx = P->trips[t];
                T = &O->trips[idx / ARRAYSIZE][idx % ARRAYSIZE];
                for (v = 0; v < 3; v ++)
                {
                    idx = T->texts[v];
                    c_c1 = idx * 4;
                    O->cols_array[1][c_c1 ++] = R;
                    O->cols_array[1][c_c1 ++] = G;
                    O->cols_array[1][c_c1 ++] = B;
                    O->cols_array[1][c_c1 ++] = A;
                }
            }
//            }
//            else
//            {
            for (e = 0; e < P->edgecount; e ++)
            {
                O->cols_array[0][c_c ++] = R;
                O->cols_array[0][c_c ++] = G;
                O->cols_array[0][c_c ++] = B;
                O->cols_array[0][c_c ++] = A;
            }
//            }
        }
    }
}

void load_id_colors_Fan_all(camera * C, int mode)
{
    int e, t, v, o, p, idx, idx0, idx1, idx2;
    object * O;
    triangle * T;
    polygon * P;
    id_color * I;

    float R, G, B, A;

    R = 1;
    G = 1;
    B = 1;
    A = 1;

    int M0 = 255 * 255 * 255;
    int M1 = 255 * 255;

    int c_c;
    int c_c1;

    for (o = 0; o < C->object_count; o++)
    {
        O = objects[C->objects[o]];
        idx = O->index;

        c_c = 0;

        if (O->vertex_array)
        {
            if (mode == OBJECT_ID_COLORS)
            {
                R = (float)(idx / M0) / (float)255;
                idx0 = idx % M0;
                G = (float)(idx0 / M1) / (float)255;
                idx1 = idx0 % M1;
                B = (float)(idx1 / 255) / (float)255;
                idx2 = idx1 % 255;
                A = (float)(idx2) / (float)255;
            }

            for (p = 0; p < O->polycount; p++)
            {
                P = &O->polys[p / ARRAYSIZE][p % ARRAYSIZE];
                if (mode == POLYGON_ID_COLORS)
                {
                    idx = P->color;
                    I = &O->icols[idx / ARRAYSIZE][idx % ARRAYSIZE];
                    R = I->R;
                    G = I->G;
                    B = I->B;
                    A = I->A;
                }
                else if (mode == OBJECT_COLORS)
                {
                    if (P->selected == 1)
                    {
                        R = 0;
                        G = 0;
                        B = 1;
                        A = 1;
                    }
                    else
                    {
                        R = Materials[P->surface].RGBA.R / 255;
                        G = Materials[P->surface].RGBA.G / 255;
                        B = Materials[P->surface].RGBA.B / 255;
                        A = Materials[P->surface].RGBA.A / 255;
                    }
                }

//                if (E)
//                {
                for (t = 0; t < P->tripcount; t ++)
                {
                    idx = P->trips[t];
                    T = &O->trips[idx / ARRAYSIZE][idx % ARRAYSIZE];
                    for (v = 0; v < 3; v ++)
                    {
                        idx = T->texts[v];
                        c_c1 = idx * 4;
                        O->cols_array[1][c_c1 ++] = R;
                        O->cols_array[1][c_c1 ++] = G;
                        O->cols_array[1][c_c1 ++] = B;
                        O->cols_array[1][c_c1 ++] = A;
                    }
                }
//                }
//                else
//                {
                for (e = 0; e < P->edgecount; e ++)
                {
                    O->cols_array[0][c_c ++] = R;
                    O->cols_array[0][c_c ++] = G;
                    O->cols_array[0][c_c ++] = B;
                    O->cols_array[0][c_c ++] = A;
                }
//                }
            }
        }
    }
}

void load_id_colors_Fan(int * Objects, int Objects_count, int mode)
{
    int e, t, v, o, p, idx, idx0, idx1, idx2;

    object * O;
    triangle * T;
    polygon * P;
    id_color * I;

    float R, G, B, A;

    R = 1;
    G = 1;
    B = 1;
    A = 1;

    int M0 = 255 * 255 * 255;
    int M1 = 255 * 255;

    int c_c;
    int c_c1;

    for (o = 0; o < Objects_count; o++)
    {
        O = objects[Objects[o]];

        c_c = 0;
        c_c1 = 0;

        idx = O->index;
        if (O->vertex_array)
        {
            if (mode == OBJECT_ID_COLORS)
            {
                R = (float)(idx / M0) / (float)255;
                idx0 = idx % M0;
                G = (float)(idx0 / M1) / (float)255;
                idx1 = idx0 % M1;
                B = (float)(idx1 / 255) / (float)255;
                idx2 = idx1 % 255;
                A = (float)(idx2) / (float)255;
            }

            for (p = 0; p < O->polycount; p++)
            {
                P = &O->polys[p / ARRAYSIZE][p % ARRAYSIZE];
                if (mode == POLYGON_ID_COLORS)
                {
                    idx = P->color;
                    I = &O->icols[idx / ARRAYSIZE][idx % ARRAYSIZE];
                    R = I->R;
                    G = I->G;
                    B = I->B;
                    A = I->A;
                }
                else if (mode == OBJECT_COLORS)
                {
                    if (P->selected == 1)
                    {
                        R = 0;
                        G = 0;
                        B = 1;
                        A = 1;
                    }
                    else
                    {
                        R = Materials[P->surface].RGBA.R / 255;
                        G = Materials[P->surface].RGBA.G / 255;
                        B = Materials[P->surface].RGBA.B / 255;
                        A = Materials[P->surface].RGBA.A / 255;
                    }
                }

//                if (E)
//                {
                for (t = 0; t < P->tripcount; t ++)
                {
                    idx = P->trips[t];
                    T = &O->trips[idx / ARRAYSIZE][idx % ARRAYSIZE];
                    for (v = 0; v < 3; v ++)
                    {
                        idx = T->texts[v];
                        c_c1 = idx * 4;
                        O->cols_array[1][c_c1 ++] = R;
                        O->cols_array[1][c_c1 ++] = G;
                        O->cols_array[1][c_c1 ++] = B;
                        O->cols_array[1][c_c1 ++] = A;
                    }
                }
//                }
//                else
//                {
                for (e = 0; e < P->edgecount; e ++)
                {
                    O->cols_array[0][c_c ++] = R;
                    O->cols_array[0][c_c ++] = G;
                    O->cols_array[0][c_c ++] = B;
                    O->cols_array[0][c_c ++] = A;
                }
//                }
            }
        }
    }
}

void load_id_colors_No_Surface(camera * C, int l)
{
    int e, o, v, q, idx, L;
    int t, p;

    object * O;
    quadrant * Q;

    polygon * P;
    triangle * T;

    float R, G, B, A;

    int c_c;
    int c_c1;

    for (o = 0; o < C->object_count; o++)
    {
        idx = C->objects[o];
        O = objects[idx];

        c_c = 0;

        if (l > O->subdlevel)
        {
            L = O->subdlevel;
        }
        else
        {
            L = l;
        }
        if (L > -1 && O->vertex_arrays[L])
        {
            for (q = 0; q < O->quadcount_[L]; q++)
            {
                Q = &O->quads_[L][q / ARRAYSIZE][q % ARRAYSIZE];

                if (Q->selected == 1)
                {
                    R = 0;
                    G = 0;
                    B = 1;
                    A = 1;
                }
                else
                {
                    R = Materials[Q->surface].RGBA.R / 255;
                    G = Materials[Q->surface].RGBA.G / 255;
                    B = Materials[Q->surface].RGBA.B / 255;
                    A = Materials[Q->surface].RGBA.A / 255;
                }

//                if (E)
//                {
                for (v = 0; v < 4; v ++)
                {
                    idx = Q->texts[v];
                    c_c1 = idx * 4;
                    O->cols_array_[L][1][c_c1 ++] = R;
                    O->cols_array_[L][1][c_c1 ++] = G;
                    O->cols_array_[L][1][c_c1 ++] = B;
                    O->cols_array_[L][1][c_c1 ++] = A;
                }
//                }
//                else
//                {
                for (e = 0; e < 4; e ++)
                {
                    O->cols_array_[L][0][c_c ++] = R;
                    O->cols_array_[L][0][c_c ++] = G;
                    O->cols_array_[L][0][c_c ++] = B;
                    O->cols_array_[L][0][c_c ++] = A;
                }
//                }
            }
        }
        else if (O->vertex_array)
        {
            for (p = 0; p < O->polycount; p++)
            {
                P = &O->polys[p / ARRAYSIZE][p % ARRAYSIZE];

                if (P->selected == 1)
                {
                    R = 0;
                    G = 0;
                    B = 1;
                    A = 1;
                }
                else
                {
                    R = Materials[P->surface].RGBA.R / 255;
                    G = Materials[P->surface].RGBA.G / 255;
                    B = Materials[P->surface].RGBA.B / 255;
                    A = Materials[P->surface].RGBA.A / 255;
                }

                for (t = 0; t < P->tripcount; t ++)
                {
                    idx = P->trips[t];
                    T = &O->trips[idx / ARRAYSIZE][idx % ARRAYSIZE];
                    for (v = 0; v < 3; v ++)
                    {
                        idx = T->texts[v];
                        c_c1 = idx * 4;
                        O->cols_array[1][c_c1 ++] = R;
                        O->cols_array[1][c_c1 ++] = G;
                        O->cols_array[1][c_c1 ++] = B;
                        O->cols_array[1][c_c1 ++] = A;
                    }
                }
                for (e = 0; e < P->edgecount; e ++)
                {
                    O->cols_array[0][c_c ++] = R;
                    O->cols_array[0][c_c ++] = G;
                    O->cols_array[0][c_c ++] = B;
                    O->cols_array[0][c_c ++] = A;
                }
            }
        }
    }
}

void load_id_colors_current(object * O, int l, int mode)
{
    int e, v, q, idx, idx0, idx1, idx2, L;

    quadrant * Q;
    id_color * I;

    float R, G, B, A;

    R = 1;
    G = 1;
    B = 1;
    A = 1;

    int M0 = 255 * 255 * 255;
    int M1 = 255 * 255;

    idx = O->index;
    if (l > O->subdlevel)
    {
        L = O->subdlevel;
    }
    else
    {
        L = l;
    }

    int c_c = 0;
    int c_c1 = 0;

    if (L > -1 && O->vertex_arrays[L])
    {
        if (mode == OBJECT_ID_COLORS)
        {
            R = (float)(idx / M0) / (float)255;
            idx0 = idx % M0;
            G = (float)(idx0 / M1) / (float)255;
            idx1 = idx0 % M1;
            B = (float)(idx1 / 255) / (float)255;
            idx2 = idx1 % 255;
            A = (float)(idx2) / (float)255;
        }

        for (q = 0; q < O->quadcount_[L]; q++)
        {
            Q = &O->quads_[L][q / ARRAYSIZE][q % ARRAYSIZE];
            if (mode == POLYGON_ID_COLORS)
            {
                idx = Q->color;
                I = &O->icols[idx / ARRAYSIZE][idx % ARRAYSIZE];
                R = I->R;
                G = I->G;
                B = I->B;
                A = I->A;
            }
            else if (mode == OBJECT_COLORS)
            {
                if (Q->selected == 1)
                {
                    R = 0;
                    G = 0;
                    B = 1;
                    A = 1;
                }
                else
                {
                    R = Materials[Q->surface].RGBA.R / 255;
                    G = Materials[Q->surface].RGBA.G / 255;
                    B = Materials[Q->surface].RGBA.B / 255;
                    A = Materials[Q->surface].RGBA.A / 255;
                }
            }

//            if (E)
//            {
            for (v = 0; v < 4; v ++)
            {
                idx = Q->texts[v];
                c_c1 = idx * 4;
                O->cols_array_[L][1][c_c1 ++] = R;
                O->cols_array_[L][1][c_c1 ++] = G;
                O->cols_array_[L][1][c_c1 ++] = B;
                O->cols_array_[L][1][c_c1 ++] = A;
            }
//            }
//            else
//            {
            for (e = 0; e < 4; e ++)
            {
                O->cols_array_[L][0][c_c ++] = R;
                O->cols_array_[L][0][c_c ++] = G;
                O->cols_array_[L][0][c_c ++] = B;
                O->cols_array_[L][0][c_c ++] = A;
            }
//            }
        }
    }
}

void load_id_colors_all(camera * C, int l, int mode)
{
    int e, v, o, q, idx, idx0, idx1, idx2, L;
    int t, p;

    object * O;
    quadrant * Q;

    polygon * P;
    triangle * T;
    id_color * I;

    float R, G, B, A;

    R = 1;
    G = 1;
    B = 1;
    A = 1;

    int M0 = 255 * 255 * 255;
    int M1 = 255 * 255;

    int c_c;
    int c_c1;

    for (o = 0; o < C->object_count; o++)
    {
        O = objects[C->objects[o]];

        c_c = 0;
        c_c1 = 0;

        idx = O->index;
        if (l > O->subdlevel)
        {
            L = O->subdlevel;
        }
        else
        {
            L = l;
        }
        if (L > -1 && O->vertex_arrays[L])
        {
            if (mode == OBJECT_ID_COLORS)
            {
                R = (float)(idx / M0) / (float)255;
                idx0 = idx % M0;
                G = (float)(idx0 / M1) / (float)255;
                idx1 = idx0 % M1;
                B = (float)(idx1 / 255) / (float)255;
                idx2 = idx1 % 255;
                A = (float)(idx2) / (float)255;
            }

            for (q = 0; q < O->quadcount_[L]; q++)
            {
                Q = &O->quads_[L][q / ARRAYSIZE][q % ARRAYSIZE];
                if (mode == POLYGON_ID_COLORS)
                {
                    idx = Q->color;
                    I = &O->icols[idx / ARRAYSIZE][idx % ARRAYSIZE];
                    R = I->R;
                    G = I->G;
                    B = I->B;
                    A = I->A;
                }
                else if (mode == OBJECT_COLORS)
                {
                    if (Q->selected == 1)
                    {
                        R = 0;
                        G = 0;
                        B = 1;
                        A = 1;
                    }
                    else
                    {
                        R = Materials[Q->surface].RGBA.R / 255;
                        G = Materials[Q->surface].RGBA.G / 255;
                        B = Materials[Q->surface].RGBA.B / 255;
                        A = Materials[Q->surface].RGBA.A / 255;
                    }
                }

//                if (E)
//                {
                for (v = 0; v < 4; v ++)
                {
                    idx = Q->texts[v];
                    c_c1 = idx * 4;
                    O->cols_array_[L][1][c_c1 ++] = R;
                    O->cols_array_[L][1][c_c1 ++] = G;
                    O->cols_array_[L][1][c_c1 ++] = B;
                    O->cols_array_[L][1][c_c1 ++] = A;
                }
//                }
//                else
//                {
                for (e = 0; e < 4; e ++)
                {
                    O->cols_array_[L][0][c_c ++] = R;
                    O->cols_array_[L][0][c_c ++] = G;
                    O->cols_array_[L][0][c_c ++] = B;
                    O->cols_array_[L][0][c_c ++] = A;
                }
//                }
            }
        }
        else if (O->vertex_array)
        {
            if (mode == OBJECT_ID_COLORS)
            {
                R = (float)(idx / M0) / (float)255;
                idx0 = idx % M0;
                G = (float)(idx0 / M1) / (float)255;
                idx1 = idx0 % M1;
                B = (float)(idx1 / 255) / (float)255;
                idx2 = idx1 % 255;
                A = (float)(idx2) / (float)255;
            }

            for (p = 0; p < O->polycount; p++)
            {
                P = &O->polys[p / ARRAYSIZE][p % ARRAYSIZE];
                if (mode == POLYGON_ID_COLORS)
                {
                    idx = P->color;
                    I = &O->icols[idx / ARRAYSIZE][idx % ARRAYSIZE];
                    R = I->R;
                    G = I->G;
                    B = I->B;
                    A = I->A;
                }
                else if (mode == OBJECT_COLORS)
                {
                    if (P->selected == 1)
                    {
                        R = 0;
                        G = 0;
                        B = 1;
                        A = 1;
                    }
                    else
                    {
                        R = Materials[P->surface].RGBA.R / 255;
                        G = Materials[P->surface].RGBA.G / 255;
                        B = Materials[P->surface].RGBA.B / 255;
                        A = Materials[P->surface].RGBA.A / 255;
                    }
                }

                for (t = 0; t < P->tripcount; t ++)
                {
                    idx = P->trips[t];
                    T = &O->trips[idx / ARRAYSIZE][idx % ARRAYSIZE];
                    for (v = 0; v < 3; v ++)
                    {
                        idx = T->texts[v];
                        c_c1 = idx * 4;
                        O->cols_array[1][c_c1 ++] = R;
                        O->cols_array[1][c_c1 ++] = G;
                        O->cols_array[1][c_c1 ++] = B;
                        O->cols_array[1][c_c1 ++] = A;
                    }
                }
                for (e = 0; e < P->edgecount; e ++)
                {
                    O->cols_array[0][c_c ++] = R;
                    O->cols_array[0][c_c ++] = G;
                    O->cols_array[0][c_c ++] = B;
                    O->cols_array[0][c_c ++] = A;
                }
            }
        }
    }
}

void update_UV_Mapping(int E)
{
    int q, idx, e, p, l, v, o;

    object * O;
    quadrant * Q;
    polygon * P;
    uv * UV;

    int t_c;

    for (o = 0; o < objectIndex; o ++)
    {
        O = objects[o];

        if (E)
        {
            t_c = 0;

            for (v = 0; v < O->textcount; v ++)
            {
                UV = &O->uvtex[v / ARRAYSIZE][v % ARRAYSIZE];
                O->text_array[1][t_c++] = UV->u;
                O->text_array[1][t_c++] = UV->v;
            }

            for (l = 0; l <= O->subdlevel; l ++)
            {
                t_c = 0;

                for (v = 0; v < O->textcount_[l]; v ++)
                {
                    UV = &O->uvtex_[l][v / ARRAYSIZE][v % ARRAYSIZE];
                    O->text_array_[l][1][t_c++] = UV->u;
                    O->text_array_[l][1][t_c++] = UV->v;
                }
            }
        }
        else
        {
            t_c = 0;
            for (p = 0; p < O->polycount; p ++)
            {
                P = &O->polys[p / ARRAYSIZE][p % ARRAYSIZE];

                for (e = 0; e < P->edgecount; e ++)
                {
                    idx = P->texts[e];
                    UV = &O->uvtex[idx / ARRAYSIZE][idx % ARRAYSIZE];
                    O->text_array[0][t_c++] = UV->u;
                    O->text_array[0][t_c++] = UV->v;
                }
            }
            for (l = 0; l <= O->subdlevel; l ++)
            {
                t_c = 0;
                for (q = 0; q < O->quadcount_[l]; q ++)
                {
                    Q = &O->quads_[l][q / ARRAYSIZE][q % ARRAYSIZE];

                    for (e = 0; e < 4; e ++)
                    {
                        idx = Q->texts[e];
                        UV = &O->uvtex_[l][idx / ARRAYSIZE][idx % ARRAYSIZE];
                        O->text_array_[l][0][t_c++] = UV->u;
                        O->text_array_[l][0][t_c++] = UV->v;
                    }
                }
            }
        }
    }
}

void load_id_colors(int * Objects, int Objects_count, int l, int mode)
{
    int e, v, o, q, idx, idx0, idx1, idx2, L;

    object * O;
    quadrant * Q;
    id_color * I;

    float R, G, B, A;

    R = 1;
    G = 1;
    B = 1;
    A = 1;

    int M0 = 255 * 255 * 255;
    int M1 = 255 * 255;

    int c_c;
    int c_c1;

    for (o = 0; o < Objects_count; o++)
    {
        O = objects[Objects[o]];
        idx = O->index;
        if (l > O->subdlevel)
        {
            L = O->subdlevel;
        }
        else
        {
            L = l;
        }
        if (L > -1 && O->vertex_arrays[L])
        {
            c_c = 0;
            c_c1 = 0;

            if (mode == OBJECT_ID_COLORS)
            {
                R = (float)(idx / M0) / (float)255;
                idx0 = idx % M0;
                G = (float)(idx0 / M1) / (float)255;
                idx1 = idx0 % M1;
                B = (float)(idx1 / 255) / (float)255;
                idx2 = idx1 % 255;
                A = (float)(idx2) / (float)255;
            }

            for (q = 0; q < O->quadcount_[L]; q++)
            {
                Q = &O->quads_[L][q / ARRAYSIZE][q % ARRAYSIZE];
                if (mode == POLYGON_ID_COLORS)
                {
                    idx = Q->color;
                    I = &O->icols[idx / ARRAYSIZE][idx % ARRAYSIZE];
                    R = I->R;
                    G = I->G;
                    B = I->B;
                    A = I->A;
                }
                else if (mode == OBJECT_COLORS)
                {
                    if (Q->selected == 1)
                    {
                        R = 0;
                        G = 0;
                        B = 1;
                        A = 1;
                    }
                    else
                    {
                        R = Materials[Q->surface].RGBA.R / 255;
                        G = Materials[Q->surface].RGBA.G / 255;
                        B = Materials[Q->surface].RGBA.B / 255;
                        A = Materials[Q->surface].RGBA.A / 255;
                    }
                }
//                if (E)
//                {
                for (v = 0; v < 4; v ++)
                {
                    idx = Q->texts[v];
                    c_c1 = idx * 4;
                    O->cols_array_[L][1][c_c1 ++] = R;
                    O->cols_array_[L][1][c_c1 ++] = G;
                    O->cols_array_[L][1][c_c1 ++] = B;
                    O->cols_array_[L][1][c_c1 ++] = A;
                }
//                }
//                else
//                {
                for (e = 0; e < 4; e ++)
                {
                    O->cols_array_[L][0][c_c ++] = R;
                    O->cols_array_[L][0][c_c ++] = G;
                    O->cols_array_[L][0][c_c ++] = B;
                    O->cols_array_[L][0][c_c ++] = A;
                }
//                }
            }
        }
    }
}

void fill_in_VertCoords_Fan_Object(object * O, int E)
{
    int p, e, v, idx;

    polygon * P;
    vertex * V;
    uv * UV;

    if (O->vertex_array)
    {
        int v_c = 0;
        int n_c = 0;
        int t_a = 0;

        if (E)
        {
            for (v = 0; v < O->textcount; v ++)
            {
                UV = &O->uvtex[v / ARRAYSIZE][v % ARRAYSIZE];

                idx = UV->vert;
                V = &O->verts[idx / ARRAYSIZE][idx % ARRAYSIZE];
                O->vert_array[1][v_c++] = V->Tx;
                O->vert_array[1][v_c++] = V->Ty;
                O->vert_array[1][v_c++] = V->Tz;
                O->norm_array[1][n_c++] = V->N.Tx;
                O->norm_array[1][n_c++] = V->N.Ty;
                O->norm_array[1][n_c++] = V->N.Tz;
                O->tang_array[1][t_a++] = UV->tangent[0];
                O->tang_array[1][t_a++] = UV->tangent[1];
                O->tang_array[1][t_a++] = UV->tangent[2];
                O->tang_array[1][t_a++] = UV->tangent[3];
            }
        }
        else
        {
            for (p = 0; p < O->polycount; p++)
            {
                P = &O->polys[p / ARRAYSIZE][p % ARRAYSIZE];

                for (e = 0; e < P->edgecount; e ++)
                {
                    idx = P->verts[e];
                    V = &O->verts[idx / ARRAYSIZE][idx % ARRAYSIZE];
                    idx = P->texts[e];
                    UV = &O->uvtex[idx / ARRAYSIZE][idx % ARRAYSIZE];
                    O->vert_array[0][v_c++] = V->Tx;
                    O->vert_array[0][v_c++] = V->Ty;
                    O->vert_array[0][v_c++] = V->Tz;

                    if (O->smooth)
                    {
                        O->norm_array[0][n_c++] = V->N.Tx;
                        O->norm_array[0][n_c++] = V->N.Ty;
                        O->norm_array[0][n_c++] = V->N.Tz;
                    }
                    else
                    {
                        O->norm_array[0][n_c++] = P->N.Tx;
                        O->norm_array[0][n_c++] = P->N.Ty;
                        O->norm_array[0][n_c++] = P->N.Tz;
                    }
                    //O->text_array[t_c++] = UV->u;
                    //O->text_array[t_c++] = UV->v;
                    O->tang_array[0][t_a++] = UV->tangent[0];
                    O->tang_array[0][t_a++] = UV->tangent[1];
                    O->tang_array[0][t_a++] = UV->tangent[2];
                    O->tang_array[0][t_a++] = UV->tangent[3];
                }
            }
        }
    }
}

void fill_in_VertCoords_Fan_Quads(camera * C, int l, int E)
{
    int p, q, e, o, v, idx, L;

    object * O;
    polygon * P;
    quadrant * Q;
    vertex * V;
    uv * UV;

    int v_c, n_c, t_a;

    for (o = 0; o < C->object_count; o++)
    {
        idx = C->objects[o];
        O = objects[idx];
        if (l > O->subdlevel)
        {
            L = O->subdlevel;
        }
        else
        {
            L = l;
        }
        if (L == -1)
        {
            if (O->vertex_array)
            {
                v_c = 0;
                n_c = 0;
                t_a = 0;
    //            t_c = 0;

                if (E)
                {
                    for (v = 0; v < O->textcount; v ++)
                    {
                        UV = &O->uvtex[v / ARRAYSIZE][v % ARRAYSIZE];

                        idx = UV->vert;
                        V = &O->verts[idx / ARRAYSIZE][idx % ARRAYSIZE];
                        O->vert_array[1][v_c++] = V->Tx;
                        O->vert_array[1][v_c++] = V->Ty;
                        O->vert_array[1][v_c++] = V->Tz;
                        O->norm_array[1][n_c++] = V->N.Tx;
                        O->norm_array[1][n_c++] = V->N.Ty;
                        O->norm_array[1][n_c++] = V->N.Tz;
                        O->tang_array[1][t_a++] = UV->tangent[0];
                        O->tang_array[1][t_a++] = UV->tangent[1];
                        O->tang_array[1][t_a++] = UV->tangent[2];
                        O->tang_array[1][t_a++] = UV->tangent[3];
                    }
                }
                else
                {
                    for (p = 0; p < O->polycount; p ++)
                    {
                        P = &O->polys[p / ARRAYSIZE][p % ARRAYSIZE];

                        for (e = 0; e < P->edgecount; e ++)
                        {
                            idx = P->verts[e];
                            V = &O->verts[idx / ARRAYSIZE][idx % ARRAYSIZE];
                            idx = P->texts[e];
                            UV = &O->uvtex[idx / ARRAYSIZE][idx % ARRAYSIZE];
                            O->vert_array[0][v_c++] = V->Tx;
                            O->vert_array[0][v_c++] = V->Ty;
                            O->vert_array[0][v_c++] = V->Tz;

                            if (O->smooth)
                            {
                                O->norm_array[0][n_c++] = V->N.Tx;
                                O->norm_array[0][n_c++] = V->N.Ty;
                                O->norm_array[0][n_c++] = V->N.Tz;
                            }
                            else
                            {
                                O->norm_array[0][n_c++] = P->N.Tx;
                                O->norm_array[0][n_c++] = P->N.Ty;
                                O->norm_array[0][n_c++] = P->N.Tz;
                            }
    //                        O->text_array[t_c++] = UV->u;
    //                        O->text_array[t_c++] = UV->v;
                            O->tang_array[0][t_a++] = UV->tangent[0];
                            O->tang_array[0][t_a++] = UV->tangent[1];
                            O->tang_array[0][t_a++] = UV->tangent[2];
                            O->tang_array[0][t_a++] = UV->tangent[3];
                        }
                    }
                }
            }
        }
        else if (O->vertex_arrays[L])
        {
            v_c = 0;
            n_c = 0;
            t_a = 0;

            if (E)
            {
                for (v = 0; v < O->textcount_[L]; v ++)
                {
                    UV = &O->uvtex_[L][v / ARRAYSIZE][v % ARRAYSIZE];

                    idx = UV->vert;
                    V = &O->verts_[L][idx / ARRAYSIZE][idx % ARRAYSIZE];
                    O->vert_array_[L][1][v_c++] = V->Tx;
                    O->vert_array_[L][1][v_c++] = V->Ty;
                    O->vert_array_[L][1][v_c++] = V->Tz;
                    O->norm_array_[L][1][n_c++] = V->N.Tx;
                    O->norm_array_[L][1][n_c++] = V->N.Ty;
                    O->norm_array_[L][1][n_c++] = V->N.Tz;
                    O->tang_array_[L][1][t_a++] = UV->tangent[0];
                    O->tang_array_[L][1][t_a++] = UV->tangent[1];
                    O->tang_array_[L][1][t_a++] = UV->tangent[2];
                    O->tang_array_[L][1][t_a++] = UV->tangent[3];
                }
            }
            else
            {
                for (q = 0; q < O->quadcount_[L]; q++)
                {
                    Q = &O->quads_[L][q / ARRAYSIZE][q % ARRAYSIZE];

                    for (e = 0; e < 4; e ++)
                    {
                        idx = Q->verts[e];
                        V = &O->verts_[L][idx / ARRAYSIZE][idx % ARRAYSIZE];
                        idx = Q->texts[e];
                        UV = &O->uvtex_[L][idx / ARRAYSIZE][idx % ARRAYSIZE];
                        O->vert_array_[L][0][v_c++] = V->Tx;
                        O->vert_array_[L][0][v_c++] = V->Ty;
                        O->vert_array_[L][0][v_c++] = V->Tz;

                        if (O->smooth)
                        {
                            O->norm_array_[L][0][n_c++] = V->N.Tx;
                            O->norm_array_[L][0][n_c++] = V->N.Ty;
                            O->norm_array_[L][0][n_c++] = V->N.Tz;
                        }
                        else
                        {
                            O->norm_array_[L][0][n_c++] = Q->N.Tx;
                            O->norm_array_[L][0][n_c++] = Q->N.Ty;
                            O->norm_array_[L][0][n_c++] = Q->N.Tz;
                        }
    //                    O->text_array_[L][t_c++] = UV->u;
    //                    O->text_array_[L][t_c++] = UV->v;
                        O->tang_array_[L][0][t_a++] = UV->tangent[0];
                        O->tang_array_[L][0][t_a++] = UV->tangent[1];
                        O->tang_array_[L][0][t_a++] = UV->tangent[2];
                        O->tang_array_[L][0][t_a++] = UV->tangent[3];
                    }
                }
            }
        }
    }
}

void fill_in_VertCoords_Fan(camera * C, int E)
{
    int p, e, o, v, idx;

    object * O;
    polygon * P;
    vertex * V;
    uv * UV;

    int v_c;
    int n_c;
    int t_a;
//    int t_c;

    for (o = 0; o < C->object_count; o++)
    {
        idx = C->objects[o];
        O = objects[idx];
        if (O->vertex_array)
        {
            v_c = 0;
            n_c = 0;
            t_a = 0;
//            t_c = 0;

            if (E)
            {
                for (v = 0; v < O->textcount; v ++)
                {
                    UV = &O->uvtex[v / ARRAYSIZE][v % ARRAYSIZE];

                    idx = UV->vert;
                    V = &O->verts[idx / ARRAYSIZE][idx % ARRAYSIZE];
                    O->vert_array[1][v_c++] = V->Tx;
                    O->vert_array[1][v_c++] = V->Ty;
                    O->vert_array[1][v_c++] = V->Tz;
                    O->norm_array[1][n_c++] = V->N.Tx;
                    O->norm_array[1][n_c++] = V->N.Ty;
                    O->norm_array[1][n_c++] = V->N.Tz;
                    O->tang_array[1][t_a++] = UV->tangent[0];
                    O->tang_array[1][t_a++] = UV->tangent[1];
                    O->tang_array[1][t_a++] = UV->tangent[2];
                    O->tang_array[1][t_a++] = UV->tangent[3];
                }
            }
            else
            {
                for (p = 0; p < O->polycount; p ++)
                {
                    P = &O->polys[p / ARRAYSIZE][p % ARRAYSIZE];

                    for (e = 0; e < P->edgecount; e ++)
                    {
                        idx = P->verts[e];
                        V = &O->verts[idx / ARRAYSIZE][idx % ARRAYSIZE];
                        idx = P->texts[e];
                        UV = &O->uvtex[idx / ARRAYSIZE][idx % ARRAYSIZE];
                        O->vert_array[0][v_c++] = V->Tx;
                        O->vert_array[0][v_c++] = V->Ty;
                        O->vert_array[0][v_c++] = V->Tz;

                        if (O->smooth)
                        {
                            O->norm_array[0][n_c++] = V->N.Tx;
                            O->norm_array[0][n_c++] = V->N.Ty;
                            O->norm_array[0][n_c++] = V->N.Tz;
                        }
                        else
                        {
                            O->norm_array[0][n_c++] = P->N.Tx;
                            O->norm_array[0][n_c++] = P->N.Ty;
                            O->norm_array[0][n_c++] = P->N.Tz;
                        }
//                        O->text_array[t_c++] = UV->u;
//                        O->text_array[t_c++] = UV->v;
                        O->tang_array[0][t_a++] = UV->tangent[0];
                        O->tang_array[0][t_a++] = UV->tangent[1];
                        O->tang_array[0][t_a++] = UV->tangent[2];
                        O->tang_array[0][t_a++] = UV->tangent[3];
                    }
                }
            }
        }
    }
}

void fill_in_VertCoords_quads_Object_Thumb(object * O, int l, int E)
{
    int e, q, v, idx, L;

    quadrant * Q;
    vertex * V;
    uv * UV;

    if (l > O->subdlevel)
    {
        L = O->subdlevel;
    }
    else
    {
        L = l;
    }
    if (O->vertex_arrays[L])
    {
        int v_c = 0;
        int n_c = 0;
        //int t_a = 0;

        if (E)
        {
            for (v = 0; v < O->textcount_[L]; v ++)
            {
                UV = &O->uvtex_[L][v / ARRAYSIZE][v % ARRAYSIZE];

                idx = UV->vert;
                V = &O->verts_[L][idx / ARRAYSIZE][idx % ARRAYSIZE];
                O->vert_array_[L][1][v_c++] = V->x;
                O->vert_array_[L][1][v_c++] = V->y;
                O->vert_array_[L][1][v_c++] = V->z;
                O->norm_array_[L][1][n_c++] = V->N.x;
                O->norm_array_[L][1][n_c++] = V->N.y;
                O->norm_array_[L][1][n_c++] = V->N.z;
//                O->tang_array_[L][1][t_a++] = UV->tangent[0];
//                O->tang_array_[L][1][t_a++] = UV->tangent[1];
//                O->tang_array_[L][1][t_a++] = UV->tangent[2];
//                O->tang_array_[L][1][t_a++] = UV->tangent[3];
            }
        }
        else
        {
            for (q = 0; q < O->quadcount_[L]; q++)
            {
                Q = &O->quads_[L][q / ARRAYSIZE][q % ARRAYSIZE];

                for (e = 0; e < 4; e ++)
                {
                    idx = Q->verts[e];
                    V = &O->verts_[L][idx / ARRAYSIZE][idx % ARRAYSIZE];
                    idx = Q->texts[e];
                    UV = &O->uvtex_[L][idx / ARRAYSIZE][idx % ARRAYSIZE];
                    O->vert_array_[L][0][v_c++] = V->x;
                    O->vert_array_[L][0][v_c++] = V->y;
                    O->vert_array_[L][0][v_c++] = V->z;
                    O->norm_array_[L][0][n_c++] = V->N.x;
                    O->norm_array_[L][0][n_c++] = V->N.y;
                    O->norm_array_[L][0][n_c++] = V->N.z;
    //                    O->text_array_[L][t_c++] = UV->u;
    //                    O->text_array_[L][t_c++] = UV->v;
//                    O->tang_array_[L][0][t_a++] = UV->tangent[0];
//                    O->tang_array_[L][0][t_a++] = UV->tangent[1];
//                    O->tang_array_[L][0][t_a++] = UV->tangent[2];
//                    O->tang_array_[L][0][t_a++] = UV->tangent[3];
                }
            }
        }
    }
}

void fill_in_VertCoords_quads_Object(object * O, int l, int E)
{
    int e, q, v, idx, L;

    quadrant * Q;
    vertex * V;
    uv * UV;

    if (l > O->subdlevel)
    {
        L = O->subdlevel;
    }
    else
    {
        L = l;
    }
    if (O->vertex_arrays[L])
    {
        int v_c = 0;
        int n_c = 0;
        int t_a = 0;

        if (E)
        {
            for (v = 0; v < O->textcount_[L]; v ++)
            {
                UV = &O->uvtex_[L][v / ARRAYSIZE][v % ARRAYSIZE];

                idx = UV->vert;
                V = &O->verts_[L][idx / ARRAYSIZE][idx % ARRAYSIZE];
                O->vert_array_[L][1][v_c++] = V->Tx;
                O->vert_array_[L][1][v_c++] = V->Ty;
                O->vert_array_[L][1][v_c++] = V->Tz;
                O->norm_array_[L][1][n_c++] = V->N.Tx;
                O->norm_array_[L][1][n_c++] = V->N.Ty;
                O->norm_array_[L][1][n_c++] = V->N.Tz;
                O->tang_array_[L][1][t_a++] = UV->tangent[0];
                O->tang_array_[L][1][t_a++] = UV->tangent[1];
                O->tang_array_[L][1][t_a++] = UV->tangent[2];
                O->tang_array_[L][1][t_a++] = UV->tangent[3];
            }
        }
        else
        {
            for (q = 0; q < O->quadcount_[L]; q++)
            {
                Q = &O->quads_[L][q / ARRAYSIZE][q % ARRAYSIZE];

                for (e = 0; e < 4; e ++)
                {
                    idx = Q->verts[e];
                    V = &O->verts_[L][idx / ARRAYSIZE][idx % ARRAYSIZE];
                    idx = Q->texts[e];
                    UV = &O->uvtex_[L][idx / ARRAYSIZE][idx % ARRAYSIZE];
                    O->vert_array_[L][0][v_c++] = V->Tx;
                    O->vert_array_[L][0][v_c++] = V->Ty;
                    O->vert_array_[L][0][v_c++] = V->Tz;

                    if (O->smooth)
                    {
                        O->norm_array_[L][0][n_c++] = V->N.Tx;
                        O->norm_array_[L][0][n_c++] = V->N.Ty;
                        O->norm_array_[L][0][n_c++] = V->N.Tz;
                    }
                    else
                    {
                        O->norm_array_[L][0][n_c++] = Q->N.Tx;
                        O->norm_array_[L][0][n_c++] = Q->N.Ty;
                        O->norm_array_[L][0][n_c++] = Q->N.Tz;
                    }
    //                    O->text_array_[L][t_c++] = UV->u;
    //                    O->text_array_[L][t_c++] = UV->v;
                    O->tang_array_[L][0][t_a++] = UV->tangent[0];
                    O->tang_array_[L][0][t_a++] = UV->tangent[1];
                    O->tang_array_[L][0][t_a++] = UV->tangent[2];
                    O->tang_array_[L][0][t_a++] = UV->tangent[3];
                }
            }
        }
    }
}

void fill_in_VertCoords_quads(camera * C, int l, int E)
{
    int q, e, o, v, idx, L;

    object * O;
    quadrant * Q;
    vertex * V;
    uv * UV;

    for (o = 0; o < C->object_count; o++)
    {
        idx = C->objects[o];
        O = objects[idx];
        if (l > O->subdlevel)
        {
            L = O->subdlevel;
        }
        else
        {
            L = l;
        }
        if (O->vertex_arrays[L])
        {
            int v_c = 0;
            int n_c = 0;
            int t_a = 0;

            if (E)
            {
                for (v = 0; v < O->textcount_[L]; v ++)
                {
                    UV = &O->uvtex_[L][v / ARRAYSIZE][v % ARRAYSIZE];

                    idx = UV->vert;
                    V = &O->verts_[L][idx / ARRAYSIZE][idx % ARRAYSIZE];
                    O->vert_array_[L][1][v_c++] = V->Tx;
                    O->vert_array_[L][1][v_c++] = V->Ty;
                    O->vert_array_[L][1][v_c++] = V->Tz;
                    O->norm_array_[L][1][n_c++] = V->N.Tx;
                    O->norm_array_[L][1][n_c++] = V->N.Ty;
                    O->norm_array_[L][1][n_c++] = V->N.Tz;
                    O->tang_array_[L][1][t_a++] = UV->tangent[0];
                    O->tang_array_[L][1][t_a++] = UV->tangent[1];
                    O->tang_array_[L][1][t_a++] = UV->tangent[2];
                    O->tang_array_[L][1][t_a++] = UV->tangent[3];
                }
            }
            else
            {
                for (q = 0; q < O->quadcount_[L]; q++)
                {
                    Q = &O->quads_[L][q / ARRAYSIZE][q % ARRAYSIZE];

                    for (e = 0; e < 4; e ++)
                    {
                        idx = Q->verts[e];
                        V = &O->verts_[L][idx / ARRAYSIZE][idx % ARRAYSIZE];
                        idx = Q->texts[e];
                        UV = &O->uvtex_[L][idx / ARRAYSIZE][idx % ARRAYSIZE];
                        O->vert_array_[L][0][v_c++] = V->Tx;
                        O->vert_array_[L][0][v_c++] = V->Ty;
                        O->vert_array_[L][0][v_c++] = V->Tz;

                        if (O->smooth)
                        {
                            O->norm_array_[L][0][n_c++] = V->N.Tx;
                            O->norm_array_[L][0][n_c++] = V->N.Ty;
                            O->norm_array_[L][0][n_c++] = V->N.Tz;
                        }
                        else
                        {
                            O->norm_array_[L][0][n_c++] = Q->N.Tx;
                            O->norm_array_[L][0][n_c++] = Q->N.Ty;
                            O->norm_array_[L][0][n_c++] = Q->N.Tz;
                        }
    //                    O->text_array_[L][t_c++] = UV->u;
    //                    O->text_array_[L][t_c++] = UV->v;
                        O->tang_array_[L][0][t_a++] = UV->tangent[0];
                        O->tang_array_[L][0][t_a++] = UV->tangent[1];
                        O->tang_array_[L][0][t_a++] = UV->tangent[2];
                        O->tang_array_[L][0][t_a++] = UV->tangent[3];
                    }
                }
            }
        }
    }
}

void render_uv_view_(camera * C, int index, int l, int normalMode, int bumpMode)
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, C->h_v_ratio, 0, 1, 1, -1);

//    GLfloat gray[4] = {1, 0.5, 0.5, 0.5};
	GLfloat white[4] = {1, 1, 1, 1};
	GLfloat yellow[4] = {1, 1, 0, 1};
	GLfloat blueish[4] = {0.4, 0.9, 1, 1};

	int p, v, e, idx;

	object * O = objects[index];
	quadrant * Q;
	polygon * P;
	uv_edge * UVE, * UVE0;
	uv * UV;

    int L, l0;

    if (l > O->subdlevel)
    {
        L = O->subdlevel;
    }
    else
    {
        L = l;
    }

	glDisable(GL_LIGHTING);
	glDisable(GL_DEPTH_TEST);

    glEnable(GL_TEXTURE_2D);

    if (normalMode)
    {
        glCallList(normalsLists[Materials[O->surface].Normal_idx % Normals_c]);
    }
    else if (bumpMode)
    {
        glCallList(bumpsLists[Materials[O->surface].Bump_idx % Bumps_c]);
    }
    else
    {
        glCallList(displayLists[Materials[O->surface].Texture_idx % Textures_c]);
    }

    glColor4fv(blueish);

	glBegin(GL_QUADS);
	glTexCoord2f(0, 0);
	glVertex2f(0, 0);
	glTexCoord2f(C->h_v_ratio, 0);
	glVertex2f(C->h_v_ratio, 0);
	glTexCoord2f(C->h_v_ratio, 1);
	glVertex2f(C->h_v_ratio, 1);
	glTexCoord2f(0, 1);
	glVertex2f(0, 1);
	glEnd();

	glDisable(GL_TEXTURE_2D);
	glDisable(GL_CULL_FACE);

	glColor4fv(blueish);
    glBegin(GL_QUADS);
    for (p = 0; p < O->polycount; p++)
    {
        P = &O->polys[p / ARRAYSIZE][p % ARRAYSIZE];
        if (P->selected)
        {
            for (e = 0; e < P->edgecount; e ++)
            {
                if (L == -1)
                {
                    idx = P->texts[e];
                    UV = &O->uvtex[idx / ARRAYSIZE][idx % ARRAYSIZE];
                    glVertex2f(UV->u, UV->v);
                }
                else
                {
                    idx = P->quads[e];
                    Q = &O->quads_[0][idx / ARRAYSIZE][idx % ARRAYSIZE];
                    render_UV_Quads_ID_recursive(O, Q, 0, L);
                }
            }
        }
    }
    glEnd();

    glEnable(GL_CULL_FACE);

	glColor4fv(white);

	for (e = 0; e < O->uvedcount; e++)
    {
        UVE = &O->uveds[e / ARRAYSIZE][e % ARRAYSIZE];
        if (UVE->split) glColor4fv(yellow);

        if (UVE->selected)
            glLineWidth(2);
        else
            glLineWidth(1);

        glBegin(GL_LINES);

        if (L == -1)
        {
            idx = UVE->uv_verts[0];
            UV = &O->uvtex[idx / ARRAYSIZE][idx % ARRAYSIZE];
            glVertex2f(UV->u, UV->v);
            idx = UVE->uv_verts[1];
            UV = &O->uvtex[idx / ARRAYSIZE][idx % ARRAYSIZE];
            glVertex2f(UV->u, UV->v);
        }
        else
        {
            idx = UVE->uv_edges[0];
            UVE0 = &O->uveds_[0][idx / ARRAYSIZE][idx % ARRAYSIZE];
            draw_UVEdge_recursive(O, UVE0, 0, L);
            idx = UVE->uv_edges[1];
            UVE0 = &O->uveds_[0][idx / ARRAYSIZE][idx % ARRAYSIZE];
            draw_UVEdge_recursive(O, UVE0, 0, L);
        }

        glEnd();

        if (UVE->split) glColor4fv(white);
    }
    glLineWidth(1);
//    glBegin(GL_LINES);
//
//    if (O->subdlevel >= L)
//    {
//        glColor4fv(gray);
//        for (e = 0; e < O->uvedcount_[L]; e++)
//        {
//            UVE = &O->uveds_[L][e / ARRAYSIZE][e % ARRAYSIZE];
//            if (UVE->visible)
//            {
//                UVE->visible = 0;
//                continue;
//            }
//            idx = UVE->uv_verts[0];
//            UV = &O->uvtex_[L][idx / ARRAYSIZE][idx % ARRAYSIZE];
//            glVertex2f(UV->u, UV->v);
//            idx = UVE->uv_verts[1];
//            UV = &O->uvtex_[L][idx / ARRAYSIZE][idx % ARRAYSIZE];
//            glVertex2f(UV->u, UV->v);
//        }
//    }
//
//	glEnd();

    glColor4fv(black);
	for (v = 0; v < O->textcount; v ++)
    {
        UV = &O->uvtex[v / ARRAYSIZE][v % ARRAYSIZE];

        if (UV->selected) glPointSize(3);
        else glPointSize(1);

        glBegin(GL_POINTS);

        l0 = 0;

        while (UV != NULL && l0 <= L)
        {
            UV = UV->uv_vert;
            l0 ++;
        }

        glVertex2f(UV->u, UV->v);

        glEnd();
    }

	glEnable(GL_TEXTURE_2D);
    glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
}

void render_uv_view(camera * C, int index, int normalMode, int bumpMode)
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, C->h_v_ratio, 0, 1, 1, -1);

	GLfloat white[4] = {1, 1, 1, 1};
	GLfloat yellow[4] = {1, 1, 0, 1};
	GLfloat blueish[4] = {0.4, 0.9, 1, 1};

	int p, v, e, idx;

	object * O = objects[index];
	polygon * P;
	uv_edge * UVE;
	uv * UV;

	glDisable(GL_LIGHTING);
	glDisable(GL_DEPTH_TEST);

    glEnable(GL_TEXTURE_2D);
    glActiveTexture(GL_TEXTURE0);

    if (normalMode)
    {
        glCallList(normalsLists[Materials[O->surface].Normal_idx % Normals_c]);
    }
    else if (bumpMode)
    {
        glCallList(bumpsLists[Materials[O->surface].Bump_idx % Bumps_c]);
    }
    else
    {
        glCallList(displayLists[Materials[O->surface].Texture_idx % Textures_c]);
    }

    glColor4fv(blueish);

	glBegin(GL_QUADS);
	glTexCoord2f(0, 0);
	glVertex2f(0, 0);
	glTexCoord2f(C->h_v_ratio, 0);
	glVertex2f(C->h_v_ratio, 0);
	glTexCoord2f(C->h_v_ratio, 1);
	glVertex2f(C->h_v_ratio, 1);
	glTexCoord2f(0, 1);
	glVertex2f(0, 1);
	glEnd();

	glDisable(GL_TEXTURE_2D);
	glDisable(GL_CULL_FACE);

	glColor4fv(blueish);

    for (p = 0; p < O->polycount; p++)
    {
        P = &O->polys[p / ARRAYSIZE][p % ARRAYSIZE];
        if (P->selected)
        {
            glBegin(GL_TRIANGLE_FAN);
            for (e = 0; e < P->edgecount; e ++)
            {
                idx = P->texts[e];
                UV = &O->uvtex[idx / ARRAYSIZE][idx % ARRAYSIZE];
                glVertex2f(UV->u, UV->v);
            }
            glEnd();
        }
    }

    glEnable(GL_CULL_FACE);

	glColor4fv(white);

	for (e = 0; e < O->uvedcount; e ++)
    {
        UVE = &O->uveds[e / ARRAYSIZE][e % ARRAYSIZE];

        if (UVE->selected)
            glLineWidth(2);
        else
            glLineWidth(1);

        if (UVE->split) glColor4fv(yellow);

        glBegin(GL_LINES);

        idx = UVE->uv_verts[0];
        UV = &O->uvtex[idx / ARRAYSIZE][idx % ARRAYSIZE];
        glVertex2f(UV->u, UV->v);
        idx = UVE->uv_verts[1];
        UV = &O->uvtex[idx / ARRAYSIZE][idx % ARRAYSIZE];
        glVertex2f(UV->u, UV->v);

        glEnd();

        if (UVE->split) glColor4fv(white);
    }

    glLineWidth(1);

    glColor4fv(black);

	for (v = 0; v < O->textcount; v ++)
    {
        UV = &O->uvtex[v / ARRAYSIZE][v % ARRAYSIZE];

        if (UV->selected) glPointSize(3);
        else glPointSize(1);

        glBegin(GL_POINTS);

        glVertex2f(UV->u, UV->v);

        glEnd();
    }

	glEnable(GL_TEXTURE_2D);
    glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
}

void render_quads_OnScreen_Shadows(camera * C, int l, int currentObject, int mode, int Selection_Mode, int update_colors, int update_uv, int elem)
{
    int o, idx, L;
    object * O;

    glEnable(GL_DEPTH_TEST);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, 0);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, 0);
    glActiveTexture(GL_TEXTURE0);

    for (o = 0; o < C->object_count; o++)
    {
        idx = C->objects[o];
        O = objects[idx];

        glCallList(displayLists[Materials[O->surface].Texture_idx % Textures_c]);

        if (l > O->subdlevel)
        {
            L = O->subdlevel;
        }
        else
        {
            L = l;
        }

        if (O->shadow && O->vertex_arrays[L])
        {
            if (VAO && SHADERS)
            {
                if (SHADOWS)
                {
                    glUseProgram(T_program[4]);
                    glUniform1i(uniform_tex2, 0); // 0th texture
//                    glUniform1i(uniform_smap4, 1); // 1th texture
//                    glBindBufferBase(GL_UNIFORM_BUFFER, 0, lightHandle);
//                    glBindBufferBase(GL_UNIFORM_BUFFER, 1, fogHandle);
                    glUniformMatrix3fv(uniform_cam4, 1, GL_TRUE, (GLfloat*)C->T->rotVec_);
                    glUniformMatrix4fv(uniform_proj4, 1, GL_FALSE, projectionMatrix);
                    glUniformMatrix4fv(uniform_sha4, 1, GL_FALSE, shadowMatrix);
                    draw_Arrays_Shader(O, L, update_colors, update_uv, elem);
                    glUseProgram(0);
                }
            }
        }
    }
}

void render_polys_quads_OnScreen(camera * C, int wireframe, int edgedraw, int vertdraw, int l, int currentObject, int mode, int Selection_Mode, int update_colors, int update_uv, int elem)
{
    int level, v, o, p, q, e, idx, L;
    object * O;
    id_color * I;
    quadrant * Q;
    polygon * P;
    vertex * V;
    uv * UV;
    edge * E, * E0;

    surface_Material Material;

    Uint8 r, g, b, a;

    glEnable(GL_TEXTURE_2D);
    glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);

    for (o = 0; o < C->object_count; o++)
    {
        idx = C->objects[o];
        O = objects[idx];

        glActiveTexture(GL_TEXTURE0);
        glCallList(displayLists[Materials[O->surface].Texture_idx % Textures_c]);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, depthTex);

        glActiveTexture(GL_TEXTURE2);
        glCallList(normalsLists[Materials[O->surface].Normal_idx % Normals_c]);

        glActiveTexture(GL_TEXTURE3);
        glCallList(bumpsLists[Materials[O->surface].Bump_idx % Bumps_c]);

        glActiveTexture(GL_TEXTURE0);

        if (l > O->subdlevel)
        {
            L = O->subdlevel;
        }
        else
        {
            L = l;
        }

        if (L == -1)
        {
            if (O->vertex_array)
            {
                if (VAO && SHADERS && Fan_Arrays_Shader)
                {
                    if (mode == ID_RENDER)
                    {
                        glUseProgram(T_program[3]);
                        glUniformMatrix4fv(uniform_proj3, 1, GL_FALSE, projectionMatrix);

                        draw_Fan_Arrays_Shader_ID(O, update_colors, update_uv, elem);
                        glUseProgram(0);
                    }
                    else if (SHADOWS)
                    {
                        glUseProgram(T_program[4]);

                        glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 1, &pass2Index);

                        glUniform1i(uniform_tex2, 0); // 0th texture
                        glUniform1i(uniform_smap4, 1); // 0th texture
                        glUniform1i(uniform_nmap4, 2); // 0th texture
                        glUniform1i(uniform_bmap4, 3); // 0th texture
                        glBindBufferBase(GL_UNIFORM_BUFFER, 0, lightHandle);
                        glBindBufferBase(GL_UNIFORM_BUFFER, 1, fogHandle);
                        glUniformMatrix3fv(uniform_cam4, 1, GL_TRUE, (GLfloat*)C->T->rotVec_);
                        glUniformMatrix4fv(uniform_proj4, 1, GL_FALSE, projectionMatrix);
                        glUniformMatrix4fv(uniform_sha4, 1, GL_FALSE, shadowMatrix);
                        glUniform1i(uniform_night4, NIGHT);
                        glUniform1f(uniform_refls4, Reflect);
                        glUniform1f(uniform_shine4, Materials[O->surface].Shininess);
                        glUniform1f(uniform_displ4, Materials[O->surface].Displacement);
                        draw_Fan_Arrays_Shader(O, update_colors, update_uv, elem);
                        glUseProgram(0);
                    }
                    else
                    {
                        glUseProgram(T_program[2]);
                        glUniform1i(uniform_tex2, 0); // 0th texture
                        glBindBufferBase(GL_UNIFORM_BUFFER, 0, lightHandle);
                        glBindBufferBase(GL_UNIFORM_BUFFER, 1, fogHandle);
                        glUniformMatrix3fv(uniform_cam2, 1, GL_TRUE, (GLfloat*)C->T->rotVec_);
                        glUniformMatrix4fv(uniform_proj2, 1, GL_FALSE, projectionMatrix);
                        glUniform1i(uniform_night2, NIGHT);
                        glUniform1f(uniform_refls2, Reflect);
                        draw_Fan_Arrays_Shader(O, update_colors, update_uv, elem);
                        glUseProgram(0);
                    }
                }
                else
                {
                    draw_Fan_Arrays(O, mode, update_colors, update_uv, elem);
                }
            }
            else
            {
                for (p = 0; p < O->polycount; p++)
                {
                    P = &O->polys[p / ARRAYSIZE][p % ARRAYSIZE];

                    idx = P->surface;
                    Material = Materials[idx];

                    r = Material.RGBA.R;
                    g = Material.RGBA.G;
                    b = Material.RGBA.B;
                    a = Material.RGBA.A;

                    if (mode == ID_RENDER)
                    {
                        idx = P->color;
                        I = &O->icols[idx / ARRAYSIZE][idx % ARRAYSIZE];
                        glColor4f(I->R, I->G, I->B, I->A);
                    }
                    else
                    {
                        glColor4ub(r, g, b, a);
                    }

                    if (wireframe)
                    {
                        glBegin(GL_LINE_LOOP);
                        for (e = 0; e < P->edgecount; e ++)
                        {
                            idx = P->verts[e];
                            V = &O->verts[idx / ARRAYSIZE][idx % ARRAYSIZE];
                            glVertex3f(V->Tx, V->Ty, V->Tz);
                        }
                        glEnd();
                    }
                    else
                    {
                        glBegin(GL_TRIANGLE_FAN);
                        for (e = 0; e < P->edgecount; e ++)
                        {
                            idx = P->verts[e];
                            V = &O->verts[idx / ARRAYSIZE][idx % ARRAYSIZE];
                            idx = P->texts[e];
                            UV = &O->uvtex[idx / ARRAYSIZE][idx % ARRAYSIZE];
                            glNormal3f(V->N.Tx, V->N.Ty, V->N.Tz);
                            glTexCoord2f(UV->u, UV->v);
                            glVertex3f(V->Tx, V->Ty, V->Tz);
                        }
                        glEnd();
                    }
                }
            }

            if (edgedraw) // assuming some objects have no geometry on all levels.
            {
                glDisable(GL_TEXTURE_2D);
                //glDisable(GL_MULTISAMPLE);
                if (Selection_Mode == POLYS)
                    glEnable(GL_LINE_STIPPLE);
                if (C->objects[o] == currentObject && O->selected)
                {
                    if (Selection_Mode == POLYS)
                    glLineStipple(1, 0xEEEE);
                    else if (Selection_Mode == VERTS)
                    glLineWidth(1);
                    else if (Selection_Mode == EDGES)
                    glLineWidth(1);
                    else
                    glLineWidth(2);
                    if (O->deforms)
                    line_color = &current_deform_color[0];
                    else
                    line_color = &current_color[0];
                }
                else if (O->selected)
                {
                    if (Selection_Mode == POLYS)
                    glLineStipple(1, 0xEEEE);
                    else if (Selection_Mode == VERTS)
                    glLineWidth(1);
                    else
                    glLineWidth(1);
                    if (O->deforms)
                    line_color = &selected_deform_color[0];
                    else
                    line_color = &selected_color[0];
                }
                else
                {
                    glLineStipple(1, 0xFFFF);
                    glLineWidth(1);
                    if (O->deforms)
                    line_color = &dark_deform_color[0];
                    else
                    line_color = &dark_wire_color[0];
                }
                if (Selection_Mode != OBJES)
                {
                    memcpy(Line_Color, line_color, sizeof Line_Color);
                    for (e = 0; e < O->edgecount; e++)
                    {
                        E = &O->edges[e / ARRAYSIZE][e % ARRAYSIZE];
                        if (E->selected)
                        {
                            glDisable(GL_LIGHTING);
    //                        if (O->selected)
    //                            glLineWidth(2);
    //                        else
    //                            glLineWidth(1);
                            line_color = &selected_edge_color[0];
                        }
                        else if (!glIsEnabled(GL_LIGHTING))
                        {
                            glEnable(GL_LIGHTING);
    //                        glLineWidth(1);
                            line_color = Line_Color;
                        }
                        draw_Edge(O, E, C, wireframe, E->B.Tradius);
                    }
                }
                else
                {
                    //if (C->objects[o] == currentObject || O->selected)
                    //{
                        render_Outline_polys(O, C);
                    //}
    //                else
    //                {
    //                    for (e = 0; e < O->edgecount; e ++)
    //                    {
    //                        E = &O->edges[e / ARRAYSIZE][e % ARRAYSIZE];
    //                        draw_Edge(O, E, C, wireframe, E->B.Tradius);
    //                    }
    //                }
                }
                glEnable(GL_TEXTURE_2D);
                //glEnable(GL_MULTISAMPLE);
                glEnable(GL_LIGHTING);
                glDisable(GL_LINE_STIPPLE);
                glLineWidth(1);
            }

            if (vertdraw)
            {
                if (C->objects[o] == currentObject)
                glPointSize(3);
                else
                glPointSize(2);
                glDisable(GL_TEXTURE_2D);
                glDisable(GL_LIGHTING);
                glBegin(GL_POINTS);
                glColor4ubv(vert_color);

                float factor;
                float dot;
                float view_minor;

                aim Aim;
                normal Normal;

                if (C->view_minor > 1)
                    view_minor = C->view_minor;
                else
                    view_minor = 1;

                for (v = 0; v < O->vertcount; v ++)
                {
                    V = &O->verts[v / ARRAYSIZE][v % ARRAYSIZE];

                    if (V->selected)
                    {
                        glColor4ubv(selected_vert_color);
                    }
                    else
                    {
                        glColor4ubv(vert_color);
                    }

                    Normal.x = -V->N.Tx;
                    Normal.y = -V->N.Ty;
                    Normal.z = -V->N.Tz;

                    Aim = vector3d_T(V, C->T->pos);

                    if (C->ortho)
                    {
                        dot = dot_productN(&Normal, C->T->aim);
                    }
                    else
                    {
                        dot = dot_productN(&Normal, Aim.vec);
                    }

                    if (dot < 0)
                        continue;

                    if (C->ortho)
                    {
                        factor = Aim.dist * view_minor * 0.002 * dot;
                        glVertex3f(V->Tx + V->N.Tx * factor, V->Ty + V->N.Ty * factor, V->Tz + V->N.Tz * factor);
                    }
                    else
                    {
                        factor = Aim.dist * view_minor * 0.3 * O->mean_Edge * dot;
                        if (factor > Aim.dist / 2)
                        {
                            factor = Aim.dist / 2;
                        }
                        glVertex3f(V->Tx - Aim.vec[0] * factor, V->Ty - Aim.vec[1] * factor, V->Tz - Aim.vec[2] * factor);
                    }
                }
                glEnd();
                glEnable(GL_LIGHTING);
                glEnable(GL_TEXTURE_2D);
            }
        }
        else if (O->vertex_arrays[L])
        {
            if (VAO && SHADERS)
            {
                if (mode == ID_RENDER)
                {
                    glUseProgram(T_program[3]);
                    glUniformMatrix4fv(uniform_proj3, 1, GL_FALSE, projectionMatrix);
                    draw_Arrays_Shader_ID(O, L, update_colors, update_uv, elem);
                    glUseProgram(0);
                }
                else if (SHADOWS)
                {
                    glUseProgram(T_program[4]);

                    glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 1, &pass2Index);

                    glUniform1i(uniform_tex2, 0); // 0th texture
                    glUniform1i(uniform_smap4, 1); // 0th texture
                    glUniform1i(uniform_nmap4, 2); // 0th texture
                    glUniform1i(uniform_bmap4, 3); // 0th texture
                    glBindBufferBase(GL_UNIFORM_BUFFER, 0, lightHandle);
                    glBindBufferBase(GL_UNIFORM_BUFFER, 1, fogHandle);
                    //glBindBufferBase(GL_UNIFORM_BUFFER, 2, materialSHandle3);
                    glUniformMatrix3fv(uniform_cam4, 1, GL_TRUE, (GLfloat*)C->T->rotVec_);
                    glUniformMatrix4fv(uniform_proj4, 1, GL_FALSE, projectionMatrix);
                    glUniformMatrix4fv(uniform_sha4, 1, GL_FALSE, shadowMatrix);
                    glUniform1i(uniform_night4, NIGHT);
                    glUniform1f(uniform_refls4, Reflect);
                    glUniform1f(uniform_shine4, Materials[O->surface].Shininess);
                    glUniform1f(uniform_displ4, Materials[O->surface].Displacement);
                    draw_Arrays_Shader(O, L, update_colors, update_uv, elem);
                    glUseProgram(0);
                }
                else
                {
                    glUseProgram(T_program[2]);
                    glUniform1i(uniform_tex2, 0); // 0th texture
                    glBindBufferBase(GL_UNIFORM_BUFFER, 0, lightHandle);
                    glBindBufferBase(GL_UNIFORM_BUFFER, 1, fogHandle);
                    glUniformMatrix3fv(uniform_cam2, 1, GL_TRUE, (GLfloat*)C->T->rotVec_);
                    glUniformMatrix4fv(uniform_proj2, 1, GL_FALSE, projectionMatrix);
                    glUniform1i(uniform_night2, NIGHT);
                    glUniform1f(uniform_refls2, Reflect);
                    draw_Arrays_Shader(O, L, update_colors, update_uv, elem);
                    glUseProgram(0);
                }
            }
            else
            {
                draw_Arrays(O, L, mode, update_colors, update_uv, elem);
            }
        }
        else if (O->subdlevel >= L)
        {
            for (q = 0; q < O->quadcount_[L]; q++)
            {
                Q = &O->quads_[L][q / ARRAYSIZE][q % ARRAYSIZE];

                idx = Q->surface;
                Material = Materials[idx];

                r = Material.RGBA.R;
                g = Material.RGBA.G;
                b = Material.RGBA.B;
                a = Material.RGBA.A;

                glColor4ub(r, g, b, a); // this call maybe can effect if fragments are implemented to blend it in.

                glBegin(GL_QUADS);
                for (e = 0; e < 4; e ++)
                {
                    idx = Q->verts[e];
                    V = &O->verts_[L][idx / ARRAYSIZE][idx % ARRAYSIZE];
                    idx = Q->texts[e];
                    UV = &O->uvtex_[L][idx / ARRAYSIZE][idx % ARRAYSIZE];
                    glTexCoord2f(UV->u, UV->v);
                    glNormal3f(V->N.Tx, V->N.Ty, V->N.Tz);
                    glVertex3f(V->Tx, V->Ty, V->Tz);
                }
                glEnd();
            }
        }

        if (edgedraw && O->subdlevel >= 0) // assuming some objects have no geometry on all levels.
        {
            glDisable(GL_TEXTURE_2D);
            //glDisable(GL_MULTISAMPLE);
            if (Selection_Mode == POLYS)
                glEnable(GL_LINE_STIPPLE);
            if (C->objects[o] == currentObject && O->selected)
            {
                if (Selection_Mode == POLYS)
                glLineStipple(1, 0xEEEE);
                else if (Selection_Mode == VERTS)
                glLineWidth(1);
                else if (Selection_Mode == EDGES)
                glLineWidth(1);
                else
                glLineWidth(2);
                if (O->deforms)
                line_color = &current_deform_color[0];
                else
                line_color = &current_color[0];
            }
            else if (O->selected)
            {
                if (Selection_Mode == POLYS)
                glLineStipple(1, 0xEEEE);
                else if (Selection_Mode == VERTS)
                glLineWidth(1);
                else
                glLineWidth(1);
                if (O->deforms)
                line_color = &selected_deform_color[0];
                else
                line_color = &selected_color[0];
            }
            else
            {
                glLineWidth(1);
                glLineStipple(1, 0xFFFF);
                if (O->deforms)
                line_color = &deform_color[0];
                else
                line_color = &wire_color[0];
            }
            if (Selection_Mode != OBJES)
            {
                memcpy(Line_Color, line_color, sizeof Line_Color);
                for (e = 0; e < O->edgecount; e++)
                {
                    E = &O->edges[e / ARRAYSIZE][e % ARRAYSIZE];
                    if (E->selected)
                    {
                        glDisable(GL_LIGHTING);
//                        if (O->selected)
//                            glLineWidth(2);
//                        else
//                            glLineWidth(1);
                        line_color = &selected_edge_color[0];
                    }
                    else if (!glIsEnabled(GL_LIGHTING))
                    {
                        glEnable(GL_LIGHTING);
//                        glLineWidth(1);
                        line_color = Line_Color;
                    }
                    idx = E->edges[0];
                    E0 = &O->edges_[0][idx / ARRAYSIZE][idx % ARRAYSIZE];
                    draw_Edge_recursive(O, E0, 0, L, C, wireframe, E->B.Tradius);
                    idx = E->edges[1];
                    E0 = &O->edges_[0][idx / ARRAYSIZE][idx % ARRAYSIZE];
                    draw_Edge_recursive(O, E0, 0, L, C, wireframe, E->B.Tradius);
                }
            }
            else
            {
                if (C->objects[o] == currentObject || O->selected)
                {
                    render_Outline_quads(O, C, L);
                }
                else
                {
                    for (e = 0; e < O->edgecount; e++)
                    {
                        E = &O->edges[e / ARRAYSIZE][e % ARRAYSIZE];
                        idx = E->edges[0];
                        E0 = &O->edges_[0][idx / ARRAYSIZE][idx % ARRAYSIZE];
                        draw_Edge_recursive(O, E0, 0, L, C, wireframe, E->B.Tradius);
                        idx = E->edges[1];
                        E0 = &O->edges_[0][idx / ARRAYSIZE][idx % ARRAYSIZE];
                        draw_Edge_recursive(O, E0, 0, L, C, wireframe, E->B.Tradius);
                    }
                }
            }
            glEnable(GL_TEXTURE_2D);
            //glEnable(GL_MULTISAMPLE);
            glEnable(GL_LIGHTING);
            glLineWidth(1);
            glDisable(GL_LINE_STIPPLE);
        }
        if (vertdraw)
        {
            if (C->objects[o] == currentObject)
            glPointSize(3);
            else
            glPointSize(2);
            glDisable(GL_TEXTURE_2D);
            glDisable(GL_LIGHTING);
            glBegin(GL_POINTS);
            glColor4ubv(vert_color);

            float factor;
            float dot;
            float view_minor;

            aim Aim;
            normal Normal;

            if (C->view_minor > 1)
                view_minor = C->view_minor;
            else
                view_minor = 1;

            for (v = 0; v < O->vertcount; v ++)
            {
                V = &O->verts[v / ARRAYSIZE][v % ARRAYSIZE];

                if (V->selected)
                {
                    glColor4ubv(selected_vert_color);
                }
                else
                {
                    glColor4ubv(vert_color);
                }

                level = -1;

                while (V->vert != NULL && level < L)
                {
                    V = V->vert;
                    level ++;
                }


                Normal.x = -V->N.Tx;
                Normal.y = -V->N.Ty;
                Normal.z = -V->N.Tz;

                Aim = vector3d_T(V, C->T->pos);

                if (C->ortho)
                {
                    dot = dot_productN(&Normal, C->T->aim);
                }
                else
                {
                    dot = dot_productN(&Normal, Aim.vec);
                }

                if (dot < 0)
                    continue;

                if (C->ortho)
                {
                    factor = Aim.dist * view_minor * 0.002 * dot;
                    glVertex3f(V->Tx + V->N.Tx * factor, V->Ty + V->N.Ty * factor, V->Tz + V->N.Tz * factor);
                }
                else
                {
                    factor = Aim.dist * view_minor * 0.3 * O->mean_Edge * dot;
                    if (factor > Aim.dist / 2)
                    {
                        factor = Aim.dist / 2;
                    }
                    glVertex3f(V->Tx - Aim.vec[0] * factor, V->Ty - Aim.vec[1] * factor, V->Tz - Aim.vec[2] * factor);
                }
            }
            glEnd();
            glEnable(GL_LIGHTING);
            glEnable(GL_TEXTURE_2D);
        }
    }
}

void render_Void(unsigned char * data, int width, int height)
{
    int x, y;
    int c = 0;

    for (y = 0; y < height; y ++)
    {
        for (x = 0; x < width; x ++)
        {
            data[c++] = 0;
            data[c++] = 0;
            data[c++] = 0;
            data[c++] = 255;
        }
    }
}

#define NUM_THREADS 8
int EXIT_RENDER;

typedef struct
{
    unsigned char * data;
    camera * C;
    int L;
    int width;
    int height;
    int index;
    int finish;
}
render_arguments;

int Preak = 0;
int Finish = 0;

void * perform_work(void * arguments)
{
    //SDL_Event event;

    render_arguments * Args = (render_arguments *)arguments;

    normalizeF((float *)&light_vec);

    int x, y, index; //gx, gy;
    float R, l;
    pixel P;

    RGBA rgba;

    union Dir D = {{0.0, 0.0, -1.0}};
    float DDy;

    float H_Mark = Args->C->h_view / 2.0;
    float V_Mark = -Args->C->v_view / 2.0;
    float h_step = (Args->C->h_view / (float)Args->width);
    float H_step = h_step * NUM_THREADS;;
    float V_step = (Args->C->v_view / (float)Args->height);
    H_Mark -= h_step / 2.0;
    V_Mark += V_step / 2.0;
    H_Mark += pi;
    V_Mark += pi_2;

    float w_points = (float)Args->width / 2.0;
    float h_points = (float)Args->height / 2.0;
    float camdist = w_points / tan(Args->C->h_view / 2.0);

//    float Group_Width = Args->C->h_view / (float)OBJECT_GROUP_H;
//    float Group_Height = Args->C->v_view / (float)OBJECT_GROUP_V;
    float View_Span_H = 0.0;
    float View_Span_V = 0.0;

    float H_MARK = H_Mark - ((float)(Args->index) * h_step);

    char Hint[] = "rendering 100";
    char Percent[] = "100";

    HexG * G[HEXAS];
    int g_idx;
    HexPack HP;

    for (y = 0; y < Args->height; y ++)
    {
        R = sin(V_Mark);
        DDy = -cos(V_Mark);
        H_Mark = H_MARK;

//        gy = (int)(View_Span_V / Group_Height);
        View_Span_H = 0.0;

        for (x = Args->index; x < Args->width; x += NUM_THREADS)
        {
//            gx = (int)(View_Span_H / Group_Width);

//            if (!Args->index && !(y % 10))
//                printf(".");
            if (PLANAR)
            {
                D.D.x = x - w_points;
                D.D.y = y - h_points;
                D.D.z = -camdist;
                l = sqrt(D.D.x * D.D.x + D.D.y * D.D.y + D.D.z * D.D.z);
                D.D.x /= l;
                D.D.y /= l;
                D.D.z /= l;
            }
            else
            {
                D.D.y = DDy;
                D.D.x = sin(H_Mark) * R;
                D.D.z = cos(H_Mark) * R;
            }

            rotate_Vector(Args->C->T, -D.D.x, D.D.y, -D.D.z, &D.D); // direction is submitted from union
            index = y * Args->width * 4 + x * 4;

            g_idx = 0;
            HP = collect_Hexa_Groups(D.D, G, g_idx);

            if (Args->L == -1)
            {
                rgba = come_With_Pixel(&P, Args->C, &D.N, HP.G, HP.g_idx); // normal is submitted from union
            }
            else
            {
                rgba = come_With_Pixel_(&P, Args->C, &D.N, Args->L, HP.G, HP.g_idx);
            }

            Args->data[index] =     (unsigned char)rgba.R;
            Args->data[index + 1] = (unsigned char)rgba.G;
            Args->data[index + 2] = (unsigned char)rgba.B;
            Args->data[index + 3] = (unsigned char)rgba.A;

            if (Preak)
            {
                break;
            }

            H_Mark -= H_step;
            View_Span_H += H_step;
        }

        if (!Args->index && !(y % 10))
        {
            sprintf(Percent, "%3d", (int)(((float)y / (float)Args->height) * 100));
            sprintf(Hint, "rendering %s", Percent);
            printf("%s", Hint);
            printf("\r");
            //printf("\n");
        }

        if (Preak)
        {
            break;
        }

        V_Mark += V_step;
        View_Span_V += V_step;
    }

    if (!Args->index)
    {
        sprintf(Percent, "%3d", 100);
        sprintf(Hint, "rendering %s", Percent);
        printf("%s", Hint);
        printf("\r");
    }

    Finish += 1;

    return NULL;
}

void render_Image(unsigned char * data, camera * C, int width, int height, int L, int Anim_frame)
{
    light_vec[0] = -light.Position[0];
    light_vec[1] = -light.Position[1];
    light_vec[2] = -light.Position[2];

    printf("Rendering started\npress ESC to quit and save\n");
    clock_t begin_ = clock();

    pthread_t threads[NUM_THREADS];
    render_arguments thread_args[NUM_THREADS];

    int result_code;

    int t;

    double time_spent;

    EXIT_RENDER = 0;

    if (!Anim_frame)
    {
        Preak = 0;
    }

    Finish = 0;

    for (t = 0; t < NUM_THREADS; t ++)
    {
        thread_args[t].data = data;
        thread_args[t].C = C;
        thread_args[t].L = L;
        thread_args[t].width = width;
        thread_args[t].height = height;
        thread_args[t].index = t;
        thread_args[t].finish = 0;
        result_code = pthread_create(&threads[t], NULL, perform_work, &thread_args[t]);
        pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
        assert(!result_code);
    }

    while (!Preak)
    {
        if (SDL_PollEvent(&event))
        {
            if (event.type == SDL_KEYUP)
            {
                if (event.key.keysym.sym == SDLK_ESCAPE)
                {
                    Preak = 1;
                }
            }
        }

        if (Finish == NUM_THREADS -1)
        {
            break;
        }
    }

    for (t = 0; t < NUM_THREADS; t ++)
    {
        result_code = pthread_join(threads[t], NULL);
        assert(!result_code);
    }

    clock_t end_ = clock();
    time_spent = (double)(end_ - begin_) / CLOCKS_PER_SEC;

    printf("\ntime spent %f\n", time_spent);
}

int makepath(char * file_path, mode_t mode);

void render_and_save_Image(char * Path, camera * C, int width, int height, int L, int Anim_frame)
{
    ILuint ImgId = 0;

	ilGenImages(1, &ImgId);

	ilBindImage(ImgId);

    void * Data = malloc(width * height * 4 * sizeof(unsigned char));

    if (Data != NULL)
    {
        render_Image(Data, C, width, height, L, Anim_frame);

        //ilLoadL(IL_TYPE_UNKNOWN, Data, width * height * 4 * 8);

        ilTexImage(width, height, 0, 4, IL_RGBA, IL_UNSIGNED_BYTE, Data);

        int result = 0;

        if (!isDirectory(renderfiles))
        {
            printf("making directory\n");
            result = makepath(renderfiles, 0777);
        }
        else
        {
            result = 1;
        }

        printf("%d\n", result);

        if (result && !Preak)
        {
            if (isFile(Path))
            {
                remove(Path);
            }

            ilSave(IL_PNG, Path);

            printf("image saved to %s\n", Path);
        }

        free(Data);
    }
    else
    {
        printf("Render: Not enough memory!\n");
    }

    ilDeleteImages(1, &ImgId);
}

void draw_Curve_Segment_Recursive_ID(curve_segment * S, int level)
{
    if (S->level + 1 == level)
    {
        glBegin(GL_LINE_STRIP);
        glVertex3fv(S->A);
        glVertex3fv(S->B);
        glVertex3fv(S->C);
        glEnd();
    }
    else if (S->subdivided)
    {
        draw_Curve_Segment_Recursive_ID(S->segment[0], level);
        draw_Curve_Segment_Recursive_ID(S->segment[1], level);
    }
}

void draw_Curve_Segment_Recursive(curve_segment * S, int level)
{
    if (S->level + 1 == level)
    {
        glBegin(GL_LINE_STRIP);
        glVertex3fv(S->A);
        glVertex3fv(S->B);
        glVertex3fv(S->C);
        glEnd();
    }
    else if (S->subdivided)
    {
        draw_Curve_Segment_Recursive(S->segment[0], level);
        draw_Curve_Segment_Recursive(S->segment[1], level);
    }
}

void render_Curves_ID()
{
    int M0 = 255 * 255 * 255;
    int M1 = 255 * 255;

    float R, G, B, A;

    glDisable(GL_LIGHTING);
    glDisable(GL_DITHER);
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);
    glDisable(GL_MULTISAMPLE);

    int c, p, idx, idx0, idx1, idx2;
    curve * C;
    cp * CP;

    glLineWidth(1);

    for (c = 0; c < curvesIndex; c ++)
    {
        C = curves[c];

        if (C->visible)
        {
            idx = C->index;

            R = (float)(idx / M0) / (float)255;
            idx0 = idx % M0;
            G = (float)(idx0 / M1) / (float)255;
            idx1 = idx0 % M1;
            B = (float)(idx1 / 255) / (float)255;
            idx2 = idx1 % 255;
            A = (float)(idx2) / (float)255;

            glColor4f(R, G, B, A);

            if (C->open)
                glBegin(GL_LINE_STRIP);
            else
                glBegin(GL_LINE_LOOP);
            for (p = 0; p < C->cps_count; p ++)
            {
                CP = C->cps[p];
                glVertex3fv(CP->pos);
            }
            glEnd();
        }
    }

    glEnable(GL_MULTISAMPLE);
    glEnable(GL_BLEND);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_DITHER);
    glEnable(GL_LIGHTING);
}

void render_Curves()
{
    glDisable(GL_TEXTURE_2D);

    int c, p;
    curve * C;
    cp * CP;

    for (c = 0; c < curvesIndex; c ++)
    {
        C = curves[c];

        if (C->visible)
        {
            if (C->selected)
            {
                glLineWidth(2);
                line_color = &selected_color[0];
                glDisable(GL_LIGHTING);
            }
            else
            {
                glLineWidth(1);
                line_color = &current_color[0];
                if (!glIsEnabled(GL_LIGHTING))
                    glEnable(GL_LIGHTING);
            }
            if (C->open)
                glBegin(GL_LINE_STRIP);
            else
                glBegin(GL_LINE_LOOP);

            for (p = 0; p < C->cps_count; p ++)
            {
                if (C->segments[p]->selected)
                {
                    glColor4ubv(segment_color);
                }
                else
                {
                    glColor4ubv(line_color);
                }
                CP = C->cps[p];
                glVertex3fv(CP->pos);
            }
            glEnd();
        }
    }
    glEnable(GL_TEXTURE_2D);
    //glEnable(GL_MULTISAMPLE);
    glEnable(GL_LIGHTING);
    glDisable(GL_LINE_STIPPLE);
    glLineWidth(1);
}

void render_Curves_ID_(int level)
{
    int M0 = 255 * 255 * 255;
    int M1 = 255 * 255;

    float R, G, B, A;

    glDisable(GL_LIGHTING);
    glDisable(GL_DITHER);
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);
    glDisable(GL_MULTISAMPLE);

    int c, p, s, idx, idx0, idx1, idx2, L;

    object * O;
    curve * C;
    curve_segment * S;
    cp * CP;

    glLineWidth(1);

    for (c = 0; c < curvesIndex; c ++)
    {
        C = curves[c];

        O = C->O;

        if (O != NULL && level > O->subdlevel)
        {
            L = O->subdlevel;
        }
        else
        {
            L = level;
        }

        if (C->visible)
        {
            idx = C->index;

            R = (float)(idx / M0) / (float)255;
            idx0 = idx % M0;
            G = (float)(idx0 / M1) / (float)255;
            idx1 = idx0 % M1;
            B = (float)(idx1 / 255) / (float)255;
            idx2 = idx1 % 255;
            A = (float)(idx2) / (float)255;

            glColor4f(R, G, B, A);

            if (L == -1)
            {
                if (C->open)
                    glBegin(GL_LINE_STRIP);
                else
                    glBegin(GL_LINE_LOOP);
                for (p = 0; p < C->cps_count; p ++)
                {
                    CP = C->cps[p];
                    glVertex3fv(CP->pos);
                }
                glEnd();
            }
            else
            {
                if (C->open)
                {
                    for (s = 0; s < C->segment_count - 1; s ++)
                    {
                        S = C->segments[s];
                        draw_Curve_Segment_Recursive_ID(S, L);
                    }
                }
                else
                {
                    for (s = 0; s < C->segment_count; s ++)
                    {
                        S = C->segments[s];
                        draw_Curve_Segment_Recursive_ID(S, L);
                    }
                }
            }
        }
    }

    glEnable(GL_MULTISAMPLE);
    glEnable(GL_BLEND);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_DITHER);
    glEnable(GL_LIGHTING);
}

void render_Curves_(int level)
{
    glDisable(GL_TEXTURE_2D);

    int p, c, s, L;

    object * O;
    curve * C;
    cp * CP;
    curve_segment * S;

    for (c = 0; c < curvesIndex; c ++)
    {
        C = curves[c];

        O = C->O;

        if (O != NULL && level > O->subdlevel)
        {
            L = O->subdlevel;
        }
        else
        {
            L = level;
        }

        if (C->visible)
        {
            if (C->selected)
            {
                glLineWidth(2);
                line_color = &selected_color[0];
                glDisable(GL_LIGHTING);
            }
            else
            {
                glLineWidth(1);
                line_color = &current_color[0];
                if (!glIsEnabled(GL_LIGHTING))
                    glEnable(GL_LIGHTING);
            }

            if (L == -1)
            {
                if (C->open)
                    glBegin(GL_LINE_STRIP);
                else
                    glBegin(GL_LINE_LOOP);

                for (p = 0; p < C->cps_count; p ++)
                {
                    if (C->segments[p]->selected)
                    {
                        glColor4ubv(segment_color);
                    }
                    else
                    {
                        glColor4ubv(line_color);
                    }
                    CP = C->cps[p];
                    glVertex3fv(CP->pos);
                }
                glEnd();
            }
            else
            {
                if (C->open)
                {
                    for (s = 0; s < C->segment_count - 1; s ++)
                    {
                        S = C->segments[s];

                        if (S->selected)
                        {
                            glColor4ubv(segment_color);
                        }
                        else
                        {
                            glColor4ubv(line_color);
                        }

                        draw_Curve_Segment_Recursive(S, L);
                    }
                }
                else
                {
                    for (s = 0; s < C->segment_count; s ++)
                    {
                        S = C->segments[s];

                        if (S->selected)
                        {
                            glColor4ubv(segment_color);
                        }
                        else
                        {
                            glColor4ubv(line_color);
                        }

                        draw_Curve_Segment_Recursive(S, L);
                    }
                }
            }
        }
    }
    glEnable(GL_TEXTURE_2D);
    //glEnable(GL_MULTISAMPLE);
    glEnable(GL_LIGHTING);
    glDisable(GL_LINE_STIPPLE);
    glLineWidth(1);
}

void render_Segments_ID(object * O)
{
    int M0 = 255 * 255 * 255;
    int M1 = 255 * 255;

    float R, G, B, A;

    glDisable(GL_LIGHTING);
    glDisable(GL_DITHER);
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);
    glDisable(GL_MULTISAMPLE);

    int p, e, c, s, idx, idx0, idx1, idx2;

    polygon * P;
    vertex * V;
    curve * C;
    curve_segment * S;

    glColor4f(1, 1, 1, 1);

    for (p = 0; p < O->polycount; p++)
    {
        P = &O->polys[p / ARRAYSIZE][p % ARRAYSIZE];

        glBegin(GL_TRIANGLE_FAN);
        for (e = 0; e < P->edgecount; e ++)
        {
            idx = P->verts[e];
            V = &O->verts[idx / ARRAYSIZE][idx % ARRAYSIZE];
            glVertex3f(V->Tx, V->Ty, V->Tz);
        }
        glEnd();
    }

    glLineWidth(6);
    glColor4f(1, 1, 1, 1);
    glBegin(GL_LINES);

    for (c = 0; c < curvesIndex; c ++)
    {
        C = curves[c];
        if (C->selected)
        {
            for (s = 0; s < C->segment_count; s ++)
            {
                S = C->segments[s];
                idx = S->index;

                R = (float)(idx / M0) / (float)255;
                idx0 = idx % M0;
                G = (float)(idx0 / M1) / (float)255;
                idx1 = idx0 % M1;
                B = (float)(idx1 / 255) / (float)255;
                idx2 = idx1 % 255;
                A = (float)(idx2) / (float)255;

                glColor4f(R, G, B, A);
                glVertex3fv(S->A);
                glVertex3fv(S->C);
            }
        }
    }
    glEnd();
    glLineWidth(1);
    glEnable(GL_MULTISAMPLE);
    glEnable(GL_BLEND);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_DITHER);
    glEnable(GL_LIGHTING);
}

void render_Cps_ID()
{
    int M0 = 255 * 255 * 255;
    int M1 = 255 * 255;

    float R, G, B, A;

    glDisable(GL_LIGHTING);
    glDisable(GL_DITHER);
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);
    glDisable(GL_MULTISAMPLE);

    glPointSize(6);
    glColor4f(1, 1, 1, 1);
    glBegin(GL_POINTS);

    int c, p, idx, idx0, idx1, idx2;
    curve * C;
    cp * CP;

    for (c = 0; c < curvesIndex; c ++)
    {
        C = curves[c];
        if (C->selected)
        {
            for (p = 0; p < C->cps_count; p ++)
            {
                CP = C->cps[p];
                idx = CP->index;

                R = (float)(idx / M0) / (float)255;
                idx0 = idx % M0;
                G = (float)(idx0 / M1) / (float)255;
                idx1 = idx0 % M1;
                B = (float)(idx1 / 255) / (float)255;
                idx2 = idx1 % 255;
                A = (float)(idx2) / (float)255;

                glColor4f(R, G, B, A);
                glVertex3fv(CP->pos);
            }
        }
    }
    glEnd();
    glEnable(GL_MULTISAMPLE);
    glEnable(GL_BLEND);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_DITHER);
    glEnable(GL_LIGHTING);
}

void render_Cps()
{
    glPointSize(3);
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_LIGHTING);
    glBegin(GL_POINTS);
    glColor4ubv(vert_color);

    int c, p;

    curve * C;
    cp * CP;

    for (c = 0; c < curvesIndex; c ++)
    {
        C = curves[c];

        if (C->selected)
        {
            for (p = 0; p < C->cps_count; p ++)
            {
                CP = C->cps[p];
                if (CP->selected)
                {
                    glColor4ubv(selected_vert_color);
                }
                else
                {
                    glColor4ubv(vert_color);
                }
                glVertex3fv(CP->pos);
            }
        }
    }

    glEnd();
    glEnable(GL_LIGHTING);
    glEnable(GL_TEXTURE_2D);
}
