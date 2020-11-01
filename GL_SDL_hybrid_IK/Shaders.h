#define SHADERS_NUM 5

GLuint T_program[SHADERS_NUM];

GLint attribute_coord, uniform_refls2, uniform_refls4, uniform_smap4, uniform_mat4, uniform_shine4, uniform_displ4;
GLint uniform_tex0, uniform_tex1, uniform_tex2, uniform_night2, uniform_night4, uniform_tex4, uniform_sha4;
GLint uniform_color0, uniform_cam2, uniform_cam4, uniform_light2, uniform_light4, uniform_fog2;
GLint uniform_proj0, uniform_proj1, uniform_proj2, uniform_proj3, uniform_proj4, uniform_nmap4, uniform_bmap4;

GLuint vs[SHADERS_NUM];
GLuint fs[SHADERS_NUM];

GLuint vbo_tex, vbo_pos;
GLfloat projectionMatrix[16];
float pM[4][4];

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

void print_log(GLuint object)
{
	GLint log_length = 0;
	if (glIsShader(object))
		glGetShaderiv(object, GL_INFO_LOG_LENGTH, &log_length);
	else if (glIsProgram(object))
		glGetProgramiv(object, GL_INFO_LOG_LENGTH, &log_length);
	else {
		fprintf(stderr, "printlog: Not a shader or a program\n");
		return;
	}

	char* log = (char*)malloc(log_length);

	if (glIsShader(object))
		glGetShaderInfoLog(object, log_length, NULL, log);
	else if (glIsProgram(object))
		glGetProgramInfoLog(object, log_length, NULL, log);

	fprintf(stderr, "%s", log);
	free(log);
}

char* file_read(const char* filename)
{
	FILE* in = fopen(filename, "rb");
	if (in == NULL) return NULL;

	int res_size = BUFSIZ;
	char* res = (char*)malloc(res_size);
	int nb_read_total = 0;

	while (!feof(in) && !ferror(in)) {
		if (nb_read_total + BUFSIZ > res_size) {
			if (res_size > 10*1024*1024) break;
			res_size = res_size * 2;
			res = (char*)realloc(res, res_size);
		}
		char* p_res = res + nb_read_total;
		nb_read_total += fread(p_res, 1, BUFSIZ, in);
	}

	fclose(in);
	res = (char*)realloc(res, nb_read_total + 1);
	res[nb_read_total] = '\0';
	return res;
}

GLint create_shader(const char* filename, GLenum type)
{
	const GLchar* source = file_read(filename);
	if (source == NULL) {
		printf("Error opening %s: ", filename); perror("");
		return 0;
	}
	GLuint res = glCreateShader(type);
	glShaderSource(res, 1, &source, NULL);
	free((void*)source);

	glCompileShader(res);
	GLint compile_ok = GL_FALSE;
	glGetShaderiv(res, GL_COMPILE_STATUS, &compile_ok);
	if (compile_ok == GL_FALSE) {
		printf("%s:", filename);
		print_log(res);
		glDeleteShader(res);
		return 0;
	}

	return res;
}

//GLuint font_tex;

void delete_Shaders()
{
    int i;
    for (i = 0; i < SHADERS_NUM; i ++)
    {
        glDeleteProgram(T_program[i]);
        glDeleteShader(vs[i]);
        glDeleteShader(fs[i]);
    }
}

GLfloat pos_coords[8] = {0, 1, 1, 1, 1, 0, 0, 0};
GLfloat tex_coords[8] = {0, 1, 1, 1, 1, 0, 0, 0};

