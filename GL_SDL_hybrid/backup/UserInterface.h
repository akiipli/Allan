/*
The MIT License

Copyright <2018> <Allan Kiipli>
*/

#ifndef USERINTERFACE_H_INCLUDED
#define USERINTERFACE_H_INCLUDED
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>

#define STRLEN 256
#define SCENE_EXT_NUM 6

#define SIDEBAR 100
#define BOTTOM_LINE 20
#define BUTTON_HEIGHT 20
#define BUTTON_WIDTH 100
#define BUTTON_WIDTH_SHORT 50
#define BUTTON_WIDTH_MEDIUM 60
#define MIN_DIALOG_WIDTH 460
#define MIN_DIALOG_HEIGHT 270
int DIALOG_WIDTH = 460;
int DIALOG_HEIGHT = 270;

#define UI_WHITE 0
#define UI_GRAYB 1
#define UI_GRAYD 2
#define UI_BACKG 3
#define UI_YELLO 4
#define UI_BLACK 5
#define UI_BACKL 6
#define UI_DIMSL 7

#define BUTTONS 15
#define BUTTONS_MODE 5

#define MAXFILES 1000
#define MAX_LISTLENGTH 100
int LISTLENGTH = 12;
#define EXT_NUM 3
#define ITEM_NUM 3
#define SELS_NUM 4
#define DEFR_NUM 3
#define H_SELS_NUM 6
#define H_HIER_NUM 5
#define H_DEFR_NUM 6
#define H_SCEN_NUM 1
#define H_POSE_NUM 4
#define H_BONE_NUM 2

#define TYPE_LENGTH 20

#define ITEM_TYPE_OBJECT "object"
#define ITEM_TYPE_CAMERA "camera"
#define ITEM_TYPE_LIGHT  "light"

int SHADERS = 0;

void display_font_(const char * text, float origin_x, float origin_y, int font_height, float color[4]);

int dir_lists = 0;
int selected_deformer_node;

typedef struct
{
    int x0, y0;
    int x1, y1;
    int x, y, w, h;
}
dragrectangle;

dragrectangle Drag_Rectangle;

int files_start = 0;
int files_num = 0;

int UI_LISTS = 1;
int UI_FILES = 1;
int UI_CLEAR = 1;

int Buttonindex;
int Buttonindex_sidebar;
int Buttonindex_bottom;
int Sidebar_Marker;

typedef void (*func_ptr)(void);
typedef void (*func_ptr_int)(int);
typedef void (*func_ptr_int_int)(int, int);

func_ptr SideBar[BUTTONS];

char resources_dir[] = "C:/Allan/resource/";
char * files_dir;
char images_dir[STRLEN];
char objects_dir[STRLEN];

typedef struct
{
    int index;
    int color;
    func_ptr func;
}
ui_button;

typedef struct
{
    int index;
    int color;
    func_ptr_int_int func;
}
ui_button_ext;

typedef struct
{
    int index;
    int color;
    func_ptr_int func;
}
ui_button_item;

typedef struct
{
    int index;
    int color;
    func_ptr_int func;
}
ui_button_sels;

typedef struct
{
    int index;
    int color;
    func_ptr_int func;
}
ui_button_defr;

typedef struct
{
    int index;
    int color;
}
ui_hierc;

typedef struct
{
    int index;
    int color;
}
ui_selec;

typedef struct
{
    int index;
    int color;
}
ui_items;

typedef struct
{
    int index;
    int color;
}
ui_texts;

typedef struct
{
    int index;
    int color;
}
ui_files;

typedef struct
{
    int index;
    int color;
}
ui_defor;

typedef struct
{
    int index;
    int color;
}
ui_poses;

typedef struct
{
    int index;
    int color;
}
ui_bones;

typedef struct
{
    union
    {
        GLfloat color[4];
        struct
        {
            GLfloat r;
            GLfloat g;
            GLfloat b;
            GLfloat a;
        };
    };
}
ui_color;

GLfloat white[4] = {1, 1, 1, 1};
GLfloat grayb[4] = {0.6, 0.3, 0.1, 1};
GLfloat grayd[4] = {0.5, 0.5, 0.5, 1};
GLfloat yelld[4] = {1, 1, 0.0, 1};
GLfloat backg[4] = {0.7, 0.4, 0.2, 1};
GLfloat black[4] = {0.5, 0.2, 0.0, 1};
GLfloat backl[4] = {1, 0.7, 0.4, 1};
GLfloat dimsl[4] = {0.7, 0.8, 0.5, 1};
GLfloat backg_semitransparent[4] = {0.7, 0.4, 0.2, 0.1};
GLfloat grayb_transparent[4] = {1, 0.5, 0.5, 0.1};
GLfloat grayb_light[4] = {0.8, 0.4, 0.3, 1};

ui_color buttoncolors[8];
ui_button Button_sidebar[BUTTONS];
ui_button Button_Mode[BUTTONS_MODE];
ui_button_ext Button_ext[EXT_NUM];
ui_button_ext Button_scene_ext[SCENE_EXT_NUM];
ui_button_item Button_item[ITEM_NUM];
ui_button_sels Button_sels[SELS_NUM];
ui_button_defr Button_defr[DEFR_NUM];
ui_button Button_h_sels[H_SELS_NUM];
ui_button Button_h_hier[H_HIER_NUM];
ui_button Button_h_defr[H_DEFR_NUM];
ui_button Button_h_pose[H_POSE_NUM];
ui_button Button_h_bone[H_BONE_NUM];
ui_button Button_h_scen[H_SCEN_NUM];
ui_files FileList[MAX_LISTLENGTH];
ui_items ItemList[MAX_LISTLENGTH];
ui_texts TextList[MAX_LISTLENGTH];
ui_selec SelsList[MAX_LISTLENGTH];
ui_hierc HierList[MAX_LISTLENGTH];
ui_defor DefrList[MAX_LISTLENGTH];
ui_poses PoseList[MAX_LISTLENGTH];
ui_bones BoneList[MAX_LISTLENGTH];

void init_ui()
{
    files_dir = (char *)&resources_dir;
    memcpy(&images_dir, resources_dir, sizeof resources_dir);
    memcpy(&objects_dir, resources_dir, sizeof resources_dir);
    Buttonindex = 0;
    memcpy(&buttoncolors[0], white, sizeof white);
    memcpy(&buttoncolors[1], grayb, sizeof grayb);
    memcpy(&buttoncolors[2], grayd, sizeof grayd);
    memcpy(&buttoncolors[3], backg, sizeof backg);
    memcpy(&buttoncolors[4], yelld, sizeof yelld);
    memcpy(&buttoncolors[5], black, sizeof black);
    memcpy(&buttoncolors[6], backl, sizeof backl);
    memcpy(&buttoncolors[7], dimsl, sizeof dimsl);
    int b;
    for(b = 0; b < BUTTONS; b ++)
    {
        Button_sidebar[b].index = b;
        Button_sidebar[b].color = UI_GRAYB;
        Button_sidebar[b].func = NULL;
    }
    for(b = 0; b < BUTTONS_MODE; b ++)
    {
        Button_Mode[b].index = b;
        Button_Mode[b].color = UI_GRAYB;
        Button_Mode[b].func = NULL;
    }
    for(b = 0; b < EXT_NUM; b ++)
    {
        Button_ext[b].index = b;
        Button_ext[b].color = UI_GRAYB;
        Button_ext[b].func = NULL;
    }
    for(b = 0; b < SCENE_EXT_NUM; b ++)
    {
        Button_scene_ext[b].index = b;
        Button_scene_ext[b].color = UI_GRAYB;
        Button_scene_ext[b].func = NULL;
    }
    for(b = 0; b < ITEM_NUM; b ++)
    {
        Button_item[b].index = b;
        Button_item[b].color = UI_GRAYB;
        Button_item[b].func = NULL;
    }
    for(b = 0; b < SELS_NUM; b ++)
    {
        Button_sels[b].index = b;
        Button_sels[b].color = UI_GRAYB;
        Button_sels[b].func = NULL;
    }
    for(b = 0; b < DEFR_NUM; b ++)
    {
        Button_defr[b].index = b;
        Button_defr[b].color = UI_GRAYB;
        Button_defr[b].func = NULL;
    }
    for(b = 0; b < H_SELS_NUM; b ++)
    {
        Button_h_sels[b].index = b;
        Button_h_sels[b].color = UI_GRAYB;
        Button_h_sels[b].func = NULL;
    }
    for(b = 0; b < H_HIER_NUM; b ++)
    {
        Button_h_hier[b].index = b;
        Button_h_hier[b].color = UI_GRAYB;
        Button_h_hier[b].func = NULL;
    }
    for(b = 0; b < H_DEFR_NUM; b ++)
    {
        Button_h_defr[b].index = b;
        Button_h_defr[b].color = UI_GRAYB;
        Button_h_defr[b].func = NULL;
    }
    for(b = 0; b < H_POSE_NUM; b ++)
    {
        Button_h_pose[b].index = b;
        Button_h_pose[b].color = UI_GRAYB;
        Button_h_pose[b].func = NULL;
    }
    for(b = 0; b < H_BONE_NUM; b ++)
    {
        Button_h_bone[b].index = b;
        Button_h_bone[b].color = UI_GRAYB;
        Button_h_bone[b].func = NULL;
    }
    for(b = 0; b < H_SCEN_NUM; b ++)
    {
        Button_h_scen[b].index = b;
        Button_h_scen[b].color = UI_GRAYB;
        Button_h_scen[b].func = NULL;
    }
    for(b = 0; b < MAX_LISTLENGTH; b ++)
    {
        FileList[b].index = b;
        FileList[b].color = UI_BLACK;
    }
    for(b = 0; b < MAX_LISTLENGTH; b ++)
    {
        ItemList[b].index = b;
        ItemList[b].color = UI_BLACK;
    }
    for(b = 0; b < MAX_LISTLENGTH; b ++)
    {
        TextList[b].index = b;
        TextList[b].color = UI_BLACK;
    }
    for(b = 0; b < MAX_LISTLENGTH; b ++)
    {
        SelsList[b].index = b;
        SelsList[b].color = UI_BLACK;
    }
    for(b = 0; b < MAX_LISTLENGTH; b ++)
    {
        HierList[b].index = b;
        HierList[b].color = UI_BLACK;
    }
    for(b = 0; b < MAX_LISTLENGTH; b ++)
    {
        DefrList[b].index = b;
        DefrList[b].color = UI_BLACK;
    }
    for(b = 0; b < MAX_LISTLENGTH; b ++)
    {
        PoseList[b].index = b;
        PoseList[b].color = UI_BLACK;
    }
    for(b = 0; b < MAX_LISTLENGTH; b ++)
    {
        BoneList[b].index = b;
        BoneList[b].color = UI_BLACK;
    }
    Button_Mode[0].color = UI_GRAYD;
    Button_ext[0].color = UI_GRAYD;
    Button_scene_ext[0].color = UI_GRAYD;
    Button_item[0].color = UI_GRAYD;
    Button_sels[0].color = UI_GRAYD;
}

typedef struct
{
	GLfloat x;
	GLfloat y;
	GLfloat s;
	GLfloat t;
}
point;

int isFile(const char * path)
{
    struct stat path_stat;
    stat(path, &path_stat);
    return S_ISREG(path_stat.st_mode);
}

int isDirectory(const char * path)
{
    struct stat path_stat;
    stat(path, &path_stat);
    return S_ISDIR(path_stat.st_mode);
}

char Out_List[MAXFILES][STRLEN];
char Out_Dirs[MAXFILES][STRLEN];
char Out_File[MAXFILES][STRLEN];
char Item_List[OBJECTS][STRLEN];

int query_items(const char * type)
{
    int s = 0;
    int i;
    for (i = 0; i < objectIndex; i++)
    {
        if (strcmp(type, ITEM_TYPE_OBJECT) == 0)
        {
            memcpy(Item_List[s], objects[s]->Name, strlen(objects[s]->Name));
            Item_List[s][strlen(objects[s]->Name)] = '\0';
            s++;
        }
    }
    return s;
}

