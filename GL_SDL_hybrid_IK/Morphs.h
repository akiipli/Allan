/*
The MIT License

Copyright <2022> <Allan Kiipli>
*/

#ifndef MORPHS_H_INCLUDED
#define MORPHS_H_INCLUDED

#define MORPH_MAPS 1000
#define OBJECT_MORPH_MAPS 100
#define MORPHS 10000
#define DEFORMER_MORPH_MAPS 1000
#define DEFORMER_MORPHS 10000

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
}
deformer_morph;

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
}
morph;

//morph * morphs[OBJECT_MORPHS]; // object morphs in morph maps

typedef struct morph_map
{
    int index;
    unsigned address;
    char * Name;
    int selected;
    morph ** Morphs;
    int MorphsCount;
    deformer * Deformer;
    deformer_morph_map * DM;
    object * Object;
    int * Verts;
    int VertCount;
}
morph_map;

//morph_map * morph_maps[MORPH_MAPS]; // in objects

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

int init_Morph_Map(morph_map * M, const char * Name)
{
    M->Name  = malloc(STRLEN * sizeof(char));

    if (M->Name == NULL)
    {
        return 0;
    }
    else
    {
        sprintf(M->Name, Name);
    }

    M->selected = 0;
    M->Morphs = malloc(0);
    M->MorphsCount = 0;
    M->Deformer = NULL;
    M->Object = NULL;
    M->Verts = malloc(0);
    M->VertCount = 0;

    return 1;
}

void free_Morph_Map(morph_map * M);

int add_New_Morph_Map(object * O, const char * Name)
{
    printf("add New Morph Map to %s\n", O->Name);

    if (O->Morph_Maps_count >= OBJECT_MORPH_MAPS || morph_mapIndex >= MORPH_MAPS)
    {
        return 0;
    }

    int result;
    morph_map ** Morph_Maps;

    morph_map * M = malloc(sizeof(morph_map));

    if (M == NULL)
    {
        return 0;
    }

    result = init_Morph_Map(M, Name);
    if (result)
    {
        //morph_maps[morph_mapIndex] = M;
        M->index = morph_mapIndex;
        M->Object = O;
        morph_mapIndex ++;

        O->Morph_Maps_count ++;
        Morph_Maps = realloc(O->Morph_Maps, O->Morph_Maps_count * sizeof(morph_map*));

        if (Morph_Maps == NULL)
        {
            morph_mapIndex --;
            O->Morph_Maps_count --;
            free_Morph_Map(M);
            return 0;
        }
        else
        {
            O->Morph_Maps = Morph_Maps;
            O->Morph_Maps[O->Morph_Maps_count - 1] = M;
            return 1;
        }
    }
    else
    {
        return 0;
    }
}

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

int init_Morph(morph * Morph, const char * Name)
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
    Morph->Positions = malloc(0);

    return 1;
}

int add_New_Morph(morph_map * M, const char * Name)
{
    printf("add New Morph to %s\n", M->Name);

    if (morphsIndex >= MORPHS)
    {
        return 0;
    }

    int result;

    morph * Morph = malloc(sizeof(morph));

    if (Morph == NULL)
    {
        return 0;
    }

    result = init_Morph(Morph, Name);
    if (result)
    {
        if(add_Morph_To_Morph_Map_(M, Morph))
        {
            return 1;
        }
        else
        {
            return 0;
        }
    }
    else
    {
        return 0;
    }
}

void list_Deformer_Morphs_c(deformer_morph_map * M)
{
    int m;

    deformer_morph * Morph;

    for (m = 0; m < M->Morphs_Count; m ++)
    {
        Morph = M->Morphs[m];
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

void list_Deformer_Morph_Maps(deformer * D)
{
    int m;
    deformer_morph_map * M;

    for (m = 0; m < D->Morph_Maps_Count; m ++)
    {
        M = D->Morph_Maps[m];
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

void create_Morphs_List()
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

#endif // MORPHS_H_INCLUDED
