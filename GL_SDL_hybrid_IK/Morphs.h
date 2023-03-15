/*
The MIT License

Copyright <2022> <Allan Kiipli>
*/

#ifndef MORPHS_H_INCLUDED
#define MORPHS_H_INCLUDED

#define MORPH_MAPS 1000
#define OBJECT_MORPHS 10000
#define MORPHS 10000
#define DEFORMER_MORPH_MAPS 1000
#define DEFORMER_MORPHS 10000

int Morph_type = 0;

int morph_mapIndex = 0;
int morphsIndex = 0;

int start_Morph;
int Morphs_c = 0;
char Morph_Names[MORPHS][STRLEN];
int MorphIndex = 0;
int morph_start = 0;

int Morph_List[MORPHS];
int current_Morph_Map = 0;
int currentMorph = 0;

int Morph_X_Offset[MORPHS];
int Morph_X_Collapsed[MORPHS];

int Deformer_Morphs_c = 0;
int Deformer_Morph_Maps_c = 0;

int Object_Node_c = 0;
int Object_Morphs_c = 0;
int Object_Morph_Maps_c = 0;

char morph_type[TYPE_LENGTH];
char * morph_types[MORPH_NUM];
int morph_types_c = 2;

#define MORPH_TYPE_DEFORMER "Deformer"
#define MORPH_TYPE_OBJECT "Object"

void init_morph_types()
{
    int i;

	for (i = 0; i < MORPH_NUM; i ++)
    {
        morph_types[i] = malloc(TYPE_LENGTH * sizeof(char));
    }

    morph_types[0] = MORPH_TYPE_DEFORMER;
    morph_types[1] = MORPH_TYPE_OBJECT;

    memcpy(&morph_type, morph_types[0], strlen(morph_types[0]));
}

typedef struct
{
    int index;
    float weight; // morph maps can overlap and weight is proportion of overlap
}
weighted_index;

weighted_index verts_selection[OBJECT_CPS];
int vert_counter = 0;

typedef struct
{
    float x;
    float y;
    float z;
}
position;

typedef struct
{
    object * O;
    int map_index;
    int morph_index;
}
object_morph_dialer;

typedef struct deformer_morph
{
    int index;
    unsigned address;
    char * Name;
    deformer * D;
    int selected;
    deformer_morph_map * Map;

    object_morph_dialer ** Object_Morph_Map;
    int objectCount;
    float Amount;
}
deformer_morph;

deformer_morph * Deformer_Morph = NULL;

deformer_morph * deformer_morphs[DEFORMER_MORPHS]; // in list
int deformer_morph_Index = 0;

typedef struct
{
    int index;
    unsigned address;
    char * Name;
    int selected;
    deformer_morph * M;
    position * Positions;
    float Amount;
}
morph;

morph * morphs[OBJECT_MORPHS]; // object morphs in morph maps

typedef struct morph_map
{
    int index;
    unsigned address;
    char * Name;
    int collapsed;
    int selected;
    morph ** Morphs;
    int MorphsCount;
    deformer * Deformer;
    deformer_morph_map * DM;
    object * Object;
    weighted_index * Verts;
    int VertCount;
}
morph_map;

morph_map * object_morph_maps[MORPH_MAPS]; // in objects

typedef struct deformer_morph_map
{
    int index;
    unsigned address; // assigned after loading
    char * Name;
    int collapsed;
    int selected;

    deformer * Deformer;
    int start;

    deformer_morph ** Morphs;
    int Morphs_Count;

    int current_morph;

    object ** Objects;
    int Object_Count;
}
deformer_morph_map;

deformer_morph_map * deformer_morph_maps[DEFORMER_MORPH_MAPS]; // in list
int deformer_morph_map_Index = 0;

int add_Morph_To_Morph_Map_(morph_map * M, morph * Morph)
{
    M->MorphsCount ++;
    morph ** NewMorph = realloc(M->Morphs, M->MorphsCount * sizeof(morph*));
    if (NewMorph == NULL)
    {
        M->MorphsCount --;
        return 0;
    }
    else
    {
        M->Morphs = NewMorph;
        M->Morphs[M->MorphsCount - 1] = Morph;
        //morphs[morphsIndex ++] = Morph;
        return 1;
    }
}

