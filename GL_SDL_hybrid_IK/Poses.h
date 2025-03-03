/*
The MIT License

Copyright <2018> <Allan Kiipli>
*/

#ifndef POSES_H_INCLUDED
#define POSES_H_INCLUDED

#define POSES 300

int Poses_c;
int Poses_Node_c;
char Pose_Names[POSES][STRLEN];
int PoseIndex;
int pose_start;

int Pose_List[POSES];
int currentPose = 0;

int Pose_X_Offset[POSES];
int Pose_X_Collapsed[POSES];

int BIND_POSE = 0;
int ROTATED_POSE = 0;

typedef struct
{
    int rot_Order;
    float scl[3];
    float scl_vec[3];
    float rot[3];
    float rotVec[3][3];
    float rotVec_[3][3];
    float rotVec_I[3][3];
    float rotVec_B[3][3];
    float pos[3];
    float pos_[3];
    int style;
}
transformer_pose;

struct pose
{
    int index;
    unsigned address;
    char * Name;
    deformer * D;
    transformer_pose * TP;
    int transformers_count;
};

void free_pose(pose * P)
{
    free(P->Name);
    free(P->TP);
    free(P);
}

pose * poses_Import[POSES];
int posesCount_Import = 0;

pose * poses[POSES];
int posesIndex = 0;
int posesCount = 0;

void init_poses()
{
    Pose_X_Offset[0] = 0;
    Pose_X_Collapsed[0] = 0;

    PoseIndex = 0;
    pose_start = 0;

    int p;

    for (p = 0; p < POSES; p ++)
    {
        poses[p] = NULL;
    }
}

void create_Frame_Pose(deformer * D, int frame)
{
    pose * P = D->P;
    timeline * Tm;
    transformer * T;
    int t, frame0;

    float d;
//    float rotVec_[3][3];

    for (t = 0; t < D->Transformers_Count; t ++)
    {
        T = D->Transformers[t];

        if (T->Timeline != NULL)
        {
            Tm = T->Timeline;

            if (frame == TimelineStart)
            {
                Tm->current_Segment = Tm->start_Segment;
            }
            if (frame < Tm->Frames[0])
            {
                Tm->current_Segment = Tm->start_Segment;
            }
            else
            {
                frame0 = Tm->current_Segment;

                P->TP[t].rot_Order = T->rot_Order;

                P->TP[t].scl_vec[0] = Tm->Values[frame0].scl_vec[0];
                P->TP[t].scl_vec[1] = Tm->Values[frame0].scl_vec[1];
                P->TP[t].scl_vec[2] = Tm->Values[frame0].scl_vec[2];

                P->TP[t].pos[0] = Tm->Values[frame0].pos[0];
                P->TP[t].pos[1] = Tm->Values[frame0].pos[1];
                P->TP[t].pos[2] = Tm->Values[frame0].pos[2];

                P->TP[t].rot[0] = Tm->Values[frame0].rot[0];
                P->TP[t].rot[1] = Tm->Values[frame0].rot[1];
                P->TP[t].rot[2] = Tm->Values[frame0].rot[2];

                P->TP[t].scl[0] = Tm->Values[frame0].scl[0];
                P->TP[t].scl[1] = Tm->Values[frame0].scl[1];
                P->TP[t].scl[2] = Tm->Values[frame0].scl[2];

                P->TP[t].rotVec_[0][0] = Tm->Values[frame0].rotVec_[0][0];
                P->TP[t].rotVec_[0][1] = Tm->Values[frame0].rotVec_[0][1];
                P->TP[t].rotVec_[0][2] = Tm->Values[frame0].rotVec_[0][2];

                P->TP[t].rotVec_[1][0] = Tm->Values[frame0].rotVec_[1][0];
                P->TP[t].rotVec_[1][1] = Tm->Values[frame0].rotVec_[1][1];
                P->TP[t].rotVec_[1][2] = Tm->Values[frame0].rotVec_[1][2];

                P->TP[t].rotVec_[2][0] = Tm->Values[frame0].rotVec_[2][0];
                P->TP[t].rotVec_[2][1] = Tm->Values[frame0].rotVec_[2][1];
                P->TP[t].rotVec_[2][2] = Tm->Values[frame0].rotVec_[2][2];

                //rotate_matrix_I(P->TP[t].rotVec_, rotVec_, T->parent->rotVec_);

                d = sqrt(P->TP[t].rotVec_[0][0] * P->TP[t].rotVec_[0][0] +
                         P->TP[t].rotVec_[0][1] * P->TP[t].rotVec_[0][1] +
                         P->TP[t].rotVec_[0][2] * P->TP[t].rotVec_[0][2]);

                if (d > 0)
                {
                    P->TP[t].rotVec_[0][0] /= d;
                    P->TP[t].rotVec_[0][1] /= d;
                    P->TP[t].rotVec_[0][2] /= d;
                }

                P->TP[t].rotVec[0][0] = P->TP[t].rotVec_[0][0] * P->TP[t].scl_vec[0];
                P->TP[t].rotVec[0][1] = P->TP[t].rotVec_[0][1] * P->TP[t].scl_vec[0];
                P->TP[t].rotVec[0][2] = P->TP[t].rotVec_[0][2] * P->TP[t].scl_vec[0];

                d = sqrt(P->TP[t].rotVec_[1][0] * P->TP[t].rotVec_[1][0] +
                         P->TP[t].rotVec_[1][1] * P->TP[t].rotVec_[1][1] +
                         P->TP[t].rotVec_[1][2] * P->TP[t].rotVec_[1][2]);

                if (d > 0)
                {
                    P->TP[t].rotVec_[1][0] /= d;
                    P->TP[t].rotVec_[1][1] /= d;
                    P->TP[t].rotVec_[1][2] /= d;
                }

                P->TP[t].rotVec[1][0] = P->TP[t].rotVec_[1][0] * P->TP[t].scl_vec[1];
                P->TP[t].rotVec[1][1] = P->TP[t].rotVec_[1][1] * P->TP[t].scl_vec[1];
                P->TP[t].rotVec[1][2] = P->TP[t].rotVec_[1][2] * P->TP[t].scl_vec[1];

                d = sqrt(P->TP[t].rotVec_[2][0] * P->TP[t].rotVec_[2][0] +
                         P->TP[t].rotVec_[2][1] * P->TP[t].rotVec_[2][1] +
                         P->TP[t].rotVec_[2][2] * P->TP[t].rotVec_[2][2]);

                if (d > 0)
                {
                    P->TP[t].rotVec_[2][0] /= d;
                    P->TP[t].rotVec_[2][1] /= d;
                    P->TP[t].rotVec_[2][2] /= d;
                }

                P->TP[t].rotVec[2][0] = P->TP[t].rotVec_[2][0] * P->TP[t].scl_vec[2];
                P->TP[t].rotVec[2][1] = P->TP[t].rotVec_[2][1] * P->TP[t].scl_vec[2];
                P->TP[t].rotVec[2][2] = P->TP[t].rotVec_[2][2] * P->TP[t].scl_vec[2];
            }
        }
    }
}

