/*
The MIT License

Copyright <2018> <Allan Kiipli>
*/

#ifndef ARRAYS_H_INCLUDED
#define ARRAYS_H_INCLUDED

#define POLY_RENDER 0
#define ID_RENDER 1

int Fan_Arrays_Shader = 1;
int ELEMENT_ARRAYS = 1;

int IsExtensionSupported(char * szTargetExtension)
{
    const GLubyte * pszExtensions = glGetString(GL_EXTENSIONS);
    //printf("Extensions %s\n", pszExtensions);

    int white_spaces[300]; // Estimate number GL_ARB_vertex_buffer_object is listed before.
    int w_count = 0;
    white_spaces[w_count] = 0;

    int L = strlen(szTargetExtension);
    int l = strlen((char *)pszExtensions);
    char extension_Name[100];
    int i;
    int p;
    for (i = 0; i < l; i ++)
    {
        if (pszExtensions[i] == ' ')
            white_spaces[w_count++] = i + 1;
    }
    for (p = 0; p < w_count; p ++)
    {
        memcpy(extension_Name, &pszExtensions[white_spaces[p]], L);
        extension_Name[L] = '\0';
        //printf("%s\n", extension_Name);
        if (strcmp(extension_Name, szTargetExtension) == 0)
            return 1;
    }
    return 0;
}

int disable_VBO()
{
    if (IsExtensionSupported("GL_ARB_vertex_buffer_object"))
    {
        glDisable(GL_VERTEX_ARRAY);
        glDisable(GL_COLOR_ARRAY);
        glDisable(GL_NORMAL_ARRAY);
        glDisable(GL_TEXTURE_COORD_ARRAY);
        glDisableClientState(GL_VERTEX_ARRAY);
        glDisableClientState(GL_COLOR_ARRAY);
        glDisableClientState(GL_NORMAL_ARRAY);
        glDisableClientState(GL_TEXTURE_COORD_ARRAY);
        return 1;
    }
    else
    {
        return 0;
    }
}

GLuint vao;

int enable_VAO()
{
    if (IsExtensionSupported("GL_ARB_vertex_array_object"))
    {
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);
        return 1;
    }
    else
    {
        printf("VAO is not supported\n");
        return 0;
    }
}

int enable_VBO()
{
    if (IsExtensionSupported("GL_ARB_vertex_buffer_object"))
    {
        printf("VBO is supported\n");
        glEnable(GL_VERTEX_ARRAY);
        glEnable(GL_COLOR_ARRAY);
        glEnable(GL_NORMAL_ARRAY);
        glEnable(GL_TEXTURE_COORD_ARRAY);
        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_COLOR_ARRAY);
        glEnableClientState(GL_NORMAL_ARRAY);
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        return 1;
    }
    else
    {
        printf("VBO is not supported\n");
        return 0;
    }
}

int setup_test_Quad()
{
    printf("set up vertex array\n");

    GLfloat vert_array_[4][2] = {{10, 10}, {10, 40}, {50, 40}, {50, 10}};
    GLfloat norm_array_[4][3] = {{1.0, 0.0, 0.0}, {1.0, 0.0, 0.0}, {1.0, 0.0, 0.0}, {1.0, 0.0, 0.0}};
    GLfloat text_array_[4][2] = {{0.1, 0.9}, {0.1, 0.1}, {0.9, 0.1}, {0.9, 0.9}};

    printf("set up VBO\n");
    GLuint buffers[3];
    glGenBuffers(3, buffers);

    printf("vert array size %d\n", sizeof vert_array_);

    glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof vert_array_, vert_array_, GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, buffers[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof norm_array_, norm_array_, GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, buffers[2]);
    glBufferData(GL_ARRAY_BUFFER, sizeof text_array_, text_array_, GL_STATIC_DRAW);

    ////

    printf("draw Arrays\n");
    glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof vert_array_, vert_array_);
    glVertexPointer(2, GL_FLOAT, 0, 0); // 2d vertexes

    glBindBuffer(GL_ARRAY_BUFFER, buffers[1]);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof norm_array_, norm_array_);
    glNormalPointer(GL_FLOAT, 0, 0);

    glBindBuffer(GL_ARRAY_BUFFER, buffers[2]);
    glTexCoordPointer(2, GL_FLOAT, 0, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glDrawArrays(GL_QUADS, 0, 1 * 4);
    return 1;
}

