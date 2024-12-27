/*
The MIT License

Copyright <2018> <Allan Kiipli>
*/

#ifndef MODELING_H_INCLUDED
#define MODELING_H_INCLUDED

int MODELING_MODE = 0;
int vertex_Manipulation = 0;

void transfer_Transformed_Coordinates(object * O)
{
    int v;
    vertex * V;

    for (v = 0; v < O->vertcount; v ++)
    {
        V = &O->verts[v / ARRAYSIZE][v % ARRAYSIZE];

        V->x = V->Tx;
        V->y = V->Ty;
        V->z = V->Tz;

        V->Rx = V->Tx; // morph coordinates
        V->Ry = V->Ty;
        V->Rz = V->Tz;

        V->N.x = V->N.Tx;
        V->N.y = V->N.Ty;
        V->N.z = V->N.Tz;
    }
}

void convert_To_Cp_Vert_Selection(curve * C)
{
    int p;

    cp * CP;

    for (p = 0; p < C->cps_count; p ++)
    {
        CP = C->cps[p];

        if (CP->vert != NULL)
        {
            CP->vert->selected = 1;
        }
    }
}

void convert_To_Cps_Vert_Selection(curve * C)
{
    int p;

    cp * CP;

    for (p = 0; p < C->cps_count; p ++)
    {
        CP = C->cps[p];

        if (CP->vert != NULL && CP->selected)
        {
            CP->vert->selected = 1;
        }
    }
}

void create_Verts_Selection_From_Curves()
{
    int c;

    curve * C;

    for (c = 0; c < curvesIndex; c ++)
    {
        C = curves[c];
        if (C->selected)
        {
            convert_To_Cp_Vert_Selection(C);
        }
    }
}

void create_Verts_Selection_From_Cps()
{
    int c;

    curve * C;

    for (c = 0; c < curvesIndex; c ++)
    {
        C = curves[c];
        if (C->selected)
        {
            convert_To_Cps_Vert_Selection(C);
        }
    }
}

void assert_Verts_Selection_(object * O)
{
    int v;
    vertex * V;

    O->selected_verts_count = 0;
    for (v = 0; v < O->vertcount; v ++)
    {
        V = &O->verts[v / ARRAYSIZE][v % ARRAYSIZE];
        if (V->selected)
            O->selected_verts[O->selected_verts_count ++] = v;
    }
}

void assert_Verts_Selection()
{
    int v, o;

    object * O;
    vertex * V;

    for (o = 0; o < selected_object_count; o ++)
    {
        O = objects[selected_objects[o]];

        O->selected_verts_count = 0;
        for (v = 0; v < O->vertcount; v ++)
        {
            V = &O->verts[v / ARRAYSIZE][v % ARRAYSIZE];
            if (V->selected)
                O->selected_verts[O->selected_verts_count ++] = v;
        }
    }
}

void print_Selection_Info()
{
    int o;

    object * O;

    for (o = 0; o < selected_object_count; o ++)
    {
        O = objects[selected_objects[o]];
        printf("%s %d\n", O->Name, O->selected_verts_count);
    }
}

void clear_Selected_Objects_Verts_Selection()
{
    int o, v;

    object * O;
    vertex * V;

    for (o = 0; o < selected_object_count; o ++)
    {
        O = objects[selected_objects[o]];

        for (v = 0; v < O->vertcount; v ++)
        {
            V = &O->verts[v / ARRAYSIZE][v % ARRAYSIZE];
            V->selected = 0;
        }
    }
}

void remember_Objects_Verts_Pos()
{
    int o, v, idx;

    object * O;
    vertex * V;

    for (o = 0; o < selected_object_count; o ++)
    {
        O = objects[selected_objects[o]];
        O->Movement_Enabled = 0;

        if (O->selected_verts_count > 0)
        {
            O->Movement_Enabled = 1;
            for (v = 0; v < O->selected_verts_count; v ++)
            {
                idx = O->selected_verts[v];
                V = &O->verts[idx / ARRAYSIZE][idx % ARRAYSIZE];
                O->vertex_Positions[v].x = V->Tx - O->T->pos[0];
                O->vertex_Positions[v].y = V->Ty - O->T->pos[1];
                O->vertex_Positions[v].z = V->Tz - O->T->pos[2];
            }
            printf("%d verts for %s remember\n", O->selected_verts_count, O->Name);
        }
    }
}

