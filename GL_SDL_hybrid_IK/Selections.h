/*
The MIT License

Copyright <2018> <Allan Kiipli>
*/

#define SELECTION_TYPE_OBJECT "object"
#define SELECTION_TYPE_POLYGON "polygon"
#define SELECTION_TYPE_EDGE  "edge"
#define SELECTION_TYPE_VERTEX  "vertex"

#define SELECTIONS 100

selection object_selection[SELECTIONS];
int object_selections = 0;

int Selections_c[SELS_NUM];
char Sels_Names[SELS_NUM][SELECTIONS][STRLEN];
char Sels_Counts[SELS_NUM][SELECTIONS][STRLEN];
char * sel_types[SELS_NUM];
int SelsIndex[SELS_NUM];
int sels_start[SELS_NUM];
int sel_type_count = SELS_NUM;
char sel_type[TYPE_LENGTH];

int current_sel_type = 0;

void init_selections()
{
    int i;
    for (i = 0; i < SELS_NUM; i ++)
    {
        SelsIndex[i] = 0;
        sels_start[i] = 0;
        memcpy(Sels_Names[i][0], "All", 4);
        memcpy(Sels_Counts[i][0], "0", 2);
        Selections_c[i] = 0;
    }

    sel_types[0] = SELECTION_TYPE_OBJECT;
    sel_types[1] = SELECTION_TYPE_POLYGON;
    sel_types[2] = SELECTION_TYPE_EDGE;
    sel_types[3] = SELECTION_TYPE_VERTEX;

    memcpy(&sel_type, sel_types[0], strlen(sel_types[0]));
}

void reinit_selections()
{
    int i;
    for (i = 0; i < SELS_NUM; i ++)
    {
        SelsIndex[i] = 0;
        sels_start[i] = 0;
        memcpy(Sels_Names[i][0], "All", 4);
        memcpy(Sels_Counts[i][0], "0", 2);
        Selections_c[i] = 0;
    }
}

void replace_Selections_Name(int * Objects, int Objects_count, int type, char * Name_Remember, char * EditString)
{
    object * O;
    int o, s;


    if (type == 0) // objects
    {

    }
    else if (type == 1) // polys
    {
        for (o = 0; o < Objects_count; o ++)
        {
            O = objects[Objects[o]];
            for (s = 0; s < O->polygon_selections; s ++)
            {
                if (strcmp(Name_Remember, O->polygon_selection[s]->Name) == 0)
                {
                    memcpy(O->polygon_selection[s]->Name, EditString, strlen(EditString));
                    O->polygon_selection[s]->Name[strlen(EditString)] = '\0';
                    break;
                }
            }
        }
    }
    else if (type == 2) // edges
    {
        for (o = 0; o < Objects_count; o ++)
        {
            O = objects[Objects[o]];
            for (s = 0; s < O->edge_selections; s ++)
            {
                if (strcmp(Name_Remember, O->edge_selection[s]->Name) == 0)
                {
                    memcpy(O->edge_selection[s]->Name, EditString, strlen(EditString));
                    O->edge_selection[s]->Name[strlen(EditString)] = '\0';
                    break;
                }
            }
        }
    }
    else if (type == 3) // verts
    {
        for (o = 0; o < Objects_count; o ++)
        {
            O = objects[Objects[o]];
            for (s = 0; s < O->vertex_selections; s ++)
            {
                if (strcmp(Name_Remember, O->vertex_selection[s]->Name) == 0)
                {
                    memcpy(O->vertex_selection[s]->Name, EditString, strlen(EditString));
                    O->vertex_selection[s]->Name[strlen(EditString)] = '\0';
                    break;
                }
            }
        }
    }
}

