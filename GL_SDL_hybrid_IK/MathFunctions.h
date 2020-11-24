/*
The MIT License

Copyright <2018> <Allan Kiipli>
*/

#ifndef MATHFUNCTIONS_H_INCLUDED
#define MATHFUNCTIONS_H_INCLUDED

//#include "Object.h"

/*inline*/ void boxt_3_2d(box_2d * b, uv * A, uv * B, uv * C, unsigned char radius)
{
    b->u = (A->u + B->u + C->u) / 3.0;
    b->v = (A->v + B->v + C->v) / 3.0;
    float min_u = A->u;
    float min_v = A->v;
    float max_u = B->u;
    float max_v = B->v;

    if (B->u < min_u) {min_u = B->u; max_u = A->u;}
    if (B->v < min_v) {min_v = B->v; max_v = A->v;}

    if (C->u < min_u) min_u = C->u; else if (C->u > max_u) max_u = C->u;
    if (C->v < min_v) min_v = C->v; else if (C->v > max_v) max_v = C->v;

    if (radius)
        b->radius = sqrt((max_u - min_u) * (max_u - min_u) +
                     (max_v - min_v) * (max_v - min_v)) / 2.0;
}

/*inline*/ void boxt_0(box_2d * b, uv * A, uv * B, unsigned char radius)
{
    b->u = (A->u + B->u) / 2.0;
    b->v = (A->v + B->v) / 2.0;
    float min_u = A->u;
    float min_v = A->v;
    float max_u = B->u;
    float max_v = B->v;
    if (B->u < min_u) {min_u = B->u; max_u = A->u;}
    if (B->v < min_v) {min_v = B->v; max_v = A->v;}
    if (radius)
        b->radius = sqrt((max_u - min_u) * (max_u - min_u) +
                     (max_v - min_v) * (max_v - min_v)) / 2.0;
}

/*inline*/ void boxt_2_T(box_3d * b, vertex * A, vertex * B, unsigned char radius)
{
    b->Tx = (A->Tx + B->Tx) / 2.0;
    b->Ty = (A->Ty + B->Ty) / 2.0;
    b->Tz = (A->Tz + B->Tz) / 2.0;

    float min_x = A->Tx;
    float min_y = A->Ty;
    float min_z = A->Tz;
    float max_x = B->Tx;
    float max_y = B->Ty;
    float max_z = B->Tz;

    if (B->Tx < min_x) {min_x = B->Tx; max_x = A->Tx;}
    if (B->Ty < min_y) {min_y = B->Ty; max_y = A->Ty;}
    if (B->Tz < min_z) {min_z = B->Tz; max_z = A->Tz;}
    if (radius)
        b->Tradius = sqrt((max_x - min_x) * (max_x - min_x) +
                      (max_y - min_y) * (max_y - min_y) +
                      (max_z - min_z) * (max_z - min_z)) / 2.0;
}

/*inline*/ void boxt_2(box_3d * b, vertex * A, vertex * B, unsigned char radius)
{
    b->x = (A->x + B->x) / 2.0;
    b->y = (A->y + B->y) / 2.0;
    b->z = (A->z + B->z) / 2.0;

    float min_x = A->x;
    float min_y = A->y;
    float min_z = A->z;
    float max_x = B->x;
    float max_y = B->y;
    float max_z = B->z;

    if (B->x < min_x) {min_x = B->x; max_x = A->x;}
    if (B->y < min_y) {min_y = B->y; max_y = A->y;}
    if (B->z < min_z) {min_z = B->z; max_z = A->z;}
    if (radius)
        b->radius = sqrt((max_x - min_x) * (max_x - min_x) +
                     (max_y - min_y) * (max_y - min_y) +
                     (max_z - min_z) * (max_z - min_z)) / 2.0;
}

