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

void snap_back_Camera_Target(transformer * T, camera * C)
{
    T->pos[0] = T->Pos_[0];
    T->pos[1] = T->Pos_[1];
    T->pos[2] = T->Pos_[2];

    C->T->target[0] = T->pos[0];
    C->T->target[1] = T->pos[1];
    C->T->target[2] = T->pos[2];
}

void move_Camera_Target(transformer * T, camera * C)
{
    C->T->target[0] = T->pos[0];
    C->T->target[1] = T->pos[1];
    C->T->target[2] = T->pos[2];
}

void rotate_Camera_Aim(camera * C)
{
    transformer * T = C->T;
    direction_Pack D;

    D = length_AB(T->pos, T->target);

    //memcpy(T->rotVec_, Identity_, sizeof(float[3][3]));

    cross_Product((float[3]){0.0, 1.0, 0.0}, D.vec, T->rotVec_[0]);

    normalizeF(T->rotVec_[0]);

    cross_Product(D.vec, T->rotVec_[0], T->rotVec_[1]);
    //normalizeF(T->rotVec_[1]);
    memcpy(T->rotVec_[2], D.vec, sizeof(float[3]));

    rotate_axis(T->rot[2], T->rotVec_[0], T->rotVec_[1], T->rotVec_[0], T->rotVec_[1]);
    memcpy(T->rotVec, T->rotVec_, sizeof(float[3][3]));
//    memcpy(T->aim, T->rotVec_[2], sizeof(float[3]));
}

void rotate_Camera(camera * C, float CamDist)
{
    rotate_T(C->T);
    move_Back(C->T, CamDist);
//    glMatrixMode(GL_PROJECTION);
//    glLoadIdentity();
//    gluPerspective(C->view * (180.0 / pi), C->h_v_ratio, 1.0, 1024.0 );
//    gluLookAt(C->T.pos[0], C->T.pos[1], C->T.pos[2], 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
}

void rotate_Cameras(float dist)
{
    int c;

    camera * C;

    for (c = 0; c < 4; c ++)
    {
        C = cameras[c];
        rotate_Camera(C, dist);
    }
}

union Dir
{
   direction D;
   normal N;
};

