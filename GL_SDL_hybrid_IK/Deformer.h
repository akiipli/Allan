/*
The MIT License

Copyright <2018> <Allan Kiipli>
*/

#ifndef DEFORMER_H_INCLUDED
#define DEFORMER_H_INCLUDED

//#include "Object.h"

#define DEFR_TYPE_DEFR "deformers"
#define DEFR_TYPE_HIR "hierarchys"
#define DEFR_TYPE_SEL "selections"

#define MAX_INFLUENCE 10
#define DEFORMERS 200

int Deformers_c;
int Deformer_Node_c;
char Defr_Names[DEFORMERS][STRLEN];
int DefrIndex;
int defr_start;

int Deformer_List[DEFORMERS];

char * defr_types[DEFR_NUM];
int defr_type_count = DEFR_NUM;
char defr_type[TYPE_LENGTH];

int current_defr_type = 0;
int currentDeformer_Node = 0;

int Defr_X_Offset[DEFORMERS];
int Defr_X_Collapsed[DEFORMERS];

char DefrName_Remember[STRLEN];

typedef struct pose pose;

struct deformer
{
    int index;
    unsigned address;
    char * Name;
    int collapsed;
    int selected;

    transformer ** Transformers;
    int Transformers_Count;

    vert_selection ** Selections;
    int Selections_Count;

    object ** Objects;
    int Objects_Count;

    bone ** Bones;
    int Bones_Count;

    pose ** Poses;
    int Poses_Count;

    int current_pose;
    float Delta[3];

    ikChain ** IKchains;
    int IKchains_Count;

    float rotVec[3][3];
};

void free_Deformer(deformer * D)
{
    free(D->Name);

    int s, t;

    vert_selection * S;
    transformer * T;

    for (t = 0; t < D->Transformers_Count; t ++)
    {
        T = D->Transformers[t];

        for (s = 0; s < T->Selections_Count; s ++)
        {
            S = T->Selections[s];
            S->Transformer = NULL;
        }
        T->Selections_Count = 0;
    }

    free(D->Transformers);
    free(D->Selections);
    free(D->Objects);
    free(D->Bones);
    free(D->IKchains);
    free(D->Poses);
    free(D);
}

deformer * deformers[DEFORMERS];
int deformerIndex = 0;

void init_deformers()
{
    Defr_X_Offset[0] = 0;
    Defr_X_Collapsed[0] = 0;

    DefrIndex = 0;
    defr_start = 0;

    defr_types[0] = DEFR_TYPE_DEFR;
    defr_types[1] = DEFR_TYPE_HIR;
    defr_types[2] = DEFR_TYPE_SEL;

    memcpy(&defr_type, defr_types[0], strlen(defr_types[0]));

    int d;

    for (d = 0; d < DEFORMERS; d ++)
    {
        deformers[d] = NULL;
    }
}

void add_Objects_To_Deformer(deformer * D)
{
    int d, o, condition;
    object * O;

    for (o = 0; o < selected_object_count; o ++)
    {
        O = objects[selected_objects[o]];
        if (O->binding || (strcmp(O->Name, "Cube") == 0)) /*CUBECOMMENT*/
            continue;
        condition = 1;
        for (d = 0; d < D->Objects_Count; d ++)
        {
            if (O == D->Objects[d])
            {
                condition = 0;
                break;
            }
        }
        if (condition)
        {
            D->Objects = realloc(D->Objects, (D->Objects_Count + 1) * sizeof(object *));
            D->Objects[D->Objects_Count ++] = O;
            O->binding = 1;
        }
    }
}

void remove_Deformer(deformer * D)
{
    int t;
    transformer * T;

    if (D->Transformers_Count > 0)
    {
        D->Transformers[0]->style = 0;
    }

    for (t = 0; t < D->Transformers_Count; t ++)
    {
        T = D->Transformers[t];
        T->Deformer = NULL;
    }

    int b;
    bone * B;
    for (b = 0; b < D->Bones_Count; b ++)
    {
        B = D->Bones[b];
        B->D = NULL;
    }

    int o;
    object * O;
    for (o = 0; o < D->Objects_Count; o ++)
    {
        O = D->Objects[o];
        O->binding = 0;
    }

    int i;
    ikChain * I;
    for (i = 0; i < D->IKchains_Count; i ++)
    {
        I = D->IKchains[i];
        I->Deformer = NULL;
    }

    int d;

    deformerIndex --;

    for(d = D->index; d < deformerIndex; d ++)
    {
        deformers[d] = deformers[d + 1];
        deformers[d]->index = d;
    }

    free_Deformer(D);
}