/*inline*/ void boxt_3(box_3d * b, vertex * A, vertex * B, vertex * C, unsigned char radius)
{
    b->x = (A->x + B->x + C->x) / 3.0;
    b->y = (A->y + B->y + C->y) / 3.0;
    b->z = (A->z + B->z + C->z) / 3.0;

    float min_x = A->x;
    float min_y = A->y;
    float min_z = A->z;
    float max_x = B->x;
    float max_y = B->y;
    float max_z = B->z;

    if (B->x < min_x) {min_x = B->x; max_x = A->x;}
    if (B->y < min_y) {min_y = B->y; max_y = A->y;}
    if (B->z < min_z) {min_z = B->z; max_z = A->z;}

    if (C->x < min_x) min_x = C->x; else if (C->x > max_x) max_x = C->x;
    if (C->y < min_y) min_y = C->y; else if (C->y > max_y) max_y = C->y;
    if (C->z < min_z) min_z = C->z; else if (C->z > max_z) max_z = C->z;

    if (radius)
        b->radius = sqrt((max_x - min_x) * (max_x - min_x) +
                     (max_y - min_y) * (max_y - min_y) +
                     (max_z - min_z) * (max_z - min_z)) / 2.0;
}

/*inline*/ void boxt_3_T(box_3d * b, vertex * A, vertex * B, vertex * C, unsigned char radius)
{
    b->Tx = (A->Tx + B->Tx + C->Tx) / 3.0;
    b->Ty = (A->Ty + B->Ty + C->Ty) / 3.0;
    b->Tz = (A->Tz + B->Tz + C->Tz) / 3.0;

    float min_x = A->Tx;
    float min_y = A->Ty;
    float min_z = A->Tz;
    float max_x = B->Tx;
    float max_y = B->Ty;
    float max_z = B->Tz;

    if (B->Tx < min_x) {min_x = B->Tx; max_x = A->Tx;}
    if (B->Ty < min_y) {min_y = B->Ty; max_y = A->Ty;}
    if (B->Tz < min_z) {min_z = B->Tz; max_z = A->Tz;}

    if (C->Tx < min_x) min_x = C->Tx; else if (C->Tx > max_x) max_x = C->Tx;
    if (C->Ty < min_y) min_y = C->Ty; else if (C->Ty > max_y) max_y = C->Ty;
    if (C->Tz < min_z) min_z = C->Tz; else if (C->Tz > max_z) max_z = C->Tz;

    if (radius)
        b->Tradius = sqrt((max_x - min_x) * (max_x - min_x) +
                      (max_y - min_y) * (max_y - min_y) +
                      (max_z - min_z) * (max_z - min_z)) / 2.0;
}

/*inline*/ void boxt_4_2d(box_2d * b, uv * A, uv * B, uv * C, uv * D, unsigned char radius)
{
    b->u = (A->u + B->u + C->u + D->u) / 4.0;
    b->v = (A->v + B->v + C->v + D->v) / 4.0;

    float min_u = A->u;
    float min_v = A->v;
    float max_u = B->u;
    float max_v = B->v;

    if (B->u < min_u) {min_u = B->u; max_u = A->u;}
    if (B->v < min_v) {min_v = B->v; max_v = A->v;}

    if (C->u < min_u) min_u = C->u; else if (C->u > max_u) max_u = C->u;
    if (C->v < min_v) min_v = C->v; else if (C->v > max_v) max_v = C->v;

    if (D->u < min_u) min_u = D->u; else if (D->u > max_u) max_u = D->u;
    if (D->v < min_v) min_v = D->v; else if (D->v > max_v) max_v = D->v;
    if (radius)
        b->radius = sqrt((max_u - min_u) * (max_u - min_u) +
                     (max_v - min_v) * (max_v - min_v)) / 2.0;
}