int init_Morph(morph_map * M, deformer_morph * DM, morph * Morph, const char * Name)
{
    Morph->Name  = malloc(STRLEN * sizeof(char));

    if (Morph->Name == NULL)
    {
        return 0;
    }
    else
    {
        sprintf(Morph->Name, Name);
    }

    Morph->selected = 0;
    Morph->Positions = calloc(M->VertCount, sizeof(position));
    Morph->M = DM;
    Morph->Amount = 0.0;

    /* fill in Positions */

    return 1;
}

void list_Object_Morphs_c(morph_map * M)
{
    int m;

    morph * Morph;

    for (m = 0; m < M->MorphsCount; m ++)
    {
        Morph = M->Morphs[m];
        Morph->index = Object_Morphs_c;
        morphs[Object_Morphs_c] = Morph;

//        if (currentSubcharacter > 0 && S == subcharacters[currentSubcharacter])
//            D->current_pose = p;

        sprintf(Morph_Names[Morphs_c], "%s", Morph->Name);
        Morph_List[Morphs_c] = MORPHS + Object_Morphs_c;

        Morph_X_Offset[Morphs_c] = 3;
        Morph_X_Collapsed[Morphs_c] = 0;

        Morphs_c ++;
        Object_Morphs_c ++;

        if (Morphs_c >= MORPHS)
        {
            break;
        }
    }
}

void list_Deformer_Morphs_c(deformer_morph_map * M)
{
    int m;

    deformer_morph * Morph;

    for (m = 0; m < M->Morphs_Count; m ++)
    {
        Morph = M->Morphs[m];
        Morph->index = Deformer_Morphs_c;
        deformer_morphs[Deformer_Morphs_c ++] = Morph;

//        if (currentSubcharacter > 0 && S == subcharacters[currentSubcharacter])
//            D->current_pose = p;

        sprintf(Morph_Names[Morphs_c], "%s", Morph->Name);
        Morph_List[Morphs_c] = MORPHS + Morph->index;

        Morph_X_Offset[Morphs_c] = 3;
        Morph_X_Collapsed[Morphs_c] = 0;

        Morphs_c ++;

        if (Morphs_c >= MORPHS)
        {
            break;
        }
    }
}

void list_Object_Morph_Maps(object * O)
{
    int m;
    morph_map * M;

    for (m = 0; m < O->Morph_Maps_count; m ++)
    {
        M = O->Morph_Maps[m];
        M->index = Object_Morph_Maps_c;
        object_morph_maps[Object_Morph_Maps_c] = M;

        if (M->selected)
            selected_morph_node = Morphs_c;

        sprintf(Morph_Names[Morphs_c], "%s", M->Name);
        Morph_List[Morphs_c] = Object_Morph_Maps_c;

        Morph_X_Offset[Morphs_c] = 2;
        Morph_X_Collapsed[Morphs_c] = M->collapsed;

        Morphs_c ++;
        Object_Morph_Maps_c ++;

        if (Morphs_c >= MORPHS)
        {
            break;
        }

        if (!M->collapsed)
        {
            list_Object_Morphs_c(M);
        }
    }
}

void list_Deformer_Morph_Maps(deformer * D)
{
    int m;
    deformer_morph_map * M;

    for (m = 0; m < D->Morph_Maps_Count; m ++)
    {
        M = D->Morph_Maps[m];
        M->index = Deformer_Morph_Maps_c;
        deformer_morph_maps[Deformer_Morph_Maps_c ++] = M;

        if (M->selected)
            selected_morph_node = Morphs_c;

        sprintf(Morph_Names[Morphs_c], "%s", M->Name);
        Morph_List[Morphs_c] = M->index;

        Morph_X_Offset[Morphs_c] = 2;
        Morph_X_Collapsed[Morphs_c] = M->collapsed;

        Morphs_c ++;

        if (Morphs_c >= MORPHS)
        {
            break;
        }

        if (!M->collapsed)
        {
            list_Deformer_Morphs_c(M);
        }
    }
}

