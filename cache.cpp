#include <bits/stdc++.h>
using namespace std;
using namespace std::chrono;
//specifications
#define CACHE_SIZE 32768 
#define BLOCK_SIZE 64 
#define ASSOCIATIVITY 8 
#define ADDRESS_SIZE 40 
#define WORD_SIZE 4 
enum State {
    Invalid,Valid,MissPending 
};
class CacheBlock {
    public:
    State state; 
    int tag;
    vector<string> data;
    CacheBlock(){
        state=Invalid;
        tag=0;
        data.resize(BLOCK_SIZE / WORD_SIZE, "");
    } 
};
class CacheSet {
public:
    vector<CacheBlock> blocks;
    CacheSet(){
        blocks.resize(ASSOCIATIVITY);
    }
    int findTag(int tag) {
        for (int i = 0; i < ASSOCIATIVITY; ++i) {
            if (blocks[i].state == Valid && blocks[i].tag == tag) 
             return i;
        }
        return -1;
    }
    
    int getRandomEvictionIndex() {
        return rand() % ASSOCIATIVITY;
    }
};
class Cache {
public:
    int cacheSize;
    int blockSize; 
    int associativity;
    int numSets;          
    int indexBits;        
    int blockOffsetBits; 
    int tagBits;        
    int totalAccesses;
    int totalHits;
    vector<CacheSet> cacheSets;

    Cache() {
        cacheSize=CACHE_SIZE;
        blockSize=BLOCK_SIZE;
        associativity=ASSOCIATIVITY;
        numSets = cacheSize / (blockSize * associativity);
        blockOffsetBits = log2(blockSize);
        indexBits = log2(numSets);
        tagBits = ADDRESS_SIZE - blockOffsetBits - indexBits;
        cacheSets.resize(numSets);
        totalAccesses = 0;
        totalHits = 0;
    }
    void cache_details() {
    cout << "-------------------------------------------" << endl;
    cout << "       CACHE CONFIGURATION           " << endl;
    cout << "-------------------------------------------" << endl;
    cout << "Cache Type                : " << associativity << "-way Set-Associative Cache" << endl;
    cout << "Cache Size                : " << cacheSize / 1024 << " KB" << endl;
    cout << "Block Size                : " << blockSize << " Bytes" << endl;
    cout << "Associativity (No of Ways): " << associativity << " -ways" << endl;
    cout << "Number of Sets            : " << numSets << endl;
    cout << "Number of Index Bits      : " << indexBits << endl;
    cout << "Number of Block Offset Bits: " << blockOffsetBits << endl;
    cout << "Number of Tag Bits        : " << tagBits << endl;
    cout << "-------------------------------------------" << endl;
    }

    void cache_check_and_process(uint64_t address, char type, string writedata) {
        totalAccesses++;
        int offset = address & ((1 << blockOffsetBits) - 1); // for offset we need only last blockoffsetno of bits
        int index = (address >> blockOffsetBits) & ((1 << indexBits) - 1); //here we remove last offset bits and then check only index no of bits
        int tag = (address >> (blockOffsetBits + indexBits)) & ((1 << tagBits) - 1);
        int wordOffsetInBlock = offset / WORD_SIZE;
        CacheSet& cacheSet = cacheSets[index];
        int blockIndex = cacheSet.findTag(tag);
        if (blockIndex != -1 && cacheSet.blocks[blockIndex].state!=Invalid) {
            CacheBlock& block = cacheSet.blocks[blockIndex];
            if (block.state == Valid) {
                totalHits++;
                if (type == 'R') {
                    cout << "Read hit. Data: " << block.data[wordOffsetInBlock] << endl;
                } else if (type == 'W') {
                    block.data[wordOffsetInBlock] = writedata;
                    //send memory request to change in memory also from here
                    cout << "Write hit. Cache updated at index: " << blockIndex << " within the set "<< index <<".Updated cache." << endl;
                    // Write-through: also update the memory send memory request
                    // Simulate memory response
                    cout << "Write-through: Memory also updated with data: " << writedata << " at address " << address << endl;
                }
            } else if (block.state == MissPending) {
                cout << "Cache miss pending. Waiting for data." << endl;
            }
        } else { 
            if (type == 'R') {
            // Load data into cache (allocate on read miss)
            cout << "Cache miss. Loading data from memory." << endl;
            int evictionIndex = -1;
            vector<int> invalidIndices;
            for (int i = 0; i < ASSOCIATIVITY; ++i) {
                if (cacheSet.blocks[i].state == Invalid) {
                    invalidIndices.push_back(i);
                }
            }
            if (!invalidIndices.empty()) {
                evictionIndex = invalidIndices[rand() % invalidIndices.size()];
            } else {
                evictionIndex = cacheSet.getRandomEvictionIndex();
            }
            CacheBlock& evictBlock = cacheSet.blocks[evictionIndex];
            evictBlock.state = MissPending;
            evictBlock.tag = tag;
            evictBlock.data = vector<string>(BLOCK_SIZE/WORD_SIZE,"ReadLoadedData");
            cout << "Data loaded. Cache updated at index: " << evictionIndex << " within the set "<<index<<"." << endl;
            evictBlock.state = Valid;
            }
            else if (type == 'W') {
            // Write-no-allocate: bypass cache and directly write to memory send memory request
            cout << "Write miss. Write-no-allocate: Directly updating memory at address " << address << " with data: " << writedata << endl;
            // Simulate memory update after getting memory response
        }
        }
    }
    void printHitRate() {
    cout << "------------------------------------" << endl;
    cout << "       CACHE HIT RATE SUMMARY      " << endl;
    cout << "------------------------------------" << endl;
    cout << "Total Accesses    : " << totalAccesses << endl;
    cout << "Total Hits        : " << totalHits << endl;
    cout << "Total Misses      : " << (totalAccesses - totalHits) << endl;
    cout << "Cache Hit Rate    : " << (100.0 * totalHits / totalAccesses) << "%" << endl;
    cout << "Cache Miss Rate    : " << (100.0-(100.0 * totalHits / totalAccesses)) << "%" << endl;
    cout << "------------------------------------" << endl;
    }
};

uint64_t generate_address() {
    static random_device rd;
    static mt19937 generator(rd()); 
    uint64_t mean = 1ULL << 39; 
    double stddev = static_cast<double>(1ULL << 18)/17;
    normal_distribution<double> dist(static_cast<double>(mean), stddev);
    uint64_t address;
    do {
        address = static_cast<uint64_t>(round(dist(generator)));
        address &= ~3;
    } while (address >= (1ULL << 40));
    return address;
}

int main() {
    srand(time(NULL));
    Cache cache;
    cache.cache_details();
    auto start_time = high_resolution_clock::now();
    for (int i = 0; i < 1e8 ; i++) {
        uint64_t address = generate_address();
        //cout<<address<<endl;
        bitset<40> binaryAddress(address); 
        cout << "Generated Address: " << address << " (Binary: " << binaryAddress.to_string() << ")" << endl;
        char type = (i % 3 == 0) ? 'W' : 'R'; 
        cache.cache_check_and_process(address, type, "Sent_Data");
        auto end_time = high_resolution_clock::now();
        duration<double> time_duration = end_time - start_time;
        //cout << "Time Duration: " << time_duration.count() << " seconds" << endl;
        if (time_duration.count() >= 15.0) {
            cout << "Stopping after 15 seconds." << endl;
            break;
        }   
    }
    cache.printHitRate();
    return 0;
}