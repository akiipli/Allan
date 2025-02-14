/*
The MIT License

Copyright <2018> <Allan Kiipli>
*/

#ifndef LOADING_H_INCLUDED
#define LOADING_H_INCLUDED

int loading_version = 0;

typedef struct
{
    unsigned address;
}
Subcharacter_In;

typedef struct
{
    unsigned address;
}
Pose_In;

typedef struct
{
    unsigned address;
}
Deformer_In;

typedef struct
{
    int chainsIndex;
}
ikChains_In;

typedef struct
{
    int constraintsIndex;
}
constraints_In;

typedef struct
{
    int bonesIndex;
}
Bones_In;

typedef struct
{
    unsigned address;
}
World_In;

typedef struct
{
    int locatorIndex;
}
Locators_In;

typedef struct
{
    int cpsIndex;
}
Cps_In;

typedef struct
{
    int curvesIndex;
}
Curves_In;

typedef struct
{
    int trajectoryIndex;
}
Trajectory_In;

typedef struct
{
    int cameraIndex;
}
Camera_In;

typedef struct
{
    int segmentIndex;
}
Segments_In;

typedef struct
{
    int keyframesIndex;
}
Keyframes_In;

typedef struct
{
    int keyframesIndex;
}
Morf_Keyframes_In;

typedef struct
{
    int deformermorphmapIndex;
    int deformermorphIndex;
}
Morphs_In;

typedef struct
{
    int t_index, obj_count, b_index, i_index;
}
hierarchys_pack;

typedef struct
{
    int dmm_index, dm_index;
}
morf_pack;

typedef struct
{
    unsigned address;
    char Name[STRLEN];
    int indice;
    float weight;
    int indices_count;
    int polygon_selections;
    int edge_selections;
    int vertex_selections;
}
Selection_In;

typedef struct
{
    unsigned address;
    int indices_count;
    int selectionIndex;
}
Object_Selection_In;

typedef struct
{
    unsigned address;
    char Name[STRLEN];
    int indice;
    float weight;
    int indices_count;
    int polygon_selections;
    int edge_selections;
    int vertex_selections;
}
Item_Selection_In;

typedef struct
{
    unsigned address;
    char Name[STRLEN];
    int textcount;
    int vertcount;
    int icolcount;
    int tripcount;
    int edgecount;
    int uvedcount;
    int polycount;
    int curvcount;
}
Object_In;

void assign_Texture_offset(object * O, int initial_M_count);
void assign_Texture(object * O);

void scale_xyz(transformer_pose * T)
{
    T->rotVec[0][0] = T->rotVec_[0][0] * T->scl_vec[0];
    T->rotVec[0][1] = T->rotVec_[0][1] * T->scl_vec[0];
    T->rotVec[0][2] = T->rotVec_[0][2] * T->scl_vec[0];

    T->rotVec[1][0] = T->rotVec_[1][0] * T->scl_vec[1];
    T->rotVec[1][1] = T->rotVec_[1][1] * T->scl_vec[1];
    T->rotVec[1][2] = T->rotVec_[1][2] * T->scl_vec[1];

    T->rotVec[2][0] = T->rotVec_[2][0] * T->scl_vec[2];
    T->rotVec[2][1] = T->rotVec_[2][1] * T->scl_vec[2];
    T->rotVec[2][2] = T->rotVec_[2][2] * T->scl_vec[2];
}

Poses_In POSESIN;

Poses_In read_Poses_file(char * fileName)
{
    POSESIN.initialized = 0;
    POSESIN.Poses_c = 0;
    POSESIN.TP_c = 0;
    FILE * fp;
    fp = fopen(fileName, "r");
    if (fp == NULL)
    {
        printf("Maybe no permission.\n");
        return POSESIN;
    }
    char buff[BUF_SIZE];
    buff[0] = '\0';

    char * p;

    pose * P;
    transformer_pose * T;

    int i, t, poses_count = 0;

    posesCount_Import = 0;

    if (fgets(buff, BUF_SIZE, fp))
    {
        if (strcmp("Poses\n", buff) == 0)
        {
            fgets(buff, BUF_SIZE, fp);
            sscanf(buff, "%d", &poses_count);

            for (i = 0; i < poses_count; i ++)
            {
                if (posesCount_Import >= POSES)
                {
                    fclose(fp);
                    POSESIN.Poses_c = i - 1;
                    return POSESIN;
                }
                P = malloc(sizeof(pose));
                poses_Import[posesCount_Import] = P;
                P->index = posesCount_Import;
                P->Name = malloc(STRLEN * sizeof(char));

                fgets(buff, BUF_SIZE, fp);

                p = strchr(buff, '\n');
                *p = '\0';

                sprintf(P->Name, "%s", buff);

                fgets(buff, BUF_SIZE, fp);
                sscanf(buff, "%u", &P->address);

                fgets(buff, BUF_SIZE, fp);
                sscanf(buff, "%u", (unsigned*)&P->D);

                fgets(buff, BUF_SIZE, fp);
                sscanf(buff, "%d", &P->transformers_count);

                P->TP = calloc(P->transformers_count, sizeof(transformer_pose));

                for (t = 0; t < P->transformers_count; t ++)
                {
                    T = &P->TP[t];
                    fgets(buff, BUF_SIZE, fp);
                    sscanf(buff, "%d", &T->rot_Order);
                    fgets(buff, BUF_SIZE, fp);
                    sscanf(buff, "%f %f %f", &T->scl[0], &T->scl[1], &T->scl[2]);
                    fgets(buff, BUF_SIZE, fp);
                    sscanf(buff, "%f %f %f", &T->scl_vec[0], &T->scl_vec[1], &T->scl_vec[2]);
                    fgets(buff, BUF_SIZE, fp);
                    sscanf(buff, "%f %f %f", &T->rot[0], &T->rot[1], &T->rot[2]);
                    fgets(buff, BUF_SIZE, fp);
                    sscanf(buff, "%f %f %f %f %f %f %f %f %f",
                            &T->rotVec_[0][0], &T->rotVec_[0][1], &T->rotVec_[0][2],
                            &T->rotVec_[1][0], &T->rotVec_[1][1], &T->rotVec_[1][2],
                            &T->rotVec_[2][0], &T->rotVec_[2][1], &T->rotVec_[2][2]);
                    fgets(buff, BUF_SIZE, fp);
                    sscanf(buff, "%f %f %f %f %f %f %f %f %f",
                            &T->rotVec_I[0][0], &T->rotVec_I[0][1], &T->rotVec_I[0][2],
                            &T->rotVec_I[1][0], &T->rotVec_I[1][1], &T->rotVec_I[1][2],
                            &T->rotVec_I[2][0], &T->rotVec_I[2][1], &T->rotVec_I[2][2]);
                    fgets(buff, BUF_SIZE, fp);
                    sscanf(buff, "%f %f %f %f %f %f %f %f %f",
                            &T->rotVec_B[0][0], &T->rotVec_B[0][1], &T->rotVec_B[0][2],
                            &T->rotVec_B[1][0], &T->rotVec_B[1][1], &T->rotVec_B[1][2],
                            &T->rotVec_B[2][0], &T->rotVec_B[2][1], &T->rotVec_B[2][2]);
                    fgets(buff, BUF_SIZE, fp);
                    sscanf(buff, "%f %f %f", &T->pos[0], &T->pos[1], &T->pos[2]);
                    fgets(buff, BUF_SIZE, fp);
                    sscanf(buff, "%f %f %f", &T->pos_[0], &T->pos_[1], &T->pos_[2]);
                    fgets(buff, BUF_SIZE, fp);
                    sscanf(buff, "%d", &T->style);
                    scale_xyz(T);
                }
                posesCount_Import ++;
            }
        }
    }

    fclose(fp);

    if (posesCount_Import > 0)
    {
        POSESIN.initialized = 1;
    }

    POSESIN.Poses_c = posesCount_Import;

    return POSESIN;
}

int read_Subcharacter_file(Subcharacter_In * SUBCHARACTER_IN, char * fileName)
{
    FILE * fp;
    fp = fopen(fileName, "r");
    if (fp == NULL)
    {
        printf("Maybe no permission.\n");
        return 0;
    }
    char buff[BUF_SIZE];
    buff[0] = '\0';

    char * p;

    subcharacter * S;

    int i, s, subcharacters_count = 0;

    if (fgets(buff, BUF_SIZE, fp))
    {
        if (strcmp("Subcharacters\n", buff) == 0)
        {
            fgets(buff, BUF_SIZE, fp);
            sscanf(buff, "%d", &subcharacters_count);

            for (s = 0; s < subcharacters_count; s ++)
            {
                if (subcharacterIndex >= SUBCHARACTERS)
                {
                    fclose(fp);
                    return s - 1;
                }
                S = malloc(sizeof(subcharacter));
                if (S == NULL)
                {
                    subcharacters_count = s;
                    break;
                }
                subcharacters[subcharacterIndex] = S;
                S->index = subcharacterIndex;
                S->Name = malloc(STRLEN * sizeof(char));

                fgets(buff, BUF_SIZE, fp);

                p = strchr(buff, '\n');
                *p = '\0';

                sprintf(S->Name, "%s", buff);

                fgets(buff, BUF_SIZE, fp);
                sscanf(buff, "%u", &SUBCHARACTER_IN->address);

                S->address = SUBCHARACTER_IN->address;

                fgets(buff, BUF_SIZE, fp);
                sscanf(buff, "%d", &S->collapsed);

                fgets(buff, BUF_SIZE, fp);
                sscanf(buff, "%u", (unsigned*)&S->Deformer);

                fgets(buff, BUF_SIZE, fp);
                sscanf(buff, "%d", &S->start);

                fgets(buff, BUF_SIZE, fp);
                sscanf(buff, "%d", &S->current_pose);

                fgets(buff, BUF_SIZE, fp);
                sscanf(buff, "%d %d %d %d",
                        &S->Transformers_Count,
                        &S->Bones_Count,
                        &S->Poses_Count,
                        &S->Subcharacters_Count);

                S->Transformers = malloc(S->Transformers_Count * sizeof(transformer*));
                if (S->Transformers == NULL)
                {
                    fclose(fp);
                    return 0;
                }

                S->Bones = malloc(S->Bones_Count * sizeof(bone*));
                if (S->Bones == NULL)
                {
                    fclose(fp);
                    return 0;
                }
                S->Poses = malloc(S->Poses_Count * sizeof(pose*));
                if (S->Poses == NULL)
                {
                    fclose(fp);
                    return 0;
                }
                S->Subcharacters = malloc(S->Subcharacters_Count * sizeof(subcharacter*));
                if (S->Subcharacters == NULL)
                {
                    fclose(fp);
                    return 0;
                }

                for (i = 0; i < S->Transformers_Count; i ++)
                {
                    fgets(buff, BUF_SIZE, fp);
                    sscanf(buff, "%u", (unsigned*)&S->Transformers[i]);
                }

                for (i = 0; i < S->Bones_Count; i ++)
                {
                    fgets(buff, BUF_SIZE, fp);
                    sscanf(buff, "%u", (unsigned*)&S->Bones[i]);
                }
                for (i = 0; i < S->Poses_Count; i ++)
                {
                    fgets(buff, BUF_SIZE, fp);
                    sscanf(buff, "%u", (unsigned*)&S->Poses[i]);
                }
                for (i = 0; i < S->Subcharacters_Count; i ++)
                {
                    fgets(buff, BUF_SIZE, fp);
                    sscanf(buff, "%u", (unsigned*)&S->Subcharacters[i]);
                }

                subcharacterIndex ++;
            }
        }
    }

    fclose(fp);
    return subcharacters_count;
}

int read_Subcharacter_Pose_file(Pose_In * POSE_IN, char * fileName)
{
    FILE * fp;
    fp = fopen(fileName, "r");
    if (fp == NULL)
    {
        printf("Maybe no permission.\n");
        return 0;
    }
    char buff[BUF_SIZE];
    buff[0] = '\0';

    char * p;

    pose * P;
    transformer_pose * T;

    int i, t, poses_count = 0;

    if (fgets(buff, BUF_SIZE, fp))
    {
        if (strcmp("SubcharacterP\n", buff) == 0)
        {
            fgets(buff, BUF_SIZE, fp);
            sscanf(buff, "%d", &poses_count);

            for (i = 0; i < poses_count; i ++)
            {
                if (subcharacter_posesIndex >= SUBCHARACTER_POSES)
                {
                    fclose(fp);
                    return i - 1;
                }
                P = malloc(sizeof(pose));
                subcharacter_Poses[subcharacter_posesIndex] = P;
                P->index = subcharacter_posesIndex;
                P->Name = malloc(STRLEN * sizeof(char));

                fgets(buff, BUF_SIZE, fp);

                p = strchr(buff, '\n');
                *p = '\0';

                sprintf(P->Name, "%s", buff);

                fgets(buff, BUF_SIZE, fp);
                sscanf(buff, "%u", &POSE_IN->address);

                P->address = POSE_IN->address;

                fgets(buff, BUF_SIZE, fp);
                sscanf(buff, "%u", (unsigned*)&P->D);

                fgets(buff, BUF_SIZE, fp);
                sscanf(buff, "%d", &P->transformers_count);

                P->TP = calloc(P->transformers_count, sizeof(transformer_pose));

                for (t = 0; t < P->transformers_count; t ++)
                {
                    T = &P->TP[t];
                    fgets(buff, BUF_SIZE, fp);
                    sscanf(buff, "%d", &T->rot_Order);
                    fgets(buff, BUF_SIZE, fp);
                    sscanf(buff, "%f %f %f", &T->scl[0], &T->scl[1], &T->scl[2]);
                    fgets(buff, BUF_SIZE, fp);
                    sscanf(buff, "%f %f %f", &T->scl_vec[0], &T->scl_vec[1], &T->scl_vec[2]);
                    fgets(buff, BUF_SIZE, fp);
                    sscanf(buff, "%f %f %f", &T->rot[0], &T->rot[1], &T->rot[2]);
                    fgets(buff, BUF_SIZE, fp);
                    sscanf(buff, "%f %f %f %f %f %f %f %f %f",
                            &T->rotVec_[0][0], &T->rotVec_[0][1], &T->rotVec_[0][2],
                            &T->rotVec_[1][0], &T->rotVec_[1][1], &T->rotVec_[1][2],
                            &T->rotVec_[2][0], &T->rotVec_[2][1], &T->rotVec_[2][2]);
                    fgets(buff, BUF_SIZE, fp);
                    sscanf(buff, "%f %f %f %f %f %f %f %f %f",
                            &T->rotVec_I[0][0], &T->rotVec_I[0][1], &T->rotVec_I[0][2],
                            &T->rotVec_I[1][0], &T->rotVec_I[1][1], &T->rotVec_I[1][2],
                            &T->rotVec_I[2][0], &T->rotVec_I[2][1], &T->rotVec_I[2][2]);
                    fgets(buff, BUF_SIZE, fp);
                    sscanf(buff, "%f %f %f %f %f %f %f %f %f",
                            &T->rotVec_B[0][0], &T->rotVec_B[0][1], &T->rotVec_B[0][2],
                            &T->rotVec_B[1][0], &T->rotVec_B[1][1], &T->rotVec_B[1][2],
                            &T->rotVec_B[2][0], &T->rotVec_B[2][1], &T->rotVec_B[2][2]);
                    fgets(buff, BUF_SIZE, fp);
                    sscanf(buff, "%f %f %f", &T->pos[0], &T->pos[1], &T->pos[2]);
                    fgets(buff, BUF_SIZE, fp);
                    sscanf(buff, "%f %f %f", &T->pos_[0], &T->pos_[1], &T->pos_[2]);
                    fgets(buff, BUF_SIZE, fp);
                    sscanf(buff, "%d", &T->style);
                    scale_xyz(T);
                }
                subcharacter_posesIndex ++;
                posesCount ++;
            }
        }
    }

    fclose(fp);
    return poses_count;
}

int read_Pose_file(Pose_In * POSE_IN, char * fileName)
{
    FILE * fp;
    fp = fopen(fileName, "r");
    if (fp == NULL)
    {
        printf("Maybe no permission.\n");
        return 0;
    }
    char buff[BUF_SIZE];
    buff[0] = '\0';

    char * p;

    pose * P;
    transformer_pose * T;

    int i, t, poses_count = 0;

    if (fgets(buff, BUF_SIZE, fp))
    {
        if (strcmp("Poses\n", buff) == 0)
        {
            fgets(buff, BUF_SIZE, fp);
            sscanf(buff, "%d", &poses_count);

            for (i = 0; i < poses_count; i ++)
            {
                if (posesIndex >= POSES)
                {
                    fclose(fp);
                    return i - 1;
                }
                P = malloc(sizeof(pose));
                poses[posesIndex] = P;
                P->index = posesIndex;
                P->Name = malloc(STRLEN * sizeof(char));

                fgets(buff, BUF_SIZE, fp);

                p = strchr(buff, '\n');
                *p = '\0';

                sprintf(P->Name, "%s", buff);

                fgets(buff, BUF_SIZE, fp);
                sscanf(buff, "%u", &POSE_IN->address);

                P->address = POSE_IN->address;

                fgets(buff, BUF_SIZE, fp);
                sscanf(buff, "%u", (unsigned*)&P->D);

                fgets(buff, BUF_SIZE, fp);
                sscanf(buff, "%d", &P->transformers_count);

                P->TP = calloc(P->transformers_count, sizeof(transformer_pose));

                for (t = 0; t < P->transformers_count; t ++)
                {
                    T = &P->TP[t];
                    fgets(buff, BUF_SIZE, fp);
                    sscanf(buff, "%d", &T->rot_Order);
                    fgets(buff, BUF_SIZE, fp);
                    sscanf(buff, "%f %f %f", &T->scl[0], &T->scl[1], &T->scl[2]);
                    fgets(buff, BUF_SIZE, fp);
                    sscanf(buff, "%f %f %f", &T->scl_vec[0], &T->scl_vec[1], &T->scl_vec[2]);
                    fgets(buff, BUF_SIZE, fp);
                    sscanf(buff, "%f %f %f", &T->rot[0], &T->rot[1], &T->rot[2]);
                    fgets(buff, BUF_SIZE, fp);
                    sscanf(buff, "%f %f %f %f %f %f %f %f %f",
                            &T->rotVec_[0][0], &T->rotVec_[0][1], &T->rotVec_[0][2],
                            &T->rotVec_[1][0], &T->rotVec_[1][1], &T->rotVec_[1][2],
                            &T->rotVec_[2][0], &T->rotVec_[2][1], &T->rotVec_[2][2]);
                    fgets(buff, BUF_SIZE, fp);
                    sscanf(buff, "%f %f %f %f %f %f %f %f %f",
                            &T->rotVec_I[0][0], &T->rotVec_I[0][1], &T->rotVec_I[0][2],
                            &T->rotVec_I[1][0], &T->rotVec_I[1][1], &T->rotVec_I[1][2],
                            &T->rotVec_I[2][0], &T->rotVec_I[2][1], &T->rotVec_I[2][2]);
                    fgets(buff, BUF_SIZE, fp);
                    sscanf(buff, "%f %f %f %f %f %f %f %f %f",
                            &T->rotVec_B[0][0], &T->rotVec_B[0][1], &T->rotVec_B[0][2],
                            &T->rotVec_B[1][0], &T->rotVec_B[1][1], &T->rotVec_B[1][2],
                            &T->rotVec_B[2][0], &T->rotVec_B[2][1], &T->rotVec_B[2][2]);
                    fgets(buff, BUF_SIZE, fp);
                    sscanf(buff, "%f %f %f", &T->pos[0], &T->pos[1], &T->pos[2]);
                    fgets(buff, BUF_SIZE, fp);
                    sscanf(buff, "%f %f %f", &T->pos_[0], &T->pos_[1], &T->pos_[2]);
                    fgets(buff, BUF_SIZE, fp);
                    sscanf(buff, "%d", &T->style);
                    scale_xyz(T);
                }
                posesIndex ++;
                posesCount ++;
            }
        }
    }

    fclose(fp);
    return poses_count;
}

