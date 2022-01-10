/*
The MIT License

Copyright <2018> <Allan Kiipli>
*/

// Allocate objects and their elements to expanding two dimensional arrays.
// Arrays are global in nature, but objects reference them individually.
// TODO: find a way to delete objects without destroying global arrays.
// All arrays duplicate into subdivision quads.

#ifndef INITIALIZINGFUNCTIONS_H_INCLUDED
#define INITIALIZINGFUNCTIONS_H_INCLUDED

void free_labels()
{
    int l;
    for (l = 0; l < LABELS; l ++)
    {
        free(labels[l]);
    }
}

void create_Transformers_List();

void free_ikChain(ikChain * I)
{
    free(I->Name);
    free(I->Bones);
    free(I->vectors);
    free(I->vectors_bone);
    free(I->positions_A);
    free(I->positions_B);
    free(I->bones_Rot);
    free(I);
}

void free_ikChains()
{
    int i;
    ikChain * I;
    for (i = 0; i < iksIndex; i ++)
    {
        I = ikChains[i];
        free_ikChain(I);
    }
}

void free_Constraints()
{
    int c;
    constraint * C;
    for (c = 0; c < constraintsIndex; c ++)
    {
        C = constraints[c];
        free_Constraint(C);
    }
}

void free_subcharacter(subcharacter * S)
{
    free(S->Name);
    free(S->Transformers);
    free(S->Bones);
    free(S->Poses);
    free(S->Subcharacters);
    free(S);
}

void free_Subcharacters()
{
    int s;
    subcharacter * S;
    for (s = 0; s < subcharacterIndex; s ++)
    {
        S = subcharacters[s];
        free_subcharacter(S);
    }
}

void free_bone(bone * B)
{
    int d;
    free(B->Name);
    for (d = 0; d < B->Distances_Count; d ++)
    {
        free(B->Distances[d]);
    }
    free(B->Distances);
    free(B);
}

void free_bones()
{
    int b;
    bone * B;
    for (b = 0; b < bonesIndex; b ++)
    {
        B = bones[b];
        free_bone(B);
    }
}

void free_poses()
{
    int p;
    pose * P;

    for(p = 0; p < posesIndex; p ++)
    {
        P = poses[p];
        free_pose(P);
    }
}

int free_subcharacter_Pose(subcharacter * S, pose * P)
{
    int i, p, index, index0;

    int condition = 0;

    for (p = 0; p < S->Poses_Count; p ++)
    {
        if (P == S->Poses[p])
        {
            index = P->index;
            index0 = p;
            condition = 1;
            break;
        }
    }

    if (condition)
    {
        S->Poses_Count --;
        for (i = index0; i < S->Poses_Count; i ++)
        {
            S->Poses[i] = S->Poses[i + 1];
        }

        subcharacter_posesIndex --;
        for (i = index; i < subcharacter_posesIndex; i ++)
        {
            subcharacter_Poses[i] = subcharacter_Poses[i + 1];
            subcharacter_Poses[i]->index = i;
        }
        free_pose(P);
    }

    return condition;
}

void free_subcharacter_Poses(subcharacter * S)
{
    int p, i;
    pose * P;

    for (p = 0; p < S->Poses_Count; p ++)
    {
        P = S->Poses[p];

        subcharacter_posesIndex --;
        for (i = P->index; i < subcharacter_posesIndex; i ++)
        {
            subcharacter_Poses[i] = subcharacter_Poses[i + 1];
            subcharacter_Poses[i]->index = i;
        }
        free_pose(P);
    }
}

void free_subcharacter_poses()
{
    int p;
    pose * P;

    for(p = 0; p < subcharacter_posesIndex; p ++)
    {
        P = subcharacter_Poses[p];
        free_pose(P);
    }
}

void free_deformers()
{
    int d;
    deformer * D;

    for(d = 0; d < deformerIndex; d ++)
    {
        D = deformers[d];
        free_Deformer(D);
    }
}

void free_Transformer(transformer * T)
{
    free(T->Name);
    free(T->childs);
    free(T->Selections);
}

void free_transformers()
{
    create_Transformers_List();
    int t;
    transformer * T;

    for (t = 1; t < transformerIndex; t ++)
    {
        T = transformers[t];
        free_Transformer(T);
    }

    int l;

    for (l = 0; l < locatorIndex; l ++)
    {
        //free(Locators[l]->Name);
        //free(Locators[l]->childs);
        free(Locators[l]);
    }
}