int draw_Fan_Arrays_Shader(object * O, int update_colors, int update_uv, int E)
{
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    glEnableVertexAttribArray(3);
    glEnableVertexAttribArray(4);

    //printf("buffers %u %u %u \n", O->vert_array_buffer_[L], O->norm_array_buffer_[L], O->text_array_buffer_[L]);
    glBindBuffer(GL_ARRAY_BUFFER, O->vert_array_buffer[E]);
    //printf("vert array size %d\n", sizeof O->vert_array_[L]);

    glBufferSubData(GL_ARRAY_BUFFER, 0, O->vert_array_size[E], O->vert_array[E]);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glBindBuffer(GL_ARRAY_BUFFER, O->text_array_buffer[E]);
    if (update_uv)
        glBufferSubData(GL_ARRAY_BUFFER, 0, O->text_array_size[E], O->text_array[E]);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);

    glBindBuffer(GL_ARRAY_BUFFER, O->cols_array_buffer[E]);
    if (update_colors)
        glBufferSubData(GL_ARRAY_BUFFER, 0, O->cols_array_size[E], O->cols_array[E]);
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 0, 0);

    glBindBuffer(GL_ARRAY_BUFFER, O->norm_array_buffer[E]);
    glBufferSubData(GL_ARRAY_BUFFER, 0, O->norm_array_size[E], O->norm_array[E]);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glBindBuffer(GL_ARRAY_BUFFER, O->tang_array_buffer[E]);
    glBufferSubData(GL_ARRAY_BUFFER, 0, O->tang_array_size[E], O->tang_array[E]);
    glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 0, 0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    if (E)
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, O->elem_array_buffer);
        // update Data
        glDrawElements(GL_TRIANGLES, O->tripcount * 3, GL_UNSIGNED_INT, 0);
    }
    else
    {
        int p;
        int s = 0;

        polygon * P;

        for(p = 0; p < O->polycount; p ++)
        {
            P = &O->polys[p / ARRAYSIZE][p % ARRAYSIZE];
            glDrawArrays(GL_TRIANGLE_FAN, s, P->edgecount);
            s += P->edgecount;
        }
    }

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
    glDisableVertexAttribArray(3);
    glDisableVertexAttribArray(4);
    return 1;
}

int draw_Fan_Arrays(object * O, int mode, int update_colors, int update_uv, int E)
{
    if (mode == ID_RENDER)
    {
        glDisable(GL_LIGHTING);
        glDisable(GL_DITHER);
        glDisable(GL_TEXTURE_2D);
        glDisable(GL_BLEND);
        glDisable(GL_MULTISAMPLE);
    }
    //printf("draw Arrays\n");

    //printf("buffers %u %u %u \n", O->vert_array_buffer_[L], O->norm_array_buffer_[L], O->text_array_buffer_[L]);
    glBindBuffer(GL_ARRAY_BUFFER, O->vert_array_buffer[E]);
    //printf("vert array size %d\n", sizeof O->vert_array_[L]);

    glBufferSubData(GL_ARRAY_BUFFER, 0, O->vert_array_size[E], O->vert_array[E]);
    glVertexPointer(3, GL_FLOAT, 0, 0); // 2d vertexes

    glBindBuffer(GL_ARRAY_BUFFER, O->cols_array_buffer[E]);
    if (update_colors)
        glBufferSubData(GL_ARRAY_BUFFER, 0, O->cols_array_size[E], O->cols_array[E]);
    glColorPointer(4, GL_FLOAT, 0, 0);

    glBindBuffer(GL_ARRAY_BUFFER, O->norm_array_buffer[E]);
    glBufferSubData(GL_ARRAY_BUFFER, 0, O->norm_array_size[E], O->norm_array[E]);
    glNormalPointer(GL_FLOAT, 0, 0);

    glBindBuffer(GL_ARRAY_BUFFER, O->text_array_buffer[E]);
    if (update_uv)
        glBufferSubData(GL_ARRAY_BUFFER, 0, O->text_array_size[E], O->text_array[E]);
    glTexCoordPointer(2, GL_FLOAT, 0, 0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    if (E)
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, O->elem_array_buffer);
        // update Data
        glDrawElements(GL_TRIANGLES, O->tripcount * 3, GL_UNSIGNED_INT, 0);
    }
    else
    {
        int p;
        int s = 0;

        polygon * P;

        for(p = 0; p < O->polycount; p ++)
        {
            P = &O->polys[p / ARRAYSIZE][p % ARRAYSIZE];
            glDrawArrays(GL_TRIANGLE_FAN, s, P->edgecount);
            s += P->edgecount;
        }
    }

    if (mode == ID_RENDER)
    {
        glEnable(GL_MULTISAMPLE);
        glEnable(GL_BLEND);
        glEnable(GL_TEXTURE_2D);
        glEnable(GL_DITHER);
        glEnable(GL_LIGHTING);
    }
    return 1;
}