void move_Verts_To_Delta(float Delta[3])
{
    int o, v, idx;

    object * O;
    vertex * V;

    float rotVec[3][3];
    float result[3];

    float Delta_result[3];

    for (o = 0; o < selected_object_count; o ++)
    {
        O = objects[selected_objects[o]];
        if (O->Movement_Enabled)
        {
            invert_Rotation_scale(O->T, rotVec);
            rotate_vector(rotVec, Delta, Delta_result);

            for (v = 0; v < O->selected_verts_count; v ++)
            {
                idx = O->selected_verts[v];
                V = &O->verts[idx / ARRAYSIZE][idx % ARRAYSIZE];

                rotate_vector(rotVec, O->vertex_Positions[v].Pos, result);

                V->Rx = result[0] + Delta_result[0];
                V->Ry = result[1] + Delta_result[1];
                V->Rz = result[2] + Delta_result[2];
            }
        }
    }
}

void snap_back_Verts_To_Pos()
{
    int o, v, idx;

    object * O;
    vertex * V;

    float rotVec[3][3];
    float result[3];

    for (o = 0; o < selected_object_count; o ++)
    {
        O = objects[selected_objects[o]];
        if (O->Movement_Enabled)
        {
            invert_Rotation_scale(O->T, rotVec);

            for (v = 0; v < O->selected_verts_count; v ++)
            {
                idx = O->selected_verts[v];
                V = &O->verts[idx / ARRAYSIZE][idx % ARRAYSIZE];

                rotate_vector(rotVec, O->vertex_Positions[v].Pos, result);

                V->Rx = result[0];
                V->Ry = result[1];
                V->Rz = result[2];
            }
        }
    }
}

void update_Vertex_Control_Point()
{
    int o, v, idx;

    object * O;
    vertex * V;

    for (o = 0; o < selected_object_count; o ++)
    {
        O = objects[selected_objects[o]];
        if (O->Movement_Enabled)
        {
            for (v = 0; v < O->selected_verts_count; v ++)
            {
                idx = O->selected_verts[v];
                V = &O->verts[idx / ARRAYSIZE][idx % ARRAYSIZE];

                if (V->control_point != NULL)
                {
                    V->control_point->pos[0] = V->Tx;
                    V->control_point->pos[1] = V->Ty;
                    V->control_point->pos[2] = V->Tz;
                }
            }
        }
    }
}

void update_selected_Objects_T_Coords()
{
    int o, v, idx;

    object * O;
    vertex * V;

    for (o = 0; o < selected_object_count; o ++)
    {
        O = objects[selected_objects[o]];
        if (O->Movement_Enabled)
        {
            for (v = 0; v < O->selected_verts_count; v ++)
            {
                idx = O->selected_verts[v];
                V = &O->verts[idx / ARRAYSIZE][idx % ARRAYSIZE];

                V->Tx = O->T->rotVec[0][0] * V->Rx + O->T->rotVec[1][0] * V->Ry + O->T->rotVec[2][0] * V->Rz + O->T->pos[0];
                V->Ty = O->T->rotVec[0][1] * V->Rx + O->T->rotVec[1][1] * V->Ry + O->T->rotVec[2][1] * V->Rz + O->T->pos[1];
                V->Tz = O->T->rotVec[0][2] * V->Rx + O->T->rotVec[1][2] * V->Ry + O->T->rotVec[2][2] * V->Rz + O->T->pos[2];
            }
        }
    }
}

