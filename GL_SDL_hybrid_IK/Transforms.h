/*
The MIT License

Copyright <2018> <Allan Kiipli>
*/

#ifndef TRANSFORMS_H_INCLUDED
#define TRANSFORMS_H_INCLUDED

// Develop vertex groups and handling of deformation subdivision.
// Make bones reality.

object * Transformer_Objects[OBJECTS];
int Transformer_Objects_Count = 0;

object * Update_Objects[OBJECTS];
int Update_Objects_Count = 0;

void rotate_Camera(camera * C, float CamDist)
{
    rotate_T(C->T);
    move_Back(C->T, CamDist);
//    glMatrixMode(GL_PROJECTION);
//    glLoadIdentity();
//    gluPerspective(C->view * (180.0 / pi), C->h_v_ratio, 1.0, 1024.0 );
//    gluLookAt(C->T.pos[0], C->T.pos[1], C->T.pos[2], 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
}

void generate_Polygroups(camera * C)
{
    int o, p, e, idx; //, v, e0;
    object * O;
    edge * E; // * E0;
    polygon * P, * P0, * P1;
    //vertex * V;

    for (o = 0; o < C->object_count; o ++)
    {
        O = objects[C->objects[o]];

        if (O->group)
        {
            continue;
        }

        for (e = 0; e < O->edgecount; e ++)
        {
            E = &O->edges[e / ARRAYSIZE][e % ARRAYSIZE];
            E->group_Polys.assigned = -1;
            E->group_Polys.indices_count = 0;
        }

        for (e = 1; e < O->edgecount; e ++)
        {
            E = &O->edges[e / ARRAYSIZE][e % ARRAYSIZE];

            if (E->polycount > 1)
            {
                idx = E->polys[0];
                P0 = &O->polys[idx / ARRAYSIZE][idx % ARRAYSIZE];

                idx = E->polys[1];
                P1 = &O->polys[idx / ARRAYSIZE][idx % ARRAYSIZE];

                if (P0->group == 0 && P1->group == 0)
                {
                    E->group_Polys.indices = malloc(2 * sizeof(int));

                    if (E->group_Polys.indices != NULL)
                    {
                        E->group_Polys.index = e;
                        E->group_Polys.indices_count = 2;

                        E->group_Polys.indices[0] = E->polys[0];
                        E->group_Polys.indices[1] = E->polys[1];

                        E->group_Polys.assigned = 0;

                        P0->group = e;
                        P1->group = e;
                    }
                }
            }
        }
/*
        for (e = 1; e < O->edgecount; e ++)
        {
            E = &O->edges[e / ARRAYSIZE][e % ARRAYSIZE];

            if (E->group_Polys.assigned < 1 && E->group_Polys.indices_count == 2)
            {
                for (v = 0; v < 2; v ++)
                {
                    idx = E->verts[v];
                    V = &O->verts[idx / ARRAYSIZE][idx % ARRAYSIZE];

                    for (e0 = 0; e0 < V->edgecount; e0 ++)
                    {
                        idx = V->edges[e0];
                        E0 = &O->edges[idx / ARRAYSIZE][idx % ARRAYSIZE];

                        if (E == E0)
                        {
                            continue;
                        }

                        if (E0->group_Polys.assigned == -1 && E0->group_Polys.indices_count == 2)
                        {
                            E->group_Polys.indices = realloc(E->group_Polys.indices, (E->group_Polys.indices_count + E0->group_Polys.indices_count) * sizeof(int));

                            if (E->group_Polys.indices != NULL)
                            {
                                E->group_Polys.indices[E->group_Polys.indices_count] = E0->polys[0];
                                E->group_Polys.indices[E->group_Polys.indices_count + 1] = E0->polys[1];

                                E->group_Polys.indices_count += E0->group_Polys.indices_count;

                                E->group_Polys.assigned = 0;
                                E0->group_Polys.assigned = 1;
                            }
                        }
                    }
                }
            }
        }
*/
        if (O->edgecount > 0)
        {
            int i, * p_indices, p_indices_count;

            p_indices = malloc(O->polycount * sizeof(int));

            p_indices_count = 0;

            if (p_indices != NULL)
            {
                for (p = 0; p < O->polycount; p ++)
                {
                    P = &O->polys[p / ARRAYSIZE][p % ARRAYSIZE];

                    if (P->group == 0)
                    {
                        p_indices[p_indices_count ++] = P->index;
                    }
                }

                if (p_indices_count > 0)
                {
                    E = &O->edges[0 / ARRAYSIZE][0 % ARRAYSIZE];

                    E->group_Polys.indices = malloc(p_indices_count * sizeof(int));

                    if (E->group_Polys.indices != NULL)
                    {
                        E->group_Polys.assigned = 0;
                        E->group_Polys.index = 0;
                        E->group_Polys.indices_count = p_indices_count;

                        for (i = 0; i < p_indices_count; i ++)
                        {
                            E->group_Polys.indices[i] = p_indices[i];
                        }
                    }
                }
            }

            free(p_indices);
        }

        int check_0 = 0;
        int check_1 = 0;

        for (e = 0; e < O->edgecount; e ++)
        {
            E = &O->edges[e / ARRAYSIZE][e % ARRAYSIZE];

            if(E->group_Polys.assigned == 0)
            {
                sum_box_3d_For_Polygroup_T(O, E, 1);

                //printf("%d ", E->group_Polys.indices_count);

                check_0 += E->group_Polys.indices_count;
                check_1 ++;
            }
        }

        O->group = 1;

        printf("%s, %d, %d, %d\n", O->Name, O->polycount, check_0, check_1);
    }
}

