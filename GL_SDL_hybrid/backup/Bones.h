#ifndef BONES_H_INCLUDED
#define BONES_H_INCLUDED

#include "Poses.h"

#define BIG_DIST 1000000
#define BONES 200
#define bone_end 1

int currentOrder = yxz;

//struct bone
//{
//    int index;
//    char * Name;
//    int selected;
//    bone * parent;
//    float rotVec_[3][3];
//    float rotVec_I[3][3];
//    transformer * A;
//    transformer * B;
//    deformer * D;
//    float ** Distances;
//    int Distances_Count;
//    /*
//    Distances_Count equals deformers Objects_Count variable.
//    For every object it allocates its vertcount amount float variables.
//    For every vertex there, it measures its distance to the bone.
//    Then the distances are summed in Deformers Objects W_Aggregate
//    variable per object.
//
//    This aggregate is compared to the current distance per vertex,
//    per object. The less the distance the more influence bone gets.
//    This means that current distance is subtracted from aggregate
//    and result is divided with aggregate and subtracted from 1.
//
//    Before aggregate is summed, top canditates are included.
//    This depends on the binding type applied. If it is 4, then
//    four smallest distances get inclusion, effectively binding four
//    bones and excluding others for this vertex. Thus vertex groups
//    are formed bound to the bones in the deformer.
//    They are just bound to A transformer at first there.
//    These weights are encapsulated in Bone A transformers Selections
//    weights variable.
//    */
//};

bone * bones[BONES];
int bonesIndex = 0;

void transfer_Bones_rotVec_()
{
    int b;
    float rotVec_I[3][3];

    transformer * T;
    bone * B;

    for(b = 0; b < bonesIndex; b ++)
    {
        B = bones[b];
        T = B->A;
        if (T->parent != NULL)
            invert_Rotation(T->parent, rotVec_I);
        rotate_matrix_I(B->rotVec_I, rotVec_I, B->rotVec_);
    }

    //////////////// identity control ////////////////

//    printf("x aim %f %f %f\n", T->rotVec_[0][0], T->rotVec_[0][1], T->rotVec_[0][2]);
//    printf("y aim %f %f %f\n", T->rotVec_[1][0], T->rotVec_[1][1], T->rotVec_[1][2]);
//    printf("z aim %f %f %f\n", T->rotVec_[2][0], T->rotVec_[2][1], T->rotVec_[2][2]);
//
//    float rotVec_[3][3];
//
//    rotate_matrix_I(rotVec_, T->parent->rotVec_, B->rotVec_I);
//    printf("----\n");
//
//    printf("x aim %f %f %f\n", rotVec_[0][0], rotVec_[0][1], rotVec_[0][2]);
//    printf("y aim %f %f %f\n", rotVec_[1][0], rotVec_[1][1], rotVec_[1][2]);
//    printf("z aim %f %f %f\n", rotVec_[2][0], rotVec_[2][1], rotVec_[2][2]);
}

float length_(float V[3])
{
    float l = sqrt(V[0] * V[0] + V[1] * V[1] + V[2] * V[2]);
    return l;
}

void generate_Bones_In_Deformer(deformer * D)
{
    int b, t;
    bone * B;
    transformer * T;

    int condition;

    D->Bones = realloc(D->Bones, (D->Transformers_Count / 2) * sizeof(bone*));

    for (t = 0; t < D->Transformers_Count; t ++)
    {
        T = D->Transformers[t];
        if (T->Bone != NULL)
        {
            if (T == T->Bone->A)
            {
                condition = 1;
                for (b = 0; b < D->Bones_Count; b ++)
                {
                    B = D->Bones[b];
                    if (T->Bone == B)
                    {
                        condition = 0;
                        break;
                    }
                }

                if (condition)
                {
                    D->Bones[D->Bones_Count ++] = T->Bone;
                    T->Bone->D = D;
                }
            }
        }
    }

    D->Bones = realloc(D->Bones, D->Bones_Count * sizeof(bone*));
}

