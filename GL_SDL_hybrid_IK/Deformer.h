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

typedef struct deformer_morph_map deformer_morph_map;
typedef struct subcharacter subcharacter;
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

    subcharacter ** Subcharacters;
    int Subcharacters_Count;

    float rotVec[3][3];
    float rot[3];

    int play;
    pose * P;
    int linear_pose;

    deformer_morph_map ** Morph_Maps;
    int Morph_Maps_Count;
};

void insert_Deformer_keyframe(deformer * D, int frame, int Indi)
{
    transformer * T;
    int t;

    for (t = 0; t < D->Transformers_Count; t ++)
    {
        T = D->Transformers[t];

        if (!Indi || (Indi && T->selected))
        {
            insert_keyframe(T, frame, D->linear_pose, D->Delta);
        }
    }
}

int insert_morf_keyframe(object * O, int frame);

void insert_Deformer_morf_keyframe(deformer * D, int frame)
{
    object * O;
    int o;

    for (o = 0; o < D->Objects_Count; o ++)
    {
        O = D->Objects[o];
        insert_morf_keyframe(O, frame);
    }
}

void delete_Deformer_keyframe(deformer * D, int frame, int Indi)
{
    transformer * T;
    int t;

    for (t = 0; t < D->Transformers_Count; t ++)
    {
        T = D->Transformers[t];

        if (!Indi || (Indi && T->selected))
        {
            delete_keyframe(T, frame);
        }
    }
}

int delete_morf_keyframe(object * O, int frame);

void delete_Deformer_morf_keyframe(deformer * D, int frame)
{
    object * O;
    int o;

    for (o = 0; o < D->Objects_Count; o ++)
    {
        O = D->Objects[o];
        delete_morf_keyframe(O, frame);
    }
}

void free_Deformer(deformer * D)
{
    free(D->Name);
    free(D->Transformers);
    free(D->Selections);
    free(D->Objects);
    free(D->Bones);
    free(D->Poses);
    free(D->IKchains);
    free(D->Subcharacters);
    free(D->Morph_Maps);
    //free(D->P);
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

void free_deformer_Morph_Maps(deformer * D);

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

    free_deformer_Morph_Maps(D);

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
    D->Subcharacters_Count = 0;
    D->Transformers = malloc(0 * sizeof(transformer*));
    D->Selections = malloc(0 * sizeof(vert_selection*));
    D->Objects = malloc(0 * sizeof(object*));
    D->Bones = malloc(0 * sizeof(bone*));
    D->Poses = malloc(0 * sizeof(pose*));
    D->IKchains = malloc(0 * sizeof(ikChain*));
    D->Subcharacters = malloc(0 * sizeof(subcharacter*));
    memcpy(D->Delta, (float[3]){0, 0, 0}, sizeof D->Delta);
    D->current_pose = 0;
    memcpy(&D->rotVec, (float[3][3]) {{1.0, 0.0, 0.0}, {0.0, 1.0, 0.0}, {0.0, 0.0, 1.0}}, sizeof D->rotVec);
    memcpy(D->rot, (float[3]){0, 0, 0}, sizeof D->rot);
    D->play = -1;
    D->P = NULL;
    deformerIndex ++;
    D->Morph_Maps = malloc(0 * sizeof(deformer_morph_map*));
    D->Morph_Maps_Count = 0;
    D->linear_pose = 0;
}