typedef struct
{
    float Value;
    float Rollz;
}
trajectory_Pack;

trajectory_Pack get_T_Trajectory_value(transformer * T, int frame)
{
    trajectory_Pack trj_P;

    timeline * Tm;

    int frame0, frame1, frame00, frame11;

    float a, b;

    if (T->Timeline != NULL)
    {
        Tm = T->Timeline;

        if (frame == TimelineStart)
        {
            Tm->current_Segment = Tm->start_Segment;
        }
        if (frame < Tm->Frames[0])
        {
            Tm->current_Segment = Tm->start_Segment;
            trj_P.Value = Tm->Values[0].trj_val;
            trj_P.Rollz = Tm->Values[0].rot[2];
        }
        else if (frame >= Tm->Frames[Tm->key_frames - 1])
        {
            Tm->current_Segment = Tm->start_Segment;
            trj_P.Value = Tm->Values[Tm->key_frames - 1].trj_val;
            trj_P.Rollz = Tm->Values[Tm->key_frames - 1].rot[2];
        }
        else
        {
            frame0 = Tm->current_Segment;
            frame1 = Tm->current_Segment + 1;

            frame00 = Tm->Frames[frame0];
            frame11 = Tm->Frames[frame1];

            if (frame11 > frame00)
            {
                b = (float)(frame - frame00) / (float)(frame11 - frame00);
                if (Tm->Acceleration[frame0].segment_type == ACCELERATION_END)
                {
                    //b *= b; // accelerated interpolation
                    b = pow(b, Tm->Acceleration[frame0].b_exponent);
                    a = 1.0 - b;
                }
                else if (Tm->Acceleration[frame0].segment_type == ACCELERATION_START)
                {
                    a = 1.0 - b;
                    //a *= a; // slowdown motion
                    a = pow(a, Tm->Acceleration[frame0].a_exponent);
                    b = 1.0 - a; // both active yields mid acceleration
                }
                else if (Tm->Acceleration[frame0].segment_type == ACCELERATION_MID)
                {
                    //b *= b; // accelerated interpolation
                    b = pow(b, Tm->Acceleration[frame0].b_exponent);
                    a = 1.0 - b;
                    //a *= a; // slowdown motion
                    a = pow(a, Tm->Acceleration[frame0].a_exponent);
                    b = 1.0 - a; // both active yields mid acceleration
                }
                else if (Tm->Acceleration[frame0].segment_type == ACCELERATION_NONE)
                {
                    a = 1.0 - b;
                }
            }
            else
            {
                frame11 = Tm->Frames[Tm->current_Segment];
                b = 1.0;
                a = 0.0;
            }
//                if(t == 0)
//                    printf("%d %d %d %f %f %d\n", frame, frame00, frame11, a, b, Tm->current_Segment);

            if (frame11 == frame)
            {
                Tm->current_Segment ++;

                if (Tm->current_Segment >= Tm->key_frames - 1)
                {
                    Tm->current_Segment = Tm->start_Segment;
                }
                else
                {
                    frame11 = Tm->Frames[Tm->current_Segment];
                    if (frame11 >= TimelineEnd)
                    {
                        Tm->current_Segment = Tm->start_Segment;
                    }
                }
            }

            trj_P.Value = Tm->Values[frame0].trj_val * a + Tm->Values[frame1].trj_val * b;
            trj_P.Rollz = Tm->Values[frame0].rot[2] * a + Tm->Values[frame1].rot[2] * b;
        }
    }

    return trj_P;
}

