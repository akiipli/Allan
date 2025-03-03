#define HEXAS 100000
#define POLYCHUNK 20

GLfloat H_Red[]    = {0.5, 0.0, 0.0, 0.5};
GLfloat H_Orange[] = {0.5, 0.5, 0.0, 0.5};
GLfloat H_Yellow[] = {0.0, 0.5, 0.0, 0.5};
GLfloat H_Green[]  = {0.0, 0.5, 0.5, 0.5};
GLfloat H_Blue[]   = {0.0, 0.0, 0.5, 0.5};
GLfloat H_Purple[] = {0.5, 0.0, 0.5, 0.5};
GLfloat H_White[]  = {0.5, 0.5, 0.5, 0.5};

GLfloat hexasColor[7][4];

void init_HexasColor()
{
    /*
    int c;
    float r, g, b;

    float i = 0.1;

    r = 0.9;
    g = 0.3;
    b = 0.6;

    for (c = 0; c < 7; c ++)
    {
        hexasColor[c][0] = r;
        hexasColor[c][1] = g;
        hexasColor[c][2] = b;
        hexasColor[c][3] = 0.5;

        r += i;
        g += i;
        b += i;

        r = fmod(r, 1.0);
        g = fmod(g, 1.0);
        b = fmod(b, 1.0);
    }
    */
    memcpy(hexasColor[0], H_Red, sizeof(float[4]));
    memcpy(hexasColor[1], H_Orange, sizeof(float[4]));
    memcpy(hexasColor[2], H_Yellow, sizeof(float[4]));
    memcpy(hexasColor[3], H_Green, sizeof(float[4]));
    memcpy(hexasColor[4], H_Blue, sizeof(float[4]));
    memcpy(hexasColor[5], H_Purple, sizeof(float[4]));
    memcpy(hexasColor[6], H_White, sizeof(float[4]));
}

typedef struct
{
    object * O;
    int idx;
}
polyPack;

typedef struct HexG HexG;

struct HexG
{
    int index;
    float Center[2];
    float Radius;
    float Verts[6][2];
    float Edges[6][2];
    direction D;
    direction D_light0;
    HexG * super; // since subdivided hexagons share parents, super may mislead
    int Level;

    union
    {
        HexG * neighbour_H[6];
//        struct
//        {
//            HexG * Red_n;
//            HexG * Orange_n;
//            HexG * Yellow_n;
//            HexG * Green_n;
//            HexG * Blue_n;
//            HexG * Purple_n;
//        };
    };

    int subdivided;
    union
    {
        HexG * subH[7];
//        struct
//        {
//            HexG * Red;
//            HexG * Orange;
//            HexG * Yellow;
//            HexG * Green;
//            HexG * Blue;
//            HexG * Purple;
//            HexG * White;
//        };
    };
    int polypacks;
    polyPack * Polygons;

    int lightpacks;
    polyPack * Lights;
    /*
      Add poly and vertex data, polygroups
    */
};

/*
            /---\   Aim is calculated counting center distance from camera to 1
           /     \  Centers can be rotated to Camera alignement rotVec_. Determining
           \  *  /  vertex positioning in the given direction and Radius is done
            \---/   with dot product comparison. If radius deviation exceeds or equals the
                    angle between ray to vertex and ray to hexagon center vertex is inside
                    hexagon radius. Further tests are taken for all vertices in polygon.
*/

int hexaIndex = 0;

HexG * Hexas[HEXAS];
HexG * Hex_Group[7];

float Hex_h_dim;

int subdivide_Hexagon(HexG * H)
{
    if (hexaIndex + 7 > HEXAS)
        return 0;

    int e;

    HexG * H_0;

    for (e = 0; e < 6; e ++)
    {
        if (H->neighbour_H[e] != NULL && H->neighbour_H[e]->subdivided)
        {
            H_0 = H->neighbour_H[e]->subH[(e + 3) % 6];
        }
        else
        {
            H_0 = calloc(1, sizeof(HexG));

            if (H_0 == NULL)
            {
                return 0;
                break;
            }
            else
            {
                H_0->Polygons = malloc(0);
                H_0->Lights = malloc(0);
                H_0->index = hexaIndex;
                H_0->super = H;
                H_0->Level = H->Level + 1;
                Hexas[hexaIndex ++] = H_0;
            }
        }

        H->subH[e] = H_0;
    }

    H_0 = calloc(1, sizeof(HexG));

    if (H_0 == NULL)
    {
        return 0;
    }
    else
    {
        H_0->Polygons = malloc(0);
        H_0->Lights = malloc(0);
        H_0->index = hexaIndex;
        H_0->super = H;
        H_0->Level = H->Level + 1;
        Hexas[hexaIndex ++] = H_0;
    }

    H->subH[6] = H_0;

    return 1;
}

