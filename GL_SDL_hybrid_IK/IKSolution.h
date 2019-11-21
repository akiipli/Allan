/*
The MIT License

Copyright <2018> <Allan Kiipli>

Here we try to set IK solution for our designated bone
chains. These chains belong to a deformer. Bones in deformer
may be in the chains. They need to be continuous in the chains
and not overlap. This means one bone is most in one chain.
Bones get IK property. Begin and end bone may be moved or
displaced, in chain bones are calculated with solution.

When saving IK chains are written to a separate file and
record deformers address. Also they record bone address
for every bone in it. When loading they are used to associate
them with already loaded deformers and bones.

Solution must calculate transformers rotVec.
This is scaled matrix. First off it measures distance in
rest position in bind pose. When applying IK effect, in
solution current distance is measured and the new shape
is applied in rest pose. Then it is transfered to new
position and angle. Meanwhile an intermediate spine for the
chain is measured and final rotation happens for this spine.
For rotation to happen, cross product is taken and rotation
matrix is composed. Then all matrices in chain are multiplied
with IKchain reverse angled matrix and multiplied with new
matrix. Before pole rotation is applied to this matrix.
*/

#ifndef IKSOLUTION_H_INCLUDED
#define IKSOLUTION_H_INCLUDED

#define BONES 400
#define IKCHAINS 200

#define IK_MEMBER 3
#define IK_START 1
#define IK_END 2

#define STRLEN 256

#define bone_end 1
#define ik_start 2
#define ik_goal 3

int ALIGN_IS_ON = 1;

typedef struct
{
    float distance;
    float vec[3];
}
direction_Pack;

typedef struct
{
    float vec[3];
}
vec3;

struct ikChain
{
    int index;
    unsigned address; // assigned after loading
    char * Name;
    int selected;
    deformer * Deformer;
    int bonescount;
    bone ** Bones;
    float sum_length; // sum of bone lengths
    vec3 * vectors; // transition into stretched pose
    vec3 * vectors_bone; // in between bone vectors
    vec3 * positions_A; // A points in the chain
    vec3 * positions_B; // B points in the chain
    vec3 * bones_Rot; // Bones rotational axis
    transformer * A;
    transformer * B;
    float rotVec_0[3][3]; // intermediate matrix
    float rotVec_1[3][3]; // final pose matrix
    float poleRot;
    direction_Pack P;

    float rotVec_B[3][3]; // bind pose matrix
    float rotVec_F[3][3]; // final pose matrix
    float rotVec_I[3][3]; // bind pose inverse
}
;

int ikChains_c = 0;
ikChain * ikChains[IKCHAINS];
int iksIndex = 0;

int bones_Count;
bone * bone_Collection[BONES];
int ikValidates;

int ik_Collection_Count;
ikChain * ik_Chains_Collection[IKCHAINS];

void collect_Chain_Bones(bone * A, bone * B)
{
    printf("bone: %s\n", B->Name);
    if (B->parent == NULL)
    {
        return;
    }
    else if (B->parent->IK_member > 0)
    {
        return;
    }
    else if (B->parent == A)
    {
        printf("begin: %s\n", B->parent->Name);
        bone_Collection[bones_Count ++] = B->parent;

        ikValidates = 1;
        return;
    }
    else
    {
        bone_Collection[bones_Count ++] = B->parent;
        collect_Chain_Bones(A, B->parent);
    }
    return;
}

direction_Pack length_AB(float A[3], float B[3])
{
    float V[3];
    direction_Pack P;
    V[0] = B[0] - A[0];
    V[1] = B[1] - A[1];
    V[2] = B[2] - A[2];
    P.distance = sqrt(V[0] * V[0] + V[1] * V[1] + V[2] * V[2]);
    if (P.distance > 0)
    {
        P.vec[0] = V[0] / P.distance;
        P.vec[1] = V[1] / P.distance;
        P.vec[2] = V[2] / P.distance;
    }
    else
    {
        P.vec[0] = 1;
        P.vec[1] = 0;
        P.vec[2] = 0;
    }
    return P;
}

