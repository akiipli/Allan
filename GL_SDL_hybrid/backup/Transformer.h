#define TRANSFORMERS 100
#define LOCATORS 100
float MIN_SCALE = 1 / 1000000;
int SCALE = 0;

float LocatorSize = 1;
int locatorIndex = 0;
int transformerIndex = 0;
int transformerCount = 0;

float zero_vec[] = {0.0, 0.0, 0.0};
float Identity_[3][3] = {{1.0, 0.0, 0.0}, {0.0, 1.0, 0.0}, {0.0, 0.0, 1.0}};

float pi = 3.141592653589793;
float pi2 = 3.141592653589793 * 2.0;
float pi_2 = 3.141592653589793 / 2.0;
float pi_3 = (3.141592653589793 / 2.0) * 3.0;
float pi30 = 3.141592653589793 / 30.0;
float pi60 = 3.141592653589793 / 60.0;
float pi10 = 3.141592653589793 / 10.0;
float pi5 = 3.141592653589793 / 5.0;
float pi4 = 3.141592653589793 / 4.0;

// to do
// hierarchies, parenting

float dot_productFF(float[3], float[3]);

#define INFLUENCES 1

#define zxy 0
#define yxz 1
#define zyx 2
#define xyz 3
#define xzy 4
#define yzx 5

#define aim_vector 2
#define world 0

typedef struct transformer transformer;
typedef struct object object;
typedef struct deformer deformer;
typedef struct bone bone;
typedef struct bone
{
    int index;
    char * Name;
    int selected;
    bone * parent;
    float rotVec_[3][3];
    float rotVec_I[3][3];
    transformer * A;
    transformer * B;
    deformer * D;
    float ** Distances;
    int Distances_Count;
}
bone;

typedef struct
{
    int index;
    object * Object;
    char * Name;
    int * indices;
    int indices_count;
}
selection;

typedef struct
{
    int index;
    object * Object;
    transformer * Transformer;
    char * Name;
    int * indices;
    float * weights;
    int indices_count;
}
vert_selection;

typedef struct
{
    float x;
    float y;
    float z;
}
direction;

struct transformer
{
    int index;
    char * Name;
    int selected;
    int style;
    transformer * parent;
    transformer ** childs;
    int childcount;
    int collapsed; // UI
    int rot_Order;
    int bind_set;
    vert_selection ** Selections;
    int Selections_Count;
    object * Object;
    deformer * Deformer;
    bone * Bone;
    float scl[3];
    float scl_vec[3];
    float rot[3];
    float rot_bind[3];
    float rot_deform[3];
    float rotVec[3][3];
    float rotVec_[3][3];
    float rotVec_I[3][3];
    float rotVec_B[3][3];
    float pos[3];
    float pos_[3];
    float Pos_[3];
    float pos_bind[3];
    float pos_deform[3];
    float target[3];
    float * aim;  //rotVec_[aim_vector]
};

transformer * child_collection[TRANSFORMERS];
int child_collection_count;

transformer * transformers[TRANSFORMERS];
transformer * Locators[LOCATORS];

void add_Child(transformer * T, transformer * parent)
{
    int c;
    int condition = 1;

    for (c = 0; c < parent->childcount; c++)
    {
        if (T == parent->childs[c])
        {
            condition = 0;
            break;
        }
    }

    if (condition)
    {
        parent->childs = realloc(parent->childs, (parent->childcount + 1) * sizeof(transformer*));
        parent->childs[parent->childcount] = T;
        parent->childcount ++;
        T->parent = parent;
    }
}

void remove_Child(transformer * T, transformer * parent, transformer * newparent)
{
    int c, index;
    int condition = 0;

    index = 0;

    for (c = 0; c < parent->childcount; c++)
    {
        if (T == parent->childs[c])
        {
            index = c;
            condition = 1;
            break;
        }
    }

    if (condition)
    {
        parent->childcount --;
        for (c = index; c < parent->childcount; c ++)
        {
            parent->childs[c] = parent->childs[c + 1];
        }
        parent->childs = realloc(parent->childs, parent->childcount * sizeof(transformer*));

        if (newparent != NULL)
        {
            add_Child(T, newparent);
        }
    }
}