int read_Deformer_file(Deformer_In * DEFR_IN, char * fileName)
{
    FILE * fp;
    fp = fopen(fileName, "r");
    if (fp == NULL)
    {
        printf("Maybe no permission.\n");
        return 0;
    }
    char buff[BUF_SIZE];
    buff[0] = '\0';

    char * p;

    if (deformerIndex >= DEFORMERS)
    {
        fclose(fp);
        return 0;
    }

    int i;

    if (fgets(buff, BUF_SIZE, fp))
    {
        if (strcmp("Deformer\n", buff) == 0)
        {
            deformer * D = malloc(sizeof(deformer));

            if (D == NULL)
            {
                fclose(fp);
                return 0;
            }

            deformers[deformerIndex] = D;
            D->index = deformerIndex;
            D->Name = malloc(STRLEN * sizeof(char));

            if (D->Name == NULL)
            {
                fclose(fp);
                return 0;
            }

            D->selected = 0;

            fgets(buff, BUF_SIZE, fp);

            p = strchr(buff, '\n');
            *p = '\0';

            sprintf(D->Name, "%s", buff);

            fgets(buff, BUF_SIZE, fp);
            sscanf(buff, "%u", &DEFR_IN->address);

            D->address = DEFR_IN->address;

            fgets(buff, BUF_SIZE, fp);

            if (loading_version >= 1012)
            {
                sscanf(buff, "%d %d %d %d %d %d %d %d %d",
                        &D->collapsed,
                        &D->Transformers_Count,
                        &D->Selections_Count,
                        &D->Objects_Count,
                        &D->Bones_Count,
                        &D->Poses_Count,
                        &D->IKchains_Count,
                        &D->Subcharacters_Count,
                        &D->Morph_Maps_Count);
            }
            else if (loading_version >= 1001)
            {
                sscanf(buff, "%d %d %d %d %d %d %d %d",
                        &D->collapsed,
                        &D->Transformers_Count,
                        &D->Selections_Count,
                        &D->Objects_Count,
                        &D->Bones_Count,
                        &D->Poses_Count,
                        &D->IKchains_Count,
                        &D->Subcharacters_Count);
                D->Morph_Maps_Count = 0;
            }
            else
            {
                sscanf(buff, "%d %d %d %d %d %d %d",
                        &D->collapsed,
                        &D->Transformers_Count,
                        &D->Selections_Count,
                        &D->Objects_Count,
                        &D->Bones_Count,
                        &D->Poses_Count,
                        &D->IKchains_Count);
                D->Subcharacters_Count = 0;
                D->Morph_Maps_Count = 0;
            }

            D->Transformers = malloc(D->Transformers_Count * sizeof(transformer*));
            if (D->Transformers == NULL)
            {
                fclose(fp);
                return 0;
            }
            D->Selections = malloc(D->Selections_Count * sizeof(vert_selection*));
            if (D->Selections == NULL)
            {
                fclose(fp);
                return 0;
            }
            D->Objects = malloc(D->Objects_Count * sizeof(object*));
            if (D->Objects == NULL)
            {
                fclose(fp);
                return 0;
            }
            D->Bones = malloc(D->Bones_Count * sizeof(bone*));
            if (D->Bones == NULL)
            {
                fclose(fp);
                return 0;
            }
            D->Poses = malloc(D->Poses_Count * sizeof(pose*));
            if (D->Poses == NULL)
            {
                fclose(fp);
                return 0;
            }
            D->IKchains = malloc(D->IKchains_Count * sizeof(ikChain*));
            if (D->IKchains == NULL)
            {
                fclose(fp);
                return 0;
            }
            D->Subcharacters = malloc(D->Subcharacters_Count * sizeof(subcharacter*));
            if (D->Subcharacters == NULL)
            {
                fclose(fp);
                return 0;
            }
            D->Morph_Maps = malloc(D->Morph_Maps_Count * sizeof(deformer_morph_map*));
            if (D->Morph_Maps == NULL)
            {
                fclose(fp);
                return 0;
            }

            for (i = 0; i < D->Transformers_Count; i ++)
            {
                fgets(buff, BUF_SIZE, fp);
                sscanf(buff, "%u", (unsigned*)&D->Transformers[i]);
            }
            for (i = 0; i < D->Selections_Count; i ++)
            {
                fgets(buff, BUF_SIZE, fp);
                sscanf(buff, "%u", (unsigned*)&D->Selections[i]);
            }
            for (i = 0; i < D->Objects_Count; i ++)
            {
                fgets(buff, BUF_SIZE, fp);
                sscanf(buff, "%u", (unsigned*)&D->Objects[i]);
            }
            for (i = 0; i < D->Bones_Count; i ++)
            {
                fgets(buff, BUF_SIZE, fp);
                sscanf(buff, "%u", (unsigned*)&D->Bones[i]);
            }
            for (i = 0; i < D->Poses_Count; i ++)
            {
                fgets(buff, BUF_SIZE, fp);
                sscanf(buff, "%u", (unsigned*)&D->Poses[i]);
            }

            for (i = 0; i < D->IKchains_Count; i ++)
            {
                fgets(buff, BUF_SIZE, fp);
                sscanf(buff, "%u", (unsigned*)&D->IKchains[i]);
            }

            for (i = 0; i < D->Subcharacters_Count; i ++)
            {
                fgets(buff, BUF_SIZE, fp);
                sscanf(buff, "%u", (unsigned*)&D->Subcharacters[i]);
            }

            for (i = 0; i < D->Morph_Maps_Count; i ++)
            {
                fgets(buff, BUF_SIZE, fp);
                sscanf(buff, "%u", (unsigned*)&D->Morph_Maps[i]);
            }

            if (fgets(buff, BUF_SIZE, fp))
                sscanf(buff, "%d", &D->current_pose);

            if (fgets(buff, BUF_SIZE, fp))
                sscanf(buff, "%f %f %f", &D->Delta[0], &D->Delta[1], &D->Delta[2]);

            if (fgets(buff, BUF_SIZE, fp))
                sscanf(buff, "%f %f %f %f %f %f %f %f %f",
                        &D->rotVec[0][0], &D->rotVec[0][1], &D->rotVec[0][2],
                        &D->rotVec[1][0], &D->rotVec[1][1], &D->rotVec[1][2],
                        &D->rotVec[2][0], &D->rotVec[2][1], &D->rotVec[2][2]);

            if (fgets(buff, BUF_SIZE, fp))
                sscanf(buff, "%f %f %f", &D->rot[0], &D->rot[1], &D->rot[2]);

            D->play = -1;
            D->P = NULL;

            if (loading_version >= 1018)
            {
                if (fgets(buff, BUF_SIZE, fp))
                    sscanf(buff, "%d %d", &D->linear_pose, &D->compose_hierarchy);
            }
            else if (loading_version >= 1015)
            {
                if (fgets(buff, BUF_SIZE, fp))
                    sscanf(buff, "%d", &D->linear_pose);
                D->compose_hierarchy = 0;
            }
            else
            {
                D->linear_pose = 1;
                D->compose_hierarchy = 0;
            }
        }
    }
    else
    {
        fclose(fp);
        return 0;
    }

    deformerIndex ++;

    fclose(fp);
    return 1;
}

int read_Constraints_file(constraints_In * CONSTRAINTS_IN, char * fileName)
{
    FILE * fp;
    fp = fopen(fileName, "r");
    if (fp == NULL)
    {
        printf("Maybe no permission.\n");
        return 0;
    }

    char buff[BUF_SIZE];
    buff[0] = '\0';

    int c;

    char * p;

    if (fgets(buff, BUF_SIZE, fp))
    {
        if (strcmp("Constraints\n", buff) == 0)
        {
            fgets(buff, BUF_SIZE, fp);
            sscanf(buff, "%d", &CONSTRAINTS_IN->constraintsIndex);

            for (c = 0; c < CONSTRAINTS_IN->constraintsIndex; c ++)
            {
                if (constraintsIndex >= CONSTRAINTS)
                {
                    fclose(fp);
                    return 0;
                }

                constraint * C = malloc(sizeof(constraint));

                if (C == NULL)
                {
                    fclose(fp);
                    return 0;
                }

                constraints[constraintsIndex] = C;
                C->index = constraintsIndex;

                C->Name = malloc(STRLEN * sizeof(char));

                if (C->Name == NULL)
                {
                    fclose(fp);
                    return 0;
                }

                fgets(buff, BUF_SIZE, fp);

                p = strchr(buff, '\n');
                *p = '\0';

                sprintf(C->Name, "%s", buff);

                fgets(buff, BUF_SIZE, fp);
                sscanf(buff, "%u", (unsigned*)&C->address);

                fgets(buff, BUF_SIZE, fp);
                sscanf(buff, "%d", &C->constraint_type);

                fgets(buff, BUF_SIZE, fp);
                sscanf(buff, "%u", (unsigned*)&C->Locator);

                fgets(buff, BUF_SIZE, fp);
                sscanf(buff, "%u", (unsigned*)&C->IK_goal);

                constraintsIndex ++;
            }
        }
    }
    else
    {
        fclose(fp);
        return 0;
    }

    fclose(fp);
    return 1;
}

int read_ikChains_file(ikChains_In * CHAINS_IN, char * fileName)
{
    FILE * fp;
    fp = fopen(fileName, "r");
    if (fp == NULL)
    {
        printf("Maybe no permission.\n");
        return 0;
    }

    char buff[BUF_SIZE];
    buff[0] = '\0';

    int i, b;

    char * p;

    if (fgets(buff, BUF_SIZE, fp))
    {
        if (strcmp("ikChains\n", buff) == 0)
        {
            fgets(buff, BUF_SIZE, fp);
            sscanf(buff, "%d", &CHAINS_IN->chainsIndex);

            for (i = 0; i < CHAINS_IN->chainsIndex; i ++)
            {
                if (iksIndex >= IKCHAINS)
                {
                    fclose(fp);
                    return 0;
                }

                ikChain * I = malloc(sizeof(ikChain));

                if (I == NULL)
                {
                    fclose(fp);
                    return 0;
                }

                ikChains[iksIndex] = I;
                I->index = iksIndex;

                I->Name = malloc(STRLEN * sizeof(char));

                if (I->Name == NULL)
                {
                    fclose(fp);
                    return 0;
                }

                fgets(buff, BUF_SIZE, fp);

                p = strchr(buff, '\n');
                *p = '\0';

                sprintf(I->Name, "%s", buff);

                fgets(buff, BUF_SIZE, fp);
                sscanf(buff, "%u", (unsigned*)&I->address);

                fgets(buff, BUF_SIZE, fp);
                sscanf(buff, "%u", (unsigned*)&I->Deformer);

                fgets(buff, BUF_SIZE, fp);
                sscanf(buff, "%d", &I->bonescount);

                I->Bones = malloc(I->bonescount * sizeof(bone*));

                for (b = 0; b < I->bonescount; b ++)
                {
                    fgets(buff, BUF_SIZE, fp);
                    sscanf(buff, "%u", (unsigned*)&I->Bones[b]);
                }

                fgets(buff, BUF_SIZE, fp);
                sscanf(buff, "%f", &I->sum_length);

                I->vectors = malloc(I->bonescount * sizeof(vec3));
                I->vectors_bone = malloc(I->bonescount * sizeof(vec3));
                I->positions_A = malloc(I->bonescount * sizeof(vec3));
                I->positions_B = malloc(I->bonescount * sizeof(vec3));
                I->bones_Rot = malloc(I->bonescount * sizeof(vec3));

                for (b = 0; b < I->bonescount; b ++)
                {
                    fgets(buff, BUF_SIZE, fp);
                    sscanf(buff, "%f %f %f", &I->vectors[b].vec[0], &I->vectors[b].vec[1], &I->vectors[b].vec[2]);
                    fgets(buff, BUF_SIZE, fp);
                    sscanf(buff, "%f %f %f", &I->vectors_bone[b].vec[0], &I->vectors_bone[b].vec[1], &I->vectors_bone[b].vec[2]);
                    fgets(buff, BUF_SIZE, fp);
                    sscanf(buff, "%f %f %f", &I->positions_A[b].vec[0], &I->positions_A[b].vec[1], &I->positions_A[b].vec[2]);
                    fgets(buff, BUF_SIZE, fp);
                    sscanf(buff, "%f %f %f", &I->positions_B[b].vec[0], &I->positions_B[b].vec[1], &I->positions_B[b].vec[2]);
                    fgets(buff, BUF_SIZE, fp);
                    sscanf(buff, "%f %f %f", &I->bones_Rot[b].vec[0], &I->bones_Rot[b].vec[1], &I->bones_Rot[b].vec[2]);
                }

                fgets(buff, BUF_SIZE, fp);
                sscanf(buff, "%u", (unsigned*)&I->A);

                fgets(buff, BUF_SIZE, fp);
                sscanf(buff, "%u", (unsigned*)&I->B);

                fgets(buff, BUF_SIZE, fp);
                sscanf(buff, "%f %f %f %f %f %f %f %f %f",
                        &I->rotVec_0[0][0], &I->rotVec_0[0][1], &I->rotVec_0[0][2],
                        &I->rotVec_0[1][0], &I->rotVec_0[1][1], &I->rotVec_0[1][2],
                        &I->rotVec_0[2][0], &I->rotVec_0[2][1], &I->rotVec_0[2][2]);

                fgets(buff, BUF_SIZE, fp);
                sscanf(buff, "%f %f %f %f %f %f %f %f %f",
                        &I->rotVec_1[0][0], &I->rotVec_1[0][1], &I->rotVec_1[0][2],
                        &I->rotVec_1[1][0], &I->rotVec_1[1][1], &I->rotVec_1[1][2],
                        &I->rotVec_1[2][0], &I->rotVec_1[2][1], &I->rotVec_1[2][2]);

                fgets(buff, BUF_SIZE, fp);
                sscanf(buff, "%f %f", &I->poleRot, &I->P.distance);
                fgets(buff, BUF_SIZE, fp);
                sscanf(buff, "%f %f %f", &I->P.vec[0], &I->P.vec[1], &I->P.vec[2]);
                if (loading_version >= 1004)
                {
                    fgets(buff, BUF_SIZE, fp);
                    sscanf(buff, "%d %d", &I->update, &I->stretch);
                }
                else if (loading_version >= 1002)
                {
                    fgets(buff, BUF_SIZE, fp);
                    sscanf(buff, "%d", &I->update);
                    I->stretch = 1;
                }
                else
                {
                    I->update = 1;
                    I->stretch = 1;
                }
                if (loading_version >= 1006)
                {
                    fgets(buff, BUF_SIZE, fp);
                    sscanf(buff, "%u %u", (unsigned*)&I->C, (unsigned*)&I->Pole);
                }
                else if (loading_version >= 1005)
                {
                    fgets(buff, BUF_SIZE, fp);
                    sscanf(buff, "%u", (unsigned*)&I->C);
                    I->Pole = NULL;
                }
                else
                {
                    I->C = NULL;
                    I->Pole = NULL;
                }
                iksIndex ++;
            }
        }
    }
    else
    {
        fclose(fp);
        return 0;
    }

    fclose(fp);
    return 1;
}

int read_Bones_file(Bones_In * BONES_IN, char * fileName)
{
    FILE * fp;
    fp = fopen(fileName, "r");
    if (fp == NULL)
    {
        printf("Maybe no permission.\n");
        return 0;
    }

    char buff[BUF_SIZE];
    buff[0] = '\0';

    int i;

    char * p;

    if (fgets(buff, BUF_SIZE, fp))
    {
        if (strcmp("Bones\n", buff) == 0)
        {
            fgets(buff, BUF_SIZE, fp);
            sscanf(buff, "%d", &BONES_IN->bonesIndex);

            for (i = 0; i < BONES_IN->bonesIndex; i ++)
            {
                if (bonesIndex >= BONES)
                {
                    fclose(fp);
                    return 0;
                }

                bone * B = malloc(sizeof(bone));

                if (B == NULL)
                {
                    fclose(fp);
                    return 0;
                }

                B->Distances = malloc(0 * sizeof(float*));
                B->Distances_Count = 0;

                bones[bonesIndex] = B;
                B->index = bonesIndex;

                B->Name = malloc(STRLEN * sizeof(char));

                if (B->Name == NULL)
                {
                    fclose(fp);
                    return 0;
                }

                fgets(buff, BUF_SIZE, fp);

                p = strchr(buff, '\n');
                *p = '\0';

                sprintf(B->Name, "%s", buff);

                B->selected = 0;

                fgets(buff, BUF_SIZE, fp);
                sscanf(buff, "%u", (unsigned*)&B->address);

                fgets(buff, BUF_SIZE, fp);
                sscanf(buff, "%u", (unsigned*)&B->parent);

                fgets(buff, BUF_SIZE, fp);
                sscanf(buff, "%f %f %f %f %f %f %f %f %f",
                        &B->rotVec_[0][0], &B->rotVec_[0][1], &B->rotVec_[0][2],
                        &B->rotVec_[1][0], &B->rotVec_[1][1], &B->rotVec_[1][2],
                        &B->rotVec_[2][0], &B->rotVec_[2][1], &B->rotVec_[2][2]);

                fgets(buff, BUF_SIZE, fp);
                sscanf(buff, "%f %f %f %f %f %f %f %f %f",
                        &B->rotVec_I[0][0], &B->rotVec_I[0][1], &B->rotVec_I[0][2],
                        &B->rotVec_I[1][0], &B->rotVec_I[1][1], &B->rotVec_I[1][2],
                        &B->rotVec_I[2][0], &B->rotVec_I[2][1], &B->rotVec_I[2][2]);

                fgets(buff, BUF_SIZE, fp);
                sscanf(buff, "%u %u %u", (unsigned*)&B->A, (unsigned*)&B->B, (unsigned*)&B->D);

                fgets(buff, BUF_SIZE, fp);
                sscanf(buff, "%d %f", &B->IK_member, &B->len);

                fgets(buff, BUF_SIZE, fp);
                sscanf(buff, "%u", (unsigned*)&B->IK);

                bonesIndex ++;
            }
        }
    }
    else
    {
        fclose(fp);
        return 0;
    }

    fclose(fp);
    return 1;
}

int read_Cameras_file(char * fileName)
{
    FILE * fp;
    fp = fopen(fileName, "r");
    if (fp == NULL)
    {
        printf("Maybe no permission.\n");
        return 0;
    }

    char buff[BUF_SIZE];
    buff[0] = '\0';

    if (fgets(buff, BUF_SIZE, fp))
    {
        if (strcmp("Cameras\n", buff) == 0)
        {
            fgets(buff, BUF_SIZE, fp);
            sscanf(buff, "%f", &CamDist);
            fgets(buff, BUF_SIZE, fp);
            sscanf(buff, "%f", &Camera_Persp.h_view);
            fgets(buff, BUF_SIZE, fp);
            sscanf(buff, "%f %f %f\n", &Camera_Persp.T->rot[0], &Camera_Persp.T->rot[1], &Camera_Persp.T->rot[2]);
            fgets(buff, BUF_SIZE, fp);
            sscanf(buff, "%f %f %f\n", &Camera_Persp.T->pos[0], &Camera_Persp.T->pos[1], &Camera_Persp.T->pos[2]);
            fgets(buff, BUF_SIZE, fp);
            sscanf(buff, "%f %f %f\n", &Camera_Persp.T->target[0], &Camera_Persp.T->target[1], &Camera_Persp.T->target[2]);
        }
    }
    else
    {
        fclose(fp);
        return 0;
    }

    fclose(fp);
    return 1;
}

