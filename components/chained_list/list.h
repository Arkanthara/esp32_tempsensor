#ifndef __LIST_H__
#define __LIST_H__

typedef struct Item Item;

typedef struct Data {
	char * ssid;
	int ssid_len;

	char * password;
	int password_len;

	int priority;
} Data;

typedef struct Item {
	Data * data;
	Item * next;
} Item;

typedef struct Head {
	Item * head;
} Head;



Head * list_init(void);
void list_add(Head * head, char * ssid, char * password, int priority);
void list_print(Head * head);
Item * list_find(Head * head, char * ssid);
void list_delete_specific_item(Head * head, char * ssid);
void list_destroy(Head * head);

void item_print(Item * item);

#endif
