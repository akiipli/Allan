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

#define ITEMS 1000

#define ITEM_TYPE_OBJECT "object"
#define ITEM_TYPE_CAMERA "camera"
#define ITEM_TYPE_LIGHT  "light"

#define TYPE_OBJECT 1
#define TYPE_CAMERA 2
#define TYPE_LIGHT  3

void * Type;

char item_type[TYPE_LENGTH];
char * item_types[ITEM_NUM];
int item_types_c = 3;

void init_item_types()
{
    int i;

	for (i = 0; i < ITEM_NUM; i ++)
    {
        item_types[i] = malloc(TYPE_LENGTH * sizeof(char));
    }

    item_types[0] = ITEM_TYPE_OBJECT;
    item_types[1] = ITEM_TYPE_CAMERA;
    item_types[2] = ITEM_TYPE_LIGHT;

    memcpy(&item_type, item_types[0], strlen(item_types[0]));
}

int Items_c = 0;
char Item_Names[ITEMS][STRLEN];
int ItemIndex;
int item_start;

int Item_List[ITEMS];
int currentItem = 0;

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

int add_Item(int type, char * Name, void * pointer)
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

int query_items(const char * type)
{
    int s = 0;
    int i;
    for (i = 0; i < objectIndex; i++)
    {
        if (strcmp(type, ITEM_TYPE_OBJECT) == 0)
        {
            memcpy(Item_Names[s], objects[s]->Name, strlen(objects[s]->Name));
            Item_Names[s][strlen(objects[s]->Name)] = '\0';
            s++;
        }
    }
    return s;
}

void replace_Item_Name(char * EditString)
{
    item * I = items[Item_List[ItemIndex]];
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
            Item_List[Items_c] = Items_c;
            Items_c ++;
            if (Items_c >= ITEMS)
                break;
        }
    }
}

int list_items(char ** item_list, int start, int n, const char * type, int * selected, int * hidden, int currentObject)
{
    int s = start;
    int i = 0;

    object * O;

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

            if (O->selected && O->index != currentObject)
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

#endif // ITEMS_H_INCLUDED
