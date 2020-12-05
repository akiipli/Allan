/*
The MIT License

Copyright <2020> <Allan Kiipli>

Subcharacters belong to deformers.
The idea is to be able animate parts of the deformer hierarchy
independent of the rest of the pose. For this puropse
subcharacters have starting transformer. This is the transformer
position in deformers transformers and matches up with subcharacters
root parent node. Subcharacters root node is its first node and the
rest is branching from it. I need to add more control to block
illegal forms of bone sets becoming a subcharacter. Another thing
is that subcharacters may not overlap in deformer and they
need to be continuous covering the entire hierarchy.
How this is implemented and verified, becomes clear in the process.

*/

#ifndef SUBCHARACTERS_H_INCLUDED
#define SUBCHARACTERS_H_INCLUDED

#define SUBCHARACTERS 200
#define SUBCHARACTER_POSES 1000

float distance(float A[3], float B[3])
{
    float C[3] = {A[0] - B[0], A[1] - B[1], A[2] - B[2]};
    return sqrt(C[0] * C[0] + C[1] * C[1] + C[2] * C[2]);
}

int start_Subcharacter;
transformer ** hi_selected_Transformers;
int hi_selected_Transformers_count;
bone ** hi_selected_Bones;
int hi_selected_Bones_count;
int Subcharacters_c = 0;
char Subcharacter_Names[SUBCHARACTERS][STRLEN];
int SubcharacterIndex = 0;
int subcharacter_start = 0;

int Subcharacter_List[SUBCHARACTERS];
int currentSubcharacter = 0;

int Subcharacter_X_Offset[SUBCHARACTERS];
int Subcharacter_X_Collapsed[SUBCHARACTERS];

pose * subcharacter_Poses[SUBCHARACTER_POSES];
int subcharacter_posesIndex = 0;
int currentSubcharacterPose = 0;

typedef struct subcharacter
{
    int index;
    unsigned address; // assigned after loading
    char * Name;
    int collapsed;
    int selected;

    deformer * Deformer;
    int start;

    transformer ** Transformers;
    int Transformers_Count;

    bone ** Bones;
    int Bones_Count;

    pose ** Poses;
    int Poses_Count;

    subcharacter ** Subcharacters;
    int Subcharacters_Count;

    int current_pose;
}
subcharacter;

void compose_Subcharacter_Pose(subcharacter * S)
{
    int b;
    bone * B;
    float len;

    for (b = 0; b < S->Bones_Count; b ++)
    {
        B = S->Bones[b];
        len = distance(B->A->pos, B->B->pos);

        B->A->pos[0] = B->A->parent->pos[0];
        B->A->pos[1] = B->A->parent->pos[1];
        B->A->pos[2] = B->A->parent->pos[2];

        if (B->A->rot_Order == yxz || B->A->rot_Order == xyz)
        {
            B->B->pos[0] = B->A->pos[0] + B->A->rotVec_[2][0] * len;
            B->B->pos[1] = B->A->pos[1] + B->A->rotVec_[2][1] * len;
            B->B->pos[2] = B->A->pos[2] + B->A->rotVec_[2][2] * len;
        }
        else if (B->A->rot_Order == zxy || B->A->rot_Order == xzy)
        {
            B->B->pos[0] = B->A->pos[0] + B->A->rotVec_[1][0] * len;
            B->B->pos[1] = B->A->pos[1] + B->A->rotVec_[1][1] * len;
            B->B->pos[2] = B->A->pos[2] + B->A->rotVec_[1][2] * len;
        }
        else if (B->A->rot_Order == zyx || B->A->rot_Order == yzx)
        {
            B->B->pos[0] = B->A->pos[0] + B->A->rotVec_[0][0] * len;
            B->B->pos[1] = B->A->pos[1] + B->A->rotVec_[0][1] * len;
            B->B->pos[2] = B->A->pos[2] + B->A->rotVec_[0][2] * len;
        }
    }
}

