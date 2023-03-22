/*
The MIT License

Copyright <2022> <Allan Kiipli>
*/

#ifndef TIMELINE_H_INCLUDED
#define TIMELINE_H_INCLUDED

int ACCELERATION_DEFAULT = 3;
#define ACCELERATION_DEFAULT_OUT 2.0
#define ACCELERATION_DEFAULT_IN 2.0
#define ACCELERATION_NONE 0
#define ACCELERATION_START 1
#define ACCELERATION_END 2
#define ACCELERATION_MID 3
#define ACCELERATIONS 4

float EXPONENT_CHANGE = 0.2;
#define EXPONENT_MAX 5.0

char Acceleration_Names[ACCELERATIONS][32];

void init_Acceleration_Names()
{
    sprintf(Acceleration_Names[0], "NONE");
    sprintf(Acceleration_Names[1], "START");
    sprintf(Acceleration_Names[2], "END");
    sprintf(Acceleration_Names[3], "MID");
}

int frame, f, result;
int currentFrame = 0;
float a, b;
int TimelineStart = 0;
int TimelineEnd = 201;
int Drag_Timeline = 0;

int highlight_start = 0;
int highlight_end = 0;

int currentKey = -1;

typedef struct
{
    union
    {
        float Pos[3];
        struct
        {
            float x;
            float y;
            float z;
        };
    };
}
morph_Pos;

typedef struct
{
    float scl[3];
    float rot[3];
    float pos[3];
    float scl_vec[3];
    float rotVec_[3][3]; // multiply with parents transpose rotVec_ (unscaled)
}
transformer_values;

typedef struct object object;

typedef struct
{
    object * O;
    morph_Pos * R_Coords;
}
morph_values;

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
    int start_Segment;
}
timeline;

typedef struct
{
    int key_frames;
    int * Frames;
    morph_values * Values;
    int current_Segment; // segment frame start index for current time
    acceleration * Acceleration;
    int start_Segment;
}
morph_timeline;

void free_Timeline(timeline * Tm)
{
    if (Tm->Frames != NULL) free(Tm->Frames);
    if (Tm->Values != NULL) free(Tm->Values);
    if (Tm->Acceleration != NULL) free(Tm->Acceleration);
}

void free_Morf_Values(morph_values * Values)
{
    free(Values->R_Coords);
    free(Values);
}

void free_Morph_Timeline(morph_timeline * Tmm)
{
    if (Tmm->Frames != NULL) free(Tmm->Frames);
    if (Tmm->Values != NULL) free_Morf_Values(Tmm->Values);
    if (Tmm->Acceleration != NULL) free(Tmm->Acceleration);
}

void interpolateColorAcceleration(float a, float b, GLfloat Col_a[4], GLfloat Col_b[4], GLfloat Col_r[4])
{
    Col_r[0] = Col_a[0] * a + Col_b[0] * b;
    Col_r[1] = Col_a[1] * a + Col_b[1] * b;
    Col_r[2] = Col_a[2] * a + Col_b[2] * b;
    Col_r[3] = Col_a[3] * a + Col_b[3] * b;
}

void find_Segment_Acceleration_Colors(acceleration Acceleration, GLfloat Col_a[4], GLfloat Col_b[4], GLfloat Col_0[4], GLfloat Col_1[4], GLfloat Col_2[4])
{
    float a = 0.0;
    float b = 1.0;
    if (Acceleration.segment_type == ACCELERATION_END)
    {
        interpolateColorAcceleration(1, 0, Col_a, Col_b, Col_0);
        interpolateColorAcceleration(1, 0, Col_a, Col_b, Col_1);
        interpolateColorAcceleration(a, b, Col_a, Col_b, Col_2);
    }
    else if (Acceleration.segment_type == ACCELERATION_START)
    {
        interpolateColorAcceleration(a, b, Col_a, Col_b, Col_0);
        interpolateColorAcceleration(1, 0, Col_a, Col_b, Col_1);
        interpolateColorAcceleration(1, 0, Col_a, Col_b, Col_2);
    }
    else if (Acceleration.segment_type == ACCELERATION_MID)
    {
        interpolateColorAcceleration(1, 0, Col_a, Col_b, Col_0);
        interpolateColorAcceleration(a, b, Col_a, Col_b, Col_1);
        interpolateColorAcceleration(1, 0, Col_a, Col_b, Col_2);
    }
    else if (Acceleration.segment_type == ACCELERATION_NONE)
    {
        interpolateColorAcceleration(1, 0, Col_a, Col_b, Col_0);
        interpolateColorAcceleration(1, 0, Col_a, Col_b, Col_1);
        interpolateColorAcceleration(1, 0, Col_a, Col_b, Col_2);
    }
}

#endif // TIMELINE_H_INCLUDED