void add_Deformer()
{
    deformer * D = malloc(sizeof(deformer));
    deformers[deformerIndex] = D;
    D->index = deformerIndex;
    D->Name = malloc(STRLEN * sizeof(char));
    sprintf(D->Name, "%s %d", "Deformer", deformerIndex);
    D->collapsed = 0;
    D->selected = 0;
    D->Transformers_Count = 0;
    D->Selections_Count = 0;
    D->Objects_Count = 0;
    D->Bones_Count = 0;
    D->Poses_Count = 0;
    D->IKchains_Count = 0;
    D->Transformers = malloc(0 * sizeof(transformer*));
    D->Selections = malloc(0 * sizeof(vert_selection*));
    D->Objects = malloc(0 * sizeof(object*));
    D->Bones = malloc(0 * sizeof(bone*));
    D->Poses = malloc(0 * sizeof(pose*));
    D->IKchains = malloc(0 * sizeof(ikChain*));
    memcpy(D->Delta, (float[3]){0, 0, 0}, sizeof D->Delta);
    D->current_pose = 0;
    memcpy(&D->rotVec, (float[3][3]) {{1.0, 0.0, 0.0}, {0.0, 1.0, 0.0}, {0.0, 0.0, 1.0}}, sizeof D->rotVec);
    deformerIndex ++;
}

void list_Deformer_Nodes(deformer * D, int SelsIndex, object * O)
{
    int t, s;
    transformer * T;
    vert_selection * S;

    for (t = 0; t < D->Transformers_Count; t ++)
    {
        if (Deformers_c >= DEFORMERS)
            break;
        T = D->Transformers[t];
        if (T->Bone != NULL && (T == T->Bone->B))
            continue;
        if (T->Bone != NULL)
            sprintf(Defr_Names[Deformers_c], "%s", T->Bone->Name);
        else
            sprintf(Defr_Names[Deformers_c], "%s", T->Name);
        Deformer_List[Deformers_c] = T->index;

        Defr_X_Offset[Deformers_c] = 2;
        Defr_X_Collapsed[Deformers_c] = 0;

        for (s = 0; s < T->Selections_Count; s ++)
        {
            S = T->Selections[s];

            if (strlen(Defr_Names[Deformers_c]) + strlen(S->Name) + 1 >= STRLEN)
            {
                break;
            }

            if (S->Name == NULL) continue;

            if (S->index == SelsIndex && S->Object == O)
            {
                strcat(Defr_Names[Deformers_c], "/");
            }
            else
            {
                strcat(Defr_Names[Deformers_c], "|");
            }
            strcat(Defr_Names[Deformers_c], S->Name);
        }

        Deformers_c ++;
    }
}

void create_Deformers_List(int SelsIndex, object * O)
{
    int d;
    deformer * D;
    Deformers_c = 0;
    Deformer_Node_c = 1;

    for (d = 0; d < deformerIndex; d ++)
    {
        if (Deformers_c >= DEFORMERS)
            break;
        D = deformers[d];

        if (D->selected)
            selected_deformer_node = Deformers_c;

        memcpy(Defr_Names[Deformers_c], D->Name, strlen(D->Name));
        Defr_Names[Deformers_c][strlen(D->Name)] = '\0';
        Deformer_List[Deformers_c] = -Deformer_Node_c;
        Defr_X_Offset[Deformers_c] = 1;
        Defr_X_Collapsed[Deformers_c] = D->collapsed;
        Deformers_c ++;
        Deformer_Node_c ++;
        if (!D->collapsed)
            list_Deformer_Nodes(D, SelsIndex, O);
    }
}

