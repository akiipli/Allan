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

<2020>

Next task is to ensure correct behaving of the branches.
If IK chain has several branches, they need to start from
solved positions and rotate from solved angles. Also if they
contain successor IK chains, they need to solve in order.
To make that happen, ordering of the IK chains in deformer
must take place. This should happen after update_IKchains
function. This is called in exiting Bind mode. To order them,
hierarchy must be scanned and new chains need to be found,
to come up with ordered list. This list is then replacement
for not ordered list.
Also in program recursive move and rotate commands need to be
implanted into solution.
*/

#ifndef IKSOLUTION_H_INCLUDED
#define IKSOLUTION_H_INCLUDED

#define BONES 400
#define IKCHAINS 200
#define CONSTRAINTS 200

#define IK_MEMBER 3
#define IK_START 1
#define IK_END 2

//int relative = 0;
/*
When Deformer is created and when leaving Bind Pose
Default Pose for Deformer is created. Question is
what should pos variable in Transformer represent.
Should it be relative to Deformers center, or should
it be derotated in its parent space.
This variable is experimental and enables poses that
have translation in non linear Deformer mode.
It breaks however linear mode, since in linear mode
pos represents relative position to Deformer center.
When relative is 0, pos is independent and is
derotated in its parent space. rotate_R uses this
variable also. In Poses.h it is relative_pos.
Next Bind Pose must be examined. Default Pose is
set and updated there for Deformers. In Poses List
it is also used. Since the idea of Default Pose
is to capture positions as they are.
*/

int ALIGN_IS_ON = 1;

int ikChains_c = 0;
char IK_Names[IKCHAINS][STRLEN];
int IKIndex;
int ikch_start;

int IK_List[IKCHAINS];
int currentIK = 0;

ikChain * ikChains[IKCHAINS];
int iksIndex = 0;

int bones_Count;
bone * bone_Collection[BONES];
int ikValidates;

int ik_Collection_Count;
ikChain * ik_Chains_Collection[IKCHAINS];

constraint * constraints[CONSTRAINTS];
int constraintsIndex = 0;

void scan_For_Locator_Constraints(transformer * T)
{
    int c;

    transformer * C;

    if (T->Constraint != NULL && T == T->Constraint->Locator)
    {
        Constraint_Pack.IK = T->Constraint->IK_goal->IK;
        Constraint_Pack.Deformer = Constraint_Pack.IK->Deformer;
    }
    else
    {
        for (c = 0; c < T->childcount; c ++)
        {
            C = T->childs[c];
            scan_For_Locator_Constraints(C);
        }
    }
}

void free_Constraint(constraint * C)
{
    free(C->Name);
    free(C);
}

transformer * delete_Pole(ikChain * I)
{
    constraint * C;
    transformer * T;

    int c, index;

    int condition = 0;

    for (c = 0; c < constraintsIndex; c ++)
    {
        C = constraints[c];

        if (C == I->Pole)
        {
            condition = 1;
            index = c;
            break;
        }
    }

    if (condition)
    {
        constraintsIndex --;
        for (c = index; c < constraintsIndex; c ++)
        {
            constraints[c] = constraints[c + 1];
            constraints[c]->index = c;
        }
        I->Pole = NULL;
        C->IK_goal->Constraint = NULL;
        C->Locator->Constraint = NULL;
        T = C->Locator;
        free_Constraint(C);
        return T;
    }
    else
    {
        return NULL;
    }
}

transformer * delete_Constraint(ikChain * I)
{
    constraint * C;
    transformer * T;

    int c, index;

    int condition = 0;

    for (c = 0; c < constraintsIndex; c ++)
    {
        C = constraints[c];

        if (C == I->C)
        {
            condition = 1;
            index = c;
            break;
        }
    }

    if (condition)
    {
        constraintsIndex --;
        for (c = index; c < constraintsIndex; c ++)
        {
            constraints[c] = constraints[c + 1];
            constraints[c]->index = c;
        }
        I->C = NULL;
        C->IK_goal->Constraint = NULL;
        C->Locator->Constraint = NULL;
        T = C->Locator;
        free_Constraint(C);
        return T;
    }
    else
    {
        return NULL;
    }
}

void parent(transformer * T, transformer * P);

