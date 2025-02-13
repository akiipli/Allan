/*
The MIT License

Copyright <2018> <Allan Kiipli>
*/

#define TRANSFORMERS 1000
#define LOCATORS 1000

#define bone_end 1
#define ik_start 2
#define ik_goal 3
#define root_node 4
#define ik_fixed 5

float MIN_SCALE = 1 / 1000000;

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

#define zxy 0
#define yxz 1
#define zyx 2
#define xyz 3
#define xzy 4
#define yzx 5

#define pin_0 0
#define pin_x 1
#define pin_y 2
#define pin_z 3

#define aim_vector 2
#define world 0

#define constraint_none 0
#define constraint_to_point 1
#define constraint_pole 2

typedef struct transformer transformer;
typedef struct object object;
typedef struct deformer deformer;
typedef struct bone bone;

typedef struct ikChain ikChain;

typedef struct
{
    float vec[3];
}
vec3;

typedef struct
{
    float distance;
    float vec[3];
}
direction_Pack;

typedef struct
{
    int index;
    unsigned address; // assigned after loading
    char * Name;
    int selected;
    int constraint_type;
    transformer * Locator;
    transformer * IK_goal;
}
constraint;

typedef struct
{
    ikChain * IK;
    deformer * Deformer;
}
constraint_Pack;

constraint_Pack Constraint_Pack;

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
    int update;
    int stretch;
    constraint * C;
    constraint * Pole;
}
;

void project_IK_goal_To_Spine(ikChain * I);

typedef struct bone
{
    int index;
    unsigned address; // assigned after loading
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
    int IK_member; // 0 not, 1 begin, 2 end, 3 in chain
    ikChain * IK;
    float len;
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
    int * indices;
    float * weights;
    int indices_count;
}
split_selection;

typedef struct
{
    int index;
    unsigned address; // assigned after loading
    object * Object;
    transformer * Transformer;
    char * Name;
    int * indices;
    float * weights;
    int indices_count;
    split_selection split[2];
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
    unsigned address; // assigned after loading
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
    float rotVec_Pin[3][3];
    float pos[3];
    float pos_[3];
    float Pos_[3];
    float pos_bind[3];
    float pos_deform[3];
    float target[3];
    float * aim;  //rotVec_[aim_vector]
    ikChain * IK;
    float LocatorSize;
    int pin; //x 1, y 2, z 3
    constraint * Constraint;
    timeline * Timeline;
    traj * Trj;
    float Trj_Value;
};

transformer * child_collection[TRANSFORMERS];
int child_collection_count;

transformer * transformers[TRANSFORMERS];
transformer * Locators[LOCATORS];

float delta, delta_x, delta_y, delta_1, delta_2;
int multi_Rotation = 1;
transformer * T_m;
int multi_Rotation_Transformers_Count = 0;
transformer * multi_Rotation_Transformers[TRANSFORMERS];

float object_Rot_[TRANSFORMERS][3];
float object_Scl_[TRANSFORMERS][3];

transformer ** Action_Begin_Transformers;
int Action_Begin_Transformers_Count;

transformer * Action_Center; /* this transformer is not listed in transformers hierarchy */
                             /* its function is to aid cps manipulation when transforming them */

void reset_Action_Center()
{
    Action_Center->scl_vec[0] = 1;
    Action_Center->scl_vec[1] = 1;
    Action_Center->scl_vec[2] = 1;
    Action_Center->rot[0] = 0;
    Action_Center->rot[1] = 0;
    Action_Center->rot[2] = 0;
}

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

int init_timeline(transformer * T)
{
    timeline * Tm;

    Tm = malloc(sizeof(timeline));
    if (Tm != NULL)
    {
        Tm->key_frames = 0;
        Tm->Frames = NULL;
        Tm->Values = NULL;
        Tm->Acceleration = NULL;
        T->Timeline = Tm;
        return 1;
    }
    else
    {
        return 0;
    }
}

int delete_keyframe(transformer * T, int frame)
{
    timeline * Tm;
    int f, index, condition;

    if (T->Timeline != NULL)
    {
        Tm = T->Timeline;
        condition = 0;

        for (f = 0; f < Tm->key_frames; f ++)
        {
            if (Tm->Frames[f] == frame)
            {
                index = f;
                condition = 1;
                break;
            }
        }

        if (condition)
        {
            Tm->key_frames --;
            for (f = index; f < Tm->key_frames; f ++)
            {
                Tm->Frames[f] = Tm->Frames[f + 1];
                memcpy(&Tm->Acceleration[f], &Tm->Acceleration[f + 1], sizeof(acceleration));
                memcpy(&Tm->Values[f], &Tm->Values[f + 1], sizeof(transformer_values));
            }
            if (f - 1 >= 0)
                return Tm->Frames[f - 1];
            else
                return -1;
        }
        else
        {
            return -1;
        }
    }
    else
    {
        return -1;
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
        memcpy(&T->rotVec_Pin, (float[3][3]) {{1.0, 0.0, 0.0}, {0.0, 1.0, 0.0}, {0.0, 0.0, 1.0}}, sizeof T->rotVec_Pin);
        memcpy(&T->pos, (float[3]) {0.0, 0.0, 0.0}, sizeof T->pos);
        memcpy(&T->pos_, (float[3]) {0.0, 0.0, 0.0}, sizeof T->pos_);
        memcpy(&T->Pos_, (float[3]) {0.0, 0.0, 0.0}, sizeof T->Pos_);
        memcpy(&T->pos_bind, (float[3]) {0.0, 0.0, 0.0}, sizeof T->pos_bind);
        memcpy(&T->target, (float[3]) {0.0, 0.0, 0.0}, sizeof T->target);
        T->aim = T->rotVec_[2];
        T->IK = NULL;
        T->LocatorSize = LocatorSize;
        T->pin = 0;
        T->Constraint = NULL;
        T->Timeline = NULL;
        T->Trj = NULL;
        T->Trj_Value = 0;
    }
}

