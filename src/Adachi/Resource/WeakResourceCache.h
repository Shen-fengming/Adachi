#pragma once

#include "Adachi/Diagnostics/Debug.h"
#include <cstddef>
#include <functional>
#include <memory>
#include <type_traits>
#include <unordered_map>
#include <utility>

namespace Adachi {

template<
    typename Resource,
    typename Key,
    typename KeyHash = std::hash<Key>
>
class WeakResourceCache {
public:
    WeakResourceCache() = default;

    WeakResourceCache(const WeakResourceCache&) = delete;
    WeakResourceCache& operator=(const WeakResourceCache&) = delete;

    template<typename Factory>
    std::shared_ptr<Resource> GetOrCreate(
        const Key& key,
        Factory&& factory)
    {
        ADACHI_CORE_ASSERT(
            (std::is_invocable_r_v<
                std::shared_ptr<Resource>,
                Factory,
                const Key&
            >),  
            "Factory must accept const Key& and return std::shared_ptr<Resource>"
        );

        auto found = m_Resources.find(key);

        if (found != m_Resources.end()) {
            if (auto resource = found->second.lock()) {
                return resource;
            }

            // The resource has already been destroyed.
            // Remove its expired lookup entry.
            m_Resources.erase(found);
        }

        auto resource = std::invoke(
            std::forward<Factory>(factory),
            key
        );

        if (resource) {
            m_Resources.emplace(key, resource);
        }

        return resource;
    }

    std::shared_ptr<Resource> Find(const Key& key) const
    {
        const auto found = m_Resources.find(key);

        if (found == m_Resources.end()) {
            return nullptr;
        }

        return found->second.lock();
    }

    bool Contains(const Key& key) const
    {
        return static_cast<bool>(Find(key));
    }

    std::size_t PurgeExpired()
    {
        std::size_t removedCount = 0;

        for (auto it = m_Resources.begin();it != m_Resources.end();) 
        {
            if (it->second.expired()) 
            {
                it = m_Resources.erase(it);
                ++removedCount;
            } 
            else 
            {
                ++it;
            }
        }

        return removedCount;
    }

    void Clear()
    {
        m_Resources.clear();
    }

    std::size_t GetTrackedKeyCount() const
    {
        return m_Resources.size();
    }

private:
    std::unordered_map<
        Key,
        std::weak_ptr<Resource>,
        KeyHash
    > m_Resources;
};

}