int read_World_file(World_In * WORLD_IN, char * fileName)
{
    FILE * fp;
    fp = fopen(fileName, "r");
    if (fp == NULL)
    {
        printf("Maybe no permission.\n");
        return 0;
    }

    char buff[BUF_SIZE];
    buff[0] = '\0';

    if (fgets(buff, BUF_SIZE, fp))
    {
        if (strcmp("World\n", buff) == 0)
        {
            fgets(buff, BUF_SIZE, fp);
            sscanf(buff, "%u", &WORLD_IN->address);
            World.address = WORLD_IN->address;
            fgets(buff, BUF_SIZE, fp);
            sscanf(buff, "%f %f %d %d %d", &LocatorSize, &Reflect, &currentTheme, &NIGHT, &SHADOWS);
        }
    }
    else
    {
        fclose(fp);
        return 0;
    }

    fclose(fp);
    return 1;
}

void place_bump_Pack(surface_Material * M, char * fullName_text)
{
    int p;

    for (p = 0; p < bump_Pack_c; p ++)
    {
        if (strcmp(fullName_text, bump_Pack[p].texture_Name) == 0)
        {
            M->bump = bump_Pack[p].texture;
            M->Bump_idx = bump_Pack[p].Texture_idx;
            break;
        }
    }
}

void place_norm_Pack(surface_Material * M, char * fullName_text)
{
    int p;

    for (p = 0; p < norm_Pack_c; p ++)
    {
        if (strcmp(fullName_text, norm_Pack[p].texture_Name) == 0)
        {
            M->normal = norm_Pack[p].texture;
            M->Normal_idx = norm_Pack[p].Texture_idx;
            break;
        }
    }
}

void place_text_Pack(surface_Material * M, char * fullName_text)
{
    int p;

    for (p = 0; p < text_Pack_c; p ++)
    {
        if (strcmp(fullName_text, text_Pack[p].texture_Name) == 0)
        {
            M->texture = text_Pack[p].texture;
            M->Texture_idx = text_Pack[p].Texture_idx;
            break;
        }
    }
}

int read_Surfaces_file(char * fileName, int obj_count)
{
    FILE * fp;
    fp = fopen(fileName, "r");
    if (fp == NULL)
    {
        printf("Maybe no permission.\n");
        return 0;
    }

    char buff[BUF_SIZE];
    buff[0] = '\0';

    char fullName_text[STRLEN];
    char fullName_norm[STRLEN];
    char fullName_bump[STRLEN];

    int o, m, a, c;

    char * p;

    int materials_count, objects_count;

    unsigned object_address;

    surface_Material * M;
    object * O;

    int initial_M_count = Materials_count - 4;

    if (fgets(buff, BUF_SIZE, fp))
    {
        if (strcmp("Materials\n", buff) == 0)
        {
            fgets(buff, BUF_SIZE, fp);
            sscanf(buff, "%d", &materials_count);

            for (m = 0; m < materials_count; m ++)
            {
                if (Materials_count >= MATERIALS_TOTAL)
                {
                    fclose(fp);
                    return 1;
                }
                c = init_Material(Materials_count ++);

                M = &Materials[c];

                fgets(buff, BUF_SIZE, fp);

                p = strchr(buff, '\n');
                *p = '\0';

                sprintf(M->Name, "%s", buff);

                fgets(buff, BUF_SIZE, fp);

                p = strchr(buff, '\n');
                *p = '\0';

                sprintf(fullName_text, "%s", buff);

                texture = NULL;

                if (isFile(fullName_text))
                {
                //
#if DEBUG_WITHOUT_IL == 0
                texture = load_ILU_image(fullName_text, 0, 0);
#else
                texture = load_image(fullName_text, 0, 0);
#endif
                }

                fgets(buff, BUF_SIZE, fp);

                p = strchr(buff, '\n');
                *p = '\0';

                sprintf(fullName_norm, "%s", buff);

                normsrf = NULL;

                if (isFile(fullName_norm))
                {
#if DEBUG_WITHOUT_IL == 0
                normsrf = load_ILU_image(fullName_norm, 1, 0);
#else
                normsrf = load_image(fullName_norm, 1, 0);
#endif
                }

                fgets(buff, BUF_SIZE, fp);

                p = strchr(buff, '\n');
                *p = '\0';

                sprintf(fullName_bump, "%s", buff);

                bumpsrf = NULL;

                if (isFile(fullName_bump))
                {
#if DEBUG_WITHOUT_IL == 0
                bumpsrf = load_ILU_image(fullName_bump, 0, 1);
#else
                bumpsrf = load_image(fullName_bump, 0, 1);
#endif
                }

                fgets(buff, BUF_SIZE, fp);

                sscanf(buff, "%d %d %d %d", &M->smooth, &M->texture, &M->Texture_idx, &M->use_texture);

                if (texture != NULL)
                {
                    sprintf(Text_Names[Surf_Text_c], basename(fullName_text));
                    sprintf(Text_Names_Full[Surf_Text_c], fullName_text);

                    Surf_Text[Surf_Text_c] = texture;
                    M->texture = Surf_Text_c;
                    M->Texture_idx = TexFrom_Surface(texture);

                    sprintf(text_Pack[text_Pack_c].texture_Name, fullName_text);
                    text_Pack[text_Pack_c].texture = M->texture;
                    text_Pack[text_Pack_c].Texture_idx = M->Texture_idx;

                    text_Pack_c ++;
                    Textures_c ++;
                    Surf_Text_c ++;
                }
                else if (isFile(fullName_text))
                {
                    place_text_Pack(M, fullName_text);
                }
                else
                {
                    M->texture = 0;
                    M->Texture_idx = 0;
                }
                //

                fgets(buff, BUF_SIZE, fp);

                sscanf(buff, "%f %f %f %f %f %d %d %d %d %f", &M->RGBA.R, &M->RGBA.G, &M->RGBA.B, &M->RGBA.A, &M->Shininess, &M->normal, &M->Normal_idx, &M->bump, &M->Bump_idx, &M->Displacement);

                if (normsrf != NULL)
                {
                    sprintf(Norm_Names[Surf_Norm_c], basename(fullName_norm));
                    sprintf(Norm_Names_Full[Surf_Norm_c], fullName_norm);

                    Surf_Norm[Surf_Norm_c] = normsrf;
                    M->normal = Surf_Norm_c;
                    M->Normal_idx = NormFrom_Surface(normsrf);

                    sprintf(norm_Pack[norm_Pack_c].texture_Name, fullName_norm);
                    norm_Pack[norm_Pack_c].texture = M->normal;
                    norm_Pack[norm_Pack_c].Texture_idx = M->Normal_idx;

                    norm_Pack_c ++;
                    Normals_c ++;
                    Surf_Norm_c ++;
                }
                else if (isFile(fullName_norm))
                {
                    place_norm_Pack(M, fullName_norm);
                }
                else
                {
                    M->normal = 0;
                    M->Normal_idx = 0;
                }

                if (bumpsrf != NULL)
                {
                    sprintf(Bump_Names[Surf_Bump_c], basename(fullName_bump));
                    sprintf(Bump_Names_Full[Surf_Bump_c], fullName_bump);

                    Surf_Bump[Surf_Bump_c] = bumpsrf;
                    M->bump = Surf_Bump_c;
                    M->Bump_idx = BumpFrom_Surface(bumpsrf);

                    sprintf(bump_Pack[bump_Pack_c].texture_Name, fullName_bump);
                    bump_Pack[bump_Pack_c].texture = M->bump;
                    bump_Pack[bump_Pack_c].Texture_idx = M->Bump_idx;

                    bump_Pack_c ++;
                    Bumps_c ++;
                    Surf_Bump_c ++;
                }
                else if (isFile(fullName_bump))
                {
                    place_bump_Pack(M, fullName_bump);
                }
                else
                {
                    M->bump = 0;
                    M->Bump_idx = 0;
                }

                fgets(buff, BUF_SIZE, fp);
                sscanf(buff, "%d", &objects_count);

                for (a = 0; a < objects_count; a ++)
                {
                    fgets(buff, BUF_SIZE, fp);
                    sscanf(buff, "%u", &object_address);

                    for (o = objectIndex - obj_count; o < objectIndex; o ++)
                    {
                        O = objects[o];
                        if (O->address == object_address)
                        {
                            O->surface = c;
                            assign_Texture_offset(O, initial_M_count);
                            //assign_Texture(O);
                            break;
                        }
                    }
                }
            }
        }
    }
    else
    {
        fclose(fp);
        return 0;
    }

    fclose(fp);
    return 1;
}

int read_Cps_file(Cps_In * CP_IN, char * fileName)
{
    FILE * fp;
    fp = fopen(fileName, "r");
    if (fp == NULL)
    {
        printf("Maybe no permission.\n");
        return 0;
    }

    char buff[BUF_SIZE];
    buff[0] = '\0';

    int s, p;

    cp * CP;

    if (fgets(buff, BUF_SIZE, fp))
    {
        if (strcmp("Cps\n", buff) == 0)
        {
            fgets(buff, BUF_SIZE, fp);
            sscanf(buff, "%d", &CP_IN->cpsIndex);

            for (p = 0; p < CP_IN->cpsIndex; p ++)
            {
                if (cpsIndex >= CPS)
                {
                    fclose(fp);
                    return 0;
                }

                CP = calloc(1, sizeof(cp));

                if (CP == NULL)
                {
                    fclose(fp);
                    return 0;
                }

                cps[cpsIndex] = CP;
                CP->index = cpsIndex;
                cpsIndex ++;

                fgets(buff, BUF_SIZE, fp);
                sscanf(buff, "%u", &CP->address);

                fgets(buff, BUF_SIZE, fp);
                sscanf(buff, "%f %f %f", &CP->pos[0], &CP->pos[1], &CP->pos[2]);

                fgets(buff, BUF_SIZE, fp);
                sscanf(buff, "%d", &CP->segment_count);

                CP->segments = malloc(CP->segment_count * sizeof(curve_segment*));
                if (CP->segments == NULL)
                {
                    fclose(fp);
                    return 0;
                }

                for (s = 0; s < CP->segment_count; s ++)
                {
                    fgets(buff, BUF_SIZE, fp);
                    sscanf(buff, "%u", (unsigned*)&CP->segments[s]);
                }
            }
        }
        else
        {
            fclose(fp);
            return 0;
        }
    }

    fclose(fp);
    return 1;
}

int read_Segments_file(Segments_In * SEG_IN, char * fileName)
{
    FILE * fp;
    fp = fopen(fileName, "r");
    if (fp == NULL)
    {
        printf("Maybe no permission.\n");
        return 0;
    }

    char buff[BUF_SIZE];
    buff[0] = '\0';

    int s;

    curve_segment * S;

    if (fgets(buff, BUF_SIZE, fp))
    {
        if (strcmp("Segments\n", buff) == 0)
        {
            fgets(buff, BUF_SIZE, fp);
            sscanf(buff, "%d", &SEG_IN->segmentIndex);

            for (s = 0; s < SEG_IN->segmentIndex; s ++)
            {
                if (segmentIndex >= SEGMENTS)
                {
                    fclose(fp);
                    return 0;
                }

                S = calloc(1, sizeof(curve_segment));

                if (S == NULL)
                {
                    fclose(fp);
                    return 0;
                }

                segments[segmentIndex] = S;
                S->index = segmentIndex;
                segmentIndex ++;

                fgets(buff, BUF_SIZE, fp);
                sscanf(buff, "%u", &S->address);

                fgets(buff, BUF_SIZE, fp);
                sscanf(buff, "%d", &S->level);

                fgets(buff, BUF_SIZE, fp);
                sscanf(buff, "%d", &S->subdivided);

                fgets(buff, BUF_SIZE, fp);
                sscanf(buff, "%u", (unsigned*)&S->Curve);

                fgets(buff, BUF_SIZE, fp);
                sscanf(buff, "%u %u", (unsigned*)&S->segment[0], (unsigned*)&S->segment[1]);

                fgets(buff, BUF_SIZE, fp);
                sscanf(buff, "%f %f %f", &S->A[0], &S->A[1], &S->A[2]);
                fgets(buff, BUF_SIZE, fp);
                sscanf(buff, "%f %f %f", &S->B[0], &S->B[1], &S->B[2]);
                fgets(buff, BUF_SIZE, fp);
                sscanf(buff, "%f %f %f", &S->C[0], &S->C[1], &S->C[2]);

                if (loading_version >= 1008)
                {
                    fgets(buff, BUF_SIZE, fp);
                    sscanf(buff, "%f", &S->weight);
                    S->weight_init = S->weight;
                }
                else
                {
                    S->weight = 0.0;
                    S->weight_init = 0.0;
                }
            }
        }
        else
        {
            fclose(fp);
            return 0;
        }
    }

    fclose(fp);
    return 1;
}

morf_pack read_Morphs_file(Morphs_In * MORPH_IN, char * fileName, int d_index)
{
    morf_pack mP = {0, 0};

    FILE * fp;
    fp = fopen(fileName, "r");
    if (fp == NULL)
    {
        printf("Maybe no permission.\n");
        return mP;
    }

    char buff[BUF_SIZE];
    buff[0] = '\0';

    char * p;

    int m, o, d, v;

    position * P;
    object * O;
    morph * OD;
    morph_map * OM;
    object_morph_dialer * OMD;
    deformer_morph * Morph;
    deformer_morph_map * M;

    int deformer_morph_map_count = 0, deformer_morph_count = 0, objects_count = 0;
    unsigned object_address;

    if (fgets(buff, BUF_SIZE, fp))
    {
        if (strcmp("Morphs\n", buff) == 0)
        {
            fgets(buff, BUF_SIZE, fp);
            sscanf(buff, "%d", &deformer_morph_map_count);

            for (m = 0; m < deformer_morph_map_count; m ++)
            {
                M = malloc(sizeof(deformer_morph_map));
                if (M == NULL)
                {
                    fclose(fp);
                    return (morf_pack) {0, 0};
                }
                deformer_morph_maps[deformer_morph_map_Index] = M;
                M->index = deformer_morph_map_Index;

                fgets(buff, BUF_SIZE, fp);
                sscanf(buff, "%u", &M->address);

                M->Name = malloc(STRLEN * sizeof(char));

                fgets(buff, BUF_SIZE, fp);

                p = strchr(buff, '\n');
                *p = '\0';

                sprintf(M->Name, "%s", buff);

                fgets(buff, BUF_SIZE, fp);
                sscanf(buff, "%d", &M->collapsed);

                fgets(buff, BUF_SIZE, fp);
                sscanf(buff, "%d", &M->Morphs_Count);

                M->Morphs = malloc(M->Morphs_Count * sizeof(deformer_morph*));
                if (M->Morphs == NULL)
                {
                    fclose(fp);
                    return (morf_pack) {0, 0};
                }

                for (d = 0; d < M->Morphs_Count; d ++)
                {
                    fgets(buff, BUF_SIZE, fp);
                    sscanf(buff, "%u", (unsigned*)&M->Morphs[d]);
                }

                fgets(buff, BUF_SIZE, fp);
                sscanf(buff, "%d", &M->Object_Count);

                M->Objects = malloc(M->Object_Count * sizeof(object*));
                if (M->Objects == NULL)
                {
                    fclose(fp);
                    return (morf_pack) {0, 0};
                }

                for (o = 0; o < M->Object_Count; o ++)
                {
                    fgets(buff, BUF_SIZE, fp);
                    sscanf(buff, "%u", (unsigned*)&M->Objects[o]);
                }

                M->start = 0;
                M->current_morph = 0;

                deformer_morph_map_Index ++;
            }
            mP.dmm_index = deformer_morph_map_count;

            fgets(buff, BUF_SIZE, fp);
            sscanf(buff, "%d", &deformer_morph_count);

            for (m = 0; m < deformer_morph_count; m ++)
            {
                Morph = malloc(sizeof(deformer_morph));
                if (Morph == NULL)
                {
                    fclose(fp);
                    return (morf_pack) {0, 0};
                }
                deformer_morphs[deformer_morph_Index] = Morph;
                Morph->index = deformer_morph_Index;

                fgets(buff, BUF_SIZE, fp);
                sscanf(buff, "%u", &Morph->address);

                Morph->Name = malloc(STRLEN * sizeof(char));

                if (Morph->Name == NULL)
                {
                    fclose(fp);
                    return (morf_pack) {0, 0};
                }

                fgets(buff, BUF_SIZE, fp);

                p = strchr(buff, '\n');
                *p = '\0';

                sprintf(Morph->Name, "%s", buff);

                fgets(buff, BUF_SIZE, fp);
                sscanf(buff, "%d", &Morph->objectCount);

                Morph->Object_Morph_Map = malloc(Morph->objectCount * sizeof(object_morph_dialer*));
                if (Morph->Object_Morph_Map == NULL)
                {
                    fclose(fp);
                    return (morf_pack) {0, 0};
                }

                for (o = 0; o < Morph->objectCount; o ++)
                {
                    OMD = malloc(sizeof(object_morph_dialer));
                    if (OMD == NULL)
                    {
                        fclose(fp);
                        return (morf_pack) {0, 0};
                    }

                    Morph->Object_Morph_Map[o] = OMD;
                    fgets(buff, BUF_SIZE, fp);
                    sscanf(buff, "%u", (unsigned*)&OMD->O);
                    fgets(buff, BUF_SIZE, fp);
                    sscanf(buff, "%d", &OMD->map_index);
                    fgets(buff, BUF_SIZE, fp);
                    sscanf(buff, "%d", &OMD->morph_index);
                }

                fgets(buff, BUF_SIZE, fp);
                sscanf(buff, "%u", (unsigned*)&Morph->Map);
                fgets(buff, BUF_SIZE, fp);
                sscanf(buff, "%f", &Morph->Amount);

                deformer_morph_Index ++;
            }
            mP.dm_index = deformer_morph_count;

            fgets(buff, BUF_SIZE, fp);
            sscanf(buff, "%d", &objects_count);

            printf("objects_count %d %d\n", objects_count, objectIndex);

            for (o = objectIndex - objects_count; o < objectIndex; o ++)
            {
                fgets(buff, BUF_SIZE, fp);
                sscanf(buff, "%u", &object_address);

                for (d = objectIndex - objects_count; d < objectIndex; d ++) // since objects depends of listing in folders
                {
                    O = objects[d];
                    if (O->address == object_address)
                    {
                        break;
                    }
                }

                fgets(buff, BUF_SIZE, fp);
                sscanf(buff, "%d", &O->Morph_Maps_count);

                if (O->Morph_Maps_count > 0)
                    O->Morph_Maps = realloc(O->Morph_Maps, O->Morph_Maps_count * sizeof(morph_map*));

                if (O->Morph_Maps == NULL)
                {
                    O->Morph_Maps_count = 0;
                    fclose(fp);
                    return mP;
                }

                for (m = 0; m < O->Morph_Maps_count; m ++)
                {
                    OM = malloc(sizeof(morph_map));
                    if (OM == NULL)
                    {
                        O->Morph_Maps_count = 0;
                        fclose(fp);
                        return mP;
                    }
                    O->Morph_Maps[m] = OM;
                    fgets(buff, BUF_SIZE, fp);
                    sscanf(buff, "%u", &OM->address);

                    OM->Name = malloc(STRLEN * sizeof(char));
                    if (OM->Name == NULL)
                    {
                        O->Morph_Maps_count = 0;
                        fclose(fp);
                        return mP;
                    }

                    fgets(buff, BUF_SIZE, fp);

                    p = strchr(buff, '\n');
                    *p = '\0';

                    sprintf(OM->Name, "%s", buff);

                    fgets(buff, BUF_SIZE, fp);
                    sscanf(buff, "%d", &OM->collapsed);

                    fgets(buff, BUF_SIZE, fp);
                    sscanf(buff, "%d", &OM->MorphsCount);

                    fgets(buff, BUF_SIZE, fp);
                    sscanf(buff, "%d", &OM->VertCount);

                    OM->Verts = calloc(OM->VertCount, sizeof(weighted_index));
                    OM->Morphs = malloc(OM->MorphsCount * sizeof(morph*));

                    if (OM->Morphs == NULL || OM->Verts == NULL)
                    {
                        OM->MorphsCount = 0;
                        fclose(fp);
                        return mP;
                    }

                    for (d = 0; d < OM->MorphsCount; d ++)
                    {
                        OD = malloc(sizeof(morph));
                        if (OD == NULL)
                        {
                            OM->MorphsCount = d;
                            fclose(fp);
                            return mP;
                        }
                        OM->Morphs[d] = OD;

                        fgets(buff, BUF_SIZE, fp);
                        sscanf(buff, "%d", &OD->address);

                        OD->Name = malloc(STRLEN * sizeof(char));
                        if (OD->Name == NULL)
                        {
                            OM->MorphsCount = 0;
                            fclose(fp);
                            return mP;
                        }

                        fgets(buff, BUF_SIZE, fp);

                        p = strchr(buff, '\n');
                        *p = '\0';

                        sprintf(OD->Name, "%s", buff);

                        fgets(buff, BUF_SIZE, fp);
                        sscanf(buff, "%u", (unsigned*)&OD->M);

                        OD->Positions = malloc(OM->VertCount * sizeof(position));

                        if (OD->Positions == NULL)
                        {
                            OM->MorphsCount = 0;
                            fclose(fp);
                            return mP;
                        }

                        for (v = 0; v < OM->VertCount; v ++)
                        {
                            P = &OD->Positions[v];
                            fgets(buff, BUF_SIZE, fp);
                            sscanf(buff, "%f %f %f", &P->x, &P->y, &P->z);
                        }
                        fgets(buff, BUF_SIZE, fp);
                        sscanf(buff, "%f", &OD->Amount);
                    }
                    fgets(buff, BUF_SIZE, fp);
                    sscanf(buff, "%u", (unsigned*)&OM->Deformer);
                    fgets(buff, BUF_SIZE, fp);
                    sscanf(buff, "%u", (unsigned*)&OM->DM);
                    fgets(buff, BUF_SIZE, fp);
                    sscanf(buff, "%u", (unsigned*)&OM->Object);

                    printf("LOADING: %u\n", (unsigned)OM->Object);

                    for (v = 0; v < OM->VertCount; v ++)
                    {
                        fgets(buff, BUF_SIZE, fp);
                        sscanf(buff, "%d", &OM->Verts[v].index);
                        fgets(buff, BUF_SIZE, fp);
                        sscanf(buff, "%f", &OM->Verts[v].weight);
                    }
                }
            }
        }
        else
        {
            fclose(fp);
            return mP;
        }
    }

    MORPH_IN->deformermorphmapIndex = mP.dmm_index;
    MORPH_IN->deformermorphIndex = mP.dm_index;

    fclose(fp);
    return mP;
}