void free_selections(object * O)
{
    int i;
    for (i = 0; i < O->polygon_selections; i ++)
    {
        free(O->polygon_selection[i]->indices);
        free(O->polygon_selection[i]->Name);
        free(O->polygon_selection[i]);
    }
    free(O->polygon_selection);
    O->polygon_selections = 0;

    for (i = 0; i < O->edge_selections; i ++)
    {
        free(O->edge_selection[i]->indices);
        free(O->edge_selection[i]->Name);
        free(O->edge_selection[i]);
    }
    free(O->edge_selection);
    O->edge_selections = 0;

    for (i = 0; i < O->vertex_selections; i ++)
    {
        free(O->vertex_selection[i]->indices);
        free(O->vertex_selection[i]->weights);
        free(O->vertex_selection[i]->Name);
        free(O->vertex_selection[i]->split[0].indices);
        free(O->vertex_selection[i]->split[0].weights);
        free(O->vertex_selection[i]->split[1].indices);
        free(O->vertex_selection[i]->split[1].weights);
        free(O->vertex_selection[i]);
    }
    free(O->vertex_selection);
    O->vertex_selections = 0;
}

void free_object_Level(object * O, int l)
{
    int i, c;

    for (i = 0; i < O->textcount_[l]; i ++)
    {
        free(O->uvtex_[l][i / ARRAYSIZE][i % ARRAYSIZE].uv_edges);
    }

    i = 0;
    c = O->textcount_[l] / ARRAYSIZE;
    while(i <= c)
    {
        free(O->uvtex_[l][i]);
        i ++;
    }

    for (i = 0; i < O->vertcount_[l]; i ++)
    {
        free(O->verts_[l][i / ARRAYSIZE][i % ARRAYSIZE].edges);
    }

    i = 0;
    c = O->vertcount_[l] / ARRAYSIZE;
    while(i <= c)
    {
        free(O->verts_[l][i]);
        i ++;
    }

    i = 0;
    c = O->tripcount_[l] / ARRAYSIZE;
    while(i <= c)
    {
        free(O->trips_[l][i]);
        i ++;
    }

    for (i = 0; i < O->edgecount_[l]; i ++)
    {
//        free(O->edges_[l][i / ARRAYSIZE][i % ARRAYSIZE].polys);
//        free(O->edges_[l][i / ARRAYSIZE][i % ARRAYSIZE].slots);
    }

    i = 0;
    c = O->edgecount_[l] / ARRAYSIZE;
    while(i <= c)
    {
        free(O->edges_[l][i]);
        i ++;
    }

    for (i = 0; i < O->uvedcount_[l]; i ++)
    {
//        free(O->uveds_[l][i / ARRAYSIZE][i % ARRAYSIZE].polys);
//        free(O->uveds_[l][i / ARRAYSIZE][i % ARRAYSIZE].slots);
    }

    i = 0;
    c = O->uvedcount_[l] / ARRAYSIZE;
    while(i <= c)
    {
        free(O->uveds_[l][i]);
        i ++;
    }

    i = 0;
    c = O->quadcount_[l] / ARRAYSIZE;
    while(i <= c)
    {
        free(O->quads_[l][i]);
        i ++;
    }
    free(O->uvtex_[l]);
    free(O->verts_[l]);
    free(O->trips_[l]);
    free(O->edges_[l]);
    free(O->uveds_[l]);
    free(O->quads_[l]);

    if (O->vertex_arrays[l])
    {
        free(O->text_array_[l][0]);
        free(O->vert_array_[l][0]);
        free(O->cols_array_[l][0]);
        free(O->norm_array_[l][0]);
        free(O->tang_array_[l][0]);
        free(O->text_array_[l][1]);
        free(O->vert_array_[l][1]);
        free(O->cols_array_[l][1]);
        free(O->norm_array_[l][1]);
        free(O->tang_array_[l][1]);
        free(O->elem_array_[l]);

        glDeleteBuffers(1, &O->vert_array_buffer_[l][0]);
        glDeleteBuffers(1, &O->cols_array_buffer_[l][0]);
        glDeleteBuffers(1, &O->norm_array_buffer_[l][0]);
        glDeleteBuffers(1, &O->text_array_buffer_[l][0]);
        glDeleteBuffers(1, &O->tang_array_buffer_[l][0]);
        glDeleteBuffers(1, &O->vert_array_buffer_[l][1]);
        glDeleteBuffers(1, &O->cols_array_buffer_[l][1]);
        glDeleteBuffers(1, &O->norm_array_buffer_[l][1]);
        glDeleteBuffers(1, &O->text_array_buffer_[l][1]);
        glDeleteBuffers(1, &O->tang_array_buffer_[l][1]);
        glDeleteBuffers(1, &O->elem_array_buffer_[l]);
    }
}

