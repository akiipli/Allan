/*
The MIT License

Copyright <2018> <Allan Kiipli>
*/

#ifndef CAMERAS_H_INCLUDED
#define CAMERAS_H_INCLUDED

//#include "MathFunctions.h"

#define CAMERA_PERSPECTIVE 0
#define CAMERA_TOP 1
#define CAMERA_FRONT 2
#define CAMERA_LEFT 3
#define CAMERA_THUMB 4
#define CAMERA_ANIM 5
#define CAMERA_LIGHT 6

#define CAMERAS 5
#define CAMERAS_TOTAL 30

#define PIXEL_SIZE_ADJUSTMENT 1.5

int RENDER_LIGHT0 = 1;
float LightDist = 30.0;
float light_cone = 0.2;
float SHADOW = 255 / 2;
float LIGHT_CONE_FIT = 0.5;
float SHADOW_DIST = 0.0005;

float CamDist = 10;
float CamDist_Camera = 10;

float persp_Near = 0.001;
float persp_Far = 2048 * 10;

float z_near = 2000.0;

int Camera_screen_lock = 0;

int currentCamera = 0;

typedef struct
{
    float h_view;
    float pos[3];
    float rot[3];
    float target[3];
    float rotVec_[3][3];
    float CamDist;
}
camera_pose;

typedef struct
{
    int index;
    int selected;
    char Name[STRLEN];
    transformer * T;
    camera_pose * P;
    int ID;
    int width;
    int height;
    float h_v_ratio;
    float h_view;
    float v_view;
    float view_minor;
    float view_major;
    int objects[OBJECTS];
    int object_count;
    int ortho;
    float dim;
    float origin_2d[2];
    float _ratio;
    int sidebar;
    int bottom_line;
    int time_line;
    int uv_draw;

    float View_Radius;
    float Resolution_Radius;
    float Pixel_Size_In_Radians;
}
camera;

void set_Camera_Pose(camera * C, float dist)
{
    C->P->h_view = C->h_view;
    memcpy(C->P->pos, C->T->pos, sizeof(C->P->pos));
    memcpy(C->P->rot, C->T->rot, sizeof(C->P->rot));
    memcpy(C->P->target, C->T->target, sizeof(C->P->target));
    memcpy(C->P->rotVec_, C->T->rotVec_, sizeof(C->P->rotVec_));
    C->P->CamDist = dist;
}

void read_Camera_Pose(camera * C)
{
    C->h_view = C->P->h_view;
    memcpy(C->T->pos, C->P->pos, sizeof(C->T->pos));
    memcpy(C->T->rot, C->P->rot, sizeof(C->T->rot));
    memcpy(C->T->target, C->P->target, sizeof(C->T->target));
    memcpy(C->T->rotVec_, C->P->rotVec_, sizeof(C->T->rotVec_));
    CamDist = C->P->CamDist;
}

camera Camera_Persp, Camera_Top, Camera_Front, Camera_Left, Camera_Thumb, Light_Render;
camera * Camera;
camera * CAM = NULL;
camera * CAM0 = &Camera_Persp;
camera * Camera_Persp_Anim = &Camera_Persp; // split view

camera * Light0 = &Light_Render;

camera * cameras[CAMERAS_TOTAL];

int camIndex = 0;

void free_Cameras(int start)
{
    int c;

    camera * C;

    for (c = start; c < camIndex; c ++)
    {
        C = cameras[c];
        free(C->P);
        free(C);
    }
}

void clear_Camera_Selection()
{
    int c;

    camera * C;

    for (c = 0; c < camIndex; c ++)
    {
        C = cameras[c];
        C->selected = 0;
    }
}

void init_Cameras()
{
    cameras[0] = &Camera_Persp;
    cameras[1] = &Camera_Top;
    cameras[2] = &Camera_Front;
    cameras[3] = &Camera_Left;
    cameras[4] = &Camera_Thumb;

    camIndex = CAMERAS;

    int c;

    for (c = 0; c < CAMERAS; c ++)
    {
        cameras[c]->T = calloc(1, sizeof(transformer));
        cameras[c]->P = malloc(sizeof(camera_pose));
        init_transformer(cameras[c]->T, &World, "Camera");
    }
}