int read_Keyframes_file(Keyframes_In * KEYFR_IN, char * fileName, int t_index)
{
    FILE * fp;
    fp = fopen(fileName, "r");
    if (fp == NULL)
    {
        printf("Maybe no permission.\n");
        return 0;
    }

    char buff[BUF_SIZE];
    buff[0] = '\0';

    int t, f;
    unsigned t_address;

    transformer * T;
    timeline * Tm;

    int k_index = 0;

    if (fgets(buff, BUF_SIZE, fp))
    {
        if (strcmp("Keyframes\n", buff) == 0)
        {
            for (t = transformerIndex - t_index; t < transformerIndex; t ++)
            {
                T = transformers[t];

                fgets(buff, BUF_SIZE, fp);
                sscanf(buff, "%u", &t_address);

                if (t_address != 0 && T->address == t_address)
                {
                    result = init_timeline(T);

                    if (result == 0)
                    {
                        fclose(fp);
                        return 0;
                    }
                    else
                    {
                        Tm = T->Timeline;
                        fgets(buff, BUF_SIZE, fp);
                        sscanf(buff, "%d", &Tm->key_frames);

                        Tm->Frames = malloc(Tm->key_frames * sizeof(int));
                        if (Tm->Frames == NULL)
                        {
                            Tm->key_frames = 0;
                            fclose(fp);
                            return 0;
                        }
                        else
                        {
                            Tm->Values = malloc(Tm->key_frames * sizeof(transformer_values));
                            Tm->Acceleration = malloc(Tm->key_frames * sizeof(acceleration));
                        }
                        if (Tm->Values == NULL || Tm->Acceleration == NULL)
                        {
                            Tm->key_frames = 0;
                            fclose(fp);
                            return 0;
                        }
                        else
                        {
                            for (f = 0; f < Tm->key_frames; f ++)
                            {
                                fgets(buff, BUF_SIZE, fp);
                                sscanf(buff, "%d", &Tm->Frames[f]);
                                fgets(buff, BUF_SIZE, fp);
                                sscanf(buff, "%f %f %f", &Tm->Values[f].scl[0], &Tm->Values[f].scl[1], &Tm->Values[f].scl[2]);
                                fgets(buff, BUF_SIZE, fp);
                                sscanf(buff, "%f %f %f", &Tm->Values[f].rot[0], &Tm->Values[f].rot[1], &Tm->Values[f].rot[2]);
                                fgets(buff, BUF_SIZE, fp);
                                sscanf(buff, "%f %f %f", &Tm->Values[f].pos[0], &Tm->Values[f].pos[1], &Tm->Values[f].pos[2]);
                                fgets(buff, BUF_SIZE, fp);
                                sscanf(buff, "%f %f %f", &Tm->Values[f].scl_vec[0], &Tm->Values[f].scl_vec[1], &Tm->Values[f].scl_vec[2]);
                                fgets(buff, BUF_SIZE, fp);
                                sscanf(buff, "%f %f %f %f %f %f %f %f %f",
                                        &Tm->Values[f].rotVec_[0][0], &Tm->Values[f].rotVec_[0][1], &Tm->Values[f].rotVec_[0][2],
                                        &Tm->Values[f].rotVec_[1][0], &Tm->Values[f].rotVec_[1][1], &Tm->Values[f].rotVec_[1][2],
                                        &Tm->Values[f].rotVec_[2][0], &Tm->Values[f].rotVec_[2][1], &Tm->Values[f].rotVec_[2][2]);
                                fgets(buff, BUF_SIZE, fp);
                                sscanf(buff, "%d", &Tm->Acceleration[f].segment_type);
                                fgets(buff, BUF_SIZE, fp);
                                sscanf(buff, "%f", &Tm->Acceleration[f].a_exponent);
                                fgets(buff, BUF_SIZE, fp);
                                sscanf(buff, "%f", &Tm->Acceleration[f].b_exponent);
                                if (loading_version >= 1016)
                                {
                                    fgets(buff, BUF_SIZE, fp);
                                    sscanf(buff, "%f", &Tm->Values[f].trj_val);
                                }
                                else
                                {
                                    Tm->Values[f].trj_val = 0;
                                }
                            }
                        }
                    }
                    k_index ++;
                }
            }
        }
        else
        {
            fclose(fp);
            return 0;
        }
    }

    KEYFR_IN->keyframesIndex = k_index;

    fclose(fp);
    return 1;
}

int read_Morf_Keyframes_file(Morf_Keyframes_In * KEYFR_IN, char * fileName, int o_index)
{
    FILE * fp;
    fp = fopen(fileName, "r");
    if (fp == NULL)
    {
        printf("Maybe no permission.\n");
        return 0;
    }

    char buff[BUF_SIZE];
    buff[0] = '\0';

    int o, o0, f, v, vertcount;
    unsigned o_address;

    object * O;
    morph_timeline * Tmm;

    int k_index = 0;

    if (fgets(buff, BUF_SIZE, fp))
    {
        if (strcmp("Morf Keyframes\n", buff) == 0)
        {
            for (o0 = objectIndex - o_index; o0 < objectIndex; o0 ++)
            {
                fgets(buff, BUF_SIZE, fp);
                sscanf(buff, "%u", &o_address);

                if (o_address == 0)
                {
                    continue;
                }

                for (o = objectIndex - o_index; o < objectIndex; o ++)
                {
                    O = objects[o];

                    if (O->address == o_address)
                    {
                        result = init_morph_timeline(O);

                        if (result == 0)
                        {
                            fclose(fp);
                            return 0;
                        }
                        else
                        {
                            Tmm = O->Morph_Timeline;

                            fgets(buff, BUF_SIZE, fp);
                            sscanf(buff, "%d", &vertcount);
                            fgets(buff, BUF_SIZE, fp);
                            sscanf(buff, "%d", &Tmm->key_frames);

                            Tmm->Frames = malloc(Tmm->key_frames * sizeof(int));
                            if (Tmm->Frames == NULL)
                            {
                                Tmm->key_frames = 0;
                                fclose(fp);
                                return 0;
                            }
                            else
                            {
                                Tmm->Values = malloc(Tmm->key_frames * sizeof(morph_values));
                                Tmm->Acceleration = malloc(Tmm->key_frames * sizeof(acceleration));
                            }
                            if (Tmm->Values == NULL || Tmm->Acceleration == NULL)
                            {
                                Tmm->key_frames = 0;
                                fclose(fp);
                                return 0;
                            }
                            else
                            {
                                for (f = 0; f < Tmm->key_frames; f ++)
                                {
                                    fgets(buff, BUF_SIZE, fp);
                                    sscanf(buff, "%d", &Tmm->Frames[f]);

                                    Tmm->Values[f].R_Coords = malloc(vertcount * sizeof(morph_Pos));

                                    if (Tmm->Values[f].R_Coords == NULL)
                                    {
                                        Tmm->key_frames = 0;
                                        fclose(fp);
                                        return 0;
                                    }

                                    for (v = 0; v < vertcount; v ++)
                                    {
                                        fgets(buff, BUF_SIZE, fp);
                                        sscanf(buff, "%f %f %f", &Tmm->Values[f].R_Coords[v].x, &Tmm->Values[f].R_Coords[v].y, &Tmm->Values[f].R_Coords[v].z);
                                    }
                                    fgets(buff, BUF_SIZE, fp);
                                    sscanf(buff, "%d", &Tmm->Acceleration[f].segment_type);
                                    fgets(buff, BUF_SIZE, fp);
                                    sscanf(buff, "%f", &Tmm->Acceleration[f].a_exponent);
                                    fgets(buff, BUF_SIZE, fp);
                                    sscanf(buff, "%f", &Tmm->Acceleration[f].b_exponent);
                                }
                            }
                        }
                        k_index ++;
                    }
                }
            }
        }
        else
        {
            fclose(fp);
            return 0;
        }
    }

    KEYFR_IN->keyframesIndex = k_index;

    fclose(fp);
    return 1;
}


int read_Curves_file(Curves_In * CURV_IN, char * fileName)
{
    FILE * fp;
    fp = fopen(fileName, "r");
    if (fp == NULL)
    {
        printf("Maybe no permission.\n");
        return 0;
    }

    char buff[BUF_SIZE];
    buff[0] = '\0';

    int c, p, s;

    curve * C;

    if (fgets(buff, BUF_SIZE, fp))
    {
        if (strcmp("Curves\n", buff) == 0)
        {
            fgets(buff, BUF_SIZE, fp);
            sscanf(buff, "%d", &CURV_IN->curvesIndex);

            for (c = 0; c < CURV_IN->curvesIndex; c ++)
            {
                if (curvesIndex >= CURVES)
                {
                    fclose(fp);
                    return 0;
                }

                C = calloc(1, sizeof(curve));

                if (C == NULL)
                {
                    fclose(fp);
                    return 0;
                }

                curves[curvesIndex] = C;
                C->index = curvesIndex;
                curvesIndex ++;

                fgets(buff, BUF_SIZE, fp);
                sscanf(buff, "%u", &C->address);

                fgets(buff, BUF_SIZE, fp);
                sscanf(buff, "%d", &C->cps_count);

                C->cps = malloc(C->cps_count * sizeof(cp*));
                if (C->cps == NULL)
                {
                    fclose(fp);
                    return 0;
                }

                C->cps_continuity = malloc(C->cps_count * sizeof(float));
                if (C->cps_continuity == NULL)
                {
                    fclose(fp);
                    return 0;
                }

                for (p = 0; p < C->cps_count; p ++)
                {
                    fgets(buff, BUF_SIZE, fp);
                    sscanf(buff, "%u", (unsigned*)&C->cps[p]);

                    fgets(buff, BUF_SIZE, fp);
                    sscanf(buff, "%f", &C->cps_continuity[p]);
                }

                fgets(buff, BUF_SIZE, fp);
                sscanf(buff, "%d", &C->segment_count);

                C->segments = malloc(C->segment_count * sizeof(curve_segment*));
                if (C->segments == NULL)
                {
                    fclose(fp);
                    return 0;
                }

                for (s = 0; s < C->segment_count; s ++)
                {
                    fgets(buff, BUF_SIZE, fp);
                    sscanf(buff, "%u", (unsigned*)&C->segments[s]);
                }

                fgets(buff, BUF_SIZE, fp);
                sscanf(buff, "%d", &C->open);

                fgets(buff, BUF_SIZE, fp);
                sscanf(buff, "%u", (unsigned*)&C->O);

                C->visible = 1;

                if (loading_version >= 1016)
                {
                    fgets(buff, BUF_SIZE, fp);
                    sscanf(buff, "%u", (unsigned*)&C->Trj);
                }
                else
                {
                    C->Trj = NULL;
                }
            }
        }
        else
        {
            fclose(fp);
            return 0;
        }
    }

    fclose(fp);
    return 1;
}

int read_Anim_Cameras_file(Camera_In * CAM_IN, char * fileName)
{
    FILE * fp;
    fp = fopen(fileName, "r");
    if (fp == NULL)
    {
        printf("Maybe no permission.\n");
        return 0;
    }

    char buff[BUF_SIZE];
    buff[0] = '\0';

    char * p;

    int c;

    camera * C;

    if (fgets(buff, BUF_SIZE, fp))
    {
        if (strcmp("Anim Cameras\n", buff) == 0)
        {
            fgets(buff, BUF_SIZE, fp);
            sscanf(buff, "%d", &CAM_IN->cameraIndex);

            for (c = 0; c < CAM_IN->cameraIndex; c ++)
            {
                if (camIndex >= CAMERAS_TOTAL)
                {
                    fclose(fp);
                    return 0;
                }

                C = calloc(1, sizeof(camera));

                if (C == NULL)
                {
                    fclose(fp);
                    return 0;
                }

                cameras[camIndex] = C;
                C->index = camIndex;
                camIndex ++;

                fgets(buff, BUF_SIZE, fp);

                p = strchr(buff, '\n');
                *p = '\0';

                sprintf(C->Name, "%s", buff);

                fgets(buff, BUF_SIZE, fp);
                sscanf(buff, "%u", (unsigned*)&C->T);

                C->P = calloc(1, sizeof(camera_pose));

                if (C->P == NULL)
                {
                    fclose(fp);
                    return 0;
                }

                fgets(buff, BUF_SIZE, fp);
                sscanf(buff, "%f %f %f %f", &C->P->h_view, &C->P->pos[0], &C->P->pos[1], &C->P->pos[2]);
                fgets(buff, BUF_SIZE, fp);
                sscanf(buff, "%f %f %f", &C->P->rot[0], &C->P->rot[1], &C->P->rot[2]);
                fgets(buff, BUF_SIZE, fp);
                sscanf(buff, "%f %f %f", &C->P->target[0], &C->P->target[1], &C->P->target[2]);
                fgets(buff, BUF_SIZE, fp);
                sscanf(buff, "%f %f %f", &C->P->rotVec_[0][0], &C->P->rotVec_[0][1], &C->P->rotVec_[0][2]);
                fgets(buff, BUF_SIZE, fp);
                sscanf(buff, "%f %f %f", &C->P->rotVec_[1][0], &C->P->rotVec_[1][1], &C->P->rotVec_[1][2]);
                fgets(buff, BUF_SIZE, fp);
                sscanf(buff, "%f %f %f", &C->P->rotVec_[2][0], &C->P->rotVec_[2][1], &C->P->rotVec_[2][2]);
                fgets(buff, BUF_SIZE, fp);
                sscanf(buff, "%f", &C->P->CamDist);
                //
                fgets(buff, BUF_SIZE, fp);
                sscanf(buff, "%d %d %d", &C->ID, &C->width, &C->height);
                fgets(buff, BUF_SIZE, fp);
                sscanf(buff, "%f %f %f %f %f", &C->h_v_ratio, &C->h_view, &C->v_view, &C->view_minor, &C->view_major);
                fgets(buff, BUF_SIZE, fp);
                sscanf(buff, "%d %f %f %f %f", &C->ortho, &C->dim, &C->origin_2d[0], &C->origin_2d[1], &C->_ratio);
                fgets(buff, BUF_SIZE, fp);
                sscanf(buff, "%d %d %d %d", &C->sidebar, &C->bottom_line, &C->time_line, &C->uv_draw);
                fgets(buff, BUF_SIZE, fp);
                sscanf(buff, "%f %f %f", &C->View_Radius, &C->Resolution_Radius, &C->Pixel_Size_In_Radians);
            }
        }
        else
        {
            fclose(fp);
            return 0;
        }
    }

    fclose(fp);
    return 1;
}

int read_Trajectories_file(Trajectory_In * TRAJ_IN, char * fileName)
{
    FILE * fp;
    fp = fopen(fileName, "r");
    if (fp == NULL)
    {
        printf("Maybe no permission.\n");
        return 0;
    }

    char buff[BUF_SIZE];
    buff[0] = '\0';

    char * p;

    int tr, t;

    trajectory * Trj;

    if (fgets(buff, BUF_SIZE, fp))
    {
        if (strcmp("Trajectories\n", buff) == 0)
        {
            fgets(buff, BUF_SIZE, fp);
            sscanf(buff, "%d", &TRAJ_IN->trajectoryIndex);

            for (tr = 0; tr < TRAJ_IN->trajectoryIndex; tr ++)
            {
                if (trjIndex >= TRAJECTORIES)
                {
                    fclose(fp);
                    return 0;
                }

                Trj = calloc(1, sizeof(trajectory));

                if (Trj == NULL)
                {
                    fclose(fp);
                    return 0;
                }

                trajectories[trjIndex] = Trj;
                Trj->index = trjIndex;
                trjIndex ++;

                fgets(buff, BUF_SIZE, fp);
                sscanf(buff, "%u", &Trj->address);

                Trj->Name = malloc(STRLEN * sizeof(char));

                if (Trj->Name == NULL)
                {
                    fclose(fp);
                    return 0;
                }

                fgets(buff, BUF_SIZE, fp);

                p = strchr(buff, '\n');
                *p = '\0';

                sprintf(Trj->Name, "%s", buff);

                fgets(buff, BUF_SIZE, fp);
                sscanf(buff, "%u", (unsigned*)&Trj->Curve);

                fgets(buff, BUF_SIZE, fp);
                sscanf(buff, "%d", &Trj->update);

                fgets(buff, BUF_SIZE, fp);
                sscanf(buff, "%d", &Trj->transformers_count);

                Trj->Transformers = malloc(Trj->transformers_count * sizeof(transformer*));

                if (Trj->Transformers == NULL)
                {
                    fclose(fp);
                    return 0;
                }

                for (t = 0; t < Trj->transformers_count; t ++)
                {
                    fgets(buff, BUF_SIZE, fp);
                    sscanf(buff, "%u", (unsigned*)&Trj->Transformers[t]);
                }
            }
        }
        else
        {
            fclose(fp);
            return 0;
        }
    }

    fclose(fp);
    return 1;
}

