#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MAX_CACHE_SIZE 100

typedef struct {
    char key;
    int ref;
} CacheItem;

typedef struct {
    CacheItem items[MAX_CACHE_SIZE];
    int size;
    int head;
    int compulsoryMisses;
    int capacityMisses;
} Cache;

Cache cache;
int policy;
bool everInCache[256] = { false };

void initCache(int size, int p) {
    cache.size = size;
    cache.head = 0;
    policy = p;
    for (int i = 0; i < MAX_CACHE_SIZE; ++i) {
        cache.items[i].key = '\0';
        cache.items[i].ref = 0;
    }
}

int findCacheItem(char key) {
    for (int i = 0; i < cache.size; ++i) {
        if (cache.items[i].key == key) {
            return i;
        }
    }
    return -1;
}

void evictAndAddItem(char key) {
    int arrayIndex = (int) key + 128;
    int index = findCacheItem(key);

    if (index == -1) {
        if (!everInCache[arrayIndex]) {
            everInCache[arrayIndex] = true;
            cache.compulsoryMisses++;
        } else {
            cache.capacityMisses++;
        }
    }

    if (policy == 0 || policy == 2) {
        cache.items[cache.head].key = key;
        cache.items[cache.head].ref = 0;
        cache.head = (cache.head + 1) % cache.size;
    } else if (policy == 1) {
        CacheItem temp = cache.items[0];
        for (int i = 0; i < cache.size - 1; ++i) {
            cache.items[i] = cache.items[i + 1];
        }
        temp.key = key;
        cache.items[cache.size - 1] = temp;
    }
}

void accessCache(char key) {
    int index = findCacheItem(key);

    if (index != -1) {
        printf("HIT\n");
        if (policy == 1) {
            CacheItem hitItem = cache.items[index];
            for (int i = index; i < cache.size - 1; ++i) {
                cache.items[i] = cache.items[i + 1];
            }
            cache.items[cache.size - 1] = hitItem;
        } else if (policy == 2) {
            cache.items[index].ref = 1;
        }
    } else {
        printf("MISS\n");
        if (policy == 2) {
            while (cache.items[cache.head].ref == 1) {
                cache.items[cache.head].ref = 0;
                cache.head = (cache.head + 1) % cache.size;
            }
        }
        evictAndAddItem(key);
    }
}

int main(int argc, char *argv[]) {
    int cacheSize = 3;
    char policyChar = 'F';

    if (argc < 2) {
        fprintf(stderr, "Usage: %s [-N size] <policy>\n", argv[0]);
        return 1;
    }

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-N") == 0 && i + 1 < argc) {
            cacheSize = atoi(argv[++i]);
        } else if (argv[i][0] == '-') {
            policyChar = argv[i][1];
        }
    }

    if (cacheSize <= 0 || cacheSize > MAX_CACHE_SIZE) {
        fprintf(stderr, "Invalid cache size.\n");
        return 1;
    }

    int policy;
    if (policyChar == 'F')
        policy = 0;
    else if (policyChar == 'L')
        policy = 1;
    else if (policyChar == 'C')
        policy = 2;
    else {
        fprintf(stderr, "Invalid cache policy.\n");
        return 1;
    }

    initCache(cacheSize, policy);
    cache.compulsoryMisses = 0;
    cache.capacityMisses = 0;

    char line[128];

    while (fgets(line, sizeof(line), stdin) != NULL) {
        line[strcspn(line, "\n")] = 0;

        char input = line[0];
        if (input) {
            accessCache(input);
        }
    }

    printf("%d %d\n", cache.compulsoryMisses, cache.capacityMisses);

    return 0;
}
