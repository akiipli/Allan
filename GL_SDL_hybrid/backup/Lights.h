GLuint lightHandle, fogHandle;

typedef struct
{
    float Position[3];
    float Ambient[3];
}
Light;

Light light;

typedef struct
{
    float Color[4];
    float Distance;
    float min_Distance;
}
Fog;

Fog fog;

const char * GetGLErrorStr(GLenum err)
{
    switch (err)
    {
    case GL_NO_ERROR:          return "No error";
    case GL_INVALID_ENUM:      return "Invalid enum";
    case GL_INVALID_VALUE:     return "Invalid value";
    case GL_INVALID_OPERATION: return "Invalid operation";
    case GL_STACK_OVERFLOW:    return "Stack overflow";
    case GL_STACK_UNDERFLOW:   return "Stack underflow";
    case GL_OUT_OF_MEMORY:     return "Out of memory";
    default:                   return "Unknown error";
    }
}

void CheckGLError()
{
    while (1)
    {
        const GLenum err = glGetError();
        if (GL_NO_ERROR == err)
            break;

        printf("%s\n", GetGLErrorStr(err));
    }
}

void init_lights()
{
    memcpy(light.Position, (float[3]){10, 10, 10}, sizeof(light.Position));
    memcpy(light.Ambient, (float[3]){0.0, 0.1, 0.1}, sizeof(light.Ambient));

    GLint blockSize;
    glGetActiveUniformBlockiv(T_program1, uniform_light1, GL_UNIFORM_BLOCK_DATA_SIZE, &blockSize);
    CheckGLError();
    GLubyte * blockBuffer;
    printf("%d\n", blockSize);
    blockBuffer = (GLubyte*)malloc(blockSize);

    const GLchar * names[] = {"Position", "Ambient"};
    GLuint indices[2];
    glGetUniformIndices(T_program1, 2, names, indices);
//
    GLint offset[2];
    offset[0] = 0;
    offset[1] = 4 * sizeof(GLfloat);
//    glGetActiveUniformsiv(T_program1, 2, indices, GL_UNIFORM_OFFSET, offset);

//
    memcpy(blockBuffer + offset[0], light.Position, 3 * sizeof(GLfloat));
    memcpy(blockBuffer + offset[1], light.Ambient, 3 * sizeof(GLfloat));

    glGenBuffers(1, &lightHandle);
    glBindBuffer(GL_UNIFORM_BUFFER, lightHandle);
    glBufferData(GL_UNIFORM_BUFFER, blockSize, blockBuffer, GL_DYNAMIC_DRAW);
}

void init_fog()
{
    GLfloat color[4];
    glGetFloatv(GL_COLOR_CLEAR_VALUE, color);

    memcpy(fog.Color, color, sizeof(fog.Color));
    fog.Distance = 100;
    fog.min_Distance = 10;

    GLint blockSize;
    glGetActiveUniformBlockiv(T_program1, uniform_fog1, GL_UNIFORM_BLOCK_DATA_SIZE, &blockSize);
    CheckGLError();
    GLubyte * blockBuffer;
    printf("%d\n", blockSize);
    blockBuffer = (GLubyte*)malloc(blockSize);

    const GLchar * names[] = {"Color", "Distance"};
    GLuint indices[2];
    glGetUniformIndices(T_program1, 2, names, indices);
//
    GLint offset[3];
    offset[0] = 0;
    offset[1] = 4 * sizeof(GLfloat);
    offset[2] = offset[1] + sizeof(GLfloat);
//    glGetActiveUniformsiv(T_program1, 2, indices, GL_UNIFORM_OFFSET, offset);

//
    memcpy(blockBuffer + offset[0], fog.Color, 4 * sizeof(GLfloat));
    memcpy(blockBuffer + offset[1], &fog.Distance, sizeof(GLfloat));
    memcpy(blockBuffer + offset[2], &fog.min_Distance, sizeof(GLfloat));

    glGenBuffers(1, &fogHandle);
    glBindBuffer(GL_UNIFORM_BUFFER, fogHandle);
    glBufferData(GL_UNIFORM_BUFFER, blockSize, blockBuffer, GL_DYNAMIC_DRAW);
}