void init_IK_Pole(ikChain * I)
{
    if (transformerIndex < TRANSFORMERS)
    {
        constraint * C = malloc(sizeof(constraint));

        if (C != NULL && constraintsIndex < CONSTRAINTS)
        {
            C->index = constraintsIndex;
            constraints[constraintsIndex] = C;

            transformer * T = calloc(1, sizeof(transformer));

            if (T != NULL)
            {
                Locators[locatorIndex ++] = T;

                init_transformer(T, &World, "Pole");

                float pos[3];

                pos[0] = (I->A->pos[0] + I->B->pos[0]) / 2.0;
                pos[1] = (I->A->pos[1] + I->B->pos[1]) / 2.0;
                pos[2] = (I->A->pos[2] + I->B->pos[2]) / 2.0;

                memcpy(T->pos, pos, sizeof(T->pos));

                T->LocatorSize *= 2;

                C->Name = malloc(STRLEN * sizeof(char));
                sprintf(C->Name, "ikPole %d", constraintsIndex);
                C->constraint_type = constraint_pole;
                I->Pole = C;
                C->Locator = T;
                C->IK_goal = I->A;
                T->Constraint = C;
                I->A->Constraint = C;
                if (I->B->Constraint != NULL)
                {
                    parent(T, I->B->Constraint->Locator);
                }
                constraintsIndex ++;
            }
            else
            {
                free(C);
            }
        }
    }
}

void init_IK_Constraint(ikChain * I)
{
    if (transformerIndex < TRANSFORMERS)
    {
        constraint * C = malloc(sizeof(constraint));

        if (C != NULL && constraintsIndex < CONSTRAINTS)
        {
            C->index = constraintsIndex;
            constraints[constraintsIndex] = C;

            transformer * T = calloc(1, sizeof(transformer));

            if (T != NULL)
            {
                Locators[locatorIndex ++] = T;

                init_transformer(T, &World, "Constraint");

                memcpy(T->pos, I->B->pos, sizeof(T->pos));

                T->LocatorSize *= 2;

                C->Name = malloc(STRLEN * sizeof(char));
                sprintf(C->Name, "ikConstraint %d", constraintsIndex);
                C->constraint_type = constraint_to_point;
                I->C = C;
                C->Locator = T;
                C->IK_goal = I->B;
                T->Constraint = C;
                I->B->Constraint = C;
                I->stretch = 0;
                constraintsIndex ++;
            }
            else
            {
                free(C);
            }
        }
    }
}

