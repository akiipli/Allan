/*
The MIT License

Copyright <2018> <Allan Kiipli>
*/

#ifndef OBJECT_H_INCLUDED
#define OBJECT_H_INCLUDED

//#include "SDL/SDL_gfxPrimitives.h"

#define OBJECTS 1000
#define OBJECTARRAY 10000 // these enable big files in
#define ARRAYSIZE 10000   //
#define SUBD 4
#define SUBD_SHAPE 3

#define PIXEL_VOLUME 100

#define INFLUENCES 1

typedef struct cp cp;
typedef struct curve curve;
typedef struct curve_segment curve_segment;

typedef struct
{
    float R;
    float G;
    float B;
    float A;
}
id_color;

typedef struct
{
    float x;
    float y;
    float z;
    float Tx;
    float Ty;
    float Tz;
    float radius;
    float Tradius;
}
box_3d;

typedef struct
{
    float u;
    float v;
    float radius;
}
box_2d;

typedef struct
{
    float x;
    float y;
    float z;
    float Tx; // transformed norms
    float Ty;
    float Tz;
}
normal;

typedef struct uv uv;

struct uv
{
    int index;
    int visible;
    int selected;
//    int color;
    float u;
    float v;
    int * uv_edges;
    int uvcount;
    int uvcount_fill; // help with subdivision
    int vert; // 3d vertex
    uv * uv_vert;
    float tangent[4];
};

void print_uv(uv * UV)
{
    printf("text   %d %d %d\n",
    UV->index,
    UV->visible,
    UV->uvcount);
    printf(" coord %f %f", UV->u, UV->v);
    int i;
    printf("\n uveds");
    for(i = 0; i < UV->uvcount; i++) printf(" %d", UV->uv_edges[i]);
    printf("\n");
}

typedef struct vertex vertex;
typedef struct cp cp;

struct vertex
{
    int index;
    int visible;
    int selected;
    float weight;
    float weight_init;
//    int color;
    float x;
    float y;
    float z;
    float Tx; // transformed verts
    float Ty;
    float Tz;
    vertex * vert;
//    float Rx; // radial coordinates, z is depth
//    float Ry;
//    float Rz;
    int * edges;
    int edgecount;
    int * uv_verts;
    int uv_vertcount;
    normal N;

    cp * control_point;
    object * O;
    int patch;
};

void print_vertex(vertex * V)
{
    printf("vert   %d %d %d\n",
    V->index,
    V->visible,
    V->edgecount);
    printf(" coord %f %f %f\n", V->x, V->y, V->z);
    printf(" norm  %f %f %f", V->N.x, V->N.y, V->N.z);
    int i;
    printf("\n edges");
    for(i = 0; i < V->edgecount; i++) printf(" %d", V->edges[i]);
    printf("\n");
}

typedef struct
{
    int index;
    int poly;
    int visible;
    int surface;
    int selected;
    int verts[3];
//    int color[3];
    int texts[3];
    float tangent[4];
    normal N;
    box_3d B;
    box_2d B2;
}
triangle;

void print_triangle(triangle * T)
{
    printf("trip   %d %d %d",
    T->index,
    T->poly,
    T->visible);
    int i;
    printf("\n verts");
    for(i = 0; i < 3; i++) printf(" %d", T->verts[i]);
    printf("\n texts");
    for(i = 0; i < 3; i++) printf(" %d", T->texts[i]);
    printf("\n");
    printf(" norm  %f %f %f\n", T->N.x, T->N.y, T->N.z);
    printf(" boxt  %f %f %f %f\n", T->B.x, T->B.y, T->B.z, T->B.radius);
}

typedef struct curve_segment curve_segment;
typedef struct edge edge;

struct edge
{
    int index;
    int visible;
    int subdivs;
    int selected;
    float weight;
    float weight_init;
    int verts[2];
//    int color[2];
    int edges[2];
    int uv_edges[2]; // split edges, same level
    int uv_edcount;
    int polys[2];
    int slots[2]; // edge index in edges polys or quads
    int polycount;
    normal N;
    box_3d B;
    float Mx; // polys mean center
    float My;
    float Mz;

    curve_segment * S;
    int flow[2]; // edge direction in edges polys or quads
    edge * perimeter; // inside edge has index for perimeter one level above
};

void print_edge(edge * E)
{
    printf("edge   %d %d %d %d",
    E->index,
    E->visible,
    E->subdivs,
    E->polycount);
    int i;
    printf("\n verts");
    for(i = 0; i < 2; i++) printf(" %d", E->verts[i]);
    printf("\n edges");
    for(i = 0; i < 2; i++) printf(" %d", E->edges[i]);
    printf("\n polys");
    for(i = 0; i < E->polycount; i++) printf(" %d", E->polys[i]);
    printf("\n");
    printf(" norm  %f %f %f\n", E->N.x, E->N.y, E->N.z);
    printf(" boxt  %f %f %f %f\n", E->B.x, E->B.y, E->B.z, E->B.radius);
}