void normalize_Deformer_Selections(deformer * D)
{
    typedef struct
    {
        int index;
        float weights;
    }
    overlap;

    int i, v, o, o_s, s;

    object * O;
    vertex * V;
    transformer * T;
    vert_selection * S;

    int condition;

    for (o = 0; o < D->Objects_Count; o ++)
    {
        condition = 0;

        for (o_s = 0; o_s < selected_object_count; o_s ++)
        {
            O = objects[selected_objects[o_s]];
            if (O == D->Objects[o])
            {
                condition = 1;
                break;
            }
        }

        if (condition)
        {
            overlap * OV = malloc(O->vertcount * sizeof(overlap));
            for (v = 0; v < O->vertcount; v ++)
            {
                OV[v].index = 0;
                OV[v].weights = 0;
            }
            for (v = 0; v < O->vertcount; v ++)
            {
                V = &O->verts[v / ARRAYSIZE][v % ARRAYSIZE];

                if (V->selected == 0)
                {
                    continue;
                }

                for (s = 0; s < O->vertex_selections; s ++)
                {
                    S = O->vertex_selection[s];
                    T = S->Transformer;
                    if (T != NULL)
                    {
                        for (i = 0; i < S->indices_count; i ++)
                        {
                            if (S->indices[i] == v)
                            {
                                OV[v].index ++;
                                OV[v].weights += S->weights[i];
                                break;
                            }
                        }
                    }
                }
            }
            for (s = 0; s < O->vertex_selections; s ++)
            {
                S = O->vertex_selection[s];
                for (i = 0; i < S->indices_count; i ++)
                {
                    if (OV[S->indices[i]].index > 0 && OV[S->indices[i]].weights > 0)
                        //S->weights[i] /= OV[S->indices[i]].weights;
                        S->weights[i] = 1.0 / (float)OV[S->indices[i]].index;
                }
            }
            free(OV);
        }
    }
}

void remove_Selection_From_Transformer(object * O, transformer * T, char * Name)
{
    int s, index;
    int condition = 0;

    vert_selection * S;

    for (s = 0; s < T->Selections_Count; s ++)
    {
        S = T->Selections[s];
        if (strcmp(Name, S->Name) == 0 && S->Object == O)
        {
            index = s;
            condition = 1;
            break;
        }
    }

    if (condition)
    {
        S->Transformer = NULL;
        T->Selections_Count --;
        for (s = index; s < T->Selections_Count; s ++)
        {
            T->Selections[s] = T->Selections[s + 1];
        }
    }
}

void remove_Selection_From_Deformer_(object * O, char * Name, deformer * D)
{
    int s, d, index;
    vert_selection * S;
    int condition = 0;

    for (s = 0; s < O->vertex_selections; s ++)
    {
        if (strcmp(Name, O->vertex_selection[s]->Name) == 0)
        {
            S = O->vertex_selection[s];
            for (d = 0; d < D->Selections_Count; d ++)
            {
                if (D->Selections[d] == S)
                {
                    index = d;
                    condition = 1;
                    break;
                }
            }
            break;
        }
    }

    if (condition)
    {
        D->Selections_Count --;
        for (d = index; d < D->Selections_Count; d ++)
        {
            D->Selections[d] = D->Selections[d + 1];
        }
    }
}

void remove_Selection_From_Deformer(object * O, char * Name, deformer * D)
{
    int o, s, d, index;
    vert_selection * S;
    int condition = 0;

    for (s = 0; s < O->vertex_selections; s ++)
    {
        if (strcmp(Name, O->vertex_selection[s]->Name) == 0)
        {
            S = O->vertex_selection[s];
            for (d = 0; d < D->Selections_Count; d ++)
            {
                if (D->Selections[d] == S)
                {
                    index = d;
                    condition = 1;
                    break;
                }
            }
            break;
        }
    }

    if (condition)
    {
        D->Selections_Count --;
        for (d = index; d < D->Selections_Count; d ++)
        {
            D->Selections[d] = D->Selections[d + 1];
        }
    }

    D->Objects_Count = 0;

    object * O0;

    for (s = 0; s < D->Selections_Count; s ++)
    {
        O0 = D->Selections[s]->Object;
        condition = 1;
        for (o = 0; o < D->Objects_Count; o ++)
        {
            if (O0 == D->Objects[o])
            {
                condition = 0;
                break;
            }
        }
        if (condition)
        {
            D->Objects[D->Objects_Count ++] = O0;
        }
    }

    deformer * D0;
    condition = 1;

    for (d = 0; d < deformerIndex; d ++)
    {
        D0 = deformers[d];
        for (o = 0; o < D0->Objects_Count; o ++)
        {
            O0 = D0->Objects[o];
            if (O0 == O)
            {
                condition = 0;
                break;
            }
        }
    }
    if (condition)
    {
        O->binding = 0;
    }
}