/*inline*/ void boxt_4_T(box_3d * b, vertex * A, vertex * B, vertex * C, vertex * D, unsigned char radius)
{
    b->Tx = (A->Tx + B->Tx + C->Tx + D->Tx) / 4.0;
    b->Ty = (A->Ty + B->Ty + C->Ty + D->Ty) / 4.0;
    b->Tz = (A->Tz + B->Tz + C->Tz + D->Tz) / 4.0;

    float min_x = A->x;
    float min_y = A->y;
    float min_z = A->z;
    float max_x = B->x;
    float max_y = B->y;
    float max_z = B->z;

    if (B->x < min_x) {min_x = B->x; max_x = A->x;}
    if (B->y < min_y) {min_y = B->y; max_y = A->y;}
    if (B->z < min_z) {min_z = B->z; max_z = A->z;}

    if (C->x < min_x) min_x = C->x; else if (C->x > max_x) max_x = C->x;
    if (C->y < min_y) min_y = C->y; else if (C->y > max_y) max_y = C->y;
    if (C->z < min_z) min_z = C->z; else if (C->z > max_z) max_z = C->z;

    if (D->x < min_x) min_x = D->x; else if (D->x > max_x) max_x = D->x;
    if (D->y < min_y) min_y = D->y; else if (D->y > max_y) max_y = D->y;
    if (D->z < min_z) min_z = D->z; else if (D->z > max_z) max_z = D->z;
    if (radius)
        b->Tradius = sqrt((max_x - min_x) * (max_x - min_x) +
                     (max_y - min_y) * (max_y - min_y) +
                     (max_z - min_z) * (max_z - min_z)) / 2.0;
}

/*inline*/ void boxt_4(box_3d * b, vertex * A, vertex * B, vertex * C, vertex * D, unsigned char radius)
{
    b->x = (A->x + B->x + C->x + D->x) / 4.0;
    b->y = (A->y + B->y + C->y + D->y) / 4.0;
    b->z = (A->z + B->z + C->z + D->z) / 4.0;

    float min_x = A->x;
    float min_y = A->y;
    float min_z = A->z;
    float max_x = B->x;
    float max_y = B->y;
    float max_z = B->z;

    if (B->x < min_x) {min_x = B->x; max_x = A->x;}
    if (B->y < min_y) {min_y = B->y; max_y = A->y;}
    if (B->z < min_z) {min_z = B->z; max_z = A->z;}

    if (C->x < min_x) min_x = C->x; else if (C->x > max_x) max_x = C->x;
    if (C->y < min_y) min_y = C->y; else if (C->y > max_y) max_y = C->y;
    if (C->z < min_z) min_z = C->z; else if (C->z > max_z) max_z = C->z;

    if (D->x < min_x) min_x = D->x; else if (D->x > max_x) max_x = D->x;
    if (D->y < min_y) min_y = D->y; else if (D->y > max_y) max_y = D->y;
    if (D->z < min_z) min_z = D->z; else if (D->z > max_z) max_z = D->z;
    if (radius)
        b->radius = sqrt((max_x - min_x) * (max_x - min_x) +
                     (max_y - min_y) * (max_y - min_y) +
                     (max_z - min_z) * (max_z - min_z)) / 2.0;
}

/*inline*/ void boxt_2d_P(object * O, polygon * P, unsigned char radius)
{
    uv * UV;
    uv * A;
    uv * B;
    int idx;
    idx = P->texts[0];
    A = &O->uvtex[idx / ARRAYSIZE][idx % ARRAYSIZE];
    idx = P->texts[1];
    B = &O->uvtex[idx / ARRAYSIZE][idx % ARRAYSIZE];

    float min_u = A->u;
    float min_v = A->v;
    float max_u = B->u;
    float max_v = B->v;

    float u = 0;
    float v = 0;

    int i;
    for (i = 0; i < P->edgecount; i ++)
    {
        idx = P->texts[i];
        UV = &O->uvtex[idx / ARRAYSIZE][idx % ARRAYSIZE];
        if (UV->u < min_u) min_u = UV->u; else if (UV->u > max_u) max_u = UV->u;
        if (UV->v < min_v) min_v = UV->v; else if (UV->v > max_v) max_v = UV->v;
        u += UV->u;
        v += UV->v;
    }
    u /= P->edgecount;
    v /= P->edgecount;
    if (radius)
        P->B2.radius = sqrt((max_u - min_u) * (max_u - min_u) +
                       (max_v - min_v) * (max_v - min_v)) / 2.0;
    P->B2.u = u;
    P->B2.v = v;
}

