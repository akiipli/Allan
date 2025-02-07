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

#define PROPERTIES_NONE 0
#define PROPERTIES_OBJECT 1
#define PROPERTIES_CAMERA 2
#define PROPERTIES_LIGHT 3
#define PROPERTIES_MATERIAL 4
#define PROPERTIES_LOCATOR 5
#define PROPERTIES_IK 6
#define PROPERTIES_BONE 7
#define PROPERTIES_MORPH 8
#define PROPERTIES_DEFORMER 9
#define PROPERTIES_TRJ 10

#define X_OFFSET 5
#define Y_OFFSET 10

int DRAW_TIMELINE = 1;
int PROPERTIES = 0;
int TABULATOR = 70;

int Drag_X = 0;
int DragDelta = 0;
int Not_Drag = 0;
int Edit_Properties = 0;

int Color;
int Color_adjusted;
int Drag_Color = 0;
int Color_Component = 0;
int Edit_Color = 0;

float Float_Value;
int Edit_Locator = 0;
int Locator_v_index = 0;
int Locator_h_index = 0;
float Locator_Values[3][3];

int Edit_Trajectory = 0;
int Trajectory_v_index = 0;
int Trajectory_h_index = 2;
float Trajectory_Value = 0;

int Drag_Trajectory = 0;
float Traj = 0;
float Trj_adjusted =0;
float TrajDelta = 0;

int Drag_Shine = 0;
float Shine;
float Shine_adjusted;
float ShineDelta;

int Drag_Displacement = 0;
float Displacement;
float Displacement_adjusted;
float DisplacementDelta;

int Drag_Morph = 0;
float Morph;
float Morph_adjusted;
float MorphDelta;
float Morphs[Y_OFFSET];

int properties[Y_OFFSET][X_OFFSET];

int left, right, top, bottom;
int prop_x, prop_y;

void init_properties()
{
    int x, y;

    for (y = 0; y < Y_OFFSET; y ++)
    {
        for (x = 0; x < X_OFFSET; x ++)
        {
            properties[y][x] = UI_BLACK;
        }
        Morphs[y] = 0.0;
    }
}

void black_out_properties()
{
    int x, y;

    for (y = 0; y < Y_OFFSET; y ++)
    {
        for (x = 0; x < X_OFFSET; x ++)
        {
            properties[y][x] = UI_BLACK;
        }
    }
}

void draw_Properties_Text(const char * text, int width, int height, int index, int highlight, int x_offset)
{
	//glDisable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);

    int font_height = 11, idx;

	FT_Set_Pixel_Sizes(face[0], 0, font_height);

	float origin_x = 5 + (x_offset * TABULATOR);
	float origin_y = BUTTON_HEIGHT * index + 15;

	if (highlight)
	{
	    idx = properties[index][x_offset];
	    if (highlight == 2)
	    {
	        if (idx == UI_BACKL)
            {
                idx = UI_YELLO;
            }
	    }
        glColor4fv(buttoncolors[idx].color);
	}
    else
        glColor4fv(buttoncolors[UI_BLACK].color);

    draw_text(text, origin_x, origin_y, font_height, 0);
}