void init_HexG(HexG * SuperH, float Center[2], float Radius)
{
    Hex_h_dim = sqrt(3.0/4.0);

    SuperH->Center[0] = Center[0];
    SuperH->Center[1] = Center[1];

    SuperH->Radius = Radius;

    SuperH->Verts[0][0] =  0;                  SuperH->Verts[0][1] = -1   * Radius;
    SuperH->Verts[1][0] =  Hex_h_dim * Radius; SuperH->Verts[1][1] = -0.5 * Radius;
    SuperH->Verts[2][0] =  Hex_h_dim * Radius; SuperH->Verts[2][1] =  0.5 * Radius;
    SuperH->Verts[3][0] =  0;                  SuperH->Verts[3][1] =  1   * Radius;
    SuperH->Verts[4][0] = -Hex_h_dim * Radius; SuperH->Verts[4][1] =  0.5 * Radius;
    SuperH->Verts[5][0] = -Hex_h_dim * Radius; SuperH->Verts[5][1] = -0.5 * Radius;

    SuperH->Verts[0][0] += Center[0]; SuperH->Verts[0][1] += Center[1];
    SuperH->Verts[1][0] += Center[0]; SuperH->Verts[1][1] += Center[1];
    SuperH->Verts[2][0] += Center[0]; SuperH->Verts[2][1] += Center[1];
    SuperH->Verts[3][0] += Center[0]; SuperH->Verts[3][1] += Center[1];
    SuperH->Verts[4][0] += Center[0]; SuperH->Verts[4][1] += Center[1];
    SuperH->Verts[5][0] += Center[0]; SuperH->Verts[5][1] += Center[1];

    SuperH->Edges[0][0] = (SuperH->Verts[0][0] + SuperH->Verts[1][0]) / 2; SuperH->Edges[0][1] = (SuperH->Verts[0][1] + SuperH->Verts[1][1]) / 2;
    SuperH->Edges[1][0] = (SuperH->Verts[1][0] + SuperH->Verts[2][0]) / 2; SuperH->Edges[1][1] = (SuperH->Verts[1][1] + SuperH->Verts[2][1]) / 2;
    SuperH->Edges[2][0] = (SuperH->Verts[2][0] + SuperH->Verts[3][0]) / 2; SuperH->Edges[2][1] = (SuperH->Verts[2][1] + SuperH->Verts[3][1]) / 2;
    SuperH->Edges[3][0] = (SuperH->Verts[3][0] + SuperH->Verts[4][0]) / 2; SuperH->Edges[3][1] = (SuperH->Verts[3][1] + SuperH->Verts[4][1]) / 2;
    SuperH->Edges[4][0] = (SuperH->Verts[4][0] + SuperH->Verts[5][0]) / 2; SuperH->Edges[4][1] = (SuperH->Verts[4][1] + SuperH->Verts[5][1]) / 2;
    SuperH->Edges[5][0] = (SuperH->Verts[5][0] + SuperH->Verts[0][0]) / 2; SuperH->Edges[5][1] = (SuperH->Verts[5][1] + SuperH->Verts[0][1]) / 2;

    if (SuperH->subdivided) // in main level 7 hexagons are created with their centers, dimensioning 6 units to cover almost full circle
                            // first is created center most hexagon and is subdivided (allocated), then neighbours are added and subdivided
                            // in counter clockwise order from bottom right edge. Subdivided flags are set first and then hexagon is initiated.
    {
        int e;
        float R;

        R = Radius / 2;

        for (e = 0; e < 6; e ++)
        {
            // check somehow if this subdivision exists
            if (SuperH->neighbour_H[e] != NULL && SuperH->neighbour_H[e]->subdivided)
            {
                SuperH->subH[e] = SuperH->neighbour_H[e]->subH[(e + 3) % 6];
            }
            else
            {
                init_HexG(SuperH->subH[e], SuperH->Edges[e], R);
            }
        }
        init_HexG(SuperH->subH[6], Center, R);

        for (e = 0; e < 6; e ++)
        {
            SuperH->subH[6]->neighbour_H[e] = SuperH->subH[e];
            SuperH->subH[e]->neighbour_H[(e + 3) % 6] = SuperH->subH[6];
        }

        SuperH->subH[0]->neighbour_H[2] = SuperH->subH[1];
        SuperH->subH[0]->neighbour_H[4] = SuperH->subH[5];

        SuperH->subH[1]->neighbour_H[3] = SuperH->subH[2];
        SuperH->subH[1]->neighbour_H[5] = SuperH->subH[0];

        SuperH->subH[2]->neighbour_H[0] = SuperH->subH[1];
        SuperH->subH[2]->neighbour_H[4] = SuperH->subH[3];

        SuperH->subH[3]->neighbour_H[1] = SuperH->subH[2];
        SuperH->subH[3]->neighbour_H[5] = SuperH->subH[4];

        SuperH->subH[4]->neighbour_H[2] = SuperH->subH[3];
        SuperH->subH[4]->neighbour_H[0] = SuperH->subH[5];

        SuperH->subH[5]->neighbour_H[1] = SuperH->subH[0];
        SuperH->subH[5]->neighbour_H[3] = SuperH->subH[4];
    }
}

