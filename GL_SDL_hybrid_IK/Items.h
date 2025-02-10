/*
The MIT License

Copyright <2020> <Allan Kiipli>

Items are of various types.
The purpose for items is to smooth out
process of development Item List.

When an item of specified type is
added to the scene, items list is updated.
When an object, .. is deleted, list needs
update.

Item list can be used to store objects
original name.

currentItem is intermediate for different
item type current object like currentObject
for objects.
*/

#ifndef ITEMS_H_INCLUDED
#define ITEMS_H_INCLUDED

typedef struct
{
    int index;
    unsigned address; // assigned after loading
    void * pointer;
    char * Name;
    int selected;
    int type;
}
item;

item * items[ITEMS];
int itemIndex = 0;

void init_items()
{
    ItemIndex = 0;
    item_start = 0;

    int i;

    for (i = 0; i < ITEMS; i ++)
    {
        items[i] = NULL;
    }
}

int add_Item(int type, char * Name, void * pointer) /* items mix different types, thus index does not reflect them */
{
    item * I = malloc(sizeof(item));
    if (I == NULL)
        return -1;
    items[itemIndex] = I;
    I->index = itemIndex;
    I->Name = malloc(STRLEN * sizeof(char));
    sprintf(I->Name, "%s", Name);
    I->pointer = pointer;
    I->selected = 0;
    I->type = type;
    itemIndex ++;
    return I->index;
}

int get_Item_Index(int Item_type, int currentItem)
{
    int i = 0;

    item * I;
    int counter = 0;

    for (i = 0; i < itemIndex; i ++)
    {
        I = items[i];

        if (I->type == Item_type)
        {
            if (counter == currentItem)
            {
                return i;
                break;
            }
            counter ++;
        }
    }

    return i;
}

void replace_Item_Name(char * EditString, item * I)
{
    memcpy(I->Name, EditString, strlen(EditString));
    I->Name[strlen(EditString)] = '\0';
}

void set_Item_H_Button(int index)
{
    int i;
    for (i = 0; i < H_ITEM_NUM; i ++)
    {
        Button_h_item[i].color = UI_GRAYB;
    }
    if (index > -1)
        Button_h_item[index].color = UI_GRAYD;
}

void create_Items_List(int type)
{
    int i;
    item * I;
    Items_c = 0;

    for (i = 0; i < itemIndex; i ++)
    {
        I = items[i];

        if (I->type == type)
        {
            memcpy(Item_Names[Items_c], I->Name, strlen(I->Name));
            Item_Names[Items_c][strlen(I->Name)] = '\0';
            //Item_Italic[Items_c] = I->selected;
            Item_List[Items_c] = i;
            Items_c ++;
            if (Items_c >= ITEMS)
                break;
        }
    }
}

int list_items(char ** item_list, int start, int n, const char * type, int * selected, int * hidden, int currentItem)
{
    int s = start;
    int i = 0;

    item * I = items[currentItem];
    object * O;
    camera * C;

    if (strcmp(type, ITEM_TYPE_OBJECT) == 0)
    {
        for (i = 0; i < n; i++)
        {
            if (i >= Items_c - start)
                break;

            O = objects[s];

            if (O->binding)
            {
                sprintf(item_list[i], "%s|%d", Item_Names[s], O->binding);
            }
            else
            {
                sprintf(item_list[i], "%s", Item_Names[s]);
            }

            if (O->selected && O != (object *)I->pointer)
                selected[i] = 1;
            else
                selected[i] = 0;
            if (loaded_objects[O->index] == 0)
                hidden[i] = 1;
            else
                hidden[i] = 0;
            s++;
        }
    }
    if (strcmp(type, ITEM_TYPE_CAMERA) == 0)
    {
        for (i = 0; i < n; i++)
        {
            if (i >= Items_c - start)
                break;

            C = cameras[s + CAMERAS];

            sprintf(item_list[i], "%s", Item_Names[s]);

            if (C->selected && C != (camera *)I->pointer)
                selected[i] = 1;
            else
                selected[i] = 0;

            hidden[i] = 2;
            s++;
        }
    }
    return i;
}

void add_objects_as_items(int obj_count)
{
    int o;
    object * O;

    for (o = objectIndex - obj_count; o < objectIndex; o ++)
    {
        O = objects[o];
        add_Item(TYPE_OBJECT, O->Name, O);
    }
}

void add_cameras_as_items(int cam_count)
{
    int c;
    camera * C;

    for (c = camIndex - cam_count; c < camIndex; c ++)
    {
        C = cameras[c];
        add_Item(TYPE_CAMERA, C->Name, C);
    }
}

void remove_Camera_From_Items(camera * C)
{
    int i, index;
    item * I;

    int condition = 0;

    for (i = 0; i < itemIndex; i ++)
    {
        I = items[i];

        if ((camera *)I->pointer == C)
        {
            index = i;
            condition = 1;
            break;
        }
    }

    if (condition)
    {
        itemIndex --;

        for (i = index; i < itemIndex; i ++)
        {
            items[i] = items[i + 1];
            items[i]->index = i;
        }
    }
}

void remove_Object_From_Items(object * O)
{
    int i, index;
    item * I;

    int condition = 0;

    for (i = 0; i < itemIndex; i ++)
    {
        I = items[i];

        if ((object *)I->pointer == O)
        {
            index = i;
            condition = 1;
            break;
        }
    }

    if (condition)
    {
        itemIndex --;

        for (i = index; i < itemIndex; i ++)
        {
            items[i] = items[i + 1];
            items[i]->index = i;
        }
    }
}

void clear_Items()
{
    int i;
    item * I;

    for (i = 1; i < itemIndex; i ++) /*CUBECOMMENT*/
    {
        I = items[i];
        free(I->Name);
        free(I);
    }
    itemIndex = 1;
}

void transfer_Item_Name_To_Object(item * I, object * O)
{
    sprintf(O->Name, "%s", I->Name);
}

void transfer_Item_Name_To_Cam(item * I, camera * C)
{
    sprintf(C->Name, "%s", I->Name);
}
#endif // ITEMS_H_INCLUDED