void create_Inbetween_Frame_Pose(deformer * D, int frame, int linear_pose)
{
    pose * P = D->P;
    timeline * Tm;
    transformer * T;
    int t, frame0, frame1, frame00, frame11;

    float a, b, d;

    for (t = 0; t < D->Transformers_Count; t ++)
    {
        T = D->Transformers[t];

        if (T->Trj != NULL) // process trajectory animation later
            continue;

        if (T->Timeline != NULL)
        {
            Tm = T->Timeline;

            if (frame == TimelineStart)
            {
                Tm->current_Segment = Tm->start_Segment;
            }
            if (frame < Tm->Frames[0])
            {
                Tm->current_Segment = Tm->start_Segment;
            }
            else if (frame >= Tm->Frames[Tm->key_frames - 1])
            {
                Tm->current_Segment = Tm->start_Segment;
            }
            else
            {
                frame0 = Tm->current_Segment;
                frame1 = Tm->current_Segment + 1;

                frame00 = Tm->Frames[frame0];
                frame11 = Tm->Frames[frame1];

                if (frame11 > frame00)
                {
                    b = (float)(frame - frame00) / (float)(frame11 - frame00);
                    if (Tm->Acceleration[frame0].segment_type == ACCELERATION_END)
                    {
                        //b *= b; // accelerated interpolation
                        b = pow(b, Tm->Acceleration[frame0].b_exponent);
                        a = 1.0 - b;
                    }
                    else if (Tm->Acceleration[frame0].segment_type == ACCELERATION_START)
                    {
                        a = 1.0 - b;
                        //a *= a; // slowdown motion
                        a = pow(a, Tm->Acceleration[frame0].a_exponent);
                        b = 1.0 - a; // both active yields mid acceleration
                    }
                    else if (Tm->Acceleration[frame0].segment_type == ACCELERATION_MID)
                    {
                        //b *= b; // accelerated interpolation
                        b = pow(b, Tm->Acceleration[frame0].b_exponent);
                        a = 1.0 - b;
                        //a *= a; // slowdown motion
                        a = pow(a, Tm->Acceleration[frame0].a_exponent);
                        b = 1.0 - a; // both active yields mid acceleration
                    }
                    else if (Tm->Acceleration[frame0].segment_type == ACCELERATION_NONE)
                    {
                        a = 1.0 - b;
                    }
                }
                else
                {
                    frame11 = Tm->Frames[Tm->current_Segment];
                    b = 1.0;
                    a = 0.0;
                }
    //                if(t == 0)
    //                    printf("%d %d %d %f %f %d\n", frame, frame00, frame11, a, b, Tm->current_Segment);

                if (frame11 == frame)
                {
                    Tm->current_Segment ++;

                    if (Tm->current_Segment >= Tm->key_frames - 1)
                    {
                        Tm->current_Segment = Tm->start_Segment;
                    }
                    else
                    {
                        frame11 = Tm->Frames[Tm->current_Segment];
                        if (frame11 >= TimelineEnd)
                        {
                            Tm->current_Segment = Tm->start_Segment;
                        }
                    }
                }

                P->TP[t].rot_Order = T->rot_Order;

                P->TP[t].scl_vec[0] = Tm->Values[frame0].scl_vec[0] * a + Tm->Values[frame1].scl_vec[0] * b;
                P->TP[t].scl_vec[1] = Tm->Values[frame0].scl_vec[1] * a + Tm->Values[frame1].scl_vec[1] * b;
                P->TP[t].scl_vec[2] = Tm->Values[frame0].scl_vec[2] * a + Tm->Values[frame1].scl_vec[2] * b;

                P->TP[t].pos[0] = Tm->Values[frame0].pos[0] * a + Tm->Values[frame1].pos[0] * b;
                P->TP[t].pos[1] = Tm->Values[frame0].pos[1] * a + Tm->Values[frame1].pos[1] * b;
                P->TP[t].pos[2] = Tm->Values[frame0].pos[2] * a + Tm->Values[frame1].pos[2] * b;

                P->TP[t].scl[0] = Tm->Values[frame0].scl[0] * a + Tm->Values[frame1].scl[0] * b;
                P->TP[t].scl[1] = Tm->Values[frame0].scl[1] * a + Tm->Values[frame1].scl[1] * b;
                P->TP[t].scl[2] = Tm->Values[frame0].scl[2] * a + Tm->Values[frame1].scl[2] * b;

                P->TP[t].rot[0] = Tm->Values[frame0].rot[0] * a + Tm->Values[frame1].rot[0] * b;
                P->TP[t].rot[1] = Tm->Values[frame0].rot[1] * a + Tm->Values[frame1].rot[1] * b;
                P->TP[t].rot[2] = Tm->Values[frame0].rot[2] * a + Tm->Values[frame1].rot[2] * b;

                if (linear_pose)
                {
                    P->TP[t].rotVec_[0][0] = Tm->Values[frame0].rotVec_[0][0] * a + Tm->Values[frame1].rotVec_[0][0] * b;
                    P->TP[t].rotVec_[0][1] = Tm->Values[frame0].rotVec_[0][1] * a + Tm->Values[frame1].rotVec_[0][1] * b;
                    P->TP[t].rotVec_[0][2] = Tm->Values[frame0].rotVec_[0][2] * a + Tm->Values[frame1].rotVec_[0][2] * b;

                    P->TP[t].rotVec_[1][0] = Tm->Values[frame0].rotVec_[1][0] * a + Tm->Values[frame1].rotVec_[1][0] * b;
                    P->TP[t].rotVec_[1][1] = Tm->Values[frame0].rotVec_[1][1] * a + Tm->Values[frame1].rotVec_[1][1] * b;
                    P->TP[t].rotVec_[1][2] = Tm->Values[frame0].rotVec_[1][2] * a + Tm->Values[frame1].rotVec_[1][2] * b;

                    P->TP[t].rotVec_[2][0] = Tm->Values[frame0].rotVec_[2][0] * a + Tm->Values[frame1].rotVec_[2][0] * b;
                    P->TP[t].rotVec_[2][1] = Tm->Values[frame0].rotVec_[2][1] * a + Tm->Values[frame1].rotVec_[2][1] * b;
                    P->TP[t].rotVec_[2][2] = Tm->Values[frame0].rotVec_[2][2] * a + Tm->Values[frame1].rotVec_[2][2] * b;

                    //rotate_matrix_I(P->TP[t].rotVec_, rotVec_, T->parent->rotVec_);

                    d = sqrt(P->TP[t].rotVec_[0][0] * P->TP[t].rotVec_[0][0] +
                             P->TP[t].rotVec_[0][1] * P->TP[t].rotVec_[0][1] +
                             P->TP[t].rotVec_[0][2] * P->TP[t].rotVec_[0][2]);

                    if (d > 0)
                    {
                        P->TP[t].rotVec_[0][0] /= d;
                        P->TP[t].rotVec_[0][1] /= d;
                        P->TP[t].rotVec_[0][2] /= d;
                    }

                    P->TP[t].rotVec[0][0] = P->TP[t].rotVec_[0][0] * P->TP[t].scl_vec[0];
                    P->TP[t].rotVec[0][1] = P->TP[t].rotVec_[0][1] * P->TP[t].scl_vec[0];
                    P->TP[t].rotVec[0][2] = P->TP[t].rotVec_[0][2] * P->TP[t].scl_vec[0];

                    d = sqrt(P->TP[t].rotVec_[1][0] * P->TP[t].rotVec_[1][0] +
                             P->TP[t].rotVec_[1][1] * P->TP[t].rotVec_[1][1] +
                             P->TP[t].rotVec_[1][2] * P->TP[t].rotVec_[1][2]);

                    if (d > 0)
                    {
                        P->TP[t].rotVec_[1][0] /= d;
                        P->TP[t].rotVec_[1][1] /= d;
                        P->TP[t].rotVec_[1][2] /= d;
                    }

                    P->TP[t].rotVec[1][0] = P->TP[t].rotVec_[1][0] * P->TP[t].scl_vec[1];
                    P->TP[t].rotVec[1][1] = P->TP[t].rotVec_[1][1] * P->TP[t].scl_vec[1];
                    P->TP[t].rotVec[1][2] = P->TP[t].rotVec_[1][2] * P->TP[t].scl_vec[1];

                    d = sqrt(P->TP[t].rotVec_[2][0] * P->TP[t].rotVec_[2][0] +
                             P->TP[t].rotVec_[2][1] * P->TP[t].rotVec_[2][1] +
                             P->TP[t].rotVec_[2][2] * P->TP[t].rotVec_[2][2]);

                    if (d > 0)
                    {
                        P->TP[t].rotVec_[2][0] /= d;
                        P->TP[t].rotVec_[2][1] /= d;
                        P->TP[t].rotVec_[2][2] /= d;
                    }

                    P->TP[t].rotVec[2][0] = P->TP[t].rotVec_[2][0] * P->TP[t].scl_vec[2];
                    P->TP[t].rotVec[2][1] = P->TP[t].rotVec_[2][1] * P->TP[t].scl_vec[2];
                    P->TP[t].rotVec[2][2] = P->TP[t].rotVec_[2][2] * P->TP[t].scl_vec[2];
                }
            }
        }
    }
}

