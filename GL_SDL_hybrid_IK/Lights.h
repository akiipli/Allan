/*
In this file i attempt uniform block bindings
for fog and light.
Maybe these features grow more complicated in
the future and Light and fog get more members
in them. Right now i use light position to create
normalized reversed light vector.
*/

/*look OFFSETCOMMENT*/

int Themes = 12;

char Hint[STRLEN];

void empty_Hint()
{
    sprintf(Hint, " ");
}

void init_Hint()
{
    sprintf(Hint, "Light theme:%d Alt+U\nNight:%d Alt+N Shadows:%d Alt+S\nELEMENT ARRAYS: %d Alt+E",
            currentTheme, NIGHT, SHADOWS, ELEMENT_ARRAYS);
}

void init_Hint_Alert(const char * Alert)
{
    sprintf(Hint, Alert);
}

GLuint lightHandle, fogHandle, materialSHandle3;

typedef struct
{
    float Ka[3]; // Ambient reflectivity
    float Kd[3]; // Diffuse reflectivity
    float Ks[3]; // Specular reflectivity
    float Shininess; // Specular shininess
}
MaterialS;

MaterialS materialS;

typedef struct
{
    float Position[3];
    float Ambient[3];
}
Light;

Light light;

Light Light_Themes[12];

void init_MATERIALS()
{
    glGenBuffers(1, &materialSHandle3);

    float color[3] = {0.7, 0.5, 0.3};
    memcpy(&materialS.Ka, (float[3]){0.07, 0.05, 0.03}, sizeof(color));
    memcpy(&materialS.Kd, color, sizeof(color));
    memcpy(&materialS.Ks, (float[3]){0.9,0.9,0.9}, sizeof(color));
    materialS.Shininess = 1;
}

void init_LIGHT_THEME()
{
    glGenBuffers(1, &lightHandle);

    memcpy(&Light_Themes[0], (void*){(float[6]){10, 10, 10, 1.0, 0.0, 0.0}}, sizeof(Light));
    memcpy(&Light_Themes[1], (void*){(float[6]){-10, 10, -10, 0.1, 0.1, 1.0}}, sizeof(Light));
    memcpy(&Light_Themes[2], (void*){(float[6]){-10, -10, -10, 0.1, 0.1, 1.0}}, sizeof(Light));
    memcpy(&Light_Themes[3], (void*){(float[6]){10, -10, -10, 0.1, 1.0, 0.1}}, sizeof(Light));
    memcpy(&Light_Themes[4], (void*){(float[6]){-10, 10, 10, 1.0, 0.0, 1.0}}, sizeof(Light));
    memcpy(&Light_Themes[5], (void*){(float[6]){-10, -10, -10, 1.0, 0.1, 0.1}}, sizeof(Light));

    memcpy(&Light_Themes[6], (void*){(float[6]){-10, -10, 10, -1.0, 0.1, 0.1}}, sizeof(Light));
    memcpy(&Light_Themes[7], (void*){(float[6]){-10, 10, 10, -0.1, 1.1, -1.0}}, sizeof(Light));
    memcpy(&Light_Themes[8], (void*){(float[6]){-10, -10, 10, -0.1, -0.1, -1.0}}, sizeof(Light));
    memcpy(&Light_Themes[9], (void*){(float[6]){10, -10, -10, 1.0, -1.0, -0.1}}, sizeof(Light));
    memcpy(&Light_Themes[10], (void*){(float[6]){-10, 10, 10, 1.0, 0.0, -1.0}}, sizeof(Light));
    memcpy(&Light_Themes[11], (void*){(float[6]){10, 10, 10, -1.0, -0.1, -1.0}}, sizeof(Light));
}

typedef struct
{
    float Color[4];
    float Distance;
    float min_Distance;
}
Fog;

Fog fog;

