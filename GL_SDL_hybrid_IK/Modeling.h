/*
The MIT License

Copyright <2018> <Allan Kiipli>
*/

#ifndef MODELING_H_INCLUDED
#define MODELING_H_INCLUDED

int MODELING_MODE = 0;

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


#endif // MODELING_H_INCLUDED
