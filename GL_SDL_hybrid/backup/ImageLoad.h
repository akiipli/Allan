#ifndef IMAGELOAD_H_INCLUDED
#define IMAGELOAD_H_INCLUDED

#define MATERIALS_TOTAL 100
#define TEXTURES 100

SDL_Surface * texture = NULL;
SDL_Surface * Surf_Text[TEXTURES];
char Text_Names[TEXTURES][STRLEN];
int Surf_Text_c = 0;

typedef struct
{
    int index;
    int smooth;
    char * Name;
    int texture;
    int Texture_idx;
    int use_texture;
    float R, G, B, A;
}
surface_Material;

surface_Material Materials[MATERIALS_TOTAL];

int Materials_count = 0;

GLuint zero_Texture;

GLuint create_Empty_Texture()
{
    GLuint displayList = glGenLists(1);
    glNewList(displayList, GL_COMPILE);
    glBindTexture(GL_TEXTURE_2D, 0);
    glEndList();

    return displayList;
}

int init_Material(int index)
{
    int i = index % MATERIALS_TOTAL;
    surface_Material * M = &Materials[i];
    M->index = index;
    M->smooth = 0;
    M->Name = "M";
    M->texture = 0;
    M->Texture_idx = 0; // index in displayLists for glCallList
    M->use_texture = 0;
    M->R = 255;
    M->G = 255;
    M->B = 255;
    M->A = 255;
    return i;
}

int Textures_c = 1;

#if DEBUG_WITHOUT_IL == 0
int HandleDevILErrors()
{
	ILenum error = ilGetError();
	if(error != IL_NO_ERROR)
    {
		do
		{
			printf ("\n\n%s\n", iluErrorString(error));
		}
        while ((error = ilGetError()));
		return 0;
	}
	return 1;
}

SDL_Surface * load_ILU_image(char * filename)
{
	SDL_Surface * SDLImage = NULL;
    int t;
    for (t = 0; t < Textures_c; t ++)
    {
        if (strcmp(basename(filename), Text_Names[t]) == 0)
            return SDLImage;
    }

    ILuint ImgId = 0;
    ILuint	width, height;

	ilGenImages(1, &ImgId);

	ilBindImage(ImgId);

	if(!ilLoadImage(filename))
    {
		HandleDevILErrors();
        return NULL;
	}

	width  = ilGetInteger(IL_IMAGE_WIDTH);
	height = ilGetInteger(IL_IMAGE_HEIGHT);

	ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);

	void * Data = malloc(width * height * ilGetInteger(IL_IMAGE_BPP));

    memcpy(Data, ilGetData(), width * height * ilGetInteger(IL_IMAGE_BPP));

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
    int pitch = 4 * width;

    SDLImage = SDL_CreateRGBSurfaceFrom(Data, width, height, 32, pitch, rmask, gmask, bmask, amask);

    ilDeleteImages(1, &ImgId);

    return SDLImage;
}
#endif
SDL_Surface * load_image(char * filename)
{
    SDL_Surface* loadedImage = NULL;
    SDL_Surface* optimizedImage = NULL;
    loadedImage = IMG_Load(filename);
    if(loadedImage != NULL)
    {
        optimizedImage = SDL_DisplayFormat(loadedImage);
        SDL_FreeSurface(loadedImage);
    }
    return optimizedImage;
}

Uint32 get_pixel32(SDL_Surface * surface, int x, int y)
{
    Uint32 *pixels = (Uint32*)surface->pixels;
    return pixels[(y * surface->w) + x];
}

void init_textures(SDL_Surface * screen)
{
    char dirfile[STRLEN];
    dirfile[0] = '\0';
    strcat(dirfile, resources_dir);
    strcat(dirfile, "00_WHITE.png");
#if DEBUG_WITHOUT_IL == 0
    texture = load_ILU_image(dirfile);
#else
    texture = load_image(dirfile);
#endif

    Surf_Text[Surf_Text_c] = texture;

    memcpy(Text_Names[Surf_Text_c++], basename(dirfile), strlen(basename(dirfile)));

    surface_Material Default_Material = {0, 0, "Default", 0, 0, 0, 255, 255, 255, 255};

    surface_Material Green_Material = {1, 0, "Green", 0, 0, 0, 0, 255, 0, 255};

    surface_Material Blue_Material = {2, 0, "Blue", 0, 0, 0, 0, 0, 255, 255};

    surface_Material Yellow_Material = {3, 0, "Yellow", 0, 0, 0, 0, 255, 255, 255};

    memcpy(&Materials[0], &Default_Material, sizeof(surface_Material));

    memcpy(&Materials[1], &Green_Material, sizeof(surface_Material));

    memcpy(&Materials[2], &Blue_Material, sizeof(surface_Material));

    memcpy(&Materials[3], &Yellow_Material, sizeof(surface_Material));

    Materials_count = 4;

    if (texture != NULL)
    {
        Uint32 pix = get_pixel32(texture, 0, 0);

        Uint8 r;
        Uint8 g;
        Uint8 b;
        Uint8 a;

        SDL_GetRGBA(pix, texture->format, &r, &g, &b, &a);

        printf("r g b a %d %d %d %d\n", r, g, b, a);
    }
}