void init_materialS()
{
    GLint blockSize;
    glGetActiveUniformBlockiv(T_program[4], uniform_mat4, GL_UNIFORM_BLOCK_DATA_SIZE, &blockSize);

    GLubyte * blockBuffer;
    blockBuffer = (GLubyte*)malloc(blockSize);

    GLint offset[4];
    offset[0] = 0;
    offset[1] = 4 * sizeof(GLfloat); /*OFFSETCOMMENT*/
    offset[2] = offset[1] + 4 * sizeof(GLfloat);
    offset[3] = offset[2] + 4 * sizeof(GLfloat);

    memcpy(blockBuffer + offset[0], materialS.Ka, 3 * sizeof(GLfloat));
    memcpy(blockBuffer + offset[1], materialS.Kd, 3 * sizeof(GLfloat));
    memcpy(blockBuffer + offset[2], materialS.Ks, 3 * sizeof(GLfloat));
    memcpy(blockBuffer + offset[3], &materialS.Shininess, sizeof(GLfloat));

    glBindBuffer(GL_UNIFORM_BUFFER, materialSHandle3);
    glBufferData(GL_UNIFORM_BUFFER, blockSize, blockBuffer, GL_DYNAMIC_DRAW);
    free(blockBuffer);
}

GLuint shadowFBO, pass1Index, pass2Index;

float shadowMapWidth = 512 * 2;
float shadowMapHeight = 512 * 2;

void init_shadows()
{
    pass1Index = glGetSubroutineIndex(T_program[4], GL_FRAGMENT_SHADER, "recordDepth");
    pass2Index = glGetSubroutineIndex(T_program[4], GL_FRAGMENT_SHADER, "shadeWithShadow");
}

void init_lights()
{
    GLint blockSize;
    glGetActiveUniformBlockiv(T_program[2], uniform_light2, GL_UNIFORM_BLOCK_DATA_SIZE, &blockSize);
//    CheckGLError();
    GLubyte * blockBuffer;
//    printf("%d\n", blockSize);
    blockBuffer = (GLubyte*)malloc(blockSize);

//    const GLchar * names[] = {"Position", "Ambient"};
//    GLuint indices[2];
//    glGetUniformIndices(T_program[2], 2, names, indices);
//
    GLint offset[2];
    offset[0] = 0;
    offset[1] = 4 * sizeof(GLfloat); /*OFFSETCOMMENT*/
//    glGetActiveUniformsiv(T_program[2], 2, indices, GL_UNIFORM_OFFSET, offset);

//
    memcpy(blockBuffer + offset[0], light.Position, 3 * sizeof(GLfloat));
    memcpy(blockBuffer + offset[1], light.Ambient, 3 * sizeof(GLfloat));

    glBindBuffer(GL_UNIFORM_BUFFER, lightHandle);
    glBufferData(GL_UNIFORM_BUFFER, blockSize, blockBuffer, GL_DYNAMIC_DRAW);
    free(blockBuffer);
}

float shadowBias[16];
float shadowMatrix[16];

void setMatrices()
{
    memcpy(shadowBias, (float[16]){
                        0.5f,0.0f,0.0f,0.0f,
                        0.0f,0.5f,0.0f,0.0f,
                        0.0f,0.0f,0.5f,0.0f,
                        0.5f,0.5f,0.5f,1.0f}, sizeof(shadowBias)
                        );

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();

    gluPerspective(45, 1, 1, 60);
    gluLookAt(light.Position[0], light.Position[1], light.Position[2], 0, 0, 0, 0, 1, 0);

    glGetFloatv(GL_PROJECTION_MATRIX, projectionMatrix);

    rotate_matrix_I_4x4((void *)shadowMatrix, (void *)shadowBias, (void *)projectionMatrix);
//    shadowMatrix = shadowBias * projectionMatrix;

}

void update_Light(Light L)
{
    //float len = sqrt(L.Position[0] * L.Position[0] + L.Position[1] * L.Position[1] + L.Position[2] * L.Position[2]);
    memcpy(light.Position, (float[3]){L.Position[0], L.Position[1], L.Position[2]}, sizeof(light.Position));
    memcpy(light.Ambient, (float[3]){L.Ambient[0], L.Ambient[1], L.Ambient[2]}, sizeof(light.Ambient));
    setMatrices();
}

