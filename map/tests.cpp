#include "map.c"
#include "stdlib.h"
#include <gtest/gtest.h>
 
 
TEST(HashMapTests, InsertGetBasicCorrectness) {
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
    }
    for (int i = 0; i < dataSize; i++) {
        hmap->m.insert(hmap, i, &data[i]);
    }
    for (int i = 0; i < dataSize; i++) {
        int* hgiven = (int*)(hmap->m.get(hmap, i));
        EXPECT_EQ(*hgiven, data[i]);
    }
    hmap->m.destroy(hmap);
}

void mul2(void* mem) {
    int* data = (int*)mem;
    *data = (*data)*2;
}

TEST(HashMapTests, ForEach) {
    int dataSize = 20000;
    int hashBasketsNum = 300;
    int keyLim = 15000;
    int dataLim = 100;

    map* hmap = (map*)create_hashmap(hashBasketsNum);
    int data[dataSize];
    int backup[dataSize];
    srand(time(NULL));
    int key[dataSize];
    for (int i = 0; i < dataSize; i++) {
        data[i] = rand() % dataLim;
        backup[i] = data[i];
    }
    for (int i = 0; i < dataSize; i++) {
        hmap->m.insert(hmap, i, &data[i]);
    }

    hmap->m.foreach(hmap, mul2);
    
    for (int i = 0; i < dataSize; i++) {
        int* hgiven = (int*)(hmap->m.get(hmap, i));
        EXPECT_EQ(*hgiven, backup[i]*2);
    }
    hmap->m.destroy(hmap);
}

int isEven(void* mem) {
    int a = *((int*)mem);
    if (a % 2 == 0) return 1;
    return 0;
}

TEST(HashMapTests, ForAll) {
    int dataSize = 20000;
    int hashBasketsNum = 300;
    int keyLim = 15000;
    int dataLim = 100;

    map* hmap = (map*)create_hashmap(hashBasketsNum);
    int data[dataSize];
    int backup[dataSize];
    srand(time(NULL));
    int key[dataSize];
    for (int i = 0; i < dataSize; i++) {
        data[i] = rand() % dataLim;
        backup[i] = data[i];
    }
    for (int i = 0; i < dataSize; i++) {
        hmap->m.insert(hmap, i, &data[i]);
    }

    hmap->m.forall(hmap, isEven, mul2);
    
    for (int i = 0; i < dataSize; i++) {
        int* hgiven = (int*)(hmap->m.get(hmap, i));
        int expected = backup[i];
        if (expected % 2 == 0) expected *= 2;
        EXPECT_EQ(*hgiven, expected);
    }
    hmap->m.destroy(hmap);
}

TEST(HashMapTests, InsertRemoveLotsOfData) {
    int dataSize = 20000;
    int hashBasketsNum = 100;
    int keyLim = 10;
    int dataLim = 100;

    map* hmap = (map*)create_hashmap(hashBasketsNum);
    int data[dataSize];
    srand(time(NULL));
    int key[dataSize];
    for (int i = 0; i < dataSize; i++) {
        data[i] = rand() % dataLim;
    }
    for (int i = 0; i < dataSize; i++) {
        hmap->m.insert(hmap, i, &data[i]);
    }
    for (int i = 0; i < dataSize; i++) {
        int* hgiven = (int*)(hmap->m.get(hmap, i));
        EXPECT_EQ(*hgiven, data[i]);
        EXPECT_EQ(hmap->m.remove(hmap, i), 1);
        EXPECT_EQ(hmap->m.remove(hmap, i), 0);
    }
    hmap->m.destroy(hmap);
}

TEST(TreeTests, ForEach) {
    int dataSize = 2000;
    int keyLim = 1500;
    int dataLim = 100;

    map* hmap = (map*)create_treemap();
    int data[dataSize];
    int backup[dataSize];
    srand(time(NULL));
    int key[dataSize];
    for (int i = 0; i < dataSize; i++) {
        data[i] = rand() % dataLim;
        backup[i] = data[i];
    }
    for (int i = 0; i < dataSize; i++) {
        hmap->m.insert(hmap, i, &data[i]);
    }

    hmap->m.foreach(hmap, mul2);
    for (int i = 0; i < dataSize; i++) {
        int* hgiven = (int*)(hmap->m.get(hmap, i));
        EXPECT_EQ(*hgiven, backup[i]*2);
    }
    hmap->m.destroy(hmap);
}