void establish_Deform_Pose(transformer * T)
{
    memcpy(&T->rot_deform, &T->rot, sizeof T->rot_deform);
    memcpy(&T->pos_deform, &T->pos, sizeof T->pos_deform);
}

void establish_Deform_Pose_For_Transformers()
{
    printf("establish Deform Pose For Transformers\n");
    int t;
    transformer * T;

    for(t = 0; t < transformerIndex; t ++)
    {
        T = transformers[t];
        establish_Deform_Pose(T);
    }
}

void goto_Deform_Pose(transformer * T)
{
    memcpy(&T->rot, &T->rot_deform, sizeof T->rot);
    memcpy(&T->pos, &T->pos_deform, sizeof T->pos);
}

void goto_Deform_Pose_For_Transformers()
{
    int t;
    transformer * T;

    for(t = 0; t < transformerIndex; t ++)
    {
        T = transformers[t];
        if (T->bind_set)
            goto_Deform_Pose(T);
    }
}

void goto_Bind_Pose(transformer * T)
{
    memcpy(&T->rot_deform, &T->rot, sizeof T->rot_deform);
    memcpy(&T->pos_deform, &T->pos, sizeof T->pos_deform);
    memcpy(&T->rot, &T->rot_bind, sizeof T->rot);
    memcpy(&T->pos, &T->pos_bind, sizeof T->pos);
}

void goto_Bind_Pose_For_Transformers()
{
    int t;
    transformer * T;

    for(t = 0; t < transformerIndex; t ++)
    {
        T = transformers[t];
        if (T->bind_set)
            goto_Bind_Pose(T);
    }
}

void goto_Bind_Pose_For_Deformer(deformer * D)
{
    int t;
    transformer * T;

    for(t = 0; t < D->Transformers_Count; t ++)
    {
        T = D->Transformers[t];
        goto_Bind_Pose(T);
    }
}

void set_Bind_Pose(transformer * T)
{
    T->bind_set = 1;

    memcpy(&T->rot_bind, &T->rot, sizeof T->rot_bind);
    memcpy(&T->pos_bind, &T->pos, sizeof T->pos_bind);

//    memcpy(&T->rot, (float[3]) {0.0, 0.0, 0.0}, sizeof T->rot);
//    memcpy(&T->pos, (float[3]) {0.0, 0.0, 0.0}, sizeof T->pos);
}

void set_Bind_Pose_For_Branch(transformer * T)
{
    int t;
    transformer * T0;

    set_Bind_Pose(T);

    for (t = 0; t < T->childcount; t ++)
    {
        T0 = T->childs[t];
        set_Bind_Pose_For_Branch(T0);
    }
}

void set_Bind_Pose_For_Transformers(int force)
{
    int t;
    transformer * T;

    for(t = 0; t < transformerIndex; t ++)
    {
        T = transformers[t];
        if (force || !T->bind_set)
            set_Bind_Pose(T);
    }
}

void set_Bind_Pose_For_Deformer(deformer * D)
{
    int t;
    transformer * T;

    for(t = 0; t < D->Transformers_Count; t ++)
    {
        T = D->Transformers[t];
        set_Bind_Pose(T);
    }
}

void remove_Object_From_Deformer(object * O, deformer * D)
{
    int s, index, o;

    transformer * T;
    vert_selection * S;

    int condition = 0;

    for (o = 0; o < D->Objects_Count; o ++)
    {
        if (O == D->Objects[o])
        {
            index = o;
            condition = 1;
            break;
        }
    }

    if (condition)
    {
        for (s = 0; s < O->vertex_selections; s ++)
        {
            S = O->vertex_selection[s];
            T = S->Transformer;
            if (T != NULL)
                remove_Selection_From_Transformer(O, T, S->Name);
        }

        for (s = 0; s < O->vertex_selections; s ++)
        {
            S = O->vertex_selection[s];
            remove_Selection_From_Deformer_(O, S->Name, D);
        }
        O->binding = 0;
        D->Objects_Count --;
        for (o = index; o < D->Objects_Count; o ++)
        {
            D->Objects[o] = D->Objects[o + 1];
        }
    }
}