void init_transformer(transformer * T, transformer * parent, char * Name)
{
    if (transformerIndex < TRANSFORMERS)
    {
        transformers[transformerIndex] = T;
        T->index = transformerIndex;
        T->Name = malloc(256 * sizeof(char));
        sprintf(T->Name, "%s %d", Name, transformerCount);
        transformerIndex ++;
        transformerCount ++;
        T->Object = NULL;
        T->Deformer = NULL;
        T->Bone = NULL;
        T->selected = 0;
        T->style = 0;
        T->collapsed = 0;
        T->parent = parent;
        T->bind_set = 0;
        if (parent != NULL)
            add_Child(T, parent);
        T->childcount = 0;
        T->Selections_Count = 0;
        T->Selections = malloc(0 * sizeof(vert_selection*));
        T->childs = malloc(0 * sizeof(transformer*));
        T->rot_Order = yxz;
        memcpy(&T->scl, (float[3]) {1.0, 1.0, 1.0}, sizeof T->scl);
        memcpy(&T->scl_vec, (float[3]) {1.0, 1.0, 1.0}, sizeof T->scl_vec);
        memcpy(&T->rot, (float[3]) {0.0, 0.0, 0.0}, sizeof T->rot);
        memcpy(&T->rot_bind, (float[3]) {0.0, 0.0, 0.0}, sizeof T->rot_bind);
        memcpy(&T->rotVec, (float[3][3]) {{1.0, 0.0, 0.0}, {0.0, 1.0, 0.0}, {0.0, 0.0, 1.0}}, sizeof T->rotVec);
        memcpy(&T->rotVec_, (float[3][3]) {{1.0, 0.0, 0.0}, {0.0, 1.0, 0.0}, {0.0, 0.0, 1.0}}, sizeof T->rotVec_);
        memcpy(&T->rotVec_I, (float[3][3]) {{1.0, 0.0, 0.0}, {0.0, 1.0, 0.0}, {0.0, 0.0, 1.0}}, sizeof T->rotVec_I);
        memcpy(&T->rotVec_B, (float[3][3]) {{1.0, 0.0, 0.0}, {0.0, 1.0, 0.0}, {0.0, 0.0, 1.0}}, sizeof T->rotVec_B);
        memcpy(&T->pos, (float[3]) {0.0, 0.0, 0.0}, sizeof T->pos);
        memcpy(&T->pos_, (float[3]) {0.0, 0.0, 0.0}, sizeof T->pos_);
        memcpy(&T->Pos_, (float[3]) {0.0, 0.0, 0.0}, sizeof T->Pos_);
        memcpy(&T->pos_bind, (float[3]) {0.0, 0.0, 0.0}, sizeof T->pos_bind);
        memcpy(&T->target, (float[3]) {0.0, 0.0, 0.0}, sizeof T->target);
        T->aim = T->rotVec_[2];
    }
}

void invert_Rotation_scale(transformer * T, float RotVec_T[3][3])
{
    if (T->scl_vec[0] == 0) T->scl_vec[0] = MIN_SCALE;
    if (T->scl_vec[1] == 0) T->scl_vec[1] = MIN_SCALE;
    if (T->scl_vec[2] == 0) T->scl_vec[2] = MIN_SCALE;
    RotVec_T[0][0] = T->rotVec_[0][0] * (1 / T->scl_vec[0]);
    RotVec_T[0][1] = T->rotVec_[1][0] * (1 / T->scl_vec[1]);
    RotVec_T[0][2] = T->rotVec_[2][0] * (1 / T->scl_vec[2]);
    RotVec_T[1][0] = T->rotVec_[0][1] * (1 / T->scl_vec[0]);
    RotVec_T[1][1] = T->rotVec_[1][1] * (1 / T->scl_vec[1]);
    RotVec_T[1][2] = T->rotVec_[2][1] * (1 / T->scl_vec[2]);
    RotVec_T[2][0] = T->rotVec_[0][2] * (1 / T->scl_vec[0]);
    RotVec_T[2][1] = T->rotVec_[1][2] * (1 / T->scl_vec[1]);
    RotVec_T[2][2] = T->rotVec_[2][2] * (1 / T->scl_vec[2]);
}

void invert_Rotation(transformer * T, float RotVec_T[3][3])
{
    RotVec_T[0][0] = T->rotVec_[0][0];
    RotVec_T[0][1] = T->rotVec_[1][0];
    RotVec_T[0][2] = T->rotVec_[2][0];
    RotVec_T[1][0] = T->rotVec_[0][1];
    RotVec_T[1][1] = T->rotVec_[1][1];
    RotVec_T[1][2] = T->rotVec_[2][1];
    RotVec_T[2][0] = T->rotVec_[0][2];
    RotVec_T[2][1] = T->rotVec_[1][2];
    RotVec_T[2][2] = T->rotVec_[2][2];
}

void rotate_center(float cen_[3], float rotVec_[3][3], float cen[3], float result[3])
                            // data, matrix, center, result
{
    float x_ = cen_[0] - cen[0];
    float y_ = cen_[1] - cen[1];
    float z_ = cen_[2] - cen[2];
    float X = rotVec_[0][0] * x_ + rotVec_[1][0] * y_ + rotVec_[2][0] * z_;
    float Y = rotVec_[0][1] * x_ + rotVec_[1][1] * y_ + rotVec_[2][1] * z_;
    float Z = rotVec_[0][2] * x_ + rotVec_[1][2] * y_ + rotVec_[2][2] * z_;
    result[0] = X + cen[0];
    result[1] = Y + cen[1];
    result[2] = Z + cen[2];
}

