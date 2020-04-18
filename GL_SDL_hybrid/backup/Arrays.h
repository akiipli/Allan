#ifndef ARRAYS_H_INCLUDED
#define ARRAYS_H_INCLUDED

#define POLY_RENDER 0
#define ID_RENDER 1

int IsExtensionSupported(char * szTargetExtension)
{
    const GLubyte * pszExtensions = glGetString(GL_EXTENSIONS);
    const GLubyte * glversion = glGetString(GL_VERSION);
    printf("OpenGL %s\n", glversion);
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
        if(pszExtensions[i] == ' ')
            white_spaces[w_count++] = i + 1;
    }
    for (p = 0; p < w_count; p ++)
    {
        memcpy(extension_Name, &pszExtensions[white_spaces[p]], L);
        extension_Name[L] = '\0';
        //printf("%s\n", extension_Name);
        if(strcmp(extension_Name, szTargetExtension) == 0)
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

int draw_Fan_Arrays(object * O, int mode, int update_colors, int update_uv)
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
    glBindBuffer(GL_ARRAY_BUFFER, O->vert_array_buffer);
    //printf("vert array size %d\n", sizeof O->vert_array_[L]);

    glBufferSubData(GL_ARRAY_BUFFER, 0, O->vert_array_size, O->vert_array);
    glVertexPointer(3, GL_FLOAT, 0, 0); // 2d vertexes

    glBindBuffer(GL_ARRAY_BUFFER, O->cols_array_buffer);
    if (update_colors)
        glBufferSubData(GL_ARRAY_BUFFER, 0, O->cols_array_size, O->cols_array);
    glColorPointer(4, GL_FLOAT, 0, 0);

    glBindBuffer(GL_ARRAY_BUFFER, O->norm_array_buffer);
    glBufferSubData(GL_ARRAY_BUFFER, 0, O->norm_array_size, O->norm_array);
    glNormalPointer(GL_FLOAT, 0, 0);

    glBindBuffer(GL_ARRAY_BUFFER, O->text_array_buffer);
    if (update_uv)
        glBufferSubData(GL_ARRAY_BUFFER, 0, O->text_array_size, O->text_array);
    glTexCoordPointer(2, GL_FLOAT, 0, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    int p;
    int s = 0;

    polygon * P;

    for(p = 0; p < O->polycount; p ++)
    {
        P = &O->polys[p / ARRAYSIZE][p % ARRAYSIZE];
        glDrawArrays(GL_TRIANGLE_FAN, s, P->edgecount);
        s += P->edgecount;
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

int draw_Arrays(object * O, int L, int mode, int update_colors, int update_uv)
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
    glBindBuffer(GL_ARRAY_BUFFER, O->vert_array_buffer_[L]);
    //printf("vert array size %d\n", sizeof O->vert_array_[L]);

    glBufferSubData(GL_ARRAY_BUFFER, 0, O->vert_array_size_[L], O->vert_array_[L]);
    glVertexPointer(3, GL_FLOAT, 0, 0); // 2d vertexes

    glBindBuffer(GL_ARRAY_BUFFER, O->cols_array_buffer_[L]);
    if (update_colors)
        glBufferSubData(GL_ARRAY_BUFFER, 0, O->cols_array_size_[L], O->cols_array_[L]);
    glColorPointer(4, GL_FLOAT, 0, 0);

    glBindBuffer(GL_ARRAY_BUFFER, O->norm_array_buffer_[L]);
    glBufferSubData(GL_ARRAY_BUFFER, 0, O->norm_array_size_[L], O->norm_array_[L]);
    glNormalPointer(GL_FLOAT, 0, 0);

    glBindBuffer(GL_ARRAY_BUFFER, O->text_array_buffer_[L]);
    if (update_uv)
        glBufferSubData(GL_ARRAY_BUFFER, 0, O->text_array_size_[L], O->text_array_[L]);
    glTexCoordPointer(2, GL_FLOAT, 0, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glDrawArrays(GL_QUADS, 0, O->quadcount_[L] * 4);

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
    GLuint buffers[4];
    glGenBuffers(4, buffers);

    printf("buffers %u %u %u %u \n", buffers[0], buffers[1], buffers[2], buffers[3]);

    O->vert_array_buffer = buffers[0];
    O->cols_array_buffer = buffers[1];
    O->norm_array_buffer = buffers[2];
    O->text_array_buffer = buffers[3];

    glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
    glBufferData(GL_ARRAY_BUFFER, O->vert_array_size, O->vert_array, GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, buffers[1]);
    glBufferData(GL_ARRAY_BUFFER, O->cols_array_size, O->cols_array, GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, buffers[2]);
    glBufferData(GL_ARRAY_BUFFER, O->norm_array_size, O->norm_array, GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, buffers[3]);
    glBufferData(GL_ARRAY_BUFFER, O->text_array_size, O->text_array, GL_DYNAMIC_DRAW);

    return 1;
}

int set_up_VBO(object * O, int L)
{
    printf("set up VBO\n");
    GLuint buffers[4];
    glGenBuffers(4, buffers);

    printf("buffers %u %u %u %u \n", buffers[0], buffers[1], buffers[2], buffers[3]);

    O->vert_array_buffer_[L] = buffers[0];
    O->cols_array_buffer_[L] = buffers[1];
    O->norm_array_buffer_[L] = buffers[2];
    O->text_array_buffer_[L] = buffers[3];

    glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
    glBufferData(GL_ARRAY_BUFFER, O->vert_array_size_[L], O->vert_array_[L], GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, buffers[1]);
    glBufferData(GL_ARRAY_BUFFER, O->cols_array_size_[L], O->cols_array_[L], GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, buffers[2]);
    glBufferData(GL_ARRAY_BUFFER, O->norm_array_size_[L], O->norm_array_[L], GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, buffers[3]);
    glBufferData(GL_ARRAY_BUFFER, O->text_array_size_[L], O->text_array_[L], GL_DYNAMIC_DRAW);

    return 1;
}

int set_up_Fan_array(object * O)
{
    printf("set up Fan array\n");
    uv * UV;
    vertex * V;
    polygon * P;
    int p, e, idx;

    int data_l = 0;

    for (p = 0; p < O->polycount; p ++)
    {
        P = &O->polys[p / ARRAYSIZE][p % ARRAYSIZE];
        data_l += P->edgecount;
    }

    O->vert_array_size = data_l * 3 * sizeof(GLfloat);
    O->cols_array_size = data_l * 4 * sizeof(GLfloat);
    O->norm_array_size = data_l * 3 * sizeof(GLfloat);
    O->text_array_size = data_l * 2 * sizeof(GLfloat);

    O->text_array = malloc(O->vert_array_size * sizeof(GLfloat));
    if (O->text_array == NULL) return 0;
    O->cols_array = malloc(O->cols_array_size * sizeof(GLfloat));
    if (O->cols_array == NULL) return 0;
    O->vert_array = malloc(O->norm_array_size * sizeof(GLfloat));
    if (O->vert_array == NULL) return 0;
    O->norm_array = malloc(O->text_array_size * sizeof(GLfloat));
    if (O->norm_array == NULL) return 0;

    int v_c = 0;
    int c_c = 0;
    int n_c = 0;
    int t_c = 0;

    for(p = 0; p < O->polycount; p ++)
    {
        P = &O->polys[p / ARRAYSIZE][p % ARRAYSIZE];
        for(e = 0; e < P->edgecount; e ++)
        {
            idx = P->verts[e];
            V = &O->verts[idx / ARRAYSIZE][idx % ARRAYSIZE];
            idx = P->texts[e];
            UV = &O->uvtex[idx / ARRAYSIZE][idx % ARRAYSIZE];
            O->vert_array[v_c++] = V->Tx;
            O->vert_array[v_c++] = V->Ty;
            O->vert_array[v_c++] = V->Tz;
            O->cols_array[c_c++] = 1;
            O->cols_array[c_c++] = 1;
            O->cols_array[c_c++] = 1;
            O->cols_array[c_c++] = 1;
            O->norm_array[n_c++] = V->N.Tx;
            O->norm_array[n_c++] = V->N.Ty;
            O->norm_array[n_c++] = V->N.Tz;
            O->text_array[t_c++] = UV->u;
            O->text_array[t_c++] = UV->v;
        }
    }
    if(set_up_Fan_VBO(O))
    {
        return 1;
    }
    else
    {
        free(O->vert_array);
        free(O->cols_array);
        free(O->norm_array);
        free(O->text_array);
        return 0;
    }
}

int set_up_vertex_array(object * O, int L)
{
    printf("set up vertex array\n");
    uv * UV;
    vertex * V;
    quadrant * Q;
    int q, e, idx;

    int data_l = O->quadcount_[L] * 4;

    O->vert_array_size_[L] = data_l * 3 * sizeof(GLfloat);
    O->cols_array_size_[L] = data_l * 4 * sizeof(GLfloat);
    O->norm_array_size_[L] = data_l * 3 * sizeof(GLfloat);
    O->text_array_size_[L] = data_l * 2 * sizeof(GLfloat);

    O->text_array_[L] = malloc(O->vert_array_size_[L] * sizeof(GLfloat));
    if (O->text_array_[L] == NULL) return 0;
    O->cols_array_[L] = malloc(O->cols_array_size_[L] * sizeof(GLfloat));
    if (O->cols_array_[L] == NULL) return 0;
    O->vert_array_[L] = malloc(O->norm_array_size_[L] * sizeof(GLfloat));
    if (O->vert_array_[L] == NULL) return 0;
    O->norm_array_[L] = malloc(O->text_array_size_[L] * sizeof(GLfloat));
    if (O->norm_array_[L] == NULL) return 0;

    int v_c = 0;
    int c_c = 0;
    int n_c = 0;
    int t_c = 0;

    for(q = 0; q < O->quadcount_[L]; q ++)
    {
        Q = &O->quads_[L][q / ARRAYSIZE][q % ARRAYSIZE];
        for(e = 0; e < 4; e ++)
        {
            idx = Q->verts[e];
            V = &O->verts_[L][idx / ARRAYSIZE][idx % ARRAYSIZE];
            idx = Q->texts[e];
            UV = &O->uvtex_[L][idx / ARRAYSIZE][idx % ARRAYSIZE];
            O->vert_array_[L][v_c++] = V->Tx;
            O->vert_array_[L][v_c++] = V->Ty;
            O->vert_array_[L][v_c++] = V->Tz;
            O->cols_array_[L][c_c++] = 1;
            O->cols_array_[L][c_c++] = 1;
            O->cols_array_[L][c_c++] = 1;
            O->cols_array_[L][c_c++] = 1;
            O->norm_array_[L][n_c++] = V->N.Tx;
            O->norm_array_[L][n_c++] = V->N.Ty;
            O->norm_array_[L][n_c++] = V->N.Tz;
            O->text_array_[L][t_c++] = UV->u;
            O->text_array_[L][t_c++] = UV->v;
        }
    }
    if(set_up_VBO(O, L))
    {
        return 1;
    }
    else
    {
        free(O->vert_array_[L]);
        free(O->cols_array_[L]);
        free(O->norm_array_[L]);
        free(O->text_array_[L]);
        return 0;
    }
}

#endif // ARRAYS_H_INCLUDED