void init_Action_Center()
{
    transformer * T = calloc(1, sizeof(transformer));
    Action_Center = T;
    memcpy(&T->scl, (float[3]) {1.0, 1.0, 1.0}, sizeof T->scl);
    memcpy(&T->scl_vec, (float[3]) {1.0, 1.0, 1.0}, sizeof T->scl_vec);
    memcpy(&T->rotVec, (float[3][3]) {{1.0, 0.0, 0.0}, {0.0, 1.0, 0.0}, {0.0, 0.0, 1.0}}, sizeof T->rotVec);
    memcpy(&T->rotVec_, (float[3][3]) {{1.0, 0.0, 0.0}, {0.0, 1.0, 0.0}, {0.0, 0.0, 1.0}}, sizeof T->rotVec_);
    memcpy(&T->rotVec_I, (float[3][3]) {{1.0, 0.0, 0.0}, {0.0, 1.0, 0.0}, {0.0, 0.0, 1.0}}, sizeof T->rotVec_I);
    memcpy(&T->rotVec_B, (float[3][3]) {{1.0, 0.0, 0.0}, {0.0, 1.0, 0.0}, {0.0, 0.0, 1.0}}, sizeof T->rotVec_B);
    memcpy(&T->rotVec_Pin, (float[3][3]) {{1.0, 0.0, 0.0}, {0.0, 1.0, 0.0}, {0.0, 0.0, 1.0}}, sizeof T->rotVec_Pin);
}

float length1(float V[3])
{
    float l = sqrt(V[0] * V[0] + V[1] * V[1] + V[2] * V[2]);
    return l;
}

void scale_rotVec(float rotVec_R[3][3], float rotVec_T[3][3], float scl_vec[3])
{
    rotVec_R[0][0] = rotVec_T[0][0] * scl_vec[0];
    rotVec_R[0][1] = rotVec_T[0][1] * scl_vec[0];
    rotVec_R[0][2] = rotVec_T[0][2] * scl_vec[0];
    rotVec_R[1][0] = rotVec_T[1][0] * scl_vec[1];
    rotVec_R[1][1] = rotVec_T[1][1] * scl_vec[1];
    rotVec_R[1][2] = rotVec_T[1][2] * scl_vec[1];
    rotVec_R[2][0] = rotVec_T[2][0] * scl_vec[2];
    rotVec_R[2][1] = rotVec_T[2][1] * scl_vec[2];
    rotVec_R[2][2] = rotVec_T[2][2] * scl_vec[2];
}