float distance_To_Point_On_Line(float Normal[3], float parentpos[3], float pos_A[3], float pos_B[3])
{
    float dist = 0;
    float Pos[3];
    float vec[3];
    float vec1[3];
    float vec2[3];
    float vec3[3];
    float dot, ndot;
    float len, len1, len2;

    vec[0] = parentpos[0] - pos_A[0];
    vec[1] = parentpos[1] - pos_A[1];
    vec[2] = parentpos[2] - pos_A[2];

    len = length_(vec);

    if (len > 0)
    {
        vec[0] /= len;
        vec[1] /= len;
        vec[2] /= len;
    }

    vec1[0] = pos_B[0] - pos_A[0];
    vec1[1] = pos_B[1] - pos_A[1];
    vec1[2] = pos_B[2] - pos_A[2];

    len1 = length_(vec1);

    if (len1 > 0)
    {
        vec1[0] /= len1;
        vec1[1] /= len1;
        vec1[2] /= len1;
    }

    dot = dot_productFF(vec, vec1);

    Pos[0] = len * dot * vec1[0] + pos_A[0];
    Pos[1] = len * dot * vec1[1] + pos_A[1];
    Pos[2] = len * dot * vec1[2] + pos_A[2];

    vec2[0] = parentpos[0] - Pos[0];
    vec2[1] = parentpos[1] - Pos[1];
    vec2[2] = parentpos[2] - Pos[2];

    len2 = length_(vec2);

    if (len2 > 0)
    {
        vec2[0] /= len2;
        vec2[1] /= len2;
        vec2[2] /= len2;
    }

    ndot = dot_productFF(vec2, Normal);

    if (dot <= 0) // to A
    {
        dist = len;
    }
    else
    {
        if (len * dot < len1) // on line
        {
            dist = len2;
        }
        else // to B
        {
            vec3[0] = parentpos[0] - pos_B[0];
            vec3[1] = parentpos[1] - pos_B[1];
            vec3[2] = parentpos[2] - pos_B[2];

            dist = length_(vec3);
        }
    }

    dist += LocatorSize * (1 - ndot);

    return dist;
}

void smallest_Distances_In_Deformer(deformer * D)
{
    int k, i, v, o, b;
    weight_encapsulator * W;
    object * O;
    bone * B;

    bone * B0;
    float D0;

    float min_Dist;

    int condition;

    for (o = 0; o < D->Objects_Count; o ++)
    {
        O = D->Objects[o];
        for (v = 0; v < O->vertcount; v ++)
        {
            O->WEncapsulator[v].Aggregate_dist = 0;
            for (i = 0; i < INFLUENCES; i ++)
            {
                O->WEncapsulator[v].Candidate_dist[i] = BIG_DIST;
                O->WEncapsulator[v].Candidate_bones[i] = NULL;
                O->WEncapsulator[v].Influences_Count = 0;
            }
        }
    }

    for (o = 0; o < D->Objects_Count; o ++)
    {
        O = D->Objects[o];
        for (v = 0; v < O->vertcount; v ++)
        {
            W = &O->WEncapsulator[v];
            min_Dist = BIG_DIST;
            for (b = 0; b < D->Bones_Count; b ++)
            {
                B = D->Bones[b];
                condition = 0;
                if (B->Distances[o][v] <= min_Dist)
                {
                    condition = 1;
                }
                else
                {
                    for (i = 0; i < INFLUENCES; i ++)
                    {
                        if (B->Distances[o][v] <= W->Candidate_dist[i])
                        {
                            condition = 1;
                            break;
                        }
                    }
                }
                if (condition)
                {
                    min_Dist = B->Distances[o][v];
                    W->Influences_Count ++;
                    for (i = 0; i < INFLUENCES - 1; i ++)
                    {
                        for (k = 0; k < INFLUENCES - i - 1; k ++)
                        {
                            if (W->Candidate_dist[k] < W->Candidate_dist[k + 1]) // to reverse use other comparison
                            {
                                B0 = W->Candidate_bones[k + 1];
                                D0 = W->Candidate_dist[k + 1];
                                W->Candidate_bones[k + 1] = W->Candidate_bones[k];
                                W->Candidate_dist[k + 1] = W->Candidate_dist[k];
                                W->Candidate_bones[k] = B0;
                                W->Candidate_dist[k] = D0;
                            }
                        }
                    }
                    O->WEncapsulator[v].Candidate_dist[0] = min_Dist;
                    O->WEncapsulator[v].Candidate_bones[0] = B;
                }
            }
        }
    }

    for (o = 0; o < D->Objects_Count; o ++)
    {
        O = D->Objects[o];
        for (v = 0; v < O->vertcount; v ++)
        {
            W = &O->WEncapsulator[v];
            if (W->Influences_Count > INFLUENCES)
                W->Influences_Count = INFLUENCES;
            for (i = 0; i < W->Influences_Count; i ++)
            {
                W->Aggregate_dist += W->Candidate_dist[i];
            }
        }
    }
}

