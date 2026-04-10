#include <iostream>
#include <chrono>
#include <new>
#include <cstddef>
#include <vector>

#define NOMINMAX
#include <windows.h>
#include <psapi.h>


// 固定容量对象池：一次性申请 N 个块，之后通过空闲链表进行 O(1) 分配与回收。

template<typename T>
class MemoryPool {
private:
    // 每个 Chunk 不是“空闲节点”(next)就是“对象存储区”(data)。
    // 使用 union 让这两种用途复用同一块内存。
    union Chunk {
        Chunk* next;                              // 空闲时：指向下一个空闲块
        alignas(T) unsigned char data[sizeof(T)]; // 使用时：存放一个 T 对象
    };

    Chunk* m_pool;               // 连续内存池起始地址
    Chunk* m_freeList;           // 空闲链表头节点
    std::size_t m_allCapacity;   // 总块数（池总容量）
    std::size_t m_freeCapacity;  // 当前剩余空闲块数

public:
    // 构造函数：预先申请 blockCount 个块并串成空闲链表。
    explicit MemoryPool(std::size_t blockCount)
        : m_pool(nullptr),
        m_freeList(nullptr),
        m_allCapacity(blockCount),
        m_freeCapacity(blockCount)
    {
        if (blockCount == 0) return;

        m_pool = new Chunk[m_allCapacity];
        m_freeList = m_pool;

        // 初始化单向空闲链表：pool[0] -> pool[1] -> ... -> nullptr
        for (std::size_t i = 0; i + 1 < m_allCapacity; ++i) {
            m_pool[i].next = &m_pool[i + 1];
        }
        m_pool[m_allCapacity - 1].next = nullptr;
    }

    // 析构函数：释放整块池内存。
    // 注意：示例中对象都在 benchmark 中显式回收，避免悬挂对象。
    ~MemoryPool() { delete[] m_pool; }

    // 对象池不可拷贝，避免多个实例管理同一内存。
    MemoryPool(const MemoryPool&) = delete;
    MemoryPool& operator=(const MemoryPool&) = delete;

    // 从空闲链表取出一个块并原地构造 T。
    // 返回值：成功返回对象指针；池空时返回 nullptr。
    T* allocate() {
        if (!m_freeList || m_freeCapacity == 0) return nullptr;

        --m_freeCapacity;
        Chunk* chunk = m_freeList;
        m_freeList = m_freeList->next;

        // 在预分配内存上原地构造对象，避免再次向系统申请内存。
        return new (chunk->data) T();
    }

    // 销毁对象并将其对应块归还到空闲链表头。
    // 参数 ptr 必须来自本对象池的 allocate()。
    void deallocate(T* ptr) {
        if (!ptr) return;

        // 手动调用析构，然后把这块内存挂回空闲链表。
        ptr->~T();
        Chunk* chunk = reinterpret_cast<Chunk*>(ptr);

        ++m_freeCapacity;
        chunk->next = m_freeList;
        m_freeList = chunk;
    }
};


// 获取进程峰值工作集（单位：字节）。
// 工作集可近似理解为进程曾占用过的物理内存峰值。
static std::size_t getPeakWorkingSetBytes() {
    PROCESS_MEMORY_COUNTERS pmc{};
    if (GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc))) {
        return static_cast<std::size_t>(pmc.PeakWorkingSetSize);
    }
    return 0;
}


// 两类游戏对象：用于模拟频繁创建/销毁场景。
struct Bullet {
    float x, y;   // 位置
    float vx, vy; // 速度
    int damage;   // 伤害值

    // 默认构造：子弹初始向上移动（vy=10）。
    Bullet() : x(0), y(0), vx(0), vy(10), damage(50) {}

    // 每帧更新位置。
    void update() { x += vx; y += vy; }
};

struct Enemy {
    float x, y; // 位置
    float hp;   // 当前生命值
    int type;   // 敌人类型 ID

    // 默认构造：满血普通敌人。
    Enemy() : x(0), y(0), hp(100), type(1) {}

    // 受到伤害，生命值减少 d。
    void hit(float d) { hp -= d; }
};

// 防止编译器优化掉
volatile int g_sink = 0;


// 模拟参数：控制每帧创建/销毁数量与总帧数。
// 设计：循环 frames 次，每帧先生成对象，再销毁对象。
struct SimConfig {
    int frames = 100;          // 总模拟帧数
    int spawnBullets = 100;    // 每帧生成子弹数量
    int spawnEnemies = 10;     // 每帧生成敌人数量
    int destroyBullets = 100;  // 每帧销毁子弹数量
    int destroyEnemies = 10;   // 每帧销毁敌人数量
};