int draw_Fan_Arrays_Shader_ID(object * O, int update_colors, int update_uv, int E)
{
    glDisable(GL_LIGHTING);
    glDisable(GL_DITHER);
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);
    glDisable(GL_MULTISAMPLE);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, O->vert_array_buffer[E]);
    glBufferSubData(GL_ARRAY_BUFFER, 0, O->vert_array_size[E], O->vert_array[E]);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glBindBuffer(GL_ARRAY_BUFFER, O->cols_array_buffer[E]);
    if (update_colors)
        glBufferSubData(GL_ARRAY_BUFFER, 0, O->cols_array_size[E], O->cols_array[E]);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, 0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    if (E)
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, O->elem_array_buffer);
        // update Data
        glDrawElements(GL_TRIANGLES, O->tripcount * 3, GL_UNSIGNED_INT, 0);
    }
    else
    {
        int p;
        int s = 0;

        polygon * P;

        for(p = 0; p < O->polycount; p ++)
        {
            P = &O->polys[p / ARRAYSIZE][p % ARRAYSIZE];
            glDrawArrays(GL_TRIANGLE_FAN, s, P->edgecount);
            s += P->edgecount;
        }
    }

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);

    glEnable(GL_MULTISAMPLE);
    glEnable(GL_BLEND);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_DITHER);
    glEnable(GL_LIGHTING);

    return 1;
}

int draw_Arrays_Shader_ID(object * O, int L, int update_colors, int update_uv, int E)
{
    glDisable(GL_LIGHTING);
    glDisable(GL_DITHER);
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);
    glDisable(GL_MULTISAMPLE);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, O->vert_array_buffer_[L][E]);
    glBufferSubData(GL_ARRAY_BUFFER, 0, O->vert_array_size_[L][E], O->vert_array_[L][E]);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glBindBuffer(GL_ARRAY_BUFFER, O->cols_array_buffer_[L][E]);
    if (update_colors)
        glBufferSubData(GL_ARRAY_BUFFER, 0, O->cols_array_size_[L][E], O->cols_array_[L][E]);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, 0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    if (E)
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, O->elem_array_buffer_[L]);
        // update Data
        glDrawElements(GL_QUADS, O->quadcount_[L] * 4, GL_UNSIGNED_INT, 0);
    }
    else
    {
        glDrawArrays(GL_QUADS, 0, O->quadcount_[L] * 4);
    }

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);

    glEnable(GL_MULTISAMPLE);
    glEnable(GL_BLEND);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_DITHER);
    glEnable(GL_LIGHTING);

    return 1;
}

int draw_Arrays_Shader(object * O, int L, int update_colors, int update_uv, int E)
{
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    glEnableVertexAttribArray(3);
    glEnableVertexAttribArray(4);

    //printf("buffers %u %u %u \n", O->vert_array_buffer_[L], O->norm_array_buffer_[L], O->text_array_buffer_[L]);
    glBindBuffer(GL_ARRAY_BUFFER, O->vert_array_buffer_[L][E]);
    //printf("vert array size %d\n", sizeof O->vert_array_[L]);

    glBufferSubData(GL_ARRAY_BUFFER, 0, O->vert_array_size_[L][E], O->vert_array_[L][E]);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glBindBuffer(GL_ARRAY_BUFFER, O->text_array_buffer_[L][E]);
    if (update_uv)
        glBufferSubData(GL_ARRAY_BUFFER, 0, O->text_array_size_[L][E], O->text_array_[L][E]);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);

    glBindBuffer(GL_ARRAY_BUFFER, O->cols_array_buffer_[L][E]);
    if (update_colors)
        glBufferSubData(GL_ARRAY_BUFFER, 0, O->cols_array_size_[L][E], O->cols_array_[L][E]);
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 0, 0);

    glBindBuffer(GL_ARRAY_BUFFER, O->norm_array_buffer_[L][E]);
    glBufferSubData(GL_ARRAY_BUFFER, 0, O->norm_array_size_[L][E], O->norm_array_[L][E]);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glBindBuffer(GL_ARRAY_BUFFER, O->tang_array_buffer_[L][E]);
    glBufferSubData(GL_ARRAY_BUFFER, 0, O->tang_array_size_[L][E], O->tang_array_[L][E]);
    glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 0, 0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    if (E)
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, O->elem_array_buffer_[L]);
        // update Data
        glDrawElements(GL_QUADS, O->quadcount_[L] * 4, GL_UNSIGNED_INT, 0);
    }
    else
    {
        glDrawArrays(GL_QUADS, 0, O->quadcount_[L] * 4);
    }

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
    glDisableVertexAttribArray(3);
    glDisableVertexAttribArray(4);
    return 1;
}

