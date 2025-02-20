/*
The MIT License

Copyright <2025> <Allan Kiipli>
*/

#ifndef TRAJECTORIES_H_INCLUDED
#define TRAJECTORIES_H_INCLUDED

/*
Trajectories come hand in hand with Curves.
They provide position info for linked Transformers
frame by frame.
*/

#define TRAJECTORIES 100

int Trj_c = 0;
char Trj_Names[TRAJECTORIES][STRLEN];

int TrjIndex = 0;
int currentTrj = 0;
int trj_start = 0;
int trjIndex = 0;

int Trj_Italic[TRAJECTORIES];
int Trj_List[TRAJECTORIES];

typedef struct
{
    int index;
    unsigned address;
    char * Name;
    int selected;
    curve * Curve;
    int update; // for animated curves
    int transformers_count;
    transformer ** Transformers;
}
trajectory;

trajectory * Trj = NULL;
trajectory * trajectories[TRAJECTORIES];

void insert_Trajectory_keyframe(trajectory * Trj, transformer * T, int frame, float Value)
{
    if ((trajectory*)T->Trj == Trj)
    {
        insert_trj_keyframe(T, frame, Value);
    }
}

void add_Transformer_To_Trajectory(transformer * T, trajectory * Trj)
{
    if (T->Trj == NULL && T != &World && T != CUBE->T)
    {
        transformer ** Transformers;

        Transformers = realloc(Trj->Transformers, sizeof(transformer *) * (Trj->transformers_count + 1));
        if (Transformers != NULL)
        {
            Trj->Transformers = Transformers;
            Trj->Transformers[Trj->transformers_count] = T;
            T->Trj = (traj *)Trj;
            Trj->transformers_count ++;
        }
    }
}

int add_New_Trajectory(curve * C)
{
    if (trjIndex >= TRAJECTORIES)
    {
        return -1;
    }
    trajectory * Trj = malloc(sizeof(trajectory));
    if (Trj == NULL)
    {
        return -1;
    }
    trajectories[trjIndex] = Trj;
    Trj->index = trjIndex;
    Trj->Name = malloc(STRLEN * sizeof(char));
    sprintf(Trj->Name, "%s %d", "Trajectory", trjIndex);
    Trj->selected = 0;
    Trj->Curve = C;
    C->Trj = (traj *)Trj;
    Trj->update = 0;
    Trj->transformers_count = 0;
    Trj->Transformers = malloc(0);
    trjIndex ++;
    return Trj->index;
}

void replace_Trj_Name(char * EditString)
{
    trajectory * Trj = trajectories[Trj_List[TrjIndex]];
    memcpy(Trj->Name, EditString, strlen(EditString));
    Trj->Name[strlen(EditString)] = '\0';
}

void set_Trj_H_Button(int index)
{
    int i;
    for (i = 0; i < H_TRAJ_NUM; i ++)
    {
        Button_h_traj[i].color = UI_GRAYB;
    }
    if (index > -1)
        Button_h_traj[index].color = UI_GRAYD;
}

void create_Trajectory_List()
{
    int t;
    trajectory * Trj;
    Trj_c = 0;

    for (t = 0; t < trjIndex; t ++)
    {
        Trj = trajectories[t];

        memcpy(Trj_Names[Trj_c], Trj->Name, strlen(Trj->Name));
        Trj_Names[Trj_c][strlen(Trj->Name)] = '\0';
        Trj_Italic[Trj_c] = Trj->selected;
        Trj_List[Trj_c] = Trj_c;
        Trj_c ++;
        if (Trj_c >= TRAJECTORIES)
            break;
    }
}

float curve_segment_Length(curve_segment * S)
{
    float V[3];
    V[0] = S->C[0] - S->A[0];
    V[1] = S->C[1] - S->A[1];
    V[2] = S->C[2] - S->A[2];
    float l = sqrt(V[0] * V[0] + V[1] * V[1] + V[2] * V[2]);
    S->len = l;
    return l;
}

float aggregate_Curve_Segment_Length(curve_segment * S, int Level)
{
    float l = 0;

    curve_segment * S0, * S1;

    if (S->subdivided && S->level <= Level)
    {
        S0 = S->segment[0];
        l += aggregate_Curve_Segment_Length(S0, Level);
        S1 = S->segment[1];
        l += aggregate_Curve_Segment_Length(S1, Level);
    }
    else
    {
        l = curve_segment_Length(S);
    }

    S->len = l;

    return l;
}

float calculate_Curve_Length_(curve * C, int Level)
{
    int s;
    curve_segment * S;

    float l = 0;

    for (s = 0; s < C->segment_count - 1; s ++)
    {
        S = C->segments[s];
        l += aggregate_Curve_Segment_Length(S, Level);
    }

    if (!C->open)
    {
        S = C->segments[C->segment_count - 1];
        l += aggregate_Curve_Segment_Length(S, Level);
    }

    return l;
}