void update_transformed_Triangles_radius(camera * C, int L)
{
    int o, level;
    object * O;

    for (o = 0; o < C->object_count; o ++)
    {
        O = objects[C->objects[o]];
        update_bounding_box_for_transformed_Trips(O, 1); // does update radius

        for (level = L; level <= O->subdlevel; level ++)
        {
            if (level > L)
            {
                break;
            }

            update_bounding_box_for_transformed_Trips_(O, level, 1); // does update radius
        }
    }
}

void update_transformed_Objects(int obj_count)
{
    int o;
    object * O;

    for (o = objectIndex - obj_count; o < objectIndex; o ++)
    {
        O = objects[o];
        //rotate_T(&O->T);
        update_bounding_box_for_transformed_Trips(O, 0); // does not update radius
        update_bounding_box_for_transformed_Polys(O, 1);
        update_bounding_box_for_transformed_Edges(O, 1);
        update_transformed_Triangles(O);
        generate_transformed_polyNormals(O);
        generate_transformed_edgeNormals(O);

        if (SHADERS)
        {
            calculate_Triangle_Tangents(O);
            average_Polygon_Tangents(O);
            average_Edge_Tangents(O);
            average_Vert_Tangents(O);
        }
    }
}

void update_tangents_Object(object * O, int L)
{
    if (SHADERS)
    {
        if (L <= O->subdlevel && L > -1)
        {
            calculate_Triangle_Tangents_(O, L);
            average_Quadrant_Tangents_(O, L);
            average_Edge_Tangents_(O, L);
            average_Vert_Tangents_(O, L);
        }
        else
        {
            calculate_Triangle_Tangents(O);
            average_Polygon_Tangents(O);
            average_Edge_Tangents(O);
            average_Vert_Tangents(O);
        }
    }
}

void update_tangents(int L)
{
    int o;
    object * O;

    for (o = 0; o < objectIndex ; o ++)
    {
        O = objects[o];
        if (SHADERS)
        {
            if (L <= O->subdlevel && L > -1)
            {
                calculate_Triangle_Tangents_(O, L);
                average_Quadrant_Tangents_(O, L);
                average_Edge_Tangents_(O, L);
                average_Vert_Tangents_(O, L);
            }
            else
            {
                calculate_Triangle_Tangents(O);
                average_Polygon_Tangents(O);
                average_Edge_Tangents(O);
                average_Vert_Tangents(O);
            }
        }
    }
}