int draw_Arrays(object * O, int L, int mode, int update_colors, int update_uv, int E)
{
    if (mode == ID_RENDER)
    {
        glDisable(GL_LIGHTING);
        glDisable(GL_DITHER);
        glDisable(GL_TEXTURE_2D);
        glDisable(GL_BLEND);
        glDisable(GL_MULTISAMPLE);
    }
    //printf("draw Arrays\n");

    //printf("buffers %u %u %u \n", O->vert_array_buffer_[L], O->norm_array_buffer_[L], O->text_array_buffer_[L]);
    glBindBuffer(GL_ARRAY_BUFFER, O->vert_array_buffer_[L][E]);
    //printf("vert array size %d\n", sizeof O->vert_array_[L]);

    glBufferSubData(GL_ARRAY_BUFFER, 0, O->vert_array_size_[L][E], O->vert_array_[L][E]);
    glVertexPointer(3, GL_FLOAT, 0, 0); // 2d vertexes

    glBindBuffer(GL_ARRAY_BUFFER, O->cols_array_buffer_[L][E]);
    if (update_colors)
        glBufferSubData(GL_ARRAY_BUFFER, 0, O->cols_array_size_[L][E], O->cols_array_[L][E]);
    glColorPointer(4, GL_FLOAT, 0, 0);

    glBindBuffer(GL_ARRAY_BUFFER, O->norm_array_buffer_[L][E]);
    glBufferSubData(GL_ARRAY_BUFFER, 0, O->norm_array_size_[L][E], O->norm_array_[L][E]);
    glNormalPointer(GL_FLOAT, 0, 0);

    glBindBuffer(GL_ARRAY_BUFFER, O->text_array_buffer_[L][E]);
    if (update_uv)
        glBufferSubData(GL_ARRAY_BUFFER, 0, O->text_array_size_[L][E], O->text_array_[L][E]);
    glTexCoordPointer(2, GL_FLOAT, 0, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    if (E)
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, O->elem_array_buffer_[L]);
        // update Data
        glDrawElements(GL_QUADS, O->quadcount_[L] * 4, GL_UNSIGNED_INT, 0);
    }
    else
    {
        glDrawArrays(GL_QUADS, 0, O->quadcount_[L] * 4);
    }

    if (mode == ID_RENDER)
    {
        glEnable(GL_MULTISAMPLE);
        glEnable(GL_BLEND);
        glEnable(GL_TEXTURE_2D);
        glEnable(GL_DITHER);
        glEnable(GL_LIGHTING);
    }
    return 1;
}