int init_shaders_()
{
//	glGenTextures(1, &font_tex);
//	/* Initialize the FreeType2 library */
//	if (FT_Init_FreeType(&ft)) {
//		fprintf(stderr, "Could not init freetype library\n");
//		return 0;
//	}
//
    char dirfile[STRLEN];
//    dirfile[0] = '\0';
//    strcat(dirfile, resources_dir);
//    strcat(dirfile, "courbd.ttf");
//	if (FT_New_Face(ft, dirfile, 0, &face)) {
//		fprintf(stderr, "Could not open font\n");
//		return 0;
//	}

	/* Compile and link the shader program */
	GLint link_ok = GL_FALSE;

    dirfile[0] = '\0';
    strcat(dirfile, resources_dir);
    strcat(dirfile, "Shaders/glsl.v0.text");
	if ((vs[0] = create_shader(dirfile, GL_VERTEX_SHADER))	== 0) return 0;
    dirfile[0] = '\0';
    strcat(dirfile, resources_dir);
    strcat(dirfile, "Shaders/glsl.f0.text");
	if ((fs[0] = create_shader(dirfile, GL_FRAGMENT_SHADER)) == 0) return 0;

    dirfile[0] = '\0';
    strcat(dirfile, resources_dir);
    strcat(dirfile, "Shaders/glsl.v1.text");
	if ((vs[1] = create_shader(dirfile, GL_VERTEX_SHADER)) == 0) return 0;
    dirfile[0] = '\0';
    strcat(dirfile, resources_dir);
    strcat(dirfile, "Shaders/glsl.f1.text");
	if ((fs[1] = create_shader(dirfile, GL_FRAGMENT_SHADER)) == 0) return 0;

    dirfile[0] = '\0';
    strcat(dirfile, resources_dir);
    strcat(dirfile, "Shaders/glsl.v2.text");
	if ((vs[2] = create_shader(dirfile, GL_VERTEX_SHADER)) == 0) return 0;
    dirfile[0] = '\0';
    strcat(dirfile, resources_dir);
    strcat(dirfile, "Shaders/glsl.f2.text");
	if ((fs[2] = create_shader(dirfile, GL_FRAGMENT_SHADER)) == 0) return 0;

    dirfile[0] = '\0';
    strcat(dirfile, resources_dir);
    strcat(dirfile, "Shaders/glsl.v3.text");
	if ((vs[3] = create_shader(dirfile, GL_VERTEX_SHADER)) == 0) return 0;
    dirfile[0] = '\0';
    strcat(dirfile, resources_dir);
    strcat(dirfile, "Shaders/glsl.f3.text");
	if ((fs[3] = create_shader(dirfile, GL_FRAGMENT_SHADER)) == 0) return 0;

    dirfile[0] = '\0';
    strcat(dirfile, resources_dir);
    strcat(dirfile, "Shaders/glsl.v4.text");
	if ((vs[4] = create_shader(dirfile, GL_VERTEX_SHADER)) == 0) return 0;
    dirfile[0] = '\0';
    strcat(dirfile, resources_dir);
    strcat(dirfile, "Shaders/glsl.f4.text");
	if ((fs[4] = create_shader(dirfile, GL_FRAGMENT_SHADER)) == 0) return 0;

	T_program[0] = glCreateProgram();
	glAttachShader(T_program[0], vs[0]);
	glAttachShader(T_program[0], fs[0]);
	glLinkProgram(T_program[0]);
	glGetProgramiv(T_program[0], GL_LINK_STATUS, &link_ok);
	if (!link_ok) {
		printf("glLinkProgram:");
		print_log(T_program[0]);
		return 0;
	}

	attribute_coord = glGetAttribLocation(T_program[0], "coord");
	if (attribute_coord == -1) {
		printf("Could not bind coord attribute \n");
		return 0;
	}

	uniform_tex0 = glGetUniformLocation(T_program[0], "tex");
	if (uniform_tex0 == -1) {
		printf("Could not bind tex uniform \n");
		return 0;
	}

	uniform_color0 = glGetUniformLocation(T_program[0], "color");
	if (uniform_color0 == -1) {
		printf("Could not bind color uniform \n");
		return 0;
	}

	uniform_proj0 = glGetUniformLocation(T_program[0], "ProjectionMatrix");
	if (uniform_proj0 == -1) {
		printf("Could not bind ProjectionMatrix uniform \n");
		return 0;
	}

	//--

	T_program[1] = glCreateProgram();
	glAttachShader(T_program[1], vs[1]);
	glAttachShader(T_program[1], fs[1]);
	glLinkProgram(T_program[1]);
	glGetProgramiv(T_program[1], GL_LINK_STATUS, &link_ok);
	if (!link_ok) {
		printf("glLinkProgram:");
		print_log(T_program[1]);
		return 0;
	}

	uniform_tex1 = glGetUniformLocation(T_program[1], "tex");
	if (uniform_tex1 == -1) {
		printf("Could not bind tex1 uniform \n");
		return 0;
	}

	uniform_proj1 = glGetUniformLocation(T_program[1], "ProjectionMatrix");
	if (uniform_proj1 == -1) {
		printf("Could not bind ProjectionMatrix1 uniform \n");
		return 0;
	}

	//--

	T_program[2] = glCreateProgram();
	glAttachShader(T_program[2], vs[2]);
	glAttachShader(T_program[2], fs[2]);
	glLinkProgram(T_program[2]);
	glGetProgramiv(T_program[2], GL_LINK_STATUS, &link_ok);
	if (!link_ok) {
		printf("glLinkProgram:");
		print_log(T_program[2]);
		return 0;
	}

	uniform_tex2 = glGetUniformLocation(T_program[2], "tex");
	if (uniform_tex2 == -1) {
		printf("Could not bind tex2 uniform \n");
		return 0;
	}

	uniform_proj2 = glGetUniformLocation(T_program[2], "ProjectionMatrix");
	if (uniform_proj2 == -1) {
		printf("Could not bind ProjectionMatrix2 uniform \n");
		return 0;
	}

	uniform_cam2 = glGetUniformLocation(T_program[2], "CameraMatrix");
	if (uniform_cam2 == -1) {
		printf("Could not bind CameraMatrix2 uniform \n");
		return 0;
	}

	uniform_night2 = glGetUniformLocation(T_program[2], "Night");
	if (uniform_night2 == -1) {
		printf("Could not bind night2 uniform \n");
		return 0;
	}

	uniform_refls2 = glGetUniformLocation(T_program[2], "Reflects");
	if (uniform_refls2 == -1) {
		printf("Could not bind refls2 uniform \n");
		return 0;
	}

	uniform_light2 = glGetUniformBlockIndex(T_program[2], "Light");
	if (uniform_light2 == -1) {
		printf("Could not bind light2 uniform \n");
		return 0;
	}

	uniform_fog2 = glGetUniformBlockIndex(T_program[2], "Fog");
	if (uniform_fog2 == -1) {
		printf("Could not bind fog1 uniform \n");
		return 0;
	}

	//--

	T_program[3] = glCreateProgram();
	glAttachShader(T_program[3], vs[3]);
	glAttachShader(T_program[3], fs[3]);
	glLinkProgram(T_program[3]);
	glGetProgramiv(T_program[3], GL_LINK_STATUS, &link_ok);
	if (!link_ok) {
		printf("glLinkProgram:");
		print_log(T_program[3]);
		return 0;
	}

	uniform_proj3 = glGetUniformLocation(T_program[3], "ProjectionMatrix");
	if (uniform_proj3 == -1) {
		printf("Could not bind ProjectionMatrix3 uniform \n");
		return 0;
	}

	//--

	T_program[4] = glCreateProgram();
	glAttachShader(T_program[4], vs[4]);
	glAttachShader(T_program[4], fs[4]);
	glLinkProgram(T_program[4]);
	glGetProgramiv(T_program[4], GL_LINK_STATUS, &link_ok);
	if (!link_ok) {
		printf("glLinkProgram:");
		print_log(T_program[4]);
		return 0;
	}

	uniform_tex4 = glGetUniformLocation(T_program[4], "tex");
	if (uniform_tex4 == -1) {
		printf("Could not bind tex4 uniform \n");
		return 0;
	}

	uniform_smap4 = glGetUniformLocation(T_program[4], "ShadowMap");
	if (uniform_smap4 == -1) {
		printf("Could not bind smap4 uniform \n");
		return 0;
	}

	uniform_nmap4 = glGetUniformLocation(T_program[4], "normalMap");
	if (uniform_nmap4 == -1) {
		printf("Could not bind nmap4 uniform \n");
		return 0;
	}

	uniform_bmap4 = glGetUniformLocation(T_program[4], "bumpMap");
	if (uniform_bmap4 == -1) {
		printf("Could not bind bmap4 uniform \n");
		return 0;
	}
/*
	uniform_mat4 = glGetUniformBlockIndex(T_program[4], "Material");
	if (uniform_mat4 == -1) {
		printf("Could not bind mat3 uniform \n");
		return 0;
	}
*/
	uniform_proj4 = glGetUniformLocation(T_program[4], "ProjectionMatrix");
	if (uniform_proj4 == -1) {
		printf("Could not bind ProjectionMatrix3 uniform \n");
		return 0;
	}

	uniform_cam4 = glGetUniformLocation(T_program[4], "CameraMatrix");
	if (uniform_cam4 == -1) {
		printf("Could not bind CameraMatrix4 uniform \n");
		return 0;
	}

	uniform_sha4 = glGetUniformLocation(T_program[4], "ShadowMatrix");
	if (uniform_sha4 == -1) {
		printf("Could not bind ShadowMatrix4 uniform \n");
		return 0;
	}

	uniform_night4 = glGetUniformLocation(T_program[4], "Night");
	if (uniform_night4 == -1) {
		printf("Could not bind night4 uniform \n");
		return 0;
	}

	uniform_refls4 = glGetUniformLocation(T_program[4], "Reflects");
	if (uniform_refls4 == -1) {
		printf("Could not bind refls4 uniform \n");
		return 0;
	}

	uniform_shine4 = glGetUniformLocation(T_program[4], "Shine");
	if (uniform_shine4 == -1) {
		printf("Could not bind shine4 uniform \n");
		return 0;
	}

	uniform_displ4 = glGetUniformLocation(T_program[4], "Displace");
	if (uniform_displ4 == -1) {
		printf("Could not bind disp4 uniform \n");
		return 0;
	}

	// Create the vertex buffer object
	glGenBuffers(1, &vbo_tex);
	glGenBuffers(1, &vbo_pos);

    glBindBuffer(GL_ARRAY_BUFFER, vbo_tex);
    glBufferData(GL_ARRAY_BUFFER, sizeof tex_coords, tex_coords, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, vbo_pos);
    glBufferData(GL_ARRAY_BUFFER, sizeof pos_coords, pos_coords, GL_DYNAMIC_DRAW);

	return 1;
}