void cross_Product(float v1[3], float v2[3], float N[3])
{
    N[0] = v1[1] * v2[2] - v1[2] * v2[1];
    N[1] = v1[2] * v2[0] - v1[0] * v2[2];
    N[2] = v1[0] * v2[1] - v1[1] * v2[0];
}

float length_(float V[3])
{
    float l = sqrt(V[0] * V[0] + V[1] * V[1] + V[2] * V[2]);
    return l;
}

float boneLength(bone * B)
{
    float V[3];
    V[0] = B->B->pos[0] - B->A->pos[0];
    V[1] = B->B->pos[1] - B->A->pos[1];
    V[2] = B->B->pos[2] - B->A->pos[2];
    float l = sqrt(V[0] * V[0] + V[1] * V[1] + V[2] * V[2]);
    return l;
}

void add_IkChain_To_Deformer(ikChain * I, deformer * D);
void add_Transformer_To_Deformer(transformer * T, deformer * D);

int init_ikChain(deformer * Deformer)
{
    if (transformerIndex >= TRANSFORMERS - 1)
    {
        return 0;
    }
    ikChain * I = malloc(sizeof(ikChain));
    if (I == NULL)
        return 0;
    ikChains[iksIndex] = I;

    I->index = iksIndex;
    I->Name = malloc(STRLEN * sizeof(char));
    sprintf(I->Name, "%s %d", "ikChain", iksIndex);
    I->selected = 0;
    I->Deformer = NULL;
    if (Deformer != NULL)
    {
        add_IkChain_To_Deformer(I, Deformer);
    }
    I->bonescount = bones_Count;
    I->Bones = malloc(bones_Count * sizeof(bone *));
    int b;
    int c = 0;
    for (b = bones_Count - 1; b >= 0; b --)
    {
        I->Bones[c ++] = bone_Collection[b];
        bone_Collection[b]->IK_member = IK_MEMBER;
        bone_Collection[b]->IK = I;
    }
    I->Bones[0]->IK_member = IK_START;
    I->Bones[bones_Count - 1]->IK_member = IK_END;

    I->vectors = calloc(bones_Count, sizeof(vec3));
    I->vectors_bone = calloc(bones_Count, sizeof(vec3));
    I->positions_A = calloc(bones_Count, sizeof(vec3));
    I->positions_B = calloc(bones_Count, sizeof(vec3));
    I->bones_Rot = calloc(bones_Count, sizeof(vec3));

    // make new A and B transformers

    I->A = calloc(1, sizeof(transformer));

    Locators[locatorIndex ++] = I->A;

    init_transformer(I->A, I->Bones[0]->A->parent, "IK A");

    memcpy(I->A->pos, I->Bones[0]->A->pos, sizeof(I->A->pos));

    I->A->IK = I;
    I->A->style = ik_start;

    remove_Child(I->Bones[0]->A, I->Bones[0]->A->parent, I->A);

    I->B = calloc(1, sizeof(transformer));

    Locators[locatorIndex ++] = I->B;

    init_transformer(I->B, I->A, "IK B");

    bone * B = I->Bones[bones_Count - 1];

    memcpy(I->B->pos, B->B->pos, sizeof(I->B->pos));

    I->B->IK = I;
    I->B->style = ik_goal;

    transformer * C;

    for (c = B->B->childcount - 1; c >= 0; c --)
    {
        C = B->B->childs[c];
        remove_Child(C, B->B, I->B);
    }

    I->poleRot = 0;
    I->P = length_AB(I->A->pos, I->B->pos);

    if (I->Deformer != NULL)
    {
        add_Transformer_To_Deformer(I->A, I->Deformer);
        add_Transformer_To_Deformer(I->B, I->Deformer);
    }

    iksIndex ++;
    return 1;
}

int add_ikChain(deformer * Deformer, transformer * A, transformer * B)
{
    if (A->Bone != NULL && B->Bone != NULL)
    {
        if (A == A->Bone->A && B == B->Bone->B)
        {
            if (A->Bone->IK_member > 0 || B->Bone->IK_member > 0)
            {
                return 0;
            }
            bones_Count = 0;
            bone_Collection[bones_Count ++] = B->Bone;
            ikValidates = 0;
            collect_Chain_Bones(A->Bone, B->Bone);
            if (ikValidates)
            {
                if (init_ikChain(Deformer))
                {
                    return 1;
                }
                else
                {
                    return 0;
                }
            }
            else
            {
                return 0;
            }
        }
    }
    else
    {
        return 0;
    }
    return 0;
}

