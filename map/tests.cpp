#include "map.c"
#include "stdlib.h"
#include <gtest/gtest.h>
 
 
TEST(HashMapTest, InsertGetBasicCorrectness) {
    int dataSize = 10;
    int hashBasketsNum = 5;
    int keyLim = 10;
    int dataLim = 100;

    map* hmap = (map*)create_hashmap(hashBasketsNum);
    int data[dataSize];
    srand(time(NULL));
    int key[dataSize];
    for (int i = 0; i < dataSize; i++) {
        data[i] = rand() % dataLim;
        key[i] = rand() % keyLim;
    }
    for (int i = 0; i < dataSize; i++) {
        hmap->m.insert(hmap, i, &data[i]);
    }
    for (int i = 0; i < dataSize; i++) {
        int* hgiven = (int*)(hmap->m.get(hmap, i));
        EXPECT_EQ(*hgiven, data[i]);
    }

} 

TEST(HashMapTest, InsertGetLotsOfData) {
    int dataSize = 20000;
    int hashBasketsNum = 20;
    int keyLim = 10;
    int dataLim = 100;

    map* hmap = (map*)create_hashmap(hashBasketsNum);
    int data[dataSize];
    srand(time(NULL));
    int key[dataSize];
    for (int i = 0; i < dataSize; i++) {
        data[i] = rand() % dataLim;
        key[i] = rand() % keyLim;
    }
    for (int i = 0; i < dataSize; i++) {
        hmap->m.insert(hmap, i, &data[i]);
    }
    for (int i = 0; i < dataSize; i++) {
        int* hgiven = (int*)(hmap->m.get(hmap, i));
        EXPECT_EQ(*hgiven, data[i]);
    }

}

TEST(HashMapTest, WorkWithDublicateKeys) {
    int dataSize = 5000;
    int hashBasketsNum = 5;
    int keyLim = 20;
    int dataLim = 100;

    map* hmap = (map*)create_hashmap(hashBasketsNum);
    int data[dataSize];
    srand(time(NULL));
    int key[dataSize];
    for (int i = 0; i < dataSize; i++) {
        data[i] = -1;
    }

    for (int i = 0; i < dataSize; i++) {
        int d = rand() % dataLim;
        int k = rand() % keyLim;
        data[k] = d;
        hmap->m.insert(hmap, k, &data[k]);
    }
    for (int i = 0; i < dataSize; i++) {
        if (data[i] != -1) {
            int* hgiven = (int*)(hmap->m.get(hmap, i));
            EXPECT_EQ(*hgiven, data[i]);
        }
    }

}



/*
TEST(MapTest, implementationEquility) {
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
*/

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

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
*/
/*
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
