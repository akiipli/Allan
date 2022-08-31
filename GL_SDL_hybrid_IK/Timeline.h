/*
The MIT License

Copyright <2022> <Allan Kiipli>
*/

#ifndef TIMELINE_H_INCLUDED
#define TIMELINE_H_INCLUDED

#define ACCELERATION_DEFAULT 3
#define ACCELERATION_DEFAULT_OUT 2.0
#define ACCELERATION_DEFAULT_IN 2.0
#define ACCELERATION_NONE 0
#define ACCELERATION_START 1
#define ACCELERATION_END 2
#define ACCELERATION_MID 3

int frame, f, result;
int currentFrame = 0;
float a, b;
int TimelineStart = 0;
int TimelineEnd = 201;
int Drag_Timeline = 0;

typedef struct
{
    float scl[3];
    float rot[3];
    float pos[3];
    float scl_vec[3];
    float rotVec_[3][3]; // multiply with parents transpose rotVec_ (unscaled)
}
transformer_values;

typedef struct
{
    int segment_type;
    float a_exponent;
    float b_exponent;
}
acceleration;

typedef struct
{
    int key_frames;
    int * Frames;
    transformer_values * Values;
    int current_Segment; // segment frame start index for current time
    acceleration * Acceleration;
}
timeline;

void free_Timeline(timeline * Tm)
{
    if (Tm->Frames != NULL) free(Tm->Frames);
    if (Tm->Values != NULL) free(Tm->Values);
    if (Tm->Acceleration != NULL) free(Tm->Acceleration);
}

#endif // TIMELINE_H_INCLUDED
