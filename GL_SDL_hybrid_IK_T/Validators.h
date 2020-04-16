/*
The MIT License

Copyright <2018> <Allan Kiipli>
*/

#ifndef VALIDATORS_H_INCLUDED
#define VALIDATORS_H_INCLUDED

void verify_textPack_indices()
{
    printf("verify textPack indices\n");
    int t;

    for (t = 0; t < text_Pack_c; t ++)
    {
        printf("%d %s\n", t, text_Pack[t].texture_Name);
        printf("%d %d\n", text_Pack[t].texture, text_Pack[t].Texture_idx);
    }
}

void verify_normPack_indices()
{
    printf("verify normPack indices\n");
    int t;

    for (t = 0; t < norm_Pack_c; t ++)
    {
        printf("%d %s\n", t, norm_Pack[t].texture_Name);
        printf("%d %d\n", norm_Pack[t].texture, norm_Pack[t].Texture_idx);
    }
}

void print_out_Deformer_T_indices(deformer * D)
{
    int v, o;

    object * O;
    vertex * V;

    for (o = 0; o < D->Objects_Count; o ++)
    {
        O = D->Objects[o];
        for (v = 0; v < O->vertcount; v ++)
        {
            V = &O->verts[v / ARRAYSIZE][v % ARRAYSIZE];
            printf("V %f %f %f ", V->Tx, V->Ty, V->Tz);
        }
        printf("\n");
    }
}

void validate_Poses()
{
    int p;

    pose * P;

    printf("VALIDATE POSES\n");

    /*    char * Name;
    deformer * D;
    transformer_pose * TP;
    int transformers_count;*/

    for (p = 0; p < posesIndex; p ++)
    {
        P = poses[p];
        printf("%s %s\n", P->Name, P->D->Name);
    }
}

void validate_Bones()
{
    int b;

    bone * B;

    printf("VALIDATE BONES\n");

    for (b = 0; b < bonesIndex; b ++)
    {
        B = bones[b];
        printf("B %s\n", B->Name);
        printf("A %s B %s D %s\n", B->A->Name, B->B->Name, B->D->Name);
    }
}

void validate_Deformers()
{
    int p, b, o, s, t, d;

    deformer * D;
    transformer * T;
    vert_selection * S;
    object * O;
    bone * B;
    pose * P;

    printf("VALIDATE DEFORMERS\n");

    for (d = 0; d < deformerIndex; d ++)
    {
        D = deformers[d];
        printf("D %s\n", D->Name);
        printf("Transformers\n");
        for (t = 0; t < D->Transformers_Count; t ++)
        {
            T = D->Transformers[t];
            printf("T %s %s\n", T->Name, T->Deformer->Name);
        }
        printf("Selections\n");
        for (s = 0; s < D->Selections_Count; s ++)
        {
            S = D->Selections[s];
            printf("S %s %s %s\n", S->Name, S->Object->Name, S->Transformer->Name);
        }
        printf("Objects\n");
        for (o = 0; o < D->Objects_Count; o ++)
        {
            O = D->Objects[o];
            printf("O %s %d %d\n", O->Name, O->binding, O->deforms);
        }
        printf("Bones\n");
        for (b = 0; b < D->Bones_Count; b ++)
        {
            B = D->Bones[b];
            printf("B %s %s %s %s\n", B->Name, B->A->Name, B->B->Name, B->D->Name);
        }
        printf("Poses\n");
        for (p = 0; p < D->Poses_Count; p ++)
        {
            P = D->Poses[p];
            printf("P %s %s\n", P->Name, P->D->Name);
        }
    }
}

void validate_Selections()
{
    int s, o;

    object * O;
    transformer * T;
    vert_selection * S;

    printf("VALIDATE SELECTIONS\n");

    for (o = 0; o < objectIndex; o ++)
    {
        O = objects[o];
        printf("O %s\n", O->Name);
        for (s = 0; s < O->vertex_selections; s ++)
        {
            S = O->vertex_selection[s];
            T = S->Transformer;
            printf("S %s, T %s, O %s\n", S->Name, T->Name, S->Object->Name);
        }
    }
}

void validate_Transformers(transformer * T)
{
    int s, c;

    transformer * T0;
    vert_selection * S;

    if (T->parent == NULL)
    {
        if (T->Deformer)
            printf("T %s %d %s\n", T->Name, T->Selections_Count, T->Deformer->Name);
        else
            printf("T %s %d\n", T->Name, T->Selections_Count);
    }
    else
    {
        if (T->Deformer)
            printf("T %s %s %d %s\n", T->Name, T->parent->Name, T->Selections_Count, T->Deformer->Name);
        else
            printf("T %s %s %d\n", T->Name, T->parent->Name, T->Selections_Count);
    }

    for (s = 0; s < T->Selections_Count; s ++)
    {
        S = T->Selections[s];
        printf("T S %s O %s\n", S->Name, S->Object->Name);
    }

    for (c = 0; c < T->childcount; c ++)
    {
        T0 = T->childs[c];
        validate_Transformers(T0);
    }
}

void validate_poly_Edges(object * O, polygon * P)
{
    int i;
    for(i = 0; i < P->edgecount; i ++)
    {
        int idx = P->verts[i];
        vertex * V = &O->verts[idx / ARRAYSIZE][idx % ARRAYSIZE];
        printf("%d ", V->index);
    }
    printf("\n");
    for(i = 0; i < P->edgecount; i ++)
    {
        int idx = P->edges[i];
        edge * E = &O->edges[idx / ARRAYSIZE][idx % ARRAYSIZE];
        printf("%d %d\n", E->verts[0], E->verts[1]);
    }
}

void validate_poly_UVEdges(object * O, polygon * P)
{
    int i;
    for(i = 0; i < P->edgecount; i ++)
    {
        int idx = P->texts[i];
        uv * UV = &O->uvtex[idx / ARRAYSIZE][idx % ARRAYSIZE];
        printf("%d ", UV->index);
    }
    printf("\n");
    for(i = 0; i < P->edgecount; i ++)
    {
        int idx = P->uveds[i];
        uv_edge * E = &O->uveds[idx / ARRAYSIZE][idx % ARRAYSIZE];
        printf("%d %d\n", E->uv_verts[0], E->uv_verts[1]);
    }
}

#endif // VALIDATORS_H_INCLUDED
