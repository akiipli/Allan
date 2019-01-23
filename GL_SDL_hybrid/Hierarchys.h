/*
The MIT License

Copyright <2018> <Allan Kiipli>
*/

#define HIERARCHYS 400

int Hierarchys_c;
char Hier_Names[HIERARCHYS][STRLEN];
int HierIndex;
int hier_start;

int Hier_X_Offset[HIERARCHYS];
int Hier_X_Collapsed[HIERARCHYS];

int selected_transformers[HIERARCHYS];
int selected_transformer_count = 0;

void init_hierarcys()
{
    HierIndex = 0;
    hier_start = 0;
    memcpy(Hier_Names[0], "None", 5);
    Hierarchys_c = 0;
}

void edit_Hierarchy()
{
    if (Hierarchys_c > 0)
    {
        sprintf(Name_Remember, "%s", Hier_Names[HierIndex]);
        sprintf(Hier_Names[HierIndex], "%s", "");
        printf("Editing now!\n");
        Edit_Lock = 1;
        set_Hier_H_Button(0);
    }
}

void replace_Hierarchy_Name(char * Name_Remember, char * EditString)
{
    int t;
    transformer * T;

    for (t = 0; t < transformerIndex; t ++)
    {
        T = transformers[t];
        if (strcmp(Name_Remember, T->Name) == 0)
        {
            memcpy(T->Name, EditString, strlen(EditString));
            T->Name[strlen(EditString)] = '\0';
            break;
        }
    }
}

void list_transformers(transformer * T, int depth)
{
    int c;
    transformer * C;

    memcpy(Hier_Names[Hierarchys_c], T->Name, strlen(T->Name));
    Hier_Names[Hierarchys_c][strlen(T->Name)] = '\0';

    Hier_X_Offset[Hierarchys_c] = depth;
    Hier_X_Collapsed[Hierarchys_c] = T->collapsed;
    T->index = transformerIndex;
    transformers[transformerIndex ++] = T;

    Hierarchys_c ++;
    depth ++;

    if (!T->collapsed)
    {
        for (c = 0; c < T->childcount; c ++)
        {
            C = T->childs[c];
            if (C->parent == T && transformerIndex < TRANSFORMERS)
            {
                list_transformers(C, depth);
            }
        }
    }
}

void create_Hierarchys_List()
{
    transformer * T;

    T = transformers[0];
    transformerIndex = 0;

    Hierarchys_c = 0;

    list_transformers(T, 1);
}

void list_transformers_list(transformer * T)
{
    int c;
    transformer * C;

    T->index = transformerIndex;
    transformers[transformerIndex ++] = T;

    for (c = 0; c < T->childcount; c ++)
    {
        C = T->childs[c];
        if (C->parent == T && transformerIndex < TRANSFORMERS)
            list_transformers_list(C);
    }
}

void create_Transformers_List()
{
    transformer * T;

    T = transformers[0];
    transformerIndex = 0;

    list_transformers_list(T);
}