/*inline*/ void boxt_P_T(object * O, polygon * P, unsigned char radius)
{
    vertex * V;
    vertex * A;
    vertex * B;
    int idx;
    idx = P->verts[0];
    A = &O->verts[idx / ARRAYSIZE][idx % ARRAYSIZE];
    idx = P->verts[1];
    B = &O->verts[idx / ARRAYSIZE][idx % ARRAYSIZE];

    float min_x = A->Tx;
    float min_y = A->Ty;
    float min_z = A->Tz;
    float max_x = B->Tx;
    float max_y = B->Ty;
    float max_z = B->Tz;

    float x = 0;
    float y = 0;
    float z = 0;

    int i;
    for (i = 0; i < P->edgecount; i ++)
    {
        idx = P->verts[i];
        V = &O->verts[idx / ARRAYSIZE][idx % ARRAYSIZE];
        if (V->Tx < min_x) min_x = V->Tx; else if (V->Tx > max_x) max_x = V->Tx;
        if (V->Ty < min_y) min_y = V->Ty; else if (V->Ty > max_y) max_y = V->Ty;
        if (V->Tz < min_z) min_z = V->Tz; else if (V->Tz > max_z) max_z = V->Tz;
        x += V->Tx;
        y += V->Ty;
        z += V->Tz;
    }
    x /= P->edgecount;
    y /= P->edgecount;
    z /= P->edgecount;
    if (radius)
        P->B.Tradius = sqrt((max_x - min_x) * (max_x - min_x) +
                       (max_y - min_y) * (max_y - min_y) +
                       (max_z - min_z) * (max_z - min_z)) / 2.0;
    P->B.Tx = x;
    P->B.Ty = y;
    P->B.Tz = z;
}

/*inline*/ void boxt_P(object * O, polygon * P, unsigned char radius)
{
    vertex * V;
    vertex * A;
    vertex * B;
    int idx;
    idx = P->verts[0];
    A = &O->verts[idx / ARRAYSIZE][idx % ARRAYSIZE];
    idx = P->verts[1];
    B = &O->verts[idx / ARRAYSIZE][idx % ARRAYSIZE];

    float min_x = A->x;
    float min_y = A->y;
    float min_z = A->z;
    float max_x = B->x;
    float max_y = B->y;
    float max_z = B->z;

    float x = 0;
    float y = 0;
    float z = 0;

    int i;
    for (i = 0; i < P->edgecount; i ++)
    {
        idx = P->verts[i];
        V = &O->verts[idx / ARRAYSIZE][idx % ARRAYSIZE];
        if (V->x < min_x) min_x = V->x; else if (V->x > max_x) max_x = V->x;
        if (V->y < min_y) min_y = V->y; else if (V->y > max_y) max_y = V->y;
        if (V->z < min_z) min_z = V->z; else if (V->z > max_z) max_z = V->z;
        x += V->x;
        y += V->y;
        z += V->z;
    }
    x /= P->edgecount;
    y /= P->edgecount;
    z /= P->edgecount;
    if (radius)
        P->B.radius = sqrt((max_x - min_x) * (max_x - min_x) +
                       (max_y - min_y) * (max_y - min_y) +
                       (max_z - min_z) * (max_z - min_z)) / 2.0;
    P->B.x = x;
    P->B.y = y;
    P->B.z = z;
}