void free_object(object * O)
{
    int i;
    int c;

    for (i = 0; i < O->textcount; i ++)
    {
        free(O->uvtex[i / ARRAYSIZE][i % ARRAYSIZE].uv_edges);
    }

    i = 0;
    c = O->textcount / ARRAYSIZE;
    while(i <= c)
    {
        free(O->uvtex[i]);
        i ++;
    }

    for (i = 0; i < O->vertcount; i ++)
    {
        free(O->verts[i / ARRAYSIZE][i % ARRAYSIZE].edges);
        free(O->verts[i / ARRAYSIZE][i % ARRAYSIZE].uv_verts);
    }

    i = 0;
    c = O->vertcount / ARRAYSIZE;
    while(i <= c)
    {
        free(O->verts[i]);
        i ++;
    }

    i = 0;
    c = O->icolcount / ARRAYSIZE;
    while(i <= c)
    {
        free(O->icols[i]);
        i ++;
    }

    i = 0;
    c = O->tripcount / ARRAYSIZE;
    while(i <= c)
    {
        free(O->trips[i]);
        i ++;
    }

//    for (i = 0; i < O->edgecount; i ++)
//    {
//        free(O->edges[i / ARRAYSIZE][i % ARRAYSIZE].polys);
//        free(O->edges[i / ARRAYSIZE][i % ARRAYSIZE].slots);
//    }

    i = 0;
    c = O->edgecount / ARRAYSIZE;
    while(i <= c)
    {
        free(O->edges[i]);
        i ++;
    }

//    for (i = 0; i < O->uvedcount; i ++)
//    {
//        free(O->uveds[i / ARRAYSIZE][i % ARRAYSIZE].polys);
//        free(O->uveds[i / ARRAYSIZE][i % ARRAYSIZE].slots);
//    }

    i = 0;
    c = O->uvedcount / ARRAYSIZE;
    while(i <= c)
    {
        free(O->uveds[i]);
        i ++;
    }

    polygon * P;

    for (i = 0; i < O->polycount; i ++)
    {
        P = &O->polys[i / ARRAYSIZE][i % ARRAYSIZE];
        free(P->trips);
        free(P->quads);
        free(P->edges);
        free(P->verts);
        free(P->texts);
        free(P->uveds);
//        free(P->uvdir);
    }

    i = 0;
    c = O->polycount / ARRAYSIZE;
    while(i <= c)
    {
        free(O->polys[i]);
        i ++;
    }
    free(O->uvtex);
    free(O->verts);
    free(O->icols);
    free(O->trips);
    free(O->edges);
    free(O->uveds);
    free(O->polys);

    for (i = O->index; i < objectIndex - 1; i ++)
    {
        uvtex[i] = uvtex[i + 1];
        verts[i] = verts[i + 1];
        icols[i] = icols[i + 1];
        trips[i] = trips[i + 1];
        edges[i] = edges[i + 1];
        uveds[i] = uveds[i + 1];
        polys[i] = polys[i + 1];
    }

    free(O->selected_polys);
    free(O->selected_edges);
    free(O->selected_verts);

    int l;

    for (l = 0; l <= O->subdlevel; l ++)
    {
        for (i = 0; i < O->textcount_[l]; i ++)
        {
            free(O->uvtex_[l][i / ARRAYSIZE][i % ARRAYSIZE].uv_edges);
        }

        i = 0;
        c = O->textcount_[l] / ARRAYSIZE;
        while(i <= c)
        {
            free(O->uvtex_[l][i]);
            i ++;
        }

        for (i = 0; i < O->vertcount_[l]; i ++)
        {
            free(O->verts_[l][i / ARRAYSIZE][i % ARRAYSIZE].edges);
        }

        i = 0;
        c = O->vertcount_[l] / ARRAYSIZE;
        while(i <= c)
        {
            free(O->verts_[l][i]);
            i ++;
        }

        i = 0;
        c = O->tripcount_[l] / ARRAYSIZE;
        while(i <= c)
        {
            free(O->trips_[l][i]);
            i ++;
        }

        for (i = 0; i < O->edgecount_[l]; i ++)
        {
//            free(O->edges_[l][i / ARRAYSIZE][i % ARRAYSIZE].polys);
//            free(O->edges_[l][i / ARRAYSIZE][i % ARRAYSIZE].slots);
        }

        i = 0;
        c = O->edgecount_[l] / ARRAYSIZE;
        while(i <= c)
        {
            free(O->edges_[l][i]);
            i ++;
        }

        for (i = 0; i < O->uvedcount_[l]; i ++)
        {
//            free(O->uveds_[l][i / ARRAYSIZE][i % ARRAYSIZE].polys);
//            free(O->uveds_[l][i / ARRAYSIZE][i % ARRAYSIZE].slots);
        }

        i = 0;
        c = O->uvedcount_[l] / ARRAYSIZE;
        while(i <= c)
        {
            free(O->uveds_[l][i]);
            i ++;
        }

        i = 0;
        c = O->quadcount_[l] / ARRAYSIZE;
        while(i <= c)
        {
            free(O->quads_[l][i]);
            i ++;
        }
        free(O->uvtex_[l]);
        free(O->verts_[l]);
        free(O->trips_[l]);
        free(O->edges_[l]);
        free(O->uveds_[l]);
        free(O->quads_[l]);

        for (i = O->index; i < objectIndex - 1; i ++)
        {
            uvtex_[l][i] = uvtex_[l][i + 1];
            verts_[l][i] = verts_[l][i + 1];
            trips_[l][i] = trips_[l][i + 1];
            edges_[l][i] = edges_[l][i + 1];
            uveds_[l][i] = uveds_[l][i + 1];
            quads_[l][i] = quads_[l][i + 1];
        }
    }

    if (O->vertex_array)
    {
        free(O->text_array[0]);
        free(O->vert_array[0]);
        free(O->cols_array[0]);
        free(O->norm_array[0]);
        free(O->tang_array[0]);
        free(O->text_array[1]);
        free(O->vert_array[1]);
        free(O->cols_array[1]);
        free(O->norm_array[1]);
        free(O->tang_array[1]);
        free(O->elem_array);
        glDeleteBuffers(1, &O->vert_array_buffer[0]);
        glDeleteBuffers(1, &O->cols_array_buffer[0]);
        glDeleteBuffers(1, &O->norm_array_buffer[0]);
        glDeleteBuffers(1, &O->text_array_buffer[0]);
        glDeleteBuffers(1, &O->tang_array_buffer[0]);
        glDeleteBuffers(1, &O->vert_array_buffer[1]);
        glDeleteBuffers(1, &O->cols_array_buffer[1]);
        glDeleteBuffers(1, &O->norm_array_buffer[1]);
        glDeleteBuffers(1, &O->text_array_buffer[1]);
        glDeleteBuffers(1, &O->tang_array_buffer[1]);
        glDeleteBuffers(1, &O->elem_array_buffer);
    }
    for (l = 0; l <= O->subdlevel; l ++)
    {
        if (O->vertex_arrays[l])
        {
            free(O->text_array_[l][0]);
            free(O->vert_array_[l][0]);
            free(O->cols_array_[l][0]);
            free(O->norm_array_[l][0]);
            free(O->tang_array_[l][0]);
            free(O->text_array_[l][1]);
            free(O->vert_array_[l][1]);
            free(O->cols_array_[l][1]);
            free(O->norm_array_[l][1]);
            free(O->tang_array_[l][1]);
            free(O->elem_array_[l]);

            glDeleteBuffers(1, &O->vert_array_buffer_[l][0]);
            glDeleteBuffers(1, &O->cols_array_buffer_[l][0]);
            glDeleteBuffers(1, &O->norm_array_buffer_[l][0]);
            glDeleteBuffers(1, &O->text_array_buffer_[l][0]);
            glDeleteBuffers(1, &O->tang_array_buffer_[l][0]);
            glDeleteBuffers(1, &O->vert_array_buffer_[l][1]);
            glDeleteBuffers(1, &O->cols_array_buffer_[l][1]);
            glDeleteBuffers(1, &O->norm_array_buffer_[l][1]);
            glDeleteBuffers(1, &O->text_array_buffer_[l][1]);
            glDeleteBuffers(1, &O->tang_array_buffer_[l][1]);
            glDeleteBuffers(1, &O->elem_array_buffer_[l]);
        }
    }

    free(O->Name);
    free(O->WEncapsulator);

    //loaded_objects[O->index] = 0;

    free_selections(O);
    free(O->curves);
    free(O->segments);
    free(O->cps);
    free(O);
}