void create_Morphs_List(int list_type)
{
    if (list_type == DEFORMER_MORPH)
    {
        int d;
        deformer * D;
        Morphs_c = 0;
        Deformer_Node_c = 1;
        Deformer_Morphs_c = 0;
        Deformer_Morph_Maps_c = 0;

        for (d = 0; d < deformerIndex; d ++)
        {
            D = deformers[d];

            if (D->selected)
                selected_deformer_node = Morphs_c;

            memcpy(Morph_Names[Morphs_c], D->Name, strlen(D->Name));
            Morph_Names[Morphs_c][strlen(D->Name)] = '\0';
            Morph_List[Morphs_c] = -Deformer_Node_c;
            Morph_X_Offset[Morphs_c] = 1;
            Morph_X_Collapsed[Morphs_c] = D->collapsed;
            Morphs_c ++;
            Deformer_Node_c ++;

            if (Morphs_c >= MORPHS)
            {
                break;
            }

            if (!D->collapsed)
            {
                list_Deformer_Morph_Maps(D);
            }
        }
    }
    else if (list_type == OBJECT_MORPH)
    {
        int o;
        object * O;
        Morphs_c = 0;
        Object_Node_c = 1;
        Object_Morphs_c = 0;
        Object_Morph_Maps_c = 0;

        for (o = 1; o < objectIndex; o ++) /*CUBECOMMENT*/
        {
            O = objects[o];

            if (O->selected)
                selected_object_node = Morphs_c;

            memcpy(Morph_Names[Morphs_c], O->Name, strlen(O->Name));
            Morph_Names[Morphs_c][strlen(O->Name)] = '\0';
            Morph_List[Morphs_c] = -Object_Node_c;
            Morph_X_Offset[Morphs_c] = 1;
            Morph_X_Collapsed[Morphs_c] = O->collapsed;
            Morphs_c ++;
            Object_Node_c ++;

            if (Morphs_c >= MORPHS)
            {
                break;
            }

            if (!O->collapsed)
            {
                list_Object_Morph_Maps(O);
            }
        }
    }
}

void set_Mrph_H_Button(int index)
{
    int i;
    for (i = 0; i < H_MRPH_NUM; i ++)
    {
        Button_h_mrph[i].color = UI_GRAYB;
    }
    if (index > -1)
        Button_h_mrph[index].color = UI_GRAYD;
}

void replace_Morph_Name(char * EditString)
{
    if (Morph_List[MorphIndex] >= 0 && Morph_List[MorphIndex] < MORPHS)
    {
        deformer_morph_map * M = deformer_morph_maps[Morph_List[MorphIndex]];
        memcpy(M->Name, EditString, strlen(EditString));
        M->Name[strlen(EditString)] = '\0';
    }
    else if (Morph_List[MorphIndex] >= MORPHS)
    {
        deformer_morph * M = deformer_morphs[Morph_List[MorphIndex] - MORPHS];
        memcpy(M->Name, EditString, strlen(EditString));
        M->Name[strlen(EditString)] = '\0';
    }
    else
    {
        deformer * D = deformers[-(Morph_List[MorphIndex] + 1)];
        memcpy(D->Name, EditString, strlen(EditString));
        D->Name[strlen(EditString)] = '\0';
    }
}

int init_Deformer_Morph(deformer_morph * Morph, deformer_morph_map * M, deformer * D, const char * Name)
{
    Morph->Name = malloc(STRLEN * sizeof(char));

    if (Morph->Name == NULL)
    {
        return 0;
    }
    else
    {
        sprintf(Morph->Name, Name);
    }

    Morph->selected = 0;
    Morph->D = D;
    Morph->Map = M;

    Morph->objectCount = D->Objects_Count;
    Morph->Object_Morph_Map = malloc(Morph->objectCount * sizeof(object_morph_dialer*));

    int o;
    object_morph_dialer * OMD;

    for (o = 0; o < Morph->objectCount; o ++)
    {
        OMD = malloc(sizeof(object_morph_dialer));
        if (OMD == NULL)
        {
            Morph->objectCount = o;
            break;
        }
        OMD->O = D->Objects[o];
        Morph->Object_Morph_Map[o] = OMD;
    }

    Morph->index = deformer_morph_Index;
    deformer_morphs[deformer_morph_Index ++] = Morph;

    return 1;
}

