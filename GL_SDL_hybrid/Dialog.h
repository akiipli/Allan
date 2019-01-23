/*
The MIT License

Copyright <2018> <Allan Kiipli>
*/

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <math.h>
#include <time.h>
#include <libgen.h>

#define BUF_SIZE 2048
#define IN_ARRAY 1000000
#define POLY_VERTS 64
#define VERT_EDGES 64
#define UV_VERT_EDGES 64

typedef struct // Face
{
    int vert[POLY_VERTS];
    int text[POLY_VERTS];
    int norm[POLY_VERTS];
    int vert_c;
    int text_c;
    int norm_c;
}
Face;

typedef struct
{
    int initialized;
    float * Vertcoords;
    float * Textcoords;
    float * Normcoords;
    Face * Faces;
    int Vertcoord_c;
    int Textcoord_c;
    int Normcoord_c;
    int Face_c;
}
OBJ_In;

char szFile[256];

float VertexCoords[IN_ARRAY];
float UVtextCoords[IN_ARRAY];
float NormalCoords[IN_ARRAY];
Face Faces[IN_ARRAY];

char * open_FileName(HWND hwnd, char * InitialDir, char * FileTitle) // owner window
{
    OPENFILENAME ofn;       // common dialog box structure
    // Initialize OPENFILENAME
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = hwnd;
    ofn.lpstrFile = szFile;
    // Set lpstrFile[0] to '\0' so that GetOpenFileName does not
    // use the contents of szFile to initialize itself.
    ofn.lpstrFile[0] = '\0';
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFilter = "All\0*.*\0Text\0*.TXT\0";
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = FileTitle;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = InitialDir;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

    // Display the Open dialog box.

    GetOpenFileName(&ofn);

    printf("%s\n", ofn.lpstrFile);

    return szFile;
}

OBJ_In OBJ;

OBJ_In read_OBJ_file(char * fileName)
{
    OBJ.initialized = 0;
    FILE * fp;
    fp = fopen(fileName, "r");
    if (fp == NULL)
    {
        printf("Maybe no permission.\n");
        return OBJ;
    }
    char buff[BUF_SIZE];
    buff[0] = '\0';

    float Vertex[3];
    int Vertexcoord_count = 0;
    float UVtext[2];
    int UVtextcoord_count = 0;
    float Normal[3];
    int Normalcoord_count = 0;
    int Face_count = 0;

    int vert_c = 0;
    int text_c = 0;
    int norm_c = 0;
    int line_c = 0;

    int b = 0;

    while(fgets(buff, BUF_SIZE, fp))
    {
        if (buff[0] == 'v' && buff[1] == ' ')
        {
            sscanf(buff, "%*s %f %f %f", &Vertex[0], &Vertex[1], &Vertex[2]);
            VertexCoords[Vertexcoord_count++] = Vertex[0];
            VertexCoords[Vertexcoord_count++] = Vertex[1];
            VertexCoords[Vertexcoord_count++] = Vertex[2];
        }
        else if (buff[0] == 'v' && buff[1] == 'n' && buff[2] == ' ')
        {
            sscanf(buff, "%*s %f %f %f", &Normal[0], &Normal[1], &Normal[2]);
            NormalCoords[Normalcoord_count++] = Normal[0];
            NormalCoords[Normalcoord_count++] = Normal[1];
            NormalCoords[Normalcoord_count++] = Normal[2];
        }
        else if (buff[0] == 'v' && buff[1] == 't' && buff[2] == ' ')
        {
            sscanf(buff, "%*s %f %f", &UVtext[0], &UVtext[1]);
            UVtextCoords[UVtextcoord_count++] = UVtext[0];
            UVtextCoords[UVtextcoord_count++] = UVtext[1];
        }
        else if (buff[0] == 'f' && buff[1] == ' ')
        {
            b = 1;
            vert_c = 0;
            text_c = 0;
            norm_c = 0;
            line_c = 0;
            while (buff[b] != '\n' && b < BUF_SIZE)
            {
                b ++;
                while (buff[b] != ' ' && buff[b] != '\n' && b < BUF_SIZE)
                {
                    sscanf(&buff[b++], "%d", &Faces[Face_count].vert[vert_c++]);
                    while (isdigit(buff[b]) && b < BUF_SIZE)
                    {
                        b++;
                    }
                    if (buff[b] == ' ' || buff[b] == '\n')
                    {
                        Faces[Face_count].text[text_c++] = 1;
                        Faces[Face_count].norm[norm_c++] = 1;
                        continue;
                    }
                    b++;
                    if (buff[b] == '/')
                    {
                        Faces[Face_count].text[text_c++] = 1;
                    }
                    else
                    {
                        sscanf(&buff[b++], "%d", &Faces[Face_count].text[text_c++]);
                    }
                    while (isdigit(buff[b]) && b < BUF_SIZE)
                    {
                        b++;
                    }
                    if (buff[b] == ' ' || buff[b] == '\n')
                    {
                        Faces[Face_count].norm[norm_c++] = 1;
                        continue;
                    }
                    b++;
                    if (buff[b] == '/')
                    {
                        Faces[Face_count].norm[norm_c++] = 1;
                    }
                    else
                    {
                        sscanf(&buff[b++], "%d", &Faces[Face_count].norm[norm_c++]);
                    }
                    while (isdigit(buff[b]) && b < BUF_SIZE)
                    {
                        b++;
                    }
                }
                if (norm_c >= POLY_VERTS && buff[b] != '\n' && b > 1) // Here polygons are prepared in chunks.
                {
                    Faces[Face_count].vert_c = vert_c;
                    Faces[Face_count].text_c = text_c;
                    Faces[Face_count].norm_c = norm_c;

                    line_c ++;

                    Face_count ++;

                    vert_c = 0;
                    text_c = 0;
                    norm_c = 0;

                    Faces[Face_count].vert[vert_c++] = Faces[Face_count - line_c].vert[0];
                    Faces[Face_count].text[text_c++] = Faces[Face_count - line_c].text[0];
                    Faces[Face_count].norm[norm_c++] = Faces[Face_count - line_c].norm[0];

                    Faces[Face_count].vert[vert_c++] = Faces[Face_count - 1].vert[Faces[Face_count - 1].vert_c - 1];
                    Faces[Face_count].text[text_c++] = Faces[Face_count - 1].text[Faces[Face_count - 1].text_c - 1];
                    Faces[Face_count].norm[norm_c++] = Faces[Face_count - 1].norm[Faces[Face_count - 1].norm_c - 1];
                }

            }
            Faces[Face_count].vert_c = vert_c;
            Faces[Face_count].text_c = text_c;
            Faces[Face_count].norm_c = norm_c;

            Face_count ++;
        }
    }
    fclose(fp);

    if (Face_count > 0)
    {
        OBJ.initialized = 1;
    }

    OBJ.Vertcoords = VertexCoords;
    OBJ.Textcoords = UVtextCoords;
    OBJ.Normcoords = NormalCoords;
    OBJ.Faces = Faces;
    OBJ.Vertcoord_c = Vertexcoord_count;
    OBJ.Textcoord_c = UVtextcoord_count;
    OBJ.Normcoord_c = Normalcoord_count;
    OBJ.Face_c = Face_count;

    int f, v;
    for (f = 0; f < Face_count; f ++) // Hence OBJ indexes start at 1.
    {
        for (v = 0; v < OBJ.Faces[f].vert_c; v ++)
        {
            OBJ.Faces[f].vert[v] -= 1;
            OBJ.Faces[f].text[v] -= 1;
            OBJ.Faces[f].norm[v] -= 1;
        }
    }

    return OBJ;
}