void put_pixel32_swap_r_b(SDL_Surface * surface, int x, int y, Uint32 pixel)
{
    Uint32 * pixels = (Uint32 *)surface->pixels;
//    Uint8 * rgba = (Uint8 *)&pixel;
//    Uint8 r = rgba[0];
//    Uint8 b = rgba[2];
//    rgba[0] = b;
//    rgba[2] = r;
    pixels[(y * surface->w) + x] = pixel;
}

const int FLIP_VERTICAL = 1;
const int FLIP_HORIZONTAL = 2;

SDL_Surface * flip_surface(SDL_Surface * surface, int flags)
{
    SDL_Surface * flipped = NULL;
    flipped = SDL_CreateRGBSurface(SDL_SWSURFACE, surface->w, surface->h, surface->format->BitsPerPixel,
                                   surface->format->Rmask,
                                   surface->format->Gmask,
                                   surface->format->Bmask,
                                   surface->format->Amask);

    if(SDL_MUSTLOCK(surface))
    {
        SDL_LockSurface(surface);
    }
    int x, y, rx, ry;
    for(x = 0, rx = flipped->w - 1; x < flipped->w; x++, rx--)
    {
        for(y = 0, ry = flipped->h - 1; y < flipped->h; y++, ry--)
        {
            Uint32 pixel = get_pixel32( surface, x, y );
            if((flags & FLIP_VERTICAL) && (flags & FLIP_HORIZONTAL))
            {
                put_pixel32_swap_r_b(flipped, rx, ry, pixel);
            }
            else if(flags & FLIP_HORIZONTAL)
            {
                put_pixel32_swap_r_b(flipped, rx, y, pixel);
            }
            else if(flags & FLIP_VERTICAL)
            {
                put_pixel32_swap_r_b(flipped, x, ry, pixel);
            }
        }
    }
    if(SDL_MUSTLOCK(surface))
    {
        SDL_UnlockSurface(surface);
    }
    return flipped;
}

GLuint Textures[TEXTURES];
GLuint displayLists[TEXTURES];
//GLint * TEX_MODE;

int TexFrom_Surface(SDL_Surface * surface)
{
    if (surface == NULL)
        return 0;
//    glGetInternalformativ(GL_TEXTURE_2D​, GL_TEXTURE_IMAGE_FORMAT, GL_TEXTURE_IMAGE_FORMAT​, 100, TEX_MODE);
//    int MODE = GL_RGB;
//
//    if(surface->format->BytesPerPixel == 4)
//    {
//        MODE = GL_RGBA;
//        surface = flip_surface(surface, FLIP_VERTICAL);
//    }
//    else
//    {
//        surface = flip_surface(surface, FLIP_HORIZONTAL | FLIP_VERTICAL);
//    }

    GLuint Texture = Textures[Textures_c]; // generated in main.setup_opengl

    GLuint displayList = glGenLists(1);

    displayLists[Textures_c] = displayList;

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glBindTexture(GL_TEXTURE_2D, Texture);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, surface->w, surface->h, GL_RGBA, GL_UNSIGNED_BYTE, surface->pixels);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface->w, surface->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, surface->pixels);

    glNewList(displayList, GL_COMPILE);
    glBindTexture(GL_TEXTURE_2D, Texture);
    glEndList();

    return Textures_c;
}

int create_Glyph()
{
    char p[] = "ABC";

    GLuint Texture = Textures[Textures_c]; // generated in main.setup_opengl

    GLuint displayList = glGenLists(1);

    displayLists[Textures_c] = displayList;

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glBindTexture(GL_TEXTURE_2D, Texture);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    FT_Set_Pixel_Sizes(face, 0, 48);

    FT_GlyphSlot G = face->glyph;
    FT_Load_Char(face, *p, FT_LOAD_RENDER);

    //gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, surface->w, surface->h, GL_RGBA, GL_UNSIGNED_BYTE, surface->pixels);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, G->bitmap.width, G->bitmap.rows, 0, GL_ALPHA, GL_UNSIGNED_BYTE, G->bitmap.buffer);

    glNewList(displayList, GL_COMPILE);
    glBindTexture(GL_TEXTURE_2D, Texture);
    glEndList();

    return Textures_c;
}

#endif // IMAGELOAD_H_INCLUDED