int set_up_Fan_VBO(object * O)
{
    printf("set up Fan VBO\n");
    GLuint buffers[11];
    glGenBuffers(11, buffers);

    //printf("buffers %u %u %u %u %u %u\n", buffers[0], buffers[1], buffers[2], buffers[3], buffers[4], buffers[5]);

    O->vert_array_buffer[0] = buffers[0];
    O->cols_array_buffer[0] = buffers[1];
    O->norm_array_buffer[0] = buffers[2];
    O->text_array_buffer[0] = buffers[3];
    O->tang_array_buffer[0] = buffers[4];
    O->vert_array_buffer[1] = buffers[5];
    O->cols_array_buffer[1] = buffers[6];
    O->norm_array_buffer[1] = buffers[7];
    O->text_array_buffer[1] = buffers[8];
    O->tang_array_buffer[1] = buffers[9];
    O->elem_array_buffer = buffers[10];

    glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
    glBufferData(GL_ARRAY_BUFFER, O->vert_array_size[0], O->vert_array[0], GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, buffers[1]);
    glBufferData(GL_ARRAY_BUFFER, O->cols_array_size[0], O->cols_array[0], GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, buffers[2]);
    glBufferData(GL_ARRAY_BUFFER, O->norm_array_size[0], O->norm_array[0], GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, buffers[3]);
    glBufferData(GL_ARRAY_BUFFER, O->text_array_size[0], O->text_array[0], GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, buffers[4]);
    glBufferData(GL_ARRAY_BUFFER, O->tang_array_size[0], O->tang_array[0], GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, buffers[5]);
    glBufferData(GL_ARRAY_BUFFER, O->vert_array_size[1], O->vert_array[1], GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, buffers[6]);
    glBufferData(GL_ARRAY_BUFFER, O->cols_array_size[1], O->cols_array[1], GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, buffers[7]);
    glBufferData(GL_ARRAY_BUFFER, O->norm_array_size[1], O->norm_array[1], GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, buffers[8]);
    glBufferData(GL_ARRAY_BUFFER, O->text_array_size[1], O->text_array[1], GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, buffers[9]);
    glBufferData(GL_ARRAY_BUFFER, O->tang_array_size[1], O->tang_array[1], GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers[10]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, O->elem_array_size, O->elem_array, GL_STATIC_DRAW);

    return 1;
}

int set_up_VBO(object * O, int L)
{
    printf("set up VBO\n");
    GLuint buffers[11];
    glGenBuffers(11, buffers);

    //printf("buffers %u %u %u %u %u %u\n", buffers[0], buffers[1], buffers[2], buffers[3], buffers[4], buffers[5]);

    O->vert_array_buffer_[L][0] = buffers[0];
    O->cols_array_buffer_[L][0] = buffers[1];
    O->norm_array_buffer_[L][0] = buffers[2];
    O->text_array_buffer_[L][0] = buffers[3];
    O->tang_array_buffer_[L][0] = buffers[4];
    O->vert_array_buffer_[L][1] = buffers[5];
    O->cols_array_buffer_[L][1] = buffers[6];
    O->norm_array_buffer_[L][1] = buffers[7];
    O->text_array_buffer_[L][1] = buffers[8];
    O->tang_array_buffer_[L][1] = buffers[9];
    O->elem_array_buffer_[L] = buffers[10];

    glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
    glBufferData(GL_ARRAY_BUFFER, O->vert_array_size_[L][0], O->vert_array_[L][0], GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, buffers[1]);
    glBufferData(GL_ARRAY_BUFFER, O->cols_array_size_[L][0], O->cols_array_[L][0], GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, buffers[2]);
    glBufferData(GL_ARRAY_BUFFER, O->norm_array_size_[L][0], O->norm_array_[L][0], GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, buffers[3]);
    glBufferData(GL_ARRAY_BUFFER, O->text_array_size_[L][0], O->text_array_[L][0], GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, buffers[4]);
    glBufferData(GL_ARRAY_BUFFER, O->tang_array_size_[L][0], O->tang_array_[L][0], GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, buffers[5]);
    glBufferData(GL_ARRAY_BUFFER, O->vert_array_size_[L][1], O->vert_array_[L][1], GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, buffers[6]);
    glBufferData(GL_ARRAY_BUFFER, O->cols_array_size_[L][1], O->cols_array_[L][1], GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, buffers[7]);
    glBufferData(GL_ARRAY_BUFFER, O->norm_array_size_[L][1], O->norm_array_[L][1], GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, buffers[8]);
    glBufferData(GL_ARRAY_BUFFER, O->text_array_size_[L][1], O->text_array_[L][1], GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, buffers[9]);
    glBufferData(GL_ARRAY_BUFFER, O->tang_array_size_[L][1], O->tang_array_[L][1], GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers[10]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, O->elem_array_size_[L], O->elem_array_[L], GL_STATIC_DRAW);

    return 1;
}

