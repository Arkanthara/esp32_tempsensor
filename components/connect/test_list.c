#include <stdio.h>

#include "list.h"

#include <string.h>

void test(void)
{

	char * salut = "coucou";
	printf("size_salut: %d\n", strlen(salut));
	char * yo = "salut";
	strncpy(salut, yo, strlen(yo));
	printf("Result = %s\n", salut);
}

void main(void)
{
	Head * head = list_init();

	list_add(head, "Sin-hae", "Yoo", 6);
	list_add(head, "Ali", "Ababoi", 7);
	list_add(head, "Gargamel", "Schtroumpf", 3);
	list_add(head, "Brett", "Hollister", 7);
	list_add(head, "Gobelion", "doliert", 4);
	list_add(head, "Gargamel_2", "blabala", 3);
	list_print(head);
	printf("\n");
	list_delete_specific_item(head, "Coucou");
	list_print(head);
	printf("\n");
	list_delete_specific_item(head, "Gargamel_2");
	list_print(head);
	printf("\n");
	list_delete_specific_item(head, "Gobelion");
	list_print(head);
	list_destroy(head);

}