void remove_Bone_From_Deformer(bone * B)
{
    printf("remove Bone From Deformer\n");

    if (B->D != NULL)
    {
        int index, b;

        int condition = 0;

        deformer * D = B->D;

        for (b = 0; b < D->Bones_Count; b ++)
        {
            if (B == D->Bones[b])
            {
                index = b;
                condition = 1;
                break;
            }
        }

        if (condition)
        {
            printf("removing Bone!\n");

            D->Bones_Count --;
            for (b = index; b < D->Bones_Count; b ++)
            {
                D->Bones[b] = D->Bones[b + 1];
            }
            B->D = NULL;
        }
    }
}

void remove_Transformer_From_Deformer_(transformer * T, deformer * D)
{
    int t, index;
    int condition = 0;

    bone * B;

    for (t = 0; t < D->Transformers_Count; t ++)
    {
        if (T == D->Transformers[t])
        {
            index = t;
            condition = 1;
            break;
        }
    }
    if (condition)
    {
        D->Transformers_Count --;
        for (t = index; t < D->Transformers_Count; t ++)
        {
            D->Transformers[t] = D->Transformers[t + 1];
        }
        T->Deformer = NULL;
        if (T->Bone != NULL)
        {
            B = T->Bone;
            if (T->Bone->A == T)
                remove_Bone_From_Deformer(B);
        }
        if (T->style == root_node)
        {
            T->style = 0;
        }
    }
}

void remove_Transformer_From_Deformer(transformer * T)
{
    deformer * D = T->Deformer;

    int t, index;
    int condition = 0;
    for (t = 0; t < D->Transformers_Count; t ++)
    {
        if (T == D->Transformers[t])
        {
            index = t;
            condition = 1;
            break;
        }
    }
    if (condition)
    {
        D->Transformers_Count --;
        for (t = index; t < D->Transformers_Count; t ++)
        {
            D->Transformers[t] = D->Transformers[t + 1];
        }
        T->Deformer = NULL;
        if (T->style == root_node)
        {
            T->style = 0;
        }
    }
}

void add_IkChain_To_Deformer(ikChain * I, deformer * D)
{
    int i;
    int condition = 1;
    for (i = 0; i < D->IKchains_Count; i ++)
    {
        if (I == D->IKchains[i])
        {
            condition = 0;
            break;
        }
    }
    if (condition && I->Deformer == NULL)
    {
        D->IKchains = realloc(D->IKchains, (D->IKchains_Count + 1) * sizeof(ikChain *));
        D->IKchains[D->IKchains_Count ++] = I;
        I->Deformer = D;
    }
}

void add_Transformer_To_Deformer(transformer * T, deformer * D)
{
    int t;
    int condition = 1;
    for (t = 0; t < D->Transformers_Count; t ++)
    {
        if (T == D->Transformers[t])
        {
            condition = 0;
            break;
        }
    }
    if (condition && T->Deformer == NULL)
    {
        D->Transformers = realloc(D->Transformers, (D->Transformers_Count + 1) * sizeof(transformer *));
        D->Transformers[D->Transformers_Count ++] = T;
        T->Deformer = D;

        if (D->Transformers_Count == 1)
        {
            T->style = root_node;
        }
    }
}

void add_Children_To_Deformer(transformer * T, deformer * D)
{
    int c;
    transformer * C;
    for (c = 0; c < T->childcount; c ++)
    {
        C = T->childs[c];
        add_Transformer_To_Deformer(C, D);
        add_Children_To_Deformer(C, D);
    }
}

void remove_Children_From_Deformer(transformer * T, deformer * D)
{
    int c;
    transformer * C;
    for (c = 0; c < T->childcount; c ++)
    {
        C = T->childs[c];
        remove_Transformer_From_Deformer_(C, D);
        remove_Children_From_Deformer(C, D);
    }
}

void remove_Branch_From_Deformer(transformer * T, deformer * D)
{
    remove_Transformer_From_Deformer_(T, D);
    remove_Children_From_Deformer(T, D);
}