int query_files(char * path, const char * ext)
{
    char Path[STRLEN];
    DIR * dir;
    struct dirent * ent;
    int s = 0;
    int extension_len = strlen(ext);
    char * Extension = malloc(extension_len * sizeof(char));
    if ((dir = opendir(path)) != NULL)
    {
        while ((ent = readdir(dir)) != NULL)
        {
            Path[0] = '\0';
            strcat(Path, path);
            strcat(Path, ent->d_name);
            if (isFile(Path) || isDirectory(Path))
            {
                memcpy(Extension, &ent->d_name[strlen(ent->d_name) - extension_len], extension_len);
                if (strcmp(Extension, ext) == 0)
                {
                    memcpy(Out_List[s], ent->d_name, strlen(ent->d_name));
                    Out_List[s][strlen(ent->d_name)] = '\0';
                    s ++;
                }
                if (s >= MAXFILES)
                    break;
            }
        }
        closedir(dir);
    }
    else
    {
        perror("could not open directory");
    }
    free(Extension);
    return s;
}

int list_bones(char **, int, int);

int list_poses(char **, int, int, int *, int *);

int list_deformers(char **, int, int, int *, int *);

int list_hierarcys(char **, int, int, int *, int *);

int list_selections(char **, int, int, char *);

int list_texts(char **, int, int);

int list_items(char ** item_list, int start, int n, const char * type, int * selected, int currentObject)
{
    int s = start;
    int i = 0;
    if (strcmp(type, ITEM_TYPE_OBJECT) == 0)
    {
        for (i = 0; i < LISTLENGTH; i++)
        {
            if (s >= objectIndex)
                break;
            if (objects[s]->binding)
            {
                sprintf(item_list[i], "%s|%d", objects[s]->Name, objects[s]->binding);
                memcpy(Item_List[s], objects[s]->Name, strlen(objects[s]->Name) + 2);
                Item_List[s][strlen(objects[s]->Name) + 2] = '\0';
            }
            else
            {
                sprintf(item_list[i], "%s", objects[s]->Name);
                memcpy(Item_List[s], objects[s]->Name, strlen(objects[s]->Name));
                Item_List[s][strlen(objects[s]->Name)] = '\0';
            }

            if (objects[s]->selected && s != currentObject)
                selected[i] = 1;
            else
                selected[i] = 0;
            s++;
        }
    }
    return i;
}

int list_directory(char * path, char ** out_list, int start, int n, const char * ext)
{
    char Path[STRLEN];
    DIR * dir;
    struct dirent * ent;
    int s = 0, j = 0, i = 0, D = 0, F = 0;
    int extension_len = strlen(ext);
    char * Extension = malloc(extension_len * sizeof(char));
    if ((dir = opendir(path)) != NULL)
    {
        while ((ent = readdir(dir)) != NULL)
        {
            Path[0] = '\0';
            strcat(Path, path);
            strcat(Path, ent->d_name);
            if (isFile(Path) || isDirectory(Path))
            {
                memcpy(Extension, &ent->d_name[strlen(ent->d_name) - extension_len], extension_len);
                if (strcmp(Extension, ext) == 0)
                {
                    if (isDirectory(Path))
                    {
                        memcpy(Out_Dirs[D], ent->d_name, strlen(ent->d_name));
                        Out_Dirs[D][strlen(ent->d_name)] = '\0';

                        D ++;
                    }
                    else
                    {
                        memcpy(Out_File[F], ent->d_name, strlen(ent->d_name));
                        Out_File[F][strlen(ent->d_name)] = '\0';

                        F ++;
                    }


                    if (F + D >= MAXFILES)
                        break;
                }
            }
        }
        closedir(dir);
        for (i = 0; i < D; i ++)
        {
            sprintf(Out_List[i], "%s", Out_Dirs[i]);
        }
        for (i = D; i < D + F; i ++)
        {
            sprintf(Out_List[i], "%s", Out_File[j]);
            j ++;
        }
        for (i = start; i < start + n; i ++)
        {
            if (i >= F + D)
                break;
            sprintf(out_list[s], "%s", Out_List[i]);
            s ++;
        }
        dir_lists = 1;
    }
    else
    {
        perror("could not open directory");
        dir_lists = 0;
    }
    files_num = F + D;
    //printf("files num S %d\n", S);
    free(Extension);
    return s;
}

FT_Library ft;
FT_Face face;

GLuint font_tex;

int init_fonts()
{
	glGenTextures(1, &font_tex);
	/* Initialize the FreeType2 library */
	if (FT_Init_FreeType(&ft)) {
		fprintf(stderr, "Could not init freetype library\n");
		return 0;
	}

    char dirfile[STRLEN];
    dirfile[0] = '\0';
    strcat(dirfile, resources_dir);
    strcat(dirfile, "courbd.ttf");
	if (FT_New_Face(ft, dirfile, 0, &face)) {
		fprintf(stderr, "Could not open font\n");
		return 0;
	}

	return 1;
}

void render_text(const char * text, float x, float y, int font_height)
{
	const char * p;
	FT_GlyphSlot G = face->glyph;

	/* Create a texture that will be used to hold one "glyph" */
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, font_tex);

	/* We require 1 byte alignment when uploading texture data */
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	/* Clamping to edges is important to prevent artifacts when scaling */
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	/* Linear filtering usually looks best for text */
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	/* Loop through all characters */

	float X = x;
	int c = 0;
	int tab = 10;
	int tab_c = 0;
	for(p = text; * p; p ++) {
		/* Try to load and render the character */
		if (FT_Load_Char(face, * p, FT_LOAD_RENDER))
        {
            continue;
        }

		if (* p == '\t')
        {
            tab_c = tab - (c % tab);
            x += (G->advance.x >> 6) * tab_c;
            c += tab_c;
            continue;
        }

		if (* p == '\n')
        {
            y += font_height;
            x = X;
            c = 0;
            continue;
        }

		/* Upload the "bitmap", which contains an 8-bit grayscale image, as an alpha texture */
		glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, G->bitmap.width, G->bitmap.rows, 0, GL_ALPHA, GL_UNSIGNED_BYTE, G->bitmap.buffer);

		/* Calculate the vertex and texture coordinates */
		float x0 = x + G->bitmap_left;
		float w = G->bitmap.width;

		float lower_part = G->bitmap.rows - G->bitmap_top;

		point box[4] =
		{{x0,     y + lower_part, 0, 1},
         {x0 + w, y + lower_part, 1, 1},
         {x0 + w, y - G->bitmap_top, 1, 0},
         {x0,     y - G->bitmap_top, 0, 0}};

		int e;
        glBegin(GL_QUADS);
        for (e = 0; e < 4; e ++)
        {
            glTexCoord2f(box[e].s, box[e].t);
            glVertex2f(box[e].x, box[e].y);
        }
        glEnd();

		/* Advance the cursor to the start of the next character */
		x += G->advance.x >> 6;
        y += G->advance.y >> 6;
        c ++;
	}
}

void draw_text(const char * text, int origin_x, int origin_y, int font_height)
{
    if (SHADERS)
    {
        float color[4];
        glGetFloatv(GL_CURRENT_COLOR, color);
        display_font_(text, origin_x, origin_y, font_height, color);
    }
    else
    {
        render_text(text, origin_x, origin_y, font_height);
    }
}

void draw_Button_bone_text(const char * text, int width, int height, int index, int colorchange, int frame_it)
{
    int font_height = 11;

	FT_Set_Pixel_Sizes(face, 0, font_height);

	float origin_x = 5;
	float origin_y = BUTTON_HEIGHT * index + 10;

	if (frame_it)
    {
        glDisable(GL_TEXTURE_2D);
        glColor4fv(white);

        glBegin(GL_LINE_LOOP);
        glVertex2f(0, BUTTON_HEIGHT * index);
        glVertex2f(0, BUTTON_HEIGHT * index + BUTTON_HEIGHT);
        glVertex2f(width, BUTTON_HEIGHT * index + BUTTON_HEIGHT);
        glVertex2f(width, BUTTON_HEIGHT * index);
        glEnd();
    }
	glEnable(GL_TEXTURE_2D);

    if (colorchange)
        glColor4fv(buttoncolors[BoneList[index].color].color);
	else
        glColor4fv(buttoncolors[UI_BLACK].color);

    draw_text(text, origin_x, origin_y, font_height);
}

void draw_Button_pose_text(const char * text, int width, int height, int index, int colorchange, int frame_it, int x_offset, int x_collapsed)
{
    int font_height = 11;

	FT_Set_Pixel_Sizes(face, 0, font_height);

	float origin_x = 5 * x_offset;
	float origin_y = BUTTON_HEIGHT * index + 10;

	if (frame_it)
    {
        glDisable(GL_TEXTURE_2D);
        glColor4fv(white);

        glBegin(GL_LINE_LOOP);
        glVertex2f(0, BUTTON_HEIGHT * index);
        glVertex2f(0, BUTTON_HEIGHT * index + BUTTON_HEIGHT);
        glVertex2f(width, BUTTON_HEIGHT * index + BUTTON_HEIGHT);
        glVertex2f(width, BUTTON_HEIGHT * index);
        glEnd();
    }
	glEnable(GL_TEXTURE_2D);

    if (colorchange)
        glColor4fv(buttoncolors[PoseList[index].color].color);
	else
        glColor4fv(buttoncolors[UI_BLACK].color);

    char Text[STRLEN];
    Text[0] = '\0';

    if (x_collapsed)
    {
        strcat(Text, "+");
        strcat(Text, text);
    }
    else
    {
        strcat(Text, "|");
        strcat(Text, text);
    }

    draw_text(Text, origin_x, origin_y, font_height);
}

void draw_Button_defr_text(const char * text, int width, int height, int index, int colorchange, int frame_it, int x_offset, int x_collapsed)
{
    int font_height = 11;

	FT_Set_Pixel_Sizes(face, 0, font_height);

	float origin_x = 5 * x_offset;
	float origin_y = BUTTON_HEIGHT * index + 10;

	if (frame_it)
    {
        glDisable(GL_TEXTURE_2D);
        glColor4fv(white);

        glBegin(GL_LINE_LOOP);
        glVertex2f(0, BUTTON_HEIGHT * index);
        glVertex2f(0, BUTTON_HEIGHT * index + BUTTON_HEIGHT);
        glVertex2f(width, BUTTON_HEIGHT * index + BUTTON_HEIGHT);
        glVertex2f(width, BUTTON_HEIGHT * index);
        glEnd();
    }
	glEnable(GL_TEXTURE_2D);

    if (colorchange)
        glColor4fv(buttoncolors[DefrList[index].color].color);
	else
        glColor4fv(buttoncolors[UI_BLACK].color);

    char Text[STRLEN];
    Text[0] = '\0';

    if (x_collapsed)
    {
        strcat(Text, "+");
        strcat(Text, text);
    }
    else
    {
        strcat(Text, "|");
        strcat(Text, text);
    }

    draw_text(Text, origin_x, origin_y, font_height);
}

void draw_Button_hier_text(const char * text, int width, int height, int index, int colorchange, int frame_it, int x_offset, int x_collapsed)
{
    int font_height = 11;

	FT_Set_Pixel_Sizes(face, 0, font_height);

	float origin_x = 5 * x_offset;
	float origin_y = BUTTON_HEIGHT * index + 10;

	if (frame_it)
    {
        glDisable(GL_TEXTURE_2D);
        glColor4fv(white);

        glBegin(GL_LINE_LOOP);
        glVertex2f(0, BUTTON_HEIGHT * index);
        glVertex2f(0, BUTTON_HEIGHT * index + BUTTON_HEIGHT);
        glVertex2f(width, BUTTON_HEIGHT * index + BUTTON_HEIGHT);
        glVertex2f(width, BUTTON_HEIGHT * index);
        glEnd();
    }
	glEnable(GL_TEXTURE_2D);

    if (colorchange)
        glColor4fv(buttoncolors[HierList[index].color].color);
	else
        glColor4fv(buttoncolors[UI_BLACK].color);

    char Text[STRLEN];
    Text[0] = '\0';

    if (x_collapsed)
    {
        strcat(Text, "+");
        strcat(Text, text);
    }
    else
    {
        strcat(Text, "|");
        strcat(Text, text);
    }

    draw_text(Text, origin_x, origin_y, font_height);
}

void draw_Button_sels_text(const char * text, int width, int height, int index, int colorchange, int frame_it)
{
    int font_height = 11;

	FT_Set_Pixel_Sizes(face, 0, font_height);

	float origin_x = 5;
	float origin_y = BUTTON_HEIGHT * index + 10;

	if (frame_it)
    {
        glDisable(GL_TEXTURE_2D);
        glColor4fv(white);

        glBegin(GL_LINE_LOOP);
        glVertex2f(0, BUTTON_HEIGHT * index);
        glVertex2f(0, BUTTON_HEIGHT * index + BUTTON_HEIGHT);
        glVertex2f(width, BUTTON_HEIGHT * index + BUTTON_HEIGHT);
        glVertex2f(width, BUTTON_HEIGHT * index);
        glEnd();
    }
	glEnable(GL_TEXTURE_2D);

    if (colorchange)
        glColor4fv(buttoncolors[SelsList[index].color].color);
	else
        glColor4fv(buttoncolors[UI_BLACK].color);

    draw_text(text, origin_x, origin_y, font_height);
}

