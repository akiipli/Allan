/*
The MIT License

Copyright <2018> <Allan Kiipli>
*/

#ifndef SUBCHARACTERS_H_INCLUDED
#define SUBCHARACTERS_H_INCLUDED

#define SUBCHARACTERS 200

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

typedef struct subcharacter
{
    int index;
    unsigned address; // assigned after loading
    char * Name;
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
}
subcharacter;

subcharacter * subcharacters[SUBCHARACTERS];
int subcharacterIndex = 0;

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

void replace_Subcharacter_Name(char * EditString)
{
    subcharacter * S = subcharacters[Subcharacter_List[SubcharacterIndex]];
    memcpy(S->Name, EditString, strlen(EditString));
    S->Name[strlen(EditString)] = '\0';
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

void delete_Subcharacter(subcharacter * S)
{
    //printf("remove Bone From Bones\n");

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
        free_subcharacter(S);

        subcharacterIndex --;
        for (s = index; s < subcharacterIndex; s ++)
        {
            subcharacters[s] = subcharacters[s + 1];
            subcharacters[s]->index = s;
        }
    }
}

#endif // SUBCHARACTERS_H_INCLUDED