int initialize_object(int index,
    int textcount,
    int vertcount,
    int icolcount,
    int tripcount,
    int edgecount,
    int uvedcount,
    int polycount,
    int put_transformer)
{

    object * O = malloc(sizeof(object));

    objects[index] = O;

    if (put_transformer)
    {
        O->T = calloc(1, sizeof(transformer));
        init_transformer(O->T, &World, "Object");
        O->T->Object = O;
        O->T->rot_Order = zxy;
    }

    uvtex[index] = (uv**)      malloc(OBJECTARRAY * sizeof(uv*));
    if (uvtex[index] == NULL) return 0;
    verts[index] = (vertex**)  malloc(OBJECTARRAY * sizeof(vertex*));
    if (verts[index] == NULL) return 0;
    icols[index] = (id_color**)malloc(OBJECTARRAY * sizeof(id_color*));
    if (icols[index] == NULL) return 0;
    trips[index] = (triangle**)malloc(OBJECTARRAY * sizeof(triangle*));
    if (trips[index] == NULL) return 0;
    edges[index] = (edge**)    malloc(OBJECTARRAY * sizeof(edge*));
    if (edges[index] == NULL) return 0;
    uveds[index] = (uv_edge**) malloc(OBJECTARRAY * sizeof(uv_edge*));
    if (uveds[index] == NULL) return 0;
    polys[index] = (polygon**) malloc(OBJECTARRAY * sizeof(polygon*));
    if (polys[index] == NULL) return 0;

    O->selected_polys = malloc(polycount * sizeof(int));
    if (O->selected_polys == NULL) return 0;
    O->selected_edges = malloc(edgecount * sizeof(int));
    if (O->selected_edges == NULL) return 0;
    O->selected_verts = malloc(vertcount * sizeof(int));
    if (O->selected_verts == NULL) return 0;

    O->selected_polys_count = 0;
    O->selected_edges_count = 0;
    O->selected_verts_count = 0;

    int i;
    int c;

    i = 0;
    c = textcount / ARRAYSIZE;
    while(i <= c)
    {
        uvtex[index][i] = (uv*) calloc(ARRAYSIZE, sizeof(uv));
        if (uvtex[index][i] == NULL)
        {
            textcount = i * ARRAYSIZE;
            return 0;
        }
        i ++;
    }

    i = 0;
    c = vertcount / ARRAYSIZE;
    while(i <= c)
    {
        verts[index][i] = (vertex*) calloc(ARRAYSIZE, sizeof(vertex));
        if (verts[index][i] == NULL)
        {
            vertcount = i * ARRAYSIZE;
            return 0;
        }
        i ++;
    }

    i = 0;
    c = icolcount / ARRAYSIZE;
    while(i <= c)
    {
        icols[index][i] = (id_color*) calloc(ARRAYSIZE, sizeof(id_color));
        if (icols[index][i] == NULL)
        {
            icolcount = i * ARRAYSIZE;
            return 0;
        }
        i ++;
    }

    i = 0;
    c = tripcount / ARRAYSIZE;
    while(i <= c)
    {
        trips[index][i] = (triangle*) calloc(ARRAYSIZE, sizeof(triangle));
        if (trips[index][i] == NULL)
        {
            tripcount = i * ARRAYSIZE;
            return 0;
        }
        i ++;
    }

    i = 0;
    c = edgecount / ARRAYSIZE;
    while(i <= c)
    {
        edges[index][i] = (edge*) calloc(ARRAYSIZE, sizeof(edge));
        if (edges[index][i] == NULL)
        {
            edgecount = i * ARRAYSIZE;
            return 0;
        }
        i ++;
    }

    i = 0;
    c = uvedcount / ARRAYSIZE;
    while(i <= c)
    {
        uveds[index][i] = (uv_edge*) calloc(ARRAYSIZE, sizeof(uv_edge));
        if (uveds[index][i] == NULL)
        {
            uvedcount = i * ARRAYSIZE;
            return 0;
        }
        i ++;
    }

    i = 0;
    c = polycount / ARRAYSIZE;
    while(i <= c)
    {
        polys[index][i] = (polygon*) calloc(ARRAYSIZE, sizeof(polygon));
        if (polys[index][i] == NULL)
        {
            polycount = i * ARRAYSIZE;
            return 0;
        }
        i ++;
    }

    O->index = index;
    O->Name  = malloc(STRLEN * sizeof(char));
    if (O->Name == NULL) return 0;

    O->uvtex = uvtex[index];
    O->verts = verts[index];
    O->icols = icols[index];
    O->trips = trips[index];
    O->edges = edges[index];
    O->uveds = uveds[index];
    O->polys = polys[index];

    O->textcount = textcount;
    O->vertcount = vertcount;
    O->icolcount = icolcount;
    O->tripcount = tripcount;
    O->edgecount = edgecount;
    O->uvedcount = uvedcount;
    O->polycount = polycount;

    O->subdlevel = -1;

    O->deforms = 1;

    O->binding = 0;

    O->shadow = 1;

    O->smooth = 1;

    for (i = 0; i < SUBD; i ++)
    {
        O->textcount_[i] = 0;
        O->vertcount_[i] = 0;
        O->tripcount_[i] = 0;
        O->edgecount_[i] = 0;
        O->uvedcount_[i] = 0;
        O->quadcount_[i] = 0;

        O->tune[i] = 1;
        O->tune_in_uvtex[i] = 1;
        O->vertex_arrays[i] = 0;
    }

    O->selected = 0;

    O->polygon_selections = 0;
    O->edge_selections = 0;
    O->vertex_selections = 0;

    O->curves = malloc(OBJECT_CURVES * sizeof(curve*));
    O->curve_count = 0;
    O->segments = malloc(OBJECT_SEGMENTS * sizeof(curve_segment*));
    O->segment_count = 0;
    O->cps = malloc(OBJECT_CPS * sizeof(cp*));
    O->cps_count = 0;

    O->polygon_selection = malloc(0 * sizeof(selection*));
    O->edge_selection = malloc(0 * sizeof(selection*));
    O->vertex_selection = malloc(0 * sizeof(vert_selection*));

    O->WEncapsulator = malloc(O->vertcount * sizeof(weight_encapsulator));

    loaded_objects[index] = 1;
    return 1;
}