void draw_Button_text_text(const char * text, int width, int height, int index, int colorchange, int frame_it)
{
    int font_height = 11;

	FT_Set_Pixel_Sizes(face, 0, font_height);

	float origin_x = 5;
	float origin_y = BUTTON_HEIGHT * index + 10;

	if (frame_it)
    {
        glDisable(GL_TEXTURE_2D);
        glColor4fv(white);

        glBegin(GL_LINE_LOOP);
        glVertex2f(0, BUTTON_HEIGHT * index);
        glVertex2f(0, BUTTON_HEIGHT * index + BUTTON_HEIGHT);
        glVertex2f(width, BUTTON_HEIGHT * index + BUTTON_HEIGHT);
        glVertex2f(width, BUTTON_HEIGHT * index);
        glEnd();
    }
	glEnable(GL_TEXTURE_2D);

    if (colorchange)
        glColor4fv(buttoncolors[TextList[index].color].color);
	else
        glColor4fv(buttoncolors[UI_BLACK].color);

    draw_text(text, origin_x, origin_y, font_height);
}

void draw_Button_item_text(const char * text, int width, int height, int index, int colorchange, int selected)
{
	glEnable(GL_TEXTURE_2D);

    int font_height = 11;

	FT_Set_Pixel_Sizes(face, 0, font_height);

	float origin_x = 5;
	float origin_y = BUTTON_HEIGHT * index + 10;

	if (selected)
        glColor4fv(buttoncolors[UI_DIMSL].color);
    else if (colorchange)
        glColor4fv(buttoncolors[ItemList[index].color].color);
	else
        glColor4fv(buttoncolors[UI_BLACK].color);

    draw_text(text, origin_x, origin_y, font_height);
}

void draw_Button_text(const char * text, int width, int height, int index, int colorchange)
{
	glEnable(GL_TEXTURE_2D);

    int font_height = 11;

	FT_Set_Pixel_Sizes(face, 0, font_height);

	float origin_x = 5;
	float origin_y = BUTTON_HEIGHT * index + 10;

    if (colorchange)
        glColor4fv(buttoncolors[FileList[index].color].color);
	else
        glColor4fv(buttoncolors[UI_BLACK].color);

    draw_text(text, origin_x, origin_y, font_height);
}

void draw_Button_item(const char * text, int width, int height, int index, int colorchange)
{
    int v_dim = BUTTON_HEIGHT * (index + 1);
	/*draw frame*/

	//glDisable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);

    if (colorchange)
        glColor4fv(buttoncolors[Button_item[index].color].color);
	else
        glColor4fv(buttoncolors[UI_GRAYB].color);

	glBegin(GL_QUADS);
	glVertex2f(0, v_dim);
	glVertex2f(0, v_dim + BUTTON_HEIGHT);
	glVertex2f(width, v_dim + BUTTON_HEIGHT);
	glVertex2f(width, v_dim);
	glEnd();

	glColor4fv(buttoncolors[UI_WHITE].color);

	glBegin(GL_LINE_LOOP);
	glVertex2f(0, v_dim);
	glVertex2f(0, v_dim + BUTTON_HEIGHT);
	glVertex2f(width, v_dim + BUTTON_HEIGHT);
	glVertex2f(width, v_dim);
	glEnd();

	//glEnable(GL_TEXTURE_2D);

    int font_height = 11;

	FT_Set_Pixel_Sizes(face, 0, font_height);

	float origin_x = 5;
	float origin_y = BUTTON_HEIGHT * (index + 1) + 10;

	glColor4fv(buttoncolors[UI_BLACK].color);

    draw_text(text, origin_x, origin_y, font_height);
}

void draw_Button_deform(const char * text, int width, int height, int index, int colorchange)
{
    int v_dim = BUTTON_HEIGHT * (index + 1);
	/*draw frame*/

	//glDisable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);

    if (colorchange)
        glColor4fv(buttoncolors[Button_defr[index].color].color);
	else
        glColor4fv(buttoncolors[UI_GRAYB].color);

	glBegin(GL_QUADS);
	glVertex2f(0, v_dim);
	glVertex2f(0, v_dim + BUTTON_HEIGHT);
	glVertex2f(width, v_dim + BUTTON_HEIGHT);
	glVertex2f(width, v_dim);
	glEnd();

	glColor4fv(buttoncolors[UI_WHITE].color);

	glBegin(GL_LINE_LOOP);
	glVertex2f(0, v_dim);
	glVertex2f(0, v_dim + BUTTON_HEIGHT);
	glVertex2f(width, v_dim + BUTTON_HEIGHT);
	glVertex2f(width, v_dim);
	glEnd();

	//glEnable(GL_TEXTURE_2D);

    int font_height = 11;

	FT_Set_Pixel_Sizes(face, 0, font_height);

	float origin_x = 5;
	float origin_y = BUTTON_HEIGHT * (index + 1) + 10;

	glColor4fv(buttoncolors[UI_BLACK].color);

    draw_text(text, origin_x, origin_y, font_height);
}

void draw_Button_sels(const char * text, int width, int height, int index, int colorchange)
{
    int v_dim = BUTTON_HEIGHT * (index + 1);
	/*draw frame*/

	//glDisable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);

    if (colorchange)
        glColor4fv(buttoncolors[Button_sels[index].color].color);
	else
        glColor4fv(buttoncolors[UI_GRAYB].color);

	glBegin(GL_QUADS);
	glVertex2f(0, v_dim);
	glVertex2f(0, v_dim + BUTTON_HEIGHT);
	glVertex2f(width, v_dim + BUTTON_HEIGHT);
	glVertex2f(width, v_dim);
	glEnd();

	glColor4fv(buttoncolors[UI_WHITE].color);

	glBegin(GL_LINE_LOOP);
	glVertex2f(0, v_dim);
	glVertex2f(0, v_dim + BUTTON_HEIGHT);
	glVertex2f(width, v_dim + BUTTON_HEIGHT);
	glVertex2f(width, v_dim);
	glEnd();

	//glEnable(GL_TEXTURE_2D);

    int font_height = 11;

	FT_Set_Pixel_Sizes(face, 0, font_height);

	float origin_x = 5;
	float origin_y = BUTTON_HEIGHT * (index + 1) + 10;

	glColor4fv(buttoncolors[UI_BLACK].color);

    draw_text(text, origin_x, origin_y, font_height);
}

void draw_Button_scene_ext(const char * text, int width, int height, int index, int colorchange)
{
    int v_dim = BUTTON_HEIGHT * (index + 1);
	/*draw frame*/

	//glDisable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);

    if (colorchange)
        glColor4fv(buttoncolors[Button_scene_ext[index].color].color);
	else
        glColor4fv(buttoncolors[UI_GRAYB].color);

	glBegin(GL_QUADS);
	glVertex2f(0, v_dim);
	glVertex2f(0, v_dim + BUTTON_HEIGHT);
	glVertex2f(width, v_dim + BUTTON_HEIGHT);
	glVertex2f(width, v_dim);
	glEnd();

	glColor4fv(buttoncolors[UI_WHITE].color);

	glBegin(GL_LINE_LOOP);
	glVertex2f(0, v_dim);
	glVertex2f(0, v_dim + BUTTON_HEIGHT);
	glVertex2f(width, v_dim + BUTTON_HEIGHT);
	glVertex2f(width, v_dim);
	glEnd();

	//glEnable(GL_TEXTURE_2D);

    int font_height = 11;

	FT_Set_Pixel_Sizes(face, 0, font_height);

	float origin_x = 5;
	float origin_y = BUTTON_HEIGHT * (index + 1) + 10;

	glColor4fv(buttoncolors[UI_BLACK].color);

    draw_text(text, origin_x, origin_y, font_height);
}

void draw_Button_ext(const char * text, int width, int height, int index, int colorchange)
{
    int v_dim = BUTTON_HEIGHT * (index + 1);
	/*draw frame*/

	//glDisable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);

    if (colorchange)
        glColor4fv(buttoncolors[Button_ext[index].color].color);
	else
        glColor4fv(buttoncolors[UI_GRAYB].color);

	glBegin(GL_QUADS);
	glVertex2f(0, v_dim);
	glVertex2f(0, v_dim + BUTTON_HEIGHT);
	glVertex2f(width, v_dim + BUTTON_HEIGHT);
	glVertex2f(width, v_dim);
	glEnd();

	glColor4fv(buttoncolors[UI_WHITE].color);

	glBegin(GL_LINE_LOOP);
	glVertex2f(0, v_dim);
	glVertex2f(0, v_dim + BUTTON_HEIGHT);
	glVertex2f(width, v_dim + BUTTON_HEIGHT);
	glVertex2f(width, v_dim);
	glEnd();

	//glEnable(GL_TEXTURE_2D);

    int font_height = 11;

	FT_Set_Pixel_Sizes(face, 0, font_height);

	float origin_x = 5;
	float origin_y = BUTTON_HEIGHT * (index + 1) + 10;

	glColor4fv(buttoncolors[UI_BLACK].color);

    draw_text(text, origin_x, origin_y, font_height);
}

void draw_Collapsed(const char * text, int width, int collapsed, int index)
{
    int v_dim = BUTTON_HEIGHT * index;
	/*draw frame*/

	//glDisable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);

    glColor4fv(buttoncolors[UI_GRAYB].color);

    glEnable(GL_POLYGON_STIPPLE);

	glBegin(GL_QUADS);
	glVertex2f(3, v_dim + 3);
	glVertex2f(3, v_dim + BUTTON_HEIGHT - 3);
	glVertex2f(width - 3, v_dim + BUTTON_HEIGHT - 3);
	glVertex2f(width - 3, v_dim + 3);
	glEnd();

	glDisable(GL_POLYGON_STIPPLE);

	glColor4fv(buttoncolors[UI_WHITE].color);

	glBegin(GL_LINE_LOOP);
	glVertex2f(0, v_dim);
	glVertex2f(0, v_dim + BUTTON_HEIGHT);
	glVertex2f(width, v_dim + BUTTON_HEIGHT);
	glVertex2f(width, v_dim);
	glEnd();

	//glEnable(GL_TEXTURE_2D);

    int font_height = 11;

	FT_Set_Pixel_Sizes(face, 0, font_height);

	float origin_x = 5;
	float origin_y = BUTTON_HEIGHT * index + 10;

	glColor4fv(buttoncolors[UI_BLACK].color);
    char Text[TYPE_LENGTH];
    Text[0] = '\0';
    if (collapsed)
        strcat(Text, "> ");
    else
        strcat(Text, "< ");
    strcat(Text, text);

    draw_text(Text, origin_x, origin_y, font_height);
}

void draw_Button(const char * text, int width, int height, int index, int colorchange)
{
    int v_dim = BUTTON_HEIGHT * index;
	/*draw frame*/

	//glDisable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);

    if (colorchange)
        glColor4fv(buttoncolors[Button_sidebar[index].color].color);
	else
        glColor4fv(buttoncolors[UI_GRAYB].color);

	glBegin(GL_QUADS);
	glVertex2f(0, v_dim);
	glVertex2f(0, v_dim + BUTTON_HEIGHT);
	glVertex2f(width, v_dim + BUTTON_HEIGHT);
	glVertex2f(width, v_dim);
	glEnd();

	glColor4fv(buttoncolors[UI_WHITE].color);

	glBegin(GL_LINE_LOOP);
	glVertex2f(0, v_dim);
	glVertex2f(0, v_dim + BUTTON_HEIGHT);
	glVertex2f(width, v_dim + BUTTON_HEIGHT);
	glVertex2f(width, v_dim);
	glEnd();

	//glEnable(GL_TEXTURE_2D);

    int font_height = 11;

	FT_Set_Pixel_Sizes(face, 0, font_height);

	float origin_x = 5;
	float origin_y = BUTTON_HEIGHT * index + 10;

	glColor4fv(buttoncolors[UI_WHITE].color);

    draw_text(text, origin_x, origin_y, font_height);
}

