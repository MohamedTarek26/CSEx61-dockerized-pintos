#include <stdio.h>
struct int_list_elem{
    struct int_list_elem* prev;
    int index;
    int64_t start_time;
    int64_t sleep_time;
    struct int_list_elem* next;
};

struct int_list{
    struct int_list_elem* head;
    struct int_list_elem* tail;
    int size;
};

void insert_int_list_element_in_order (struct int_list* l, struct int_list_elem* elem);

int is_empty(struct int_list* l);

struct int_list_elem* search_backwards_for_finished_threads (struct int_list* l);

void remove_from_int_list(struct int_list* l, struct int_list_elem* elem);