void create_Selections_List(int * Objects, int Objects_count, int type)
{
    object * O;
    int o, s, c, condition;


    if (type == 0) // objects
    {
        for (s = 0; s < Selections_c[0]; s ++)
        {
            sprintf(Sels_Counts[0][s], "%d", object_selection[s].indices_count);
        }
    }
    else if (type == 1) // polys
    {
        Selections_c[1] = 0;
        for (o = 0; o < Objects_count; o ++)
        {
            O = objects[Objects[o]];
            for (s = 0; s < O->polygon_selections; s ++)
            {
                condition = 1;
                for (c = 0; c < Selections_c[1]; c ++)
                {
                    if (strcmp(Sels_Names[1][c], O->polygon_selection[s]->Name) == 0)
                    {
                        sprintf(Sels_Counts[1][c], "%s|%d", Sels_Counts[1][c], O->polygon_selection[s]->indices_count);
                        condition = 0;
                        break;
                    }
                }
                if (condition)
                {
                    O->polygon_selection[s]->index = Selections_c[1];
                    sprintf(Sels_Names[1][Selections_c[1]], "%s", O->polygon_selection[s]->Name);
                    sprintf(Sels_Counts[1][Selections_c[1]], "%d", O->polygon_selection[s]->indices_count);
                    Selections_c[1] ++;
                }
            }
        }
    }
    else if (type == 2) // edges
    {
        Selections_c[2] = 0;
        for (o = 0; o < Objects_count; o ++)
        {
            O = objects[Objects[o]];
            for (s = 0; s < O->edge_selections; s ++)
            {
                condition = 1;
                for (c = 0; c < Selections_c[2]; c ++)
                {
                    if (strcmp(Sels_Names[2][c], O->edge_selection[s]->Name) == 0)
                    {
                        sprintf(Sels_Counts[2][c], "%s|%d", Sels_Counts[2][c], O->edge_selection[s]->indices_count);
                        condition = 0;
                        break;
                    }
                }
                if (condition)
                {
                    O->edge_selection[s]->index = Selections_c[2];
                    sprintf(Sels_Names[2][Selections_c[2]], "%s", O->edge_selection[s]->Name);
                    sprintf(Sels_Counts[2][Selections_c[2]], "%d", O->edge_selection[s]->indices_count);
                    Selections_c[2] ++;
                }
            }
        }
    }
    else if (type == 3) // verts
    {
        Selections_c[3] = 0;
        for (o = 0; o < Objects_count; o ++)
        {
            O = objects[Objects[o]];
            for (s = 0; s < O->vertex_selections; s ++)
            {
                condition = 1;
                for (c = 0; c < Selections_c[3]; c ++)
                {
                    if (strcmp(Sels_Names[3][c], O->vertex_selection[s]->Name) == 0)
                    {
                        sprintf(Sels_Counts[3][c], "%s|%d", Sels_Counts[3][c], O->vertex_selection[s]->indices_count);
                        condition = 0;
                        break;
                    }
                }
                if (condition)
                {
                    O->vertex_selection[s]->index = Selections_c[3];
                    sprintf(Sels_Names[3][Selections_c[3]], "%s", O->vertex_selection[s]->Name);
                    sprintf(Sels_Counts[3][Selections_c[3]], "%d", O->vertex_selection[s]->indices_count);
                    Selections_c[3] ++;
                }
            }
        }
    }

    if (SelsIndex[current_sel_type] >= Selections_c[current_sel_type]) SelsIndex[current_sel_type] = Selections_c[current_sel_type] - 1;
    if (SelsIndex[current_sel_type] < 0) SelsIndex[current_sel_type] = 0;
}

void set_Vert_Selection(vert_selection * S)
{
    int c;

    for (c = 0; c < Selections_c[3]; c ++)
    {
        if (strcmp(Sels_Names[3][c], S->Name) == 0)
        {
            SelsList[SelsIndex[3] - sels_start[3]].color = UI_BLACK;
            SelsIndex[3] = c;
            break;
        }
    }
}

void set_Hier_H_Button(int index)
{
    int i;
    for (i = 0; i < H_HIER_NUM; i ++)
    {
        Button_h_hier[i].color = UI_GRAYB;
    }
    if (index > -1)
        Button_h_hier[index].color = UI_GRAYD;
}

void set_Sels_H_Button(int index)
{
    int i;
    for (i = 0; i < H_SELS_NUM; i ++)
    {
        Button_h_sels[i].color = UI_GRAYB;
    }
    if (index > -1)
        Button_h_sels[index].color = UI_GRAYD;
}

void edit_Selection()
{
    if (Selections_c[current_sel_type] > 0)
    {
        sprintf(Name_Remember, "%s", Sels_Names[current_sel_type][SelsIndex[current_sel_type]]);
        sprintf(Sels_Names[current_sel_type][SelsIndex[current_sel_type]], "%s", "");
        printf("Editing now!\n");
        Edit_Lock = 1;
        init_Selection_Rectangle();
        set_Sels_H_Button(1);
    }
}

void add_Selection()
{
    if (Selections_c[current_sel_type] < SELECTIONS - 1)
    {
        printf("Add selection!\n");
        memcpy(Sels_Names[current_sel_type][Selections_c[current_sel_type]], "Untitled", sizeof("Untitled"));
        if (SelsIndex[current_sel_type] - sels_start[current_sel_type] >= 0)
            SelsList[SelsIndex[current_sel_type] - sels_start[current_sel_type]].color = UI_BLACK;
        SelsIndex[current_sel_type] = Selections_c[current_sel_type];
        if (SelsIndex[current_sel_type] - sels_start[current_sel_type] >= 0)
            SelsList[SelsIndex[current_sel_type] - sels_start[current_sel_type]].color = UI_BACKL;
        Selections_c[current_sel_type] ++;

        if (current_sel_type == 0)
        {
            object_selection[object_selections].indices = malloc(1 * sizeof(int));
            object_selection[object_selections].indices_count = 0;
            object_selection[object_selections ++].Name = Sels_Names[current_sel_type][Selections_c[current_sel_type]];
        }
        set_Sels_H_Button(0);
    }
}