void create_Inbetween_Frame_Morf(deformer * D, int frame)
{
    morph_timeline * Tmm;

    object * O;
    vertex * V;

    int o, v, frame0, frame1, frame00, frame11;

    float a, b;
//    float rotVec_[3][3];

    for (o = 0; o < D->Objects_Count; o ++)
    {
        O = D->Objects[o];

        if (O->Morph_Timeline != NULL)
        {
            Tmm = O->Morph_Timeline;

            if (frame == TimelineStart)
            {
                Tmm->current_Segment = Tmm->start_Segment;
            }
            if (frame < Tmm->Frames[0])
            {
                Tmm->current_Segment = Tmm->start_Segment;
            }
            else if (frame >= Tmm->Frames[Tmm->key_frames - 1])
            {
                Tmm->current_Segment = Tmm->start_Segment;
            }
            else
            {
                frame0 = Tmm->current_Segment;
                frame1 = Tmm->current_Segment + 1;

                frame00 = Tmm->Frames[frame0];
                frame11 = Tmm->Frames[frame1];

                if (frame11 > frame00)
                {
                    b = (float)(frame - frame00) / (float)(frame11 - frame00);
                    if (Tmm->Acceleration[frame0].segment_type == ACCELERATION_END)
                    {
                        //b *= b; // accelerated interpolation
                        b = pow(b, Tmm->Acceleration[frame0].b_exponent);
                        a = 1.0 - b;
                    }
                    else if (Tmm->Acceleration[frame0].segment_type == ACCELERATION_START)
                    {
                        a = 1.0 - b;
                        //a *= a; // slowdown motion
                        a = pow(a, Tmm->Acceleration[frame0].a_exponent);
                        b = 1.0 - a; // both active yields mid acceleration
                    }
                    else if (Tmm->Acceleration[frame0].segment_type == ACCELERATION_MID)
                    {
                        //b *= b; // accelerated interpolation
                        b = pow(b, Tmm->Acceleration[frame0].b_exponent);
                        a = 1.0 - b;
                        //a *= a; // slowdown motion
                        a = pow(a, Tmm->Acceleration[frame0].a_exponent);
                        b = 1.0 - a; // both active yields mid acceleration
                    }
                    else if (Tmm->Acceleration[frame0].segment_type == ACCELERATION_NONE)
                    {
                        a = 1.0 - b;
                    }
                }
                else
                {
                    frame11 = Tmm->Frames[Tmm->current_Segment];
                    b = 1.0;
                    a = 0.0;
                }

                if (frame11 == frame)
                {
                    Tmm->current_Segment ++;

                    if (Tmm->current_Segment >= Tmm->key_frames - 1)
                    {
                        Tmm->current_Segment = Tmm->start_Segment;
                    }
                    else
                    {
                        frame11 = Tmm->Frames[Tmm->current_Segment];
                        if (frame11 >= TimelineEnd)
                        {
                            Tmm->current_Segment = Tmm->start_Segment;
                        }
                    }
                }
                for (v = 0; v < O->vertcount; v ++)
                {
                    V = &O->verts[v / ARRAYSIZE][v % ARRAYSIZE];
                    V->Rx = Tmm->Values[frame0].R_Coords[v].x * a + Tmm->Values[frame1].R_Coords[v].x * b;
                    V->Ry = Tmm->Values[frame0].R_Coords[v].y * a + Tmm->Values[frame1].R_Coords[v].y * b;
                    V->Rz = Tmm->Values[frame0].R_Coords[v].z * a + Tmm->Values[frame1].R_Coords[v].z * b;
                }
            }
        }
    }
}

void create_Inbetween_Pose_(deformer * D, pose * P, pose * P0, pose * P1, float w0, float w1)
{
    int t;

    for (t = 0; t < P->transformers_count; t ++)
    {
        if (t >= P0->transformers_count || t >= P1->transformers_count)
            break;

        P->TP[t].rot_Order = P0->TP[t].rot_Order;

        P->TP[t].scl_vec[0] = P0->TP[t].scl_vec[0] * w0 + P1->TP[t].scl_vec[0] * w1;
        P->TP[t].scl_vec[1] = P0->TP[t].scl_vec[1] * w0 + P1->TP[t].scl_vec[1] * w1;
        P->TP[t].scl_vec[2] = P0->TP[t].scl_vec[2] * w0 + P1->TP[t].scl_vec[2] * w1;

        P->TP[t].pos[0] = P0->TP[t].pos[0] * w0 + P1->TP[t].pos[0] * w1;
        P->TP[t].pos[1] = P0->TP[t].pos[1] * w0 + P1->TP[t].pos[1] * w1;
        P->TP[t].pos[2] = P0->TP[t].pos[2] * w0 + P1->TP[t].pos[2] * w1;

        P->TP[t].scl[0] = P0->TP[t].scl[0] * w0 + P1->TP[t].scl[0] * w1;
        P->TP[t].scl[1] = P0->TP[t].scl[1] * w0 + P1->TP[t].scl[1] * w1;
        P->TP[t].scl[2] = P0->TP[t].scl[2] * w0 + P1->TP[t].scl[2] * w1;

        P->TP[t].rot[0] = P0->TP[t].rot[0] * w0 + P1->TP[t].rot[0] * w1;
        P->TP[t].rot[1] = P0->TP[t].rot[1] * w0 + P1->TP[t].rot[1] * w1;
        P->TP[t].rot[2] = P0->TP[t].rot[2] * w0 + P1->TP[t].rot[2] * w1;
    }
}