typedef struct
{
    int index;
    int visible;
    int subdivs;
    int selected;
    int uv_verts[2];
//    int uv_color[2];
    int uv_edges[2];
    int split;
    int edge;
    int polys[2];
    int slots[2];
    int polycount;
    float tangent[4];
    box_2d B;
    float Mu; // polys mean center
    float Mv;
}
uv_edge;

void print_uv_edge(uv_edge * E)
{
    printf("uved   %d %d %d %d",
    E->index,
    E->visible,
    E->subdivs,
    E->polycount);
    int i;
    printf("\n uvvrs");
    for(i = 0; i < 2; i++) printf(" %d", E->uv_verts[i]);
    printf("\n uveds");
    for(i = 0; i < 2; i++) printf(" %d", E->uv_edges[i]);
    printf("\n polys");
    for(i = 0; i < E->polycount; i++) printf(" %d", E->polys[i]);
    printf("\n");
    printf(" boxt  %f %f %f\n", E->B.u, E->B.v, E->B.radius);
}

typedef struct
{
    int index;
    int color;
    int poly;
    int subdivs;
    int selected;
    int visible;
    int surface;
    int filler; // filler is help when filling in indexes
    int verts[4];
    int texts[4];
    int trips[2];
    int quads[4];
    int edges[4];
    int uveds[4];
    float tangent[4];
    normal N;
    box_3d B;
    box_2d B2;

    float center[3]; // for curve based subdivision, normally it equals mean center
    float dist;
}
quadrant;

void print_quadrant(quadrant * Q)
{
    printf("quad   %d %d %d %d",
    Q->index,
    Q->poly,
    Q->subdivs,
    Q->visible);
    int i;
    printf("\n verts");
    for(i = 0; i < 4; i++) printf(" %d", Q->verts[i]);
    printf("\n texts");
    for(i = 0; i < 4; i++) printf(" %d", Q->texts[i]);
    printf("\n trips");
    for(i = 0; i < 2; i++) printf(" %d", Q->trips[i]);
    printf("\n quads");
    for(i = 0; i < 4; i++) printf(" %d", Q->quads[i]);
    printf("\n edges");
    for(i = 0; i < 4; i++) printf(" %d", Q->edges[i]);
    printf("\n");
    printf(" norm  %f %f %f\n", Q->N.x, Q->N.y, Q->N.z);
    printf(" boxt  %f %f %f %f\n", Q->B.x, Q->B.y, Q->B.z, Q->B.radius);
    printf(" bx2d  %f %f %f\n", Q->B2.u, Q->B2.v, Q->B2.radius);
}

typedef struct
{
    int index;
    int color;
    int subdivs;
    int visible;
    int surface;
    int selected;
    int * trips;
    int * quads;
    int * edges;
    int * verts;
    int * texts;
    int * uveds;
    //int * uvdir;
    int tripcount;
    int edgecount;
    float tangent[4];
    normal N;
    box_3d B;
    box_2d B2;

    float center[3]; // for curve based subdivision, normally it equals mean center
    float dist;
}
polygon;

void print_polygon(polygon * P)
{
    printf("poly   %d %d %d %d %d %d",
    P->index,
    P->subdivs,
    P->visible,
    P->surface,
    P->tripcount,
    P->edgecount);
    int i;
    printf("\n trips");
    for(i = 0; i < P->tripcount; i++) printf(" %d", P->trips[i]);
    printf("\n quads");
    for(i = 0; i < P->edgecount; i++) printf(" %d", P->quads[i]);
    printf("\n edges");
    for(i = 0; i < P->edgecount; i++) printf(" %d", P->edges[i]);
    printf("\n verts");
    for(i = 0; i < P->edgecount; i++) printf(" %d", P->verts[i]);
    printf("\n uveds");
    for(i = 0; i < P->edgecount; i++) printf(" %d", P->uveds[i]);
    printf("\n");
    printf(" norm  %f %f %f\n", P->N.x, P->N.y, P->N.z);
    printf(" boxt  %f %f %f %f\n", P->B.x, P->B.y, P->B.z, P->B.radius);
    printf(" bx2d  %f %f %f\n", P->B2.u, P->B2.v, P->B2.radius);
}

typedef struct
{
    float Aggregate_dist;
    float Candidate_dist[INFLUENCES];
    bone * Candidate_bones[INFLUENCES];
    int Influences_Count;
}
weight_encapsulator;

struct object
{
    int index;
    unsigned address; // assigned after loading
    char * Name;
    int surface;
    int selected;
    int binding;
    int shadow;

    uv       ** uvtex;
    vertex   ** verts;
    id_color ** icols;
    triangle ** trips;
    edge     ** edges;
    uv_edge  ** uveds;
    polygon  ** polys;

    int textcount;
    int vertcount;
    int icolcount;
    int tripcount;
    int edgecount;
    int uvedcount;
    int polycount;