int set_up_Fan_array(object * O)
{
    printf("set up Fan array\n");
    uv * UV;
    vertex * V;
    polygon * P;
    triangle * T;
    int p, v, t, e, idx;

    int data_t = 0;

    for (p = 0; p < O->polycount; p ++)
    {
        P = &O->polys[p / ARRAYSIZE][p % ARRAYSIZE];
        data_t += P->edgecount;
    }

    O->vert_array_size[0] = data_t * 3 * sizeof(GLfloat);
    O->cols_array_size[0] = data_t * 4 * sizeof(GLfloat);
    O->norm_array_size[0] = data_t * 3 * sizeof(GLfloat);
    O->text_array_size[0] = data_t * 2 * sizeof(GLfloat);
    O->tang_array_size[0] = data_t * 4 * sizeof(GLfloat);

    int data_e = O->textcount;
    int data_l = O->tripcount;

    O->vert_array_size[1] = data_e * 3 * sizeof(GLfloat);
    O->cols_array_size[1] = data_e * 4 * sizeof(GLfloat);
    O->norm_array_size[1] = data_e * 3 * sizeof(GLfloat);
    O->text_array_size[1] = data_e * 2 * sizeof(GLfloat);
    O->tang_array_size[1] = data_e * 4 * sizeof(GLfloat);
    O->elem_array_size = data_l * 3 * sizeof(GLuint);

    O->vert_array[0] = malloc(O->vert_array_size[0]);
    if (O->vert_array[0] == NULL) return 0;
    O->cols_array[0] = malloc(O->cols_array_size[0]);
    if (O->cols_array[0] == NULL) return 0;
    O->norm_array[0] = malloc(O->norm_array_size[0]);
    if (O->norm_array[0] == NULL) return 0;
    O->text_array[0] = malloc(O->text_array_size[0]);
    if (O->text_array[0] == NULL) return 0;
    O->tang_array[0] = malloc(O->tang_array_size[0]);
    if (O->tang_array[0] == NULL) return 0;

    O->vert_array[1] = malloc(O->vert_array_size[1]);
    if (O->vert_array[1] == NULL) return 0;
    O->cols_array[1] = malloc(O->cols_array_size[1]);
    if (O->cols_array[1] == NULL) return 0;
    O->norm_array[1] = malloc(O->norm_array_size[1]);
    if (O->norm_array[1] == NULL) return 0;
    O->text_array[1] = malloc(O->text_array_size[1]);
    if (O->text_array[1] == NULL) return 0;
    O->tang_array[1] = malloc(O->tang_array_size[1]);
    if (O->tang_array[1] == NULL) return 0;
    O->elem_array = malloc(O->elem_array_size);
    if (O->elem_array == NULL) return 0;

    int v_c = 0;
    int c_c = 0;
    int n_c = 0;
    int t_c = 0;
    int t_a = 0;

    for(p = 0; p < O->polycount; p ++)
    {
        P = &O->polys[p / ARRAYSIZE][p % ARRAYSIZE];
        for(e = 0; e < P->edgecount; e ++)
        {
            idx = P->verts[e];
            V = &O->verts[idx / ARRAYSIZE][idx % ARRAYSIZE];
            idx = P->texts[e];
            UV = &O->uvtex[idx / ARRAYSIZE][idx % ARRAYSIZE];
            O->vert_array[0][v_c++] = V->Tx;
            O->vert_array[0][v_c++] = V->Ty;
            O->vert_array[0][v_c++] = V->Tz;
            O->cols_array[0][c_c++] = 1;
            O->cols_array[0][c_c++] = 1;
            O->cols_array[0][c_c++] = 1;
            O->cols_array[0][c_c++] = 1;
            O->norm_array[0][n_c++] = V->N.Tx;
            O->norm_array[0][n_c++] = V->N.Ty;
            O->norm_array[0][n_c++] = V->N.Tz;
            O->text_array[0][t_c++] = UV->u;
            O->text_array[0][t_c++] = UV->v;
            O->tang_array[0][t_a++] = UV->tangent[0];
            O->tang_array[0][t_a++] = UV->tangent[1];
            O->tang_array[0][t_a++] = UV->tangent[2];
            O->tang_array[0][t_a++] = UV->tangent[3];
        }
    }

    v_c = 0;
    c_c = 0;
    n_c = 0;
    t_c = 0;
    t_a = 0;
    int t_e = 0;

    for (v = 0; v < O->textcount; v ++)
    {
        UV = &O->uvtex[v / ARRAYSIZE][v % ARRAYSIZE];
        O->text_array[1][t_c++] = UV->u;
        O->text_array[1][t_c++] = UV->v;

        idx = UV->vert;
        V = &O->verts[idx / ARRAYSIZE][idx % ARRAYSIZE];
        O->vert_array[1][v_c++] = V->Tx;
        O->vert_array[1][v_c++] = V->Ty;
        O->vert_array[1][v_c++] = V->Tz;
        O->cols_array[1][c_c++] = 1;
        O->cols_array[1][c_c++] = 1;
        O->cols_array[1][c_c++] = 1;
        O->cols_array[1][c_c++] = 1;
        O->norm_array[1][n_c++] = V->N.Tx;
        O->norm_array[1][n_c++] = V->N.Ty;
        O->norm_array[1][n_c++] = V->N.Tz;
        O->tang_array[1][t_a++] = UV->tangent[0];
        O->tang_array[1][t_a++] = UV->tangent[1];
        O->tang_array[1][t_a++] = UV->tangent[2];
        O->tang_array[1][t_a++] = UV->tangent[3];
    }

    for (t = 0; t < O->tripcount; t ++)
    {
        T = &O->trips[t / ARRAYSIZE][t % ARRAYSIZE];
        for(e = 0; e < 3; e ++)
        {
            O->elem_array[t_e++] = T->texts[e];
        }
    }

    if (set_up_Fan_VBO(O))
    {
        return 1;
    }
    else
    {
        free(O->vert_array);
        free(O->cols_array);
        free(O->norm_array);
        free(O->text_array);
        free(O->tang_array);
        free(O->elem_array);
        return 0;
    }
}