void make_Spine(float rotVec_[3][3], float P_vec[3])
{
    float angle;

    rotVec_[2][0] = P_vec[0];
    rotVec_[2][1] = P_vec[1];
    rotVec_[2][2] = P_vec[2];

    angle = atan2(rotVec_[2][1], rotVec_[2][0]);
    angle += pi_2;
    rotVec_[0][0] = cos(angle);
    rotVec_[0][1] = sin(angle);
    rotVec_[0][2] = 0;

    cross_Product(rotVec_[2], rotVec_[0], rotVec_[1]);
}

void update_IKchains()
{
    int i, b;
    ikChain * I;
    bone * B;
    float len, mag, mag1, angle;
    direction_Pack bonevec;

    mag = 0;

    for (i = 0; i < iksIndex; i ++)
    {
        I = ikChains[i];
        I->P = length_AB(I->A->pos, I->B->pos);
        I->sum_length = 0;
        for (b = 0; b < I->bonescount; b ++)
        {
            B = I->Bones[b];
            I->sum_length += B->len;
        }
        if (I->sum_length > 0)
        {
            len = 0;
            for (b = 0; b < I->bonescount; b ++)
            {
                B = I->Bones[b];
                len += B->len;
                I->vectors[b].vec[0] = (I->P.vec[0] * len + I->A->pos[0]) - B->B->pos[0];
                I->vectors[b].vec[1] = (I->P.vec[1] * len + I->A->pos[1]) - B->B->pos[1];
                I->vectors[b].vec[2] = (I->P.vec[2] * len + I->A->pos[2]) - B->B->pos[2];
            }

            // assume bone z axis be in direction of the bone
            // y axis the axis of rotation
            // x axis perpendicular to them
            for (b = 0; b < I->bonescount; b ++)
            {
                B = I->Bones[b];
                bonevec = length_AB(B->A->pos, B->B->pos);
                cross_Product(I->P.vec, bonevec.vec, I->bones_Rot[b].vec);
                mag1 = mag;
                mag = length_(I->bones_Rot[b].vec);
                if (mag > 0)
                {
                    I->bones_Rot[b].vec[0] /= mag;
                    I->bones_Rot[b].vec[1] /= mag;
                    I->bones_Rot[b].vec[2] /= mag;
                }
                else if (b > 1 && mag1 > 0)
                {
                    I->bones_Rot[b].vec[0] = I->bones_Rot[b - 1].vec[0];
                    I->bones_Rot[b].vec[1] = I->bones_Rot[b - 1].vec[1];
                    I->bones_Rot[b].vec[2] = I->bones_Rot[b - 1].vec[2];
                }
                else
                {
                    angle = atan2(bonevec.vec[1], bonevec.vec[0]);
                    angle += pi_2;
                    I->bones_Rot[b].vec[0] = cos(angle);
                    I->bones_Rot[b].vec[1] = sin(angle);
                    I->bones_Rot[b].vec[2] = 0;
                }

                // Update for deformations

                memcpy(I->Bones[b]->A->rotVec_[1], I->bones_Rot[b].vec, sizeof(float[3]));
                memcpy(I->Bones[b]->A->rotVec_[2], bonevec.vec, sizeof(float[3]));
                cross_Product(I->Bones[b]->A->rotVec_[1], I->Bones[b]->A->rotVec_[2], I->Bones[b]->A->rotVec_[0]);

                memcpy(I->Bones[b]->A->rotVec, I->Bones[b]->A->rotVec_, sizeof(float[3][3]));
                memcpy(I->Bones[b]->B->rotVec_, I->Bones[b]->A->rotVec_, sizeof(float[3][3]));
                memcpy(I->Bones[b]->B->rotVec, I->Bones[b]->A->rotVec_, sizeof(float[3][3]));

            }

            make_Spine(I->rotVec_B, I->P.vec);

            memcpy(I->A->rotVec, I->rotVec_B, sizeof(float[3][3]));
            memcpy(I->A->rotVec_, I->rotVec_B, sizeof(float[3][3]));

            memcpy(I->B->rotVec, I->rotVec_B, sizeof(float[3][3]));
            memcpy(I->B->rotVec_, I->rotVec_B, sizeof(float[3][3]));

            memcpy(I->A->rotVec_I, I->rotVec_B, sizeof(float[3][3]));
            memcpy(I->B->rotVec_I, I->rotVec_B, sizeof(float[3][3]));

            invert_Rotation_1(I->rotVec_I, I->rotVec_B);

            I->Bones[0]->A->collapsed = 1;

        }
    }
}