void draw_Properties_List(int s_height, int clear_background, int type, void * subject)
{
    int d_width = DIALOG_WIDTH - SIDEBAR;
    int p_height = s_height - DIALOG_HEIGHT - (TIMELINE_HEIGHT * DRAW_TIMELINE);
    glScissor(SIDEBAR * 2, BOTTOM_LINE + (TIMELINE_HEIGHT * DRAW_TIMELINE), d_width, p_height);
    if (clear_background)
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glViewport(SIDEBAR * 2, BOTTOM_LINE + (TIMELINE_HEIGHT * DRAW_TIMELINE), d_width, p_height);

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
    int i, m_count = 0;

    if (type == PROPERTIES_OBJECT)
    {
        object * O = (object *)subject;
        sprintf(text, "\tpolys\tedges\tverts");
        draw_Properties_Text(text, d_width, p_height, idx, 0, 0);
        idx ++;
        sprintf(text, "elements\t%d\t%d\t%d", O->polycount, O->edgecount, O->vertcount);
        draw_Properties_Text(text, d_width, p_height, idx, 0, 0);
        idx ++;
        sprintf(text, "selected\t%d\t%d\t%d", O->selected_polys_count, O->selected_edges_count, O->selected_verts_count);
        draw_Properties_Text(text, d_width, p_height, idx, 0, 0);
        idx ++;
        sprintf(text, "surface\t%d %s", O->surface, Materials[O->surface].Name);
        draw_Properties_Text(text, d_width, p_height, idx, 0, 0);
        idx ++;
        sprintf(text, "smooth");
        draw_Properties_Text(text, d_width, p_height, idx, 0, 0);
        sprintf(text, "%d", O->smooth);
        draw_Properties_Text(text, d_width, p_height, idx, 1, 1);
        idx ++;
        sprintf(text, "subdivMax");
        draw_Properties_Text(text, d_width, p_height, idx, 0, 0);
        sprintf(text, "%d", O->subdlevel_Max);
        draw_Properties_Text(text, d_width, p_height, idx, 2, 1); /* index 2 indicates combo value */
        idx ++;
        sprintf(text, "deforms %d", O->deforms);
        draw_Properties_Text(text, d_width, p_height, idx, 0, 0);
        idx ++;
    }
    else if (type == PROPERTIES_MATERIAL)
    {
        surface_Material * M = (surface_Material *)subject;

        sprintf(text, "R %d", (int)M->RGBA.R);
        draw_Properties_Text(text, d_width, p_height, idx, 1, 1);
        sprintf(text, "G %d", (int)M->RGBA.G);
        draw_Properties_Text(text, d_width, p_height, idx, 1, 2);
        sprintf(text, "B %d", (int)M->RGBA.B);
        draw_Properties_Text(text, d_width, p_height, idx, 1, 3);
        sprintf(text, "A %d", (int)M->RGBA.A);
        draw_Properties_Text(text, d_width, p_height, idx, 1, 4);
        idx ++;

        GLfloat color[4] = {M->RGBA.R / 255.0, M->RGBA.G / 255.0, M->RGBA.B / 255.0, 255.0}; // , M->RGBA.A / 255.0};

        draw_Color_Button(SIDEBAR * 2, p_height, 0, color);

        sprintf(text, "\tShininess");
        draw_Properties_Text(text, d_width, p_height, idx, 0, 0);
        sprintf(text, "%1.2f", M->Shininess);
        draw_Properties_Text(text, d_width, p_height, idx, 1, 2);
        idx ++;
        sprintf(text, "\tDisplacem");
        draw_Properties_Text(text, d_width, p_height, idx, 0, 0);
        sprintf(text, "%1.2f", M->Displacement);
        draw_Properties_Text(text, d_width, p_height, idx, 1, 2);
        idx ++;
        sprintf(text, "\tSmooth");
        draw_Properties_Text(text, d_width, p_height, idx, 0, 0);
        sprintf(text, "%1d", M->smooth);
        draw_Properties_Text(text, d_width, p_height, idx, 1, 2);
        idx ++;
    }
    else if (type == PROPERTIES_LOCATOR)
    {
        transformer * T = (transformer *)subject;
        if (T->rot_Order == zxy)
            sprintf(text, "zxy");
        else if (T->rot_Order == yxz)
            sprintf(text, "yxz");
        else if (T->rot_Order == zyx)
            sprintf(text, "zyx");
        else if (T->rot_Order == xyz)
            sprintf(text, "xyz");
        else if (T->rot_Order == xzy)
            sprintf(text, "xzy");
        else if (T->rot_Order == yzx)
            sprintf(text, "yzx");

        draw_Properties_Text(text, d_width, p_height, idx, 0, 0);
        idx ++;
        if (T->pin == pin_0)
            sprintf(text, "Pin 0");
        else if (T->pin == pin_x)
            sprintf(text, "Pin x");
        else if (T->pin == pin_y)
            sprintf(text, "Pin y");
        else if (T->pin == pin_z)
            sprintf(text, "Pin z");
        draw_Properties_Text(text, d_width, p_height, idx, 0, 0);
        sprintf(text, "%d", T->pin);
        draw_Properties_Text(text, d_width, p_height, idx, 1, 1);
        idx ++;
        sprintf(text, "Rotation");
        draw_Properties_Text(text, d_width, p_height, idx, 0, 0);
        sprintf(text, "%1.2f", T->rot[0]);
        draw_Properties_Text(text, d_width, p_height, idx, 1, 1);
        sprintf(text, "%1.2f", T->rot[1]);
        draw_Properties_Text(text, d_width, p_height, idx, 1, 2);
        sprintf(text, "%1.2f", T->rot[2]);
        draw_Properties_Text(text, d_width, p_height, idx, 1, 3);
        idx ++;
        sprintf(text, "Position");
        draw_Properties_Text(text, d_width, p_height, idx, 0, 0);
        sprintf(text, "%1.2f", T->pos[0]);
        draw_Properties_Text(text, d_width, p_height, idx, 1, 1);
        sprintf(text, "%1.2f", T->pos[1]);
        draw_Properties_Text(text, d_width, p_height, idx, 1, 2);
        sprintf(text, "%1.2f", T->pos[2]);
        draw_Properties_Text(text, d_width, p_height, idx, 1, 3);
        idx ++;
        sprintf(text, "Scaling");
        draw_Properties_Text(text, d_width, p_height, idx, 0, 0);
        sprintf(text, "%1.2f", T->scl[0]);
        draw_Properties_Text(text, d_width, p_height, idx, 1, 1);
        sprintf(text, "%1.2f", T->scl[1]);
        draw_Properties_Text(text, d_width, p_height, idx, 1, 2);
        sprintf(text, "%1.2f", T->scl[2]);
        draw_Properties_Text(text, d_width, p_height, idx, 1, 3);
        idx ++;
    }
    else if (subject != NULL && type == PROPERTIES_IK)
    {
        ikChain * I = (ikChain *)subject;

        if (I->Deformer != NULL)
            sprintf(text, "Deformer\t%s", I->Deformer->Name);
        else
            sprintf(text, "Deformer");
        draw_Properties_Text(text, d_width, p_height, idx, 0, 0);
        idx ++;
        sprintf(text, "Bones\t%d", I->bonescount);
        draw_Properties_Text(text, d_width, p_height, idx, 0, 0);
        idx ++;
        sprintf(text, "Update");
        draw_Properties_Text(text, d_width, p_height, idx, 0, 0);
        sprintf(text, "%d", I->update);
        draw_Properties_Text(text, d_width, p_height, idx, 1, 1);
        idx ++;
        sprintf(text, "Stretch");
        draw_Properties_Text(text, d_width, p_height, idx, 0, 0);
        sprintf(text, "%d", I->stretch);
        draw_Properties_Text(text, d_width, p_height, idx, 1, 1);
        idx ++;
        if (I->C != NULL)
            sprintf(text, "Constraint %s", I->C->Locator->Name);
        else
            sprintf(text, "Constraint");
        draw_Properties_Text(text, d_width, p_height, idx, 0, 0);
        idx ++;
        if (I->Pole != NULL)
            sprintf(text, "Pole %s", I->Pole->Locator->Name);
        else
            sprintf(text, "Pole");
        draw_Properties_Text(text, d_width, p_height, idx, 0, 0);
        idx ++;
    }
    else if (subject != NULL && type == PROPERTIES_BONE)
    {
        bone * B = (bone *)subject;

        sprintf(text, "A\t%s", B->A->Name);
        draw_Properties_Text(text, d_width, p_height, idx, 0, 0);
        idx ++;
        sprintf(text, "B\t%s", B->B->Name);
        draw_Properties_Text(text, d_width, p_height, idx, 0, 0);
        idx ++;
        if (B->D != NULL)
            sprintf(text, "Deformer\t%s", B->D->Name);
        else
            sprintf(text, "Deformer");
        draw_Properties_Text(text, d_width, p_height, idx, 0, 0);
        idx ++;
        if (B->IK != NULL)
            sprintf(text, "IK\t%s", B->IK->Name);
        else
            sprintf(text, "IK");
        draw_Properties_Text(text, d_width, p_height, idx, 0, 0);
        idx ++;
    }
    else if (subject != NULL && type == PROPERTIES_MORPH)
    {
        deformer_morph * M = (deformer_morph *)subject;
        object_morph_dialer * OMD;
        morph_map * OM;
        morph * OMorph;

        sprintf(text, "%s", M->Name);
        draw_Properties_Text(text, d_width, p_height, idx, 0, 0);
        sprintf(text, "%1.2f", M->Amount);
        draw_Properties_Text(text, d_width, p_height, idx, 1, 2);
        Morphs[m_count ++] = M->Amount;
        idx ++;

        for (i = 0; i < M->objectCount; i ++)
        {
            OMD = M->Object_Morph_Map[i];
            OM = OMD->O->Morph_Maps[OMD->map_index];
            OMorph = OM->Morphs[OMD->morph_index];
            sprintf(text, "%s", OMD->O->Name);
            draw_Properties_Text(text, d_width, p_height, idx, 0, 0);
            sprintf(text, "%1.2f", OMorph->Amount);
            draw_Properties_Text(text, d_width, p_height, idx, 1, 2);
            if (m_count < Y_OFFSET)
                Morphs[m_count ++] = OMorph->Amount;
            idx ++;
        }
    }
    else if (subject != NULL && type == PROPERTIES_DEFORMER)
    {
        deformer * D = (deformer *)subject;
        object * O;

        sprintf(text, "linear pose");
        draw_Properties_Text(text, d_width, p_height, idx, 0, 0);
        sprintf(text, "%d", D->linear_pose);
        draw_Properties_Text(text, d_width, p_height, idx, 1, 2);
        idx ++;
        for (i = 0; i < D->Objects_Count; i ++)
        {
            O = D->Objects[i];
            sprintf(text, O->Name);
            draw_Properties_Text(text, d_width, p_height, idx, 0, 1);
            idx ++;
        }
    }
    else if (subject != NULL && type == PROPERTIES_TRJ)
    {
        trajectory * Trj = (trajectory *)subject;
        transformer * T;
        float Value = 0.0;

        sprintf(text, "Update");
        draw_Properties_Text(text, d_width, p_height, idx, 0, 0);
        sprintf(text, "%d", Trj->update);
        draw_Properties_Text(text, d_width, p_height, idx, 1, 1);
        idx ++;

        for (i = 0; i < Trj->transformers_count; i ++)
        {
            T = Trj->Transformers[i];

            Value = T->Trj_Value; //get_T_Trajectory_value(T, currentFrame);

            sprintf(text, T->Name);
            draw_Properties_Text(text, d_width, p_height, idx, 1, 1);

            sprintf(text, "%1.2f", Value);
            draw_Properties_Text(text, d_width, p_height, idx, 1, 2);
            idx ++;
        }
    }

	//glEnable(GL_TEXTURE_2D);

	// files list



    glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glPopMatrix();
}

