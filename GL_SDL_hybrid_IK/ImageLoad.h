/*
The MIT License

Copyright <2018> <Allan Kiipli>
*/

#ifndef IMAGELOAD_H_INCLUDED
#define IMAGELOAD_H_INCLUDED

#define TEXTURES 100

SDL_Surface * texture = NULL;
SDL_Surface * normsrf = NULL;
SDL_Surface * bumpsrf = NULL;
SDL_Surface * Surf_Text[TEXTURES];
SDL_Surface * Surf_Norm[TEXTURES];
SDL_Surface * Surf_Bump[TEXTURES];
char Text_Names[TEXTURES][STRLEN];
char Text_Names_Full[TEXTURES][STRLEN];
char Norm_Names[TEXTURES][STRLEN];
char Norm_Names_Full[TEXTURES][STRLEN];
char Bump_Names[TEXTURES][STRLEN];
char Bump_Names_Full[TEXTURES][STRLEN];
int Surf_Text_c = 0;
int Surf_Norm_c = 0;
int Surf_Bump_c = 0;
int Textures_c = 0;
int Normals_c = 0;
int Bumps_c = 0;

typedef struct
{
    char texture_Name[STRLEN];
    int texture;
    int Texture_idx;
}
textPack;

textPack text_Pack[TEXTURES];
textPack norm_Pack[TEXTURES];
textPack bump_Pack[TEXTURES];

int text_Pack_c = 0;
int norm_Pack_c = 0;
int bump_Pack_c = 0;

GLuint zero_Texture, zero_Normal, zero_Bump;

GLuint create_Empty_Texture()
{
    GLuint displayList = glGenLists(1);
    glNewList(displayList, GL_COMPILE);
    glBindTexture(GL_TEXTURE_2D, 0);
    glEndList();

    return displayList;
}

