#include "stdlib.h"
#include "stdio.h"
#include "unistd.h"
#include "time.h"

typedef struct map_methods {
    void (*insert)(void*, int, void*);
    void* (*get)(void*, int);
    int (*remove)(void*, int);
    void (*forall)(void*, int (*predicate)(void*), void (*func)(void*));
    void (*foreach)(void*, void (*func)(void*));
} map_methods;

typedef struct tree_methods {
    //ordered by key, values in    
} tree_methods;

typedef struct hashmap_methods {
    
} hashmap_methods;

typedef struct tree {
    map_methods m;
    tree_methods t;
} tree;

typedef struct hashmap {
    map_methods m;
    hashmap_methods h;
} hashmap;

typedef struct map {
    map_methods m;
} map;

typedef struct node {
    int key;
    void* value;
    struct node* left;
    struct node* right;
    struct node* par;
} node;

typedef struct tree_private {
    map_methods m;
    tree_methods t;
    node* root;
} tree_private;

typedef struct hash_node {
    int key;
    void* value;
    struct hash_node* next;
} h_node;

typedef struct hashmap_private {
    map_methods m;
    hashmap_methods h;
    h_node** data;
    int size;
} hashmap_private;

void h_insert(void* mem, int key, void* value) {
    hashmap_private* hmap = (hashmap_private*)mem;
    int pos = key % hmap->size;
    h_node* current = hmap->data[pos];
    if (current == NULL) {
        h_node* new_node = (h_node*)malloc(sizeof(h_node));
        new_node->key = key;
        new_node->value = value;
        new_node->next = NULL;
        hmap->data[pos] = new_node;
    } else {
        while (1) {
            if (current->key == key) {
                current->value = value;
                return;
            }
            if (current->next == NULL) {
                break;   
            }
            current = current->next;
        }
        h_node* new_node = (h_node*)malloc(sizeof(h_node));
        new_node->key = key;
        new_node->value = value;
        new_node->next = NULL;

        current->next = new_node;
    }
}

void* h_get(void* mem, int key) {
    hashmap_private* hmap = (hashmap_private*)mem;
    int pos = key % hmap->size;
    h_node* current = hmap->data[pos];
    if (current == NULL) {
        return NULL;
    } else {
        do {
            if (current->key == key) {
                return current->value;
            }
            current = current->next;
        } while (current != NULL);
        return NULL;
    }
}

int h_remove(void* mem, int key) {
    hashmap_private* hmap = (hashmap_private*)mem;
    int pos = key % hmap->size;
    h_node* current = hmap->data[pos];
    int deleted = 0;
    h_node* prev = NULL;
    if (current == NULL) {
        return deleted;
    } else {
        do {
            if (current->key == key) {
                if (prev != NULL) {
                    prev->next = current->next;
                } else {
                    hmap->data[pos] = current->next;
                }
                h_node* next = current->next;
                free(current);
                current = next;
                deleted++;
            } else {
                prev = current;
                current = current->next;
            }
        } while(current != NULL);
    }
    return deleted;
}

void h_forall(void* mem, int (*predicate)(void*), void (*func)(void*)) {
    hashmap_private* hmap = (hashmap_private*)mem;
    for (int i = 0; i < hmap->size; i++) {
        h_node* current = hmap->data[i];
        while(current != NULL) {
            if ((*predicate)(current->value) == 0) (*func)(current->value);
        }
    }
}

void h_foreach(void* mem, void (*func)(void*)) {
    hashmap_private* hmap = (hashmap_private*)mem;
    for (int i = 0; i < hmap->size; i++) {
        h_node* current = hmap->data[i];
        while(current != NULL) {
            (*func)(current->value);
        }
    }
}

hashmap* create_hashmap(int size) {
    hashmap_private* hmap = (hashmap_private*)malloc(sizeof(hashmap_private));
    hmap->data = (h_node**)malloc(size*sizeof(h_node*));
    hmap->size = size;
    for (int i = 0; i < size; i++) {
        hmap->data[i] = NULL;
    }
    hmap->m.insert = h_insert;
    hmap->m.get = h_get;
    hmap->m.remove = h_remove;
    hmap->m.foreach = h_foreach;
    hmap->m.forall = h_forall;

    return (hashmap*)hmap;
}

void t_insert(void* mem, int key, void* value) {
    tree_private* t = (tree_private*)mem;
    node* current = t->root;
    node* new_node = (node*)malloc(sizeof(node));
    new_node->key = key;
    new_node->value = value;
    new_node->left = NULL;
    new_node->right = NULL;
    new_node->par = NULL;
    if (current == NULL) {
        t->root = new_node;
        return;
    }
    while(1) {
        if (key < current->key) {
            if (current->left == NULL) {
                current->left = new_node;
                new_node->par = current;
                return;
            }
            current = current->left;
        } else if (key > current->key) {
            if (current->right == NULL) {
                current->right = new_node;
                new_node->par = current;
                return;
            }
            current = current->right;
        } else {
            current->value = value;
            free(new_node);
            return;
        }
    }
}

