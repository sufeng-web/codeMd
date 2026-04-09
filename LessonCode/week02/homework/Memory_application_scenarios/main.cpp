#include <iostream>
#include <chrono>
#include <new>
#include <cstddef>
#include <vector>

#define NOMINMAX
#include <windows.h>
#include <psapi.h>


// MemoryPool

template<typename T>
class MemoryPool {
private:
    union Chunk {
        Chunk* next;
        alignas(T) unsigned char data[sizeof(T)];
    };

    Chunk* m_pool;
    Chunk* m_freeList;
    std::size_t m_allCapacity;
    std::size_t m_freeCapacity;

public:
    explicit MemoryPool(std::size_t blockCount)
        : m_pool(nullptr),
        m_freeList(nullptr),
        m_allCapacity(blockCount),
        m_freeCapacity(blockCount)
    {
        if (blockCount == 0) return;

        m_pool = new Chunk[m_allCapacity];
        m_freeList = m_pool;

        for (std::size_t i = 0; i + 1 < m_allCapacity; ++i) {
            m_pool[i].next = &m_pool[i + 1];
        }
        m_pool[m_allCapacity - 1].next = nullptr;
    }

    ~MemoryPool() { delete[] m_pool; }

    MemoryPool(const MemoryPool&) = delete;
    MemoryPool& operator=(const MemoryPool&) = delete;

    T* allocate() {
        if (!m_freeList || m_freeCapacity == 0) return nullptr;

        --m_freeCapacity;
        Chunk* chunk = m_freeList;
        m_freeList = m_freeList->next;

        return new (chunk->data) T();
    }

    void deallocate(T* ptr) {
        if (!ptr) return;

        ptr->~T();
        Chunk* chunk = reinterpret_cast<Chunk*>(ptr);

        ++m_freeCapacity;
        chunk->next = m_freeList;
        m_freeList = chunk;
    }
};


// Windows: Peak Working Set
static std::size_t getPeakWorkingSetBytes() {
    PROCESS_MEMORY_COUNTERS pmc{};
    if (GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc))) {
        return static_cast<std::size_t>(pmc.PeakWorkingSetSize);
    }
    return 0;
}


//       Ϸ    
struct Bullet {
    float x, y;
    float vx, vy;
    int damage;
    Bullet() : x(0), y(0), vx(0), vy(10), damage(50) {}
    void update() { x += vx; y += vy; }
};

struct Enemy {
    float x, y;
    float hp;
    int type;
    Enemy() : x(0), y(0), hp(100), type(1) {}
    void hit(float d) { hp -= d; }
};

//   ֹ       Ż   
volatile int g_sink = 0;


// ģ           ɺ     
//   ƣ ѭ   frames  Σ ÿ֡     spawn  ζ   Ȼ       destroy  ζ   
struct SimConfig {
    int frames = 100;          // ģ   100 ֡
    int spawnBullets = 100;    // ÿ֡     100  ӵ 
    int spawnEnemies = 10;     // ÿ֡     10     
    int destroyBullets = 100;  // ÿ֡     100  ӵ 
    int destroyEnemies = 10;   // ÿ֡     10     
};


