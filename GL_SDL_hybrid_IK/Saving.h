/*
The MIT License

Copyright <2018> <Allan Kiipli>
*/

#ifndef SAVING_H_INCLUDED
#define SAVING_H_INCLUDED

int NIGHT = 0;
int SHADOWS = 0;
float Reflect = 1;
int currentTheme = 0;

char scene_extension[20];
char * scene_extensions[SCENE_EXT_NUM];
int scene_ext_count = 6;
int scene_files_start = 0;

char scene_files_dir[STRLEN];

void init_scene_extensions()
{
    int i;
    for (i = 0; i < scene_ext_count; i ++)
    {
        scene_extensions[i] = malloc(20 * sizeof(char));
    }
    sprintf(scene_extensions[0], "Objects");
    sprintf(scene_extensions[1], "Selections");
    sprintf(scene_extensions[2], "Deformers");
    sprintf(scene_extensions[3], "Hierarchys");
    sprintf(scene_extensions[4], "Poses");
    sprintf(scene_extensions[5], "Surfaces");

    sprintf(scene_files_dir, "%s", "c:/Trips Code/Scenes IK");

    sprintf(scene_extension, "%s", "");
}

int save_Surfaces(char * surfaces_files_dir)
{
    char dirfile[STRLEN];

    unsigned objects_address[objectIndex];

    int objects_counter;

    int o, m;

    object * O;
    surface_Material * M;

    dirfile[0] = '\0';
    strcat(dirfile, surfaces_files_dir);
    strcat(dirfile, "/");
    strcat(dirfile, "Materials");
    strcat(dirfile, ".txt");

    FILE * F;
    F = fopen(dirfile, "w");
    if (F == NULL) return 0;
    fprintf(F, "Materials\n");
    fprintf(F, "%d\n", Materials_count - 4); // Default, Green, Blue, Yellow skip

    for (m = 4; m < Materials_count; m ++)
    {
        M = &Materials[m];
        fprintf(F, "%s\n", M->Name);
        fprintf(F, "%s\n", Text_Names_Full[M->texture]);
        fprintf(F, "%s\n", Norm_Names_Full[M->normal]);
        fprintf(F, "%s\n", Bump_Names_Full[M->bump]);
        fprintf(F, "%d %d %d %d\n", M->smooth, M->texture, M->Texture_idx, M->use_texture);
        fprintf(F, "%f %f %f %f %f %d %d %d %d %f\n", M->R, M->G, M->B, M->A, M->Shininess, M->normal, M->Normal_idx, M->bump, M->Bump_idx, M->Displacement);

        objects_counter = 0;

        for (o = 1; o < objectIndex; o ++) /*CUBECOMMENT*/
        {
            O = objects[o];
            if (O->surface == M->index)
            {
                objects_address[objects_counter ++] = (unsigned)O;
            }
        }

        fprintf(F, "%d\n", objects_counter);

        for (o = 0; o < objects_counter; o ++)
        {
            fprintf(F, "%u\n", objects_address[o]);
        }
        /*
        int index;
        int smooth;
        char Name[STRLEN];
        int texture;
        int Texture_idx;
        int use_texture;
        float R, G, B, A;*/
    }

    fprintf(F, "\n");
    fclose(F);
    return 1;
}