// 基准1：直接使用 new/delete。
// 用途：作为对象池方案的对照组，观察时间与内存峰值。
static void benchmarkNormal(const SimConfig& cfg) {
    // 记录计时与峰值内存（注意峰值是“到当前时刻为止”的历史最大值）。
    std::size_t memBefore = getPeakWorkingSetBytes();
    auto start = std::chrono::high_resolution_clock::now();

    std::vector<Bullet*> bullets;
    std::vector<Enemy*> enemies;
    // 预留容量，避免 vector 扩容影响基准结果。
    bullets.reserve(static_cast<size_t>(cfg.frames) * cfg.spawnBullets);
    enemies.reserve(static_cast<size_t>(cfg.frames) * cfg.spawnEnemies);

    for (int f = 0; f < cfg.frames; ++f) {
        // 生成
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

        // 销毁：按 LIFO 回收，模拟对象生命周期结束。
        for (int i = 0; i < cfg.destroyBullets && !bullets.empty(); ++i) {
            delete bullets.back();
            bullets.pop_back();
        }
        for (int i = 0; i < cfg.destroyEnemies && !enemies.empty(); ++i) {
            delete enemies.back();
            enemies.pop_back();
        }
    }

    // 清理剩余
    for (auto* b : bullets) delete b;
    for (auto* e : enemies) delete e;

    auto end = std::chrono::high_resolution_clock::now();
    auto us = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
    std::size_t memAfter = getPeakWorkingSetBytes();

    std::cout << "Normal(Bullet+Enemy): " << us << " us"
        << " | PeakWorkingSet: " << memAfter << " bytes"
        << " (before " << memBefore << ")\n";
}


// 基准2：使用对象池（子弹池 + 敌人池）。
// 用途：减少频繁堆分配/释放带来的开销与碎片化风险。
static void benchmarkPool(const SimConfig& cfg) {
    // 这里取“总生成量”作为容量上限，简单但偏保守，保证不会因容量不足影响结果。
    const std::size_t bulletCap = static_cast<std::size_t>(cfg.frames) * cfg.spawnBullets;
    const std::size_t enemyCap = static_cast<std::size_t>(cfg.frames) * cfg.spawnEnemies;

    MemoryPool<Bullet> bulletPool(bulletCap);
    MemoryPool<Enemy>  enemyPool(enemyCap);

    std::size_t memBefore = getPeakWorkingSetBytes();
    auto start = std::chrono::high_resolution_clock::now();

    std::vector<Bullet*> bullets;
    std::vector<Enemy*> enemies;
    // 预留容量，避免容器扩容干扰对象池本身性能观察。
    bullets.reserve(bulletCap);
    enemies.reserve(enemyCap);

    for (int f = 0; f < cfg.frames; ++f) {
        // 生成
        for (int i = 0; i < cfg.spawnBullets; ++i) {
            Bullet* b = bulletPool.allocate();
            if (!b) break; // 容量不足保护
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

        // 销毁：对象回池，不触发系统级 free。
        for (int i = 0; i < cfg.destroyBullets && !bullets.empty(); ++i) {
            bulletPool.deallocate(bullets.back());
            bullets.pop_back();
        }
        for (int i = 0; i < cfg.destroyEnemies && !enemies.empty(); ++i) {
            enemyPool.deallocate(enemies.back());
            enemies.pop_back();
        }
    }

    // 清理剩余
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
    // 入口：依次运行对照组与对象池组，打印两轮结果。
    std::cout << "Benchmark: Bullet + Enemy spawn/destroy (Windows)\n";

    SimConfig cfg;
    cfg.frames = 100;
    cfg.spawnBullets = 100;
    cfg.spawnEnemies = 10;
    cfg.destroyBullets = 100;
    cfg.destroyEnemies = 10;

    // 热身：先跑一轮，减少首次运行带来的缓存/分配器冷启动干扰。
    benchmarkNormal(cfg);
    benchmarkPool(cfg);

    // 第二轮通常更稳定，可观察热启动后的对比。
    std::cout << "---- repeat ----\n";
    std::cout << "---- repeat111 ----\n";
    benchmarkNormal(cfg);
    benchmarkPool(cfg);

    return 0;
}