void update_rotate_bounding_box()
{
    int o;
    object * O;

    for (o = 0; o < Update_Objects_Count; o ++)
    {
        O = Update_Objects[o];
        update_bounding_box_for_transformed_Trips(O, 0); // does not update radius
        update_bounding_box_for_transformed_Polys(O, 1);
        update_bounding_box_for_transformed_Edges(O, 1);
        update_transformed_Triangles(O);
        generate_transformed_polyNormals(O);
        generate_transformed_edgeNormals(O);
        generate_transformed_vertexNormals(O);
    }
}

void update_move_bounding_box()
{
    int o;
    object * O;

    for (o = 0; o < Update_Objects_Count; o ++)
    {
        O = Update_Objects[o];
        update_bounding_box_for_transformed_Trips(O, 0); // does not update radius
        update_transformed_Triangles(O);
        update_transformed_bounding_box_for_Trip_Polys(O);
        update_bounding_box_for_transformed_Edges(O, 0);
    }
}

void move_vertex_groups_T(transformer * T)
{
    int idx, i, s;
    object * O;
    vert_selection * S;
    vertex * V;

    float x, y, z;

    x = T->pos[0] - T->pos_bind[0];
    y = T->pos[1] - T->pos_bind[1];
    z = T->pos[2] - T->pos_bind[2];

    for (s = 0; s < T->Selections_Count; s ++)
    {
        S = T->Selections[s];
        O = S->Object;

        for (i = 0; i < S->indices_count; i ++)
        {
            idx = S->indices[i];
            V = &O->verts[idx / ARRAYSIZE][idx % ARRAYSIZE];
            V->Tx = V->x + x;
            V->Ty = V->y + y;
            V->Tz = V->z + z;
        }
    }
}

void move_vertex_groups(transformer * T)
{
    int idx, i, s;
    object * O;
    vert_selection * S;
    vertex * V;

    float x, y, z;

    x = T->pos[0] - T->pos_bind[0];
    y = T->pos[1] - T->pos_bind[1];
    z = T->pos[2] - T->pos_bind[2];

    for (s = 0; s < T->Selections_Count; s ++)
    {
        S = T->Selections[s];
        O = S->Object;

        for (i = 0; i < S->indices_count; i ++)
        {
            idx = S->indices[i];
            V = &O->verts[idx / ARRAYSIZE][idx % ARRAYSIZE];
            V->Tx += (V->x + x) * S->weights[i];
            V->Ty += (V->y + y) * S->weights[i];
            V->Tz += (V->z + z) * S->weights[i];
        }
    }
}

void move_verts(object * O, float x, float y, float z)
{
    O->T->pos[0] = x;
    O->T->pos[1] = y;
    O->T->pos[2] = z;

    O->B.Tx = O->B.x + x;
    O->B.Ty = O->B.y + y;
    O->B.Tz = O->B.z + z;

    vertex * V;
    int i;

    for (i = 0; i < O->vertcount; i++)
    {
        V = &O->verts[i / ARRAYSIZE][i % ARRAYSIZE];
        V->Tx = V->x + x;
        V->Ty = V->y + y;
        V->Tz = V->z + z;
    }
    update_bounding_box_for_transformed_Trips(O, 0); // does not update radius
    update_transformed_Triangles(O);
    update_transformed_bounding_box_for_Trip_Polys(O);
    update_bounding_box_for_transformed_Edges(O, 0);
}

void move_verts_(object * O, float x, float y, float z, int l)
{
    O->T->pos[0] = x;
    O->T->pos[1] = y;
    O->T->pos[2] = z;

    O->B.Tx = O->B.x + x;
    O->B.Ty = O->B.y + y;
    O->B.Tz = O->B.z + z;

    vertex * V;
    int i, L;
    if (l > O->subdlevel)
    {
        L = O->subdlevel;
    }
    else
    {
        L = l;
    }
    for (i = 0; i < O->vertcount_[L]; i++)
    {
        V = &O->verts_[L][i / ARRAYSIZE][i % ARRAYSIZE];
        V->Tx = V->x + x;
        V->Ty = V->y + y;
        V->Tz = V->z + z;
    }
    update_bounding_box_for_transformed_Trips_(O, L, 0); // does not update radius
    update_transformed_bounding_box_for_Trip_Quads_(O, L);
    update_bounding_box_for_transformed_Edges_(O, L, 0);
}