int read_Locators_file(Locators_In * LOC_IN, char * fileName)
{
    FILE * fp;
    fp = fopen(fileName, "r");
    if (fp == NULL)
    {
        printf("Maybe no permission.\n");
        return 0;
    }

    char buff[BUF_SIZE];
    buff[0] = '\0';

    int j, i;

    transformer * T;

    char * p;

    if (fgets(buff, BUF_SIZE, fp))
    {
        if (strcmp("Transformers\n", buff) == 0)
        {
            fgets(buff, BUF_SIZE, fp);
            sscanf(buff, "%d", &LOC_IN->locatorIndex);

            for (i = 0; i < LOC_IN->locatorIndex; i ++)
            {
                if (transformerIndex >= TRANSFORMERS)
                {
                    fclose(fp);
                    return 0;
                }

                T = calloc(1, sizeof(transformer));

                if (T == NULL)
                {
                    fclose(fp);
                    return 0;
                }

                transformers[transformerIndex] = T;
                T->index = transformerIndex;

                T->Name = malloc(STRLEN * sizeof(char));

                if (T->Name == NULL)
                {
                    fclose(fp);
                    return 0;
                }

                fgets(buff, BUF_SIZE, fp);

                p = strchr(buff, '\n');
                *p = '\0';

                sprintf(T->Name, "%s", buff);

                fgets(buff, BUF_SIZE, fp);
                sscanf(buff, "%u", &T->address);

                fgets(buff, BUF_SIZE, fp);

                if (loading_version >= 1003)
                {
                    sscanf(buff, "%d %d %d %d %d %d %f %d",
                            &T->style,
                            &T->childcount,
                            &T->collapsed,
                            &T->rot_Order,
                            &T->bind_set,
                            &T->Selections_Count,
                            &T->LocatorSize,
                            &T->pin);
                }
                else
                {
                    sscanf(buff, "%d %d %d %d %d %d %f",
                            &T->style,
                            &T->childcount,
                            &T->collapsed,
                            &T->rot_Order,
                            &T->bind_set,
                            &T->Selections_Count,
                            &T->LocatorSize);
                    T->pin = pin_0;
                }

                //T->LocatorSize = LocatorSize;
                if (loading_version >= 1005)
                {
                    fgets(buff, BUF_SIZE, fp);
                    sscanf(buff, "%u %u %u %u %u %u",
                            (unsigned*)&T->parent,
                            (unsigned*)&T->Object,
                            (unsigned*)&T->Deformer,
                            (unsigned*)&T->Bone,
                            (unsigned*)&T->IK,
                            (unsigned*)&T->Constraint);
                }
                else
                {
                    fgets(buff, BUF_SIZE, fp);
                    sscanf(buff, "%u %u %u %u %u",
                            (unsigned*)&T->parent,
                            (unsigned*)&T->Object,
                            (unsigned*)&T->Deformer,
                            (unsigned*)&T->Bone,
                            (unsigned*)&T->IK);
                    T->Constraint = NULL;
                }

                if (T->Object == 0)
                    Locators[locatorIndex ++] = T;
                //T->Object = NULL;

                T->Selections = malloc(T->Selections_Count * sizeof(vert_selection*));
                if (T->Selections == NULL)
                {
                    fclose(fp);
                    return 0;
                }

                T->childs = malloc(T->childcount * sizeof(transformer*));
                if (T->childs == NULL)
                {
                    fclose(fp);
                    return 0;
                }

                fgets(buff, BUF_SIZE, fp);
                sscanf(buff, "%f %f %f", &T->scl[0], &T->scl[1], &T->scl[2]);
                fgets(buff, BUF_SIZE, fp);
                sscanf(buff, "%f %f %f", &T->scl_vec[0], &T->scl_vec[1], &T->scl_vec[2]);
                fgets(buff, BUF_SIZE, fp);
                sscanf(buff, "%f %f %f", &T->rot[0], &T->rot[1], &T->rot[2]);
                fgets(buff, BUF_SIZE, fp);
                sscanf(buff, "%f %f %f", &T->rot_bind[0], &T->rot_bind[1], &T->rot_bind[2]);
                fgets(buff, BUF_SIZE, fp);
                sscanf(buff, "%f %f %f %f %f %f %f %f %f",
                        &T->rotVec[0][0], &T->rotVec[0][1], &T->rotVec[0][2],
                        &T->rotVec[1][0], &T->rotVec[1][1], &T->rotVec[1][2],
                        &T->rotVec[2][0], &T->rotVec[2][1], &T->rotVec[2][2]);
                fgets(buff, BUF_SIZE, fp);
                sscanf(buff, "%f %f %f %f %f %f %f %f %f",
                        &T->rotVec_[0][0], &T->rotVec_[0][1], &T->rotVec_[0][2],
                        &T->rotVec_[1][0], &T->rotVec_[1][1], &T->rotVec_[1][2],
                        &T->rotVec_[2][0], &T->rotVec_[2][1], &T->rotVec_[2][2]);
                fgets(buff, BUF_SIZE, fp);
                sscanf(buff, "%f %f %f %f %f %f %f %f %f",
                        &T->rotVec_I[0][0], &T->rotVec_I[0][1], &T->rotVec_I[0][2],
                        &T->rotVec_I[1][0], &T->rotVec_I[1][1], &T->rotVec_I[1][2],
                        &T->rotVec_I[2][0], &T->rotVec_I[2][1], &T->rotVec_I[2][2]);
                fgets(buff, BUF_SIZE, fp);
                sscanf(buff, "%f %f %f %f %f %f %f %f %f",
                        &T->rotVec_B[0][0], &T->rotVec_B[0][1], &T->rotVec_B[0][2],
                        &T->rotVec_B[1][0], &T->rotVec_B[1][1], &T->rotVec_B[1][2],
                        &T->rotVec_B[2][0], &T->rotVec_B[2][1], &T->rotVec_B[2][2]);

                memcpy(&T->rotVec_Pin, T->rotVec_, sizeof T->rotVec_Pin);

                fgets(buff, BUF_SIZE, fp);
                sscanf(buff, "%f %f %f", &T->pos[0], &T->pos[1], &T->pos[2]);
                fgets(buff, BUF_SIZE, fp);
                sscanf(buff, "%f %f %f", &T->pos_[0], &T->pos_[1], &T->pos_[2]);
                //fgets(buff, BUF_SIZE, fp);
                //sscanf(buff, "%f %f %f", &T->Pos_[0], &T->Pos_[1], &T->Pos_[2]);
                fgets(buff, BUF_SIZE, fp);
                sscanf(buff, "%f %f %f", &T->pos_bind[0], &T->pos_bind[1], &T->pos_bind[2]);
                fgets(buff, BUF_SIZE, fp);
                sscanf(buff, "%f %f %f", &T->target[0], &T->target[1], &T->target[2]);

                T->aim = T->rotVec_[2];

                for (j = 0; j < T->childcount; j ++)
                {
                    fgets(buff, BUF_SIZE, fp);
                    sscanf(buff, "%u", (unsigned*)&T->childs[j]);
                }

                for (j = 0; j < T->Selections_Count; j ++)
                {
                    fgets(buff, BUF_SIZE, fp);
                    sscanf(buff, "%u\n", (unsigned*)&T->Selections[j]);
                }

                if (loading_version >= 1016)
                {
                    fgets(buff, BUF_SIZE, fp);
                    sscanf(buff, "%u", (unsigned*)&T->Trj);
                    fgets(buff, BUF_SIZE, fp);
                    sscanf(buff, "%f", &T->Trj_Value);
                }
                else
                {
                    T->Trj = NULL;
                    T->Trj_Value = 0;
                }

                transformerIndex ++;
                transformerCount ++;
            }
        }
    }
    else
    {
        fclose(fp);
        return 0;
    }

    fclose(fp);
    return 1;
}

int read_Object_Selection_file(Object_Selection_In * OBJ_SEL_IN, char * fileName, int obj_count)
{
    object * O;
    selection * S;

    FILE * fp;
    fp = fopen(fileName, "r");
    if (fp == NULL)
    {
        printf("Maybe no permission.\n");
        return 0;
    }
    char buff[BUF_SIZE];
    buff[0] = '\0';

    char * p;

    int s, i, o, indices_count;

    if (fgets(buff, BUF_SIZE, fp))
    {
        if (strcmp("Objects Selection\n", buff) == 0)
        {
            fgets(buff, BUF_SIZE, fp);
            sscanf(buff, "%u", &OBJ_SEL_IN->selectionIndex);

            object_selections += OBJ_SEL_IN->selectionIndex;

            if (object_selections >= SELECTIONS)
            {
                object_selections = SELECTIONS;
            }

            for (s = Selections_c[0]; s < object_selections; s ++)
            {
                S = &object_selection[s];

                fgets(buff, BUF_SIZE, fp);
                p = strchr(buff, '\n');
                *p = '\0';

                sprintf(Sels_Names[0][s], "%s", buff);
                S->Name = Sels_Names[0][s];

                fgets(buff, BUF_SIZE, fp);
                sscanf(buff, "%d", &OBJ_SEL_IN->indices_count);

                if (S->indices != NULL)
                {
                    S->indices = realloc(S->indices, OBJ_SEL_IN->indices_count * sizeof(int));
                }
                else
                {
                    S->indices = malloc(OBJ_SEL_IN->indices_count * sizeof(int));
                }

                indices_count = 0;

                for (i = 0; i < OBJ_SEL_IN->indices_count; i ++)
                {
                    fgets(buff, BUF_SIZE, fp);
                    sscanf(buff, "%u", &OBJ_SEL_IN->address);

                    for (o = objectIndex - obj_count; o < objectIndex; o ++)
                    {
                        O = objects[o];
                        if (O->address == OBJ_SEL_IN->address)
                        {
                            S->indices[indices_count ++] = O->index;
                            break;
                        }
                    }
                }
                S->indices_count = indices_count;
            }

            Selections_c[0] = object_selections;

            fclose(fp);
            return 1;
        }
        else
        {
            fclose(fp);
            return 0;
        }
    }
    else
    {
        fclose(fp);
        return 0;
    }
}

int read_Selection_file(Selection_In * SEL_IN, char * fileName)
{
    vert_selection * VS;
    selection * S;
    object * O;

    FILE * fp;
    fp = fopen(fileName, "r");
    if (fp == NULL)
    {
        printf("Maybe no permission.\n");
        return 0;
    }
    char buff[BUF_SIZE];
    buff[0] = '\0';

    char * p;

    int index, condition, o, s, i;

    if (fgets(buff, BUF_SIZE, fp))
    {
        if (strcmp("Selections\n", buff) == 0)
        {
            fgets(buff, BUF_SIZE, fp);
            sscanf(buff, "%u", &SEL_IN->address);
            condition = 0;
            for (o = 0; o < objectIndex; o ++)
            {
                O = objects[o];
                if (O->address == SEL_IN->address)
                {
                    index = o;
                    condition = 1;
                    break;
                }
            }
            if (condition)
            {
                fgets(buff, BUF_SIZE, fp);
                sscanf(buff, "%d %d %d",
                &SEL_IN->polygon_selections,
                &SEL_IN->edge_selections,
                &SEL_IN->vertex_selections);

                O = objects[index];

                O->polygon_selections = SEL_IN->polygon_selections;
                O->edge_selections = SEL_IN->edge_selections;
                O->vertex_selections = SEL_IN->vertex_selections;

                O->polygon_selection = realloc(O->polygon_selection, O->polygon_selections * sizeof(selection*));
                O->edge_selection = realloc(O->edge_selection, O->edge_selections * sizeof(selection*));
                O->vertex_selection = realloc(O->vertex_selection, O->vertex_selections * sizeof(vert_selection*));

                if (O->polygon_selections)
                {
                    fgets(buff, BUF_SIZE, fp);
                    if (strcmp("polygon\n", buff) == 0)
                    {
                        for (s = 0; s < O->polygon_selections; s ++)
                        {
                            O->polygon_selection[s] = malloc(sizeof(selection));
                            if (O->polygon_selection[s] != NULL)
                            {
                                S = O->polygon_selection[s];

                                fgets(buff, BUF_SIZE, fp);
                                p = strchr(buff, '\n');
                                *p = '\0';

                                S->Name = malloc(STRLEN * sizeof(char));

                                sprintf(S->Name, "%s", buff);

                                //printf("NAME : %s\n", S->Name);

                                fgets(buff, BUF_SIZE, fp);
                                sscanf(buff, "%d", &SEL_IN->indices_count);
                                S->indices_count = SEL_IN->indices_count;

                                //printf("INDICES : %d\n", S->indices_count);

                                S->indices = malloc(S->indices_count * sizeof(int));
                                S->Object = O;

                                for (i = 0; i < S->indices_count; i ++)
                                {
                                    fgets(buff, BUF_SIZE, fp);
                                    sscanf(buff, "%d", &SEL_IN->indice);
                                    S->indices[i] = SEL_IN->indice;
                                }
                            }
                            else
                            {
                                O->polygon_selections = 0;
                                O->edge_selections = 0;
                                O->vertex_selections = 0;
                                fclose(fp);
                                return 0;
                            }
                        }
                    }
                    else
                    {
                        fclose(fp);
                        return 0;
                    }
                }

                if (O->edge_selections)
                {
                    fgets(buff, BUF_SIZE, fp);
                    if (strcmp("edge\n", buff) == 0)
                    {
                        for (s = 0; s < O->edge_selections; s ++)
                        {
                            O->edge_selection[s] = malloc(sizeof(selection));
                            if (O->edge_selection[s] != NULL)
                            {
                                S = O->edge_selection[s];

                                fgets(buff, BUF_SIZE, fp);
                                p = strchr(buff, '\n');
                                *p = '\0';
                                S->Name = malloc(STRLEN * sizeof(char));

                                sprintf(S->Name, "%s", buff);

                                //printf("NAME : %s\n", S->Name);

                                fgets(buff, BUF_SIZE, fp);
                                sscanf(buff, "%d", &SEL_IN->indices_count);
                                S->indices_count = SEL_IN->indices_count;

                                //printf("INDICES : %d\n", S->indices_count);

                                S->indices = malloc(S->indices_count * sizeof(int));
                                S->Object = O;

                                for (i = 0; i < S->indices_count; i ++)
                                {
                                    fgets(buff, BUF_SIZE, fp);
                                    sscanf(buff, "%d", &SEL_IN->indice);
                                    S->indices[i] = SEL_IN->indice;
                                }
                            }
                            else
                            {
                                O->polygon_selections = 0;
                                O->edge_selections = 0;
                                O->vertex_selections = 0;
                                fclose(fp);
                                return 0;
                            }
                        }
                    }
                    else
                    {
                        fclose(fp);
                        return 0;
                    }
                }

                if (O->vertex_selections)
                {
                    fgets(buff, BUF_SIZE, fp);
                    if (strcmp("vertex\n", buff) == 0)
                    {
                        for (s = 0; s < O->vertex_selections; s ++)
                        {
                            O->vertex_selection[s] = malloc(sizeof(vert_selection));
                            if (O->vertex_selection[s] != NULL)
                            {
                                VS = O->vertex_selection[s];

                                fgets(buff, BUF_SIZE, fp);
                                p = strchr(buff, '\n');
                                *p = '\0';
                                VS->Name = malloc(STRLEN * sizeof(char));

                                //VS->index = s;

                                sprintf(VS->Name, "%s", buff);

                                //printf("NAME : %s\n", S->Name);

                                fgets(buff, BUF_SIZE, fp);
                                sscanf(buff, "%u", &SEL_IN->address);

                                VS->address = SEL_IN->address;

                                fgets(buff, BUF_SIZE, fp);
                                sscanf(buff, "%u", (unsigned*)&VS->Transformer);

                                //VS->Transformer = NULL;

                                fgets(buff, BUF_SIZE, fp);
                                sscanf(buff, "%d", &SEL_IN->indices_count);
                                VS->indices_count = SEL_IN->indices_count;

                                //printf("INDICES : %d\n", S->indices_count);

                                VS->indices = malloc(VS->indices_count * sizeof(int));
                                VS->weights = malloc(VS->indices_count * sizeof(float));

                                VS->split[0].indices_count = 0;
                                VS->split[0].indices = malloc(0 * sizeof(int));
                                VS->split[0].weights = malloc(0 * sizeof(int));
                                VS->split[1].indices_count = 0;
                                VS->split[1].indices = malloc(0 * sizeof(int));
                                VS->split[1].weights = malloc(0 * sizeof(int));

                                VS->Object = O;

                                for (i = 0; i < VS->indices_count; i ++)
                                {
                                    fgets(buff, BUF_SIZE, fp);
                                    sscanf(buff, "%d %f", &SEL_IN->indice, &SEL_IN->weight);
                                    VS->indices[i] = SEL_IN->indice;
                                    VS->weights[i] = SEL_IN->weight;
                                }
                            }
                            else
                            {
                                O->polygon_selections = 0;
                                O->edge_selections = 0;
                                O->vertex_selections = 0;
                                fclose(fp);
                                return 0;
                            }
                        }
                    }
                    else
                    {
                        fclose(fp);
                        return 0;
                    }
                }
            }
            else
            {
                printf("no object for it\n");
                fclose(fp);
                return 0;
            }
        }
        else
        {
            fclose(fp);
            return 0;
        }
    }
    else
    {
        fclose(fp);
        return 0;
    }

    fclose(fp);
    return 1;
}