void bake_coordinates(transformer * T, float cen[3], float rotVec[3][3])
{
                // data, matrix, center, result
    rotate_center(T->pos, rotVec, cen, T->pos_);
}

void bake_coordinates_children(transformer * T, float center[3], float rotVec[3][3])
{
    int c;
    transformer * C;
    for (c = 0; c < T->childcount; c ++)
    {
        C = T->childs[c];
        bake_coordinates(C, center, rotVec);
        bake_coordinates_children(C, center, rotVec);
    }
}

void bake_scale(transformer * T)
{
    float rotVec_I[3][3];
    invert_Rotation_scale(T, rotVec_I);
    bake_coordinates_children(T, T->pos, rotVec_I);
}

void bake(transformer * T)
{
    float rotVec_I[3][3];
    invert_Rotation(T, rotVec_I);
    bake_coordinates_children(T, T->pos, rotVec_I);
}

void move_Back(transformer * T, float amount)
{
    T->pos[0] = T->target[0] - T->aim[0] * amount;
    T->pos[1] = T->target[1] - T->aim[1] * amount;
    T->pos[2] = T->target[2] - T->aim[2] * amount;
}

void rotate_Vector(transformer * T, float x, float y, float z, direction * D)
{
    D->x = T->rotVec_[0][0] * x + T->rotVec_[1][0] * y + T->rotVec_[2][0] * z;
    D->y = T->rotVec_[0][1] * x + T->rotVec_[1][1] * y + T->rotVec_[2][1] * z;
    D->z = T->rotVec_[0][2] * x + T->rotVec_[1][2] * y + T->rotVec_[2][2] * z;
}

void rotate_matrix_I(float rotVec_R[3][3], float rotVec_[3][3], float rotVec_I[3][3])
{
    float x_0 = rotVec_I[0][0];
    float y_0 = rotVec_I[0][1];
    float z_0 = rotVec_I[0][2];
    rotVec_R[0][0] = rotVec_[0][0] * x_0 + rotVec_[1][0] * y_0 + rotVec_[2][0] * z_0;
    rotVec_R[0][1] = rotVec_[0][1] * x_0 + rotVec_[1][1] * y_0 + rotVec_[2][1] * z_0;
    rotVec_R[0][2] = rotVec_[0][2] * x_0 + rotVec_[1][2] * y_0 + rotVec_[2][2] * z_0;
    float x_1 = rotVec_I[1][0];
    float y_1 = rotVec_I[1][1];
    float z_1 = rotVec_I[1][2];
    rotVec_R[1][0] = rotVec_[0][0] * x_1 + rotVec_[1][0] * y_1 + rotVec_[2][0] * z_1;
    rotVec_R[1][1] = rotVec_[0][1] * x_1 + rotVec_[1][1] * y_1 + rotVec_[2][1] * z_1;
    rotVec_R[1][2] = rotVec_[0][2] * x_1 + rotVec_[1][2] * y_1 + rotVec_[2][2] * z_1;
    float x_2 = rotVec_I[2][0];
    float y_2 = rotVec_I[2][1];
    float z_2 = rotVec_I[2][2];
    rotVec_R[2][0] = rotVec_[0][0] * x_2 + rotVec_[1][0] * y_2 + rotVec_[2][0] * z_2;
    rotVec_R[2][1] = rotVec_[0][1] * x_2 + rotVec_[1][1] * y_2 + rotVec_[2][1] * z_2;
    rotVec_R[2][2] = rotVec_[0][2] * x_2 + rotVec_[1][2] * y_2 + rotVec_[2][2] * z_2;
}

void load_Identity(transformer * T)
{
    memcpy(T->rotVec_, (float[3][3]) {{1.0, 0.0, 0.0}, {0.0, 1.0, 0.0}, {0.0, 0.0, 1.0}}, sizeof T->rotVec_);
}

void normalize_rotation_parent(transformer * T)
{
    float rotVec_I[3][3];
    if (T->parent != NULL)
        invert_Rotation(T->parent, rotVec_I);
    rotate_matrix_I(T->rotVec_I, rotVec_I, T->rotVec_I);
}

void normalize_rotation_unparent(transformer * T)
{
    if (T->parent != NULL)
        rotate_matrix_I(T->rotVec_I, T->parent->rotVec_, T->rotVec_I);
}

void normalize_rotation_unparent_childs(transformer * T)
{
    int c;

    transformer * C;

    for(c = 0; c < T->childcount; c ++)
    {
        C = T->childs[c];
        normalize_rotation_unparent(C);
        normalize_rotation_unparent_childs(C);
    }
}

void normalize_rotation_parent_childs(transformer * T)
{
    int c;

    transformer * C;

    for(c = 0; c < T->childcount; c ++)
    {
        C = T->childs[c];
        normalize_rotation_parent(C);
        normalize_rotation_parent_childs(C);
    }
}