void add_Encapsulated_Weights_To_Deformer(deformer * D)
{
    int s, i, v, o;
    object * O;
    bone * B;

    transformer * T;
    vert_selection * S;
    weight_encapsulator * W;
    float Weight;

    for (o = 0; o < D->Objects_Count; o ++)
    {
        O = D->Objects[o];
        for (v = 0; v < O->vertcount; v ++)
        {
            W = &O->WEncapsulator[v];
            for (i = 0; i < W->Influences_Count; i ++)
            {
                B = W->Candidate_bones[i];
                if (B != NULL)
                {
                    T = B->A;
                    if (T != NULL)
                    {
                        //    This means that current distance is subtracted from aggregate
                        //    and result is divided with aggregate and subtracted from 1.

                        if (W->Influences_Count > 1)
                            Weight = (W->Aggregate_dist - W->Candidate_dist[i]) / ((W->Influences_Count - 1) * W->Aggregate_dist);
                        else
                            Weight = 1;
                        //Weight = 1 - ((W->Aggregate_dist - W->Candidate_dist[i]) / W->Aggregate_dist);
                        //Weight = 1 - (W->Candidate_dist[i] / W->Aggregate_dist);
                        for (s = 0; s < T->Selections_Count; s ++)
                        {
                            S = T->Selections[s];
                            if (S->index == B->index && S->Object == O)
                            {
                                S->indices = realloc(S->indices, (S->indices_count + 1) * sizeof(int));
                                S->weights = realloc(S->weights, (S->indices_count + 1) * sizeof(float));
                                S->indices[S->indices_count] = v;
                                S->weights[S->indices_count ++] = Weight;
                            }
                        }
                    }
                }
            }
        }
    }
}

void generate_Vertex_Groups_In_Deformer(deformer * D)
{
    int b, o, i, index;
    int condition;

    bone * B;
    object * O;
    transformer * T;

    char Name[STRLEN];

    for (o = 0; o < D->Objects_Count; o ++)
    {
        for (b = 0; b < D->Bones_Count; b ++)
        {
            B = D->Bones[b];
            T = B->A;
            O = D->Objects[o];

            sprintf(Name, "%s %d", "Vert", B->index);
            condition = 1;
            for (i = 0; i < O->vertex_selections; i ++)
            {
                if (strcmp(O->vertex_selection[i]->Name, Name) == 0)
                {
                    index = i;
                    printf("Selection exists!\n");
                    condition = 0;
                    break;
                }
            }
            if (condition)
            {
                index = O->vertex_selections;
                O->vertex_selections ++;
                O->vertex_selection = realloc(O->vertex_selection, O->vertex_selections * sizeof(vert_selection*));
                O->vertex_selection[index] = malloc(sizeof(vert_selection));
                O->vertex_selection[index]->indices_count = 0;
                O->vertex_selection[index]->indices = malloc(0 * sizeof(int));
                O->vertex_selection[index]->weights = malloc(0 * sizeof(float));
                O->vertex_selection[index]->Object = O;
                O->vertex_selection[index]->Transformer = T;

                O->vertex_selection[index]->index = B->index; // temporarily
                O->vertex_selection[index]->Name = malloc(STRLEN * sizeof(char));
                memcpy(O->vertex_selection[index]->Name, Name,
                       strlen(Name));
                O->vertex_selection[index]->Name[strlen(Name)] = '\0';
            }
            else
            {
                O->vertex_selection[index]->indices_count = 0;
                O->vertex_selection[index]->index = B->index; // temporarily
            }
            add_Selection_To_Deformer(O, Name, T, D);
        }
    }
    add_Encapsulated_Weights_To_Deformer(D);
    create_Deformers_List(0, NULL);
}

void generate_Bones_Distances_In_Deformer(deformer * D)
{
    int v, o, b;
    vertex * V;
    object * O;
    bone * B;

    float pos[3];
    float norm[3];

    for (o = 0; o < D->Objects_Count; o ++)
    {
        O = D->Objects[o];
        for (v = 0; v < O->vertcount; v ++)
        {
            V = &O->verts[v / ARRAYSIZE][v % ARRAYSIZE];

            pos[0] = V->x;
            pos[1] = V->y;
            pos[2] = V->z;

            norm[0] = V->N.x;
            norm[1] = V->N.y;
            norm[2] = V->N.z;

            for (b = 0; b < D->Bones_Count; b ++)
            {
                B = D->Bones[b];
                B->Distances[o][v] = distance_To_Point_On_Line(norm, pos, B->A->pos, B->B->pos);
            }
        }
    }
}