/*inline*/ void init_uv(int index, uv * UV, uv * DAT)
{
    UV->index = index;
    UV->visible = DAT->visible;
    UV->selected = DAT->selected;
    UV->u = DAT->u;
    UV->v = DAT->v;
    UV->uv_edges = DAT->uv_edges;
    UV->uvcount = DAT->uvcount;
}

/*inline*/ void init_vertex(int index, vertex * V, vertex * DAT)
{
    V->index = index;
    V->visible = DAT->visible;
    V->selected = DAT->selected;
    V->x = DAT->x;
    V->y = DAT->y;
    V->z = DAT->z;
    V->edges = DAT->edges;
    V->edgecount = DAT->edgecount;
}

/*inline*/ void init_triangle(int index, object * O, triangle * T, triangle * DAT)
{
    T->index = index;
    T->poly = DAT->poly;
    T->visible = DAT->visible;
    T->selected = DAT->selected;
    T->verts[0] = DAT->verts[0];
    T->verts[1] = DAT->verts[1];
    T->verts[2] = DAT->verts[2];
    T->texts[0] = DAT->texts[0];
    T->texts[1] = DAT->texts[1];
    T->texts[2] = DAT->texts[2];

    vertex * V0 = &O->verts[T->verts[0] / ARRAYSIZE][T->verts[0] % ARRAYSIZE];
    vertex * V1 = &O->verts[T->verts[1] / ARRAYSIZE][T->verts[1] % ARRAYSIZE];
    vertex * V2 = &O->verts[T->verts[2] / ARRAYSIZE][T->verts[2] % ARRAYSIZE];
    cross(&T->N, V0, V1, V2);
    boxt_3(&T->B, V0, V1, V2, 1);
}

