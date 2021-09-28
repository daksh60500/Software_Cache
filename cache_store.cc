#include "cache.hh"
#include <unordered_map>
#include <iostream>
#include <vector>
#include <tuple>
#include <cassert>
#include <cstring>

using namespace std;

class Cache::Impl{
    
    public:
    size_type maxMem;
    size_type usedMem;
    Evictor* evictor;
    mutable pair<int, int> hitRate = {0, 0}; 
    hash_func hash_function;
    unordered_map<key_type, val_type, hash_func> cache;
    
    bool set(key_type key, val_type val){
        del(key);//added after HW04
        size_type& size = val.size_;
        byte_type* newData = new byte_type[size];
        bool result = false;
        for (size_type i = 0; i < size; i++){
            newData[i] = val.data_[i];
        }looping using size
        if (usedMem <= maxMem - size){ //writing
            val_type newVal;
            newVal.data_ = newData;
            newVal.size_ = size;
            cache[key] = newVal;
            usedMem += size;
            cache.reserve(cache.size());
            if (evictor != nullptr)
            {
                evictor->touch_key(key);
            }
            result = !result;
        }
        else{
            if (evictor == nullptr){
                delete [] newData;
            }
            else{
                while (usedMem > maxMem - size)
                {
                    bool evictorCheck = del(evictor->evict());
                    while (!evictorCheck)
                    {
                        evictorCheck = del(evictor->evict());
                    }
                    
                }
                val_type newVal;
                newVal.data_ = newData;
                newVal.size_ = size;
                cache[key] = newVal;
                usedMem += size;
                    
            }
        }
        return result;
    }

    val_type get (key_type key) const {
        hitRate.second += 1;
        if (cache.find(key) == cache.end()){
            val_type val = {nullptr, 0};\
            return val;
        }
        if (evictor != nullptr){
            evictor->touch_key(key);
        }
        hitRate.first += 1;//get was successful
        val_type newVal;
        newVal.data_ = cache.at(key).data_;
        newVal.size_ = cache.at(key).size_;
        return newVal;
    }

    double hit_rate() const{
        if(hitRate.second == 0){
            return 0.0;
        }
        return double(hitRate.first)/hitRate.second;
    }
    bool del(key_type key) {
        if (cache.find(key) != cache.end())
                {
                    usedMem -= cache.at(key).size_;
                    delete[] cache.at(key).data_;
                    cache.erase(key);
                    return true;
                }
                return false;
    }

    size_type space_used() const {
        return usedMem;
    }

    bool reset(void) {
        auto i = cache.begin();
        if(i == cache.end()){
            return false;
        }
        while(i != cache.end()){
            del(i->first);
            i=cache.begin();
        }
        usedMem = 0;
        cache.clear();
        maxMem = 0;
        return true;
    }
};

Cache::Cache(size_type maxMem, float maxLoadFactor, Evictor* evictor, hash_func hasher) : pImpl_ (new Impl()) {
        
        if (hasher == nullptr)
        {
            unordered_map<key_type, val_type, hash_func> newCache(1);
            pImpl_->cache = newCache;
        }
        else{
            pImpl_->hash_function =  hasher;
            unordered_map<key_type, val_type, hash_func> newCache(1, hasher);
            pImpl_->cache = newCache;
        }
        

        pImpl_->cache.max_load_factor(maxLoadFactor);
        pImpl_->usedMem = 0;
        pImpl_->maxMem = maxMem;
        pImpl_->evictor = evictor;
}


Cache::~Cache() = default;

bool Cache::set(key_type key, val_type val){
    return pImpl_->set(key, val);
}

Cache::val_type Cache::get(key_type key) const{
    return pImpl_->get(key);
}

bool Cache::del(key_type key){
    return pImpl_->del(key);
}

Cache::size_type Cache::space_used() const{
    return pImpl_->space_used();
}

bool Cache::reset(){
    return pImpl_->reset();
}

double Cache::hit_rate() const{
    return pImpl_->hit_rate();
}