int init_Deformer_Morph_Map(deformer_morph_map * M, deformer * D, const char * Name)
{
    char Name0[STRLEN];
    M->Name = malloc(STRLEN * sizeof(char));

    if (M->Name == NULL)
    {
        return 0;
    }
    else
    {
        sprintf(M->Name, Name);
    }

    int m, result;
    deformer_morph * Morph;
    M->Morphs_Count = 5;

    M->Morphs = malloc(M->Morphs_Count * sizeof(deformer_morph*));

    if (M->Morphs == NULL)
    {
        M->Morphs_Count = 0;
        return 0;
    }

    for (m = 0; m < M->Morphs_Count; m ++)
    {
        Morph = malloc(sizeof(deformer_morph));
        if (Morph == NULL)
        {
            M->Morphs_Count = m;
            break;
        }
        M->Morphs[m] = Morph;
        sprintf(Name0, "D Morph %d", m);
        result = init_Deformer_Morph(Morph, M, D, Name0);
        if (result == 0)
        {
            M->Morphs_Count = m;
            break;
        }
    }

    M->collapsed = 0;
    M->selected = 0;
    M->Deformer = D;
    M->start = 0;
    M->current_morph = 0;

    M->index = deformer_morph_map_Index;
    deformer_morph_maps[deformer_morph_map_Index ++] = M;

    return 1;
}

int generate_Morph_Tree(deformer * D)
{
    printf("generate Morph Tree\n");
    int m, result;
    deformer_morph_map * M;

    D->Morph_Maps_Count = 10;
    char Name[STRLEN];

    D->Morph_Maps = malloc(D->Morph_Maps_Count * sizeof(deformer_morph_map*));
    if (D->Morph_Maps == NULL)
    {
        D->Morph_Maps_Count = 0;
        return 0;
    }

    for (m = 0; m < D->Morph_Maps_Count; m ++)
    {
        M = malloc(sizeof(deformer_morph_map));
        if (M == NULL)
        {
            D->Morph_Maps_Count = m;
            break;
        }
        D->Morph_Maps[m] = M;
        sprintf(Name, "D Morph Map %d", m);
        result = init_Deformer_Morph_Map(M, D, Name);
        if (result == 0)
        {
            D->Morph_Maps_Count = m;
            break;
        }
    }
    return 1;
}

void distribute_Morph_Map_Positions_Weight(deformer * D, deformer_morph_map * M)
{
    int v, m, o, vert_counter;

    object * O;
    morph_map * OM;

    int * Index_Counter;

    for (o = 0; o < M->Object_Count; o ++)
    {
        O = M->Objects[o];

        Index_Counter = calloc(O->vertcount, sizeof(int));

        if (Index_Counter != NULL)
        {
            for (m = 0; m < O->Morph_Maps_count; m ++)
            {
                OM = O->Morph_Maps[m];
                vert_counter = 0;

                for (v = 0; v < O->vertcount; v ++)
                {
                    if (v == OM->Verts[vert_counter].index)
                    {
                        Index_Counter[v] ++;
                        vert_counter ++;
                    }
                }
            }

            for (m = 0; m < O->Morph_Maps_count; m ++)
            {
                OM = O->Morph_Maps[m];
                vert_counter = 0;

                for (v = 0; v < O->vertcount; v ++)
                {
                    if (v == OM->Verts[vert_counter].index)
                    {
                        OM->Verts[vert_counter].weight = 1.0 / (float)Index_Counter[v];
                        vert_counter ++;
                    }
                }
            }

            free(Index_Counter);
        }
    }
}

void transfer_Morph_Coordinates(deformer_morph * DM)
{
    printf("transfer Morph Coordinates\n");

    int i, o, m, condition, idx;

    morph * Morf;
    morph_map * M;
    object * O, * O0;
    vertex * V;
    weighted_index * I;
    object_morph_dialer * OMD;
    deformer_morph_map * Map = DM->Map;

    for (o = 0; o < selected_object_count; o ++)
    {
        O = objects[selected_objects[o]];
        if (O->Movement_Enabled)
        {
            condition = 0;
            for (m = 0; m < Map->Object_Count; m ++)
            {
                O0 = Map->Objects[m];
                if (O == O0)
                {
                    condition = 1;
                    break;
                }
            }
            if (condition)
            {
                OMD = DM->Object_Morph_Map[m];
                M = O->Morph_Maps[OMD->map_index];
                Morf = M->Morphs[OMD->morph_index];

                printf("%s %s %d\n", O->Name, M->Name, M->VertCount);

                for (i = 0; i < M->VertCount; i ++)
                {
                    I = &M->Verts[i];
                    idx = I->index;
                    V = &O->verts[idx / ARRAYSIZE][idx % ARRAYSIZE];
                    Morf->Positions[i].x = V->Rx - V->x;
                    Morf->Positions[i].y = V->Ry - V->y;
                    Morf->Positions[i].z = V->Rz - V->z;
                }
            }
        }
    }
}