void find_Curves_Connected_To_Verts()
{
    int o, v, idx, s, t, condition;

    object * O;
    vertex * V;

    cp * CP;
    curve * C;
    curve_segment * S;

    connected_curves_count = 0;

    for (o = 0; o < selected_object_count; o ++)
    {
        O = objects[selected_objects[o]];
        if (O->Movement_Enabled)
        {
            for (v = 0; v < O->selected_verts_count; v ++)
            {
                idx = O->selected_verts[v];
                V = &O->verts[idx / ARRAYSIZE][idx % ARRAYSIZE];

                if (V->control_point != NULL)
                {
                    CP = V->control_point;

                    for (s = 0; s < CP->segment_count; s ++)
                    {
                        S = CP->segments[s];
                        condition = 1;

                        if (S == NULL)
                        {

                        }
                        else if (S->Curve != NULL)
                        {
                            for (t = 0; t < connected_curves_count; t ++)
                            {
                                C = curves[connected_curves[t]];
                                if (C == S->Curve)
                                {
                                    condition = 0;
                                    break;
                                }
                            }
                            if (condition)
                                connected_curves[connected_curves_count ++] = S->Curve->index;
                        }
                    }
                }
            }
        }
    }
}

void find_Verts_action_Center()
{
    int o, v, idx;

    object * O;
    vertex * V;

    Action_Center->pos[0] = 0;
    Action_Center->pos[1] = 0;
    Action_Center->pos[2] = 0;

    float Objects_Center[3];

    Objects_Center[0] = 0;
    Objects_Center[1] = 0;
    Objects_Center[2] = 0;

    int counter = 0;
    int o_counter = 0;

    for (o = 0; o < selected_object_count; o ++)
    {
        O = objects[selected_objects[o]];
        if (O->Movement_Enabled)
        {
            for (v = 0; v < O->selected_verts_count; v ++)
            {
                idx = O->selected_verts[v];
                V = &O->verts[idx / ARRAYSIZE][idx % ARRAYSIZE];

                counter ++;
                Action_Center->pos[0] += V->Tx;
                Action_Center->pos[1] += V->Ty;
                Action_Center->pos[2] += V->Tz;
            }
            o_counter ++;
            Objects_Center[0] -= O->T->pos[0];
            Objects_Center[1] -= O->T->pos[1];
            Objects_Center[2] -= O->T->pos[2];
        }
    }

    if (counter > 0)
    {
        Action_Center->pos[0] /= counter;
        Action_Center->pos[1] /= counter;
        Action_Center->pos[2] /= counter;
    }

    if (o_counter > 0)
    {
        Objects_Center[0] /= o_counter;
        Objects_Center[1] /= o_counter;
        Objects_Center[2] /= o_counter;
    }

    Action_Center->pos[0] += Objects_Center[0];
    Action_Center->pos[1] += Objects_Center[1];
    Action_Center->pos[2] += Objects_Center[2];

    printf("Action center %f %f %f\n", Action_Center->pos[0], Action_Center->pos[1], Action_Center->pos[2]);
}

void tune_subdivide_post_transformed(object * O, int L);

void rotate_T_Transformers()
{
    int t;

    transformer * T;

    for (t = 0; t < locatorIndex; t ++)
    {
        T = Locators[t];
        rotate_T(T);
    }
}

void rotate_Objects()
{
    int o;

    object * O;

    for (o = 0; o < objectIndex; o ++)
    {
        O = objects[o];

        if (!O->binding)
        {
            rotate_T(O->T);
            rotate_verts(O, *O->T);
        }
    }
}

void update_selected_Objects(int level)
{
    int o;

    object * O;

    for (o = 0; o < selected_object_count; o ++)
    {
        O = objects[selected_objects[o]];
        if (O->Movement_Enabled)
        {
            tune_subdivide_post_transformed(O, level);
        }
    }
}