#if DEBUG_WITHOUT_IL == 0
int HandleDevILErrors()
{
	ILenum error = ilGetError();
	if (error != IL_NO_ERROR)
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

SDL_Surface * reload_ILU_image(char * filename)
{
	SDL_Surface * SDLImage = NULL;

    ILuint ImgId = 0;
    ILuint	width, height;

	ilGenImages(1, &ImgId);

	ilBindImage(ImgId);

	if (!ilLoadImage(filename))
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

SDL_Surface * load_ILU_image(char * filename, int normal_image, int bump_image)
{
	SDL_Surface * SDLImage = NULL;

    int b, n, t;

    if (normal_image)
    {
        for (n = 0; n < Normals_c; n ++)
        {
            if (strcmp(basename(filename), Norm_Names[n]) == 0)
                return SDLImage;
        }
    }
    else if (bump_image)
    {
        for (b = 0; b < Bumps_c; b ++)
        {
            if (strcmp(basename(filename), Bump_Names[b]) == 0)
                return SDLImage;
        }
    }
    else
    {
        for (t = 0; t < Textures_c; t ++)
        {
            if (strcmp(basename(filename), Text_Names[t]) == 0)
                return SDLImage;
        }
    }

    ILuint ImgId = 0;
    ILuint	width, height;

	ilGenImages(1, &ImgId);

	ilBindImage(ImgId);

	if (!ilLoadImage(filename))
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
SDL_Surface * load_image(char * filename, int normal_image, int bump_image)
{
    int b, n, t;

    if (normal_image)
    {
        for (n = 0; n < Normals_c; n ++)
        {
            if (strcmp(basename(filename), Norm_Names[n]) == 0)
                return NULL;
        }
    }
    else if (bump_image)
    {
        for (b = 0; b < Bumps_c; b ++)
        {
            if (strcmp(basename(filename), Bump_Names[b]) == 0)
                return NULL;
        }
    }
    else
    {
        for (t = 0; t < Textures_c; t ++)
        {
            if (strcmp(basename(filename), Text_Names[t]) == 0)
                return NULL;
        }
    }

    SDL_Surface* loadedImage = NULL;
    SDL_Surface* optimizedImage = NULL;
    loadedImage = IMG_Load(filename);
    if (loadedImage != NULL)
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

void average_Seam_Pixels(SDL_Surface * surface, object * O, int L)
{
    Uint32 pixel, Pixel;
    int rgba[4];
    unsigned char * p, * P;
    int x, y;

    int idx, v0, v;
    int pix_count;

    vertex * V;
    uv * UV;

    if (SDL_MUSTLOCK(surface))
    {
        SDL_LockSurface(surface);
    }

    Uint32 * pixels = (Uint32 *)surface->pixels;

    if (L == -1)
    {
        for (v = 0; v < O->vertcount; v ++)
        {
            V = &O->verts[v / ARRAYSIZE][v % ARRAYSIZE];
            if (V->uv_vertcount > 1)
            {
                rgba[0] = 0;
                rgba[1] = 0;
                rgba[2] = 0;
                rgba[3] = 0;
                pix_count = 0;
                for (v0 = 0; v0 < V->uv_vertcount; v0 ++)
                {
                    idx = V->uv_verts[v0];
                    UV = &O->uvtex[idx / ARRAYSIZE][idx % ARRAYSIZE];
                    x = (int)((float)surface->w * UV->u);
                    y = (int)((float)surface->h * UV->v);
                    pixel = pixels[(y * surface->w) + x];
                    p = (unsigned char*)(&pixel);
                    rgba[0] += (unsigned char)*(p);
                    rgba[1] += (unsigned char)*(p + 1);
                    rgba[2] += (unsigned char)*(p + 2);
                    rgba[3] += (unsigned char)*(p + 3);
                    pix_count ++;
                }
                rgba[0] /= pix_count;
                rgba[1] /= pix_count;
                rgba[2] /= pix_count;
                rgba[3] /= pix_count;
                P = (unsigned char*)(&Pixel);
                *(P) = (unsigned char)rgba[0];
                *(P + 1) = (unsigned char)rgba[1];
                *(P + 2) = (unsigned char)rgba[2];
                *(P + 3) = (unsigned char)rgba[3];
//                Pixel = pixel;
                for (v0 = 0; v0 < V->uv_vertcount; v0 ++)
                {
                    idx = V->uv_verts[v0];
                    UV = &O->uvtex[idx / ARRAYSIZE][idx % ARRAYSIZE];
                    x = (int)((float)surface->w * UV->u);
                    y = (int)((float)surface->h * UV->v);
                    pixels[(y * surface->w) + x] = Pixel;
                    if (y - 1 >= 0 && x - 1 >= 0)
                    pixels[((y - 1) * surface->w) + (x - 1)] = Pixel;
                    if (y - 1 >= 0)
                    pixels[((y - 1) * surface->w) + x] = Pixel;
                    if (y - 1 >= 0 && x + 1 <= surface->w)
                    pixels[((y - 1) * surface->w) + (x + 1)] = Pixel;
                    if (x - 1 >= 0)
                    pixels[(y * surface->w) + (x - 1)] = Pixel;
                    if (x + 1 <= surface->w)
                    pixels[(y * surface->w) + (x + 1)] = Pixel;
                    if (y + 1 <= surface->h && x - 1 >= 0)
                    pixels[((y + 1) * surface->w) + (x - 1)] = Pixel;
                    if (y + 1 <= surface->h)
                    pixels[((y + 1) * surface->w) + x] = Pixel;
                    if (y + 1 <= surface->h && x + 1 <= surface->w)
                    pixels[((y + 1) * surface->w) + (x + 1)] = Pixel;
                }
            }
        }
    }
    else if (O->subdlevel <= L)
    {
        for (v = 0; v < O->vertcount_[L]; v ++)
        {
            V = &O->verts_[L][v / ARRAYSIZE][v % ARRAYSIZE];
            if (V->uv_vertcount > 1)
            {
                rgba[0] = 0;
                rgba[1] = 0;
                rgba[2] = 0;
                rgba[3] = 0;
                pix_count = 0;
                for (v0 = 0; v0 < V->uv_vertcount; v0 ++)
                {
                    idx = V->uv_verts[v0];
                    UV = &O->uvtex_[L][idx / ARRAYSIZE][idx % ARRAYSIZE];
                    x = (int)((float)surface->w * UV->u);
                    y = (int)((float)surface->h * UV->v);
                    pixel = pixels[(y * surface->w) + x];
                    p = (unsigned char*)(&pixel);
                    rgba[0] += (unsigned char)*(p);
                    rgba[1] += (unsigned char)*(p + 1);
                    rgba[2] += (unsigned char)*(p + 2);
                    rgba[3] += (unsigned char)*(p + 3);
                    pix_count ++;
                }
                rgba[0] /= pix_count;
                rgba[1] /= pix_count;
                rgba[2] /= pix_count;
                rgba[3] /= pix_count;
                P = (unsigned char*)(&Pixel);
                *(P) = (unsigned char)rgba[0];
                *(P + 1) = (unsigned char)rgba[1];
                *(P + 2) = (unsigned char)rgba[2];
                *(P + 3) = (unsigned char)rgba[3];
//                Pixel = pixel;
                for (v0 = 0; v0 < V->uv_vertcount; v0 ++)
                {
                    idx = V->uv_verts[v0];
                    UV = &O->uvtex_[L][idx / ARRAYSIZE][idx % ARRAYSIZE];
                    x = (int)((float)surface->w * UV->u);
                    y = (int)((float)surface->h * UV->v);
                    pixels[(y * surface->w) + x] = Pixel;
                    if (y - 1 >= 0 && x - 1 >= 0)
                    pixels[((y - 1) * surface->w) + (x - 1)] = Pixel;
                    if (y - 1 >= 0)
                    pixels[((y - 1) * surface->w) + x] = Pixel;
                    if (y - 1 >= 0 && x + 1 <= surface->w)
                    pixels[((y - 1) * surface->w) + (x + 1)] = Pixel;
                    if (x - 1 >= 0)
                    pixels[(y * surface->w) + (x - 1)] = Pixel;
                    if (x + 1 <= surface->w)
                    pixels[(y * surface->w) + (x + 1)] = Pixel;
                    if (y + 1 <= surface->h && x - 1 >= 0)
                    pixels[((y + 1) * surface->w) + (x - 1)] = Pixel;
                    if (y + 1 <= surface->h)
                    pixels[((y + 1) * surface->w) + x] = Pixel;
                    if (y + 1 <= surface->h && x + 1 <= surface->w)
                    pixels[((y + 1) * surface->w) + (x + 1)] = Pixel;
                }
            }
        }
    }
    if (SDL_MUSTLOCK(surface))
    {
        SDL_UnlockSurface(surface);
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

    if (SDL_MUSTLOCK(surface))
    {
        SDL_LockSurface(surface);
    }
    int x, y, rx, ry;
    for(x = 0, rx = flipped->w - 1; x < flipped->w; x++, rx--)
    {
        for(y = 0, ry = flipped->h - 1; y < flipped->h; y++, ry--)
        {
            Uint32 pixel = get_pixel32( surface, x, y );
            if ((flags & FLIP_VERTICAL) && (flags & FLIP_HORIZONTAL))
            {
                put_pixel32_swap_r_b(flipped, rx, ry, pixel);
            }
            else if (flags & FLIP_HORIZONTAL)
            {
                put_pixel32_swap_r_b(flipped, rx, y, pixel);
            }
            else if (flags & FLIP_VERTICAL)
            {
                put_pixel32_swap_r_b(flipped, x, ry, pixel);
            }
        }
    }
    if (SDL_MUSTLOCK(surface))
    {
        SDL_UnlockSurface(surface);
    }
    return flipped;
}

GLuint Textures[TEXTURES];
GLuint displayLists[TEXTURES];
GLuint Normals[TEXTURES];
GLuint normalsLists[TEXTURES];
GLuint Bumps[TEXTURES];
GLuint bumpsLists[TEXTURES];
//GLint * TEX_MODE;

int update_BumpFrom_Surface(SDL_Surface * surface, int b)
{
    if (surface == NULL)
        return 0;
//    glGetInternalformativ(GL_TEXTURE_2D​, GL_TEXTURE_IMAGE_FORMAT, GL_TEXTURE_IMAGE_FORMAT​, 100, TEX_MODE);
//    int MODE = GL_RGB;
//
//    if (surface->format->BytesPerPixel == 4)
//    {
//        MODE = GL_RGBA;
//        surface = flip_surface(surface, FLIP_VERTICAL);
//    }
//    else
//    {
//        surface = flip_surface(surface, FLIP_HORIZONTAL | FLIP_VERTICAL);
//    }

    GLuint Bump = Bumps[b]; // generated in main.setup_opengl

    GLuint bumpList = glGenLists(1);

    glDeleteLists(bumpsLists[b], 1);

    bumpsLists[b] = bumpList;

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glBindTexture(GL_TEXTURE_2D, Bump);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, surface->w, surface->h, GL_RGBA, GL_UNSIGNED_BYTE, surface->pixels);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface->w, surface->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, surface->pixels);

    glNewList(bumpList, GL_COMPILE);
    glBindTexture(GL_TEXTURE_2D, Bump);
    glEndList();

    return b;
}

int BumpFrom_Surface(SDL_Surface * surface)
{
    if (surface == NULL)
        return 0;
//    glGetInternalformativ(GL_TEXTURE_2D​, GL_TEXTURE_IMAGE_FORMAT, GL_TEXTURE_IMAGE_FORMAT​, 100, TEX_MODE);
//    int MODE = GL_RGB;
//
//    if (surface->format->BytesPerPixel == 4)
//    {
//        MODE = GL_RGBA;
//        surface = flip_surface(surface, FLIP_VERTICAL);
//    }
//    else
//    {
//        surface = flip_surface(surface, FLIP_HORIZONTAL | FLIP_VERTICAL);
//    }

    GLuint Bump = Bumps[Bumps_c]; // generated in main.setup_opengl

    GLuint bumpList = glGenLists(1);

    bumpsLists[Bumps_c] = bumpList;

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glBindTexture(GL_TEXTURE_2D, Bump);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, surface->w, surface->h, GL_RGBA, GL_UNSIGNED_BYTE, surface->pixels);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface->w, surface->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, surface->pixels);

    glNewList(bumpList, GL_COMPILE);
    glBindTexture(GL_TEXTURE_2D, Bump);
    glEndList();

    return Bumps_c;
}

int update_NormFrom_Surface(SDL_Surface * surface, int n)
{
    if (surface == NULL)
        return 0;
//    glGetInternalformativ(GL_TEXTURE_2D​, GL_TEXTURE_IMAGE_FORMAT, GL_TEXTURE_IMAGE_FORMAT​, 100, TEX_MODE);
//    int MODE = GL_RGB;
//
//    if (surface->format->BytesPerPixel == 4)
//    {
//        MODE = GL_RGBA;
//        surface = flip_surface(surface, FLIP_VERTICAL);
//    }
//    else
//    {
//        surface = flip_surface(surface, FLIP_HORIZONTAL | FLIP_VERTICAL);
//    }

    GLuint Normal = Normals[n]; // generated in main.setup_opengl

    GLuint normalList = glGenLists(1);

    glDeleteLists(normalsLists[n], 1);

    normalsLists[n] = normalList;

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glBindTexture(GL_TEXTURE_2D, Normal);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, surface->w, surface->h, GL_RGBA, GL_UNSIGNED_BYTE, surface->pixels);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface->w, surface->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, surface->pixels);

    glNewList(normalList, GL_COMPILE);
    glBindTexture(GL_TEXTURE_2D, Normal);
    glEndList();

    return n;
}