/*inline*/ void init_edge(int index, object * O, edge * E, edge * DAT)
{
    E->index = index;
    E->visible = DAT->visible;
    E->subdivs = DAT->subdivs;
    E->selected = DAT->selected;
    E->verts[0] = DAT->verts[0];
    E->verts[1] = DAT->verts[1];
    E->edges[0] = DAT->edges[0];
    E->edges[1] = DAT->edges[1];
//    E->polys = DAT->polys;
//    E->slots = DAT->slots;
    E->polycount = DAT->polycount;
    vertex * V0 = &O->verts[E->verts[0] / ARRAYSIZE][E->verts[0] % ARRAYSIZE];
    vertex * V1 = &O->verts[E->verts[1] / ARRAYSIZE][E->verts[1] % ARRAYSIZE];
    boxt_2(&E->B, V0, V1, 1);
}

/*inline*/ void init_uv_edge(int index, object * O, uv_edge * E, uv_edge * DAT)
{
    E->index = index;
    E->visible = DAT->visible;
    E->subdivs = DAT->subdivs;
    E->selected = DAT->selected;
    E->uv_verts[0] = DAT->uv_verts[0];
    E->uv_verts[1] = DAT->uv_verts[1];
    E->uv_edges[0] = DAT->uv_edges[0];
    E->uv_edges[1] = DAT->uv_edges[1];
//    E->polys = DAT->polys;
//    E->slots = DAT->slots;
    E->polycount = DAT->polycount;
    uv * V0 = &O->uvtex[E->uv_verts[0] / ARRAYSIZE][E->uv_verts[0] % ARRAYSIZE];
    uv * V1 = &O->uvtex[E->uv_verts[1] / ARRAYSIZE][E->uv_verts[1] % ARRAYSIZE];
    boxt_0(&E->B, V0, V1, 1);
    E->split = 0;
}

/*inline*/ void init_quadrant(int index, quadrant * Q, quadrant * DAT)
{
    Q->index = index;
    Q->subdivs = DAT->subdivs;
    Q->visible = DAT->visible;
    Q->surface = DAT->surface;
    Q->selected = DAT->selected;
    Q->verts[0] = DAT->verts[0];
    Q->verts[1] = DAT->verts[1];
    Q->verts[2] = DAT->verts[2];
    Q->verts[3] = DAT->verts[3];
    Q->texts[0] = DAT->texts[0];
    Q->texts[1] = DAT->texts[1];
    Q->texts[2] = DAT->texts[2];
    Q->texts[3] = DAT->texts[3];
}

/*inline*/ void init_color(int index, id_color * I, id_color * DAT)
{
    I->R = DAT->R;
    I->G = DAT->G;
    I->B = DAT->B;
    I->A = DAT->A;
}

/*inline*/ void init_polygon(int index, polygon * P, polygon * DAT)
{
    P->index = index;
    P->color = DAT->color;
    P->subdivs = DAT->subdivs;
    P->visible = DAT->visible;
    P->surface = DAT->surface;
    P->selected = DAT->selected;
    P->trips = DAT->trips;
    P->quads = DAT->quads;
    P->edges = DAT->edges;
    P->verts = DAT->verts;
    P->texts = DAT->texts;
    P->uveds = DAT->uveds;
//    P->uvdir = DAT->uvdir;
    P->tripcount = DAT->tripcount;
    P->edgecount = DAT->edgecount;
}

