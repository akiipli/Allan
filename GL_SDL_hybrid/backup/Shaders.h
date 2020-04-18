GLuint S_program;
GLint attribute_coord;
GLint uniform_tex;
GLint uniform_color;

GLuint vbo;

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

GLuint font_tex;

int init_fonts_()
{
	glGenTextures(1, &font_tex);
	/* Initialize the FreeType2 library */
	if(FT_Init_FreeType(&ft)) {
		fprintf(stderr, "Could not init freetype library\n");
		return 0;
	}

    char dirfile[STRLEN];
    dirfile[0] = '\0';
    strcat(dirfile, resources_dir);
    strcat(dirfile, "courbd.ttf");
	if(FT_New_Face(ft, dirfile, 0, &face)) {
		fprintf(stderr, "Could not open font\n");
		return 0;
	}

	/* Compile and link the shader program */
	GLint link_ok = GL_FALSE;

	GLuint vs, fs;
    dirfile[0] = '\0';
    strcat(dirfile, resources_dir);
    strcat(dirfile, "Shaders/text.v.glsl");
	if ((vs = create_shader(dirfile, GL_VERTEX_SHADER))	 == 0) return 0;
    dirfile[0] = '\0';
    strcat(dirfile, resources_dir);
    strcat(dirfile, "Shaders/text.f.glsl");
	if ((fs = create_shader(dirfile, GL_FRAGMENT_SHADER)) == 0) return 0;

	S_program = glCreateProgram();
	glAttachShader(S_program, vs);
	glAttachShader(S_program, fs);
	glLinkProgram(S_program);
	glGetProgramiv(S_program, GL_LINK_STATUS, &link_ok);
	if (!link_ok) {
		printf("glLinkProgram:");
		print_log(S_program);
		return 0;
	}

	attribute_coord = glGetAttribLocation(S_program, "coord");
	if (attribute_coord == -1) {
		printf("Could not bind coord attribute \n");
		return 0;
	}

	uniform_tex = glGetUniformLocation(S_program, "tex");
	if (uniform_tex == -1) {
		printf("Could not bind tex uniform \n");
		return 0;
	}

	uniform_color = glGetUniformLocation(S_program, "color");
	if (uniform_color == -1) {
		printf("Could not bind color uniform \n");
		return 0;
	}

	// Create the vertex buffer object
	glGenBuffers(1, &vbo);

	return 1;
}

void render_text_(const char * text, float x, float y, int font_height)
{
	const char * p;
	FT_GlyphSlot G = face->glyph;

	/* Create a texture that will be used to hold one "glyph" */
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, font_tex);
	glUniform1i(uniform_tex, 0);

	/* We require 1 byte alignment when uploading texture data */
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	/* Clamping to edges is important to prevent artifacts when scaling */
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	/* Linear filtering usually looks best for text */
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	/* Set up the VBO for our vertex data */
	glEnableVertexAttribArray(attribute_coord);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glVertexAttribPointer(attribute_coord, 4, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 4, 0);

	/* Loop through all characters */

	float X = x;
	int c = 0;
	int tab = 10;
	int tab_c = 0;
	for(p = text; * p; p ++) {
		/* Try to load and render the character */
		if(FT_Load_Char(face, * p, FT_LOAD_RENDER))
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

		point box[4] =
		{{x0,     y + lower_part, 0, 1},
         {x0 + w, y + lower_part, 1, 1},
         {x0 + w, y - G->bitmap_top, 1, 0},
         {x0,     y - G->bitmap_top, 0, 0}};

		/* Draw the character on the screen */
		glBufferData(GL_ARRAY_BUFFER, 16 * sizeof(GLfloat), box, GL_DYNAMIC_DRAW);
		glDrawArrays(GL_QUADS, 0, 4);

		/* Advance the cursor to the start of the next character */
		x += G->advance.x >> 6;
        y += G->advance.y >> 6;
        c ++;
	}

	glDisableVertexAttribArray(attribute_coord);
}

void display_font(const char * text, int width, int height)
{
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0, width, height, 0, 1, -1);

	glUseProgram(S_program);

	GLfloat white[4] = {1, 1, 1, 1};

	glColor4fv(white);

    int font_height = 11; //120 * view;
	/* Set font size to 48 pixels */
	FT_Set_Pixel_Sizes(face, 0, font_height);

	glUniform4fv(uniform_color, 1, white);

	/* Effects of alignment */
	float origin_x = 10; //-(float)width * view;
	float origin_y = 20; //-(float)height * view;

	render_text_(text, origin_x, origin_y, font_height);

	glPopMatrix();

	glUseProgram(0);
}

//void free_resources()
//{
//	glDeleteProgram(S_program);
//}