void display_selected_Morph(deformer_morph * DM)
{
    printf("display selected Morph\n");

    int i, m, idx;

    morph * Morf;
    morph_map * M;
    object * O;
    vertex * V;
    weighted_index * I;
    object_morph_dialer * OMD;
    deformer_morph_map * Map = DM->Map;

    for (m = 0; m < Map->Object_Count; m ++)
    {
        O = Map->Objects[m];

        OMD = DM->Object_Morph_Map[m];
        M = O->Morph_Maps[OMD->map_index];
        Morf = M->Morphs[OMD->morph_index];

        printf("%s %s %d\n", O->Name, M->Name, M->VertCount);

        for (i = 0; i < M->VertCount; i ++)
        {
            I = &M->Verts[i];
            idx = I->index;
            V = &O->verts[idx / ARRAYSIZE][idx % ARRAYSIZE];
            V->Rx = Morf->Positions[i].x + V->x;
            V->Ry = Morf->Positions[i].y + V->y;
            V->Rz = Morf->Positions[i].z + V->z;
        }
    }
}

void print_out_Morphs_Info()
{
    int d, m, o;

    object * O;
    morph * Morf;
    morph_map * M;

    object_morph_dialer * OMD;
    deformer_morph * DM;
    deformer_morph_map * Map;

    for (d = 0; d < deformer_morph_map_Index; d ++)
    {
        Map = deformer_morph_maps[d];
        printf("%s %s\n", Map->Name, Map->Deformer->Name);
        for (m = 0; m < Map->Morphs_Count; m ++)
        {
            DM = Map->Morphs[m];
            printf("\t%s %s\n", DM->Name, DM->D->Name);
            for (o = 0; o < DM->objectCount; o ++)
            {
                OMD = DM->Object_Morph_Map[o];
                O = OMD->O;
                M = O->Morph_Maps[OMD->map_index];
                Morf = M->Morphs[OMD->morph_index];
                printf("\t\t%s %s %d %s %s\n", O->Name, M->Name, M->VertCount, Morf->Name, Morf->M->Name);
            }
        }
    }
}

void create_composite_Morph(deformer * D)
{
    int o, m, v, idx, s;

    object * O;
    vertex * V;
    vertex_Pos * vertex_Positions;
    morph * Morf;
    morph_map * M;
    weighted_index * I;

    for (o = 0; o < D->Objects_Count; o ++)
    {
        O = D->Objects[o];
        O->Movement_Enabled = 0;
        vertex_Positions = realloc(O->vertex_Positions, O->vertcount * sizeof(vertex_Pos));
        if (vertex_Positions != NULL && O->vertcount > 0)
        {
            O->vertex_Positions = vertex_Positions;
            O->Movement_Enabled = 1;
            for (v = 0; v < O->vertcount; v ++)
            {
                V = &O->verts[v / ARRAYSIZE][v % ARRAYSIZE];
                O->vertex_Positions[v].x = V->x;
                O->vertex_Positions[v].y = V->y;
                O->vertex_Positions[v].z = V->z;
            }
        }
    }

    for (o = 0; o < D->Objects_Count; o ++)
    {
        O = D->Objects[o];
        if (O->Movement_Enabled)
        {
            for (m = 0; m < O->Morph_Maps_count; m ++)
            {
                M = O->Morph_Maps[m];
                for (v = 0; v < M->VertCount; v ++)
                {
                    I = &M->Verts[v];
                    idx = I->index;
                    for (s = 0; s < M->MorphsCount; s ++)
                    {
                        Morf = M->Morphs[s];
                        O->vertex_Positions[idx].x += Morf->Positions[v].x * I->weight * Morf->Amount;
                        O->vertex_Positions[idx].y += Morf->Positions[v].y * I->weight * Morf->Amount;
                        O->vertex_Positions[idx].z += Morf->Positions[v].z * I->weight * Morf->Amount;
                    }
                }
            }
        }
    }
}