void rotate_Vector_I(transformer * T, float x, float y, float z, direction * D)
{
    float rotVec_I[3][3];
    invert_Rotation(T, rotVec_I);
    D->x = rotVec_I[0][0] * x + rotVec_I[1][0] * y + rotVec_I[2][0] * z;
    D->y = rotVec_I[0][1] * x + rotVec_I[1][1] * y + rotVec_I[2][1] * z;
    D->z = rotVec_I[0][2] * x + rotVec_I[1][2] * y + rotVec_I[2][2] * z;
}

void rotate_Vertex_I(float rotVec_I[3][3], float x, float y, float z, direction * D)
{
    D->x = rotVec_I[0][0] * x + rotVec_I[1][0] * y + rotVec_I[2][0] * z;
    D->y = rotVec_I[0][1] * x + rotVec_I[1][1] * y + rotVec_I[2][1] * z;
    D->z = rotVec_I[0][2] * x + rotVec_I[1][2] * y + rotVec_I[2][2] * z;
}

void rotate_axis(float angle, float axis1[3], float axis2[3], float result1[3], float result2[3])
{
    float point_1[3];
    float point_2[3];

    float axis3[3] = {-axis1[0], -axis1[1], -axis1[2]};
    float axis4[3] = {-axis2[0], -axis2[1], -axis2[2]};
    float S_0[3] = {axis2[0] - axis1[0], axis2[1] - axis1[1], axis2[2] - axis1[2]};
    float S_1[3] = {axis3[0] - axis2[0], axis3[1] - axis2[1], axis3[2] - axis2[2]};
    float S_2[3] = {axis4[0] - axis3[0], axis4[1] - axis3[1], axis4[2] - axis3[2]};
    float S_3[3] = {axis1[0] - axis4[0], axis1[1] - axis4[1], axis1[2] - axis4[2]};
    if (angle < 0)
    {
        angle = -angle;
        angle = fmod(angle, pi2);
        angle = pi2 - angle;
    }
    else
    {
        angle = fmod(angle, pi2);
    }
    float a = fmod(angle, pi_2);
    float D = a / pi_2;

    if (angle == 0 || angle == pi2)
    {
        result1[0] = axis1[0];
        result1[1] = axis1[1];
        result1[2] = axis1[2];
        result2[0] = axis2[0];
        result2[1] = axis2[1];
        result2[2] = axis2[2];
        goto END;
//        point_1[0] = axis1[0];
//        point_1[1] = axis1[1];
//        point_1[2] = axis1[2];
//        point_2[0] = axis2[0];
//        point_2[1] = axis2[1];
//        point_2[2] = axis2[2];
    }
    else if (angle == pi_2)
    {
        point_1[0] = axis2[0];
        point_1[1] = axis2[1];
        point_1[2] = axis2[2];
        point_2[0] = axis3[0];
        point_2[1] = axis3[1];
        point_2[2] = axis3[2];
    }
    else if (angle == pi)
    {
        point_1[0] = axis3[0];
        point_1[1] = axis3[1];
        point_1[2] = axis3[2];
        point_2[0] = axis4[0];
        point_2[1] = axis4[1];
        point_2[2] = axis4[2];
    }
    else if (angle == pi_3)
    {
        point_1[0] = axis4[0];
        point_1[1] = axis4[1];
        point_1[2] = axis4[2];
        point_2[0] = axis1[0];
        point_2[1] = axis1[1];
        point_2[2] = axis1[2];
    }
    else if (angle < pi_2 && angle >= 0.0)
    {
        point_1[0] = S_0[0] * D + axis1[0];
        point_1[1] = S_0[1] * D + axis1[1];
        point_1[2] = S_0[2] * D + axis1[2];
        point_2[0] = S_1[0] * D + axis2[0];
        point_2[1] = S_1[1] * D + axis2[1];
        point_2[2] = S_1[2] * D + axis2[2];
    }
    else if (angle < pi && angle >= pi_2)
    {
        point_1[0] = S_1[0] * D + axis2[0];
        point_1[1] = S_1[1] * D + axis2[1];
        point_1[2] = S_1[2] * D + axis2[2];
        point_2[0] = S_2[0] * D + axis3[0];
        point_2[1] = S_2[1] * D + axis3[1];
        point_2[2] = S_2[2] * D + axis3[2];
    }
    else if (angle < pi_3 && angle >= pi)
    {
        point_1[0] = S_2[0] * D + axis3[0];
        point_1[1] = S_2[1] * D + axis3[1];
        point_1[2] = S_2[2] * D + axis3[2];
        point_2[0] = S_3[0] * D + axis4[0];
        point_2[1] = S_3[1] * D + axis4[1];
        point_2[2] = S_3[2] * D + axis4[2];
    }
    else if (angle < pi2 && angle >= pi_3)
    {
        point_1[0] = S_3[0] * D + axis4[0];
        point_1[1] = S_3[1] * D + axis4[1];
        point_1[2] = S_3[2] * D + axis4[2];
        point_2[0] = S_0[0] * D + axis1[0];
        point_2[1] = S_0[1] * D + axis1[1];
        point_2[2] = S_0[2] * D + axis1[2];
    }

    float vec_len = sqrt(point_1[0] * point_1[0] + point_1[1] * point_1[1] + point_1[2] * point_1[2]);
    result1[0] = point_1[0] / vec_len;
    result1[1] = point_1[1] / vec_len;
    result1[2] = point_1[2] / vec_len;
    result2[0] = point_2[0] / vec_len;
    result2[1] = point_2[1] / vec_len;
    result2[2] = point_2[2] / vec_len;
    END:;
}

