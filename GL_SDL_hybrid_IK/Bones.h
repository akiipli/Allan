/*
The MIT License

Copyright <2018> <Allan Kiipli>
*/

#ifndef BONES_H_INCLUDED
#define BONES_H_INCLUDED

#define BIG_DIST 1000000

float  sharpness = 2.0;
float  sharpness2 = 4.0;

int Bones_c;
char Bone_Names[BONES][STRLEN];
int BoneIndex;
int bone_start;

int Bone_Italic[BONES];

int Bone_List[BONES];
int currentBone = 0;

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

float distance_To_Point_On_Line(float Normal[3], float parentpos[3], float pos_A[3], float pos_B[3], transformer * T)
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

    dist += T->LocatorSize * (1 - ndot);

    return dist;
}

void smallest_Distances_In_Deformer(deformer * D)
{
    int k, i, v, o_s, o, b;
    weight_encapsulator * W;
    object * O;
    bone * B;

    bone * B0;
    float D0;

    float min_Dist;

    int condition;

    for (o = 0; o < D->Objects_Count; o ++)
    {
        condition = 0;

        for (o_s = 0; o_s < selected_object_count; o_s ++)
        {
            O = objects[selected_objects[o_s]];
            if (O == D->Objects[o])
            {
                condition = 1;
                break;
            }
        }

        if (condition)
        {
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
    }

    for (o = 0; o < D->Objects_Count; o ++)
    {
        condition = 0;

        for (o_s = 0; o_s < selected_object_count; o_s ++)
        {
            O = objects[selected_objects[o_s]];
            if (O == D->Objects[o])
            {
                condition = 1;
                break;
            }
        }

        if (condition)
        {
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
    }

    for (o = 0; o < D->Objects_Count; o ++)
    {
        condition = 0;

        for (o_s = 0; o_s < selected_object_count; o_s ++)
        {
            O = objects[selected_objects[o_s]];
            if (O == D->Objects[o])
            {
                condition = 1;
                break;
            }
        }

        if (condition)
        {
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
}

void add_Encapsulated_Weights_To_Deformer(deformer * D)
{
    int s, i, v, o_s, o;
    object * O;
    bone * B;

    transformer * T;
    vert_selection * S;
    weight_encapsulator * W;
    float Weight;

    int condition;

    for (o = 0; o < D->Objects_Count; o ++)
    {
        condition = 0;

        for (o_s = 0; o_s < selected_object_count; o_s ++)
        {
            O = objects[selected_objects[o_s]];
            if (O == D->Objects[o])
            {
                condition = 1;
                break;
            }
        }

        if (condition)
        {
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
}

void init_split_Selections(vert_selection * S)
{
    S->split[0].indices_count = 0;
    S->split[0].indices = realloc(S->split[0].indices, S->indices_count * sizeof(int));
    S->split[0].weights = realloc(S->split[0].weights, S->indices_count * sizeof(float));

    S->split[1].indices_count = 0;
    S->split[1].indices = realloc(S->split[1].indices, S->indices_count * sizeof(int));
    S->split[1].weights = realloc(S->split[1].weights, S->indices_count * sizeof(float));
}

float dist_Vert_To_Pos(vertex * V, float pos[3])
{
    float x, y, z;
    x = V->x - pos[0];
    y = V->y - pos[1];
    z = V->z - pos[2];
    return sqrt(x * x + y * y + z * z);
}

float dist_To_Pos(float Pos[3], float pos[3])
{
    float x, y, z;
    x = Pos[0] - pos[0];
    y = Pos[1] - pos[1];
    z = Pos[2] - pos[2];
    return sqrt(x * x + y * y + z * z);
}


void join_vertex_groups(vert_selection * S, split_selection S0)
{
    int i;
    int c = S->indices_count;
    S->indices_count += S0.indices_count;
    S->indices = realloc(S->indices, S->indices_count * sizeof(int));
    S->weights = realloc(S->weights, S->indices_count * sizeof(float));

    for (i = 0; i < S0.indices_count; i ++)
    {
        S->indices[i + c] = S0.indices[i];
        S->weights[i + c] = S0.weights[i];
    }
}

void normalize_Weights_In_Deformer(deformer * D)
{
    int i, w, idx, s, o_s, o;

    object * O;
    vert_selection * S;

    int condition;

    for (o = 0; o < D->Objects_Count; o ++)
    {
        condition = 0;

        for (o_s = 0; o_s < selected_object_count; o_s ++)
        {
            O = objects[selected_objects[o_s]];
            if (O == D->Objects[o])
            {
                condition = 1;
                break;
            }
        }

        if (condition)
        {

            for (w = 0; w < O->vertcount; w ++)
            {
                O->WEncapsulator[w].Aggregate_dist = 0;
            }

            for (s = 0; s < D->Selections_Count; s ++)
            {
                S = D->Selections[s];

                if (S->Object == O)
                {
                    for (i = 0; i < S->indices_count; i ++)
                    {
                        idx = S->indices[i];
                        O->WEncapsulator[idx].Aggregate_dist += S->weights[i];
                    }
                }
            }

            for (s = 0; s < D->Selections_Count; s ++)
            {
                S = D->Selections[s];

                if (S->Object == O)
                {
                    for (i = 0; i < S->indices_count; i ++)
                    {
                        idx = S->indices[i];
                        if (O->WEncapsulator[idx].Aggregate_dist > 0)
                            S->weights[i] /= O->WEncapsulator[idx].Aggregate_dist;
                        else
                            S->weights[i] = 0;
                    }
                }
            }
        }
    }
}

void generate_Extended_Groups_In_Deformer(deformer * D)
{
    int c, o_s, o, s, v, b;

    bone * B, * B0, * B1;
    object * O, * O0, * O1;
    vert_selection * S, * S0;

    int condition;

    for (o = 0; o < D->Objects_Count; o ++)
    {
        condition = 0;

        for (o_s = 0; o_s < selected_object_count; o_s ++)
        {
            O = objects[selected_objects[o_s]];
            if (O == D->Objects[o])
            {
                condition = 1;
                break;
            }
        }

        if (condition)
        {
            for (b = 0; b < D->Bones_Count; b ++)
            {
                B = D->Bones[b];
                B0 = B->A->parent->Bone;
                for (s = 0; s < B->A->Selections_Count; s ++)
                {
                    S = B->A->Selections[s];
                    O0 = S->Object;
                    if (O == O0)
                    {
                        if (B0 != NULL)
                        {
                            for (v = 0; v < B0->A->Selections_Count; v ++)
                            {
                                S0 = B0->A->Selections[v];
                                O1 = S0->Object;
                                if (O == O1)
                                {
                                    join_vertex_groups(S, S0->split[1]);
                                }
                            }
                        }
                        for (c = 0; c < B->B->childcount; c ++)
                        {
                            B1 = B->B->childs[c]->Bone;
                            if (B1 != NULL)
                            {
                                for (v = 0; v < B1->A->Selections_Count; v ++)
                                {
                                    S0 = B1->A->Selections[v];
                                    O1 = S0->Object;
                                    if (O == O1)
                                    {
                                        join_vertex_groups(S, S0->split[0]);
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

void generate_Split_Groups_In_Deformer(deformer * D)
{
    printf ("generate Split Groups In Deformer\n");

    int idx, i, s, b;

    bone * B;
    object * O;
    vertex * V;
    vert_selection * S;

    float dist_To_A, dist_To_B, lenght_sum, lenght_sum_2;

    for (b = 0; b < D->Bones_Count; b ++)
    {
        B = D->Bones[b];
        for (s = 0; s < B->A->Selections_Count; s ++)
        {
            S = B->A->Selections[s];
            init_split_Selections(S);
            O = S->Object;
            for (i = 0; i < S->indices_count; i ++)
            {
                idx = S->indices[i];
                V = &O->verts[idx / ARRAYSIZE][idx % ARRAYSIZE];

                dist_To_A = dist_Vert_To_Pos(V, B->A->pos);
                dist_To_B = dist_Vert_To_Pos(V, B->B->pos);

                lenght_sum = (dist_To_A + dist_To_B);
                lenght_sum_2 = lenght_sum / 2;

                if (dist_To_A < dist_To_B)
                {
                    S->split[0].indices[S->split[0].indices_count] = idx;
                    if (lenght_sum > 0)
                        S->split[0].weights[S->split[0].indices_count] = (lenght_sum_2 - dist_To_A) / lenght_sum;
                    else
                        S->split[0].weights[S->split[0].indices_count] = 0;
                    S->split[0].indices_count ++;
                }
                else
                {
                    S->split[1].indices[S->split[1].indices_count] = idx;
                    if (lenght_sum > 0)
                        S->split[1].weights[S->split[1].indices_count] = (lenght_sum_2 - dist_To_B) / lenght_sum;
                    else
                        S->split[1].weights[S->split[1].indices_count] = 0;
                    S->split[1].indices_count ++;
                }
            }
            S->split[0].indices = realloc(S->split[0].indices, S->split[0].indices_count * sizeof(int));
            S->split[0].weights = realloc(S->split[0].weights, S->split[0].indices_count * sizeof(float));

            S->split[1].indices = realloc(S->split[1].indices, S->split[1].indices_count * sizeof(int));
            S->split[1].weights = realloc(S->split[1].weights, S->split[1].indices_count * sizeof(float));
        }
    }
}

void free_Bone_Distances_In_Deformer(deformer * D)
{
    int d, b;
    bone * B;

    for (b = 0; b < D->Bones_Count; b ++)
    {
        B = D->Bones[b];
        for (d = 0; d < B->Distances_Count; d ++)
        {
            free(B->Distances[d]);
        }
        B->Distances_Count = 0;
    }
}

void delete_Zero_Selections_From_Objects(deformer * D)
{
    int o_s, o, v, i;
    int condition;

    object * O;
    vert_selection * S;
    transformer * T;

    for (o = 0; o < D->Objects_Count; o ++)
    {
        condition = 0;

        for (o_s = 0; o_s < selected_object_count; o_s ++)
        {
            O = objects[selected_objects[o_s]];
            if (O == D->Objects[o])
            {
                condition = 1;
                break;
            }
        }

        if (condition)
        {
            for (v = O->vertex_selections - 1; v >= 0; v --)
            {
                S = O->vertex_selection[v];
                T = S->Transformer;

                if (S->indices_count == 0)
                {
                    if (T == NULL)
                    {
                        //printf("%u %u\n", sizeof(S->indices), sizeof(S->weights));
                        free(O->vertex_selection[v]->indices);
                        free(O->vertex_selection[v]->weights);

                        //printf("%u %u\n", sizeof(S->split[0].indices), sizeof(S->split[0].weights));
                        free(O->vertex_selection[v]->split[0].indices);
                        free(O->vertex_selection[v]->split[0].weights);

                        //printf("%u %u\n", sizeof(S->split[1].indices), sizeof(S->split[1].weights));
                        free(O->vertex_selection[v]->split[1].indices);
                        free(O->vertex_selection[v]->split[1].weights);

                        free(O->vertex_selection[v]);

                        O->vertex_selections -= 1;
                        for (i = v; i < O->vertex_selections; i ++)
                        {
                            O->vertex_selection[i] = O->vertex_selection[i + 1];
                            O->vertex_selection[i]->index -= 1;
                        }
                    }
                }
            }
        }
    }
}

void delete_Zero_Selections_From_Deformer(deformer * D)
{
    int o_s, o, v, index, s, d, t, t_s;
    int condition;

    object * O;
    vert_selection * S;
    transformer * T;

    for (o = 0; o < D->Objects_Count; o ++)
    {
        condition = 0;

        for (o_s = 0; o_s < selected_object_count; o_s ++)
        {
            O = objects[selected_objects[o_s]];
            if (O == D->Objects[o])
            {
                condition = 1;
                break;
            }
        }

        if (condition)
        {
            for (v = 0; v < O->vertex_selections; v ++)
            {
                S = O->vertex_selection[v];
                if (S->indices_count == 0)
                {
                    condition = 0;

                    for (s = 0; s < D->Selections_Count; s ++)
                    {
                        if (D->Selections[s] == S)
                        {
                            index = s;
                            condition = 1;
                            break;
                        }
                    }

                    if (condition)
                    {
                        //printf("--- %s %d %u\n", S->Name, S->indices_count, (unsigned)&S);

                        D->Selections_Count --;
                        for (d = index; d < D->Selections_Count; d ++)
                        {
                            D->Selections[d] = D->Selections[d + 1];
                        }

                        T = S->Transformer;

                        if (T != NULL)
                        {
                            //printf("%s\n", T->Name);

                            for (t_s = 0; t_s < T->Selections_Count; t_s ++)
                            {
                                condition = 0;

                                //printf("%s %d %u\n", T->Selections[t_s]->Name, T->Selections[t_s]->indices_count, (unsigned)&T->Selections[t_s]);

                                if (T->Selections[t_s]->indices_count == 0)
                                {
                                    index = t_s;
                                    condition = 1;
                                }

                                if (condition)
                                {
                                    //printf("1\n");
                                    S->Transformer = NULL;
                                    T->Selections_Count --;
                                    for (t = index; t < T->Selections_Count; t ++)
                                    {
                                        T->Selections[t] = T->Selections[t + 1];
                                    }
                                }
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
    int b, o_s, o, i, index;
    int condition;

    bone * B;
    object * O;
    transformer * T;

    char Name[STRLEN];

    for (o = 0; o < D->Objects_Count; o ++)
    {
        condition = 0;

        for (o_s = 0; o_s < selected_object_count; o_s ++)
        {
            O = objects[selected_objects[o_s]];
            if (O == D->Objects[o])
            {
                condition = 1;
                break;
            }
        }

        if (condition)
        {
            for (b = 0; b < D->Bones_Count; b ++)
            {
                B = D->Bones[b];
                T = B->A;

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

                    O->vertex_selection[index]->split[0].indices_count = 0;
                    O->vertex_selection[index]->split[0].indices = malloc(0 * sizeof(int));
                    O->vertex_selection[index]->split[0].weights = malloc(0 * sizeof(int));
                    O->vertex_selection[index]->split[1].indices_count = 0;
                    O->vertex_selection[index]->split[1].indices = malloc(0 * sizeof(int));
                    O->vertex_selection[index]->split[1].weights = malloc(0 * sizeof(int));

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
    }
    add_Encapsulated_Weights_To_Deformer(D);
    create_Deformers_List(0, NULL);
}

void generate_Bones_Distances_In_Deformer(deformer * D)
{
    int v, o_s, o, b;
    vertex * V;
    object * O;
    bone * B;

    float pos[3];
    float norm[3];

    int condition;

    for (o = 0; o < D->Objects_Count; o ++)
    {
        condition = 0;

        for (o_s = 0; o_s < selected_object_count; o_s ++)
        {
            O = objects[selected_objects[o_s]];
            if (O == D->Objects[o])
            {
                condition = 1;
                break;
            }
        }

        if (condition)
        {
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
                    B->Distances[o][v] = distance_To_Point_On_Line(norm, pos, B->A->pos, B->B->pos, B->A);
                }
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
    B->IK_member = 0;
    B->IK = NULL;
    B->selected = 0;
    bonesIndex ++;
    return B->index;
}

void synthesize_Bone_Axis(bone * B, float scale[3])
{
    float mag;

//    memcpy(B->rotVec_, B->A->rotVec, sizeof(B->rotVec_));

    if (B->A->rot_Order == zxy || B->A->rot_Order == xzy)
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
        B->rotVec_[1][0] *= scale[1];
        B->rotVec_[1][1] *= scale[1];
        B->rotVec_[1][2] *= scale[1];
        memcpy(B->A->rotVec[1], B->rotVec_[1], sizeof(B->A->rotVec[1]));
        memcpy(B->B->rotVec[1], B->rotVec_[1], sizeof(B->B->rotVec[1]));
    }
    else if (B->A->rot_Order == yxz || B->A->rot_Order == xyz)
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
        B->rotVec_[2][0] *= scale[2];
        B->rotVec_[2][1] *= scale[2];
        B->rotVec_[2][2] *= scale[2];
        memcpy(B->A->rotVec[2], B->rotVec_[2], sizeof(B->A->rotVec[2]));
        memcpy(B->B->rotVec[2], B->rotVec_[2], sizeof(B->B->rotVec[2]));
    }
    else if (B->A->rot_Order == zyx || B->A->rot_Order == yzx)
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
        B->rotVec_[0][0] *= scale[0];
        B->rotVec_[0][1] *= scale[0];
        B->rotVec_[0][2] *= scale[0];
        memcpy(B->A->rotVec[0], B->rotVec_[0], sizeof(B->A->rotVec[0]));
        memcpy(B->B->rotVec[0], B->rotVec_[0], sizeof(B->B->rotVec[0]));
    }
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

        if (ALIGN_IS_ON && B->A->parent->parent != NULL)
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

        if (ALIGN_IS_ON && B->A->parent->parent != NULL)
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

        if (ALIGN_IS_ON && B->A->parent->parent != NULL)
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

        if (ALIGN_IS_ON && B->A->parent->parent != NULL)
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

        if (ALIGN_IS_ON && B->A->parent->parent != NULL)
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

        if (ALIGN_IS_ON && B->A->parent->parent != NULL)
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

void create_Bones_List(int BoneIndex)
{
    int b;
    bone * B;
    Bones_c = 0;

    for (b = 0; b < bonesIndex; b ++)
    {
        B = bones[b];

        memcpy(Bone_Names[Bones_c], B->Name, strlen(B->Name));
        Bone_Names[Bones_c][strlen(B->Name)] = '\0';
        Bone_Italic[Bones_c] = B->selected;
        Bone_List[Bones_c] = Bones_c;
        Bones_c ++;
        if (Bones_c >= BONES)
            break;
    }
}

void replace_Bone_Name(char * EditString)
{
    bone * B = bones[Bone_List[BoneIndex]];
    memcpy(B->Name, EditString, strlen(EditString));
    B->Name[strlen(EditString)] = '\0';
}

void set_Bone_H_Button(int index)
{
    int i;
    for (i = 0; i < H_BONE_NUM; i ++)
    {
        Button_h_bone[i].color = UI_GRAYB;
    }
    if (index > -1)
        Button_h_bone[index].color = UI_GRAYD;
}

void delete_Bone_Transformer(transformer * T);
void free_bone(bone * B);

void remove_Bone_From_Bones(bone * B)
{
    //printf("remove Bone From Bones\n");

    int index, b;
    int condition = 0;

    for (b = 0; b < bonesIndex; b ++)
    {
        if (B == bones[b])
        {
            index = b;
            condition = 1;
            break;
        }
    }

    if (condition)
    {
        B->A->Bone = NULL;
        B->B->Bone = NULL;

        delete_Bone_Transformer(B->A);
        delete_Bone_Transformer(B->B);

        free_bone(B);

        bonesIndex --;
        for (b = index; b < bonesIndex; b ++)
        {
            bones[b] = bones[b + 1];
            bones[b]->index = b;
        }
    }
}

void reset_Bones_Rotation()
{
    int b;
    bone * B;

    for (b = 0; b < bonesIndex; b ++)
    {
        B = bones[b];
        B->A->rot[0] = 0;
        B->A->rot[1] = 0;
        B->A->rot[2] = 0;
        B->B->rot[0] = 0;
        B->B->rot[1] = 0;
        B->B->rot[2] = 0;
    }
}

void set_Bone_len()
{
    int b;
    bone * B;
    float V[3];

    for (b = 0; b < bonesIndex; b ++)
    {
        B = bones[b];
        V[0] = B->B->pos[0] - B->A->pos[0];
        V[1] = B->B->pos[1] - B->A->pos[1];
        V[2] = B->B->pos[2] - B->A->pos[2];
        B->len = sqrt(V[0] * V[0] + V[1] * V[1] + V[2] * V[2]);
    }
}

#endif // BONES_H_INCLUDED