void display_composite_Morph(deformer * D)
{
    int o, v;

    object * O;
    vertex * V;

    for (o = 0; o < D->Objects_Count; o ++)
    {
        O = D->Objects[o];
        if (O->Movement_Enabled)
        {
            for (v = 0; v < O->vertcount; v ++)
            {
                V = &O->verts[v / ARRAYSIZE][v % ARRAYSIZE];
                V->Rx = O->vertex_Positions[v].x;
                V->Ry = O->vertex_Positions[v].y;
                V->Rz = O->vertex_Positions[v].z;
            }
        }
    }
}

int insert_morf_keyframe(object * O, int frame)
{
    if (O == NULL)
        return 0;

    vertex * V;
    morph_timeline * Tmm;
    int result, f, v, index, condition;

//    float rotVec_I[3][3];

    if (O->Morph_Timeline == NULL)
    {
        result = init_morph_timeline(O);

        if (result)
        {
            Tmm = O->Morph_Timeline;
            Tmm->Frames = malloc(sizeof(int));
            if (Tmm->Frames != NULL)
            {
                Tmm->Values = malloc(sizeof(morph_values));
                Tmm->Acceleration = malloc(sizeof(acceleration));
            }
            else
            {
                Tmm->key_frames = 0;
                return 0;
            }
            if (Tmm->Values == NULL || Tmm->Acceleration == NULL)
            {
                Tmm->key_frames = 0;
                return 0;
            }
            else
            {
                Tmm->key_frames = 1;
                Tmm->Frames[0] = frame;
                Tmm->Acceleration[0].segment_type = ACCELERATION_DEFAULT;
                Tmm->Acceleration[0].a_exponent = ACCELERATION_DEFAULT_OUT;
                Tmm->Acceleration[0].b_exponent = ACCELERATION_DEFAULT_IN;
                Tmm->Values[0].R_Coords = malloc(O->vertcount * sizeof(morph_Pos));
                for (v = 0; v < O->vertcount; v ++)
                {
                    V = &O->verts[v / ARRAYSIZE][v % ARRAYSIZE];
                    Tmm->Values[0].R_Coords[v].x = V->Rx;
                    Tmm->Values[0].R_Coords[v].y = V->Ry;
                    Tmm->Values[0].R_Coords[v].z = V->Rz;
                }

                return 1;
            }
        }
        else
        {
            return 0;
        }
    }
    else
    {
        Tmm = O->Morph_Timeline;
        index = 0;
        condition = 0;
        for (f = 0; f < Tmm->key_frames; f ++)
        {
            if (Tmm->Frames[f] == frame)
            {
                index = f;
                break;
            }
            else if (Tmm->Frames[f] > frame)
            {
                index = f;
                condition = 1;
                break;
            }
        }

        if (Tmm->Frames[Tmm->key_frames - 1] < frame)
        {
            index = Tmm->key_frames;
            condition = 1;
        }

        if (condition)
        {
            Tmm->key_frames ++;
            Tmm->Frames = realloc(Tmm->Frames, Tmm->key_frames * sizeof(int));
            if (Tmm->Frames != NULL)
            {
                Tmm->Values = realloc(Tmm->Values, Tmm->key_frames * sizeof(morph_values));
                Tmm->Acceleration = realloc(Tmm->Acceleration, Tmm->key_frames * sizeof(acceleration));
            }
            else
            {
                Tmm->key_frames = 0;
                return 0;
            }
            if (Tmm->Values == NULL || Tmm->Acceleration == NULL)
            {
                Tmm->key_frames = 0;
                return 0;
            }
            else
            {
                for (f = Tmm->key_frames - 1; f > index; f --)
                {
                    Tmm->Frames[f] = Tmm->Frames[f - 1];
                    memcpy(&Tmm->Acceleration[f], &Tmm->Acceleration[f - 1], sizeof(acceleration));
                    Tmm->Values[f] = Tmm->Values[f - 1];
                }
                Tmm->Frames[index] = frame;
                Tmm->Acceleration[index].segment_type = ACCELERATION_DEFAULT;
                Tmm->Acceleration[index].a_exponent = ACCELERATION_DEFAULT_OUT;
                Tmm->Acceleration[index].b_exponent = ACCELERATION_DEFAULT_IN;
                Tmm->Values[index].R_Coords = malloc(O->vertcount * sizeof(morph_Pos));
                for (v = 0; v < O->vertcount; v ++)
                {
                    V = &O->verts[v / ARRAYSIZE][v % ARRAYSIZE];
                    Tmm->Values[index].R_Coords[v].x = V->Rx;
                    Tmm->Values[index].R_Coords[v].y = V->Ry;
                    Tmm->Values[index].R_Coords[v].z = V->Rz;
                }
                return 1;
            }
        }
        else
        {
            Tmm->Frames[index] = frame;
            Tmm->Acceleration[index].segment_type = ACCELERATION_DEFAULT;
            Tmm->Acceleration[index].a_exponent = ACCELERATION_DEFAULT_OUT;
            Tmm->Acceleration[index].b_exponent = ACCELERATION_DEFAULT_IN;
            for (v = 0; v < O->vertcount; v ++)
            {
                V = &O->verts[v / ARRAYSIZE][v % ARRAYSIZE];
                Tmm->Values[index].R_Coords[v].x = V->Rx;
                Tmm->Values[index].R_Coords[v].y = V->Ry;
                Tmm->Values[index].R_Coords[v].z = V->Rz;
            }
            return 1;
        }
    }
}