void rotate_vertex_groups_D_Init()
{
    int o, v;
    object * O;
    vertex * V;

    for (o = 0; o < Update_Objects_Count; o ++)
    {
        O = Update_Objects[o];

        if (O->deforms)
        {
            for (v = 0; v < O->vertcount; v ++)
            {
                V = &O->verts[v / ARRAYSIZE][v % ARRAYSIZE];

                V->Tx = 0;
                V->Ty = 0;
                V->Tz = 0;

//                V->N.Tx = 0;
//                V->N.Ty = 0;
//                V->N.Tz = 0;
            }
        }
    }
}

void remove_Selections_From_Transformers(object * O)
{
    int s, t;

    transformer * T;
    vert_selection * S;

    for(t = 0; t < transformerIndex; t ++)
    {
        T = transformers[t];
        for (s = 0; s < O->vertex_selections; s ++)
        {
            S = O->vertex_selection[s];
            remove_Selection_From_Transformer(O, T, S->Name);
        }
    }
}

void rotate_collect_Deformers(deformer * D)
{
    int o, condition;
    object * O;

    for (o = 0; o < D->Objects_Count; o ++)
    {
        O = D->Objects[o];

        if (O->deforms)
        {
            condition = 1;
            for (o = 0; o < Update_Objects_Count; o ++)
            {
                if (Update_Objects[o] == O)
                {
                    condition = 0;
                    break;
                }
            }
            if (condition)
            {
                Update_Objects[Update_Objects_Count ++] = O;
            }
        }
    }
}

void rotate_vertex_groups_collect_Objects(transformer * T)
{
    int o, s, condition;
    object * O;
    vert_selection * S;

    if (T->Bone != NULL && T == T->Bone->B)
    {

    }
    else
    {
        for (s = 0; s < T->Selections_Count; s ++)
        {
            S = T->Selections[s];

            //printf("S->Name %s\n", S->Name);

            //if (S->Name == NULL) continue;

            O = S->Object;

            if (O->deforms)
            {
                condition = 1;
                for (o = 0; o < Update_Objects_Count; o ++)
                {
                    if (Update_Objects[o] == O)
                    {
                        condition = 0;
                        break;
                    }
                }
                if (condition)
                {
                    Update_Objects[Update_Objects_Count ++] = O;
                }
            }
        }
    }
}

void rotate_vertex_groups(transformer * T)
{
    int idx, i, s;
    object * O;
    vert_selection * S;
    vertex * V;

    float X, Y, Z;

    float rotVec[3][3];
//    float rotVec_[3][3];

    rotate_matrix_I(rotVec, T->rotVec, T->rotVec_B);
//    rotate_matrix_I(rotVec_, T->rotVec_, T->rotVec_B);

    for (s = 0; s < T->Selections_Count; s ++)
    {
        S = T->Selections[s];

        //if (S->Name == NULL) continue;

        O = S->Object;

        //printf("SELECTION %s OBJECT %s\n", S->Name, O->Name);

        if (O->deforms)
        {
            for (i = 0; i < S->indices_count; i ++)
            {
                idx = S->indices[i];
                V = &O->verts[idx / ARRAYSIZE][idx % ARRAYSIZE];

                X = V->Rx - T->pos_bind[0];
                Y = V->Ry - T->pos_bind[1];
                Z = V->Rz - T->pos_bind[2];

                V->Tx += (rotVec[0][0] * X + rotVec[1][0] * Y + rotVec[2][0] * Z + T->pos[0]) * S->weights[i];
                V->Ty += (rotVec[0][1] * X + rotVec[1][1] * Y + rotVec[2][1] * Z + T->pos[1]) * S->weights[i];
                V->Tz += (rotVec[0][2] * X + rotVec[1][2] * Y + rotVec[2][2] * Z + T->pos[2]) * S->weights[i];

//                V->N.Tx += (rotVec_[0][0] * V->N.x + rotVec_[1][0] * V->N.y + rotVec_[2][0] * V->N.z) * S->weights[i];
//                V->N.Ty += (rotVec_[0][1] * V->N.x + rotVec_[1][1] * V->N.y + rotVec_[2][1] * V->N.z) * S->weights[i];
//                V->N.Tz += (rotVec_[0][2] * V->N.x + rotVec_[1][2] * V->N.y + rotVec_[2][2] * V->N.z) * S->weights[i];

            }
        }
    }
}