char Text[STRLEN];

void make_character_map(const char * text, float x, float y, int italic)
{
    Text[0] = '\0';
    strcat(Text, text);
    strcat(Text, ".");

	const char * p;
	FT_GlyphSlot G = face[italic]->glyph;

    Pos_coords_c = 0;

    FT_Load_Char(face[italic], 'j', FT_LOAD_RENDER);
    int bitmap_top = G->bitmap_top;
    int bitmap_rows = G->bitmap.rows;

	for(p = Text; * p; p ++)
    {
		/* Try to load and render the character */
		if (FT_Load_Char(face[italic], * p, FT_LOAD_RENDER))
        {
            continue;
        }

		/* Calculate the vertex and texture coordinates */
		float x0 = x + G->bitmap_left;
		float w = G->advance.x >> 6; //G->bitmap.width;

		float lower_part = bitmap_rows - bitmap_top;

		memcpy(Pos_coords[Pos_coords_c],(GLfloat[8])
		{x0,     y + lower_part,
         x0 + w, y + lower_part,
         x0 + w, y - bitmap_top,
         x0,     y - bitmap_top}, sizeof(GLfloat[8]));

		/* Advance the cursor to the start of the next character */
		x += G->advance.x >> 6;
        y += G->advance.y >> 6;
        Pos_coords_c ++;
	}
}

