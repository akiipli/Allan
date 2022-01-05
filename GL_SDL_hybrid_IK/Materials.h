/*
The MIT License

Copyright <2018> <Allan Kiipli>
*/

#ifndef MATERIALS_H_INCLUDED
#define MATERIALS_H_INCLUDED

typedef struct
{
    union
    {
        float Color[4];
        struct
        {
            float R;
            float G;
            float B;
            float A;
        };
    };
}
color;

typedef struct
{
    int index;
    int smooth;
    char Name[STRLEN];
    int texture;
    int Texture_idx;
    int use_texture;
    //float R, G, B, A;
    color RGBA;
    float Shininess;
    float Displacement;
    int normal;
    int Normal_idx;
    int bump;
    int Bump_idx;
}
surface_Material;

surface_Material Materials[MATERIALS_TOTAL];
int Materials_count = 0;

int currentMaterial = 0;

int THUMBNAILS = 0;

GLuint Material_Textures[MATERIALS_TOTAL];

GLsizei Material_Thumbnail_width = 30;
GLsizei Material_Thumbnail_height = 30;

int Materials_c = 0;

int init_Material(int index)
{
    int i = index % MATERIALS_TOTAL;
    surface_Material * M = &Materials[i];
    M->index = index;
    M->smooth = 1;
    sprintf(M->Name, "M");
    M->texture = 0;
    M->normal = 0;
    M->bump = 0;
    M->Texture_idx = 0; // index in displayLists for glCallList
    M->Normal_idx = 0; // index in normalsLists for glCallList
    M->Bump_idx = 0; // index in bumpsLists for glCallList
    M->use_texture = 1;
    M->RGBA.R = 255;
    M->RGBA.G = 255;
    M->RGBA.B = 255;
    M->RGBA.A = 255;
    M->Shininess = 2;
    return i;
}

void set_Material_H_Button(int index)
{
    int i;
    for (i = 0; i < H_MATR_NUM; i ++)
    {
        Button_h_matr[i].color = UI_GRAYB;
    }
    if (index > -1)
        Button_h_matr[index].color = UI_GRAYD;
}

GLuint materialTexFBO;

SDL_Surface * Material_Thumbnail_Surfaces[MATERIALS_TOTAL];

int generate_Material_Thumbnail_Textures(GLsizei width, GLsizei height)
{
    int t;

    Uint32 rmask, gmask, bmask, amask;

#if SDL_BYTEORDER == SDL_BIG_ENDIAN
    rmask = 0xff000000;
    gmask = 0x00ff0000;
    bmask = 0x0000ff00;
    amask = 0x000000ff;
#else
    rmask = 0x000000ff;
    gmask = 0x0000ff00;
    bmask = 0x00ff0000;
    amask = 0xff000000;
#endif

    for (t = 0; t < MATERIALS_TOTAL; t ++)
    {
        GLuint Texture = Material_Textures[t]; // generated in main.setup_opengl

        GLuint displayList = glGenLists(1);

        Material_Textures_Lists[t] = displayList;

        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glBindTexture(GL_TEXTURE_2D, Texture);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

        Material_Thumbnail_Surfaces[t] = SDL_CreateRGBSurface(SDL_SWSURFACE, width, height, 32, rmask, gmask, bmask, amask);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

        glNewList(displayList, GL_COMPILE);
        glBindTexture(GL_TEXTURE_2D, Texture);
        glEndList();
    }

    return MATERIALS_TOTAL;
}

int setup_Material_Thumbnail_FBO()
{
    int r = 0;
    GLuint Texture = Material_Textures[0];
    // Assign the depth buffer texture to texture channel 0
    //glActiveTexture(GL_TEXTURE0);
    //glBindTexture(GL_TEXTURE_2D, Texture);

    // Create and set up the FBO
    glGenFramebuffers(1, &materialTexFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, materialTexFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1,
                           GL_TEXTURE_2D, Texture, 0);

    GLenum drawBuffers[] = {GL_NONE};
    glDrawBuffers(1, drawBuffers);

    GLenum result = glCheckFramebufferStatus(GL_FRAMEBUFFER);

    if( result == GL_FRAMEBUFFER_COMPLETE)
    {
        printf("Framebuffer is complete.\n");
        r = 1;
    }
    else
    {
        printf("Framebuffer is not complete.\n");
        r = 0;
    }

    glBindFramebuffer(GL_FRAMEBUFFER,0);

    return r;
}

#endif // MATERIALS_H_INCLUDED