void* t_get(void* mem, int key) {
    tree_private* t = (tree_private*)mem;
    node* current = t->root;
    if (current == NULL) {
        return NULL;
    }
    while(1) {
        if (key < current->key) {
            if (current->left == NULL) {
                return NULL;
            }
            current = current->left;
        } else if (key > current->key) {
            if (current->right == NULL) {
                return NULL;
            }
            current = current->right;
        } else {
            return current->value;
        }
    }
}

int t_remove(void* mem, int key) {
    tree_private* t = (tree_private*)mem;
    node* current = t->root;
    while (current != NULL) {
        if (key < current->key) {
            if (current->left == NULL) {
                return 0;
            }
            current = current->left;
            continue;
        } else if (key > current->key) {
            if (current->right == NULL) {
                return 0;
            }
            current = current->right;
            continue;
        } else {
            if (current->left == NULL && current->right == NULL) {
                if (current == t->root) {
                    free(t->root);
                    t->root = NULL;
                    return 1;
                }
                if (current->par->left == current) {
                    current->par->left = NULL;
                } else {
                    current->par->right = NULL;
                }
                free(current);
                return 1;
            }

            if ((current->left == NULL) | (current->right == NULL)) {
                if (current->left != NULL) {
                    current->left->par = current->par;
                    if (current->par != NULL) {
                        if (current->par->left == current) {
                            current->par->left = current->left;
                        } else {
                            current->par->right = current->left;
                        }
                    } else {
                        t->root = current->left;
                    }
                    free(current);
                    return 1;
                } else {
                    current->right->par = current->par;
                    if (current->par != NULL) {
                        if (current->par->left == current) {
                            current->par->left = current->right;
                        } else {
                            current->par->right = current->right;
                        }
                    } else {
                        t->root = current->right;
                    }
                    free(current);
                    return 1;
                }
            }

            if (current->left != NULL && current->right != NULL) {
                node* successor = current->right;
                while(successor->left != NULL) successor = successor->left;
                current->value = successor->value;
                int key_remembered = successor->key;
                t_remove(mem, successor->key);
                current->key = key_remembered;
                return 1;
            }

        }
    }
    return 0;
}

tree* create_treemap() {
    tree_private* tmap = (tree_private*)malloc(sizeof(tree_private));
    tmap->m.insert = t_insert;
    tmap->m.get = t_get;
    tmap->m.remove = t_remove;
    return (tree*)tmap;
}
/*
void ttrace(node* n, int depth) {
    for (int i = 0; i < depth; i++) {
        printf(" ");
    }
    if (n == NULL) {
        return;
    }
        printf("(%d, %d)\n", n->key, *((int*)n->value));
            if (n->left != NULL) ttrace(n->left, depth+1);
    if (n->right != NULL) ttrace(n->right, depth+1);
}
*/

/*
TEST(equality,test1) {
    map* hmap = (map*)create_hashmap(5);
    map* tmap = (map*)create_treemap();
    int data[10];
    srand(time(NULL));
    int key[10];
    for (int i = 0; i < 10; i++) {
        data[i] = rand() % 100;
        key[i] = rand() % 10;
    }
    for (int i = 0; i < 10; i++) {
        hmap->m.insert(hmap, key[i], &data[i]);
        tmap->m.insert(tmap, key[i], &data[i]);
    }
    for (int i = 0; i < 10; i++) {
        int* hgiven = (int*)(hmap->m.get(hmap, i));
        int* tgiven = (int*)(tmap->m.get(tmap, i));
        if (hgiven != NULL && tgiven != NULL) {
            ASSERT_EQ(*hgiven, *tgiven);
        }
    }

} 

int main(int argc, char** argv) {
    
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();

    return 0;
    map* m = (map*)create_treemap();
    int data[10];
    srand(time(NULL));
    int key[10];
    for (int i = 0; i < 10; i++) {
        data[i] = rand() % 100;
        key[i] = rand() % 10;
        printf("(%d, %d) ",key[i], data[i]);
    }
    printf("\n"); 
    for (int i = 0; i < 10; i++) {
        m->m.insert(m, key[i], &data[i]);
    }
    for (int i = 0; i < 10; i++) {
        int* given = (int*)(m->m.get(m, i));
        if (given != NULL) {
            printf("%d ", *given);
        } else {
            printf("null ");
        }
    }
    printf("\n");
    ttrace(((tree_private*)m)->root, 0);
    for (int i = 0; i < 10; i++) {
        if (m->m.get(m, key[i]) != NULL) {
            printf("\n");
            printf("del: %d \n", key[i]);
            m->m.remove(m, key[i]);
            ttrace(((tree_private*)m)->root, 0);
        } 
    }
    
    
    printf("\n");
    for (int i = 0; i < 5; i++) {
        m->m.remove(m, (i*13*19)%10);
        printf("%d ", (i*13*19)%10);
    }
    printf("\n");

    for (int i = 0; i < 10; i++) {
        int* given = (int*)(m->m.get(m, i));
        if (given != NULL) {
            printf("(%d, %d) ",i, *given);
        } else {
            printf("null ");
        }
    }
    printf("\n");
}
*/