int create_Seven_Hexagons()
{
    int e, h, h1, h2, h3, r, r_0, r_1, r_2, r_3;

    HexG * H, * H_0, * H_1, * H_2, * H_3;

    for (h = 0; h < 7; h ++)
    {
        H = calloc(1, sizeof(HexG));
        if (H == NULL)
        {
            return 0;
            break;
        }
        else
        {
            H->Polygons = malloc(0);
            H->Lights = malloc(0);
            H->index = hexaIndex;
            H->super = H;
            H->Level = 0;
            Hexas[hexaIndex ++] = H;
        }
        Hex_Group[h] = H;
    }

    for (e = 0; e < 6; e ++)
    {
        Hex_Group[6]->neighbour_H[e] = Hex_Group[e];
        Hex_Group[e]->neighbour_H[(e + 3) % 6] = Hex_Group[6];
    }

    Hex_Group[0]->neighbour_H[2] = Hex_Group[1];
    Hex_Group[0]->neighbour_H[4] = Hex_Group[5];

    Hex_Group[1]->neighbour_H[3] = Hex_Group[2];
    Hex_Group[1]->neighbour_H[5] = Hex_Group[0];

    Hex_Group[2]->neighbour_H[0] = Hex_Group[1];
    Hex_Group[2]->neighbour_H[4] = Hex_Group[3];

    Hex_Group[3]->neighbour_H[1] = Hex_Group[2];
    Hex_Group[3]->neighbour_H[5] = Hex_Group[4];

    Hex_Group[4]->neighbour_H[2] = Hex_Group[3];
    Hex_Group[4]->neighbour_H[0] = Hex_Group[5];

    Hex_Group[5]->neighbour_H[1] = Hex_Group[0];
    Hex_Group[5]->neighbour_H[3] = Hex_Group[4];

    H = Hex_Group[6];
    r = subdivide_Hexagon(H);
    if (r)
    {
        H->subdivided = 1;

        for (h = 6; h < 7; h ++)
        {
            H_0 = H->subH[h];
            r_0 = subdivide_Hexagon(H_0);
            if (r_0)
            {
                H_0->subdivided = 1;
            }
            // /*
            for (h1 = 0; h1 < 7; h1 ++)
            {
                H_1 = H_0->subH[h1];
                r_1 = subdivide_Hexagon(H_1);
                if (r_1)
                {
                    H_1->subdivided = 1;
                }

                for (h2 = 0; h2 < 7; h2 ++)
                {
                    H_2 = H_1->subH[h2];
                    r_2 = subdivide_Hexagon(H_2);
                    if (r_2)
                    {
                        H_2->subdivided = 1;
                    }

                    for (h3 = 0; h3 < 7; h3 ++)
                    {
                        H_3 = H_2->subH[h3];
                        r_3 = subdivide_Hexagon(H_3);
                        if (r_3)
                        {
                            H_3->subdivided = 1;
                        }
                        /*
                        for (h4 = 0; h4 < 7; h4 ++)
                        {
                            H_4 = H_3->subH[h4];
                            r_4 = subdivide_Hexagon(H_4);
                            if (r_4)
                            {
                                H_4->subdivided = 1;
                            }
                        }
                        */
                    }
                }
            }
            // */
        }

        init_HexG(H, (float[2]){0, 0}, 1.0);
    }

    Hex_h_dim = sqrt(3.0/4.0);

    float Center[6][2] = {{Hex_h_dim, -1.5}, {Hex_h_dim * 2, 0}, {Hex_h_dim, 1.5}, {-Hex_h_dim, 1.5}, {-Hex_h_dim * 2, 0}, {-Hex_h_dim, -1.5}};

    for (h = 0; h < 6; h ++)
    {
        H = Hex_Group[h];
        //r = subdivide_Hexagon(H);
        //if (r)
        //{
        //    H->subdivided = 1;
            init_HexG(H, Center[h], 1.0);
        //}
    }

    return 1;
}