int place_Polygon_Into_HexG(camera * C, object * O, polygon * P, HexG * H, HexG * Super, int result, int * cancel)
{
    HexG * H_0;
    int h;
    float deviation;
    polyPack * PP;

    deviation = acos(dot_productN((normal *)&H->D, P->B.Aim.vec));

    if (H->Radius > P->B.deviation && deviation < H->Radius - P->B.deviation)
    // (H->Radius > P->B.deviation && deviation < H->Radius)
    // (H->Radius > P->B.deviation && deviation < H->Radius + P->B.deviation)
    // (H->Radius > P->B.deviation && deviation < H->Radius - P->B.deviation) // is inside hexagon
    // (H->Radius > P->B.deviation && deviation < H->Radius - P->B.deviation * 2)

    // Polygon must be smaller than hexagon to fit in and
    // more than half intersects // it intersects hexagon // is inside hexagon // it intersects more
    {
        if (H->subdivided) // && H->Radius * 0.5 > P->B.deviation) // && H->Level == 2
        {
            for (h = 0; h < 7; h ++)
            {
                H_0 = H->subH[h];
                result = place_Polygon_Into_HexG(C, O, P, H_0, H, result, cancel);
            }
        }

        if (result == 0 && cancel[0] == 0)
        {
            if (H->Polygons != NULL && (H->polypacks + (POLYCHUNK / 2)) > (sizeof(H->Polygons) / sizeof(polyPack)))
            {
                H->Polygons = realloc(H->Polygons, sizeof(polyPack) * (H->polypacks + POLYCHUNK));
            }
            if (H->Polygons != NULL)
            {
                PP = &H->Polygons[H->polypacks];
                PP->O = O;
                PP->idx = P->index;
                H->polypacks ++;
            }
            else
            {
                H->polypacks = 0;
            }

            cancel[0] = 1;
        }
    }

    if (cancel[0])
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

void generate_Hexa_Groups(camera * C)
{
    int result, * cancel;
    int h, o, p;
    int Cancel;
    cancel = &Cancel;

    object * O;
    polygon * P;

    HexG * H;

    float R;

    union Dir D = {{0.0, 0.0, -1.0}};

    float H_Mark;
    float V_Mark;

    for (h = 0; h < hexaIndex; h ++)
    {
        H = Hexas[h];
        H->polypacks = 0;

        H_Mark = H->Center[0];
        V_Mark = H->Center[1];

        R = cos(V_Mark);
        D.D.y = sin(V_Mark);
        D.D.x = sin(H_Mark) * R;
        D.D.z = cos(H_Mark) * R;

        rotate_Vector(C->T, -D.D.x, D.D.y, D.D.z, &H->D);
    }

    for (o = 0; o < C->object_count; o ++)
    {
        O = objects[C->objects[o]];

        printf("%s\n", O->Name);

        for (p = 0; p < O->polycount; p ++)
        {
            P = &O->polys[p / ARRAYSIZE][p % ARRAYSIZE];

            if (P->B.backface)
            {
                continue;
            }

            for (h = 6; h < 7; h ++)
            {
                H = Hex_Group[h];
                result = 0;
                Cancel = 0;
                result = place_Polygon_Into_HexG(C, O, P, H, H, result, cancel);
            }
        }
    }
}

int place_Polygon_Into_HexG_light(camera * C, object * O, polygon * P, HexG * H, HexG * Super, int result, int * cancel)
{
    HexG * H_0;
    int h;
    float deviation;
    polyPack * PP;

    deviation = acos(dot_productN((normal *)&H->D_light0, P->B_light.Aim.vec));

    if (H->Radius > P->B_light.deviation && deviation < H->Radius - P->B_light.deviation)
    {
        if (H->subdivided)
        {
            for (h = 0; h < 7; h ++)
            {
                H_0 = H->subH[h];
                result = place_Polygon_Into_HexG_light(C, O, P, H_0, H, result, cancel);
            }
        }

        if (result == 0 && cancel[0] == 0)
        {
            if (H->Lights != NULL && (H->lightpacks + (POLYCHUNK / 2)) > (sizeof(H->Lights) / sizeof(polyPack)))
            {
                H->Lights = realloc(H->Lights, sizeof(polyPack) * (H->lightpacks + POLYCHUNK));
            }

            if (H->Lights != NULL)
            {
                PP = &H->Lights[H->lightpacks];
                PP->O = O;
                PP->idx = P->index;
                H->lightpacks ++;
            }
            else
            {
                H->lightpacks = 0;
            }

            cancel[0] = 1;
        }
    }

    if (cancel[0])
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

void generate_Hexa_Groups_light(camera * C)
{
    int result, * cancel;
    int h, o, p;
    int Cancel;
    cancel = &Cancel;

    object * O;
    polygon * P;

    HexG * H;

    float R;

    union Dir D = {{0.0, 0.0, -1.0}};

    float H_Mark;
    float V_Mark;

    for (h = 0; h < hexaIndex; h ++)
    {
        H = Hexas[h];
        H->lightpacks = 0;

        H_Mark = H->Center[0];
        V_Mark = H->Center[1];

        R = cos(V_Mark);
        D.D.y = sin(V_Mark);
        D.D.x = sin(H_Mark) * R;
        D.D.z = cos(H_Mark) * R;

        rotate_Vector(C->T, -D.D.x, D.D.y, D.D.z, &H->D_light0);
    }

    for (o = 0; o < C->object_count; o ++)
    {
        O = objects[C->objects[o]];

        for (p = 0; p < O->polycount; p ++)
        {
            P = &O->polys[p / ARRAYSIZE][p % ARRAYSIZE];

//            if (P->B.backface)
//            {
//                continue;
//            }

            for (h = 6; h < 7; h ++)
            {
                H = Hex_Group[h];
                result = 0;
                Cancel = 0;
                result = place_Polygon_Into_HexG_light(C, O, P, H, H, result, cancel);
            }
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

void move_Objects_To_Delta(float Delta[3])
{
    int o, v;

    object * O;
    vertex * V;

    for (o = 0; o < Update_Objects_Count; o ++)
    {
        O = Update_Objects[o];
        for (v = 0; v < O->vertcount; v ++)
        {
            V = &O->verts[v / ARRAYSIZE][v % ARRAYSIZE];
            V->Rx = V->x + Delta[0];
            V->Ry = V->y + Delta[1];
            V->Rz = V->z + Delta[2];
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
    if (L == -1)
    {

    }
    else
    {
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
}

void rotate_vertex_groups_Def_Init(deformer * D)
{
    int o, v;
    object * O;
    vertex * V;

    for (o = 0; o < D->Objects_Count; o ++)
    {
        O = D->Objects[o];

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

void collect_Transformer_Objects(transformer * T)
{
    int c;

    transformer * C;

    if (T->Object != NULL)
    {
        Update_Objects[Update_Objects_Count ++] = T->Object;
    }

    for (c = 0; c < T->childcount; c ++)
    {
        C = T->childs[c];
        collect_Transformer_Objects(C);
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

void rotate_vertex_groups_I_selective_Move(transformer * T, float Delta[3])
{
    int v_idx, idx, i, s;
    object * O;
    vert_selection * S;
    vertex * V;

    float rotVec_I[3][3];
    float rotVec[3][3];
    float pos[3];
    float Pos[3];

    float Delta_result[3];

    invert_Rotation_1(rotVec_I, T->rotVec_B);

//                if (T->IK != NULL)
//                    generate_scl_vec(T);

    invert_Rotation_scale(T, rotVec);
    rotate_matrix_I(rotVec, rotVec_I, rotVec);

    rotate_vector(rotVec, Delta, Delta_result);

    for (s = 0; s < T->Selections_Count; s ++)
    {
        S = T->Selections[s];

        O = S->Object;

        if (O->deforms && O->selected && O->Movement_Enabled)
        {
            for (i = 0; i < S->indices_count; i ++)
            {
                idx = S->indices[i];
                V = &O->verts[idx / ARRAYSIZE][idx % ARRAYSIZE];

                if (V->selected)
                {
                    v_idx = V->selected - 1; // avoid zero

                    if (v_idx < O->selected_verts_count)
                    {
                        Pos[0] = O->vertex_Positions[v_idx].Pos[0] - T->pos[0];
                        Pos[1] = O->vertex_Positions[v_idx].Pos[1] - T->pos[1];
                        Pos[2] = O->vertex_Positions[v_idx].Pos[2] - T->pos[2];

                        rotate_vector(rotVec, Pos, pos);

                        V->Rx += (pos[0] + T->pos_bind[0] + Delta_result[0]) * S->weights[i];
                        V->Ry += (pos[1] + T->pos_bind[1] + Delta_result[1]) * S->weights[i];
                        V->Rz += (pos[2] + T->pos_bind[2] + Delta_result[2]) * S->weights[i];
                    }
                }
            }
        }
    }
}

void rotate_vertex_groups_I_selective(transformer * T, int Alt_down)
{
    int v_idx, idx, i, s;
    object * O;
    vert_selection * S;
    vertex * V;

    float rotVec_I[3][3];
    float rotVec[3][3];
    float result[3];
    float pos[3];
    float Pos[3];
    float Delta_length = 0.0;
    float Aim_vec[3];

    invert_Rotation_1(rotVec_I, T->rotVec_B);

//                if (T->IK != NULL)
//                    generate_scl_vec(T);

    invert_Rotation_scale(T, rotVec);
    rotate_matrix_I(rotVec, rotVec_I, rotVec);

    if (Alt_down)
    {
        Delta_length = Action_Center->scl_vec[0] - 1;
    }

    for (s = 0; s < T->Selections_Count; s ++)
    {
        S = T->Selections[s];

        O = S->Object;

        if (O->deforms && O->selected && O->Movement_Enabled)
        {
            for (i = 0; i < S->indices_count; i ++)
            {
                idx = S->indices[i];
                V = &O->verts[idx / ARRAYSIZE][idx % ARRAYSIZE];

                if (V->selected)
                {
                    v_idx = V->selected - 1; // avoid zero

                    if (v_idx < O->selected_verts_count)
                    {
                        if (Alt_down)
                        {
                            Pos[0] = O->vertex_Positions[v_idx].Pos[0] - T->pos[0];
                            Pos[1] = O->vertex_Positions[v_idx].Pos[1] - T->pos[1];
                            Pos[2] = O->vertex_Positions[v_idx].Pos[2] - T->pos[2];

                            rotate_vector(rotVec, Pos, pos);

                            rotate_vector(rotVec, V->aim_vec, Aim_vec);

                            V->Rx += (pos[0] + T->pos_bind[0] + (Delta_length * Aim_vec[0])) * S->weights[i];
                            V->Ry += (pos[1] + T->pos_bind[1] + (Delta_length * Aim_vec[1])) * S->weights[i];
                            V->Rz += (pos[2] + T->pos_bind[2] + (Delta_length * Aim_vec[2])) * S->weights[i];
                        }
                        else
                        {
                            rotate_center(O->vertex_Positions[v_idx].Pos, Action_Center->rotVec, Action_Center->pos, result);

                            Pos[0] = result[0] - T->pos[0];
                            Pos[1] = result[1] - T->pos[1];
                            Pos[2] = result[2] - T->pos[2];

                            rotate_vector(rotVec, Pos, pos);

                            V->Rx += (pos[0] + T->pos_bind[0]) * S->weights[i];
                            V->Ry += (pos[1] + T->pos_bind[1]) * S->weights[i];
                            V->Rz += (pos[2] + T->pos_bind[2]) * S->weights[i];
                        }
                    }
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
    if (L == -1)
    {

    }
    else
    {
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
}

void remember_Objects_With_Curves()
{
    int o;

    object * O;

    for (o = 0; o < Update_Objects_Count; o ++)
    {
        O = Update_Objects[o];
        if (O->curve_count > 0)
        {
            update_Curve_Objects = 1;
            remember_Object_Curves_pos(O);
        }
    }
}

void update_Objects_With_Curves(float Delta[3], int subdLevel)
{
    int o;

    object * O;

    for (o = 0; o < Update_Objects_Count; o ++)
    {
        O = Update_Objects[o];
        transfer_Delta_To_Object_Cps(O, Delta);
        update_object_Curves(O, subdLevel);
    }
}

void rotate_update_Objects()
{
    int o;

    object * O;

    for (o = 0; o < Update_Objects_Count; o ++)
    {
        O = Update_Objects[o];
        rotate_verts(O, *O->T);
    }
}

void update_ROT_Objects_With_Curves(int subdLevel)
{
    int o;

    object * O;

    for (o = 0; o < Update_Objects_Count; o ++)
    {
        O = Update_Objects[o];
        update_Object_Curves_Cps_Positions(O);
        update_object_Curves(O, subdLevel);
        update_object_Curves(O, subdLevel);
    }
}

void snap_back_Curve_Objects(int subdLevel)
{
    int o;

    object * O;

    for (o = 0; o < Update_Objects_Count; o ++)
    {
        O = Update_Objects[o];
        snap_back_Object_Cps_To_Pos(O);
        update_object_Curves(O, subdLevel);
        update_object_Curves(O, subdLevel);
    }
}

void move_Cameras()
{
    int c;

    camera * C;

    for (c = CAMERAS; c < camIndex; c ++)
    {
        C = cameras[c];
        if (C->T->Constraint != NULL)
            move_Camera_Target(C->T->Constraint->Locator, C);

//        direction_Pack D = length_AB(C->T->pos, C->T->target);
//        rotate_Camera(C, D.distance);

        rotate_Camera_Aim(C);
    }
}

#endif // TRANSFORMS_H_INCLUDED