void draw_Button_bone_horizontal(const char * text, int index, int colorchange)
{
    int h_dim = BUTTON_WIDTH_SHORT * index;
	/*draw frame*/

	//glDisable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);

    if (colorchange)
        glColor4fv(buttoncolors[Button_h_bone[index].color].color);
	else
        glColor4fv(buttoncolors[UI_GRAYB].color);

	glBegin(GL_QUADS);
	glVertex2f(h_dim, 0);
	glVertex2f(h_dim, BUTTON_HEIGHT);
	glVertex2f(h_dim + BUTTON_WIDTH_SHORT, BUTTON_HEIGHT);
	glVertex2f(h_dim + BUTTON_WIDTH_SHORT, 0);
	glEnd();

	glColor4fv(buttoncolors[UI_WHITE].color);

	glBegin(GL_LINE_LOOP);
	glVertex2f(h_dim, 0);
	glVertex2f(h_dim, BUTTON_HEIGHT);
	glVertex2f(h_dim + BUTTON_WIDTH_SHORT, BUTTON_HEIGHT);
	glVertex2f(h_dim + BUTTON_WIDTH_SHORT, 0);
	glEnd();

	//glEnable(GL_TEXTURE_2D);

    int font_height = 11;

	FT_Set_Pixel_Sizes(face, 0, font_height);

	float origin_x = 5 + h_dim;
	float origin_y = 10;

	glColor4fv(buttoncolors[UI_WHITE].color);

    draw_text(text, origin_x, origin_y, font_height);
}

void draw_Button_scene_horizontal(const char * text, int width, int colorchange)
{
    int h_dim = width - SIDEBAR - BUTTON_WIDTH_MEDIUM - 20;
	/*draw frame*/

	//glDisable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);

    if (colorchange)
        glColor4fv(buttoncolors[Button_h_scen[0].color].color);
	else
        glColor4fv(buttoncolors[UI_GRAYB].color);

	glBegin(GL_QUADS);
	glVertex2f(h_dim, 0);
	glVertex2f(h_dim, BUTTON_HEIGHT);
	glVertex2f(h_dim + BUTTON_WIDTH_MEDIUM, BUTTON_HEIGHT);
	glVertex2f(h_dim + BUTTON_WIDTH_MEDIUM, 0);
	glEnd();

	glColor4fv(buttoncolors[UI_WHITE].color);

	glBegin(GL_LINE_LOOP);
	glVertex2f(h_dim, 0);
	glVertex2f(h_dim, BUTTON_HEIGHT);
	glVertex2f(h_dim + BUTTON_WIDTH_MEDIUM, BUTTON_HEIGHT);
	glVertex2f(h_dim + BUTTON_WIDTH_MEDIUM, 0);
	glEnd();

	//glEnable(GL_TEXTURE_2D);

    int font_height = 11;

	FT_Set_Pixel_Sizes(face, 0, font_height);

	float origin_x = 5 + h_dim;
	float origin_y = 10;

	glColor4fv(buttoncolors[UI_WHITE].color);

    draw_text(text, origin_x, origin_y, font_height);
}

void draw_Button_pose_horizontal(const char * text, int index, int colorchange)
{
    int h_dim = BUTTON_WIDTH_SHORT * index;
	/*draw frame*/

	//glDisable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);

    if (colorchange)
        glColor4fv(buttoncolors[Button_h_pose[index].color].color);
	else
        glColor4fv(buttoncolors[UI_GRAYB].color);

	glBegin(GL_QUADS);
	glVertex2f(h_dim, 0);
	glVertex2f(h_dim, BUTTON_HEIGHT);
	glVertex2f(h_dim + BUTTON_WIDTH_SHORT, BUTTON_HEIGHT);
	glVertex2f(h_dim + BUTTON_WIDTH_SHORT, 0);
	glEnd();

	glColor4fv(buttoncolors[UI_WHITE].color);

	glBegin(GL_LINE_LOOP);
	glVertex2f(h_dim, 0);
	glVertex2f(h_dim, BUTTON_HEIGHT);
	glVertex2f(h_dim + BUTTON_WIDTH_SHORT, BUTTON_HEIGHT);
	glVertex2f(h_dim + BUTTON_WIDTH_SHORT, 0);
	glEnd();

	//glEnable(GL_TEXTURE_2D);

    int font_height = 11;

	FT_Set_Pixel_Sizes(face, 0, font_height);

	float origin_x = 5 + h_dim;
	float origin_y = 10;

	glColor4fv(buttoncolors[UI_WHITE].color);

    draw_text(text, origin_x, origin_y, font_height);
}

void draw_Button_defr_horizontal(const char * text, int index, int colorchange)
{
    int h_dim = BUTTON_WIDTH_SHORT * index;
	/*draw frame*/

	//glDisable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);

    if (colorchange)
        glColor4fv(buttoncolors[Button_h_defr[index].color].color);
	else
        glColor4fv(buttoncolors[UI_GRAYB].color);

	glBegin(GL_QUADS);
	glVertex2f(h_dim, 0);
	glVertex2f(h_dim, BUTTON_HEIGHT);
	glVertex2f(h_dim + BUTTON_WIDTH_SHORT, BUTTON_HEIGHT);
	glVertex2f(h_dim + BUTTON_WIDTH_SHORT, 0);
	glEnd();

	glColor4fv(buttoncolors[UI_WHITE].color);

	glBegin(GL_LINE_LOOP);
	glVertex2f(h_dim, 0);
	glVertex2f(h_dim, BUTTON_HEIGHT);
	glVertex2f(h_dim + BUTTON_WIDTH_SHORT, BUTTON_HEIGHT);
	glVertex2f(h_dim + BUTTON_WIDTH_SHORT, 0);
	glEnd();

	//glEnable(GL_TEXTURE_2D);

    int font_height = 11;

	FT_Set_Pixel_Sizes(face, 0, font_height);

	float origin_x = 5 + h_dim;
	float origin_y = 10;

	glColor4fv(buttoncolors[UI_WHITE].color);

    draw_text(text, origin_x, origin_y, font_height);
}

void draw_Button_hier_horizontal(const char * text, int index, int colorchange)
{
    int h_dim = BUTTON_WIDTH_MEDIUM * index;
	/*draw frame*/

	//glDisable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);

    if (colorchange)
        glColor4fv(buttoncolors[Button_h_hier[index].color].color);
	else
        glColor4fv(buttoncolors[UI_GRAYB].color);

	glBegin(GL_QUADS);
	glVertex2f(h_dim, 0);
	glVertex2f(h_dim, BUTTON_HEIGHT);
	glVertex2f(h_dim + BUTTON_WIDTH_MEDIUM, BUTTON_HEIGHT);
	glVertex2f(h_dim + BUTTON_WIDTH_MEDIUM, 0);
	glEnd();

	glColor4fv(buttoncolors[UI_WHITE].color);

	glBegin(GL_LINE_LOOP);
	glVertex2f(h_dim, 0);
	glVertex2f(h_dim, BUTTON_HEIGHT);
	glVertex2f(h_dim + BUTTON_WIDTH_MEDIUM, BUTTON_HEIGHT);
	glVertex2f(h_dim + BUTTON_WIDTH_MEDIUM, 0);
	glEnd();

	//glEnable(GL_TEXTURE_2D);

    int font_height = 11;

	FT_Set_Pixel_Sizes(face, 0, font_height);

	float origin_x = 5 + h_dim;
	float origin_y = 10;

	glColor4fv(buttoncolors[UI_WHITE].color);

    draw_text(text, origin_x, origin_y, font_height);
}

void draw_Button_sels_horizontal(const char * text, int index, int colorchange)
{
    int h_dim = BUTTON_WIDTH_SHORT * index;
	/*draw frame*/

	//glDisable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);

    if (colorchange)
        glColor4fv(buttoncolors[Button_h_sels[index].color].color);
	else
        glColor4fv(buttoncolors[UI_GRAYB].color);

	glBegin(GL_QUADS);
	glVertex2f(h_dim, 0);
	glVertex2f(h_dim, BUTTON_HEIGHT);
	glVertex2f(h_dim + BUTTON_WIDTH_SHORT, BUTTON_HEIGHT);
	glVertex2f(h_dim + BUTTON_WIDTH_SHORT, 0);
	glEnd();

	glColor4fv(buttoncolors[UI_WHITE].color);

	glBegin(GL_LINE_LOOP);
	glVertex2f(h_dim, 0);
	glVertex2f(h_dim, BUTTON_HEIGHT);
	glVertex2f(h_dim + BUTTON_WIDTH_SHORT, BUTTON_HEIGHT);
	glVertex2f(h_dim + BUTTON_WIDTH_SHORT, 0);
	glEnd();

	//glEnable(GL_TEXTURE_2D);

    int font_height = 11;

	FT_Set_Pixel_Sizes(face, 0, font_height);

	float origin_x = 5 + h_dim;
	float origin_y = 10;

	glColor4fv(buttoncolors[UI_WHITE].color);

    draw_text(text, origin_x, origin_y, font_height);
}

void draw_Button_horizontal(const char * text, int index, int colorchange)
{
    int h_dim = BUTTON_WIDTH * index;
	/*draw frame*/

	//glDisable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);

    if (colorchange)
        glColor4fv(buttoncolors[Button_Mode[index].color].color);
	else
        glColor4fv(buttoncolors[UI_GRAYB].color);

	glBegin(GL_QUADS);
	glVertex2f(h_dim, 0);
	glVertex2f(h_dim, BUTTON_HEIGHT);
	glVertex2f(h_dim + BUTTON_WIDTH, BUTTON_HEIGHT);
	glVertex2f(h_dim + BUTTON_WIDTH, 0);
	glEnd();

	glColor4fv(buttoncolors[UI_WHITE].color);

	glBegin(GL_LINE_LOOP);
	glVertex2f(h_dim, 0);
	glVertex2f(h_dim, BUTTON_HEIGHT);
	glVertex2f(h_dim + BUTTON_WIDTH, BUTTON_HEIGHT);
	glVertex2f(h_dim + BUTTON_WIDTH, 0);
	glEnd();

	//glEnable(GL_TEXTURE_2D);

    int font_height = 11;

	FT_Set_Pixel_Sizes(face, 0, font_height);

	float origin_x = 5 + h_dim;
	float origin_y = 10;

	glColor4fv(buttoncolors[UI_WHITE].color);

    draw_text(text, origin_x, origin_y, font_height);
}

void draw_Bones_List(int s_height, int start, int clear_background, int current_bone)
{
    int d_width = DIALOG_WIDTH - SIDEBAR;
    int d_height = DIALOG_HEIGHT - BUTTON_HEIGHT;
    glScissor(SIDEBAR * 2, s_height - d_height + BOTTOM_LINE, d_width, d_height);
    if (clear_background)
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glViewport(SIDEBAR * 2, s_height - d_height + BOTTOM_LINE, d_width, d_height);

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0, d_width, d_height, 0, 1, -1);

	glDisable(GL_LIGHTING);
	glDisable(GL_DEPTH_TEST);

	glBindTexture(GL_TEXTURE_2D, 0);

	glColor4fv(grayb_light);

	glBegin(GL_QUADS);
	glVertex2f(0, 0);
	glVertex2f(0, d_height);
	glVertex2f(d_width, d_height);
	glVertex2f(d_width, 0);
	glEnd();

	glColor4fv(white);

	glBegin(GL_LINE_LOOP);
	glVertex2f(0, 0);
	glVertex2f(0, d_height);
	glVertex2f(d_width, d_height);
	glVertex2f(d_width, 0);
	glEnd();

	char * bones_list[LISTLENGTH];

    int i;
	for (i = 0; i < LISTLENGTH; i ++)
    {
        bones_list[i] = malloc(255 * sizeof(char));
    }

    int s = list_bones(bones_list, start, LISTLENGTH);

	for (i = 0; i < s; i ++)
    {
        draw_Button_bone_text(bones_list[i], d_width, d_height, i, 1, 0);
    }

	for (i = 0; i < LISTLENGTH; i ++)
    {
        free(bones_list[i]);
    }

    glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glPopMatrix();
}

