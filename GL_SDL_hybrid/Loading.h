/*
The MIT License

Copyright <2018> <Allan Kiipli>
*/

#ifndef LOADING_H_INCLUDED
#define LOADING_H_INCLUDED

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
    char Name[STRLEN];
    int textcount;
    int vertcount;
    int icolcount;
    int tripcount;
    int edgecount;
    int uvedcount;
    int polycount;
}
Object_In;

void assign_Texture(object * O);

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
            sscanf(buff, "%d %d %d %d %d %d",
                    &D->collapsed,
                    &D->Transformers_Count,
                    &D->Selections_Count,
                    &D->Objects_Count,
                    &D->Bones_Count,
                    &D->Poses_Count);

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
            if (fgets(buff, BUF_SIZE, fp))
                sscanf(buff, "%d", &D->current_pose);
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

                sscanf(buff, "%f %f %f %f %f %d %d %d %d %f", &M->R, &M->G, &M->B, &M->A, &M->Shininess, &M->normal, &M->Normal_idx, &M->bump, &M->Bump_idx, &M->Displacement);

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
                            assign_Texture(O);
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
                sscanf(buff, "%d %d %d %d %d %d",
                        &T->style,
                        &T->childcount,
                        &T->collapsed,
                        &T->rot_Order,
                        &T->bind_set,
                        &T->Selections_Count);

                fgets(buff, BUF_SIZE, fp);
                sscanf(buff, "%u %u %u %u",
                        (unsigned*)&T->parent,
                        (unsigned*)&T->Object,
                        (unsigned*)&T->Deformer,
                        (unsigned*)&T->Bone);

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

    if (fgets(buff, BUF_SIZE, fp))
    {
        if (strcmp("Object\n", buff) == 0)
        {
            fgets(buff, BUF_SIZE, fp);
            sscanf(buff, "%s", OB_IN->Name);
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

                sprintf(O->Name, "%s", OB_IN->Name);
                O->address = OB_IN->address;
                printf("%u\n", O->address);

                O->surface = 0;

                fgets(buff, BUF_SIZE, fp);
                sscanf(buff, "%d %d", &O->deforms, &O->binding);

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

                //printf("DIAGNOS %d %d\n", j, V->uv_vertcount);

//                fprintf(F, "vert %d %d %d\n",
//                V->index,
//                V->edgecount,
//                V->uv_vertcount);
//                fprintf(F, " weight %f\n", V->weight);
//                fprintf(F, " coord %f %f %f\n", V->x, V->y, V->z);
//                int i;
//                fprintf(F, " edges");
//                for(i = 0; i < V->edgecount; i++) fprintf(F, " %d", V->edges[i]);
//                fprintf(F, "\n");
//                fprintf(F, " uvvrs");
//                for(i = 0; i < V->uv_vertcount; i++) fprintf(F, " %d", V->uv_verts[i]);
//                fprintf(F, "\n");
            }

//            triangle * T;
//
//            for (i = 0; i < O->tripcount; i ++)
//            {
//                T = &O->trips[i / ARRAYSIZE][i % ARRAYSIZE];
//                fgets(buff, BUF_SIZE, fp);
//                sscanf(buff, "%*s %d %d %d", &T->index, &T->poly, &T->surface);
//                fgets(buff, BUF_SIZE, fp);
//                sscanf(buff, "%*s %d %d %d", &T->verts[0], &T->verts[1], &T->verts[2]);
//                fgets(buff, BUF_SIZE, fp);
//                sscanf(buff, "%*s %d %d %d", &T->texts[0], &T->texts[1], &T->texts[2]);
//                fgets(buff, BUF_SIZE, fp);
//
////                fprintf(F, "trip %d %d %d",
////                T->index,
////                T->poly,
////                T->surface);
////                int i;
////                fprintf(F, "\n verts");
////                for(i = 0; i < 3; i++) fprintf(F, " %d", T->verts[i]);
////                fprintf(F, "\n texts");
////                for(i = 0; i < 3; i++) fprintf(F, " %d", T->texts[i]);
////                fprintf(F, "\n");
//            }

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

//                fprintf(F, "edge %d %d %d\n",
//                E->index,
//                E->uv_edcount,
//                E->polycount);
//                fprintf(F, " weight %f\n", E->weight);
//                int i;
//                fprintf(F, " verts");
//                for(i = 0; i < 2; i++) fprintf(F, " %d", E->verts[i]);
//                fprintf(F, "\n polys");
//                for(i = 0; i < E->polycount; i++) fprintf(F, " %d", E->polys[i]);
//                fprintf(F, "\n slots");
//                for(i = 0; i < E->polycount; i++) fprintf(F, " %d", E->slots[i]);
//                fprintf(F, "\n uveds");
//                for(i = 0; i < E->uv_edcount; i++) fprintf(F, " %d", E->uv_edges[i]);
//                fprintf(F, "\n");
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

void load_Selections(char * path)
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
                    int result = 0;
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

void null_Loaded_Addresses(int t_index, int obj_count, int b_index)
{
    int b, s, o, t;

    bone * B;
    object * O;
    vert_selection * S;
    transformer * T;

    for (t = transformerIndex - t_index; t < transformerIndex; t ++)
    {
        T = transformers[t];
        T->address = 0;
    }
    for (o = objectIndex - obj_count; o < objectIndex; o ++)
    {
        O = objects[o];
        O->address = 0;
        for (s = 0; s < O->vertex_selections; s ++)
        {
            S = O->vertex_selection[s];
            S->address = 0;
        }
    }
    for (b = bonesIndex - b_index; b < bonesIndex; b ++)
    {
        B = bones[b];
        B->address = 0;
    }
}

void load_Hierarchys(char * path, int obj_count, int defr_count)
{
    char Path[STRLEN];
    DIR * dir;
    struct dirent * ent;

    int result, b_index, t_index;
    unsigned w_address;

    w_address = 0;
    t_index = 0;
    b_index = 0;

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

    if (defr_count)
    {
        int b, s, o, t, i, d;

        bone * B;
        object * O;
        deformer * D;
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

    null_Loaded_Addresses(t_index, obj_count, b_index);
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

void validate_Loading()
{
    validate_Transformers(&World);
    validate_Selections();
    validate_Bones();
    validate_Deformers();
    validate_Poses();
}

#endif // LOADING_H_INCLUDED
