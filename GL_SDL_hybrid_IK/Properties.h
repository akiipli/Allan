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

void draw_Properties(const char * text, int s_height, int clear_background)
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

    glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glPopMatrix();
}

#endif // PROPERTIES_H_INCLUDED