void collect_Chain_Bones(bone * A, bone * B)
{
    printf("bone: %s\n", B->Name);
    if (A == B)
    {
        ikValidates = 1;
        return;
    }
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

direction_Pack length_AB_(float x0, float y0, float z0, float x1, float y1, float z1)
{
    float V[3];
    direction_Pack P;
    V[0] = x1 - x0;
    V[1] = y1 - y0;
    V[2] = z1 - z0;
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

    I->update = 0;
    I->stretch = 1;
    I->C = NULL;
    I->Pole = NULL;

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

void make_preferred_Axis()
{

}

void closest_Point_On_Vec(float polepos[3], float pos_A[3], float P_vec[3], float Pos[3])
{
    float vec[3];
    float len;

    vec[0] = polepos[0] - pos_A[0];
    vec[1] = polepos[1] - pos_A[1];
    vec[2] = polepos[2] - pos_A[2];

    len = length_(vec);

    if (len > 0)
    {
        vec[0] /= len;
        vec[1] /= len;
        vec[2] /= len;
    }

    float dot = dot_productFF(vec, P_vec);

    if (dot == 0)
    {
        Pos[0] = pos_A[0];
        Pos[1] = pos_A[1];
        Pos[2] = pos_A[2];
    }
    else
    {
        Pos[0] = len * dot * P_vec[0] + pos_A[0];
        Pos[1] = len * dot * P_vec[1] + pos_A[1];
        Pos[2] = len * dot * P_vec[2] + pos_A[2];
    }
}

void make_Spine(ikChain * I, float rotVec_[3][3], float P_vec[3], float rotVec_P[3][3], int order, int bind_pose)
{
    float x_axis[3];
    float y_axis[3];
    float rotVec_p[3][3];

    //memcpy(rotVec_p, (float[3][3]) {{1.0, 0.0, 0.0}, {0.0, 1.0, 0.0}, {0.0, 0.0, 1.0}}, sizeof rotVec_p);

    if (I->Pole != NULL)
    {
        float Pos[3], vec_[3];

        if (bind_pose)
        {
            closest_Point_On_Vec(I->Pole->Locator->pos_bind, I->A->pos_bind, P_vec, Pos);
            vec_[0] = I->Pole->Locator->pos_bind[0] - Pos[0];
            vec_[1] = I->Pole->Locator->pos_bind[1] - Pos[1];
            vec_[2] = I->Pole->Locator->pos_bind[2] - Pos[2];
        }
        else
        {
            closest_Point_On_Vec(I->Pole->Locator->pos, I->A->pos, P_vec, Pos);
            vec_[0] = I->Pole->Locator->pos[0] - Pos[0];
            vec_[1] = I->Pole->Locator->pos[1] - Pos[1];
            vec_[2] = I->Pole->Locator->pos[2] - Pos[2];
        }

        float d = sqrt(vec_[0] * vec_[0] + vec_[1] * vec_[1] + vec_[2] * vec_[2]);

        if (d > 0)
        {
            vec_[0] /= d;
            vec_[1] /= d;
            vec_[2] /= d;
        }
        else
        {
            vec_[0] = 0;
            vec_[1] = 1.0;
            vec_[2] = 0;
        }

        rotVec_p[1][0] = vec_[0];
        rotVec_p[1][1] = vec_[1];
        rotVec_p[1][2] = vec_[2];

        cross_Product(P_vec, rotVec_p[1], rotVec_p[0]);
    }
    else
    {
        /* simulating order yxz
        where z is aim */

        if (order == zxy)
        {
            memcpy(rotVec_p[2], rotVec_P[1], sizeof(float[3][3]));
            memcpy(rotVec_p[0], rotVec_P[0], sizeof(float[3][3]));
            memcpy(rotVec_p[1], rotVec_P[2], sizeof(float[3][3]));
        }
        else if (order == yxz)
        {
            memcpy(rotVec_p[1], rotVec_P[1], sizeof(float[3][3]));
            memcpy(rotVec_p[0], rotVec_P[0], sizeof(float[3][3]));
            memcpy(rotVec_p[2], rotVec_P[2], sizeof(float[3][3]));
        }
        else if (order == zyx)
        {
            memcpy(rotVec_p[2], rotVec_P[1], sizeof(float[3][3]));
            memcpy(rotVec_p[1], rotVec_P[0], sizeof(float[3][3]));
            memcpy(rotVec_p[0], rotVec_P[2], sizeof(float[3][3]));
        }
        else if (order == xyz)
        {
            memcpy(rotVec_p[0], rotVec_P[1], sizeof(float[3][3]));
            memcpy(rotVec_p[1], rotVec_P[0], sizeof(float[3][3]));
            memcpy(rotVec_p[2], rotVec_P[2], sizeof(float[3][3]));
        }
        else if (order == xzy)
        {
            memcpy(rotVec_p[0], rotVec_P[1], sizeof(float[3][3]));
            memcpy(rotVec_p[2], rotVec_P[0], sizeof(float[3][3]));
            memcpy(rotVec_p[1], rotVec_P[2], sizeof(float[3][3]));
        }
        else if (order == yzx)
        {
            memcpy(rotVec_p[1], rotVec_P[1], sizeof(float[3][3]));
            memcpy(rotVec_p[2], rotVec_P[0], sizeof(float[3][3]));
            memcpy(rotVec_p[0], rotVec_P[2], sizeof(float[3][3]));
        }
    }

    float Dot_0 = dot_productFF(P_vec, rotVec_p[0]);
    float Dot_1 = dot_productFF(P_vec, rotVec_p[1]);

    if (abs(Dot_0) > abs(Dot_1))
    {
        cross_Product(rotVec_p[0], P_vec, y_axis);

        float d = sqrt(y_axis[0] * y_axis[0] + y_axis[1] * y_axis[1] + y_axis[2] * y_axis[2]);

        if (d > 0)
        {
            y_axis[0] /= d;
            y_axis[1] /= d;
            y_axis[2] /= d;
        }
        else
        {
            y_axis[0] = rotVec_p[1][0];
            y_axis[1] = rotVec_p[1][1];
            y_axis[2] = rotVec_p[1][2];
        }

        rotVec_[1][0] = y_axis[0];
        rotVec_[1][1] = y_axis[1];
        rotVec_[1][2] = y_axis[2];

        rotVec_[2][0] = P_vec[0];
        rotVec_[2][1] = P_vec[1];
        rotVec_[2][2] = P_vec[2];

        cross_Product(rotVec_[2], rotVec_[1], rotVec_[0]);
    }
    else
    {
        cross_Product(rotVec_p[1], P_vec, x_axis);

        float d = sqrt(x_axis[0] * x_axis[0] + x_axis[1] * x_axis[1] + x_axis[2] * x_axis[2]);

        if (d > 0)
        {
            x_axis[0] /= d;
            x_axis[1] /= d;
            x_axis[2] /= d;
        }
        else
        {
            x_axis[0] = rotVec_p[0][0];
            x_axis[1] = rotVec_p[0][1];
            x_axis[2] = rotVec_p[0][2];
        }

        rotVec_[0][0] = x_axis[0];
        rotVec_[0][1] = x_axis[1];
        rotVec_[0][2] = x_axis[2];

        rotVec_[2][0] = P_vec[0];
        rotVec_[2][1] = P_vec[1];
        rotVec_[2][2] = P_vec[2];

        cross_Product(rotVec_[2], rotVec_[0], rotVec_[1]);
    }

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

            make_Spine(I, I->rotVec_B, I->P.vec, I->A->parent->rotVec_, I->A->parent->rot_Order, 0);

            memcpy(I->A->rotVec, I->rotVec_B, sizeof(float[3][3]));
            memcpy(I->A->rotVec_, I->rotVec_B, sizeof(float[3][3]));

            memcpy(I->B->rotVec, I->rotVec_B, sizeof(float[3][3]));
            memcpy(I->B->rotVec_, I->rotVec_B, sizeof(float[3][3]));

            memcpy(I->A->rotVec_I, I->rotVec_B, sizeof(float[3][3]));
            memcpy(I->B->rotVec_I, I->rotVec_B, sizeof(float[3][3]));

            invert_Rotation_1(I->rotVec_I, I->rotVec_B);

            int collapse = 1;

            for (b = 0; b < I->bonescount - 1; b ++)
            {
                B = I->Bones[b];
                if (B->B->childcount > 1)
                {
                    collapse = 0;
                    break;
                }
            }

            I->Bones[0]->A->collapsed = collapse;

        }
    }
}

