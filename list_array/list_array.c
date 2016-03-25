#include "unistd.h"
#include "stdlib.h"
#include "stdio.h"

typedef struct box_methods {
    int (*insert)(void*, void*);
    int (*remove)(void*, void*);
    void* (*begin)(void*);
    void* (*end)(void*);
    void* (*next)(void*);
    void* (*prev)(void*);
    void* (*delete)(void*);
} box_methods;

typedef struct list_methods {
    void* (*reverse)(void*);
} list_methods;

typedef struct vec_methods {
    void* (*sort)(void*);
    void* (*get)(void*, int);
} vec_methods;

typedef struct list {
    box_methods m;
    list_methods l;
} list;

typedef struct vec {
    box_methods m;
    vec_methods v;
} vec;

typedef struct list_elem {
    void* data;
    struct list_elem* prev;
    struct list_elem* next;
} list_elem;

typedef struct vec_elem {
    void* value;
    int num;
    void* vec;
} vec_elem;

typedef struct iter {
    void* value;
} iter;

typedef struct box {
    box_methods m;
} box;

typedef struct list_private {
    box_methods m;
    list_methods l;
    list_elem* begin;
    list_elem* end;
} list_private;

typedef struct vec_private {
    box_methods m;
    vec_methods v;
    void** data;
    int pointer;
} vec_private;

int list_insert(void* mem, void* object) {
    list_private* a = (list_private*)mem;
    list_elem* new_elem = (list_elem*)malloc(sizeof(list_elem));
    new_elem->data = object;
    if (a->begin == NULL) {
        a->begin = new_elem;
        a->end = new_elem;
        new_elem->prev = NULL;
        new_elem->next = NULL;
    } else {
        a->end->next = new_elem;
        new_elem->next = NULL;
        new_elem->prev = a->end;
        a->end = new_elem;
    }
    return 0;
}

int list_remove(void* mem, void* object) {
    list_private* a = (list_private*)mem;
    int result = 0;
    for (list_elem* i = a-> begin; i != NULL; i=i->next) {
        if (i->data == object) {
            i->prev->next = i->next;
            i->next->prev = i->prev;
            free(i);
            result++;
        }
    }
    return result;
}

void* list_begin(void* mem) {
    list_private* a = (list_private*)mem;
    list_elem* iter = (list_elem*)malloc(sizeof(list_elem));
    iter->prev = a->begin->prev;
    iter->next = a->begin->next;
    iter->data = a->begin->data;
    return iter;
}

void* list_end(void* mem) {
    list_private* a = (list_private*)mem;
    list_elem* iter = (list_elem*)malloc(sizeof(list_elem));
    iter->prev = a->end->prev;
    iter->next = a->end->next;
    iter->data = a->end->data;
    return iter;
}

void list_delete(void* mem) {
    list_private* a = (list_private*)mem;
    list_elem* i = a->begin;
    while (i != NULL) {
        void* trash = i;
        i = i->next;
        free(trash);
    }
    free(a);
}   

void* list_prev(void* mem) {
    list_elem* a = (list_elem*)mem;
    return a->prev;
}

void* list_next(void* mem) {
    list_elem* a = (list_elem*)mem;
    return a->next;
}

void* new_list() {
    list_private* a = (list_private*)malloc(sizeof(list_private));
    a->m.insert = list_insert;
    a->m.remove = list_remove;
    a->m.begin = list_begin;
    a->m.end = list_end;
    a->m.prev = list_prev;
    a->m.next = list_next;
    a->begin = NULL;
    a->end = NULL;
    return a;
}

int vec_insert(void* mem, void* object) {
    vec_private* a = (vec_private*)mem;
    if (a->data[a->pointer] == NULL) {
        return -1;
    }
    a->data[a->pointer] = object;
    a->pointer++;
    return 0;
}

int vec_remove(void* mem, void* object) {
    vec_private* a = (vec_private*)mem;
    int counter = 0;
    for (int i = 1; i < a->pointer; i++) {
        if (a->data[i] == object) {
            a->data[i] = NULL;
            counter++;
        }
    }
    return counter;
}

void* vec_begin(void* mem) {
    vec_private* a = (vec_private*)mem;
    if (a->pointer == 1) {
        return NULL;
    }
    vec_elem* iter = (vec_elem*)malloc(sizeof(vec_elem));
    iter->num = 1;
    iter->value = a->data[1];
    iter->vec = mem;
    return iter;
}

void* vec_end(void* mem) {
    vec_private* a = (vec_private*)mem;
    if (a->pointer == 1) {
        return NULL;
    }
    vec_elem* iter = (vec_elem*)malloc(sizeof(vec_elem));
    iter->num = a->pointer-1;
    iter->value = a->data[iter->num];
    iter->vec = mem;
    return iter;
}

void* vec_prev(void* mem) {
    vec_elem* iter = (vec_elem*)mem;
    if (iter->num == 1) {
        return NULL;
    }
    iter->num--;
    iter->value = ((vec_private*)iter->vec)->data[iter->num];
    return iter;
}

void* vec_next(void* mem) {
    vec_elem* iter = (vec_elem*)mem;
    vec_private* a = (vec_private*)iter->vec;
    if (iter->num == a->pointer - 1) {
        return NULL;
    }
    iter->num++;
    iter->value = a->data[iter->num];
    return iter;
}
 
void* vec_get(void* mem, int num) {
    vec_private* v = (vec_private*)mem;
    return v->data[num+1];
}

void* new_vec(int len) {
    vec_private* a = (vec_private*)malloc(sizeof(vec_private));
    a->data = (void**)malloc((len+2)*sizeof(void*));
    a->pointer = 1;
    a->data[0] = NULL;
    a->data[len+1] = NULL;
    for (int i = 1; i <= len; i++) {
        a->data[i] = a;
    }
    a->m.insert = vec_insert;
    a->m.remove = vec_remove;
    a->m.begin = vec_begin;
    a->m.end = vec_end;
    a->m.prev = vec_prev;
    a->m.next = vec_next;
    a->v.get = vec_get;
    return a;
}

int main() {
    int test_size = 100;
    vec* a = new_vec(test_size);
    int* data = (int*)malloc(test_size*sizeof(int));
    for (int i = 0; i < test_size; i++) {
        data[i] = i;
        a->m.insert(a, &data[i]);
    }
    
    for (iter* i = a->m.begin(a); i != NULL; i = a->m.next(i)) {
        printf("%d ", *((int*)(i->value)));
    }

    printf("\n %d \n", *(int*)a->v.get(a, 50));
    return 0;
}


