#include "stdlib.h"
#include "stdio.h"
#include "unistd.h"
#include "time.h"

/*
    This small library contains implementation of "map" data structure. It has a common interface and two 
    implementations: based on has table and search binary tree. It was done as an academic project aimed
    to emulate polymorphism and incapsulation on C.

    Both implementations works according to the well-known algorithms (written in Wikipedia, for example), 
    so I won't describe the mechanics in comments - only several task-specific decisions.

    P.S. CT department guys, don't laugh a lot, I'd had to do this :)  
*/

/*
    Map interface.
*/
typedef struct map_methods {
    void (*insert)(void*, int, void*);          // insert pair (K, V) to map. If node with K is present, change it's value to V
    void* (*get)(void*, int);                   // get value (V) by key (K) from map
    int (*remove)(void*, int);                  // remove element by K from map. Return: 1 if element removed, 0 otherwise
    void (*destroy)(void*);                     // destructor for map. Because of missing kinda "this()" in C, needs passing itself as an argument
    void (*forall)(void*, int (*predicate)(void*), void (*func)(void*));    // inplace transform all V, satisying predicate, to func(V)
    void (*foreach)(void*, void (*func)(void*));                            // implace transform all V to func(V)
} map_methods;

/*
 * Kind of "trait" in Scala: additional interface to tree structure
 */
typedef struct tree_methods {
    //it's emmpty, because it's not important for the task :) 
} tree_methods;

/*
 * The same for hash map 
 */
typedef struct hashmap_methods {
    
} hashmap_methods;

/*
 * "Public" tree struct. Implements map and tree interfaces
 */
typedef struct tree {
    map_methods m;
    tree_methods t;
} tree;

/*
 * "Public" hashmap struct. Implements map and hashmap interfaces
 */
typedef struct hashmap {
    map_methods m;
    hashmap_methods h;
} hashmap;

/*
 * An "abstract class" - map. It has no own constructor, but it could be cast
 * to tree, hashmap and vica-versa 
 */
typedef struct map {
    map_methods m;
} map;

/*
 * Tree node class.
 */
typedef struct node {
    int key;
    void* value;
    struct node* left;
    struct node* right;
    struct node* par;
} node;

/*
 * "Private" part of tree class. Contains a "private" field - root - link to
 * first node in tree. May be NULL. 
 */
typedef struct tree_private {
    map_methods m;
    tree_methods t;
    node* root;
} tree_private;

/*
 * A node for hashmap baskets. 
 */
typedef struct hash_node {
    int key;
    void* value;
    struct hash_node* next;
} h_node;

/*
 * "Private" part of hashmap class. Contains two "private" fields: data and
 * size
 */
typedef struct hashmap_private {
    map_methods m;
    hashmap_methods h;
    h_node** data;  // array of baskets: linked lists for maintaining hash collisions.
    int size;       // num of baskets in hashmap (the same - size of data array).
} hashmap_private;

/*
 * Insert function for hashtable implementation of map.
 */