void update_Spine(ikChain * I)
{
    direction_Pack P;
    P = length_AB(I->A->pos, I->B->pos);
    make_Spine(I, I->rotVec_F, P.vec, I->A->parent->rotVec_, I->A->parent->rot_Order, 0);

    memcpy(I->A->rotVec, I->rotVec_F, sizeof(float[3][3]));
    memcpy(I->A->rotVec_, I->rotVec_F, sizeof(float[3][3]));

    memcpy(I->B->rotVec, I->rotVec_F, sizeof(float[3][3]));
    memcpy(I->B->rotVec_, I->rotVec_F, sizeof(float[3][3]));
}

void rotate_B(transformer * T, float rotVec[3][3], float pos[3], float pos_bind[3])
{
    int c;
    transformer * C;

    if (T->Bone != NULL && T->Bone->IK_member > 0)
    {

    }
    else if (T->IK != NULL && T->style == ik_start)
    {
        T->pos[0] = T->parent->pos[0];
        T->pos[1] = T->parent->pos[1];
        T->pos[2] = T->parent->pos[2];
    }
    else if (T->IK != NULL)// && (T->style == ik_goal || T->style == ik_start || T->style == ik_fixed))
    {

    }
    else if (T->parent->IK != NULL && T->parent->style == ik_goal)
    {
        T->pos[0] = T->parent->pos[0];
        T->pos[1] = T->parent->pos[1];
        T->pos[2] = T->parent->pos[2];
    }
    else
    {
        float X, Y, Z;

        X = T->pos_bind[0] - pos_bind[0];
        Y = T->pos_bind[1] - pos_bind[1];
        Z = T->pos_bind[2] - pos_bind[2];

        T->pos[0] = rotVec[0][0] * X + rotVec[1][0] * Y + rotVec[2][0] * Z + pos[0];
        T->pos[1] = rotVec[0][1] * X + rotVec[1][1] * Y + rotVec[2][1] * Z + pos[1];
        T->pos[2] = rotVec[0][2] * X + rotVec[1][2] * Y + rotVec[2][2] * Z + pos[2];
    }

    float rotVec_[3][3];
    rotate_matrix_I(rotVec_, T->rotVec, T->rotVec_B);

    for (c = 0; c < T->childcount; c ++)
    {
        C = T->childs[c];
        if (C->IK != NULL && C->style == ik_fixed)
        {

        }
        else
        {
            rotate_B(C, rotVec_, T->pos, T->pos_bind);
        }
    }
}