void print_Transformer_Status(transformer * T)
{
    printf("--- %s\n", T->Name);
    printf("ord\t%d\n", T->rot_Order);
    printf("scl\t%f %f %f\n", T->scl[0], T->scl[1], T->scl[2]);
    printf("vec\t%f %f %f\n", T->scl_vec[0], T->scl_vec[1], T->scl_vec[2]);
    printf("rot\t%f %f %f\n", T->rot[0], T->rot[1], T->rot[2]);
    printf("pos\t%f %f %f\n", T->pos[0], T->pos[1], T->pos[2]);
    printf("rotVec0\t%f %f %f\n", T->rotVec[0][0], T->rotVec[0][1], T->rotVec[0][2]);
    printf("rotVec1\t%f %f %f\n", T->rotVec[1][0], T->rotVec[1][1], T->rotVec[1][2]);
    printf("rotVec2\t%f %f %f\n", T->rotVec[2][0], T->rotVec[2][1], T->rotVec[2][2]);
    printf("rotVec_0\t%f %f %f\n", T->rotVec_[0][0], T->rotVec_[0][1], T->rotVec_[0][2]);
    printf("rotVec_1\t%f %f %f\n", T->rotVec_[1][0], T->rotVec_[1][1], T->rotVec_[1][2]);
    printf("rotVec_2\t%f %f %f\n", T->rotVec_[2][0], T->rotVec_[2][1], T->rotVec_[2][2]);
}

void paste_Pose_position(deformer * D, pose * P)
{
    int t;

    transformer * T;

    for (t = 0; t < P->transformers_count; t ++)
    {
        if (t >= D->Transformers_Count)
            break;
        T = D->Transformers[t];
        T->rot_Order = P->TP[t].rot_Order;
        memcpy(T->scl, P->TP[t].scl, sizeof(T->scl));
        memcpy(T->scl_vec, P->TP[t].scl_vec, sizeof(T->scl_vec));
        memcpy(T->rot, P->TP[t].rot, sizeof(T->rot));
        memcpy(T->rotVec, P->TP[t].rotVec, sizeof(T->rotVec));
//        memcpy(T->rotVec_, P->TP[t].rotVec_, sizeof(T->rotVec_));
//        memcpy(T->rotVec_I, P->TP[t].rotVec_I, sizeof(T->rotVec_I));
//        memcpy(T->rotVec_B, P->TP[t].rotVec_B, sizeof(T->rotVec_B));
        memcpy(T->pos, P->TP[t].pos, sizeof(T->pos));
//        memcpy(T->pos_, P->TP[t].pos_, sizeof(T->pos_));
    }
}

void paste_Pose_rotation(deformer * D, pose * P)
{
    int t;

    transformer * T;

    for (t = 0; t < P->transformers_count; t ++)
    {
        if (t >= D->Transformers_Count)
            break;
        T = D->Transformers[t];
        T->rot_Order = P->TP[t].rot_Order;
        memcpy(T->scl, P->TP[t].scl, sizeof(T->scl));
        memcpy(T->scl_vec, P->TP[t].scl_vec, sizeof(T->scl_vec));
        memcpy(T->rot, P->TP[t].rot, sizeof(T->rot));
//        memcpy(T->rotVec, P->TP[t].rotVec, sizeof(T->rotVec));
//        memcpy(T->rotVec_, P->TP[t].rotVec_, sizeof(T->rotVec_));  // because of IK
//        memcpy(T->rotVec_I, P->TP[t].rotVec_I, sizeof(T->rotVec_I));
//        memcpy(T->rotVec_B, P->TP[t].rotVec_B, sizeof(T->rotVec_B));
        memcpy(T->pos, P->TP[t].pos, sizeof(T->pos));
//        memcpy(T->pos_, P->TP[t].pos_, sizeof(T->pos_));
    }
}

void update_Deformer_Pose(deformer * D, pose * P, int linear_pose)
{
    int t;

    transformer * T;

    float rotVec_I[3][3];

    for (t = 0; t < P->transformers_count; t ++)
    {
        if (t >= D->Transformers_Count)
            break;
        T = D->Transformers[t];
        P->TP[t].rot_Order = T->rot_Order;
        memcpy(P->TP[t].scl, T->scl, sizeof(float[3]));
        memcpy(P->TP[t].scl_vec, T->scl_vec, sizeof(float[3]));
        memcpy(P->TP[t].rot, T->rot, sizeof(float[3]));
        memcpy(P->TP[t].rotVec, T->rotVec, sizeof(float[3][3]));
        memcpy(P->TP[t].rotVec_, T->rotVec_, sizeof(float[3][3]));
        memcpy(P->TP[t].rotVec_I, T->rotVec_I, sizeof(float[3][3]));
        memcpy(P->TP[t].rotVec_B, T->rotVec_B, sizeof(float[3][3]));

        if (linear_pose || (T->IK != NULL && T == T->IK->B))
        {
            P->TP[t].pos[0] = T->pos[0] - D->Delta[0];
            P->TP[t].pos[1] = T->pos[1] - D->Delta[1];
            P->TP[t].pos[2] = T->pos[2] - D->Delta[2];
        }
        else if (T->parent != NULL)
        {
            invert_Rotation_scale(T->parent, rotVec_I);
            rotate_center_(T->pos, rotVec_I, T->parent->pos, P->TP[t].pos);
        }

        P->TP[t].style = T->style;
    }
}