void update_fog()
{
    GLint blockSize;
    glGetActiveUniformBlockiv(T_program[2], uniform_fog2, GL_UNIFORM_BLOCK_DATA_SIZE, &blockSize);
    GLubyte * blockBuffer;
    blockBuffer = (GLubyte*)malloc(blockSize);
//
    GLint offset[3];
    offset[0] = 0;
    offset[1] = 4 * sizeof(GLfloat); /*OFFSETCOMMENT*/
    offset[2] = offset[1] + sizeof(GLfloat);
//    glGetActiveUniformsiv(T_program[2], 2, indices, GL_UNIFORM_OFFSET, offset);

//
    memcpy(blockBuffer + offset[0], fog.Color, 4 * sizeof(GLfloat));
    memcpy(blockBuffer + offset[1], &fog.Distance, sizeof(GLfloat));
    memcpy(blockBuffer + offset[2], &fog.min_Distance, sizeof(GLfloat));

    glBindBuffer(GL_UNIFORM_BUFFER, fogHandle);
    glBufferData(GL_UNIFORM_BUFFER, blockSize, blockBuffer, GL_DYNAMIC_DRAW);

    free(blockBuffer);
}

void init_fog()
{
    GLfloat color[4];
    glGetFloatv(GL_COLOR_CLEAR_VALUE, color);

    memcpy(fog.Color, color, sizeof(fog.Color));
    fog.Distance = 100;
    fog.min_Distance = 10;

    GLint blockSize;
    glGetActiveUniformBlockiv(T_program[2], uniform_fog2, GL_UNIFORM_BLOCK_DATA_SIZE, &blockSize);
    CheckGLError();
    GLubyte * blockBuffer;
    printf("%d\n", blockSize);
    blockBuffer = (GLubyte*)malloc(blockSize);

    const GLchar * names[] = {"Color", "Distance"};
    GLuint indices[2];
    glGetUniformIndices(T_program[2], 2, names, indices);
//
    GLint offset[3];
    offset[0] = 0;
    offset[1] = 4 * sizeof(GLfloat); /*OFFSETCOMMENT*/
    offset[2] = offset[1] + sizeof(GLfloat);
//    glGetActiveUniformsiv(T_program[2], 2, indices, GL_UNIFORM_OFFSET, offset);

//
    memcpy(blockBuffer + offset[0], fog.Color, 4 * sizeof(GLfloat));
    memcpy(blockBuffer + offset[1], &fog.Distance, sizeof(GLfloat));
    memcpy(blockBuffer + offset[2], &fog.min_Distance, sizeof(GLfloat));

    glGenBuffers(1, &fogHandle);
    glBindBuffer(GL_UNIFORM_BUFFER, fogHandle);
    glBufferData(GL_UNIFORM_BUFFER, blockSize, blockBuffer, GL_DYNAMIC_DRAW);
}

void render_polys_OnScreen_Shadows(camera * C, int currentObject, int mode, int Selection_Mode, int update_colors, int update_uv, int elem);
void render_quads_OnScreen_Shadows(camera * C, int l, int currentObject, int mode, int Selection_Mode, int update_colors, int update_uv, int elem);
void render_polys_quads_OnScreen_Shadows(camera * C, int l, int currentObject, int mode, int Selection_Mode, int update_colors, int update_uv, int elem);