// Normal: new/delete  Ż ǰ
static void benchmarkNormal(const SimConfig& cfg) {
    std::size_t memBefore = getPeakWorkingSetBytes();
    auto start = std::chrono::high_resolution_clock::now();

    std::vector<Bullet*> bullets;
    std::vector<Enemy*> enemies;
    bullets.reserve(static_cast<size_t>(cfg.frames) * cfg.spawnBullets);
    enemies.reserve(static_cast<size_t>(cfg.frames) * cfg.spawnEnemies);

    for (int f = 0; f < cfg.frames; ++f) {
        //     
        for (int i = 0; i < cfg.spawnBullets; ++i) {
            Bullet* b = new Bullet();
            b->update();
            g_sink ^= b->damage;
            bullets.push_back(b);
        }
        for (int i = 0; i < cfg.spawnEnemies; ++i) {
            Enemy* e = new Enemy();
            e->hit(1.0f);
            g_sink ^= e->type;
            enemies.push_back(e);
        }

        //     
        for (int i = 0; i < cfg.destroyBullets && !bullets.empty(); ++i) {
            delete bullets.back();
            bullets.pop_back();
        }
        for (int i = 0; i < cfg.destroyEnemies && !enemies.empty(); ++i) {
            delete enemies.back();
            enemies.pop_back();
        }
    }

    //     ʣ  
    for (auto* b : bullets) delete b;
    for (auto* e : enemies) delete e;

    auto end = std::chrono::high_resolution_clock::now();
    auto us = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
    std::size_t memAfter = getPeakWorkingSetBytes();

    std::cout << "Normal(Bullet+Enemy): " << us << " us"
        << " | PeakWorkingSet: " << memAfter << " bytes"
        << " (before " << memBefore << ")\n";
}


// Pool:       
static void benchmarkPool(const SimConfig& cfg) {
    //    㡰  ֵ ڳ          ڳ     
    // spawn==destroy  ÿ֡     ۻ     ô       Ե   ÿ֡  󲢷     
    const std::size_t bulletCap = static_cast<std::size_t>(cfg.frames) * cfg.spawnBullets;
    const std::size_t enemyCap = static_cast<std::size_t>(cfg.frames) * cfg.spawnEnemies;

    MemoryPool<Bullet> bulletPool(bulletCap);
    MemoryPool<Enemy>  enemyPool(enemyCap);

    std::size_t memBefore = getPeakWorkingSetBytes();
    auto start = std::chrono::high_resolution_clock::now();

    std::vector<Bullet*> bullets;
    std::vector<Enemy*> enemies;
    bullets.reserve(bulletCap);
    enemies.reserve(enemyCap);

    for (int f = 0; f < cfg.frames; ++f) {
        //     
        for (int i = 0; i < cfg.spawnBullets; ++i) {
            Bullet* b = bulletPool.allocate();
            if (!b) break; //        㱣  
            b->update();
            g_sink ^= b->damage;
            bullets.push_back(b);
        }
        for (int i = 0; i < cfg.spawnEnemies; ++i) {
            Enemy* e = enemyPool.allocate();
            if (!e) break;
            e->hit(1.0f);
            g_sink ^= e->type;
            enemies.push_back(e);
        }

        //     
        for (int i = 0; i < cfg.destroyBullets && !bullets.empty(); ++i) {
            bulletPool.deallocate(bullets.back());
            bullets.pop_back();
        }
        for (int i = 0; i < cfg.destroyEnemies && !enemies.empty(); ++i) {
            enemyPool.deallocate(enemies.back());
            enemies.pop_back();
        }
    }

    //     ʣ  
    for (auto* b : bullets) bulletPool.deallocate(b);
    for (auto* e : enemies) enemyPool.deallocate(e);

    auto end = std::chrono::high_resolution_clock::now();
    auto us = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
    std::size_t memAfter = getPeakWorkingSetBytes();

    std::cout << "Pool(Bullet+Enemy):   " << us << " us"
        << " | PeakWorkingSet: " << memAfter << " bytes"
        << " (before " << memBefore << ")\n";
}

int main() {
    std::cout << "Benchmark: Bullet + Enemy spawn/destroy (Windows)\n";

    SimConfig cfg;
    cfg.frames = 100;
    cfg.spawnBullets = 100;
    cfg.spawnEnemies = 10;
    cfg.destroyBullets = 100;
    cfg.destroyEnemies = 10;

    //     
    benchmarkNormal(cfg);
    benchmarkPool(cfg);

    std::cout << "---- repeat ----\n";
    benchmarkNormal(cfg);
    benchmarkPool(cfg);
	system("pause");
    return 0;
}