void fill_Start_Pose(deformer * D, pose * P, int linear_pose)
{
    int t;

    transformer * T;
    timeline * Tm;

    float rotVec_I[3][3];

    if (D->Transformers_Count > 0)
    {
        for (t = 0; t < P->transformers_count; t ++)
        {
            if (t >= D->Transformers_Count)
                break;

            T = D->Transformers[t];

            if (T->Timeline != NULL)
            {
                Tm = T->Timeline;

                P->TP[t].rot_Order = T->rot_Order;
                memcpy(P->TP[t].scl, Tm->Values[0].scl, sizeof(float[3]));
                memcpy(P->TP[t].scl_vec, Tm->Values[0].scl_vec, sizeof(float[3]));
                memcpy(P->TP[t].rot, Tm->Values[0].rot, sizeof(float[3]));
                memcpy(P->TP[t].rotVec_, Tm->Values[0].rotVec_, sizeof(float[3][3]));

                scale_rotVec(P->TP[t].rotVec, P->TP[t].rotVec_, P->TP[t].scl_vec);

                memcpy(P->TP[t].pos, Tm->Values[0].pos, sizeof(float[3]));
            }
            else
            {
                P->TP[t].rot_Order = T->rot_Order;
                memcpy(P->TP[t].scl, T->scl, sizeof(float[3]));
                memcpy(P->TP[t].scl_vec, T->scl_vec, sizeof(float[3]));
                memcpy(P->TP[t].rot, T->rot, sizeof(float[3]));
                memcpy(P->TP[t].rotVec, T->rotVec, sizeof(float[3][3]));
                memcpy(P->TP[t].rotVec_, T->rotVec_, sizeof(float[3][3]));

                memcpy(P->TP[t].rotVec_I, T->rotVec_I, sizeof(float[3][3]));
                memcpy(P->TP[t].rotVec_B, T->rotVec_B, sizeof(float[3][3]));

                if (linear_pose || (T->IK != NULL && T == T->IK->B))
                {
                    P->TP[t].pos[0] = T->pos[0] - D->Delta[0];
                    P->TP[t].pos[1] = T->pos[1] - D->Delta[1];
                    P->TP[t].pos[2] = T->pos[2] - D->Delta[2];
                }
                else if (T->parent != NULL)
                {
                    invert_Rotation_scale(T->parent, rotVec_I);
                    rotate_center_(T->pos, rotVec_I, T->parent->pos, P->TP[t].pos);
                    //memcpy(P->TP[t].pos, T->pos, sizeof(float[3]));
                }

                memcpy(P->TP[t].pos_, T->pos_, sizeof(float[3]));
            }
        }
    }
}

void paste_Pose_(deformer * D, pose * P)
{
    int t;

    transformer * T;

    for (t = 0; t < P->transformers_count; t ++)
    {
        if (t >= D->Transformers_Count)
            break;
        T = D->Transformers[t];
        T->rot_Order = P->TP[t].rot_Order;
        memcpy(T->scl, P->TP[t].scl, sizeof(float[3]));
        memcpy(T->scl_vec, P->TP[t].scl_vec, sizeof(float[3]));
        memcpy(T->rot, P->TP[t].rot, sizeof(float[3]));
        memcpy(T->rotVec, P->TP[t].rotVec, sizeof(float[3][3]));
        memcpy(T->rotVec_, P->TP[t].rotVec_, sizeof(float[3][3]));

        memcpy(T->rotVec_I, P->TP[t].rotVec_I, sizeof(float[3][3]));
        memcpy(T->rotVec_B, P->TP[t].rotVec_B, sizeof(float[3][3]));

        memcpy(T->pos, P->TP[t].pos, sizeof(float[3]));
        memcpy(T->pos_, P->TP[t].pos_, sizeof(float[3]));

        //T->style = P->TP[t].style;
    }
}

void remove_Pose_From_Deformer(pose * P)
{
    deformer * D = P->D;
    int index, p;

    int condition = 0;

    for (p = 1; p < D->Poses_Count; p ++)
    {
        if (D->Poses[p] == P)
        {
            index = p;
            condition = 1;
            break;
        }
    }

    if (condition)
    {
        D->Poses_Count --;
        for (p = index; p < D->Poses_Count; p ++)
        {
            D->Poses[p] = D->Poses[p + 1];
        }

        posesIndex --;

        for (p = P->index; p < posesIndex; p ++)
        {
            poses[p] = poses[p + 1];
            poses[p]->index --;
        }

        free_pose(P);
    }
    if (index >= D->Poses_Count)
    {
        if (D->Poses_Count - 1 > 0)
            D->current_pose = D->Poses_Count - 1;
        else
            D->current_pose = 0;
    }
    else
    {
        D->current_pose = index;
    }
    if (currentPose >= posesIndex)
    {
        if (posesIndex - 1 > 0)
            currentPose = posesIndex - 1;
        else
            currentPose = 0;
    }
}

pose * init_a_pose()
{
    pose * P = malloc(sizeof(pose));
    P->index = 0;
    P->Name = malloc(STRLEN * sizeof(char));
    sprintf(P->Name, "%s", "pose");
    P->D = NULL;
    P->transformers_count = 0;
    P->TP = calloc(P->transformers_count, sizeof(transformer_pose));
    return P;
}

pose * init_Deformer_P(deformer * D)
{
    pose * P;

    if (D->P == NULL)
    {
        P = malloc(sizeof(pose));
        P->transformers_count = D->Transformers_Count;
        P->TP = calloc(P->transformers_count, sizeof(transformer_pose));
    }
    else
    {
        P = D->P;
        P->transformers_count = D->Transformers_Count;
        P->TP = realloc(P->TP, P->transformers_count * sizeof(transformer_pose));
    }

    return P;
}

void add_Default_Pose_To_Deformer(deformer * D)
{
    pose * P;
    if (D->Poses_Count == 0)
    {
        P = malloc(sizeof(pose));
        poses[posesIndex] = P;
        P->index = posesIndex;
        P->Name = malloc(STRLEN * sizeof(char));
        sprintf(P->Name, "%s %d", "Default", posesCount ++);
        P->D = D;
        D->Poses_Count ++;
        D->Poses = realloc(D->Poses, D->Poses_Count * sizeof(pose*));
        D->Poses[D->Poses_Count - 1] = P;
        posesIndex ++;
        P->transformers_count = D->Transformers_Count;
        P->TP = calloc(P->transformers_count, sizeof(transformer_pose));
    }
    else
    {
        P = D->Poses[0];
        P->transformers_count = D->Transformers_Count;
        P->TP = realloc(P->TP, P->transformers_count * sizeof(transformer_pose));
    }

    int t;

    transformer * T;

    for (t = 0; t < D->Transformers_Count; t ++)
    {
        T = D->Transformers[t];

        P->TP[t].rot_Order = T->rot_Order;
        memcpy(P->TP[t].scl, T->scl, sizeof(float[3]));
        memcpy(P->TP[t].scl_vec, T->scl_vec, sizeof(float[3]));
        memcpy(P->TP[t].rot, T->rot, sizeof(float[3]));
        memcpy(P->TP[t].rotVec, T->rotVec, sizeof(float[3][3]));
        memcpy(P->TP[t].rotVec_, T->rotVec_, sizeof(float[3][3]));
        memcpy(P->TP[t].rotVec_I, T->rotVec_I, sizeof(float[3][3]));
        memcpy(P->TP[t].rotVec_B, T->rotVec_B, sizeof(float[3][3]));
        memcpy(P->TP[t].pos, T->pos, sizeof(float[3]));
        memcpy(P->TP[t].pos_, T->pos_, sizeof(float[3]));
        P->TP[t].style = T->style;
    }
}