void rotate_axis_yxz(transformer * T)
{
    rotate_axis(T->rot[1], T->rotVec_[2], T->rotVec_[0], T->rotVec[2], T->rotVec[0]);
    rotate_axis(T->rot[0], T->rotVec_[1], T->rotVec[2], T->rotVec[1], T->rotVec[2]);
    rotate_axis(T->rot[2], T->rotVec[0], T->rotVec[1], T->rotVec[0], T->rotVec[1]);
}

void rotate_axis_zxy(transformer * T)
{
    rotate_axis(T->rot[2], T->rotVec_[0], T->rotVec_[1], T->rotVec[0], T->rotVec[1]);
    rotate_axis(T->rot[0], T->rotVec[1], T->rotVec_[2], T->rotVec[1], T->rotVec[2]);
    rotate_axis(T->rot[1], T->rotVec[2], T->rotVec[0], T->rotVec[2], T->rotVec[0]);
}

void rotate_axis_zyx(transformer * T)
{
    rotate_axis(T->rot[2], T->rotVec_[0], T->rotVec_[1], T->rotVec[0], T->rotVec[1]);
    rotate_axis(T->rot[1], T->rotVec_[2], T->rotVec[0], T->rotVec[2], T->rotVec[0]);
    rotate_axis(T->rot[0], T->rotVec[1], T->rotVec[2], T->rotVec[1], T->rotVec[2]);
}

void rotate_axis_xyz(transformer * T)
{
    rotate_axis(T->rot[0], T->rotVec_[1], T->rotVec_[2], T->rotVec[1], T->rotVec[2]);
    rotate_axis(T->rot[1], T->rotVec[2], T->rotVec_[0], T->rotVec[2], T->rotVec[0]);
    rotate_axis(T->rot[2], T->rotVec[0], T->rotVec[1], T->rotVec[0], T->rotVec[1]);
}

void rotate_axis_xzy(transformer * T)
{
    rotate_axis(T->rot[0], T->rotVec_[1], T->rotVec_[2], T->rotVec[1], T->rotVec[2]);
    rotate_axis(T->rot[2], T->rotVec_[0], T->rotVec[1], T->rotVec[0], T->rotVec[1]);
    rotate_axis(T->rot[1], T->rotVec[2], T->rotVec[0], T->rotVec[2], T->rotVec[0]);
}

void rotate_axis_yzx(transformer * T)
{
    rotate_axis(T->rot[1], T->rotVec_[2], T->rotVec_[0], T->rotVec[2], T->rotVec[0]);
    rotate_axis(T->rot[2], T->rotVec[0], T->rotVec_[1], T->rotVec[0], T->rotVec[1]);
    rotate_axis(T->rot[0], T->rotVec[1], T->rotVec[2], T->rotVec[1], T->rotVec[2]);
}

void scale_x(transformer * T)
{
    T->rotVec[0][0] = T->rotVec_[0][0] * T->scl_vec[0];
    T->rotVec[0][1] = T->rotVec_[0][1] * T->scl_vec[0];
    T->rotVec[0][2] = T->rotVec_[0][2] * T->scl_vec[0];
}

void scale_y(transformer * T)
{
    T->rotVec[1][0] = T->rotVec_[1][0] * T->scl_vec[1];
    T->rotVec[1][1] = T->rotVec_[1][1] * T->scl_vec[1];
    T->rotVec[1][2] = T->rotVec_[1][2] * T->scl_vec[1];
}

void scale_z(transformer * T)
{
    T->rotVec[2][0] = T->rotVec_[2][0] * T->scl_vec[2];
    T->rotVec[2][1] = T->rotVec_[2][1] * T->scl_vec[2];
    T->rotVec[2][2] = T->rotVec_[2][2] * T->scl_vec[2];
}

