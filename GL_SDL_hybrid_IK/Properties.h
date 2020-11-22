/*
The MIT License

Copyright <2020> <Allan Kiipli>

Properties List is drawn under
dialog. Starting from dialog height
and reaching to bottom buttons bar.
DIALOG_HEIGHT, BUTTON_HEIGHT are
variables to calculate drawing.

Maximum dialog height needs to be limited
in a way that shrink properties fit below.

User can adjust dialog height with drag
from corner.
*/

#ifndef PROPERTIES_H_INCLUDED
#define PROPERTIES_H_INCLUDED

#define MIN_PROPERTIES_HEIGHT 20

#define PROPERTIES_OBJECT 1
#define PROPERTIES_CAMERA 2
#define PROPERTIES_LIGHT 3
#define PROPERTIES_MATERIAL 4

int PROPERTIES;
int TABULATOR = 70;

int Color;
int Drag_Color = 0;
int Color_Component = 0;
int Drag_X = 0;
int ColorDelta = 0;

void draw_Properties_Text(const char * text, int width, int height, int index)
{
	//glDisable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);

    int font_height = 11;

	FT_Set_Pixel_Sizes(face[0], 0, font_height);

	float origin_x = 5;
	float origin_y = BUTTON_HEIGHT * index + 15;

	glColor4fv(buttoncolors[UI_BLACK].color);

    draw_text(text, origin_x, origin_y, font_height, 0);
}

void draw_Properties_List(int s_height, int clear_background, int type, void * subject)
{
    int d_width = DIALOG_WIDTH - SIDEBAR;
    int p_height = s_height - DIALOG_HEIGHT;
    glScissor(SIDEBAR * 2, BOTTOM_LINE, d_width, p_height);
    if (clear_background)
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glViewport(SIDEBAR * 2, BOTTOM_LINE, d_width, p_height);

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0, d_width, p_height, 0, 1, -1);

	glDisable(GL_LIGHTING);
	glDisable(GL_DEPTH_TEST);

	//glDisable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);

	glColor4fv(grayb_light);

    draw_Rectangle((float[8]){0, 0,
        0, p_height,
        d_width, p_height,
        d_width, 0}, QUADS);

	glColor4fv(white);

    draw_Rectangle((float[8]){0, 0,
        0, p_height,
        d_width, p_height,
        d_width, 0}, LINE_LOOP);

    char text[STRLEN];
    int idx = 0;

    if (type == PROPERTIES_OBJECT)
    {
        object * O = (object *)subject;
        sprintf(text, "\tpolys\tedges\tverts");
        draw_Properties_Text(text, d_width, p_height, idx);
        idx ++;
        sprintf(text, "elements\t%d\t%d\t%d", O->polycount, O->edgecount, O->vertcount);
        draw_Properties_Text(text, d_width, p_height, idx);
        idx ++;
        sprintf(text, "selected\t%d\t%d\t%d", O->selected_polys_count, O->selected_edges_count, O->selected_verts_count);
        draw_Properties_Text(text, d_width, p_height, idx);
        idx ++;
        sprintf(text, "surface\t%d %s", O->surface, Materials[O->surface].Name);
        draw_Properties_Text(text, d_width, p_height, idx);
        idx ++;
    }
    else if (type == PROPERTIES_MATERIAL)
    {
        surface_Material * M = (surface_Material *)subject;

        sprintf(text, "\tR %d\tG %d\tB %d\tA %d", (int)M->RGBA.R, (int)M->RGBA.G, (int)M->RGBA.B, (int)M->RGBA.A);
        draw_Properties_Text(text, d_width, p_height, idx);
        idx ++;

        GLfloat color[4] = {M->RGBA.R / 255.0, M->RGBA.G / 255.0, M->RGBA.B / 255.0, M->RGBA.A / 255.0};

        draw_Color_Button(SIDEBAR * 2, p_height, 0, color);
    }



	//glEnable(GL_TEXTURE_2D);

	// files list



    glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glPopMatrix();
}

void draw_Properties(const char * text, int s_height, int clear_background, int type, void * subject)
{
    int d_width = DIALOG_WIDTH;
    int p_height = s_height - DIALOG_HEIGHT;
    glScissor(SIDEBAR, BUTTON_HEIGHT, d_width, p_height);

    if (clear_background)
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glViewport(SIDEBAR, BUTTON_HEIGHT, d_width, p_height);

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0, d_width, p_height, 0, 1, -1);

	glDisable(GL_LIGHTING);
	glDisable(GL_DEPTH_TEST);

	//glDisable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);

	glColor4fv(backg);

    draw_Rectangle((float[8]){0, 0,
        0, p_height,
        d_width, p_height,
        d_width, 0}, QUADS);

	glColor4fv(white);

    draw_Rectangle((float[8]){0, 0,
        0, p_height,
        d_width, p_height,
        d_width, 0}, LINE_LOOP);

	glColor4fv(white);

    draw_Rectangle((float[8]){0, BUTTON_HEIGHT,
        0, p_height,
        SIDEBAR, p_height,
        SIDEBAR, BUTTON_HEIGHT}, LINE_LOOP);

	//glEnable(GL_TEXTURE_2D);

	draw_Button(text, SIDEBAR, p_height, 0, 0); // Title bar

	draw_Properties_List(s_height, clear_background, type, subject);

    glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glPopMatrix();
}

#endif // PROPERTIES_H_INCLUDED