void remove_Selection()
{
    int no_action = 0;

    if (current_sel_type == 0) // objects
    {
        int i;
        char * Name;
        int index = SelsIndex[0];

        object_selection[index].indices_count = 0;
        object_selection[index].indices = realloc(object_selection[index].indices, 1);
        object_selections --;
        for (i = SelsIndex[0]; i < object_selections; i ++)
        {
            Name = object_selection[i].Name;
            object_selection[i] = object_selection[i + 1];
            object_selection[i].index -= 1;
            object_selection[i].Name = Name;
        }
    }
    else if (current_sel_type == 1) // polys
    {
        int o, i, index;
        int condition;

        object * O;

        printf("Polygon selection type\n");

        for (o = 0; o < selected_object_count; o ++)
        {
            condition = 0;
            O = objects[selected_objects[o]];
            printf("%s %d\n", O->Name, O->polygon_selections);
            for (i = 0; i < O->polygon_selections; i ++)
            {
                if (strcmp(O->polygon_selection[i]->Name, Sels_Names[1][SelsIndex[1]]) == 0)
                {
                    index = i;
                    condition = 1;
                    printf("Selection exists! %s %s\n", O->Name, O->polygon_selection[index]->Name);
                    break;
                }
            }
            if (condition)
            {
                free(O->polygon_selection[index]->indices);
                free(O->polygon_selection[index]);

                O->polygon_selections -= 1;
                for (i = index; i < O->polygon_selections; i ++)
                {
                    O->polygon_selection[i] = O->polygon_selection[i + 1];
                    O->polygon_selection[i]->index -= 1;
                }
            }
        }
    }
    else if (current_sel_type == 2) // edges
    {
        int o, i, index;
        int condition;

        object * O;

        printf("Edge selection type\n");

        for (o = 0; o < selected_object_count; o ++)
        {
            condition = 0;
            O = objects[selected_objects[o]];
            printf("%s %d\n", O->Name, O->edge_selections);
            for (i = 0; i < O->edge_selections; i ++)
            {
                if (strcmp(O->edge_selection[i]->Name, Sels_Names[2][SelsIndex[2]]) == 0)
                {
                    index = i;
                    condition = 1;
                    printf("Selection exists! %s %s\n", O->Name, O->edge_selection[index]->Name);
                    break;
                }
            }
            if (condition)
            {
                free(O->edge_selection[index]->indices);
                free(O->edge_selection[index]);

                O->edge_selections -= 1;
                for (i = index; i < O->edge_selections; i ++)
                {
                    O->edge_selection[i] = O->edge_selection[i + 1];
                    O->edge_selection[i]->index -= 1;
                }
            }
        }
    }
    else if (current_sel_type == 3) // verts
    {
        int o, i, index;
        int condition;

        object * O;
        //deformer * D;
        transformer * T;
        vert_selection * S;

        printf("Vertex selection type\n");

        for (o = 0; o < selected_object_count; o ++)
        {
            condition = 0;
            O = objects[selected_objects[o]];
            if (O->binding)
                continue;
            printf("%s %d\n", O->Name, O->vertex_selections);
            for (i = 0; i < O->vertex_selections; i ++)
            {
                if (strcmp(O->vertex_selection[i]->Name, Sels_Names[3][SelsIndex[3]]) == 0)
                {
                    index = i;
                    condition = 1;
                    printf("Selection exists! %s %s\n", O->Name, O->vertex_selection[index]->Name);
                    break;
                }
            }
            if (condition)
            {
                S = O->vertex_selection[index];
                T = S->Transformer;
                if (T == NULL)
                {
                    free(O->vertex_selection[index]->indices);
                    free(O->vertex_selection[index]->weights);

                    free(O->vertex_selection[index]->split[0].indices);
                    free(O->vertex_selection[index]->split[0].weights);

                    free(O->vertex_selection[index]->split[1].indices);
                    free(O->vertex_selection[index]->split[1].weights);

                    free(O->vertex_selection[index]);

                    O->vertex_selections -= 1;
                    for (i = index; i < O->vertex_selections; i ++)
                    {
                        O->vertex_selection[i] = O->vertex_selection[i + 1];
                        O->vertex_selection[i]->index -= 1;
                    }
                }
                else
                {
                    no_action = 1;
                }
            }
        }
    }

    if (!no_action)
    {
        printf("Remove selection!\n");
        int s;
        if (SelsIndex[current_sel_type] == Selections_c[current_sel_type] - 1)
        {

        }
        else
        {
            for (s = SelsIndex[current_sel_type]; s < Selections_c[current_sel_type] - 1; s ++)
            {
                memcpy(Sels_Names[current_sel_type][s], Sels_Names[current_sel_type][s + 1], sizeof(Sels_Names[current_sel_type][s + 1]));
            }
        }

        SelsList[SelsIndex[current_sel_type] - sels_start[current_sel_type]].color = UI_BLACK;

        Selections_c[current_sel_type] --;
        SelsIndex[current_sel_type] --;
        if (Selections_c[current_sel_type] < 0)
        {
            Selections_c[current_sel_type] = 0;
        }
        if (SelsIndex[current_sel_type] < 0)
        {
            SelsIndex[current_sel_type] = 0;
        }
        sels_start[current_sel_type] --;
        if (sels_start[current_sel_type] < 0)
        {
            sels_start[current_sel_type] = 0;
        }
        SelsList[SelsIndex[current_sel_type] - sels_start[current_sel_type]].color = UI_BACKL;
        set_Sels_H_Button(2);
    }
    else
    {
        printf("Remove selection failed!\n");
    }

    //create_Selections_List(selected_objects, selected_object_count, current_sel_type);
}