void scale_axis(transformer * T)
{
    memcpy(T->rotVec_, T->rotVec, sizeof T->rotVec_);

    if (T->parent != NULL) // transfer scale
    {
        T->scl_vec[0] = T->scl[0] * T->parent->scl_vec[0];
        T->scl_vec[1] = T->scl[1] * T->parent->scl_vec[1];
        T->scl_vec[2] = T->scl[2] * T->parent->scl_vec[2];
    }

    if (T->scl_vec[0] != 1.0)
        scale_x(T);
    if (T->scl_vec[1] != 1.0)
        scale_y(T);
    if (T->scl_vec[2] != 1.0)
        scale_z(T);
}

void rotate_T_(transformer * T)
{
    memcpy(T->rotVec_, Identity_, sizeof(T->rotVec_));

    if (T->rot_Order == zxy)
        rotate_axis_zxy(T);
    else if (T->rot_Order == yxz)
        rotate_axis_yxz(T);
    else if (T->rot_Order == zyx)
        rotate_axis_zyx(T);
    else if (T->rot_Order == xyz)
        rotate_axis_xyz(T);
    else if (T->rot_Order == xzy)
        rotate_axis_xzy(T);
    else if (T->rot_Order == yzx)
        rotate_axis_yzx(T);

    scale_axis(T);
}

void rotate_T(transformer * T)
{
    if (T->Bone != NULL && T->parent != NULL)
    {
        if (T == T->Bone->B)
            memcpy(T->rotVec_, T->Bone->A->rotVec_, sizeof(T->rotVec_));
        else
            rotate_matrix_I(T->rotVec_, T->parent->rotVec_, T->Bone->rotVec_I);
    }
    else if (T->parent != NULL)
    {
        rotate_matrix_I(T->rotVec_, T->parent->rotVec_, T->rotVec_I);
    }

    // rotVec_I serves as inverse matrix and should not be modified unless by parenting
    // other inverse matrices are constructed on the fly.
    // If objects parent is World object, this should result for rotVec_ identity matrix.

    //load_Identity(T);

//    printf("CAM x aim %f %f %f\n", T->rotVec_[0][0], T->rotVec_[0][1], T->rotVec_[0][2]);
//    printf("CAM y aim %f %f %f\n", T->rotVec_[1][0], T->rotVec_[1][1], T->rotVec_[1][2]);
//    printf("CAM z aim %f %f %f\n", T->rotVec_[2][0], T->rotVec_[2][1], T->rotVec_[2][2]);
//
//    float dx = dot_productFF(T->rotVec[1], T->rotVec[2]);
//    float dy = dot_productFF(T->rotVec[2], T->rotVec[0]);
//    float dz = dot_productFF(T->rotVec[0], T->rotVec[1]);
//
//    printf("dx, dy, dz %f %f %f\n\n", dx, dy, dz);

    if (T->rot_Order == zxy)
        rotate_axis_zxy(T);
    else if (T->rot_Order == yxz)
        rotate_axis_yxz(T);
    else if (T->rot_Order == zyx)
        rotate_axis_zyx(T);
    else if (T->rot_Order == xyz)
        rotate_axis_xyz(T);
    else if (T->rot_Order == xzy)
        rotate_axis_xzy(T);
    else if (T->rot_Order == yzx)
        rotate_axis_yzx(T);

    scale_axis(T);
}

void rotate_vertex_groups(transformer * T);

void rotate_children_reset(transformer * T, float pos[3], float rotVec_[3][3], int vertexgroups)
{
    int c;
    transformer * C;

    for (c = 0; c < T->childcount; c ++)
    {
        C = T->childs[c];
        rotate_center(C->pos_, rotVec_, pos, C->pos);
                      // data, matrix, center, result
        if (C->Bone != NULL)
        {
            if (C == C->Bone->B)
                memcpy(C->rotVec_, C->Bone->A->rotVec_, sizeof(C->rotVec_));
            else
                rotate_matrix_I(C->rotVec_, C->parent->rotVec_, C->Bone->rotVec_I);
        }
        else if (C->parent != NULL)
        {
            rotate_matrix_I(C->rotVec_, C->parent->rotVec_, C->rotVec_I);
        }

        memcpy(C->rotVec, C->rotVec_, sizeof C->rotVec);

        if (vertexgroups)
            rotate_vertex_groups(C);

        rotate_children_reset(C, pos, rotVec_, vertexgroups);
    }
}

