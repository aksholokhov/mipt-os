#include "map.c"
#include "stdlib.h"
#include <gtest/gtest.h>
 
/*
 * This is a bunch of tests for my map library implemented in "map.c". Also it
 * was done as an academic task for studying Google Test framework.
 *
 * NOTE: All tests are REALLY simular: same structure, same variables. So, I'll
 * comment it on in detalis just in the first time.
 */

/*
 * Check basic "put-get" correctness: compare given with inserted before
 */
TEST(HashMapTests, InsertGetBasicCorrectness) {
    int dataSize = 10; // Number of (key, value) pair aquiring from random (normal) distribution         
    int hashBasketsNum = 5; // Number of buskets in hashmap used in test
    int keyLim = 10;    // Upper bound of key value.
    int dataLim = 100;  //Upper bound of "value" value.

    // Create map for testing. It could be hash table or searching tree,
    // according to the interface.
    map* hmap = (map*)create_hashmap(hashBasketsNum);
    int data[dataSize]; // Create data storage. 
    srand(time(NULL));  // Prepare random generator
    int key[dataSize];  //
    
    // Fill in data array with random values 
    for (int i = 0; i < dataSize; i++) {
        data[i] = rand() % dataLim;
    }

    // Insert (K, V) = (i, data[i]) pairs in the tree. Note that in this case
    // all keys are different and ordered. It's not a common for all tests.
    for (int i = 0; i < dataSize; i++) {
        hmap->m.insert(hmap, i, &data[i]);
    }

    // Get V by K from the tree and compare to the data sored in data array.
    for (int i = 0; i < dataSize; i++) {
        int* hgiven = (int*)(hmap->m.get(hmap, i));
        EXPECT_EQ(*hgiven, data[i]);
    }

    // Destroy hashmap
    hmap->m.destroy(hmap);
}

/* Dummy func for "ForEach" and "ForAll" testing */
void mul2(void* mem) {
    int* data = (int*)mem;
    *data = (*data)*2;
}

/* Test for ForEach function in hash table implementation. Puts array of (K, V)
 * pairs to the map, multiplies everything by 2 applying the mul2 func, and
 * compares everything with backup.
 */ 
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

/*
 * Dummy predicate for ForAll testing
 */
int isEven(void* mem) {
    int a = *((int*)mem);
    if (a % 2 == 0) return 1;
    return 0;
}

/* Test for ForAll function in hash table implementation. Puts array of (K, V)
 * pairs to the map, multiplies all even values by 2 applying the mul2 func, and
 * compares everything with backup.
 */ 
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

/*
 * Just a large scale test for establishing general efficiency of hash map
 * implementation. Also check for removing correctness.
 */
TEST(HashMapTests, InsertRemoveLotsOfData) {
    int dataSize = 20000;
    int hashBasketsNum = 200;
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

/* =============================================================================
 *  NOTE: there are same several tests for tree implementation of map
 *  structure. Because of common interface, this code is just a copy-paste from
 *  above. I won't comment it just because it's literally the same code.
 * =============================================================================
 */

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