void init_object_uvs(object * O, int start, float * Coords, int * Edges, int E_LEN, int * E_tiks, int LEN)
{
    int i;
    int c_E = 0;
    int c_v = 0;
    int c_e = 0;
    int LEN_2 = LEN * 2;
    for(i = start; i < O->textcount; i ++)
    {
        uv * UV = &O->uvtex[i / ARRAYSIZE][i % ARRAYSIZE];
        UV->index = i;
        UV->visible = 0;
        UV->selected = 0;
        UV->u = Coords[c_v++ % LEN_2];
        UV->v = Coords[c_v++ % LEN_2];
        UV->uvcount = E_tiks[c_E++ % LEN];
        UV->uv_edges = malloc(UV->uvcount * sizeof(int));
        int j;
        for(j = 0; j < UV->uvcount; j++)
        {
            UV->uv_edges[j] = Edges[c_e++ % E_LEN];
        }
        //init_uv(i, &O->uvtex[i / ARRAYSIZE][i % ARRAYSIZE], &UV);
    }
}

void init_object_verts(object * O, int start, float * Coords, int * Edges, int E_LEN, int * E_tiks, int LEN)
{
    int i;
    int c_E = 0;
    int c_v = 0;
    int c_e = 0;
    int LEN_3 = LEN * 3;
    for(i = start; i < O->vertcount; i ++)
    {
        vertex * V = &O->verts[i / ARRAYSIZE][i % ARRAYSIZE];
        V->O = O;
        V->index = i;
        V->visible = 0;
        V->selected = 0;
        V->weight = 0;
        V->weight_init = 0;
        V->vert = NULL;
        V->uv_vertcount = 0;
        V->x = Coords[c_v++ % LEN_3];
        V->y = Coords[c_v++ % LEN_3];
        V->z = Coords[c_v++ % LEN_3];

        V->Rx = V->x; // morph coordinates
        V->Ry = V->y;
        V->Rz = V->z;

        V->edgecount = E_tiks[c_E++ % LEN];
        V->edges = malloc(V->edgecount * sizeof(int));
        int j;
        for(j = 0; j < V->edgecount; j++)
        {
            V->edges[j] = Edges[c_e++ % E_LEN];
        }
        V->control_point = NULL;
        //init_vertex(i, &O->verts[i / ARRAYSIZE][i % ARRAYSIZE], &V);
    }
}

void init_object_triangles(object * O, int start, int * Idx_V, int * Idx_T, int LEN)
{
    int i;
    int c_v = 0;
    int c_t = 0;
    int LEN_3 = LEN * 3;
    for(i = start; i < O->tripcount; i ++)
    {
        triangle * T = &O->trips[i / ARRAYSIZE][i % ARRAYSIZE];
        T->index = i;
        T->poly = -1;
        T->visible = 1;
        T->selected = 0;
        T->verts[0] = Idx_V[c_v++ % LEN_3];
        T->verts[1] = Idx_V[c_v++ % LEN_3];
        T->verts[2] = Idx_V[c_v++ % LEN_3];
        T->texts[0] = Idx_T[c_t++ % LEN_3];
        T->texts[1] = Idx_T[c_t++ % LEN_3];
        T->texts[2] = Idx_T[c_t++ % LEN_3];
        //init_triangle(i, O, &O->trips[i / ARRAYSIZE][i % ARRAYSIZE], &T);

        vertex * V0 = &O->verts[T->verts[0] / ARRAYSIZE][T->verts[0] % ARRAYSIZE];
        vertex * V1 = &O->verts[T->verts[1] / ARRAYSIZE][T->verts[1] % ARRAYSIZE];
        vertex * V2 = &O->verts[T->verts[2] / ARRAYSIZE][T->verts[2] % ARRAYSIZE];
        cross(&T->N, V0, V1, V2);
        boxt_3(&T->B, V0, V1, V2, 1);
    }
}

void init_object_edges(object * O, int start, int * Idx_V, int * e_polys, int * e_slots, int P_LEN, int * P_Tiks, int LEN)
{
    int i;
    int c_v = 0;
    int c_p = 0;
    int c_e = 0;
    int c_s = 0;
    int LEN_2 = LEN * 2;
    for(i = start; i < O->edgecount; i ++)
    {
        edge * E = &O->edges[i / ARRAYSIZE][i % ARRAYSIZE];
        E->index = i;
        E->visible = 1;
        E->subdivs = 0;
        E->selected = 0;
        E->weight = 0;
        E->weight_init = 0;
        E->polycount = P_Tiks[c_p++ % LEN];
//        E->polys = malloc(E->polycount * sizeof(int));
//        E->slots = malloc(E->polycount * sizeof(int));
        E->uv_edcount = 0;
        int j;
        for(j = 0; j < E->polycount; j++)
        {
            E->polys[j] = e_polys[c_e++ % P_LEN];
            E->slots[j] = e_slots[c_s++ % P_LEN];
        }
        E->verts[0] = Idx_V[c_v++ % LEN_2];
        E->verts[1] = Idx_V[c_v++ % LEN_2];
        E->edges[0] = -1;
        E->edges[1] = -1;
        //init_edge(i, O, &O->edges[i / ARRAYSIZE][i % ARRAYSIZE], &E);

        vertex * V0 = &O->verts[E->verts[0] / ARRAYSIZE][E->verts[0] % ARRAYSIZE];
        vertex * V1 = &O->verts[E->verts[1] / ARRAYSIZE][E->verts[1] % ARRAYSIZE];
        boxt_2(&E->B, V0, V1, 1);
        E->S = NULL;
        E->O = O;
    }
}

