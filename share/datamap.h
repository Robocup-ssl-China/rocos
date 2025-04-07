#include <map>
#include <string>
#include <any>
#include <mutex>
#include <shared_mutex>
#include <optional>
#include <stdexcept>

class DataMap {
public:
    DataMap() = default;
    virtual ~DataMap() = default;
    // 插入或更新键值对
    void insert(const std::string& key, std::any value) {
        std::unique_lock lock(mutex_);
        map_[key] = std::move(value);
    }

    // 获取值的拷贝（若不存在返回 std::nullopt）
    std::optional<std::any> get(const std::string& key) {
        std::shared_lock lock(mutex_);
        auto it = map_.find(key);
        if (it != map_.end()) {
            return it->second;
        }
        return std::nullopt;
    }

    // 获取特定类型的值（若类型不匹配或不存在则抛出异常）
    template <typename T>
    T get_as(const std::string& key) const{
        std::shared_lock lock(mutex_);
        auto it = map_.find(key);
        if (it == map_.end()) {
            throw std::out_of_range("Key not found: " + key);
        }
        return std::any_cast<T>(it->second);
    }

    // 删除键值对
    bool erase(const std::string& key) {
        std::unique_lock lock(mutex_);
        return map_.erase(key) > 0;
    }

    // 检查键是否存在
    bool contains(const std::string& key) {
        std::shared_lock lock(mutex_);
        return map_.find(key) != map_.end();
    }

    // 清空所有键值对
    void clear() {
        std::unique_lock lock(mutex_);
        map_.clear();
    }

    // 重载 operator[] 用于写入（隐式插入）
    std::any& operator[](const std::string& key) {
        std::unique_lock lock(mutex_);
        return map_[key]; // 返回引用，但需注意外部访问时的线程安全性！
    }

    // 重载 const operator[] 用于读取（返回副本）
    std::any operator[](const std::string& key) const {
        std::shared_lock lock(mutex_);
        auto it = map_.find(key);
        if (it != map_.end()) {
            return it->second;
        }
        return std::any(); // 返回空 any（类似 std::map 行为）
    }

    DataMap(const DataMap& other){
        std::shared_lock<std::shared_mutex> src_lock(other.mutex_);
        map_ = other.map_;
    }
    DataMap& operator=(const DataMap& other){
        if (this != &other) {
            std::unique_lock<std::shared_mutex> dst_lock(mutex_, std::defer_lock);
            std::shared_lock<std::shared_mutex> src_lock(other.mutex_, std::defer_lock);
            std::lock(dst_lock, src_lock); // 锁定两个互斥量
            map_ = other.map_;
        }
        return *this;
    }
private:
    std::map<std::string, std::any> map_;
    mutable std::shared_mutex mutex_;
};