void collect_IK_Chains(transformer * T)
{
    int c;
    transformer * C;
    for (c = 0; c < T->childcount; c ++)
    {
        C = T->childs[c];
        if (C->IK != NULL && (C->style == ik_goal || C->style == ik_fixed))
        {
            ik_Chains_Collection[ik_Collection_Count ++] = C->IK;
        }
        collect_IK_Chains(C);
    }
}

void add_Branch_IK_Chains(transformer * T, deformer * D)
{
    ik_Collection_Count = 0;
    collect_IK_Chains(T);

    int i;
    for (i = 0; i < ik_Collection_Count; i ++)
    {
        add_IkChain_To_Deformer(ik_Chains_Collection[i], D);
    }
}

void add_Branch_To_Deformer(transformer * T, deformer * D)
{
    add_Transformer_To_Deformer(T, D);
    add_Children_To_Deformer(T, D);
}

void add_Selection_To_Deformer(object * O, char * Name, transformer * T, deformer * D)
{
    int o, s, d, t;
    vert_selection * S;
    int condition = 0;
    int Condition = 0;

    for (s = 0; s < O->vertex_selections; s ++)
    {
        if (strcmp(Name, O->vertex_selection[s]->Name) == 0)
        {
            S = O->vertex_selection[s];
            condition = 1;
            for (t = 0; t < T->Selections_Count; t ++)
            {
                if (T->Selections[t] == S)
                {
                    condition = 0;
                    break;
                }
            }
            Condition = 1;
            for (d = 0; d < D->Selections_Count; d ++)
            {
                if (D->Selections[d] == S)
                {
                    Condition = 0;
                    break;
                }
            }
            break;
        }
    }
    if (condition && Condition)
    {
        int condition_ = 1;
        T->Selections = realloc(T->Selections, (T->Selections_Count + 1) * sizeof(vert_selection *));
        T->Selections[T->Selections_Count ++] = S;
        S->Transformer = T;
        for (t = 0; t < D->Transformers_Count; t ++)
        {
            if (D->Transformers[t] == T)
            {
                condition_ = 0;
                break;
            }
        }
        if (condition_)
        {
            D->Transformers = realloc(D->Transformers, (D->Transformers_Count + 1) * sizeof(transformer *));
            D->Transformers[D->Transformers_Count ++] = T;
            T->Deformer = D;
        }

        condition_ = 1;
        D->Selections = realloc(D->Selections, (D->Selections_Count + 1) * sizeof(vert_selection *));
        D->Selections[D->Selections_Count ++] = S;
        for (o = 0; o < D->Objects_Count; o ++)
        {
            if (D->Objects[o] == O)
            {
                condition_ = 0;
                break;
            }
        }
        if (condition_)
        {
            D->Objects = realloc(D->Objects, (D->Objects_Count + 1) * sizeof(object *));
            D->Objects[D->Objects_Count ++] = O;
            O->binding = 1;
        }
    }
}

void set_Defr_H_Button(int index)
{
    int i;
    for (i = 0; i < H_DEFR_NUM; i ++)
    {
        Button_h_defr[i].color = UI_GRAYB;
    }
    if (index > -1)
        Button_h_defr[index].color = UI_GRAYD;
}

void move_vertex_groups_T(transformer * T);

void move_Deformer_verts(deformer * D)
{
    int t;
    transformer * T;

    for (t = 0; t < D->Transformers_Count; t ++)
    {
        T = D->Transformers[t];
        move_vertex_groups_T(T);
    }
}

void rotate_Deformer_verts(deformer * D)
{
    int t;
    transformer * T;

    for (t = 0; t < D->Transformers_Count; t ++)
    {
        T = D->Transformers[t];
        rotate_vertex_groups(T);
    }
}

void paste_Deformer_rotVec(deformer * D)
{
    int t;
    transformer * T;

    for (t = 0; t < D->Transformers_Count; t ++)
    {
        T = D->Transformers[t];
        memcpy(T->rotVec, T->rotVec_, sizeof(T->rotVec));
        if (T->Bone != NULL)
        {
            memcpy(T->Bone->rotVec_, T->rotVec_, sizeof(T->Bone->rotVec_));
        }
    }
}

void free_ikChain(ikChain * I);