void generate_Bones_Distances_Mapping_In_Deformer(deformer * D)
{
    int o, b;
    object * O;
    bone * B;

    for (b = 0; b < D->Bones_Count; b ++)
    {
        B = D->Bones[b];
        B->Distances = realloc(B->Distances, D->Objects_Count * sizeof(float*));
        B->Distances_Count = D->Objects_Count;
    }

    for (o = 0; o < D->Objects_Count; o ++)
    {
        O = D->Objects[o];
        for (b = 0; b < D->Bones_Count; b ++)
        {
            B = D->Bones[b];
            B->Distances[o] = malloc(O->vertcount * sizeof(float));
        }
    }
}

int add_Bone(transformer * T0, transformer * T1)
{
    bone * B = malloc(sizeof(bone));
    if (B == NULL)
        return -1;
    bones[bonesIndex] = B;
    B->index = bonesIndex;
    B->Name = malloc(STRLEN * sizeof(char));
    sprintf(B->Name, "%s %d", "Bone", bonesIndex);
    memcpy(&B->rotVec_, (float[3][3]) {{1.0, 0.0, 0.0}, {0.0, 1.0, 0.0}, {0.0, 0.0, 1.0}}, sizeof B->rotVec_);
    B->A = T0;
    B->B = T1;
    B->B->style = bone_end;
    B->A->Bone = B;
    B->B->Bone = B;
    B->parent = B->A->parent->Bone;
    B->A->rot_Order = currentOrder;
    B->B->rot_Order = currentOrder;
    B->D = NULL;
    B->Distances = malloc(0 * sizeof(float*));
    B->Distances_Count = 0;

    bonesIndex ++;
    return B->index;
}

void cross_Product(float v1[3], float v2[3], float N[3])
{
    N[0] = v1[1] * v2[2] - v1[2] * v2[1];
    N[1] = v1[2] * v2[0] - v1[0] * v2[2];
    N[2] = v1[0] * v2[1] - v1[1] * v2[0];
}

void closest_Point_On_Line(float parentpos[3], float pos_A[3], float pos_B[3], float Pos[3])
{
    float vec[3];
    float vec1[3];
    float len, len1;

    vec[0] = parentpos[0] - pos_A[0];
    vec[1] = parentpos[1] - pos_A[1];
    vec[2] = parentpos[2] - pos_A[2];

    len = length_(vec);

    if (len > 0)
    {
        vec[0] /= len;
        vec[1] /= len;
        vec[2] /= len;
    }

    vec1[0] = pos_B[0] - pos_A[0];
    vec1[1] = pos_B[1] - pos_A[1];
    vec1[2] = pos_B[2] - pos_A[2];

    len1 = length_(vec1);

    if (len1 > 0)
    {
        vec1[0] /= len1;
        vec1[1] /= len1;
        vec1[2] /= len1;
    }

    float dot = dot_productFF(vec, vec1);

    if (dot == 0)
    {
        Pos[0] = pos_A[0];
        Pos[1] = pos_A[1];
        Pos[2] = pos_A[2];
    }
    else
    {
        Pos[0] = len * dot * vec1[0] + pos_A[0];
        Pos[1] = len * dot * vec1[1] + pos_A[1];
        Pos[2] = len * dot * vec1[2] + pos_A[2];
    }
}