void rotate_rotVec_pose_B(transformer * T)
{
    int c;
    transformer * C;

    float rotVec_[3][3];
    rotate_matrix_I(rotVec_, T->rotVec, T->rotVec_B);

    for (c = 0; c < T->childcount; c ++)
    {
        C = T->childs[c];
        rotate_B(C, rotVec_, T->pos, T->pos_bind);
    }
}

float distance(float A[3], float B[3]);

void compose_Hierarchy_S(transformer * T)
{
    int t;
    transformer * C;
    float len;

    for (t = 0; t < T->childcount; t ++)
    {
        C = T->childs[t];
        len = distance(T->pos_bind, C->pos_bind);

        if (C->IK != NULL && C->style != ik_start)
        {
            C->pos[0] = C->IK->Bones[C->IK->bonescount - 1]->B->pos[0];
            C->pos[1] = C->IK->Bones[C->IK->bonescount - 1]->B->pos[1];
            C->pos[2] = C->IK->Bones[C->IK->bonescount - 1]->B->pos[2];
        }
        else if (len > 0)
        {
            if (T->rot_Order == yxz || T->rot_Order == xyz)
            {
                C->pos[0] = T->pos[0] + T->rotVec[2][0] * len;
                C->pos[1] = T->pos[1] + T->rotVec[2][1] * len;
                C->pos[2] = T->pos[2] + T->rotVec[2][2] * len;
            }
            else if (T->rot_Order == zxy || T->rot_Order == xzy)
            {
                C->pos[0] = T->pos[0] + T->rotVec[1][0] * len;
                C->pos[1] = T->pos[1] + T->rotVec[1][1] * len;
                C->pos[2] = T->pos[2] + T->rotVec[1][2] * len;
            }
            else if (T->rot_Order == zyx || T->rot_Order == yzx)
            {
                C->pos[0] = T->pos[0] + T->rotVec[0][0] * len;
                C->pos[1] = T->pos[1] + T->rotVec[0][1] * len;
                C->pos[2] = T->pos[2] + T->rotVec[0][2] * len;
            }
        }
        else
        {
            C->pos[0] = T->pos[0];
            C->pos[1] = T->pos[1];
            C->pos[2] = T->pos[2];
        }

        compose_Hierarchy_S(C);
    }
}

void compose_Hierarchy(transformer * T)
{
    int t;
    transformer * C;
    float len;

    for (t = 0; t < T->childcount; t ++)
    {
        C = T->childs[t];
        len = distance(T->pos_bind, C->pos_bind);

        if (C->Bone != NULL && C->Bone->IK_member > 0)
        {

        }
        else if (C->IK != NULL && C->style == ik_start)
        {
            C->pos[0] = T->pos[0];
            C->pos[1] = T->pos[1];
            C->pos[2] = T->pos[2];
        }
        else if (C->IK != NULL)
        {

        }
        else if (len > 0)
        {
            if (T->rot_Order == yxz || T->rot_Order == xyz)
            {
                C->pos[0] = T->pos[0] + T->rotVec[2][0] * len;
                C->pos[1] = T->pos[1] + T->rotVec[2][1] * len;
                C->pos[2] = T->pos[2] + T->rotVec[2][2] * len;
            }
            else if (T->rot_Order == zxy || T->rot_Order == xzy)
            {
                C->pos[0] = T->pos[0] + T->rotVec[1][0] * len;
                C->pos[1] = T->pos[1] + T->rotVec[1][1] * len;
                C->pos[2] = T->pos[2] + T->rotVec[1][2] * len;
            }
            else if (T->rot_Order == zyx || T->rot_Order == yzx)
            {
                C->pos[0] = T->pos[0] + T->rotVec[0][0] * len;
                C->pos[1] = T->pos[1] + T->rotVec[0][1] * len;
                C->pos[2] = T->pos[2] + T->rotVec[0][2] * len;
            }
        }
        else
        {
            C->pos[0] = T->pos[0];
            C->pos[1] = T->pos[1];
            C->pos[2] = T->pos[2];
        }

        if (C->IK != NULL && C->style != ik_start)
        {

        }
        else
        {
            compose_Hierarchy(C);
        }
    }
}