void init_object_uv_edges(object * O, int start, int * Idx_V, int * e_polys, int P_LEN, int * P_Tiks, int LEN)
{
    int i;
    int c_v = 0;
    int c_p = 0;
    int c_e = 0;
    int LEN_2 = LEN * 2;
    for(i = start; i < O->uvedcount; i ++)
    {
        uv_edge * E = &O->uveds[i / ARRAYSIZE][i % ARRAYSIZE];
        E->index = i;
        E->visible = 1;
        E->subdivs = 0;
        E->selected = 0;
        E->polycount = P_Tiks[c_p++ % LEN];
//        E->polys = malloc(E->polycount * sizeof(int));
//        E->slots = malloc(E->polycount * sizeof(int));
        int j;
        for(j = 0; j < E->polycount; j++)
        {
            E->polys[j] = e_polys[c_e++ % P_LEN];
            //E->slots[j] = 0;
        }
        E->uv_verts[0] = Idx_V[c_v++ % LEN_2];
        E->uv_verts[1] = Idx_V[c_v++ % LEN_2];
        E->uv_edges[0] = -1;
        E->uv_edges[1] = -1;
        //init_uv_edge(i, O, &O->uveds[i / ARRAYSIZE][i % ARRAYSIZE], &E);

        uv * V0 = &O->uvtex[E->uv_verts[0] / ARRAYSIZE][E->uv_verts[0] % ARRAYSIZE];
        uv * V1 = &O->uvtex[E->uv_verts[1] / ARRAYSIZE][E->uv_verts[1] % ARRAYSIZE];
        boxt_0(&E->B, V0, V1, 1);
        E->split = 0;
    }
}

void init_object_colors(object * O, int start)
{
    int i, idx, idx0, idx1, idx2;

    id_color I;

    int M0 = 255 * 255 * 255;
    int M1 = 255 * 255;

    for(i = start; i < O->icolcount; i ++)
    {
        idx = i;
        I.R = (float)(idx / M0) / (float)255;
        idx0 = idx % M0;
        I.G = (float)(idx0 / M1) / (float)255;
        idx1 = idx0 % M1;
        I.B = (float)(idx1 / 255) / (float)255;
        idx2 = idx1 % 255;
        I.A = (float)(idx2) / (float)255;
        init_color(i, &O->icols[i / ARRAYSIZE][i % ARRAYSIZE], &I);
    }
}

void init_object_polys(object * O, int start, int * p_trips, int T_LEN, int * p_edges,
                       int * p_verts, int * p_texts, int E_LEN, int * p_uveds, int * E_tiks, int LEN)
{
    int i;
    int c_E = 0;
    int c_t = 0;
    int c_e = 0;
    int c_v = 0;
    int c_u = 0;
    int c_d = 0;
    for(i = start; i < O->polycount; i ++)
    {
        polygon * P = &O->polys[i / ARRAYSIZE][i % ARRAYSIZE];
        P->index = i;
        P->subdivs = 0;
        P->visible = 1;
        P->surface = 0;
        P->selected = 0;
        P->color = i;
        P->group = 0;
        P->edgecount = E_tiks[c_E++ % LEN];
        P->tripcount = P->edgecount - 2;
        P->trips = calloc(P->tripcount, sizeof(int));
        P->quads = calloc(P->edgecount, sizeof(int));
        P->edges = calloc(P->edgecount, sizeof(int));
        P->verts = calloc(P->edgecount, sizeof(int));
        P->texts = calloc(P->edgecount, sizeof(int));
        P->uveds = calloc(P->edgecount, sizeof(int));
        //P->uvdir = calloc(P->edgecount, sizeof(int));
        int j;
        for(j = 0; j < P->tripcount; j++)
        {
            P->trips[j] = p_trips[c_t++ % T_LEN];
        }
        for(j = 0; j < P->edgecount; j++)
        {
            P->edges[j] = p_edges[c_e++ % E_LEN];
            P->verts[j] = p_verts[c_v++ % E_LEN];
            P->texts[j] = p_texts[c_u++ % E_LEN];
            P->uveds[j] = p_uveds[c_d++ % E_LEN];
        }
        //init_polygon(i, &O->polys[i / ARRAYSIZE][i % ARRAYSIZE], &P);
    }
}

#endif // INITIALIZINGFUNCTIONS_H_INCLUDED