void render_text_(const char * text, float x, float y, int font_height, int italic)
{
	const char * p;
	FT_GlyphSlot G = face[italic]->glyph;

	/* Create a texture that will be used to hold one "glyph" */
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, font_tex);
	glUniform1i(uniform_tex1, 0);

	/* We require 1 byte alignment when uploading texture data */
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	/* Clamping to edges is important to prevent artifacts when scaling */
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	/* Linear filtering usually looks best for text */
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	/* Set up the VBO for our vertex data */
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, vbo_tex);
	//glBufferSubData(GL_ARRAY_BUFFER, 0, 8 * sizeof(GLfloat), tex_coords);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);

    glBindBuffer(GL_ARRAY_BUFFER, vbo_pos);
    //glBufferSubData(GL_ARRAY_BUFFER, 0, 8 * sizeof(GLfloat), pos_coords);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);

	/* Loop through all characters */

	float X = x;
	int c = 0;
	int tab = 10;
	int tab_c = 0;
	for(p = text; * p; p ++)
    {
		/* Try to load and render the character */
		if (FT_Load_Char(face[italic], * p, FT_LOAD_RENDER))
        {
            continue;
        }

		if (* p == '\t')
        {
            tab_c = tab - (c % tab);
            x += (G->advance.x >> 6) * tab_c;
            c += tab_c;
            continue;
        }

		if (* p == '\n')
        {
            y += font_height;
            x = X;
            c = 0;
            continue;
        }

		/* Upload the "bitmap", which contains an 8-bit grayscale image, as an alpha texture */
		glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, G->bitmap.width, G->bitmap.rows, 0, GL_ALPHA, GL_UNSIGNED_BYTE, G->bitmap.buffer);

		/* Calculate the vertex and texture coordinates */
		float x0 = x + G->bitmap_left;
		float w = G->bitmap.width;

		float lower_part = G->bitmap.rows - G->bitmap_top;

		memcpy(pos_coords,(GLfloat[8])
		{x0,     y + lower_part,
         x0 + w, y + lower_part,
         x0 + w, y - G->bitmap_top,
         x0,     y - G->bitmap_top}, sizeof pos_coords);

		/* Draw the character on the screen */
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(GLfloat[8]), pos_coords);
		glDrawArrays(GL_QUADS, 0, 4);

		/* Advance the cursor to the start of the next character */
		x += G->advance.x >> 6;
        y += G->advance.y >> 6;
        c ++;
	}

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
}

