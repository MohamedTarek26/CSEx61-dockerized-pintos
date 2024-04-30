#include "int_list.h"
#include <stdio.h>
void insert_int_list_element_in_order (struct int_list* l, struct int_list_elem* elem){
    if (is_empty(l)){
        l->head->next = elem;
        elem->prev = l->head;
        elem->next = l->tail;
        l->tail->prev = elem;
    }else{
        struct int_list_elem *temp = l->head->next;
        while (temp != l->tail && temp->sleep_time > elem->sleep_time){
            temp = temp->next;
        }
        if (temp == l->tail){
            elem->prev = l->tail->prev;
            elem->next = l->tail;
            l->tail->prev = elem;
        }else{
            elem->prev = temp->prev;
            elem->next = temp;
            temp->prev = elem;
        }
    }
}

struct int_list_elem* search_backwards_for_finished_threads (struct int_list* l){
    struct int_list_elem* temp = l->tail->prev;
    while(temp != l->head){
        int64_t now_time = timer_ticks();
        if (now_time - temp->start_time >= temp->sleep_time)
            return temp;
        temp = temp->next;
    }    
    return NULL;
    

};

void remove_from_int_list(struct int_list* l, struct int_list_elem* elem){
    struct int_list_elem* temp = l->tail->prev;
    while(temp != elem)
        temp = temp->next;
    temp->prev->next = temp->next;
    temp->next->prev = temp->prev;
    temp->next = NULL;
    temp->prev = NULL;
}

int is_empty(struct int_list* l){
    if (l->size == 0) return 1;
    else return 0;
}