void apply_Subcharacter_Pose(subcharacter * S, pose * P)
{
    int t;

    if (P->transformers_count == S->Transformers_Count)
    {
        transformer * T = S->Deformer->Bones[S->start]->A->parent;

        for (t = 0; t < P->transformers_count; t ++)
        {
            S->Transformers[t]->rot_Order = P->TP[t].rot_Order;
            memcpy(S->Transformers[t]->scl, P->TP[t].scl, sizeof(float[3]));
            memcpy(S->Transformers[t]->scl_vec, P->TP[t].scl_vec, sizeof(float[3]));
            memcpy(S->Transformers[t]->rot, P->TP[t].rot, sizeof(float[3]));

            //memcpy(S->Transformers[t]->rotVec, P->TP[t].rotVec, sizeof(float[3][3]));
            //memcpy(S->Transformers[t]->rotVec_, P->TP[t].rotVec_, sizeof(float[3][3]));

            memcpy(S->Transformers[t]->rotVec_I, P->TP[t].rotVec_I, sizeof(float[3][3]));
            memcpy(S->Transformers[t]->rotVec_B, P->TP[t].rotVec_B, sizeof(float[3][3]));
            memcpy(S->Transformers[t]->pos, P->TP[t].pos, sizeof(float[3]));
            memcpy(S->Transformers[t]->pos_, P->TP[t].pos_, sizeof(float[3]));
            S->Transformers[t]->style = P->TP[t].style;

            rotate_matrix_I(S->Transformers[t]->rotVec_, T->rotVec_, P->TP[t].rotVec_);
            rotate_matrix_I(S->Transformers[t]->rotVec, T->rotVec, P->TP[t].rotVec);
        }
    }
}

int add_Subcharacter_Pose(subcharacter * S)
{
    pose * P = malloc(sizeof(pose));
    if (P == NULL)
    {
        return -1;
    }
    subcharacter_Poses[subcharacter_posesIndex] = P;
    P->index = subcharacter_posesIndex;
    P->Name = malloc(STRLEN);
    sprintf(P->Name, "S Pose %d", subcharacter_posesIndex);
    P->D = S->Deformer;
    P->transformers_count = S->Transformers_Count;
    P->TP = calloc(P->transformers_count, sizeof(transformer_pose));

    int t;

    float rotVec_I[3][3];
    invert_Rotation(S->Deformer->Bones[S->start]->A->parent, rotVec_I);

    for (t = 0; t < P->transformers_count; t ++)
    {
        P->TP[t].rot_Order = S->Transformers[t]->rot_Order;
        memcpy(P->TP[t].scl, S->Transformers[t]->scl, sizeof(float[3]));
        memcpy(P->TP[t].scl_vec, S->Transformers[t]->scl_vec, sizeof(float[3]));
        memcpy(P->TP[t].rot, S->Transformers[t]->rot, sizeof(float[3]));
//        memcpy(P->TP[t].rotVec, S->Transformers[t]->rotVec, sizeof(float[3][3]));
//        memcpy(P->TP[t].rotVec_, S->Transformers[t]->rotVec_, sizeof(float[3][3]));
        memcpy(P->TP[t].rotVec_I, S->Transformers[t]->rotVec_I, sizeof(float[3][3]));
        memcpy(P->TP[t].rotVec_B, S->Transformers[t]->rotVec_B, sizeof(float[3][3]));
        memcpy(P->TP[t].pos, S->Transformers[t]->pos, sizeof(float[3]));
        memcpy(P->TP[t].pos_, S->Transformers[t]->pos_, sizeof(float[3]));
        P->TP[t].style = S->Transformers[t]->style;

        rotate_matrix_I(P->TP[t].rotVec_, rotVec_I, S->Transformers[t]->rotVec_);
        rotate_matrix_I(P->TP[t].rotVec, rotVec_I, S->Transformers[t]->rotVec);
    }

    /* multiply with S->Deformer->Transformers[S->start]->parent Inverse Matrix */


    S->Poses = realloc(S->Poses, (S->Poses_Count + 1) * sizeof(pose *));
    S->Poses[S->Poses_Count ++] = P;
    subcharacter_posesIndex ++;

    return subcharacter_posesIndex;
}

subcharacter * subcharacters[SUBCHARACTERS];
int subcharacterIndex = 0;

void list_Subcharacter_Poses(subcharacter * S)
{
    int p;

    pose * P;

    for (p = 0; p < S->Poses_Count; p ++)
    {
        P = S->Poses[p];

//        if (currentSubcharacter > 0 && S == subcharacters[currentSubcharacter])
//            D->current_pose = p;

        sprintf(Subcharacter_Names[Subcharacters_c], "%s", P->Name);
        Subcharacter_List[Subcharacters_c] = SUBCHARACTERS + P->index;

        Subcharacter_X_Offset[Subcharacters_c] = 3;
        Subcharacter_X_Collapsed[Subcharacters_c] = 0;

        Subcharacters_c ++;

        if (Subcharacters_c >= SUBCHARACTERS)
        {
            break;
        }
    }
}