void assign_Selection_L()
{
    printf("Assign selection!\n");
    set_Sels_H_Button(3);
    if (current_sel_type == 0) // objects
    {
        int o, i, idx, condition;

        for (o = 0; o < selected_object_count; o ++)
        // scroll through selected objects
        {
            condition = 1;
            idx = selected_objects[o];
            for (i = 0; i < object_selection[SelsIndex[0]].indices_count; i ++)
            // scroll through current object selection indexes
            {
                if (object_selection[SelsIndex[0]].indices[i] == idx)
                {
                    // object is already in this list
                    condition = 0;
                    break;
                }
            }
            if (condition)
            {
                printf("Assigning %s\n", objects[o]->Name);
                int indices_count = object_selection[SelsIndex[0]].indices_count + 1;
                object_selection[SelsIndex[0]].indices = realloc(object_selection[SelsIndex[0]].indices, indices_count * sizeof(int));
                object_selection[SelsIndex[0]].indices[object_selection[SelsIndex[0]].indices_count ++] = idx;
            }
        }
    }
    else if (current_sel_type == 1) // polys
    {
        int o, i, p, i_c, index;
        polygon * P;
        int condition;
        printf("Polygon selection type\n");
        object * O;

        for (o = 0; o < selected_object_count; o ++)
        {
            condition = 1;
            O = objects[selected_objects[o]];

            if (!O->selected_polys_count) continue;

            for (i = 0; i < O->polygon_selections; i ++)
            {
                if (strcmp(O->polygon_selection[i]->Name, Sels_Names[1][SelsIndex[1]]) == 0)
                {
                    index = i;
                    printf("Selection exists! Appending.\n");
                    condition = 0;
                    break;
                }
            }
            if (condition)
            {
                index = O->polygon_selections;
                O->polygon_selections ++;
                O->polygon_selection = realloc(O->polygon_selection, O->polygon_selections * sizeof(selection*));
                O->polygon_selection[index] = malloc(sizeof(selection));
                O->polygon_selection[index]->indices_count = 0;
                O->polygon_selection[index]->indices = malloc(0 * sizeof(int));
                O->polygon_selection[index]->Object = O;

                //O->polygon_selection[index]->index = index;
                O->polygon_selection[index]->Name = malloc(STRLEN * sizeof(char));
                memcpy(O->polygon_selection[index]->Name, Sels_Names[1][SelsIndex[1]],
                       strlen(Sels_Names[1][SelsIndex[1]]));
                O->polygon_selection[index]->Name[strlen(Sels_Names[1][SelsIndex[1]])] = '\0';
            }

            int * selected_elements = malloc(O->polycount * sizeof(int));
            int selected_elements_count = 0;

            for (p = 0; p < O->polycount; p ++)
            {
                P = &O->polys[p / ARRAYSIZE][p % ARRAYSIZE];
                if (P->selected)
                {
                    selected_elements[selected_elements_count ++] = p;
                }
                else
                {
                    for (i_c = 0; i_c < O->polygon_selection[index]->indices_count; i_c ++)
                    {
                        if (O->polygon_selection[index]->indices[i_c] == p)
                        {
                            selected_elements[selected_elements_count ++] = p;
                            break;
                        }
                    }
                }
            }
            O->polygon_selection[index]->indices = realloc(O->polygon_selection[index]->indices, selected_elements_count * sizeof(int));
            O->polygon_selection[index]->indices_count = selected_elements_count;
            printf("selected elements count %d\n", selected_elements_count);
            for (i = 0; i < selected_elements_count; i ++)
            {
                O->polygon_selection[index]->indices[i] = selected_elements[i];
            }
            free(selected_elements);
        }
    }
    else if (current_sel_type == 2) // edges
    {
        int o, i, e, i_c, index;
        edge * E;
        int condition;
        printf("Edge selection type\n");
        object * O;

        for (o = 0; o < selected_object_count; o ++)
        {
            condition = 1;
            O = objects[selected_objects[o]];

            if (!O->selected_edges_count) continue;

            for (i = 0; i < O->edge_selections; i ++)
            {
                if (strcmp(O->edge_selection[i]->Name, Sels_Names[2][SelsIndex[2]]) == 0)
                {
                    index = i;
                    printf("Selection exists! Appending.\n");
                    condition = 0;
                    break;
                }
            }
            if (condition)
            {
                index = O->edge_selections;
                O->edge_selections ++;
                O->edge_selection = realloc(O->edge_selection, O->edge_selections * sizeof(selection*));
                O->edge_selection[index] = malloc(sizeof(selection));
                O->edge_selection[index]->indices_count = 0;
                O->edge_selection[index]->indices = malloc(0 * sizeof(int));
                O->edge_selection[index]->Object = O;

                //O->edge_selection[index]->index = index;
                O->edge_selection[index]->Name = malloc(STRLEN * sizeof(char));
                memcpy(O->edge_selection[index]->Name, Sels_Names[2][SelsIndex[2]],
                       strlen(Sels_Names[2][SelsIndex[2]]));
                O->edge_selection[index]->Name[strlen(Sels_Names[2][SelsIndex[2]])] = '\0';
            }

            int * selected_elements = malloc(O->edgecount * sizeof(int));
            int selected_elements_count = 0;

            for (e = 0; e < O->edgecount; e ++)
            {
                E = &O->edges[e / ARRAYSIZE][e % ARRAYSIZE];
                if (E->selected)
                {
                    selected_elements[selected_elements_count ++] = e;
                }
                else
                {
                    for (i_c = 0; i_c < O->edge_selection[index]->indices_count; i_c ++)
                    {
                        if (O->edge_selection[index]->indices[i_c] == e)
                        {
                            selected_elements[selected_elements_count ++] = e;
                            break;
                        }
                    }
                }
            }
            O->edge_selection[index]->indices = realloc(O->edge_selection[index]->indices, selected_elements_count * sizeof(int));
            O->edge_selection[index]->indices_count = selected_elements_count;
            printf("selected elements count %d\n", selected_elements_count);
            for (i = 0; i < selected_elements_count; i ++)
            {
                O->edge_selection[index]->indices[i] = selected_elements[i];
            }
            free(selected_elements);
        }
    }
    else if (current_sel_type == 3) // verts
    {
        int o, i, v, i_c, index;
        vertex * V;
        int condition;
        printf("Vertex selection type\n");
        object * O;

        for (o = 0; o < selected_object_count; o ++)
        {
            condition = 1;
            O = objects[selected_objects[o]];

            if (!O->selected_verts_count) continue;

            for (i = 0; i < O->vertex_selections; i ++)
            {
                if (strcmp(O->vertex_selection[i]->Name, Sels_Names[3][SelsIndex[3]]) == 0)
                {
                    index = i;
                    printf("Selection exists! Appending.\n");
                    condition = 0;
                    break;
                }
            }
            if (condition)
            {
                index = O->vertex_selections;
                O->vertex_selections ++;
                O->vertex_selection = realloc(O->vertex_selection, O->vertex_selections * sizeof(vert_selection*));
                O->vertex_selection[index] = malloc(sizeof(vert_selection));
                O->vertex_selection[index]->indices_count = 0;
                O->vertex_selection[index]->indices = malloc(0 * sizeof(int));
                O->vertex_selection[index]->weights = malloc(0 * sizeof(float));
                O->vertex_selection[index]->Object = O;
                O->vertex_selection[index]->Transformer = NULL;

                O->vertex_selection[index]->split[0].indices_count = 0;
                O->vertex_selection[index]->split[0].indices = malloc(0 * sizeof(int));
                O->vertex_selection[index]->split[0].weights = malloc(0 * sizeof(int));
                O->vertex_selection[index]->split[1].indices_count = 0;
                O->vertex_selection[index]->split[1].indices = malloc(0 * sizeof(int));
                O->vertex_selection[index]->split[1].weights = malloc(0 * sizeof(int));

                //O->vertex_selection[index]->index = index;
                O->vertex_selection[index]->Name = malloc(STRLEN * sizeof(char));
                memcpy(O->vertex_selection[index]->Name, Sels_Names[3][SelsIndex[3]],
                       strlen(Sels_Names[3][SelsIndex[3]]));
                O->vertex_selection[index]->Name[strlen(Sels_Names[3][SelsIndex[3]])] = '\0';
            }

            int * selected_elements = malloc(O->vertcount * sizeof(int));
            float * selected_weights = malloc(O->vertcount * sizeof(int));
            int selected_elements_count = 0;

            for (v = 0; v < O->vertcount; v ++)
            {
                V = &O->verts[v / ARRAYSIZE][v % ARRAYSIZE];
                if (V->selected)
                {
                    selected_weights[selected_elements_count] = 1.0;
                    selected_elements[selected_elements_count ++] = v;
                }
                else
                {
                    for (i_c = 0; i_c < O->vertex_selection[index]->indices_count; i_c ++)
                    {
                        if (O->vertex_selection[index]->indices[i_c] == v)
                        {
                            selected_weights[selected_elements_count] = O->vertex_selection[index]->weights[i_c];
                            selected_elements[selected_elements_count ++] = v;
                            break;
                        }
                    }
                }
            }
            O->vertex_selection[index]->indices = realloc(O->vertex_selection[index]->indices, selected_elements_count * sizeof(int));
            O->vertex_selection[index]->weights = realloc(O->vertex_selection[index]->weights, selected_elements_count * sizeof(int));
            O->vertex_selection[index]->indices_count = selected_elements_count;
            printf("selected elements count %d\n", selected_elements_count);
            if (selected_elements_count > 0)
            {
                for (i = 0; i < selected_elements_count; i ++)
                {
                    O->vertex_selection[index]->indices[i] = selected_elements[i];
                    O->vertex_selection[index]->weights[i] = selected_weights[i];
                }
                if (O->vertex_selection[index]->Transformer != NULL && O->vertex_selection[index]->Transformer->Deformer != NULL)
                {
                    normalize_Deformer_Selections(O->vertex_selection[index]->Transformer->Deformer);
                }
            }
            free(selected_elements);
            free(selected_weights);
        }
    }
    create_Selections_List(selected_objects, selected_object_count, current_sel_type);
}