void draw_Poses_List(int s_height, int start, int clear_background, int current_pose)
{
    int d_width = DIALOG_WIDTH - SIDEBAR;
    int d_height = DIALOG_HEIGHT - BUTTON_HEIGHT;
    glScissor(SIDEBAR * 2, s_height - d_height + BOTTOM_LINE, d_width, d_height);
    if (clear_background)
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glViewport(SIDEBAR * 2, s_height - d_height + BOTTOM_LINE, d_width, d_height);

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0, d_width, d_height, 0, 1, -1);

	glDisable(GL_LIGHTING);
	glDisable(GL_DEPTH_TEST);

	glBindTexture(GL_TEXTURE_2D, 0);

	glColor4fv(grayb_light);

	glBegin(GL_QUADS);
	glVertex2f(0, 0);
	glVertex2f(0, d_height);
	glVertex2f(d_width, d_height);
	glVertex2f(d_width, 0);
	glEnd();

	glColor4fv(white);

	glBegin(GL_LINE_LOOP);
	glVertex2f(0, 0);
	glVertex2f(0, d_height);
	glVertex2f(d_width, d_height);
	glVertex2f(d_width, 0);
	glEnd();

	char * poses_list[LISTLENGTH];
	int pose_x_offset[LISTLENGTH];
	int pose_x_collapsed[LISTLENGTH];

    int i;
	for (i = 0; i < LISTLENGTH; i ++)
    {
        poses_list[i] = malloc(255 * sizeof(char));
    }

    int s = list_poses(poses_list, start, LISTLENGTH, pose_x_offset, pose_x_collapsed);

	for (i = 0; i < s; i ++)
    {
        if (i == selected_deformer_node - start)
            draw_Button_pose_text(poses_list[i], d_width, d_height, i, 1, 1, pose_x_offset[i], pose_x_collapsed[i]);
        else
            draw_Button_pose_text(poses_list[i], d_width, d_height, i, 1, 0, pose_x_offset[i], pose_x_collapsed[i]);
    }

	for (i = 0; i < LISTLENGTH; i ++)
    {
        free(poses_list[i]);
    }

    glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glPopMatrix();
}

void draw_Deformers_List(int s_height, int start, int clear_background, int current_deform)
{
    int d_width = DIALOG_WIDTH - SIDEBAR;
    int d_height = DIALOG_HEIGHT - BUTTON_HEIGHT;
    glScissor(SIDEBAR * 2, s_height - d_height + BOTTOM_LINE, d_width, d_height);
    if (clear_background)
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glViewport(SIDEBAR * 2, s_height - d_height + BOTTOM_LINE, d_width, d_height);

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0, d_width, d_height, 0, 1, -1);

	glDisable(GL_LIGHTING);
	glDisable(GL_DEPTH_TEST);

	//glDisable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);

	glColor4fv(grayb_light);

	glBegin(GL_QUADS);
	glVertex2f(0, 0);
	glVertex2f(0, d_height);
	glVertex2f(d_width, d_height);
	glVertex2f(d_width, 0);
	glEnd();

	glColor4fv(white);

	glBegin(GL_LINE_LOOP);
	glVertex2f(0, 0);
	glVertex2f(0, d_height);
	glVertex2f(d_width, d_height);
	glVertex2f(d_width, 0);
	glEnd();

	//glEnable(GL_TEXTURE_2D);

	// files list

	char * deformers_list[LISTLENGTH];
	int defr_x_offset[LISTLENGTH];
	int defr_x_collapsed[LISTLENGTH];

    int i;
	for (i = 0; i < LISTLENGTH; i ++)
    {
        deformers_list[i] = malloc(255 * sizeof(char));
    }

    int s = list_deformers(deformers_list, start, LISTLENGTH, defr_x_offset, defr_x_collapsed);

	for (i = 0; i < s; i ++)
    {
        if (i == selected_deformer_node - start)
            draw_Button_defr_text(deformers_list[i], d_width, d_height, i, 1, 1, defr_x_offset[i], defr_x_collapsed[i]);
        else
            draw_Button_defr_text(deformers_list[i], d_width, d_height, i, 1, 0, defr_x_offset[i], defr_x_collapsed[i]);
    }

	for (i = 0; i < LISTLENGTH; i ++)
    {
        free(deformers_list[i]);
    }

    glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glPopMatrix();
}

void draw_Hierarchys_List(int s_height, int start, int clear_background, int current_trans)
{
    int d_width = DIALOG_WIDTH - SIDEBAR;
    int d_height = DIALOG_HEIGHT - BUTTON_HEIGHT;
    glScissor(SIDEBAR * 2, s_height - d_height + BOTTOM_LINE, d_width, d_height);
    if (clear_background)
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glViewport(SIDEBAR * 2, s_height - d_height + BOTTOM_LINE, d_width, d_height);

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0, d_width, d_height, 0, 1, -1);

	glDisable(GL_LIGHTING);
	glDisable(GL_DEPTH_TEST);

	//glDisable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);

	glColor4fv(grayb_light);

	glBegin(GL_QUADS);
	glVertex2f(0, 0);
	glVertex2f(0, d_height);
	glVertex2f(d_width, d_height);
	glVertex2f(d_width, 0);
	glEnd();

	glColor4fv(white);

	glBegin(GL_LINE_LOOP);
	glVertex2f(0, 0);
	glVertex2f(0, d_height);
	glVertex2f(d_width, d_height);
	glVertex2f(d_width, 0);
	glEnd();

	//glEnable(GL_TEXTURE_2D);

	// files list

	char * hierarchys_list[LISTLENGTH];
	int hier_x_offset[LISTLENGTH];
	int hier_x_collapsed[LISTLENGTH];

    int i;
	for (i = 0; i < LISTLENGTH; i ++)
    {
        hierarchys_list[i] = malloc(255 * sizeof(char));
    }

    int s = list_hierarcys(hierarchys_list, start, LISTLENGTH, hier_x_offset, hier_x_collapsed);

	for (i = 0; i < s; i ++)
    {
        draw_Button_hier_text(hierarchys_list[i], d_width, d_height, i, 1, 0, hier_x_offset[i], hier_x_collapsed[i]);
    }

	for (i = 0; i < LISTLENGTH; i ++)
    {
        free(hierarchys_list[i]);
    }

    glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glPopMatrix();
}

void draw_Selections_List(int s_height, int start, char * type, int clear_background, int update_edit, int current_sel)
{
    int d_width = DIALOG_WIDTH - SIDEBAR;
    int d_height = DIALOG_HEIGHT - BUTTON_HEIGHT;
    glScissor(SIDEBAR * 2, s_height - d_height + BOTTOM_LINE, d_width, d_height);
    if (clear_background)
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glViewport(SIDEBAR * 2, s_height - d_height + BOTTOM_LINE, d_width, d_height);

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0, d_width, d_height, 0, 1, -1);

	glDisable(GL_LIGHTING);
	glDisable(GL_DEPTH_TEST);

	//glDisable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);

	glColor4fv(grayb_light);

	glBegin(GL_QUADS);
	glVertex2f(0, 0);
	glVertex2f(0, d_height);
	glVertex2f(d_width, d_height);
	glVertex2f(d_width, 0);
	glEnd();

	glColor4fv(white);

	glBegin(GL_LINE_LOOP);
	glVertex2f(0, 0);
	glVertex2f(0, d_height);
	glVertex2f(d_width, d_height);
	glVertex2f(d_width, 0);
	glEnd();

	//glEnable(GL_TEXTURE_2D);

	// files list

	char * selections_list[LISTLENGTH];

    int i;
	for (i = 0; i < LISTLENGTH; i ++)
    {
        selections_list[i] = malloc(255 * sizeof(char));
    }

    int s = list_selections(selections_list, start, LISTLENGTH, type);

	for (i = 0; i < s; i ++)
    {
        if (update_edit && i == current_sel)
            draw_Button_sels_text(selections_list[i], d_width, d_height, i, 1, 1);
        else
            draw_Button_sels_text(selections_list[i], d_width, d_height, i, 1, 0);
    }

	for (i = 0; i < LISTLENGTH; i ++)
    {
        free(selections_list[i]);
    }

    glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glPopMatrix();
}

void draw_Textures_List(int s_height, int start, int clear_background)
{
    int d_width = DIALOG_WIDTH - SIDEBAR;
    int d_height = DIALOG_HEIGHT - BUTTON_HEIGHT;
    glScissor(SIDEBAR * 2, s_height - d_height + BOTTOM_LINE, d_width, d_height);
    if (clear_background)
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glViewport(SIDEBAR * 2, s_height - d_height + BOTTOM_LINE, d_width, d_height);

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0, d_width, d_height, 0, 1, -1);

	glDisable(GL_LIGHTING);
	glDisable(GL_DEPTH_TEST);

	//glDisable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);

	glColor4fv(grayb_light);

	glBegin(GL_QUADS);
	glVertex2f(0, 0);
	glVertex2f(0, d_height);
	glVertex2f(d_width, d_height);
	glVertex2f(d_width, 0);
	glEnd();

	glColor4fv(white);

	glBegin(GL_LINE_LOOP);
	glVertex2f(0, 0);
	glVertex2f(0, d_height);
	glVertex2f(d_width, d_height);
	glVertex2f(d_width, 0);
	glEnd();

	//glEnable(GL_TEXTURE_2D);

	// files list

	char * text_list[LISTLENGTH];

    int i;
	for (i = 0; i < LISTLENGTH; i ++)
    {
        text_list[i] = malloc(255 * sizeof(char));
    }

    int s = list_texts(text_list, start, LISTLENGTH);

	for (i = 0; i < s; i ++)
    {
        draw_Button_text_text(text_list[i], d_width, d_height, i, 1, 0);
    }

	for (i = 0; i < LISTLENGTH; i ++)
    {
        free(text_list[i]);
    }

    glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glPopMatrix();
}

void draw_Items_List(int s_height, int start, char * type, int clear_background, int currentObject)
{
    int d_width = DIALOG_WIDTH - SIDEBAR;
    int d_height = DIALOG_HEIGHT - BUTTON_HEIGHT;
    glScissor(SIDEBAR * 2, s_height - d_height + BOTTOM_LINE, d_width, d_height);
    if (clear_background)
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glViewport(SIDEBAR * 2, s_height - d_height + BOTTOM_LINE, d_width, d_height);

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0, d_width, d_height, 0, 1, -1);

	glDisable(GL_LIGHTING);
	glDisable(GL_DEPTH_TEST);

	//glDisable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);

	glColor4fv(grayb_light);

	glBegin(GL_QUADS);
	glVertex2f(0, 0);
	glVertex2f(0, d_height);
	glVertex2f(d_width, d_height);
	glVertex2f(d_width, 0);
	glEnd();

	glColor4fv(white);

	glBegin(GL_LINE_LOOP);
	glVertex2f(0, 0);
	glVertex2f(0, d_height);
	glVertex2f(d_width, d_height);
	glVertex2f(d_width, 0);
	glEnd();

	//glEnable(GL_TEXTURE_2D);

	// files list

	char * item_list[LISTLENGTH];

    int i;
	for (i = 0; i < LISTLENGTH; i ++)
    {
        item_list[i] = malloc(255 * sizeof(char));
    }

    int selected_items[LISTLENGTH];

    int s = list_items(item_list, start, LISTLENGTH, type, selected_items, currentObject);

	for (i = 0; i < s; i ++)
    {
        draw_Button_item_text(item_list[i], d_width, d_height, i, 1, selected_items[i]);
    }

	for (i = 0; i < LISTLENGTH; i ++)
    {
        free(item_list[i]);
    }

    glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glPopMatrix();
}

void draw_Button_path(char * path, int Edit_Lock)
{
	glEnable(GL_TEXTURE_2D);

    int font_height = 12;

	FT_Set_Pixel_Sizes(face, 0, font_height);

	float origin_x = 5 + BUTTON_WIDTH;
	float origin_y = DIALOG_HEIGHT - BUTTON_HEIGHT + 10;

	if (Edit_Lock)
        glColor4fv(buttoncolors[UI_YELLO].color);
    else
        glColor4fv(buttoncolors[UI_BLACK].color);

    draw_text(path, origin_x, origin_y, font_height);
}

void draw_Files_Box(int s_height, int clear_background)
{
    int d_width = DIALOG_WIDTH - SIDEBAR;
    int d_height = DIALOG_HEIGHT - BUTTON_HEIGHT;
    glScissor(SIDEBAR * 2, s_height - d_height + BOTTOM_LINE, d_width, d_height);
    if (clear_background)
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glViewport(SIDEBAR * 2, s_height - d_height + BOTTOM_LINE, d_width, d_height);

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0, d_width, d_height, 0, 1, -1);

	glDisable(GL_LIGHTING);
	glDisable(GL_DEPTH_TEST);

	//glDisable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);

	glColor4fv(grayb_light);

	glBegin(GL_QUADS);
	glVertex2f(0, 0);
	glVertex2f(0, d_height);
	glVertex2f(d_width, d_height);
	glVertex2f(d_width, 0);
	glEnd();

	glColor4fv(white);

	glBegin(GL_LINE_LOOP);
	glVertex2f(0, 0);
	glVertex2f(0, d_height);
	glVertex2f(d_width, d_height);
	glVertex2f(d_width, 0);
	glEnd();

	//glEnable(GL_TEXTURE_2D);

    glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glPopMatrix();
}