int read_Object_file(Object_In * OB_IN, char * fileName, int VBO)
{
    if (objectIndex >= OBJECTS)
    {
        return 0;
    }
    object * O;
    FILE * fp;
    fp = fopen(fileName, "r");
    if (fp == NULL)
    {
        printf("Maybe no permission.\n");
        return 0;
    }
    char buff[BUF_SIZE];
    buff[0] = '\0';

    char * p;

    if (fgets(buff, BUF_SIZE, fp))
    {
        if (strcmp("Object\n", buff) == 0)
        {
            fgets(buff, BUF_SIZE, fp);
            p = strchr(buff, '\n');
            *p = '\0';
            sprintf(OB_IN->Name, "%s", buff);
            fgets(buff, BUF_SIZE, fp);
            sscanf(buff, "%u", &OB_IN->address);
            fgets(buff, BUF_SIZE, fp);

            sscanf(buff, "%d %d %d %d %d %d %d",
            &OB_IN->textcount,
            &OB_IN->vertcount,
            &OB_IN->icolcount,
            &OB_IN->tripcount,
            &OB_IN->edgecount,
            &OB_IN->uvedcount,
            &OB_IN->polycount);

            // init O here
            int result = init_object(objectIndex, OB_IN->textcount, OB_IN->vertcount, OB_IN->icolcount, OB_IN->tripcount, OB_IN->edgecount, OB_IN->uvedcount, OB_IN->polycount, 0);

            if (result)
            {
                O = objects[objectIndex];

                loaded_objects[objectIndex] = 1;

                objectIndex ++;

                if (loading_version >= 1012)
                {
                    fgets(buff, BUF_SIZE, fp);
                    sscanf(buff, "%d", &O->collapsed);
                }
                else
                {
                    O->collapsed = 0;
                }

                sprintf(O->Name, "%s", OB_IN->Name);
                O->address = OB_IN->address;
                printf("%u\n", O->address);

                O->surface = 0;

                fgets(buff, BUF_SIZE, fp);

                if (loading_version >= 1014)
                {
                    sscanf(buff, "%d %d %d %d", &O->deforms, &O->binding, &O->smooth, &O->subdlevel_Max);
                }
                else if (loading_version >= 1010)
                {
                    sscanf(buff, "%d %d %d", &O->deforms, &O->binding, &O->smooth);
                    O->subdlevel_Max = 1;
                }
                else
                {
                    sscanf(buff, "%d %d", &O->deforms, &O->binding);
                    O->smooth = 1;
                }

//                fgets(buff, BUF_SIZE, fp);
//                sscanf(buff, "%f %f %f", &O->T.rot[0], &O->T.rot[1], &O->T.rot[2]);
//
//                fgets(buff, BUF_SIZE, fp);
//                sscanf(buff, "%f %f %f", &O->T.pos[0], &O->T.pos[1], &O->T.pos[2]);
            }
            else
            {
                fclose(fp);
                return 0;
            }

            // use new object to read in components

            int i, j, b;

            uv * UV;

            for (i = 0; i < O->textcount; i ++)
            {
                UV = &O->uvtex[i / ARRAYSIZE][i % ARRAYSIZE];
                fgets(buff, BUF_SIZE, fp);
                sscanf(buff, "%*s %d %d %d", &UV->index, &UV->uvcount, &UV->vert);
                fgets(buff, BUF_SIZE, fp);
                sscanf(buff, "%*s %f %f", &UV->u, &UV->v);
                fgets(buff, BUF_SIZE, fp);

                b = 5; // uveds

                UV->uv_edges = malloc(UV->uvcount * sizeof(int));
                j = 0;

                while (buff[b] != '\n' && b < BUF_SIZE)
                {
                    b ++;
                    while (buff[b] != ' ' && buff[b] != '\n' && b < BUF_SIZE)
                    {
                        sscanf(&buff[b++], "%d", &UV->uv_edges[j ++]);
                        while (isdigit(buff[b]) && b < BUF_SIZE)
                        {
                            b++;
                        }
                    }
                }
                //printf("DIAGNOS %d %d\n", j, UV->uvcount);
            }

            vertex * V;

            for (i = 0; i < O->vertcount; i ++)
            {
                V = &O->verts[i / ARRAYSIZE][i % ARRAYSIZE];
                V->O = O;
                fgets(buff, BUF_SIZE, fp);
                sscanf(buff, "%*s %d %d %d", &V->index, &V->edgecount, &V->uv_vertcount);
                fgets(buff, BUF_SIZE, fp);
                sscanf(buff, "%*s %f", &V->weight);
                fgets(buff, BUF_SIZE, fp);
                sscanf(buff, "%*s %f %f %f", &V->x, &V->y, &V->z);
                fgets(buff, BUF_SIZE, fp);
                sscanf(buff, "%*s %f %f %f", &V->Tx, &V->Ty, &V->Tz);
                fgets(buff, BUF_SIZE, fp);
                sscanf(buff, "%*s %f %f %f", &V->N.Tx, &V->N.Ty, &V->N.Tz);
                fgets(buff, BUF_SIZE, fp);

                V->Rx = V->x; // morph coordinates
                V->Ry = V->y;
                V->Rz = V->z;

                b = 6;

                V->edges = malloc(V->edgecount * sizeof(int));
                j = 0;

                while (buff[b] != '\n' && b < BUF_SIZE)
                {
                    b ++;
                    while (buff[b] != ' ' && buff[b] != '\n' && b < BUF_SIZE)
                    {
                        sscanf(&buff[b++], "%d", &V->edges[j ++]);
                        while (isdigit(buff[b]) && b < BUF_SIZE)
                        {
                            b++;
                        }
                    }
                }

                //printf("DIAGNOS %d %d\n", j, V->edgecount);

                fgets(buff, BUF_SIZE, fp);

                b = 6;

                V->uv_verts = malloc(V->uv_vertcount * sizeof(int));
                j = 0;

                while (buff[b] != '\n' && b < BUF_SIZE)
                {
                    b ++;
                    while (buff[b] != ' ' && buff[b] != '\n' && b < BUF_SIZE)
                    {
                        sscanf(&buff[b++], "%d", &V->uv_verts[j ++]);
                        while (isdigit(buff[b]) && b < BUF_SIZE)
                        {
                            b++;
                        }
                    }
                }
                if (loading_version >= 1007)
                {
                    fgets(buff, BUF_SIZE, fp);
                    sscanf(buff, "%u", (unsigned*)&V->control_point);
                }
                else
                {
                    V->control_point = NULL;
                }
            }

            edge * E;

            for (i = 0; i < O->edgecount; i ++)
            {
                E = &O->edges[i / ARRAYSIZE][i % ARRAYSIZE];
                fgets(buff, BUF_SIZE, fp);
                sscanf(buff, "%*s %d %d %d", &E->index, &E->uv_edcount, &E->polycount);
                fgets(buff, BUF_SIZE, fp);
                sscanf(buff, "%*s %f", &E->weight);
                fgets(buff, BUF_SIZE, fp);
                sscanf(buff, "%*s %d %d", &E->verts[0], &E->verts[1]);
                fgets(buff, BUF_SIZE, fp);
                sscanf(buff, "%*s %d %d", &E->polys[0], &E->polys[1]);
                fgets(buff, BUF_SIZE, fp);
                sscanf(buff, "%*s %d %d", &E->slots[0], &E->slots[1]);
                fgets(buff, BUF_SIZE, fp);
                sscanf(buff, "%*s %d %d", &E->uv_edges[0], &E->uv_edges[1]);

                if (loading_version >= 1007)
                {
                    fgets(buff, BUF_SIZE, fp);
                    sscanf(buff, "%u", (unsigned*)&E->S);
                }
                else
                {
                    E->S = NULL;
                }
                if (loading_version >= 1009)
                {
                    fgets(buff, BUF_SIZE, fp);
                    sscanf(buff, "%d", &E->patch);
                }
                else
                {
                    E->patch = 0;
                }

                E->O = O;
            }

            uv_edge * UVE;

            for (i = 0; i < O->uvedcount; i ++)
            {
                UVE = &O->uveds[i / ARRAYSIZE][i % ARRAYSIZE];
                fgets(buff, BUF_SIZE, fp);
                sscanf(buff, "%*s %d %d %d %d", &UVE->index, &UVE->polycount, &UVE->split, &UVE->edge);
                fgets(buff, BUF_SIZE, fp);
                sscanf(buff, "%*s %d %d", &UVE->uv_verts[0], &UVE->uv_verts[1]);
                fgets(buff, BUF_SIZE, fp);
                sscanf(buff, "%*s %d %d", &UVE->uv_edges[0], &UVE->uv_edges[1]);
                fgets(buff, BUF_SIZE, fp);
                sscanf(buff, "%*s %d %d", &UVE->polys[0], &UVE->polys[1]);
                fgets(buff, BUF_SIZE, fp);
                sscanf(buff, "%*s %d %d", &UVE->slots[0], &UVE->slots[1]);

//                fprintf(F, "uved %d %d %d %d",
//                E->index,
//                E->polycount,
//                E->split,
//                E->edge);
//                int i;
//                fprintf(F, "\n uvvrs");
//                for(i = 0; i < 2; i++) fprintf(F, " %d", E->uv_verts[i]);
//                fprintf(F, "\n uveds");
//                for(i = 0; i < 2; i++) fprintf(F, " %d", E->uv_edges[i]);
//                fprintf(F, "\n polys");
//                for(i = 0; i < E->polycount; i++) fprintf(F, " %d", E->polys[i]);
//                fprintf(F, "\n slots");
//                for(i = 0; i < E->polycount; i++) fprintf(F, " %d", E->slots[i]);
//                fprintf(F, "\n");
            }

            polygon * P;

            for (i = 0; i < O->polycount; i ++)
            {
                P = &O->polys[i / ARRAYSIZE][i % ARRAYSIZE];
                fgets(buff, BUF_SIZE, fp);
                sscanf(buff, "%*s %d %d %d %d %d", &P->index, &P->color, &P->surface, &P->tripcount, &P->edgecount);

                P->trips = calloc(P->tripcount, sizeof(int));
                P->quads = calloc(P->edgecount, sizeof(int));
                P->edges = calloc(P->edgecount, sizeof(int));
                P->verts = calloc(P->edgecount, sizeof(int));
                P->texts = calloc(P->edgecount, sizeof(int));
                P->uveds = calloc(P->edgecount, sizeof(int));

                fgets(buff, BUF_SIZE, fp);

                b = 5;
                j = 0;

                while (buff[b] != '\n' && b < BUF_SIZE)
                {
                    b ++;
                    while (buff[b] != ' ' && buff[b] != '\n' && b < BUF_SIZE)
                    {
                        sscanf(&buff[b++], "%d", &P->trips[j ++]);
                        while (isdigit(buff[b]) && b < BUF_SIZE)
                        {
                            b++;
                        }
                    }
                }

                //printf("DIAGNOS %d %d\n", j, P->tripcount);

                fgets(buff, BUF_SIZE, fp);

                b = 5;
                j = 0;

                while (buff[b] != '\n' && b < BUF_SIZE)
                {
                    b ++;
                    while (buff[b] != ' ' && buff[b] != '\n' && b < BUF_SIZE)
                    {
                        sscanf(&buff[b++], "%d", &P->quads[j ++]);
                        while (isdigit(buff[b]) && b < BUF_SIZE)
                        {
                            b++;
                        }
                    }
                }

                //printf("DIAGNOS %d %d\n", j, P->edgecount);

                fgets(buff, BUF_SIZE, fp);

                b = 5;
                j = 0;

                while (buff[b] != '\n' && b < BUF_SIZE)
                {
                    b ++;
                    while (buff[b] != ' ' && buff[b] != '\n' && b < BUF_SIZE)
                    {
                        sscanf(&buff[b++], "%d", &P->edges[j ++]);
                        while (isdigit(buff[b]) && b < BUF_SIZE)
                        {
                            b++;
                        }
                    }
                }

                //printf("DIAGNOS %d %d\n", j, P->edgecount);

                fgets(buff, BUF_SIZE, fp);

                b = 5;
                j = 0;

                while (buff[b] != '\n' && b < BUF_SIZE)
                {
                    b ++;
                    while (buff[b] != ' ' && buff[b] != '\n' && b < BUF_SIZE)
                    {
                        sscanf(&buff[b++], "%d", &P->verts[j ++]);
                        while (isdigit(buff[b]) && b < BUF_SIZE)
                        {
                            b++;
                        }
                    }
                }

                //printf("DIAGNOS %d %d\n", j, P->edgecount);

                fgets(buff, BUF_SIZE, fp);

                b = 5;
                j = 0;

                while (buff[b] != '\n' && b < BUF_SIZE)
                {
                    b ++;
                    while (buff[b] != ' ' && buff[b] != '\n' && b < BUF_SIZE)
                    {
                        sscanf(&buff[b++], "%d", &P->texts[j ++]);
                        while (isdigit(buff[b]) && b < BUF_SIZE)
                        {
                            b++;
                        }
                    }
                }

                //printf("DIAGNOS %d %d\n", j, P->edgecount);

                fgets(buff, BUF_SIZE, fp);

                b = 5;
                j = 0;

                while (buff[b] != '\n' && b < BUF_SIZE)
                {
                    b ++;
                    while (buff[b] != ' ' && buff[b] != '\n' && b < BUF_SIZE)
                    {
                        sscanf(&buff[b++], "%d", &P->uveds[j ++]);
                        while (isdigit(buff[b]) && b < BUF_SIZE)
                        {
                            b++;
                        }
                    }
                }

                //printf("DIAGNOS %d %d\n", j, P->edgecount);
            }
//            fprintf(F, "poly %d %d %d %d",
//            P->index,
//            P->surface,
//            P->tripcount,
//            P->edgecount);
//            int i;
//            fprintf(F, "\n trips");
//            for(i = 0; i < P->tripcount; i++) fprintf(F, " %d", P->trips[i]);
//            fprintf(F, "\n quads");
//            for(i = 0; i < P->edgecount; i++) fprintf(F, " %d", P->quads[i]);
//            fprintf(F, "\n edges");
//            for(i = 0; i < P->edgecount; i++) fprintf(F, " %d", P->edges[i]);
//            fprintf(F, "\n verts");
//            for(i = 0; i < P->edgecount; i++) fprintf(F, " %d", P->verts[i]);
//            fprintf(F, "\n texts");
//            for(i = 0; i < P->edgecount; i++) fprintf(F, " %d", P->texts[i]);
//            fprintf(F, "\n uveds");
//            for(i = 0; i < P->edgecount; i++) fprintf(F, " %d", P->uveds[i]);
//            fprintf(F, "\n");

//            int d = generate_UV_Verts(O);
//            if (d)
//            {
//    //            d = generate_Edges(O);
//                if (d)
//                {
//                    generate_UVEdges(O);
//                }
//            }
//            generate_Edge_Split(O);
//            generate_Vert_UV_Verts(O);

            generate_Triangles(O);
            generate_polyNormals(O);
            generate_edgeNormals(O);
            generate_vertexNormals(O);

            update_bounding_box_for_Trips(O, 1); // updates also 2d box for triangles
            update_bounding_box_for_Edges(O, 1);
            update_bounding_box_for_UV_Edges(O, 1);
            update_bounding_box_for_Polys(O, 1);
            update_2d_bounding_box_for_Polys(O, 1);
            boxt_O(O, 1);
            mean_Edge_Length(O);

            init_object_colors(O, 0);

            if (VBO)
            {
                O->vertex_array = set_up_Fan_array(O);
            }
        }
        else
        {
            fclose(fp);
            return 0;
        }
    }
    else
    {
        fclose(fp);
        return 0;
    }

    fclose(fp);
    return 1;
}

int load_Surfaces(char * path, int obj_count)
{
    char Path[STRLEN];
    DIR * dir;
    struct dirent * ent;

    char ext[] = ".txt";
    char Extension[] = ".txt";

    int extension_len = strlen(ext);

    if ((dir = opendir(path)) != NULL)
    {
        while ((ent = readdir(dir)) != NULL)
        {
            Path[0] = '\0';
            strcat(Path, path);
            strcat(Path, "/");
            strcat(Path, ent->d_name);
            if (isFile(Path))
            {
                memcpy(Extension, &ent->d_name[strlen(ent->d_name) - extension_len], extension_len);
                if (strcmp(Extension, ext) == 0)
                {
                    read_Surfaces_file(Path, obj_count);
                }
            }
        }
    }
    else
    {
        return 0;
    }

    return 1;
}

int load_version(char * path)
{
    char Path[STRLEN];
    Path[0] = '\0';
    strcat(Path, path);
    strcat(Path, "/");
    strcat(Path, "version.txt");
    FILE * fp;
    fp = fopen(Path, "r");
    if (fp == NULL)
    {
        printf("Maybe no permission.\n");
        return 0;
    }
    char buff[BUF_SIZE];
    buff[0] = '\0';

    fgets(buff, BUF_SIZE, fp);
    sscanf(buff, "%d", &loading_version);

    fclose(fp);
    printf("version %d\n", loading_version);
    return loading_version;
}

int load_Objects(char * path, int VBO)
{
    int obj_count = 0;
    // obtain listing of files in "Objects"
    char Path[STRLEN];
    DIR * dir;
    struct dirent * ent;

    char ext[] = ".txt";
    char Extension[] = ".txt";

    int extension_len = strlen(ext);

    if ((dir = opendir(path)) != NULL)
    {
        while ((ent = readdir(dir)) != NULL)
        {
            Path[0] = '\0';
            strcat(Path, path);
            strcat(Path, "/");
            strcat(Path, ent->d_name);
            if (isFile(Path))
            {
                memcpy(Extension, &ent->d_name[strlen(ent->d_name) - extension_len], extension_len);
                if (strcmp(Extension, ext) == 0)
                {
                    int result = 0;
                    //printf("%s\n", Path);
                    Object_In * OB_IN = calloc(1, sizeof(Object_In));
                    result = read_Object_file(OB_IN, Path, VBO);
                    if (result)
                    {
                        obj_count ++;
                        printf("%s\n", OB_IN->Name);
                        printf("%d %d %d %d %d %d %d\n",
                        OB_IN->textcount,
                        OB_IN->vertcount,
                        OB_IN->icolcount,
                        OB_IN->tripcount,
                        OB_IN->edgecount,
                        OB_IN->uvedcount,
                        OB_IN->polycount);
                    }
                    free(OB_IN);
                }
            }
        }
    }
    return obj_count;
}

void load_Selections(char * path, int obj_count)
{
    char Path[STRLEN];
    DIR * dir;
    struct dirent * ent;

    char ext[] = ".txt";
    char Extension[] = ".txt";

    int extension_len = strlen(ext);
    int result;

    if ((dir = opendir(path)) != NULL)
    {
        while ((ent = readdir(dir)) != NULL)
        {
            Path[0] = '\0';
            strcat(Path, path);
            strcat(Path, "/");
            strcat(Path, ent->d_name);
            if (isFile(Path))
            {
                memcpy(Extension, &ent->d_name[strlen(ent->d_name) - extension_len], extension_len);
                if (strcmp(ent->d_name, "Objects_Selection.txt") == 0)
                {
                    result = 0;
                    printf("OBJECTS SELECTION\n");
                    Object_Selection_In * OBJ_SEL_IN = calloc(1, sizeof(Object_Selection_In));
                    result = read_Object_Selection_file(OBJ_SEL_IN, Path, obj_count);
                    if (result)
                    {
                        printf("%d\n", OBJ_SEL_IN->selectionIndex);
                    }
                    free(OBJ_SEL_IN);
                }
                else if (strcmp(Extension, ext) == 0)
                {
                    result = 0;
                    //printf("%s\n", Path);
                    Selection_In * SEL_IN = calloc(1, sizeof(Selection_In));
                    result = read_Selection_file(SEL_IN, Path);
                    if (result)
                    {
                        printf("%u\n", SEL_IN->address);
                        printf("%d %d %d\n",
                        SEL_IN->polygon_selections,
                        SEL_IN->edge_selections,
                        SEL_IN->vertex_selections);
                    }
                    free(SEL_IN);
                }
            }
        }
    }
}

int load_Deformers(char * path)
{
    char Path[STRLEN];
    DIR * dir;
    struct dirent * ent;

    char ext[] = ".txt";
    char Extension[] = ".txt";

    int extension_len = strlen(ext);

    int defr_count = 0;

    if ((dir = opendir(path)) != NULL)
    {
        while ((ent = readdir(dir)) != NULL)
        {
            Path[0] = '\0';
            strcat(Path, path);
            strcat(Path, "/");
            strcat(Path, ent->d_name);
            if (isFile(Path))
            {
                memcpy(Extension, &ent->d_name[strlen(ent->d_name) - extension_len], extension_len);
                if (strcmp(Extension, ext) == 0)
                {
                    int result = 0;
                    //printf("%s\n", Path);
                    Deformer_In * DEFR_IN = calloc(1, sizeof(Deformer_In));
                    result = read_Deformer_file(DEFR_IN, Path);
                    if (result)
                    {
                        defr_count ++;
                        printf("%u\n", DEFR_IN->address);
                    }
                    free(DEFR_IN);
                }
            }
        }
    }

    return defr_count;
}

int load_Subcharacter_Poses(char * path)
{
    char Path[STRLEN];
    DIR * dir;
    struct dirent * ent;

    char ext[] = ".txt";
    char Extension[] = ".txt";

    int extension_len = strlen(ext);

    int p_index = 0;

    if ((dir = opendir(path)) != NULL)
    {
        while ((ent = readdir(dir)) != NULL)
        {
            Path[0] = '\0';
            strcat(Path, path);
            strcat(Path, "/");
            strcat(Path, ent->d_name);
            if (isFile(Path))
            {
                memcpy(Extension, &ent->d_name[strlen(ent->d_name) - extension_len], extension_len);
                if (strcmp(Extension, ext) == 0)
                {
                    int result = 0;
                    //printf("%s\n", Path);
                    Pose_In * POSE_IN = calloc(1, sizeof(Pose_In));
                    result = read_Subcharacter_Pose_file(POSE_IN, Path);
                    if (result)
                    {
                        p_index += result;
                        printf("%u\n", POSE_IN->address);
                    }
                    free(POSE_IN);
                }
            }
        }
    }
    return p_index;
}