void save_Objects(char * object_files_dir)
{
    char dirfile[STRLEN];

    int o;

    char n[8];

    object * O;

    for (o = 1; o < objectIndex; o ++) /*CUBECOMMENT*/
    {
        O = objects[o];

        dirfile[0] = '\0';
        strcat(dirfile, object_files_dir);
        strcat(dirfile, "/");

        strcat(dirfile, O->Name);
        strcat(dirfile, "_");
        sprintf(n, "%d", o);
        strcat(dirfile, n);
        strcat(dirfile, ".txt");

        FILE * F;
        F = fopen(dirfile, "w");
        if (F == NULL) continue;
        fprintf(F, "Object\n");
        fprintf(F, "%s\n", O->Name);
        fprintf(F, "%u\n", (unsigned)O);
        fprintf(F, "%d ", O->textcount);
        fprintf(F, "%d ", O->vertcount);
        fprintf(F, "%d ", O->icolcount);
        fprintf(F, "%d ", O->tripcount);
        fprintf(F, "%d ", O->edgecount);
        fprintf(F, "%d ", O->uvedcount);
        fprintf(F, "%d\n", O->polycount);

        fprintf(F, "%d %d\n", O->deforms, O->binding);

//        fprintf(F, "%f %f %f\n", O->T.rot[0], O->T.rot[1], O->T.rot[2]);
//        fprintf(F, "%f %f %f\n", O->T.pos[0], O->T.pos[1], O->T.pos[2]);

        int i;
        uv * UV;
        vertex * V;
//        triangle * T;
        edge * E;
        uv_edge * UVE;
        polygon * P;

        for(i = 0; i < O->textcount; i ++)
        {
            UV = &O->uvtex[i / ARRAYSIZE][i % ARRAYSIZE];
            print_uv_f(F, UV);
        }
        for(i = 0; i < O->vertcount; i ++)
        {
            V = &O->verts[i / ARRAYSIZE][i % ARRAYSIZE];
            print_vertex_f(F, V);
        }
//        for(i = 0; i < O->tripcount; i ++)
//        {
//            T = &O->trips[i / ARRAYSIZE][i % ARRAYSIZE];
//            print_triangle_f(F, T);
//        }
        for(i = 0; i < O->edgecount; i ++)
        {
            E = &O->edges[i / ARRAYSIZE][i % ARRAYSIZE];
            print_edge_f(F, E);
        }
        for(i = 0; i < O->uvedcount; i ++)
        {
            UVE = &O->uveds[i / ARRAYSIZE][i % ARRAYSIZE];
            print_uv_edge_f(F, UVE);
        }
        for(i = 0; i < O->polycount; i ++)
        {
            P = &O->polys[i / ARRAYSIZE][i % ARRAYSIZE];
            print_polygon_f(F, P);
        }
        fprintf(F, "\n");
        fclose(F);
    }
}

void save_Selections(char * selection_files_dir)
{
    char dirfile[STRLEN];

    int i, o, s;

    object * O;

    selection * S;
    vert_selection * VS;

//    typedef struct
//    {
//        int index;
//        object * Object;
//        char * Name;
//        int * indices;
//        int indices_count;
//    }
//    selection;
//
//    typedef struct
//    {
//        int index;
//        object * Object;
//        transformer * Transformer;
//        char * Name;
//        int * indices;
//        float * weights;
//        int indices_count;
//    }
//    vert_selection;

    char n[8];

    for (o = 1; o < objectIndex; o ++) /*CUBECOMMENT*/
    {
        O = objects[o];

        if (O->polygon_selections || O->edge_selections || O->vertex_selections)
        {
            dirfile[0] = '\0';
            strcat(dirfile, selection_files_dir);
            strcat(dirfile, "/");
            strcat(dirfile, O->Name);
            strcat(dirfile, "_");
            sprintf(n, "%d", o);
            strcat(dirfile, n);
            strcat(dirfile, ".txt");

            FILE * F;
            F = fopen(dirfile, "w");

            if (F == NULL) continue;

            fprintf(F, "Selections\n");
            fprintf(F, "%u\n", (unsigned)O);
            fprintf(F, "%d %d %d\n", O->polygon_selections, O->edge_selections, O->vertex_selections);

            if (O->polygon_selections)
            {
                fprintf(F, "polygon\n");
                for (s = 0; s < O->polygon_selections; s ++)
                {
                    S = O->polygon_selection[s];
                    fprintf(F, "%s\n", S->Name);
                    fprintf(F, "%d\n", S->indices_count);
                    for (i = 0; i < S->indices_count; i ++)
                    {
                        fprintf(F, "%d\n", S->indices[i]);
                    }
                }
            }
            if (O->edge_selections)
            {
                fprintf(F, "edge\n");
                for (s = 0; s < O->edge_selections; s ++)
                {
                    S = O->edge_selection[s];
                    fprintf(F, "%s\n", S->Name);
                    fprintf(F, "%d\n", S->indices_count);
                    for (i = 0; i < S->indices_count; i ++)
                    {
                        fprintf(F, "%d\n", S->indices[i]);
                    }
                }
            }
            if (O->vertex_selections)
            {
                fprintf(F, "vertex\n");
                for (s = 0; s < O->vertex_selections; s ++)
                {
                    VS = O->vertex_selection[s];
                    fprintf(F, "%s\n", VS->Name);
                    fprintf(F, "%u\n", (unsigned)VS);
                    fprintf(F, "%u\n", (unsigned)VS->Transformer);
                    fprintf(F, "%d\n", VS->indices_count);
                    for (i = 0; i < VS->indices_count; i ++)
                    {
                        fprintf(F, "%d %f\n", VS->indices[i], VS->weights[i]);
                    }
                }
            }
            fprintf(F, "\n");
            fclose(F);
        }
    }
}