void list_Subcharacter_Nodes(deformer * D, int currentSubcharacter)
{
    int s;
    subcharacter * S;

    for (s = 0; s < D->Subcharacters_Count; s ++)
    {
        S = D->Subcharacters[s];

        if (S->selected)
            selected_subcharacter_node = Subcharacters_c;

        sprintf(Subcharacter_Names[Subcharacters_c], "%s", S->Name);
        Subcharacter_List[Subcharacters_c] = S->index;

        Subcharacter_X_Offset[Subcharacters_c] = 2;
        Subcharacter_X_Collapsed[Subcharacters_c] = S->collapsed;

        Subcharacters_c ++;

        if (Subcharacters_c >= SUBCHARACTERS)
        {
            break;
        }

        if (!S->collapsed)
        {
            list_Subcharacter_Poses(S);
        }
    }
}

void create_Subcharacters_List(int SubcIndex)
{
    int d;
    deformer * D;
    Subcharacters_c = 0;
    Deformer_Node_c = 1;

    int currentSubcharacter = Subcharacter_List[SubcIndex];

    for (d = 0; d < deformerIndex; d ++)
    {
        D = deformers[d];

        if (D->selected)
            selected_deformer_node = Subcharacters_c;

        memcpy(Subcharacter_Names[Subcharacters_c], D->Name, strlen(D->Name));
        Subcharacter_Names[Subcharacters_c][strlen(D->Name)] = '\0';
        Subcharacter_List[Subcharacters_c] = -Deformer_Node_c;
        Subcharacter_X_Offset[Subcharacters_c] = 1;
        Subcharacter_X_Collapsed[Subcharacters_c] = D->collapsed;
        Subcharacters_c ++;
        Deformer_Node_c ++;

        if (Subcharacters_c >= SUBCHARACTERS)
        {
            break;
        }

        if (!D->collapsed)
        {
            list_Subcharacter_Nodes(D, currentSubcharacter);
        }
    }
}
/*
void create_Subcharacters_List(int SubcIndex)
{
    int s;
    subcharacter * S;
    Subcharacters_c = 0;

    for (s = 0; s < subcharacterIndex; s ++)
    {
        S = subcharacters[s];

        memcpy(Subcharacter_Names[Subcharacters_c], S->Name, strlen(S->Name));
        Subcharacter_Names[Subcharacters_c][strlen(S->Name)] = '\0';
        Subcharacter_List[Subcharacters_c] = Subcharacters_c;
        Subcharacters_c ++;
        if (Subcharacters_c >= SUBCHARACTERS - 1)
            break;
    }
}
*/
void replace_Subcharacter_Name(char * EditString)
{
    if (Subcharacter_List[SubcharacterIndex] >= 0 && Subcharacter_List[SubcharacterIndex] < SUBCHARACTERS)
    {
        subcharacter * S = subcharacters[Subcharacter_List[SubcharacterIndex]];
        memcpy(S->Name, EditString, strlen(EditString));
        S->Name[strlen(EditString)] = '\0';
    }
    else if (Subcharacter_List[SubcharacterIndex] >= SUBCHARACTERS)
    {
        pose * P = subcharacter_Poses[Subcharacter_List[SubcharacterIndex] - SUBCHARACTERS];
        memcpy(P->Name, EditString, strlen(EditString));
        P->Name[strlen(EditString)] = '\0';
    }
    else
    {
        deformer * D = deformers[-(Subcharacter_List[SubcharacterIndex] + 1)];
        memcpy(D->Name, EditString, strlen(EditString));
        D->Name[strlen(EditString)] = '\0';
    }
}

void create_Hi_Selected_Bones_List(deformer * D)
{
    start_Subcharacter = -1;
    hi_selected_Transformers = malloc(sizeof(bone *) * D->Transformers_Count);
    hi_selected_Transformers_count = 0;
    hi_selected_Bones = malloc(sizeof(bone *) * D->Bones_Count);
    hi_selected_Bones_count = 0;

    int b;
    bone * B;

    for (b = 0; b < D->Bones_Count; b ++)
    {
        B = D->Bones[b];
        if (B->selected)
        {
            if (start_Subcharacter == -1)
                start_Subcharacter = b;
            hi_selected_Bones[hi_selected_Bones_count ++] = B;
            hi_selected_Transformers[hi_selected_Transformers_count ++] = B->A;
            hi_selected_Transformers[hi_selected_Transformers_count ++] = B->B;
        }
    }
    printf("hi selected Bones count %d\n", hi_selected_Bones_count);
}