/*inline*/ void boxt_O_T(object * O, unsigned char radius)
{
    vertex * V;
    vertex * A = &O->verts[0][0];
    vertex * B = &O->verts[0][1];

    float min_x = A->Tx;
    float min_y = A->Ty;
    float min_z = A->Tz;
    float max_x = B->Tx;
    float max_y = B->Ty;
    float max_z = B->Tz;

    float x = 0;
    float y = 0;
    float z = 0;

    int i;
    for (i = 0; i < O->vertcount; i ++)
    {
        V = &O->verts[i / ARRAYSIZE][i % ARRAYSIZE];
        if (V->Tx < min_x) min_x = V->Tx; else if (V->Tx > max_x) max_x = V->Tx;
        if (V->Ty < min_y) min_y = V->Ty; else if (V->Ty > max_y) max_y = V->Ty;
        if (V->Tz < min_z) min_z = V->Tz; else if (V->Tz > max_z) max_z = V->Tz;
        x += V->Tx;
        y += V->Ty;
        z += V->Tz;
    }
    x /= O->vertcount;
    y /= O->vertcount;
    z /= O->vertcount;
    if (radius)
        O->B.radius = sqrt((max_x - min_x) * (max_x - min_x) +
                       (max_y - min_y) * (max_y - min_y) +
                       (max_z - min_z) * (max_z - min_z)) / 2.0;
    O->B.Tx = x;
    O->B.Ty = y;
    O->B.Tz = z;
}

/*inline*/ void boxt_O(object * O, unsigned char radius)
{
    vertex * V;
    vertex * A = &O->verts[0][0];
    vertex * B = &O->verts[0][1];

    float min_x = A->x;
    float min_y = A->y;
    float min_z = A->z;
    float max_x = B->x;
    float max_y = B->y;
    float max_z = B->z;

    float x = 0;
    float y = 0;
    float z = 0;

    int i;
    for (i = 0; i < O->vertcount; i ++)
    {
        V = &O->verts[i / ARRAYSIZE][i % ARRAYSIZE];
        if (V->x < min_x) min_x = V->x; else if (V->x > max_x) max_x = V->x;
        if (V->y < min_y) min_y = V->y; else if (V->y > max_y) max_y = V->y;
        if (V->z < min_z) min_z = V->z; else if (V->z > max_z) max_z = V->z;
        x += V->x;
        y += V->y;
        z += V->z;
    }
    x /= O->vertcount;
    y /= O->vertcount;
    z /= O->vertcount;
    if (radius)
        O->B.radius = sqrt((max_x - min_x) * (max_x - min_x) +
                       (max_y - min_y) * (max_y - min_y) +
                       (max_z - min_z) * (max_z - min_z)) / 2.0;
    O->B.x = x;
    O->B.y = y;
    O->B.z = z;
}

/*inline*/ float dot_productFF(float A[3], float B[3])
{
    float v = A[0] * B[0] + A[1] * B[1] + A[2] * B[2];
    if (v > 1.0)
    {
        v = 1.0;
    }
    else if (v < -1.0)
    {
        v = -1.0;
    }
    return v;
}

/*inline*/ float dot_productN_T(normal * A, float B[3])
{
    float v = A->Tx * B[0] + A->Ty * B[1] + A->Tz * B[2];
    if (v > 1.0)
    {
        v = 1.0;
    }
    else if (v < -1.0)
    {
        v = -1.0;
    }
    return v;
}

/*inline*/ float dot_productN(normal * A, float B[3])
{
    float v = A->x * B[0] + A->y * B[1] + A->z * B[2];
    if (v > 1.0)
    {
        v = 1.0;
    }
    else if (v < -1.0)
    {
        v = -1.0;
    }
    return v;
}

/*inline*/ float dot_productF(float A[3], normal * B)
{
    float v = A[0] * B->x + A[1] * B->y + A[2] * B->z;
    if (v > 1.0)
    {
        v = 1.0;
    }
    else if (v < -1.0)
    {
        v = -1.0;
    }
    return v;
}