void save_Deformers(char * deformers_files_dir)
{
    char dirfile[STRLEN];

    int i, d;

    deformer * D;

/*
    int index;
    unsigned address;
    char * Name;
    int collapsed;
    int selected;

    transformer ** Transformers;
    int Transformers_Count;

    vert_selection ** Selections;
    int Selections_Count;

    object ** Objects;
    int Objects_Count;

    bone ** Bones;
    int Bones_Count;

    pose ** Poses;
    int Poses_Count;

    ikChain ** IKchains;
    int IKchains_Count;
*/

    char n[8];

    for (d = 0; d < deformerIndex; d ++)
    {
        D = deformers[d];

        dirfile[0] = '\0';
        strcat(dirfile, deformers_files_dir);
        strcat(dirfile, "/");
        strcat(dirfile, D->Name);
        strcat(dirfile, "_");
        sprintf(n, "%d", d);
        strcat(dirfile, n);
        strcat(dirfile, ".txt");

        FILE * F;
        F = fopen(dirfile, "w");

        if (F == NULL) continue;

        fprintf(F, "Deformer\n");
        fprintf(F, "%s\n", D->Name);
        fprintf(F, "%u\n", (unsigned)D);
        fprintf(F, "%d %d %d %d %d %d %d\n",
                D->collapsed,
                D->Transformers_Count,
                D->Selections_Count,
                D->Objects_Count,
                D->Bones_Count,
                D->Poses_Count,
                D->IKchains_Count);
        for (i = 0; i < D->Transformers_Count; i ++)
        {
            fprintf(F, "%u\n", (unsigned)D->Transformers[i]);
        }
        for (i = 0; i < D->Selections_Count; i ++)
        {
            fprintf(F, "%u\n", (unsigned)D->Selections[i]);
        }
        for (i = 0; i < D->Objects_Count; i ++)
        {
            fprintf(F, "%u\n", (unsigned)D->Objects[i]);
        }
        for (i = 0; i < D->Bones_Count; i ++)
        {
            fprintf(F, "%u\n", (unsigned)D->Bones[i]);
        }
        for (i = 0; i < D->Poses_Count; i ++)
        {
            fprintf(F, "%u\n", (unsigned)D->Poses[i]);
        }
        for (i = 0; i < D->IKchains_Count; i ++)
        {
            fprintf(F, "%u\n", (unsigned)D->IKchains[i]);
        }

        fprintf(F, "%d\n", D->current_pose);

        fclose(F);
    }
}