int load_Poses(char * path)
{
    char Path[STRLEN];
    DIR * dir;
    struct dirent * ent;

    char ext[] = ".txt";
    char Extension[] = ".txt";

    int extension_len = strlen(ext);

    int p_index = 0;

    if ((dir = opendir(path)) != NULL)
    {
        while ((ent = readdir(dir)) != NULL)
        {
            Path[0] = '\0';
            strcat(Path, path);
            strcat(Path, "/");
            strcat(Path, ent->d_name);
            if (isFile(Path))
            {
                memcpy(Extension, &ent->d_name[strlen(ent->d_name) - extension_len], extension_len);
                if (strcmp(Extension, ext) == 0)
                {
                    int result = 0;
                    //printf("%s\n", Path);
                    Pose_In * POSE_IN = calloc(1, sizeof(Pose_In));
                    result = read_Pose_file(POSE_IN, Path);
                    if (result)
                    {
                        p_index += result;
                        printf("%u\n", POSE_IN->address);
                    }
                    free(POSE_IN);
                }
            }
        }
    }
    return p_index;
}

int load_Subcharacters(char * path)
{
    char Path[STRLEN];
    DIR * dir;
    struct dirent * ent;

    char ext[] = ".txt";
    char Extension[] = ".txt";

    int extension_len = strlen(ext);

    int s_index = 0;

    if ((dir = opendir(path)) != NULL)
    {
        while ((ent = readdir(dir)) != NULL)
        {
            Path[0] = '\0';
            strcat(Path, path);
            strcat(Path, "/");
            strcat(Path, ent->d_name);
            if (isFile(Path))
            {
                memcpy(Extension, &ent->d_name[strlen(ent->d_name) - extension_len], extension_len);
                if (strcmp(Extension, ext) == 0)
                {
                    int result = 0;
                    //printf("%s\n", Path);
                    Subcharacter_In * SUBCHARACTER_IN = calloc(1, sizeof(Subcharacter_In));
                    result = read_Subcharacter_file(SUBCHARACTER_IN, Path);
                    if (result)
                    {
                        s_index += result;
                        printf("%u\n", SUBCHARACTER_IN->address);
                    }
                    free(SUBCHARACTER_IN);
                }
            }
        }
    }
    return s_index;
}

void null_Loaded_Addresses(hierarchys_pack hP, morf_pack mP, int defr_count, int tr_count)
{
    int tr, b, s, o, t, i;
    int m, d;

    bone * B;
    object * O;
    ikChain * I;
    vert_selection * S;
    transformer * T;
    deformer * D;
    trajectory * Trj;

    deformer_morph_map * M;
    deformer_morph * Morf;
    morph_map * oM;
    morph * oMorf;

    for (t = transformerIndex - hP.t_index; t < transformerIndex; t ++)
    {
        T = transformers[t];
        T->address = 0;
    }
    for (o = objectIndex - hP.obj_count; o < objectIndex; o ++)
    {
        O = objects[o];
        O->address = 0;
        for (s = 0; s < O->vertex_selections; s ++)
        {
            S = O->vertex_selection[s];
            S->address = 0;
        }
        for (m = 0; m < O->Morph_Maps_count; m ++)
        {
            oM = O->Morph_Maps[m];
            oM->address = 0;
            for (d = 0; d < oM->MorphsCount; d ++)
            {
                oMorf = oM->Morphs[d];
                oMorf->address = 0;
            }
        }
    }
    for (b = bonesIndex - hP.b_index; b < bonesIndex; b ++)
    {
        B = bones[b];
        B->address = 0;
    }

    for (i = iksIndex - hP.i_index; i < iksIndex; i ++)
    {
        I = ikChains[i];
        I->address = 0;
    }

    for (m = deformer_morph_map_Index - mP.dmm_index; m < deformer_morph_map_Index; m ++)
    {
        M = deformer_morph_maps[m];
        M->address = 0;
    }

    for (m = deformer_morph_Index - mP.dm_index; m < deformer_morph_Index; m ++)
    {
        Morf = deformer_morphs[m];
        Morf->address = 0;
    }

    for (d = deformerIndex - defr_count; d < deformerIndex; d ++)
    {
        D = deformers[d];
        D->address = 0;
    }

    for (tr = trjIndex - tr_count; tr < trjIndex; tr ++)
    {
        Trj = trajectories[tr];
        Trj->address = 0;
    }
}

morf_pack load_Morphs(char * path, int loaded_deformers)
{
    char Path[STRLEN];
    DIR * dir;
    struct dirent * ent;

    morf_pack mP = {0, 0};

    if ((dir = opendir(path)) != NULL)
    {
        while ((ent = readdir(dir)) != NULL)
        {
            Path[0] = '\0';
            strcat(Path, path);
            strcat(Path, "/");
            strcat(Path, ent->d_name);
            if (isFile(Path))
            {
                if (strcmp(ent->d_name, "Morphs.txt") == 0)
                {
                    printf("MORPHS\n");
                    Morphs_In * MORPHS_IN = calloc(1, sizeof(Morphs_In));
                    mP = read_Morphs_file(MORPHS_IN, Path, loaded_deformers);
                    if (result)
                    {
                        printf("%d\n", MORPHS_IN->deformermorphmapIndex);
                        printf("%d\n", MORPHS_IN->deformermorphIndex);
                    }
                    free(MORPHS_IN);
                }
            }
        }
    }
    return mP;
}

void load_Keyframes(char * path, int loaded_transformers)
{
    char Path[STRLEN];
    DIR * dir;
    struct dirent * ent;

    int result;

    if ((dir = opendir(path)) != NULL)
    {
        while ((ent = readdir(dir)) != NULL)
        {
            Path[0] = '\0';
            strcat(Path, path);
            strcat(Path, "/");
            strcat(Path, ent->d_name);
            if (isFile(Path))
            {
                if (strcmp(ent->d_name, "Keyframes.txt") == 0)
                {
                    result = 0;
                    printf("KEYFRAMES\n");
                    Keyframes_In * KEYFRAMES_IN = calloc(1, sizeof(Keyframes_In));
                    result = read_Keyframes_file(KEYFRAMES_IN, Path, loaded_transformers);
                    if (result)
                    {
                        printf("%d\n", KEYFRAMES_IN->keyframesIndex);
                    }
                    free(KEYFRAMES_IN);
                }
            }
        }
    }
}

void load_Morph_Keyframes(char * path, int loaded_objects)
{
    char Path[STRLEN];
    DIR * dir;
    struct dirent * ent;

    int result;

    if ((dir = opendir(path)) != NULL)
    {
        while ((ent = readdir(dir)) != NULL)
        {
            Path[0] = '\0';
            strcat(Path, path);
            strcat(Path, "/");
            strcat(Path, ent->d_name);
            if (isFile(Path))
            {
                if (strcmp(ent->d_name, "Morf_Keyframes.txt") == 0)
                {
                    result = 0;
                    printf("KEYFRAMES\n");
                    Morf_Keyframes_In * KEYFRAMES_IN = calloc(1, sizeof(Morf_Keyframes_In));
                    result = read_Morf_Keyframes_file(KEYFRAMES_IN, Path, loaded_objects);
                    if (result)
                    {
                        printf("%d\n", KEYFRAMES_IN->keyframesIndex);
                    }
                    free(KEYFRAMES_IN);
                }
            }
        }
    }
}

int load_Cameras(char * path, int t_index)
{
    char Path[STRLEN];
    DIR * dir;
    struct dirent * ent;

    int result;
    int cam_index = 0;

    int t, c;
    camera * Cam;
    transformer * T;

    if ((dir = opendir(path)) != NULL)
    {
        while ((ent = readdir(dir)) != NULL)
        {
            Path[0] = '\0';
            strcat(Path, path);
            strcat(Path, "/");
            strcat(Path, ent->d_name);
            if (isFile(Path))
            {
                if (strcmp(ent->d_name, "Anim Cameras.txt") == 0)
                {
                    result = 0;
                    printf("ANIM CAMERAS\n");
                    Camera_In * CAM_IN = calloc(1, sizeof(Camera_In));
                    result = read_Anim_Cameras_file(CAM_IN, Path);
                    if (result)
                    {
                        printf("%d\n", CAM_IN->cameraIndex);
                        cam_index = CAM_IN->cameraIndex;
                    }
                    free(CAM_IN);
                }
            }
        }
    }

    for (c = camIndex - cam_index; c < camIndex; c ++)
    {
        Cam = cameras[c];

        for (t = transformerIndex - t_index; t < transformerIndex; t ++)
        {
            T = transformers[t];

            if (T->address == (unsigned)Cam->T)
            {
                Cam->T = T;
            }
        }
    }

    return cam_index;
}

int load_Trajectories(char * path, int t_index, int c_index)
{
    char Path[STRLEN];
    DIR * dir;
    struct dirent * ent;

    int result;
    int tr_index = 0;

    int i, t, tr, c;
    trajectory * Trj;
    transformer * T, * T_Trj;
    curve * C;

    if ((dir = opendir(path)) != NULL)
    {
        while ((ent = readdir(dir)) != NULL)
        {
            Path[0] = '\0';
            strcat(Path, path);
            strcat(Path, "/");
            strcat(Path, ent->d_name);
            if (isFile(Path))
            {
                if (strcmp(ent->d_name, "Trajectories.txt") == 0)
                {
                    result = 0;
                    printf("TRAJECTORIES\n");
                    Trajectory_In * TRAJE_IN = calloc(1, sizeof(Trajectory_In));
                    result = read_Trajectories_file(TRAJE_IN, Path);
                    if (result)
                    {
                        printf("%d\n", TRAJE_IN->trajectoryIndex);
                        tr_index = TRAJE_IN->trajectoryIndex;
                    }
                    free(TRAJE_IN);
                }
            }
        }
    }

    for (tr = trjIndex - tr_index; tr < trjIndex; tr ++)
    {
        Trj = trajectories[tr];

        for (i = 0; i < Trj->transformers_count; i ++)
        {
            T_Trj = Trj->Transformers[i];
            for (t = transformerIndex - t_index; t < transformerIndex; t ++)
            {
                T = transformers[t];
                if (T->address == (unsigned)T_Trj)
                {
                    Trj->Transformers[i] = T;
                    T->Trj = (traj*)Trj;
                }
            }
        }

        for (c = curvesIndex - c_index; c < curvesIndex; c ++)
        {
            C = curves[c];

            if (C->address == (unsigned)Trj->Curve)
            {
                C->Trj = (traj*)Trj;
                Trj->Curve = C;
            }
        }
    }

    return tr_index;
}

int load_Curves(char * path, int obj_count)
{
    char Path[STRLEN];
    DIR * dir;
    struct dirent * ent;

    int result;
    int c_index = 0;
    int s_index = 0;
    int p_index = 0;

    if ((dir = opendir(path)) != NULL)
    {
        while ((ent = readdir(dir)) != NULL)
        {
            Path[0] = '\0';
            strcat(Path, path);
            strcat(Path, "/");
            strcat(Path, ent->d_name);
            if (isFile(Path))
            {
                if (strcmp(ent->d_name, "Curves.txt") == 0)
                {
                    result = 0;
                    printf("CURVES\n");
                    Curves_In * CURVES_IN = calloc(1, sizeof(Curves_In));
                    result = read_Curves_file(CURVES_IN, Path);
                    if (result)
                    {
                        printf("%d\n", CURVES_IN->curvesIndex);
                        c_index = CURVES_IN->curvesIndex;
                    }
                    free(CURVES_IN);
                }
                else if (strcmp(ent->d_name, "Segments.txt") == 0)
                {
                    result = 0;
                    printf("SEGMENTS\n");
                    Segments_In * SEGMENTS_IN = calloc(1, sizeof(Segments_In));
                    result = read_Segments_file(SEGMENTS_IN, Path);
                    if (result)
                    {
                        printf("%d\n", SEGMENTS_IN->segmentIndex);
                        s_index = SEGMENTS_IN->segmentIndex;
                    }
                    free(SEGMENTS_IN);
                }
                else if (strcmp(ent->d_name, "Cps.txt") == 0)
                {
                    result = 0;
                    printf("CPS\n");
                    Cps_In * CPS_IN = calloc(1, sizeof(Cps_In));
                    result = read_Cps_file(CPS_IN, Path);
                    if (result)
                    {
                        printf("%d\n", CPS_IN->cpsIndex);
                        p_index = CPS_IN->cpsIndex;
                    }
                    free(CPS_IN);
                }
            }
        }
    }

    if (c_index && s_index && p_index)
    {
        int c, o, s, t, p, e, v, condition;

        vertex * V;
        edge * E;
        curve_segment * S, * S0;
        cp * CP, * CP0;
        curve * C;
        object * O;

        /* make object and curve association */

        for (c = curvesIndex - c_index; c < curvesIndex; c ++)
        {
            C = curves[c];

            condition = 1;

            for (o = objectIndex - obj_count; o < objectIndex; o ++)
            {
                O = objects[o];

                if (O->address == (unsigned)C->O)
                {
                    C->O = O;
                    condition = 0;
                    if (O->curve_count < OBJECT_CURVES)
                        O->curves[O->curve_count ++] = C;
                    break;
                }
            }

            if (condition)
            {
                C->O = NULL;
            }
        }

        /* make curve and segment association */

        for (c = curvesIndex - c_index; c < curvesIndex; c ++)
        {
            C = curves[c];

            for (t = 0; t < C->segment_count; t ++)
            {
                S0 = C->segments[t];

                for (s = segmentIndex - s_index; s < segmentIndex; s ++)
                {
                    S = segments[s];

                    if (S->address == (unsigned)S0)
                    {
                        C->segments[t] = S;
                        S->Curve = C;
                        break;
                    }
                }
            }
        }

        /* make curve cps associations */

        for (c = curvesIndex - c_index; c < curvesIndex; c ++)
        {
            C = curves[c];

            for (t = 0; t < C->cps_count; t ++)
            {
                CP0 = C->cps[t];

                for (p = cpsIndex - p_index; p < cpsIndex; p ++)
                {
                    CP = cps[p];

                    if (CP->address == (unsigned)CP0)
                    {
                        C->cps[t] = CP;
                        break;
                    }
                }
            }
        }

        /* for each curve associate its object edges with its segments */

        for (c = curvesIndex - c_index; c < curvesIndex; c ++)
        {
            C = curves[c];

            if (C->O != NULL)
            {
                for (e = 0; e < C->O->edgecount; e ++)
                {
                    E = &C->O->edges[e / ARRAYSIZE][e % ARRAYSIZE];

                    for (s = 0; s < C->segment_count; s ++)
                    {
                        S = C->segments[s];

                        if (S->address == (unsigned)E->S)
                        {
                            E->S = S;
                            S->E = E;
                            break;
                        }
                    }
                }
            }
        }

        /* associate curves cps with their object vertex */

        for (c = curvesIndex - c_index; c < curvesIndex; c ++)
        {
            C = curves[c];

            if (C->O != NULL)
            {
                for (v = 0; v < C->O->vertcount; v ++)
                {
                    V = &C->O->verts[v / ARRAYSIZE][v % ARRAYSIZE];

                    for (p = 0; p < C->cps_count; p ++)
                    {
                        CP = C->cps[p];

                        if (CP->address == (unsigned)V->control_point)
                        {
                            V->control_point = CP;
                            CP->vert = V;
                            break;
                        }
                    }
                }
            }
        }

        /* associate cp segments */

        for (c = curvesIndex - c_index; c < curvesIndex; c ++)
        {
            C = curves[c];

            for (t = 0; t < C->cps_count; t ++)
            {
                CP = C->cps[t];

                for (p = 0; p < CP->segment_count; p ++)
                {
                    S0 = CP->segments[p];

                    for (s = segmentIndex - s_index; s < segmentIndex; s ++)
                    {
                        S = segments[s];

                        if (S->address == (unsigned)S0)
                        {
                            CP->segments[p] = S;
                            break;
                        }
                    }
                }
            }
        }
    }

    return c_index;
}