void SceneShadowMap_render(camera * C, int width, int height, int subdLevel, int elem)
{
    glUseProgram(T_program[4]);
    // Pass 1 (shadow map generation)

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();

    gluPerspective(45, 1, 1, 60);
    gluLookAt(light.Position[0], light.Position[1], light.Position[2], 0, 0, 0, 0, 1, 0);


    glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);

    GLint params[4];
    glGetIntegerv(GL_SCISSOR_BOX, params);
    glScissor(0, 0, shadowMapWidth,shadowMapHeight);

    glClear(GL_DEPTH_BUFFER_BIT);
    glViewport(0,0,shadowMapWidth,shadowMapHeight);
    glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 1, &pass1Index);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);
    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(2.5f,10.0f);

    glGetFloatv(GL_PROJECTION_MATRIX, projectionMatrix);

    //glEnable(GL_ALPHA_TEST);
    //glAlphaFunc(GL_GREATER, 0);

    //glEnable(GL_STENCIL_TEST);
    //glStencilFunc(GL_GEQUAL, 1, 0xFF);
    //glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
    //glStencilMask(0x00);
    //glDepthMask(GL_TRUE);

    render_polys_quads_OnScreen_Shadows(C, subdLevel, 0, 0, 0, 0, 0, elem);
/*
    if (subdLevel == -1)
        render_polys_OnScreen_Shadows(C, 0, 0, 0, 0, 0, elem);
    else
        render_quads_OnScreen_Shadows(C, subdLevel, 0, 0, 0, 0, 0, elem);
*/
    //glDisable(GL_STENCIL_TEST);
    //glDisable(GL_ALPHA_TEST);

    glCullFace(GL_BACK);
    //glFlush();

    glPopMatrix();

    // Pass 2 (render)

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glScissor(params[0], params[1], params[2], params[3]);

    glUseProgram(0);
}

GLuint depthFBO;
GLuint depthTex, drag_texture;

#define D_plane 3000
float drag_depth_Width;
float drag_depth_Height;
//float Drag_Plane[4][3] = {{-D_plane, D_plane, 1}, {-D_plane, -D_plane, 1}, {D_plane, -D_plane, 1}, {D_plane, D_plane, 1}};
GLfloat winX, winY, winZ;

struct
{
    float A[3], A_[3];
    float B[3], B_[3];
    float C[3], C_[3];
    float D[3], D_[3];
}
Drag_Plane;

void init_Drag_Plane(float width, float height)
{
    memcpy(Drag_Plane.A_, (float[3]){-D_plane, D_plane, 1}, sizeof(float[3]));
    memcpy(Drag_Plane.B_, (float[3]){-D_plane, -D_plane, 1}, sizeof(float[3]));
    memcpy(Drag_Plane.C_, (float[3]){D_plane, -D_plane, 1}, sizeof(float[3]));
    memcpy(Drag_Plane.D_, (float[3]){D_plane, D_plane, 1}, sizeof(float[3]));

    drag_depth_Width = width;
    drag_depth_Height = height;
}

void render_Depth_Drag_Plane();

void rotate_Drag_Plane(camera * C)
{
    direction D1;
    rotate_Vertex_I(C->T->rotVec_, Drag_Plane.A_[0], Drag_Plane.A_[1], Drag_Plane.A_[2], &D1);
    memcpy(Drag_Plane.A, &D1, sizeof(float[3]));
    rotate_Vertex_I(C->T->rotVec_, Drag_Plane.B_[0], Drag_Plane.B_[1], Drag_Plane.B_[2], &D1);
    memcpy(Drag_Plane.B, &D1, sizeof(float[3]));
    rotate_Vertex_I(C->T->rotVec_, Drag_Plane.C_[0], Drag_Plane.C_[1], Drag_Plane.C_[2], &D1);
    memcpy(Drag_Plane.C, &D1, sizeof(float[3]));
    rotate_Vertex_I(C->T->rotVec_, Drag_Plane.D_[0], Drag_Plane.D_[1], Drag_Plane.D_[2], &D1);
    memcpy(Drag_Plane.D, &D1, sizeof(float[3]));
}

void translate_Drag_Plane(camera * C)
{
    Drag_Plane.A[0] += C->T->pos[0];
    Drag_Plane.A[1] += C->T->pos[1];
    Drag_Plane.A[2] += C->T->pos[2];
    Drag_Plane.B[0] += C->T->pos[0];
    Drag_Plane.B[1] += C->T->pos[1];
    Drag_Plane.B[2] += C->T->pos[2];
    Drag_Plane.C[0] += C->T->pos[0];
    Drag_Plane.C[1] += C->T->pos[1];
    Drag_Plane.C[2] += C->T->pos[2];
    Drag_Plane.D[0] += C->T->pos[0];
    Drag_Plane.D[1] += C->T->pos[1];
    Drag_Plane.D[2] += C->T->pos[2];
}