void update_light_ratio(camera * C, int hres, int vres)
{
    C->width = hres;
    C->height = vres;
    C->h_v_ratio = (float)hres / (float)vres;
    C->v_view = C->h_view / C->h_v_ratio;

    if (C->h_view > C->v_view)
    {
        C->view_minor = C->v_view;
        C->view_major = C->h_view;
    }
    else
    {
        C->view_minor = C->v_view;
        C->view_major = C->h_view;
    }

    C->View_Radius = sqrt(C->h_view * C->h_view + C->v_view * C->v_view) * PIXEL_SIZE_ADJUSTMENT;
    C->Resolution_Radius = sqrt(C->width * C->width + C->height * C->height) * PIXEL_SIZE_ADJUSTMENT;
    C->Pixel_Size_In_Radians = C->View_Radius / C->Resolution_Radius;

    //printf("Pixel Size In Radians %f\n", C->Pixel_Size_In_Radians);

    if (hres > vres){C->dim = hres; C->_ratio = C->h_v_ratio;} else {C->dim = vres; C->_ratio = 1.0 / C->h_v_ratio;}
}

void update_camera_ratio(camera * C, int hres, int vres)
{
    C->width = hres;
    C->height = vres;
    C->h_v_ratio = (float)hres / (float)vres;
    C->v_view = C->h_view / C->h_v_ratio;

    if (C->h_view > C->v_view)
    {
        C->view_minor = C->v_view;
        C->view_major = C->h_view;
    }
    else
    {
        C->view_minor = C->v_view;
        C->view_major = C->h_view;
    }

    C->View_Radius = sqrt(C->h_view * C->h_view + C->v_view * C->v_view) * PIXEL_SIZE_ADJUSTMENT;
    C->Resolution_Radius = sqrt(C->width * C->width + C->height * C->height) * PIXEL_SIZE_ADJUSTMENT;
    C->Pixel_Size_In_Radians = C->View_Radius / C->Resolution_Radius;

    //printf("Pixel Size In Radians %f\n", C->Pixel_Size_In_Radians);

    if (hres > vres){C->dim = hres; C->_ratio = C->h_v_ratio;} else {C->dim = vres; C->_ratio = 1.0 / C->h_v_ratio;}
}

void update_camera_persp(camera * C, float CamDist)
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    if (C->ortho)
    {
        float frame = CamDist * tan(C->view_minor / 2);
        glOrtho(-C->h_v_ratio * frame, C->h_v_ratio * frame, -frame, frame, -z_near, z_near);
        gluLookAt(C->T->pos[0], C->T->pos[1], C->T->pos[2], C->T->target[0], C->T->target[1], C->T->target[2], C->T->rotVec_[1][0], C->T->rotVec_[1][1], C->T->rotVec_[1][2]);
    }
    else
    {
        float angle = atan(C->v_view / 2.0);
        float R = sqrt((C->v_view / 2.0) + 1.0);
        float v_angle = angle * 2.0 * R;
        //printf("Camera v_view, v_angle %f, %f\n", C->v_view, v_angle);
        gluPerspective(v_angle * (180.0 / pi), C->h_v_ratio, persp_Near, persp_Far);
        gluLookAt(C->T->pos[0], C->T->pos[1], C->T->pos[2], C->T->target[0], C->T->target[1], C->T->target[2], C->T->rotVec_[1][0], C->T->rotVec_[1][1], C->T->rotVec_[1][2]);
    }
//    GLdouble m[4][4];
//    glGetDoublev(GL_PROJECTION_MATRIX, m);
//    C->T.rotVec_[0][0] = -m[0][0];
//    C->T.rotVec_[0][1] = -m[1][0];
//    C->T.rotVec_[0][2] = -m[2][0];
//    C->T.rotVec_[1][0] = m[0][1];
//    C->T.rotVec_[1][1] = m[1][1];
//    C->T.rotVec_[1][2] = m[2][1];
//    C->T.rotVec_[2][0] = m[0][2];
//    C->T.rotVec_[2][1] = m[1][2];
//    C->T.rotVec_[2][2] = m[2][2];
//    normalizeF(C->T.rotVec_[0]);
//    normalizeF(C->T.rotVec_[1]);
//    normalizeF(C->T.rotVec_[2]);
}