int delete_morf_keyframe(object * O, int frame)
{
    morph_timeline * Tmm;
    int f, index, condition;

    if (O->Morph_Timeline != NULL)
    {
        Tmm = O->Morph_Timeline;
        condition = 0;

        for (f = 0; f < Tmm->key_frames; f ++)
        {
            if (Tmm->Frames[f] == frame)
            {
                index = f;
                condition = 1;
                break;
            }
        }

        if (condition)
        {
            Tmm->key_frames --;
            for (f = index; f < Tmm->key_frames; f ++)
            {
                Tmm->Frames[f] = Tmm->Frames[f + 1];
                memcpy(&Tmm->Acceleration[f], &Tmm->Acceleration[f + 1], sizeof(acceleration));
                Tmm->Values[f] = Tmm->Values[f + 1];
            }
            if (f - 1 >= 0)
                return Tmm->Frames[f - 1];
            else
                return -1;
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

void reset_deformer_Morphs(deformer * D, float value)
{
    int o, m, d;

    object * O;
    deformer_morph * M;
    deformer_morph_map * Map;

    morph * Morf;
    morph_map * OM;

    for (m = 0; m < D->Morph_Maps_Count; m ++)
    {
        Map = D->Morph_Maps[m];

        for (d = 0; d < Map->Morphs_Count; d ++)
        {
            M = Map->Morphs[d];
            M->Amount = value;
        }
    }

    for (o = 0; o < D->Objects_Count; o ++)
    {
        O = D->Objects[o];

        for (d = 0; d < O->Morph_Maps_count; d ++)
        {
            OM = O->Morph_Maps[d];

            for (m = 0; m < OM->MorphsCount; m ++)
            {
                Morf = OM->Morphs[m];
                Morf->Amount = value;
            }
        }
    }
}

int init_Deformer_Morph_Map_(deformer_morph_map * M, deformer * D, const char * Name);
int init_Deformer_Morph_(deformer_morph * Morph, deformer_morph_map * M, deformer * D, const char * Name);
void add_Morph_To_Object(deformer * D, deformer_morph_map * DM, deformer_morph * Morph, object * O, const char * Name, object_morph_dialer * OMD);

deformer_morph_map * create_Deformer_Morph_Map(morph_map * M, deformer * D)
{
    int result = 0;
    char Name[STRLEN];

    deformer_morph_map * Map = NULL;
    deformer_morph_map ** Morph_Maps;

    Morph_Maps = realloc(D->Morph_Maps, (D->Morph_Maps_Count + 1) * sizeof(deformer_morph_map*));

    if (Morph_Maps != NULL)
    {
        Map = malloc(sizeof(deformer_morph_map));
        if (Map != NULL)
        {
            D->Morph_Maps = Morph_Maps;
            D->Morph_Maps[D->Morph_Maps_Count] = Map;

            sprintf(Name, "%s", M->Name);
            result = init_Deformer_Morph_Map_(Map, D, Name);

            if (result)
            {
                D->Morph_Maps_Count ++;
                return Map;
            }
            else
            {
                return NULL;
            }
        }
    }
    return Map;
}

int insert_Object_To_Deformer_Morph_Map(deformer_morph_map * Map, object * O)
{
    /* update Map Objects */

    int o, condition = 1;

    for (o = 0; o < Map->Object_Count; o ++)
    {
        if (O == Map->Objects[o])
        {
            condition = 0;
            break;
        }
    }

    if (condition)
    {
        object ** Objects;

        Objects = realloc(Map->Objects, (Map->Object_Count + 1) * sizeof(object*));
        if (Objects != NULL)
        {
            Map->Objects = Objects;
            Map->Objects[Map->Object_Count] = O;
            Map->Object_Count ++;

            return 1;
        }
    }

    return 0;
}

int insert_Morph_To_Deformer(deformer * D, deformer_morph_map * Map, morph * Morf, object_morph_dialer * OMD)
{
    /* update Map Morphs */
    /* update Map Objects */

    char Name[STRLEN];

    int r = 0;
    int m, condition = 1;

    deformer_morph * Morph;
    deformer_morph ** Morphs;

    for (m = 0; m < Map->Morphs_Count; m ++)
    {
        Morph = Map->Morphs[m];
        if (strcmp(Morf->Name, Morph->Name) == 0)
        {
            condition = 0;
            break;
        }
    }

    if (condition)
    {
        /* create new morph inside Map */
        /* insert Object Morf */

        Morphs = realloc(Map->Morphs, (Map->Morphs_Count + 1) * sizeof(deformer_morph*));
        if (Morphs != NULL)
        {
            Map->Morphs = Morphs;
            Morph = malloc(sizeof(deformer_morph));
            if (Morph != NULL)
            {
                Map->Morphs[Map->Morphs_Count] = Morph;
                sprintf(Name, "%s", Morf->Name);
                result = init_Deformer_Morph_(Morph, Map, D, Name);

                if (result)
                {
                    Morf->M = Morph;
                    Morph->Object_Morph_Map = realloc(Morph->Object_Morph_Map, (Morph->objectCount + 1) * sizeof(object_morph_dialer*));
                    if (Morph->Object_Morph_Map != NULL)
                    {
                        Morph->Object_Morph_Map[Morph->objectCount] = OMD;
                        Morph->objectCount ++;
                    }
                }
                Map->Morphs_Count ++;
            }
        }
    }
    else
    {
        Morf->M = Morph;
        Morph->Object_Morph_Map = realloc(Morph->Object_Morph_Map, (Morph->objectCount + 1) * sizeof(object_morph_dialer*));
        if (Morph->Object_Morph_Map != NULL)
        {
            Morph->Object_Morph_Map[Morph->objectCount] = OMD;
            Morph->objectCount ++;
        }
    }

    return r;
}

void scan_Deformer_Objects_Morphs(deformer * D)
{
    int o, m, d, s, condition, result;

    object * O;
    morph * Morf;
    morph_map * M;
    deformer_morph_map * Map;

    object_morph_dialer * OMD;

    for (o = 0; o < D->Objects_Count; o ++)
    {
        O = D->Objects[o];
        printf("%s\n", O->Name);

        for (m = 0; m < O->Morph_Maps_count; m ++)
        {
            M = O->Morph_Maps[m];
            condition = 1;

            for (d = 0; d < D->Morph_Maps_Count; d ++)
            {
                Map = D->Morph_Maps[d];
                if (strcmp(M->Name, Map->Name) == 0)
                {
                    condition = 0;
                    break;
                }
            }

            if (condition)
            {
                Map = create_Deformer_Morph_Map(M, D);
                if (Map != NULL)
                {
                    result = insert_Object_To_Deformer_Morph_Map(Map, O);
                }
            }
            else
            {
                result = insert_Object_To_Deformer_Morph_Map(Map, O);
            }

            /* insert M Morphs to Deformer */

            if (result)
            {
                M->Deformer = D;
                M->DM = Map;
                printf("C: %s MorphsCount %d\n", M->Name, M->MorphsCount);

                for (s = 0; s < M->MorphsCount; s ++)
                {
                    Morf = M->Morphs[s];
                    printf("\t %s\n", Morf->Name);

                    OMD = malloc(sizeof(object_morph_dialer));
                    if (OMD != NULL)
                    {
                        OMD->O = O;
                        OMD->map_index = m;
                        OMD->morph_index = s;
                        insert_Morph_To_Deformer(D, Map, Morf, OMD);
                    }
                }
            }
        }
    }
}

#endif // MORPHS_H_INCLUDED