void draw_Files_List(char * path, int s_height, int start, char * ext, int clear_background, int UpdateFilesList)
{
    int d_width = DIALOG_WIDTH - SIDEBAR;
    int d_height = DIALOG_HEIGHT - BUTTON_HEIGHT;
    glScissor(SIDEBAR * 2, s_height - d_height + BOTTOM_LINE, d_width, d_height);
    if (clear_background)
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glViewport(SIDEBAR * 2, s_height - d_height + BOTTOM_LINE, d_width, d_height);

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0, d_width, d_height, 0, 1, -1);

	glDisable(GL_LIGHTING);
	glDisable(GL_DEPTH_TEST);

	//glDisable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);

	glColor4fv(grayb_light);

	glBegin(GL_QUADS);
	glVertex2f(0, 0);
	glVertex2f(0, d_height);
	glVertex2f(d_width, d_height);
	glVertex2f(d_width, 0);
	glEnd();

	glColor4fv(white);

	glBegin(GL_LINE_LOOP);
	glVertex2f(0, 0);
	glVertex2f(0, d_height);
	glVertex2f(d_width, d_height);
	glVertex2f(d_width, 0);
	glEnd();

	//glEnable(GL_TEXTURE_2D);

	// files list

    int i, s;

	if (UpdateFilesList)
    {
        //printf("updating files list\n");
        char * out_list[LISTLENGTH];

        for (i = 0; i < LISTLENGTH; i ++)
        {
            out_list[i] = malloc(255 * sizeof(char));
        }

        s = list_directory(path, out_list, start, LISTLENGTH, ext);

        for (i = 0; i < s; i ++)
        {
            draw_Button_text(out_list[i], d_width, d_height, i, 1);
        }

        for (i = 0; i < LISTLENGTH; i ++)
        {
            free(out_list[i]);
        }
    }
    else
    {
        //printf("using Out List\n");
        s = start + LISTLENGTH;
        if (s > files_num) s = files_num;
        for (i = start; i < s; i ++)
        {
            draw_Button_text(Out_List[i], d_width, d_height, i - start, 1);
        }
    }

    glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glPopMatrix();
}

void draw_Textures_Dialog(const char * text, int s_height, int texts_start, int clear_background)
{
    int d_width = DIALOG_WIDTH;
    int d_height = DIALOG_HEIGHT;
    glScissor(SIDEBAR, s_height - d_height + BOTTOM_LINE, d_width, d_height);
    if (clear_background)
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glViewport(SIDEBAR, s_height - d_height + BOTTOM_LINE, d_width, d_height);

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0, d_width, d_height, 0, 1, -1);

	glDisable(GL_LIGHTING);
	glDisable(GL_DEPTH_TEST);

	//glDisable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);

	glColor4fv(backg);

	glBegin(GL_QUADS);
	glVertex2f(0, 0);
	glVertex2f(0, d_height);
	glVertex2f(d_width, d_height);
	glVertex2f(d_width, 0);
	glEnd();

	glColor4fv(white);

	glBegin(GL_LINE_LOOP);
	glVertex2f(0, 0);
	glVertex2f(0, d_height);
	glVertex2f(d_width, d_height);
	glVertex2f(d_width, 0);
	glEnd();

	glColor4fv(white);

	glBegin(GL_LINE_LOOP);
	glVertex2f(0, BUTTON_HEIGHT);
	glVertex2f(0, d_height);
	glVertex2f(SIDEBAR, d_height);
	glVertex2f(SIDEBAR, BUTTON_HEIGHT);
	glEnd();

	//glEnable(GL_TEXTURE_2D);

	draw_Button(text, SIDEBAR, d_height, 0, 0); // Title bar

	draw_Textures_List(s_height, texts_start, clear_background);

    glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glPopMatrix();
}

void draw_Items_Dialog(const char * text, int s_height, char * item, char ** items, int items_count, int items_start, int clear_background, int currentObject)
{
    int d_width = DIALOG_WIDTH;
    int d_height = DIALOG_HEIGHT;
    glScissor(SIDEBAR, s_height - d_height + BOTTOM_LINE, d_width, d_height);
    if (clear_background)
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glViewport(SIDEBAR, s_height - d_height + BOTTOM_LINE, d_width, d_height);

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0, d_width, d_height, 0, 1, -1);

	glDisable(GL_LIGHTING);
	glDisable(GL_DEPTH_TEST);

	//glDisable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);

	glColor4fv(backg);

	glBegin(GL_QUADS);
	glVertex2f(0, 0);
	glVertex2f(0, d_height);
	glVertex2f(d_width, d_height);
	glVertex2f(d_width, 0);
	glEnd();

	glColor4fv(white);

	glBegin(GL_LINE_LOOP);
	glVertex2f(0, 0);
	glVertex2f(0, d_height);
	glVertex2f(d_width, d_height);
	glVertex2f(d_width, 0);
	glEnd();

	glColor4fv(white);

	glBegin(GL_LINE_LOOP);
	glVertex2f(0, BUTTON_HEIGHT);
	glVertex2f(0, d_height);
	glVertex2f(SIDEBAR, d_height);
	glVertex2f(SIDEBAR, BUTTON_HEIGHT);
	glEnd();

	//glEnable(GL_TEXTURE_2D);

	draw_Button(text, SIDEBAR, d_height, 0, 0); // Title bar

	int i;
	for (i = 0; i < items_count; i ++)
    {
        draw_Button_item(items[i], SIDEBAR, d_height, i, 1);
    }

	draw_Items_List(s_height, items_start, item, clear_background, currentObject);

    glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glPopMatrix();
}

void draw_Bones_Bottom_Line(int width, int height)
{
    glScissor(SIDEBAR * 2, height - DIALOG_HEIGHT + BOTTOM_LINE, width, BUTTON_HEIGHT);
    //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glViewport(SIDEBAR * 2, height - DIALOG_HEIGHT + BOTTOM_LINE, width, BUTTON_HEIGHT);

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0, width, BOTTOM_LINE, 0, 1, -1);

	glDisable(GL_LIGHTING);
	glDisable(GL_DEPTH_TEST);

    draw_Button_bone_horizontal("Remove", 0, 1);
    draw_Button_bone_horizontal("Rename", 1, 1);

    glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glPopMatrix();
}

void draw_Scenes_Bottom_Line(char * Label, int width, int height)
{
    glScissor(SIDEBAR * 2, height - DIALOG_HEIGHT + BOTTOM_LINE, width, BUTTON_HEIGHT);
    //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glViewport(SIDEBAR * 2, height - DIALOG_HEIGHT + BOTTOM_LINE, width, BUTTON_HEIGHT);

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0, width, BOTTOM_LINE, 0, 1, -1);

	glDisable(GL_LIGHTING);
	glDisable(GL_DEPTH_TEST);

    draw_Button_scene_horizontal(Label, width, 1);

    glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glPopMatrix();
}

void draw_Poses_Bottom_Line(int width, int height)
{
    glScissor(SIDEBAR * 2, height - DIALOG_HEIGHT + BOTTOM_LINE, width, BUTTON_HEIGHT);
    //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glViewport(SIDEBAR * 2, height - DIALOG_HEIGHT + BOTTOM_LINE, width, BUTTON_HEIGHT);

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0, width, BOTTOM_LINE, 0, 1, -1);

	glDisable(GL_LIGHTING);
	glDisable(GL_DEPTH_TEST);

    draw_Button_pose_horizontal("Add", 0, 1);
    draw_Button_pose_horizontal("Remove", 1, 1);
    draw_Button_pose_horizontal("Rename", 2, 1);
    draw_Button_pose_horizontal("Update", 3, 1);

    glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glPopMatrix();
}

void draw_Deformers_Bottom_Line(int width, int height)
{
    glScissor(SIDEBAR * 2, height - DIALOG_HEIGHT + BOTTOM_LINE, width, BUTTON_HEIGHT);
    //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glViewport(SIDEBAR * 2, height - DIALOG_HEIGHT + BOTTOM_LINE, width, BUTTON_HEIGHT);

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0, width, BOTTOM_LINE, 0, 1, -1);

	glDisable(GL_LIGHTING);
	glDisable(GL_DEPTH_TEST);

    draw_Button_defr_horizontal("Add", 0, 1);
    draw_Button_defr_horizontal("Remove", 1, 1);
    draw_Button_defr_horizontal("Bind", 2, 1);
    draw_Button_defr_horizontal("Unbind", 3, 1);
    draw_Button_defr_horizontal("New", 4, 1);
    draw_Button_defr_horizontal("Delete", 5, 1);

    glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glPopMatrix();
}

void draw_Hierarchys_Bottom_Line(int width, int height)
{
    glScissor(SIDEBAR * 2, height - DIALOG_HEIGHT + BOTTOM_LINE, width, BUTTON_HEIGHT);
    //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glViewport(SIDEBAR * 2, height - DIALOG_HEIGHT + BOTTOM_LINE, width, BUTTON_HEIGHT);

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0, width, BOTTOM_LINE, 0, 1, -1);

	glDisable(GL_LIGHTING);
	glDisable(GL_DEPTH_TEST);

    draw_Button_hier_horizontal("Edit", 0, 1);
    draw_Button_hier_horizontal("Parent", 1, 1);
    draw_Button_hier_horizontal("Unparent", 2, 1);
    draw_Button_hier_horizontal("New", 3, 1);
    draw_Button_hier_horizontal("Delete", 4, 1);

    glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glPopMatrix();
}

void draw_Selections_Bottom_Line(int width, int height)
{
    glScissor(SIDEBAR * 2, height - DIALOG_HEIGHT + BOTTOM_LINE, width, BUTTON_HEIGHT);
    //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glViewport(SIDEBAR * 2, height - DIALOG_HEIGHT + BOTTOM_LINE, width, BUTTON_HEIGHT);

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0, width, BOTTOM_LINE, 0, 1, -1);

	glDisable(GL_LIGHTING);
	glDisable(GL_DEPTH_TEST);

    draw_Button_sels_horizontal("Add", 0, 1);
    draw_Button_sels_horizontal("Edit", 1, 1);
    draw_Button_sels_horizontal("Remove", 2, 1);
    draw_Button_sels_horizontal("Assign", 3, 1);
    draw_Button_sels_horizontal("Unass.", 4, 1);
    draw_Button_sels_horizontal("Select", 5, 1);

    glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glPopMatrix();
}

void draw_dialog_Box(int s_height, int clear_background, int frame)
{
    int d_width = DIALOG_WIDTH;
    int d_height = DIALOG_HEIGHT;
    glScissor(SIDEBAR, s_height - d_height + BOTTOM_LINE, d_width, d_height);
    if (clear_background)
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glViewport(SIDEBAR, s_height - d_height + BOTTOM_LINE, d_width, d_height);

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0, d_width, d_height, 0, 1, -1);

	glDisable(GL_LIGHTING);
	glDisable(GL_DEPTH_TEST);

	glBindTexture(GL_TEXTURE_2D, 0);

	glColor4fv(backg);

	glBegin(GL_QUADS);
	glVertex2f(0, 0);
	glVertex2f(0, d_height);
	glVertex2f(d_width, d_height);
	glVertex2f(d_width, 0);
	glEnd();

	glColor4fv(white);

	glBegin(GL_LINE_LOOP);
	glVertex2f(0, 0);
	glVertex2f(0, d_height);
	glVertex2f(d_width, d_height);
	glVertex2f(d_width, 0);
	glEnd();

	glColor4fv(white);

	glBegin(GL_LINE_LOOP);
	glVertex2f(0, BUTTON_HEIGHT);
	glVertex2f(0, d_height);
	glVertex2f(SIDEBAR, d_height);
	glVertex2f(SIDEBAR, BUTTON_HEIGHT);
	glEnd();

	glColor4fv(grayb_light);

	glBegin(GL_QUADS);
	glVertex2f(SIDEBAR, 0);
	glVertex2f(SIDEBAR, d_height - BUTTON_HEIGHT);
	glVertex2f(d_width, d_height - BUTTON_HEIGHT);
	glVertex2f(d_width, 0);
	glEnd();

	glColor4fv(white);

	glBegin(GL_LINE_LOOP);
	glVertex2f(SIDEBAR, 0);
	glVertex2f(SIDEBAR, d_height);
	glVertex2f(d_width, d_height);
	glVertex2f(d_width, 0);
	glEnd();

	glBegin(GL_LINE_LOOP);
	glVertex2f(SIDEBAR, d_height - BUTTON_HEIGHT);
	glVertex2f(SIDEBAR, d_height);
	glVertex2f(d_width, d_height);
	glVertex2f(d_width, d_height - BUTTON_HEIGHT);
	glEnd();

	glColor4fv(buttoncolors[UI_GRAYB].color);

	glBegin(GL_QUADS);
	glVertex2f(SIDEBAR, d_height - BUTTON_HEIGHT);
	glVertex2f(SIDEBAR, d_height);
	glVertex2f(d_width / frame, d_height);
	glVertex2f(d_width / frame, d_height - BUTTON_HEIGHT);
	glEnd();

	glEnable(GL_LIGHTING);
	glEnable(GL_DEPTH_TEST);
}

