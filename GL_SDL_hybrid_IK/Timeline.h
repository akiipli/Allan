/*
The MIT License

Copyright <2022> <Allan Kiipli>
*/

#ifndef TIMELINE_H_INCLUDED
#define TIMELINE_H_INCLUDED

typedef struct
{
//    float scl[3];
//    float rot[3];
    float pos[3];
    float scl_vec[3];
    float rotVec_[3][3]; // multiply with parents transpose rotVec_ (unscaled)
}
transformer_values;

typedef struct
{
    int key_frames;
    int * Frames;
    transformer_values * Values;
    int current_Segment; // segment frame start index for current time
}
timeline;

#endif // TIMELINE_H_INCLUDED