GLfloat White[4] = {1, 1, 1, 1};

void display_font_(const char * text, float origin_x, float origin_y, int font_height, float color[4], int italic)
{
    glMatrixMode(GL_PROJECTION);

    glGetFloatv(GL_PROJECTION_MATRIX, projectionMatrix);

	glUseProgram(T_program[1]);

	glUniformMatrix4fv(uniform_proj1, 1, GL_FALSE, projectionMatrix);

	glUniform4fv(uniform_color0, 1, color);

	render_text_(text, origin_x, origin_y, font_height, italic);

	glUseProgram(0);
}

void display_font(const char * text, int width, int height, int italic)
{
	//glDisable(GL_LIGHTING);
	//glDisable(GL_DEPTH_TEST);

	float dim_x = 300;
	float dim_y = 200;
    glScissor(SIDEBAR * 2, height - dim_y + BOTTOM_LINE, dim_x, dim_y);
//    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glViewport(SIDEBAR * 2, height - dim_y + BOTTOM_LINE, dim_x, dim_y);
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();

    glOrtho(0, dim_x, dim_y, 0, 1, -1);

//    gluLookAt(0, 0, 0, 0, 0, 2, 0, 1, 0);

//    glRotatef(180, 0, 0, 1);
//    glRotatef(180, 0, 1, 0);

    glGetFloatv(GL_PROJECTION_MATRIX, projectionMatrix);

//    memcpy(pM, projectionMatrix, sizeof(pM));
//    printf("--------------\n");
//    printf("%f\t%f\t%f\t%f\n", pM[0][0], pM[1][0], pM[2][0], pM[3][0]);
//    printf("%f\t%f\t%f\t%f\n", pM[0][1], pM[1][1], pM[2][1], pM[3][1]);
//    printf("%f\t%f\t%f\t%f\n", pM[0][2], pM[1][2], pM[2][2], pM[3][2]);
//    printf("%f\t%f\t%f\t%f\n", pM[0][3], pM[1][3], pM[2][3], pM[3][3]);

	glUseProgram(T_program[1]);

	glColor4fv(white);

    int font_height = 11; //120 * view;
	/* Set font size to 48 pixels */
	FT_Set_Pixel_Sizes(face[italic], 0, font_height);

	glUniformMatrix4fv(uniform_proj1, 1, GL_FALSE, projectionMatrix);

	glUniform4fv(uniform_color0, 1, White);

	/* Effects of alignment */
	float origin_x = 10; //-(float)width * view;
	float origin_y = 20; //-(float)height * view;

	render_text_(text, origin_x, origin_y, font_height, italic);

	glPopMatrix();

	glUseProgram(0);

	//glEnable(GL_LIGHTING);
	//glEnable(GL_DEPTH_TEST);
}

//void free_resources()
//{
//	glDeleteProgram(T_program[0]);
//}