void print_Sub_Hex_Info(HexG * H)
{
    int p, e;

    if (H->polypacks > 0)
    {
        printf("\t%d\t%f\t%f\t%f\t%d\n", H->index, H->Center[0], H->Center[1], H->Radius, H->subdivided);
        for (p = 0; p < H->polypacks; p ++)
        {
            printf("%s ", H->Polygons[p].O->Name);
            printf("%d ", H->Polygons[p].idx);
        }
        printf("\n");
    }

    if (H->subdivided)
    {
        for (e = 0; e < 7; e ++)
        {
            if (H->subH[e] != NULL)
                print_Sub_Hex_Info(H->subH[e]);
        }
    }
}

void print_Hexa_Groups_Info()
{
    int p, e, h;

    HexG * H;

    printf("hexaIndex %d\n", hexaIndex);

    for (h = 0; h < 7; h ++)
    {
        H = Hex_Group[h];
        if (H->polypacks > 0)
        {
            printf("\t%d\t%f\t%f\t%f\t%d\n", H->index, H->Center[0], H->Center[1], H->Radius, H->subdivided);
            for (p = 0; p < H->polypacks; p ++)
            {
                printf("%s ", H->Polygons[p].O->Name);
                printf("%d ", H->Polygons[p].idx);
            }
            printf("\n");
        }

        if (H->subdivided)
        {
            for (e = 0; e < 7; e ++)
            {
                if (H->subH[e] != NULL)
                    print_Sub_Hex_Info(H->subH[e]);
            }
        }
    }
}

void print_SubH_Info(HexG * H)
{
    int e;
    printf("\t%d\t%f\t%f\t%f\t%d\n", H->index, H->Center[0], H->Center[1], H->Radius, H->subdivided);
    printf("\t");
    for (e = 0; e < 6; e ++)
    {
        if (H->neighbour_H[e] != NULL)
            printf("%d-", H->neighbour_H[e]->index);
        else
            printf("n-");
        if (H->neighbour_H[e] != NULL && H->neighbour_H[e]->neighbour_H[(e + 3) % 6] != NULL)
            printf("%d ", H->neighbour_H[e]->neighbour_H[(e + 3) % 6]->index);
        else
            printf("n ");
    }
    printf("\n");
    if (H->subdivided)
    {
        for (e = 0; e < 7; e ++)
        {
            if (H->subH[e] != NULL)
                print_SubH_Info(H->subH[e]);
            else
                printf("n ");
        }
        printf("\n");
    }
}

void print_Hexagons_Info()
{
    int e, h;

    HexG * H;

    printf("hexaIndex %d\n", hexaIndex);

    for (h = 0; h < 7; h ++)
    {
        H = Hex_Group[h];

//    for (h = 0; h < hexaIndex; h ++)
//    {
//        H = Hexas[h];

        printf("%d\t%f\t%f\t%f\t%d\n", H->index, H->Center[0], H->Center[1], H->Radius, H->subdivided);
        for (e = 0; e < 6; e ++)
        {
            if (H->neighbour_H[e] != NULL)
                printf("%d-", H->neighbour_H[e]->index);
            else
                printf("n-");
            if (H->neighbour_H[e] != NULL && H->neighbour_H[e]->neighbour_H[(e + 3) % 6] != NULL)
                printf("%d ", H->neighbour_H[e]->neighbour_H[(e + 3) % 6]->index);
            else
                printf("n ");
        }
        printf("\n");
        if (H->subdivided)
        {
            for (e = 0; e < 7; e ++)
            {
                if (H->subH[e] != NULL)
                    print_SubH_Info(H->subH[e]);
                else
                    printf("n ");
            }
        }
        printf("\n");
    }
}

void free_Hexagons()
{
    int h;

    HexG * H;

    for (h = 0; h < hexaIndex; h ++)
    {
        H = Hexas[h];
        if (H->Polygons != NULL)
        {
            free(H->Polygons);
        }
        if (H->Lights != NULL)
        {
            free(H->Lights);
        }
        free(H);
    }

    hexaIndex = 0;
}