void remove_ikChain_From_Deformer(ikChain * I, deformer * D)
{
    int i, index;
    int condition = 0;

    for (i = 0; i < D->IKchains_Count; i ++)
    {
        if (I == D->IKchains[i])
        {
            index = i;
            condition = 1;
            break;
        }
    }

    if (condition)
    {
        D->IKchains_Count --;
        for (i = index; i < D->IKchains_Count; i ++)
        {
            D->IKchains[i] = D->IKchains[i + 1];
        }
        D->IKchains = realloc(D->IKchains, D->IKchains_Count * sizeof(ikChain*));
    }
}

void remove_IK_Chains_From_Deformer(transformer * T, deformer * D)
{
    ik_Collection_Count = 0;
    collect_IK_Chains(T);

    int i;
    for (i = ik_Collection_Count - 1; i >= 0; i --)
    {
        remove_ikChain_From_Deformer(ik_Chains_Collection[i], D);
    }
}

void delete_IK_Transformers(ikChain * I);

void remove_ikChain_From_ikChains_(ikChain * I, int no_delete)
{
    int index, i, b;

    bone * B;

    int condition = 0;

    for (i = 0; i < iksIndex; i ++)
    {
        if (I == ikChains[i])
        {
            index = i;
            condition = 1;
            break;
        }
    }

    if (condition)
    {

        // cycle and pack Deformers ikChains list
        // cycle bones and set their IK member and IK to zero

        for (b = 0; b < I->bonescount; b ++)
        {
            B = I->Bones[b];
            B->IK = NULL;
            B->IK_member = 0;
        }

        I->A->IK = NULL;
        I->A->style = 0;
        I->B->IK = NULL;
        I->B->style = 0;

        if (no_delete)
        {

        }
        else
        {
            if(I->Deformer != NULL)
            {
                //printf("DEFORMER %s\n", I->Deformer->Name);
                remove_ikChain_From_Deformer(I, I->Deformer);
            }
            delete_IK_Transformers(I);
        }

        free_ikChain(I);

        iksIndex --;
        for (i = index; i < iksIndex; i ++)
        {
            ikChains[i] = ikChains[i + 1];
            ikChains[i]->index = i;
        }
    }
}

void solve_IK_Chains(deformer * D, int update)
{
    int i;
    ikChain * I;

    for (i = 0; i < D->IKchains_Count; i ++)
    {
        I = D->IKchains[i];
        solve_IK_Chain(I, update);
    }
}

void solve_all_IK_Chains()
{
    int d;

    for (d = 0; d < deformerIndex; d ++)
    {
        solve_IK_Chains(deformers[d], 0);
    }
}

void unfix_deformer_ik_goals(deformer * D)
{
    int i;
    ikChain * I;

    for (i = 0; i < D->IKchains_Count; i ++)
    {
        I = D->IKchains[i];
        if (I->B->style == ik_fixed)
        {
            I->B->style = ik_goal;
            unfix_ik_goal(I->B);
        }
    }
}

int find_fixed_goals(deformer * D)
{
    int i;
    ikChain * I;

    int r = 0;

    for (i = 0; i < D->IKchains_Count; i ++)
    {
        I = D->IKchains[i];
        if (I->B->style == ik_fixed)
        {
            r = 1;
            break;
        }
    }

    return r;
}

void update_Deformers_Poses()
{
    int d;
    deformer * D;
    transformer * T;

    for (d = 0; d < deformerIndex; d ++)
    {
        D = deformers[d];

        if (D->Transformers_Count > 0)
        {
            T = D->Transformers[0];

            bake(T);

            rotate_(T);

            solve_IK_Chains(D, 0); // specify affected IK chains before, collect them
        }
    }
}

void transfer_Deformers_rotVec(transformer * T)
{
    if (T->Deformer != NULL)
    {
        memcpy(T->Deformer->rotVec, T->Deformer->Transformers[0]->rotVec, sizeof(float[3][3]));
    }
}

void reset_Deformer_rotation(deformer * D)
{
    memcpy(&D->rotVec, (float[3][3]) {{1.0, 0.0, 0.0}, {0.0, 1.0, 0.0}, {0.0, 0.0, 1.0}}, sizeof D->rotVec);
}

#endif // DEFORMER_H_INCLUDED
