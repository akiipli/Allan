/*
The MIT License

Copyright <2018> <Allan Kiipli>
*/

#ifndef MATERIALS_H_INCLUDED
#define MATERIALS_H_INCLUDED

#define MATERIALS_TOTAL 100

typedef struct
{
    int index;
    int smooth;
    char Name[STRLEN];
    int texture;
    int Texture_idx;
    int use_texture;
    float R, G, B, A;
    float Shininess;
    float Displacement;
    int normal;
    int Normal_idx;
    int bump;
    int Bump_idx;
}
surface_Material;

surface_Material Materials[MATERIALS_TOTAL];
int Materials_count = 0;

int currentMaterial = 0;

int init_Material(int index)
{
    int i = index % MATERIALS_TOTAL;
    surface_Material * M = &Materials[i];
    M->index = index;
    M->smooth = 0;
    sprintf(M->Name, "M");
    M->texture = 0;
    M->normal = 0;
    M->bump = 0;
    M->Texture_idx = 0; // index in displayLists for glCallList
    M->Normal_idx = 0; // index in normalsLists for glCallList
    M->Bump_idx = 0; // index in bumpsLists for glCallList
    M->use_texture = 0;
    M->R = 255;
    M->G = 255;
    M->B = 255;
    M->A = 255;
    M->Shininess = 2;
    return i;
}

void set_Material_H_Button(int index)
{
    int i;
    for (i = 0; i < H_MATR_NUM; i ++)
    {
        Button_h_matr[i].color = UI_GRAYB;
    }
    if (index > -1)
        Button_h_matr[index].color = UI_GRAYD;
}

#endif // MATERIALS_H_INCLUDED