void solve_IK_Chain(ikChain * I)
{
    /*
    i match intermediate spine with true length with adjust Proportional
    this causes bones be not proper length
    need some way to estimate transition amount to come closer to true length
    */

    int b;
    bone * B;

    float mag;

    float Transition_Amount;
    float median_Point_Offset;
    float adjust_Proportional;

    direction_Pack P;
    P = length_AB(I->A->pos, I->B->pos);

    if (I->bonescount == 1)
    {
        Transition_Amount = 1;
    }
    else
    {
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

    /*
    start of spine length adjustment
    */

    float len = length_(I->positions_B[I->bonescount - 1].vec);

    if (Transition_Amount < 0 && P.distance > 0)
    {
        adjust_Proportional = len / P.distance;
        Transition_Amount *= adjust_Proportional;

        if (Transition_Amount < -20)
        {
            Transition_Amount = -20;
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
    }

    /*
    end of spine length adjustment
    */

    // make intermediate spine

    len = length_(I->positions_B[I->bonescount - 1].vec);

    adjust_Proportional = 1;

    if (len > 0)
    {
        I->rotVec_0[2][0] = I->positions_B[I->bonescount - 1].vec[0] / len;
        I->rotVec_0[2][1] = I->positions_B[I->bonescount - 1].vec[1] / len;
        I->rotVec_0[2][2] = I->positions_B[I->bonescount - 1].vec[2] / len;

        adjust_Proportional = P.distance / len;
    }

    float rotVec_I[3][3];
    float rotVec_[3][3];
    float result[3];

    invert_Rotation_1(rotVec_I, I->A->parent->rotVec_B);

    make_Spine(I, I->rotVec_0, I->rotVec_0[2], rotVec_I, I->A->parent->rot_Order, 1);
    make_Spine(I, I->rotVec_1, P.vec, I->A->parent->rotVec_, I->A->parent->rot_Order, 0);

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

    int c;
    transformer * T;
    transformer * C;
    float Delta[3];

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

//        T = I->Bones[b]->A;

        memcpy(I->Bones[b]->A->rotVec, I->Bones[b]->A->rotVec_, sizeof(float[3][3]));

/*      // normalize x and y axis

        normalizeF(I->Bones[b]->A->rotVec_[0]);
        normalizeF(I->Bones[b]->A->rotVec_[1]);

        T->scl_vec[0] = I->A->scl_vec[0];
        T->scl_vec[1] = I->A->scl_vec[1];
        T->scl_vec[2] = I->A->scl_vec[2];
*/

//
//        if (T->scl_vec[0] != 1.0)
//            scale_x(T);
//        if (T->scl_vec[1] != 1.0)
//            scale_y(T);
//        if (T->scl_vec[2] != 1.0)
//            scale_z(T);

        memcpy(I->Bones[b]->B->rotVec_, I->Bones[b]->A->rotVec_, sizeof(float[3][3]));
        memcpy(I->Bones[b]->B->rotVec, I->Bones[b]->A->rotVec_, sizeof(float[3][3]));

    }

    // transfer positions

    for (b = 0; b < I->bonescount; b ++)
    {
        memcpy(I->Bones[b]->A->pos, I->positions_A[b].vec, sizeof(float[3]));
        memcpy(I->Bones[b]->B->pos, I->positions_B[b].vec, sizeof(float[3]));
    }

    /*
    child movement
    */

    for (b = 0; b < I->bonescount - 1; b ++)
    {
        T = I->Bones[b]->B;

        for (c = 0; c < T->childcount; c ++)
        {
            C = T->childs[c];

            if (C == I->Bones[b + 1]->A)
            {

            }
            else
            {
                rotate_rotVec_pose_B(C);
                Delta[0] = T->pos[0] - C->pos[0];
                Delta[1] = T->pos[1] - C->pos[1];
                Delta[2] = T->pos[2] - C->pos[2];
                move_C_IK(C, Delta);
            }
        }
    }

    /*
    child movement section end
    */

    if (I->update)
    {
        make_Spine(I, I->rotVec_F, P.vec, I->A->parent->rotVec_, I->A->parent->rot_Order, 0);

        memcpy(I->A->rotVec, I->rotVec_F, sizeof(float[3][3]));
        memcpy(I->A->rotVec_, I->rotVec_F, sizeof(float[3][3]));

        T = I->A;

        if (T->scl_vec[0] != 1.0)
            scale_x(T);
        if (T->scl_vec[1] != 1.0)
            scale_y(T);
        if (T->scl_vec[2] != 1.0)
            scale_z(T);

        memcpy(I->B->rotVec, I->A->rotVec, sizeof(float[3][3]));
        memcpy(I->B->rotVec_, I->A->rotVec_, sizeof(float[3][3]));

        T = I->B;

        rotate_M(T);

        compose_Hierarchy(T);
    }
}

void create_Ik_List(int IKIndex)
{
    int i;
    ikChain * I;
    ikChains_c = 0;

    for (i = 0; i < iksIndex; i ++)
    {
        I = ikChains[i];

        memcpy(IK_Names[ikChains_c], I->Name, strlen(I->Name));
        IK_Names[ikChains_c][strlen(I->Name)] = '\0';
        IK_List[ikChains_c] = ikChains_c;
        ikChains_c ++;
        if (ikChains_c >= IKCHAINS)
            break;
    }
}

void replace_IK_Name(char * EditString)
{
    ikChain * I = ikChains[IK_List[IKIndex]];
    memcpy(I->Name, EditString, strlen(EditString));
    I->Name[strlen(EditString)] = '\0';
}

void set_IK_H_Button(int index)
{
    int i;
    for (i = 0; i < H_IKCH_NUM; i ++)
    {
        Button_h_ikch[i].color = UI_GRAYB;
    }
    if (index > -1)
        Button_h_ikch[index].color = UI_GRAYD;
}

void fix_ik_goal(transformer * T)
{
//    solve_IK_Chain(T->IK, 0);
//    normalize_rotation_unparent(T);
}

void normalize_IK_Spine(ikChain * I);

void unfix_ik_goal(transformer * T)
{
//    T->scl[0] *= (1 / T->parent->scl_vec[0]);
//    T->scl[1] *= (1 / T->parent->scl_vec[1]);
//    T->scl[2] *= (1 / T->parent->scl_vec[2]);
//    normalize_rotation_parent(T);
//    scale_axis(T);
    T->IK->update = 1;
    solve_IK_Chain(T->IK);
    normalize_IK_Spine(T->IK);
}

void unfix_ik_goals()
{
    int i;
    ikChain * I;

    for (i = 0; i < iksIndex; i ++)
    {
        I = ikChains[i];
        if (I->B->style == ik_fixed)
        {
            I->B->style = ik_goal;
            unfix_ik_goal(I->B);
        }
    }
}

void rotate_R(transformer * T, float rotVec[3][3], float pos[3], float pos_bind[3], int linear_pose)
{
    int c;
    transformer * C;
//    ikChain * I;

    if (T->Bone != NULL && T->Bone->IK_member > 0)
    {

    }
    else if (T->IK != NULL && T->style == ik_start)
    {

    }
    else if (T->IK != NULL) /* T is goal or fixed */
    {
        //printf("rotate_R %s\n", T->Name);
//        I = T->IK;
//        T->pos[0] = I->Bones[I->bonescount - 1]->B->pos[0];
//        T->pos[1] = I->Bones[I->bonescount - 1]->B->pos[1];
//        T->pos[2] = I->Bones[I->bonescount - 1]->B->pos[2];
    }
    else if (T->parent->IK != NULL && T->parent->style == ik_goal)
    {
//        T->pos[0] = T->parent->pos[0];
//        T->pos[1] = T->parent->pos[1];
//        T->pos[2] = T->parent->pos[2];
    }
    else
    {
        float X, Y, Z;

        if (linear_pose)
        {
            X = T->pos_bind[0] - pos_bind[0];
            Y = T->pos_bind[1] - pos_bind[1];
            Z = T->pos_bind[2] - pos_bind[2];
        }
        else
        {
            X = T->pos[0];
            Y = T->pos[1];
            Z = T->pos[2];
        }

        T->pos[0] = rotVec[0][0] * X + rotVec[1][0] * Y + rotVec[2][0] * Z + pos[0];
        T->pos[1] = rotVec[0][1] * X + rotVec[1][1] * Y + rotVec[2][1] * Z + pos[1];
        T->pos[2] = rotVec[0][2] * X + rotVec[1][2] * Y + rotVec[2][2] * Z + pos[2];
    }

    rotate_T(T);

    float rotVec_[3][3];

    if (linear_pose)
        rotate_matrix_I(rotVec_, T->rotVec, T->rotVec_B);
    else
        memcpy(rotVec_, T->rotVec, sizeof(rotVec_));

    for (c = 0; c < T->childcount; c ++)
    {
        C = T->childs[c];
        rotate_R(C, rotVec_, T->pos, T->pos_bind, linear_pose);
    }
}

void rotate_rotVec_pose(transformer * T, int linear_pose)
{
    int c;
    transformer * C;

    rotate_T(T);

    float rotVec_[3][3];

    if (linear_pose)
        rotate_matrix_I(rotVec_, T->rotVec, T->rotVec_B);
    else
        memcpy(rotVec_, T->rotVec, sizeof(rotVec_));

    for (c = 0; c < T->childcount; c ++)
    {
        C = T->childs[c];
        rotate_R(C, rotVec_, T->pos, T->pos_bind, linear_pose);
    }
}

void move_R(transformer * T, float Delta[3])
{
    int c;
    transformer * C;

    float Delta1[3];

    Delta1[0] = Delta[0];
    Delta1[1] = Delta[1];
    Delta1[2] = Delta[2];

    if ((T->IK != NULL && T->style == ik_start) || T->parent->IK != NULL)
    {
        Delta1[0] = T->parent->pos[0] - T->pos[0];
        Delta1[1] = T->parent->pos[1] - T->pos[1];
        Delta1[2] = T->parent->pos[2] - T->pos[2];
    }

    if (T->IK != NULL && T == T->IK->B)
    {
        //printf("move_R %s\n", T->Name);
    }
    else
    {
        T->pos[0] += Delta1[0];
        T->pos[1] += Delta1[1];
        T->pos[2] += Delta1[2];
    }

    for (c = 0; c < T->childcount; c ++)
    {
        C = T->childs[c];
        move_R(C, Delta1);
    }
}

void move_IKs_To_Parent(transformer * T)
{
    int c;
    transformer * C;

    float Delta[3];

    Delta[0] = 0;
    Delta[1] = 0;
    Delta[2] = 0;

    for (c = 0; c < T->childcount; c ++)
    {
        C = T->childs[c];
        move_R(C, Delta);
    }
}

void populate_IK_Update(deformer * D, ikChain * I, transformer * T)
{
    int c;

    transformer * C;

    if (T->Bone != NULL && T->Bone->IK_member > 0)
    {

    }
    else if (T->childcount == 0)
    {
        if (I != NULL)
            I->update = 1;
        I = NULL;
    }

    for (c = 0; c < T->childcount; c ++)
    {
        C = T->childs[c];

        if (C->Bone != NULL && C->Bone->IK_member > 0)
        {
            I = C->Bone->IK;
            I->update = 0;
        }

        populate_IK_Update(D, I, C);
    }
}

void project_IK_goal_To_Spine(ikChain * I)
{
    /* Pos_ is prepared in create_Action_Begin_Pose */

    direction_Pack P;

    if (I->C != NULL)
    {
        P = length_AB(I->A->pos, I->C->Locator->pos);
    }
    else
    {
        P = length_AB(I->A->pos, I->B->Pos_);
    }

    float sum_length = I->sum_length * 1.1;

    if (P.distance > sum_length)
    {
        I->B->pos[0] = I->A->pos[0] + P.vec[0] * sum_length;
        I->B->pos[1] = I->A->pos[1] + P.vec[1] * sum_length;
        I->B->pos[2] = I->A->pos[2] + P.vec[2] * sum_length;
    }
    else
    {
        if (I->C != NULL)
        {
            I->B->pos[0] = I->C->Locator->pos[0];
            I->B->pos[1] = I->C->Locator->pos[1];
            I->B->pos[2] = I->C->Locator->pos[2];
        }
        else
        {
            I->B->pos[0] = I->B->Pos_[0];
            I->B->pos[1] = I->B->Pos_[1];
            I->B->pos[2] = I->B->Pos_[2];
        }
    }
}

#endif // IKSOLUTION_H_INCLUDED