void update_Default_Pose()
{
    int d;
    deformer * D;

    for (d = 0; d < deformerIndex; d ++)
    {
        D = deformers[d];
        add_Default_Pose_To_Deformer(D);
    }
}

pose * Action_Begin_Pose;

void init_Action_Begin_Transformers()
{
    Action_Begin_Transformers = malloc(TRANSFORMERS * sizeof(transformer *));
    Action_Begin_Transformers_Count = 0;
    Action_Begin_Pose = malloc(sizeof(pose));
    Action_Begin_Pose->transformers_count = 0;
    Action_Begin_Pose->TP = malloc(0);
}

void paste_Action_Begin()
{
    int t;

    transformer * T;
    pose * P = Action_Begin_Pose;

    for (t = 0; t < P->transformers_count; t ++)
    {
        T = Action_Begin_Transformers[t];
        T->rot_Order = P->TP[t].rot_Order;
        memcpy(T->scl, P->TP[t].scl, sizeof(float[3]));
        memcpy(T->scl_vec, P->TP[t].scl_vec, sizeof(float[3]));
        memcpy(T->rot, P->TP[t].rot, sizeof(float[3]));
        memcpy(T->rotVec, P->TP[t].rotVec, sizeof(float[3][3]));
        memcpy(T->rotVec_, P->TP[t].rotVec_, sizeof(float[3][3]));

        memcpy(T->rotVec_I, P->TP[t].rotVec_I, sizeof(float[3][3]));
        memcpy(T->rotVec_B, P->TP[t].rotVec_B, sizeof(float[3][3]));

        memcpy(T->pos, P->TP[t].pos, sizeof(float[3]));
        memcpy(T->pos_, P->TP[t].pos_, sizeof(float[3]));

        T->style = P->TP[t].style;
    }
}

void list_Action_Begin_Transformers(transformer * T)
{
    int c;
    transformer * C;

    for (c = 0; c < T->childcount; c ++)
    {
        C = T->childs[c];
        Action_Begin_Transformers[Action_Begin_Transformers_Count ++] = C;
        list_Action_Begin_Transformers(C);
    }
}

void create_Action_Begin_Pose(transformer * T)
{
    Action_Begin_Transformers[Action_Begin_Transformers_Count ++] = T;

    list_Action_Begin_Transformers(T);

    Action_Begin_Pose->transformers_count = Action_Begin_Transformers_Count;
    Action_Begin_Pose->TP = realloc(Action_Begin_Pose->TP, Action_Begin_Pose->transformers_count * sizeof(transformer_pose));

    int t;

    transformer * T0;

    pose * P = Action_Begin_Pose;

    printf("Action_Begin_Pose transformers_count %d\n", Action_Begin_Pose->transformers_count);

    for (t = 0; t < Action_Begin_Pose->transformers_count; t ++)
    {
        T0 = Action_Begin_Transformers[t];

        P->TP[t].rot_Order = T0->rot_Order;
        memcpy(P->TP[t].scl, T0->scl, sizeof(float[3]));
        memcpy(P->TP[t].scl_vec, T0->scl_vec, sizeof(float[3]));
        memcpy(P->TP[t].rot, T0->rot, sizeof(float[3]));
        memcpy(P->TP[t].rotVec, T0->rotVec, sizeof(float[3][3]));
        memcpy(P->TP[t].rotVec_, T0->rotVec_, sizeof(float[3][3]));
        memcpy(P->TP[t].rotVec_I, T0->rotVec_I, sizeof(float[3][3]));
        memcpy(P->TP[t].rotVec_B, T0->rotVec_B, sizeof(float[3][3]));
        memcpy(P->TP[t].pos, T0->pos, sizeof(float[3]));
        memcpy(P->TP[t].pos_, T0->pos_, sizeof(float[3]));
        P->TP[t].style = T0->style;

        memcpy(T0->rotVec_Pin, T0->rotVec_, sizeof(float[3][3]));
        //memcpy(T0->Pos_, T0->pos, sizeof(float[3]));
    }
}

void add_Pose_To_Deformer(deformer * D, int linear_pose)
{
    pose * P = malloc(sizeof(pose));
    poses[posesIndex] = P;
    P->index = posesIndex;
    P->Name = malloc(STRLEN * sizeof(char));
    sprintf(P->Name, "%s %d", "Pose", posesCount ++);
    P->transformers_count = D->Transformers_Count;
    P->TP = calloc(P->transformers_count, sizeof(transformer_pose));
    P->D = D;

    int t;
    transformer * T;

    float rotVec_I[3][3];

    for (t = 0; t < D->Transformers_Count; t ++)
    {
        T = D->Transformers[t];

        P->TP[t].rot_Order = T->rot_Order;
        memcpy(P->TP[t].scl, T->scl, sizeof(float[3]));
        memcpy(P->TP[t].scl_vec, T->scl_vec, sizeof(float[3]));
        memcpy(P->TP[t].rot, T->rot, sizeof(float[3]));
        memcpy(P->TP[t].rotVec, T->rotVec, sizeof(float[3][3]));
        memcpy(P->TP[t].rotVec_, T->rotVec_, sizeof(float[3][3]));
        memcpy(P->TP[t].rotVec_I, T->rotVec_I, sizeof(float[3][3]));
        memcpy(P->TP[t].rotVec_B, T->rotVec_B, sizeof(float[3][3]));

        if (linear_pose || (T->IK != NULL && T == T->IK->B))
        {
            P->TP[t].pos[0] = T->pos[0] - D->Delta[0];
            P->TP[t].pos[1] = T->pos[1] - D->Delta[1];
            P->TP[t].pos[2] = T->pos[2] - D->Delta[2];
        }
        else if (T->parent != NULL)
        {
            invert_Rotation_scale(T->parent, rotVec_I);
            rotate_center_(T->pos, rotVec_I, T->parent->pos, P->TP[t].pos);

//            memcpy(P->TP[t].pos, T->pos, sizeof(float[3]));
//            memcpy(P->TP[t].pos_, T->pos_, sizeof(float[3]));
        }

        P->TP[t].style = T->style;
    }

    D->Poses_Count ++;
    D->Poses = realloc(D->Poses, D->Poses_Count * sizeof(pose*));
    D->Poses[D->Poses_Count - 1] = P;

    posesIndex ++;
}