void synthesize_Bone_Alignement(bone * B)
{
    float mag, angle;
    float pos[3];

    if (B->A->rot_Order == zxy)
    {
        B->rotVec_[1][0] = B->B->pos[0] - B->A->pos[0];
        B->rotVec_[1][1] = B->B->pos[1] - B->A->pos[1];
        B->rotVec_[1][2] = B->B->pos[2] - B->A->pos[2];
        mag = sqrt(B->rotVec_[1][0] * B->rotVec_[1][0] +
                   B->rotVec_[1][1] * B->rotVec_[1][1] +
                   B->rotVec_[1][2] * B->rotVec_[1][2]);
        if (mag > 0)
        {
            B->rotVec_[1][0] /= mag;
            B->rotVec_[1][1] /= mag;
            B->rotVec_[1][2] /= mag;
        }
        else
        {
            B->rotVec_[1][0] = 0;
            B->rotVec_[1][1] = 1;
            B->rotVec_[1][2] = 0;
        }

        if(B->A->parent->parent != NULL)
        {
            closest_Point_On_Line(B->A->parent->parent->pos, B->A->pos, B->B->pos, pos);
            B->rotVec_[0][0] = B->A->parent->parent->pos[0] - pos[0];
            B->rotVec_[0][1] = B->A->parent->parent->pos[1] - pos[1];
            B->rotVec_[0][2] = B->A->parent->parent->pos[2] - pos[2];
            mag = length_(B->rotVec_[0]);
            if (mag > 0)
            {
                B->rotVec_[0][0] /= mag;
                B->rotVec_[0][1] /= mag;
                B->rotVec_[0][2] /= mag;
            }
            else
            {
                angle = atan2(B->rotVec_[1][0], B->rotVec_[1][2]);
                angle += pi_2;
                B->rotVec_[0][0] = sin(angle);
                B->rotVec_[0][1] = 0;
                B->rotVec_[0][2] = cos(angle);
            }
        }
        else
        {
            angle = atan2(B->rotVec_[1][0], B->rotVec_[1][2]);
            angle += pi_2;
            B->rotVec_[0][0] = sin(angle);
            B->rotVec_[0][1] = 0;
            B->rotVec_[0][2] = cos(angle);
        }

        cross_Product(B->rotVec_[0], B->rotVec_[1], B->rotVec_[2]);
    }
    else if (B->A->rot_Order == yxz)
    {
        B->rotVec_[2][0] = B->B->pos[0] - B->A->pos[0];
        B->rotVec_[2][1] = B->B->pos[1] - B->A->pos[1];
        B->rotVec_[2][2] = B->B->pos[2] - B->A->pos[2];
        mag = sqrt(B->rotVec_[2][0] * B->rotVec_[2][0] +
                   B->rotVec_[2][1] * B->rotVec_[2][1] +
                   B->rotVec_[2][2] * B->rotVec_[2][2]);
        if (mag > 0)
        {
            B->rotVec_[2][0] /= mag;
            B->rotVec_[2][1] /= mag;
            B->rotVec_[2][2] /= mag;
        }
        else
        {
            B->rotVec_[2][0] = 0;
            B->rotVec_[2][1] = 0;
            B->rotVec_[2][2] = 1;
        }

        if(B->A->parent->parent != NULL)
        {
            closest_Point_On_Line(B->A->parent->parent->pos, B->A->pos, B->B->pos, pos);
            B->rotVec_[0][0] = B->A->parent->parent->pos[0] - pos[0];
            B->rotVec_[0][1] = B->A->parent->parent->pos[1] - pos[1];
            B->rotVec_[0][2] = B->A->parent->parent->pos[2] - pos[2];
            mag = length_(B->rotVec_[0]);
            if (mag > 0)
            {
                B->rotVec_[0][0] /= mag;
                B->rotVec_[0][1] /= mag;
                B->rotVec_[0][2] /= mag;
            }
            else
            {
                angle = atan2(B->rotVec_[2][1], B->rotVec_[2][0]);
                angle += pi_2;
                B->rotVec_[0][0] = cos(angle);
                B->rotVec_[0][1] = sin(angle);
                B->rotVec_[0][2] = 0;
            }
        }
        else
        {
            angle = atan2(B->rotVec_[2][1], B->rotVec_[2][0]);
            angle += pi_2;
            B->rotVec_[0][0] = cos(angle);
            B->rotVec_[0][1] = sin(angle);
            B->rotVec_[0][2] = 0;
        }

        cross_Product(B->rotVec_[2], B->rotVec_[0], B->rotVec_[1]);
    }
    else if (B->A->rot_Order == zyx)
    {
        B->rotVec_[0][0] = B->B->pos[0] - B->A->pos[0];
        B->rotVec_[0][1] = B->B->pos[1] - B->A->pos[1];
        B->rotVec_[0][2] = B->B->pos[2] - B->A->pos[2];
        mag = sqrt(B->rotVec_[0][0] * B->rotVec_[0][0] +
                   B->rotVec_[0][1] * B->rotVec_[0][1] +
                   B->rotVec_[0][2] * B->rotVec_[0][2]);
        if (mag > 0)
        {
            B->rotVec_[0][0] /= mag;
            B->rotVec_[0][1] /= mag;
            B->rotVec_[0][2] /= mag;
        }
        else
        {
            B->rotVec_[0][0] = 1;
            B->rotVec_[0][1] = 0;
            B->rotVec_[0][2] = 0;
        }

        if(B->A->parent->parent != NULL)
        {
            closest_Point_On_Line(B->A->parent->parent->pos, B->A->pos, B->B->pos, pos);
            B->rotVec_[1][0] = B->A->parent->parent->pos[0] - pos[0];
            B->rotVec_[1][1] = B->A->parent->parent->pos[1] - pos[1];
            B->rotVec_[1][2] = B->A->parent->parent->pos[2] - pos[2];
            mag = length_(B->rotVec_[1]);
            if (mag > 0)
            {
                B->rotVec_[1][0] /= mag;
                B->rotVec_[1][1] /= mag;
                B->rotVec_[1][2] /= mag;
            }
            else
            {
                angle = atan2(B->rotVec_[0][2], B->rotVec_[0][1]);
                angle += pi_2;
                B->rotVec_[1][0] = 0;
                B->rotVec_[1][1] = cos(angle);
                B->rotVec_[1][2] = sin(angle);
            }
        }
        else
        {
            angle = atan2(B->rotVec_[0][2], B->rotVec_[0][1]);
            angle += pi_2;
            B->rotVec_[1][0] = 0;
            B->rotVec_[1][1] = cos(angle);
            B->rotVec_[1][2] = sin(angle);
        }

        cross_Product(B->rotVec_[0], B->rotVec_[1], B->rotVec_[2]);
    }
    else if (B->A->rot_Order == xyz)
    {
        B->rotVec_[2][0] = B->B->pos[0] - B->A->pos[0];
        B->rotVec_[2][1] = B->B->pos[1] - B->A->pos[1];
        B->rotVec_[2][2] = B->B->pos[2] - B->A->pos[2];
        mag = sqrt(B->rotVec_[2][0] * B->rotVec_[2][0] +
                   B->rotVec_[2][1] * B->rotVec_[2][1] +
                   B->rotVec_[2][2] * B->rotVec_[2][2]);
        if (mag > 0)
        {
            B->rotVec_[2][0] /= mag;
            B->rotVec_[2][1] /= mag;
            B->rotVec_[2][2] /= mag;
        }
        else
        {
            B->rotVec_[2][0] = 0;
            B->rotVec_[2][1] = 0;
            B->rotVec_[2][2] = 1;
        }

        if(B->A->parent->parent != NULL)
        {
            closest_Point_On_Line(B->A->parent->parent->pos, B->A->pos, B->B->pos, pos);
            B->rotVec_[1][0] = B->A->parent->parent->pos[0] - pos[0];
            B->rotVec_[1][1] = B->A->parent->parent->pos[1] - pos[1];
            B->rotVec_[1][2] = B->A->parent->parent->pos[2] - pos[2];
            mag = length_(B->rotVec_[1]);
            if (mag > 0)
            {
                B->rotVec_[1][0] /= mag;
                B->rotVec_[1][1] /= mag;
                B->rotVec_[1][2] /= mag;
            }
            else
            {
                angle = atan2(B->rotVec_[2][1], B->rotVec_[2][0]);
                angle += pi_2;
                B->rotVec_[1][0] = cos(angle);
                B->rotVec_[1][1] = sin(angle);
                B->rotVec_[1][2] = 0;
            }
        }
        else
        {
            angle = atan2(B->rotVec_[2][1], B->rotVec_[2][0]);
            angle += pi_2;
            B->rotVec_[1][0] = cos(angle);
            B->rotVec_[1][1] = sin(angle);
            B->rotVec_[1][2] = 0;
        }

        cross_Product(B->rotVec_[1], B->rotVec_[2], B->rotVec_[0]);
    }
    else if (B->A->rot_Order == xzy)
    {
        B->rotVec_[1][0] = B->B->pos[0] - B->A->pos[0];
        B->rotVec_[1][1] = B->B->pos[1] - B->A->pos[1];
        B->rotVec_[1][2] = B->B->pos[2] - B->A->pos[2];
        mag = sqrt(B->rotVec_[1][0] * B->rotVec_[1][0] +
                   B->rotVec_[1][1] * B->rotVec_[1][1] +
                   B->rotVec_[1][2] * B->rotVec_[1][2]);
        if (mag > 0)
        {
            B->rotVec_[1][0] /= mag;
            B->rotVec_[1][1] /= mag;
            B->rotVec_[1][2] /= mag;
        }
        else
        {
            B->rotVec_[1][0] = 0;
            B->rotVec_[1][1] = 1;
            B->rotVec_[1][2] = 0;
        }

        if(B->A->parent->parent != NULL)
        {
            closest_Point_On_Line(B->A->parent->parent->pos, B->A->pos, B->B->pos, pos);
            B->rotVec_[2][0] = B->A->parent->parent->pos[0] - pos[0];
            B->rotVec_[2][1] = B->A->parent->parent->pos[1] - pos[1];
            B->rotVec_[2][2] = B->A->parent->parent->pos[2] - pos[2];
            mag = length_(B->rotVec_[2]);
            if (mag > 0)
            {
                B->rotVec_[2][0] /= mag;
                B->rotVec_[2][1] /= mag;
                B->rotVec_[2][2] /= mag;
            }
            else
            {
                angle = atan2(B->rotVec_[1][0], B->rotVec_[1][2]);
                angle += pi_2;
                B->rotVec_[2][0] = sin(angle);
                B->rotVec_[2][1] = 0;
                B->rotVec_[2][2] = cos(angle);
            }
        }
        else
        {
            angle = atan2(B->rotVec_[1][0], B->rotVec_[1][2]);
            angle += pi_2;
            B->rotVec_[2][0] = sin(angle);
            B->rotVec_[2][1] = 0;
            B->rotVec_[2][2] = cos(angle);
        }

        cross_Product(B->rotVec_[1], B->rotVec_[2], B->rotVec_[0]);
    }
    else if (B->A->rot_Order == yzx)
    {
        B->rotVec_[0][0] = B->B->pos[0] - B->A->pos[0];
        B->rotVec_[0][1] = B->B->pos[1] - B->A->pos[1];
        B->rotVec_[0][2] = B->B->pos[2] - B->A->pos[2];
        mag = sqrt(B->rotVec_[0][0] * B->rotVec_[0][0] +
                   B->rotVec_[0][1] * B->rotVec_[0][1] +
                   B->rotVec_[0][2] * B->rotVec_[0][2]);
        if (mag > 0)
        {
            B->rotVec_[0][0] /= mag;
            B->rotVec_[0][1] /= mag;
            B->rotVec_[0][2] /= mag;
        }
        else
        {
            B->rotVec_[0][0] = 1;
            B->rotVec_[0][1] = 0;
            B->rotVec_[0][2] = 0;
        }

        if(B->A->parent->parent != NULL)
        {
            closest_Point_On_Line(B->A->parent->parent->pos, B->A->pos, B->B->pos, pos);
            B->rotVec_[2][0] = B->A->parent->parent->pos[0] - pos[0];
            B->rotVec_[2][1] = B->A->parent->parent->pos[1] - pos[1];
            B->rotVec_[2][2] = B->A->parent->parent->pos[2] - pos[2];
            mag = length_(B->rotVec_[2]);
            if (mag > 0)
            {
                B->rotVec_[2][0] /= mag;
                B->rotVec_[2][1] /= mag;
                B->rotVec_[2][2] /= mag;
            }
            else
            {
                angle = atan2(B->rotVec_[0][2], B->rotVec_[0][1]);
                angle += pi_2;
                B->rotVec_[2][0] = 0;
                B->rotVec_[2][1] = cos(angle);
                B->rotVec_[2][2] = sin(angle);
            }
        }
        else
        {
            angle = atan2(B->rotVec_[0][2], B->rotVec_[0][1]);
            angle += pi_2;
            B->rotVec_[2][0] = 0;
            B->rotVec_[2][1] = cos(angle);
            B->rotVec_[2][2] = sin(angle);
        }

        cross_Product(B->rotVec_[2], B->rotVec_[0], B->rotVec_[1]);
    }

    memcpy(B->A->rotVec, B->rotVec_, sizeof(B->A->rotVec));
    memcpy(B->A->rotVec_, B->rotVec_, sizeof(B->A->rotVec_));

    memcpy(B->B->rotVec, B->A->rotVec, sizeof(B->B->rotVec));
    memcpy(B->B->rotVec_, B->A->rotVec_, sizeof(B->B->rotVec_));
}

#endif // BONES_H_INCLUDED
