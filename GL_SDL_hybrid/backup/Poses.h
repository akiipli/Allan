#ifndef POSES_H_INCLUDED
#define POSES_H_INCLUDED

#include "Deformer.h"

#define POSES 100

int Poses_c;
int Poses_Node_c;
char Pose_Names[POSES][STRLEN];
int PoseIndex;
int pose_start;

int Pose_List[POSES];
int currentPose = 0;

int Pose_X_Offset[POSES];
int Pose_X_Collapsed[POSES];

int BIND_POSE = 0;

typedef struct
{
    int rot_Order;
    float scl[3];
    float scl_vec[3];
    float rot[3];
    float rotVec[3][3];
    float rotVec_[3][3];
    float rotVec_I[3][3];
    float rotVec_B[3][3];
    float pos[3];
    float pos_[3];
}
transformer_pose;

struct pose
{
    int index;
    char * Name;
    deformer * D;
    transformer_pose * TP;
    int transformers_count;
};

void free_Pose(pose * P)
{
    free(P->Name);
    free(P->TP);
    free(P);
}

pose * poses[POSES];
int posesIndex = 0;
int posesCount = 0;

void init_poses()
{
    Pose_X_Offset[0] = 0;
    Pose_X_Collapsed[0] = 0;

    PoseIndex = 0;
    pose_start = 0;

    int p;

    for (p = 0; p < POSES; p ++)
    {
        poses[p] = NULL;
    }
}

void paste_Pose_rotation(deformer * D, pose * P)
{
    int t;

    transformer * T;

    for (t = 0; t < P->transformers_count; t ++)
    {
        if (t >= D->Transformers_Count)
            break;
        T = D->Transformers[t];
        T->rot_Order = P->TP[t].rot_Order;
        memcpy(T->scl, P->TP[t].scl, sizeof(T->scl));
        memcpy(T->scl_vec, P->TP[t].scl_vec, sizeof(T->scl_vec));
        memcpy(T->rot, P->TP[t].rot, sizeof(T->rot));
//        memcpy(T->rotVec, P->TP[t].rotVec, sizeof(T->rotVec));
//        memcpy(T->rotVec_, P->TP[t].rotVec_, sizeof(T->rotVec_));
//        memcpy(T->rotVec_I, P->TP[t].rotVec_I, sizeof(T->rotVec_I));
//        memcpy(T->rotVec_B, P->TP[t].rotVec_B, sizeof(T->rotVec_B));
//        memcpy(T->pos, P->TP[t].pos, sizeof(T->pos));
//        memcpy(T->pos_, P->TP[t].pos_, sizeof(T->pos_));
    }
}

void paste_Pose_(deformer * D, pose * P)
{
    int t;

    transformer * T;

    for (t = 0; t < P->transformers_count; t ++)
    {
        if (t >= D->Transformers_Count)
            break;
        T = D->Transformers[t];
        T->rot_Order = P->TP[t].rot_Order;
        memcpy(T->scl, P->TP[t].scl, sizeof(T->scl));
        memcpy(T->scl_vec, P->TP[t].scl_vec, sizeof(T->scl_vec));
        memcpy(T->rot, P->TP[t].rot, sizeof(T->rot));
        memcpy(T->rotVec, P->TP[t].rotVec, sizeof(T->rotVec));
        memcpy(T->rotVec_, P->TP[t].rotVec_, sizeof(T->rotVec_));
        memcpy(T->rotVec_I, P->TP[t].rotVec_I, sizeof(T->rotVec_I));
        memcpy(T->rotVec_B, P->TP[t].rotVec_B, sizeof(T->rotVec_B));
        memcpy(T->pos, P->TP[t].pos, sizeof(T->pos));
        memcpy(T->pos_, P->TP[t].pos_, sizeof(T->pos_));
    }
}

void remove_Pose_From_Deformer(pose * P)
{
    deformer * D = P->D;
    int index, p;

    int condition = 0;

    for (p = 0; p < D->Poses_Count; p ++)
    {
        if (D->Poses[p] == P)
        {
            index = p;
            condition = 1;
            break;
        }
    }

    if (condition)
    {
        D->Poses_Count --;
        for (p = index; p < D->Poses_Count; p ++)
        {
            D->Poses[p] = D->Poses[p + 1];
        }

        posesIndex --;

        for (p = P->index; p < posesIndex; p ++)
        {
            poses[p] = poses[p + 1];
            poses[p]->index --;
        }

        free_Pose(P);
    }
}