void generate_scl_vec(transformer * T)
{
    float len[3];
    len[0] = length1(T->rotVec[0]);
    len[1] = length1(T->rotVec[1]);
    len[2] = length1(T->rotVec[2]);
    T->scl_vec[0] = len[0];
    T->scl_vec[1] = len[1];
    T->scl_vec[2] = len[2];
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

void invert_Rotation_1(float RotVec_R[3][3], float RotVec_T[3][3])
{
    RotVec_R[0][0] = RotVec_T[0][0];
    RotVec_R[0][1] = RotVec_T[1][0];
    RotVec_R[0][2] = RotVec_T[2][0];
    RotVec_R[1][0] = RotVec_T[0][1];
    RotVec_R[1][1] = RotVec_T[1][1];
    RotVec_R[1][2] = RotVec_T[2][1];
    RotVec_R[2][0] = RotVec_T[0][2];
    RotVec_R[2][1] = RotVec_T[1][2];
    RotVec_R[2][2] = RotVec_T[2][2];
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

void rotate_vector(float rotVec_[3][3], float vec[3], float result[3])
                            // matrix, center, result
{
    result[0] = rotVec_[0][0] * vec[0] + rotVec_[1][0] * vec[1] + rotVec_[2][0] * vec[2];
    result[1] = rotVec_[0][1] * vec[0] + rotVec_[1][1] * vec[1] + rotVec_[2][1] * vec[2];
    result[2] = rotVec_[0][2] * vec[0] + rotVec_[1][2] * vec[1] + rotVec_[2][2] * vec[2];
}

void rotate_center_(float cen_[3], float rotVec_[3][3], float cen[3], float result[3])
                            // data, matrix, center, result
{
    float x_ = cen_[0] - cen[0];
    float y_ = cen_[1] - cen[1];
    float z_ = cen_[2] - cen[2];
    float X = rotVec_[0][0] * x_ + rotVec_[1][0] * y_ + rotVec_[2][0] * z_;
    float Y = rotVec_[0][1] * x_ + rotVec_[1][1] * y_ + rotVec_[2][1] * z_;
    float Z = rotVec_[0][2] * x_ + rotVec_[1][2] * y_ + rotVec_[2][2] * z_;
    result[0] = X;
    result[1] = Y;
    result[2] = Z;
}

void rotate_center_1(float cen_[3], float rotVec_[3][3], float cen[3], float result[3])
                            // data, matrix, center, result
{
    float x_ = cen_[0];
    float y_ = cen_[1];
    float z_ = cen_[2];
    float X = rotVec_[0][0] * x_ + rotVec_[1][0] * y_ + rotVec_[2][0] * z_;
    float Y = rotVec_[0][1] * x_ + rotVec_[1][1] * y_ + rotVec_[2][1] * z_;
    float Z = rotVec_[0][2] * x_ + rotVec_[1][2] * y_ + rotVec_[2][2] * z_;
    result[0] = X + cen[0];
    result[1] = Y + cen[1];
    result[2] = Z + cen[2];
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

void transfer_coordinates(transformer * T)
{
    T->pos_[0] = T->pos[0];
    T->pos_[1] = T->pos[1];
    T->pos_[2] = T->pos[2];
}

void bake_coordinates(transformer * T)
{
    float rotVec_I[3][3];
    invert_Rotation(T->parent, rotVec_I);
                // data, matrix, center, result
    rotate_center_(T->pos, rotVec_I, T->parent->pos, T->pos_);
}

void bake_coordinates_children(transformer * T)
{
    int c;
    transformer * C;
    for (c = 0; c < T->childcount; c ++)
    {
        C = T->childs[c];
        bake_coordinates(C);
        bake_coordinates_children(C);
    }
}

void bake_coordinates_scale(transformer * T)
{
    float rotVec_I[3][3];
    invert_Rotation_scale(T->parent, rotVec_I);
                // data, matrix, center, result
    rotate_center_(T->pos, rotVec_I, T->parent->pos, T->pos_);
}

void bake_coordinates_children_scale(transformer * T)
{
    int c;
    transformer * C;
    for (c = 0; c < T->childcount; c ++)
    {
        C = T->childs[c];
        bake_coordinates_scale(C);
        bake_coordinates_children_scale(C);
    }
}

void bake_scale(transformer * T)
{
    bake_coordinates_children_scale(T);
}

void bake(transformer * T)
{
    bake_coordinates_children(T);
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

void rotate_matrix_I_4x4(float rotVec_R[4][4], float rotVec_[4][4], float rotVec_I[4][4])
{
    float x_0 = rotVec_I[0][0];
    float y_0 = rotVec_I[0][1];
    float z_0 = rotVec_I[0][2];
    float w_0 = rotVec_I[0][3];
    rotVec_R[0][0] = rotVec_[0][0] * x_0 + rotVec_[1][0] * y_0 + rotVec_[2][0] * z_0 + rotVec_[3][0] * w_0;
    rotVec_R[0][1] = rotVec_[0][1] * x_0 + rotVec_[1][1] * y_0 + rotVec_[2][1] * z_0 + rotVec_[3][1] * w_0;
    rotVec_R[0][2] = rotVec_[0][2] * x_0 + rotVec_[1][2] * y_0 + rotVec_[2][2] * z_0 + rotVec_[3][2] * w_0;
    rotVec_R[0][3] = rotVec_[0][3] * x_0 + rotVec_[1][3] * y_0 + rotVec_[2][3] * z_0 + rotVec_[3][3] * w_0;
    float x_1 = rotVec_I[1][0];
    float y_1 = rotVec_I[1][1];
    float z_1 = rotVec_I[1][2];
    float w_1 = rotVec_I[1][3];
    rotVec_R[1][0] = rotVec_[0][0] * x_1 + rotVec_[1][0] * y_1 + rotVec_[2][0] * z_1 + rotVec_[3][0] * w_1;
    rotVec_R[1][1] = rotVec_[0][1] * x_1 + rotVec_[1][1] * y_1 + rotVec_[2][1] * z_1 + rotVec_[3][1] * w_1;
    rotVec_R[1][2] = rotVec_[0][2] * x_1 + rotVec_[1][2] * y_1 + rotVec_[2][2] * z_1 + rotVec_[3][2] * w_1;
    rotVec_R[1][3] = rotVec_[0][3] * x_1 + rotVec_[1][3] * y_1 + rotVec_[2][3] * z_1 + rotVec_[3][3] * w_1;
    float x_2 = rotVec_I[2][0];
    float y_2 = rotVec_I[2][1];
    float z_2 = rotVec_I[2][2];
    float w_2 = rotVec_I[2][3];
    rotVec_R[2][0] = rotVec_[0][0] * x_2 + rotVec_[1][0] * y_2 + rotVec_[2][0] * z_2 + rotVec_[3][0] * w_2;
    rotVec_R[2][1] = rotVec_[0][1] * x_2 + rotVec_[1][1] * y_2 + rotVec_[2][1] * z_2 + rotVec_[3][1] * w_2;
    rotVec_R[2][2] = rotVec_[0][2] * x_2 + rotVec_[1][2] * y_2 + rotVec_[2][2] * z_2 + rotVec_[3][2] * w_2;
    rotVec_R[2][3] = rotVec_[0][3] * x_2 + rotVec_[1][3] * y_2 + rotVec_[2][3] * z_2 + rotVec_[3][3] * w_2;
    float x_3 = rotVec_I[3][0];
    float y_3 = rotVec_I[3][1];
    float z_3 = rotVec_I[3][2];
    float w_3 = rotVec_I[3][3];
    rotVec_R[3][0] = rotVec_[0][0] * x_3 + rotVec_[1][0] * y_3 + rotVec_[2][0] * z_3 + rotVec_[3][0] * w_3;
    rotVec_R[3][1] = rotVec_[0][1] * x_3 + rotVec_[1][1] * y_3 + rotVec_[2][1] * z_3 + rotVec_[3][1] * w_3;
    rotVec_R[3][2] = rotVec_[0][2] * x_3 + rotVec_[1][2] * y_3 + rotVec_[2][2] * z_3 + rotVec_[3][2] * w_3;
    rotVec_R[3][3] = rotVec_[0][3] * x_3 + rotVec_[1][3] * y_3 + rotVec_[2][3] * z_3 + rotVec_[3][3] * w_3;
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

void normalize_rotation_parent_IK(transformer * T)
{
    float rotVec_I[3][3];
    if (T->parent != NULL)
        invert_Rotation(T->parent, rotVec_I);
    rotate_matrix_I(T->rotVec_I, rotVec_I, T->rotVec_);
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

int insert_trj_keyframe(transformer * T, int frame, float Value)
{
    if (T == NULL)
        return 0;

    timeline * Tm;
    int result, f, index, condition;

    if (T->Timeline == NULL)
    {
        result = init_timeline(T);

        if (result)
        {
            Tm = T->Timeline;
            Tm->Frames = malloc(sizeof(int));
            if (Tm->Frames != NULL)
            {
                Tm->Values = malloc(sizeof(transformer_values));
                Tm->Acceleration = malloc(sizeof(acceleration));
            }
            else
            {
                Tm->key_frames = 0;
                return 0;
            }
            if (Tm->Values == NULL || Tm->Acceleration == NULL)
            {
                Tm->key_frames = 0;
                return 0;
            }
            else
            {
                Tm->key_frames = 1;
                Tm->Frames[0] = frame;
                Tm->Acceleration[0].segment_type = ACCELERATION_DEFAULT;
                Tm->Acceleration[0].a_exponent = ACCELERATION_DEFAULT_OUT;
                Tm->Acceleration[0].b_exponent = ACCELERATION_DEFAULT_IN;
                Tm->Values[0].trj_val = Value;

                return 1;
            }
        }
        else
        {
            return 0;
        }
    }
    else
    {
        Tm = T->Timeline;
        index = 0;
        condition = 0;
        for (f = 0; f < Tm->key_frames; f ++)
        {
            if (Tm->Frames[f] == frame)
            {
                index = f;
                break;
            }
            else if (Tm->Frames[f] > frame)
            {
                index = f;
                condition = 1;
                break;
            }
        }

        if (Tm->Frames[Tm->key_frames - 1] < frame)
        {
            index = Tm->key_frames;
            condition = 1;
        }

        if (condition)
        {
            Tm->key_frames ++;
            Tm->Frames = realloc(Tm->Frames, Tm->key_frames * sizeof(int));
            if (Tm->Frames != NULL)
            {
                Tm->Values = realloc(Tm->Values, Tm->key_frames * sizeof(transformer_values));
                Tm->Acceleration = realloc(Tm->Acceleration, Tm->key_frames * sizeof(acceleration));
            }
            else
            {
                Tm->key_frames = 0;
                return 0;
            }
            if (Tm->Values == NULL || Tm->Acceleration == NULL)
            {
                Tm->key_frames = 0;
                return 0;
            }
            else
            {
                for (f = Tm->key_frames - 1; f > index; f --)
                {
                    Tm->Frames[f] = Tm->Frames[f - 1];
                    memcpy(&Tm->Acceleration[f], &Tm->Acceleration[f - 1], sizeof(acceleration));
                    memcpy(&Tm->Values[f], &Tm->Values[f - 1], sizeof(transformer_values));
                }
                Tm->Frames[index] = frame;
                Tm->Acceleration[index].segment_type = ACCELERATION_DEFAULT;
                Tm->Acceleration[index].a_exponent = ACCELERATION_DEFAULT_OUT;
                Tm->Acceleration[index].b_exponent = ACCELERATION_DEFAULT_IN;
                Tm->Values[index].trj_val = Value;

                return 1;
            }
        }
        else
        {
            Tm->Frames[index] = frame;
            Tm->Acceleration[index].segment_type = ACCELERATION_DEFAULT;
            Tm->Acceleration[index].a_exponent = ACCELERATION_DEFAULT_OUT;
            Tm->Acceleration[index].b_exponent = ACCELERATION_DEFAULT_IN;
            Tm->Values[index].trj_val = Value;

            return 1;
        }
    }
}

int insert_keyframe(transformer * T, int frame, int linear_pose, float Delta[3])
{
    if (T == NULL)
        return 0;

    timeline * Tm;
    int result, f, index, condition;

    float rotVec_I[3][3];

    if (T->Timeline == NULL)
    {
        result = init_timeline(T);

        if (result)
        {
            Tm = T->Timeline;
            Tm->Frames = malloc(sizeof(int));
            if (Tm->Frames != NULL)
            {
                Tm->Values = malloc(sizeof(transformer_values));
                Tm->Acceleration = malloc(sizeof(acceleration));
            }
            else
            {
                Tm->key_frames = 0;
                return 0;
            }
            if (Tm->Values == NULL || Tm->Acceleration == NULL)
            {
                Tm->key_frames = 0;
                return 0;
            }
            else
            {
                Tm->key_frames = 1;
                Tm->Frames[0] = frame;
                Tm->Acceleration[0].segment_type = ACCELERATION_DEFAULT;
                Tm->Acceleration[0].a_exponent = ACCELERATION_DEFAULT_OUT;
                Tm->Acceleration[0].b_exponent = ACCELERATION_DEFAULT_IN;
                memcpy(Tm->Values[0].scl, T->scl, sizeof(float[3]));
                memcpy(Tm->Values[0].rot, T->rot, sizeof(float[3]));

                if (linear_pose || (T->IK != NULL && T == T->IK->B))
                {
                    Tm->Values[0].pos[0] = T->pos[0] - Delta[0];
                    Tm->Values[0].pos[1] = T->pos[1] - Delta[1];
                    Tm->Values[0].pos[2] = T->pos[2] - Delta[2];
                }
                else if (T->parent != NULL)
                {
                    invert_Rotation_scale(T->parent, rotVec_I);
                    rotate_center_(T->pos, rotVec_I, T->parent->pos, Tm->Values[0].pos);
                }

                memcpy(Tm->Values[0].scl_vec, T->scl_vec, sizeof(float[3]));
                memcpy(Tm->Values[0].rotVec_, T->rotVec_, sizeof(float[3][3]));

                return 1;
            }
        }
        else
        {
            return 0;
        }
    }
    else
    {
        Tm = T->Timeline;
        index = 0;
        condition = 0;
        for (f = 0; f < Tm->key_frames; f ++)
        {
            if (Tm->Frames[f] == frame)
            {
                index = f;
                break;
            }
            else if (Tm->Frames[f] > frame)
            {
                index = f;
                condition = 1;
                break;
            }
        }

        if (Tm->Frames[Tm->key_frames - 1] < frame)
        {
            index = Tm->key_frames;
            condition = 1;
        }

        if (condition)
        {
            Tm->key_frames ++;
            Tm->Frames = realloc(Tm->Frames, Tm->key_frames * sizeof(int));
            if (Tm->Frames != NULL)
            {
                Tm->Values = realloc(Tm->Values, Tm->key_frames * sizeof(transformer_values));
                Tm->Acceleration = realloc(Tm->Acceleration, Tm->key_frames * sizeof(acceleration));
            }
            else
            {
                Tm->key_frames = 0;
                return 0;
            }
            if (Tm->Values == NULL || Tm->Acceleration == NULL)
            {
                Tm->key_frames = 0;
                return 0;
            }
            else
            {
                for (f = Tm->key_frames - 1; f > index; f --)
                {
                    Tm->Frames[f] = Tm->Frames[f - 1];
                    memcpy(&Tm->Acceleration[f], &Tm->Acceleration[f - 1], sizeof(acceleration));
                    memcpy(&Tm->Values[f], &Tm->Values[f - 1], sizeof(transformer_values));
                }
                Tm->Frames[index] = frame;
                Tm->Acceleration[index].segment_type = ACCELERATION_DEFAULT;
                Tm->Acceleration[index].a_exponent = ACCELERATION_DEFAULT_OUT;
                Tm->Acceleration[index].b_exponent = ACCELERATION_DEFAULT_IN;
                memcpy(Tm->Values[index].scl, T->scl, sizeof(float[3]));
                memcpy(Tm->Values[index].rot, T->rot, sizeof(float[3]));

                if (linear_pose || (T->IK != NULL && T == T->IK->B))
                {
//                    invert_Rotation_scale(T->IK->A->parent, rotVec_I);
//                    rotate_center_(T->pos, rotVec_I, T->parent->pos, Tm->Values[index].pos);
                    Tm->Values[index].pos[0] = T->pos[0] - Delta[0];
                    Tm->Values[index].pos[1] = T->pos[1] - Delta[1];
                    Tm->Values[index].pos[2] = T->pos[2] - Delta[2];
                }
                else if (T->parent != NULL)
                {
                    invert_Rotation_scale(T->parent, rotVec_I);
                    rotate_center_(T->pos, rotVec_I, T->parent->pos, Tm->Values[index].pos);
                    //memcpy(Tm->Values[index].pos, T->pos, sizeof(float[3]));
                }

                memcpy(Tm->Values[index].scl_vec, T->scl_vec, sizeof(float[3]));
                memcpy(Tm->Values[index].rotVec_, T->rotVec_, sizeof(float[3][3]));
/*
                if (T->parent != NULL)
                {
                    invert_Rotation(T->parent, rotVec_I);
                }
                else
                {
                    invert_Rotation(T, rotVec_I);
                }
                rotate_matrix_I(Tm->Values[index].rotVec_, T->rotVec_, rotVec_I);
*/
                return 1;
            }
        }
        else
        {
            Tm->Frames[index] = frame;
            Tm->Acceleration[index].segment_type = ACCELERATION_DEFAULT;
            Tm->Acceleration[index].a_exponent = ACCELERATION_DEFAULT_OUT;
            Tm->Acceleration[index].b_exponent = ACCELERATION_DEFAULT_IN;
            memcpy(Tm->Values[index].scl, T->scl, sizeof(float[3]));
            memcpy(Tm->Values[index].rot, T->rot, sizeof(float[3]));

            if (linear_pose || (T->IK != NULL && T == T->IK->B))
            {
//                invert_Rotation_scale(T->IK->A->parent, rotVec_I);
//                rotate_center_(T->pos, rotVec_I, T->parent->pos, Tm->Values[index].pos);
                Tm->Values[index].pos[0] = T->pos[0] - Delta[0];
                Tm->Values[index].pos[1] = T->pos[1] - Delta[1];
                Tm->Values[index].pos[2] = T->pos[2] - Delta[2];
            }
            else if (T->parent != NULL)
            {
                invert_Rotation_scale(T->parent, rotVec_I);
                rotate_center_(T->pos, rotVec_I, T->parent->pos, Tm->Values[index].pos);
                //memcpy(Tm->Values[index].pos, T->pos, sizeof(float[3]));
            }

            memcpy(Tm->Values[index].scl_vec, T->scl_vec, sizeof(float[3]));
            memcpy(Tm->Values[index].rotVec_, T->rotVec_, sizeof(float[3][3]));
/*
            if (T->parent != NULL)
            {
                invert_Rotation(T->parent, rotVec_I);
            }
            else
            {
                invert_Rotation(T, rotVec_I);
            }
            rotate_matrix_I(Tm->Values[index].rotVec_, T->rotVec_, rotVec_I);
*/
            return 1;
        }
    }
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
    float D;
    //float D = a / pi_2;

    D = (sin(a) / sin(pi - (a + (pi4)))) / sqrt(2);

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

void cross_Product(float v1[3], float v2[3], float N[3]);
void normalizeF(float * vec);

void pin_down(transformer * T)
{
    if (T->pin == pin_x)
    {
        memcpy(T->rotVec[0], T->rotVec_Pin[0], sizeof(float[3]));
        cross_Product(T->rotVec[0], T->rotVec[1], T->rotVec[2]);
        normalizeF(T->rotVec[2]);
        cross_Product(T->rotVec[2], T->rotVec[0], T->rotVec[1]);
    }
    else if (T->pin == pin_y)
    {
        memcpy(T->rotVec[1], T->rotVec_Pin[1], sizeof(float[3]));
        cross_Product(T->rotVec[1], T->rotVec[2], T->rotVec[0]);
        normalizeF(T->rotVec[0]);
        cross_Product(T->rotVec[0], T->rotVec[1], T->rotVec[2]);
    }
    else if (T->pin == pin_z)
    {
        memcpy(T->rotVec[2], T->rotVec_Pin[2], sizeof(float[3]));
        cross_Product(T->rotVec[2], T->rotVec[0], T->rotVec[1]);
        normalizeF(T->rotVec[1]);
        cross_Product(T->rotVec[1], T->rotVec[2], T->rotVec[0]);
    }
}

void rotate_axis_yxz(transformer * T)
{
    rotate_axis(T->rot[1], T->rotVec_[2], T->rotVec_[0], T->rotVec[2], T->rotVec[0]);
    rotate_axis(T->rot[0], T->rotVec_[1], T->rotVec[2], T->rotVec[1], T->rotVec[2]);
    rotate_axis(T->rot[2], T->rotVec[0], T->rotVec[1], T->rotVec[0], T->rotVec[1]);
    if (T->pin != pin_0)
    {
        pin_down(T);
    }
}

void rotate_axis_zxy(transformer * T)
{
    rotate_axis(T->rot[2], T->rotVec_[0], T->rotVec_[1], T->rotVec[0], T->rotVec[1]);
    rotate_axis(T->rot[0], T->rotVec[1], T->rotVec_[2], T->rotVec[1], T->rotVec[2]);
    rotate_axis(T->rot[1], T->rotVec[2], T->rotVec[0], T->rotVec[2], T->rotVec[0]);
    if (T->pin != pin_0)
    {
        pin_down(T);
    }
}

void rotate_axis_zyx(transformer * T)
{
    rotate_axis(T->rot[2], T->rotVec_[0], T->rotVec_[1], T->rotVec[0], T->rotVec[1]);
    rotate_axis(T->rot[1], T->rotVec_[2], T->rotVec[0], T->rotVec[2], T->rotVec[0]);
    rotate_axis(T->rot[0], T->rotVec[1], T->rotVec[2], T->rotVec[1], T->rotVec[2]);
    if (T->pin != pin_0)
    {
        pin_down(T);
    }
}

void rotate_axis_xyz(transformer * T)
{
    rotate_axis(T->rot[0], T->rotVec_[1], T->rotVec_[2], T->rotVec[1], T->rotVec[2]);
    rotate_axis(T->rot[1], T->rotVec[2], T->rotVec_[0], T->rotVec[2], T->rotVec[0]);
    rotate_axis(T->rot[2], T->rotVec[0], T->rotVec[1], T->rotVec[0], T->rotVec[1]);
    if (T->pin != pin_0)
    {
        pin_down(T);
    }
}

void rotate_axis_xzy(transformer * T)
{
    rotate_axis(T->rot[0], T->rotVec_[1], T->rotVec_[2], T->rotVec[1], T->rotVec[2]);
    rotate_axis(T->rot[2], T->rotVec_[0], T->rotVec[1], T->rotVec[0], T->rotVec[1]);
    rotate_axis(T->rot[1], T->rotVec[2], T->rotVec[0], T->rotVec[2], T->rotVec[0]);
    if (T->pin != pin_0)
    {
        pin_down(T);
    }
}

void rotate_axis_yzx(transformer * T)
{
    rotate_axis(T->rot[1], T->rotVec_[2], T->rotVec_[0], T->rotVec[2], T->rotVec[0]);
    rotate_axis(T->rot[2], T->rotVec[0], T->rotVec_[1], T->rotVec[0], T->rotVec[1]);
    rotate_axis(T->rot[0], T->rotVec[1], T->rotVec[2], T->rotVec[1], T->rotVec[2]);
    if (T->pin != pin_0)
    {
        pin_down(T);
    }
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
//    float rotVec[3][3];
//    float rotVec_[3][3];
//
//    memcpy(rotVec, T->rotVec, sizeof(rotVec));
//    memcpy(rotVec_, T->rotVec_, sizeof(rotVec_));

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

//    memcpy(T->rotVec, rotVec, sizeof(T->rotVec));
//    memcpy(T->rotVec_, rotVec_, sizeof(T->rotVec_));
}

void synthesize_Bone_Axis(bone * B, float scale[3]);

void update_Spine(ikChain * I);

void rotate_T(transformer * T)
{
    if (T->Bone != NULL && T->Bone->IK_member > 0)
    {

    }
    else if (T->IK != NULL && T->style == ik_fixed && !T->IK->update)
    {
        //update_Spine(T->IK);
    }
    else
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

//    if (T->Bone != NULL && T == T->Bone->A)
//    {
//        synthesize_Bone_Axis(T->Bone, T->scl_vec);
//    }
}

void rotate_vertex_groups(transformer * T);

void rotate_children_M(transformer * T)
{
    int c;
    transformer * C;

    for (c = 0; c < T->childcount; c ++)
    {
        C = T->childs[c];
        if (C->Bone != NULL && C->Bone->IK_member > 0)
        {

        }
        else if (C->IK != NULL && C->style == ik_fixed && !C->IK->update)
        {
            //update_Spine(C->IK);
        }
        else
        {
            if (C->IK != NULL && C->style == ik_fixed && C->IK->update)
            {

            }

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
        }

        //rotate_vertex_groups(C);
        if (C->IK != NULL && C->style != ik_start)
        {

        }
        else
        {
            rotate_children_M(C);
        }
    }
}

void rotate_children_(transformer * T)
{
    int c;
    transformer * C;

    for (c = 0; c < T->childcount; c ++)
    {
        C = T->childs[c];
        if (C->Bone != NULL && C->Bone->IK_member > 0)
        {

        }
        else if (C->IK != NULL && C->style == ik_fixed && !C->IK->update)
        {
            //update_Spine(C->IK);
        }
        else
        {

            if (C->IK != NULL && C->style == ik_fixed && C->IK->update)
            {
                if (!C->IK->stretch)
                {
                    project_IK_goal_To_Spine(C->IK);
                }
            }
            else
            {
                if (SCALE)
                    rotate_center_1(C->pos_, C->parent->rotVec, C->parent->pos, C->pos);
                else
                    rotate_center_1(C->pos_, C->parent->rotVec_, C->parent->pos, C->pos);
                          // data, matrix, center, result
            }

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
        }

        //rotate_vertex_groups(C);
        if (C->IK != NULL && C->style == ik_fixed && !C->IK->update)
        {

        }
        else
        {
            rotate_children_(C);
        }
    }
}

void rotate_children_P(transformer * T)
{
    int c;
    transformer * C;

    for (c = 0; c < T->childcount; c ++)
    {
        C = T->childs[c];

        rotate_vertex_groups(C);

        rotate_children_P(C);
    }
}

void rotate_children(transformer * T)
{
    int c;
    transformer * C;

    for (c = 0; c < T->childcount; c ++)
    {
        C = T->childs[c];
        if (C->Bone != NULL && C->Bone->IK_member > 0)
        {

        }
        else if (C->IK != NULL && C->style == ik_fixed && !C->IK->update)
        {
            //update_Spine(C->IK);
        }
        else
        {
            if (C->IK != NULL && C->style == ik_fixed && C->IK->update)
            {

            }
            else
            {
                if (SCALE)
                    rotate_center_1(C->pos_, C->parent->rotVec, C->parent->pos, C->pos);
                else
                    rotate_center_1(C->pos_, C->parent->rotVec_, C->parent->pos, C->pos);
                              // data, matrix, center, result
            }
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
        }

//        if (C->Bone != NULL && C == C->Bone->A)
//        {
//            synthesize_Bone_Axis(C->Bone, C->scl_vec);
//        }

        rotate_vertex_groups(C);
        if (C->IK != NULL && C->style == ik_fixed && !C->IK->update)
        {

        }
        else
        {
            rotate_children(C);
        }
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

void rotate_children_scale(transformer * T, float pos[3], float rotVec_[3][3])
{
    int c;
    transformer * C;

    for (c = 0; c < T->childcount; c ++)
    {
        C = T->childs[c];
        rotate_center(C->pos_, rotVec_, pos, C->pos);
                      // data, matrix, center, result

        rotate_children_scale(C, pos, rotVec_);
    }
}

void rotate_scale(transformer * T)
{
    float rotVec[3][3];

    scale_rotVec(rotVec, T->rotVec_, T->scl);

    rotate_children_scale(T, T->pos, rotVec);
}

void rotate_M(transformer * T)
{
    if (T->Bone != NULL && T->Bone->IK_member > 0)
    {

    }
    else if (T->IK != NULL && T->style == ik_fixed && !T->IK->update)
    {

    }
    else
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
    }

    //rotate_vertex_groups(T);

//    float rotVec[3][3];
//
//    scale_rotVec(rotVec, T->rotVec_, T->scl);
//    rotate_children_(T, T->pos, rotVec);

    if (T->IK != NULL && T->style == ik_fixed && !T->IK->update)
    {

    }
    else
    {
        rotate_children_M(T);
    }
}

void rotate_(transformer * T)
{
    if (T->Bone != NULL && T->Bone->IK_member > 0)
    {

    }
    else if (T->IK != NULL && T->style == ik_fixed && !T->IK->update)
    {
        //update_Spine(T->IK);
    }
    else
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
    }

    //rotate_vertex_groups(T);

//    float rotVec[3][3];
//
//    scale_rotVec(rotVec, T->rotVec_, T->scl);
//    rotate_children_(T, T->pos, rotVec);
    if (T->IK != NULL && T->style == ik_fixed && !T->IK->update)
    {

    }
    else
    {
        rotate_children_(T);
    }
}

void rotate_P(transformer * T)
{
    rotate_vertex_groups(T);

    rotate_children_P(T);
}

void rotate(transformer * T)
{
    if (T->Bone != NULL && T->Bone->IK_member > 0)
    {

    }
    else if (T->IK != NULL && T->style == ik_fixed && !T->IK->update)
    {
        //update_Spine(T->IK);
    }
    else
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
    }

//    if (T->Bone != NULL && T == T->Bone->A)
//    {
//        synthesize_Bone_Axis(T->Bone, T->scl_vec);
//    }

    rotate_vertex_groups(T);
    if (T->IK != NULL && T->style == ik_fixed && !T->IK->update)
    {

    }
    else
    {
        rotate_children(T);
    }
}

void rotate_hierarchy_T(transformer * P, transformer * T)
{
    int c;
    transformer * C;
    rotate_T(P);
    //rotate_vertex_groups(P);
    for (c = 0; c < P->childcount; c ++)
    {
        C = P->childs[c];
        if (C == T)
        {
            continue;
        }

        if (C->IK != NULL && C->style == ik_fixed && !C->IK->update)
        {

        }
        else
        {
            rotate_hierarchy_T(C, T);
        }
    }
}

void collect_selected_T(transformer * T)
{
    int c;
    transformer * C;

    if (T->selected)
        child_collection[child_collection_count ++] = T;

    for (c = 0; c < T->childcount; c ++)
    {
        C = T->childs[c];
        collect_selected_T(C);
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

void rotate_vertex_groups_D_Init();
void solve_IK_Chain(ikChain * I);
void solve_IK_Chains(deformer * D);
void rotate_Deformer_verts(deformer * D);
void paste_Action_Begin();

void paste_Deformer(transformer * T)
{
    transformer * P = T->parent;

    while (P != NULL && P->Deformer == T->Deformer)
    {
        P = P->parent;
    }

    //rotate_collect(P);
    rotate_vertex_groups_D_Init();

    rotate_Deformer_verts(T->Deformer);
}

void rotate_Deformer(transformer * T)
{
//    transformer * P = T->parent;
//
//    while (P != NULL && P->Deformer == T->Deformer)
//    {
//        P = P->parent;
//    }

    //rotate_collect(P);
    rotate_vertex_groups_D_Init();

//    rotate_hierarchy_T(P, T);

    rotate_(T);

    solve_IK_Chains(T->Deformer); // specify affected IK chains before, collect them

    rotate_Deformer_verts(T->Deformer);
}

void rotate_Deformer_pose(transformer * T)
{
    transformer * P = T->parent;

    if (P != NULL && P->Deformer != T->Deformer)
    {
        P = T;
    }
    else
    {
        while (P != NULL && P->Deformer == T->Deformer)
        {
            P = P->parent;
        }
    }

    SCALE = 1;

    bake(P);
    rotate_(P);

    solve_IK_Chains(T->Deformer);

    SCALE = 0;
}

void transfer_rotVec_B(transformer * T)
{
    float rotVec_I[3][3];
    invert_Rotation(T, rotVec_I);
    memcpy(T->rotVec_B, rotVec_I, sizeof(T->rotVec_B));

    if (T->Bone != NULL && T == T->Bone->A)
    {
        memcpy(T->Bone->rotVec_, T->rotVec_, sizeof(T->Bone->rotVec_));
    }

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

        memcpy(T->rotVec_, T->rotVec, sizeof T->rotVec_);
    }
    else if (T == T->Bone->A)
    {
        memcpy(T->rotVec_, T->Bone->rotVec_, sizeof(T->rotVec_));

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

        memcpy(T->rotVec_, T->rotVec, sizeof T->rotVec_);

        memcpy(T->Bone->B->rotVec, T->rotVec, sizeof(T->Bone->B->rotVec));
        memcpy(T->Bone->B->rotVec_, T->rotVec_, sizeof(T->Bone->B->rotVec_));
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

void move_IK(transformer * T, float Delta[3])
{
    T->pos[0] += Delta[0];
    T->pos[1] += Delta[1];
    T->pos[2] += Delta[2];

    //rotate_vertex_groups(T);
}

void normalizeF(float * vec);

void paste_rotVec()
{
    int t;
    transformer * T;

    for (t = 0; t < transformerIndex; t ++)
    {
        T = transformers[t];
        memcpy(T->rotVec, T->rotVec_, sizeof(T->rotVec));
        if (T->Bone != NULL)
        {
            memcpy(T->Bone->rotVec_, T->rotVec_, sizeof(T->Bone->rotVec_));
        }
    }
}

void paste_rotVec_()
{
    int t;
    transformer * T;

    for (t = 0; t < transformerIndex; t ++)
    {
        T = transformers[t];
        memcpy(T->rotVec_, T->rotVec, sizeof(T->rotVec_));
        if (T->Bone != NULL)
        {
            memcpy(T->Bone->rotVec_, T->rotVec, sizeof(T->Bone->rotVec_));
        }
    }
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
        T->scl_vec[0] = 1;
        T->scl_vec[1] = 1;
        T->scl_vec[2] = 1;
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

void move_C_IK(transformer * T, float Delta[3])
{
    int c;
    transformer * C;

    if (T->IK != NULL && T == T->IK->B)
    {

    }
    else
    {
        T->pos[0] += Delta[0];
        T->pos[1] += Delta[1];
        T->pos[2] += Delta[2];
    }

    for (c = 0; c < T->childcount; c ++)
    {
        C = T->childs[c];
        if (C->IK != NULL && C->style == ik_fixed)
        {

        }
        else
        {
            move_C_IK(C, Delta);
        }
    }
}

void move_Children_IK(transformer * T, float Delta[3])
{
    int c;
    transformer * C;

    for (c = 0; c < child_collection_count; c ++)
    {
        C = child_collection[c];
        move_IK(C, Delta);
    }
}

void move_Children(transformer * T, float Delta[3])
{
    int c;
    transformer * C;

    for (c = 0; c < child_collection_count; c ++)
    {
        C = child_collection[c];
        move_T(C, Delta);
    }
}

void move_Pose_T(transformer * T, float Delta[3])
{
    T->pos[0] += Delta[0];
    T->pos[1] += Delta[1];
    T->pos[2] += Delta[2];

    int c;
    transformer * C;

    for (c = 0; c < T->childcount; c ++)
    {
        C = T->childs[c];
        move_Pose_T(C, Delta);
    }
}

void move_H(transformer * T, float Delta[3])
{
    int c;
    transformer * C;

    for (c = 0; c < T->childcount; c ++)
    {
        C = T->childs[c];

        if (C->Bone != NULL && C->Bone->IK_member > 0)
        {
            move_H(C, Delta);
        }
        else if (C->IK != NULL && C->style == ik_start)
        {
            C->pos[0] = C->parent->pos[0];
            C->pos[1] = C->parent->pos[1];
            C->pos[2] = C->parent->pos[2];
            move_H(C, Delta);
        }
        else if (C->IK != NULL && C->style == ik_fixed)
        {
            if (C->IK->update && !C->IK->stretch)
            {
                project_IK_goal_To_Spine(C->IK);
            }
        }
        else
        {
            move_T(C, Delta);
            move_H(C, Delta);
        }
    }
}

void move_T_Constraint(transformer * T, float Delta[3]);

void move_H_Constraint(transformer * T, float Delta[3])
{
    int c;
    transformer * C, * C0;

    for (c = 0; c < T->childcount; c ++)
    {
        C = T->childs[c];

        if (C->Constraint != NULL)
        {
            C0 = C->Constraint->IK_goal;

            if (C0->IK != NULL && C0->style == ik_fixed)
            {
                if (C0->IK->update && !C0->IK->stretch)
                {
                    project_IK_goal_To_Spine(C0->IK);
                }
            }
        }

        move_T_Constraint(C, Delta);
        move_H_Constraint(C, Delta);
    }
}

void rotate_Constraint(transformer * T)
{
    int c;
    transformer * C, * C0;

    if (T->Constraint != NULL)
    {
        C0 = T->Constraint->IK_goal;

        if (C0->IK != NULL && C0->style == ik_fixed)
        {
            if (C0->IK->update && !C0->IK->stretch)
            {
                project_IK_goal_To_Spine(C0->IK);
            }
        }
    }

    for (c = 0; c < T->childcount; c ++)
    {
        C = T->childs[c];
        rotate_Constraint(C);
    }
}

void move_T_Constraint(transformer * T, float Delta[3])
{
    transformer * C0;

    if (T->Constraint != NULL)
    {
        C0 = T->Constraint->IK_goal;

        if (C0->IK != NULL && C0->style == ik_fixed)
        {
            if (C0->IK->update && !C0->IK->stretch)
            {
                project_IK_goal_To_Spine(C0->IK);
            }
        }
    }

    move_T(T, Delta);
}

void rotate_Deformer_Constraint(transformer * T)
{
    rotate_vertex_groups_D_Init();

    rotate_(T);

    rotate_Constraint(T);

    solve_IK_Chains(Constraint_Pack.IK->Deformer);

    rotate_Deformer_verts(Constraint_Pack.IK->Deformer);
}

void move_Deformer_Constraint(transformer * T, float Delta[3])
{
    rotate_vertex_groups_D_Init();

    move_T_Constraint(T, Delta);
    move_H_Constraint(T, Delta);

    solve_IK_Chains(Constraint_Pack.IK->Deformer);

    rotate_Deformer_verts(Constraint_Pack.IK->Deformer);
}

void move_Deformer(transformer * T, float Delta[3])
{
    rotate_vertex_groups_D_Init();

    move_T(T, Delta);
    move_H(T, Delta);

    solve_IK_Chains(T->Deformer);

    rotate_Deformer_verts(T->Deformer);
}

void update_rotate_bounding_box();

void move_Constraint(transformer * T, float Delta[3])
{
    if (Constraint_Pack.IK->Deformer != NULL)
    {
        move_Deformer_Constraint(T, Delta);
    }
    else
    {
        rotate_vertex_groups_D_Init();
        move_T(T, Delta);
        move_Children(T, Delta);
    }
    update_rotate_bounding_box();
}

void move_Objects_To_Delta(float Delta[3]);

void move_(transformer * T, float Delta[3])
{
    if (T->Deformer != NULL)
    {
        move_Deformer(T, Delta);
    }
    else
    {
        rotate_vertex_groups_D_Init();
        move_T(T, Delta);
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

    for (c = 0; c < child_collection_count; c ++)
    {
        C = child_collection[c];
        bake_pose(C);
    }
}
