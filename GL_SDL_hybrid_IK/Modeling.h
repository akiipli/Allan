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

void remember_Objects_Verts_Pos()
{
    int o, v, idx;

    object * O;
    vertex * V;
    vertex_Pos * vertex_Positions;

    for (o = 0; o < selected_object_count; o ++)
    {
        O = objects[selected_objects[o]];
        O->Movement_Enabled = 0;
        vertex_Positions = realloc(O->vertex_Positions, O->selected_verts_count * sizeof(vertex_Pos));
        if (vertex_Positions != NULL)
        {
            O->vertex_Positions = vertex_Positions;
            O->Movement_Enabled = 1;
            for (v = 0; v < O->selected_verts_count; v ++)
            {
                idx = O->selected_verts[v];
                V = &O->verts[idx / ARRAYSIZE][idx % ARRAYSIZE];
                O->vertex_Positions[v].x = V->Tx - O->T->pos[0];
                O->vertex_Positions[v].y = V->Ty - O->T->pos[1];
                O->vertex_Positions[v].z = V->Tz - O->T->pos[2];
            }
            //printf("%d verts for %s allocated\n", O->selected_verts_count, O->Name);
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

#endif // MODELING_H_INCLUDED