int set_up_vertex_array(object * O, int L)
{
    printf("set up vertex array\n");
    uv * UV;
    vertex * V;
    quadrant * Q;
    int v, q, e, idx;

    int data_t = O->quadcount_[L] * 4;

    O->vert_array_size_[L][0] = data_t * 3 * sizeof(GLfloat);
    O->cols_array_size_[L][0] = data_t * 4 * sizeof(GLfloat);
    O->norm_array_size_[L][0] = data_t * 3 * sizeof(GLfloat);
    O->text_array_size_[L][0] = data_t * 2 * sizeof(GLfloat);
    O->tang_array_size_[L][0] = data_t * 4 * sizeof(GLfloat);

    int data_e = O->textcount_[L];
    int data_l = O->quadcount_[L];

    O->vert_array_size_[L][1] = data_e * 3 * sizeof(GLfloat);
    O->cols_array_size_[L][1] = data_e * 4 * sizeof(GLfloat);
    O->norm_array_size_[L][1] = data_e * 3 * sizeof(GLfloat);
    O->text_array_size_[L][1] = data_e * 2 * sizeof(GLfloat);
    O->tang_array_size_[L][1] = data_e * 4 * sizeof(GLfloat);
    O->elem_array_size_[L] = data_l * 4 * sizeof(GLuint);

    O->vert_array_[L][0] = malloc(O->vert_array_size_[L][0]);
    if (O->vert_array_[L][0] == NULL) return 0;
    O->cols_array_[L][0] = malloc(O->cols_array_size_[L][0]);
    if (O->cols_array_[L][0] == NULL) return 0;
    O->norm_array_[L][0] = malloc(O->norm_array_size_[L][0]);
    if (O->norm_array_[L][0] == NULL) return 0;
    O->text_array_[L][0] = malloc(O->text_array_size_[L][0]);
    if (O->text_array_[L][0] == NULL) return 0;
    O->tang_array_[L][0] = malloc(O->tang_array_size_[L][0]);
    if (O->tang_array_[L][0] == NULL) return 0;

    O->vert_array_[L][1] = malloc(O->vert_array_size_[L][1]);
    if (O->vert_array_[L][1] == NULL) return 0;
    O->cols_array_[L][1] = malloc(O->cols_array_size_[L][1]);
    if (O->cols_array_[L][1] == NULL) return 0;
    O->norm_array_[L][1] = malloc(O->norm_array_size_[L][1]);
    if (O->norm_array_[L][1] == NULL) return 0;
    O->text_array_[L][1] = malloc(O->text_array_size_[L][1]);
    if (O->text_array_[L][1] == NULL) return 0;
    O->tang_array_[L][1] = malloc(O->tang_array_size_[L][1]);
    if (O->tang_array_[L][1] == NULL) return 0;
    O->elem_array_[L] = malloc(O->elem_array_size_[L]);
    if (O->elem_array_[L] == NULL) return 0;

    int v_c = 0;
    int c_c = 0;
    int n_c = 0;
    int t_c = 0;
    int t_a = 0;

    for(q = 0; q < O->quadcount_[L]; q ++)
    {
        Q = &O->quads_[L][q / ARRAYSIZE][q % ARRAYSIZE];
        for(e = 0; e < 4; e ++)
        {
            idx = Q->verts[e];
            V = &O->verts_[L][idx / ARRAYSIZE][idx % ARRAYSIZE];
            idx = Q->texts[e];
            UV = &O->uvtex_[L][idx / ARRAYSIZE][idx % ARRAYSIZE];
            O->vert_array_[L][0][v_c++] = V->Tx;
            O->vert_array_[L][0][v_c++] = V->Ty;
            O->vert_array_[L][0][v_c++] = V->Tz;
            O->cols_array_[L][0][c_c++] = 1;
            O->cols_array_[L][0][c_c++] = 1;
            O->cols_array_[L][0][c_c++] = 1;
            O->cols_array_[L][0][c_c++] = 1;
            O->norm_array_[L][0][n_c++] = V->N.Tx;
            O->norm_array_[L][0][n_c++] = V->N.Ty;
            O->norm_array_[L][0][n_c++] = V->N.Tz;
            O->text_array_[L][0][t_c++] = UV->u;
            O->text_array_[L][0][t_c++] = UV->v;
            O->tang_array_[L][0][t_a++] = UV->tangent[0];
            O->tang_array_[L][0][t_a++] = UV->tangent[1];
            O->tang_array_[L][0][t_a++] = UV->tangent[2];
            O->tang_array_[L][0][t_a++] = UV->tangent[3];
        }
    }

    v_c = 0;
    c_c = 0;
    n_c = 0;
    t_c = 0;
    t_a = 0;
    int t_e = 0;

    for (v = 0; v < O->textcount_[L]; v ++)
    {
        UV = &O->uvtex_[L][v / ARRAYSIZE][v % ARRAYSIZE];
        O->text_array_[L][1][t_c++] = UV->u;
        O->text_array_[L][1][t_c++] = UV->v;

        idx = UV->vert;
        V = &O->verts_[L][idx / ARRAYSIZE][idx % ARRAYSIZE];
        O->vert_array_[L][1][v_c++] = V->Tx;
        O->vert_array_[L][1][v_c++] = V->Ty;
        O->vert_array_[L][1][v_c++] = V->Tz;
        O->cols_array_[L][1][c_c++] = 1;
        O->cols_array_[L][1][c_c++] = 1;
        O->cols_array_[L][1][c_c++] = 1;
        O->cols_array_[L][1][c_c++] = 1;
        O->norm_array_[L][1][n_c++] = V->N.Tx;
        O->norm_array_[L][1][n_c++] = V->N.Ty;
        O->norm_array_[L][1][n_c++] = V->N.Tz;
        O->tang_array_[L][1][t_a++] = UV->tangent[0];
        O->tang_array_[L][1][t_a++] = UV->tangent[1];
        O->tang_array_[L][1][t_a++] = UV->tangent[2];
        O->tang_array_[L][1][t_a++] = UV->tangent[3];
    }

    for (q = 0; q < O->quadcount_[L]; q ++)
    {
        Q = &O->quads_[L][q / ARRAYSIZE][q % ARRAYSIZE];
        for(e = 0; e < 4; e ++)
        {
            O->elem_array_[L][t_e++] = Q->texts[e];
        }
    }

    if (set_up_VBO(O, L))
    {
        return 1;
    }
    else
    {
        free(O->vert_array_[L][0]);
        free(O->cols_array_[L][0]);
        free(O->norm_array_[L][0]);
        free(O->text_array_[L][0]);
        free(O->tang_array_[L][0]);
        free(O->vert_array_[L][1]);
        free(O->cols_array_[L][1]);
        free(O->norm_array_[L][1]);
        free(O->text_array_[L][1]);
        free(O->tang_array_[L][1]);
        free(O->elem_array_[L]);

        return 0;
    }
}

#endif // ARRAYS_H_INCLUDED