void unassign_All()
{
    printf("Unassign All!\n");
    if (current_sel_type == 1) // polys
    {
        int o, i, s, p, index;

        object * O;
        polygon * P;
        selection * S;

        int condition;

        printf("Polygon selection type\n");

        for (o = 0; o < selected_object_count; o ++)
        {
            O = objects[selected_objects[o]];

            for (p = 0; p < O->polycount; p ++)
            {
                P = &O->polys[p / ARRAYSIZE][p % ARRAYSIZE];

                if (P->selected)
                {
                    for (i = 0; i < O->polygon_selections; i ++)
                    {
                        condition = 0;
                        S = O->polygon_selection[i];
                        for (s = 0; s < S->indices_count; s ++)
                        {
                            if (p == S->indices[s])
                            {
                                index = s;
                                condition = 1;
                                break;
                            }
                        }
                        if (condition)
                        {
                            S->indices_count --;
                            for (s = index; s < S->indices_count; s ++)
                            {
                                S->indices[s] = S->indices[s + 1];
                            }
                        }
                    }
                }
            }
            for (i = 0; i < O->polygon_selections; i ++)
            {
                S = O->polygon_selection[i];
                S->indices = realloc(S->indices, S->indices_count * sizeof(int));
            }
        }
    }
    else if (current_sel_type == 2) // edges
    {
        int o, i, s, e, index;

        object * O;
        edge * E;
        selection * S;

        int condition;

        printf("Edge selection type\n");

        for (o = 0; o < selected_object_count; o ++)
        {
            O = objects[selected_objects[o]];

            for (e = 0; e < O->edgecount; e ++)
            {
                E = &O->edges[e / ARRAYSIZE][e % ARRAYSIZE];

                if (E->selected)
                {
                    for (i = 0; i < O->edge_selections; i ++)
                    {
                        condition = 0;
                        S = O->edge_selection[i];
                        for (s = 0; s < S->indices_count; s ++)
                        {
                            if (e == S->indices[s])
                            {
                                index = s;
                                condition = 1;
                                break;
                            }
                        }
                        if (condition)
                        {
                            S->indices_count --;
                            for (s = index; s < S->indices_count; s ++)
                            {
                                S->indices[s] = S->indices[s + 1];
                            }
                        }
                    }
                }
            }
            for (i = 0; i < O->edge_selections; i ++)
            {
                S = O->edge_selection[i];
                S->indices = realloc(S->indices, S->indices_count * sizeof(int));
            }
        }
    }
    else if (current_sel_type == 3) // verts
    {
        int o, i, s, v, index;

        object * O;
        vertex * V;
        vert_selection * VS;

        int condition;

        printf("Vertex selection type\n");

        for (o = 0; o < selected_object_count; o ++)
        {
            O = objects[selected_objects[o]];

            for (v = 0; v < O->vertcount; v ++)
            {
                V = &O->verts[v / ARRAYSIZE][v % ARRAYSIZE];

                if (V->selected)
                {
                    for (i = 0; i < O->vertex_selections; i ++)
                    {
                        condition = 0;
                        VS = O->vertex_selection[i];
                        for (s = 0; s < VS->indices_count; s ++)
                        {
                            if (v == VS->indices[s])
                            {
                                index = s;
                                condition = 1;
                                break;
                            }
                        }
                        if (condition)
                        {
                            VS->indices_count --;
                            for (s = index; s < VS->indices_count; s ++)
                            {
                                VS->indices[s] = VS->indices[s + 1];
                                VS->weights[s] = VS->weights[s + 1];
                            }
                        }
                    }
                }
            }
            for (i = 0; i < O->vertex_selections; i ++)
            {
                VS = O->vertex_selection[i];
                VS->indices = realloc(VS->indices, VS->indices_count * sizeof(int));
                VS->weights = realloc(VS->weights, VS->indices_count * sizeof(float));
            }
        }
    }
    create_Selections_List(selected_objects, selected_object_count, current_sel_type);
}

