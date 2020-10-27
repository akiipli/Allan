/*
The MIT License

Copyright <2018> <Allan Kiipli>
*/

#ifndef SUBCHARACTERS_H_INCLUDED
#define SUBCHARACTERS_H_INCLUDED

#define SUBCHARACTERS 200

int Subcharacters_c = 0;
char Subcharacter_Names[SUBCHARACTERS][STRLEN];
int SubcharacterIndex = 0;
int subcharacter_start = 0;

int Subcharacter_List[SUBCHARACTERS];
int currentSubcharacter = 0;

typedef struct subcharacter subcharacter;

typedef struct subcharacter
{
    int index;
    unsigned address; // assigned after loading
    char * Name;
    int selected;

    deformer * Deformer;
    transformer * T;

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


#endif // SUBCHARACTERS_H_INCLUDED