int save_Hierarchys(char * hierarchys_files_dir, float CamDist)
{
    int i, t;

    transformer * T;

    char dirfile[STRLEN];

    dirfile[0] = '\0';
    strcat(dirfile, hierarchys_files_dir);
    strcat(dirfile, "/");
    strcat(dirfile, "World");
    strcat(dirfile, ".txt");

    FILE * F;
    F = fopen(dirfile, "w");

    if (F == NULL) return 0;

    fprintf(F, "World\n");
    fprintf(F, "%u\n", (unsigned)&World);
    fprintf(F, "%f %f %d %d %d", LocatorSize, Reflect, currentTheme, NIGHT, SHADOWS);

    fprintf(F, "\n");
    fclose(F);

    dirfile[0] = '\0';
    strcat(dirfile, hierarchys_files_dir);
    strcat(dirfile, "/");
    strcat(dirfile, "Cameras");
    strcat(dirfile, ".txt");

    F = fopen(dirfile, "w");

    if (F == NULL) return 0;

    fprintf(F, "Cameras\n");
    fprintf(F, "%f\n", CamDist);
    fprintf(F, "%f\n", Camera_Persp.h_view);
    fprintf(F, "%f %f %f\n", Camera_Persp.T->rot[0], Camera_Persp.T->rot[1], Camera_Persp.T->rot[2]);
    fprintf(F, "%f %f %f\n", Camera_Persp.T->pos[0], Camera_Persp.T->pos[1], Camera_Persp.T->pos[2]);
    fprintf(F, "%f %f %f\n", Camera_Persp.T->target[0], Camera_Persp.T->target[1], Camera_Persp.T->target[2]);

    fprintf(F, "\n");
    fclose(F);

    dirfile[0] = '\0';
    strcat(dirfile, hierarchys_files_dir);
    strcat(dirfile, "/");
    strcat(dirfile, "Locators");
    strcat(dirfile, ".txt");

    F = fopen(dirfile, "w");

    if (F == NULL) return 0;

    fprintf(F, "Transformers\n");
    fprintf(F, "%d\n", transformerIndex - 6); /*CUBECOMMENT*/

    for (t = 6; t < transformerIndex; t ++) /*CUBECOMMENT*/
    {
        //T = Locators[l];
        T = transformers[t];

        fprintf(F, "%s\n", T->Name);
        fprintf(F, "%u\n", (unsigned)T);
        fprintf(F, "%d %d %d %d %d %d\n",
                T->style,
                T->childcount,
                T->collapsed,
                T->rot_Order,
                T->bind_set,
                T->Selections_Count);
        fprintf(F, "%u %u %u %u %u\n",
                (unsigned)T->parent,
                (unsigned)T->Object,
                (unsigned)T->Deformer,
                (unsigned)T->Bone,
                (unsigned)T->IK);

        fprintf(F, "%f %f %f\n", T->scl[0], T->scl[1], T->scl[2]);
        fprintf(F, "%f %f %f\n", T->scl_vec[0], T->scl_vec[1], T->scl_vec[2]);
        fprintf(F, "%f %f %f\n", T->rot[0], T->rot[1], T->rot[2]);
        fprintf(F, "%f %f %f\n", T->rot_bind[0], T->rot_bind[1], T->rot_bind[2]);
        //fprintf(F, "%f %f %f\n", T->rot_deform[0], T->rot_deform[1], T->rot_deform[2]);
        fprintf(F, "%f %f %f %f %f %f %f %f %f\n",
                T->rotVec[0][0], T->rotVec[0][1], T->rotVec[0][2],
                T->rotVec[1][0], T->rotVec[1][1], T->rotVec[1][2],
                T->rotVec[2][0], T->rotVec[2][1], T->rotVec[2][2]);
        fprintf(F, "%f %f %f %f %f %f %f %f %f\n",
                T->rotVec_[0][0], T->rotVec_[0][1], T->rotVec_[0][2],
                T->rotVec_[1][0], T->rotVec_[1][1], T->rotVec_[1][2],
                T->rotVec_[2][0], T->rotVec_[2][1], T->rotVec_[2][2]);
        fprintf(F, "%f %f %f %f %f %f %f %f %f\n",
                T->rotVec_I[0][0], T->rotVec_I[0][1], T->rotVec_I[0][2],
                T->rotVec_I[1][0], T->rotVec_I[1][1], T->rotVec_I[1][2],
                T->rotVec_I[2][0], T->rotVec_I[2][1], T->rotVec_I[2][2]);
        fprintf(F, "%f %f %f %f %f %f %f %f %f\n",
                T->rotVec_B[0][0], T->rotVec_B[0][1], T->rotVec_B[0][2],
                T->rotVec_B[1][0], T->rotVec_B[1][1], T->rotVec_B[1][2],
                T->rotVec_B[2][0], T->rotVec_B[2][1], T->rotVec_B[2][2]);
        fprintf(F, "%f %f %f\n", T->pos[0], T->pos[1], T->pos[2]);
        fprintf(F, "%f %f %f\n", T->pos_[0], T->pos_[1], T->pos_[2]);
        //fprintf(F, "%f %f %f\n", T->Pos_[0], T->Pos_[1], T->Pos_[2]);
        fprintf(F, "%f %f %f\n", T->pos_bind[0], T->pos_bind[1], T->pos_bind[2]);
        //fprintf(F, "%f %f %f\n", T->pos_deform[0], T->pos_deform[1], T->pos_deform[2]);
        fprintf(F, "%f %f %f\n", T->target[0], T->target[1], T->target[2]);

        for (i = 0; i < T->childcount; i ++)
        {
            fprintf(F, "%u\n", (unsigned)T->childs[i]);
        }

        for (i = 0; i < T->Selections_Count; i ++)
        {
            fprintf(F, "%u\n", (unsigned)T->Selections[i]);
        }

        /*
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
    //    float rotVec_O[3][3];
        float pos[3];
        float pos_[3];
        float Pos_[3];
        float pos_bind[3];
        float pos_deform[3];
        float target[3];
        float * aim;  //rotVec_[aim_vector]
        */
    }

    fprintf(F, "\n");
    fclose(F);

    dirfile[0] = '\0';
    strcat(dirfile, hierarchys_files_dir);
    strcat(dirfile, "/");
    strcat(dirfile, "Bones");
    strcat(dirfile, ".txt");

    F = fopen(dirfile, "w");

    if (F == NULL) return 0;

    fprintf(F, "Bones\n");
    fprintf(F, "%d\n", bonesIndex);

    int b;

    bone * B;

    for (b = 0; b < bonesIndex; b ++)
    {
        B = bones[b];

        fprintf(F, "%s\n", B->Name);
        fprintf(F, "%u\n", (unsigned)B);
        fprintf(F, "%u\n", (unsigned)B->parent);
        fprintf(F, "%f %f %f %f %f %f %f %f %f\n",
                B->rotVec_[0][0], B->rotVec_[0][1], B->rotVec_[0][2],
                B->rotVec_[1][0], B->rotVec_[1][1], B->rotVec_[1][2],
                B->rotVec_[2][0], B->rotVec_[2][1], B->rotVec_[2][2]);
        fprintf(F, "%f %f %f %f %f %f %f %f %f\n",
                B->rotVec_I[0][0], B->rotVec_I[0][1], B->rotVec_I[0][2],
                B->rotVec_I[1][0], B->rotVec_I[1][1], B->rotVec_I[1][2],
                B->rotVec_I[2][0], B->rotVec_I[2][1], B->rotVec_I[2][2]);
        fprintf(F, "%u %u %u\n", (unsigned)B->A, (unsigned)B->B, (unsigned)B->D);
        fprintf(F, "%d %f\n", B->IK_member, B->len);
        fprintf(F, "%u\n", (unsigned)B->IK);
    }

    fprintf(F, "\n");
    fclose(F);

    dirfile[0] = '\0';
    strcat(dirfile, hierarchys_files_dir);
    strcat(dirfile, "/");
    strcat(dirfile, "ikChains");
    strcat(dirfile, ".txt");

    F = fopen(dirfile, "w");

    if (F == NULL) return 0;

    fprintf(F, "ikChains\n");
    fprintf(F, "%d\n", iksIndex);

    ikChain * I;

    for (i = 0; i < iksIndex; i ++)
    {
        I = ikChains[i];

        fprintf(F, "%s\n", I->Name);
        fprintf(F, "%u\n", (unsigned)I);
        fprintf(F, "%u\n", (unsigned)I->Deformer);
        fprintf(F, "%d\n", I->bonescount);
        for (b = 0; b < I->bonescount; b ++)
        {
            fprintf(F, "%u\n", (unsigned)I->Bones[b]);
        }
        fprintf(F, "%f\n", I->sum_length);
        for (b = 0; b < I->bonescount; b ++)
        {
            fprintf(F, "%f %f %f\n", I->vectors[b].vec[0], I->vectors[b].vec[1], I->vectors[b].vec[2]);
            fprintf(F, "%f %f %f\n", I->vectors_bone[b].vec[0], I->vectors_bone[b].vec[1], I->vectors_bone[b].vec[2]);
            fprintf(F, "%f %f %f\n", I->positions_A[b].vec[0], I->positions_A[b].vec[1], I->positions_A[b].vec[2]);
            fprintf(F, "%f %f %f\n", I->positions_B[b].vec[0], I->positions_B[b].vec[1], I->positions_B[b].vec[2]);
            fprintf(F, "%f %f %f\n", I->bones_Rot[b].vec[0], I->bones_Rot[b].vec[1], I->bones_Rot[b].vec[2]);
        }
        fprintf(F, "%u\n", (unsigned)I->A);
        fprintf(F, "%u\n", (unsigned)I->B);
        fprintf(F, "%f %f %f %f %f %f %f %f %f\n",
                I->rotVec_0[0][0], I->rotVec_0[0][1], I->rotVec_0[0][2],
                I->rotVec_0[1][0], I->rotVec_0[1][1], I->rotVec_0[1][2],
                I->rotVec_0[2][0], I->rotVec_0[2][1], I->rotVec_0[2][2]);
        fprintf(F, "%f %f %f %f %f %f %f %f %f\n",
                I->rotVec_1[0][0], I->rotVec_1[0][1], I->rotVec_1[0][2],
                I->rotVec_1[1][0], I->rotVec_1[1][1], I->rotVec_1[1][2],
                I->rotVec_1[2][0], I->rotVec_1[2][1], I->rotVec_1[2][2]);
        fprintf(F, "%f %f\n", I->poleRot, I->P.distance);
        fprintf(F, "%f %f %f\n", I->P.vec[0], I->P.vec[1], I->P.vec[2]);
    }

    fprintf(F, "\n");
    fclose(F);
    /*
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
    */
    return 1;
}