void h_insert(void* mem, int key, void* value) {
    hashmap_private* hmap = (hashmap_private*)mem;
    int pos = key % hmap->size;     //Super-Innovative-Smart hash-function - just a modulo by size of the table
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

//helping recursive function for hashmap destructor. Deletes a node and it's
//children
void h_free(h_node* n) {
    if (n->next != NULL) {
        h_free(n->next);
    }
    free(n);
}

/*
 * Hashmap destructor
 */
void h_destroy(void* mem) {
    hashmap_private* hmap = (hashmap_private*)mem;
    for (int i = 0; i < hmap->size; i++) {
        if (hmap->data[i] != NULL) h_free(hmap->data[i]);
    }
    free(mem);
}

/*
 * Hashmap get-by-key function. returns *value of key. NULL otherwise 
 */
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

/*
 * Remove value from map by key. 1 if removed, 0 otherwise
 */
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

/* 
 * Hashmap "ForAll" function. Apply func to all V satisfying predicate
 */
void h_forall(void* mem, int (*predicate)(void*), void (*func)(void*)) {
    hashmap_private* hmap = (hashmap_private*)mem;
    for (int i = 0; i < hmap->size; i++) {
        h_node* current = hmap->data[i];
        while(current != NULL) {
            if ((*predicate)(current->value) == 1) (*func)(current->value);
            current = current->next;
        }
    }
}

/*
 * Hashmap "ForEach" function. Apply func to all V.
 */
void h_foreach(void* mem, void (*func)(void*)) {
    hashmap_private* hmap = (hashmap_private*)mem;
    for (int i = 0; i < hmap->size; i++) {
        h_node* current = hmap->data[i];
        while(current != NULL) {
            (*func)(current->value);
            current = current->next;
        }
    }
}

/*
 * Hashmap constructor.
 * ARGS:
 *  size - number of baskets in hash table. The bigger it is, the faster it
 *  works.
 */

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
    hmap->m.destroy = h_destroy;
    hmap->m.foreach = h_foreach;
    hmap->m.forall = h_forall;

    return (hashmap*)hmap;
}

/*
 * Insert new (K, V) pair to the tree. If K is present in tree - change value
 * to V
 */
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

/*
 * Get value by key from tree
 */
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

/*
 *  Remove node by key from tree. 1 if removed, 0 otherwise.
 *  Written in non-recursive style because of 32K stack depth limit.
 */
int t_remove(void* mem, int key) {
    tree_private* t = (tree_private*)mem;
    node* current = t->root;
    while (current != NULL) {
        if (key < current->key) {
            current = current->left;
            continue;
        } else if (key > current->key) {
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

            if (current->left != NULL && current->right != NULL) {
                node* successor = current->right;
                while(successor->left != NULL) successor = successor->left;
                current->value = successor->value;
                int key_remembered = successor->key;
                t_remove(mem, successor->key);
                current->key = key_remembered;
                return 1;
            }

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
    }
    return 0;
}

/*
 * Helping recursive function for tree destructor. Destroys node and it's
 * children
 */
void t_free(node* n) {
    if (n->left != NULL) {
        t_free(n->left);
    }
    n->left = NULL;
    if (n->right != NULL) {
        t_free(n->right);
    }
    n->right = NULL;
    free(n);
}

/* Just helper for tree debugging. Displays tree structure in text
 * representation.
 */
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

/* Tree destructor */
void t_destroy(void* mem) {
    tree_private* t = (tree_private*)mem;
    if (t->root != NULL) {
        t_free(t->root);
    }
    free(mem);
}

/* Helper for tree "ForAll" and "ForEach" functions. Applies func to value,
 * satisfying predicate, and for children, recursively
 */
void dfs_apply(node* n, int (*predicate)(void*), void (*func)(void*)) {
    if (predicate(n->value) == 1) {
        func(n->value);
    }
    if (n->left != NULL) {
        dfs_apply(n->left, predicate, func);
    }
    if (n->right != NULL) {
        dfs_apply(n->right, predicate, func);
    }
}

/* Dummy predicate for "ForEach" tree function */
int alwaysTrue(void* mem) {
    return 1;
}

/* Tree "ForAll" function */
void t_forall(void* mem, int (*predicate)(void*), void (*func)(void*)) {
    tree_private* t = (tree_private*)mem;
    if (t->root != NULL) dfs_apply(t->root, predicate, func);
}

/* Tree "ForEach" function */
void t_foreach(void* mem, void (*func)(void*)) {
    tree_private* t = (tree_private*)mem;
    if (t->root != NULL) dfs_apply(t->root, alwaysTrue, func);
}

/* Tree constructor */
tree* create_treemap() {
    tree_private* tmap = (tree_private*)malloc(sizeof(tree_private));
    tmap->m.insert = t_insert;
    tmap->m.destroy = t_destroy;
    tmap->m.get = t_get;
    tmap->m.remove = t_remove;
    tmap->m.foreach = t_foreach;
    tmap->m.forall = t_forall;
    return (tree*)tmap;
}