/*inline*/ float dot_product_T(normal * A, normal * B)
{
    float v = A->Tx * B->Tx + A->Ty * B->Ty + A->Tz * B->Tz;
    if (v > 1.0)
    {
        v = 1.0;
    }
    else if (v < -1.0)
    {
        v = -1.0;
    }
    return v;
}

/*inline*/ float dot_product(normal * A, normal * B)
{
    float v = A->x * B->x + A->y * B->y + A->z * B->z;
    if (v > 1.0)
    {
        v = 1.0;
    }
    else if (v < -1.0)
    {
        v = -1.0;
    }
    return v;
}

/*inline*/ void normalizeD(direction * D)
{
    float d = sqrt(D->x * D->x + D->y * D->y + D->z * D->z);

    if (d > 0)
    {
        D->x /= d;
        D->y /= d;
        D->z /= d;
    }
}

/*inline*/ void normalizeF(float * vec)
{
    float d = sqrt(vec[0] * vec[0] + vec[1] * vec[1] + vec[2] * vec[2]);

    if (d > 0)
    {
        vec[0] /= d;
        vec[1] /= d;
        vec[2] /= d;
    }
}

/*inline*/ void normalize_T(normal * n)
{
    float d = sqrt(n->Tx * n->Tx + n->Ty * n->Ty + n->Tz * n->Tz);

    if (d > 0)
    {
        n->Tx /= d;
        n->Ty /= d;
        n->Tz /= d;
    }
}

/*inline*/ void normalize(normal * n)
{
    float d = sqrt(n->x * n->x + n->y * n->y + n->z * n->z);

    if (d > 0)
    {
        n->x /= d;
        n->y /= d;
        n->z /= d;
    }
}

/*inline*/ void cross(normal * n, vertex * A, vertex * B, vertex * C)
{
    // look from top and count vectors counterclockwise
    float v1[3] = {C->x - B->x, C->y - B->y, C->z - B->z};
    float v2[3] = {A->x - B->x, A->y - B->y, A->z - B->z};
    n->x = v1[1] * v2[2] - v1[2] * v2[1];
    n->y = v1[2] * v2[0] - v1[0] * v2[2];
    n->z = v1[0] * v2[1] - v1[1] * v2[0];
    float d = sqrt(n->x * n->x + n->y * n->y + n->z * n->z);

    if (d > 0)
    {
        n->x /= d;
        n->y /= d;
        n->z /= d;
    }
}

/*inline*/ void cross_T(normal * n, vertex * A, vertex * B, vertex * C)
{
    // look from top and count vectors counterclockwise
    float v1[3] = {C->Tx - B->Tx, C->Ty - B->Ty, C->Tz - B->Tz};
    float v2[3] = {A->Tx - B->Tx, A->Ty - B->Ty, A->Tz - B->Tz};
    n->Tx = v1[1] * v2[2] - v1[2] * v2[1];
    n->Ty = v1[2] * v2[0] - v1[0] * v2[2];
    n->Tz = v1[0] * v2[1] - v1[1] * v2[0];
    float d = sqrt(n->Tx * n->Tx + n->Ty * n->Ty + n->Tz * n->Tz);

    if (d > 0)
    {
        n->Tx /= d;
        n->Ty /= d;
        n->Tz /= d;
    }
}

void mean_Edge_Length(object * O)
{
    int e;

    edge * E;

    float l = 0;

    for (e = 0; e < O->edgecount; e ++)
    {
        E = &O->edges[e / ARRAYSIZE][e % ARRAYSIZE];
        l += E->B.radius;
    }
    if (O->edgecount > 0)
    {
        l /= O->edgecount;
        O->mean_Edge = l;
    }
}

float clamp_f(float v, float min1, float max1)
{
    if (v < min1) return min1;
    if (v > max1) return max1;
    return v;
}

int clamp_i(int v, int min1, int max1)
{
    if (v < min1) return min1;
    if (v > max1) return max1;
    return v;
}

#endif // MATHFUNCTIONS_H_INCLUDED