void save_Poses(char * poses_files_dir)
{
    int t, d, p;

    pose * P;
    deformer * D;
    transformer_pose * T;

    char dirfile[STRLEN];

    FILE * F;

    /*
    int index;
    unsigned address;
    char * Name;
    deformer * D;
    transformer_pose * TP;
    int transformers_count;
    */

    char n[8];

    for (d = 0; d < deformerIndex; d ++)
    {
        D = deformers[d];

        dirfile[0] = '\0';
        strcat(dirfile, poses_files_dir);
        strcat(dirfile, "/");
        strcat(dirfile, D->Name);
        strcat(dirfile, "_");
        sprintf(n, "%d", d);
        strcat(dirfile, n);
        strcat(dirfile, ".txt");

        F = fopen(dirfile, "w");

        if (F == NULL) continue;

        fprintf(F, "%s\n", "Poses");
        fprintf(F, "%d\n", D->Poses_Count);

        for (p = 0; p < D->Poses_Count; p ++)
        {
            P = D->Poses[p];
            fprintf(F, "%s\n", P->Name);
            fprintf(F, "%u\n", (unsigned)P);
            fprintf(F, "%u\n", (unsigned)P->D);
            fprintf(F, "%d\n", P->transformers_count);

            for (t = 0; t < P->transformers_count; t ++)
            {
                T = &P->TP[t];
                fprintf(F, "%d\n", T->rot_Order);
                fprintf(F, "%f %f %f\n", T->scl[0], T->scl[1], T->scl[2]);
                fprintf(F, "%f %f %f\n", T->scl_vec[0], T->scl_vec[1], T->scl_vec[2]);
                fprintf(F, "%f %f %f\n", T->rot[0], T->rot[1], T->rot[2]);

                fprintf(F, "%f %f %f %f %f %f %f %f %f\n",
                        T->rotVec_[0][0], T->rotVec_[0][1], T->rotVec_[0][2],
                        T->rotVec_[1][0], T->rotVec_[1][1], T->rotVec_[1][2],
                        T->rotVec_[2][0], T->rotVec_[2][1], T->rotVec_[2][2]);
                fprintf(F, "%f %f %f %f %f %f %f %f %f\n",
                        T->rotVec_I[0][0], T->rotVec_I[0][1], T->rotVec_I[0][2],
                        T->rotVec_I[1][0], T->rotVec_I[1][1], T->rotVec_I[1][2],
                        T->rotVec_I[2][0], T->rotVec_I[2][1], T->rotVec_I[2][2]);
                fprintf(F, "%f %f %f %f %f %f %f %f %f\n",
                        T->rotVec_B[0][0], T->rotVec_B[0][1], T->rotVec_B[0][2],
                        T->rotVec_B[1][0], T->rotVec_B[1][1], T->rotVec_B[1][2],
                        T->rotVec_B[2][0], T->rotVec_B[2][1], T->rotVec_B[2][2]);
                fprintf(F, "%f %f %f\n", T->pos[0], T->pos[1], T->pos[2]);
                fprintf(F, "%f %f %f\n", T->pos_[0], T->pos_[1], T->pos_[2]);
            }
        }

        fprintf(F, "\n");
        fclose(F);
    }
}

#endif // SAVING_H_INCLUDED