void list_Deformer_Nodes(deformer * D, int SelsIndex, object * O)
{
    int t, s;
    transformer * T;
    vert_selection * S;

    for (t = 0; t < D->Transformers_Count; t ++)
    {
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

        if (Deformers_c >= DEFORMERS)
        {
            break;
        }
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

        if (Deformers_c >= DEFORMERS)
        {
            break;
        }

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
        I->Deformer = NULL;
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

void solve_IK_Chains(deformer * D)
{
    int i;
    ikChain * I;

    for (i = 0; i < D->IKchains_Count; i ++)
    {
        I = D->IKchains[i];
        solve_IK_Chain(I);
    }
}

void solve_all_IK_Chains()
{
    int d;

    for (d = 0; d < deformerIndex; d ++)
    {
        solve_IK_Chains(deformers[d]);
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

            solve_IK_Chains(D); // specify affected IK chains before, collect them
        }
    }
}

void transfer_Transformers_rotVec(deformer * D)
{
    if (D != NULL && D->Transformers_Count > 0)
    {
        memcpy(D->Transformers[0]->rotVec, D->rotVec, sizeof(float[3][3]));
        memcpy(D->Transformers[0]->rot, D->rot, sizeof(float[3]));
    }
}

void transfer_Deformers_rotVec(transformer * T)
{
    if (T->Deformer != NULL)
    {
        memcpy(T->Deformer->rotVec, T->Deformer->Transformers[0]->rotVec, sizeof(float[3][3]));
        memcpy(T->Deformer->rot, T->Deformer->Transformers[0]->rot, sizeof(float[3]));
    }
}

void reset_Deformer_rotation(deformer * D)
{
    memcpy(&D->rotVec, (float[3][3]) {{1.0, 0.0, 0.0}, {0.0, 1.0, 0.0}, {0.0, 0.0, 1.0}}, sizeof D->rotVec);
}

void order_IK_Chains(ikChain ** IKList, int l[1], ikChain * currentIK, transformer * T)
{
    int c;
    transformer * C;

    if (currentIK != NULL)
    {
        IKList[l[0]] = currentIK;
        //printf("currentIK %s\n", IKList[l[0]]->Name);
        l[0] ++;
    }

    if (T->Bone != NULL && T->Bone->IK != NULL && T->Bone->IK != currentIK)
    {
        currentIK = T->Bone->IK;
    }

    for (c = 0; c < T->childcount; c ++)
    {
        C = T->childs[c];
        order_IK_Chains(IKList, l, currentIK, C);
    }
}

ikChain * IKList[IKCHAINS];

void order_deformers_IK_Chains(deformer * D)
{
    int c;
    transformer * T = D->Transformers[0];
    transformer * C;

    int l[1] = {0};
    int counter = 0;
    int i, d, condition;

    if (IKList != NULL)
    {
        ikChain * currentIK = NULL;

        if (T->Bone != NULL && T->Bone->IK != NULL)
        {
            IKList[l[0] ++] = T->Bone->IK;
            currentIK = T->Bone->IK;
        }

        for (c = 0; c < T->childcount; c ++)
        {
            C = T->childs[c];
            order_IK_Chains(IKList, l, currentIK, C);
        }

        D->IKchains[counter ++] = IKList[0];

        for (i = 1; i < l[0]; i ++)
        {
            if (counter >= D->IKchains_Count)
                break;

            condition = 1;
            for (d = 0; d < counter; d ++)
            {
                if (IKList[i] == D->IKchains[d])
                {
                    condition = 0;
                    break;
                }
            }
            if (condition)
                D->IKchains[counter ++] = IKList[i];
        }

        //free(IKList);
    }
}

void hierarchycal_IK_Chains()
{
    int d;
    deformer * D;

    for (d = 0; d < deformerIndex; d ++)
    {
        D = deformers[d];

        if (D->IKchains_Count > 0)
        {
            order_deformers_IK_Chains(D);
        }
    }
}

void populate_IK_Updates()
{
    int d;
    deformer * D;
    transformer * T;

    for (d = 0; d < deformerIndex; d ++)
    {
        D = deformers[d];

        if (D->Transformers_Count > 0 && D->IKchains_Count > 0)
        {
            T = D->Transformers[0];
            populate_IK_Update(D, NULL, T);
        }
    }
}

void normalize_IK_Spine(ikChain * I)
{
    normalize_rotation_parent_IK(I->A);
}

void normalize_IK_Spines(deformer * D)
{
    int i;
    ikChain * I;

    for (i = 0; i < D->IKchains_Count; i ++)
    {
        I = D->IKchains[i];
        normalize_IK_Spine(I);
    }
}

void normalize_all_IK_Spines()
{
    int i;
    ikChain * I;

    for (i = 0; i < iksIndex; i ++)
    {
        I = ikChains[i];
        normalize_IK_Spine(I);
    }
}

void transformer_pin_Preparation(deformer * D)
{
    int t;
    transformer * T;

    for (t = 0; t < D->Transformers_Count; t ++)
    {
        T = D->Transformers[t];
        memcpy(&T->rotVec_Pin, T->rotVec_, sizeof T->rotVec_Pin);
    }
}

void print_Deformer_IK_Chains(deformer * D)
{
    int i;
    ikChain * I;

    for (i = 0; i < D->IKchains_Count; i ++)
    {
        I = D->IKchains[i];
        printf("Deformer %s %d %s\n", D->Name, i, I->Name);
    }
}

void remember_Deformer_Object_Curves_pos(deformer * D)
{
    int o;

    object * O;

    for (o = 0; o < D->Objects_Count; o ++)
    {
        O = D->Objects[o];

        if (O->curve_count > 0)
        {
            remember_Object_Curves_pos(O);
        }
    }
}

void snap_back_Deformer_Object_Cps_To_Pos(deformer * D)
{
    int o;

    object * O;

    for (o = 0; o < D->Objects_Count; o ++)
    {
        O = D->Objects[o];

        if (O->curve_count > 0)
        {
            snap_back_Object_Cps_To_Pos(O);
        }
    }
}

void update_Deformer_Objects_Curves_To_Original_Coordinates()
{
    int d, o;

    object * O;
    deformer * D;

    for (d = 0; d < deformerIndex; d ++)
    {
        D = deformers[d];

        for (o = 0; o < D->Objects_Count; o ++)
        {
            O = D->Objects[o];

            if (O->curve_count > 0)
            {
                update_Objects_Curves_To_Original_Coordinates(O);
            }
        }
    }
}

void update_Deformer_Objects_Curves_To_Coordinates()
{
    int d, o;

    object * O;
    deformer * D;

    for (d = 0; d < deformerIndex; d ++)
    {
        D = deformers[d];

        for (o = 0; o < D->Objects_Count; o ++)
        {
            O = D->Objects[o];

            if (O->curve_count > 0)
            {
                update_Objects_Curves_To_Original_Coordinates(O);
            }
        }
    }
}

void update_Deformer_Objects_Curves_Coordinates(deformer * D)
{
    int o;

    object * O;

    for (o = 0; o < D->Objects_Count; o ++)
    {
        O = D->Objects[o];

        if (O->curve_count > 0)
        {
            update_Objects_Curves_Coordinates(O);
        }
    }
}

void update_Deformer_object_Curves(deformer * D, int level)
{
    int o;

    object * O;

    for (o = 0; o < D->Objects_Count; o ++)
    {
        O = D->Objects[o];

        if (O->curve_count > 0)
        {
            update_object_Curves(O, level);
        }
    }
}

int find_currentKey(transformer * T, deformer * D, int frame, int Indi)
{
    int f;

    transformer * T0;
    timeline * Tm;

    if (T != NULL && Indi)
    {
        T0 = T;
    }
    else if (D != NULL && D->Transformers_Count > 0)
    {
        T0 = D->Transformers[0];
    }
    else
    {
        T0 = T;
    }

    if (T0->Timeline != NULL)
    {
        Tm = T0->Timeline;

        for (f = 0; f < Tm->key_frames; f ++)
        {
            if (frame >= Tm->Frames[f] && frame < Tm->Frames[(f + 1) % Tm->key_frames]) //(frame == Tm->Frames[f])
            {
                return f;
            }
        }
        return -1;
    }
    else
    {
        return -1;
    }
}

int find_current_Morph_Key(deformer * D, int frame)
{
    int f, o, condition = 0;

    object * O;
    morph_timeline * Tmm;

    if (D->Objects_Count > 0)
    {
        for (o = 0; o < D->Objects_Count; o ++)
        {
            O = D->Objects[o];

            if (O->Morph_Timeline != NULL)
            {
                Tmm = O->Morph_Timeline;

                for (f = 0; f < Tmm->key_frames; f ++)
                {
                    if (frame >= Tmm->Frames[f] && frame < Tmm->Frames[(f + 1) % Tmm->key_frames]) //(frame == Tmm->Frames[f])
                    {
                        condition = 1;
                        break;
                    }
                }
            }
            else
            {
                return -1;
            }
        }

        if (condition)
        {
            return f;
        }
        else
        {
            return -1;
        }
    }
    else
    {
        return -1;
    }
}

int change_Keyframe_Frame(deformer * D, int f, int frame, int change, int Indi)
{
    int t;

    transformer * T;
    timeline * Tm;

    int result = 0;

    for (t = 0; t < D->Transformers_Count; t ++)
    {
        T = D->Transformers[t];

        if (!Indi || (Indi && T->selected))
        {
            if (T->Timeline != NULL)
            {
                Tm = T->Timeline;

                if (f < Tm->key_frames)
                {
                    if (Tm->Frames[f] == frame)
                    {
                        if (change < 0 && f > 0 && Tm->Frames[f - 1] < frame + change)
                        {
                            Tm->Frames[f] += change;
                            result = 1;
                        }
                        else if (change > 0 && f < Tm->key_frames - 1 && Tm->Frames[f + 1] > frame + change)
                        {
                            Tm->Frames[f] += change;
                            result = 1;
                        }
                        else if (change < 0 && f == 0 && Tm->Frames[f] + change >= 0)
                        {
                            Tm->Frames[f] += change;
                            result = 1;
                        }
                        else if (change > 0 && f == Tm->key_frames - 1 && Tm->Frames[f] + change < TimelineEnd)
                        {
                            Tm->Frames[f] += change;
                            result = 1;
                        }
                    }
                }
            }
        }
    }

    return result;
}

int change_Morph_Keyframe_Frame(deformer * D, int f, int frame, int change)
{
    int o;

    object * O;
    morph_timeline * Tmm;

    int result = 0;

    for (o = 0; o < D->Objects_Count; o ++)
    {
        O = D->Objects[o];

        if (O->Morph_Timeline != NULL)
        {
            Tmm = O->Morph_Timeline;

            if (f < Tmm->key_frames)
            {
                if (frame >= Tmm->Frames[f] && frame < Tmm->Frames[(f + 1) % Tmm->key_frames]) //(Tmm->Frames[f] == frame)
                {
                    if (change < 0 && f > 0 && Tmm->Frames[f - 1] < frame + change)
                    {
                        Tmm->Frames[f] += change;
                        result = 1;
                    }
                    else if (change > 0 && f < Tmm->key_frames - 1 && Tmm->Frames[f + 1] > frame + change)
                    {
                        Tmm->Frames[f] += change;
                        result = 1;
                    }
                    else if (change < 0 && f == 0 && Tmm->Frames[f] + change >= 0)
                    {
                        Tmm->Frames[f] += change;
                        result = 1;
                    }
                    else if (change > 0 && f == Tmm->key_frames - 1 && Tmm->Frames[f] + change < TimelineEnd)
                    {
                        Tmm->Frames[f] += change;
                        result = 1;
                    }
                }
            }
        }
    }

    return result;
}

void change_Key_AB_Exponent(deformer * D, int f, int frame, int AB, int change, int Indi)
{
    int t;

    transformer * T;
    timeline * Tm;

    for (t = 0; t < D->Transformers_Count; t ++)
    {
        T = D->Transformers[t];

        if (!Indi || (Indi && T->selected))
        {
            if (T->Timeline != NULL)
            {
                Tm = T->Timeline;

                if (f < Tm->key_frames)
                {
                    if (frame >= Tm->Frames[f] && frame < Tm->Frames[(f + 1) % Tm->key_frames]) //(Tm->Frames[f] == frame)
                    {
                        if (AB == 0) // A
                        {
                            if (change == 1)
                                Tm->Acceleration[f].a_exponent += EXPONENT_CHANGE;
                            else
                                Tm->Acceleration[f].a_exponent -= EXPONENT_CHANGE;
                            if (Tm->Acceleration[f].a_exponent > EXPONENT_MAX)
                            {
                                Tm->Acceleration[f].a_exponent = EXPONENT_MAX;
                            }
                            if (Tm->Acceleration[f].a_exponent < 1)
                            {
                                Tm->Acceleration[f].a_exponent = 1.0;
                            }
                            if (t == 0)
                            {
                                printf("ACCELERATION A EXPONENT %f\n", Tm->Acceleration[f].a_exponent);
                            }
                        }
                        else if (AB == 1) // B
                        {
                            if (change == 1)
                                Tm->Acceleration[f].b_exponent += EXPONENT_CHANGE;
                            else
                                Tm->Acceleration[f].b_exponent -= EXPONENT_CHANGE;
                            if (Tm->Acceleration[f].b_exponent > EXPONENT_MAX)
                            {
                                Tm->Acceleration[f].b_exponent = EXPONENT_MAX;
                            }
                            if (Tm->Acceleration[f].b_exponent < 1)
                            {
                                Tm->Acceleration[f].b_exponent = 1.0;
                            }
                            if (t == 0)
                            {
                                printf("ACCELERATION B EXPONENT %f\n", Tm->Acceleration[f].b_exponent);
                            }
                        }
                    }
                }
            }
        }
    }
}

void change_Morph_Key_AB_Exponent(deformer * D, int f, int frame, int AB, int change)
{
    int o;

    object * O;
    morph_timeline * Tmm;

    for (o = 0; o < D->Objects_Count; o ++)
    {
        O = D->Objects[o];

        if (O->Morph_Timeline != NULL)
        {
            Tmm = O->Morph_Timeline;

            if (f < Tmm->key_frames)
            {
                if (frame >= Tmm->Frames[f] && frame < Tmm->Frames[(f + 1) % Tmm->key_frames]) //(Tmm->Frames[f] == frame)
                {
                    if (AB == 0) // A
                    {
                        if (change == 1)
                            Tmm->Acceleration[f].a_exponent += EXPONENT_CHANGE;
                        else
                            Tmm->Acceleration[f].a_exponent -= EXPONENT_CHANGE;
                        if (Tmm->Acceleration[f].a_exponent > EXPONENT_MAX)
                        {
                            Tmm->Acceleration[f].a_exponent = EXPONENT_MAX;
                        }
                        if (Tmm->Acceleration[f].a_exponent < 1)
                        {
                            Tmm->Acceleration[f].a_exponent = 1.0;
                        }
                        if (o == 0)
                        {
                            printf("ACCELERATION A EXPONENT %f\n", Tmm->Acceleration[f].a_exponent);
                        }
                    }
                    else if (AB == 1) // B
                    {
                        if (change == 1)
                            Tmm->Acceleration[f].b_exponent += EXPONENT_CHANGE;
                        else
                            Tmm->Acceleration[f].b_exponent -= EXPONENT_CHANGE;
                        if (Tmm->Acceleration[f].b_exponent > EXPONENT_MAX)
                        {
                            Tmm->Acceleration[f].b_exponent = EXPONENT_MAX;
                        }
                        if (Tmm->Acceleration[f].b_exponent < 1)
                        {
                            Tmm->Acceleration[f].b_exponent = 1.0;
                        }
                        if (o == 0)
                        {
                            printf("ACCELERATION B EXPONENT %f\n", Tmm->Acceleration[f].b_exponent);
                        }
                    }
                }
            }
        }
    }
}

void change_Key_Acceleration(deformer * D, int f, int frame, int change, int Indi)
{
    int t;

    transformer * T;
    timeline * Tm;

    for (t = 0; t < D->Transformers_Count; t ++)
    {
        T = D->Transformers[t];

        if (!Indi || (Indi && T->selected))
        {
            if (T->Timeline != NULL)
            {
                Tm = T->Timeline;

                if (f < Tm->key_frames)
                {
                    if (frame >= Tm->Frames[f] && frame < Tm->Frames[(f + 1) % Tm->key_frames]) //(Tm->Frames[f] == frame)
                    {
                        if (change > 0)
                            Tm->Acceleration[f].segment_type ++;
                        else
                            Tm->Acceleration[f].segment_type --;
                        if (Tm->Acceleration[f].segment_type > 3)
                        {
                            Tm->Acceleration[f].segment_type = 0;
                        }
                        if (Tm->Acceleration[f].segment_type < 0)
                        {
                            Tm->Acceleration[f].segment_type = 3;
                        }
                        if (t == 0)
                        {
                            ACCELERATION_DEFAULT = Tm->Acceleration[f].segment_type;
                            printf("ACCELERATION DEFAULT %s\n", Acceleration_Names[Tm->Acceleration[f].segment_type]);
                        }
                    }
                }
            }
        }
    }
}

void change_Morph_Key_Acceleration(deformer * D, int f, int frame, int change)
{
    int o;

    object * O;
    morph_timeline * Tmm;

    for (o = 0; o < D->Objects_Count; o ++)
    {
        O = D->Objects[o];

        if (O->Morph_Timeline != NULL)
        {
            Tmm = O->Morph_Timeline;

            if (f < Tmm->key_frames)
            {
                if (frame >= Tmm->Frames[f] && frame < Tmm->Frames[(f + 1) % Tmm->key_frames]) //(Tmm->Frames[f] == frame)
                {
                    if (change > 0)
                        Tmm->Acceleration[f].segment_type ++;
                    else
                        Tmm->Acceleration[f].segment_type --;
                    if (Tmm->Acceleration[f].segment_type > 3)
                    {
                        Tmm->Acceleration[f].segment_type = 0;
                    }
                    if (Tmm->Acceleration[f].segment_type < 0)
                    {
                        Tmm->Acceleration[f].segment_type = 3;
                    }
                    if (o == 0)
                    {
                        ACCELERATION_DEFAULT = Tmm->Acceleration[f].segment_type;
                        printf("ACCELERATION DEFAULT %s\n", Acceleration_Names[Tmm->Acceleration[f].segment_type]);
                    }
                }
            }
        }
    }
}

void load_Deformers_Original_Coordinates()
{
    int d, o, v;

    deformer * D;
    object * O;
    vertex * V;

    for (d = 0; d < deformerIndex; d ++)
    {
        D = deformers[d];
        for (o = 0; o < D->Objects_Count; o ++)
        {
            O = D->Objects[o];
            for (v = 0; v < O->vertcount; v ++)
            {
                V = &O->verts[v / ARRAYSIZE][v % ARRAYSIZE];
                V->Rx = V->x;
                V->Ry = V->y;
                V->Rz = V->z;
            }
        }
    }
}

void load_Deformer_Original_Coordinates(deformer * D)
{
    int o, v;

    object * O;
    vertex * V;

    for (o = 0; o < D->Objects_Count; o ++)
    {
        O = D->Objects[o];
        for (v = 0; v < O->vertcount; v ++)
        {
            V = &O->verts[v / ARRAYSIZE][v % ARRAYSIZE];
            V->Rx = V->x;
            V->Ry = V->y;
            V->Rz = V->z;
        }
    }
}

void replace_Defr_Name(char * EditString, deformer * D)
{
    memcpy(D->Name, EditString, strlen(EditString));
    D->Name[strlen(EditString)] = '\0';
}

deformer * Deformer_0 = NULL;

void scan_For_Deformer_Down(transformer * T)
{
    int c;

    transformer * C;

    if (T->Deformer != NULL)
    {
        Deformer_0 = T->Deformer;
    }
    else
    {
        for (c = 0; c < T->childcount; c ++)
        {
            C = T->childs[c];
            scan_For_Deformer_Down(C);
        }
    }
}

int assumption_about_Deformer_T(transformer * T)
{
    int a_msg = 0;
    int o;

    object * O;

    for (o = 0; o < selected_object_count; o ++)
    {
        O = objects[selected_objects[o]];

        if (O->binding && T->Deformer == NULL)
        {
            a_msg = 1;
        }
    }
    return a_msg;
}


#endif // DEFORMER_H_INCLUDED