void draw_Bones_Dialog(const char * text, int s_height,
                           int bones_start,
                           int clear_background, int current_bone)
{
    int d_width = DIALOG_WIDTH;
    int d_height = DIALOG_HEIGHT;
    glScissor(SIDEBAR, s_height - d_height + BOTTOM_LINE, d_width, d_height);
    if (clear_background)
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glViewport(SIDEBAR, s_height - d_height + BOTTOM_LINE, d_width, d_height);

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0, d_width, d_height, 0, 1, -1);

	glDisable(GL_LIGHTING);
	glDisable(GL_DEPTH_TEST);

	glBindTexture(GL_TEXTURE_2D, 0);

	glColor4fv(backg);

	glBegin(GL_QUADS);
	glVertex2f(0, 0);
	glVertex2f(0, d_height);
	glVertex2f(d_width, d_height);
	glVertex2f(d_width, 0);
	glEnd();

	glColor4fv(white);

	glBegin(GL_LINE_LOOP);
	glVertex2f(0, 0);
	glVertex2f(0, d_height);
	glVertex2f(d_width, d_height);
	glVertex2f(d_width, 0);
	glEnd();

	glColor4fv(white);

	glBegin(GL_LINE_LOOP);
	glVertex2f(0, BUTTON_HEIGHT);
	glVertex2f(0, d_height);
	glVertex2f(SIDEBAR, d_height);
	glVertex2f(SIDEBAR, BUTTON_HEIGHT);
	glEnd();

	draw_Button(text, SIDEBAR, d_height, 0, 0); // Title bar

    draw_Bones_List(s_height, bones_start, clear_background, current_bone);

    glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glPopMatrix();

    draw_Bones_Bottom_Line(d_width, s_height);
}

void draw_Poses_Dialog(const char * text, int s_height,
                           int poses_start,
                           int clear_background, int current_pose)
{
    int d_width = DIALOG_WIDTH;
    int d_height = DIALOG_HEIGHT;
    glScissor(SIDEBAR, s_height - d_height + BOTTOM_LINE, d_width, d_height);
    if (clear_background)
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glViewport(SIDEBAR, s_height - d_height + BOTTOM_LINE, d_width, d_height);

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0, d_width, d_height, 0, 1, -1);

	glDisable(GL_LIGHTING);
	glDisable(GL_DEPTH_TEST);

	glBindTexture(GL_TEXTURE_2D, 0);

	glColor4fv(backg);

	glBegin(GL_QUADS);
	glVertex2f(0, 0);
	glVertex2f(0, d_height);
	glVertex2f(d_width, d_height);
	glVertex2f(d_width, 0);
	glEnd();

	glColor4fv(white);

	glBegin(GL_LINE_LOOP);
	glVertex2f(0, 0);
	glVertex2f(0, d_height);
	glVertex2f(d_width, d_height);
	glVertex2f(d_width, 0);
	glEnd();

	glColor4fv(white);

	glBegin(GL_LINE_LOOP);
	glVertex2f(0, BUTTON_HEIGHT);
	glVertex2f(0, d_height);
	glVertex2f(SIDEBAR, d_height);
	glVertex2f(SIDEBAR, BUTTON_HEIGHT);
	glEnd();

	draw_Button(text, SIDEBAR, d_height, 0, 0); // Title bar

    draw_Poses_List(s_height, poses_start, clear_background, current_pose);

    glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glPopMatrix();

    draw_Poses_Bottom_Line(d_width, s_height);
}

void draw_Deformers_Dialog(const char * text, int s_height,
                           char * defr_type, char ** defr_types, int defr_type_count, int deformers_start,
                           int clear_background, int current_deformer,
                           int transformers_start, int current_trans,
                           int selections_start, int current_sel)
{
    int d_width = DIALOG_WIDTH;
    int d_height = DIALOG_HEIGHT;
    glScissor(SIDEBAR, s_height - d_height + BOTTOM_LINE, d_width, d_height);
    if (clear_background)
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glViewport(SIDEBAR, s_height - d_height + BOTTOM_LINE, d_width, d_height);

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0, d_width, d_height, 0, 1, -1);

	glDisable(GL_LIGHTING);
	glDisable(GL_DEPTH_TEST);

	//glDisable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);

	glColor4fv(backg);

	glBegin(GL_QUADS);
	glVertex2f(0, 0);
	glVertex2f(0, d_height);
	glVertex2f(d_width, d_height);
	glVertex2f(d_width, 0);
	glEnd();

	glColor4fv(white);

	glBegin(GL_LINE_LOOP);
	glVertex2f(0, 0);
	glVertex2f(0, d_height);
	glVertex2f(d_width, d_height);
	glVertex2f(d_width, 0);
	glEnd();

	glColor4fv(white);

	glBegin(GL_LINE_LOOP);
	glVertex2f(0, BUTTON_HEIGHT);
	glVertex2f(0, d_height);
	glVertex2f(SIDEBAR, d_height);
	glVertex2f(SIDEBAR, BUTTON_HEIGHT);
	glEnd();

	//glEnable(GL_TEXTURE_2D);

	draw_Button(text, SIDEBAR, d_height, 0, 0); // Title bar

	int s;
	for (s = 0; s < defr_type_count; s ++)
    {
        draw_Button_deform(defr_types[s], SIDEBAR, d_height, s, 1);
    }

    if (strcmp(defr_type, "deformers") == 0)
        draw_Deformers_List(s_height, deformers_start, clear_background, current_deformer);
    else if (strcmp(defr_type, "hierarchys") == 0)
        draw_Hierarchys_List(s_height, transformers_start, clear_background, current_trans);
    else if (strcmp(defr_type, "selections") == 0)
        draw_Selections_List(s_height, selections_start, "vertex", clear_background, 1, current_sel);

    glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glPopMatrix();

    if (strcmp(defr_type, "deformers") == 0)
        draw_Deformers_Bottom_Line(d_width, s_height);
    else if (strcmp(defr_type, "hierarchys") == 0)
        draw_Hierarchys_Bottom_Line(d_width, s_height);
    else if (strcmp(defr_type, "selections") == 0)
        draw_Selections_Bottom_Line(d_width, s_height);
}

void draw_Hierarchys_Dialog(const char * text, int s_height, int transformers_start, int clear_background, int current_trans)
{
    int d_width = DIALOG_WIDTH;
    int d_height = DIALOG_HEIGHT;
    glScissor(SIDEBAR, s_height - d_height + BOTTOM_LINE, d_width, d_height);
    if (clear_background)
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glViewport(SIDEBAR, s_height - d_height + BOTTOM_LINE, d_width, d_height);

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0, d_width, d_height, 0, 1, -1);

	glDisable(GL_LIGHTING);
	glDisable(GL_DEPTH_TEST);

	//glDisable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);

	glColor4fv(backg);

	glBegin(GL_QUADS);
	glVertex2f(0, 0);
	glVertex2f(0, d_height);
	glVertex2f(d_width, d_height);
	glVertex2f(d_width, 0);
	glEnd();

	glColor4fv(white);

	glBegin(GL_LINE_LOOP);
	glVertex2f(0, 0);
	glVertex2f(0, d_height);
	glVertex2f(d_width, d_height);
	glVertex2f(d_width, 0);
	glEnd();

	glColor4fv(white);

	glBegin(GL_LINE_LOOP);
	glVertex2f(0, BUTTON_HEIGHT);
	glVertex2f(0, d_height);
	glVertex2f(SIDEBAR, d_height);
	glVertex2f(SIDEBAR, BUTTON_HEIGHT);
	glEnd();

	//glEnable(GL_TEXTURE_2D);

	draw_Button(text, SIDEBAR, d_height, 0, 0); // Title bar

	draw_Hierarchys_List(s_height, transformers_start, clear_background, current_trans);

    glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glPopMatrix();

	draw_Hierarchys_Bottom_Line(d_width, s_height);
}

void draw_Selections_Dialog(const char * text, int s_height, char * sel_type, char ** sel_types, int sel_type_count, int selections_start, int clear_background, int current_sel)
{
    int d_width = DIALOG_WIDTH;
    int d_height = DIALOG_HEIGHT;
    glScissor(SIDEBAR, s_height - d_height + BOTTOM_LINE, d_width, d_height);
    if (clear_background)
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glViewport(SIDEBAR, s_height - d_height + BOTTOM_LINE, d_width, d_height);

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0, d_width, d_height, 0, 1, -1);

	glDisable(GL_LIGHTING);
	glDisable(GL_DEPTH_TEST);

	//glDisable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);

	glColor4fv(backg);

	glBegin(GL_QUADS);
	glVertex2f(0, 0);
	glVertex2f(0, d_height);
	glVertex2f(d_width, d_height);
	glVertex2f(d_width, 0);
	glEnd();

	glColor4fv(white);

	glBegin(GL_LINE_LOOP);
	glVertex2f(0, 0);
	glVertex2f(0, d_height);
	glVertex2f(d_width, d_height);
	glVertex2f(d_width, 0);
	glEnd();

	glColor4fv(white);

	glBegin(GL_LINE_LOOP);
	glVertex2f(0, BUTTON_HEIGHT);
	glVertex2f(0, d_height);
	glVertex2f(SIDEBAR, d_height);
	glVertex2f(SIDEBAR, BUTTON_HEIGHT);
	glEnd();

	//glEnable(GL_TEXTURE_2D);

	draw_Button(text, SIDEBAR, d_height, 0, 0); // Title bar

	int s;
	for (s = 0; s < sel_type_count; s ++)
    {
        draw_Button_sels(sel_types[s], SIDEBAR, d_height, s, 1);
    }

	draw_Selections_List(s_height, selections_start, sel_type, clear_background, 1, current_sel);

    glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glPopMatrix();

	draw_Selections_Bottom_Line(d_width, s_height);
}

void draw_Loading_Dialog(char * path, const char * text, int s_height, char * ext, char ** extensions, int ext_count, int files_start, int clear_background, int update, int Edit_Lock)
{
    int d_width = DIALOG_WIDTH;
    int d_height = DIALOG_HEIGHT;
    glScissor(SIDEBAR, s_height - d_height + BOTTOM_LINE, d_width, d_height);
    if (clear_background)
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glViewport(SIDEBAR, s_height - d_height + BOTTOM_LINE, d_width, d_height);

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0, d_width, d_height, 0, 1, -1);

	glDisable(GL_LIGHTING);
	glDisable(GL_DEPTH_TEST);

	//glDisable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);

	glColor4fv(backg);

	glBegin(GL_QUADS);
	glVertex2f(0, 0);
	glVertex2f(0, d_height);
	glVertex2f(d_width, d_height);
	glVertex2f(d_width, 0);
	glEnd();

	glColor4fv(white);

	glBegin(GL_LINE_LOOP);
	glVertex2f(0, 0);
	glVertex2f(0, d_height);
	glVertex2f(d_width, d_height);
	glVertex2f(d_width, 0);
	glEnd();

	glColor4fv(white);

	glBegin(GL_LINE_LOOP);
	glVertex2f(0, BUTTON_HEIGHT);
	glVertex2f(0, d_height);
	glVertex2f(SIDEBAR, d_height);
	glVertex2f(SIDEBAR, BUTTON_HEIGHT);
	glEnd();

	//glEnable(GL_TEXTURE_2D);

	draw_Button(text, SIDEBAR, d_height, 0, 0); // Title bar

	int e;
	int condition = 0;

	for (e = 0; e < ext_count; e ++)
    {
        draw_Button_scene_ext(extensions[e], SIDEBAR, d_height, e, 1);
        if (strcmp(ext, extensions[e]) == 0)
            condition = 1;
    }

	draw_Button_path(path, Edit_Lock);
	draw_Scenes_Bottom_Line("Load", d_width, s_height);

	char Path[STRLEN];

	Path[0] = '\0';

	strcat(Path, path);

	if (condition)
    {
        if (isDirectory(Path))
        {
            if (Path[strlen(Path) - 1] != '/')
                strcat(Path, "/");
            strcat(Path, ext);
            strcat(Path, "/");
            //printf("%s\n", Path);
            draw_Files_List(Path, s_height, files_start, "", clear_background, update);
        }
        else
        {
            //printf("%s is not directory\n", Path);
            dir_lists = 0;
            draw_Files_Box(s_height, clear_background);
        }
    }
    else
    {
        if (isDirectory(Path))
        {
            if (Path[strlen(Path) - 1] != '/')
                strcat(Path, "/");
            //printf("%s\n", Path);
            draw_Files_List(Path, s_height, files_start, "", clear_background, update);
        }
        else
        {
            //printf("%s is not directory\n", Path);
            dir_lists = 0;
            draw_Files_Box(s_height, clear_background);
        }
    }

    glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glPopMatrix();
}