void unassign_Selection_L()
{
    printf("Unassign selection!\n");
    set_Sels_H_Button(4);

    if (current_sel_type == 0) // objects
    {
        int o, i, idx, Condition;
        int * final_elements = malloc(object_selection[SelsIndex[current_sel_type]].indices_count * sizeof(int));
        int final_elements_count = 0;
        for (i = 0; i < object_selection[SelsIndex[current_sel_type]].indices_count; i ++)
        // scroll through current object selection indexes
        {
            idx = object_selection[SelsIndex[current_sel_type]].indices[i];
            Condition = 1;
            for (o = 0; o < selected_object_count; o ++)
            // scroll through selected objects
            {
                if (selected_objects[o] == idx)
                {
                    // object is in this list
                    printf("Unassign %s\n", object_selection[SelsIndex[current_sel_type]].Name);
                    Condition = 0;
                    break;
                }
            }
            if (Condition)
            {
                final_elements[final_elements_count ++] = idx;
            }
        }

        object_selection[SelsIndex[current_sel_type]].indices = realloc(object_selection[SelsIndex[current_sel_type]].indices, final_elements_count * sizeof(int));
        object_selection[SelsIndex[current_sel_type]].indices_count = final_elements_count;
        for (i = 0; i < final_elements_count; i ++)
        {
            object_selection[SelsIndex[current_sel_type]].indices[i] = final_elements[i];
        }
        free(final_elements);
    }
    else if (current_sel_type == 1) // polys
    {
        int o, i, s, p, i_c, index;
        polygon * P;
        int condition = 0;
        int Condition = 0;
        printf("Polygon selection type\n");
        object * O;

        for (o = 0; o < selected_object_count; o ++)
        {
            O = objects[selected_objects[o]];
            for (i = 0; i < O->polygon_selections; i ++)
            {
                if (strcmp(O->polygon_selection[i]->Name, Sels_Names[current_sel_type][SelsIndex[current_sel_type]]) == 0)
                {
                    index = i;
                    printf("Selection exists! Removing.\n");
                    condition = 1;
                    break;
                }
            }
            if (condition)
            {

                int * selected_elements = malloc(O->polycount * sizeof(int));
                int selected_elements_count = 0;

                for (p = 0; p < O->polycount; p ++)
                {
                    P = &O->polys[p / ARRAYSIZE][p % ARRAYSIZE];
                    if (P->selected)
                    {
                        selected_elements[selected_elements_count ++] = p;
                    }
                }

                int * final_elements = malloc(O->polygon_selection[index]->indices_count * sizeof(int));
                int final_elements_count = 0;

                for (i_c = 0; i_c < O->polygon_selection[index]->indices_count; i_c ++)
                {
                    Condition = 1;
                    for (s = 0; s < selected_elements_count; s ++)
                    {
                        if (O->polygon_selection[index]->indices[i_c] == selected_elements[s])
                        {
                            Condition = 0;
                            break;
                        }
                    }

                    if (Condition)
                    {
                        final_elements[final_elements_count ++] = O->polygon_selection[index]->indices[i_c];
                    }
                }

                O->polygon_selection[index]->indices = realloc(O->polygon_selection[index]->indices, final_elements_count * sizeof(int));
                O->polygon_selection[index]->indices_count = final_elements_count;
                printf("final elements count %d\n", final_elements_count);
                for (i = 0; i < final_elements_count; i ++)
                {
                    O->polygon_selection[index]->indices[i] = final_elements[i];
                }
                free(selected_elements);
                free(final_elements);
            }
        }
    }
    else if (current_sel_type == 2) // edges
    {
        int o, i, s, e, i_c, index;
        edge * E;
        int condition = 0;
        int Condition = 0;
        printf("Edge selection type\n");
        object * O;

        for (o = 0; o < selected_object_count; o ++)
        {
            O = objects[selected_objects[o]];
            for (i = 0; i < O->edge_selections; i ++)
            {
                if (strcmp(O->edge_selection[i]->Name, Sels_Names[current_sel_type][SelsIndex[current_sel_type]]) == 0)
                {
                    index = i;
                    printf("Selection exists! Removing.\n");
                    condition = 1;
                    break;
                }
            }
            if (condition)
            {

                int * selected_elements = malloc(O->edgecount * sizeof(int));
                int selected_elements_count = 0;

                for (e = 0; e < O->edgecount; e ++)
                {
                    E = &O->edges[e / ARRAYSIZE][e % ARRAYSIZE];
                    if (E->selected)
                    {
                        selected_elements[selected_elements_count ++] = e;
                    }
                }

                int * final_elements = malloc(O->edge_selection[index]->indices_count * sizeof(int));
                int final_elements_count = 0;

                for (i_c = 0; i_c < O->edge_selection[index]->indices_count; i_c ++)
                {
                    Condition = 1;
                    for (s = 0; s < selected_elements_count; s ++)
                    {
                        if (O->edge_selection[index]->indices[i_c] == selected_elements[s])
                        {
                            Condition = 0;
                            break;
                        }
                    }

                    if (Condition)
                    {
                        final_elements[final_elements_count ++] = O->edge_selection[index]->indices[i_c];
                    }
                }

                O->edge_selection[index]->indices = realloc(O->edge_selection[index]->indices, final_elements_count * sizeof(int));
                O->edge_selection[index]->indices_count = final_elements_count;
                printf("final elements count %d\n", final_elements_count);
                for (i = 0; i < final_elements_count; i ++)
                {
                    O->edge_selection[index]->indices[i] = final_elements[i];
                }
                free(selected_elements);
                free(final_elements);
            }
        }
    }
    else if (current_sel_type == 3) // verts
    {
        int o, i, s, v, i_c, index;
        vertex * V;
        int condition = 0;
        int Condition = 0;
        printf("Vertex selection type\n");
        object * O;

        for (o = 0; o < selected_object_count; o ++)
        {
            O = objects[selected_objects[o]];
            for (i = 0; i < O->vertex_selections; i ++)
            {
                if (strcmp(O->vertex_selection[i]->Name, Sels_Names[current_sel_type][SelsIndex[current_sel_type]]) == 0)
                {
                    index = i;
                    printf("Selection exists! Removing.\n");
                    condition = 1;
                    break;
                }
            }
            if (condition)
            {

                int * selected_elements = malloc(O->vertcount * sizeof(int));
                int selected_elements_count = 0;

                for (v = 0; v < O->vertcount; v ++)
                {
                    V = &O->verts[v / ARRAYSIZE][v % ARRAYSIZE];
                    if (V->selected)
                    {
                        selected_elements[selected_elements_count ++] = v;
                    }
                }

                int * final_elements = malloc(O->vertex_selection[index]->indices_count * sizeof(int));
                float * final_weights = malloc(O->vertex_selection[index]->indices_count * sizeof(float));
                int final_elements_count = 0;

                for (i_c = 0; i_c < O->vertex_selection[index]->indices_count; i_c ++)
                {
                    Condition = 1;
                    for (s = 0; s < selected_elements_count; s ++)
                    {
                        if (O->vertex_selection[index]->indices[i_c] == selected_elements[s])
                        {
                            Condition = 0;
                            break;
                        }
                    }

                    if (Condition)
                    {
                        final_weights[final_elements_count] = O->vertex_selection[index]->weights[i_c];
                        final_elements[final_elements_count ++] = O->vertex_selection[index]->indices[i_c];
                    }
                }

                O->vertex_selection[index]->indices = realloc(O->vertex_selection[index]->indices, final_elements_count * sizeof(int));
                O->vertex_selection[index]->weights = realloc(O->vertex_selection[index]->weights, final_elements_count * sizeof(float));
                O->vertex_selection[index]->indices_count = final_elements_count;
                printf("final elements count %d\n", final_elements_count);
                for (i = 0; i < final_elements_count; i ++)
                {
                    O->vertex_selection[index]->indices[i] = final_elements[i];
                    O->vertex_selection[index]->weights[i] = final_weights[i];
                }
                if (O->vertex_selection[index]->Transformer != NULL)
                {
                    normalize_Deformer_Selections(O->vertex_selection[index]->Transformer->Deformer);
                }
                free(selected_elements);
                free(final_elements);
                free(final_weights);
            }
        }
    }
    create_Selections_List(selected_objects, selected_object_count, current_sel_type);
}