int NormFrom_Surface(SDL_Surface * surface)
{
    if (surface == NULL)
        return 0;
//    glGetInternalformativ(GL_TEXTURE_2D​, GL_TEXTURE_IMAGE_FORMAT, GL_TEXTURE_IMAGE_FORMAT​, 100, TEX_MODE);
//    int MODE = GL_RGB;
//
//    if (surface->format->BytesPerPixel == 4)
//    {
//        MODE = GL_RGBA;
//        surface = flip_surface(surface, FLIP_VERTICAL);
//    }
//    else
//    {
//        surface = flip_surface(surface, FLIP_HORIZONTAL | FLIP_VERTICAL);
//    }

    GLuint Normal = Normals[Normals_c]; // generated in main.setup_opengl

    GLuint normalList = glGenLists(1);

    normalsLists[Normals_c] = normalList;

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glBindTexture(GL_TEXTURE_2D, Normal);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, surface->w, surface->h, GL_RGBA, GL_UNSIGNED_BYTE, surface->pixels);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface->w, surface->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, surface->pixels);

    glNewList(normalList, GL_COMPILE);
    glBindTexture(GL_TEXTURE_2D, Normal);
    glEndList();

    return Normals_c;
}

int update_TexFrom_Surface(SDL_Surface * surface, int t)
{
    if (surface == NULL)
        return 0;
//    glGetInternalformativ(GL_TEXTURE_2D​, GL_TEXTURE_IMAGE_FORMAT, GL_TEXTURE_IMAGE_FORMAT​, 100, TEX_MODE);
//    int MODE = GL_RGB;
//
//    if (surface->format->BytesPerPixel == 4)
//    {
//        MODE = GL_RGBA;
//        surface = flip_surface(surface, FLIP_VERTICAL);
//    }
//    else
//    {
//        surface = flip_surface(surface, FLIP_HORIZONTAL | FLIP_VERTICAL);
//    }

    GLuint Texture = Textures[t]; // generated in main.setup_opengl

    GLuint displayList = glGenLists(1);

    glDeleteLists(displayLists[t], 1);

    displayLists[t] = displayList;

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

    return t;
}