void draw_Saves_Dialog(char * path, const char * text, int s_height, char * ext, char ** extensions, int ext_count, int files_start, int clear_background, int update, int Edit_Lock)
{
    int d_width = DIALOG_WIDTH;
    int d_height = DIALOG_HEIGHT;
    glScissor(SIDEBAR, s_height - d_height + BOTTOM_LINE, d_width, d_height);
    if (clear_background)
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glViewport(SIDEBAR, s_height - d_height + BOTTOM_LINE, d_width, d_height);

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0, d_width, d_height, 0, 1, -1);

	glDisable(GL_LIGHTING);
	glDisable(GL_DEPTH_TEST);

	//glDisable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);

	glColor4fv(backg);

	glBegin(GL_QUADS);
	glVertex2f(0, 0);
	glVertex2f(0, d_height);
	glVertex2f(d_width, d_height);
	glVertex2f(d_width, 0);
	glEnd();

	glColor4fv(white);

	glBegin(GL_LINE_LOOP);
	glVertex2f(0, 0);
	glVertex2f(0, d_height);
	glVertex2f(d_width, d_height);
	glVertex2f(d_width, 0);
	glEnd();

	glColor4fv(white);

	glBegin(GL_LINE_LOOP);
	glVertex2f(0, BUTTON_HEIGHT);
	glVertex2f(0, d_height);
	glVertex2f(SIDEBAR, d_height);
	glVertex2f(SIDEBAR, BUTTON_HEIGHT);
	glEnd();

	//glEnable(GL_TEXTURE_2D);

	draw_Button(text, SIDEBAR, d_height, 0, 0); // Title bar

	int e;
	int condition = 0;

	for (e = 0; e < ext_count; e ++)
    {
        draw_Button_scene_ext(extensions[e], SIDEBAR, d_height, e, 1);
        if (strcmp(ext, extensions[e]) == 0)
            condition = 1;
    }

	draw_Button_path(path, Edit_Lock);
	draw_Scenes_Bottom_Line("Save", d_width, s_height);

	char Path[STRLEN];

	Path[0] = '\0';

	strcat(Path, path);

	if (condition)
    {
        if (isDirectory(Path))
        {
            if (Path[strlen(Path) - 1] != '/')
                strcat(Path, "/");
            strcat(Path, ext);
            strcat(Path, "/");
            //printf("%s\n", Path);
            draw_Files_List(Path, s_height, files_start, "", clear_background, update);
        }
        else
        {
            //printf("%s is not directory\n", Path);
            dir_lists = 0;
            draw_Files_Box(s_height, clear_background);
        }
    }
    else
    {
        if (isDirectory(Path))
        {
            if (Path[strlen(Path) - 1] != '/')
                strcat(Path, "/");
            //printf("%s\n", Path);
            draw_Files_List(Path, s_height, files_start, "", clear_background, update);
        }
        else
        {
            //printf("%s is not directory\n", Path);
            dir_lists = 0;
            draw_Files_Box(s_height, clear_background);
        }
    }


    glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glPopMatrix();
}

void draw_Files_Dialog(char * path, const char * text, int s_height, char * ext, char ** extensions, int ext_count, int files_start, int clear_background, int update, int Edit_Lock)
{
    int d_width = DIALOG_WIDTH;
    int d_height = DIALOG_HEIGHT;
    glScissor(SIDEBAR, s_height - d_height + BOTTOM_LINE, d_width, d_height);
    if (clear_background)
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glViewport(SIDEBAR, s_height - d_height + BOTTOM_LINE, d_width, d_height);

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0, d_width, d_height, 0, 1, -1);

	glDisable(GL_LIGHTING);
	glDisable(GL_DEPTH_TEST);

	//glDisable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);

	glColor4fv(backg);

	glBegin(GL_QUADS);
	glVertex2f(0, 0);
	glVertex2f(0, d_height);
	glVertex2f(d_width, d_height);
	glVertex2f(d_width, 0);
	glEnd();

	glColor4fv(white);

	glBegin(GL_LINE_LOOP);
	glVertex2f(0, 0);
	glVertex2f(0, d_height);
	glVertex2f(d_width, d_height);
	glVertex2f(d_width, 0);
	glEnd();

	glColor4fv(white);

	glBegin(GL_LINE_LOOP);
	glVertex2f(0, BUTTON_HEIGHT);
	glVertex2f(0, d_height);
	glVertex2f(SIDEBAR, d_height);
	glVertex2f(SIDEBAR, BUTTON_HEIGHT);
	glEnd();

	//glEnable(GL_TEXTURE_2D);

	draw_Button(text, SIDEBAR, d_height, 0, 0); // Title bar

	int e;
	for (e = 0; e < ext_count; e ++)
    {
        draw_Button_ext(extensions[e], SIDEBAR, d_height, e, 1);
    }

	draw_Button_path(path, Edit_Lock);

	draw_Files_List(path, s_height, files_start, ext, clear_background, update);

    glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glPopMatrix();
}

void draw_Bottom_Line(int width, int height)
{
    glScissor(SIDEBAR, 0, width, BOTTOM_LINE);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glViewport(SIDEBAR, 0, width, BOTTOM_LINE);

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0, width, BOTTOM_LINE, 0, 1, -1);

	glDisable(GL_LIGHTING);
	glDisable(GL_DEPTH_TEST);

	glBindTexture(GL_TEXTURE_2D, 0);

	glColor4fv(white);

	/*draw frame*/
	glBegin(GL_LINE_LOOP);
	glVertex2f(0, 0);
	glVertex2f(0, BOTTOM_LINE);
	glVertex2f(width + SIDEBAR, BOTTOM_LINE);
	glVertex2f(width + SIDEBAR, 0);
	glEnd();

    draw_Button_horizontal("Object mode", 0, 1);
    draw_Button_horizontal("Polygon mode", 1, 1);
    draw_Button_horizontal("Edge mode", 2, 1);
    draw_Button_horizontal("Vertex mode", 3, 1);
    draw_Button_horizontal("Bind mode", 4, 1);

    glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glPopMatrix();
}

void draw_UI_elements(int width, int height)
{
    glScissor(0, BOTTOM_LINE, width, height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glViewport(0, BOTTOM_LINE, width, height);

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0, width, height, 0, 1, -1);

	glDisable(GL_LIGHTING);
	glDisable(GL_DEPTH_TEST);

	glColor4fv(white);

	//glDisable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);

	/*draw frame*/
	glBegin(GL_LINE_LOOP);
	glVertex2f(0, 0);
	glVertex2f(0, height);
	glVertex2f(SIDEBAR, height);
	glVertex2f(SIDEBAR, 0);
	glEnd();

    Sidebar_Marker = 0;
    int Func_Marker = 0;

    draw_Collapsed("Lists", width, UI_LISTS, Sidebar_Marker);
    Button_sidebar[Sidebar_Marker].func = SideBar[Func_Marker];
    Func_Marker ++;
    Sidebar_Marker ++;
	if (UI_LISTS)
    {
        draw_Button("Items List", width, height, Sidebar_Marker, 1);
        Button_sidebar[Sidebar_Marker].func = SideBar[Func_Marker];
        Func_Marker ++;
        Sidebar_Marker ++;
        draw_Button("Textures List", width, height, Sidebar_Marker, 1);
        Button_sidebar[Sidebar_Marker].func = SideBar[Func_Marker];
        Func_Marker ++;
        Sidebar_Marker ++;
        draw_Button("Selections L.", width, height, Sidebar_Marker, 1);
        Button_sidebar[Sidebar_Marker].func = SideBar[Func_Marker];
        Func_Marker ++;
        Sidebar_Marker ++;
        draw_Button("Hierarchys L.", width, height, Sidebar_Marker, 1);
        Button_sidebar[Sidebar_Marker].func = SideBar[Func_Marker];
        Func_Marker ++;
        Sidebar_Marker ++;
        draw_Button("Deformers L.", width, height, Sidebar_Marker, 1);
        Button_sidebar[Sidebar_Marker].func = SideBar[Func_Marker];
        Func_Marker ++;
        Sidebar_Marker ++;
        draw_Button("Poses List", width, height, Sidebar_Marker, 1);
        Button_sidebar[Sidebar_Marker].func = SideBar[Func_Marker];
        Func_Marker ++;
        Sidebar_Marker ++;
        draw_Button("Bones List", width, height, Sidebar_Marker, 1);
        Button_sidebar[Sidebar_Marker].func = SideBar[Func_Marker];
        Func_Marker ++;
        Sidebar_Marker ++;
    }
    else
    {
        Func_Marker += 7;
    }
    draw_Collapsed("Files", width, UI_FILES, Sidebar_Marker);
    Button_sidebar[Sidebar_Marker].func = SideBar[Func_Marker];
    Func_Marker ++;
    Sidebar_Marker ++;
    if (UI_FILES)
    {
        draw_Button("Open obj file", width, height, Sidebar_Marker, 1);
        Button_sidebar[Sidebar_Marker].func = SideBar[Func_Marker];
        Func_Marker ++;
        Sidebar_Marker ++;
        draw_Button("Open img file", width, height, Sidebar_Marker, 1);
        Button_sidebar[Sidebar_Marker].func = SideBar[Func_Marker];
        Func_Marker ++;
        Sidebar_Marker  ++;
        draw_Button("Save scene", width, height, Sidebar_Marker, 1);
        Button_sidebar[Sidebar_Marker].func = SideBar[Func_Marker];
        Func_Marker ++;
        Sidebar_Marker ++;
        draw_Button("Load scene", width, height, Sidebar_Marker, 1);
        Button_sidebar[Sidebar_Marker].func = SideBar[Func_Marker];
        Func_Marker ++;
        Sidebar_Marker  ++;
    }
    else
    {
        Func_Marker += 4;
    }
    draw_Collapsed("Clear", width, UI_CLEAR, Sidebar_Marker);
    Button_sidebar[Sidebar_Marker].func = SideBar[Func_Marker];
    Func_Marker ++;
    Sidebar_Marker ++;
    if (UI_CLEAR)
    {
        draw_Button("Clear All", width, height, Sidebar_Marker, 1);
        Button_sidebar[Sidebar_Marker].func = SideBar[Func_Marker];
        Func_Marker ++;
        Sidebar_Marker ++;
    }
    else
    {
        Func_Marker += 1;
    }

    glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glPopMatrix();
}

void display_osd_font(const char * text, int width, int height)
{
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0, width, height, 0, 1, -1);

	GLfloat white[4] = {1, 1, 1, 1};

	glDisable(GL_LIGHTING);
	glDisable(GL_DEPTH_TEST);

	glColor4fv(white);

	//glDisable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);

	/*draw frame*/
	glBegin(GL_LINE_LOOP);
	glVertex2f(0, 0);
	glVertex2f(0, height);
	glVertex2f(width, height);
	glVertex2f(width, 0);
	glEnd();

	//glEnable(GL_TEXTURE_2D);

    int font_height = 11;

	FT_Set_Pixel_Sizes(face, 0, font_height);

	float origin_x = 10;
	float origin_y = 20;

    draw_text(text, origin_x, origin_y, font_height);

    glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glPopMatrix();
}
#endif // USERINTERFACE_H_INCLUDED