void update_Selected_Verts_Positions(transformer * T)
{
    int o, v, idx;

    object * O;
    vertex * V;

    float rotVec_I[3][3];
    float rotVec[3][3];
    float result[3];
    float pos[3];
    float Pos[3];

    transformer * backup_T = T;

    for (o = 0; o < selected_object_count; o ++)
    {
        O = objects[selected_objects[o]];
        if (O->Movement_Enabled)
        {
            if (!O->binding)
            {
                T = O->T;
            }

            if (T == O->T)
            {
                invert_Rotation_scale(O->T, rotVec);
            }
            else
            {
                invert_Rotation_1(rotVec_I, T->rotVec_B);

//                if (T->IK != NULL)
//                    generate_scl_vec(T);

                invert_Rotation_scale(T, rotVec);
                rotate_matrix_I(rotVec, rotVec_I, rotVec);
            }

            for (v = 0; v < O->selected_verts_count; v ++)
            {
                idx = O->selected_verts[v];
                V = &O->verts[idx / ARRAYSIZE][idx % ARRAYSIZE];

                if (T == O->T)
                {
                    rotate_vector(rotVec, O->vertex_Positions[v].Pos, result);
                    rotate_center(result, Action_Center->rotVec, Action_Center->pos, pos);

                    V->Rx = pos[0];
                    V->Ry = pos[1];
                    V->Rz = pos[2];
                }
                else
                {
                    rotate_center(O->vertex_Positions[v].Pos, Action_Center->rotVec, Action_Center->pos, result);

                    Pos[0] = result[0] - T->pos[0];
                    Pos[1] = result[1] - T->pos[1];
                    Pos[2] = result[2] - T->pos[2];

                    rotate_vector(rotVec, Pos, pos);

                    V->Rx = pos[0] + T->pos_bind[0];
                    V->Ry = pos[1] + T->pos_bind[1];
                    V->Rz = pos[2] + T->pos_bind[2];

                }
            }
        }
    }

    T = backup_T;
}

void update_Selected_Verts_Positions_Move(transformer * T, float Delta[3])
{
    int o, v, idx;

    object * O;
    vertex * V;

    float rotVec_I[3][3];
    float rotVec[3][3];
    float result[3];
    float pos[3];
    float Pos[3];

    float Delta_result[3];

    transformer * backup_T = T;

    for (o = 0; o < selected_object_count; o ++)
    {
        O = objects[selected_objects[o]];
        if (O->Movement_Enabled)
        {
            if (!O->binding)
            {
                T = O->T;
            }

            if (T == O->T)
            {
                invert_Rotation_scale(O->T, rotVec);
            }
            else
            {
                invert_Rotation_1(rotVec_I, T->rotVec_B);

//                if (T->IK != NULL)
//                    generate_scl_vec(T);

                invert_Rotation_scale(T, rotVec);
                rotate_matrix_I(rotVec, rotVec_I, rotVec);
            }

            rotate_vector(rotVec, Delta, Delta_result);

            for (v = 0; v < O->selected_verts_count; v ++)
            {
                idx = O->selected_verts[v];
                V = &O->verts[idx / ARRAYSIZE][idx % ARRAYSIZE];

                if (T == O->T)
                {
                    rotate_vector(rotVec, O->vertex_Positions[v].Pos, result);

                    V->Rx = result[0] + Delta_result[0];
                    V->Ry = result[1] + Delta_result[1];
                    V->Rz = result[2] + Delta_result[2];
                }
                else
                {
                    Pos[0] = O->vertex_Positions[v].Pos[0] - T->pos[0];
                    Pos[1] = O->vertex_Positions[v].Pos[1] - T->pos[1];
                    Pos[2] = O->vertex_Positions[v].Pos[2] - T->pos[2];

                    rotate_vector(rotVec, Pos, pos);

                    V->Rx = pos[0] + T->pos_bind[0] + Delta_result[0];
                    V->Ry = pos[1] + T->pos_bind[1] + Delta_result[1];
                    V->Rz = pos[2] + T->pos_bind[2] + Delta_result[2];

                }
            }
        }
    }

    T = backup_T;
}

#endif // MODELING_H_INCLUDED