void rotate_children(transformer * T, float pos[3], float rotVec_[3][3], int vertexgroups)
{
    int c;
    transformer * C;

    for (c = 0; c < T->childcount; c ++)
    {
        C = T->childs[c];
        rotate_center(C->pos_, rotVec_, pos, C->pos);
                      // data, matrix, center, result
        if (C->Bone != NULL)
        {
            if (C == C->Bone->B)
                memcpy(C->rotVec_, C->Bone->A->rotVec_, sizeof(C->rotVec_));
            else
                rotate_matrix_I(C->rotVec_, C->parent->rotVec_, C->Bone->rotVec_I);
        }
        else if (C->parent != NULL)
        {
            rotate_matrix_I(C->rotVec_, C->parent->rotVec_, C->rotVec_I);
        }

        if (C->rot_Order == zxy)
            rotate_axis_zxy(C);
        else if (C->rot_Order == yxz)
            rotate_axis_yxz(C);
        else if (C->rot_Order == zyx)
            rotate_axis_zyx(C);
        else if (C->rot_Order == xyz)
            rotate_axis_xyz(C);
        else if (C->rot_Order == xzy)
            rotate_axis_xzy(C);
        else if (C->rot_Order == yzx)
            rotate_axis_yzx(C);

        scale_axis(C);

        if (vertexgroups)
            rotate_vertex_groups(C);

        rotate_children(C, pos, rotVec_, vertexgroups);
    }
}

void rotate_vertex_groups_collect_Objects(transformer * T);

void rotate_collect(transformer * T)
{
    rotate_vertex_groups_collect_Objects(T);
    int c;
    transformer * C;

    for (c = 0; c < T->childcount; c ++)
    {
        C = T->childs[c];
        rotate_collect(C);
    }
}

void rotate_reset(transformer * T, int vertexgroups)
{
    if (T->Bone != NULL && T->parent != NULL)
    {
        if (T == T->Bone->B)
            memcpy(T->rotVec_, T->Bone->A->rotVec_, sizeof(T->rotVec_));
        else
            rotate_matrix_I(T->rotVec_, T->parent->rotVec_, T->Bone->rotVec_I);
    }
    else if (T->parent != NULL)
    {
        rotate_matrix_I(T->rotVec_, T->parent->rotVec_, T->rotVec_I);
    }

    memcpy(T->rotVec, T->rotVec_, sizeof T->rotVec);

    if (vertexgroups)
        rotate_vertex_groups(T);

    if (SCALE)
        rotate_children_reset(T, T->pos, T->rotVec, vertexgroups);
    else
        rotate_children_reset(T, T->pos, T->rotVec_, vertexgroups);
}

void rotate(transformer * T, int vertexgroups)
{
    if (T->Bone != NULL && T->parent != NULL)
    {
        if (T == T->Bone->B)
            memcpy(T->rotVec_, T->Bone->A->rotVec_, sizeof(T->rotVec_));
        else
            rotate_matrix_I(T->rotVec_, T->parent->rotVec_, T->Bone->rotVec_I);
    }
    else if (T->parent != NULL)
    {
        rotate_matrix_I(T->rotVec_, T->parent->rotVec_, T->rotVec_I);
    }

    if (T->rot_Order == zxy)
        rotate_axis_zxy(T);
    else if (T->rot_Order == yxz)
        rotate_axis_yxz(T);
    else if (T->rot_Order == zyx)
        rotate_axis_zyx(T);
    else if (T->rot_Order == xyz)
        rotate_axis_xyz(T);
    else if (T->rot_Order == xzy)
        rotate_axis_xzy(T);
    else if (T->rot_Order == yzx)
        rotate_axis_yzx(T);

    scale_axis(T);

    if (vertexgroups)
        rotate_vertex_groups(T);

    if (SCALE)
        rotate_children(T, T->pos, T->rotVec, vertexgroups);
    else
        rotate_children(T, T->pos, T->rotVec_, vertexgroups);
}

void rotate_hierarchy_T(transformer * P, transformer * T)
{
    int c;
    transformer * C;
    rotate_T(P);
    rotate_vertex_groups(P);
    for (c = 0; c < P->childcount; c ++)
    {
        C = P->childs[c];
        if (C == T)
        {
            continue;
        }
        rotate_hierarchy_T(C, T);
    }
}

void rotate_vertex_groups_D_Init();

void rotate_Deformer(transformer * T)
{
    transformer * P = T->parent;
    while (P != NULL && P->Deformer == T->Deformer)
    {
        P = P->parent;
    }
    rotate_collect(P);
    rotate_vertex_groups_D_Init();
    rotate_hierarchy_T(P, T);
    rotate(T, 1);
}

void rotate_hierarchy_pose(transformer * T)
{
    int c;
    transformer * C;
    bake_scale(T);
    rotate(T, 0);
    for (c = 0; c < T->childcount; c ++)
    {
        C = T->childs[c];
        rotate_hierarchy_pose(C);
    }
}

void rotate_Deformer_pose(transformer * T)
{
    transformer * P = T->parent;
    while (P != NULL && P->Deformer == T->Deformer)
    {
        P = P->parent;
    }
    rotate_collect(P);
    rotate_vertex_groups_D_Init();
    SCALE = 1;
    rotate_hierarchy_pose(P);
    SCALE = 0;
}