void Drag_Plane_render_action()
{
    glClear(GL_DEPTH_BUFFER_BIT);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);
    glGetFloatv(GL_PROJECTION_MATRIX, projectionMatrix);

    render_Depth_Drag_Plane();

    glCullFace(GL_BACK);
}

void Drag_Plane_transforms(float ObjDist, camera * C)
{
    Drag_Plane.A_[2] = ObjDist;
    Drag_Plane.B_[2] = ObjDist;
    Drag_Plane.C_[2] = ObjDist;
    Drag_Plane.D_[2] = ObjDist;

    rotate_Drag_Plane(C);
    translate_Drag_Plane(C);
}
/*
void Drag_Plane_render(float ObjDist, camera * C)
{
    Drag_Plane.A_[2] = ObjDist;
    Drag_Plane.B_[2] = ObjDist;
    Drag_Plane.C_[2] = ObjDist;
    Drag_Plane.D_[2] = ObjDist;

    rotate_Drag_Plane(C);
    translate_Drag_Plane(C);

    GLint params[4];
    glGetIntegerv(GL_SCISSOR_BOX, params);
    glScissor(SIDEBAR, BOTTOM_LINE, drag_depth_Width, drag_depth_Height);

    glClear(GL_DEPTH_BUFFER_BIT);
    glViewport(SIDEBAR, BOTTOM_LINE, drag_depth_Width, drag_depth_Height);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);
    glGetFloatv(GL_PROJECTION_MATRIX, projectionMatrix);

    render_Depth_Drag_Plane();

    glCullFace(GL_BACK);
    glScissor(params[0], params[1], params[2], params[3]);
}
*/

GLfloat * Windepth = NULL;

int resize_Depth_Buffer(float width, float height)
{
    int r = 0;

    drag_depth_Width = width;
    drag_depth_Height = height;

    Windepth = realloc(Windepth, drag_depth_Width * drag_depth_Height * sizeof(GLfloat));

    if (!Windepth)
        return 0;

    glDeleteTextures(1, &drag_texture);
    glGenTextures(1, &drag_texture);
    glBindTexture(GL_TEXTURE_2D, drag_texture);
    glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH_COMPONENT24, drag_depth_Width, drag_depth_Height);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LESS);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, drag_texture);

    glBindFramebuffer(GL_FRAMEBUFFER, depthFBO);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                           GL_TEXTURE_2D, drag_texture, 0);

    GLenum drawBuffers[] = {GL_NONE};
    glDrawBuffers(1, drawBuffers);

    GLenum result = glCheckFramebufferStatus(GL_FRAMEBUFFER);

    if( result == GL_FRAMEBUFFER_COMPLETE)
    {
        printf("Depth Framebuffer is complete.\n");
        r = 1;
    }
    else
    {
        printf("Depth Framebuffer is not complete.\n");
        r = 0;
    }

    glBindFramebuffer(GL_FRAMEBUFFER,0);

    return r;
}

int setup_Depth_Buffer()
{
    int r = 0;

    Windepth = malloc(drag_depth_Width * drag_depth_Height * sizeof(GLfloat));

    if (!Windepth)
        return 0;

    glGenTextures(1, &drag_texture);
    glBindTexture(GL_TEXTURE_2D, drag_texture);
    //glTexImage2D(GL_TEXTURE_2D, 1, GL_DEPTH_COMPONENT24, drag_depth_Width, drag_depth_Height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH_COMPONENT24, drag_depth_Width, drag_depth_Height);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LESS);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, drag_texture);

    glGenFramebuffers(1, &depthFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, depthFBO);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                           GL_TEXTURE_2D, drag_texture, 0);

    GLenum drawBuffers[] = {GL_NONE};
    glDrawBuffers(1, drawBuffers);

    GLenum result = glCheckFramebufferStatus(GL_FRAMEBUFFER);

    if( result == GL_FRAMEBUFFER_COMPLETE)
    {
        printf("Depth Framebuffer is complete.\n");
        r = 1;
    }
    else
    {
        printf("Depth Framebuffer is not complete.\n");
        r = 0;
    }

    glBindFramebuffer(GL_FRAMEBUFFER,0);

    return r;
}