void find_Trajectory_Pos(float Proportion, trajectory * Trj, float Pos[3], int Level)
{
    int s;
    int L = 0;

    curve * C = Trj->Curve;
    curve_segment * S = NULL;
    curve_segment * S0, * S1;

    float l = 0, l0;
    float remaining = 0;

    float Curve_Length;

    if (Trj->update)
    {
        Curve_Length = calculate_Curve_Length_(C, Level);
    }
    else
    {
        Curve_Length = C->len;
    }

    if (C->open && Proportion >= 1.0)
    {
        Proportion = 1.0;
    }
    else
    {
        Proportion = fmod(Proportion, 1.0);
    }

    float Position_On_Curve = Curve_Length * Proportion;
    float segment_count = C->segment_count - 1;

    if (!C->open)
    {
        segment_count = C->segment_count;
    }

    for (s = 0; s < segment_count; s ++)
    {
        S = C->segments[s];
        l0 = l;
        l += S->len;
        if (l >= Position_On_Curve)
        {
            remaining = Position_On_Curve - l0;
            break;
        }
    }

    if (S != NULL)
    {
        while (S->subdivided && L <= Level)
        {
            L ++;
            l = 0;
            S0 = S->segment[0];
            l += S0->len;
            S1 = S->segment[1];
            l += S1->len;

            if (remaining <= S0->len)
            {
                S = S0;
            }
            else
            {
                S = S1;
                remaining -= S0->len;
            }
        }

        float offset = remaining / S->len;

        float V[3];
        V[0] = S->C[0] - S->A[0];
        V[1] = S->C[1] - S->A[1];
        V[2] = S->C[2] - S->A[2];

        Pos[0] = V[0] * offset + S->A[0];
        Pos[1] = V[1] * offset + S->A[1];
        Pos[2] = V[2] * offset + S->A[2];
    }
    else
    {
        Pos[0] = 0.0;
        Pos[1] = 0.0;
        Pos[2] = 0.0;
    }
}

void set_T_Roll_z(transformer * T, float Value)
{
    T->rot[2] = Value;
}

void set_T_Trajectory_value(trajectory * Trj, transformer * T, float Value, int Level)
{
    float Pos[3];

    find_Trajectory_Pos(Value, Trj, Pos, Level);

    T->pos[0] = Pos[0];
    T->pos[1] = Pos[1];
    T->pos[2] = Pos[2];
}

int find_current_Segment(transformer * T, int frame)
{
    int f, s = 0;

    timeline * Tm;

    if (T->Timeline != NULL)
    {
        Tm = T->Timeline;

        for (f = 0; f < Tm->key_frames; f ++)
        {
            if (frame >= Tm->Frames[f] && frame < Tm->Frames[(f + 1) % Tm->key_frames]) //(frame == Tm->Frames[f])
            {
                s = f;
                break;
            }
            else if (frame >= Tm->Frames[f] && f == Tm->key_frames - 1)
            {
                s = f;
                break;
            }
        }
    }
    return s;
}

void move_Trajectories_Transformers_D(deformer * D, int frame, int level)
{
    int t, tr;

    transformer * T;
    trajectory * Trj;
    trajectory_Pack trj_P;

    for (tr = 0; tr < trjIndex; tr ++)
    {
        Trj = trajectories[tr];

        for (t = 0; t < Trj->transformers_count; t ++)
        {
            T = Trj->Transformers[t];
            if (T->Timeline != NULL && T->Deformer == D)
            {
                //T->Timeline->current_Segment = find_current_Segment(T, frame);
                //T->Timeline->start_Segment = 0;//T->Timeline->current_Segment;
                trj_P = get_T_Trajectory_value(T, frame);
                T->Trj_Value = trj_P.Value;
                set_T_Trajectory_value(Trj, T, T->Trj_Value, level);
                T->rot[2] = trj_P.Rollz;
            }
        }
    }
}

void move_Trajectories_Transformers(int frame, int level)
{
    int t, tr;

    transformer * T;
    trajectory * Trj;

    trajectory_Pack trj_P;

    for (tr = 0; tr < trjIndex; tr ++)
    {
        Trj = trajectories[tr];

        for (t = 0; t < Trj->transformers_count; t ++)
        {
            T = Trj->Transformers[t];
            if (T->Timeline != NULL && T->Deformer == NULL) // Deformer T is processed in move_Trajectories_Transformers_D before
            {
                //T->Timeline->current_Segment = find_current_Segment(T, frame);
                //T->Timeline->start_Segment = 0;//T->Timeline->current_Segment;
                trj_P = get_T_Trajectory_value(T, frame);
                T->Trj_Value = trj_P.Value;
                set_T_Trajectory_value(Trj, T, T->Trj_Value, level);
                T->rot[2] = trj_P.Rollz;
            }
        }
    }
}

void travel_By_Trajectory(transformer * T, trajectory * Trj, float Proportion, int subdLevel)
{
    float Pos[3];

    find_Trajectory_Pos(Proportion, Trj, Pos, subdLevel);

    T->pos[0] = Pos[0];
    T->pos[1] = Pos[1];
    T->pos[2] = Pos[2];
}

void create_Experimental_Trajectory(curve * C)
{
    trajectory * Trj = malloc(sizeof(trajectory));
    if (Trj != NULL)
    {
        trajectories[trjIndex ++] = Trj;
        Trj->Curve = C;
        Trj->update = 0;
    }
    else
    {
        printf("failed!\n");
    }
}

void free_Trajectory(trajectory * Trj)
{
    free(Trj->Name);
    free(Trj->Transformers);
    free(Trj);
}

void free_Trajectories()
{
    int t;

    for (t = 0; t < trjIndex; t ++)
    {
        free_Trajectory(trajectories[t]);
    }

    trjIndex = 0;
}

#endif // TRAJECTORIES_H_INCLUDED