int init_Subcharacter(deformer * D)
{
    subcharacter * S = malloc(sizeof(subcharacter));
    if (S == NULL)
        return -1;
    subcharacters[subcharacterIndex] = S;
    S->index = subcharacterIndex;
    S->Name = malloc(STRLEN * sizeof(char));
    sprintf(S->Name, "%s %d", "Subcharacter", subcharacterIndex);
    S->collapsed = 0;
    S->selected = 0;

    S->Transformers = malloc(sizeof(transformer *) * hi_selected_Transformers_count);

    int t;
    for (t = 0; t < hi_selected_Transformers_count; t ++)
    {
        S->Transformers[t] = hi_selected_Transformers[t];
    }
    S->Transformers_Count = hi_selected_Transformers_count;

    S->Bones = malloc(sizeof(bone *) * hi_selected_Bones_count);

    int b;
    for (b = 0; b < hi_selected_Bones_count; b ++)
    {
        S->Bones[b] = hi_selected_Bones[b];
    }
    S->Bones_Count = hi_selected_Bones_count;

    S->Deformer = D;
    D->Subcharacters_Count ++;
    D->Subcharacters = realloc(D->Subcharacters, D->Subcharacters_Count * sizeof(subcharacter *));
    D->Subcharacters[D->Subcharacters_Count - 1] = S;
    S->start = start_Subcharacter;

    S->Poses_Count = 0;
    S->Poses = malloc(sizeof(pose *) * S->Poses_Count);

    S->current_pose = -1;

    S->Subcharacters_Count = 0;
    S->Subcharacters = malloc(sizeof(subcharacter *) * S->Subcharacters_Count);

    subcharacterIndex ++;
    return S->index;
}

void set_Subc_H_Button(int index)
{
    int i;
    for (i = 0; i < H_DEFR_NUM; i ++)
    {
        Button_h_subc[i].color = UI_GRAYB;
    }
    if (index > -1)
        Button_h_subc[index].color = UI_GRAYD;
}

void free_subcharacter(subcharacter * S);
void free_subcharacter_Poses(subcharacter * S);

void remove_Subcharacter_From_Subcharacter(subcharacter * S, subcharacter * S0)
{
    int s, index;

    int condition = 0;

    for (s = 0; s < S->Subcharacters_Count; s ++)
    {
        if (S0 == S->Subcharacters[s])
        {
            index = s;
            condition = 1;
            break;
        }
    }

    if (condition)
    {
        S->Subcharacters_Count --;

        for (s = index; s < S->Subcharacters_Count; s ++)
        {
            S->Subcharacters[s] = S->Subcharacters[s + 1];
        }
    }
}

void remove_Subcharacter_From_Deformer_Subcharacters(deformer * D, subcharacter * S)
{
    int s;
    subcharacter * S0;

    for (s = 0; s < D->Subcharacters_Count; s ++)
    {
        S0 = D->Subcharacters[s];

        remove_Subcharacter_From_Subcharacter(S, S0);
    }
}

void remove_Subcharacter_From_Deformer(deformer * D, subcharacter * S)
{
    int s, index;

    int condition = 0;

    for (s = 0; s < D->Subcharacters_Count; s ++)
    {
        if (S == D->Subcharacters[s])
        {
            index = s;
            condition = 1;
            break;
        }
    }

    if (condition)
    {
        D->Subcharacters_Count --;

        for (s = index; s < D->Subcharacters_Count; s ++)
        {
            D->Subcharacters[s] = D->Subcharacters[s + 1];
        }
    }
}

int delete_Subcharacter(subcharacter * S)
{
    int index, s;
    int condition = 0;

    for (s = 0; s < subcharacterIndex; s ++)
    {
        if (S == subcharacters[s])
        {
            index = s;
            condition = 1;
            break;
        }
    }

    if (condition)
    {
        deformer * D = S->Deformer;

        remove_Subcharacter_From_Deformer(D, S);

        remove_Subcharacter_From_Deformer_Subcharacters(D, S);

        free_subcharacter_Poses(S);

        free_subcharacter(S);

        subcharacterIndex --;
        for (s = index; s < subcharacterIndex; s ++)
        {
            subcharacters[s] = subcharacters[s + 1];
            subcharacters[s]->index = s;
        }
    }
    return condition;
}

void delete_Deformer_Subcharacters(deformer * D)
{
    int s, i, index;
    subcharacter * S;

    for (s = 0; s < D->Subcharacters_Count; s ++)
    {
        S = D->Subcharacters[s];
        index = S->index;

        remove_Subcharacter_From_Deformer(D, S);

        free_subcharacter_Poses(S);

        free_subcharacter(S);

        subcharacterIndex --;
        for (i = index; i < subcharacterIndex; i ++)
        {
            subcharacters[i] = subcharacters[i + 1];
            subcharacters[i]->index = i;
        }
    }
}

#endif // SUBCHARACTERS_H_INCLUDED
