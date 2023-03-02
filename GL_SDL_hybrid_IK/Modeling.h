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

    for (o = 0; o < selected_object_count; o ++)
    {
        O = objects[selected_objects[o]];
        if (O->Movement_Enabled)
        {
            for (v = 0; v < O->selected_verts_count; v ++)
            {
                idx = O->selected_verts[v];
                V = &O->verts[idx / ARRAYSIZE][idx % ARRAYSIZE];
                V->Rx = O->vertex_Positions[v].x + Delta[0];
                V->Ry = O->vertex_Positions[v].y + Delta[1];
                V->Rz = O->vertex_Positions[v].z + Delta[2];
            }
        }
    }
}

void snap_back_Verts_To_Pos()
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
                V->Rx = O->vertex_Positions[v].x;
                V->Ry = O->vertex_Positions[v].y;
                V->Rz = O->vertex_Positions[v].z;
            }
        }
    }
}


#endif // MODELING_H_INCLUDED