hierarchys_pack load_Hierarchys(char * path, int obj_count, int defr_count, int subcharacter_count)
{
    hierarchys_pack hP;

    char Path[STRLEN];
    DIR * dir;
    struct dirent * ent;

    int result, b_index, t_index, i_index, c_index;
    unsigned w_address;

    w_address = 0;
    t_index = 0;
    b_index = 0;
    i_index = 0;
    c_index = 0;

    if ((dir = opendir(path)) != NULL)
    {
        while ((ent = readdir(dir)) != NULL)
        {
            Path[0] = '\0';
            strcat(Path, path);
            strcat(Path, "/");
            strcat(Path, ent->d_name);
            if (isFile(Path))
            {
                if (strcmp(ent->d_name, "World.txt") == 0)
                {
                    result = 0;
                    printf("WORLD\n");
                    World_In * WORLD_IN = calloc(1, sizeof(World_In));
                    result = read_World_file(WORLD_IN, Path);
                    if (result)
                    {
                        printf("%u\n", WORLD_IN->address);
                        w_address = WORLD_IN->address;

                        update_Light(Light_Themes[currentTheme]);
                        if (SHADERS)
                        {
                            init_lights();
                            init_Hint();
                        }
                    }
                    free(WORLD_IN);
                }
                else if (strcmp(ent->d_name, "Cameras.txt") == 0)
                {
                    result = 0;
                    printf("CAMERAS\n");
                    result = read_Cameras_file(Path);
                    if (result)
                    {
                        Camera_Persp.T->bind_set = 0;
                        Camera_Front.T->bind_set = 0;
                        Camera_Left.T->bind_set = 0;
                        Camera_Top.T->bind_set = 0;
                        printf("%f\n", CamDist);
                    }
                }
                else if (strcmp(ent->d_name, "Locators.txt") == 0)
                {
                    result = 0;
                    printf("LOCATORS\n");
                    Locators_In * LOC_IN = calloc(1, sizeof(Locators_In));
                    result = read_Locators_file(LOC_IN, Path);
                    if (result)
                    {
                        printf("%u\n", LOC_IN->locatorIndex);
                        t_index = LOC_IN->locatorIndex;
                    }
                    free(LOC_IN);
                }
                else if (strcmp(ent->d_name, "Bones.txt") == 0)
                {
                    result = 0;
                    printf("BONES\n");
                    Bones_In * BONES_IN = calloc(1, sizeof(Bones_In));
                    result = read_Bones_file(BONES_IN, Path);
                    if (result)
                    {
                        printf("%u\n", BONES_IN->bonesIndex);
                        b_index = BONES_IN->bonesIndex;
                    }
                    free(BONES_IN);
                }
                else if (strcmp(ent->d_name, "ikChains.txt") == 0)
                {
                    result = 0;
                    printf("IKCHAINS\n");
                    ikChains_In * IKCHAINS_IN = calloc(1, sizeof(ikChains_In));
                    result = read_ikChains_file(IKCHAINS_IN, Path);
                    if (result)
                    {
                        printf("%u\n", IKCHAINS_IN->chainsIndex);
                        i_index = IKCHAINS_IN->chainsIndex;
                    }
                    free(IKCHAINS_IN);
                }
                else if (strcmp(ent->d_name, "Constraints.txt") == 0)
                {
                    result = 0;
                    printf("CONSTRAINTS\n");
                    constraints_In * CONSTRAINTS_IN = calloc(1, sizeof(constraints_In));
                    result = read_Constraints_file(CONSTRAINTS_IN, Path);
                    if (result)
                    {
                        printf("%u\n", CONSTRAINTS_IN->constraintsIndex);
                        c_index = CONSTRAINTS_IN->constraintsIndex;
                    }
                    free(CONSTRAINTS_IN);
                }
            }
        }
    }

    if (subcharacter_count && w_address && t_index && b_index)
    {
        int s, t, b, p;

        subcharacter * S;
        transformer * T, * T0;
        bone * B, * B0;

        for (s = subcharacterIndex - subcharacter_count; s < subcharacterIndex; s ++)
        {
            S = subcharacters[s];

            for (t = 0; t < S->Transformers_Count; t ++)
            {
                T = S->Transformers[t];

                for (p = transformerIndex - t_index; p < transformerIndex; p ++)
                {
                    T0 = transformers[p];
                    if (T0->address == (unsigned)T)
                    {
                        S->Transformers[t] = T0;
                        break;
                    }
                }
            }

            for (b = 0; b < S->Bones_Count; b ++)
            {
                B = S->Bones[b];

                for (p = bonesIndex - b_index; p < bonesIndex; p ++)
                {
                    B0 = bones[p];
                    if (B0->address == (unsigned)B)
                    {
                        S->Bones[b] = B0;
                        break;
                    }
                }
            }
        }
    }

    if (w_address && t_index)
    {
        int b, v, s, o, c, t, p;

        bone * B;
        object * O;
        vert_selection * VS, * VS0;
        transformer * T, * T0;

        for (t = transformerIndex - t_index; t < transformerIndex; t ++)
        {
            //T = Locators[t];
            T = transformers[t];
            if ((unsigned)T->parent == w_address)
            {
                add_Child(T, &World);
            }
            else
            {
                for (p = transformerIndex - t_index; p < transformerIndex; p ++)
                {
                    //T0 = Locators[p];
                    T0 = transformers[p];
                    if (T0->address == (unsigned)T->parent)
                    {
                        T->parent = T0;
                        break;
                    }
                }
            }
            for (c = 0; c < T->childcount; c ++)
            {
                for (p = transformerIndex - t_index; p < transformerIndex; p ++)
                {
                    //T0 = Locators[p];
                    T0 = transformers[p];
                    if (T0->address == (unsigned)T->childs[c])
                    {
                        T->childs[c] = T0;
                        break;
                    }
                }
            }
            for (v = 0; v < T->Selections_Count; v ++)
            {
                VS = T->Selections[v];
                for (o = objectIndex - obj_count; o < objectIndex; o ++)
                {
                    O = objects[o];
                    for (s = 0; s < O->vertex_selections; s ++)
                    {
                        VS0 = O->vertex_selection[s];
                        if (VS0->address == (unsigned)VS)
                        {
                            T->Selections[v] = VS0;
                            T->Selections[v]->Transformer = T;
                            //printf("T %s S %s\n", T->Name, VS->Name);
                            break;
                        }
                    }
                }
            }
            for (b = bonesIndex - b_index; b < bonesIndex; b ++)
            {
                B = bones[b];
                if (B->address == (unsigned)T->Bone)
                {
                    T->Bone = B;
                    break;
                }
            }
            // T->Deformer = NULL;
            // deformers after Deformers
        }
    }
    if (w_address && t_index && b_index)
    {
        int t, p, b;

        transformer * T;
        bone * B, * B0;

        /*
        int index;
        unsigned address; // assigned after loading
        char * Name;
        int selected;
        bone * parent;
        float rotVec_[3][3];
        float rotVec_I[3][3];
        transformer * A;
        transformer * B;
        deformer * D;
        float ** Distances;
        int Distances_Count;
        */

        int condition;

        for (b = bonesIndex - b_index; b < bonesIndex; b ++)
        {
            B = bones[b];
            condition = 1;
            for (p = bonesIndex - b_index; p < bonesIndex; p ++)
            {
                B0 = bones[p];
                if (B0->address == (unsigned)B->parent)
                {
                    condition = 0;
                    B->parent = B0;
                    break;
                }
            }
            if (condition)
            {
                B->parent = NULL;
            }

            condition = 0;

            for (t = transformerIndex - t_index; t < transformerIndex; t ++)
            {
                //T = Locators[t];
                T = transformers[t];
                if (T->address == (unsigned)B->A)
                {
                    B->A = T;
                    condition ++;
                }
                else if (T->address == (unsigned)B->B)
                {
                    B->B = T;
                    condition ++;
                }
                if (condition >= 2)
                {
                    break;
                }
            }

            // deformers after Deformers
        }
    }

    if (w_address && t_index && b_index && i_index)
    {
        int t, i, p, b;

        ikChain * I;
        bone * B;
        transformer * T;

        /*
        int index;
        unsigned address; // assigned after loading
        char * Name;
        int selected;
        deformer * Deformer;
        int bonescount;
        bone ** Bones;
        float sum_length; // sum of bone lengths
        vec3 * vectors; // transition into stretched pose
        vec3 * vectors_bone; // in between bone vectors
        vec3 * positions_A; // A points in the chain
        vec3 * positions_B; // B points in the chain
        vec3 * bones_Rot; // Bones rotational axis
        transformer * A;
        transformer * B;
        float rotVec_0[3][3]; // intermediate matrix
        float rotVec_1[3][3]; // final pose matrix
        float poleRot;
        direction_Pack P;
        */

        int condition;

        for (i = iksIndex - i_index; i < iksIndex; i ++)
        {
            I = ikChains[i];
            for (p = 0; p < I->bonescount; p ++)
            {
                condition = 1;
                for (b = bonesIndex - b_index; b < bonesIndex; b ++)
                {
                    B = bones[b];
                    if (B->address == (unsigned)I->Bones[p])
                    {
                        condition = 0;
                        I->Bones[p] = B;
                        break;
                    }
                }
                if (condition)
                {
                    I->Bones[p] = NULL;
                }
            }

            condition = 0;

            for (t = transformerIndex - t_index; t < transformerIndex; t ++)
            {
                //T = Locators[t];
                T = transformers[t];
                if (T->address == (unsigned)I->A)
                {
                    I->A = T;
                    condition ++;
                }
                else if (T->address == (unsigned)I->B)
                {
                    I->B = T;
                    condition ++;
                }
                if (condition >= 2)
                {
                    break;
                }
            }
        }

        for (b = bonesIndex - b_index; b < bonesIndex; b ++)
        {
            B = bones[b];
            condition = 1;
            for (i = iksIndex - i_index; i < iksIndex; i ++)
            {
                I = ikChains[i];
                if (I->address == (unsigned)B->IK)
                {
                    condition = 0;
                    B->IK = I;
                    break;
                }
            }
            if (condition)
            {
                B->IK = NULL;
            }
        }

        for (t = transformerIndex - t_index; t < transformerIndex; t ++)
        {
            T = transformers[t];
            condition = 1;
            for (i = iksIndex - i_index; i < iksIndex; i ++)
            {
                I = ikChains[i];
                if (I->address == (unsigned)T->IK)
                {
                    condition = 0;
                    T->IK = I;
                    break;
                }
            }
            if (condition)
            {
                T->IK = NULL;
            }
        }
    }

   if (w_address && t_index && i_index && c_index)
    {
        int t, i, c;

        ikChain * I;
        constraint * C;
        transformer * T;

        int condition;

        for (i = iksIndex - i_index; i < iksIndex; i ++)
        {
            I = ikChains[i];

            condition = 1;

            for (c = constraintsIndex - c_index; c < constraintsIndex; c ++)
            {
                C = constraints[c];
                if (C->address == (unsigned)I->C)
                {
                    condition = 0;
                    I->C = C;
                    break;
                }
            }
            if (condition)
            {
                I->C = NULL;
            }
        }

        for (i = iksIndex - i_index; i < iksIndex; i ++)
        {
            I = ikChains[i];

            condition = 1;

            for (c = constraintsIndex - c_index; c < constraintsIndex; c ++)
            {
                C = constraints[c];
                if (C->address == (unsigned)I->Pole)
                {
                    condition = 0;
                    I->Pole = C;
                    break;
                }
            }
            if (condition)
            {
                I->Pole = NULL;
            }
        }

        for (c = constraintsIndex - c_index; c < constraintsIndex; c ++)
        {
            C = constraints[c];

            for (t = transformerIndex - t_index; t < transformerIndex; t ++)
            {
                T = transformers[t];
                if (T->address == (unsigned)C->Locator)
                {
                    C->Locator = T;
                    T->Constraint = C;
                    break;
                }
            }

            for (t = transformerIndex - t_index; t < transformerIndex; t ++)
            {
                T = transformers[t];
                if (T->address == (unsigned)C->IK_goal)
                {
                    C->IK_goal = T;
                    T->Constraint = C;
                    break;
                }
            }
        }
    }

    if (defr_count)
    {
        int b, s, o, t, i, d, k;

        bone * B;
        object * O;
        deformer * D;
        ikChain * I;
        transformer * T;
        vert_selection * S, * S0;

        /*
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
        */

        for (d = deformerIndex - defr_count; d < deformerIndex; d ++)
        {
            D = deformers[d];

            for (i = 0; i < D->Transformers_Count; i ++)
            {
                for (t = transformerIndex - t_index; t < transformerIndex; t ++)
                {
                    //T = Locators[t];
                    T = transformers[t];
                    if (T->address == (unsigned)D->Transformers[i])
                    {
                        //T->address = 0;
                        D->Transformers[i] = T;
                        T->Deformer = D;
                        break;
                    }
                }
            }
            for (i = 0; i < D->Selections_Count; i ++)
            {
                S = D->Selections[i];

                for (o = objectIndex - obj_count; o < objectIndex; o ++)
                {
                    O = objects[o];
                    for (s = 0; s < O->vertex_selections; s ++)
                    {
                        S0 = O->vertex_selection[s];
                        if (S0->address == (unsigned)S)
                        {
                            //S0->address = 0;
                            D->Selections[i] = S0;
                            break;
                        }
                    }
                }
            }
            for (i = 0; i < D->Objects_Count; i ++)
            {
                for (o = objectIndex - obj_count; o < objectIndex; o ++)
                {
                    O = objects[o];
                    if (O->address == (unsigned)D->Objects[i])
                    {
                        //O->address = 0;
                        D->Objects[i] = O;
                        break;
                    }
                }
            }
            for (i = 0; i < D->Bones_Count; i ++)
            {
                for (b = bonesIndex - b_index; b < bonesIndex; b ++)
                {
                    B = bones[b];
                    if (B->address == (unsigned)D->Bones[i])
                    {
                        //B->address = 0;
                        D->Bones[i] = B;
                        D->Bones[i]->D = D;
                        break;
                    }
                }
            }
            for (i = 0; i < D->IKchains_Count; i ++)
            {
                for (k = iksIndex - i_index; k < iksIndex; k ++)
                {
                    I = ikChains[k];
                    if (I->address == (unsigned)D->IKchains[i])
                    {
                        D->IKchains[i] = I;
                        D->IKchains[i]->Deformer = D;
                        break;
                    }
                }
            }
        }
    }

    if (w_address && t_index && obj_count)
    {
        int o, t;

        object * O;
        transformer * T;

        for (o = objectIndex - obj_count; o < objectIndex; o ++)
        {
            O = objects[o];
            for (t = transformerIndex - t_index; t < transformerIndex; t ++)
            {
                T = transformers[t];
                if (O->address == (unsigned)T->Object)
                {
                    O->T = transformers[t];
                    T->Object = O;
                    break;
                }
            }
        }
    }

    hP.t_index = t_index;
    hP.obj_count = obj_count;
    hP.b_index = b_index;
    hP.i_index = i_index;

    return hP;

    //null_Loaded_Addresses(t_index, obj_count, b_index, i_index);
}

void assign_Poses(int pose_count, int defr_count)
{
    if (defr_count)
    {
        int p, i, d;

        pose * P, * P0;
        deformer * D;

        /*
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
        */

        for (d = deformerIndex - defr_count; d < deformerIndex; d ++)
        {
            D = deformers[d];

            for (i = 0; i < D->Poses_Count; i ++)
            {
                P = D->Poses[i];
                for (p = posesIndex - pose_count; p < posesIndex; p ++)
                {
                    P0 = poses[p];
                    if (P0->address == (unsigned)P)
                    {
                        P0->address = 0;
                        D->Poses[i] = P0;
                        D->Poses[i]->D = D;
                        break;
                    }
                }
            }
        }
    }
}

void assign_Subcharacter_Poses(int subcharacter_count, int subcharacter_poses_count, int defr_count)
{
    if (defr_count)
    {
        int s, i, d, p;

        pose * P, * P0;
        subcharacter * S;
        deformer * D;

        for (p = subcharacter_posesIndex - subcharacter_poses_count; p < subcharacter_posesIndex; p ++)
        {
            P = subcharacter_Poses[p];

            for (d = deformerIndex - defr_count; d < deformerIndex; d ++)
            {
                D = deformers[d];

                if (D->address == (unsigned)P->D)
                {
                    P->D = D;
                    break;
                }
            }
        }

        for (s = subcharacterIndex - subcharacter_count; s < subcharacterIndex; s ++)
        {
            S = subcharacters[s];

            for (i = 0; i < S->Poses_Count; i ++)
            {
                P = S->Poses[i];

                for (p = subcharacter_posesIndex - subcharacter_poses_count; p < subcharacter_posesIndex; p ++)
                {
                    P0 = subcharacter_Poses[p];
                    if (P0->address == (unsigned)P)
                    {
                        S->Poses[i] = P0;
                        break;
                    }
                }
            }
        }
    }
}

void assign_Object_Dialer(int morph_count, int obj_count)
{
    int m, d, o;

    object * O;
    object_morph_dialer * OMD;
    deformer_morph * Morph;

    for (m = deformer_morph_Index - morph_count; m < deformer_morph_Index; m ++)
    {
        Morph = deformer_morphs[m];

        for (d = 0; d < Morph->objectCount; d ++)
        {
            OMD = Morph->Object_Morph_Map[d];

            for (o = objectIndex - obj_count; o < objectIndex; o ++)
            {
                O = objects[o];
                if (O->address == (unsigned)OMD->O)
                {
                    OMD->O = O;
                    break;
                }
            }
        }
    }
}

void associate_Object_Morphs(int obj_count, int defr_count)
{
    if (defr_count)
    {
        int o, d, m, i, f, k;

        object * O;
        morph * OD;
        morph_map * OM;

        deformer_morph * Morph;
        deformer_morph_map * M;
        deformer * D;

        printf("obj_count %d %d\n", obj_count, objectIndex);

        for (o = objectIndex - obj_count; o < objectIndex; o ++)
        {
            O = objects[o];
            printf("%u %s\n", (unsigned)O->address, O->Name);

            for (m = 0; m < O->Morph_Maps_count; m ++)
            {
                OM = O->Morph_Maps[m];

                printf("ASSOCIATE: %u, %s %d %d\n", (unsigned)OM->Object, OM->Name, OM->MorphsCount, OM->VertCount);

                if (O->address != (unsigned)OM->Object)
                {
                    printf("Error, mismatch between loaded objects and object morph!\n");
                }
                OM->Object = O;

                for (d = deformerIndex - defr_count; d < deformerIndex; d ++)
                {
                    D = deformers[d];

                    if ((unsigned)OM->Deformer == D->address)
                    {
                        printf("Deformer found!\n");
                        OM->Deformer = D;

                        for (i = 0; i < D->Morph_Maps_Count; i ++)
                        {
                            M = D->Morph_Maps[i];

                            if ((unsigned)OM->DM == M->address)
                            {
                                printf("\tDeformer map found!\n");
                                OM->DM = M;

                                for (f = 0; f < M->Morphs_Count; f ++)
                                {
                                    Morph = M->Morphs[f];

                                    for (k = 0; k < OM->MorphsCount; k ++)
                                    {
                                        OD = OM->Morphs[k];

                                        if ((unsigned)OD->M == Morph->address)
                                        {
                                            printf("\t\tMorph Map found!\n");
                                            OD->M = Morph;
                                            break;
                                        }
                                    }
                                }
                                break;
                            }
                        }
                        break;
                    }
                }
            }
        }
    }
}

void assign_Deformer_Morphs(int morph_count, int defr_count)
{
    if (defr_count)
    {
        int m, i, d, f;

        deformer_morph * Morph, * Morph0;
        deformer_morph_map * M;
        deformer * D;

        for (d = deformerIndex - defr_count; d < deformerIndex; d ++)
        {
            D = deformers[d];

            for (i = 0; i < D->Morph_Maps_Count; i ++)
            {
                M = D->Morph_Maps[i];

                for (m = 0; m < M->Morphs_Count; m ++)
                {
                    Morph = M->Morphs[m];
                    for (f = deformer_morph_Index - morph_count; f < deformer_morph_Index; f ++)
                    {
                        Morph0 = deformer_morphs[f];
                        if ((unsigned)Morph == Morph0->address)
                        {
                            M->Morphs[m] = Morph0;
                            M->Morphs[m]->D = D;
                            M->Morphs[m]->Map = M;
                            break;
                        }
                    }
                }
            }
        }
    }
}

void assign_Deformer_Morph_Maps(int morph_map_count, int defr_count, int obj_count)
{
    if (defr_count)
    {
        int m, i, d, o, f;

        object * O, * O0;
        deformer_morph_map * M, * M0;
        deformer * D;

        for (d = deformerIndex - defr_count; d < deformerIndex; d ++)
        {
            D = deformers[d];

            for (i = 0; i < D->Morph_Maps_Count; i ++)
            {
                M = D->Morph_Maps[i];
                for (m = deformer_morph_map_Index - morph_map_count; m < deformer_morph_map_Index; m ++)
                {
                    M0 = deformer_morph_maps[m];
                    if (M0->address == (unsigned)M)
                    {
                        //S0->address = 0;
                        D->Morph_Maps[i] = M0;
                        D->Morph_Maps[i]->Deformer = D;
                        break;
                    }
                }
            }

            for (i = 0; i < D->Morph_Maps_Count; i ++)
            {
                M = D->Morph_Maps[i];
                for (f = 0; f < M->Object_Count; f ++)
                {
                    O0 = M->Objects[f];
                    for (o = objectIndex - obj_count; o < objectIndex; o ++)
                    {
                        O = objects[o];
                        if (O->address == (unsigned)O0)
                        {
                            M->Objects[f] = O;
                            break;
                        }
                    }
                }
            }
        }
    }
}

void assign_Subcharacters(int subcharacter_count, int defr_count)
{
    if (defr_count)
    {
        int s, i, d, p;

        subcharacter * S, * S0, * S1;
        deformer * D;

        for (d = deformerIndex - defr_count; d < deformerIndex; d ++)
        {
            D = deformers[d];

            for (i = 0; i < D->Subcharacters_Count; i ++)
            {
                S = D->Subcharacters[i];
                for (s = subcharacterIndex - subcharacter_count; s < subcharacterIndex; s ++)
                {
                    S0 = subcharacters[s];
                    if (S0->address == (unsigned)S)
                    {
                        //S0->address = 0;
                        D->Subcharacters[i] = S0;
                        D->Subcharacters[i]->Deformer = D;
                        break;
                    }
                }
            }
        }

        for (s = subcharacterIndex - subcharacter_count; s < subcharacterIndex; s ++)
        {
            S = subcharacters[s];

            for (i = 0; i < S->Subcharacters_Count; i ++)
            {
                S0 = S->Subcharacters[i];

                for (p = subcharacterIndex - subcharacter_count; p < subcharacterIndex; p ++)
                {
                    S1 = subcharacters[p];
                    if (S1->address == (unsigned)S0)
                    {
                        S->Subcharacters[i] = S1;
                        break;
                    }
                }
            }
        }
    }
}

void validate_Loading()
{
    validate_Transformers(&World);
    validate_Selections();
    validate_Bones();
    validate_Deformers();
    validate_Poses();
}

#endif // LOADING_H_INCLUDED