void solve_IK_Chain(ikChain * I, int update)
{
    /*
    i match intermediate spine with true length with adjust Proportional
    this causes bones be not proper length
    need some way to estimate transition amount to come closer to true length
    */

    int b;
    bone * B;

    float mag, angle;

    float Transition_Amount;
    float median_Point_Offset;
    float adjust_Proportional;

    direction_Pack P;
    P = length_AB(I->A->pos, I->B->pos);

    Transition_Amount = (P.distance - I->P.distance) / (I->sum_length - I->P.distance);

    median_Point_Offset = abs(I->sum_length / 2 - I->P.distance) / I->sum_length;

    if (Transition_Amount > 0)
    {
        Transition_Amount -= median_Point_Offset * Transition_Amount;
    }

    if (Transition_Amount > 1)
    {
        Transition_Amount = 1;
    }

    I->positions_A[0].vec[0] = I->A->pos_bind[0];
    I->positions_A[0].vec[1] = I->A->pos_bind[1];
    I->positions_A[0].vec[2] = I->A->pos_bind[2];

    for (b = 0; b < I->bonescount; b ++)
    {
        B = I->Bones[b];

        I->positions_B[b].vec[0] = I->vectors[b].vec[0] * Transition_Amount + B->B->pos_bind[0];
        I->positions_B[b].vec[1] = I->vectors[b].vec[1] * Transition_Amount + B->B->pos_bind[1];
        I->positions_B[b].vec[2] = I->vectors[b].vec[2] * Transition_Amount + B->B->pos_bind[2];

        if (b < I->bonescount - 1)
        {
            I->positions_A[b + 1].vec[0] = I->positions_B[b].vec[0];
            I->positions_A[b + 1].vec[1] = I->positions_B[b].vec[1];
            I->positions_A[b + 1].vec[2] = I->positions_B[b].vec[2];
        }
    }

//     the final segment is just drawn without check.
//
//     now construct new segments proper length
//     first take vectors in the segments

    for (b = 0; b < I->bonescount; b ++)
    {
        I->vectors_bone[b].vec[0] = I->positions_B[b].vec[0] - I->positions_A[b].vec[0];
        I->vectors_bone[b].vec[1] = I->positions_B[b].vec[1] - I->positions_A[b].vec[1];
        I->vectors_bone[b].vec[2] = I->positions_B[b].vec[2] - I->positions_A[b].vec[2];

        mag = length_(I->vectors_bone[b].vec);

        if (mag > 0)
        {
            I->vectors_bone[b].vec[0] /= mag;
            I->vectors_bone[b].vec[1] /= mag;
            I->vectors_bone[b].vec[2] /= mag;
        }
    }

    // proper length

    I->positions_A[0].vec[0] = 0;
    I->positions_A[0].vec[1] = 0;
    I->positions_A[0].vec[2] = 0;

    I->positions_B[0].vec[0] = I->vectors_bone[0].vec[0] * I->Bones[0]->len;
    I->positions_B[0].vec[1] = I->vectors_bone[0].vec[1] * I->Bones[0]->len;
    I->positions_B[0].vec[2] = I->vectors_bone[0].vec[2] * I->Bones[0]->len;

    for (b = 1; b < I->bonescount; b ++)
    {
        I->positions_B[b].vec[0] = I->positions_B[b - 1].vec[0] + I->vectors_bone[b].vec[0] * I->Bones[b]->len;
        I->positions_B[b].vec[1] = I->positions_B[b - 1].vec[1] + I->vectors_bone[b].vec[1] * I->Bones[b]->len;
        I->positions_B[b].vec[2] = I->positions_B[b - 1].vec[2] + I->vectors_bone[b].vec[2] * I->Bones[b]->len;

        if (b < I->bonescount - 1)
        {
            I->positions_A[b + 1].vec[0] = I->positions_B[b].vec[0];
            I->positions_A[b + 1].vec[1] = I->positions_B[b].vec[1];
            I->positions_A[b + 1].vec[2] = I->positions_B[b].vec[2];
        }
    }

    // make intermediate spine

    float len = length_(I->positions_B[I->bonescount - 1].vec);

    adjust_Proportional = 1;

    if (len > 0)
    {
        I->rotVec_0[2][0] = I->positions_B[I->bonescount - 1].vec[0] / len;
        I->rotVec_0[2][1] = I->positions_B[I->bonescount - 1].vec[1] / len;
        I->rotVec_0[2][2] = I->positions_B[I->bonescount - 1].vec[2] / len;

        adjust_Proportional = P.distance / len;
    }

    I->rotVec_1[2][0] = P.vec[0];
    I->rotVec_1[2][1] = P.vec[1];
    I->rotVec_1[2][2] = P.vec[2];

    float crossAxis[3];

    cross_Product(I->rotVec_0[2], P.vec, crossAxis);

    mag = length_(crossAxis);

    if (mag > 0)
    {
        crossAxis[0] /= mag;
        crossAxis[1] /= mag;
        crossAxis[2] /= mag;
    }
    else
    {
        angle = atan2(P.vec[1], P.vec[0]);
        angle += pi_2;
        crossAxis[0] = cos(angle);
        crossAxis[1] = sin(angle);
        crossAxis[2] = 0;
    }

    memcpy(I->rotVec_0[1], crossAxis, sizeof(float[3]));
    memcpy(I->rotVec_1[1], crossAxis, sizeof(float[3]));

    float rotVec_I[3][3];
    float rotVec_[3][3];
//    float rotVec_1[3][3];
    float result[3];
//    float result1[3];

    // rotate to final position and redraw last segment
    // make final aligned vectors for bones and
    // do per bone matrix multiplication using bone spines

    // insert parental angle

//    rotate_vector(I->A->parent->rotVec_I, I->rotVec_0[2], result);
//    rotate_vector(I->A->parent->rotVec_, result, result1);
//
//    cross_Product(result1, I->rotVec_1[2], result);
//
//    mag = length_(result);
//
//    if (mag > 0)
//    {
//        result[0] /= mag;
//        result[1] /= mag;
//        result[2] /= mag;
//    }
//    else
//    {
//        angle = atan2(I->rotVec_1[2][1], I->rotVec_1[2][0]);
//        angle += pi_2;
//        result[0] = cos(angle);
//        result[1] = sin(angle);
//        result[2] = 0;
//    }
//
//    memcpy(rotVec_1[2], result1, sizeof(float[3]));
//    memcpy(rotVec_1[1], result, sizeof(float[3]));
//
//    cross_Product(P.vec, result, rotVec_1[0]);
//
//    rotate_vector(I->A->parent->rotVec_I, I->rotVec_0[2], result);
//    rotate_vector(rotVec_1, result, I->rotVec_1[1]);

    // first construct missing axis for both matrices
    cross_Product(I->rotVec_0[1], I->rotVec_0[2], I->rotVec_0[0]);
    cross_Product(I->rotVec_1[1], I->rotVec_1[2], I->rotVec_1[0]);

    I->rotVec_1[2][0] *= adjust_Proportional;
    I->rotVec_1[2][1] *= adjust_Proportional;
    I->rotVec_1[2][2] *= adjust_Proportional;

    if (P.distance > I->P.distance)
    {
        float adjust_Proportional1 = 1 - (adjust_Proportional - 1) / 4;

        I->rotVec_1[1][0] *= adjust_Proportional1;
        I->rotVec_1[1][1] *= adjust_Proportional1;
        I->rotVec_1[1][2] *= adjust_Proportional1;

        I->rotVec_1[0][0] *= adjust_Proportional1;
        I->rotVec_1[0][1] *= adjust_Proportional1;
        I->rotVec_1[0][2] *= adjust_Proportional1;
    }

    // construct intermediate reverse

    invert_Rotation_1(rotVec_I, I->rotVec_0);
    rotate_matrix_I(rotVec_, I->rotVec_1, rotVec_I);

    I->positions_A[0].vec[0] = I->A->pos[0];
    I->positions_A[0].vec[1] = I->A->pos[1];
    I->positions_A[0].vec[2] = I->A->pos[2];

    for (b = 0; b < I->bonescount - 1; b ++)
    {
        rotate_vector(rotVec_, I->positions_B[b].vec, result);

        I->positions_A[b + 1].vec[0] = result[0] + I->A->pos[0];
        I->positions_A[b + 1].vec[1] = result[1] + I->A->pos[1];
        I->positions_A[b + 1].vec[2] = result[2] + I->A->pos[2];
    }

    for (b = 0; b < I->bonescount - 1; b ++)
    {
        I->positions_B[b].vec[0] = I->positions_A[b + 1].vec[0];
        I->positions_B[b].vec[1] = I->positions_A[b + 1].vec[1];
        I->positions_B[b].vec[2] = I->positions_A[b + 1].vec[2];
    }

    rotate_vector(rotVec_, I->positions_B[I->bonescount - 1].vec, result);

    I->positions_B[I->bonescount - 1].vec[0] = result[0] + I->A->pos[0];
    I->positions_B[I->bonescount - 1].vec[1] = result[1] + I->A->pos[1];
    I->positions_B[I->bonescount - 1].vec[2] = result[2] + I->A->pos[2];

    // now construct matrix for every bone in final form

    direction_Pack z_Axis;

    for (b = 0; b < I->bonescount; b ++)
    {
        z_Axis = length_AB(I->positions_A[b].vec, I->positions_B[b].vec);
        I->Bones[b]->A->rotVec_[2][0] = z_Axis.vec[0];
        I->Bones[b]->A->rotVec_[2][1] = z_Axis.vec[1];
        I->Bones[b]->A->rotVec_[2][2] = z_Axis.vec[2];

        // TO DO: need to rotate to parent orientation, also pole rotation

        // update bones rotvec with this rotation

        rotate_vector(rotVec_, I->bones_Rot[b].vec, result);

        I->Bones[b]->A->rotVec_[1][0] = result[0];
        I->Bones[b]->A->rotVec_[1][1] = result[1];
        I->Bones[b]->A->rotVec_[1][2] = result[2];

        // final axis

        cross_Product(I->Bones[b]->A->rotVec_[1], I->Bones[b]->A->rotVec_[2], I->Bones[b]->A->rotVec_[0]);

        // TO DO: scaling

        memcpy(I->Bones[b]->A->rotVec, I->Bones[b]->A->rotVec_, sizeof(float[3][3]));
        memcpy(I->Bones[b]->B->rotVec_, I->Bones[b]->A->rotVec_, sizeof(float[3][3]));
        memcpy(I->Bones[b]->B->rotVec, I->Bones[b]->A->rotVec_, sizeof(float[3][3]));
    }

    // transfer positions

    for (b = 0; b < I->bonescount; b ++)
    {
        memcpy(I->Bones[b]->A->pos, I->positions_A[b].vec, sizeof(float[3]));
        memcpy(I->Bones[b]->B->pos, I->positions_B[b].vec, sizeof(float[3]));
    }

    if (update)
    {
        make_Spine(I->rotVec_F, P.vec);

        memcpy(I->A->rotVec, I->rotVec_F, sizeof(float[3][3]));
        memcpy(I->A->rotVec_, I->rotVec_F, sizeof(float[3][3]));

        memcpy(I->B->rotVec, I->rotVec_F, sizeof(float[3][3]));
        memcpy(I->B->rotVec_, I->rotVec_F, sizeof(float[3][3]));
    }
}

#endif // IKSOLUTION_H_INCLUDED