void update_camera_view(camera * C, float h_view, int hres, int vres)
{
    C->h_v_ratio = (float)hres / (float)vres;
    C->h_view = h_view;
    C->v_view = C->h_view / C->h_v_ratio;

    if (C->h_view > C->v_view)
    {
        C->view_minor = C->v_view;
        C->view_major = C->h_view;
    }
    else
    {
        C->view_minor = C->v_view;
        C->view_major = C->h_view;
    }

    C->View_Radius = sqrt(C->h_view * C->h_view + C->v_view * C->v_view) * PIXEL_SIZE_ADJUSTMENT;
    C->Resolution_Radius = sqrt(C->width * C->width + C->height * C->height) * PIXEL_SIZE_ADJUSTMENT;
    C->Pixel_Size_In_Radians = C->View_Radius / C->Resolution_Radius;

    if (hres > vres){C->dim = hres; C->_ratio = C->h_v_ratio;} else {C->dim = vres; C->_ratio = 1.0 / C->h_v_ratio;}
}

void update_camera_front(camera * C, float CamDist)
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    if (C->ortho)
    {
        float frame = CamDist * tan(C->view_minor / 2);
        glOrtho(-C->h_v_ratio * frame, C->h_v_ratio * frame, -frame, frame, -z_near, z_near);
        gluLookAt(C->T->pos[0], C->T->pos[1], C->T->pos[2], C->T->target[0], C->T->target[1], C->T->target[2], C->T->rotVec_[1][0], C->T->rotVec_[1][1], C->T->rotVec_[1][2]);
    }
    else
    {
        float angle = atan(C->v_view / 2.0);
        float R = sqrt((C->v_view / 2.0) + 1.0);
        float v_angle = angle * 2.0 * R;
        gluPerspective(v_angle * (180.0 / pi), C->h_v_ratio, persp_Near, persp_Far);
        gluLookAt(C->T->pos[0], C->T->pos[1], C->T->pos[2], C->T->target[0], C->T->target[1], C->T->target[2], C->T->rotVec_[1][0], C->T->rotVec_[1][1], C->T->rotVec_[1][2]);
    }
}

void update_camera_left(camera * C, float CamDist)
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    if (C->ortho)
    {
        float frame = CamDist * tan(C->view_minor / 2);
        glOrtho(-C->h_v_ratio * frame, C->h_v_ratio * frame, -frame, frame, -z_near, z_near);
        gluLookAt(C->T->pos[0], C->T->pos[1], C->T->pos[2], C->T->target[0], C->T->target[1], C->T->target[2], C->T->rotVec_[1][0], C->T->rotVec_[1][1], C->T->rotVec_[1][2]);
    }
    else
    {
        float angle = atan(C->v_view / 2.0);
        float R = sqrt((C->v_view / 2.0) + 1.0);
        float v_angle = angle * 2.0 * R;
        gluPerspective(v_angle * (180.0 / pi), C->h_v_ratio, persp_Near, persp_Far);
        gluLookAt(C->T->pos[0], C->T->pos[1], C->T->pos[2], C->T->target[0], C->T->target[1], C->T->target[2], C->T->rotVec_[1][0], C->T->rotVec_[1][1], C->T->rotVec_[1][2]);
    }
}

void update_camera_top(camera * C, float CamDist)
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    if (C->ortho)
    {
        float frame = CamDist * tan(C->view_minor / 2);
        glOrtho(-C->h_v_ratio * frame, C->h_v_ratio * frame, -frame, frame, -z_near, z_near);
        gluLookAt(C->T->pos[0], C->T->pos[1], C->T->pos[2], C->T->target[0], C->T->target[1], C->T->target[2], C->T->rotVec_[1][0], C->T->rotVec_[1][1], C->T->rotVec_[1][2]);
    }
    else
    {
        float angle = atan(C->v_view / 2.0);
        float R = sqrt((C->v_view / 2.0) + 1.0);
        float v_angle = angle * 2.0 * R;
        gluPerspective(v_angle * (180.0 / pi), C->h_v_ratio, persp_Near, persp_Far);
        gluLookAt(C->T->pos[0], C->T->pos[1], C->T->pos[2], C->T->target[0], C->T->target[1], C->T->target[2], C->T->rotVec_[1][0], C->T->rotVec_[1][1], C->T->rotVec_[1][2]);
    }
}

