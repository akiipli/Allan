/*
The MIT License

Copyright <2018> <Allan Kiipli>
*/
/*
saving "Cube" and its transformer is disabled,
"Cube" cannot be included in deformers,
and its transformer cannot be in deformers.
Because "Cube" is not saved and cannot
be loaded. However he is necessary for
proper objects array functioning.
Cube cannot be reparented from World
and he cannot be deleted.
look for CUBECOMMENT
*/

/*CORRECT IT*/


#define DEBUG_WITHOUT_IL 0 //  change this to 1

#include <Windows.h> // dialog
#include "Dialog.h"
#include <dirent.h>
#include "Cursors.h"

#include "SDL/SDL_image.h"
#include "SDL/SDL_syswm.h"
#include "SDL/SDL_mouse.h"
#define GLEW_STATIC
#include "SDL/glew.h"
#include <GL/gl.h>
#include <GL/glu.h>

#if DEBUG_WITHOUT_IL == 0
#define ILUT_USE_OPENGL
#include <IL/il.h>
#include <IL/ilu.h>
#include <IL/ilut.h>
#endif

#define SCREEN_WIDTH 300
#define SCREEN_HEIGHT 200
#define SCREEN_BPP 32

#include "Transformer.h"
#include "Object.h"
#include "Curves.h"

#include <ft2build.h>
#include FT_FREETYPE_H
#include "UserInterface.h"
#include "IKSolution.h"
#include "Deformer.h"
#include "Poses.h"
#include "Bones.h"
#include "Shaders.h"
#include "File_IO.h"
#include "Materials.h"
#include "ImageLoad.h"
#include "MathFunctions.h"
#include "Cameras.h"
#include "Saving.h"

#include "InitializingFunctions.h"
#include "ExtensionFunctions.h"
#include "Subdivision.h"
#include "Generators.h"
#include "Creators.h"
#include "Transforms.h"
#include "Arrays.h"
#include "Lights.h"
#include "Rendering.h"

#include "Selections.h"
#include "Hierarchys.h"

#include "Validators.h"

#include "Loading.h"

#include "Modeling.h"

#include <windows.h> // console
#include <io.h>
#include <fcntl.h>

#define ITEM_DIALOG 0
#define MATERIAL_DIALOG 1
#define TEXT_DIALOG 2
#define SELS_DIALOG 3
#define HIER_DIALOG 4
#define DEFR_DIALOG 5
#define POSE_DIALOG 6
#define BONE_DIALOG 7
#define IK_DIALOG 8
#define OBJ_DIALOG 9
#define IMG_DIALOG 10
#define SAVES_DIALOG 11
#define LOADING_DIALOG 12

#define obj_EXTENSION 0
#define OBJ_EXTENSION 1
#define png_EXTENSION 0
#define jpg_EXTENSION 1
#define tif_EXTENSION 2
#define PSD_EXTENSION 3

int NVIDIA = 0;
int Bottom_Message = 1;
char * caption = "Trips Code c program | ";
char bottom_message[STRLEN];
char title[STRLEN];
char msg[STRLEN];

int LIGHTSHOW = 0;
int NormalMode = 0;
int BumpMode = 0;
int NORMAL_IMAGE = 0;
int BUMP_IMAGE = 0;
int TURNTABLE = 0;
int blit_flipped = 0;
int frames_count = 10;

int Drag_Dialog = 0;
int Axis_lock = 0;
float Magnitude;
int VIEWS_FLIPPED = 0;

int cull_Selection = 0;
int controlDown = 0;
int altDown = 0;
int UPDATEDRAG = 0;
int UPDATE_SELECTIONS = 1;
int UPDATE_DEFORMERS = 0;
int ROTATION = 0;
int MOVEMENT = 0;

int wireframe = 0;
int edgedraw = 0;
int edgeWeights = 0;
int vertdraw = 0;
int tripsRender = 0;
int splitview = 0;
char fileName[STRLEN];

float CamDist_mem = 10;
int message = -1;
int dialog_lock = 0;
int obj_files_start = 0;
int OBJ_files_start = 0;
int png_files_start = 0;
int jpg_files_start = 0;
int tif_files_start = 0;
int PSD_files_start = 0;

char extension[5] = ".obj";

int img_extension_type = 0;
int obj_extension_type = 0;

char * extensions[EXT_NUM];
int ext_count = 1;
int dialog_type = 0;

char item[TYPE_LENGTH];
char * items[ITEM_NUM];
int items_count = 3;
int items_start = 0;

char text[TYPE_LENGTH];
char * texts[TEXT_NUM];
int texts_count = 3;
int texts_start = 0;

int text_start = 0;
int materials_start = 0;
int norm_start = 0;
int bump_start = 0;

int TextIndex = 0;
int ItemIndex = 0;

int Object_Mode = 1;
int Polygon_Mode = 0;
int Edge_Mode = 0;
int Vertex_Mode = 0;
int selection_Mode = 0;

int mouse_button_down = 0;
int add_selection_mode = 1;
int drag_rectangle = 0;

int camera_rotate = 0;
int camera_z_move = 0;
float ObjDist;
float object_Pos[3];
float target_Pos[3];
float object_Rot[3];
float object_Scl[3];
float T_pos[3];

object * O;
transformer * T;
deformer * D;

int rendermode = POLY_RENDER;
int POLYS_ID_RENDER = 0;
int EDGES_ID_RENDER = 0;
int VERTS_ID_RENDER = 0;
int LOCAT_ID_RENDER = 0;

float m[4][4];

float Delta[3];
float Pos[3];

int mouse_x, mouse_y;
int ortho_on = 0;

int screen_width = SCREEN_WIDTH - SIDEBAR;
int screen_height = SCREEN_HEIGHT - BOTTOM_LINE;

SDL_Surface * screen = NULL;
SDL_Surface * Message = NULL;
SDL_Surface * foreground = NULL;
SDL_Event event;
SDLMod mod;

HWND gHwnd;
HANDLE hEvent;
HANDLE hFile;
DWORD dwReturnValue;
OBJ_In OBJ_File;

int object_hook = 0;
int currentObject = 0;
int currentMaterial = 0;
int currentLocator = 0;
int currentDeformer = 0;
int tune = 1;

SDL_Color textColor = {100, 255, 255};

int delay_ms = 100;

int subdLevel = 1; // -1 is no subdivision
int subdLevel_mem = -1;

float cage_v = 1.0;
float edge_c = 1.0;

int VBO = 0;
int fonts_on = 0;
char osd_font[STRLEN];
int Osd = 1;

int draw_uv_view;

int UPDATE_BACKGROUND = 1;
int UPDATE_COLORS = 0;
int UPDATE_UV = 0;
int DRAW_UI = 1;

int DRAW_LABELS = 1;
int DRAW_LOCATORS = 0;
int BONES_MODE = 0;

GLubyte pattern[] = {
51, 51, 51, 51,
51, 51, 51, 51,
204, 204, 204, 204,
204, 204, 204, 204,
51, 51, 51, 51,
51, 51, 51, 51,
204, 204, 204, 204,
204, 204, 204, 204,
51, 51, 51, 51,
51, 51, 51, 51,
204, 204, 204, 204,
204, 204, 204, 204,
51, 51, 51, 51,
51, 51, 51, 51,
204, 204, 204, 204,
204, 204, 204, 204,
51, 51, 51, 51,
51, 51, 51, 51,
204, 204, 204, 204,
204, 204, 204, 204,
51, 51, 51, 51,
51, 51, 51, 51,
204, 204, 204, 204,
204, 204, 204, 204,
51, 51, 51, 51,
51, 51, 51, 51,
204, 204, 204, 204,
204, 204, 204, 204,
51, 51, 51, 51,
51, 51, 51, 51,
204, 204, 204, 204,
204, 204, 204, 204
};

//int p_index[25] = {12, 7, 11, 17, 13, 6, 8, 16, 18, 2, 10, 22, 14, 1, 15, 23, 9, 3, 5, 21, 19, 0, 4, 20, 24};
//int pixel_id[25];

//int p_index[49] = {24, 17, 23, 31, 25, 16, 30, 32, 18, 10, 22, 38, 26, 9, 29, 37, 33, 11, 15, 39, 19, 8, 12, 36, 40,
//3, 21, 45, 27, 2, 28, 46, 20, 4, 14, 44, 34, 1, 35, 47, 13, 5, 7, 43, 41, 0, 42, 48, 6};
//int pixel_id[49];

int p_index[81] = {40, 31, 41, 49, 39, 30, 32, 50, 48,
22, 42, 58, 38, 21, 33, 59, 47,
23, 51, 57, 29, 20, 24, 60, 56,
13, 43, 67, 37, 12, 34, 68, 46,
14, 52, 66, 28, 11, 25, 69, 55,
15, 61, 65, 19, 10, 16, 70, 64,
4, 44, 76, 36, 3, 35, 77, 45,
5, 53, 75, 27, 2, 26, 78, 54,
6, 62, 74, 18, 1, 17, 79, 63,
7, 71, 73, 9, 0, 8, 80, 72};
int pixel_id[81];

int quit = 0;

void blit_ViewPort()
{
    glReadBuffer(GL_FRONT);
    glDrawBuffer(GL_BACK);

    if (blit_flipped)
    {
        glBlitFramebuffer(SIDEBAR, 0, screen_width, screen_height + BUTTON_HEIGHT,
            SIDEBAR, screen_height + BUTTON_HEIGHT, screen_width, 0, GL_COLOR_BUFFER_BIT, GL_NEAREST);
    }
    else
    {
        glBlitFramebuffer(SIDEBAR, 0, screen_width, screen_height + BUTTON_HEIGHT,
            SIDEBAR, 0, screen_width, screen_height + BUTTON_HEIGHT, GL_COLOR_BUFFER_BIT, GL_NEAREST);
    }

    glReadBuffer(GL_BACK);
}

float distance(float A[3], float B[3])
{
    float C[3] = {A[0] - B[0], A[1] - B[1], A[2] - B[2]};
    return sqrt(C[0] * C[0] + C[1] * C[1] + C[2] * C[2]);
}

void find_Camera_Objects()
{
    if (splitview)
    {
        find_objects_in_frame(&Camera_Persp);
        find_objects_in_frame(&Camera_Front);
        find_objects_in_frame(&Camera_Left);
        find_objects_in_frame(&Camera_Top);
    }
    else
    {
        find_objects_in_frame(Camera);
    }
}

void make_osd(object * O)
{
    /* uv tunes happen also with deforming on */
    /* to gain speed, disable uv tuning when deforming */
    /* when uv tuning is not necessary, in quad render */
    /* disable uv transform array and make texture array */
    /* static */
    int i, p, s;
    p = 0;
    p = sprintf(&osd_font[p], "%s\n", Camera->Name);
    p += sprintf(&osd_font[p], "ortho: %d\n", ortho_on);
    if (!O->deforms)
        p += sprintf(&osd_font[p], "%s\ndeforms:a\t%d\tF1-F4\n", O->Name, O->deforms);
    else
        p += sprintf(&osd_font[p], "%s\ndeforms:a\t%d\n", O->Name, O->deforms);

    if (subdLevel >= 0)
    {
        p += sprintf(&osd_font[p], "tune:u\tuv:Shift-u\n");
    }

    if (O->subdlevel < subdLevel)
    {
        s = O->subdlevel;
    }
    else
    {
        s = subdLevel;
    }

    for (i = 0; i <= s; i ++)
    {
        p += sprintf(&osd_font[p], "%d\t%d\n", O->tune[i], O->tune_in_uvtex[i]);
    }

    T = transformers[currentLocator];

    if (DRAW_LOCATORS && T != NULL)
    {
        if (T->Bone != NULL)
            p += sprintf(&osd_font[p], "%s\n", T->Bone->Name);
        else
            p += sprintf(&osd_font[p], "%s\n", T->Name);
    }

    if (BIND_POSE)
    {
        p += sprintf(&osd_font[p], "ALIGN\t%d\n", ALIGN_IS_ON);
    }

    if (subdLevel >= 0)
        p += sprintf(&osd_font[p], "quads\t%d\n", O->quadcount_[subdLevel]);
    else
        p += sprintf(&osd_font[p], "polys\t%d\n", O->polycount);

}

static void setup_opengl(int width, int height)
{
    const GLubyte * glversion = glGetString(GL_VERSION);
    printf("OpenGL %s\n", glversion);

    const GLubyte * glvendor = glGetString(GL_VENDOR);

    NVIDIA = find_card(glvendor);
    printf("%s %d\n", glvendor, NVIDIA);

    VAO = enable_VAO();
    VBO = enable_VBO();

    glReadBuffer(GL_BACK);
    glClearColor(0.8, 0.5, 0.3, 0.0);

    glShadeModel(GL_SMOOTH);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    GLfloat lightDiffuse[3] = {1.0, 1.0, 1.0};
    GLfloat lightAmbient[3] = {0.0, 0.1, 0.1};
    GLfloat lightSpecular[3] = {1.0, 0.0, 0.0};
    GLfloat lightPosition[3] = {10.0, 10.0, 10.0};

    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

	GLfloat globalAmbient[3] = {0.0, 0.0, 0.0};
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, globalAmbient);

    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_LIGHTING);

    glEnable(GL_LIGHT0);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);
    glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);
    glLightfv(GL_LIGHT0, GL_SPECULAR, lightSpecular);
    glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);

    lightPosition[0] = -10.0;
    lightPosition[1] = -10.0;
    lightPosition[2] = -10.0;

    glEnable(GL_LIGHT1);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, lightDiffuse);
    glLightfv(GL_LIGHT1, GL_AMBIENT, lightAmbient);
    glLightfv(GL_LIGHT1, GL_SPECULAR, lightSpecular);
    glLightfv(GL_LIGHT1, GL_POSITION, lightPosition);

    glEnable(GL_TEXTURE_2D);

    glEnable(GL_NORMALIZE);

    glEnable(GL_RESCALE_NORMAL);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

//    glEnable(GL_LINE_SMOOTH);
//    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
//    glEnable(GL_POINT_SMOOTH);
//    glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);

    glEnable(GL_MULTISAMPLE);
    glHint(GL_MULTISAMPLE_FILTER_HINT_NV, GL_NICEST);

//    glDisable(GL_POINT_SMOOTH);
//    glHint(GL_POINT_SMOOTH_HINT, GL_FASTEST);

    glEnable(GL_LINE_STIPPLE);

    zero_Texture = create_Empty_Texture();
    zero_Normal = create_Empty_Texture();
    zero_Bump = create_Empty_Texture();
    displayLists[0] = zero_Texture;
    normalsLists[0] = zero_Normal;
    bumpsLists[0] = zero_Bump;

    glGenTextures(TEXTURES, Textures);
    glGenTextures(TEXTURES, Normals);
    glGenTextures(TEXTURES, Bumps);

    glViewport(SIDEBAR, 0, width - SIDEBAR, height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_SCISSOR_TEST);

//    int x, y;
//    GLubyte b = 0b00110011;
//    for (y = 0; y < 2; y ++)
//    {
//        b = ~b;
//        for (x = 0; x < 8; x ++)
//        {
//            pattern[y][x] = b;
//        }
//    }

    glPolygonStipple(pattern);

//    float ratio = (float)width / (float)height;
//    glOrtho(0.0, (GLdouble)width, (GLdouble)height, 0.0, 6000.0, -10000.0);

//    gluPerspective(45.0, ratio, 1.0, 1024.0 );
//    gluLookAt(0.0, 0.0, 10.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
}

void quit_app(int c)
{
    SDL_Quit();
    exit(c);
}

int init()
{
    const SDL_VideoInfo* info = NULL;
    int width = 0;
    int height = 0;
    int bpp = 0;
    int flags = 0;

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        fprintf(stderr, "Video initialization failed: %s\n",
             SDL_GetError());
        quit_app(1);
    }
    info = SDL_GetVideoInfo();

    if (!info) {
        fprintf(stderr, "Video query failed: %s\n",
             SDL_GetError());
        quit_app(1);
    }
    width = SCREEN_WIDTH;
    height = SCREEN_HEIGHT;
    bpp = info->vfmt->BitsPerPixel;

    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    flags = SDL_OPENGL | SDL_RESIZABLE; //| SDL_FULLSCREEN;

    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);
    //SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 0);

    if (SDL_SetVideoMode(width, height, bpp, flags) == 0) {
        fprintf(stderr, "Video mode set failed: %s\n",
             SDL_GetError());
        quit_app(1);
    }

    glewInit();
    setup_opengl(width, height);

#if DEBUG_WITHOUT_IL == 0
    ilInit();
    iluInit();

    ilutRenderer(ILUT_OPENGL);
    ilEnable (IL_CONV_PAL);
    ilutEnable (ILUT_OPENGL_CONV);

    ilEnable(IL_ORIGIN_SET);
    ilSetInteger(IL_ORIGIN_MODE, IL_ORIGIN_LOWER_LEFT);
#endif

    SDL_SysWMinfo sdlinfo;
    SDL_version sdlver;
    SDL_VERSION(&sdlver);
    sdlinfo.version = sdlver;
    SDL_GetWMInfo(&sdlinfo);
    gHwnd = sdlinfo.window;

    SDL_WM_SetCaption(caption, NULL);
    fonts_on = init_fonts();

    SHADERS = init_shaders_();
    if (SHADERS)
    {
        disable_VBO();
        init_Hint();
        init_LIGHT_THEME();
        update_Light(Light_Themes[0]);
        init_lights();
        init_fog();
        /* shadows */
        //init_MATERIALS();
        //init_materialS();
        init_shadows();
        /* shadow map */
        setupFBO();
        setMatrices();
    }

    if (!fonts_on) quit_app(0);
    printf("fonts on %d\n", fonts_on);
    init_ui();
    init_labels();
    init_cursors();
    init_scene_extensions();
    return 1;
}

void cleanup()
{
    int i;
    for (i = 0; i < objectIndex; i++)
        free_object(objects[i]);
    glDeleteLists(0, Textures_c);
    glDeleteTextures(Textures_c, Textures);
    int t;
    for (t = 0; t < Surf_Text_c; t++)
        SDL_FreeSurface(Surf_Text[t]);
    disable_VBO();
    glDeleteTextures(1, &font_tex);
//    if (fonts_on)
//        glDeleteProgram(T_program[1][0]);
	for (i = 0; i < EXT_NUM; i ++)
    {
        free(extensions[i]);
    }
    for (i = 0; i < object_selections; i ++)
    {
        free(object_selection[i].indices);
    }

    free_transformers();
    free_deformers();
    free_poses();
    free_bones();
    free_ikChains();

    if (SHADERS)
    {
        delete_Shaders();
    }

    free_labels();
    free_Curves();
    free_Segments();
    free_Cps();

    quit_app(0);
}

void draw_Dialog();

void assert_Object_Selection()
{
    int o;
    object * O0;
    selected_object_count = 0;
    for (o = 0; o < objectIndex; o ++)
    {
        O0 = objects[o];
        if (O0->selected && loaded_objects[o])
        {
            selected_objects[selected_object_count ++] = o;
        }
    }
}

void assert_Locators_Selection()
{
    int t;
    transformer * T;
    transformer * current_T;
    current_T = transformers[currentLocator];

    selected_transformer_count = 0;

    for (t = 0; t < transformerIndex; t ++)
    {
        T = transformers[t];
        if (T->selected && T != current_T)
        {
            selected_transformers[selected_transformer_count ++] = t;
        }
    }
}

void assert_Element_Selection()
{
    int v, e, p, o;

    object * O;
    polygon * P;
    edge * E;
    vertex * V;

    for (o = 0; o < selected_object_count; o ++)
    {
        O = objects[selected_objects[o]];
        if (selection_Mode == 1) // polys
        {
            O->selected_polys_count = 0;
            for (p = 0; p < O->polycount; p ++)
            {
                P = &O->polys[p / ARRAYSIZE][p % ARRAYSIZE];
                if (P->selected)
                    O->selected_polys[O->selected_polys_count ++] = p;
            }
        }
        else if (selection_Mode == 2) // edges
        {
            O->selected_edges_count = 0;
            for (e = 0; e < O->edgecount; e ++)
            {
                E = &O->edges[e / ARRAYSIZE][e % ARRAYSIZE];
                if (E->selected)
                    O->selected_edges[O->selected_edges_count ++] = e;
            }
        }
        else if (selection_Mode == 3) // verts
        {
            O->selected_verts_count = 0;
            for (v = 0; v < O->vertcount; v ++)
            {
                V = &O->verts[v / ARRAYSIZE][v % ARRAYSIZE];
                if (V->selected)
                    O->selected_verts[O->selected_verts_count ++] = v;
            }
        }
    }

    if (selection_Mode == 1)
    {
        ELEMENT_ARRAYS = 0;
    }
}

void assert_Element_Selection_(object * O)
{
    int v, e, p;

    polygon * P;
    edge * E;
    vertex * V;

    if (selection_Mode == 1) // polys
    {
        O->selected_polys_count = 0;
        for (p = 0; p < O->polycount; p ++)
        {
            P = &O->polys[p / ARRAYSIZE][p % ARRAYSIZE];
            if (P->selected)
                O->selected_polys[O->selected_polys_count ++] = p;
        }
    }
    else if (selection_Mode == 2) // edges
    {
        O->selected_edges_count = 0;
        for (e = 0; e < O->edgecount; e ++)
        {
            E = &O->edges[e / ARRAYSIZE][e % ARRAYSIZE];
            if (E->selected)
                O->selected_edges[O->selected_edges_count ++] = e;
        }
    }
    else if (selection_Mode == 3) // verts
    {
        O->selected_verts_count = 0;
        for (v = 0; v < O->vertcount; v ++)
        {
            V = &O->verts[v / ARRAYSIZE][v % ARRAYSIZE];
            if (V->selected)
                O->selected_verts[O->selected_verts_count ++] = v;
        }
    }

    if (selection_Mode == 1)
    {
        ELEMENT_ARRAYS = 0;
    }
}

void assert_Current_Object()
{
    int o;
    object * O0;
    for (o = objectIndex -1; o >= 0; o --)
    {
        O0 = objects[o];
        if (O0->selected && loaded_objects[o])
        {
            currentObject = o;
            break;
        }
    }
    O = objects[currentObject];
}

void unhide_Object(int index)
{
    loaded_objects[index] = 1;
    objects[index]->selected = 1;
    assert_Object_Selection();
}

void hide_Object(int index)
{
    loaded_objects[index] = 0;
    objects[index]->selected = 0;

    assert_Object_Selection();
    assert_Current_Object();
}

void assignToQuads(object * O, quadrant * Q, int L, int surf)
{
    int idx;
    triangle * T;
    quadrant * Q0;
    int t, q;
    for (t = 0; t < 2; t ++)
    {
        idx = Q->trips[t];
        T = &O->trips_[L - 1][idx / ARRAYSIZE][idx % ARRAYSIZE];
        T->surface = surf;
    }
    if (Q->subdivs && L < SUBD)
    {
        for (q = 0; q < 4; q ++)
        {
            idx = Q->quads[q];
            Q0 = &O->quads_[L][idx / ARRAYSIZE][idx % ARRAYSIZE];
            Q0->surface = surf;
            assignToQuads(O, Q0, L + 1, surf);
        }
    }
}

void assignSurfToQuads(object * O, polygon * P, int surf)
{
    int idx;
    triangle * T;
    quadrant * Q;
    int t, q;
    int L = 0;
    P->surface = surf;
    for (t = 0; t < P->tripcount; t ++)
    {
        idx = P->trips[t];
        T = &O->trips[idx / ARRAYSIZE][idx % ARRAYSIZE];
        T->surface = surf;
    }
    if (P->subdivs)
    {
        for (q = 0; q < P->edgecount; q ++)
        {
            idx = P->quads[q];
            Q = &O->quads_[L][idx / ARRAYSIZE][idx % ARRAYSIZE];
            Q->surface = surf;
            assignToQuads(O, Q, L + 1, surf);
        }
    }
}

void assign_Texture(object * O)
{
    int t, p, q, l;
    triangle * T;
    polygon * P;
    quadrant * Q;
    for (t = 0; t < O->tripcount; t ++)
    {
        T = &O->trips[t / ARRAYSIZE][t % ARRAYSIZE];
        T->surface = O->surface;
    }
    for (p = 0; p < O->polycount; p ++)
    {
        P = &O->polys[p / ARRAYSIZE][p % ARRAYSIZE];
        P->surface = O->surface;
    }
    for(l = 0; l <= O->subdlevel; l ++)
    {
        for (t = 0; t < O->tripcount_[l]; t ++)
        {
            T = &O->trips_[l][t / ARRAYSIZE][t % ARRAYSIZE];
            T->surface = O->surface;
        }
        for (q = 0; q < O->quadcount_[l]; q ++)
        {
            Q = &O->quads_[l][q / ARRAYSIZE][q % ARRAYSIZE];
            Q->surface = O->surface;
        }
    }
}

void tune_subdivide_post_transformed(object * O, int L)
{
    // for animated subdivision objects
    // subdivision on spot step by step
    // starting from deformed cage.
    int level;
    for (level = 0; level <= O->subdlevel; level ++)
    {
        if (level > L)
        {
            break;
        }

        if (level > 0)
        {
            generate_cage_for_Subdivision_Quads_transformed(O, level);

            update_bounding_box_for_transformed_Edges_(O, level - 1, 0);
            update_bounding_box_for_transformed_Quads(O, level - 1, 0);

            tune_In_Subdivision_Shape_transformed_(O, level);

            update_bounding_box_for_transformed_Edges_(O, level, 0);
            update_bounding_box_for_transformed_Quads(O, level, 0);

            update_transformed_Triangles_level_(O, level); // updates also quad normals
            generate_transformed_edgeNormals_(O, level);
            generate_vertexNormals_transformed_(O, level);
        }
        else // level is 0, first subdivision, this is OK.
        {
            generate_cage_for_Subdivision_transformed(O, level);

            update_bounding_box_for_transformed_Edges(O, 0);
            update_bounding_box_for_transformed_Polys(O, 0);

            tune_In_Subdivision_Shape_transformed(O);

            update_bounding_box_for_transformed_Edges_(O, level, 0);
            update_bounding_box_for_transformed_Quads(O, level, 0);

            update_transformed_Triangles_level_(O, level); // updates also quad normals
            generate_transformed_edgeNormals_(O, level);
            generate_vertexNormals_transformed_(O, level);
        }
    }
}

void tune_subdivide_post(object * O)
{
    int level;
    for (level = 0; level <= O->subdlevel; level ++)
    {
        if (level > 0)
        {
            generate_cage_for_Subdivision_Quads(O, level);
            if (O->tune[level])
                tune_In_Subdivision_Shape_(O, level, edge_c, cage_v);
            if (O->tune_in_uvtex[level])
                tune_In_Subdivision_Shape_uvtex_(O, level);
        }
        else
        {
            generate_cage_for_Subdivision(O, level);
            if (O->tune[level])
                tune_In_Subdivision_Shape(O);
            if (O->tune_in_uvtex[level])
                tune_In_Subdivision_Shape_uvtex(O);
        }

        update_bounding_box_for_Edges_(O, level, 1);
        generate_Triangles_level_(O, level); // this function updates also quadrants normals
        update_bounding_box_for_Trips_(O, level, 1);
        update_bounding_box_for_Quads(O, level, 1);

        update_bounding_box_for_UV_Edges_(O, level, 1);
        update_2d_bounding_box_for_Quads(O, level, 1);

        generate_edgeNormals_level_(O, level);
        generate_vertexNormals_(O, level);
    }
}

void tune_subdivide_post_Objects()
{
    int i;
    object * O;
    for (i = 0; i < objectIndex; i ++)
    {
        O = objects[i];
        tune_subdivide_post(O);
    }
}

void subdivide_post(object * O, int level, int tune)
{
    O->subdlevel++;
    if (tune)
    {
        if (level > 0)
        {
            if (O->tune[level])
                tune_In_Subdivision_Shape_(O, level, edge_c, cage_v);
            if (O->tune_in_uvtex[level])
                tune_In_Subdivision_Shape_uvtex_(O, level);
        }
        else
        {
            if (O->tune[level])
                tune_In_Subdivision_Shape(O);
            if (O->tune_in_uvtex[level])
                tune_In_Subdivision_Shape_uvtex(O);
        }
    }
    update_bounding_box_for_Edges_(O, level, 1);
    generate_Triangles_level_(O, level); // this function updates also quadrants normals
    update_bounding_box_for_Trips_(O, level, 1); // updates also 2d box for triangles
    update_bounding_box_for_Quads(O, level, 1);
    update_2d_bounding_box_for_Quads(O, level, 1);

    update_bounding_box_for_UV_Edges_(O, level, 1);
    generate_edgeNormals_level_(O, level);
    generate_vertexNormals_(O, level);
    if (VBO)
    {
        generate_Vert_UV_Assoc_(O, level);
        O->vertex_arrays[level] = set_up_vertex_array(O, level);
    }
}

void Draw_Bottom_Message(const char * text)
{
    sprintf(bottom_message, text);
    draw_Bottom_Line(screen_width, screen_height);
    display_bottom_message(bottom_message, screen_width, screen_height);
    SDL_GL_SwapBuffers();
    blit_ViewPort();
}

int subdivide_after_Creation(object * O, int level, int tune)
{
    Draw_Bottom_Message("subdivide after Creation\n");

    int result;

    if (O->subdlevel == level - 1)
    {
        if (level > 0)
        {
            result = object_Subdivide_Quads(O, level);
        }
        else
        {
            result = object_Subdivide(O, level);
        }

//        write_Object_to_Disk_level_(O, level);

        if (result)
        {
            subdivide_post(O, level, tune);
        }
        else
        {
            //free_object_Level(O, level);
            return 0;
        }
        //rotate_verts_(O, O->T, level);
    }
    return 1;
}

int create_Object_OBJ(char * fileName, OBJ_In * OBJ_File)
{
    int result = 0;
    result = create_OBJ(objectIndex, fileName, OBJ_File);
    if (result)
    {
        object * O = objects[objectIndex];

        int m = init_Material(Materials_count);
        O->surface = m;
        Materials[m].use_texture = Materials[m - 1].use_texture;
        Materials_count ++;

        assign_Texture(O);

        objectIndex ++;

        int d = generate_UV_Verts(O);
        if (d)
        {
//            d = generate_Edges(O);
            if (d)
            {
                generate_UVEdges(O);
            }
        }
        generate_Edge_Split(O);
        generate_Vert_UV_Verts(O);
        generate_Triangles(O);
        generate_polyNormals(O);
        generate_edgeNormals(O);
        generate_vertexNormals(O);

        if (SHADERS)
        {
            calculate_Triangle_Tangents(O);
            average_Polygon_Tangents(O);
            average_Edge_Tangents(O);
            average_Vert_Tangents(O);
        }

        update_bounding_box_for_Trips(O, 1); // updates also 2d box for triangles
        update_bounding_box_for_Edges(O, 1);
        update_bounding_box_for_UV_Edges(O, 1);
        update_bounding_box_for_Polys(O, 1);
        update_2d_bounding_box_for_Polys(O, 1);
        boxt_O(O, 1);
        mean_Edge_Length(O);

        if (VBO)
        {
            O->vertex_array = set_up_Fan_array(O);
        }

//        int i;
//        edge * E;
//        for (i = 0; i < O->edgecount; i ++)
//        {
//            E = &O->edges[i / ARRAYSIZE][i % ARRAYSIZE];
//            printf("%d\n", E->uv_edcount);
//        }

//        write_Object_to_Disk(O);
    }
    return result;
}

int create_Objects()
{
    int result, i, level, tune;

    level = -1;

    result = create_cube(objectIndex, "Cube"); /*CUBECOMMENT*/

    if (result) objectIndex ++; else return level;

//
//    result = create_Z(objectIndex, "Z");
//
//    if (result) objectIndex ++; else return level;

//    result = create_ZPlane(objectIndex, "ZPlane");
//
//    if (result) objectIndex ++; else return level;

    for (i = 0; i < objectIndex; i ++)
    {
        object * O = objects[i];

        O->surface = 0;

        generate_Edge_UV_Assoc(O); // picking in 3d and 2d
        generate_Vert_UV_Assoc(O); // picking in 3d and 2d

        generate_Triangles(O);
        generate_polyNormals(O);
        generate_edgeNormals(O);
        generate_vertexNormals(O);

        update_bounding_box_for_Trips(O, 1);
        update_bounding_box_for_Edges(O, 1);
        update_bounding_box_for_UV_Edges(O, 1);
        update_bounding_box_for_Polys(O, 1);
        update_2d_bounding_box_for_Polys(O, 1);
        boxt_O(O, 1);
        mean_Edge_Length(O);

        if (VBO)
        {
            O->vertex_array = set_up_Fan_array(O);
        }

        level = 0;
        tune = 1;
        result = object_Subdivide(O, level);

        if (result)
        {
            subdivide_post(O, level, tune);
        }
        else
        {
            return level;
        }

        level = 1;
        result = object_Subdivide_Quads(O, level);

        if (result)
        {
            subdivide_post(O, level, tune);
        }
        else
        {
            return level;
        }

        level = 2;
        result = object_Subdivide_Quads(O, level);

        if (result)
        {
            subdivide_post(O, level, tune);
        }

//        int l;

//        write_Object_to_Disk(O);
//
//        for (l = 0; l <= level; l ++)
//        {
//            write_Object_to_Disk_level_(O, l);
//        }
    }
    return level;
}

void render_Objects(camera * C, int tripsRender, int wireframe, int uv_draw, int rendermode, int level)
{
    glGetFloatv(GL_PROJECTION_MATRIX, projectionMatrix);

    if (level < 0)
    {
        if (uv_draw && C->ID == CAMERA_PERSPECTIVE)
        {
            if (rendermode == ID_RENDER)
            {
                if (VERTS_ID_RENDER)
                    render_UV_Verts_ID(C, O);
                else if (EDGES_ID_RENDER)
                    render_UV_Edges_ID(C, O);
                else if (POLYS_ID_RENDER)
                    render_UV_Polys_ID(C, O);
            }
            else
            {
                render_uv_view(C, currentObject, NormalMode, BumpMode);
            }
        }
        else if (VERTS_ID_RENDER)
        {
            render_poly_Verts_ID(C, O);
        }
        else if (EDGES_ID_RENDER)
        {
            render_poly_Edges_ID(C, O);
        }
//        else if (Polygon_Mode && POLYS_ID_RENDER)
//        {
//            render_poly_ID(C, O); // because draw_Fan_Arrays_Shader_ID requires polygon based approach
//        }
        else if (LOCAT_ID_RENDER && rendermode == ID_RENDER && !POLYS_ID_RENDER)
        {
            render_Transformers_ID();
        }
        else if (tripsRender)
        {
            render_trips_OnScreen(C, wireframe, edgedraw, vertdraw, currentObject, selection_Mode);
        }
        else
        {
            render_polys_OnScreen(C, wireframe, edgedraw, vertdraw, currentObject, rendermode, selection_Mode, UPDATE_COLORS, UPDATE_UV, ELEMENT_ARRAYS);
        }
    }
    else
    {
        if (uv_draw && C->ID == CAMERA_PERSPECTIVE)
        {
            if (rendermode == ID_RENDER)
            {
                if (VERTS_ID_RENDER)
                    render_UV_Verts_ID_(C, O, level);
                else if (EDGES_ID_RENDER)
                    render_UV_Edges_ID_(C, O, level);
                else if (POLYS_ID_RENDER)
                    render_UV_Quads_ID(C, O, level);
            }
            else
            {
                render_uv_view_(C, currentObject, level, NormalMode, BumpMode);
            }
        }
        else if (VERTS_ID_RENDER)
        {
            render_quad_Verts_ID(C, O, level);
        }
        else if (EDGES_ID_RENDER)
        {
            render_quads_Edges_ID(C, O, level);
        }
//        else if (Polygon_Mode && POLYS_ID_RENDER)
//        {
//            render_Quads_ID(C, O, level); // because draw_Arrays_Shader_ID requires quad based approach
//        }
        else if (LOCAT_ID_RENDER && rendermode == ID_RENDER && !POLYS_ID_RENDER)
        {
            render_Transformers_ID();
        }
        else if (tripsRender)
        {
            render_trips_OnScreen_(C, wireframe, edgedraw, vertdraw, level, currentObject, selection_Mode);
        }
        else
        {
            render_quads_OnScreen(C, wireframe, edgedraw, vertdraw, level, currentObject, rendermode, selection_Mode, UPDATE_COLORS, UPDATE_UV, ELEMENT_ARRAYS);
        }
    }
}

void flip_Cameras(camera * C)
{
    VIEWS_FLIPPED = !VIEWS_FLIPPED;

    Camera_Persp.T->rot[1] -= pi;
    Camera_Top.T->rot[0] -= pi;
    Camera_Left.T->rot[1] -= pi;
    Camera_Front.T->rot[1] -= pi;

    rotate_Camera(&Camera_Persp, CamDist);
    rotate_Camera(&Camera_Top, CamDist);
    rotate_Camera(&Camera_Front, CamDist);
    rotate_Camera(&Camera_Left, CamDist);

    update_camera_view(&Camera_Persp, C->h_view, screen_width, screen_height);
    update_camera_view(&Camera_Top, C->h_view, screen_width, screen_height);
    update_camera_view(&Camera_Left, C->h_view, screen_width, screen_height);
    update_camera_view(&Camera_Front, C->h_view, screen_width, screen_height);

    if (VIEWS_FLIPPED)
    {
        sprintf(Camera_Top.Name, "%s", "Bottom");
        sprintf(Camera_Left.Name, "%s", "Right");
        sprintf(Camera_Front.Name, "%s", "Back");
    }
    else
    {
        sprintf(Camera_Top.Name, "%s", "Top");
        sprintf(Camera_Left.Name, "%s", "Left");
        sprintf(Camera_Front.Name, "%s", "Front");
    }
}

void frame_object(camera * C, int all_Views)
{
    int c, t, o, e, v, p;

    object * O;
    vertex * V;
    edge * E;
    polygon * P;
    transformer * T;

    float X = 0;
    float Y = 0;
    float Z = 0;

    float min_X = BIG_DIST;
    float min_Y = BIG_DIST;
    float min_Z = BIG_DIST;

    float max_X = -BIG_DIST;
    float max_Y = -BIG_DIST;
    float max_Z = -BIG_DIST;

    int elements_count = 0;

    if (DRAW_LOCATORS && Object_Mode)
    {
        for (t = 5; t < transformerIndex; t ++) // skip world and cameras
        {
            T = transformers[t];
            if (T->selected || t == currentLocator)
            {
                elements_count ++;

                X += T->pos[0];
                Y += T->pos[1];
                Z += T->pos[2];

                if (T->pos[0] - LocatorSize < min_X) min_X = T->pos[0] - LocatorSize;
                if (T->pos[1] - LocatorSize < min_Y) min_Y = T->pos[1] - LocatorSize;
                if (T->pos[2] - LocatorSize < min_Z) min_Z = T->pos[2] - LocatorSize;

                if (T->pos[0] + LocatorSize > max_X) max_X = T->pos[0] + LocatorSize;
                if (T->pos[1] + LocatorSize > max_Y) max_Y = T->pos[1] + LocatorSize;
                if (T->pos[2] + LocatorSize > max_Z) max_Z = T->pos[2] + LocatorSize;
            }
        }
        if (elements_count > 0)
        {
            X /= elements_count;
            Y /= elements_count;
            Z /= elements_count;
        }
    }
    else if (Object_Mode)
    {
        for (o = 0; o < selected_object_count; o ++)
        {
            O = objects[selected_objects[o]];
            if (O->selected || o == currentObject)
            {
                boxt_O_T(O, 1);

                elements_count ++;

                X += O->B.Tx;
                Y += O->B.Ty;
                Z += O->B.Tz;

                if (O->B.Tx - O->B.radius < min_X) min_X = O->B.Tx - O->B.radius;
                if (O->B.Ty - O->B.radius < min_Y) min_Y = O->B.Ty - O->B.radius;
                if (O->B.Tz - O->B.radius < min_Z) min_Z = O->B.Tz - O->B.radius;

                if (O->B.Tx + O->B.radius > max_X) max_X = O->B.Tx + O->B.radius;
                if (O->B.Ty + O->B.radius > max_Y) max_Y = O->B.Ty + O->B.radius;
                if (O->B.Tz + O->B.radius > max_Z) max_Z = O->B.Tz + O->B.radius;
            }
        }
        if (elements_count > 0)
        {
            X /= elements_count;
            Y /= elements_count;
            Z /= elements_count;
        }
    }
    else if (Polygon_Mode)
    {
        for (o = 0; o < selected_object_count; o ++)
        {
            O = objects[selected_objects[o]];
            for (p = 0; p < O->polycount; p ++)
            {
                P = &O->polys[p / ARRAYSIZE][p % ARRAYSIZE];
                if (P->selected)
                {
                    elements_count ++;

                    X += P->B.Tx;
                    Y += P->B.Ty;
                    Z += P->B.Tz;

                    if (P->B.Tx - P->B.Tradius < min_X) min_X = P->B.Tx - P->B.Tradius;
                    if (P->B.Ty - P->B.Tradius < min_Y) min_Y = P->B.Ty - P->B.Tradius;
                    if (P->B.Tz - P->B.Tradius < min_Z) min_Z = P->B.Tz - P->B.Tradius;

                    if (P->B.Tx + P->B.Tradius > max_X) max_X = P->B.Tx + P->B.Tradius;
                    if (P->B.Ty + P->B.Tradius > max_Y) max_Y = P->B.Ty + P->B.Tradius;
                    if (P->B.Tz + P->B.Tradius > max_Z) max_Z = P->B.Tz + P->B.Tradius;
                }
            }
        }
        if (elements_count > 0)
        {
            X /= elements_count;
            Y /= elements_count;
            Z /= elements_count;
        }
    }
    else if (Edge_Mode)
    {
        for (o = 0; o < selected_object_count; o ++)
        {
            O = objects[selected_objects[o]];
            for (e = 0; e < O->edgecount; e ++)
            {
                E = &O->edges[e / ARRAYSIZE][e % ARRAYSIZE];
                if (E->selected)
                {
                    elements_count ++;

                    X += E->B.Tx;
                    Y += E->B.Ty;
                    Z += E->B.Tz;

                    if (E->B.Tx - E->B.Tradius < min_X) min_X = E->B.Tx - E->B.Tradius;
                    if (E->B.Ty - E->B.Tradius < min_Y) min_Y = E->B.Ty - E->B.Tradius;
                    if (E->B.Tz - E->B.Tradius < min_Z) min_Z = E->B.Tz - E->B.Tradius;

                    if (E->B.Tx + E->B.Tradius > max_X) max_X = E->B.Tx + E->B.Tradius;
                    if (E->B.Ty + E->B.Tradius > max_Y) max_Y = E->B.Ty + E->B.Tradius;
                    if (E->B.Tz + E->B.Tradius > max_Z) max_Z = E->B.Tz + E->B.Tradius;
                }
            }
        }
        if (elements_count > 0)
        {
            X /= elements_count;
            Y /= elements_count;
            Z /= elements_count;
        }
    }
    else if (Vertex_Mode)
    {
        for (o = 0; o < selected_object_count; o ++)
        {
            O = objects[selected_objects[o]];
            for (v = 0; v < O->vertcount; v ++)
            {
                V = &O->verts[v / ARRAYSIZE][v % ARRAYSIZE];
                if (V->selected)
                {
                    elements_count ++;

                    X += V->Tx;
                    Y += V->Ty;
                    Z += V->Tz;

                    if (V->Tx < min_X) min_X = V->Tx;
                    if (V->Ty < min_Y) min_Y = V->Ty;
                    if (V->Tz < min_Z) min_Z = V->Tz;

                    if (V->Tx > max_X) max_X = V->Tx;
                    if (V->Ty > max_Y) max_Y = V->Ty;
                    if (V->Tz > max_Z) max_Z = V->Tz;
                }
            }
        }
        if (elements_count > 0)
        {
            X /= elements_count;
            Y /= elements_count;
            Z /= elements_count;
        }
    }

    if (elements_count > 0)
    {
        printf("%d selected elements\n", elements_count);

        float vec[3];

        vec[0] = max_X - min_X;
        vec[1] = max_Y - min_Y;
        vec[2] = max_Z - min_Z;

        float len = sqrt(vec[0] * vec[0] + vec[1] * vec[1] + vec[2] * vec[2]) / 2;

        if (len > MIN_SCALE && C->view_minor > MIN_SCALE)
        {
            CamDist = len / tan(C->view_minor / 2);
            if (Object_Mode)
                CamDist /= 2;
        }

        if (all_Views)
        {
            for (c = 0; c < CAMERAS; c ++)
            {
                C = cameras[c];
                C->T->target[0] = X;
                C->T->target[1] = Y;
                C->T->target[2] = Z;

                move_Back(C->T, CamDist);
            }
        }
        else
        {
            C->T->target[0] = X;
            C->T->target[1] = Y;
            C->T->target[2] = Z;

            move_Back(C->T, CamDist);
        }
    }
    else
    {
        Draw_Bottom_Message("no selected elements\n");
        O = objects[currentObject];
        boxt_O_T(O, 1);
        float len = O->B.radius;

        if (len > MIN_SCALE&& C->view_minor > MIN_SCALE)
        {
            CamDist = len / tan(C->view_minor / 2);
            if (Object_Mode)
                CamDist /= 2;
        }

        if (all_Views)
        {
            for (c = 0; c < CAMERAS; c ++)
            {
                C = cameras[c];

                C->T->target[0] = O->B.Tx;
                C->T->target[1] = O->B.Ty;
                C->T->target[2] = O->B.Tz;

                move_Back(C->T, CamDist);
            }
        }
        else
        {
            C->T->target[0] = O->B.Tx;
            C->T->target[1] = O->B.Ty;
            C->T->target[2] = O->B.Tz;

            move_Back(C->T, CamDist);
        }
    }
}

void reset_View(camera * C, float CamDist, int ortho)
{
    CamDist = 10;
    VIEWS_FLIPPED = 0;

    init_camera(&Camera_Persp, "Perspective", screen_width, screen_height, CamDist, ortho_on, CAMERA_PERSPECTIVE);
    init_camera(&Camera_Top, "Top", screen_width, screen_height, CamDist, ortho_on, CAMERA_TOP);
    init_camera(&Camera_Front, "Front", screen_width, screen_height, CamDist, ortho_on, CAMERA_FRONT);
    init_camera(&Camera_Left, "Left", screen_width, screen_height, CamDist, ortho_on, CAMERA_LEFT);

    Camera_Top.T->rot[0] = pi_2;
    Camera_Left.T->rot[1] = -pi_2;

    rotate_Camera(&Camera_Persp, CamDist);
    rotate_Camera(&Camera_Top, CamDist);
    rotate_Camera(&Camera_Front, CamDist);
    rotate_Camera(&Camera_Left, CamDist);

    update_camera_view(&Camera_Persp, Camera_Persp.h_view, screen_width, screen_height);
    update_camera_view(&Camera_Top, Camera_Top.h_view, screen_width, screen_height);
    update_camera_view(&Camera_Left, Camera_Left.h_view, screen_width, screen_height);
    update_camera_view(&Camera_Front, Camera_Front.h_view, screen_width, screen_height);
}

direction set_Target(float m_offset_x, float m_offset_y, float hres, float vres, float hview, float vview, float ObjDist)
{
    direction D;
//    D.x = 2 * (tan(hview / 2.0) * ObjDist);
//    D.x /= hres;
//    D.x *= m_offset_x;
//
//    D.y = 2 * (tan(vview / 2.0) * ObjDist);
//    D.y /= vres;
//    D.y *= m_offset_y;

    float r;

    D.x = hview * ObjDist;
    r = m_offset_x / hres;
    D.x *= r;

    D.y = vview * ObjDist;
    r = m_offset_y / vres;
    D.y *= r;

    return D;
}

void Draw_Rectangle()
{
    glViewport(Drag_Rectangle.x, Drag_Rectangle.y, Drag_Rectangle.w, Drag_Rectangle.h);

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0, Drag_Rectangle.w, Drag_Rectangle.h, 0, 1, -1);

	GLfloat white[4] = {1, 1, 1, 1};

	glDisable(GL_LIGHTING);
	glDisable(GL_DEPTH_TEST);

	glColor4fv(white);

	//glDisable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);

	/*draw frame*/
	glBegin(GL_LINE_LOOP);
	glVertex2f(0, 0);
	glVertex2f(0, Drag_Rectangle.h);
	glVertex2f(Drag_Rectangle.w, Drag_Rectangle.h);
	glVertex2f(Drag_Rectangle.w, 0);
	glEnd();

    glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glPopMatrix();
}

void draw_Locators()
{
    render_Transformers(currentLocator);
    render_Parent_Lines();
    render_IK_Chains();
}

void Draw_Ui()
{
    draw_UI_elements(SIDEBAR, screen_height);
    draw_Bottom_Line(screen_width, screen_height);
    display_bottom_message(bottom_message, screen_width, screen_height);
}

void poly_Render(int tripsRender, int wireframe, int splitview, float CamDist, int Swap, int Level)
{
    if (!drag_rectangle && (rendermode != ID_RENDER) && SHADERS && SHADOWS)
    {
        if (Fan_Arrays_Shader || subdLevel > -1)
        {
            SceneShadowMap_render(Camera, screen_width, screen_height, Level, ELEMENT_ARRAYS);
        }
    }

    if (drag_rectangle)
    {
        if (UPDATEDRAG)
            glClearColor(0.8, 0.5, 0.3, 0.0);
        else
            glClearColor(1, 0.5, 0.3, 0.0);
        glScissor(Drag_Rectangle.x, Drag_Rectangle.y, Drag_Rectangle.w, Drag_Rectangle.h);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }
    else if (rendermode == ID_RENDER || mouse_button_down)
    {
        /* amount of ids depends on clear color R value most, this needs to be high, closer to 1 */
        glClearColor(1, 0.5, 0.3, 0.0);
        glScissor(mouse_x - 4, screen_height + BOTTOM_LINE - mouse_y - 4, 9, 9);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0.8, 0.5, 0.3, 0.0);
    }
    else
    {
        glClearColor(0.8, 0.5, 0.3, 0.0);
        glScissor(SIDEBAR, BOTTOM_LINE, screen_width, screen_height);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    if (!tripsRender)
    {
        if (Level > -1)
        {
            fill_in_VertCoords_quads(Camera, Level, ELEMENT_ARRAYS);
        }
        else
        {
            fill_in_VertCoords_Fan(Camera, ELEMENT_ARRAYS);
        }
    }

    if (splitview)
    {
        glMatrixMode(GL_PROJECTION);
        glPushMatrix();
        glViewport(SIDEBAR, BOTTOM_LINE, screen_width/2, screen_height/2); // down left, front view

        update_camera_front(&Camera_Front, CamDist);

        render_Objects(&Camera_Front, tripsRender, wireframe, 0, rendermode, Level);
        if (DRAW_LOCATORS)
        {
            draw_Locators();
        }
        if (Axis_lock)
        {
            draw_Axis(Axis_lock, T_pos);
        }
        glPopMatrix();
        if (Osd)
        display_osd_font(Camera_Front.Name, screen_width, screen_height);

        glMatrixMode(GL_PROJECTION);
        glPushMatrix();
        glViewport(screen_width/2 + SIDEBAR, BOTTOM_LINE, screen_width/2, screen_height/2); // down right, left view

        update_camera_left(&Camera_Left, CamDist);

        render_Objects(&Camera_Left, tripsRender, wireframe, 0, rendermode, Level);
        if (DRAW_LOCATORS)
        {
            draw_Locators();
        }
        if (Axis_lock)
        {
            draw_Axis(Axis_lock, T_pos);
        }
        glPopMatrix();
        if (Osd)
        display_osd_font(Camera_Left.Name, screen_width, screen_height);

        glMatrixMode(GL_PROJECTION);
        glPushMatrix();
        glViewport(SIDEBAR, screen_height/2 + BOTTOM_LINE, screen_width/2, screen_height/2); // top left, top view

        update_camera_top(&Camera_Top, CamDist);

        render_Objects(&Camera_Top, tripsRender, wireframe, 0, rendermode, Level);
        if (DRAW_LOCATORS)
        {
            draw_Locators();
        }
        if (Axis_lock)
        {
            draw_Axis(Axis_lock, T_pos);
        }
        glPopMatrix();
        if (Osd)
        display_osd_font(Camera_Top.Name, screen_width, screen_height);

        glMatrixMode(GL_PROJECTION);
        glPushMatrix();
        glViewport(screen_width/2 + SIDEBAR, screen_height/2 + BOTTOM_LINE, screen_width/2, screen_height/2); // top right, perspective view

        update_camera_persp(&Camera_Persp, CamDist);

        render_Objects(&Camera_Persp, tripsRender, wireframe, draw_uv_view, rendermode, Level);
        if (!draw_uv_view)
        {
            if (DRAW_LOCATORS)
            {
                draw_Locators();
            }
            if (Axis_lock)
            {
                draw_Axis(Axis_lock, T_pos);
            }
        }
        glPopMatrix();
        if (Osd)
        {
            if (draw_uv_view)
                display_osd_font("UVs", screen_width, screen_height);
            else
                display_osd_font("Perspective", screen_width, screen_height);
        }
    }

    else
    {
        glViewport(SIDEBAR, BOTTOM_LINE, screen_width, screen_height);
        update_camera(Camera, CamDist);
        render_Objects(Camera, tripsRender, wireframe, draw_uv_view, rendermode, Level);
        if (!draw_uv_view)
        {
            if (DRAW_LOCATORS)
            {
                draw_Locators();
            }
            if (DRAW_LABELS)
            {
                render_Labels(screen_width, screen_height);
            }
            if (Axis_lock)
            {
                draw_Axis(Axis_lock, T_pos);
            }
        }
        if (Osd && fonts_on)
        {
            display_osd_font(osd_font, screen_width, screen_height);
        }
    }

    if (DRAW_UI && !mouse_button_down && !Camera_screen_lock)
    {
        Draw_Ui();
    }

    if (drag_rectangle && !UPDATEDRAG)
    {
        Draw_Rectangle();
    }

    if (!drag_rectangle && SHADERS && HINTS)
    {
        display_font(Hint, screen_width, screen_height, 0);
    }

    if (Swap)
    {
        SDL_GL_SwapBuffers();
        //SDL_Delay(delay_ms);
    }
}

void update_Resize_Event()
{
    if (screen_width != event.resize.w || screen_height != event.resize.h)
    {
        printf("received video resize event!\n");

        screen_width = event.resize.w - SIDEBAR;
        screen_height = event.resize.h - BOTTOM_LINE;

        if (screen_height % 2) screen_height ++;

        glViewport(SIDEBAR, BOTTOM_LINE, screen_width, screen_height);
        glDisable(GL_SCISSOR_TEST);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_SCISSOR_TEST);

        update_camera_ratio(&Camera_Persp, screen_width, screen_height);
        update_camera_ratio(&Camera_Top, screen_width, screen_height);
        update_camera_ratio(&Camera_Front, screen_width, screen_height);
        update_camera_ratio(&Camera_Left, screen_width, screen_height);

//                    poly_Render(tripsRender, wireframe, splitview, CamDist, 0, subdLevel);

        if (Edit_Lock)
        {
            Edit_Lock = 0;
            EditCursor = 0;
            EditString[EditCursor] = '\0';
            printf("Edit finishing!\n");
            set_Sels_H_Button(-1);
        }
        DIALOG_WIDTH = MIN_DIALOG_WIDTH;
        DIALOG_HEIGHT = MIN_DIALOG_HEIGHT;
        LISTLENGTH = DIALOG_HEIGHT / BUTTON_HEIGHT - 1;
        if (dialog_lock)
        {
            Bottom_Message = 0;
            draw_Dialog();
        }
        else
        {
            poly_Render(tripsRender, wireframe, splitview, CamDist, 1, subdLevel);
        }
    }
}

void cycle_frame_Player(camera * C, int all_Views)
{
    Draw_Bottom_Message("cycle frame Player");

    all_objects_in_frame(C);

    time_t start_time, current_time, local_time, local_time0;
    time(&start_time);

    local_time0 = 3;

    int o = 0;

    object * O;

    float X = 0;
    float Y = 0;
    float Z = 0;

    float min_X;
    float min_Y;
    float min_Z;

    float max_X;
    float max_Y;
    float max_Z;

    float vec[3];
    int c;

    DRAW_UI = 0;

    while (1 && selected_object_count > 0)
    {
        if (SDL_PollEvent(&event))
        {
            if (event.type == SDL_VIDEORESIZE)
            {
                DRAW_UI = 1;
                update_Resize_Event();
                poly_Render(tripsRender, wireframe, splitview, CamDist, 1, subdLevel);
                DRAW_UI = 0;
            }
            else if (event.type == SDL_KEYUP)
            {
                if (event.key.keysym.sym == SDLK_ESCAPE)
                {
                    break;
                }
            }
        }
        time(&current_time);
        local_time = current_time - start_time;
        if (local_time % 3 == 0 && local_time != local_time0)
        {
            min_X = BIG_DIST;
            min_Y = BIG_DIST;
            min_Z = BIG_DIST;

            max_X = -BIG_DIST;
            max_Y = -BIG_DIST;
            max_Z = -BIG_DIST;

            currentObject = selected_objects[o % selected_object_count];
            O = objects[currentObject];
            boxt_O_T(O, 1);

            X = O->B.Tx;
            Y = O->B.Ty;
            Z = O->B.Tz;

            if (O->B.Tx - O->B.radius < min_X) min_X = O->B.Tx - O->B.radius;
            if (O->B.Ty - O->B.radius < min_Y) min_Y = O->B.Ty - O->B.radius;
            if (O->B.Tz - O->B.radius < min_Z) min_Z = O->B.Tz - O->B.radius;

            if (O->B.Tx + O->B.radius > max_X) max_X = O->B.Tx + O->B.radius;
            if (O->B.Ty + O->B.radius > max_Y) max_Y = O->B.Ty + O->B.radius;
            if (O->B.Tz + O->B.radius > max_Z) max_Z = O->B.Tz + O->B.radius;

            vec[0] = max_X - min_X;
            vec[1] = max_Y - min_Y;
            vec[2] = max_Z - min_Z;

            float len = sqrt(vec[0] * vec[0] + vec[1] * vec[1] + vec[2] * vec[2]) / 2;

            if (len > MIN_SCALE && C->view_minor > MIN_SCALE)
            {
                CamDist = len / tan(C->view_minor / 2);
                if (Object_Mode)
                    CamDist /= 2;
            }

            if (all_Views)
            {
                for (c = 0; c < CAMERAS; c ++)
                {
                    C = cameras[c];
                    C->T->target[0] = X;
                    C->T->target[1] = Y;
                    C->T->target[2] = Z;

                    move_Back(C->T, CamDist);
                }
            }
            else
            {
                C->T->target[0] = X;
                C->T->target[1] = Y;
                C->T->target[2] = Z;

                move_Back(C->T, CamDist);
            }

            if (fonts_on)
            {
                make_osd(O);
            }

            poly_Render(tripsRender, wireframe, splitview, CamDist, 1, subdLevel);

            local_time0 = local_time;
            o ++;
        }
    }
    DRAW_UI = 1;
}

camera * find_View(int mouse_x, int mouse_y, int splitview)
{
    Camera_Top.uv_draw = 0;
    Camera_Front.uv_draw = 0;
    Camera_Persp.uv_draw = 0;
    Camera_Left.uv_draw = 0;

    if (splitview)
    {
        if (mouse_x >= SIDEBAR && mouse_x < screen_width / 2 + SIDEBAR) // left side
        {
            if (mouse_y >= 0 && mouse_y < screen_height / 2) // top
            {
                Camera = &Camera_Top;
            }
            else if (mouse_y >= screen_height / 2 && mouse_y <= screen_height) // low
            {
                Camera = &Camera_Front;
            }
        }
        else if (mouse_x >= screen_width / 2 + SIDEBAR && mouse_x < screen_width + SIDEBAR) // right side
        {
            if (mouse_y >= 0 && mouse_y < screen_height / 2) // top
            {
                Camera = &Camera_Persp;
            }
            else if (mouse_y >= screen_height / 2 && mouse_y <= screen_height) // low
            {
                Camera = &Camera_Left;
            }
        }
    }

    if (draw_uv_view && Camera->ID == CAMERA_PERSPECTIVE)
    {
        Camera->uv_draw = 1;
    }

    if (mouse_x < SIDEBAR)
    {
        Camera->sidebar = 1;
    }
    else
    {
        Camera->sidebar = 0;
        if (mouse_y > screen_height)
        {
            Camera->bottom_line = 1;
        }
        else
        {
            Camera->bottom_line = 0;
        }
    }
    return Camera;
}

void assign_Selection_sel(object * O)
{
    int p;
    polygon * P;
    for (p = 0; p < O->polycount; p ++)
    {
        P = &O->polys[p / ARRAYSIZE][p % ARRAYSIZE];
        assignSelectionToQuads(O, P, P->selected);
    }
}

void assign_Selection(object * O, int sel)
{
    int p;
    polygon * P;
    for (p = 0; p < O->polycount; p ++)
    {
        P = &O->polys[p / ARRAYSIZE][p % ARRAYSIZE];
        if (P->selected != sel)
        {
            P->selected = sel;
            assignSelectionToQuads(O, P, sel);
        }
    }
}

void set_Object_Mode()
{
    // printf("Object Mode\n");
    selection_Mode = 0;
    Object_Mode = 1;
    Polygon_Mode = 0;
    Edge_Mode = 0;
    Vertex_Mode = 0;
    vertdraw = 0;
    Button_Mode[0].color = UI_GRAYD;
    Button_Mode[1].color = UI_GRAYB;
    Button_Mode[2].color = UI_GRAYB;
    Button_Mode[3].color = UI_GRAYB;
    SDL_SetCursor(Arrow);
}

void set_Polygon_Mode()
{
    printf("Polygon Mode\n");
    selection_Mode = 1;
    Object_Mode = 0;
    Polygon_Mode = 1;
    Edge_Mode = 0;
    Vertex_Mode = 0;
    vertdraw = 0;
    Button_Mode[0].color = UI_GRAYB;
    Button_Mode[1].color = UI_GRAYD;
    Button_Mode[2].color = UI_GRAYB;
    Button_Mode[3].color = UI_GRAYB;
    if (add_selection_mode)
        SDL_SetCursor(Arrow_Plus);
    else
        SDL_SetCursor(Arrow_Minus);
}

void set_Edge_Mode()
{
    printf("Edge Mode\n");
    selection_Mode = 2;
    Object_Mode = 0;
    Polygon_Mode = 0;
    Edge_Mode = 1;
    Vertex_Mode = 0;
    vertdraw = 0;
    Button_Mode[0].color = UI_GRAYB;
    Button_Mode[1].color = UI_GRAYB;
    Button_Mode[2].color = UI_GRAYD;
    Button_Mode[3].color = UI_GRAYB;

    edgedraw = 1;

    if (add_selection_mode)
        SDL_SetCursor(Arrow_Plus);
    else
        SDL_SetCursor(Arrow_Minus);
}

void set_Vertex_Mode()
{
    printf("Vertex Mode\n");
    selection_Mode = 3;
    Object_Mode = 0;
    Polygon_Mode = 0;
    Edge_Mode = 0;
    Vertex_Mode = 1;
    vertdraw = 1;
    Button_Mode[0].color = UI_GRAYB;
    Button_Mode[1].color = UI_GRAYB;
    Button_Mode[2].color = UI_GRAYB;
    Button_Mode[3].color = UI_GRAYD;

    if (add_selection_mode)
        SDL_SetCursor(Arrow_Plus);
    else
        SDL_SetCursor(Arrow_Minus);
}

void select_Transformer()
{
    currentLocator = HierIndex;
    DRAW_UI = 0;
    poly_Render(tripsRender, wireframe, splitview, CamDist, 0, subdLevel);
    DRAW_UI = 1;
    draw_Dialog();
}

void select_Selection_AB(int index)
{
    Draw_Bottom_Message("Select selection AB!\n");
    Bottom_Message = 0;
    set_Sels_H_Button(5);

    if (current_sel_type == 3) // verts
    {
        set_Vertex_Mode();
        int o, i, idx, v, i_c;
        int condition = 1;
        vertex * V;

        printf("Vertex selection type\n");
        object * O;

        for (o = 0; o < selected_object_count; o ++)
        {
            O = objects[selected_objects[o]];
            for (i = 0; i < O->vertex_selections; i ++)
            {
                if (strcmp(O->vertex_selection[i]->Name, Sels_Names[current_sel_type][SelsIndex[current_sel_type]]) == 0)
                {
                    printf("Selection exists! Selecting.\n");
                    for (v = 0; v < O->vertcount; v ++)
                    {
                        V = &O->verts[v / ARRAYSIZE][v % ARRAYSIZE];
                        V->selected = 0;
                    }
                    for (i_c = 0; i_c < O->vertex_selection[i]->split[index].indices_count; i_c ++)
                    {
                        idx = O->vertex_selection[i]->split[index].indices[i_c];
                        V = &O->verts[idx / ARRAYSIZE][idx % ARRAYSIZE];
                        V->selected = 1;
                    }
                    printf("selected elements count %d\n", O->vertex_selection[i]->indices_count);

                    condition = 0;
                    break;
                }
            }

            if (condition)
            {
                for (v = 0; v < O->vertcount; v ++)
                {
                    V = &O->verts[v / ARRAYSIZE][v % ARRAYSIZE];
                    V->selected = 0;
                }
            }
        }
    }
    DRAW_UI = 0;
    poly_Render(tripsRender, wireframe, splitview, CamDist, 0, subdLevel);
    if (SelsIndex[current_sel_type] - sels_start[current_sel_type] >= 0)
        SelsList[SelsIndex[current_sel_type] - sels_start[current_sel_type]].color = UI_BACKL;
    draw_Dialog();
    DRAW_UI = 1;
    UPDATE_COLORS = 0;
}

void select_Selection()
{
    printf("Select selection!\n");
    set_Sels_H_Button(5);

    if (current_sel_type == 0) // objects
    {
        set_Object_Mode();
        int o, i;
        printf("Object selections\n");
        selected_object_count = 0;
        for (i = 0; i < object_selection[SelsIndex[current_sel_type]].indices_count; i ++)
        {
            selected_objects[selected_object_count ++] = object_selection[SelsIndex[current_sel_type]].indices[i];
        }
        for (o = 0; o < objectIndex; o ++)
        {
            O = objects[o];
            O->selected = 0;
        }
        for (o = 0; o < selected_object_count; o ++)
        {
            O = objects[selected_objects[o]];
            O->selected = 1;
        }
        if (selected_object_count < 1)
        {
            selected_object_count = 1;
            selected_objects[0] = currentObject;
        }
        currentObject = selected_objects[selected_object_count - 1];
        O = objects[currentObject];
        O->selected = 1;
    }
    else if (current_sel_type == 1) // polys
    {
        set_Polygon_Mode();
        int o, i, idx, p, i_c;
        int condition;
        polygon * P;

        printf("Polygon selection type\n");
        object * O;

        for (o = 0; o < selected_object_count; o ++)
        {
            condition = 1;
            O = objects[selected_objects[o]];
            for (i = 0; i < O->polygon_selections; i ++)
            {
                if (strcmp(O->polygon_selection[i]->Name, Sels_Names[current_sel_type][SelsIndex[current_sel_type]]) == 0)
                {
                    printf("Selection exists! Selecting.\n");
                    for (p = 0; p < O->polycount; p ++)
                    {
                        P = &O->polys[p / ARRAYSIZE][p % ARRAYSIZE];
                        P->selected = 0;
                    }
                    for (i_c = 0; i_c < O->polygon_selection[i]->indices_count; i_c ++)
                    {
                        idx = O->polygon_selection[i]->indices[i_c];
                        P = &O->polys[idx / ARRAYSIZE][idx % ARRAYSIZE];
                        P->selected = 1;
                    }
                    printf("selected elements count %d\n", O->polygon_selection[i]->indices_count);

                    condition = 0;
                    break;
                }
            }

            if (condition)
            {
                for (p = 0; p < O->polycount; p ++)
                {
                    P = &O->polys[p / ARRAYSIZE][p % ARRAYSIZE];
                    P->selected = 0;
                }
            }
        }
        int l;

        for (o = 0; o < selected_object_count; o ++)
        {
            O = objects[selected_objects[o]];
            assign_Selection_sel(O);
        }
        for (l = 0; l < SUBD; l++)
            load_id_colors(selected_objects, selected_object_count, l, OBJECT_COLORS);
        load_id_colors_Fan(selected_objects, selected_object_count, OBJECT_COLORS);
        UPDATE_COLORS = 1;
    }
    else if (current_sel_type == 2) // edges
    {
        set_Edge_Mode();
        int o, i, idx, e, i_c;
        int condition = 1;
        edge * E;

        printf("Edge selection type\n");
        object * O;

        for (o = 0; o < selected_object_count; o ++)
        {
            O = objects[selected_objects[o]];
            for (i = 0; i < O->edge_selections; i ++)
            {
                if (strcmp(O->edge_selection[i]->Name, Sels_Names[current_sel_type][SelsIndex[current_sel_type]]) == 0)
                {
                    printf("Selection exists! Selecting.\n");
                    for (e = 0; e < O->edgecount; e ++)
                    {
                        E = &O->edges[e / ARRAYSIZE][e % ARRAYSIZE];
                        E->selected = 0;
                    }
                    for (i_c = 0; i_c < O->edge_selection[i]->indices_count; i_c ++)
                    {
                        idx = O->edge_selection[i]->indices[i_c];
                        E = &O->edges[idx / ARRAYSIZE][idx % ARRAYSIZE];
                        E->selected = 1;
                    }
                    printf("selected elements count %d\n", O->edge_selection[i]->indices_count);

                    condition = 0;
                    break;
                }
            }

            if (condition)
            {
                for (e = 0; e < O->edgecount; e ++)
                {
                    E = &O->edges[e / ARRAYSIZE][e % ARRAYSIZE];
                    E->selected = 0;
                }
            }
        }
    }
    else if (current_sel_type == 3) // verts
    {
        set_Vertex_Mode();
        int o, i, idx, v, i_c;
        int condition = 1;
        vertex * V;

        printf("Vertex selection type\n");
        object * O;

        for (o = 0; o < selected_object_count; o ++)
        {
            O = objects[selected_objects[o]];
            for (i = 0; i < O->vertex_selections; i ++)
            {
                if (strcmp(O->vertex_selection[i]->Name, Sels_Names[current_sel_type][SelsIndex[current_sel_type]]) == 0)
                {
                    printf("Selection exists! Selecting.\n");
                    for (v = 0; v < O->vertcount; v ++)
                    {
                        V = &O->verts[v / ARRAYSIZE][v % ARRAYSIZE];
                        V->selected = 0;
                    }
                    for (i_c = 0; i_c < O->vertex_selection[i]->indices_count; i_c ++)
                    {
                        idx = O->vertex_selection[i]->indices[i_c];
                        V = &O->verts[idx / ARRAYSIZE][idx % ARRAYSIZE];
                        V->selected = 1;
                    }
                    printf("selected elements count %d\n", O->vertex_selection[i]->indices_count);

                    condition = 0;
                    break;
                }
            }

            if (condition)
            {
                for (v = 0; v < O->vertcount; v ++)
                {
                    V = &O->verts[v / ARRAYSIZE][v % ARRAYSIZE];
                    V->selected = 0;
                }
            }
        }
    }
    DRAW_UI = 0;
    poly_Render(tripsRender, wireframe, splitview, CamDist, 0, subdLevel);
    if (SelsIndex[current_sel_type] - sels_start[current_sel_type] >= 0)
        SelsList[SelsIndex[current_sel_type] - sels_start[current_sel_type]].color = UI_BACKL;
    draw_Dialog();
    DRAW_UI = 1;
    UPDATE_COLORS = 0;
}

void select_Material()
{
    DRAW_UI = 0;
    poly_Render(tripsRender, wireframe, splitview, CamDist, 0, subdLevel);
    if (currentMaterial - materials_start >= 0)
        MatrList[currentMaterial - materials_start].color = UI_BACKL;
    draw_Dialog();
    DRAW_UI = 1;
    UPDATE_COLORS = 0;
}

void grow_Selection()
{
    if (Polygon_Mode)
    {
        Draw_Bottom_Message("grow polygon Selection\n");
        int c, o, e, idx, p;

        edge * E;
        polygon * P;

        printf("Polygon selection type\n");
        object * O;

        int * candidates = malloc(0);
        int candidates_count;

        for (o = 0; o < selected_object_count; o ++)
        {
            O = objects[selected_objects[o]];
            candidates_count = 0;
            candidates = realloc(candidates, O->edgecount * 2 * sizeof(int));
            for (p = 0; p < O->polycount; p ++)
            {
                P = &O->polys[p / ARRAYSIZE][p % ARRAYSIZE];
                if (P->selected)
                {
                    for (e = 0; e < P->edgecount; e ++)
                    {
                        idx = P->edges[e];
                        E = &O->edges[idx / ARRAYSIZE][idx % ARRAYSIZE];
                        if (E->polycount > 1)
                        {
                            if (P->index == E->polys[0])
                                candidates[candidates_count ++] = E->polys[1];
                            else
                                candidates[candidates_count ++] = E->polys[0];
                        }
                    }
                }
            }
            for (c = 0; c < candidates_count; c ++)
            {
                idx = candidates[c];
                P = &O->polys[idx / ARRAYSIZE][idx % ARRAYSIZE];
                P->selected = 1;
            }
        }
        free(candidates);

        int l;

        for (o = 0; o < selected_object_count; o ++)
        {
            O = objects[selected_objects[o]];
            assign_Selection_sel(O);
        }
        for (l = 0; l < SUBD; l++)
            load_id_colors(selected_objects, selected_object_count, l, OBJECT_COLORS);
        load_id_colors_Fan(selected_objects, selected_object_count, OBJECT_COLORS);
        UPDATE_COLORS = 1;
    }
    else if (Edge_Mode)
    {
        Draw_Bottom_Message("grow edge Selection\n");
        int i, c, o, e, idx, v;

        edge * E, * E0;
        uv_edge * UVE;
        vertex * V;

        printf("Edge selection type\n");
        object * O;

        int * candidates = malloc(0);
        int candidates_count;

        for (o = 0; o < selected_object_count; o ++)
        {
            O = objects[selected_objects[o]];
            candidates_count = 0;
            candidates = realloc(candidates, O->vertcount * 2 * sizeof(int));
            for (e = 0; e < O->edgecount; e ++)
            {
                E = &O->edges[e / ARRAYSIZE][e % ARRAYSIZE];
                if (E->selected)
                {
                    for (i = 0; i < 2; i ++)
                    {
                        idx = E->verts[i];
                        V = &O->verts[idx / ARRAYSIZE][idx % ARRAYSIZE];
                        for (v = 0; v < V->edgecount; v ++)
                        {
                            idx = V->edges[v];
                            E0 = &O->edges[idx / ARRAYSIZE][idx % ARRAYSIZE];
                            if (!E0->selected && E0 != E)
                            {
                                candidates[candidates_count ++] = E0->index;
                            }
                        }
                    }
                }
            }
            for (c = 0; c < candidates_count; c ++)
            {
                idx = candidates[c];
                E = &O->edges[idx / ARRAYSIZE][idx % ARRAYSIZE];
                E->selected = 1;
                for (v = 0; v < E->uv_edcount; v ++)
                {
                    idx = E->uv_edges[v];
                    if (idx > -1 && idx < O->uvedcount)
                    {
                        UVE = &O->uveds[idx / ARRAYSIZE][idx % ARRAYSIZE];
                        UVE->selected = E->selected;
                    }
                }
            }
        }
        free(candidates);
    }
    else if (Vertex_Mode)
    {
        Draw_Bottom_Message("grow vertex Selection\n");
        int u, c, o, e, idx, v;

        edge * E;
        vertex * V;
        uv * UV;

        printf("Vertex selection type\n");
        object * O;

        int * candidates = malloc(0);
        int candidates_count;

        for (o = 0; o < selected_object_count; o ++)
        {
            O = objects[selected_objects[o]];
            candidates_count = 0;
            candidates = realloc(candidates, O->edgecount * 2 * sizeof(int));
            for (v = 0; v < O->vertcount; v ++)
            {
                V = &O->verts[v / ARRAYSIZE][v % ARRAYSIZE];
                if (V->selected)
                {
                    for (e = 0; e < V->edgecount; e ++)
                    {
                        idx = V->edges[e];
                        E = &O->edges[idx / ARRAYSIZE][idx % ARRAYSIZE];
                        if (V->index == E->verts[0])
                            candidates[candidates_count ++] = E->verts[1];
                        else
                            candidates[candidates_count ++] = E->verts[0];
                    }
                }
            }
            for (c = 0; c < candidates_count; c ++)
            {
                idx = candidates[c];
                V = &O->verts[idx / ARRAYSIZE][idx % ARRAYSIZE];
                V->selected = 1;
                for (u = 0; u < V->uv_vertcount; u ++)
                {
                    idx = V->uv_verts[u];
                    if (idx > -1 && idx < O->textcount)
                    {
                        UV = &O->uvtex[idx / ARRAYSIZE][idx % ARRAYSIZE];
                        UV->selected = V->selected;
                    }
                }
            }
        }
        free(candidates);
    }
}

void convert_To_Vert_Selection()
{
    Draw_Bottom_Message("convert To Vert Selection\n");
    if (Polygon_Mode)
    {
        int u, v, o, idx, p;

        polygon * P;
        vertex * V;
        uv * UV;

        printf("Polygon selection type\n");
        object * O;

        for (o = 0; o < selected_object_count; o ++)
        {
            O = objects[selected_objects[o]];

            for (v = 0; v < O->vertcount; v ++)
            {
                V = &O->verts[v / ARRAYSIZE][v % ARRAYSIZE];
                V->selected = 0;
            }

            for (p = 0; p < O->polycount; p ++)
            {
                P = &O->polys[p / ARRAYSIZE][p % ARRAYSIZE];
                if (P->selected)
                {
                    for (v = 0; v < P->edgecount; v ++)
                    {
                        idx = P->verts[v];
                        V = &O->verts[idx / ARRAYSIZE][idx % ARRAYSIZE];
                        V->selected = 1;
                        for (u = 0; u < V->uv_vertcount; u ++)
                        {
                            idx = V->uv_verts[u];
                            if (idx > -1 && idx < O->textcount)
                            {
                                UV = &O->uvtex[idx / ARRAYSIZE][idx % ARRAYSIZE];
                                UV->selected = V->selected;
                            }
                        }
                    }
                }
            }
        }
    }
    else if (Edge_Mode)
    {
        int u, i, o, e, idx, v;

        edge * E;
        vertex * V;
        uv * UV;

        printf("Edge selection type\n");
        object * O;

        for (o = 0; o < selected_object_count; o ++)
        {
            O = objects[selected_objects[o]];

            for (v = 0; v < O->vertcount; v ++)
            {
                V = &O->verts[v / ARRAYSIZE][v % ARRAYSIZE];
                V->selected = 0;
            }

            for (u = 0; u < O->textcount; u ++)
            {
                UV = &O->uvtex[u / ARRAYSIZE][u % ARRAYSIZE];
                UV->selected = 0;
            }

            for (e = 0; e < O->edgecount; e ++)
            {
                E = &O->edges[e / ARRAYSIZE][e % ARRAYSIZE];
                if (E->selected)
                {
                    for (i = 0; i < 2; i ++)
                    {
                        idx = E->verts[i];
                        V = &O->verts[idx / ARRAYSIZE][idx % ARRAYSIZE];
                        V->selected = 1;
                        for (u = 0; u < V->uv_vertcount; u ++)
                        {
                            idx = V->uv_verts[u];
                            if (idx > -1 && idx < O->textcount)
                            {
                                UV = &O->uvtex[idx / ARRAYSIZE][idx % ARRAYSIZE];
                                UV->selected = V->selected;
                            }
                        }
                    }
                }
            }
        }
    }
}

void convert_To_Border_Verts(object * O)
{
    Draw_Bottom_Message("convert To Border Verts\n");
    int u, i, v, e, idx;

    edge * E, * E0;
    vertex * V;
    uv * UV;

    int condition;

    for(v = 0; v < O->vertcount; v ++)
    {
        V = &O->verts[v / ARRAYSIZE][v % ARRAYSIZE];
        V->selected = 0;
    }

    for (u = 0; u < O->textcount; u ++)
    {
        UV = &O->uvtex[u / ARRAYSIZE][u % ARRAYSIZE];
        UV->selected = 0;
    }

    for(e = 0; e < O->edgecount; e ++)
    {
        E = &O->edges[e / ARRAYSIZE][e % ARRAYSIZE];
        if (E->selected)
        {
            for (v = 0; v < 2; v ++)
            {
                idx = E->verts[v];
                V = &O->verts[idx / ARRAYSIZE][idx % ARRAYSIZE];
                condition = 1;
                for (i = 0; i < V->edgecount; i ++)
                {
                    idx = V->edges[i];
                    E0 = &O->edges[idx / ARRAYSIZE][idx % ARRAYSIZE];
                    if (E0 != E && E0->selected)
                    {
                        condition = 0;
                        break;
                    }
                }
                if (condition)
                {
                    V->selected = 1;
                    for (u = 0; u < V->uv_vertcount; u ++)
                    {
                        idx = V->uv_verts[u];
                        if (idx > -1 && idx < O->textcount)
                        {
                            UV = &O->uvtex[idx / ARRAYSIZE][idx % ARRAYSIZE];
                            UV->selected = V->selected;
                        }
                    }
                }
            }
        }
    }
}

void convert_To_Border_Edges(object * O)
{
    Draw_Bottom_Message("convert To Border Edges\n");
    int v, e, idx;

    uv_edge * UVE;
    edge * E;

    polygon * P0, * P1;

    for(e = 0; e < O->edgecount; e ++)
    {
        E = &O->edges[e / ARRAYSIZE][e % ARRAYSIZE];
        E->selected = 0;
    }

    for(e = 0; e < O->uvedcount; e ++)
    {
        UVE = &O->uveds[e / ARRAYSIZE][e % ARRAYSIZE];
        UVE->selected = 0;
    }

    for(e = 0; e < O->edgecount; e ++)
    {
        E = &O->edges[e / ARRAYSIZE][e % ARRAYSIZE];
        if (E->polycount > 1)
        {
            idx = E->polys[0];
            P0 = &O->polys[idx / ARRAYSIZE][idx % ARRAYSIZE];
            idx = E->polys[1];
            P1 = &O->polys[idx / ARRAYSIZE][idx % ARRAYSIZE];
            if ((P0->selected && !P1->selected) || (!P0->selected && P1->selected))
            {
                E->selected = 1;
                for (v = 0; v < E->uv_edcount; v ++)
                {
                    idx = E->uv_edges[v];
                    if (idx > -1 && idx < O->uvedcount)
                    {
                        UVE = &O->uveds[idx / ARRAYSIZE][idx % ARRAYSIZE];
                        UVE->selected = E->selected;
                    }
                }
            }
        }
        else
        {
            idx = E->polys[0];
            P0 = &O->polys[idx / ARRAYSIZE][idx % ARRAYSIZE];
            if (P0->selected)
            {
                E->selected = 1;
                for (v = 0; v < E->uv_edcount; v ++)
                {
                    idx = E->uv_edges[v];
                    if (idx > -1 && idx < O->uvedcount)
                    {
                        UVE = &O->uveds[idx / ARRAYSIZE][idx % ARRAYSIZE];
                        UVE->selected = E->selected;
                    }
                }
            }
        }
    }
}

void select_Loop()
{
    Draw_Bottom_Message("select Loop\n");
    if (Polygon_Mode)
    {
        printf("Polygon Mode\n");
        int idx, idx0, idx1, e0, e1;

        edge * E;
        polygon * P, * P0, * P1;

        int index, condition, selected;

        if (O->selected_polys_count > 1)
        {
            printf("selected polys count %d\n", O->selected_polys_count);
            idx0 = O->last_selected_polys[0];
            idx1 = O->last_selected_polys[1];
            P0 = &O->polys[idx0 / ARRAYSIZE][idx0 % ARRAYSIZE];
            P1 = &O->polys[idx1 / ARRAYSIZE][idx1 % ARRAYSIZE];
            condition = 0;
            for (e0 = 0; e0 < P0->edgecount; e0 ++)
            {
                idx0 = P0->edges[e0];
                for (e1 = 0; e1 < P1->edgecount; e1 ++)
                {
                    idx1 = P1->edges[e1];
                    if (idx0 == idx1)
                    {
                        index = e1;
                        condition = 1;
                        break;
                    }
                }
            }
            if (condition)
            {
                printf("condition %d\n", condition);
                P = P1;
                selected = 1;
                do
                {
                    idx = P->edges[(index + 2) % P->edgecount];
                    E = &O->edges[idx / ARRAYSIZE][idx % ARRAYSIZE];
                    if (E->polycount > 1)
                    {
                        if (E->polys[0] == P->index)
                        {
                            idx = E->polys[1];
                            index = E->slots[1];
                        }
                        else
                        {
                            idx = E->polys[0];
                            index = E->slots[0];
                        }
                        P = &O->polys[idx / ARRAYSIZE][idx % ARRAYSIZE];
                        if (P->selected)
                            selected = 1;
                        else
                            selected = 0;
                        P->selected = 1;
                        if (P->edgecount != 4)
                            break;
                    }
                }
                while (!selected && E->polycount > 1);
            }
        }
        int o, l;

        for (o = 0; o < selected_object_count; o ++)
        {
            O = objects[selected_objects[o]];
            assign_Selection_sel(O);
        }
        for (l = 0; l < SUBD; l++)
            load_id_colors(selected_objects, selected_object_count, l, OBJECT_COLORS);
        load_id_colors_Fan(selected_objects, selected_object_count, OBJECT_COLORS);
        UPDATE_COLORS = 1;
    }
    else if (Edge_Mode)
    {
        printf("Edge Mode\n");
        // find if two last edges share a vertex
        int v, e;
        int idx, idx0, idx1;

        vertex * V;
        edge * E, * E0, * E1;
        uv_edge * UVE;

        int index, condition, selected, open;

        if (O->selected_edges_count > 1)
        {
            printf("selected edges count %d\n", O->selected_edges_count);
            idx0 = O->last_selected_edges[0];
            idx1 = O->last_selected_edges[1];
            E0 = &O->edges[idx0 / ARRAYSIZE][idx0 % ARRAYSIZE];
            E1 = &O->edges[idx1 / ARRAYSIZE][idx1 % ARRAYSIZE];
            condition = 0;
            if (E0->verts[1] == E1->verts[0])
            {
                condition = 1;
                index = E1->verts[1];
            }
            else if (E0->verts[0] == E1->verts[0])
            {
                condition = 1;
                index = E1->verts[1];
            }
            else if (E0->verts[1] == E1->verts[1])
            {
                condition = 1;
                index = E1->verts[0];
            }
            else if (E0->verts[0] == E1->verts[1])
            {
                condition = 1;
                index = E1->verts[0];
            }
            else
            {
                condition = 1;
                index = E1->verts[1];
            }
        }
        else
        {
            printf("selected edges count %d\n", O->selected_edges_count);
            idx1 = O->last_selected_edges[1];
            E1 = &O->edges[idx1 / ARRAYSIZE][idx1 % ARRAYSIZE];
            condition = 1;
            index = E1->verts[1];
        }
        if (condition)
        {
            printf("condition %d\n", condition);
            E = E1;
            idx0 = E->polys[0];
            idx1 = E->polys[1];
            selected = 1;
            open = 0;
            if (E->polycount != 2)
            {
                open = 1;
            }
            if (open)
            {
                do
                {
                    V = &O->verts[index / ARRAYSIZE][index % ARRAYSIZE];
                    open = 0;
                    for (e = 0; e < V->edgecount; e ++)
                    {
                        idx = V->edges[e];
                        E0 = &O->edges[idx / ARRAYSIZE][idx % ARRAYSIZE];
                        if (E0 != E && E0->polycount < 2)
                        {
                            E = E0;
                            open = 1;
                            break;
                        }
                    }
                    if (E->selected)
                        selected = 1;
                    else
                        selected = 0;
                    E->selected = 1;
                    for (v = 0; v < E->uv_edcount; v ++)
                    {
                        idx = E->uv_edges[v];
                        if (idx > -1 && idx < O->uvedcount)
                        {
                            UVE = &O->uveds[idx / ARRAYSIZE][idx % ARRAYSIZE];
                            UVE->selected = E->selected;
                        }
                    }
                    if (E->verts[0] == V->index)
                        index = E->verts[1];
                    else
                        index = E->verts[0];
                }
                while (!selected && open);
            }
            else
            {
                do
                {
                    V = &O->verts[index / ARRAYSIZE][index % ARRAYSIZE];
                    if (V->edgecount != 4)
                        break;
                    for (e = 0; e < V->edgecount; e ++)
                    {
                        idx = V->edges[e];
                        E0 = &O->edges[idx / ARRAYSIZE][idx % ARRAYSIZE];
                        if (E0->polycount != 2)
                        {
                            open = 1;
                            break;
                        }
                        else if (E0 != E)
                        {
                            if (idx0 == E0->polys[0] || idx0 == E0->polys[1] ||
                                idx1 == E0->polys[0] || idx1 == E0->polys[1])
                            {
                                continue;
                            }
                            else
                            {
                                idx0 = E0->polys[0];
                                idx1 = E0->polys[1];
                                E = E0;
                                break;
                            }
                        }
                    }
                    if (E->selected)
                        selected = 1;
                    else
                        selected = 0;
                    E->selected = 1;
                    for (v = 0; v < E->uv_edcount; v ++)
                    {
                        idx = E->uv_edges[v];
                        if (idx > -1 && idx < O->uvedcount)
                        {
                            UVE = &O->uveds[idx / ARRAYSIZE][idx % ARRAYSIZE];
                            UVE->selected = E->selected;
                        }
                    }
                    if (E->verts[0] == V->index)
                        index = E->verts[1];
                    else
                        index = E->verts[0];
                }
                while (!selected && !open);
            }
        }
    }
}

void select_Connected()
{
    if (Polygon_Mode)
    {
        int o, e, idx, p;

        edge * E;
        polygon * P, * P0, * P1;

        printf("Polygon selection type\n");
        object * O;

        int * selected_edges = malloc(0);
        int selected_edge_count;

        for (o = 0; o < selected_object_count; o ++)
        {
            O = objects[selected_objects[o]];

            selected_edges = realloc(selected_edges, O->edgecount * sizeof(int));

            do
            {
                selected_edge_count = 0;

                for (e = 0; e < O->edgecount; e ++)
                {
                    E = &O->edges[e / ARRAYSIZE][e % ARRAYSIZE];
                    if (E->polycount > 1)
                    {
                        idx = E->polys[0];
                        P0 = &O->polys[idx / ARRAYSIZE][idx % ARRAYSIZE];
                        idx = E->polys[1];
                        P1 = &O->polys[idx / ARRAYSIZE][idx % ARRAYSIZE];
                        if ((P0->selected && !P1->selected) || (!P0->selected && P1->selected))
                            selected_edges[selected_edge_count ++] = e;
                    }
                }

                for (e = 0; e < selected_edge_count; e ++)
                {
                    idx = selected_edges[e];
                    E = &O->edges[idx / ARRAYSIZE][idx % ARRAYSIZE];
                    for (p = 0; p < E->polycount; p ++)
                    {
                        idx = E->polys[p];
                        P = &O->polys[idx / ARRAYSIZE][idx % ARRAYSIZE];
                        P->selected = 1;
                    }
                }
            }
            while (selected_edge_count > 0);
        }

        free(selected_edges);

        int l;

        for (o = 0; o < selected_object_count; o ++)
        {
            O = objects[selected_objects[o]];
            assign_Selection_sel(O);
        }
        for (l = 0; l < SUBD; l++)
            load_id_colors(selected_objects, selected_object_count, l, OBJECT_COLORS);
        load_id_colors_Fan(selected_objects, selected_object_count, OBJECT_COLORS);
        UPDATE_COLORS = 1;
    }
    else if (Edge_Mode)
    {
        int o, e, idx, v;

        edge * E;
        vertex * V;

        printf("Edge selection type\n");
        object * O;

        int * selected_verts = malloc(0);
        int selected_vert_count;

        int condition0;
        int condition1;

        for (o = 0; o < selected_object_count; o ++)
        {
            O = objects[selected_objects[o]];

            selected_verts = realloc(selected_verts, O->vertcount * sizeof(int));

            do
            {
                selected_vert_count = 0;

                for (v = 0; v < O->vertcount; v ++)
                {
                    V = &O->verts[v / ARRAYSIZE][v % ARRAYSIZE];
                    condition0 = 0;
                    condition1 = 0;
                    for (e = 0; e < V->edgecount; e ++)
                    {
                        idx = V->edges[e];
                        E = &O->edges[idx / ARRAYSIZE][idx % ARRAYSIZE];
                        if (E->selected)
                            condition0 = 1;
                        else if (!E->selected)
                            condition1 = 1;
                        if (condition0 && condition1)
                        {
                            selected_verts[selected_vert_count ++] = v;
                            break;
                        }
                    }
                }

                for (v = 0; v < selected_vert_count; v ++)
                {
                    idx = selected_verts[v];
                    V = &O->verts[idx / ARRAYSIZE][idx % ARRAYSIZE];
                    for (e = 0; e < V->edgecount; e ++)
                    {
                        idx = V->edges[e];
                        E = &O->edges[idx / ARRAYSIZE][idx % ARRAYSIZE];
                        E->selected = 1;
                    }
                }
            }
            while (selected_vert_count > 0);
        }
        free(selected_verts);
    }
    else if (Vertex_Mode)
    {
        int o, e, idx;

        edge * E;
        vertex * V0, * V1;

        printf("Vertex selection type\n");
        object * O;

        int * selected_edges = malloc(0);
        int selected_edge_count;

        for (o = 0; o < selected_object_count; o ++)
        {
            O = objects[selected_objects[o]];

            selected_edges = realloc(selected_edges, O->edgecount * sizeof(int));

            do
            {
                selected_edge_count = 0;

                for (e = 0; e < O->edgecount; e ++)
                {
                    E = &O->edges[e / ARRAYSIZE][e % ARRAYSIZE];
                    idx = E->verts[0];
                    V0 = &O->verts[idx / ARRAYSIZE][idx % ARRAYSIZE];
                    idx = E->verts[1];
                    V1 = &O->verts[idx / ARRAYSIZE][idx % ARRAYSIZE];
                    if ((V0->selected && !V1->selected) || (!V0->selected && V1->selected))
                        selected_edges[selected_edge_count ++] = e;
                }

                for (e = 0; e < selected_edge_count; e ++)
                {
                    idx = selected_edges[e];
                    E = &O->edges[idx / ARRAYSIZE][idx % ARRAYSIZE];
                    idx = E->verts[0];
                    V0 = &O->verts[idx / ARRAYSIZE][idx % ARRAYSIZE];
                    V0->selected = 1;
                    idx = E->verts[1];
                    V1 = &O->verts[idx / ARRAYSIZE][idx % ARRAYSIZE];
                    V1->selected = 1;
                }
            }
            while (selected_edge_count > 0);
        }
        free(selected_edges);
    }
}

void set_Button_sels(int idx)
{
    int b;
    for (b = 0; b < SELS_NUM; b ++)
    {
        Button_sels[b].color = UI_GRAYB;
    }
    Button_sels[idx].color = UI_GRAYD;
    memcpy(&sel_type, sel_types[idx], TYPE_LENGTH);
    if (SelsIndex[current_sel_type] - sels_start[current_sel_type] >= 0)
        SelsList[SelsIndex[current_sel_type] - sels_start[current_sel_type]].color = UI_BLACK;
    current_sel_type = idx;

    selection_Mode = idx;
    if (selection_Mode == OBJES)
    {
        set_Object_Mode();
    }
    else if (selection_Mode == POLYS)
    {
        set_Polygon_Mode();
    }
    else if (selection_Mode == EDGES)
    {
        set_Edge_Mode();
    }
    else if (selection_Mode == VERTS)
    {
        set_Vertex_Mode();
    }

    if (SelsIndex[current_sel_type] - sels_start[current_sel_type] >= 0)
        SelsList[SelsIndex[current_sel_type] - sels_start[current_sel_type]].color = UI_BACKL;
    if (UPDATE_SELECTIONS)
        create_Selections_List(selected_objects, selected_object_count, idx);

    if (!NVIDIA) glDrawBuffer(GL_FRONT_AND_BACK);
    if (dialog_lock)
    {
        if (dialog_type == SELS_DIALOG)
        {
            poly_Render(tripsRender, wireframe, splitview, CamDist, 0, subdLevel);
            draw_Selections_Dialog("Selections L.", screen_height, sel_type, sel_types, sel_type_count, sels_start[current_sel_type], 1, SelsIndex[current_sel_type] - sels_start[current_sel_type]);
        }
    }
    draw_Bottom_Line(screen_width, screen_height);
    display_bottom_message(bottom_message, screen_width, screen_height);

    message = -1;
}

void black_out_HierarchyList()
{
    int h;

    for (h = 0; h < Hierarchys_c; h ++)
    {
        HierList[h].color = UI_BLACK;
    }
    if (HierIndex - hier_start >= 0)
        HierList[HierIndex - hier_start].color = UI_BACKL;
}

void set_Button_defr(int idx)
{
    int b;
    for (b = 0; b < DEFR_NUM; b ++)
    {
        Button_defr[b].color = UI_GRAYB;
    }
    Button_defr[idx].color = UI_GRAYD;
    memcpy(&defr_type, defr_types[idx], TYPE_LENGTH);
    current_defr_type = idx;

    if (current_defr_type == 0)
    {
        create_Deformers_List(SelsIndex[3], O);
        set_Button_sels(3);
    }
    else if (current_defr_type == 1)
    {
        create_Hierarchys_List();
        HierIndex = currentLocator;
        black_out_HierarchyList();
        set_Button_sels(0);
    }
    else if (current_defr_type == 2)
    {
        create_Selections_List(selected_objects, selected_object_count, 3);
        set_Button_sels(3);
    }
}

void set_Button_text(int idx)
{
    int b;
    for (b = 0; b < TEXT_NUM; b ++)
    {
        Button_text[b].color = UI_GRAYB;
    }
    Button_text[idx].color = UI_GRAYD;
    memcpy(&text, texts[idx], TYPE_LENGTH);
}

void set_Button_item(int idx)
{
    int b;
    for (b = 0; b < ITEM_NUM; b ++)
    {
        Button_item[b].color = UI_GRAYB;
    }
    Button_item[idx].color = UI_GRAYD;
    memcpy(&item, items[idx], TYPE_LENGTH);
}

void set_Button_scene_ext(int idx, int type)
{
    int b;
    for (b = 0; b < SCENE_EXT_NUM; b ++)
    {
        Button_scene_ext[b].color = UI_GRAYB;
    }
    Button_scene_ext[idx].color = UI_GRAYD;
    memcpy(&scene_extension, scene_extensions[idx], 20);
}

void set_Button_ext(int idx, int type)
{
    int b;
    for (b = 0; b < EXT_NUM; b ++)
    {
        Button_ext[b].color = UI_GRAYB;
    }
    Button_ext[idx].color = UI_GRAYD;
    memcpy(&extension, extensions[idx], 4);
    if (type == OBJ_DIALOG)
        obj_extension_type = idx;
    else if (type == IMG_DIALOG)
        img_extension_type = idx;
}

void replace_bslash_with_fslash(char * fileName)
{
    char * p;

    for(p = fileName; * p; p ++)
    {
		if (* p == '\\')
        {
            * p = '/';
        }
    }
}

void open_IMG(char * fileName)
{
    replace_bslash_with_fslash(fileName);

    if (strlen(fileName) > 1)
    {
#if DEBUG_WITHOUT_IL == 0
                texture = load_ILU_image(fileName, NORMAL_IMAGE, BUMP_IMAGE);
#else
                texture = load_image(fileName, NORMAL_IMAGE, BUMP_IMAGE);
#endif
        surface_Material * M = &Materials[O->surface];

        if (texture != NULL)
        {
            if (NORMAL_IMAGE)
            {
                Surf_Norm[Surf_Norm_c] = texture;
                M->normal = Surf_Norm_c;
                sprintf(Norm_Names[Surf_Norm_c], basename(fileName));
                sprintf(Norm_Names_Full[Surf_Norm_c], fileName);
                Surf_Norm_c ++;
                M->Normal_idx = NormFrom_Surface(texture);

                sprintf(norm_Pack[norm_Pack_c].texture_Name, fileName);
                norm_Pack[norm_Pack_c].texture = M->normal;
                norm_Pack[norm_Pack_c].Texture_idx = M->Normal_idx;
                norm_Pack_c ++;

                Normals_c ++;

                update_tangents_Object(O, subdLevel);

                Draw_Bottom_Message("O normals\n");
            }
            else if (BUMP_IMAGE)
            {
                Surf_Bump[Surf_Bump_c] = texture;
                M->bump = Surf_Bump_c;
                sprintf(Bump_Names[Surf_Bump_c], basename(fileName));
                sprintf(Bump_Names_Full[Surf_Bump_c], fileName);
                Surf_Bump_c ++;
                M->Bump_idx = BumpFrom_Surface(texture);

                sprintf(bump_Pack[bump_Pack_c].texture_Name, fileName);
                bump_Pack[bump_Pack_c].texture = M->bump;
                bump_Pack[bump_Pack_c].Texture_idx = M->Bump_idx;
                bump_Pack_c ++;

                Bumps_c ++;

                Draw_Bottom_Message("O bumps\n");
            }
            else
            {
                Surf_Text[Surf_Text_c] = texture;
                M->texture = Surf_Text_c;
                sprintf(Text_Names[Surf_Text_c], basename(fileName));
                sprintf(Text_Names_Full[Surf_Text_c], fileName);
                Surf_Text_c ++;
                M->Texture_idx = TexFrom_Surface(texture);

                sprintf(text_Pack[text_Pack_c].texture_Name, fileName);
                text_Pack[text_Pack_c].texture = M->texture;
                text_Pack[text_Pack_c].Texture_idx = M->Texture_idx;
                text_Pack_c ++;

                Textures_c ++;

                assign_Texture(O);
                Draw_Bottom_Message("O surface\n");
            }
        }
        else
        {
            if (NORMAL_IMAGE)
            {
                place_norm_Pack(M, fileName);
            }
            else if (BUMP_IMAGE)
            {
                place_bump_Pack(M, fileName);
            }
            else
            {
                place_text_Pack(M, fileName);
                assign_Texture(O);
            }
        }
    }
    dialog_lock = 0;

    if (!Object_Mode)
    {
        if (add_selection_mode)
            SDL_SetCursor(Arrow_Plus);
        else
            SDL_SetCursor(Arrow_Minus);
    }
    message = -1;
}

void open_ImagesDir()
{
    char * fileName;
    fileName = open_FileName(gHwnd, images_dir, "Open Images");
    int len_f = strlen(fileName);
    if (len_f > 1)
    {
        int end_of_line = len_f;
        while (fileName[end_of_line] != '/' && fileName[end_of_line] != '\\')
        {
            end_of_line--;
            if (end_of_line <= 0)
            {
                break;
            }
        }
        if (end_of_line > 0)
        {
            memcpy(images_dir, fileName, end_of_line + 1);
            images_dir[end_of_line + 1] = '\0';
            files_dir = (char *)&images_dir;
        }
    }
    Draw_Bottom_Message("images dir\n");
    png_files_start = 0;
    jpg_files_start = 0;
    tif_files_start = 0;
    PSD_files_start = 0;
    files_start = 0;
//    SetFocus(gHwnd);
}

void open_ObjectsDir()
{
    char * fileName;
    fileName = open_FileName(gHwnd, objects_dir, "Open Objects");
    int len_f = strlen(fileName);
    if (len_f > 1)
    {
        int end_of_line = len_f;
        while (fileName[end_of_line] != '/' && fileName[end_of_line] != '\\')
        {
            end_of_line--;
            if (end_of_line <= 0)
            {
                break;
            }
        }
        if (end_of_line > 0)
        {
            memcpy(objects_dir, fileName, end_of_line + 1);
            objects_dir[end_of_line + 1] = '\0';
            files_dir = (char *)objects_dir;
        }
    }
    Draw_Bottom_Message("objects dir\n");
    obj_files_start = 0;
    files_start = 0;
//    SetFocus(gHwnd);
}

void open_Img()
{
    char * fileName;
    fileName = open_FileName(gHwnd, images_dir, "Open Images");

    replace_bslash_with_fslash(fileName);

    int len_f = strlen(fileName);
    if (len_f > 1)
    {
        int end_of_line = len_f;
        while (fileName[end_of_line] != '/' && fileName[end_of_line] != '\\')
        {
            end_of_line--;
            if (end_of_line <= 0)
            {
                break;
            }
        }
        if (end_of_line > 0)
        {
            memcpy(images_dir, fileName, end_of_line + 1);
            images_dir[end_of_line + 1] = '\0';
            files_dir = (char *)&images_dir;
        }
    }
    Draw_Bottom_Message("images dir\n");
    if (strlen(fileName) > 1)
    {
#if DEBUG_WITHOUT_IL == 0
                texture = load_ILU_image(fileName, NORMAL_IMAGE, BUMP_IMAGE);
#else
                texture = load_image(fileName, NORMAL_IMAGE, BUMP_IMAGE);
#endif
        surface_Material * M = &Materials[O->surface];

        if (texture != NULL)
        {
            if (NORMAL_IMAGE)
            {
                Surf_Norm[Surf_Norm_c] = texture;
                M->normal = Surf_Norm_c;
                sprintf(Norm_Names[Surf_Norm_c], basename(fileName));
                sprintf(Norm_Names_Full[Surf_Norm_c], fileName);
                Surf_Norm_c ++;
                M->Normal_idx = NormFrom_Surface(texture);

                sprintf(norm_Pack[norm_Pack_c].texture_Name, fileName);
                norm_Pack[norm_Pack_c].texture = M->normal;
                norm_Pack[norm_Pack_c].Texture_idx = M->Normal_idx;
                norm_Pack_c ++;

                Normals_c ++;

                update_tangents_Object(O, subdLevel);

                Draw_Bottom_Message("O normals\n");
            }
            else if (BUMP_IMAGE)
            {
                Surf_Bump[Surf_Bump_c] = texture;
                M->bump = Surf_Bump_c;
                sprintf(Bump_Names[Surf_Bump_c], basename(fileName));
                sprintf(Bump_Names_Full[Surf_Bump_c], fileName);
                Surf_Bump_c ++;
                M->Bump_idx = BumpFrom_Surface(texture);

                sprintf(bump_Pack[bump_Pack_c].texture_Name, fileName);
                bump_Pack[bump_Pack_c].texture = M->bump;
                bump_Pack[bump_Pack_c].Texture_idx = M->Bump_idx;
                bump_Pack_c ++;

                Bumps_c ++;

                Draw_Bottom_Message("O bumps\n");
            }
            else
            {
                Surf_Text[Surf_Text_c] = texture;
                M->texture = Surf_Text_c;
                sprintf(Text_Names[Surf_Text_c], basename(fileName));
                sprintf(Text_Names_Full[Surf_Text_c], fileName);
                Surf_Text_c ++;
                M->Texture_idx = TexFrom_Surface(texture);

                sprintf(text_Pack[text_Pack_c].texture_Name, fileName);
                text_Pack[text_Pack_c].texture = M->texture;
                text_Pack[text_Pack_c].Texture_idx = M->Texture_idx;
                text_Pack_c ++;

                Textures_c ++;

                assign_Texture(O);
                Draw_Bottom_Message("O surface\n");
            }
        }
        else
        {
            if (NORMAL_IMAGE)
            {
                place_norm_Pack(M, fileName);
            }
            else if (BUMP_IMAGE)
            {
                place_bump_Pack(M, fileName);
            }
            else
            {
                place_text_Pack(M, fileName);
                assign_Texture(O);
            }
        }
    }
//    SetFocus(0);
//    SetFocus(gHwnd);
}

void deselect_Objects()
{
    int o;
    object * O;
    for (o = 0; o < objectIndex; o ++)
    {
        O = objects[o];
        O->selected = 0;
    }
    O = objects[currentObject];
    O->selected = 1;
    selected_object_count = 1;
    selected_objects[0] = currentObject;
}

void open_OBJ_dialog()
{
    int result;
    char * fileName;
    fileName = open_FileName(gHwnd, objects_dir, "Open objects");
    int len_f = strlen(fileName);
    if (len_f > 1)
    {
        int end_of_line = len_f;
        while (fileName[end_of_line] != '/' && fileName[end_of_line] != '\\')
        {
            end_of_line--;
            if (end_of_line <= 0)
            {
                break;
            }
        }
        if (end_of_line > 0)
        {
            memcpy(objects_dir, fileName, end_of_line + 1);
            objects_dir[end_of_line + 1] = '\0';
            files_dir = (char *)&objects_dir;
        }
    }
    Draw_Bottom_Message("objects dir\n");
    OBJ_File = read_OBJ_file(fileName);

    if (OBJ_File.initialized)
    {
        result = create_Object_OBJ(fileName, &OBJ_File);

        Draw_Bottom_Message("OBJ create result\n");

        if (result)
        {
            currentLocator = transformerIndex - 1;
            currentObject = objectIndex - 1;
            deselect_Objects();
            subdLevel = -1;
        }
    }
//    SetFocus(0);
//    SetFocus(gHwnd);
}

void open_OBJ(char * fileName)
{
    int result;
    OBJ_File = read_OBJ_file(fileName);

    if (OBJ_File.initialized)
    {
        result = create_Object_OBJ(fileName, &OBJ_File);

        Draw_Bottom_Message("OBJ create result\n");

        if (result)
        {
            currentLocator = transformerIndex - 1;
            currentObject = objectIndex - 1;

            deselect_Objects();

            subdLevel = -1;
        }
    }
    dialog_lock = 0;

    if (!Object_Mode)
    {
        if (add_selection_mode)
            SDL_SetCursor(Arrow_Plus);
        else
            SDL_SetCursor(Arrow_Minus);
    }
    message = -1;
}

int list_materials(char ** material_list, int start, int n)
{
    int m = start;
    int material = 0;
    for (material = 0; material < n; material ++)
    {
        if (material >= Materials_count - start)
            break;
        sprintf(material_list[material], "%s", Materials[m].Name);
        m ++;
    }
    return material;
}

int list_ik(char ** ikch_list, int start, int n)
{
    int i = start;
    int chain = 0;
    for (chain = 0; chain < n; chain ++)
    {
        if (chain >= ikChains_c - start)
            break;
        sprintf(ikch_list[chain], "%s", IK_Names[i]);
        i ++;
    }
    return chain;
}

int list_bones(char ** bones_list, int start, int n)
{
    int b = start;
    int bone = 0;
    for (bone = 0; bone < n; bone ++)
    {
        if (bone >= Bones_c - start)
            break;
        sprintf(bones_list[bone], "%s", Bone_Names[b]);
        b ++;
    }
    return bone;
}

int list_poses(char ** poses_list, int start, int n, int * pose_x_offset, int * pose_x_collapsed)
{
    int p = start;
    int pose = 0;
    for (pose = 0; pose < n; pose ++)
    {
        if (pose >= Poses_c - start)
            break;
        sprintf(poses_list[pose], "%s", Pose_Names[p]);
        pose_x_offset[pose] = Pose_X_Offset[p];
        pose_x_collapsed[pose] = Pose_X_Collapsed[p];
        p ++;
    }
    return pose;
}

int list_deformers(char ** deformers_list, int start, int n, int * defr_x_offset, int * defr_x_collapsed)
{
    int d = start;
    int def = 0;
    for (def = 0; def < n; def ++)
    {
        if (def >= Deformers_c - start)
            break;
        sprintf(deformers_list[def], "%s", Defr_Names[d]);
        defr_x_offset[def] = Defr_X_Offset[d];
        defr_x_collapsed[def] = Defr_X_Collapsed[d];
        d ++;
    }
    return def;
}

int list_hierarcys(char ** hierarchys_list, int start, int n, int * hier_x_offset, int * hier_x_collapsed, int * hier_italic)
{
    int h = start;
    int hir = 0;
    for (hir = 0; hir < n; hir ++)
    {
        if (hir >= Hierarchys_c - start)
            break;
        sprintf(hierarchys_list[hir], "%s", Hier_Names[h]);
        hier_x_offset[hir] = Hier_X_Offset[h];
        hier_x_collapsed[hir] = Hier_X_Collapsed[h];
        hier_italic[hir] = Hier_Italic[h];
        h ++;
    }
    return hir;
}

int list_selections(char ** selections_list, int start, int n, char * type)
{
    int s = start;
    int sel = 0;
    for (sel = 0; sel < n; sel ++)
    {
        if (sel >= Selections_c[current_sel_type])
            break;
        sprintf(selections_list[sel], "%s|%s", Sels_Names[current_sel_type][s], Sels_Counts[current_sel_type][s]);
        s ++;
    }
    return sel;
}

int list_texts(char ** texts_list, int start, int n, const char * type)
{
    int s = start;
    int t = 0;
    if (strcmp(type, TEXT_TYPE_TEXT) == 0)
    {
        for (t = 0; t < n; t++)
        {
            if (s >= Textures_c)
                break;
            sprintf(texts_list[t], "%s", Text_Names[s]);
            s ++;
        }
    }
    else if (strcmp(type, TEXT_TYPE_NORM) == 0)
    {
        for (t = 0; t < n; t++)
        {
            if (s >= Normals_c)
                break;
            sprintf(texts_list[t], "%s", Norm_Names[s]);
            s ++;
        }
    }
    else if (strcmp(type, TEXT_TYPE_BUMP) == 0)
    {
        for (t = 0; t < n; t++)
        {
            if (s >= Bumps_c)
                break;
            sprintf(texts_list[t], "%s", Bump_Names[s]);
            s ++;
        }
    }
    return t;
}

void black_out_SelectionsList()
{
    int s;

    for (s = 0; s < LISTLENGTH; s ++)
    {
        SelsList[s].color = UI_BLACK;
    }
}

void open_Selections_List()
{
//    int idx = 0;
//
//    if (selection_Mode == POLYS)
//    {
//        idx = 1;
//    }
//    else if (selection_Mode == EDGES)
//    {
//        idx = 2;
//    }
//    else if (selection_Mode == VERTS)
//    {
//        idx = 3;
//    }

    //set_Button_sels(idx);
    set_Button_sels(current_sel_type);

    black_out_SelectionsList();

//    if (UPDATE_SELECTIONS)
//        create_Selections_List(selected_objects, selected_object_count, idx);


    printf("selections num %d\n", Selections_c[current_sel_type]);

    if (Bottom_Message)
    {
        Draw_Bottom_Message("Selections List\n");
    }
    Bottom_Message = 0;

    SDL_SetCursor(Arrow);
    dialog_lock = 1;
    dialog_type = SELS_DIALOG;
    if (!NVIDIA) glDrawBuffer(GL_FRONT_AND_BACK);
    UPDATE_COLORS = 1;
    poly_Render(tripsRender, wireframe, splitview, CamDist, 0, subdLevel);
    UPDATE_COLORS = 0;

    if (SelsIndex[current_sel_type] - sels_start[current_sel_type] >= 0 && SelsIndex[current_sel_type] - sels_start[current_sel_type] < LISTLENGTH)
    {
        if (SelsIndex[current_sel_type] - sels_start[current_sel_type] >= 0)
            SelsList[SelsIndex[current_sel_type] - sels_start[current_sel_type]].color = UI_BACKL;
        draw_Selections_Dialog("Selections L.", screen_height, sel_type, sel_types, sel_type_count, sels_start[current_sel_type], 1, SelsIndex[current_sel_type] - sels_start[current_sel_type]);
        if (SelsIndex[current_sel_type] - sels_start[current_sel_type] >= 0)
            SelsList[SelsIndex[current_sel_type] - sels_start[current_sel_type]].color = UI_BLACK;
    }
    else
    {
        draw_Selections_Dialog("Selections L.", screen_height, sel_type, sel_types, sel_type_count, sels_start[current_sel_type], 1, SelsIndex[current_sel_type] - sels_start[current_sel_type]);
    }
    blit_ViewPort();
    glDrawBuffer(GL_BACK);
    SDL_GL_SwapBuffers();
    message = 0;
}

void black_out_IkList()
{
    int i;

    for (i = 0; i < ikChains_c; i ++)
    {
        IkchList[i].color = UI_BLACK;
    }
    if (IKIndex - ikch_start >= 0)
        IkchList[IKIndex - ikch_start].color = UI_BACKL;
}

void open_IK_List()
{
    create_Ik_List(IKIndex);

    if (Bottom_Message)
    {
        Draw_Bottom_Message("IK List\n");
    }
    Bottom_Message = 0;

    black_out_IkList();

    //DRAW_LOCATORS = 1;
    //LOCAT_ID_RENDER = 1;

    SDL_SetCursor(Arrow);
    dialog_lock = 1;
    dialog_type = IK_DIALOG;
    if (!NVIDIA) glDrawBuffer(GL_FRONT_AND_BACK);
    UPDATE_COLORS = 1;
    if (dialog_lock)
        poly_Render(tripsRender, wireframe, splitview, CamDist, 0, subdLevel);
    else
        poly_Render(tripsRender, wireframe, splitview, CamDist, 1, subdLevel);
    UPDATE_COLORS = 0;

    draw_IK_Dialog("IK List", screen_height, ikch_start, 1, IKIndex - ikch_start);

    glDrawBuffer(GL_BACK);
    SDL_GL_SwapBuffers();
    message = 0;
}

void black_out_BoneList()
{
    int b;

    for (b = 0; b < Bones_c; b ++)
    {
        BoneList[b].color = UI_BLACK;
    }
    if (BoneIndex - bone_start >= 0)
        BoneList[BoneIndex - bone_start].color = UI_BACKL;
}

void open_Bones_List()
{
    create_Bones_List(BoneIndex);

    if (Bottom_Message)
    {
        Draw_Bottom_Message("Bones List\n");
    }
    Bottom_Message = 0;

    black_out_BoneList();

    //DRAW_LOCATORS = 1;
    //LOCAT_ID_RENDER = 1;

    SDL_SetCursor(Arrow);
    dialog_lock = 1;
    dialog_type = BONE_DIALOG;
    if (!NVIDIA) glDrawBuffer(GL_FRONT_AND_BACK);
    UPDATE_COLORS = 1;
    if (dialog_lock)
        poly_Render(tripsRender, wireframe, splitview, CamDist, 0, subdLevel);
    else
        poly_Render(tripsRender, wireframe, splitview, CamDist, 1, subdLevel);
    UPDATE_COLORS = 0;

    draw_Bones_Dialog("Bones List", screen_height, bone_start, 1, BoneIndex - bone_start);

    glDrawBuffer(GL_BACK);
    SDL_GL_SwapBuffers();
    message = 0;
}

void black_out_PoseList()
{
    int p;

    for (p = 0; p < Poses_c; p ++)
    {
        PoseList[p].color = UI_BLACK;
    }
    if (PoseIndex - pose_start >= 0)
        PoseList[PoseIndex - pose_start].color = UI_BACKL;
}

int find_Pose_Index()
{
    int d, index = 0;

    deformer * D;

    for (d = 0; d < deformerIndex; d ++)
    {
        D = deformers[d];

        index ++;

        if (!D->collapsed)
        {
            if (d == currentDeformer_Node)
            {
                index += D->current_pose;
                break;
            }
            else
            {
                index += D->Poses_Count;
            }
        }
    }
    return index;
}

void open_Poses_List()
{
    PoseIndex = find_Pose_Index();
    create_Poses_List(PoseIndex);
    currentPose = Pose_List[PoseIndex];

    if (Bottom_Message)
    {
        Draw_Bottom_Message("Poses List\n");
    }
    Bottom_Message = 0;

    black_out_PoseList();

    set_Pose_H_Button(-1);

    //DRAW_LOCATORS = 1;
    //LOCAT_ID_RENDER = 1;

    SDL_SetCursor(Arrow);
    dialog_lock = 1;
    dialog_type = POSE_DIALOG;
    if (!NVIDIA) glDrawBuffer(GL_FRONT_AND_BACK);
    UPDATE_COLORS = 1;
    if (dialog_lock)
        poly_Render(tripsRender, wireframe, splitview, CamDist, 0, subdLevel);
    else
        poly_Render(tripsRender, wireframe, splitview, CamDist, 1, subdLevel);
    UPDATE_COLORS = 0;

    draw_Poses_Dialog("Poses List", screen_height, pose_start, 1, PoseIndex - pose_start);

    glDrawBuffer(GL_BACK);
    SDL_GL_SwapBuffers();
    message = 0;
}

void black_out_DefrList()
{
    int d;

    for (d = 0; d < Deformers_c; d ++)
    {
        DefrList[d].color = UI_BLACK;
    }
}

void open_Deformers_List()
{
    //create_Deformers_List(SelsIndex[3], O);

    if (Bottom_Message)
    {
        Draw_Bottom_Message("Deformers List\n");
    }
    Bottom_Message = 0;

    //DefrIndex = selected_deformer_node;

    black_out_DefrList();

    set_Button_defr(current_defr_type);

    DRAW_LOCATORS = 1;
    LOCAT_ID_RENDER = 1;

    SDL_SetCursor(Arrow);
    dialog_lock = 1;
    dialog_type = DEFR_DIALOG;
    if (!NVIDIA) glDrawBuffer(GL_FRONT_AND_BACK);
    UPDATE_COLORS = 1;
    if (dialog_lock)
        poly_Render(tripsRender, wireframe, splitview, CamDist, 0, subdLevel);
    else
        poly_Render(tripsRender, wireframe, splitview, CamDist, 1, subdLevel);
    UPDATE_COLORS = 0;

    draw_Deformers_Dialog("Deformers L.", screen_height, defr_type, defr_types, defr_type_count,
                          defr_start, 1, DefrIndex - defr_start,
                          hier_start, HierIndex - hier_start,
                          sels_start[current_sel_type], SelsIndex[current_sel_type] - sels_start[current_sel_type]);

    glDrawBuffer(GL_BACK);
    SDL_GL_SwapBuffers();
    message = 0;
}

void open_Hierarchys_List()
{
    create_Hierarchys_List();

    if (Bottom_Message)
    {
        Draw_Bottom_Message("Hierarchys List\n");
    }
    Bottom_Message = 0;

    //printf("Transformers num %d\n", Hierarchys_c);

    HierIndex = currentLocator;

    black_out_HierarchyList();

    DRAW_LOCATORS = 1;
    LOCAT_ID_RENDER = DRAW_LOCATORS;

    selection_Mode = OBJES;
    set_Object_Mode();

    SDL_SetCursor(Arrow);
    dialog_lock = 1;
    dialog_type = HIER_DIALOG;
    if (!NVIDIA) glDrawBuffer(GL_FRONT_AND_BACK);
    UPDATE_COLORS = 1;
    if (dialog_lock)
        poly_Render(tripsRender, wireframe, splitview, CamDist, 0, subdLevel);
    else
        poly_Render(tripsRender, wireframe, splitview, CamDist, 1, subdLevel);
    UPDATE_COLORS = 0;

    draw_Hierarchys_Dialog("Hierarchys L.", screen_height, hier_start, 1, HierIndex - hier_start);

    glDrawBuffer(GL_BACK);
    SDL_GL_SwapBuffers();
    message = 0;
}

void black_out_TextList()
{
    int i;

    for (i = 0; i < MAX_LISTLENGTH; i ++)
    {
        TextList[i].color = UI_BLACK;
    }
}

void open_Textures_List()
{
    if (Bottom_Message)
    {
        Draw_Bottom_Message("Textures List\n");
    }
    Bottom_Message = 0;

    black_out_TextList();

    if (Textures_c > 0)
    {
        SDL_SetCursor(Arrow);
        dialog_lock = 1;
        dialog_type = TEXT_DIALOG;
        if (!NVIDIA) glDrawBuffer(GL_FRONT_AND_BACK);
        UPDATE_COLORS = 1;
        if (dialog_lock)
            poly_Render(tripsRender, wireframe, splitview, CamDist, 0, subdLevel);
        else
            poly_Render(tripsRender, wireframe, splitview, CamDist, 1, subdLevel);
        UPDATE_COLORS = 0;

        if (strcmp(text, TEXT_TYPE_TEXT) == 0)
        {
            TextIndex = Materials[O->surface].Texture_idx;
            texts_start = text_start;
        }
        else if (strcmp(text, TEXT_TYPE_NORM) == 0)
        {
            TextIndex = Materials[O->surface].Normal_idx;
            texts_start = norm_start;
        }
        else if (strcmp(text, TEXT_TYPE_BUMP) == 0)
        {
            TextIndex = Materials[O->surface].Bump_idx;
            texts_start = bump_start;
        }

        if (TextIndex - texts_start >= 0 && TextIndex - texts_start < MAX_LISTLENGTH)
        {
            TextList[TextIndex - texts_start].color = UI_BACKL;
        }

        draw_Textures_Dialog("Textures List", screen_height, text, texts, texts_count, texts_start, 1, currentObject);

        glDrawBuffer(GL_BACK);
        SDL_GL_SwapBuffers();
        message = 0;
    }
}

void black_out_MaterialsList()
{
    int i;

    for (i = 0; i < MAX_LISTLENGTH; i ++)
    {
        MatrList[i].color = UI_BLACK;
    }
    if (currentMaterial - materials_start >= 0 && currentMaterial - materials_start < LISTLENGTH)
        MatrList[currentMaterial - materials_start].color = UI_BACKL;
}

void open_Materials_List()
{
    if (Bottom_Message)
    {
        Draw_Bottom_Message("Materials List\n");
    }
    Bottom_Message = 0;

    black_out_MaterialsList();

    SDL_SetCursor(Arrow);
    dialog_lock = 1;
    dialog_type = MATERIAL_DIALOG;
    if (!NVIDIA) glDrawBuffer(GL_FRONT_AND_BACK);
    UPDATE_COLORS = 1;
    if (dialog_lock)
        poly_Render(tripsRender, wireframe, splitview, CamDist, 0, subdLevel);
    else
        poly_Render(tripsRender, wireframe, splitview, CamDist, 1, subdLevel);
    UPDATE_COLORS = 0;

    draw_Materials_Dialog("Materials L.", screen_height, materials_start, 1, currentObject);

    glDrawBuffer(GL_BACK);
    SDL_GL_SwapBuffers();
    message = 0;
}

void black_out_ItemsList()
{
    int i;

    for (i = 0; i < MAX_LISTLENGTH; i ++)
    {
        ItemList[i].color = UI_BLACK;
    }
    if (currentObject - items_start >= 0 && currentObject - items_start < LISTLENGTH)
        ItemList[currentObject - items_start].color = UI_BACKL;
}

void open_Items_List()
{
    if (Bottom_Message)
    {
        Draw_Bottom_Message("Items List\n");
    }
    Bottom_Message = 0;

    if (objectIndex > 0)
    {
        black_out_ItemsList();

        SDL_SetCursor(Arrow);
        edgedraw = 1;
        dialog_lock = 1;
        dialog_type = ITEM_DIALOG;
        if (!NVIDIA) glDrawBuffer(GL_FRONT_AND_BACK);
        UPDATE_COLORS = 1;
        if (dialog_lock)
            poly_Render(tripsRender, wireframe, splitview, CamDist, 0, subdLevel);
        else
            poly_Render(tripsRender, wireframe, splitview, CamDist, 1, subdLevel);
        UPDATE_COLORS = 0;

        draw_Items_Dialog("Items List", screen_height, item, items, items_count, items_start, 1, currentObject);

        glDrawBuffer(GL_BACK);
        SDL_GL_SwapBuffers();
        message = 0;
    }
}

void open_Saves_List()
{
    if (Bottom_Message)
    {
        Draw_Bottom_Message("Saves List\n");
    }
    Bottom_Message = 0;

    files_dir = (char *)&scene_files_dir;
    files_num = query_files(files_dir, scene_extension);
    printf("files_num %d\n", files_num);
    SDL_SetCursor(Arrow);
    dialog_lock = 1;
    dialog_type = SAVES_DIALOG;
    if (!NVIDIA) glDrawBuffer(GL_FRONT_AND_BACK);
    UPDATE_COLORS = 1;
    if (dialog_lock)
        poly_Render(tripsRender, wireframe, splitview, CamDist, 0, subdLevel);
    else
        poly_Render(tripsRender, wireframe, splitview, CamDist, 1, subdLevel);
    UPDATE_COLORS = 0;
    draw_Saves_Dialog(scene_files_dir, "Save scene", screen_height, scene_extension, scene_extensions, scene_ext_count, scene_files_start, 1, 1, Edit_Lock);
    glDrawBuffer(GL_BACK);
    SDL_GL_SwapBuffers();
    message = 0;
}

void export_OBJ_Format()
{
    int o;

    object * O0;

    Draw_Bottom_Message("export OBJ Format\n");

    DRAW_UI = 1;
    poly_Render(tripsRender, wireframe, splitview, CamDist, 1, subdLevel);

    for(o = 0; o < selected_object_count; o ++)
    {
        O0 = objects[selected_objects[o]];
        if (subdLevel > -1 && O0->subdlevel <= subdLevel)
        {
            write_OBJ_to_Disk_level_(O0, subdLevel);
        }
        else
        {
            write_OBJ_to_Disk(O0);
        }
    }
}

void open_Loading_List()
{
    if (Bottom_Message)
    {
        Draw_Bottom_Message("Loading List\n");
    }
    Bottom_Message = 0;

    files_dir = (char *)&scene_files_dir;
    files_num = query_files(files_dir, scene_extension);
    printf("files_num %d\n", files_num);
    SDL_SetCursor(Arrow);
    dialog_lock = 1;
    dialog_type = LOADING_DIALOG;
    if (!NVIDIA) glDrawBuffer(GL_FRONT_AND_BACK);
    UPDATE_COLORS = 1;
    if (dialog_lock)
        poly_Render(tripsRender, wireframe, splitview, CamDist, 0, subdLevel);
    else
        poly_Render(tripsRender, wireframe, splitview, CamDist, 1, subdLevel);
    UPDATE_COLORS = 0;
    draw_Loading_Dialog(scene_files_dir, "Load scene", screen_height, scene_extension, scene_extensions, scene_ext_count, scene_files_start, 1, 1, Edit_Lock);
    glDrawBuffer(GL_BACK);
    SDL_GL_SwapBuffers();
    message = 0;
}

void open_OBJ_List()
{
    if (Bottom_Message)
    {
        Draw_Bottom_Message("OBJ List\n");
    }
    Bottom_Message = 0;

    if (dialog_type != OBJ_DIALOG)
    {
        extensions[0] = ".obj";
        extensions[1] = ".OBJ";
        set_Button_ext(obj_extension_type, OBJ_DIALOG);
        ext_count = 2;
        if (obj_extension_type == obj_EXTENSION)
            files_start = obj_files_start;
        else if (obj_extension_type == OBJ_EXTENSION)
            files_start = OBJ_files_start;
    }
    files_dir = (char *)&objects_dir;
    files_num = query_files(files_dir, extension);
    printf("files num %d\n", files_num);

    SDL_SetCursor(Arrow);
    dialog_lock = 1;
    dialog_type = OBJ_DIALOG;
    if (!NVIDIA) glDrawBuffer(GL_FRONT_AND_BACK);
    UPDATE_COLORS = 1;
    if (dialog_lock)
        poly_Render(tripsRender, wireframe, splitview, CamDist, 0, subdLevel);
    else
        poly_Render(tripsRender, wireframe, splitview, CamDist, 1, subdLevel);
    UPDATE_COLORS = 0;
    draw_Files_Dialog(files_dir, "Open file", screen_height, extension, extensions, ext_count, files_start, 1, 1, Edit_Lock);
    glDrawBuffer(GL_BACK);
    SDL_GL_SwapBuffers();
    message = 0;
}

void open_Img_List()
{
    if (Bottom_Message)
    {
        Draw_Bottom_Message("Img List\n");
    }
    Bottom_Message = 0;

    if (dialog_type != IMG_DIALOG)
    {
        extensions[0] = ".png";
        extensions[1] = ".jpg";
        extensions[2] = ".tif";
        extensions[3] = ".PSD";

        set_Button_ext(img_extension_type, IMG_DIALOG);

        ext_count = 4;
        if (img_extension_type == png_EXTENSION)
            files_start = png_files_start;
        else if (img_extension_type == jpg_EXTENSION)
            files_start = jpg_files_start;
        else if (img_extension_type == tif_EXTENSION)
            files_start = tif_files_start;
        else if (img_extension_type == PSD_EXTENSION)
            files_start = PSD_files_start;
    }
    files_dir = (char *)&images_dir;
    files_num = query_files(files_dir, extension);
    printf("files num %d\n", files_num);

    SDL_SetCursor(Arrow);
    dialog_lock = 1;
    dialog_type = IMG_DIALOG;
    if (!NVIDIA) glDrawBuffer(GL_FRONT_AND_BACK);
    UPDATE_COLORS = 1;
    if (dialog_lock)
        poly_Render(tripsRender, wireframe, splitview, CamDist, 0, subdLevel);
    else
        poly_Render(tripsRender, wireframe, splitview, CamDist, 1, subdLevel);
    UPDATE_COLORS = 0;
    draw_Files_Dialog(files_dir, "Open file", screen_height, extension, extensions, ext_count, files_start, 1, 1, Edit_Lock);
    glDrawBuffer(GL_BACK);
    SDL_GL_SwapBuffers();
    message = 0;
}

void open_Bump_List()
{
    NORMAL_IMAGE = 0;
    BUMP_IMAGE = 1;
    open_Img_List();
}

void open_Norm_List()
{
    NORMAL_IMAGE = 1;
    BUMP_IMAGE = 0;
    open_Img_List();
}

void open_Text_List()
{
    NORMAL_IMAGE = 0;
    BUMP_IMAGE = 0;
    open_Img_List();
}

void collapse_Lists()
{
    if (!dialog_lock)
        UI_LISTS = !UI_LISTS;
}

void collapse_Files()
{
    if (!dialog_lock)
        UI_FILES = !UI_FILES;
}

void collapse_Clear()
{
    if (!dialog_lock)
        UI_CLEAR = !UI_CLEAR;
}

void assert_Deformers_Selected()
{
    int d;

    for (d = 0; d < deformerIndex; d ++)
    {
        if (d == currentDeformer_Node)
        {
            deformers[d]->selected = 1;
        }
        else
        {
            deformers[d]->selected = 0;
        }
    }
    if (currentDeformer_Node >= 0 && currentDeformer_Node < deformerIndex)
    {
        D = deformers[currentDeformer_Node];
    }
}

void clear_Verts_Selection(object * O)
{
    int u, v, idx;
    uv * UV;
    vertex * V;
    for (v = 0; v < O->vertcount; v ++)
    {
        V = &O->verts[v / ARRAYSIZE][v % ARRAYSIZE];
        V->selected = 0;
        for (u = 0; u < V->uv_vertcount; u ++)
        {
            idx = V->uv_verts[u];
            if (idx > -1 && idx < O->textcount)
            {
                UV = &O->uvtex[idx / ARRAYSIZE][idx % ARRAYSIZE];
                UV->selected = 0;
            }
        }
    }
}

void clear_Edge_Selection(object * O)
{
    int e, u, idx;
    edge * E;
    uv_edge * UVE;

    for (e = 0; e < O->edgecount; e ++)
    {
        E = &O->edges[e / ARRAYSIZE][e % ARRAYSIZE];
        E->selected = 0;
        for (u = 0; u < E->uv_edcount; u ++)
        {
            idx = E->uv_edges[u];
            if (idx > -1 && idx < O->uvedcount)
            {
                UVE = &O->uveds[idx / ARRAYSIZE][idx % ARRAYSIZE];
                UVE->selected = 0;
            }
        }
    }
}

void assign_No_Surface(object * O)
{
    int p;
    polygon * P;
    for (p = 0; p < O->polycount; p ++)
    {
        P = &O->polys[p / ARRAYSIZE][p % ARRAYSIZE];
        if (P->surface != O->surface)
        {
            assignSurfToQuads(O, P, O->surface);
        }
    }
}

void update_Deformers_List(int blit)
{
    if (DefrIndex - defr_start >= 0)
        DefrList[DefrIndex - defr_start].color = UI_BACKL;

    if (blit)
    {
        blit_ViewPort();
    }

    if (!NVIDIA) glDrawBuffer(GL_FRONT_AND_BACK);

    draw_Deformers_Dialog("Deformers L.", screen_height, defr_type, defr_types, defr_type_count,
                    defr_start, 1, DefrIndex - defr_start,
                    hier_start, HierIndex - hier_start,
                    sels_start[current_sel_type], SelsIndex[current_sel_type] - sels_start[current_sel_type]);

    SDL_GL_SwapBuffers();
    glDrawBuffer(GL_BACK);
}

void update_Loading_List(int update, int blit)
{
    if (blit)
    {
        blit_ViewPort();
    }

    if (!NVIDIA) glDrawBuffer(GL_FRONT_AND_BACK);

    if (update)
        draw_Loading_Dialog(scene_files_dir, "Load scene", screen_height, scene_extension, scene_extensions, scene_ext_count, scene_files_start, 1, 1, Edit_Lock);
    else
        draw_Loading_Dialog(scene_files_dir, "Load scene", screen_height, scene_extension, scene_extensions, scene_ext_count, scene_files_start, 1, 0, Edit_Lock);

    SDL_GL_SwapBuffers();
    glDrawBuffer(GL_BACK);
}

void update_Saves_List(int update, int blit)
{
    if (blit)
    {
        blit_ViewPort();
    }

    if (!NVIDIA) glDrawBuffer(GL_FRONT_AND_BACK);

    if (update)
        draw_Saves_Dialog(scene_files_dir, "Save scene", screen_height, scene_extension, scene_extensions, scene_ext_count, scene_files_start, 1, 1, Edit_Lock);
    else
        draw_Saves_Dialog(scene_files_dir, "Save scene", screen_height, scene_extension, scene_extensions, scene_ext_count, scene_files_start, 1, 0, Edit_Lock);

    SDL_GL_SwapBuffers();
    glDrawBuffer(GL_BACK);
}

void update_IK_List(int update, int blit)
{
    if (IKIndex - ikch_start >= 0)
        IkchList[IKIndex - ikch_start].color = UI_BACKL;

    if (blit)
    {
        blit_ViewPort();
    }

    if (!NVIDIA) glDrawBuffer(GL_FRONT_AND_BACK);
    if (UPDATE_BACKGROUND || update)
    {
        draw_IK_Dialog("IK List", screen_height, ikch_start, 1, IKIndex - ikch_start);
    }
    else
    {
        draw_IK_List(screen_height, ikch_start, 0, IKIndex - ikch_start);
        draw_IK_Bottom_Line(DIALOG_WIDTH, screen_height);
    }
    SDL_GL_SwapBuffers();
    glDrawBuffer(GL_BACK);
}

void update_Bones_List(int update, int blit)
{
    if (BoneIndex - bone_start >= 0)
        BoneList[BoneIndex - bone_start].color = UI_BACKL;

    if (blit)
    {
        blit_ViewPort();
    }

    if (!NVIDIA) glDrawBuffer(GL_FRONT_AND_BACK);
    if (UPDATE_BACKGROUND || update)
    {
        draw_Bones_Dialog("Bones List", screen_height, bone_start, 1, BoneIndex - bone_start);
    }
    else
    {
        draw_Bones_List(screen_height, bone_start, 0, BoneIndex - bone_start);
        draw_Bones_Bottom_Line(DIALOG_WIDTH, screen_height);
    }
    SDL_GL_SwapBuffers();
    glDrawBuffer(GL_BACK);
}

void update_Poses_List(int update, int blit)
{
    if (PoseIndex - pose_start >= 0)
        PoseList[PoseIndex - pose_start].color = UI_BACKL;

    if (blit)
    {
        blit_ViewPort();
    }

    if (!NVIDIA) glDrawBuffer(GL_FRONT_AND_BACK);
    if (UPDATE_BACKGROUND || update)
    {
        draw_Poses_Dialog("Poses List", screen_height, pose_start, 1, PoseIndex - pose_start);
    }
    else
    {
        draw_Poses_List(screen_height, pose_start, 0, PoseIndex - pose_start);
        draw_Poses_Bottom_Line(DIALOG_WIDTH, screen_height);
    }
    SDL_GL_SwapBuffers();
    glDrawBuffer(GL_BACK);
}

void update_Texts_List()
{
    if (!NVIDIA) glDrawBuffer(GL_FRONT_AND_BACK);
    DRAW_UI = 0;
    poly_Render(tripsRender, wireframe, splitview, CamDist, 0, subdLevel);
    draw_Textures_Dialog("Textures List", screen_height, text, texts, texts_count, texts_start, 1, currentObject);
    SDL_GL_SwapBuffers();
    DRAW_UI = 1;
    glDrawBuffer(GL_BACK);
    UPDATE_BACKGROUND = 1;
}

void update_Items_List()
{
    if (!NVIDIA) glDrawBuffer(GL_FRONT_AND_BACK);
    DRAW_UI = 0;
    poly_Render(tripsRender, wireframe, splitview, CamDist, 0, subdLevel);
    draw_Items_Dialog("Items List", screen_height, item, items, items_count, items_start, 1, currentObject);
    SDL_GL_SwapBuffers();
    DRAW_UI = 1;
    UPDATE_BACKGROUND = 1;
    glDrawBuffer(GL_BACK);
}

void update_Materials_List(int update, int blit)
{
    if (currentMaterial - materials_start >= 0)
        MatrList[currentMaterial - materials_start].color = UI_BACKL;

    if (blit)
    {
        blit_ViewPort();
    }

    if (!NVIDIA) glDrawBuffer(GL_FRONT_AND_BACK);
    if (UPDATE_BACKGROUND || update)
    {
        draw_Materials_Dialog("Materials L.", screen_height, materials_start, 1, currentObject);
    }
    else
    {
        draw_Materials_List(screen_height, materials_start, 1, currentObject);
        //draw_Selections_Bottom_Line(DIALOG_WIDTH, screen_height);
    }
    SDL_GL_SwapBuffers();
    glDrawBuffer(GL_BACK);
}

void update_Selections_List(int update, int blit)
{
    if (SelsIndex[current_sel_type] - sels_start[current_sel_type] >= 0)
        SelsList[SelsIndex[current_sel_type] - sels_start[current_sel_type]].color = UI_BACKL;

    if (blit)
    {
        blit_ViewPort();
    }

    if (!NVIDIA) glDrawBuffer(GL_FRONT_AND_BACK);
    if (UPDATE_BACKGROUND || update)
    {
        draw_Selections_Dialog("Selections L.", screen_height, sel_type, sel_types, sel_type_count, sels_start[current_sel_type], 1, SelsIndex[current_sel_type] - sels_start[current_sel_type]);
    }
    else
    {
        draw_Selections_List(screen_height, sels_start[current_sel_type], sel_type, 1, 1, SelsIndex[current_sel_type] - sels_start[current_sel_type]);
        draw_Selections_Bottom_Line(DIALOG_WIDTH, screen_height);
    }
    SDL_GL_SwapBuffers();
    glDrawBuffer(GL_BACK);
}

void apply_Pose_rotation_Play(deformer * D, pose * P, int frame, float Delta[3])
{
    if (!BIND_POSE)
    {
        paste_Pose_(D, D->Poses[0]); // default pose

        paste_Pose_rotation(D, P);

        if (D->Transformers_Count > 0)
        {
            transformer * T = D->Transformers[0];

            move_Pose_T(T, Delta);

            rotate_Deformer_pose(T);

            rotate_Deformer_verts(D);
        }
    }
}

void apply_Pose_position_(deformer * D, pose * P, int frame, float Delta[3])
{
    if (!BIND_POSE)
    {
        paste_Pose_position(D, P);

        //paste_Pose_rotation(D, P);

        if (D->Transformers_Count > 0)
        {
            transformer * T = D->Transformers[0];

            //move_Pose_T(T, Delta);

            Update_Objects_Count = 0;

            rotate_collect(T);
            rotate_vertex_groups_D_Init();

            //rotate_Deformer_pose(T);

            rotate_Deformer_verts(D);

            update_rotate_bounding_box();

            if (subdLevel > -1)
            {
                int o;
                object * O0;

                for (o = 0; o < Update_Objects_Count; o ++)
                {
                    O0 = Update_Objects[o];
                    if (O0->deforms)
                    {
                        tune_subdivide_post_transformed(O0, subdLevel);
                    }
                }
            }
        }
    }
    if (dialog_lock)
    {
        poly_Render(tripsRender, wireframe, splitview, CamDist, 0, subdLevel);
        draw_dialog_Box(screen_height, 0, frame);
        SDL_GL_SwapBuffers();
    }
    else
    {
        poly_Render(tripsRender, wireframe, splitview, CamDist, 1, subdLevel);
    }
}

void apply_Pose_rotation_(deformer * D, pose * P, int frame, float Delta[3])
{
    if (!BIND_POSE)
    {
        paste_Pose_(D, D->Poses[0]); // default pose

        paste_Pose_rotation(D, P);

        if (D->Transformers_Count > 0)
        {
            transformer * T = D->Transformers[0];

            move_Pose_T(T, Delta);

            Update_Objects_Count = 0;

            rotate_collect(T);
            rotate_vertex_groups_D_Init();

            rotate_Deformer_pose(T);

            rotate_Deformer_verts(D);

            update_rotate_bounding_box();

            if (subdLevel > -1)
            {
                int o;
                object * O0;

                for (o = 0; o < Update_Objects_Count; o ++)
                {
                    O0 = Update_Objects[o];
                    if (O0->deforms)
                    {
                        tune_subdivide_post_transformed(O0, subdLevel);
                    }
                }
            }
        }
    }
    if (dialog_lock)
    {
        poly_Render(tripsRender, wireframe, splitview, CamDist, 0, subdLevel);
        draw_dialog_Box(screen_height, 0, frame);
        SDL_GL_SwapBuffers();
    }
    else
    {
        poly_Render(tripsRender, wireframe, splitview, CamDist, 1, subdLevel);
    }
}

void apply_Pose(deformer * D, pose * P, int dialog)
{
    set_Pose_H_Button(3);
    printf("paste Pose\n");
    if (!BIND_POSE && currentDeformer_Node >= 0 && currentDeformer_Node < deformerIndex)
    {
        if (currentPose >= 0 && currentPose < posesIndex)
        {
            /*
            Poses need to be rig based.
            New rig system needs to be made.
            Rig has only positional poses.
            */
//            pose * P = poses[currentPose];
//            deformer * D = P->D;

            paste_Pose_(D, P);

            unfix_pose_ik_goals(D, P);

            // deformations
//            Update_Objects_Count = 0;
//
//            rotate_collect(transformers[currentLocator]);
//            rotate_vertex_groups_D_Init();
//
//            rotate_Deformer_(D);
            //
            if (D->Transformers_Count > 0)
            {
                transformer * T = D->Transformers[0];

                Update_Objects_Count = 0;

                rotate_collect(T);
                rotate_vertex_groups_D_Init();

                rotate_Deformer_verts(D);

                update_rotate_bounding_box();

                if (subdLevel > -1)
                {
                    int o;
                    object * O0;

                    for (o = 0; o < Update_Objects_Count; o ++)
                    {
                        O0 = Update_Objects[o];
                        if (O0->deforms)
                        {
                            tune_subdivide_post_transformed(O0, subdLevel);
                        }
                    }
                }
            }

            message = -1;
        }
    }
    if (dialog && dialog_lock)
        draw_Dialog();
}

void transition_into_Pose(deformer * D, pose * P0, pose * P1)
{
    Draw_Bottom_Message("transition into Pose\n");

    int f;
    float w0, w1;

    pose * P;

    DRAW_UI = 0;

    float Delta[3];

    if (D->Transformers_Count > 0)
    {
        T = D->Transformers[0];
        Delta[0] = T->pos[0] - D->Poses[0]->TP[0].pos[0];
        Delta[1] = T->pos[1] - D->Poses[0]->TP[0].pos[1];
        Delta[2] = T->pos[2] - D->Poses[0]->TP[0].pos[2];
    }

    int osd = Osd;
    int hints = HINTS;
    HINTS = 0;
    Osd = 0;

    for (f = 1; f <= frames_count; f ++)
    {
        w1 = (float)f / frames_count;
        w0 = 1 - w1;
        P = create_Inbetween_Pose_(D, P0, P1, w0, w1);

        apply_Pose_position_(D, P, f, Delta);
        //apply_Pose_rotation_(D, P, f, Delta);

        free(P->TP);
        free(P);
    }

    apply_Pose(D, P1, 0);

    HINTS = hints;
    Osd = osd;

    //poly_Render(tripsRender, wireframe, splitview, CamDist, 0, subdLevel);

    DRAW_UI = 1;
}

void deformer_Player()
{
    int t, p, u, o, f, d;
    float w0, w1;

    pose * P;
    object * O, * O0;
    deformer * D;
    transformer * T;

    int frames = 10;

    Update_Objects_Count = 0;

    int condition;

    DRAW_UI = 1;

    ELEMENT_ARRAYS = 1;
    init_Hint();

    if (subdLevel > -1)
    load_id_colors_No_Surface(Camera, subdLevel);
    else
    load_id_colors_Fan_all(Camera, OBJECT_COLORS);

    UPDATE_COLORS = 1;

    poly_Render(tripsRender, wireframe, splitview, CamDist, 1, subdLevel);

    UPDATE_COLORS = 0;

    DRAW_UI = 0;

    for (d = 0; d < deformerIndex; d ++)
    {
        D = deformers[d];
        if (D->Transformers_Count > 0)
        {
            T = D->Transformers[0];
            D->Delta[0] = T->pos[0] - D->Poses[D->current_pose]->TP[0].pos[0];
            D->Delta[1] = T->pos[1] - D->Poses[D->current_pose]->TP[0].pos[1];
            D->Delta[2] = T->pos[2] - D->Poses[D->current_pose]->TP[0].pos[2];
        }
        for (o = 0; o < D->Objects_Count; o ++)
        {
            O = D->Objects[o];
            condition = 1;
            for (u = 0; u < Update_Objects_Count; u ++)
            {
                if (Update_Objects[u] == O)
                {
                    condition = 0;
                    break;
                }
            }
            if (condition)
            {
                Update_Objects[Update_Objects_Count ++] = O;
            }
        }
        Transformer_Objects_Count = 0;
        for (t = 0; t < D->Transformers_Count; t ++)
        {
            if (D->Transformers[t]->Object != NULL)
            {
                Transformer_Objects[Transformer_Objects_Count ++] = D->Transformers[t]->Object;
            }
        }
    }

    int Preak = 0;
    int theme = 0;

    for (p = 0; p >= 0; p ++)
    {
        if (Preak)
        {
            for (d = 0; d < deformerIndex; d ++)
            {
                D = deformers[d];
                if (D->Transformers_Count > 0)
                {
                    D->current_pose = (p + D->current_pose) % D->Poses_Count;
                }
            }
            break;
        }

        for (f = 0; f < frames; f ++)
        {
            if (SDL_PollEvent(&event))
            {
                if (event.type == SDL_VIDEORESIZE)
                {
                    DRAW_UI = 1;
                    update_Resize_Event();
                    poly_Render(tripsRender, wireframe, splitview, CamDist, 1, subdLevel);
                    DRAW_UI = 0;
                }
                else if (event.type == SDL_KEYUP)
                {
                    if (event.key.keysym.sym == SDLK_ESCAPE)
                    {
                        Preak = 1;
                    }
                }
                else if (event.type == SDL_KEYDOWN)
                {
                    if (event.key.keysym.sym == SDLK_f)
                    {
                        TURNTABLE = !TURNTABLE;
                    }
                    else if (event.key.keysym.sym == SDLK_l)
                    {
                        LIGHTSHOW = !LIGHTSHOW;
                    }
                }
            }
            w1 = (float)f / frames;
            w0 = 1 - w1;
            rotate_vertex_groups_D_Init();
            for (d = 0; d < deformerIndex; d ++)
            {
                D = deformers[d];
                if (D->Transformers_Count > 0)
                {
                    P = create_Inbetween_Pose(D, D->Poses[(p + D->current_pose) % D->Poses_Count], D->Poses[(p + D->current_pose + 1) % D->Poses_Count], w0, w1);
                    Delta[0] = D->Delta[0] + P->TP[0].pos[0] - D->Poses[0]->TP[0].pos[0];
                    Delta[1] = D->Delta[1] + P->TP[0].pos[1] - D->Poses[0]->TP[0].pos[1];
                    Delta[2] = D->Delta[2] + P->TP[0].pos[2] - D->Poses[0]->TP[0].pos[2];
                    apply_Pose_rotation_Play(D, P, f % frames, Delta);
                    free(P->TP);
                    free(P);
                }
            }

            update_rotate_bounding_box();

            if (subdLevel > -1)
            {
                for (o = 0; o < Update_Objects_Count; o ++)
                {
                    O = Update_Objects[o];
                    if (O->deforms)
                    {
                        tune_subdivide_post_transformed(O, subdLevel);
                    }
                }
            }

            for (o = 0; o < Transformer_Objects_Count; o++)
            {
                O0 = Transformer_Objects[o];

                rotate_verts(O0, *O0->T);
                if (O->deforms)
                {
                    tune_subdivide_post_transformed(O0, subdLevel);
                }
            }
            if (TURNTABLE && Camera == &Camera_Persp)
            {
                Camera->T->rot[1] += 0.01;
                rotate_Camera(Camera, CamDist);
                update_camera(Camera, CamDist);
            }
            poly_Render(tripsRender, wireframe, splitview, CamDist, 1, subdLevel);
        }

        if (LIGHTSHOW)
        {
            update_Light(Light_Themes[theme % Themes]);
            theme ++;
            init_lights();
        }
    }

    DRAW_UI = 1;
}

void select_IK_Goal(ikChain * I)
{
    currentLocator = I->B->index;
    T->selected = 0;
    T = transformers[currentLocator];
    T->selected = 1;
}

void handle_UP_IK(int scrollbar)
{
    if (scrollbar)
    {
        if (IKIndex - ikch_start >= 0)
            IkchList[IKIndex - ikch_start].color = UI_BLACK;
        ikch_start --;
        if (ikch_start < 0) ikch_start = 0;
        if (IKIndex - ikch_start >= 0)
            IkchList[IKIndex - ikch_start].color = UI_BACKL;
    }
    else
    {
        if (IKIndex - ikch_start >= 0)
            IkchList[IKIndex - ikch_start].color = UI_BLACK;
        IKIndex --;
        if (IKIndex < 0) IKIndex ++;
        if (IKIndex - ikch_start >= 0)
            IkchList[IKIndex - ikch_start].color = UI_BACKL;
        currentIK = IK_List[IKIndex];
    }
    if (currentIK >= 0 && currentIK < iksIndex)
        select_IK_Goal(ikChains[currentIK]);

    DRAW_UI = 0;
    poly_Render(tripsRender, wireframe, splitview, CamDist, 0, subdLevel);
    DRAW_UI = 1;
    update_IK_List(1, 0);
}

void handle_UP_Bone(int scrollbar)
{
    if (scrollbar)
    {
        if (BoneIndex - bone_start >= 0)
            BoneList[BoneIndex - bone_start].color = UI_BLACK;
        bone_start --;
        if (bone_start < 0) bone_start = 0;
        if (BoneIndex - bone_start >= 0)
            BoneList[BoneIndex - bone_start].color = UI_BACKL;
    }
    else
    {
        if (BoneIndex - bone_start >= 0)
            BoneList[BoneIndex - bone_start].color = UI_BLACK;
        BoneIndex --;
        if (BoneIndex < 0) BoneIndex ++;
        if (BoneIndex - bone_start >= 0)
            BoneList[BoneIndex - bone_start].color = UI_BACKL;
        currentBone = Bone_List[BoneIndex];
    }
    update_Bones_List(1, 0);
}

void swap_Poses_up(pose * P)
{
    int p, index0, index1;
    int condition = 0;
    deformer * D;

    pose * P0, * P1;

    if (P->D != NULL)
    {
        D = P->D;
        for (p = 1; p < D->Poses_Count; p ++)
        {
            if (D->Poses[p] == P)
            {
                index1 = p;
                condition = 1;
                break;
            }
        }

        if (condition && index1 > 1)
        {
            index0 = index1 - 1;
            P0 = D->Poses[index0];
            P1 = D->Poses[index1];
            D->Poses[index0] = P1;
            D->Poses[index1] = P0;
            create_Poses_List(PoseIndex);
        }
    }
}

void select_Deformer()
{
    T = transformers[currentLocator];

    if (T->Deformer != NULL)
    {
        D = deformers[currentDeformer_Node];
        D->selected = 0;
        currentDeformer_Node = T->Deformer->index;
        D = deformers[currentDeformer_Node];
        if (D->selected == 0)
            Draw_Bottom_Message(D->Name);
        D->selected = 1;
    }
}

void set_Deformer_current_pose(int currentPose)
{
    int idx, index, d;

    deformer * D;

    index = 0;

    for (d = 0; d < deformerIndex; d ++)
    {
        D = deformers[d];

        idx = index;

        if (!D->collapsed)
        {
            index += D->Poses_Count;
        }

        if (index >= currentPose)
        {
            D->current_pose = currentPose - idx;
            break;
        }
    }

}

void handle_UP_Pose(int scrollbar)
{
    if (scrollbar)
    {
        if (PoseIndex - pose_start >= 0)
            PoseList[PoseIndex - pose_start].color = UI_BLACK;
        pose_start --;
        if (pose_start < 0) pose_start = 0;
        if (PoseIndex - pose_start >= 0)
            PoseList[PoseIndex - pose_start].color = UI_BACKL;
    }
    else
    {
        if (PoseIndex - pose_start >= 0)
            PoseList[PoseIndex - pose_start].color = UI_BLACK;
        PoseIndex --;
        if (PoseIndex < 0) PoseIndex ++;
        if (PoseIndex - pose_start >= 0)
            PoseList[PoseIndex - pose_start].color = UI_BACKL;
        if (controlDown)
        {
            swap_Poses_up(poses[currentPose]);
            set_Deformer_current_pose(currentPose);
        }
        else
        {
            if (Pose_List[PoseIndex] >= 0)
            {
                pose * P0 = poses[currentPose];
                currentPose = Pose_List[PoseIndex];
                if (currentPose >= 0)
                {
                    pose * P1 = poses[currentPose];

                    if (!BIND_POSE && P0 != NULL && P1 != NULL && P0 != P1 && P0->D == P1->D)
                    {
                        deformer * D = P1->D;
                        set_Deformer_current_pose(currentPose);
                        transition_into_Pose(D, P0, P1);
                    }
                }
            }
            else
            {
                currentDeformer_Node = -(Pose_List[PoseIndex] + 1);
                assert_Deformers_Selected();
            }
        }
    }
    update_Poses_List(1, 0);
}

void update_Hierarcys_List(int update, int blit)
{
    if (HierIndex - hier_start >= 0)
        HierList[HierIndex - hier_start].color = UI_BACKL;

    if (blit)
    {
        blit_ViewPort();
    }

    if (!NVIDIA) glDrawBuffer(GL_FRONT_AND_BACK);
    if (UPDATE_BACKGROUND || update)
    {
        draw_Hierarchys_Dialog("Hierarchys L.", screen_height, hier_start, 1, HierIndex - hier_start);
    }
    else
    {
        draw_Hierarchys_List(screen_height, hier_start, 1, HierIndex - hier_start);
        draw_Hierarchys_Bottom_Line(DIALOG_WIDTH, screen_height);
    }

    if (HierIndex - hier_start >= 0)
        HierList[HierIndex - hier_start].color = UI_BLACK;

    SDL_GL_SwapBuffers();
    glDrawBuffer(GL_BACK);
}

void handle_UP_Hier(int scrollbar)
{
    if (scrollbar)
    {
        if (HierIndex - hier_start >= 0)
            HierList[HierIndex - hier_start].color = UI_BLACK;
        hier_start --;
        if (hier_start < 0) hier_start = 0;
        if (HierIndex - hier_start >= 0)
            HierList[HierIndex - hier_start].color = UI_BACKL;
        update_Hierarcys_List(0, 1);
    }
    else
    {
        if (HierIndex - hier_start >= 0)
            HierList[HierIndex - hier_start].color = UI_BLACK;
        HierIndex --;
        if (HierIndex < 0) HierIndex ++;
        if (HierIndex - hier_start >= 0)
            HierList[HierIndex - hier_start].color = UI_BACKL;
        select_Transformer();
    }
}

void handle_UP_Material(int scrollbar)
{
    if (scrollbar)
    {
        if (currentMaterial - materials_start >= 0)
            MatrList[currentMaterial - materials_start].color = UI_BLACK;
        materials_start --;
        if (materials_start < 0) materials_start = 0;
        if (currentMaterial - materials_start >= 0)
            MatrList[currentMaterial - materials_start].color = UI_BACKL;

        update_Materials_List(0, 1);
    }
    else
    {
        if (currentMaterial - materials_start >= 0)
            MatrList[currentMaterial - materials_start].color = UI_BLACK;
        currentMaterial --;
        if (currentMaterial < 0)
            currentMaterial = Materials_count - 1;
        if (currentMaterial - materials_start >= 0)
            MatrList[currentMaterial - materials_start].color = UI_BACKL;
        select_Material();
    }
}

void handle_UP_Sels(int scrollbar)
{
    if (scrollbar)
    {
        if (SelsIndex[current_sel_type] - sels_start[current_sel_type] >= 0)
            SelsList[SelsIndex[current_sel_type] - sels_start[current_sel_type]].color = UI_BLACK;
        sels_start[current_sel_type] --;
        if (sels_start[current_sel_type] < 0) sels_start[current_sel_type] = 0;
        if (SelsIndex[current_sel_type] - sels_start[current_sel_type] >= 0)
            SelsList[SelsIndex[current_sel_type] - sels_start[current_sel_type]].color = UI_BACKL;
        if (dialog_type == DEFR_DIALOG)
            UPDATE_BACKGROUND = 0;
        update_Selections_List(0, 1);
    }
    else
    {
        if (SelsIndex[current_sel_type] - sels_start[current_sel_type] >= 0)
            SelsList[SelsIndex[current_sel_type] - sels_start[current_sel_type]].color = UI_BLACK;
        SelsIndex[current_sel_type] --;
        if (SelsIndex[current_sel_type] < 0)
            SelsIndex[current_sel_type] = Selections_c[current_sel_type] - 1;
        if (SelsIndex[current_sel_type] - sels_start[current_sel_type] >= 0)
            SelsList[SelsIndex[current_sel_type] - sels_start[current_sel_type]].color = UI_BACKL;
        select_Selection();
    }
}

void handle_UP_Defr(int scrollbar)
{
    if (scrollbar)
    {
        if (DefrIndex - defr_start >= 0)
            DefrList[DefrIndex - defr_start].color = UI_BLACK;
        defr_start --;

        if (defr_start < 0)
        {
            defr_start = 0;
        }
        if (DefrIndex - defr_start >= 0)
            DefrList[DefrIndex - defr_start].color = UI_BACKL;

        update_Deformers_List(0);
    }
    else
    {
        if (DefrIndex - defr_start >= 0)
            DefrList[DefrIndex - defr_start].color = UI_BLACK;
        DefrIndex --;
        if (DefrIndex < 0)
            DefrIndex ++;
        if (DefrIndex - defr_start >= 0)
            DefrList[DefrIndex - defr_start].color = UI_BACKL;

        currentDeformer = DefrIndex;
        if (Deformer_List[currentDeformer] >= 0)
        {
            currentLocator = Deformer_List[currentDeformer];
        }
        DRAW_UI = 0;
        poly_Render(tripsRender, wireframe, splitview, CamDist, 0, subdLevel);
        draw_Deformers_Dialog("Deformers L.", screen_height, defr_type, defr_types, defr_type_count,
                            defr_start, 1, DefrIndex - defr_start,
                            hier_start, HierIndex - hier_start,
                            sels_start[current_sel_type], SelsIndex[current_sel_type] - sels_start[current_sel_type]);
        SDL_GL_SwapBuffers();
        DRAW_UI = 1;
    }
}

void handle_UP_Saves(int scrollbar)
{
    if (scrollbar)
    {
        scene_files_start --;

        if (scene_files_start < 0)
        {
            scene_files_start = 0;
        }

        //printf("%d\n", scene_files_start);

        update_Saves_List(0, 0);
    }
}

void handle_UP_Loading(int scrollbar)
{
    if (scrollbar)
    {
        scene_files_start --;

        if (scene_files_start < 0)
        {
            scene_files_start = 0;
        }

        //printf("%d\n", scene_files_start);

        update_Loading_List(0, 0);
    }
}

void handle_UP(int scrollbar)
{
    if (!Edit_Lock)
    {
        if (UPDATE_BACKGROUND)
        {
            DRAW_UI = 0;
            poly_Render(tripsRender, wireframe, splitview, CamDist, 0, subdLevel);
            DRAW_UI = 1;
        }
        if (dialog_type == SAVES_DIALOG)
        {
            handle_UP_Saves(scrollbar);
        }
        else if (dialog_type == LOADING_DIALOG)
        {
            handle_UP_Loading(scrollbar);
        }
        else if (dialog_type == BONE_DIALOG)
        {
            handle_UP_Bone(scrollbar);
        }
        else if (dialog_type == IK_DIALOG)
        {
            handle_UP_IK(scrollbar);
        }
        else if (dialog_type == POSE_DIALOG)
        {
            handle_UP_Pose(scrollbar);
        }
        else if (dialog_type == DEFR_DIALOG)
        {
            if (current_defr_type == 0)
            {
                handle_UP_Defr(scrollbar);
            }
            else if (current_defr_type == 1)
            {
                handle_UP_Hier(scrollbar);
            }
            else if (current_defr_type == 2)
            {
                handle_UP_Sels(scrollbar);
            }
        }
        else if (dialog_type == HIER_DIALOG)
        {
            handle_UP_Hier(scrollbar);
        }
        else if (dialog_type == SELS_DIALOG)
        {
            handle_UP_Sels(scrollbar);
        }
        else if (dialog_type == TEXT_DIALOG)
        {
            if (TextIndex - texts_start >= 0)
                TextList[TextIndex - texts_start].color = UI_BLACK;

            texts_start --;

            if (texts_start < 0) texts_start = 0;

            if (TextIndex - texts_start >= 0)
                TextList[TextIndex - texts_start].color = UI_BACKL;

            if (strcmp(text, TEXT_TYPE_TEXT) == 0)
            {
                text_start = texts_start;
            }
            else if (strcmp(text, TEXT_TYPE_NORM) == 0)
            {
                norm_start = texts_start;
            }
            else if (strcmp(text, TEXT_TYPE_BUMP) == 0)
            {
                bump_start = texts_start;
            }

            if (UPDATE_BACKGROUND)
            {
                draw_Textures_Dialog("Textures List", screen_height, text, texts, texts_count, texts_start, 1, currentObject);
            }
            else
            {
                draw_Textures_List(screen_height, texts_start, text, 1);
            }
            SDL_GL_SwapBuffers();
        }
        else if (dialog_type == ITEM_DIALOG)
        {
            if (currentObject - items_start >= 0)
                ItemList[currentObject - items_start].color = UI_BLACK;
            items_start --;
            if (items_start < 0) items_start = 0;
            if (currentObject - items_start >= 0)
                ItemList[currentObject - items_start].color = UI_BACKL;
            if (UPDATE_BACKGROUND)
            {
                draw_Items_Dialog("Items List", screen_height, item, items, items_count, items_start, 1, currentObject);
            }
            else
            {
                draw_Items_List(screen_height, items_start, item, 1, currentObject);
            }
            SDL_GL_SwapBuffers();
        }
        else if (dialog_type == MATERIAL_DIALOG)
        {
            handle_UP_Material(scrollbar);
        }
        else
        {
            files_start --;
            if (files_start < 0) files_start = 0;
            if (dialog_type == OBJ_DIALOG)
            {
                if (obj_extension_type == obj_EXTENSION)
                    obj_files_start = files_start;
                else if (obj_extension_type == OBJ_EXTENSION)
                    OBJ_files_start = files_start;
            }
            else if (dialog_type == IMG_DIALOG)
            {
                if (img_extension_type == png_EXTENSION)
                    png_files_start = files_start;
                else if (img_extension_type == jpg_EXTENSION)
                    jpg_files_start = files_start;
                else if (img_extension_type == tif_EXTENSION)
                    tif_files_start = files_start;
                else if (img_extension_type == PSD_EXTENSION)
                    PSD_files_start = files_start;
            }
            if (UPDATE_BACKGROUND)
            {
                draw_Files_Dialog(files_dir, "Open file", screen_height, extension, extensions, ext_count, files_start, 1, 0, Edit_Lock);
            }
            else
            {
                draw_Files_List(files_dir, screen_height, files_start, extension, 1, 0);
            }
            SDL_GL_SwapBuffers();
        }
        UPDATE_BACKGROUND = 0;
    }
    else
    {
        if (dialog_type == SAVES_DIALOG)
        {
            handle_UP_Saves(scrollbar);
        }
        else if (dialog_type == LOADING_DIALOG)
        {
            handle_UP_Loading(scrollbar);
        }
    }
}

void handle_DOWN_IK(int scrollbar)
{
    if (scrollbar)
    {
        if (IKIndex - ikch_start >= 0)
            IkchList[IKIndex - ikch_start].color = UI_BLACK;
        ikch_start ++;
        if (ikch_start > ikChains_c - LISTLENGTH) ikch_start --;
        if (IKIndex - ikch_start >= 0)
            IkchList[IKIndex - ikch_start].color = UI_BACKL;
    }
    else
    {
        if (IKIndex - ikch_start >= 0)
            IkchList[IKIndex - ikch_start].color = UI_BLACK;
        IKIndex ++;
        if (IKIndex > ikChains_c - 1)
            IKIndex --;
        if (IKIndex - ikch_start >= 0)
            IkchList[IKIndex - ikch_start].color = UI_BACKL;
        currentIK = IK_List[IKIndex];
    }
    if (currentIK >= 0 && currentIK < iksIndex)
        select_IK_Goal(ikChains[currentIK]);

    DRAW_UI = 0;
    poly_Render(tripsRender, wireframe, splitview, CamDist, 0, subdLevel);
    DRAW_UI = 1;
    update_IK_List(1, 0);
}

void handle_DOWN_Bone(int scrollbar)
{
    if (scrollbar)
    {
        if (BoneIndex - bone_start >= 0)
            BoneList[BoneIndex - bone_start].color = UI_BLACK;
        bone_start ++;
        if (bone_start > Bones_c - LISTLENGTH) bone_start --;
        if (BoneIndex - bone_start >= 0)
            BoneList[BoneIndex - bone_start].color = UI_BACKL;
    }
    else
    {
        if (BoneIndex - bone_start >= 0)
            BoneList[BoneIndex - bone_start].color = UI_BLACK;
        BoneIndex ++;
        if (BoneIndex > Bones_c - 1)
            BoneIndex --;
        if (BoneIndex - bone_start >= 0)
            BoneList[BoneIndex - bone_start].color = UI_BACKL;
        currentBone = Bone_List[BoneIndex];
    }
    update_Bones_List(1, 0);
}

void swap_Poses_down(pose * P)
{
    int p, index0, index1;
    int condition = 0;
    deformer * D;

    pose * P0, * P1;

    if (P->D != NULL)
    {
        D = P->D;
        for (p = 1; p < D->Poses_Count; p ++)
        {
            if (D->Poses[p] == P)
            {
                index0 = p;
                condition = 1;
                break;
            }
        }

        if (condition && index0 < D->Poses_Count - 1)
        {
            index1 = index0 + 1;
            P0 = D->Poses[index0];
            P1 = D->Poses[index1];
            D->Poses[index0] = P1;
            D->Poses[index1] = P0;
            create_Poses_List(PoseIndex);
        }
    }
}

void handle_DOWN_Pose(int scrollbar)
{
    if (scrollbar)
    {
        if (PoseIndex - pose_start >= 0)
            PoseList[PoseIndex - pose_start].color = UI_BLACK;
        pose_start ++;
        if (pose_start > Poses_c - LISTLENGTH) pose_start --;
        if (PoseIndex - pose_start >= 0)
            PoseList[PoseIndex - pose_start].color = UI_BACKL;
    }
    else
    {
        if (PoseIndex - pose_start >= 0)
            PoseList[PoseIndex - pose_start].color = UI_BLACK;
        PoseIndex ++;
        if (PoseIndex > Poses_c - 1)
            PoseIndex --;
        if (PoseIndex - pose_start >= 0)
            PoseList[PoseIndex - pose_start].color = UI_BACKL;
        if (controlDown)
        {
            swap_Poses_down(poses[currentPose]);
            set_Deformer_current_pose(currentPose);
        }
        else
        {
            if (Pose_List[PoseIndex] >= 0)
            {
                pose * P0 = poses[currentPose];
                currentPose = Pose_List[PoseIndex];
                if (currentPose >= 0)
                {
                    pose * P1 = poses[currentPose];
                    if (!BIND_POSE && P0 != NULL && P1 != NULL && P0 != P1 && P0->D == P1->D)
                    {
                        deformer * D = P1->D;
                        set_Deformer_current_pose(currentPose);
                        transition_into_Pose(D, P0, P1);
                    }
                }
            }
            else
            {
                currentDeformer_Node = -(Pose_List[PoseIndex] + 1);
                assert_Deformers_Selected();
            }
        }
    }
    update_Poses_List(1, 0);
}

void handle_DOWN_Hier(int scrollbar)
{
    if (scrollbar)
    {
        if (HierIndex - hier_start >= 0)
            HierList[HierIndex - hier_start].color = UI_BLACK;
        hier_start ++;
        if (hier_start > Hierarchys_c - LISTLENGTH) hier_start --;
        if (HierIndex - hier_start >= 0)
            HierList[HierIndex - hier_start].color = UI_BACKL;
        update_Hierarcys_List(0, 1);
    }
    else
    {
        if (HierIndex - hier_start >= 0)
            HierList[HierIndex - hier_start].color = UI_BLACK;
        HierIndex ++;
        if (HierIndex > Hierarchys_c - 1)
            HierIndex --;
        if (HierIndex - hier_start >= 0)
            HierList[HierIndex - hier_start].color = UI_BACKL;
        select_Transformer();
    }
}

void handle_DOWN_Material(int scrollbar)
{
    if (scrollbar)
    {
        if (currentMaterial - materials_start >= 0)
            MatrList[currentMaterial - materials_start].color = UI_BLACK;
        materials_start ++;
        if (materials_start > Materials_count - LISTLENGTH) materials_start --;
        if (currentMaterial - materials_start >= 0)
            MatrList[currentMaterial - materials_start].color = UI_BACKL;

        update_Materials_List(0, 1);
    }
    else
    {
        if (currentMaterial - materials_start >= 0)
            MatrList[currentMaterial - materials_start].color = UI_BLACK;
        currentMaterial ++;
        if (currentMaterial > Materials_count - 1)
            currentMaterial = 0;
        if (currentMaterial - materials_start >= 0)
            MatrList[currentMaterial - materials_start].color = UI_BACKL;
        select_Material();
    }
}

void handle_DOWN_Sels(int scrollbar)
{
    if (scrollbar)
    {
        if (SelsIndex[current_sel_type] - sels_start[current_sel_type] >= 0)
            SelsList[SelsIndex[current_sel_type] - sels_start[current_sel_type]].color = UI_BLACK;
        sels_start[current_sel_type] ++;
        if (sels_start[current_sel_type] > Selections_c[current_sel_type] - LISTLENGTH) sels_start[current_sel_type] --;
        if (SelsIndex[current_sel_type] - sels_start[current_sel_type] >= 0)
            SelsList[SelsIndex[current_sel_type] - sels_start[current_sel_type]].color = UI_BACKL;
        if (dialog_type == DEFR_DIALOG)
            UPDATE_BACKGROUND = 0;
        update_Selections_List(0, 1);
    }
    else
    {
        if (SelsIndex[current_sel_type] - sels_start[current_sel_type] >= 0)
            SelsList[SelsIndex[current_sel_type] - sels_start[current_sel_type]].color = UI_BLACK;
        SelsIndex[current_sel_type] ++;
        if (SelsIndex[current_sel_type] > Selections_c[current_sel_type] - 1)
            SelsIndex[current_sel_type] = 0;
        if (SelsIndex[current_sel_type] - sels_start[current_sel_type] >= 0)
            SelsList[SelsIndex[current_sel_type] - sels_start[current_sel_type]].color = UI_BACKL;
        select_Selection();
    }
}

void handle_DOWN_Defr(int scrollbar)
{
    if (scrollbar)
    {
        if (DefrIndex - defr_start >= 0)
            DefrList[DefrIndex - defr_start].color = UI_BLACK;
        defr_start ++;

        if (defr_start > Deformers_c - LISTLENGTH)
        {
            defr_start --;
        }
        if (DefrIndex - defr_start >= 0)
            DefrList[DefrIndex - defr_start].color = UI_BACKL;
        update_Deformers_List(0);
    }
    else
    {
        if (DefrIndex - defr_start >= 0)
            DefrList[DefrIndex - defr_start].color = UI_BLACK;
        DefrIndex ++;
        if (DefrIndex > Deformers_c - 1)
            DefrIndex --;
        if (DefrIndex - defr_start >= 0)
            DefrList[DefrIndex - defr_start].color = UI_BACKL;

        currentDeformer = DefrIndex;
        if (Deformer_List[currentDeformer] >= 0)
        {
            currentLocator = Deformer_List[currentDeformer];
        }
        DRAW_UI = 0;
        poly_Render(tripsRender, wireframe, splitview, CamDist, 0, subdLevel);
        draw_Deformers_Dialog("Deformers L.", screen_height, defr_type, defr_types, defr_type_count,
                            defr_start, 1, DefrIndex - defr_start,
                            hier_start, HierIndex - hier_start,
                            sels_start[current_sel_type], SelsIndex[current_sel_type] - sels_start[current_sel_type]);
        SDL_GL_SwapBuffers();
        DRAW_UI = 1;
    }
}

void handle_DOWN_Saves(int scrollbar)
{
    if (scrollbar)
    {
        scene_files_start ++;

        if (scene_files_start > files_num - LISTLENGTH)
        {
            scene_files_start --;
        }

        //printf("%d\n", scene_files_start);

        update_Saves_List(0, 0);
    }
}

void handle_DOWN_Loading(int scrollbar)
{
    if (scrollbar)
    {
        scene_files_start ++;

        if (scene_files_start > files_num - LISTLENGTH)
        {
            scene_files_start --;
        }

        //printf("%d\n", scene_files_start);

        update_Loading_List(0, 0);
    }
}

void handle_DOWN(int scrollbar)
{
    if (!Edit_Lock)
    {
        if (UPDATE_BACKGROUND)
        {
            DRAW_UI = 0;
            poly_Render(tripsRender, wireframe, splitview, CamDist, 0, subdLevel);
            DRAW_UI = 1;
        }
        if (dialog_type == SAVES_DIALOG)
        {
            handle_DOWN_Saves(scrollbar);
        }
        else if (dialog_type == LOADING_DIALOG)
        {
            handle_DOWN_Loading(scrollbar);
        }
        else if (dialog_type == BONE_DIALOG)
        {
            handle_DOWN_Bone(scrollbar);
        }
        else if (dialog_type == IK_DIALOG)
        {
            handle_DOWN_IK(scrollbar);
        }
        else if (dialog_type == POSE_DIALOG)
        {
            handle_DOWN_Pose(scrollbar);
        }
        else if (dialog_type == DEFR_DIALOG)
        {
            if (current_defr_type == 0)
            {
                handle_DOWN_Defr(scrollbar);
            }
            else if (current_defr_type == 1)
            {
                handle_DOWN_Hier(scrollbar);
            }
            else if (current_defr_type == 2)
            {
                handle_DOWN_Sels(scrollbar);
            }
        }
        else if (dialog_type == HIER_DIALOG)
        {
            handle_DOWN_Hier(scrollbar);
        }
        else if (dialog_type == SELS_DIALOG)
        {
            handle_DOWN_Sels(scrollbar);
        }
        else if (dialog_type == TEXT_DIALOG)
        {
            if (TextIndex - text_start >= 0)
                TextList[TextIndex - text_start].color = UI_BLACK;

            texts_start ++;

            if (strcmp(text, TEXT_TYPE_TEXT) == 0)
            {
                if (texts_start > Textures_c - LISTLENGTH) texts_start --;
                text_start = texts_start;
            }
            else if (strcmp(text, TEXT_TYPE_NORM) == 0)
            {
                if (texts_start > Normals_c - LISTLENGTH) texts_start --;
                norm_start = texts_start;
            }
            else if (strcmp(text, TEXT_TYPE_BUMP) == 0)
            {
                if (texts_start > Bumps_c - LISTLENGTH) texts_start --;
                bump_start = texts_start;
            }

            if (TextIndex - text_start >= 0)
                TextList[TextIndex - text_start].color = UI_BACKL;

            if (UPDATE_BACKGROUND)
            {
                draw_Textures_Dialog("Textures List", screen_height, text, texts, texts_count, texts_start, 1, currentObject);
            }
            else
            {
                draw_Textures_List(screen_height, texts_start, text, 1);
            }
            SDL_GL_SwapBuffers();
        }
        else if (dialog_type == ITEM_DIALOG)
        {
            if (currentObject - items_start >= 0)
                ItemList[currentObject - items_start].color = UI_BLACK;
            items_start ++;
            if (items_start > objectIndex - LISTLENGTH) items_start --;
            if (currentObject - items_start >= 0)
                ItemList[currentObject - items_start].color = UI_BACKL;

            if (UPDATE_BACKGROUND)
            {
                draw_Items_Dialog("Items List", screen_height, item, items, items_count, items_start, 1, currentObject);
            }
            else
            {
                draw_Items_List(screen_height, items_start, item, 1, currentObject);
            }
            SDL_GL_SwapBuffers();
        }
        else if (dialog_type == MATERIAL_DIALOG)
        {
            handle_DOWN_Material(scrollbar);
        }
        else
        {
            files_start ++;
            if (files_start > files_num - LISTLENGTH) files_start --;
            if (dialog_type == OBJ_DIALOG)
            {
                if (obj_extension_type == obj_EXTENSION)
                    obj_files_start = files_start;
                else if (obj_extension_type == OBJ_EXTENSION)
                    OBJ_files_start = files_start;
            }
            else if (dialog_type == IMG_DIALOG)
            {
                if (img_extension_type == png_EXTENSION)
                    png_files_start = files_start;
                else if (img_extension_type == jpg_EXTENSION)
                    jpg_files_start = files_start;
                else if (img_extension_type == tif_EXTENSION)
                    tif_files_start = files_start;
                else if (img_extension_type == PSD_EXTENSION)
                    PSD_files_start = files_start;
            }
            if (UPDATE_BACKGROUND)
            {
                draw_Files_Dialog(files_dir, "Open file", screen_height, extension, extensions, ext_count, files_start, 1, 0, Edit_Lock);
            }
            else
            {
                draw_Files_List(files_dir, screen_height, files_start, extension, 1, 0);
            }
            SDL_GL_SwapBuffers();
        }
        UPDATE_BACKGROUND = 0;
    }
    else
    {
        if (dialog_type == SAVES_DIALOG)
        {
            handle_DOWN_Saves(scrollbar);
        }
        else if (dialog_type == LOADING_DIALOG)
        {
            handle_DOWN_Loading(scrollbar);
        }
    }
}

void generate_Smooth_Binding(deformer * D)
{
    Draw_Bottom_Message("generate Smooth Binding\n");
    Bottom_Message = 0;
    generate_Split_Groups_In_Deformer(D);
    generate_Extended_Groups_In_Deformer(D);
    normalize_Weights_In_Deformer(D);

    draw_Dialog();
}

void init_Deformer_Objects_Binding(deformer * D)
{
    Draw_Bottom_Message("init Deformer Objects Binding\n");
    Bottom_Message = 0;
    create_Transformers_List();
    add_Objects_To_Deformer(D);
    generate_Bones_In_Deformer(D);
    generate_Bones_Distances_Mapping_In_Deformer(D);
    generate_Bones_Distances_In_Deformer(D);
    smallest_Distances_In_Deformer(D);
    generate_Vertex_Groups_In_Deformer(D);
    free_Bone_Distances_In_Deformer(D);
    delete_Zero_Selections_From_Deformer(D);
    delete_Zero_Selections_From_Objects(D);

    /* smooth binding */
    create_Hierarchys_List();

    draw_Dialog();
}

void set_Deformers_Delta()
{
    int d;

    deformer * D;

    for (d = 0; d < deformerIndex; d ++)
    {
        D = deformers[d];
        if (D->Transformers_Count > 0)
        {
            D->Delta[0] = D->Transformers[0]->pos[0] - D->Poses[0]->TP[0].pos[0];
            D->Delta[1] = D->Transformers[0]->pos[1] - D->Poses[0]->TP[0].pos[1];
            D->Delta[2] = D->Transformers[0]->pos[2] - D->Poses[0]->TP[0].pos[2];
        }
    }
}

void add_Pose()
{
    set_Pose_H_Button(0);
    printf("add Pose\n");

    int relative = 0;

    if (!BIND_POSE && posesIndex < POSES)
    {
        if (currentDeformer_Node >= 0 && currentDeformer_Node < deformerIndex)
        {
            D = deformers[currentDeformer_Node];
            if (D->Transformers_Count > 0 && D->Transformers[0]->index != currentLocator)
            {
                set_Deformers_Delta();
                relative = 1;
            }
            paste_Deformer_rotVec(D);
            add_Pose_To_Deformer(D, relative);
            D->current_pose = D->Poses_Count - 1;
//            PoseIndex ++;
//            if (Pose_List[PoseIndex] >= 0)
//            {
//                currentPose = Pose_List[PoseIndex];
//            }
            printf("%s\n", D->Poses[D->current_pose]->Name);
            if (dialog_lock)
                draw_Dialog();
        }
    }
}

void delete_Bone(bone * B)
{
    Draw_Bottom_Message("delete Bone\n");
    if (B->D == NULL)
    {
        //if (BIND_POSE)
        //{
        remove_Bone_From_Deformer(B);
        remove_Bone_From_Bones(B);
        //}
    }
}

void remove_IK()
{
    set_IK_H_Button(0);
    printf("remove IK\n");
    if (currentIK >= 0 && currentIK < iksIndex)
    {
        if (BIND_POSE)
        {
            /* remove here */
            ikChain * I = ikChains[currentIK];
            remove_ikChain_From_ikChains_(I, 0);
            IKIndex --;
            currentIK --;
            if (currentIK < 0)
                currentIK = 0;
            if (currentLocator >= 0 && currentLocator < transformerIndex)
            {
                T = transformers[currentLocator];
            }
            else
            {
                T = transformers[0];
                currentLocator = 0;
            }
        }
        if (dialog_lock)
            draw_Dialog();
    }
}

void remove_Bone()
{
    set_Bone_H_Button(0);
    printf("remove Bone\n");
    if (currentBone >= 0 && currentBone < bonesIndex)
    {
        bone * B = bones[currentBone];
        if (BIND_POSE && B->IK == NULL)
        {
            remove_Bone_From_Deformer(B);
            remove_Bone_From_Bones(B);
            BoneIndex --;
            currentBone --;
            if (currentBone < 0)
                currentBone = 0;
        }
        if (dialog_lock)
            draw_Dialog();
    }
}

void rename_Scene_dir()
{
    printf("rename Scene dir\n");
    if (dialog_lock)
    {
        if (!Edit_Lock)
        {
            scene_files_start = 0;
            sprintf(scene_extension, "%s", "");
            sprintf(Name_Remember, "%s", scene_files_dir);
            sprintf(EditString, "%s", scene_files_dir);
            //sprintf(scene_files_dir, "%s", "");
            Edit_Lock = 1;
            EditCursor = strlen(scene_files_dir);
            if (dialog_type == SAVES_DIALOG)
                update_Saves_List(0, 0);
            else if (dialog_type == LOADING_DIALOG)
                update_Loading_List(0, 0);
        }
    }
}

void rename_IK()
{
    set_IK_H_Button(1);
    printf("rename IK\n");
    if (dialog_lock)
    {
        if (!Edit_Lock && ikChains_c > 0)
        {
            sprintf(Name_Remember, "%s", IK_Names[IKIndex]);
            sprintf(IK_Names[IKIndex], "%s", "");
            Edit_Lock = 1;
            update_IK_List(0, 0);
        }
    }
}

void rename_Bone()
{
    set_Bone_H_Button(1);
    printf("rename Bone\n");
    if (dialog_lock)
    {
        if (!Edit_Lock && Bones_c > 0)
        {
            sprintf(Name_Remember, "%s", Bone_Names[BoneIndex]);
            sprintf(Bone_Names[BoneIndex], "%s", "");
            Edit_Lock = 1;
            update_Bones_List(0, 0);
        }
    }
}

void remove_Pose()
{
    set_Pose_H_Button(1);
    printf("remove Pose\n");
    if (currentPose >= 0 && currentPose < posesIndex)
    {
        remove_Pose_From_Deformer(poses[currentPose]);
//        PoseIndex --;
//        if (Pose_List[PoseIndex] >= 0)
//        {
//            currentPose = Pose_List[PoseIndex];
//        }
        if (dialog_lock)
            draw_Dialog();
    }
}

void update_Pose()
{
    if (posesIndex > 0 && currentPose < posesIndex)
    {
        set_Pose_H_Button(3);
        pose * P = poses[currentPose];

        sprintf(msg, "update %s\n", P->Name);
        Draw_Bottom_Message(msg);

        deformer * D = P->D;

        if (D->Transformers_Count > 0 && P != D->Poses[0])
        {
            transformer * T0 = D->Transformers[0];

            if (T != T0)
            {
                D->Delta[0] = T0->pos[0] - D->Poses[D->current_pose]->TP[0].pos[0];
                D->Delta[1] = T0->pos[1] - D->Poses[D->current_pose]->TP[0].pos[1];
                D->Delta[2] = T0->pos[2] - D->Poses[D->current_pose]->TP[0].pos[2];

                T0->pos[0] -= D->Delta[0];
                T0->pos[1] -= D->Delta[1];
                T0->pos[2] -= D->Delta[2];
            }

            update_Deformer_Pose(D, P);
            update_Poses_List(0, 0);

            if (T != T0)
            {
                T0->pos[0] += D->Delta[0];
                T0->pos[1] += D->Delta[1];
                T0->pos[2] += D->Delta[2];
            }
        }
    }
}

void rename_Pose()
{
    set_Pose_H_Button(2);
    printf("rename Pose\n");
    if (dialog_lock)
    {
        if (!Edit_Lock && Poses_c > 0)
        {
            sprintf(Name_Remember, "%s", Pose_Names[PoseIndex]);
            sprintf(Pose_Names[PoseIndex], "%s", "");
            Edit_Lock = 1;
            update_Poses_List(0, 0);
        }
    }
}

void apply_Pose_rotation()
{
    set_Pose_H_Button(3);
    printf("paste Pose rotation\n");
    if (!BIND_POSE && currentDeformer_Node >= 0 && currentDeformer_Node < deformerIndex)
    {
        if (currentPose >= 0 && currentPose < posesIndex)
        {
            /*
            Poses need to be rig based.
            New rig system needs to be made.
            Rig has only positional poses.
            */
            pose * P = poses[currentPose];
            deformer * D = P->D;

            unfix_deformer_ik_goals(D);

            float Delta[3];

            if (D->Transformers_Count > 0)
            {
                T = D->Transformers[0];
                Delta[0] = T->pos[0] - D->Poses[0]->TP[0].pos[0];
                Delta[1] = T->pos[1] - D->Poses[0]->TP[0].pos[1];
                Delta[2] = T->pos[2] - D->Poses[0]->TP[0].pos[2];
            }

            paste_Pose_(D, D->Poses[0]); // default pose

            paste_Pose_rotation(D, P);

            // deformations
//            Update_Objects_Count = 0;
//
//            rotate_collect(transformers[currentLocator]);
//            rotate_vertex_groups_D_Init();
//
//            rotate_Deformer_(D);
            //
            if (D->Transformers_Count > 0)
            {
                Update_Objects_Count = 0;

                move_Pose_T(T, Delta);

                rotate_collect(T);
                rotate_vertex_groups_D_Init();

                rotate_Deformer_pose(T);

                rotate_Deformer_verts(D);

                update_rotate_bounding_box();

                if (subdLevel > -1)
                {
                    int o;
                    object * O0;

                    for (o = 0; o < Update_Objects_Count; o ++)
                    {
                        O0 = Update_Objects[o];
                        if (O0->deforms)
                        {
                            tune_subdivide_post_transformed(O0, subdLevel);
                        }
                    }
                }
            }

            message = -1;
        }
    }
    if (dialog_lock)
        draw_Dialog();
}

void add_Branch()
{
    printf("add Branch\n");

    if (BIND_POSE && deformerIndex > 0)
    {
        set_Defr_H_Button(0);
        D = deformers[currentDeformer_Node];
        transformer * T = transformers[currentLocator];

        int l, condition = 0;

        for (l = 0; l < locatorIndex; l ++)
        {
            if (T == Locators[l])
            {
                condition = 1;
                break;
            }
        }

        if (condition && T != &World)
        {
            add_Branch_To_Deformer(T, D);
            add_Branch_IK_Chains(T, D);
//            create_Deformers_List(SelsIndex[3], O);
//            draw_Deformers_List(screen_height, defr_start, 1, currentDeformer);
//            draw_Deformers_Bottom_Line(DIALOG_WIDTH, screen_height);
//            SDL_GL_SwapBuffers();
            draw_Dialog();
        }
    }
}

void handle_Defr_Dialog(char letter, SDLMod mod)
{
    if (Edit_Lock)
    {

    }
    else
    {
        if (letter == 'a')
        {
            add_Branch();
        }
        else if (letter == 'b')
        {
            if (deformerIndex > 0)
            {
                D = deformers[currentDeformer_Node];
                if (BIND_POSE && D != NULL)
                    init_Deformer_Objects_Binding(D);
            }
        }
        else if (letter == 's')
        {
            if (deformerIndex > 0)
            {
                D = deformers[currentDeformer_Node];
                if (D != NULL)
                    generate_Smooth_Binding(D);
            }
        }
        else if (letter == 'd')
        {
            if (deformerIndex > 0)
            {
                D = deformers[currentDeformer_Node];
                if (D != NULL)
                    remove_Object_From_Deformer(O, D);
                    draw_Dialog();
            }
        }
    }
}

void handle_Scene_Dialog(char letter, SDLMod mod)
{
    if (Edit_Lock)
    {
        if (letter == '-')
        {
            if (mod & KMOD_SHIFT)
            {
                letter = '_';
            }
        }
        else if (isalnum(letter) && (mod & KMOD_SHIFT))
        {
            letter -= 32;
        }
        if (isalnum(letter) || letter == ' ' || letter == '_' || letter == '-' || letter == ':' || letter == '/')
        {
            if (EditCursor < STRLEN - 1)
            {
                EditString[EditCursor] = letter;
                EditCursor ++;
                EditString[EditCursor] = '\0';
            }
        }
        else if (letter == 13 || letter == 10) // return, enter
        {
            if (strlen(EditString) > 1)
            {
                sprintf(scene_files_dir, "%s", EditString);
                sprintf(Name_Remember, "%s", EditString);
            }
            else
            {
                sprintf(scene_files_dir, "%s", Name_Remember);
            }
            Edit_Lock = 0;
            EditCursor = 0;
            printf("Edit finishing!\n");
//            files_dir = (char *)&scene_files_dir;
//            files_num = query_files(files_dir, scene_extension);
//            printf("files_num %d\n", files_num);
            //update = 1;
        }
        else if (letter == 8) // backspace
        {
            EditCursor --;
            if (EditCursor < 0)
                EditCursor = 0;
            EditString[EditCursor] = '\0';
        }
        sprintf(scene_files_dir, "%s", EditString);
        scene_files_start = 0;
        if (dialog_type == SAVES_DIALOG)
            update_Saves_List(1, 0);
        else if (dialog_type == LOADING_DIALOG)
            update_Loading_List(1, 0);
        //printf("%c%s", 13, EditString);
        message = 0;
    }
}

void handle_IK_Dialog(char letter, SDLMod mod)
{
    if (Edit_Lock)
    {
        //int update = 0;
        if (letter == '-')
        {
            if (mod & KMOD_SHIFT)
            {
                letter = '_';
            }
        }
        else if (isalnum(letter) && (mod & KMOD_SHIFT))
        {
            letter -= 32;
        }
        if (isalnum(letter) || letter == ' ' || letter == '_' || letter == '-')
        {
            if (EditCursor < STRLEN - 1)
            {
                EditString[EditCursor] = letter;
                EditCursor ++;
                EditString[EditCursor] = '\0';
            }
        }
        else if (letter == 13 || letter == 10) // return, enter
        {
            if (strlen(EditString) > 1)
            {
                sprintf(IK_Names[IKIndex], "%s", EditString);
                replace_IK_Name(EditString);
                sprintf(Name_Remember, "%s", EditString);
            }
            else
            {
                sprintf(IK_Names[IKIndex], "%s", Name_Remember);
            }
            Edit_Lock = 0;
            EditCursor = 0;
            printf("Edit finishing!\n");
            set_IK_H_Button(-1);
            //update = 1;
        }
        else if (letter == 8) // backspace
        {
            EditCursor --;
            if (EditCursor < 0)
                EditCursor = 0;
            EditString[EditCursor] = '\0';
        }
        sprintf(IK_Names[IKIndex], "%s", EditString);
        update_IK_List(0, 0);
        //printf("%c%s", 13, EditString);
        message = 0;
    }
}

void handle_Bone_Dialog(char letter, SDLMod mod)
{
    if (Edit_Lock)
    {
        //int update = 0;
        if (letter == '-')
        {
            if (mod & KMOD_SHIFT)
            {
                letter = '_';
            }
        }
        else if (isalnum(letter) && (mod & KMOD_SHIFT))
        {
            letter -= 32;
        }
        if (isalnum(letter) || letter == ' ' || letter == '_' || letter == '-')
        {
            if (EditCursor < STRLEN - 1)
            {
                EditString[EditCursor] = letter;
                EditCursor ++;
                EditString[EditCursor] = '\0';
            }
        }
        else if (letter == 13 || letter == 10) // return, enter
        {
            if (strlen(EditString) > 1)
            {
                sprintf(Bone_Names[BoneIndex], "%s", EditString);
                replace_Bone_Name(EditString);
                sprintf(Name_Remember, "%s", EditString);
            }
            else
            {
                sprintf(Bone_Names[BoneIndex], "%s", Name_Remember);
            }
            Edit_Lock = 0;
            EditCursor = 0;
            printf("Edit finishing!\n");
            set_Bone_H_Button(-1);
            //update = 1;
        }
        else if (letter == 8) // backspace
        {
            EditCursor --;
            if (EditCursor < 0)
                EditCursor = 0;
            EditString[EditCursor] = '\0';
        }
        sprintf(Bone_Names[BoneIndex], "%s", EditString);
        update_Bones_List(0, 0);
        //printf("%c%s", 13, EditString);
        message = 0;
    }
}

void handle_Pose_Dialog(char letter, SDLMod mod)
{
    if (Edit_Lock)
    {
        //int update = 0;
        if (letter == '-')
        {
            if (mod & KMOD_SHIFT)
            {
                letter = '_';
            }
        }
        else if (isalnum(letter) && (mod & KMOD_SHIFT))
        {
            letter -= 32;
        }
        if (isalnum(letter) || letter == ' ' || letter == '_' || letter == '-')
        {
            if (EditCursor < STRLEN - 1)
            {
                EditString[EditCursor] = letter;
                EditCursor ++;
                EditString[EditCursor] = '\0';
            }
        }
        else if (letter == 13 || letter == 10) // return, enter
        {
            if (strlen(EditString) > 1)
            {
                sprintf(Pose_Names[PoseIndex], "%s", EditString);
                replace_Pose_Name(EditString);
                sprintf(Name_Remember, "%s", EditString);
            }
            else
            {
                sprintf(Pose_Names[PoseIndex], "%s", Name_Remember);
            }
            Edit_Lock = 0;
            EditCursor = 0;
            printf("Edit finishing!\n");
            set_Pose_H_Button(-1);
            //update = 1;
        }
        else if (letter == 8) // backspace
        {
            EditCursor --;
            if (EditCursor < 0)
                EditCursor = 0;
            EditString[EditCursor] = '\0';
        }
        sprintf(Pose_Names[PoseIndex], "%s", EditString);
        update_Poses_List(0, 0);
        //printf("%c%s", 13, EditString);
        message = 0;
    }
    else if (letter == 13 || letter == 10) // return, enter
    {
        apply_Pose_rotation(); //apply_Pose(); // for some reason pose with positions fails to transform vertexes
    }
    else if (letter == 'p')
    {
        pose * P = poses[currentPose];
        deformer * D = P->D;
        apply_Pose(D, P, 1);
    }
}

void handle_Sels_Dialog(char letter, SDLMod mod)
{
    if (Edit_Lock)
    {
        //int update = 0;
        if (letter == '-')
        {
            if (mod & KMOD_SHIFT)
            {
                letter = '_';
            }
        }
        else if (isalnum(letter) && (mod & KMOD_SHIFT))
        {
            letter -= 32;
        }
        if (isalnum(letter) || letter == ' ' || letter == '_' || letter == '-')
        {
            if (EditCursor < STRLEN - 1)
            {
                EditString[EditCursor] = letter;
                EditCursor ++;
                EditString[EditCursor] = '\0';
            }
        }
        else if (letter == 13 || letter == 10) // return, enter
        {
            if (strlen(EditString) > 1)
            {
                sprintf(Sels_Names[current_sel_type][SelsIndex[current_sel_type]], "%s", EditString);
                // Rename individual selections in current type for selected objects
                // Find names with Remembered name and replace it with EditString
                replace_Selections_Name(selected_objects, selected_object_count, current_sel_type, Name_Remember, EditString);
                sprintf(Name_Remember, "%s", EditString);

                assign_Selection_L();
            }
            else
            {
                sprintf(Sels_Names[current_sel_type][SelsIndex[current_sel_type]], "%s", Name_Remember);
            }
            Edit_Lock = 0;
            EditCursor = 0;
            printf("Edit finishing!\n");
            set_Sels_H_Button(-1);
            //update = 1;
        }
        else if (letter == 8) // backspace
        {
            EditCursor --;
            if (EditCursor < 0)
                EditCursor = 0;
            EditString[EditCursor] = '\0';
        }
        sprintf(Sels_Names[current_sel_type][SelsIndex[current_sel_type]], "%s", EditString);
        update_Selections_List(0, 0);
        //printf("%c%s", 13, EditString);
        message = 0;
    }
    else if (letter == 13 || letter == 10) // return, enter
    {
        select_Selection();
    }
    else if (letter == 'a')
    {
        select_Selection_AB(0);
    }
    else if (letter == 'b')
    {
        select_Selection_AB(1);
    }
    else if (letter == 'u')
    {
        unassign_All();
        draw_Dialog();
    }
}

void handle_Hier_Dialog(char letter, SDLMod mod)
{
    if (Edit_Lock)
    {
        //int update = 0;
        if (letter == '-')
        {
            if (mod & KMOD_SHIFT)
            {
                letter = '_';
            }
        }
        else if (isalnum(letter) && (mod & KMOD_SHIFT))
        {
            letter -= 32;
        }
        if (isalnum(letter) || letter == ' ' || letter == '_' || letter == '-')
        {
            if (EditCursor < STRLEN - 1)
            {
                EditString[EditCursor] = letter;
                EditCursor ++;
                EditString[EditCursor] = '\0';
            }
        }
        else if (letter == 13 || letter == 10) // return, enter
        {
            if (strlen(EditString) > 1)
            {
                sprintf(Hier_Names[HierIndex], "%s", EditString);
                replace_Hierarchy_Name(Name_Remember, EditString);
                sprintf(Name_Remember, "%s", EditString);
            }
            else
            {
                sprintf(Hier_Names[HierIndex], "%s", Name_Remember);
            }
            Edit_Lock = 0;
            EditCursor = 0;
            printf("Edit finishing!\n");
            set_Hier_H_Button(-1);
            //update = 1;
        }
        else if (letter == 8) // backspace
        {
            EditCursor --;
            if (EditCursor < 0)
                EditCursor = 0;
            EditString[EditCursor] = '\0';
        }
        sprintf(Hier_Names[HierIndex], "%s", EditString);
        update_Hierarcys_List(0, 0);
        //printf("%c%s", 13, EditString);
        message = 0;
    }
    else if (letter == 13 || letter == 10) // return, enter
    {
        select_Transformer();
    }
}

void handle_dialog(char letter, SDLMod mod)
{
    if (UPDATE_BACKGROUND)
    {
        DRAW_UI = 0;
        poly_Render(tripsRender, wireframe, splitview, CamDist, 0, subdLevel);
        DRAW_UI = 1;
    }
    if (dialog_type == SAVES_DIALOG)
    {
        handle_Scene_Dialog(letter, mod);
        if (!Edit_Lock && letter == '`')
        {
            rename_Scene_dir();
        }
    }
    else if (dialog_type == LOADING_DIALOG)
    {
        handle_Scene_Dialog(letter, mod);
        if (!Edit_Lock && letter == '`')
        {
            rename_Scene_dir();
        }
    }
    else if (dialog_type == BONE_DIALOG)
    {
        handle_Bone_Dialog(letter, mod);
        if (!Edit_Lock && letter == '`')
        {
            rename_Bone();
        }
    }
    else if (dialog_type == IK_DIALOG)
    {
        handle_IK_Dialog(letter, mod);
        if (!Edit_Lock && letter == '`')
        {
            rename_IK();
        }
    }
    else if (dialog_type == POSE_DIALOG)
    {
        handle_Pose_Dialog(letter, mod);
        if (!Edit_Lock && letter == '`')
        {
            rename_Pose();
        }
    }
    else if (dialog_type == DEFR_DIALOG)
    {
        if (current_defr_type == 0)
        {
            handle_Defr_Dialog(letter, mod);
        }
        else if (current_defr_type == 1)
        {
            handle_Hier_Dialog(letter, mod);
            if (!Edit_Lock && letter == '`')
            {
                edit_Hierarchy();
                update_Hierarcys_List(0, 1);
            }
        }
        else if (current_defr_type == 2)
        {
            handle_Sels_Dialog(letter, mod);
            if (!Edit_Lock && letter == '`')
            {
                edit_Selection();
                UPDATE_BACKGROUND = 1;
                update_Selections_List(0, 1);
            }
        }
    }
    else if (dialog_type == HIER_DIALOG)
    {
        handle_Hier_Dialog(letter, mod);
        if (!Edit_Lock && letter == '`')
        {
            edit_Hierarchy();
            update_Hierarcys_List(1, 0);
        }
    }
    else if (dialog_type == SELS_DIALOG)
    {
        handle_Sels_Dialog(letter, mod);
        if (!Edit_Lock && letter == '`')
        {
            edit_Selection();
            UPDATE_BACKGROUND = 1;
            update_Selections_List(0, 1);
        }
    }
    else if (dialog_type == TEXT_DIALOG)
    {
        if (letter == 'a')
        {
            texts_start = 0;
        }
//        else
//        {
//            letter -= 32;
//            int start_Big = 0;
//            int start_Low = 0;
//            int texts_num = Textures_c;
//            printf("texts num %d\n", texts_num);
//            int i;
//            for (i = 0; i < texts_num; i ++)
//            {
//                if (Item_List[i][0] == letter)
//                {
//                    start_Big = i;
//                    break;
//                }
//            }
//            letter += 32;
//            for (i = 0; i < texts_num; i ++)
//            {
//                if (Item_List[i][0] == letter)
//                {
//                    start_Low = i;
//                    break;
//                }
//            }
//            if (start_Big)
//                texts_start = start_Big;
//            else if (start_Low)
//                texts_start = start_Low;
//            if (texts_start > texts_num - LISTLENGTH)
//                texts_start = texts_num - LISTLENGTH;
//            if (texts_start < 0) texts_start = 0;
//        }
        if (UPDATE_BACKGROUND)
        {
            draw_Textures_Dialog("Textures List", screen_height, text, texts, texts_count, texts_start, 1, currentObject);
        }
        else
        {
            draw_Textures_List(screen_height, texts_start, text, 1);
        }
        SDL_GL_SwapBuffers();
    }
    else if (dialog_type == ITEM_DIALOG)
    {
        if (letter == 'u')
        {
            unhide_Object(currentObject);
            UPDATE_BACKGROUND = 1;
        }
        else if (letter == 8) // backspace
        {
            hide_Object(currentObject);
            UPDATE_BACKGROUND = 1;
        }
        else if (letter == 'a')
        {
            items_start = 0;
        }
        else
        {
            letter -= 32;
            int start_Big = 0;
            int start_Low = 0;
            int items_num = query_items(item);
            printf("items num %d\n", items_num);
            int i;
            for (i = 0; i < items_num; i ++)
            {
                if (Item_List[i][0] == letter)
                {
                    start_Big = i;
                    break;
                }
            }
            letter += 32;
            for (i = 0; i < items_num; i ++)
            {
                if (Item_List[i][0] == letter)
                {
                    start_Low = i;
                    break;
                }
            }
            if (start_Big)
                items_start = start_Big;
            else if (start_Low)
                items_start = start_Low;
            if (items_start > items_num - LISTLENGTH)
                items_start = items_num - LISTLENGTH;
            if (items_start < 0) items_start = 0;
        }
        if (UPDATE_BACKGROUND)
        {
            all_objects_in_frame(Camera);
            if (!NVIDIA) glDrawBuffer(GL_FRONT_AND_BACK);
            DRAW_UI = 0;
            poly_Render(tripsRender, wireframe, splitview, CamDist, 0, subdLevel);
            ItemList[currentObject - items_start].color = UI_BACKL;
            draw_Items_Dialog("Items List", screen_height, item, items, items_count, items_start, 1, currentObject);
            ItemList[currentObject - items_start].color = UI_BLACK;
            DRAW_UI = 1;
            glDrawBuffer(GL_BACK);
        }
        else
        {
            draw_Items_List(screen_height, items_start, item, 1, currentObject);
        }
        SDL_GL_SwapBuffers();
        UPDATE_BACKGROUND = 0;
    }
    else if (dialog_type == OBJ_DIALOG || dialog_type == IMG_DIALOG)
    {
        if (letter == 'a')
        {
            files_start = 0;
        }
        else
        {
            letter -= 32;
            int start_Big = 0;
            int start_Low = 0;
            files_num = query_files(files_dir, extension);
            printf("files num %d\n", files_num);
            int f;
            for (f = 0; f < files_num; f ++)
            {
                if (Out_List[f][0] == letter)
                {
                    start_Big = f;
                    break;
                }
            }
            letter += 32;
            for (f = 0; f < files_num; f ++)
            {
                if (Out_List[f][0] == letter)
                {
                    start_Low = f;
                    break;
                }
            }
            if (start_Big)
                files_start = start_Big;
            else if (start_Low)
                files_start = start_Low;
            if (files_start > files_num - LISTLENGTH)
                files_start = files_num - LISTLENGTH;
            if (files_start < 0) files_start = 0;
        }
        if (img_extension_type == obj_EXTENSION)
            obj_files_start = files_start;
        else if (img_extension_type == OBJ_EXTENSION)
            OBJ_files_start = files_start;
        else if (img_extension_type == png_EXTENSION)
            png_files_start = files_start;
        else if (img_extension_type == jpg_EXTENSION)
            jpg_files_start = files_start;
        else if (img_extension_type == tif_EXTENSION)
            tif_files_start = files_start;
        else if (img_extension_type == PSD_EXTENSION)
            PSD_files_start = files_start;
        if (UPDATE_BACKGROUND)
        {
            draw_Files_Dialog(files_dir, "Open file", screen_height, extension, extensions, ext_count, files_start, 1, 0, Edit_Lock);
        }
        else
        {
            draw_Files_List(files_dir, screen_height, files_start, extension, 1, 0);
        }
        SDL_GL_SwapBuffers();
    }
    UPDATE_BACKGROUND = 0;
}

void draw_Dialog()
{
    Button_sidebar[Buttonindex_sidebar].color = UI_GRAYD;
    //SDL_SetCursor(Arrow);
    if (dialog_type == SAVES_DIALOG)
    {
        open_Saves_List();
    }
    else if (dialog_type == LOADING_DIALOG)
    {
        open_Loading_List();
    }
    else if (dialog_type == BONE_DIALOG)
    {
        open_Bones_List();
    }
    else if (dialog_type == IK_DIALOG)
    {
        open_IK_List();
    }
    else if (dialog_type == POSE_DIALOG)
    {
        open_Poses_List();
    }
    else if (dialog_type == DEFR_DIALOG)
    {
        open_Deformers_List();
    }
    else if (dialog_type == HIER_DIALOG)
    {
        open_Hierarchys_List();
    }
    else if (dialog_type == SELS_DIALOG)
    {
        open_Selections_List();
    }
    else if (dialog_type == TEXT_DIALOG)
    {
        open_Textures_List();
    }
    else if (dialog_type == ITEM_DIALOG)
    {
        open_Items_List();
    }
    else if (dialog_type == MATERIAL_DIALOG)
    {
        open_Materials_List();
    }
    else
    {
        if (dialog_type == OBJ_DIALOG)
        {
            open_OBJ_List();
        }
        else if (dialog_type == IMG_DIALOG)
        {
            open_Img_List();
        }
    }
    Button_sidebar[Buttonindex_sidebar].color = UI_GRAYB;
}

void handle_ControlDown()
{
    if (controlDown)
    {
        add_selection_mode = 0;
        if (!Object_Mode)
        {
            if (cull_Selection)
                SDL_SetCursor(Arrow_Minus_Cull);
            else
                SDL_SetCursor(Arrow_Minus);
        }
    }
    else
    {
        add_selection_mode = 1;
        if (!Object_Mode)
        {
            if (cull_Selection)
                SDL_SetCursor(Arrow_Plus_Cull);
            else
                SDL_SetCursor(Arrow_Plus);
        }
    }
}

void clear_Selection()
{
    if (Object_Mode)
    {
        int o;
        object * O;
        for (o = 0; o < objectIndex; o ++)
        {
            O = objects[o];
            O->selected = 0;
        }
        O = objects[currentObject];
        O->selected = 1;
        selected_object_count = 1;
        selected_objects[0] = currentObject;
    }
    else if (Polygon_Mode)
    {
        int l, o;
        camera * C = Camera;
        C->objects[0] = currentObject;
        C->object_count = 1;
        for (l = 0; l < SUBD; l++)
            load_id_colors(selected_objects, selected_object_count, l, -1);
        load_id_colors_Fan(selected_objects, selected_object_count, -1);
        object * O;
        for (o = 0; o < selected_object_count; o ++)
        {
            O = objects[selected_objects[o]];
            assign_Selection(O, 0);
        }
    }
    else if (Edge_Mode)
    {
        int o;
        object * O;
        for (o = 0; o < selected_object_count; o ++)
        {
            O = objects[selected_objects[o]];
            clear_Edge_Selection(O);
        }
    }
    else if (Vertex_Mode)
    {
        int o;
        object * O;
        for (o = 0; o < selected_object_count; o ++)
        {
            O = objects[selected_objects[o]];
            clear_Verts_Selection(O);
        }
    }
}

void unparent(transformer * T, transformer * P)
{
    if (P != &World && P != NULL)
    {
        if (!BIND_POSE)
            normalize_rotation_unparent(T);
        remove_Child(T, P, &World);
    }
}

void parent(transformer * T, transformer * P)
{
    if (strcmp(T->Name, "Object 5") == 0)
    {
        /*CUBECOMMENT*/
    }
    else if (T->Bone == NULL || (T->Bone && T->Bone->A == T))
    {
        if (T->parent != NULL)
        {
            unparent(T, P);
            remove_Child(T, T->parent, P);
            if (!BIND_POSE)
                normalize_rotation_parent(T);
        }
    }
}

void parent_Locators()
{
    int t, p, condition;
    transformer * T;
    selected_transformer_count = 0;
    transformer * current_T;
    current_T = transformers[currentLocator];

    transformer * P = current_T;
    transformer * parent_transformers[transformerIndex];
    int parent_transformers_count = 0;
    while (P != &World)
    {
        P = P->parent;
        parent_transformers[parent_transformers_count ++] = P;
    }
    for (t = 0; t < transformerIndex; t ++)
    {
        T = transformers[t];
        if (T->selected && T != current_T)
        {
            condition = 1;
            for (p = 0; p < parent_transformers_count; p ++)
            {
                if (parent_transformers[p] == T)
                {
                    condition = 0;
                }
            }
            if (condition)
            {
                selected_transformers[selected_transformer_count ++] = t;
            }
        }
    }
    for (t = 0; t < selected_transformer_count; t ++)
    {
        T = transformers[selected_transformers[t]];
        parent(T, current_T);
    }
    if (dialog_lock)
    {
        set_Hier_H_Button(1);
        draw_Dialog();
    }
}

void unparent_Locator()
{
    transformer * current_T;
    current_T = transformers[currentLocator];
    if (current_T->Bone == NULL || (current_T->Bone && T->Bone->A == T))
    {
        unparent(current_T, current_T->parent);

        if (dialog_lock)
        {
            set_Hier_H_Button(2);
            draw_Dialog();
        }
    }
}

int add_New_Bone()
{
    if (bonesIndex >= BONES - 1)
        return -1;
    transformer * current_T = transformers[currentLocator];

    //create_Transformers_List();

    if (transformerIndex < TRANSFORMERS)
    {
        transformer * T = calloc(1, sizeof(transformer));

        Locators[locatorIndex ++] = T;

        init_transformer(T, current_T, "Locator");

        memcpy(T->pos, current_T->pos, sizeof(T->pos));
    }
    else
    {
        return -1;
    }
    if (transformerIndex < TRANSFORMERS)
    {
        currentLocator = transformerIndex - 1;
        current_T = transformers[currentLocator];

        transformer * T = calloc(1, sizeof(transformer));

        Locators[locatorIndex ++] = T;

        init_transformer(T, current_T, "Locator");

        memcpy(T->pos, current_T->pos, sizeof(T->pos));

        currentLocator = transformerIndex - 1;

        return add_Bone(current_T, transformers[currentLocator]);
    }
    else
    {
        return -1;
    }
}

void new_Locator()
{
    // place new transformer under currentLocator
    transformer * current_T = transformers[currentLocator];

    create_Transformers_List();

    if (transformerIndex < TRANSFORMERS)
    {
        T = calloc(1, sizeof(transformer));

        Locators[locatorIndex ++] = T;

        if (BIND_POSE)
        {
            init_transformer(T, &World, "Locator");
        }
        else
        {
            init_transformer(T, current_T, "Locator");
        }
        T->selected = 1;
    }

    create_Hierarchys_List();

    if (dialog_lock)
    {
        set_Hier_H_Button(3);
        UPDATE_BACKGROUND = 0;
        update_Hierarcys_List(0, 0);
    }
}

void delete_Bone_Transformer(transformer * T0)
{
    int l, c;

    int index;

    int condition;

    //create_Transformers_List();

    child_collection_count = 0;

    for (c = 0; c < T0->childcount; c ++)
    {
        child_collection[child_collection_count ++] = T0->childs[c];
    }
    for (c = 0; c < child_collection_count; c ++)
    {
        if (BIND_POSE)
        {
            remove_Child(child_collection[c], T0, T0->parent);
        }
        else
        {
            normalize_rotation_unparent(child_collection[c]);
            remove_Child(child_collection[c], T0, T0->parent);
            normalize_rotation_parent(child_collection[c]);
        }
    }
    remove_Child(T0, T0->parent, NULL);

    vert_selection * S;

    int s;

    for (s = 0; s < T0->Selections_Count; s ++)
    {
        S = T0->Selections[s];
        S->Transformer = NULL;
    }

    if (T0->Deformer != NULL)
    {
        remove_Transformer_From_Deformer(T0);
    }

    condition = 0;

    for (l = 0; l < locatorIndex; l ++)
    {
        if (T0 == Locators[l])
        {
            index = l;
            condition = 1;
            break;
        }
    }

    if (condition)
    {
        locatorIndex --;
        for (l = index; l < locatorIndex; l ++)
        {
            Locators[l] = Locators[l + 1];
        }
    }
/*
    condition = 0;

    for (t = 0; t < transformerIndex; t ++)
    {
        if (T0 == transformers[t])
        {
            index = t;
            condition = 1;
            break;
        }
    }

    if (condition)
    {
        transformerIndex --;
        for (t = index; t < transformerIndex; t ++)
        {
            transformers[t] = transformers[t + 1];
        }
    }
*/
    free_Transformer(T0);
    free(T0);

    create_Hierarchys_List();
    if (currentLocator >= transformerIndex - 1)
        currentLocator = transformerIndex - 1;
    if (currentLocator < 0)
        currentLocator = 0;

    T = transformers[currentLocator];
}

void delete_IK_Transformers(ikChain * I)
{
    int c;

    for (c = I->B->childcount - 1; c >= 0; c --)
    {
        if (BIND_POSE)
        {
            remove_Child(I->B->childs[c], I->B, I->Bones[I->bonescount - 1]->B);
        }
        else
        {
            normalize_rotation_unparent(I->B->childs[c]);
            remove_Child(I->B->childs[c], I->B, I->Bones[I->bonescount - 1]->B);
            normalize_rotation_parent(I->B->childs[c]);
        }
    }
    remove_Child(I->Bones[0]->A, I->A, I->A->parent);
    I->Bones[0]->A->collapsed = 0;
    remove_Child(I->B, I->B->parent, NULL);
    remove_Child(I->A, I->A->parent, NULL);

    if (I->B->Deformer != NULL)
    {
        remove_Transformer_From_Deformer(I->B);
    }

    if (I->A->Deformer != NULL)
    {
        remove_Transformer_From_Deformer(I->A);
    }

    free_Transformer(I->B);
    free(I->B);
    free_Transformer(I->A);
    free(I->A);

    create_Hierarchys_List();

    if (currentLocator >= transformerIndex - 1)
        currentLocator = transformerIndex - 1;
    if (currentLocator < 0)
        currentLocator = 0;
}

void delete_Transformer(transformer * T0)
{
    int c;

    //create_Transformers_List();

    selected_transformer_count = 0;

    child_collection_count = 0;

    for (c = 0; c < T0->childcount; c ++)
    {
        child_collection[child_collection_count ++] = T0->childs[c];
    }
    for (c = 0; c < child_collection_count; c ++)
    {
        if (BIND_POSE)
        {
            remove_Child(child_collection[c], T0, T0->parent);
        }
        else
        {
            normalize_rotation_unparent(child_collection[c]);
            remove_Child(child_collection[c], T0, T0->parent);
            normalize_rotation_parent(child_collection[c]);
        }
    }
    remove_Child(T0, T0->parent, NULL);

    if (T0->Deformer != NULL)
    {
        remove_Transformer_From_Deformer(T0);
    }

    free_Transformer(T0);
    free(T0);

    create_Hierarchys_List();

    if (currentLocator >= transformerIndex - 1)
        currentLocator = transformerIndex - 1;
    if (currentLocator < 0)
        currentLocator = 0;

    T = transformers[currentLocator];

    if (dialog_lock)
    {
        set_Hier_H_Button(4);
        UPDATE_BACKGROUND = 0;
        update_Hierarcys_List(0, 0);
    }
}

void delete_Locator()
{
    printf("delete Locator\n");

    int c, l, index, condition;

    selected_transformer_count = 0;

    transformer * current_T = transformers[currentLocator];

    //create_Transformers_List();

    condition = 0;

    for (l = 0; l < locatorIndex; l ++)
    {
        if (current_T == Locators[l])
        {
            index = l;
            condition = 1;
            break;
        }
    }

    if (condition)
    {
        if (!BIND_POSE && current_T->Deformer != NULL)
        {
            condition = 0;
        }
    }

    if (condition && current_T->Bone == NULL)
    {
        printf("condition\n");

        transformer * collect_childs[current_T->childcount];
        int collect_c = 0;
        for (c = 0; c < current_T->childcount; c ++)
        {
            collect_childs[collect_c ++] = current_T->childs[c];
        }
        for (c = 0; c < collect_c; c ++)
        {
            if (BIND_POSE)
            {
                remove_Child(collect_childs[c], current_T, current_T->parent);
            }
            else
            {
                normalize_rotation_unparent(collect_childs[c]);
                remove_Child(collect_childs[c], current_T, current_T->parent);
                normalize_rotation_parent(collect_childs[c]);
            }
        }
        remove_Child(current_T, current_T->parent, NULL);

        vert_selection * S;

        int s;

        for (s = 0; s < current_T->Selections_Count; s ++)
        {
            S = current_T->Selections[s];
            S->Transformer = NULL;
        }

        locatorIndex --;
        for (l = index; l < locatorIndex; l ++)
        {
            Locators[l] = Locators[l + 1];
        }
        if (current_T->Deformer != NULL)
        {
            remove_Transformer_From_Deformer(current_T);
        }
        free_Transformer(current_T);
        free(current_T);
    }

    create_Hierarchys_List();
    if (currentLocator >= transformerIndex - 1)
        currentLocator = transformerIndex - 1;
    if (currentLocator < 0)
        currentLocator = 0;

    T = transformers[currentLocator];

    if (dialog_lock)
    {
        set_Hier_H_Button(4);
        UPDATE_BACKGROUND = 0;
        update_Hierarcys_List(0, 0);
    }
}

void select_Parent_Locator()
{
    T = transformers[currentLocator];
    if (T->parent != NULL)
    {
        transformer * P = T->parent;

        if (T->Bone != NULL)
        {
            P = P->parent;
        }

        int t;
        for (t = 0; t < transformerIndex; t ++)
        {
            if (transformers[t] == P)
            {
                currentLocator = t;
                T = transformers[currentLocator];
                break;
            }
        }
    }
}

void select_Child_Locator()
{
    T = transformers[currentLocator];
    if (T->childcount > 0)
    {
        transformer * C = T->childs[0];

        if (T->Bone != NULL && C->childcount > 0)
        {
            C = C->childs[0];
        }

        int t;
        for (t = 0; t < transformerIndex; t ++)
        {
            if (transformers[t] == C)
            {
                currentLocator = t;
                T = transformers[currentLocator];
                break;
            }
        }
    }
}

void remove_Branch()
{
    printf("remove Branch\n");
    set_Defr_H_Button(1);
    if (BIND_POSE && deformerIndex > 0)
    {
        deformer * D = deformers[currentDeformer_Node];
        transformer * T = transformers[currentLocator];
        remove_Branch_From_Deformer(T, D);
        remove_IK_Chains_From_Deformer(T, D);
        create_Deformers_List(SelsIndex[3], O);
        draw_Deformers_List(screen_height, defr_start, 1, currentDeformer);
        draw_Deformers_Bottom_Line(DIALOG_WIDTH, screen_height);
        SDL_GL_SwapBuffers();
    }
}

void bind_Selection()
{
    printf("bind Selection\n");
    set_Defr_H_Button(2);
    if (deformerIndex > 0)
    {
        deformer * D = deformers[currentDeformer_Node];
        transformer * T = transformers[currentLocator];
        if (SelsIndex[3] >= 0 && SelsIndex[3] < Selections_c[3])
        {
            int o;
            for (o = 0; o < selected_object_count; o ++)
            {
                if (strcmp(O->Name, "Cube") == 0) /*CUBECOMMENT*/
                    continue;
                add_Selection_To_Deformer(objects[selected_objects[o]], Sels_Names[3][SelsIndex[3]], T, D);
            }

            normalize_Deformer_Selections(D);
        }
        create_Deformers_List(SelsIndex[3], O);
        draw_Dialog();
    }
}

void unbind_Selection()
{
    printf("unbind Selection\n");
    set_Defr_H_Button(3);
    if (currentLocator >= 0 && currentLocator < transformerIndex)
    {
        deformer * D = deformers[currentDeformer_Node];
        transformer * T = transformers[currentLocator];
        if (SelsIndex[3] >= 0 && SelsIndex[3] < Selections_c[3])
        {
            remove_Selection_From_Transformer(O, T, Sels_Names[3][SelsIndex[3]]);
            remove_Selection_From_Deformer(O, Sels_Names[3][SelsIndex[3]], D);
            normalize_Deformer_Selections(D);
        }
        create_Deformers_List(SelsIndex[3], O);
        draw_Dialog();
    }
}

void paste_Default_Pose()
{
    int d;

    deformer * D;

    for (d = 0; d < deformerIndex; d ++)
    {
        D = deformers[d];
        paste_Pose_(D, D->Poses[0]);
    }
}

void move_Deformers_To_Delta_Position()
{
    int d;

    deformer * D;
    transformer * T;

    Update_Objects_Count = 0;

    for (d = 0; d < deformerIndex; d ++)
    {
        D = deformers[d];

        rotate_collect_Deformers(D);
    }

    for (d = 0; d < deformerIndex; d ++)
    {
        D = deformers[d];

        if (D->Transformers_Count > 0)
        {
            T = D->Transformers[0];

            move_Pose_T(T, D->Delta);

            move_Deformer_verts(D);
        }
    }

    update_rotate_bounding_box();
}

void set_Bind_Mode()
{
    Draw_Bottom_Message("Bind Mode\n");

    Button_Mode[0].color = UI_GRAYB;
    Button_Mode[1].color = UI_GRAYB;
    Button_Mode[2].color = UI_GRAYB;
    Button_Mode[3].color = UI_GRAYB;

    SDL_SetCursor(Arrow);

    BIND_POSE = !BIND_POSE;

    create_Transformers_List(); // all transformer tree

    if (BIND_POSE)
    {
        DRAW_LOCATORS = 1;
        LOCAT_ID_RENDER = DRAW_LOCATORS;

        set_Deformers_Delta(); // to move Deformers To Delta Position

        set_Bind_Pose_For_Transformers(0);

        goto_Bind_Pose_For_Transformers();

//        bake(&World);
//        rotate(&World);

        paste_Default_Pose();

        paste_rotVec(); // because of synthesize_Bone_Axis // axis alignement during animation

        normalize_rotation_unparent_childs(&World);

        reset_Bones_Rotation();

        create_Hierarchys_List(); // because of collapsed locators

        Button_Mode[4].color = UI_GRAYD;
    }
    else
    {
        reset_Bones_Rotation();

        set_Bone_len(); // IK theme

        update_IKchains(); // IK theme

        set_Bind_Pose_For_Transformers(1);

        normalize_rotation_parent_childs(&World);

        transfer_rotVec_B(&World);

        transfer_Bones_rotVec_();

        reset_Scale();

        update_Default_Pose();

        move_Deformers_To_Delta_Position();

        create_Hierarchys_List(); // because of collapsed locators

        paste_rotVec_(); // because of synthesize_Bone_Axis // axis alignement during animation

        Button_Mode[4].color = UI_GRAYB;
        Button_Mode[selection_Mode].color = UI_GRAYD;

        set_Object_Mode();
    }

    poly_Render(tripsRender, wireframe, splitview, CamDist, 1, subdLevel);

    message = -1;
}

void new_Deformer()
{
    printf("new Deformer\n");
    add_Deformer();
    set_Defr_H_Button(4);

    currentDeformer_Node = deformerIndex - 1;
    add_Default_Pose_To_Deformer(deformers[currentDeformer_Node]);
//    PoseIndex ++;
//    if (Pose_List[PoseIndex] >= 0)
//    {
//        currentPose = Pose_List[PoseIndex];
//    }

    assert_Deformers_Selected();

    create_Deformers_List(SelsIndex[3], O);
    if (DefrIndex - defr_start >= 0)
        DefrList[DefrIndex - defr_start].color = UI_BLACK;
    if (dialog_lock)
    {
        draw_Dialog();
    }
}

void delete_Deformer()
{
    printf("delete Deformer\n");
    if (deformerIndex > 0)
    {
        deformer * D = deformers[currentDeformer_Node];
        delete_Deformer_Poses(D);
        remove_Deformer(D);
        if (currentDeformer_Node >= deformerIndex)
            currentDeformer_Node = deformerIndex - 1;
        if (currentDeformer_Node < 0)
            currentDeformer_Node = 0;

//        PoseIndex --;
//        if (Pose_List[PoseIndex] >= 0)
//        {
//            currentPose = Pose_List[PoseIndex];
//        }

        assert_Deformers_Selected();

        set_Defr_H_Button(5);

        create_Deformers_List(SelsIndex[3], O);

        if (DefrIndex - defr_start >= 0)
            DefrList[DefrIndex - defr_start].color = UI_BLACK;
        defr_start = 0;
        draw_Deformers_List(screen_height, defr_start, 1, currentDeformer);

        draw_Deformers_Bottom_Line(DIALOG_WIDTH, screen_height);
        SDL_GL_SwapBuffers();
    }
}

void select_Locator_Selections(int currentLocator)
{
    T = transformers[currentLocator];
    object * O0;
    vertex * V;
    triangle * T0;
    polygon * P;
    quadrant * Q;
    vert_selection * S = NULL;

    int idx, e, t, p, i, v, s;
    float w;
    id_color I;
    id_color I0;
    int c_c;

    for (s = 0; s < T->Selections_Count; s ++)
    {
        S = T->Selections[s];
        O0 = S->Object;
        if (O0 == O)
        {
            c_c = 0;

            for (v = 0; v < O0->vertcount; v ++)
            {
                V = &O0->verts[v / ARRAYSIZE][v % ARRAYSIZE];
                V->selected = 0;
            }
            for (v = 0; v < O0->vertcount; v ++)
            {
                V = &O0->verts[v / ARRAYSIZE][v % ARRAYSIZE];
                for (i = 0; i < S->indices_count; i ++)
                {
                    if (S->indices[i] == v)
                    {
                        V->selected = 1;
                    }
                }
            }
            for (p = 0; p < O0->polycount; p ++)
            {
                P = &O0->polys[p / ARRAYSIZE][p % ARRAYSIZE];
                if (P->selected == 1)
                {
                    I0.R = 0;
                    I0.G = 0;
                    I0.B = 1;
                    I0.A = 1;
                }
                else
                {
                    I0.R = Materials[P->surface].R / 255;
                    I0.G = Materials[P->surface].G / 255;
                    I0.B = Materials[P->surface].B / 255;
                    I0.A = Materials[P->surface].A / 255;
                }

                if (ELEMENT_ARRAYS)
                {
                    for (t = 0; t < P->tripcount; t ++)
                    {
                        idx = P->trips[t];
                        T0 = &O0->trips[idx / ARRAYSIZE][idx % ARRAYSIZE];
                        for (v = 0; v < 3; v ++)
                        {
                            idx = T0->verts[v];
                            V = &O0->verts[idx / ARRAYSIZE][idx % ARRAYSIZE];
                            if (V->selected)
                            {
                                for (i = 0; i < S->indices_count; i ++)
                                {
                                    if (S->indices[i] == V->index)
                                    {
                                        w = S->weights[i];
                                        I.R = 1;
                                        I.G = 1 - w;
                                        I.B = 1 - w;
                                        I.A = 1;
                                        break;
                                    }
                                }
                            }
                            else
                            {
                                I.R = I0.R;
                                I.G = I0.G;
                                I.B = I0.B;
                                I.A = I0.A;
                            }
                            idx = T0->texts[v];
                            c_c = idx * 4;
                            O0->cols_array[1][c_c++] = I.R;
                            O0->cols_array[1][c_c++] = I.G;
                            O0->cols_array[1][c_c++] = I.B;
                            O0->cols_array[1][c_c++] = I.A;

                        }
                    }
                }
                else
                {
                    for (e = 0; e < P->edgecount; e ++)
                    {
                        idx = P->verts[e];
                        V = &O0->verts[idx / ARRAYSIZE][idx % ARRAYSIZE];
                        if (V->selected)
                        {
                            for (i = 0; i < S->indices_count; i ++)
                            {
                                if (S->indices[i] == V->index)
                                {
                                    w = S->weights[i];
                                    I.R = 1;
                                    I.G = 1 - w;
                                    I.B = 1 - w;
                                    I.A = 1;
                                    break;
                                }
                            }
                        }
                        else
                        {
                            I.R = I0.R;
                            I.G = I0.G;
                            I.B = I0.B;
                            I.A = I0.A;
                        }

                        O0->cols_array[0][c_c++] = I.R;
                        O0->cols_array[0][c_c++] = I.G;
                        O0->cols_array[0][c_c++] = I.B;
                        O0->cols_array[0][c_c++] = I.A;
                    }
                }

                if (subdLevel > -1 && O0->subdlevel > -1)
                {
                    for (e = 0; e < P->edgecount; e ++)
                    {
                        idx = P->quads[e];
                        Q = &O0->quads_[0][idx / ARRAYSIZE][idx % ARRAYSIZE];
                        assign_Quad_Color(O0, Q, &I, 0);
                    }
                }
            }
        }
    }
//    if (S != NULL)
//        set_Vert_Selection(S);
}

void start_Movement()
{
    subdLevel_mem = subdLevel;

    Camera = find_View(mouse_x, mouse_y, splitview);
    Camera->origin_2d[0] = mouse_x;
    Camera->origin_2d[1] = mouse_y;

    ObjDist = distance(O->T->pos, Camera->T->pos);
    T = Camera->T;

    if ((mod & KMOD_SHIFT) || BONES_MODE)
    {
        object_hook = 1;
        MOVEMENT = 1;

        if (DRAW_LOCATORS)
        {
            if (currentLocator == 0)
                currentLocator = transformerIndex - 1;
            T = transformers[currentLocator];
        }
        else
        {
            T = O->T;
        }

        direction D1;
        direction D;
        if (splitview)
        {
            if (Camera == &Camera_Front)
            {
                D = screen_point_to_vector(mouse_x - SIDEBAR, mouse_y - screen_height / 2, screen_width / 2, screen_height / 2, Camera_Front.h_view, Camera_Front.v_view);
                ObjDist = distance(T->pos, Camera_Front.T->pos);
            }
            else if  (Camera == &Camera_Top)
            {
                D = screen_point_to_vector(mouse_x - SIDEBAR, mouse_y, screen_width / 2, screen_height / 2, Camera_Top.h_view, Camera_Top.v_view);
                ObjDist = distance(T->pos, Camera_Top.T->pos);
            }
            else if  (Camera == &Camera_Left)
            {
                D = screen_point_to_vector(mouse_x - SIDEBAR - screen_width / 2, mouse_y - screen_height / 2, screen_width / 2, screen_height / 2, Camera_Left.h_view, Camera_Left.v_view);
                ObjDist = distance(T->pos, Camera_Left.T->pos);
            }
            else if (Camera == &Camera_Persp)
            {
                D = screen_point_to_vector(mouse_x - SIDEBAR - screen_width / 2, mouse_y, screen_width / 2, screen_height / 2, Camera_Persp.h_view, Camera_Persp.v_view);
                ObjDist = distance(T->pos, Camera_Persp.T->pos);
            }
        }
        else
        {
            D = screen_point_to_vector(mouse_x - SIDEBAR, mouse_y, screen_width, screen_height, Camera->h_view, Camera->v_view);
            ObjDist = distance(T->pos, Camera->T->pos);
        }
        D.x = -D.x;
        //D.y = -D.y;
        D.z = -D.z;
        rotate_Vertex_I(Camera->T->rotVec, D.x, D.y, D.z, &D1);
        float Dir_vec[3] = {0, 0, 1};
        float dot = dot_productN((normal*)&D, Dir_vec);
        T_pos[0] = Camera->T->pos[0] + D1.x * (ObjDist / dot);
        T_pos[1] = Camera->T->pos[1] + D1.y * (ObjDist / dot);
        T_pos[2] = Camera->T->pos[2] + D1.z * (ObjDist / dot);

        Update_Objects_Count = 0;
        if (T->Deformer != NULL && T->Deformer->Transformers_Count > 0)
        {
            rotate_collect(T->Deformer->Transformers[0]);
        }
        else
        {
            rotate_collect(T);
        }
        printf("Update Objects Count %d\n", Update_Objects_Count);
        collect_Children(T);

        bake_position(T);
        bake_position_Children(T);
        bake(T);
    }
    else if (mod & KMOD_CTRL)
    {
        if (Camera == &Camera_Persp)
            camera_rotate = 1;
    }
    else
    {
        object_hook = 0;
        ROTATION = 0;
        MOVEMENT = 0;
        altDown = 0;
        SCALE = 0;
        Axis_lock = 0;
    }
    memcpy(object_Pos, T->pos, sizeof(object_Pos));
    memcpy(target_Pos, T->target, sizeof(target_Pos));
    memcpy(object_Rot, T->rot, sizeof(object_Rot));
    memcpy(object_Scl, T->scl, sizeof(object_Scl));
    UPDATE_COLORS = 1;
}

void make_Movement()
{
    if (BIND_POSE)
    {
        move_T(T, Delta);
        if (!BONES_MODE && T->Bone == NULL && altDown)
        {
            move_Children_T(T, Delta);
        }
        else if (BONES_MODE)
        {
            synthesize_Bone_Alignement(T->Bone);
        }
        else if (T->Bone != NULL)
        {
            synthesize_Bone_Alignement(T->Bone);
            if (altDown)
            {
                move_Children_T(T, Delta);
            }
            else if (T->Bone->B == T)
            {
                move_T_childs(T, Delta);
                synthesize_Bone_Alignement_childs(T);
            }
        }
    }
    else
    {
        if (T->IK != NULL && (T->style == ik_goal || T->style == ik_fixed))
        {
            T->pos[0] = T->Pos_[0] + Delta[0];
            T->pos[1] = T->Pos_[1] + Delta[1];
            T->pos[2] = T->Pos_[2] + Delta[2];



            if (T->childcount > 0 && T->childs[0]->IK != NULL)
            {
                if (T->childs[0]->IK->bonescount > 1)
                {
                    memcpy(T->childs[0]->pos, T->pos, sizeof(float[3]));
                    if (T->Deformer != NULL)
                        solve_IK_Chains(T->Deformer, 0);
                    move_Deformer_IK(T, 0);

                    memcpy(T->childs[0]->pos, T->pos, sizeof(float[3]));
                    update_rotate_bounding_box();
                }
                else
                {
                    solve_IK_Chain(T->IK, 0);
                    move_(T, Delta, subdLevel);
                }
            }
            else
            {
                solve_IK_Chain(T->IK, 0);
                move_(T, Delta, subdLevel);
            }
        }
        else
        {
            move_(T, Delta, subdLevel);
        }
    }
}

void transform_Objects_And_Render()
{
    int i;

    O = objects[currentObject];

    if (mod || camera_rotate || camera_z_move)
    {
        if (splitview)
        {
            rotate_Camera(&Camera_Persp, CamDist);
            rotate_Camera(&Camera_Top, CamDist);
            rotate_Camera(&Camera_Front, CamDist);
            rotate_Camera(&Camera_Left, CamDist);
        }
        else
        {
            rotate_Camera(Camera, CamDist);
        }
        mod = KMOD_NONE;
    }
    else if (TURNTABLE)
    {
        Camera_Persp.T->rot[1] += 0.01;
        rotate_Camera(&Camera_Persp, CamDist);
    }

    update_camera(Camera, CamDist);

//            O->T->rot[1] -= pi60; // profiling


    if (DRAW_LOCATORS)
    {
        if (T != &World && !BONES_MODE)
        {
            if (ROTATION || SCALE)
            {
                if (BIND_POSE)
                {
                    rotate_bind(T);
                }
                else
                {
                    Update_Objects_Count = 0;
                    if (T->Deformer != NULL)
                    {
                        if (T->Deformer->Transformers_Count > 0)
                        {
                            rotate_collect(T->Deformer->Transformers[0]);
                        }
                        else
                        {
                            rotate_collect(T);
                        }
                        rotate_Deformer(T);
                    }
                    else
                    {
                        rotate_collect(T);
                        rotate_vertex_groups_D_Init();
                        rotate(T);
                    }
                    update_rotate_bounding_box();
                }
            }
            else
            {
                if (BIND_POSE)
                    rotate_bind(T);
                else
                    rotate_T(T);
            }
        }
    }
    else
    {
        if (ROTATION)
        {
            if (BIND_POSE)
                rotate_bind(O->T);
            else
                rotate(O->T);
        }
        else
        {
            if (BIND_POSE)
                rotate_bind(O->T);
            else
                rotate_T(O->T);
        }
    }

    find_Camera_Objects();

    //printf("O rot %f %f %f\n\n", O->T->rot[0], O->T->rot[1], O->T->rot[2]);

    int o;

    object * O0;

    for (o = 0; o < Camera->object_count; o++)
    {
        i = Camera->objects[o];
        O0 = objects[i];

        if (subdLevel < 0)
        {
            if (BIND_POSE || !O0->binding)
                rotate_verts(O0, *O0->T);
        }
        else
        {
            // now deformations sake subdivision is live
            // however bounding box radius is not necessary, maybe leave it out
            if (O0->deforms)
            {
                if (BIND_POSE || !O0->binding)
                    rotate_verts(O0, *O0->T);
                tune_subdivide_post_transformed(O0, subdLevel);
            }
            else
            {
                rotate_verts(O0, *O0->T);
                rotate_verts_(O0, *O0->T, 0);
                rotate_verts_(O0, *O0->T, subdLevel);
            }
        }
    }

    if (fonts_on)
    {
        make_osd(O);
    }

    if (UPDATE_COLORS)
    {
        if (subdLevel > -1)
            poly_Render(tripsRender, wireframe, splitview, CamDist, 0, 0);
    }

    poly_Render(tripsRender, wireframe, splitview, CamDist, 1, subdLevel);

    if (message == -10)
    {
        UPDATE_COLORS = 0;
        printf("update colors\n");
        poly_Render(tripsRender, wireframe, splitview, CamDist, 1, subdLevel);
    }

    if (message == -11)
    {
        glDrawBuffer(GL_BACK);
    }

    //foreground = create_Surface(foreground, screen->w, screen->h, raytrace);
    //apply_surface(0, 0, texture, foreground, &clip);

    //render_counter ++; // profiling
    if (TURNTABLE)
    {
        message = -1;
    }
    else
    {
        message = 0;
    }
}

void clear_Deformers()
{
    if (!dialog_lock)
    {
        printf("clear Deformers\n");

        int d;

        deformer * D;

        for (d = deformerIndex - 1; d >= 0; d --)
        {
            D = deformers[d];
            delete_Deformer_Poses(D);
            remove_Deformer(D);
        }

        currentDeformer_Node = 0;
        Deformers_c = 0;
        Deformer_Node_c = 1;
    }
}

void clear_ikChains(int no_delete)
{
    if (!dialog_lock)
    {
        int i;

        ikChain * I;

        for (i = iksIndex - 1; i >= 0; i --)
        {
            I = ikChains[i];

            remove_ikChain_From_ikChains_(I, no_delete);
        }
    }
}

void clear_Bones()
{
    if (!dialog_lock)
    {
        int b;

        bone * B;

        for (b = bonesIndex - 1; b >= 0; b --)
        {
            B = bones[b];

            remove_Bone_From_Bones_(B);
        }
    }
}

void delete_Object(int index, int render)
{
    if (index > 0 && index < objectIndex && objectIndex > 1) /*CUBECOMMENT*/
    {
        int o, d;

        object * O = objects[index];

        deformer * D;

        int condition = 1;

        for (d = 0; d < deformerIndex; d ++)
        {
            D = deformers[d];
            for (o = 0; o < D->Objects_Count; o ++)
            {
                if (D->Objects[o] == O)
                {
                    condition = 0;
                    break;
                }
            }
            if (condition == 0)
            {
                break;
            }
        }
        if (condition)
        {
            delete_Transformer(O->T);

            remove_Selections_From_Transformers(O);

            objectIndex --;
            for (o = index; o < objectIndex; o ++)
            {
                objects[o] = objects[o + 1];
                loaded_objects[o] = loaded_objects[o + 1];
                objects[o]->index = o;
            }

//            deleted_objects[deleted_objects_count ++] = O;

            free_object(O);

            assert_Object_Selection();
        }
    }

    if (currentObject > objectIndex - 1)
    {
        currentObject = objectIndex - 1;
    }
    O = objects[currentObject];

    if (render)
        transform_Objects_And_Render();
}

void clear_Objects_()
{
    if (!dialog_lock)
    {
        printf("clear Objects_\n");

        int o;

        object * O;

        for (o = objectIndex - 1; o > 0; o --) /*CUBECOMMENT*/
        {

            O = objects[o];

            free_Transformer(O->T);
            free(O->T);

            free_object(O);
        }

        objectIndex = 1;
    }
}

void clear_Objects()
{
    if (!dialog_lock)
    {
        printf("clear Objects\n");

        int i;

        for (i = objectIndex - 1; i > 0; i --) /*CUBECOMMENT*/
        {
            delete_Object(i, 0);
        }
    }
}

void free_deleted_objects()
{
    int o;
    for (o = 0; o < deleted_objects_count; o ++)
    {
        free_object(deleted_objects[o]);
    }
    deleted_objects_count = 0;
}

void clear_Locators()
{
    int t;

    transformer * T;

    for (t = 0; t < locatorIndex; t ++)
    {
        T = Locators[t];
        free_Transformer(T);
        free(T);
    }

    transformerIndex = 6;
    locatorIndex = 0;
}

void clear_All()
{
    if (!dialog_lock)
    {
        clear_ikChains(1);
        clear_Deformers();

        //clear_Objects();
        /* for some reason it fails,
        substituted with clear_Objects_*/

        clear_Bones();
        clear_Locators();
        clear_Objects_();

    /*
    This is cheap clean;

    */

        deformerIndex = 0;
        objectIndex = 1; /*CUBECOMMENT*/
        bonesIndex = 0;
        iksIndex = 0; // IK
        PoseIndex = 0;
        posesCount = 0;
        Materials_count = 4;

        transformerIndex = 6;
        transformerCount = 6;
        locatorIndex = 0;

        HierIndex = 0;
        hier_start = 0;


    /*
    This is cheap clean;

    */

        World.childcount = 5; /*CUBECOMMENT*/

        create_Transformers_List();

        currentObject = 0;
        currentMaterial = 0;
        currentIK = 0;
        materials_start = 0;
        items_start = 0;
        pose_start = 0;
        defr_start = 0;
        hier_start = 0;
        bone_start = 0;
        ikch_start = 0;
        memcpy(sels_start, (int[4]){0, 0, 0, 0}, sizeof(sels_start));
        currentLocator = 5;
        currentPose = 0;
        O = objects[currentObject];
        O->selected = 1;
        T = transformers[currentLocator];
        O->T->Bone = NULL;
        O->T->childcount = 0;

        loaded_objects[0] = 1;

        selected_transformer_count = 0;
        selected_objects[0] = 0;
        selected_object_count = 1;
    }
}

void select_Deformer_Objects()
{
    int o;

    object * O;
    deformer * D = deformers[currentDeformer_Node];

    if (D->Objects_Count > 0)
    {
        for (o = 0; o < objectIndex; o ++)
        {
            O = objects[o];
            O->selected = 0;
        }

        for (o = 0; o < D->Objects_Count; o ++)
        {
            O = D->Objects[o];
            O->selected = 1;
        }

        assert_Object_Selection();
        assert_Current_Object();
    }
}

int makepath(char * file_path, mode_t mode)
{
    int l = strlen(file_path);
    if (l >= STRLEN - 2)
        return 0;
    if (file_path[l - 1] != '/')
    {
        file_path[l] = '/';
        file_path[l + 1] = '\0';
    }
    printf("file_path %s\n", file_path);
    char * p;
    for (p = strchr(file_path + 1, '/'); p; p = strchr(p + 1, '/'))
    {
        *p = '\0';
        if (mkdir(file_path) == -1)
        {
            if (errno != EEXIST)
            {
                *p = '/';
                if (scene_files_dir[strlen(scene_files_dir) - 1] == '/')
                {
                    scene_files_dir[strlen(scene_files_dir) - 1] = '\0';
                }
                return 0;
            }
        }
        chmod(file_path, mode);
        *p = '/';
    }
    if (file_path[l] == '/')
    {
        file_path[l] = '\0';
    }
    return 1;
}

void remove_Ingredients(char * path)
{
    char Path[STRLEN];

    DIR * dir;
    struct dirent * ent;

    if ((dir = opendir(path)) != NULL)
    {
        while ((ent = readdir(dir)) != NULL)
        {
            Path[0] = '\0';
            strcat(Path, path);
            strcat(Path, "/");
            strcat(Path, ent->d_name);
            if (isFile(Path))
            {
                remove(Path);
            }
        }
        closedir(dir);
    }
}

void reload_All_Textures()
{
#if DEBUG_WITHOUT_IL == 0
    int t;

    if (dialog_lock)
    {
        Button_h_text[1].color = UI_GRAYD;
        draw_Textures_Bottom_Line(DIALOG_WIDTH, screen_height);
        SDL_GL_SwapBuffers();
    }

    if (strcmp(text, TEXT_TYPE_TEXT) == 0)
    {
        printf("reload text\n");
        for (t = 1; t < Textures_c; t ++)
        {
            texture = reload_ILU_image(Text_Names_Full[t]);
            if (texture != NULL)
            {
                SDL_FreeSurface(Surf_Text[t]);
                Surf_Text[t] = texture;
                update_TexFrom_Surface(texture, t);
            }
        }
    }
    else if (strcmp(text, TEXT_TYPE_NORM) == 0)
    {
        printf("reload norm\n");
        for (t = 1; t < Normals_c; t ++)
        {
            texture = reload_ILU_image(Norm_Names_Full[t]);
            if (texture != NULL)
            {
                SDL_FreeSurface(Surf_Norm[t]);
                Surf_Norm[t] = texture;
                update_NormFrom_Surface(texture, t);
            }
        }
    }
    else if (strcmp(text, TEXT_TYPE_BUMP) == 0)
    {
        printf("reload bump\n");
        for (t = 1; t < Bumps_c; t ++)
        {
            texture = reload_ILU_image(Bump_Names_Full[t]);
            if (texture != NULL)
            {
                SDL_FreeSurface(Surf_Bump[t]);
                Surf_Bump[t] = texture;
                update_BumpFrom_Surface(texture, t);
            }
        }
    }

    if (dialog_lock)
    {
        DRAW_UI = 0;
        poly_Render(tripsRender, wireframe, splitview, CamDist, 0, subdLevel);
        DRAW_UI = 1;
        draw_Dialog();
        Button_h_text[1].color = UI_GRAYB;
    }
    else
    {
        DRAW_UI = 0;
        poly_Render(tripsRender, wireframe, splitview, CamDist, 1, subdLevel);
        DRAW_UI = 1;
    }
    message = -1;
#endif
}

void modify_Texture_Seams(object * O)
{
#if DEBUG_WITHOUT_IL == 0
    // for displacement to be seamed

    surface_Material * M = &Materials[O->surface];

    if (M->texture > 0)
    {
        printf("modify text\n");

        texture = reload_ILU_image(Text_Names_Full[M->texture]);
        if (texture != NULL)
        {
            SDL_FreeSurface(Surf_Text[M->texture]);

            Surf_Text[M->texture] = texture;
            average_Seam_Pixels(texture, O, subdLevel);
            update_TexFrom_Surface(texture, M->texture);
        }
    }
    if (M->normal > 0)
    {
        printf("modify norm\n");

        texture = reload_ILU_image(Norm_Names_Full[M->normal]);
        if (texture != NULL)
        {
            SDL_FreeSurface(Surf_Norm[M->normal]);

            Surf_Norm[M->normal] = texture;
            average_Seam_Pixels(texture, O, subdLevel);
            update_NormFrom_Surface(texture, M->normal);
        }
    }

    if (M->bump > 0)
    {
        printf("modify bump\n");

        texture = reload_ILU_image(Bump_Names_Full[M->bump]);
        if (texture != NULL)
        {
            SDL_FreeSurface(Surf_Bump[M->bump]);

            Surf_Bump[M->bump] = texture;
            average_Seam_Pixels(texture, O, subdLevel);
            update_BumpFrom_Surface(texture, M->bump);
        }
    }
#endif
}

void reload_Texture()
{
#if DEBUG_WITHOUT_IL == 0
    if (dialog_lock)
    {
        Button_h_text[0].color = UI_GRAYD;
        draw_Textures_Bottom_Line(DIALOG_WIDTH, screen_height);
        SDL_GL_SwapBuffers();
    }

    if (strcmp(text, TEXT_TYPE_TEXT) == 0)
    {
        printf("reload text\n");

        texture = reload_ILU_image(Text_Names_Full[TextIndex]);
        if (texture != NULL)
        {
            SDL_FreeSurface(Surf_Text[TextIndex]);
            Surf_Text[TextIndex] = texture;
            update_TexFrom_Surface(texture, TextIndex);
        }
    }
    else if (strcmp(text, TEXT_TYPE_NORM) == 0)
    {
        printf("reload norm\n");

        texture = reload_ILU_image(Norm_Names_Full[TextIndex]);
        if (texture != NULL)
        {
            SDL_FreeSurface(Surf_Norm[TextIndex]);
            Surf_Norm[TextIndex] = texture;
            update_NormFrom_Surface(texture, TextIndex);
        }
    }
    else if (strcmp(text, TEXT_TYPE_BUMP) == 0)
    {
        printf("reload bump\n");

        texture = reload_ILU_image(Bump_Names_Full[TextIndex]);
        if (texture != NULL)
        {
            SDL_FreeSurface(Surf_Bump[TextIndex]);
            Surf_Bump[TextIndex] = texture;
            update_BumpFrom_Surface(texture, TextIndex);
        }
    }

    if (dialog_lock)
    {
        DRAW_UI = 0;
        poly_Render(tripsRender, wireframe, splitview, CamDist, 0, subdLevel);
        DRAW_UI = 1;
        draw_Dialog();
        Button_h_text[0].color = UI_GRAYB;
    }
    else
    {
        DRAW_UI = 0;
        poly_Render(tripsRender, wireframe, splitview, CamDist, 1, subdLevel);
        DRAW_UI = 1;
    }
    message = -1;
#endif
}

void exit_Bind_Mode()
{
    reset_Bones_Rotation();

    set_Bone_len(); // IK theme

    update_IKchains(); // IK theme

    set_Bind_Pose_For_Transformers(1);

    normalize_rotation_parent_childs(&World);

    transfer_rotVec_B(&World);

    transfer_Bones_rotVec_();

    reset_Scale();

    update_Default_Pose();

    move_Deformers_To_Delta_Position();

    create_Hierarchys_List(); // because of collapsed locators

    paste_rotVec_(); // because of synthesize_Bone_Axis // axis alignement during animation

    Button_Mode[4].color = UI_GRAYB;
    Button_Mode[selection_Mode].color = UI_GRAYD;

    set_Object_Mode();
}

void save_load_Scene()
{
    if (BIND_POSE)
    {
        exit_Bind_Mode();
        BIND_POSE = 0;
    }

    create_Transformers_List();

    int flip = 0;

    if (VIEWS_FLIPPED)
    {
        flip_Cameras(Camera);
        flip = 1;
    }

    Button_h_scen[0].color = UI_GRAYD;
    if (dialog_type == SAVES_DIALOG)
    {
        printf("saving %s\n", scene_files_dir);
        printf("%s\n", scene_extension);
        int result = 0;
        if (!isDirectory(scene_files_dir))
        {
            printf("making directory\n");
            result = makepath(scene_files_dir, 0777);
        }
        else
        {
            result = 1;
        }
        printf("%d\n", result);
        if (result)
        {
            char Path[STRLEN];
            int e;

            for (e = 0; e < scene_ext_count; e ++)
            {
                Path[0] = '\0';

                strcat(Path, scene_files_dir);
                strcat(Path, "/");
                strcat(Path, scene_extensions[e]);
                if (!isDirectory(Path))
                {
                    mkdir(Path);
                    chmod(Path, 0777);
                }
                else
                {
                    remove_Ingredients(Path);
                }
            }

            Path[0] = '\0';
            strcat(Path, scene_files_dir);
            strcat(Path, "/");
            strcat(Path, "Objects");
            save_Objects(Path);

            Path[0] = '\0';
            strcat(Path, scene_files_dir);
            strcat(Path, "/");
            strcat(Path, "Selections");
            save_Selections(Path);

            Path[0] = '\0';
            strcat(Path, scene_files_dir);
            strcat(Path, "/");
            strcat(Path, "Deformers");
            save_Deformers(Path);

            Path[0] = '\0';
            strcat(Path, scene_files_dir);
            strcat(Path, "/");
            strcat(Path, "Hierarchys");
            save_Hierarchys(Path, CamDist);

            Path[0] = '\0';
            strcat(Path, scene_files_dir);
            strcat(Path, "/");
            strcat(Path, "Poses");
            save_Poses(Path);

            Path[0] = '\0';
            strcat(Path, scene_files_dir);
            strcat(Path, "/");
            strcat(Path, "Surfaces");
            save_Surfaces(Path);
        }

        if (flip)
        {
            flip_Cameras(Camera);
        }

        title[0] = '\0';
        strcat(title, caption);
        strcat(title, scene_files_dir);

        SDL_WM_SetCaption(title, NULL);

        if (dialog_lock)
            update_Saves_List(1, 0);
    }
    else if (dialog_type == LOADING_DIALOG)
    {
        printf("loading %s\n", scene_files_dir);
        printf("%s\n", scene_extension);

        if (dialog_lock)
            update_Loading_List(1, 0);

        int obj_count = 0, defr_count = 0, pose_count = 0;

        if (!isDirectory(scene_files_dir))
        {
            printf("Scene is not in the path");
        }
        else
        {
            char Path[STRLEN];
            int e;

            for (e = 0; e < scene_ext_count; e ++)
            {
                Path[0] = '\0';

                strcat(Path, scene_files_dir);
                strcat(Path, "/");
                strcat(Path, scene_extensions[e]);
                if (isDirectory(Path))
                {
                    printf("%s exists!\n", Path);
                }
            }

            Path[0] = '\0';
            strcat(Path, scene_files_dir);
            strcat(Path, "/");
            strcat(Path, "Objects");
            obj_count = load_Objects(Path, VBO);
            subdLevel = -1;

            Path[0] = '\0';
            strcat(Path, scene_files_dir);
            strcat(Path, "/");
            strcat(Path, "Surfaces");
            load_Surfaces(Path, obj_count);

            Path[0] = '\0';
            strcat(Path, scene_files_dir);
            strcat(Path, "/");
            strcat(Path, "Selections");
            load_Selections(Path);

            Path[0] = '\0';
            strcat(Path, scene_files_dir);
            strcat(Path, "/");
            strcat(Path, "Deformers");
            defr_count = load_Deformers(Path);

            Path[0] = '\0';
            strcat(Path, scene_files_dir);
            strcat(Path, "/");
            strcat(Path, "Hierarchys");
            load_Hierarchys(Path, obj_count, defr_count);

            Path[0] = '\0';
            strcat(Path, scene_files_dir);
            strcat(Path, "/");
            strcat(Path, "Poses");
            pose_count = load_Poses(Path);

            assign_Poses(pose_count, defr_count);

            //validate_Loading();
            //verify_textPack_indices();
            //verify_normPack_indices();

            update_transformed_Objects(obj_count);

            title[0] = '\0';
            strcat(title, caption);
            strcat(title, scene_files_dir);

            SDL_WM_SetCaption(title, NULL);

            float h_view = Camera_Persp.h_view;

            Camera_Top.h_view = h_view;
            Camera_Left.h_view = h_view;
            Camera_Front.h_view = h_view;

            update_camera_view(&Camera_Persp, h_view, screen_width, screen_height);
            update_camera_view(&Camera_Top, h_view, screen_width, screen_height);
            update_camera_view(&Camera_Left, h_view, screen_width, screen_height);
            update_camera_view(&Camera_Front, h_view, screen_width, screen_height);

            if (splitview)
            {
                rotate_Camera(&Camera_Persp, CamDist);
                rotate_Camera(&Camera_Top, CamDist);
                rotate_Camera(&Camera_Front, CamDist);
                rotate_Camera(&Camera_Left, CamDist);
            }
            else
            {
                rotate_Camera(Camera, CamDist);
            }

            update_camera(Camera, CamDist);

            set_Camera_Pose(&Camera_Persp, CamDist);

            find_Camera_Objects();

            if (objectIndex > 1)
            {
                currentObject = objectIndex - 1;
                O = objects[currentObject];
                O->selected = 1;
                assert_Object_Selection();
            }

            assert_Deformers_Selected();

            //poly_Render(tripsRender, wireframe, splitview, CamDist, 1, subdLevel);

            dialog_lock = 0;
        }
    }

    create_Hierarchys_List();

    Button_h_scen[0].color = UI_GRAYB;
}

void select_currentIK()
{
    printf("select currentIK %d\n", currentIK);

    if (currentIK >= 0 && currentIK < iksIndex)
    {

    }

    if (dialog_lock)
    {
        draw_Dialog();
    }
}

void select_currentBone()
{
    printf("select currentBone %d\n", currentBone);

    if (currentBone >= 0 && currentBone < bonesIndex)
    {
        int t;
        int condition = 0;

        for (t = 0; t < transformerIndex; t ++)
        {
            if (transformers[t] == bones[currentBone]->A)
            {
                condition = 1;
                break;
            }
        }
        if (condition)
        {
            T = bones[currentBone]->A;
            T->selected = 1;
            currentLocator = T->index;
        }
    }

    if (dialog_lock)
    {
        draw_Dialog();
    }
}

void select_Transformer_IK(transformer * T)
{
    if (T->IK != NULL)
    {
        currentIK = T->IK->index;
        IKIndex = currentIK;
        if (currentIK > LISTLENGTH / 2)
            ikch_start = ikChains[currentIK]->index - LISTLENGTH / 2;
        else
            ikch_start = 0;
    }
}

void select_Transformer_Bone(transformer * T)
{
    if (T->Bone != NULL)
    {
        currentBone = T->Bone->index;
        BoneIndex = currentBone;
        if (currentBone > LISTLENGTH / 2)
            bone_start = bones[currentBone]->index - LISTLENGTH / 2;
        else
            bone_start = 0;
    }
}

void freeze_Object_Coordinates(object * O)
{
    Draw_Bottom_Message("freeze Object Coordinates");

    if (O->binding && D != NULL)
    {
        remove_Object_From_Deformer(O, D);
    }

    if (!O->binding)
    {
        T = O->T;

        transfer_Transformed_Coordinates(O);

        memcpy(T->pos, (float[3]){0, 0, 0}, sizeof(T->pos));
        memcpy(T->rot, (float[3]){0, 0, 0}, sizeof(T->rot));
        memcpy(T->scl, (float[3]){1, 1, 1}, sizeof(T->scl));

        rotate(T);
        rotate_verts(O, *T);

        int l;

        for (l = 0; l <= O->subdlevel; l ++)
        {
            rotate_verts_(O, *T, l);
        }
    }
}

void transfer_LocatorSize(float size)
{
    int t;
    transformer * T0;

    for (t = 0; t < selected_transformer_count; t ++)
    {
        T0 = transformers[selected_transformers[t]];
        T0->LocatorSize = size;
    }

    T = transformers[currentLocator];
    T->LocatorSize = size;
}

void Exit()
{
    quit = 1;
    message = -1;

    DRAW_UI = 1;
    poly_Render(tripsRender, wireframe, splitview, CamDist, 1, subdLevel);
    SDL_Delay(100);
}

int main(int argc, char * args[])
{
    AllocConsole();

    HANDLE handle_out = GetStdHandle(STD_OUTPUT_HANDLE);
    int hCrt = _open_osfhandle((long) handle_out, _O_TEXT);
    FILE * hf_out = _fdopen(hCrt, "w");
    setvbuf(hf_out, NULL, _IONBF, 1);
    * stdout = * hf_out;

    HANDLE handle_in = GetStdHandle(STD_INPUT_HANDLE);
    hCrt = _open_osfhandle((long) handle_in, _O_TEXT);
    FILE * hf_in = _fdopen(hCrt, "r");
    setvbuf(hf_in, NULL, _IONBF, 128);
    * stdin = * hf_in;

    if (init() == 0)
    {
        return 1;
    }

    sprintf(bottom_message, "Welcome to Trips C Program");

    init_transformer(&World, NULL, "World");
    init_Cameras();
    init_camera(&Camera_Persp, "Perspective", screen_width, screen_height, CamDist, ortho_on, CAMERA_PERSPECTIVE);
    init_camera(&Camera_Top, "Top", screen_width, screen_height, CamDist, ortho_on, CAMERA_TOP);
    init_camera(&Camera_Front, "Front", screen_width, screen_height, CamDist, ortho_on, CAMERA_FRONT);
    init_camera(&Camera_Left, "Left", screen_width, screen_height, CamDist, ortho_on, CAMERA_LEFT);

    rotate_Camera(&Camera_Persp, CamDist); // this line is essential
//    // because it places Camera rotVec_ into initial pose.
//    // If it has parent other than World.
//    // With World it is identity matrix.
//    // From rotVec_ rotVec is loaded while rotating.
//    // rotVec is used to transform verts (and childs (TODO)).

    Camera_Top.T->rot[0] = pi_2;
    Camera_Left.T->rot[1] = -pi_2;
    rotate_Camera(&Camera_Top, CamDist);
    rotate_Camera(&Camera_Front, CamDist);
    rotate_Camera(&Camera_Left, CamDist);

    set_Camera_Pose(&Camera_Persp, CamDist);

    Camera = &Camera_Persp;

    memset(loaded_objects, 0, sizeof(loaded_objects));

    subdLevel = create_Objects();

    O = objects[currentObject];
    O->selected = 1;

    selected_objects[0] = currentObject;

    selected_object_count = 1;

    subdLevel = -1;

//    if (font != NULL)
//    Message = TTF_RenderText_Blended(font, "Cube", textColor);

//    O->T->rot_Order = zxy;

    if (subdLevel < 0)
    {
        move_verts(O, -0.0, -0.5, 3.0);
    }
    else
    {
        move_verts_(O, -0.0, -0.5, 3.0, subdLevel);
    }

//    int raytrace = 0;

    int i;
//    int rotAxis = 1;

    init_textures(screen);

    create_Normal_Map();
    create_Bump_Map();

    SideBar[0] = &collapse_Lists;
    SideBar[1] = &open_Items_List;
    SideBar[2] = &open_Materials_List;
    SideBar[3] = &open_Textures_List;
    SideBar[4] = &open_Selections_List;
    SideBar[5] = &open_Hierarchys_List;
    SideBar[6] = &open_Deformers_List;
    SideBar[7] = &open_Poses_List;
    SideBar[8] = &open_Bones_List;
    SideBar[9] = &open_IK_List;
    SideBar[10] = &collapse_Files;
    SideBar[11] = &open_OBJ_List;
    SideBar[12] = &open_Text_List;
    SideBar[13] = &open_Norm_List;
    SideBar[14] = &open_Bump_List;
    SideBar[15] = &open_Saves_List;
    SideBar[16] = &export_OBJ_Format;
    SideBar[17] = &open_Loading_List;
    SideBar[18] = &collapse_Clear;
    SideBar[19] = &clear_All;
    SideBar[20] = &Exit;

    Button_Mode[0].func = &set_Object_Mode;
    Button_Mode[1].func = &set_Polygon_Mode;
    Button_Mode[2].func = &set_Edge_Mode;
    Button_Mode[3].func = &set_Vertex_Mode;
    Button_Mode[4].func = &set_Bind_Mode;

    Button_ext[0].func = &set_Button_ext;
    Button_ext[1].func = &set_Button_ext;
    Button_ext[2].func = &set_Button_ext;
    Button_ext[3].func = &set_Button_ext;

    Button_scene_ext[0].func = &set_Button_scene_ext;
    Button_scene_ext[1].func = &set_Button_scene_ext;
    Button_scene_ext[2].func = &set_Button_scene_ext;
    Button_scene_ext[3].func = &set_Button_scene_ext;
    Button_scene_ext[4].func = &set_Button_scene_ext;
    Button_scene_ext[5].func = &set_Button_scene_ext;

    Button_item[0].func = &set_Button_item;
    Button_item[1].func = &set_Button_item;
    Button_item[2].func = &set_Button_item;

    Button_text[0].func = &set_Button_text;
    Button_text[1].func = &set_Button_text;
    Button_text[2].func = &set_Button_text;

    Button_sels[0].func = &set_Button_sels;
    Button_sels[1].func = &set_Button_sels;
    Button_sels[2].func = &set_Button_sels;
    Button_sels[3].func = &set_Button_sels;

    Button_defr[0].func = &set_Button_defr;
    Button_defr[1].func = &set_Button_defr;
    Button_defr[2].func = &set_Button_defr;

    Button_h_sels[0].func = &add_Selection;
    Button_h_sels[1].func = &edit_Selection;
    Button_h_sels[2].func = &remove_Selection;
    Button_h_sels[3].func = &assign_Selection_L;
    Button_h_sels[4].func = &unassign_Selection_L;
    Button_h_sels[5].func = &select_Selection;

    Button_h_hier[0].func = &edit_Hierarchy;
    Button_h_hier[1].func = &parent_Locators;
    Button_h_hier[2].func = &unparent_Locator;
    Button_h_hier[3].func = &new_Locator;
    Button_h_hier[4].func = &delete_Locator;

    Button_h_defr[0].func = &add_Branch;
    Button_h_defr[1].func = &remove_Branch;
    Button_h_defr[2].func = &bind_Selection;
    Button_h_defr[3].func = &unbind_Selection;
    Button_h_defr[4].func = &new_Deformer;
    Button_h_defr[5].func = &delete_Deformer;

    Button_h_pose[0].func = &add_Pose;
    Button_h_pose[1].func = &remove_Pose;
    Button_h_pose[2].func = &rename_Pose;
    Button_h_pose[3].func = &update_Pose;

    Button_h_bone[0].func = &remove_Bone;
    Button_h_bone[1].func = &rename_Bone;

    Button_h_ikch[0].func = &remove_IK;
    Button_h_ikch[1].func = &rename_IK;

    Button_h_scen[0].func = &save_load_Scene;

    Button_h_text[0].func = &reload_Texture;
    Button_h_text[1].func = &reload_All_Textures;

	for (i = 0; i < EXT_NUM; i ++)
    {
        extensions[i] = malloc(5 * sizeof(char));
    }

	for (i = 0; i < TEXT_NUM; i ++)
    {
        texts[i] = malloc(TYPE_LENGTH * sizeof(char));
    }

    texts[0] = TEXT_TYPE_TEXT;
    texts[1] = TEXT_TYPE_NORM;
    texts[2] = TEXT_TYPE_BUMP;

    memcpy(&text, texts[0], strlen(texts[0]));

	for (i = 0; i < ITEM_NUM; i ++)
    {
        items[i] = malloc(TYPE_LENGTH * sizeof(char));
    }

    items[0] = ITEM_TYPE_OBJECT;
    items[1] = ITEM_TYPE_CAMERA;
    items[2] = ITEM_TYPE_LIGHT;

    memcpy(&item, items[0], strlen(items[0]));

    init_selections();
    init_hierarcys();
    init_deformers();
    init_poses();

//    dialog_type = LOADING_DIALOG;
//    save_load_Scene();

    while(quit == 0)
    {
        if (SDL_PollEvent(&event))
        {
            if (event.type == SDL_KEYUP)
            {
                switch(event.key.keysym.sym)
                {
                    case SDLK_LCTRL: controlDown = 0; handle_ControlDown();
                    case SDLK_LALT: altDown = 0;
                    default: break;
                }
            }
            else if (event.type == SDL_KEYDOWN)
            {
                mod = event.key.keysym.mod;

                switch(event.key.keysym.sym)
                {
                    case SDLK_LCTRL: controlDown = 1; handle_ControlDown(); printf("l Ctrl\n"); break;
                    case SDLK_LALT: altDown = 1; break;
                    case SDLK_UP: message = 1; printf("up\n"); break;
                    case SDLK_DOWN: message = 2; printf("down\n"); break;
                    case SDLK_LEFT: message = 3; printf("left\n"); break;
                    case SDLK_RIGHT: message = 4; printf("right\n"); break;
                    case SDLK_INSERT: message = 5; printf("insert\n"); break;
                    case SDLK_DELETE: message = 6; printf("delete\n"); break;
                    case SDLK_HOME: message = 7; printf("home\n"); break;
                    case SDLK_END: message = 8; printf("end\n"); break;
                    case SDLK_PAGEUP: message = 9; printf("page up\n"); break;
                    case SDLK_PAGEDOWN: message = 10; printf("page down\n"); break;

                    case SDLK_KP_PLUS: message = 17; printf("plus\n"); break;
                    case SDLK_KP_MINUS: message = 18; printf("minus\n"); break;
                    case SDLK_F1: message = 19; printf("F1\n"); break;
                    case SDLK_F2: message = 20; printf("F2\n"); break;
                    case SDLK_F3: message = 21; printf("F3\n"); break;
                    case SDLK_F4: message = 22; printf("F4\n"); break;
                    case SDLK_F5: message = 76; printf("F5\n"); break;
                    case SDLK_F6: message = 77; printf("F5\n"); break;

                    case SDLK_TAB: message = 25; printf("TAB\n"); break;

                    case SDLK_BACKSPACE: message = 27; if (dialog_lock) handle_dialog(8, mod); break;

                    case SDLK_a: message = 30; if (dialog_lock) handle_dialog('a', mod); break;
                    case SDLK_b: message = 36; if (dialog_lock) handle_dialog('b', mod); break;
                    case SDLK_c: message = 37; if (dialog_lock) handle_dialog('c', mod); break;
                    case SDLK_d: message = 26; if (dialog_lock) handle_dialog('d', mod); break;
                    case SDLK_e: message = 24; if (dialog_lock) handle_dialog('e', mod); break;
                    case SDLK_f: message = 32; if (dialog_lock) handle_dialog('f', mod); break;
                    case SDLK_g: message = 31; if (dialog_lock) handle_dialog('g', mod); break;
                    case SDLK_h: message = 38; if (dialog_lock) handle_dialog('h', mod); break;
                    case SDLK_i: message = 29; if (dialog_lock) handle_dialog('i', mod); break;
                    case SDLK_j: message = 39; if (dialog_lock) handle_dialog('j', mod); break;
                    case SDLK_k: message = 40; if (dialog_lock) handle_dialog('k', mod); break;
                    case SDLK_l: message = 41; if (dialog_lock) handle_dialog('l', mod); break;
                    case SDLK_m: message = 11; if (dialog_lock) handle_dialog('m', mod); break;
                    case SDLK_n: message = 42; if (dialog_lock) handle_dialog('n', mod); break;
                    case SDLK_o: message = 23; if (dialog_lock) handle_dialog('o', mod); break;
                    case SDLK_p: message = 15; if (dialog_lock) handle_dialog('p', mod); break;
                    case SDLK_q: message = 13; if (dialog_lock) handle_dialog('q', mod); break;
                    case SDLK_r: message = 14; if (dialog_lock) handle_dialog('r', mod); break;
                    case SDLK_s: message = 34; if (dialog_lock) handle_dialog('s', mod); break;
                    case SDLK_t: message = 12; if (dialog_lock) handle_dialog('t', mod); break;
                    case SDLK_u: message = 28; if (dialog_lock) handle_dialog('u', mod); break;
                    case SDLK_v: message = 33; if (dialog_lock) handle_dialog('v', mod); break;
                    case SDLK_w: message = 16; if (dialog_lock) handle_dialog('w', mod); break;
                    case SDLK_x: message = 44; if (dialog_lock) handle_dialog('x', mod); break;
                    case SDLK_y: message = 45; if (dialog_lock) handle_dialog('y', mod); break;
                    case SDLK_z: message = 46; if (dialog_lock) handle_dialog('z', mod); break;
                    case SDLK_ESCAPE: message = 35; printf("esc\n"); break;

                    case SDLK_1: message = 47; if (dialog_lock) handle_dialog('1', mod); break;
                    case SDLK_2: message = 48; if (dialog_lock) handle_dialog('2', mod); break;
                    case SDLK_3: message = 49; if (dialog_lock) handle_dialog('3', mod); break;
                    case SDLK_4: message = 50; if (dialog_lock) handle_dialog('4', mod); break;
                    case SDLK_5: message = 51; if (dialog_lock) handle_dialog('5', mod); break;
                    case SDLK_6: message = 52; if (dialog_lock) handle_dialog('6', mod); break;
                    case SDLK_7: message = 53; if (dialog_lock) handle_dialog('7', mod); break;
                    case SDLK_8: message = 54; if (dialog_lock) handle_dialog('8', mod); break;
                    case SDLK_9: message = 55; if (dialog_lock) handle_dialog('9', mod); break;
                    case SDLK_0: message = 56; if (dialog_lock) handle_dialog('0', mod); break;
                    case SDLK_SPACE: message = 57; if (dialog_lock) handle_dialog(' ', mod); break;
                    case SDLK_MINUS: message = 58; if (dialog_lock) handle_dialog('-', mod); break;
                    case SDLK_KP_ENTER: message = 59; if (dialog_lock) handle_dialog(10, mod); break;
                    case SDLK_RETURN: message = 60; if (dialog_lock) handle_dialog(13, mod); break;
                    case SDLK_KP1: message = 61; if (dialog_lock) handle_dialog('1', mod); break;
                    case SDLK_KP2: message = 62; if (dialog_lock) handle_dialog('2', mod); break;
                    case SDLK_KP3: message = 63; if (dialog_lock) handle_dialog('3', mod); break;
                    case SDLK_KP4: message = 64; if (dialog_lock) handle_dialog('4', mod); break;
                    case SDLK_KP5: message = 65; if (dialog_lock) handle_dialog('5', mod); break;
                    case SDLK_KP6: message = 66; if (dialog_lock) handle_dialog('6', mod); break;
                    case SDLK_KP7: message = 67; if (dialog_lock) handle_dialog('7', mod); break;
                    case SDLK_KP8: message = 68; if (dialog_lock) handle_dialog('8', mod); break;
                    case SDLK_KP9: message = 69; if (dialog_lock) handle_dialog('9', mod); break;
                    case SDLK_KP0: message = 70; if (dialog_lock) handle_dialog('0', mod); break;
                    case SDLK_BACKQUOTE: message = 71; if (dialog_lock) handle_dialog('`', mod); break;
                    case SDLK_SEMICOLON: message = 72; if (dialog_lock) handle_dialog(':', mod); break;
                    case SDLK_SLASH: message = 74; if (dialog_lock) handle_dialog('/', mod); break;
                    case SDLK_BACKSLASH: message = 75; if (dialog_lock) handle_dialog('/', mod); break;
                    default: break;
                }
                if (message != 1 && message != 2 && message != 25 && message != 35)
                {
                    if (dialog_lock)
                    {
                        message = -1;
                    }
                    else if ((MOVEMENT || ROTATION || SCALE) && message != 47 && message != 48 && message != 49)
                    {
                        message = -1;
                    }
                }
            }
            else if (event.type == SDL_VIDEORESIZE)
            {
                update_Resize_Event();
            }
            else if (event.type == SDL_VIDEOEXPOSE)
            {
                if (!Camera_screen_lock)
                {
                    glDisable(GL_SCISSOR_TEST);
                    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                    glEnable(GL_SCISSOR_TEST);
                    if (dialog_lock)
                    {
                        Bottom_Message = 0;
                        draw_Dialog();
                    }
                    else
                    {
                        message = -1;
                    }
                }
            }
            else if (event.type == SDL_VIDEO_OPENGL)
            {
                if (!Camera_screen_lock)
                {
                    glDisable(GL_SCISSOR_TEST);
                    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                    glEnable(GL_SCISSOR_TEST);
                    if (dialog_lock)
                    {
                        Bottom_Message = 0;
                        draw_Dialog();
                    }
                    else
                    {
                        message = -1;
                    }
                }
            }
            else if (event.type == SDL_MOUSEBUTTONDOWN) // scroll up
            {
                if (!dialog_lock)
                {
                    poly_Render(tripsRender, wireframe, splitview, CamDist, 1, subdLevel);
                }

                SDL_GetMouseState(&mouse_x, &mouse_y);

                if (!Camera_screen_lock)
                    Camera = find_View(mouse_x, mouse_y, splitview);

                if (Camera->sidebar && !Camera_screen_lock && !Edit_Lock && !Drag_Dialog)// && !dialog_lock)
                {
                    int index = mouse_y / BUTTON_HEIGHT;
                    printf("mouse button %d\n", index);
                    if (index < Sidebar_Marker && index >= 0)
                    {
                        Buttonindex_sidebar = index;
                        Button_sidebar[Buttonindex_sidebar].color = UI_GRAYD;
                        if (Button_sidebar[Buttonindex_sidebar].func != NULL)
                        {
                            (*Button_sidebar[Buttonindex_sidebar].func)();
                        }
                        Button_sidebar[Buttonindex_sidebar].color = UI_GRAYB;
                        message = -1;
                    }
                }
                else if (Camera->bottom_line && !Camera_screen_lock && !dialog_lock && !Drag_Dialog)
                {
                    int index = (mouse_x / BUTTON_WIDTH) - 1;
                    printf("mouse button mode %d\n", index);
                    if (index < BUTTONS_MODE && index >= 0)
                    {
                        Buttonindex_bottom = index;
                        if (Button_Mode[Buttonindex_bottom].func != NULL)
                        {
                            (*Button_Mode[Buttonindex_bottom].func)();
                        }
                        message = -1;
                    }
                }
                else if (event.button.button == SDL_BUTTON_WHEELUP)
                {
                    if (dialog_lock)
                    {
                        handle_UP(1);
                    }
                    else if(altDown)
                    {
                        Reflect += 0.1;
                        printf("%f\n", Reflect);
                    }
                    else if(controlDown)
                    {
                        if (fog.Distance > 100 && fog.Distance < 1000)
                        {
                            fog.Distance += 10;
                        }
                        else if (fog.Distance > 10 && fog.Distance <= 100)
                        {
                            fog.Distance += 2;
                        }
                        else
                        {
                            fog.Distance += 0.5;
                        }
                        if (fog.Distance > 1000)
                        {
                            fog.Distance = 1000;
                        }
                        printf("%f\n", fog.Distance);
                        update_fog();
                    }
                    else if (BONES_MODE)
                    {
                        bones[currentBone]->A->rot_Order ++;
                        bones[currentBone]->A->rot_Order %= 5;
                        currentOrder = bones[currentBone]->A->rot_Order;
                        bones[currentBone]->B->rot_Order = currentOrder;
                        synthesize_Bone_Alignement(bones[currentBone]);
                    }
                    else if (DRAW_LOCATORS)
                    {
                        if (LocatorSize < 0.1)
                        {
                            LocatorSize += 0.01;
                        }
                        else if (LocatorSize < 1.0)
                        {
                            LocatorSize += 0.1;
                        }
                        else if (LocatorSize < 10)
                        {
                            LocatorSize += 1;
                        }

                        if (LocatorSize > 10)
                            LocatorSize = 10;

                        transfer_LocatorSize(LocatorSize);
                    }
                    else
                    {
                        //v_scale += 0.05;
                        //printf("wheel up\n");
                        float h_view = Camera->h_view - 0.1;
                        if (h_view < 0.3) h_view = 0.3;
                        Camera_Persp.h_view = h_view;
                        Camera_Top.h_view = h_view;
                        Camera_Left.h_view = h_view;
                        Camera_Front.h_view = h_view;
                        printf("view %f\n", h_view);
                        Camera->origin_2d[0] = screen_width / 2.0 + (Camera->origin_2d[0] - screen_width / 2.0) * 0.3;
                        Camera->origin_2d[1] = screen_height / 2.0 + (Camera->origin_2d[1] - screen_height / 2.0) * 0.3;
                        update_camera_view(&Camera_Persp, h_view, screen_width, screen_height);
                        update_camera_view(&Camera_Top, h_view, screen_width, screen_height);
                        update_camera_view(&Camera_Left, h_view, screen_width, screen_height);
                        update_camera_view(&Camera_Front, h_view, screen_width, screen_height);
                        message = -1;
                    }
                }
                else if (event.button.button == SDL_BUTTON_WHEELDOWN)
                {
                    if (dialog_lock)
                    {
                        handle_DOWN(1);
                    }
                    else if(altDown)
                    {
                        Reflect -= 0.1;
                        printf("%f\n", Reflect);
                    }
                    else if(controlDown)
                    {
                        if (fog.Distance < 1000 && fog.Distance >= 100)
                        {
                            fog.Distance -= 10;
                        }
                        else if (fog.Distance < 100 && fog.Distance >= 10)
                        {
                            fog.Distance -= 2;
                        }
                        else
                        {
                            fog.Distance -= 0.5;
                        }
                        printf("%f\n", fog.Distance);
                        update_fog();
//                        if (fog.Distance < 0)
//                            fog.Distance = 0;
                    }
                    else if (BONES_MODE)
                    {
                        bones[currentBone]->A->rot_Order --;
                        bones[currentBone]->A->rot_Order = abs(bones[currentBone]->A->rot_Order);
                        bones[currentBone]->A->rot_Order %= 5;
                        currentOrder = bones[currentBone]->A->rot_Order;
                        bones[currentBone]->B->rot_Order = currentOrder;
                        synthesize_Bone_Alignement(bones[currentBone]);
                    }
                    else if (DRAW_LOCATORS)
                    {
                        if (LocatorSize <= 0.1)
                        {
                            LocatorSize -= 0.01;
                        }
                        else if (LocatorSize <= 1.0)
                        {
                            LocatorSize -= 0.1;
                        }
                        else if (LocatorSize <= 10)
                        {
                            LocatorSize -= 1.0;
                        }
                        if (LocatorSize < 0.01)
                            LocatorSize = 0.01;

                        transfer_LocatorSize(LocatorSize);
                    }
                    else
                    {
                        //printf("wheel down\n");
                        float h_view = Camera->h_view + 0.1;
                        if (h_view >= pi2) h_view = pi2 - 0.001;
                        Camera_Persp.h_view = h_view;
                        Camera_Top.h_view = h_view;
                        Camera_Left.h_view = h_view;
                        Camera_Front.h_view = h_view;
                        printf("view %f\n", h_view);
                        Camera->origin_2d[0] = screen_width / 2.0 + (Camera->origin_2d[0] - screen_width / 2.0) * 1.03;
                        Camera->origin_2d[1] = screen_height / 2.0 + (Camera->origin_2d[1] - screen_height / 2.0) * 1.03;
                        update_camera_view(&Camera_Persp, h_view, screen_width, screen_height);
                        update_camera_view(&Camera_Top, h_view, screen_width, screen_height);
                        update_camera_view(&Camera_Left, h_view, screen_width, screen_height);
                        update_camera_view(&Camera_Front, h_view, screen_width, screen_height);
                        message = -1;
                    }
                }
                else if (event.button.button == SDL_BUTTON_RIGHT)
                {
                    if (edgeWeights)
                    {
                        snap_back_Edges_Weights();
                        snap_back_Verts_Weights();
                        edgeWeights = 0;
                    }
                    else if (Camera_screen_lock)
                    {
                        if (camera_rotate)
                        {
                            T = Camera->T;
                        }
                        else if (object_hook)
                        {
                            if (BIND_POSE)
                            {
                                T = transformers[currentLocator];
                                if (MOVEMENT)
                                {
                                    bake_pose(T);
                                    bake_pose_Children(T);

                                    if (BONES_MODE || T->Bone != NULL)
                                    {
                                        synthesize_Bone_Alignement(T->Bone);
                                        if (T->Bone->B == T)
                                        {
                                            synthesize_Bone_Alignement_childs(T);
                                        }
                                    }
                                }
                            }
                            else if (DRAW_LOCATORS)
                            {
                                T = transformers[currentLocator];
                                bake_pose_Children(T);
                            }
                            else
                            {
                                T = objects[currentObject]->T;
                                bake_pose_Children(T);
                            }
                        }
                        else
                        {
                            T = Camera->T;
                        }

                        T->rot[0] = object_Rot[0];
                        T->rot[1] = object_Rot[1];
                        T->rot[2] = object_Rot[2];
                        T->scl[0] = object_Scl[0];
                        T->scl[1] = object_Scl[1];
                        T->scl[2] = object_Scl[2];
                        T->target[0] = target_Pos[0];
                        T->target[1] = target_Pos[1];
                        T->target[2] = target_Pos[2];
                        T->pos[0] = object_Pos[0];
                        T->pos[1] = object_Pos[1];
                        T->pos[2] = object_Pos[2];

                        if (BIND_POSE)
                        {
                            rotate_bind(T);
                        }
                        else
                        {
                            Update_Objects_Count = 0;
                            if (T->IK != NULL && (T->style == ik_goal || T->style == ik_fixed))
                            {
                                solve_IK_Chain(T->IK, 0);
                            }
                            if (T->Deformer != NULL)
                            {
                                if (T->Deformer->Transformers_Count > 0)
                                {
                                    rotate_collect(T->Deformer->Transformers[0]);
                                }
                                else
                                {
                                    rotate_collect(T);
                                }
                                rotate_Deformer(T);
                            }
                            else
                            {
                                rotate_collect(T);
                                rotate_vertex_groups_D_Init();
                                rotate(T);
                            }
                            update_rotate_bounding_box();
                        }

                        if (camera_rotate || camera_z_move)
                        {
                            if (camera_z_move)
                            {
                                CamDist = CamDist_mem;
                            }
                            rotate_Camera(&Camera_Persp, CamDist);
                            rotate_Camera(&Camera_Top, CamDist);
                            rotate_Camera(&Camera_Front, CamDist);
                            rotate_Camera(&Camera_Left, CamDist);
                        }

                        subdLevel = subdLevel_mem;
                        camera_rotate = 0;
                        camera_z_move = 0;
                        object_hook = 0;
                        ROTATION = 0;
                        MOVEMENT = 0;
                        BONES_MODE = 0;
                        SCALE = 0;
                        Axis_lock = 0;
                        altDown = 0;
                        Camera_screen_lock = 0;
                        message = -1;
                    }
                    else if (mouse_x > SIDEBAR && mouse_y < screen_height && !Drag_Dialog)
                    {
                        DRAW_UI = 0;

                        //if (!NVIDIA) glDrawBuffer(GL_FRONT_AND_BACK);
                        //poly_Render(tripsRender, wireframe, splitview, CamDist, 0, subdLevel);
                        //if (dialog_lock)
                            //draw_Dialog();
                        //SDL_GL_SwapBuffers();

                        drag_rectangle = 1;
                        Drag_Rectangle.x0 = mouse_x;
                        Drag_Rectangle.y0 = mouse_y;
                        Drag_Rectangle.x1 = mouse_x;
                        Drag_Rectangle.y1 = mouse_y;
                        Drag_Rectangle.x = mouse_x;
                        Drag_Rectangle.y = screen_height + BOTTOM_LINE - Drag_Rectangle.y0;
                        Drag_Rectangle.w = 0;
                        Drag_Rectangle.h = 0;
                        message = -10;
                    }
                }
                else if (event.button.button == SDL_BUTTON_LEFT)
                {
//                    if (!NVIDIA) glDrawBuffer(GL_FRONT_AND_BACK);
//                    poly_Render(tripsRender, wireframe, splitview, CamDist, 0, subdLevel);
//                    if (dialog_lock)
//                        draw_Dialog();
//                    SDL_GL_SwapBuffers();

                    if (edgeWeights)
                    {
                        finish_adjusting_Edges_Weights();
                        finish_adjusting_Verts_Weights();
                        edgeWeights = 0;
                    }

                    memcpy(object_Pos, O->T->pos, sizeof(object_Pos));
                    memcpy(target_Pos, O->T->target, sizeof(target_Pos));
                    if (Drag_Dialog)
                    {
                        Drag_Dialog = 0;
                    }
                    else if (dialog_lock)
                    {
                        int index = mouse_y / BUTTON_HEIGHT;
                        if (mouse_x > SIDEBAR * 2 && mouse_x < SIDEBAR + DIALOG_WIDTH && index < LISTLENGTH && mouse_y < DIALOG_HEIGHT)
                        {
                            if (dialog_type == SAVES_DIALOG)
                            {
                                if (dir_lists && index + scene_files_start < files_num)
                                {
                                    printf("%s\n", Out_List[index + scene_files_start]);
                                    if (Edit_Lock)
                                    {
                                        char Path[STRLEN];
                                        Path[0] = '\0';
                                        strcat(Path, scene_files_dir);
                                        if (Out_List[index + scene_files_start][0] != '.')
                                        {
                                            if (Path[strlen(Path) - 1] != '/')
                                                sprintf(Path, "%s/%s", scene_files_dir, Out_List[index + scene_files_start]);
                                            else
                                                sprintf(Path, "%s%s", scene_files_dir, Out_List[index + scene_files_start]);
                                            if (isDirectory(Path))
                                            {
                                                scene_files_start = 0;
                                                sprintf(scene_files_dir, "%s", Path);
                                                sprintf(EditString, "%s", Path);
                                                EditCursor = strlen(scene_files_dir);
                                                update_Saves_List(1, 0);
                                            }
                                        }
                                        else
                                        {
                                            EditCursor --;
                                            while(EditString[EditCursor] != '/' && EditCursor > 1)
                                            {
                                                EditCursor --;
                                            }
                                            if (EditString[EditCursor - 1] == ':')
                                                EditCursor ++;
                                            EditString[EditCursor] = '\0';
                                            sprintf(scene_files_dir, "%s", EditString);
                                            update_Saves_List(1, 0);
                                        }
                                    }
                                }
                            }
                            else if (dialog_type == LOADING_DIALOG)
                            {
                                if (dir_lists && index + scene_files_start < files_num)
                                {
                                    printf("%s\n", Out_List[index + scene_files_start]);
                                    if (Edit_Lock)
                                    {
                                        char Path[STRLEN];
                                        Path[0] = '\0';
                                        strcat(Path, scene_files_dir);
                                        if (Out_List[index + scene_files_start][0] != '.')
                                        {
                                            if (Path[strlen(Path) - 1] != '/')
                                                sprintf(Path, "%s/%s", scene_files_dir, Out_List[index + scene_files_start]);
                                            else
                                                sprintf(Path, "%s%s", scene_files_dir, Out_List[index + scene_files_start]);
                                            if (isDirectory(Path))
                                            {
                                                scene_files_start = 0;
                                                sprintf(scene_files_dir, "%s", Path);
                                                sprintf(EditString, "%s", Path);
                                                EditCursor = strlen(scene_files_dir);
                                                update_Loading_List(1, 0);
                                            }
                                        }
                                        else
                                        {
                                            EditCursor --;
                                            while(EditString[EditCursor] != '/' && EditCursor > 1)
                                            {
                                                EditCursor --;
                                            }
                                            if (EditString[EditCursor - 1] == ':')
                                                EditCursor ++;
                                            EditString[EditCursor] = '\0';
                                            sprintf(scene_files_dir, "%s", EditString);
                                            update_Loading_List(1, 0);
                                        }
                                    }
                                }
                            }
                            else if (dialog_type == BONE_DIALOG)
                            {
                                if (index + bone_start < Bones_c)
                                {
                                    if (BoneIndex - bone_start >= 0)
                                        BoneList[BoneIndex - bone_start].color = UI_BLACK;
                                    BoneIndex = index + bone_start;
                                    if (BoneIndex - bone_start >= 0)
                                        BoneList[BoneIndex - bone_start].color = UI_BACKL;

                                    currentBone = Bone_List[BoneIndex];

                                    select_currentBone();

                                    create_Bones_List(BoneIndex);

                                    DRAW_UI = 0;
                                    if (!NVIDIA) glDrawBuffer(GL_FRONT_AND_BACK);
                                    poly_Render(tripsRender, wireframe, splitview, CamDist, 0, subdLevel);
                                    draw_Bones_Dialog("Bones List", screen_height,
                                                bone_start, 1, BoneIndex - bone_start);
                                    SDL_GL_SwapBuffers();
                                    glDrawBuffer(GL_BACK);
                                    DRAW_UI = 1;
                                }
                            }
                            else if (dialog_type == IK_DIALOG)
                            {
                                if (index + ikch_start < ikChains_c)
                                {
                                    if (IKIndex - ikch_start >= 0)
                                        IkchList[IKIndex - ikch_start].color = UI_BLACK;
                                    IKIndex = index + ikch_start;
                                    if (IKIndex - ikch_start >= 0)
                                        IkchList[IKIndex - ikch_start].color = UI_BACKL;

                                    currentIK = IK_List[IKIndex];

                                    select_currentIK();

                                    create_Ik_List(IKIndex);

                                    DRAW_UI = 0;
                                    if (!NVIDIA) glDrawBuffer(GL_FRONT_AND_BACK);
                                    poly_Render(tripsRender, wireframe, splitview, CamDist, 0, subdLevel);
                                    draw_IK_Dialog("IK List", screen_height,
                                                ikch_start, 1, IKIndex - ikch_start);
                                    SDL_GL_SwapBuffers();
                                    glDrawBuffer(GL_BACK);
                                    DRAW_UI = 1;
                                }
                            }
                            else if (dialog_type == POSE_DIALOG)
                            {
                                if (index + pose_start < Poses_c)
                                {
                                    int X_Expand = Pose_X_Offset[index + pose_start] * 5;
                                    if (mouse_x > SIDEBAR * 2 + X_Expand - 10 && mouse_x < SIDEBAR * 2 + X_Expand + 10)
                                    {
                                        if (X_Expand == 5)
                                        {
                                            deformers[-(Pose_List[index + pose_start] + 1)]->collapsed =
                                            !deformers[-(Pose_List[index + pose_start] + 1)]->collapsed;
                                        }
                                        create_Poses_List(PoseIndex);
                                    }
                                    else
                                    {
                                        if (PoseIndex - pose_start >= 0)
                                            PoseList[PoseIndex - pose_start].color = UI_BLACK;
                                        PoseIndex = index + pose_start;
                                        if (PoseIndex - pose_start >= 0)
                                            PoseList[PoseIndex - pose_start].color = UI_BACKL;
                                        if (Pose_List[PoseIndex] >= 0)
                                        {
                                            currentPose = Pose_List[PoseIndex];
                                        }
                                        else
                                        {
                                            currentDeformer_Node = -(Pose_List[index + pose_start] + 1);
                                            assert_Deformers_Selected();
                                            select_Deformer_Objects();
                                        }
                                        create_Poses_List(PoseIndex);
                                    }
                                    DRAW_UI = 0;
                                    if (!NVIDIA) glDrawBuffer(GL_FRONT_AND_BACK);
                                    poly_Render(tripsRender, wireframe, splitview, CamDist, 0, subdLevel);
                                    draw_Poses_Dialog("Poses List", screen_height,
                                                pose_start, 1, PoseIndex - pose_start);
                                    SDL_GL_SwapBuffers();
                                    glDrawBuffer(GL_BACK);
                                    DRAW_UI = 1;
                                }
                            }
                            else if (dialog_type == DEFR_DIALOG)
                            {
                                if (current_defr_type == 0) // defr
                                {
                                    if (index + defr_start < Deformers_c)
                                    {
                                        int X_Expand = Defr_X_Offset[index + defr_start] * 5;
                                        if (mouse_x > SIDEBAR * 2 + X_Expand - 10 && mouse_x < SIDEBAR * 2 + X_Expand + 10)
                                        {
                                            if (X_Expand == 5)
                                            {
                                                deformers[-(Deformer_List[index + defr_start] + 1)]->collapsed =
                                                !deformers[-(Deformer_List[index + defr_start] + 1)]->collapsed;
                                            }
                                            create_Deformers_List(SelsIndex[3], O);
                                        }
                                        else
                                        {
                                            if (DefrIndex - defr_start >= 0)
                                                DefrList[DefrIndex - defr_start].color = UI_BLACK;
                                            DefrIndex = index + defr_start;
                                            if (DefrIndex - defr_start >= 0)
                                                DefrList[DefrIndex - defr_start].color = UI_BACKL;
                                            currentDeformer = DefrIndex;
                                            if (Deformer_List[currentDeformer] >= 0)
                                            {
                                                currentLocator = Deformer_List[currentDeformer];
                                                HierIndex = currentLocator;
                                                select_Locator_Selections(currentLocator);
                                                frame_object(Camera, 1);
                                            }
                                            else
                                            {
                                                currentDeformer_Node = -(Deformer_List[index + defr_start] + 1);
                                                assert_Deformers_Selected();
                                                select_Deformer_Objects();
                                            }
                                            create_Deformers_List(SelsIndex[3], O);
                                        }
                                        DRAW_UI = 0;
                                        UPDATE_COLORS = 1;
                                        if (!NVIDIA) glDrawBuffer(GL_FRONT_AND_BACK);
                                        poly_Render(tripsRender, wireframe, splitview, CamDist, 0, subdLevel);
                                        draw_Deformers_Dialog("Deformers L.", screen_height, defr_type, defr_types, defr_type_count,
                                                            defr_start, 1, DefrIndex - defr_start,
                                                            hier_start, HierIndex - hier_start,
                                                            sels_start[current_sel_type], SelsIndex[current_sel_type] - sels_start[current_sel_type]);
                                        SDL_GL_SwapBuffers();
                                        glDrawBuffer(GL_BACK);
                                        UPDATE_COLORS = 0;
                                        DRAW_UI = 1;
                                    }
                                }
                                else if (current_defr_type == 1) // hier
                                {
                                    if (index + hier_start < Hierarchys_c)
                                    {
                                        if (Edit_Lock)
                                        {
                                            Edit_Lock = 0;
                                            if (strlen(EditString) > 0)
                                            {
                                                sprintf(Hier_Names[HierIndex], "%s", EditString);
                                                sprintf(Name_Remember, "%s", EditString);
                                            }
                                            EditCursor = 0;
                                            EditString[EditCursor] = '\0';
                                            printf("Edit finishing!\n");
                                            set_Hier_H_Button(-1);
                                        }
                                        else
                                        {
                                            int X_Expand = Hier_X_Offset[index + hier_start] * 5;
                                            if (mouse_x > SIDEBAR * 2 + X_Expand - 10 && mouse_x < SIDEBAR * 2 + X_Expand + 10)
                                            {
                                                transformers[index + hier_start]->collapsed = !transformers[index + hier_start]->collapsed;
                                                create_Hierarchys_List();
                                            }
                                            else
                                            {
                                                if (HierIndex - hier_start >= 0)
                                                    HierList[HierIndex - hier_start].color = UI_BLACK;
                                                HierIndex = index + hier_start;
                                                if (HierIndex - hier_start >= 0)
                                                    HierList[HierIndex - hier_start].color = UI_BACKL;
                                                currentLocator = HierIndex;
                                            }

                                            select_Transformer();
                                        }
                                        UPDATE_BACKGROUND = 0;
                                        update_Hierarcys_List(0, 1);
                                    }
                                }
                                else if (current_defr_type == 2) // sels
                                {
                                    if (index + sels_start[current_sel_type] < Selections_c[current_sel_type])
                                    {
                                        if (Edit_Lock)
                                        {
                                            Edit_Lock = 0;
                                            if (strlen(EditString) > 0)
                                            {
                                                sprintf(Sels_Names[current_sel_type][SelsIndex[current_sel_type]], "%s", EditString);
                                                sprintf(Name_Remember, "%s", EditString);
                                            }
                                            EditCursor = 0;
                                            EditString[EditCursor] = '\0';
                                            printf("Edit finishing!\n");
                                            set_Sels_H_Button(-1);
                                        }
                                        else
                                        {
                                            if (SelsIndex[current_sel_type] - sels_start[current_sel_type] >= 0)
                                                SelsList[SelsIndex[current_sel_type] - sels_start[current_sel_type]].color = UI_BLACK;
                                            SelsIndex[current_sel_type] = index + sels_start[current_sel_type];
                                            if (SelsIndex[current_sel_type] - sels_start[current_sel_type] >= 0)
                                                SelsList[SelsIndex[current_sel_type] - sels_start[current_sel_type]].color = UI_BACKL;

                                            select_Selection();
                                        }
                                        UPDATE_BACKGROUND = 0;
                                        update_Selections_List(0, 1);
                                    }
                                }
                            }
                            else if (dialog_type == HIER_DIALOG)
                            {
                                if (index + hier_start < Hierarchys_c)
                                {
                                    if (Edit_Lock)
                                    {
                                        Edit_Lock = 0;
                                        if (strlen(EditString) > 0)
                                        {
                                            sprintf(Hier_Names[HierIndex], "%s", EditString);
                                            sprintf(Name_Remember, "%s", EditString);
                                        }
                                        EditCursor = 0;
                                        EditString[EditCursor] = '\0';
                                        printf("Edit finishing!\n");
                                        set_Hier_H_Button(-1);
                                    }
                                    else
                                    {
                                        int X_Expand = Hier_X_Offset[index + hier_start] * 5;
                                        if (mouse_x > SIDEBAR * 2 + X_Expand - 10 && mouse_x < SIDEBAR * 2 + X_Expand + 10)
                                        {
                                            transformers[index + hier_start]->collapsed = !transformers[index + hier_start]->collapsed;
                                            create_Hierarchys_List();
                                        }
                                        else
                                        {
                                            if (HierIndex - hier_start >= 0)
                                                HierList[HierIndex - hier_start].color = UI_BLACK;
                                            HierIndex = index + hier_start;
                                            if (HierIndex - hier_start >= 0)
                                                HierList[HierIndex - hier_start].color = UI_BACKL;
                                            currentLocator = HierIndex;
                                        }

                                        select_Transformer();
                                    }
                                    UPDATE_BACKGROUND = 1;
                                    update_Hierarcys_List(0, 1);
                                }
                            }
                            else if (dialog_type == SELS_DIALOG)
                            {
                                if (index + sels_start[current_sel_type] < Selections_c[current_sel_type])
                                {
                                    if (Edit_Lock)
                                    {
                                        Edit_Lock = 0;
                                        if (strlen(EditString) > 0)
                                        {
                                            sprintf(Sels_Names[current_sel_type][SelsIndex[current_sel_type]], "%s", EditString);
                                            sprintf(Name_Remember, "%s", EditString);
                                        }
                                        EditCursor = 0;
                                        EditString[EditCursor] = '\0';
                                        printf("Edit finishing!\n");
                                        set_Sels_H_Button(-1);
                                    }
                                    else
                                    {
                                        if (SelsIndex[current_sel_type] - sels_start[current_sel_type] >= 0)
                                            SelsList[SelsIndex[current_sel_type] - sels_start[current_sel_type]].color = UI_BLACK;
                                        SelsIndex[current_sel_type] = index + sels_start[current_sel_type];
                                        if (SelsIndex[current_sel_type] - sels_start[current_sel_type] >= 0)
                                            SelsList[SelsIndex[current_sel_type] - sels_start[current_sel_type]].color = UI_BACKL;
                                    }
                                    UPDATE_BACKGROUND = 1;
                                    update_Selections_List(0, 1);
                                }
                            }
                            else if (dialog_type == TEXT_DIALOG)
                            {
                                black_out_TextList();

                                if (strcmp(text, TEXT_TYPE_TEXT) == 0)
                                {
                                    TextIndex = index + texts_start;
                                    if (TextIndex < Textures_c && TextIndex >= 0)
                                    {
                                        Materials[O->surface].Texture_idx = TextIndex;
                                        Materials[O->surface].texture = TextIndex;
                                        TextList[index].color = UI_BACKL;
                                        update_Texts_List();
                                        TextList[index].color = UI_BLACK;
                                    }
                                }
                                else if (strcmp(text, TEXT_TYPE_NORM) == 0)
                                {
                                    TextIndex = index + texts_start;
                                    if (TextIndex < Normals_c && TextIndex >= 0)
                                    {
                                        Materials[O->surface].Normal_idx = TextIndex;
                                        Materials[O->surface].normal = TextIndex;
                                        TextList[index].color = UI_BACKL;
                                        update_Texts_List();
                                        TextList[index].color = UI_BLACK;
                                    }
                                }
                                else if (strcmp(text, TEXT_TYPE_BUMP) == 0)
                                {
                                    TextIndex = index + texts_start;
                                    if (TextIndex < Bumps_c && TextIndex >= 0)
                                    {
                                        Materials[O->surface].Bump_idx = TextIndex;
                                        Materials[O->surface].bump = TextIndex;
                                        TextList[index].color = UI_BACKL;
                                        update_Texts_List();
                                        TextList[index].color = UI_BLACK;
                                    }
                                }
                            }
                            else if (dialog_type == ITEM_DIALOG)
                            {
                                if (strcmp(item, ITEM_TYPE_OBJECT) == 0)
                                {
                                    ItemIndex = index + items_start;
                                    if (currentObject - items_start >= 0 && currentObject - items_start < LISTLENGTH)
                                    {
                                        ItemList[currentObject - items_start].color = UI_BLACK;
                                    }

                                    if (ItemIndex < objectIndex && ItemIndex >= 0)
                                    {
                                        sels_start[current_sel_type] = 0;
                                        if (controlDown)
                                        {
                                            objects[ItemIndex]->selected = 0;
                                            ItemList[index].color = UI_BLACK;
                                            assert_Object_Selection();
                                            if (ItemIndex == currentObject && selected_object_count > 0)
                                            {
                                                currentObject = selected_objects[selected_object_count - 1];
                                                O = objects[currentObject];
                                            }
                                            if (currentObject - items_start >= 0)
                                                ItemList[currentObject - items_start].color = UI_BACKL;

                                            update_Items_List();
                                            glDrawBuffer(GL_BACK);
                                        }
                                        else
                                        {
                                            currentObject = ItemIndex;
                                            O = objects[currentObject];
                                            O->selected = 1;
                                            ItemList[index].color = UI_BACKL;
                                            assert_Object_Selection();

                                            update_Items_List();
                                            ItemList[index].color = UI_BLACK;
                                        }
                                    }
                                }
                            }
                            else if (dialog_type == MATERIAL_DIALOG)
                            {
                                if (index + materials_start < Materials_count)
                                {
                                    MatrList[currentMaterial - materials_start].color = UI_BLACK;
                                    currentMaterial = index + materials_start;
                                    MatrList[index].color = UI_BACKL;
                                    draw_Materials_List(screen_height, materials_start, 1, currentObject);
                                    SDL_GL_SwapBuffers();
                                    MatrList[index].color = UI_BLACK;
                                }
                            }
                            else
                            {
                                if (index + files_start < files_num)
                                {
                                    fileName[0] = '\0';
                                    strcat(fileName, files_dir);
                                    strcat(fileName, Out_List[index + files_start]);
                                    printf("Out List %s\n", fileName);
                                    FileList[index].color = UI_BACKL;
                                    draw_Files_List(files_dir, screen_height, files_start, extension, 1, 1);
                                    SDL_GL_SwapBuffers();
                                    FileList[index].color = UI_BLACK;
                                    if (dialog_type == OBJ_DIALOG)
                                        open_OBJ(fileName);
                                    else if (dialog_type == IMG_DIALOG)
                                        open_IMG(fileName);
                                }
                            }
                        }
                        else if (!Edit_Lock && mouse_x > SIDEBAR && mouse_x < SIDEBAR * 2)
                        {
                            Buttonindex = index - 1;
                            if (dialog_type == SAVES_DIALOG)
                            {
                                if (Buttonindex > -1 && Buttonindex < SCENE_EXT_NUM && Button_scene_ext[Buttonindex].func != NULL)
                                {
                                    (*Button_scene_ext[Buttonindex].func)(Buttonindex, dialog_type);
                                    if (!NVIDIA) glDrawBuffer(GL_FRONT_AND_BACK);
                                    draw_Saves_Dialog(scene_files_dir, "Save scene", screen_height, scene_extension, scene_extensions, scene_ext_count, scene_files_start, 1, 1, Edit_Lock);
                                    SDL_GL_SwapBuffers();
                                    glDrawBuffer(GL_BACK);
                                }
                            }
                            else if (dialog_type == LOADING_DIALOG)
                            {
                                if (Buttonindex > -1 && Buttonindex < SCENE_EXT_NUM && Button_scene_ext[Buttonindex].func != NULL)
                                {
                                    (*Button_scene_ext[Buttonindex].func)(Buttonindex, dialog_type);
                                    if (!NVIDIA) glDrawBuffer(GL_FRONT_AND_BACK);
                                    draw_Loading_Dialog(scene_files_dir, "Load scene", screen_height, scene_extension, scene_extensions, scene_ext_count, scene_files_start, 1, 1, Edit_Lock);
                                    SDL_GL_SwapBuffers();
                                    glDrawBuffer(GL_BACK);
                                }
                            }
                            else if (dialog_type == POSE_DIALOG)
                            {

                            }
                            else if (dialog_type == BONE_DIALOG)
                            {

                            }
                            else if (dialog_type == IK_DIALOG)
                            {

                            }
                            else if (dialog_type == HIER_DIALOG)
                            {

                            }
                            else if (dialog_type == DEFR_DIALOG)
                            {
                                if (Buttonindex > -1 && Buttonindex < defr_type_count && Button_defr[Buttonindex].func != NULL)
                                {
                                    (*Button_defr[Buttonindex].func)(Buttonindex);
                                    if (!NVIDIA) glDrawBuffer(GL_FRONT_AND_BACK);
                                    draw_Deformers_Dialog("Deformers L.", screen_height, defr_type, defr_types, defr_type_count,
                                                            defr_start, 1, DefrIndex - defr_start,
                                                            hier_start, HierIndex - hier_start,
                                                            sels_start[current_sel_type], SelsIndex[current_sel_type] - sels_start[current_sel_type]);
                                    SDL_GL_SwapBuffers();
                                    glDrawBuffer(GL_BACK);
                                }
                            }
                            else if (dialog_type == SELS_DIALOG)
                            {
                                if (Buttonindex > -1 && Buttonindex < sel_type_count && Button_sels[Buttonindex].func != NULL)
                                {
                                    (*Button_sels[Buttonindex].func)(Buttonindex);
                                    if (!NVIDIA) glDrawBuffer(GL_FRONT_AND_BACK);
                                    draw_Selections_Dialog("Selections L.", screen_height, sel_type, sel_types, sel_type_count, sels_start[current_sel_type], 1, SelsIndex[current_sel_type] - sels_start[current_sel_type]);
                                    SDL_GL_SwapBuffers();
                                    glDrawBuffer(GL_BACK);
                                }
                            }
                            else if (dialog_type == TEXT_DIALOG)
                            {
                                if (Buttonindex > -1 && Buttonindex < texts_count && Button_text[Buttonindex].func != NULL)
                                {
                                    (*Button_text[Buttonindex].func)(Buttonindex);
                                    DRAW_UI = 0;
                                    open_Textures_List();
                                    DRAW_UI = 1;
                                }
                            }
                            else if (dialog_type == ITEM_DIALOG)
                            {
                                if (Buttonindex > -1 && Buttonindex < items_count && Button_item[Buttonindex].func != NULL)
                                {
                                    (*Button_item[Buttonindex].func)(Buttonindex);
                                    if (!NVIDIA) glDrawBuffer(GL_FRONT_AND_BACK);
                                    draw_Items_Dialog("Items List", screen_height, item, items, items_count, items_start, 1, currentObject);
                                    SDL_GL_SwapBuffers();
                                    glDrawBuffer(GL_BACK);
                                }
                            }
                            else
                            {
                                if (Buttonindex > -1 && Buttonindex < ext_count && Button_ext[Buttonindex].func != NULL)
                                {
                                    (*Button_ext[Buttonindex].func)(Buttonindex, dialog_type);
                                    if (!NVIDIA) glDrawBuffer(GL_FRONT_AND_BACK);
                                    files_num = query_files(files_dir, extension);
                                    if (dialog_type == OBJ_DIALOG)
                                    {
                                        if (obj_extension_type == obj_EXTENSION)
                                            files_start = obj_files_start;
                                        else if (obj_extension_type == OBJ_EXTENSION)
                                            files_start = OBJ_files_start;
                                    }
                                    else if (dialog_type == IMG_DIALOG)
                                    {
                                        if (img_extension_type == png_EXTENSION)
                                            files_start = png_files_start;
                                        else if (img_extension_type == jpg_EXTENSION)
                                            files_start = jpg_files_start;
                                        else if (img_extension_type == tif_EXTENSION)
                                            files_start = tif_files_start;
                                        else if (img_extension_type == PSD_EXTENSION)
                                            files_start = PSD_files_start;
                                    }
                                    draw_Files_Dialog(files_dir, "Open file", screen_height, extension, extensions, ext_count, files_start, 1, 0, Edit_Lock);
                                    printf("files num %d\n", files_num);
                                    SDL_GL_SwapBuffers();
                                    glDrawBuffer(GL_BACK);
                                }
                            }
                        }
                        else if (mouse_x > SIDEBAR * 2 && mouse_x < SIDEBAR + DIALOG_WIDTH && mouse_y > DIALOG_HEIGHT - BUTTON_HEIGHT && mouse_y < DIALOG_HEIGHT)
                        {
                            int h_index;
                            if (mouse_x > SIDEBAR + DIALOG_WIDTH - 20 && mouse_x < SIDEBAR + DIALOG_WIDTH)
                            {
                                Drag_Dialog = 1;
                            }
                            else if (dialog_type == SAVES_DIALOG)
                            {
                                if (!Edit_Lock)
                                {
                                    if (mouse_x > SIDEBAR + DIALOG_WIDTH - BUTTON_WIDTH_MEDIUM - 20 && mouse_x <= SIDEBAR + DIALOG_WIDTH - 20)
                                    {
                                        (*Button_h_scen[0].func)();
                                    }
                                    else
                                    {
                                        rename_Scene_dir();
                                    }
                                }
                            }
                            else if (dialog_type == LOADING_DIALOG)
                            {
                                if (!Edit_Lock)
                                {
                                    if (mouse_x > SIDEBAR + DIALOG_WIDTH - BUTTON_WIDTH_MEDIUM - 20 && mouse_x <= SIDEBAR + DIALOG_WIDTH - 20)
                                    {
                                        (*Button_h_scen[0].func)();
                                    }
                                    else
                                    {
                                        rename_Scene_dir();
                                    }
                                }
                            }
                            else if (dialog_type == BONE_DIALOG && !Edit_Lock)
                            {
                                h_index = (mouse_x - SIDEBAR * 2) / BUTTON_WIDTH_SHORT;
                                if (h_index < H_BONE_NUM)
                                {
                                    (*Button_h_bone[h_index].func)();
                                }
                            }
                            else if (dialog_type == IK_DIALOG && !Edit_Lock)
                            {
                                h_index = (mouse_x - SIDEBAR * 2) / BUTTON_WIDTH_SHORT;
                                if (h_index < H_IKCH_NUM)
                                {
                                    (*Button_h_ikch[h_index].func)();
                                }
                            }
                            else if (dialog_type == POSE_DIALOG && !Edit_Lock)
                            {
                                h_index = (mouse_x - SIDEBAR * 2) / BUTTON_WIDTH_SHORT;
                                if (h_index < H_POSE_NUM)
                                {
                                    (*Button_h_pose[h_index].func)();
                                }
                            }
                            else if (dialog_type == DEFR_DIALOG && !Edit_Lock)
                            {
                                if (current_defr_type == 0)
                                {
                                    h_index = (mouse_x - SIDEBAR * 2) / BUTTON_WIDTH_SHORT;
                                    if (h_index < H_DEFR_NUM)
                                    {
                                        (*Button_h_defr[h_index].func)();
                                    }
                                }
                                else if (current_defr_type == 1)
                                {
                                    h_index = (mouse_x - SIDEBAR * 2) / BUTTON_WIDTH_MEDIUM;
                                    if (h_index < H_HIER_NUM)
                                    {
                                        (*Button_h_hier[h_index].func)();
                                        UPDATE_BACKGROUND = 0;
                                        update_Hierarcys_List(0, 1);
                                    }
                                }
                                else if (current_defr_type == 2)
                                {
                                    h_index = (mouse_x - SIDEBAR * 2) / BUTTON_WIDTH_SHORT;
                                    if (h_index < H_SELS_NUM)
                                    {
                                        (*Button_h_sels[h_index].func)();
                                        UPDATE_BACKGROUND = 0;
                                        update_Selections_List(0, 1);
                                    }
                                }
                            }
                            else if (dialog_type == HIER_DIALOG && !Edit_Lock)
                            {
                                h_index = (mouse_x - SIDEBAR * 2) / BUTTON_WIDTH_MEDIUM;
                                if (h_index < H_HIER_NUM)
                                {
                                    (*Button_h_hier[h_index].func)();
                                    UPDATE_BACKGROUND = 1;
                                    update_Hierarcys_List(0, 1);
                                }
                            }
                            else if (dialog_type == SELS_DIALOG && !Edit_Lock)
                            {
                                h_index = (mouse_x - SIDEBAR * 2) / BUTTON_WIDTH_SHORT;
                                if (h_index < H_SELS_NUM)
                                {
                                    (*Button_h_sels[h_index].func)();
                                    UPDATE_BACKGROUND = 1;
                                    update_Selections_List(0, 1);
                                    message = -1;
                                }
                            }
                            else if (dialog_type == OBJ_DIALOG)
                            {
                                open_ObjectsDir();
                                Button_sidebar[Buttonindex_sidebar].color = UI_GRAYD;
                                open_OBJ_List();
                                Button_sidebar[Buttonindex_sidebar].color = UI_GRAYB;
                            }
                            else if (dialog_type == IMG_DIALOG)
                            {
                                open_ImagesDir();
                                Button_sidebar[Buttonindex_sidebar].color = UI_GRAYD;
                                open_Img_List();
                                Button_sidebar[Buttonindex_sidebar].color = UI_GRAYB;
                            }
                            else if (dialog_type == TEXT_DIALOG)
                            {
                                h_index = (mouse_x - SIDEBAR * 2) / BUTTON_WIDTH;
                                if (h_index < H_TEXT_NUM)
                                {
                                    (*Button_h_text[h_index].func)();
                                    UPDATE_BACKGROUND = 1;
                                    message = -1;
                                }
                            }
                        }
                    }
                    else if (!Camera_screen_lock && mouse_x > SIDEBAR && mouse_y < screen_height)
                    {
                        mouse_button_down = 1;
                        cull_Selection = 0;
                        handle_ControlDown();
                        rendermode = ID_RENDER;

                        ELEMENT_ARRAYS = 0;
                        init_Hint();

                        int level, idx;

                        if (subdLevel > -1)
                        {
                            level = subdLevel; // because large polygon counts
                            if (LOCAT_ID_RENDER && Object_Mode)
                            {

                            }
                            else if (Object_Mode)
                            {
                                load_id_colors_all(Camera, level, OBJECT_ID_COLORS);
                            }
                            else if (Polygon_Mode)
                            {
                                Camera->objects[0] = currentObject;
                                Camera->object_count = 1;
                                load_id_colors_current(O, level, POLYGON_ID_COLORS);
                            }
                            else if (Edge_Mode)
                            {
                                Camera->objects[0] = currentObject;
                                Camera->object_count = 1;
                            }
                            else if (Vertex_Mode)
                            {
                                Camera->objects[0] = currentObject;
                                Camera->object_count = 1;
                            }
                        }
                        else
                        {
                            level = -1;
                            if (LOCAT_ID_RENDER && Object_Mode)
                            {

                            }
                            else if (Object_Mode)
                            {
                                load_id_colors_Fan_all(Camera, OBJECT_ID_COLORS);
                            }
                            else if (Polygon_Mode)
                            {
                                Camera->objects[0] = currentObject;
                                Camera->object_count = 1;
                                load_id_colors_Fan_current(O, POLYGON_ID_COLORS);
                            }
                            else if (Edge_Mode)
                            {
                                Camera->objects[0] = currentObject;
                                Camera->object_count = 1;
                            }
                            else if (Vertex_Mode)
                            {
                                Camera->objects[0] = currentObject;
                                Camera->object_count = 1;
                            }
                        }
                        UPDATE_COLORS = 1;
                        DRAW_UI = 0;
                        if (LOCAT_ID_RENDER && Object_Mode)
                        {
                            poly_Render(0, 0, splitview, CamDist, 0, level);
                        }
                        else if (Vertex_Mode)
                        {
                            VERTS_ID_RENDER = 1;
                            poly_Render(0, 0, splitview, CamDist, 0, level);
                            VERTS_ID_RENDER = 0;
                        }
                        else if (Edge_Mode)
                        {
                            EDGES_ID_RENDER = 1;
                            poly_Render(0, 0, splitview, CamDist, 0, level);
                            EDGES_ID_RENDER = 0;
                        }
                        else
                        {
                            POLYS_ID_RENDER = 1;
                            poly_Render(0, 0, splitview, CamDist, 0, level);
                            POLYS_ID_RENDER = 0;
                        }

                        //SDL_Delay(delay_ms);

                        rendermode = POLY_RENDER;

                        GLubyte pixel[4 * 81];
                        glReadPixels(mouse_x - 4, screen_height + BOTTOM_LINE - mouse_y - 4, 9, 9, GL_RGBA, GL_UNSIGNED_BYTE, pixel);
                        printf("%u %u %u %u\n", pixel[0 + 162], pixel[1 + 162], pixel[2 + 162], pixel[3 + 162]);

                        int u, p, o;

                        for (p = 0; p < 81; p ++)
                        {
                            pixel_id[p] = (GLubyte)pixel[0 + p * 4] * 255 * 255 * 255 + (GLubyte)pixel[1 + p * 4] * 255 * 255 + (GLubyte)pixel[2 + p * 4] * 255 + (GLubyte)pixel[3 + p * 4];
                        }

                        for (p = 0; p < 81; p ++)
                        {
                            o = pixel_id[p_index[p]];
                            if (Object_Mode && LOCAT_ID_RENDER && o < transformerIndex && o >= 0)
                            {
                                T = transformers[o];
                                if (T->Bone != NULL && T == T->Bone->A)
                                {
                                    if (T->parent != NULL && T->parent->Bone != NULL)
                                    {
                                        T = T->parent;
                                        o = T->index;
                                    }
                                }
                                if (add_selection_mode)
                                {
                                    T->selected = 1;
                                    currentLocator = o;
                                    //LocatorSize = T->LocatorSize;
                                    select_Transformer_Bone(T);
                                    select_Transformer_IK(T);
                                    select_Deformer();
                                }
                                else
                                {
                                    T->selected = 0;
                                }
                                break;
                            }
                            else if (Object_Mode && o < objectIndex && o >= 0)
                            {
                                sels_start[current_sel_type] = 0;
                                O = objects[o];
                                currentObject = o;
                                printf("%s\n", objects[currentObject]->Name);
                                if (add_selection_mode)
                                {
                                    if (!O->selected)
                                    {
                                        O->selected = 1;
                                        selected_objects[selected_object_count ++] = o;
                                    }
                                }
                                else
                                {
                                    if (O->selected && selected_object_count > 1)
                                    {
                                        O->selected = 0;
                                        int s_o[OBJECTS];
                                        int s_o_count = 0;
                                        int s;
                                        for (s = 0; s < selected_object_count; s ++)
                                        {
                                            if (selected_objects[s] != o)
                                            {
                                                s_o[s_o_count ++] = selected_objects[s];
                                            }
                                        }
                                        selected_object_count --;
                                        for (s = 0; s < selected_object_count; s ++)
                                        {
                                            selected_objects[s] = s_o[s];
                                        }
                                        if (selected_object_count - 1 >= 0)
                                        {
                                            currentObject = selected_objects[selected_object_count - 1];
                                            O = objects[currentObject];
                                        }
                                    }
                                }
                                break;
                            }
                            else if (Polygon_Mode && o < objects[currentObject]->polycount && o >= 0)
                            {
                                polygon * P = &O->polys[o / ARRAYSIZE][o % ARRAYSIZE];
                                printf("poly id %d\n", o);
                                if (add_selection_mode)
                                {
                                    if (P->selected == 0)
                                    {
                                        assignSelectionToQuads(O, P, 1);
                                        O->last_selected_polys[0] = O->last_selected_polys[1];
                                        O->last_selected_polys[1] = P->index;
                                    }
                                }
                                else
                                {
                                    if (P->selected == 1)
                                    {
                                        assignSelectionToQuads(O, P, 0);
                                    }
                                }
                                load_id_colors_polygon(O, P, OBJECT_COLORS);
                                break;
                            }
                            else if (Edge_Mode)
                            {
                                uv_edge * UVE;
                                edge * E;
                                if (Camera->uv_draw)
                                {
                                    if (o > -1 && o < O->uvedcount)
                                    {
                                        UVE = &O->uveds[o / ARRAYSIZE][o % ARRAYSIZE];
                                        printf("uv edge id %d\n", o);
                                        if (add_selection_mode)
                                        {
                                            UVE->selected = 1;
                                        }
                                        else
                                        {
                                            UVE->selected = 0;
                                        }
                                        idx = UVE->edge;
                                        if (idx > -1 && idx < O->edgecount)
                                        {
                                            E = &O->edges[idx / ARRAYSIZE][idx % ARRAYSIZE];
                                            E->selected = UVE->selected;
                                        }
                                    }
                                    break;
                                }
                                else
                                {
                                    if (o > -1 && o < O->edgecount)
                                    {
                                        E = &O->edges[o / ARRAYSIZE][o % ARRAYSIZE];
                                        printf("edge id %d\n", o);
                                        if (add_selection_mode)
                                        {
                                            E->selected = 1;
                                            O->last_selected_edges[0] = O->last_selected_edges[1];
                                            O->last_selected_edges[1] = E->index;
                                        }
                                        else
                                        {
                                            E->selected = 0;
                                        }
                                        for (u = 0; u < E->uv_edcount; u ++)
                                        {
                                            idx = E->uv_edges[u];
                                            if (idx > -1 && idx < O->uvedcount)
                                            {
                                                UVE = &O->uveds[idx / ARRAYSIZE][idx % ARRAYSIZE];
                                                UVE->selected = E->selected;
                                            }
                                        }
                                        break;
                                    }
                                }
                            }
                            else if (Vertex_Mode)
                            {
                                uv * UV;
                                vertex * V;
                                if (Camera->uv_draw)
                                {
                                    if (o > -1 && o < O->textcount)
                                    {
                                        UV = &O->uvtex[o / ARRAYSIZE][o % ARRAYSIZE];
                                        printf("uv vertex id %d\n", o);
                                        if (add_selection_mode)
                                        {
                                            UV->selected = 1;
                                        }
                                        else
                                        {
                                            UV->selected = 0;
                                        }
                                        idx = UV->vert;
                                        if (idx > -1 && idx < O->vertcount)
                                        {
                                            V = &O->verts[idx / ARRAYSIZE][idx % ARRAYSIZE];
                                            V->selected = UV->selected;
                                        }
                                        break;
                                    }
                                }
                                else
                                {
                                    if (o > -1 && o < O->vertcount)
                                    {
                                        V = &O->verts[o / ARRAYSIZE][o % ARRAYSIZE];
                                        printf("vertex id %d\n", o);
                                        if (add_selection_mode)
                                        {
                                            V->selected = 1;
                                            O->last_selected_verts[0] = O->last_selected_verts[1];
                                            O->last_selected_verts[1] = V->index;
                                        }
                                        else
                                        {
                                            V->selected = 0;
                                        }
                                        for (u = 0; u < V->uv_vertcount; u ++)
                                        {
                                            idx = V->uv_verts[u];
                                            if (idx > -1 && idx < O->textcount)
                                            {
                                                UV = &O->uvtex[idx / ARRAYSIZE][idx % ARRAYSIZE];
                                                UV->selected = V->selected;
                                            }
                                        }
                                        break;
                                    }
                                }
                            }
                        }

                        if (Object_Mode && LOCAT_ID_RENDER)
                        {

                        }
                        else if (Object_Mode || Polygon_Mode)
                        {
                            if (subdLevel > -1)
                            load_id_colors_No_Surface(Camera, level);
                            else
                            load_id_colors_Fan_all(Camera, OBJECT_COLORS);
                        }

                        assert_Element_Selection_(O);

                        if (!NVIDIA) glDrawBuffer(GL_FRONT_AND_BACK);

                        DRAW_UI = 1;
                    }
                    else
                    {
                        subdLevel = subdLevel_mem;
                        camera_rotate = 0;
                        camera_z_move = 0;
                        object_hook = 0;
                        ROTATION = 0;
                        MOVEMENT = 0;
                        BONES_MODE = 0;
                        SCALE = 0;
                        Axis_lock = 0;
                        altDown = 0;
                        Camera_screen_lock = 0;
                    }
                }

                blit_ViewPort();

                message = -10;
            }
            else if (event.type == SDL_MOUSEBUTTONUP) // scroll up
            {
                mouse_button_down = 0;
                Drag_Dialog = 0;

                if (drag_rectangle)
                {
                    printf("Camera width %d, height %d\n", Camera->width, Camera->height);
                    O = objects[currentObject];
                    put_In_Rectangle_Selection(Camera, O, selected_objects, selected_object_count, add_selection_mode, selection_Mode,
                                               &Drag_Rectangle, splitview, CamDist, draw_uv_view, cull_Selection, DRAW_LOCATORS, subdLevel, ELEMENT_ARRAYS);

                    if (DRAW_LOCATORS)
                    {
                        assert_Locators_Selection();
                    }
                    else if (Object_Mode)
                    {
                        assert_Object_Selection();
                    }
                    else
                    {
                        assert_Element_Selection();
                    }

                    drag_rectangle = 0;
                    cull_Selection = 0;
                    handle_ControlDown();
                    DRAW_UI = 1;
                    UPDATE_COLORS = 1;
                    if (dialog_lock)
                    {
                        UPDATE_SELECTIONS = 0;
                        draw_Dialog();
                        UPDATE_SELECTIONS = 1;
                    }
                    message = -10;
                }
                else
                {
                    if (DRAW_LOCATORS)
                    {
                        assert_Locators_Selection();
                    }
                    assert_Element_Selection_(O);

                    message = -11;
                }
            }
            else if (event.type == SDL_MOUSEMOTION)
            {
                mouse_x = event.motion.x;
                mouse_y = event.motion.y;
                if (Drag_Dialog)
                {
                    DIALOG_WIDTH = mouse_x - SIDEBAR;
                    DIALOG_HEIGHT = mouse_y;
                    if (DIALOG_HEIGHT < MIN_DIALOG_HEIGHT)
                    {
                        DIALOG_HEIGHT = MIN_DIALOG_HEIGHT;
                    }
                    LISTLENGTH = DIALOG_HEIGHT / BUTTON_HEIGHT - 1;
                    if (LISTLENGTH > MAX_LISTLENGTH)
                    {
                        LISTLENGTH = MAX_LISTLENGTH;
                        DIALOG_HEIGHT = LISTLENGTH * BUTTON_HEIGHT + 1;
                    }
                    if (DIALOG_WIDTH < MIN_DIALOG_WIDTH)
                    {
                        DIALOG_WIDTH = MIN_DIALOG_WIDTH;
                    }
                    draw_Dialog();
                }
                else if (edgeWeights)
                {
                    float x_offset = mouse_x - Camera->origin_2d[0];
                    float y_offset = mouse_y - Camera->origin_2d[1];

                    float x = (float)x_offset / ((float)screen_width / 6.0);
                    float y = (float)y_offset / ((float)screen_height / 6.0);

                    adjust_Selected_Edges_Weights(x);
                    adjust_Selected_Verts_Weights(y, subdLevel);
                    message = -1;
                }
                else if (Camera_screen_lock)
                {
                    //subdLevel = -1;

                    float d;
                    if (object_hook)
                    {
                        if (DRAW_LOCATORS)
                        {
                            T = transformers[currentLocator];
                        }
                        else
                        {
                            T = objects[currentObject]->T;
                        }
                        d = -1.0;
                    }
                    else
                    {
                        T = Camera->T;
                        d = 1.0;
                    }
                    float x_offset;
                    float y_offset;
                    direction D;
                    //printf("%d %d\n", mouse_x, mouse_y);

                    if (splitview)
                    {
                        if (Camera == &Camera_Top) // Top
                        {
                            x_offset = mouse_x - Camera_Top.origin_2d[0];
                            y_offset = mouse_y - Camera_Top.origin_2d[1];
                            if (object_hook == 2) //scale
                            {
                                if (Axis_lock)
                                {
                                    T->scl[0] = object_Scl[0];
                                    T->scl[1] = object_Scl[1];
                                    T->scl[2] = object_Scl[2];
                                    T->scl[Axis_lock - 1] = object_Scl[Axis_lock - 1] + (float)x_offset / ((float)screen_width / 4.0);
                                }
                                else
                                {
                                    T->scl[0] = object_Scl[0] + (float)x_offset / ((float)screen_width / 4.0);
                                    T->scl[1] = object_Scl[1] + (float)x_offset / ((float)screen_width / 4.0);
                                    T->scl[2] = object_Scl[2] + (float)x_offset / ((float)screen_width / 4.0);
                                }
                            }
                            else if (object_hook == 3) //rotate
                            {
                                if (Axis_lock)
                                {
                                    T->rot[0] = object_Rot[0];
                                    T->rot[1] = object_Rot[1];
                                    T->rot[2] = object_Rot[2];
                                    T->rot[Axis_lock - 1] += (float)x_offset / ((float)screen_width / 4.0);
                                }
                                else
                                {
                                    T->rot[0] = object_Rot[0] + (float)y_offset / ((float)screen_height / 4.0);
                                    T->rot[2] = object_Rot[2] - (float)x_offset / ((float)screen_width / 4.0);
                                }
                            }
                            else // move
                            {
                                if (object_hook)
                                {
                                    direction D1;
                                    D = screen_point_to_vector(mouse_x - SIDEBAR, mouse_y, screen_width / 2, screen_height / 2, Camera_Top.h_view, Camera_Top.v_view);
                                    D.x = -D.x;
                                    //D.y = -D.y;
                                    D.z = -D.z;
                                    rotate_Vertex_I(Camera_Top.T->rotVec, D.x, D.y, D.z, &D1);
                                    float Dir_vec[3] = {0, 0, 1};
                                    float dot = dot_productN((normal*)&D, Dir_vec);

                                    Pos[0] = Camera_Top.T->pos[0] + D1.x * (ObjDist / dot);
                                    Pos[1] = Camera_Top.T->pos[1] + D1.y * (ObjDist / dot);
                                    Pos[2] = Camera_Top.T->pos[2] + D1.z * (ObjDist / dot);

                                    Delta[0] = Pos[0] - T_pos[0];
                                    Delta[1] = Pos[1] - T_pos[1];
                                    Delta[2] = Pos[2] - T_pos[2];

                                    if (Axis_lock)
                                    {
                                        Magnitude = sqrt(Delta[0] * Delta[0] + Delta[1] * Delta[1] + Delta[2] * Delta[2]);
                                        Delta[0] = 0;
                                        Delta[1] = 0;
                                        Delta[2] = 0;
                                        if (Axis_lock == 3)
                                        {
                                            if (y_offset > 0)
                                                Delta[Axis_lock - 1] = Magnitude;
                                            else
                                                Delta[Axis_lock - 1] = -Magnitude;
                                        }
                                        else
                                        {
                                            if (x_offset > 0)
                                                Delta[Axis_lock - 1] = Magnitude;
                                            else
                                                Delta[Axis_lock - 1] = -Magnitude;
                                        }
                                    }
                                    make_Movement();
                                }
                                else
                                {
                                    Camera_Top.origin_2d[0] = mouse_x;
                                    Camera_Top.origin_2d[1] = mouse_y;
                                    D = set_Target(x_offset, y_offset, screen_width / 2.0, screen_height / 2.0, Camera_Top.h_view, Camera_Top.v_view, CamDist);
                                    if (camera_z_move)
                                    {
                                        CamDist += D.x * 2;
                                    }
                                    else
                                    {
                                        if (VIEWS_FLIPPED)
                                        {
                                            T->target[0] -= D.x * d;
                                            T->target[2] += D.y * d;
                                            T->pos[0] -= D.x * d;
                                            T->pos[2] += D.y * d;
                                        }
                                        else
                                        {
                                            T->target[0] -= D.x * d;
                                            T->target[2] -= D.y * d;
                                            T->pos[0] -= D.x * d;
                                            T->pos[2] -= D.y * d;
                                        }
                                    }
                                }
                            }
                        }
                        else if (Camera == &Camera_Front) // Front
                        {
                            x_offset = mouse_x - Camera_Front.origin_2d[0];
                            y_offset = mouse_y - Camera_Front.origin_2d[1];
                            if (object_hook == 2) //scale
                            {
                                if (Axis_lock)
                                {
                                    T->scl[0] = object_Scl[0];
                                    T->scl[1] = object_Scl[1];
                                    T->scl[2] = object_Scl[2];
                                    T->scl[Axis_lock - 1] = object_Scl[Axis_lock - 1] + (float)x_offset / ((float)screen_width / 4.0);
                                }
                                else
                                {
                                    T->scl[0] = object_Scl[0] + (float)x_offset / ((float)screen_width / 4.0);
                                    T->scl[1] = object_Scl[1] + (float)x_offset / ((float)screen_width / 4.0);
                                    T->scl[2] = object_Scl[2] + (float)x_offset / ((float)screen_width / 4.0);
                                }
                            }
                            else if (object_hook == 3) //rotate
                            {
                                if (Axis_lock)
                                {
                                    T->rot[0] = object_Rot[0];
                                    T->rot[1] = object_Rot[1];
                                    T->rot[2] = object_Rot[2];
                                    T->rot[Axis_lock - 1] += (float)x_offset / ((float)screen_width / 4.0);
                                }
                                else
                                {
                                    T->rot[0] = object_Rot[0] + (float)y_offset / ((float)screen_height / 4.0);
                                    T->rot[1] = object_Rot[1] + (float)x_offset / ((float)screen_width / 4.0);
                                }
                            }
                            else // move
                            {
                                if (object_hook)
                                {
                                    direction D1;
                                    D = screen_point_to_vector(mouse_x - SIDEBAR, mouse_y - screen_height / 2, screen_width / 2, screen_height / 2, Camera_Front.h_view, Camera_Front.v_view);
                                    D.x = -D.x;
                                    //D.y = -D.y;
                                    D.z = -D.z;
                                    rotate_Vertex_I(Camera_Front.T->rotVec, D.x, D.y, D.z, &D1);
                                    float Dir_vec[3] = {0, 0, 1};
                                    float dot = dot_productN((normal*)&D, Dir_vec);

                                    Pos[0] = Camera_Front.T->pos[0] + D1.x * (ObjDist / dot);
                                    Pos[1] = Camera_Front.T->pos[1] + D1.y * (ObjDist / dot);
                                    Pos[2] = Camera_Front.T->pos[2] + D1.z * (ObjDist / dot);

                                    Delta[0] = Pos[0] - T_pos[0];
                                    Delta[1] = Pos[1] - T_pos[1];
                                    Delta[2] = Pos[2] - T_pos[2];

                                    if (Axis_lock)
                                    {
                                        Magnitude = sqrt(Delta[0] * Delta[0] + Delta[1] * Delta[1] + Delta[2] * Delta[2]);
                                        Delta[0] = 0;
                                        Delta[1] = 0;
                                        Delta[2] = 0;
                                        if (Axis_lock == 2)
                                        {
                                            if (y_offset < 0)
                                                Delta[Axis_lock - 1] = Magnitude;
                                            else
                                                Delta[Axis_lock - 1] = -Magnitude;
                                        }
                                        else
                                        {
                                            if (x_offset > 0)
                                                Delta[Axis_lock - 1] = Magnitude;
                                            else
                                                Delta[Axis_lock - 1] = -Magnitude;
                                        }
                                    }
                                    make_Movement();
                                }
                                else
                                {
                                    Camera_Front.origin_2d[0] = mouse_x;
                                    Camera_Front.origin_2d[1] = mouse_y;
                                    D = set_Target(x_offset, y_offset, screen_width / 2.0, screen_height / 2.0, Camera_Front.h_view, Camera_Front.v_view, CamDist);
                                    if (camera_z_move)
                                    {
                                        CamDist += D.x * 2;
                                    }
                                    else
                                    {
                                        if (VIEWS_FLIPPED)
                                        {
                                            T->target[0] += D.x * d;
                                            T->target[1] += D.y * d;
                                            T->pos[0] += D.x * d;
                                            T->pos[1] += D.y * d;
                                        }
                                        else
                                        {
                                            T->target[0] -= D.x * d;
                                            T->target[1] += D.y * d;
                                            T->pos[0] -= D.x * d;
                                            T->pos[1] += D.y * d;
                                        }
                                    }
                                }
                            }
                        }
                        else if (Camera == &Camera_Left) // Left
                        {
                            x_offset = mouse_x - Camera_Left.origin_2d[0];
                            y_offset = mouse_y - Camera_Left.origin_2d[1];
                            if (object_hook == 2) //scale
                            {
                                if (Axis_lock)
                                {
                                    T->scl[0] = object_Scl[0];
                                    T->scl[1] = object_Scl[1];
                                    T->scl[2] = object_Scl[2];
                                    T->scl[Axis_lock - 1] = object_Scl[Axis_lock - 1] + (float)x_offset / ((float)screen_width / 4.0);
                                }
                                else
                                {
                                    T->scl[0] = object_Scl[0] + (float)x_offset / ((float)screen_width / 4.0);
                                    T->scl[1] = object_Scl[1] + (float)x_offset / ((float)screen_width / 4.0);
                                    T->scl[2] = object_Scl[2] + (float)x_offset / ((float)screen_width / 4.0);
                                }
                            }
                            else if (object_hook == 3) //rotate
                            {
                                if (Axis_lock)
                                {
                                    T->rot[0] = object_Rot[0];
                                    T->rot[1] = object_Rot[1];
                                    T->rot[2] = object_Rot[2];
                                    T->rot[Axis_lock - 1] += (float)x_offset / ((float)screen_width / 4.0);
                                }
                                else
                                {
                                    T->rot[1] = object_Rot[1] + (float)x_offset / ((float)screen_width / 4.0);
                                    T->rot[2] = object_Rot[2] - (float)y_offset / ((float)screen_height / 4.0);
                                }
                            }
                            else // move
                            {
                                if (object_hook)
                                {
                                    direction D1;
                                    D = screen_point_to_vector(mouse_x - SIDEBAR - screen_width / 2, mouse_y - screen_height / 2, screen_width / 2, screen_height / 2, Camera_Left.h_view, Camera_Left.v_view);
                                    D.x = -D.x;
                                    //D.y = -D.y;
                                    D.z = -D.z;
                                    rotate_Vertex_I(Camera_Left.T->rotVec, D.x, D.y, D.z, &D1);
                                    float Dir_vec[3] = {0, 0, 1};
                                    float dot = dot_productN((normal*)&D, Dir_vec);

                                    Pos[0] = Camera_Left.T->pos[0] + D1.x * (ObjDist / dot);
                                    Pos[1] = Camera_Left.T->pos[1] + D1.y * (ObjDist / dot);
                                    Pos[2] = Camera_Left.T->pos[2] + D1.z * (ObjDist / dot);

                                    Delta[0] = Pos[0] - T_pos[0];
                                    Delta[1] = Pos[1] - T_pos[1];
                                    Delta[2] = Pos[2] - T_pos[2];

                                    if (Axis_lock)
                                    {
                                        Magnitude = sqrt(Delta[0] * Delta[0] + Delta[1] * Delta[1] + Delta[2] * Delta[2]);
                                        Delta[0] = 0;
                                        Delta[1] = 0;
                                        Delta[2] = 0;
                                        if (Axis_lock == 2)
                                        {
                                            if (y_offset < 0)
                                                Delta[Axis_lock - 1] = Magnitude;
                                            else
                                                Delta[Axis_lock - 1] = -Magnitude;
                                        }
                                        else
                                        {
                                            if (x_offset > 0)
                                                Delta[Axis_lock - 1] = Magnitude;
                                            else
                                                Delta[Axis_lock - 1] = -Magnitude;
                                        }
                                    }
                                    make_Movement();
                                }
                                else
                                {
                                    Camera_Left.origin_2d[0] = mouse_x;
                                    Camera_Left.origin_2d[1] = mouse_y;
                                    D = set_Target(x_offset, y_offset, screen_width / 2.0, screen_height / 2.0, Camera_Left.h_view, Camera_Left.v_view, CamDist);
                                    if (camera_z_move)
                                    {
                                        CamDist += D.x * 2;
                                    }
                                    else
                                    {
                                        if (VIEWS_FLIPPED)
                                        {
                                            T->target[2] -= D.x * d;
                                            T->target[1] += D.y * d;
                                            T->pos[2] -= D.x * d;
                                            T->pos[1] += D.y * d;
                                        }
                                        else
                                        {
                                            T->target[2] += D.x * d;
                                            T->target[1] += D.y * d;
                                            T->pos[2] += D.x * d;
                                            T->pos[1] += D.y * d;
                                        }
                                    }
                                }
                            }
                        }
                        else if (Camera_screen_lock) // Perspective
                        {
                            x_offset = mouse_x - Camera->origin_2d[0];
                            y_offset = mouse_y - Camera->origin_2d[1];
                            if (object_hook == 2) //scale
                            {
                                if (Axis_lock)
                                {
                                    T->scl[0] = object_Scl[0];
                                    T->scl[1] = object_Scl[1];
                                    T->scl[2] = object_Scl[2];
                                    T->scl[Axis_lock - 1] = object_Scl[Axis_lock - 1] + (float)x_offset / ((float)screen_width / 4.0);
                                }
                                else
                                {
                                    T->scl[0] = object_Scl[0] + (float)x_offset / ((float)screen_width / 4.0);
                                    T->scl[1] = object_Scl[1] + (float)x_offset / ((float)screen_width / 4.0);
                                    T->scl[2] = object_Scl[2] + (float)x_offset / ((float)screen_width / 4.0);
                                }
                            }
                            else if (object_hook == 3) //rotate
                            {
                                if (Axis_lock)
                                {
                                    T->rot[0] = object_Rot[0];
                                    T->rot[1] = object_Rot[1];
                                    T->rot[2] = object_Rot[2];
                                    T->rot[Axis_lock - 1] += (float)x_offset / ((float)screen_width / 4.0);
                                }
                                else
                                {
                                    T->rot[0] = object_Rot[0] + (float)y_offset / ((float)screen_height / 4.0);
                                    T->rot[1] = object_Rot[1] + (float)x_offset / ((float)screen_width / 4.0);
                                }
                            }
                            else // move
                            {
                                if (object_hook)
                                {
                                    if (Camera->ortho)
                                    {
                                        direction D1;
                                        D = screen_point_to_vector(mouse_x - SIDEBAR - screen_width / 2, mouse_y, screen_width / 2, screen_height / 2, Camera->h_view, Camera->v_view);
                                        D.x = -D.x;
                                        //D.y = -D.y;
                                        D.z = -D.z;
                                        rotate_Vertex_I(Camera->T->rotVec, D.x, D.y, D.z, &D1);
                                        float Dir_vec[3] = {0, 0, 1};
                                        float dot = dot_productN((normal*)&D, Dir_vec);

                                        Pos[0] = Camera->T->pos[0] + D1.x * (ObjDist / dot);
                                        Pos[1] = Camera->T->pos[1] + D1.y * (ObjDist / dot);
                                        Pos[2] = Camera->T->pos[2] + D1.z * (ObjDist / dot);

                                        Delta[0] = Pos[0] - T_pos[0];
                                        Delta[1] = Pos[1] - T_pos[1];
                                        Delta[2] = Pos[2] - T_pos[2];

                                        if (Axis_lock)
                                        {
                                            Magnitude = sqrt(Delta[0] * Delta[0] + Delta[1] * Delta[1] + Delta[2] * Delta[2]);
                                            Delta[0] = 0;
                                            Delta[1] = 0;
                                            Delta[2] = 0;
                                            if (Axis_lock == 2)
                                            {
                                                if (y_offset < 0)
                                                    Delta[Axis_lock - 1] = Magnitude;
                                                else
                                                    Delta[Axis_lock - 1] = -Magnitude;
                                            }
                                            else
                                            {
                                                if (x_offset > 0)
                                                    Delta[Axis_lock - 1] = Magnitude;
                                                else
                                                    Delta[Axis_lock - 1] = -Magnitude;
                                            }
                                        }
                                        make_Movement();
                                    }
                                    else
                                    {
                                        direction D1;
                                        D = screen_point_to_vector(mouse_x - SIDEBAR - screen_width / 2, mouse_y, screen_width / 2, screen_height / 2, Camera->h_view, Camera->v_view);
                                        D.x = -D.x;
                                        //D.y = -D.y;
                                        D.z = -D.z;
                                        rotate_Vertex_I(Camera->T->rotVec, D.x, D.y, D.z, &D1);
                                        float Dir_vec[3] = {0, 0, 1};
                                        float dot = dot_productN((normal*)&D, Dir_vec);

                                        Pos[0] = Camera->T->pos[0] + D1.x * (ObjDist / dot);
                                        Pos[1] = Camera->T->pos[1] + D1.y * (ObjDist / dot);
                                        Pos[2] = Camera->T->pos[2] + D1.z * (ObjDist / dot);

                                        Delta[0] = Pos[0] - T_pos[0];
                                        Delta[1] = Pos[1] - T_pos[1];
                                        Delta[2] = Pos[2] - T_pos[2];

                                        if (Axis_lock)
                                        {
                                            Magnitude = sqrt(Delta[0] * Delta[0] + Delta[1] * Delta[1] + Delta[2] * Delta[2]);
                                            Delta[0] = 0;
                                            Delta[1] = 0;
                                            Delta[2] = 0;
                                            if (Axis_lock == 2)
                                            {
                                                if (y_offset < 0)
                                                    Delta[Axis_lock - 1] = Magnitude;
                                                else
                                                    Delta[Axis_lock - 1] = -Magnitude;
                                            }
                                            else
                                            {
                                                if (x_offset > 0)
                                                    Delta[Axis_lock - 1] = Magnitude;
                                                else
                                                    Delta[Axis_lock - 1] = -Magnitude;
                                            }
                                        }
                                        make_Movement();
                                    }
                                }
                                else
                                {

                                    Camera->origin_2d[0] = mouse_x;
                                    Camera->origin_2d[1] = mouse_y;
                                    D = set_Target(x_offset, y_offset, screen_width / 2.0, screen_height / 2.0, Camera->h_view, Camera->v_view, CamDist);
                                    if (camera_z_move)
                                    {
                                        CamDist += D.x * 2;
                                    }
                                    else if (camera_rotate)
                                    {
                                        Camera->T->rot[0] += D.y / (CamDist / 10 + 0.1);
                                        Camera->T->rot[1] -= D.x / (CamDist / 10 + 0.1);
                                        // to make it work, rotVec_ needs to be updated in
                                        // update_camera
                                        // also rotation matrix needs to decompose in order
                                        // into rot variable
                                    }
                                    else
                                    {
                                        direction D1, D0;
                                        float rotVec_I[3][3]; // inverse is constructed on the fly
                                        invert_Rotation(Camera->T, rotVec_I);
                                        rotate_Vertex_I(rotVec_I, T->pos[0], T->pos[1], T->pos[2], &D1);

                                        D1.x += D.x * d;
                                        D1.y += D.y * d;

                                        rotate_Vector(Camera->T, D1.x, D1.y, D1.z, &D0);

                                        T->pos[0] = D0.x;
                                        T->pos[1] = D0.y;
                                        T->pos[2] = D0.z;

                                        rotate_Vertex_I(rotVec_I, T->target[0], T->target[1], T->target[2], &D1);

                                        D1.x += D.x * d;
                                        D1.y += D.y * d;

                                        rotate_Vector(Camera->T, D1.x, D1.y, D1.z, &D0);

                                        T->target[0] = D0.x;
                                        T->target[1] = D0.y;
                                        T->target[2] = D0.z;
                                    }
                                }
                            }
                        }
                    }
                    else
                    {
                        x_offset = mouse_x - Camera->origin_2d[0];
                        y_offset = mouse_y - Camera->origin_2d[1];
                        if (object_hook == 2) //scale
                        {
                            if (Axis_lock)
                            {
                                T->scl[0] = object_Scl[0];
                                T->scl[1] = object_Scl[1];
                                T->scl[2] = object_Scl[2];
                                T->scl[Axis_lock - 1] = object_Scl[Axis_lock - 1] + (float)x_offset / ((float)screen_width / 4.0);
                            }
                            else
                            {
                                T->scl[0] = object_Scl[0] + ((float)x_offset / ((float)screen_width / 2.0));
                                T->scl[1] = object_Scl[1] + ((float)x_offset / ((float)screen_width / 2.0));
                                T->scl[2] = object_Scl[2] + ((float)x_offset / ((float)screen_width / 2.0));
                            }
                        }
                        else if (object_hook == 3) //rotate
                        {
                            if (Axis_lock)
                            {
                                T->rot[0] = object_Rot[0];
                                T->rot[1] = object_Rot[1];
                                T->rot[2] = object_Rot[2];
                                T->rot[Axis_lock - 1] += (float)x_offset / ((float)screen_width / 4.0);
                            }
                            else
                            {
                                T->rot[0] = object_Rot[0] + (float)y_offset / ((float)screen_height / 2.0);
                                T->rot[1] = object_Rot[1] + (float)x_offset / ((float)screen_width / 2.0);
                            }
                        }
                        else // move
                        {
                            if (object_hook)
                            {
                                if (Camera->ortho)
                                {
                                    direction D1;
                                    direction D = screen_point_to_vector(mouse_x - SIDEBAR, mouse_y, screen_width, screen_height, Camera->h_view, Camera->v_view);
                                    D.x = -D.x;
                                    //D.y = -D.y;
                                    D.z = -D.z;
                                    rotate_Vertex_I(Camera->T->rotVec, D.x, D.y, D.z, &D1);
                                    float Dir_vec[3] = {0, 0, 1};
                                    float dot = dot_productN((normal*)&D, Dir_vec);

                                    Pos[0] = Camera->T->pos[0] + D1.x * (ObjDist / dot);
                                    Pos[1] = Camera->T->pos[1] + D1.y * (ObjDist / dot);
                                    Pos[2] = Camera->T->pos[2] + D1.z * (ObjDist / dot);

                                    Delta[0] = Pos[0] - T_pos[0];
                                    Delta[1] = Pos[1] - T_pos[1];
                                    Delta[2] = Pos[2] - T_pos[2];

                                    if (Axis_lock)
                                    {
                                        Magnitude = sqrt(Delta[0] * Delta[0] + Delta[1] * Delta[1] + Delta[2] * Delta[2]);
                                        Delta[0] = 0;
                                        Delta[1] = 0;
                                        Delta[2] = 0;
                                        if (Axis_lock == 2)
                                        {
                                            if (y_offset < 0)
                                                Delta[Axis_lock - 1] = Magnitude;
                                            else
                                                Delta[Axis_lock - 1] = -Magnitude;
                                        }
                                        else
                                        {
                                            if (x_offset > 0)
                                                Delta[Axis_lock - 1] = Magnitude;
                                            else
                                                Delta[Axis_lock - 1] = -Magnitude;
                                        }
                                    }
                                    make_Movement();
                                }
                                else
                                {
                                    direction D1;
                                    direction D = screen_point_to_vector(mouse_x - SIDEBAR, mouse_y, screen_width, screen_height, Camera->h_view, Camera->v_view);
                                    D.x = -D.x;
                                    //D.y = -D.y;
                                    D.z = -D.z;
                                    rotate_Vertex_I(Camera->T->rotVec, D.x, D.y, D.z, &D1);
                                    float Dir_vec[3] = {0, 0, 1};
                                    float dot = dot_productN((normal*)&D, Dir_vec);

                                    Pos[0] = Camera->T->pos[0] + D1.x * (ObjDist / dot);
                                    Pos[1] = Camera->T->pos[1] + D1.y * (ObjDist / dot);
                                    Pos[2] = Camera->T->pos[2] + D1.z * (ObjDist / dot);

                                    Delta[0] = Pos[0] - T_pos[0];
                                    Delta[1] = Pos[1] - T_pos[1];
                                    Delta[2] = Pos[2] - T_pos[2];

                                    if (Axis_lock)
                                    {
                                        Magnitude = sqrt(Delta[0] * Delta[0] + Delta[1] * Delta[1] + Delta[2] * Delta[2]);
                                        Delta[0] = 0;
                                        Delta[1] = 0;
                                        Delta[2] = 0;
                                        if (Axis_lock == 2)
                                        {
                                            if (y_offset < 0)
                                                Delta[Axis_lock - 1] = Magnitude;
                                            else
                                                Delta[Axis_lock - 1] = -Magnitude;
                                        }
                                        else
                                        {
                                            if (x_offset > 0)
                                                Delta[Axis_lock - 1] = Magnitude;
                                            else
                                                Delta[Axis_lock - 1] = -Magnitude;
                                        }
                                    }
                                    make_Movement();
                                }
                            }
                            else
                            {
                                Camera->origin_2d[0] = mouse_x;
                                Camera->origin_2d[1] = mouse_y;
                                D = set_Target(x_offset, y_offset, screen_width, screen_height, Camera->h_view, Camera->v_view, CamDist);
                                if (camera_z_move)
                                {
                                    CamDist += D.x * 2;
                                }
                                else if (camera_rotate)
                                {
                                    Camera->T->rot[0] += D.y / (CamDist / 10 + 0.1);;
                                    Camera->T->rot[1] -= D.x / (CamDist / 10 + 0.1);;
                                    // to make it work, rotVec_ needs to be updated in
                                    // update_camera
                                    // also rotation matrix needs to decompose in order
                                    // into rot variable
                                }
                                else
                                {
                                    direction D1, D0;
                                    float rotVec_I[3][3]; // inverse is constructed on the fly
                                    invert_Rotation(Camera->T, rotVec_I);
                                    rotate_Vertex_I(rotVec_I, T->pos[0], T->pos[1], T->pos[2], &D1);

                                    D1.x += D.x * d;
                                    D1.y += D.y * d;

                                    rotate_Vector(Camera->T, D1.x, D1.y, D1.z, &D0);

                                    T->pos[0] = D0.x;
                                    T->pos[1] = D0.y;
                                    T->pos[2] = D0.z;

                                    rotate_Vertex_I(rotVec_I, T->target[0], T->target[1], T->target[2], &D1);

                                    D1.x += D.x * d;
                                    D1.y += D.y * d;

                                    rotate_Vector(Camera->T, D1.x, D1.y, D1.z, &D0);

                                    T->target[0] = D0.x;
                                    T->target[1] = D0.y;
                                    T->target[2] = D0.z;
                                }
                            }
                        }
                    }
                    message = -1;
                }
                else if (drag_rectangle)
                {
                    if (!NVIDIA) glDrawBuffer(GL_FRONT_AND_BACK);
                    UPDATEDRAG = 1;
                    if (subdLevel > -1)
                        poly_Render(tripsRender, wireframe, splitview, CamDist, 0, 0);
                    else
                        poly_Render(tripsRender, wireframe, splitview, CamDist, 0, subdLevel);
                    UPDATEDRAG = 0;
                    if (mouse_x < SIDEBAR)
                        mouse_x = SIDEBAR;
                    else if (mouse_x > SIDEBAR + screen_width)
                        mouse_x = SIDEBAR + screen_width;
                    if (mouse_y > screen_height)
                        mouse_y = screen_height;
                    Drag_Rectangle.x1 = mouse_x;
                    Drag_Rectangle.y1 = mouse_y;
                    if (mouse_x < Drag_Rectangle.x0)
                    {
                        Drag_Rectangle.x = mouse_x;
                        Drag_Rectangle.w = Drag_Rectangle.x0 - Drag_Rectangle.x1;
                        cull_Selection = 1;
                    }
                    else
                    {
                        Drag_Rectangle.x = Drag_Rectangle.x0;
                        Drag_Rectangle.w = Drag_Rectangle.x1 - Drag_Rectangle.x0;
                        cull_Selection = 0;
                    }
                    handle_ControlDown();
                    if (mouse_y < Drag_Rectangle.y0)
                    {
                        Drag_Rectangle.y = screen_height + BOTTOM_LINE - Drag_Rectangle.y0;
                        Drag_Rectangle.h = Drag_Rectangle.y0 - Drag_Rectangle.y1;
                    }
                    else
                    {
                        Drag_Rectangle.y = screen_height + BOTTOM_LINE - mouse_y;
                        Drag_Rectangle.h = Drag_Rectangle.y1 - Drag_Rectangle.y0;
                    }
                    if (subdLevel > -1)
                        poly_Render(tripsRender, wireframe, splitview, CamDist, 1, 0);
                    else
                        poly_Render(tripsRender, wireframe, splitview, CamDist, 1, subdLevel);
                    glDrawBuffer(GL_BACK);
                    message = 0;
                }
                else if (mouse_button_down && mouse_x > SIDEBAR && mouse_y < screen_height)
                {
                    rendermode = ID_RENDER;

                    ELEMENT_ARRAYS = 0;
                    init_Hint();

                    int level, idx;

                    if (subdLevel > -1)
                    {
                        level = subdLevel; // because large polygon counts
                        if (LOCAT_ID_RENDER && Object_Mode)
                        {

                        }
                        else if (Object_Mode)
                        {
                            load_id_colors_all(Camera, level, OBJECT_ID_COLORS);
                        }
                        else if (Polygon_Mode)
                        {
                            Camera->objects[0] = currentObject;
                            Camera->object_count = 1;
                            load_id_colors_current(O, level, POLYGON_ID_COLORS);
                        }
                        else if (Edge_Mode)
                        {
                            Camera->objects[0] = currentObject;
                            Camera->object_count = 1;
                        }
                        else if (Vertex_Mode)
                        {
                            Camera->objects[0] = currentObject;
                            Camera->object_count = 1;
                        }
                    }
                    else
                    {
                        level = -1;
                        if (LOCAT_ID_RENDER && Object_Mode)
                        {

                        }
                        else if (Object_Mode)
                        {
                            load_id_colors_Fan_all(Camera, OBJECT_ID_COLORS);
                        }
                        else if (Polygon_Mode)
                        {
                            Camera->objects[0] = currentObject;
                            Camera->object_count = 1;
                            load_id_colors_Fan_current(O, POLYGON_ID_COLORS);
                        }
                        else if (Edge_Mode)
                        {
                            Camera->objects[0] = currentObject;
                            Camera->object_count = 1;
                        }
                        else if (Vertex_Mode)
                        {
                            Camera->objects[0] = currentObject;
                            Camera->object_count = 1;
                        }
                    }
                    UPDATE_COLORS = 1;
                    DRAW_UI = 0;
                    if (LOCAT_ID_RENDER && Object_Mode)
                    {
                        poly_Render(0, 0, splitview, CamDist, 0, level);
                    }
                    else if (Vertex_Mode)
                    {
                        VERTS_ID_RENDER = 1;
                        poly_Render(0, 0, splitview, CamDist, 0, level);
                        VERTS_ID_RENDER = 0;
                    }
                    else if (Edge_Mode)
                    {
                        EDGES_ID_RENDER = 1;
                        poly_Render(0, 0, splitview, CamDist, 0, level);
                        EDGES_ID_RENDER = 0;
                    }
                    else
                    {
                        POLYS_ID_RENDER = 1;
                        poly_Render(0, 0, splitview, CamDist, 0, level);
                        POLYS_ID_RENDER = 0;
                    }

                    //SDL_Delay(delay_ms);

                    GLubyte pixel[4 * 81];
                    glReadPixels(mouse_x - 4, screen_height + BOTTOM_LINE - mouse_y - 4, 9, 9, GL_RGBA, GL_UNSIGNED_BYTE, pixel);
                    //printf("%u %u %u %u\n", pixel[0], pixel[1], pixel[2], pixel[3]);
                    rendermode = POLY_RENDER;

                    int u, p, o;

                    for (p = 0; p < 81; p ++)
                    {
                        pixel_id[p] = (GLubyte)pixel[0 + p * 4] * 255 * 255 * 255 + (GLubyte)pixel[1 + p * 4] * 255 * 255 + (GLubyte)pixel[2 + p * 4] * 255 + (GLubyte)pixel[3 + p * 4];
                    }

                    for (p = 0; p < 81; p ++)
                    {
                        o = pixel_id[p_index[p]];
                        if (Object_Mode && LOCAT_ID_RENDER && o < transformerIndex && o >= 0)
                        {
                            T = transformers[o];
                            if (add_selection_mode)
                            {
                                T->selected = 1;
                                currentLocator = o;
                                //LocatorSize = T->LocatorSize;
                                select_Transformer_Bone(T);
                                select_Transformer_IK(T);
                                select_Deformer();
                            }
                            else
                            {
                                T->selected = 0;
                            }
                            break;
                        }
                        else if (Object_Mode && o < objectIndex && o >= 0)
                        {
                            sels_start[current_sel_type] = 0;
                            O = objects[o];
                            currentObject = o;
                            if (add_selection_mode)
                            {
                                if (!O->selected)
                                {
                                    O->selected = 1;
                                    selected_objects[selected_object_count ++] = o;
                                }
                            }
                            else
                            {
                                if (O->selected && selected_object_count > 1)
                                {
                                    O->selected = 0;
                                    int s_o[OBJECTS];
                                    int s_o_count = 0;
                                    int s;
                                    for (s = 0; s < selected_object_count; s ++)
                                    {
                                        if (selected_objects[s] != o)
                                        {
                                            s_o[s_o_count ++] = selected_objects[s];
                                        }
                                    }
                                    selected_object_count --;
                                    for (s = 0; s < selected_object_count; s ++)
                                    {
                                        selected_objects[s] = s_o[s];
                                    }
                                    if (selected_object_count - 1 >= 0)
                                    {
                                        currentObject = selected_objects[selected_object_count - 1];
                                        O = objects[currentObject];
                                    }
                                }
                            }
                            break;
                            //printf("%s\n", objects[currentObject].Name);
                        }
                        else if (Polygon_Mode && o < objects[currentObject]->polycount && o >= 0)
                        {
                            polygon * P = &O->polys[o / ARRAYSIZE][o % ARRAYSIZE];
                            if (add_selection_mode)
                            {
                                if (P->selected == 0)
                                {
                                    assignSelectionToQuads(O, P, 1);
                                    O->last_selected_polys[0] = O->last_selected_polys[1];
                                    O->last_selected_polys[1] = P->index;
                                }
                            }
                            else
                            {
                                if (P->selected == 1)
                                {
                                    assignSelectionToQuads(O, P, 0);
                                }
                            }
                            load_id_colors_polygon(O, P, OBJECT_COLORS);
                            break;
                        }
                        else if (Edge_Mode)
                        {
                            uv_edge * UVE;
                            edge * E;
                            if (Camera->uv_draw)
                            {
                                if (o > -1 && o < O->uvedcount)
                                {
                                    UVE = &O->uveds[o / ARRAYSIZE][o % ARRAYSIZE];
                                    if (add_selection_mode)
                                    {
                                        UVE->selected = 1;
                                    }
                                    else
                                    {
                                        UVE->selected = 0;
                                    }
                                    idx = UVE->edge;
                                    if (idx > -1 && idx < O->edgecount)
                                    {
                                        E = &O->edges[idx / ARRAYSIZE][idx % ARRAYSIZE];
                                        E->selected = UVE->selected;
                                    }
                                    break;
                                }
                            }
                            else
                            {
                                if (o > -1 && o < O->edgecount)
                                {
                                    E = &O->edges[o / ARRAYSIZE][o % ARRAYSIZE];
                                    if (add_selection_mode)
                                    {
                                        E->selected = 1;
                                        O->last_selected_edges[0] = O->last_selected_edges[1];
                                        O->last_selected_edges[1] = E->index;
                                    }
                                    else
                                    {
                                        E->selected = 0;
                                    }
                                    for (u = 0; u < E->uv_edcount; u ++)
                                    {
                                        idx = E->uv_edges[u];
                                        if (idx > -1 && idx < O->uvedcount)
                                        {
                                            UVE = &O->uveds[idx / ARRAYSIZE][idx % ARRAYSIZE];
                                            UVE->selected = E->selected;
                                        }
                                    }
                                    break;
                                }
                            }
                        }
                        else if (Vertex_Mode)
                        {
                            uv * UV;
                            vertex * V;
                            if (Camera->uv_draw)
                            {
                                if (o > -1 && o < O->textcount)
                                {
                                    UV = &O->uvtex[o / ARRAYSIZE][o % ARRAYSIZE];
                                    if (add_selection_mode)
                                    {
                                        UV->selected = 1;
                                    }
                                    else
                                    {
                                        UV->selected = 0;
                                    }
                                    idx = UV->vert;
                                    if (idx > -1 && idx < O->vertcount)
                                    {
                                        V = &O->verts[idx / ARRAYSIZE][idx % ARRAYSIZE];
                                        V->selected = UV->selected;
                                    }
                                    break;
                                }
                            }
                            else
                            {
                                if (o > -1 && o < O->vertcount)
                                {
                                    V = &O->verts[o / ARRAYSIZE][o % ARRAYSIZE];
                                    if (add_selection_mode)
                                    {
                                        V->selected = 1;
                                        O->last_selected_verts[0] = O->last_selected_verts[1];
                                        O->last_selected_verts[1] = V->index;
                                    }
                                    else
                                    {
                                        V->selected = 0;
                                    }
                                    for (u = 0; u < V->uv_vertcount; u ++)
                                    {
                                        idx = V->uv_verts[u];
                                        if (idx > -1 && idx < O->textcount)
                                        {
                                            UV = &O->uvtex[idx / ARRAYSIZE][idx % ARRAYSIZE];
                                            UV->selected = V->selected;
                                        }
                                    }
                                    break;
                                }
                            }
                        }
                    }

                    if (Object_Mode && LOCAT_ID_RENDER)
                    {

                    }
                    else if (Object_Mode || Polygon_Mode)
                    {
                        if (subdLevel > -1)
                        load_id_colors_No_Surface(Camera, level);
                        else
                        load_id_colors_Fan_all(Camera, OBJECT_COLORS);
                    }

                    DRAW_UI = 1;

                    SDL_GL_SwapBuffers();

                    message = 0;
                }
                else
                {
                    message = 0;
                }

            }
            else if (event.type == SDL_QUIT)
            {
                quit = 1;
            }
        }

        if (message == 1)
        {
            if (dialog_lock)
            {
                UPDATE_BACKGROUND = 1;
                if (dialog_type == TEXT_DIALOG)
                {
                    if (strcmp(text, TEXT_TYPE_TEXT) == 0)
                    {
                        TextIndex = Materials[O->surface].Texture_idx;
                        TextIndex --;
                        if (TextIndex < 0) TextIndex = 0;
                        if (TextIndex < Textures_c && TextIndex >= 0)
                        {
                            Materials[O->surface].Texture_idx = TextIndex;
                            Materials[O->surface].texture = TextIndex;
                            DRAW_UI = 0;
                            open_Textures_List();
                            DRAW_UI = 1;
                        }
                    }
                    else if (strcmp(text, TEXT_TYPE_NORM) == 0)
                    {
                        TextIndex = Materials[O->surface].Normal_idx;
                        TextIndex --;
                        if (TextIndex < 0) TextIndex = 0;
                        if (TextIndex < Normals_c && TextIndex >= 0)
                        {
                            Materials[O->surface].Normal_idx = TextIndex;
                            Materials[O->surface].normal = TextIndex;
                            DRAW_UI = 0;
                            open_Textures_List();
                            DRAW_UI = 1;
                        }
                    }
                    else if (strcmp(text, TEXT_TYPE_BUMP) == 0)
                    {
                        TextIndex = Materials[O->surface].Bump_idx;
                        TextIndex --;
                        if (TextIndex < 0) TextIndex = 0;
                        if (TextIndex < Bumps_c && TextIndex >= 0)
                        {
                            Materials[O->surface].Bump_idx = TextIndex;
                            Materials[O->surface].bump = TextIndex;
                            DRAW_UI = 0;
                            open_Textures_List();
                            DRAW_UI = 1;
                        }
                    }
                }
                else if (dialog_type == ITEM_DIALOG)
                {
                    if (strcmp(item, ITEM_TYPE_OBJECT) == 0)
                    {
                        if (currentObject - items_start >= 0 && currentObject - items_start < LISTLENGTH)
                        {
                            ItemList[currentObject - items_start].color = UI_BLACK;
                        }
                        currentObject --;
                        if (currentObject < 0)
                            currentObject = 0;
                        if (currentObject < objectIndex && currentObject >= 0)
                        {
                            O = objects[currentObject];
                            DRAW_UI = 0;

                            if (!NVIDIA) glDrawBuffer(GL_FRONT_AND_BACK);

                            poly_Render(tripsRender, wireframe, splitview, CamDist, 0, subdLevel);
                            if (currentObject - items_start >= 0 && currentObject - items_start < LISTLENGTH)
                            {
                                ItemList[currentObject - items_start].color = UI_BACKL;
                                draw_Items_Dialog("Items List", screen_height, item, items, items_count, items_start, 1, currentObject);
                                ItemList[currentObject - items_start].color = UI_BLACK;
                            }
                            else
                            {
                                draw_Items_Dialog("Items List", screen_height, item, items, items_count, items_start, 1, currentObject);
                            }
                            SDL_GL_SwapBuffers();
                            glDrawBuffer(GL_BACK);
                            DRAW_UI = 1;
                        }
                    }
                }
                else
                {
                    handle_UP(0);
                }
            }
            else if (mod & KMOD_CTRL)
            {
                Camera->T->rot[0] += pi30;
            }
            else if (mod & KMOD_SHIFT)
            {
                O->T->pos[1] += 0.5;
            }
            else if (DRAW_LOCATORS)
            {
                if (!Camera_screen_lock)
                    select_Parent_Locator();
            }
            else
            {
                O->T->rot[0] -= pi30;
            }
            message = -1;
        }
        else if (message == 2)
        {
            if (dialog_lock)
            {
                if (dialog_type == TEXT_DIALOG)
                {
                    if (strcmp(text, TEXT_TYPE_TEXT) == 0)
                    {
                        TextIndex = Materials[O->surface].Texture_idx;
                        TextIndex ++;
                        if (TextIndex >= Textures_c) TextIndex = Textures_c - 1;
                        if (TextIndex < Textures_c && TextIndex >= 0)
                        {
                            Materials[O->surface].Texture_idx = TextIndex;
                            Materials[O->surface].texture = TextIndex;
                            DRAW_UI = 0;
                            open_Textures_List();
                            DRAW_UI = 1;
                        }
                    }
                    else if (strcmp(text, TEXT_TYPE_NORM) == 0)
                    {
                        TextIndex = Materials[O->surface].Normal_idx;
                        TextIndex ++;
                        if (TextIndex >= Normals_c) TextIndex = Normals_c - 1;
                        if (TextIndex < Normals_c && TextIndex >= 0)
                        {
                            Materials[O->surface].Normal_idx = TextIndex;
                            Materials[O->surface].normal = TextIndex;
                            DRAW_UI = 0;
                            open_Textures_List();
                            DRAW_UI = 1;
                        }
                    }
                    else if (strcmp(text, TEXT_TYPE_BUMP) == 0)
                    {
                        TextIndex = Materials[O->surface].Bump_idx;
                        TextIndex ++;
                        if (TextIndex >= Bumps_c) TextIndex = Bumps_c - 1;
                        if (TextIndex < Bumps_c && TextIndex >= 0)
                        {
                            Materials[O->surface].Bump_idx = TextIndex;
                            Materials[O->surface].bump = TextIndex;
                            DRAW_UI = 0;
                            open_Textures_List();
                            DRAW_UI = 1;
                        }
                    }
                }
                else if (dialog_type == ITEM_DIALOG)
                {
                    if (strcmp(item, ITEM_TYPE_OBJECT) == 0)
                    {
                        if (currentObject - items_start >= 0 && currentObject - items_start < LISTLENGTH)
                        {
                            ItemList[currentObject - items_start].color = UI_BLACK;
                        }
                        currentObject ++;
                        if (currentObject >= objectIndex) currentObject = objectIndex - 1;
                        if (currentObject < objectIndex && currentObject >= 0)
                        {
                            O = objects[currentObject];
                            DRAW_UI = 0;

                            if (!NVIDIA) glDrawBuffer(GL_FRONT_AND_BACK);

                            poly_Render(tripsRender, wireframe, splitview, CamDist, 0, subdLevel);
                            if (currentObject - items_start >= 0 && currentObject - items_start < LISTLENGTH)
                            {
                                ItemList[currentObject - items_start].color = UI_BACKL;
                                draw_Items_Dialog("Items List", screen_height, item, items, items_count, items_start, 1, currentObject);
                                ItemList[currentObject - items_start].color = UI_BLACK;
                            }
                            else
                            {
                                draw_Items_Dialog("Items List", screen_height, item, items, items_count, items_start, 1, currentObject);
                            }
                            SDL_GL_SwapBuffers();
                            glDrawBuffer(GL_BACK);
                            DRAW_UI = 1;
                        }
                    }
                }
                else
                {
                    handle_DOWN(0);
                }
            }
            else if (mod & KMOD_CTRL)
            {
                Camera->T->rot[0] -= pi30;
            }
            else if (mod & KMOD_SHIFT)
            {
                O->T->pos[1] -= 0.5;
            }
            else if (DRAW_LOCATORS)
            {
                if (!Camera_screen_lock)
                    select_Child_Locator();
            }
            else
            {
                O->T->rot[0] += pi30;
            }
            message = -1;
        }
        else if (message == 3)
        {
            if (mod & KMOD_CTRL)
                Camera->T->rot[1] += pi30;
            else if (mod & KMOD_SHIFT)
                O->T->pos[0] -= 0.5;
            else
                O->T->rot[1] -= pi30;
            message = -1;
        }
        else if (message == 4)
        {
            if (mod & KMOD_CTRL)
                Camera->T->rot[1] -= pi30;
            else if (mod & KMOD_SHIFT)
                O->T->pos[0] += 0.5;
            else
                O->T->rot[1] += pi30;
            message = -1;
        }
        else if (message == 5)
        {
            if (mod & KMOD_CTRL)
                Camera->T->rot[2] += pi30;
            else if (mod & KMOD_SHIFT)
                O->T->pos[2] -= 0.5;
            else
                O->T->rot[2] += pi30;
            message = -1;
        }
        else if (message == 6)
        {
            if (mod & KMOD_CTRL)
                Camera->T->rot[2] -= pi30;
            else if (mod & KMOD_SHIFT)
                O->T->pos[2] += 0.5;
            else if (DRAW_LOCATORS)
            {
                if (T->Bone != NULL)
                    delete_Bone(T->Bone);
                else
                    delete_Locator();
            }
            else
                delete_Object(currentObject, 1);

            message = -1;
        }
        else if (message == 7)
        {
            CamDist += 1.0;
            move_Back(Camera->T, CamDist);
            message = -1;
        }
        else if (message == 8)
        {
            CamDist -= 1.0;
            move_Back(Camera->T, CamDist);
            message = -1;
        }
        else if (message == 9)
        {
            subdLevel ++;
            if (subdLevel > SUBD - 1) subdLevel = SUBD - 1;
            int o;
            for (o = 0; o < objectIndex; o ++)
                subdivide_after_Creation(objects[o], subdLevel, tune);
            all_objects_in_frame(Camera);
            load_id_colors(selected_objects, selected_object_count, subdLevel, OBJECT_COLORS);

            object * O0;

            for (o = 0; o < objectIndex; o ++)
            {
                O0 = objects[o];
                if (BIND_POSE || !O0->binding)
                    rotate_verts_(O0, *O0->T, subdLevel);
                else
                    tune_subdivide_post_transformed(O0, subdLevel);
                update_tangents_Object(O0, subdLevel);
            }

            printf("level quads %d %d\n", subdLevel, O->quadcount_[subdLevel]);
            UPDATE_COLORS = 1;
            message = -10;
        }
        else if (message == 10)
        {
            subdLevel --;
            if (subdLevel < -1)
            {
                subdLevel = -1;
                printf("level polys %d %d\n", subdLevel, O->polycount);
            }
            else
            {
                printf("level quads %d %d\n", subdLevel, O->quadcount_[subdLevel]);
            }
            UPDATE_COLORS = 1;
            message = -10;
        }
        else if (message == 11)
        {
            if (mod & KMOD_SHIFT)
            {
                modify_Texture_Seams(O);
            }
            else
            {
                int m;
                for (m = 0; m < Materials_count; m ++)
                Materials[m].smooth = !Materials[m].smooth;
            }
            message = -1;
        }
        else if (message == 12)
        {
            if (mod & KMOD_SHIFT)
            {
                int t = Materials[O->surface].Texture_idx;
                t ++;
                if (t >= Textures_c) t = 0;
                Materials[O->surface].Texture_idx = t;

                t = Materials[O->surface].texture;
                t ++;
                if (t >= Surf_Text_c) t = 0;
                Materials[O->surface].texture = t;
            }
            else if (mod & KMOD_CTRL)
            {
                int t = Materials[O->surface].Texture_idx;
                t --;
                if (t < 0) t = Textures_c - 1;
                Materials[O->surface].Texture_idx = t;

                t = Materials[O->surface].texture;
                t --;
                if (t < 0) t = Surf_Text_c - 1;
                Materials[O->surface].texture = t;
            }
            else
            {
                int m;
                for (m = 0; m < Materials_count; m ++)
                Materials[m].use_texture = !Materials[m].use_texture;
            }
            message = -10;
        }
        else if (message == 13)
        {
            quit = 1;
            message = 0;
        }
        else if (message == 14)
        {
            SDL_GetMouseState(&mouse_x, &mouse_y);
            Camera = find_View(mouse_x, mouse_y, splitview);

            if (mod & KMOD_CTRL)
            {
                reset_View(Camera, CamDist, ortho_on);
                message = -10;
            }
            else if (mod & KMOD_ALT)
            {
                SCALE = 1;

                if (DRAW_LOCATORS)
                {
                    T = transformers[currentLocator];
                }
                else
                {
                    T = objects[currentObject]->T;
                }

                bake_scale(T);

                T->scl[0] = 1.0;
                T->scl[1] = 1.0;
                T->scl[2] = 1.0;

                message = -10;

                transform_Objects_And_Render();
                SCALE = 0;
            }
            else if (mod & KMOD_SHIFT)
            {
                ROTATION = 1;

                if (DRAW_LOCATORS)
                {
                    T = transformers[currentLocator];
                }
                else
                {
                    T = objects[currentObject]->T;
                }

                bake(T);

                T->rot[0] = 0.0;
                T->rot[1] = 0.0;
                T->rot[2] = 0.0;

                message = -10;

                transform_Objects_And_Render();
                ROTATION = 0;
            }
            else
            {
                if (DRAW_LOCATORS)
                {
                    T = transformers[currentLocator];
                }
                else
                {
                    T = objects[currentObject]->T;
                }
                if (T->Bone == NULL && T->Deformer == NULL)
                {
                    T->pos[0] = 0.0;
                    T->pos[1] = 0.0;
                    T->pos[2] = 0.0;

                    if (BIND_POSE)
                    {
                        rotate_bind(T);
                    }
                    else
                    {
                        bake(T);
                        rotate(T);
                    }
                }
                else if (T->Deformer != NULL)
                {
                    deformer * D = T->Deformer;

                    T = D->Transformers[0];

                    //paste_Pose_(D, D->Poses[0]);

                    Delta[0] = D->Poses[0]->TP[0].pos[0] - T->pos[0];
                    Delta[1] = D->Poses[0]->TP[0].pos[1] - T->pos[1];
                    Delta[2] = D->Poses[0]->TP[0].pos[2] - T->pos[2];

                    move_Pose_T(T, Delta);

                    Update_Objects_Count = 0;

                    rotate_collect(T);
                    rotate_vertex_groups_D_Init();

                    rotate_Deformer_verts(D);

//                    bake(T);
//
//                    rotate(T);

                    update_rotate_bounding_box();

                    if (subdLevel > -1)
                    {
                        int o;
                        object * O0;

                        for (o = 0; o < Update_Objects_Count; o ++)
                        {
                            O0 = Update_Objects[o];
                            if (O0->deforms)
                            {
                                tune_subdivide_post_transformed(O0, subdLevel);
                            }
                        }
                    }
                }
            }
        }
        else if (message == 15)
        {
            if (BIND_POSE)
            {
                parent_Locators();
            }
            else
            {
                add_Pose();
            }
            message = -1;
        }
        else if (message == 16)
        {
            wireframe = !wireframe;
            message = -10;
        }
        else if (message == 17)
        {
            if (mod & KMOD_ALT)
            {
                float amount = 0;
                if (O->mean_Edge > 0.1)
                {
                    amount = 0.1;
                }
                else if (O->mean_Edge > 1)
                {
                    amount = 1;
                }
                else if (O->mean_Edge > 10)
                {
                    amount = 10;
                }
                else
                {
                    amount = 0.01;
                }
                Materials[O->surface].Displacement += amount;
                printf("%f\n", Materials[O->surface].Displacement);
            }
            else
            {
                Materials[O->surface].Shininess ++;
                printf("%f\n", Materials[O->surface].Shininess);
            }

            message = -1;
        }
        else if (message == 18)
        {
            if (mod & KMOD_ALT)
            {
                float amount = 0;
                if (O->mean_Edge > 0.1)
                {
                    amount = 0.1;
                }
                else if (O->mean_Edge > 1)
                {
                    amount = 1;
                }
                else if (O->mean_Edge > 10)
                {
                    amount = 10;
                }
                else
                {
                    amount = 0.01;
                }
                Materials[O->surface].Displacement -= amount;
                printf("%f\n", Materials[O->surface].Displacement);
            }
            else
            {
                Materials[O->surface].Shininess --;
                printf("%f\n", Materials[O->surface].Shininess);
            }
            message = -1;
        }
        else if (message == 19)
        {
            cage_v -= 0.1;
            printf("cage_v %f\n", cage_v);
            tune_subdivide_post_Objects();
            message = -1;
        }
        else if (message == 20)
        {
            cage_v += 0.1;
            printf("cage_v %f\n", cage_v);
            tune_subdivide_post_Objects();
            message = -1;
        }
        else if (message == 21)
        {
            edge_c -= 0.1;
            printf("edge_c %f\n", edge_c);
            tune_subdivide_post_Objects();
            message = -1;
        }
        else if (message == 22)
        {
            edge_c += 0.1;
            printf("edge_c %f\n", edge_c);
            tune_subdivide_post_Objects();
            message = -1;
        }
        else if (message == 23)
        {
            if (mod & KMOD_CTRL)
            {
                open_ObjectsDir();
            }
            else if (mod & KMOD_SHIFT)
            {
                ortho_on = !ortho_on;
                Camera_Persp.ortho = ortho_on;
                Camera_Top.ortho = ortho_on;
                Camera_Left.ortho = ortho_on;
                Camera_Front.ortho = ortho_on;
                update_camera_view(&Camera_Persp, Camera->h_view, screen_width, screen_height);
                update_camera_view(&Camera_Top, Camera->h_view, screen_width, screen_height);
                update_camera_view(&Camera_Left, Camera->h_view, screen_width, screen_height);
                update_camera_view(&Camera_Front, Camera->h_view, screen_width, screen_height);
                message = -1;
            }
            else if (mod & KMOD_ALT)
            {
                O->shadow = !O->shadow;
            }
            else
            {
                open_OBJ_dialog();
                /*
                int result;
                char * fileName;
                fileName = open_FileName(gHwnd);
                OBJ_File = read_OBJ_file(fileName);

                if (OBJ_File.initialized)
                {
                    result = create_Object_OBJ(fileName, &OBJ_File);

                    printf("OBJ create result %d\n", result);

                    if (result)
                    {
                        currentObject = objectIndex - 1;
                        O0 = &objects[objectIndex - 1];
                        subdLevel = -1;
                    }

//                    int k;
//
//                    for (i = 0; i < OBJ_File.Vertcoord_c; i += 3)
//                    {
//                        printf("v %f %f %f\n", OBJ_File.Vertcoords[i], OBJ_File.Vertcoords[i + 1], OBJ_File.Vertcoords[i + 2]);
//                    }
//                    for (i = 0; i < OBJ_File.Normcoord_c; i += 3)
//                    {
//                        printf("vn %f %f %f\n", OBJ_File.Normcoords[i], OBJ_File.Normcoords[i + 1], OBJ_File.Normcoords[i + 2]);
//                    }
//                    for (i = 0; i < OBJ_File.Textcoord_c; i += 2)
//                    {
//                        printf("vt %f %f\n", OBJ_File.Textcoords[i], OBJ_File.Textcoords[i + 1]);
//                    }
//                    for (i = 0; i < OBJ_File.Face_c; i ++)
//                    {
//                        printf("f ");
//                        for (k = 0; k < OBJ_File.Faces[i].vert_c; k ++)
//                        {
//                            printf("%d/", OBJ_File.Faces[i].vert[k]);
//                            printf("%d/", OBJ_File.Faces[i].text[k]);
//                            printf("%d ", OBJ_File.Faces[i].norm[k]);
//                        }
//                        printf("\n");
//                    }
                }*/
                message = -10;
            }
        }
        else if (message == 24)
        {
            if (mod & KMOD_SHIFT)
            {
                edgeWeights = !edgeWeights;
                if (edgeWeights)
                {
                    SDL_GetMouseState(&mouse_x, &mouse_y);
                    printf("mouse at %d, %d\n", mouse_x, mouse_y);

                    subdLevel_mem = subdLevel;

                    Camera = find_View(mouse_x, mouse_y, splitview);
                    Camera->origin_2d[0] = mouse_x;
                    Camera->origin_2d[1] = mouse_y;
                }
                else
                {
                    finish_adjusting_Edges_Weights();
                    finish_adjusting_Verts_Weights();
                }
            }
            else if (mod & KMOD_CTRL)
            {
                clear_Selected_Edges_Weights();
            }
            else if (mod & KMOD_ALT)
            {
                ELEMENT_ARRAYS = !ELEMENT_ARRAYS;
                init_Hint();

                if (subdLevel > -1)
                load_id_colors_No_Surface(Camera, subdLevel);
                else
                load_id_colors_Fan_all(Camera, OBJECT_COLORS);

                UPDATE_COLORS = 1;
            }
            else
            {
                edgedraw = !edgedraw;
            }
            message = -10;
        }
        else if (message == 25)
        {
            if (dialog_lock && dialog_type == DEFR_DIALOG)
            {
                if (mod & KMOD_SHIFT)
                {
                    handle_UP_Sels(0);
                }
                else
                {
                    handle_DOWN_Sels(0);
                }
            }
            else
            {
                if (DRAW_LOCATORS && Object_Mode)
                {
                    int t;
                    for (t = 0; t < transformerIndex; t ++)
                    {
                        T = transformers[t];
                        T->selected = 0;
                    }
                    if (mod & KMOD_SHIFT)
                    {
                        currentLocator -= 1;
                        if (currentLocator < 0) currentLocator = transformerIndex - 1;
                    }
                    else
                    {
                        currentLocator += 1;
                        if (currentLocator >= transformerIndex) currentLocator = 0;
                    }
                    T = transformers[currentLocator];
                    T->selected = 1;
                    //LocatorSize = T->LocatorSize;
                    select_Transformer_Bone(T);
                    select_Transformer_IK(T);
                    select_Deformer();
                }
                else
                {
                    int o;
                    for (o = 0; o < objectIndex; o ++)
                    {
                        O = objects[o];
                        O->selected = 0;
                    }

                    if (mod & KMOD_SHIFT)
                    {
                        currentObject -= 1;
                        if (currentObject < 0) currentObject = objectIndex - 1;
                    }
                    else
                    {
                        currentObject += 1;
                        if (currentObject >= objectIndex) currentObject = 0;
                    }
                    loaded_objects[currentObject] = 1;
                    O = objects[currentObject];
                    selected_object_count = 1;
                    selected_objects[0] = O->index;
                    O->selected = 1;
                    printf("%s\n", objects[currentObject]->Name);
                }

                frame_object(Camera, 1);

                if (dialog_lock)
                {
                    if (dialog_type == SELS_DIALOG)
                    {
                        sels_start[current_sel_type] = 0;
                    }
                    draw_Dialog();
                }

            }
            if (dialog_lock);
            else
                message = -10;
        }
        else if (message == 26)
        {
            if (mod & KMOD_CTRL)
            {
                controlDown = 0;
                add_selection_mode = 1;
                SDL_SetCursor(Arrow);
                if (!BIND_POSE)
                {
                    unfix_ik_goals();
                    deformer_Player();
                }
            }
            else if (mod & KMOD_SHIFT)
            {
                if (dialog_lock)
                {
                    dialog_lock = 0;

                    if (!Object_Mode)
                    {
                        if (add_selection_mode)
                            SDL_SetCursor(Arrow_Plus);
                        else
                            SDL_SetCursor(Arrow_Minus);
                    }
                    message = -1;
                }
                else
                {
                    dialog_lock = 1;
                    draw_Dialog();
                }
            }
            else if (mod & KMOD_ALT)
            {
                if (DRAW_LOCATORS)
                {
                    select_Deformer();
                }
                else
                {
                    if (O->curve_count > 0)
                    {
                        remove_Curve(O, O->curves[0]);
                        print_Object_Curves(O);
                    }
                }
            }
            else
            {
                tripsRender = !tripsRender;
                UPDATE_COLORS = 1;
                message = -10;
            }
        }
        else if (message == 27)
        {
            hide_Object(currentObject);
            message = -10;
        }
        else if (message == 28)
        {
            if (mod & KMOD_ALT)
            {
                currentTheme ++;
                if (currentTheme >= Themes)
                {
                    currentTheme = 0;
                }
                init_Hint();
                update_Light(Light_Themes[currentTheme]);
                init_lights();
            }
            else if (DRAW_LOCATORS)
            {
                unparent_Locator();
            }
            else
            {
                if (subdLevel > -1)
                {
                    if (mod & KMOD_SHIFT)
                    {
                        O->tune_in_uvtex[subdLevel] = !O->tune_in_uvtex[subdLevel];
                        printf("tune in uvtex %u\n", O->tune_in_uvtex[subdLevel]);
                    }
                    else
                    {
                        O->tune[subdLevel] = !O->tune[subdLevel];
                        printf("tune %u\n", O->tune[subdLevel]);
                    }
                }
                tune_subdivide_post(O);
            }
            message = -1;
        }
        else if (message == 29)
        {
            if (mod & KMOD_SHIFT)
            {
                open_ImagesDir();
            }
            else
            {
                open_Img();
            }
            message = -1;
        }
        else if (message == 30)
        {
            if (mod & KMOD_SHIFT)
            {
                SDL_GetMouseState(&mouse_x, &mouse_y);
                Camera = find_View(mouse_x, mouse_y, splitview);
                frame_object(Camera, 1);
            }
            else if (BIND_POSE)
            {
                if (mod & KMOD_ALT)
                {
                    T = transformers[selected_transformers[0]];
                    if (T->Bone != NULL && T == T->Bone->B)
                    {
                        if (T->childcount > 0)
                        {
                            T = T->childs[0];
                        }
                    }

                    if (deformerIndex > 0 && currentDeformer < deformerIndex)
                    {
                        D = deformers[currentDeformer];
                    }
                    else
                    {
                        D = NULL;
                    }

                    int r = add_ikChain(D, T, transformers[currentLocator]);
                    printf("Adding IK chain result %d\n", r);
                    if (r)
                        currentLocator += 2;
                }
                else
                {
                    ALIGN_IS_ON = !ALIGN_IS_ON;
                }
            }
            else if (mod & KMOD_ALT)
            {
                //add_Curve(O);
                //print_Object_Curves(O);
            }
            else
            {
                O->deforms = !O->deforms;
            }
            message = -10;
        }
        else if (message == 31)
        {
            if (!edgeWeights)
            {
                SDL_GetMouseState(&mouse_x, &mouse_y);
                printf("mouse at %d, %d\n", mouse_x, mouse_y);
                Camera_screen_lock = !Camera_screen_lock;
                if (Camera_screen_lock)
                {
                    if (mod & KMOD_ALT)
                    {
                        altDown = 1;
                    }
                    start_Movement();
                }
                else
                {
                    subdLevel = subdLevel_mem;
                    object_hook = 0;
                    ROTATION = 0;
                    MOVEMENT = 0;
                    SCALE = 0;
                    Axis_lock = 0;
                    altDown = 0;
                    camera_rotate = 0;
                    camera_z_move = 0;
                    Camera_screen_lock = 0;
                }
                message = -1;
            }
        }
        else if (message == 32)
        {
            if (mod & KMOD_SHIFT)
            {
                Osd = !Osd;
                message = -1;
            }
            else if (mod & KMOD_CTRL)
            {
                freeze_Object_Coordinates(O);
            }
            else if (mod & KMOD_ALT)
            {
                cycle_frame_Player(Camera, 1);
            }
            else
            {
                if (TURNTABLE)
                {
                    TURNTABLE = 0;
                }
                else if(DRAW_LOCATORS)
                {
                    if (T->style == ik_goal)
                    {
                        T->style = ik_fixed;
                        fix_ik_goal(T);
                    }
                    else if (T->style == ik_fixed)
                    {
                        T->style = ik_goal;
                        unfix_ik_goal(T);
                    }
                }
                else
                {
                    SDL_GetMouseState(&mouse_x, &mouse_y);
                    Camera = find_View(mouse_x, mouse_y, splitview);
                    flip_Cameras(Camera);
                }
                message = -10;
            }
        }
        else if (message == 33)
        {
            if (mod & KMOD_SHIFT)
            {
                draw_uv_view = !draw_uv_view;
            }
            else
            {
                splitview = !splitview;
                if (!splitview)
                {
                    Camera = find_View(mouse_x, mouse_y, 1);
                }
                else
                {
                    rotate_Camera(&Camera_Persp, CamDist);
                    rotate_Camera(&Camera_Top, CamDist);
                    rotate_Camera(&Camera_Front, CamDist);
                    rotate_Camera(&Camera_Left, CamDist);
                }
            }
            message = -10;
        }
        else if (message == 34)
        {
            if (mod & KMOD_ALT)
            {
                SHADOWS = !SHADOWS;
                init_Hint();
            }
            else if (!edgeWeights)
            {
                SDL_GetMouseState(&mouse_x, &mouse_y);
                printf("mouse at %d, %d\n", mouse_x, mouse_y);
                Camera_screen_lock = !Camera_screen_lock;
                if (Camera_screen_lock)
                {
                    subdLevel_mem = subdLevel;

                    O = objects[currentObject];

                    Camera = find_View(mouse_x, mouse_y, splitview);
                    Camera->origin_2d[0] = mouse_x;
                    Camera->origin_2d[1] = mouse_y;

                    if (mod & KMOD_CTRL)
                    {
                        T = Camera->T;
                        CamDist_mem = CamDist;
                        camera_z_move = 1;
                    }
                    else
                    {
                        if (DRAW_LOCATORS)
                        {
                            if (currentLocator == 0)
                                currentLocator = transformerIndex - 1;
                            T = transformers[currentLocator];
                        }
                        else
                        {
                            T = O->T;
                        }
                        if (T->IK != NULL && T->style == ik_fixed)
                        {
                            Camera_screen_lock = 0;
                        }
                        else
                        {
                            bake_position(T);
                            bake_position_Children(T);
                            if (mod & KMOD_SHIFT)
                            {
                                ROTATION = 1;
                                bake(T);
                                object_hook = 3;
                            }
                            else
                            {
                                SCALE = 1;
                                bake_scale(T);
                                object_hook = 2;
                            }
                        }
                    }
                    memcpy(object_Pos, T->pos, sizeof(object_Pos));
                    memcpy(target_Pos, T->target, sizeof(target_Pos));
                    memcpy(object_Rot, T->rot, sizeof(object_Rot));
                    memcpy(object_Scl, T->scl, sizeof(object_Scl));
                    UPDATE_COLORS = 1;
                }
                else
                {
                    subdLevel = subdLevel_mem;
                    object_hook = 0;
                    ROTATION = 0;
                    MOVEMENT = 0;
                    SCALE = 0;
                    Axis_lock = 0;
                    camera_rotate = 0;
                    camera_z_move = 0;
                    Camera_screen_lock = 0;
                }
                message = -1;
            }
        }
        else if (message == 35)
        {
            if (dialog_lock)
            {
                if (Edit_Lock)
                {
                    Edit_Lock = 0;
                    EditCursor = 0;
                    EditString[EditCursor] = '\0';
                    printf("Edit finishing!\n");
                    if (dialog_type == SAVES_DIALOG)
                    {
                        sprintf(scene_files_dir, "%s", Name_Remember);
                        update_Saves_List(1, 0);
                    }
                    else if (dialog_type == LOADING_DIALOG)
                    {
                        sprintf(scene_files_dir, "%s", Name_Remember);
                        update_Loading_List(1, 0);
                    }
                    else if (dialog_type == BONE_DIALOG)
                    {
                        sprintf(Bone_Names[BoneIndex], "%s", Name_Remember);
                        set_Bone_H_Button(-1);
                        update_Bones_List(1, 0);
                    }
                    else if (dialog_type == IK_DIALOG)
                    {
                        sprintf(IK_Names[IKIndex], "%s", Name_Remember);
                        set_IK_H_Button(-1);
                        update_IK_List(1, 0);
                    }
                    else if (dialog_type == POSE_DIALOG)
                    {
                        sprintf(Pose_Names[PoseIndex], "%s", Name_Remember);
                        set_Pose_H_Button(-1);
                        update_Poses_List(1, 0);
                    }
                    else if (dialog_type == HIER_DIALOG)
                    {
                        sprintf(Hier_Names[HierIndex], "%s", Name_Remember);
                        set_Hier_H_Button(-1);
                        update_Hierarcys_List(1, 0);
                    }
                    else if (dialog_type == SELS_DIALOG)
                    {
                        sprintf(Sels_Names[current_sel_type][SelsIndex[current_sel_type]], "%s", Name_Remember);
                        set_Sels_H_Button(-1);
                        update_Selections_List(1, 0);
                    }
                }
                else
                {
                    dialog_lock = 0;

//                    set_Object_Mode();
                }

            }
            else if (DRAW_LOCATORS && Object_Mode)
            {
                int t;
                for (t = 0; t < transformerIndex; t ++)
                {
                    T = transformers[t];
                    T->selected = 0;
                }
            }
            else if (Object_Mode)
            {
                deselect_Objects();
            }
            else if (Polygon_Mode)
            {
                int l, o;
                camera * C = Camera;
                C->objects[0] = currentObject;
                C->object_count = 1;

                ELEMENT_ARRAYS = 1;
                init_Hint();

                for (l = 0; l < SUBD; l++)
                    load_id_colors(selected_objects, selected_object_count, l, -1);
                load_id_colors_Fan(selected_objects, selected_object_count, -1);

                object * O;
                for (o = 0; o < selected_object_count; o ++)
                {
                    O = objects[selected_objects[o]];
                    assign_Selection(O, 0);
                    assert_Element_Selection(O);
                }
            }
            else if (Edge_Mode)
            {
                int o;
                object * O;
                for (o = 0; o < selected_object_count; o ++)
                {
                    O = objects[selected_objects[o]];
                    clear_Edge_Selection(O);
                    assert_Element_Selection(O);
                }
            }
            else if (Vertex_Mode)
            {
                int o;
                object * O;
                for (o = 0; o < selected_object_count; o ++)
                {
                    O = objects[selected_objects[o]];
                    clear_Verts_Selection(O);
                    assert_Element_Selection(O);
                }
            }

            Bottom_Message = 1;
            UPDATE_COLORS = 1;
            message = -10;
        }
        else if (message == 36)
        {
            if (draw_uv_view)
            {
                NormalMode = 0;
                BumpMode = !BumpMode;
            }
            else if (!BIND_POSE && mod & KMOD_ALT)
            {
                Draw_Bottom_Message("set Bind Pose For Branch\n");
                set_Bind_Pose_For_Branch(T);
            }
            else if (!BIND_POSE && mod & KMOD_SHIFT)
            {
                //rotate_Deformer_IK(T);
            }
            else if (DRAW_LOCATORS && !BONES_MODE && BIND_POSE && !Camera_screen_lock && !dialog_lock)
            {
                BONES_MODE = 1;
                int result = add_New_Bone();
                if (result < 0)
                {
                    BONES_MODE = 0;
                    message = 0;
                }
                else
                {
                    currentBone = result;
                    SDL_GetMouseState(&mouse_x, &mouse_y);
                    printf("mouse at %d, %d\n", mouse_x, mouse_y);
                    Camera_screen_lock = 1;
                    start_Movement();
                }
            }
        }
        else if (message == 37)
        {
            if (Object_Mode)
            {
                if (mod & KMOD_ALT)
                {
                    Draw_Bottom_Message("Camera Pose Set");
                    set_Camera_Pose(&Camera_Persp, CamDist);
                }
                else
                {
                    Draw_Bottom_Message("Camera Pose Read");
                    read_Camera_Pose(&Camera_Persp);
                    rotate_Camera(&Camera_Persp, CamDist);
                    rotate_Camera(&Camera_Top, CamDist);
                    rotate_Camera(&Camera_Front, CamDist);
                    rotate_Camera(&Camera_Left, CamDist);

                    update_camera_view(&Camera_Persp, Camera_Persp.h_view, screen_width, screen_height);
                    update_camera_view(&Camera_Top, Camera_Persp.h_view, screen_width, screen_height);
                    update_camera_view(&Camera_Left, Camera_Persp.h_view, screen_width, screen_height);
                    update_camera_view(&Camera_Front, Camera_Persp.h_view, screen_width, screen_height);
                }
            }
            else if (mod & KMOD_SHIFT)
            {
                select_Connected();
                assert_Element_Selection();
            }
            else if (mod & KMOD_CTRL)
            {
                convert_To_Vert_Selection();
                set_Button_sels(3);
                assert_Element_Selection();
            }
            else
            {
                if (Edge_Mode)
                {
                    convert_To_Border_Verts(O);
                    set_Button_sels(3);
                }
                else
                {
                    convert_To_Border_Edges(O);
                    set_Button_sels(2);
                }
                assert_Element_Selection();
            }
        }
        else if (message == 41)
        {
            if (mod & KMOD_SHIFT)
            {
                select_Loop();
                assert_Element_Selection();
            }
            else
            {
                DRAW_LOCATORS = !DRAW_LOCATORS;
                LOCAT_ID_RENDER = DRAW_LOCATORS;
                poly_Render(tripsRender, wireframe, splitview, CamDist, 1, subdLevel);
            }
        }
        else if (message == 42)
        {
            if (mod & KMOD_ALT)
            {
                NIGHT = !NIGHT;
                init_Hint();
            }
            else if (draw_uv_view)
            {
                BumpMode = 0;
                NormalMode = !NormalMode;
            }
            else
            {
                new_Locator();
                currentLocator = transformerIndex - 1;
            }
        }
        else if (message == 44)
        {
            if (!dialog_lock)
            {
                grow_Selection();
                message = -10;
            }
        }
        else if (message == 47)
        {
            if (ROTATION || MOVEMENT || SCALE)
            {
                if (Axis_lock == 1)
                    Axis_lock = 0;
                else
                    Axis_lock = 1;
            }
            else
            {
                set_Object_Mode();
            }
            message = -10;
        }
        else if (message == 48)
        {
            if (ROTATION || MOVEMENT || SCALE)
            {
                if (Axis_lock == 2)
                    Axis_lock = 0;
                else
                    Axis_lock = 2;
            }
            else
            {
                set_Polygon_Mode();
            }
            message = -10;
        }
        else if (message == 49)
        {
            if (ROTATION || MOVEMENT || SCALE)
            {
                if (Axis_lock == 3)
                    Axis_lock = 0;
                else
                    Axis_lock = 3;
            }
            else
            {
                set_Edge_Mode();
            }
            message = -10;
        }
        else if (message == 50)
        {
            set_Vertex_Mode();
            message = -10;
        }
        else if (message == 51)
        {
            set_Bind_Mode();
            message = -1;
        }
        else if (message == 76)
        {
            SHADERS = !SHADERS;
            if (!SHADERS && VBO)
            {
                enable_VBO();
            }
            else
            {
                disable_VBO();
            }
        }
        else if (message == 77)
        {
            //calculate_Triangle_Tangents(O);
        }
        if (message != 0 && !dialog_lock)
        {
            transform_Objects_And_Render();
        }
    }

    cleanup();

    return 0;
}
