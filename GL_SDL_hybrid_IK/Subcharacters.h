/*
The MIT License

Copyright <2018> <Allan Kiipli>
*/

#ifndef SUBCHARACTERS_H_INCLUDED
#define SUBCHARACTERS_H_INCLUDED

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


#endif // SUBCHARACTERS_H_INCLUDED