int setupFBO()
{
    int r = 0;
    GLfloat border[] = {1.0f, 0.0f,0.0f,0.0f };
    // The depth buffer texture
    glGenTextures(1, &depthTex);
    glBindTexture(GL_TEXTURE_2D, depthTex);
    glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH_COMPONENT24, shadowMapWidth, shadowMapHeight);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, border);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LESS);

    // Assign the depth buffer texture to texture channel 0
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, depthTex);

    // Create and set up the FBO
    glGenFramebuffers(1, &shadowFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                           GL_TEXTURE_2D, depthTex, 0);

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

void render_polys_OnScreen(camera * C, int wireframe, int edgedraw, int vertdraw, int currentObject, int rendermode, int Selection_Mode, int update_colors, int update_uv, int elem);
void render_polys_quads_OnScreen(camera * C, int wireframe, int edgedraw, int vertdraw, int l, int currentObject, int mode, int Selection_Mode, int update_colors, int update_uv, int elem);
void load_m_colors_object(object * O);

void Materials_Thumbnail_render(camera * C, int width, int height, int subdLevel, int elem)
{
/*
    if (SHADERS && SHADOWS)
    {
        if (Fan_Arrays_Shader || subdLevel > -1)
        {
            SceneShadowMap_render(C, width, height, subdLevel, ELEMENT_ARRAYS);
        }
    }
*/

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();

    gluPerspective(C->v_view * (180.0 / pi), 1.0, persp_Near, persp_Far);

    gluLookAt(0.0, 0.0, 1.5, 0.0, 0.0, 0.0, C->T->rotVec_[1][0], C->T->rotVec_[1][1], C->T->rotVec_[1][2]);

    glGetFloatv(GL_PROJECTION_MATRIX, projectionMatrix);

    glBindFramebuffer(GL_FRAMEBUFFER, materialTexFBO);
    glDrawBuffer(GL_COLOR_ATTACHMENT1);

    glScissor(0, 0, width, height);
    glViewport(0, 0, width, height);
    glClearColor(0.8, 0.5, 0.3, 0.0);

    int s;

    object * O = objects[C->objects[0]];

    //glActiveTexture(GL_TEXTURE0);

    GLuint Texture;

    int r;

    //for (s = 0; s < Materials_count; s ++)

    for (s = Materials_count - 1; s >= 0; s --)
    {
        Texture = Material_Textures[s];

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1,
                               GL_TEXTURE_2D, Texture, 0);

        GLenum result = glCheckFramebufferStatus(GL_FRAMEBUFFER);

        if( result == GL_FRAMEBUFFER_COMPLETE)
        {
            //printf("Framebuffer is complete.\n");
            r = 1;
        }
        else
        {
            //printf("Framebuffer is not complete.\n");
            r = 0;
        }

        if (r)
        {
            O->surface = s;
            load_m_colors_object(O);

            render_polys_quads_OnScreen(C, subdLevel, 0, 0, 1, 0, POLY_RENDER, 0, 1, 0, elem);

            SDL_Surface * surface = Material_Thumbnail_Surfaces[s];

            glBindTexture(GL_TEXTURE_2D, Material_Textures[s]);

            glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8_REV, surface->pixels);
//            SDL_LockSurface(surface);
//            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface->w, surface->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, surface->pixels);
//            SDL_UnlockSurface(surface);
        }
    }

    O->surface = 0;

    glPopMatrix();

    // Pass 2 (render)

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