void transfer_rotVec_B(transformer * T)
{
    float rotVec_I[3][3];
    invert_Rotation(T, rotVec_I);
    memcpy(T->rotVec_B, rotVec_I, sizeof(T->rotVec_B));

    int c;
    transformer * C;

    for (c = 0; c < T->childcount; c ++)
    {
        C = T->childs[c];
        transfer_rotVec_B(C);
    }
}

void rotate_bind(transformer * T)
{
    if (T->Bone == NULL)
    {
        memcpy(T->rotVec_, T->rotVec_I, sizeof(T->rotVec_));

        if (T->rot_Order == zxy)
            rotate_axis_zxy(T);
        else if (T->rot_Order == yxz)
            rotate_axis_yxz(T);
        else if (T->rot_Order == zyx)
            rotate_axis_zyx(T);
        else if (T->rot_Order == xyz)
            rotate_axis_xyz(T);
        else if (T->rot_Order == xzy)
            rotate_axis_xzy(T);
        else if (T->rot_Order == yzx)
            rotate_axis_yzx(T);

        scale_axis(T);
    }

    //memcpy(T->rotVec_B, T->rotVec_, sizeof(T->rotVec_B));


}

void synthesize_Bone_Alignement(bone * B);

void synthesize_Bone_Alignement_childs(transformer * T)
{
    int c;
    bone * B = T->Bone;
    if (B != NULL)
    {
        transformer * C;
        for (c = 0; c < T->childcount; c ++)
        {
            C = T->childs[c];
            B = C->Bone;
            if (B != NULL)
                synthesize_Bone_Alignement(B);
        }
    }

}

void move_T(transformer * T, float Delta[3])
{
    T->pos[0] = T->Pos_[0] + Delta[0];
    T->pos[1] = T->Pos_[1] + Delta[1];
    T->pos[2] = T->Pos_[2] + Delta[2];
}

void move_T_childs(transformer * T, float Delta[3])
{
    int c;
    transformer * C;
    for (c = 0; c < T->childcount; c ++)
    {
        C = T->childs[c];
        move_T(C, Delta);
    }
}

void move_vertex_groups(transformer * T);

void move(transformer * T, float Delta[3])
{
    T->pos[0] = T->Pos_[0] + Delta[0];
    T->pos[1] = T->Pos_[1] + Delta[1];
    T->pos[2] = T->Pos_[2] + Delta[2];

    rotate_vertex_groups(T);
}

void reset_Scale()
{
    int t;
    transformer * T;

    for (t = 0; t < transformerIndex; t ++)
    {
        T = transformers[t];
        T->scl[0] = 1;
        T->scl[1] = 1;
        T->scl[2] = 1;
    }
}

void collect_Children(transformer * T)
{
    int c;
    for (c = 0; c < T->childcount; c ++)
    {
        child_collection[child_collection_count ++] = T->childs[c];
        collect_Children(T->childs[c]);
    }
}

void move_Children_T(transformer * T, float Delta[3])
{
    int c;
    transformer * C;

    for (c = 0; c < child_collection_count; c ++)
    {
        C = child_collection[c];
        move_T(C, Delta);
    }
}

void move_Children(transformer * T, float Delta[3])
{
    int c;
    transformer * C;

    for (c = 0; c < child_collection_count; c ++)
    {
        C = child_collection[c];
        move(C, Delta);
    }
}

void move_Deformer(transformer * T, float Delta[3])
{
    transformer * P = T->parent;
    while (P != NULL && P->Deformer == T->Deformer)
    {
        P = P->parent;
    }
    rotate_collect(P);
    rotate_vertex_groups_D_Init();
    rotate_hierarchy_T(P, T);
    move(T, Delta);
    move_Children(T, Delta);
}

void update_rotate_bounding_box();

void move_(transformer * T, float Delta[3])
{
    if(T->Deformer != NULL)
    {
        move_Deformer(T, Delta);
    }
    else
    {
        rotate_vertex_groups_D_Init();
        move(T, Delta);
        move_Children(T, Delta);
    }
    update_rotate_bounding_box();
}

void bake_position(transformer * T)
{
    T->Pos_[0] = T->pos[0];
    T->Pos_[1] = T->pos[1];
    T->Pos_[2] = T->pos[2];
}

void bake_position_Children(transformer * T)
{
    int c;
    transformer * C;

    child_collection_count = 0;

    collect_Children(T);
    for (c = 0; c < child_collection_count; c ++)
    {
        C = child_collection[c];
        bake_position(C);
    }
}

void bake_pose(transformer * T)
{
    T->pos[0] = T->Pos_[0];
    T->pos[1] = T->Pos_[1];
    T->pos[2] = T->Pos_[2];
}

void bake_pose_Children(transformer * T)
{
    int c;
    transformer * C;

    child_collection_count = 0;

    collect_Children(T);
    for (c = 0; c < child_collection_count; c ++)
    {
        C = child_collection[c];
        bake_pose(C);
    }
}