void rotate_verts(object * O, transformer T)
{
    vertex * V;
    int i;
    for (i = 0; i < O->vertcount; i++)
    {
        V = &O->verts[i / ARRAYSIZE][i % ARRAYSIZE];
        V->Tx = T.rotVec[0][0] * V->Rx + T.rotVec[1][0] * V->Ry + T.rotVec[2][0] * V->Rz + T.pos[0];
        V->Ty = T.rotVec[0][1] * V->Rx + T.rotVec[1][1] * V->Ry + T.rotVec[2][1] * V->Rz + T.pos[1];
        V->Tz = T.rotVec[0][2] * V->Rx + T.rotVec[1][2] * V->Ry + T.rotVec[2][2] * V->Rz + T.pos[2];

        V->N.Tx = T.rotVec_[0][0] * V->N.x + T.rotVec_[1][0] * V->N.y + T.rotVec_[2][0] * V->N.z;
        V->N.Ty = T.rotVec_[0][1] * V->N.x + T.rotVec_[1][1] * V->N.y + T.rotVec_[2][1] * V->N.z;
        V->N.Tz = T.rotVec_[0][2] * V->N.x + T.rotVec_[1][2] * V->N.y + T.rotVec_[2][2] * V->N.z;
    }

    update_bounding_box_for_transformed_Trips(O, 0); // does not update radius
    update_bounding_box_for_transformed_Polys(O, 1);
    update_bounding_box_for_transformed_Edges(O, 1);
    update_transformed_Triangles(O);
    generate_transformed_polyNormals(O);
    generate_transformed_edgeNormals(O);

    if (SHADERS)
    {
        calculate_Triangle_Tangents(O);
        average_Polygon_Tangents(O);
        average_Edge_Tangents(O);
        average_Vert_Tangents(O);
    }
}

void rotate_verts_(object * O, transformer T, int l)
{
    vertex * V;
    int i, L;
    if (l > O->subdlevel)
    {
        L = O->subdlevel;
    }
    else
    {
        L = l;
    }
    for (i = 0; i < O->vertcount_[L]; i++)
    {
        V = &O->verts_[L][i / ARRAYSIZE][i % ARRAYSIZE];
        V->Tx = T.rotVec[0][0] * V->x + T.rotVec[1][0] * V->y + T.rotVec[2][0] * V->z + T.pos[0];
        V->Ty = T.rotVec[0][1] * V->x + T.rotVec[1][1] * V->y + T.rotVec[2][1] * V->z + T.pos[1];
        V->Tz = T.rotVec[0][2] * V->x + T.rotVec[1][2] * V->y + T.rotVec[2][2] * V->z + T.pos[2];

        V->N.Tx = T.rotVec_[0][0] * V->N.x + T.rotVec_[1][0] * V->N.y + T.rotVec_[2][0] * V->N.z;
        V->N.Ty = T.rotVec_[0][1] * V->N.x + T.rotVec_[1][1] * V->N.y + T.rotVec_[2][1] * V->N.z;
        V->N.Tz = T.rotVec_[0][2] * V->N.x + T.rotVec_[1][2] * V->N.y + T.rotVec_[2][2] * V->N.z;
    }

    update_bounding_box_for_transformed_Trips_(O, L, 0); // does not update radius
    update_bounding_box_for_transformed_Quads(O, L, 1);
    update_bounding_box_for_transformed_Edges_(O, L, 1);
    update_transformed_Triangles_level_(O, L); // updates also quad normals
    generate_transformed_edgeNormals_(O, L);

    if (SHADERS)
    {
        calculate_Triangle_Tangents_(O, L);
        average_Quadrant_Tangents_(O, L);
        average_Edge_Tangents_(O, L);
        average_Vert_Tangents_(O, L);
    }
}

#endif // TRANSFORMS_H_INCLUDED