TEST(TreeTests, ForAll) {
    int dataSize = 2000;
    int keyLim = 1500;
    int dataLim = 100;

    map* hmap = (map*)create_treemap();
    int data[dataSize];
    int backup[dataSize];
    srand(time(NULL));
    int key[dataSize];
    for (int i = 0; i < dataSize; i++) {
        data[i] = rand() % dataLim;
        backup[i] = data[i];
    }
    for (int i = 0; i < dataSize; i++) {
        hmap->m.insert(hmap, i, &data[i]);
    }

    hmap->m.forall(hmap, isEven, mul2);
    
    for (int i = 0; i < dataSize; i++) {
        int* hgiven = (int*)(hmap->m.get(hmap, i));
        int expected = backup[i];
        if (expected % 2 == 0) expected *= 2;
        EXPECT_EQ(*hgiven, expected);
    }
}



TEST(TreeTests, InsertGetBasicCorrectness) {
    int dataSize = 30000;
    int keyLim = 15000;
    int dataLim = 100;

    map* hmap = (map*)create_treemap();
    int data[dataSize];
    srand(time(NULL));
    int key[dataSize];
    
    for (int i = 0; i < dataSize; i++) {
        data[i] = -1;
    }
    for (int i = 0; i < dataSize; i++) {
        int k = rand() % keyLim;
        data[k] = rand() % dataLim;
        hmap->m.insert(hmap, k, &data[k]);
    }

    for (int i = 0; i < dataSize; i++) {
        if (data[i] != -1) {
            int* hgiven = (int*)(hmap->m.get(hmap, i));
            if (hgiven == NULL) {
                ASSERT_EQ(0, 1);
            }
            EXPECT_EQ(*hgiven, data[i]);
        }
    }
} 

TEST(TreeTests, InsertRemoveLotsOfData) {
    int dataSize = 30000;
    int keyLim = 15000;
    int dataLim = 100;

    map* hmap = (map*)create_treemap();
    int data[dataSize];
    srand(time(NULL));
    int key[dataSize];
    
    for (int i = 0; i < dataSize; i++) {
        data[i] = -1;
    }
    for (int i = 0; i < dataSize; i++) {
        int k = rand() % keyLim;
        data[k] = rand() % dataLim;
        hmap->m.insert(hmap, k, &data[k]);
    }

    for (int i = 0; i < dataSize; i++) {
        if (data[i] != -1) {
            int* hgiven = (int*)(hmap->m.get(hmap, i));
            if (hgiven == NULL) {
                ASSERT_EQ(0, 1);
            }
            EXPECT_EQ(*hgiven, data[i]);
            EXPECT_EQ(hmap->m.remove(hmap, i), 1);
            EXPECT_EQ(hmap->m.remove(hmap, i), 0);
        }
    }
    hmap->m.destroy(hmap);
} 


TEST(GeneralTests, ImplementationEquility) {
    int dataSize = 4000;
    int keyLim = 1500;
    int dataLim = 100;
    int baskets_num = 100;

    map* hmap = (map*)create_hashmap(baskets_num);
    map* tmap = (map*)create_treemap();
    int data[dataSize];
    srand(time(NULL));
    int key[dataSize];
    for (int i = 0; i < dataSize; i++) {
        data[i] = -1;
    }
    for (int i = 0; i < dataSize; i++) {
        int k = rand() % keyLim;
        data[k] = rand() % dataLim;
        hmap->m.insert(hmap, k, &data[k]);
        tmap->m.insert(tmap, k, &data[k]);
    }
    for (int i = 0; i < dataSize; i++) {
        if (data[i] != -1) {
            int* hgiven = (int*)(hmap->m.get(hmap, i));
            int* tgiven = (int*)(tmap->m.get(tmap, i));
            EXPECT_EQ(*hgiven, *tgiven);
        }
    }
    tmap->m.destroy(tmap);
    hmap->m.destroy(hmap);
} 



int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