    uv       ** uvtex_[SUBD];
    vertex   ** verts_[SUBD];
    triangle ** trips_[SUBD];
    edge     ** edges_[SUBD];
    uv_edge  ** uveds_[SUBD];
    quadrant ** quads_[SUBD];

    int textcount_[SUBD];
    int vertcount_[SUBD];
    int tripcount_[SUBD];
    int edgecount_[SUBD];
    int uvedcount_[SUBD];
    int quadcount_[SUBD];

    box_3d B;
    float mean_Edge;

    transformer * T;
    int subdlevel;
    int tune[SUBD];
    int tune_in_uvtex[SUBD];
    int vertex_arrays[SUBD];
    int vertex_array;
    int deforms;

    GLfloat * vert_array[2];
    GLfloat * cols_array[2];
    GLfloat * norm_array[2];
    GLfloat * text_array[2];
    GLfloat * tang_array[2];
    GLuint * elem_array;

    int vert_array_size[2];
    int cols_array_size[2];
    int norm_array_size[2];
    int text_array_size[2];
    int tang_array_size[2];
    int elem_array_size;

    GLuint vert_array_buffer[2];
    GLuint cols_array_buffer[2];
    GLuint norm_array_buffer[2];
    GLuint text_array_buffer[2];
    GLuint tang_array_buffer[2];
    GLuint elem_array_buffer;

    GLfloat * vert_array_[SUBD][2];
    GLfloat * cols_array_[SUBD][2];
    GLfloat * norm_array_[SUBD][2];
    GLfloat * text_array_[SUBD][2];
    GLfloat * tang_array_[SUBD][2];
    GLuint * elem_array_[SUBD];

    int vert_array_size_[SUBD][2];
    int cols_array_size_[SUBD][2];
    int norm_array_size_[SUBD][2];
    int text_array_size_[SUBD][2];
    int tang_array_size_[SUBD][2];
    int elem_array_size_[SUBD];

    GLuint vert_array_buffer_[SUBD][2];
    GLuint cols_array_buffer_[SUBD][2];
    GLuint norm_array_buffer_[SUBD][2];
    GLuint text_array_buffer_[SUBD][2];
    GLuint tang_array_buffer_[SUBD][2];
    GLuint elem_array_buffer_[SUBD];

    selection ** polygon_selection;
    selection ** edge_selection;
    vert_selection ** vertex_selection;

    int polygon_selections;
    int edge_selections;
    int vertex_selections;

    int * selected_polys;
    int * selected_edges;
    int * selected_verts;

    int selected_polys_count;
    int selected_edges_count;
    int selected_verts_count;

    int last_selected_polys[2];
    int last_selected_edges[2];
    int last_selected_verts[2];

    weight_encapsulator * WEncapsulator;

    curve ** curves;
    int curve_count;
    curve_segment ** segments;
    int segment_count;
    cp ** cps;
    int cps_count;
};

void print_object(object * O)
{
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
        print_uv(UV);
    }
    for(i = 0; i < O->vertcount; i ++)
    {
        V = &O->verts[i / ARRAYSIZE][i % ARRAYSIZE];
        print_vertex(V);
    }
    for(i = 0; i < O->tripcount; i ++)
    {
        T = &O->trips[i / ARRAYSIZE][i % ARRAYSIZE];
        print_triangle(T);
    }
    for(i = 0; i < O->edgecount; i ++)
    {
        E = &O->edges[i / ARRAYSIZE][i % ARRAYSIZE];
        print_edge(E);
    }
    for(i = 0; i < O->uvedcount; i ++)
    {
        UVE = &O->uveds[i / ARRAYSIZE][i % ARRAYSIZE];
        print_uv_edge(UVE);
    }
    for(i = 0; i < O->polycount; i ++)
    {
        P = &O->polys[i / ARRAYSIZE][i % ARRAYSIZE];
        print_polygon(P);
    }
    printf(" boxt %s %f %f %f %f\n", O->Name, O->B.x, O->B.y, O->B.z, O->B.radius);
}

object    * objects[OBJECTS];
object    * deleted_objects[OBJECTS];
polygon  ** polys[OBJECTS];

uv       ** uvtex[OBJECTS];
vertex   ** verts[OBJECTS];
id_color ** icols[OBJECTS];
triangle ** trips[OBJECTS];
edge     ** edges[OBJECTS];
uv_edge  ** uveds[OBJECTS];

uv       ** uvtex_[SUBD][OBJECTS];
vertex   ** verts_[SUBD][OBJECTS];
triangle ** trips_[SUBD][OBJECTS];
edge     ** edges_[SUBD][OBJECTS];
uv_edge  ** uveds_[SUBD][OBJECTS];
quadrant ** quads_[SUBD][OBJECTS];

int deleted_objects_count = 0;
int loaded_objects[OBJECTS];
int selected_objects[OBJECTS];
int selected_object_count = 0;
int objectIndex = 0;

transformer World;

#endif // OBJECT_H_INCLUDED