int TexFrom_Surface(SDL_Surface * surface)
{
    if (surface == NULL)
        return 0;
//    glGetInternalformativ(GL_TEXTURE_2D​, GL_TEXTURE_IMAGE_FORMAT, GL_TEXTURE_IMAGE_FORMAT​, 100, TEX_MODE);
//    int MODE = GL_RGB;
//
//    if (surface->format->BytesPerPixel == 4)
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

void create_Bump_Map()
{
    char dirfile[STRLEN];
    dirfile[0] = '\0';
    strcat(dirfile, resources_dir);
    strcat(dirfile, "PM3D_Plane3D_2-DM_u0_v0.PSD");
#if DEBUG_WITHOUT_IL == 0
    bumpsrf = load_ILU_image(dirfile, 0, 1);
#else
    normsrf = load_image(dirfile, 0, 1);
#endif

    Surf_Bump[Surf_Bump_c] = bumpsrf;
    sprintf(Bump_Names[Surf_Bump_c], basename(dirfile));

    Materials[0].Bump_idx = BumpFrom_Surface(bumpsrf);
    Materials[0].bump = Surf_Bump_c;

    sprintf(bump_Pack[bump_Pack_c].texture_Name, dirfile);
    bump_Pack[bump_Pack_c].texture = Materials[0].bump;
    bump_Pack[bump_Pack_c].Texture_idx = Materials[0].Bump_idx;
    bump_Pack_c ++;

    Surf_Bump_c ++;
    Bumps_c ++;
}

void create_Normal_Map()
{
    char dirfile[STRLEN];
    dirfile[0] = '\0';
    strcat(dirfile, resources_dir);
    strcat(dirfile, "PM3D_Plane3D_1-NM_u0_v0.tif");
#if DEBUG_WITHOUT_IL == 0
    normsrf = load_ILU_image(dirfile, 1, 0);
#else
    normsrf = load_image(dirfile, 1, 0);
#endif

    Surf_Norm[Surf_Norm_c] = normsrf;
    sprintf(Norm_Names[Surf_Norm_c], basename(dirfile));

    Materials[0].Normal_idx = NormFrom_Surface(normsrf);
    Materials[0].normal = Surf_Norm_c;

    sprintf(norm_Pack[norm_Pack_c].texture_Name, dirfile);
    norm_Pack[norm_Pack_c].texture = Materials[0].normal;
    norm_Pack[norm_Pack_c].Texture_idx = Materials[0].Normal_idx;
    norm_Pack_c ++;

    Surf_Norm_c ++;
    Normals_c ++;
}

void init_textures(SDL_Surface * screen)
{
    char dirfile[STRLEN];
    dirfile[0] = '\0';
    strcat(dirfile, resources_dir);
    strcat(dirfile, "00_WHITE.png");
#if DEBUG_WITHOUT_IL == 0
    texture = load_ILU_image(dirfile, 0, 0);
#else
    texture = load_image(dirfile, 0, 0);
#endif

    Surf_Text[Surf_Text_c] = texture;
    sprintf(Text_Names[Surf_Text_c], basename(dirfile));

    surface_Material Default_Material = {0, 0, "Default", 0, 0, 0, 255, 255, 255, 255, 2, 0, 0, 0};
    surface_Material Green_Material   = {1, 0, "Green",   0, 0, 0, 0,   255, 0,   255, 2, 0, 0, 0};
    surface_Material Blue_Material    = {2, 0, "Blue",    0, 0, 0, 0,   0,   250, 255, 2, 0, 0, 0};
    surface_Material Yellow_Material  = {3, 0, "Yellow",  0, 0, 0, 255, 255, 0,   255, 2, 0, 0, 0};
    memcpy(&Materials[0], &Default_Material, sizeof(surface_Material));
    memcpy(&Materials[1], &Green_Material, sizeof(surface_Material));
    memcpy(&Materials[2], &Blue_Material, sizeof(surface_Material));
    memcpy(&Materials[3], &Yellow_Material, sizeof(surface_Material));

    Materials_count = 4;

    Materials[0].Texture_idx = TexFrom_Surface(texture);
    Materials[0].texture = Surf_Text_c;

    sprintf(text_Pack[text_Pack_c].texture_Name, dirfile);
    text_Pack[text_Pack_c].texture = Materials[0].texture;
    text_Pack[text_Pack_c].Texture_idx = Materials[0].Texture_idx;
    text_Pack_c ++;

    Surf_Text_c ++;
    Textures_c ++;

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

    FT_Set_Pixel_Sizes(face[0], 0, 48);

    FT_GlyphSlot G = face[0]->glyph;
    FT_Load_Char(face[0], *p, FT_LOAD_RENDER);

    //gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, surface->w, surface->h, GL_RGBA, GL_UNSIGNED_BYTE, surface->pixels);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, G->bitmap.width, G->bitmap.rows, 0, GL_ALPHA, GL_UNSIGNED_BYTE, G->bitmap.buffer);

    glNewList(displayList, GL_COMPILE);
    glBindTexture(GL_TEXTURE_2D, Texture);
    glEndList();

    return Textures_c;
}

void render_thumbnail(float x, float y, int m_index)
{
    //glDisable(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE0);
	//glBindTexture(GL_TEXTURE_2D, 0);
	SDL_Surface * surface = Material_Thumbnail_Surfaces[m_index];
	glBindTexture(GL_TEXTURE_2D, Material_Textures[m_index]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface->w, surface->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, surface->pixels);

    point box[4] =
    {{10, y + 5, 0, 1},
     {25, y + 5, 1, 1},
     {25, y - 10, 1, 0},
     {10, y - 10, 0, 0}};

    int e;

    //glColor4ub(0, 0, 255, 255);

    glBegin(GL_QUADS);
    for (e = 0; e < 4; e ++)
    {
        glTexCoord2f(box[e].s, box[e].t);
        glVertex2f(box[e].x, box[e].y);
    }
    glEnd();
}

void render_thumbnail_(float x, float y, int m_index)
{
    glActiveTexture(GL_TEXTURE0);
    //glCallList(displayLists[Textures_c - 1]);
	//glCallList(Material_Textures_Lists[m_index]);
	SDL_Surface * surface = Material_Thumbnail_Surfaces[m_index];
	glBindTexture(GL_TEXTURE_2D, Material_Textures[m_index]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface->w, surface->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, surface->pixels);

	glUniform1i(uniform_tex1, 0);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, vbo_tex);
	//glBufferSubData(GL_ARRAY_BUFFER, 0, 8 * sizeof(GLfloat), tex_coords);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);

    glBindBuffer(GL_ARRAY_BUFFER, vbo_pos);
    //glBufferSubData(GL_ARRAY_BUFFER, 0, 8 * sizeof(GLfloat), pos_coords);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);

    memcpy(pos_coords,(GLfloat[8])
    {10,  y + 5,
     25,  y + 5,
     25,  y - 10,
     10,  y - 10}, sizeof pos_coords);

    /* Draw the character on the screen */
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(GLfloat[8]), pos_coords);
    glDrawArrays(GL_QUADS, 0, 4);

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
}

void display_thumbnail_(float origin_x, float origin_y, int m_index, float color[4])
{
    glMatrixMode(GL_PROJECTION);

    glGetFloatv(GL_PROJECTION_MATRIX, projectionMatrix);

	glUseProgram(T_program[0]);

	glUniform1i(uniform_tex0, 0);

	glUniformMatrix4fv(uniform_proj1, 1, GL_FALSE, projectionMatrix);

	glUniform4fv(uniform_color0, 1, color);

	render_thumbnail_(origin_x, origin_y, m_index);

	glUseProgram(0);
}

void draw_thumbnail(int origin_x, int origin_y, int m_index)
{
    if (SHADERS)
    {
        float color[4];
        glGetFloatv(GL_CURRENT_COLOR, color);
        display_thumbnail_(origin_x, origin_y, m_index, color);
    }
    else
    {
        render_thumbnail(origin_x, origin_y, m_index);
    }
}

#endif // IMAGELOAD_H_INCLUDED