void list_Pose_Nodes(deformer * D, int currentPose)
{
    int p;
    pose * P;

    for (p = 0; p < D->Poses_Count; p ++)
    {
        P = D->Poses[p];

        //if (P->Name == NULL) continue;

        if (currentPose > 0 && P == poses[currentPose])
            D->current_pose = p;

        sprintf(Pose_Names[Poses_c], "%s", P->Name);
        Pose_List[Poses_c] = P->index;

        Pose_X_Offset[Poses_c] = 2;
        Pose_X_Collapsed[Poses_c] = 0;

        Poses_c ++;

        if (Poses_c >= POSES)
        {
            break;
        }
    }
}

void create_Poses_List(int PoseIndex)
{
    int d;
    deformer * D;
    Poses_c = 0;
    Deformer_Node_c = 1;

    int currentPose = Pose_List[PoseIndex];

    for (d = 0; d < deformerIndex; d ++)
    {
        D = deformers[d];

        if (D->selected)
            selected_deformer_node = Poses_c;

        memcpy(Pose_Names[Poses_c], D->Name, strlen(D->Name));
        Pose_Names[Poses_c][strlen(D->Name)] = '\0';
        Pose_List[Poses_c] = -Deformer_Node_c;
        Pose_X_Offset[Poses_c] = 1;
        Pose_X_Collapsed[Poses_c] = D->collapsed;
        Poses_c ++;
        Deformer_Node_c ++;

        if (Poses_c >= POSES)
        {
            break;
        }

        if (!D->collapsed)
            list_Pose_Nodes(D, currentPose);
    }
}

void replace_Pose_Name(char * EditString)
{
    if (Pose_List[PoseIndex] >= 0)
    {
        pose * P = poses[Pose_List[PoseIndex]];
        memcpy(P->Name, EditString, strlen(EditString));
        P->Name[strlen(EditString)] = '\0';
    }
    else
    {
        deformer * D = deformers[-(Pose_List[PoseIndex] + 1)];
        memcpy(D->Name, EditString, strlen(EditString));
        D->Name[strlen(EditString)] = '\0';
    }
}

void set_Pose_H_Button(int index)
{
    int i;
    for (i = 0; i < H_POSE_NUM; i ++)
    {
        Button_h_pose[i].color = UI_GRAYB;
    }
    if (index > -1)
        Button_h_pose[index].color = UI_GRAYD;
}

void remove_Deformer_Pose(pose * P)
{
    int index, p;

    int condition = 0;

    for (p = 0; p < posesIndex; p ++)
    {
        if (P == poses[p])
        {
            index = p;
            condition = 1;
            break;
        }
    }

    if (condition)
    {
        posesIndex --;

        for (p = index; p < posesIndex; p ++)
        {
            poses[p] = poses[p + 1];
            poses[p]->index = p;
        }
        free_pose(P);
    }
}

void delete_Deformer_Poses(deformer * D)
{
    int p;
    pose * P;

    for (p = 0; p < D->Poses_Count; p ++)
    {
        P = D->Poses[p];
        remove_Deformer_Pose(P);
    }
}

void unfix_pose_ik_goals(deformer * D, pose * P)
{
    int t;

    for (t = 0; t < P->transformers_count; t ++)
    {
        if (t >= D->Transformers_Count)
            break;

        if (P->TP[t].style == ik_fixed)
        {
            D->Transformers[t]->style = ik_goal;
            unfix_ik_goal(D->Transformers[t]);
        }
    }
}

void set_Deformer_Delta(deformer * D)
{
    transformer * T;

    if (D->Transformers_Count > 0)
    {
        T = D->Transformers[0];
        D->Delta[0] = T->pos[0] - D->Poses[0]->TP[0].pos[0];
        D->Delta[1] = T->pos[1] - D->Poses[0]->TP[0].pos[1];
        D->Delta[2] = T->pos[2] - D->Poses[0]->TP[0].pos[2];
    }
}

void rotate_Pose(deformer * D)
{
    int t;
    transformer * T;

    float pos[3];
    float rotVec[3][3];

    pos[0] = D->Poses[0]->TP->pos[0] + D->Delta[0];
    pos[1] = D->Poses[0]->TP->pos[1] + D->Delta[1];
    pos[2] = D->Poses[0]->TP->pos[2] + D->Delta[2];

    for (t = 0; t < D->Transformers_Count; t ++)
    {
        T = D->Transformers[t];

        rotate_center(T->pos, D->rotVec, pos, T->pos);
              // data, matrix, center, result

        memcpy(rotVec, T->rotVec, sizeof(float[3][3]));
        rotate_matrix_I(T->rotVec, D->rotVec, rotVec);
        rotate_matrix_I(T->rotVec_, D->rotVec, rotVec); // because keyframes need unscaled rotVec_ matrix
    }
}

int transfer_Poses_To_Deformer(Poses_In * Deformer_Poses, deformer * D)
{
    printf("transfer Poses To Deformer\n");

    int result = 0;

    int p;

    pose * P;

    for (p = 1; p < Deformer_Poses->Poses_c; p ++) // skip default pose
    {

        if (posesIndex >= POSES)
        {
            break;
        }

        P = poses_Import[p];

        if (P->transformers_count == D->Transformers_Count)
        {
            poses[posesIndex] = P;
            P->index = posesIndex;

            P->D = D;

            D->Poses = realloc(D->Poses, (D->Poses_Count + 1) * sizeof(pose*));

            if (D->Poses == NULL)
            {
                break;
            }

            D->Poses[D->Poses_Count] = P;

            D->Poses_Count ++;

            posesIndex ++;
            posesCount ++;

            result ++;
        }
        else
        {
            free(P->TP);
            free(P);
        }
    }

    return result;
}

#endif // POSES_H_INCLUDED