void add_Pose_To_Deformer(deformer * D)
{
    pose * P = malloc(sizeof(pose));
    poses[posesIndex] = P;
    P->index = posesIndex;
    P->Name = malloc(STRLEN * sizeof(char));
    sprintf(P->Name, "%s %d", "Pose", posesCount ++);
    P->transformers_count = D->Transformers_Count;
    P->TP = calloc(P->transformers_count, sizeof(transformer_pose));
    P->D = D;

    int t;

    for (t = 0; t < D->Transformers_Count; t ++)
    {
        P->TP[t].rot_Order = D->Transformers[t]->rot_Order;
        memcpy(P->TP[t].scl, D->Transformers[t]->scl, sizeof(P->TP[t].scl));
        memcpy(P->TP[t].scl_vec, D->Transformers[t]->scl_vec, sizeof(P->TP[t].scl_vec));
        memcpy(P->TP[t].rot, D->Transformers[t]->rot, sizeof(P->TP[t].rot));
        memcpy(P->TP[t].rotVec, D->Transformers[t]->rotVec, sizeof(P->TP[t].rotVec));
        memcpy(P->TP[t].rotVec_, D->Transformers[t]->rotVec_, sizeof(P->TP[t].rotVec_));
        memcpy(P->TP[t].rotVec_I, D->Transformers[t]->rotVec_I, sizeof(P->TP[t].rotVec_I));
        memcpy(P->TP[t].rotVec_B, D->Transformers[t]->rotVec_B, sizeof(P->TP[t].rotVec_B));
        memcpy(P->TP[t].pos, D->Transformers[t]->pos, sizeof(P->TP[t].pos));
        memcpy(P->TP[t].pos_, D->Transformers[t]->pos_, sizeof(P->TP[t].pos_));
    }

    D->Poses[D->Poses_Count ++] = P;

    posesIndex ++;
}

void list_Pose_Nodes(deformer * D, int PoseIndex)
{
    int p;
    pose * P;

    for (p = 0; p < D->Poses_Count; p ++)
    {
        P = D->Poses[p];
        sprintf(Pose_Names[Poses_c], "%s", P->Name);
        Pose_List[Poses_c] = P->index;

        Pose_X_Offset[Poses_c] = 2;
        Pose_X_Collapsed[Poses_c] = 0;

        Poses_c ++;
    }
}

void create_Poses_List(int PoseIndex)
{
    int d;
    deformer * D;
    Poses_c = 0;
    Deformer_Node_c = 1;

    for (d = 0; d < deformerIndex; d ++)
    {
        D = deformers[d];

        if (D->selected)
            selected_deformer_node = Poses_c;

        memcpy(Pose_Names[Poses_c], D->Name, strlen(D->Name));
        Pose_Names[Poses_c][strlen(D->Name)] = '\0';
        Pose_List[Poses_c] = -Deformer_Node_c;
        Pose_X_Offset[Poses_c] = 1;
        Pose_X_Collapsed[Poses_c] = D->collapsed;
        Poses_c ++;
        Deformer_Node_c ++;
        if (!D->collapsed)
            list_Pose_Nodes(D, PoseIndex);
    }
}

void replace_Pose_Name(char * EditString)
{
    if (Pose_List[PoseIndex] >= 0)
    {
        pose * P = poses[Pose_List[PoseIndex]];
        memcpy(P->Name, EditString, strlen(EditString));
        P->Name[strlen(EditString)] = '\0';
    }
    else
    {
        deformer * D = deformers[-(Pose_List[PoseIndex] + 1)];
        memcpy(D->Name, EditString, strlen(EditString));
        D->Name[strlen(EditString)] = '\0';
    }
}

void set_Pose_H_Button(int index)
{
    int i;
    for (i = 0; i < H_POSE_NUM; i ++)
    {
        Button_h_pose[i].color = UI_GRAYB;
    }
    if (index > -1)
        Button_h_pose[index].color = UI_GRAYD;
}

#endif // POSES_H_INCLUDED
