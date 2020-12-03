/*
The MIT License

Copyright <2018> <Allan Kiipli>
*/

#define tolerance 1000

#define POLYGON_ID_COLORS 2
#define OBJECT_ID_COLORS 1
#define OBJECT_COLORS 0

#define OBJES 0
#define POLYS 1
#define EDGES 2
#define VERTS 3
#define BONEZ 4

// to do: migrate pixels to GL
// make ray traced rendering progressive
// develop lights, shadows, alpha for pixel textures

int VAO = 0;

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
    float uv[2];
    float normal[3];
}
texelPack;

union Dir
{
   direction D;
   normal N;
};

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
    float vec[3];
    float dist;
}
aim;

typedef struct
{
    float R[PIXEL_VOLUME], G[PIXEL_VOLUME], B[PIXEL_VOLUME], A[PIXEL_VOLUME], D[PIXEL_VOLUME];
    int trip[PIXEL_VOLUME];
    int level[PIXEL_VOLUME]; // level of triangle
    int object[PIXEL_VOLUME];
}
pixel;

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

/*inline*/ int cull(float point[3], float p_points[3][3])
{
    float angle = 0;
    float dot;
    int on_edge = 0;

    aim A, B;

    int i, C, C0;

    A = vector3dF(p_points[2], point);

    for (i = 0; i < 3; i ++)
    {
        B = vector3dF(p_points[i], point);
        dot = dot_productFF(A.vec, B.vec);
        A.vec[0] = B.vec[0];
        A.vec[1] = B.vec[1];
        A.vec[2] = B.vec[2];
        if (dot < -0.98) on_edge = 1;
        angle += acos(dot);
    }

    C = angle * tolerance;
    C0 = pi2 * tolerance;

    if (C == C0)
    {
        if (on_edge) return 2; else return 1;
    }
    else
    {
        return 0;
    }
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

/*inline*/ void cross_product(float * N, float A[3], float B[3], float C[3])
{
    float v1[3] = {A[0] - B[0], A[1] - B[1], A[2] - B[2]};
    float v2[3] = {C[0] - B[0], C[1] - B[1], C[2] - B[2]};
    N[0] = v1[1] * v2[2] - v1[2] * v2[1];
    N[1] = v1[2] * v2[0] - v1[0] * v2[2];
    N[2] = v1[0] * v2[1] - v1[1] * v2[0];
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

/*inline*/ int come_With_Pixel_(pixel * P, camera * C, normal * D, int L)
{
    Uint32 pix;
    Uint8 r, g, b, a;

    int i, idx, c, k, x, y, o;
    float dot, dist, dot_light;
    normal polynormal;
    triangle * T;
    object * O;
    vertex * V;
    uv * UV;
    aim polyAim;
    float polyradius, deviation, aim_deviation;
    polyplane plane;
    float polypoints[3][3];
    float polynormals[3][3];
    float polytexts[3][2];
    float intersection_Point[3];
    texelPack T_uvNormal;
    SDL_Surface * texture;
    surface_Material Material;

    int volume_counter = 0;

    P->R[volume_counter] = 100;
    P->G[volume_counter] = 100;
    P->B[volume_counter] = 100;
    P->A[volume_counter] = 100;

    for (o = 0; o < C->object_count; o++)
    {
        i = C->objects[o];
        O = objects[i];
        // make switch for renderlevels
        // when it comes to switchable properties
        // later when UI enables it.
        // Then level -1 geometry when displayed as level 0
        // has level 0 geometry but no tune of it.
        int t;
        for (t = 0; t < O->tripcount_[L]; t++)
        {
            T = &O->trips_[L][t / ARRAYSIZE][t % ARRAYSIZE];

            polynormal.x = -T->N.Tx;
            polynormal.y = -T->N.Ty;
            polynormal.z = -T->N.Tz;

//            printf("\ntriangle %d\n", t);
//            printf("polynormal %f %f %f\n", polynormal.x, polynormal.y, polynormal.z);

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

            dot = dot_product(&polynormal, D);

//            printf("dot %f\n", dot);

            if (dot > 0)
            {
                polyradius = T->B.Tradius;

//                printf("polyradius %f\n", polyradius);

                polyAim = vector3d(T->B, C->T->pos);

//                printf("polyAim.dist %f\n", polyAim.dist);
//
//                printf("polyAim.vec %f %f %f\n", polyAim.vec[0], polyAim.vec[1], polyAim.vec[2]);

                if (polyAim.dist > polyradius)
                {
                    deviation = atan2(polyradius, polyAim.dist - polyradius);
                }
                else
                {
                    deviation = pi;
                }

//                printf("deviation %f\n", deviation);

                aim_deviation = acos(dot_productN(D, polyAim.vec));

//                printf("aim_deviation %f\n", aim_deviation);

                if (aim_deviation > deviation)
                {
//                    printf("continue\n");
                    continue;
                }

                P->R[volume_counter] = 255;

                plane = init_plane(polypoints[1], &polynormal);
                dist = nearest(C->T->pos, plane);

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

                    if (Material.use_texture && texture != NULL)
                    {
                        T_uvNormal = uv_value(intersection_Point, polypoints, polynormals, polytexts);
                        x = abs((int)(texture->w * T_uvNormal.uv[0])) % texture->w;
                        y = abs((int)(texture->h * T_uvNormal.uv[1])) % texture->h;
                        pix = get_pixel32(texture, x, y);
                        SDL_GetRGBA(pix, texture->format, &r, &g, &b, &a);
                    }
                    else
                    {
                        r = Material.RGBA.R;
                        g = Material.RGBA.G;
                        b = Material.RGBA.B;
                        a = Material.RGBA.A;
                        normal_value(intersection_Point, polypoints, polynormals, T_uvNormal.normal);;
                    }
                    if (Material.smooth)
                    {
                        dot_light = dot_productFF(T_uvNormal.normal, light_vec);
                    }
                    else
                    {
                        dot_light = dot_productN(&polynormal, light_vec);
                    }

                    P->D[volume_counter] = dist;
                    P->trip[volume_counter] = t;
                    P->level[volume_counter] = L;
                    P->object[volume_counter] = i;
                    P->R[volume_counter] = r * dot_light;
                    P->G[volume_counter] = g * dot_light;
                    P->B[volume_counter] = b * dot_light;
                    P->A[volume_counter] = a;
                    volume_counter ++;
                }
                else
                {
                    continue;
                }
            }
        }
    }
    if (volume_counter == 0)
    {
        return 0;
    }

    int volume_index[volume_counter], d;
    float d_index[volume_counter], s;

    for (i = 0; i < volume_counter; i ++)
    {
        volume_index[i] = i;
        d_index[i] = P->D[i];
    }

    for (i = 0; i < volume_counter - 1; i ++)
    {
        for (k = 0; k < volume_counter - i - 1; k ++)
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

    return volume_index[0];
}

/*inline*/ int come_With_Pixel(pixel * P, camera * C, normal * D)
{
    Uint32 pix;
    Uint8 r, g, b, a;

    int i, idx, c, k, x, y, o;
    float dot, dist, dot_light;
    normal polynormal;
    triangle * T;
    object * O;
    vertex * V;
    uv * UV;
    aim polyAim;
    float polyradius, deviation, aim_deviation;
    polyplane plane;
    float polypoints[3][3];
    float polynormals[3][3];
    float polytexts[3][2];
    float intersection_Point[3];
    texelPack T_uvNormal;
    SDL_Surface * texture;
    surface_Material Material;

    int volume_counter = 0;

    P->R[volume_counter] = 100;
    P->G[volume_counter] = 100;
    P->B[volume_counter] = 100;
    P->A[volume_counter] = 100;

    for (o = 0; o < C->object_count; o++)
    {
        i = C->objects[o];
        O = objects[i];
        int t;
        for (t = 0; t < O->tripcount; t++)
        {
            T = &O->trips[t / ARRAYSIZE][t % ARRAYSIZE];

            polynormal.x = -T->N.Tx;
            polynormal.y = -T->N.Ty;
            polynormal.z = -T->N.Tz;

//            printf("\ntriangle %d\n", t);
//            printf("polynormal %f %f %f\n", polynormal.x, polynormal.y, polynormal.z);

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

            dot = dot_product(&polynormal, D);

//            printf("dot %f\n", dot);

            if (dot > 0)
            {
                polyradius = T->B.Tradius;

//                printf("polyradius %f\n", polyradius);

                polyAim = vector3d(T->B, C->T->pos);

//                printf("polyAim.dist %f\n", polyAim.dist);
//
//                printf("polyAim.vec %f %f %f\n", polyAim.vec[0], polyAim.vec[1], polyAim.vec[2]);

                if (polyAim.dist > polyradius)
                {
                    deviation = atan2(polyradius, polyAim.dist - polyradius);
                }
                else
                {
                    deviation = pi;
                }

//                printf("deviation %f\n", deviation);

                aim_deviation = acos(dot_productN(D, polyAim.vec));

//                printf("aim_deviation %f\n", aim_deviation);

                if (aim_deviation > deviation)
                {
//                    printf("continue\n");
                    continue;
                }

                P->R[volume_counter] = 255;

                plane = init_plane(polypoints[1], &polynormal);
                dist = nearest(C->T->pos, plane);

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

                    if (Material.use_texture && texture != NULL)
                    {
                        T_uvNormal = uv_value(intersection_Point, polypoints, polynormals, polytexts);
                        x = abs((int)(texture->w * T_uvNormal.uv[0])) % texture->w;
                        y = abs((int)(texture->h * T_uvNormal.uv[1])) % texture->h;
                        pix = get_pixel32(texture, x, y);
                        SDL_GetRGBA(pix, texture->format, &r, &g, &b, &a);
                    }
                    else
                    {
                        r = Material.RGBA.R;
                        g = Material.RGBA.G;
                        b = Material.RGBA.B;
                        a = Material.RGBA.A;
                        normal_value(intersection_Point, polypoints, polynormals, T_uvNormal.normal);;
                    }
                    if (Material.smooth)
                    {
                        dot_light = dot_productFF(T_uvNormal.normal, light_vec);
                    }
                    else
                    {
                        dot_light = dot_productN(&polynormal, light_vec);
                    }

                    P->D[volume_counter] = dist;
                    P->trip[volume_counter] = t;
                    P->level[volume_counter] = -1;
                    P->object[volume_counter] = i;
                    P->R[volume_counter] = r * dot_light;
                    P->G[volume_counter] = g * dot_light;
                    P->B[volume_counter] = b * dot_light;
                    P->A[volume_counter] = a;
                    volume_counter ++;
                }
                else
                {
                    continue;
                }
            }
        }
    }
    if (volume_counter == 0)
    {
        return 0;
    }

    int volume_index[volume_counter], d;
    float d_index[volume_counter], s;

    for (i = 0; i < volume_counter; i ++)
    {
        volume_index[i] = i;
        d_index[i] = P->D[i];
    }

    for (i = 0; i < volume_counter - 1; i ++)
    {
        for (k = 0; k < volume_counter - i - 1; k ++)
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

    return volume_index[0];
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
    float deviation, objectradius, aim_deviation;
    aim objectAim;

    int object_c = 0;

    float d_index[objectIndex], s;

    for (o = 0; o < objectIndex; o ++)
    {
        if (loaded_objects[o] == 0)
            continue;
        O = objects[o];
        update_Box_T(O);
        objectradius = O->B.radius;
        objectAim = vector3d(O->B, C->T->pos);

        if (objectAim.dist > objectradius)
        {
            deviation = atan2(objectradius * 2, objectAim.dist - objectradius);
        }
        else
        {
            deviation = pi;
        }

        deviation += C->view_minor;

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

    for (i = 0; i < C->object_count - 1; i ++)
    {
        for (k = 0; k < C->object_count - i - 1; k ++)
        {
            if (d_index[k] < d_index[k + 1]) // to reverse use other comparison
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

direction screen_point_to_vector(int x, int y, int hres, int vres, float h_view, float v_view)
{
    direction D;

    float H_Mark = h_view / 2.0;
    float V_Mark = v_view / 2.0;
    float H_step = h_view / (float)hres;
    float V_step = v_view / (float)vres;
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
//        if (E)
//        {
        for (v = 0; v < 4; v ++)
        {
            idx = Q->texts[v];
            c_c1 = idx * 4;
            O->cols_array_[L][1][c_c1 ++] = I->R;
            O->cols_array_[L][1][c_c1 ++] = I->G;
            O->cols_array_[L][1][c_c1 ++] = I->B;
            O->cols_array_[L][1][c_c1 ++] = I->A;
        }
//        }
//        else
//        {
        for (v = 0; v < 4; v ++)
        {
            O->cols_array_[L][0][c_c ++] = I->R;
            O->cols_array_[L][0][c_c ++] = I->G;
            O->cols_array_[L][0][c_c ++] = I->B;
            O->cols_array_[L][0][c_c ++] = I->A;
        }
//        }
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
    int p, l, q;
    polygon * P;
    quadrant * Q;

    for (p = 0; p < O->polycount; p ++)
    {
        P = &O->polys[p / ARRAYSIZE][p % ARRAYSIZE];
        P->surface = m;
    }

    for (l = 0; l <= O->subdlevel; l ++)
    {
        for (q = 0; q < O->quadcount_[l]; q ++)
        {
            Q = &O->quads_[l][q / ARRAYSIZE][q % ARRAYSIZE];
            Q->surface = m;
        }
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
        if (sel_type == BONEZ)
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

            object * O;

            aim vertAim;
            normal vertnormal;

            for (o = 0; o < Objects_count; o ++)
            {
                O = objects[Objects[o]];
                for (v = 0; v < O->vertcount; v ++)
                {
                    V = &O->verts[v / ARRAYSIZE][v % ARRAYSIZE];

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
                                V0->selected = 1;
                            else
                                V0->selected = 0;
                            for (u = 0; u < V0->uv_vertcount; u ++)
                            {
                                idx = V0->uv_verts[u];
                                if (idx > -1 && idx < O->textcount)
                                {
                                    UV = &O->uvtex[idx / ARRAYSIZE][idx % ARRAYSIZE];
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

            object * O;

            aim edgeAim;
            normal edgenormal;

            for (o = 0; o < Objects_count; o ++)
            {
                O = objects[Objects[o]];
                for (e = 0; e < O->edgecount; e ++)
                {
                    E = &O->edges[e / ARRAYSIZE][e % ARRAYSIZE];
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
                                E->selected = 1;
                            else
                                E->selected = 0;
                            for (u = 0; u < E->uv_edcount; u ++)
                            {
                                idx = E->uv_edges[u];
                                if (idx > -1 && idx < O->uvedcount)
                                {
                                    UVE = &O->uveds[idx / ARRAYSIZE][idx % ARRAYSIZE];
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

        if (edgedraw && O->subdlevel >= L) // assuming some objects have no geometry on all levels.
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
    glEnable(GL_LIGHTING);
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
    if (O->subdlevel >= L)
    {
        int e, idx, idx0, idx1, idx2;
        uv_edge * UVE, * UVE0;

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

            idx = UVE->uv_edges[0];
            UVE0 = &O->uveds_[0][idx / ARRAYSIZE][idx % ARRAYSIZE];
            draw_UV_Edge_ID_recursive(C, O, UVE0, 0, L);
            idx = UVE->uv_edges[1];
            UVE0 = &O->uveds_[0][idx / ARRAYSIZE][idx % ARRAYSIZE];
            draw_UV_Edge_ID_recursive(C, O, UVE0, 0, L);
        }

        glEnd();

        glLineWidth(1);

        glEnable(GL_MULTISAMPLE);
        glEnable(GL_BLEND);
        glEnable(GL_TEXTURE_2D);
        glEnable(GL_DITHER);
        glEnable(GL_LIGHTING);
    }
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

void render_quads_Edges_ID(camera * C, object * O, int l)
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

    if (O->subdlevel >= L)
    {
        int q, e, idx, idx0, idx1, idx2;
        quadrant * Q;
        edge * E, * E0;
        vertex * V;

        int M0 = 255 * 255 * 255;
        int M1 = 255 * 255;

        float R, G, B, A;

        glDisable(GL_LIGHTING);
        glDisable(GL_DITHER);
        glDisable(GL_TEXTURE_2D);
        glDisable(GL_BLEND);
        glDisable(GL_MULTISAMPLE);

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

        glLineWidth(1);

        glEnable(GL_MULTISAMPLE);
        glEnable(GL_BLEND);
        glEnable(GL_TEXTURE_2D);
        glEnable(GL_DITHER);
        glEnable(GL_LIGHTING);
    }
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

void render_quad_Verts_ID(camera * C, object * O, int l)
{
    int level, e, q, v, idx, idx0, idx1, idx2;
    quadrant * Q;
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

void render_UV_Quads_ID(camera * C, object * O, int l)
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, C->h_v_ratio, 0, 1, 1, -1);

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
            render_UV_Quads_ID_recursive(O, Q, 0, L);
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

void render_Transformers_ID()
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

void render_Labels(int width, int height)
{
    int t;
    transformer * T;
    label * L;

    label_count = 0;

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

        if (T->style == ik_fixed)
        {
            point[0] = T->pos[0];
            point[1] = T->pos[1];
            point[2] = T->pos[2];

            result = point_on_screen_GLU(point, coords);

            if (result)
            {
                L = labels[label_count ++];
                //strcpy(L->text, "F");
                L->x = coords[0] - 120;
                L->y = height - (coords[1] + 10);
            }
        }
    }

    display_labels(width, height);
}

void render_Transformers(int currentLocator)
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
                        line_color = &selected_edge_color[0];
                    }
                    else if (!glIsEnabled(GL_LIGHTING))
                    {
                        glEnable(GL_LIGHTING);
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
    object * O;
    quadrant * Q;

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
                    O->norm_array[0][n_c++] = V->N.Tx;
                    O->norm_array[0][n_c++] = V->N.Ty;
                    O->norm_array[0][n_c++] = V->N.Tz;
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
                        O->norm_array[0][n_c++] = V->N.Tx;
                        O->norm_array[0][n_c++] = V->N.Ty;
                        O->norm_array[0][n_c++] = V->N.Tz;
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
                    O->norm_array_[L][0][n_c++] = V->N.Tx;
                    O->norm_array_[L][0][n_c++] = V->N.Ty;
                    O->norm_array_[L][0][n_c++] = V->N.Tz;
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
                        O->norm_array_[L][0][n_c++] = V->N.Tx;
                        O->norm_array_[L][0][n_c++] = V->N.Ty;
                        O->norm_array_[L][0][n_c++] = V->N.Tz;
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
                idx = P->quads[e];
                Q = &O->quads_[0][idx / ARRAYSIZE][idx % ARRAYSIZE];
                render_UV_Quads_ID_recursive(O, Q, 0, L);
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

        idx = UVE->uv_edges[0];
        UVE0 = &O->uveds_[0][idx / ARRAYSIZE][idx % ARRAYSIZE];
        draw_UVEdge_recursive(O, UVE0, 0, L);
        idx = UVE->uv_edges[1];
        UVE0 = &O->uveds_[0][idx / ARRAYSIZE][idx % ARRAYSIZE];
        draw_UVEdge_recursive(O, UVE0, 0, L);

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

void render_quads_OnScreen(camera * C, int wireframe, int edgedraw, int vertdraw, int l, int currentObject, int mode, int Selection_Mode, int update_colors, int update_uv, int elem)
{
    int level, v, o, q, e, idx, L;
    object * O;
    quadrant * Q;
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

        if (O->vertex_arrays[L])
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

        if (edgedraw && O->subdlevel >= L) // assuming some objects have no geometry on all levels.
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
                        line_color = &selected_edge_color[0];
                    }
                    else if (!glIsEnabled(GL_LIGHTING))
                    {
                        glEnable(GL_LIGHTING);
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

void render_Image(unsigned char * data, camera * C, int width, int height, int L)
{
    normalizeF((float *)&light_vec);
//    printf("light_vec %f %f %f\n", light_vec[0], light_vec[1], light_vec[2]);
//    printf("render_Image\n");

    // TODO
    // exclude objects from camera
    // exclude objects from lights

    int x, y, idx;
    int c = 0;
    float R;
    pixel P;

    union Dir D = {{0.0, 0.0, -1.0}};
    float DDy;

    float H_Mark = C->h_view / 2.0;
    float V_Mark = C->v_view / 2.0;
    float H_step = (C->h_view / (float)width);
    float V_step = (C->v_view / (float)height);
    H_Mark -= H_step / 2.0;
    V_Mark -= V_step / 2.0;
    H_Mark += pi;
    V_Mark += pi_2;

    float H_MARK = H_Mark;

    for (y = 0; y < height; y ++)
    {
        R = sin(V_Mark);
        DDy = -cos(V_Mark);
        H_Mark = H_MARK;
        for (x = 0; x < width; x ++)
        {
            D.D.y = DDy;
            D.D.x = sin(H_Mark) * R;
            D.D.z = cos(H_Mark) * R;

            rotate_Vector(C->T, -D.D.x, D.D.y, -D.D.z, &D.D); // direction is submitted from union

            if (L == -1)
            idx = come_With_Pixel(&P, C, &D.N); // normal is submitted from union
            else
            idx = come_With_Pixel_(&P, C, &D.N, L);

            data[c++] = (unsigned char)P.R[idx];
            data[c++] = (unsigned char)P.G[idx];
            data[c++] = (unsigned char)P.B[idx];
            data[c++] = (unsigned char)P.A[idx];

            H_Mark -= H_step;
        }
        V_Mark -= V_step;
    }
}