void update_camera(camera * C, float CamDist)
{
    if (C->ID == CAMERA_ANIM)
    {
        update_camera_persp(C, CamDist);
    }
    else if (C->ID == CAMERA_PERSPECTIVE)
    {
        update_camera_persp(C, CamDist);
    }
    else if (C->ID == CAMERA_TOP)
    {
        update_camera_top(C, CamDist);
    }
    else if (C->ID == CAMERA_FRONT)
    {
        update_camera_front(C, CamDist);
    }
    else if (C->ID == CAMERA_LEFT)
    {
        update_camera_left(C, CamDist);
    }
}

void init_camera(camera * C, char * Name, int hres, int vres, float CamDist, int ortho, int Id)
{
    sprintf(C->Name, "%s", Name);
    C->ID = Id;
    C->width = hres;
    C->height = vres;
    C->h_v_ratio = (float)hres / (float)vres;
    C->h_view = pi / 4.0;
    C->v_view = C->h_view / C->h_v_ratio;

    if (C->h_view > C->v_view)
    {
        C->view_minor = C->v_view;
        C->view_major = C->h_view;
    }
    else
    {
        C->view_minor = C->v_view;
        C->view_major = C->h_view;
    }

    C->T->rot[0] = 0.0;//pi60;
    C->T->rot[1] = pi; //pi60 + pi;
    C->ortho = ortho;

    if (hres > vres){C->dim = hres; C->_ratio = C->h_v_ratio;} else {C->dim = vres; C->_ratio = 1.0 / C->h_v_ratio;}

    C->origin_2d[0] = (float)hres / 2.0;
    C->origin_2d[1] = (float)vres / 2.0;

    C->sidebar = 0;
    C->bottom_line = 0;
    C->time_line = 0;
    C->uv_draw = 0;
}

int add_Item(int type, char * Name, void * pointer);

void add_Camera(int screen_width, int screen_height, int CamDist, int ortho_on, int CAMERA_TYPE)
{
    if (camIndex < CAMERAS_TOTAL)
    {
        char Name[STRLEN];
        camera * Camera = calloc(1, sizeof(camera));
        if (Camera != NULL)
        {
            Camera->T = calloc(1, sizeof(transformer));
            Camera->P = malloc(sizeof(camera_pose));

            init_transformer(Camera->T, &World, "Camera");
            sprintf(Name, "Cam Anim %d", camIndex);
            add_Item(TYPE_CAMERA, Name, Camera);

            Camera->index = camIndex;
            Camera->selected = 0;
            cameras[camIndex] = Camera;
            Camera->ID = CAMERA_TYPE;
            currentCamera = camIndex;
            CAM = Camera;
            init_camera(Camera, Name, screen_width, screen_height, CamDist, ortho_on, CAMERA_TYPE);
            //Camera->h_view = camIndex;
            camIndex ++;
        }
    }
}

void rotate_Camera_Aim(camera * C);

void init_Light0()
{
    Light0->T = calloc(1, sizeof(transformer));
    Light0->P = malloc(sizeof(camera_pose));
    init_transformer(Light0->T, &World, "Light Render");

    init_camera(Light0, "Light0", 100, 100, 30, 0, CAMERA_LIGHT);
/*
    Light0->T->rot[0] = 2.0;
    Light0->T->rot[1] = 1.0;
    Light0->T->rot[2] = 0.0;

    rotate_T(Light0->T);
*/
    Light0->T->pos[0] = 10.0;
    Light0->T->pos[1] = 10.0;
    Light0->T->pos[2] = 10.0;

    rotate_Camera_Aim(Light0);

    move_Back(Light0->T, LightDist);

    update_light_ratio(Light0, 100, 100);
}

void release_Light0()
{
    free(Light0->T);
    free(Light0->P);
}

#endif // CAMERAS_H_INCLUDED
