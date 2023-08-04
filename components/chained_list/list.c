#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include "list.h"
//#include "esp_log.h"


// Function for initialize our chain list
Head * list_init(void)
{
	printf("Sizeof head: %d\n", sizeof(Head));
	Head * head = malloc(sizeof(Head));
	head->head = NULL;
	return head;
}

// Function for create new data
Data * create_data(char *ssid, char * password, int priority)
{
	// We verify that the sizes are respected
	if (strlen(ssid) > 32)
	{
		fprintf(stderr, "Error: ssid is too long because it must have 32 char and it has: %d char\n", strlen(ssid));
		return NULL;
	}
	if (strlen(password) > 64)
	{
		fprintf(stderr, "Error: password is too long because it must have 64 char and it has: %d char\n", strlen(password));
		return NULL;
	}

	// We create new data and allocate necessary space
	Data * data = malloc(sizeof(Data));

	// Add our priority number
	if (priority <= 10 && priority >= 0)
	{
		data->priority = priority;
	}
	else
	{
		fprintf(stderr, "Error: priority must be between 0 and 10 (10: higher priority)\n");
		return NULL;
	}

	data->ssid_len = strlen(ssid);
	data->password_len = strlen(password);

	// Allocution resources
	data->ssid = malloc(sizeof(char) * data->ssid_len);
	data->password = malloc(sizeof(char) * data->password_len);

	// Copy data in our structure
	strncpy(data->ssid, ssid, data->ssid_len);
	strncpy(data->password, password, data->password_len);

	// Add \0 at the end of chain
	data->ssid = realloc(data->ssid, (data->ssid_len + 1) * sizeof(char));
	data->ssid[data->ssid_len] = '\0';
	data->password = realloc(data->password, (data->password_len + 1) * sizeof(char));
	data->password[data->password_len] = '\0';

	return data;

}

/*
// Function for add data to our chain list. Data will be placed at the begining of the list
void list_add(Head * head, char * ssid, char * password)
{
	// Initialization of our data
	Data * data = create_data(ssid, password);
	if (data == NULL)
	{
		fprintf(stderr, "We don't add this to our chain list\n");
		return;
	}

	// Initialisation of our item
	Item * item = malloc(sizeof(Item));

	// We put our item at the begining of our chain list
	item->data = data;
	item->next = head->head;
	head->head = item;

}
*/

// Function for add data in list with priority order
void list_add(Head * head, char * ssid, char * password, int priority)
{
	// Initialization of our data
	Data * data = create_data(ssid, password, priority);
	if (data == NULL)
	{
		fprintf(stderr, "The item wouldn't be added to the list because he is empty");
		return;
	}

	// Initialization of our item
	Item * item = malloc(sizeof(Item));
	item->data = data;

	// If we have an empty list, we add item to the beginning of the list
	if (head->head == NULL)
	{
		item->next = head->head;
		head->head = item;
		return;
	}

	// Compare with the first item. Here, current = head->head...
	if (item->data->priority > head->head->data->priority)
	{
		printf("Coucou\n");
		item->next = head->head;
		head->head = item;
		return;
	}

	// Current item for browse our list
	Item * current = head->head;

	// Compare with the other items... We try to insert the item order by priority order (highest values are at the beginning of the list)
	while (current->next != NULL && data->priority  <= current->next->data->priority)
	{
		current = current->next;
	}

	// Insert
	item->next = current->next;
	current->next = item;
}


// Function for print item
void item_print(Item * item)
{
	// We test if item isn't null
	if (item == NULL)
	{
		// ESP_LOGE("Item", "Item is null");
		return;
	}

//	write(1, "SSID: ", 6);
//	write(1, item->data->ssid, item->data->ssid_len);
//	write(1, "\n", 2);
	printf("SSID: %s		priority: %d\n", item->data->ssid, item->data->priority);
}


// Function for print ssid
void list_print(Head * head)
{
	// Create a pointer for browse the chained list
	Item * item = head->head;

	// Go to the end of the list and display the ssid
	while (item != NULL)
	{
		item_print(item);
		item = item->next;
	}
}

// Function that return password for an ssid given
Item * list_find(Head * head, char * ssid)
{
	// Create a pointer for browse our element
	Item * item = head->head;

	// Search for our element by browsing our list
	while (item != NULL)
	{
		if (strncmp(item->data->ssid, ssid, item->data->ssid_len) == 0)
		{
			return item;
		}
		item = item->next;
	}

	printf("Error");

	fprintf(stderr, "We don't find the item of ssid: %s\n", ssid);

	// Return NULL if we don't find our element
	return NULL;
}


// Function for destroy an item
void item_destroy(Item * item)
{
	free(item->data->ssid);
	free(item->data->password);
	free(item->data);
	free(item);
}


// Function for destroy an item from the list
void list_delete_specific_item(Head * head, char * ssid)
{

	if (head->head != NULL && strcmp(head->head->data->ssid, ssid) == 0)
	{
		Item * delete = head->head;
		head->head = head->head->next;
		item_destroy(delete);
		return;
	}

	// Create a pointer for browse the list
	Item * item = head->head;

	// Browse our list
	while (item->next != NULL)
	{
		if (strcmp(item->next->data->ssid, ssid) == 0)
		{
			Item * delete = item->next;
			item->next = item->next->next;
			item_destroy(delete);
			return;
		}
		item = item->next;
	}

	// Print that we don't find the item with ssid like the ssid given
	fprintf(stderr, "We don't find the item with ssid: %s\n", ssid);
}


// Function for destroy a list
void list_destroy(Head * head)
{
	// We browse our list and destroy each item
	while (head->head != NULL)
	{
		Item * delete = head->head;
		head->head = head->head->next;
		item_destroy(delete);
	}

	// We destroy the head
	free(head);

	// We print that's ok
	printf("That's ok for the destruction of the list\n");
}




