void transfer_Morph_Amount(deformer_morph * M)
{
    int i, m_count = 0;

    object_morph_dialer * OMD;
    morph_map * OM;
    morph * OMorph;

    M->Amount = Morphs[m_count ++];

    for (i = 0; i < M->objectCount; i ++)
    {
        OMD = M->Object_Morph_Map[i];
        OM = OMD->O->Morph_Maps[OMD->map_index];
        OMorph = OM->Morphs[OMD->morph_index];

        if (m_count < Y_OFFSET)
            OMorph->Amount = Morphs[m_count ++];
    }
}

void transfer_Morph_Amount_To_Morfs(deformer_morph * M)
{
    int i, m_count = 0;

    object_morph_dialer * OMD;
    morph_map * OM;
    morph * OMorph;

    M->Amount = Morphs[m_count ++];

    for (i = 0; i < M->objectCount; i ++)
    {
        OMD = M->Object_Morph_Map[i];
        OM = OMD->O->Morph_Maps[OMD->map_index];
        OMorph = OM->Morphs[OMD->morph_index];

        if (m_count < Y_OFFSET)
        {
            OMorph->Amount = M->Amount;
            m_count ++;
        }
    }
}

void draw_Properties_Edit(const char * text, int s_height, int v_index, int h_index, int clear_background)
{
    int d_width = BUTTON_WIDTH_SHORT;
    int p_height = s_height - DIALOG_HEIGHT - BUTTON_HEIGHT * v_index;
    glScissor(SIDEBAR * 2 + h_index * TABULATOR, p_height, d_width, BUTTON_HEIGHT);

    if (clear_background)
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glViewport(SIDEBAR * 2 + h_index * TABULATOR, p_height, d_width, BUTTON_HEIGHT);

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0, d_width, BUTTON_HEIGHT, 0, 1, -1);

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

	draw_Properties_Text(text, d_width, p_height, 0, 0, 0);

    glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glPopMatrix();
}

void draw_Properties(const char * text, int s_height, int clear_background, int type, void * subject)
{
    int d_width = DIALOG_WIDTH;
    int p_height = s_height - DIALOG_HEIGHT - (TIMELINE_HEIGHT * DRAW_TIMELINE);
    glScissor(SIDEBAR, BUTTON_HEIGHT + (TIMELINE_HEIGHT * DRAW_TIMELINE), d_width, p_height);

    if (clear_background)
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glViewport(SIDEBAR, BUTTON_HEIGHT + (TIMELINE_HEIGHT * DRAW_TIMELINE), d_width, p_height);

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
