//
//  test_cache_store.cpp
//
//
//  Created by dakshami on 10/11/20.
//
#define CATCH_CONFIG_MAIN

#include "evictor.hh"
#include "fifo_evictor.hpp"
#include "cache.hh"
#include "catch.hpp"
#include <cassert>
#include <cstring>
#include <iostream>

using namespace std;

int strToStrSize(key_type str){
    size_t length = str.length();
    return length;
}

function<size_t(key_type)> hash_func = strToStrSize;

TEST_CASE("Testing hit rate"){
    Cache cache = Cache(40, 0.75, nullptr, hash_func);
    Cache::val_type val1 = {"value_1", 8};
    Cache::val_type val2 = {"testingValue_2", 16};
    //Cache::val_type val3 = {"value_3", 8};
    SECTION("Hit rate when cache is empty"){
        REQUIRE(cache.hit_rate() == 0.0);
    }
    SECTION("Hit rate when all get calls fail"){
        cache.get("key1");
        cache.get("key2");
        REQUIRE(cache.hit_rate() == 0.0);
    }
    SECTION("Hit rate when half get calls pass and half fail"){
        cache.set("key1", val1);
        cache.set("key2", val2);
        cache.get("key1");
        cache.get("key2");
        cache.get("key3");
        cache.get("key4");
        REQUIRE(cache.hit_rate() == 0.5);
    }
    cache.reset();
}

TEST_CASE("Testing Set"){
    auto cache = Cache(40, 0.75, nullptr, hash_func);

    Cache::val_type val1 = {"value_1", 8};
    Cache::val_type val2 = {"testingValue_2", 16};
    Cache::val_type val3 = {"value_3", 8};
    Cache::val_type val4 = {"computersystemshomework", 24};
    SECTION("Adding val_type works"){
        cache.set("key1", val1);
    REQUIRE(strcmp(cache.get("key1").data_, val1.data_) == 0);
    }
    SECTION("Overwriting"){
        cache.set("key1", val1);
        cache.set("key1", val2);
    REQUIRE(strcmp(cache.get("key1").data_, val2.data_) == 0);
    }
    SECTION("If maximum memory is exceeded, new value is not inserted"){
        cache.set("key1", val4);
        cache.set("key2", val3);
    REQUIRE(strcmp(cache.get("key1").data_, val4.data_) == 0);
    REQUIRE(strcmp(cache.get("key2").data_, val3.data_) == 0);
        cache.set("key4", val2);
        REQUIRE(cache.get("key4").data_ == nullptr);
        
    }
    cache.reset();
}


TEST_CASE("Testing Get") {
    Cache cache = Cache(50, 0.75, nullptr, hash_func);
    SECTION("Testing for key that does not exist") {
        REQUIRE(cache.get("DNE").data_ == nullptr);
        REQUIRE(cache.get("DNE").size_ == 0);
    }
    SECTION("Testing key that exists") {
        Cache::val_type val = {"value", 6};
        cache.set("key", val);
        REQUIRE(strcmp(cache.get("key").data_, val.data_)==0);
        REQUIRE(cache.get("key").size_ == val.size_);
    }
    cache.reset();
}

TEST_CASE("Testing Space_used") {
    Cache cache = Cache(50, 0.75, nullptr, hash_func);
    SECTION("Testing cache of size 0") {
        REQUIRE(cache.space_used() == 0);
    }
    SECTION("Testing cache of size greater than 0") {
        Cache::val_type val = {"value", 6};
        cache.set("key", val);
        REQUIRE(cache.space_used() == 6);
    }
    cache.reset();
}

TEST_CASE("Testing del") {
    Cache cache = Cache(50, 0.75, nullptr, hash_func);
    SECTION("Deleting an object that doesnt exist") {
        auto result = cache.del("DNE");
        REQUIRE(result == false);
    }
    SECTION("Deleting an object") {
        Cache::val_type val = {"value", 6};
        cache.set("key", val);
        auto result = cache.del("key");
        REQUIRE(result == true);
    }
    cache.reset();
}

TEST_CASE("Testing Reset") {
    SECTION("Cache with elements"){
    Cache cache = Cache(50, 0.75, nullptr, hash_func);
    Cache::val_type val_1 = {"value_1", 8};
    cache.set("key_1", val_1);
    Cache::val_type val_2 = {"value_2", 8};
    cache.set("key_2", val_2);
    Cache::val_type val_3 = {"value_3", 8};
    cache.set("key_3", val_3);
    REQUIRE(cache.reset());
    }
    SECTION("Cache with no elements"){
        Cache cache = Cache(50, 0.75, nullptr, hash_func);
        REQUIRE(!cache.reset());
    }
}
/*
TEST_CASE("Standalone Evictor test (borderline cases)"){//no use of cache yet
    fifoEvictor FIFO = fifoEvictor();
    SECTION("Testing eviction when evictor is empty"){
        REQUIRE(FIFO.evict() == "");
    }
    SECTION("Testing evictor"){
        FIFO.touch_key("keyEvict");
        auto evict = FIFO.evict();
        REQUIRE(evict != "");
    }
}

TEST_CASE("FIFO Touch Test"){
    fifoEvictor FIFO = fifoEvictor();
    Cache::val_type val1 = {"value_1", 8};
    Cache::val_type val2 = {"testingValue_2", 16};
    Cache::val_type val3 = {"value_3", 8};
    Cache::val_type val4 = {"computersystemshomework", 24};
    
    SECTION("Testing Touch happening in Set"){
        Cache cache = Cache(51, 0.75, &FIFO, hash_func);
        cache.set("key4", val4);
        cache.set("key1", val1);
        REQUIRE(strcmp(cache.get("key4").data_, val4.data_) == 0);
        REQUIRE(strcmp(cache.get("key1").data_, val1.data_) == 0);
        cache.set("key3", val3);
        cache.set("key2", val2);
        REQUIRE(cache.get("key4").data_ == nullptr);
        REQUIRE(strcmp(cache.get("key1").data_, val1.data_) == 0);
        REQUIRE(strcmp(cache.get("key3").data_, val3.data_) == 0);
        cache.reset();
    }
    SECTION("Testing Touch happening in Get"){
        Cache cache = Cache(45, 0.75, &FIFO, hash_func);
        cache.set("key1", val1);
        cache.set("key2", val2);
        cache.get("key1");
        cache.set("key4", val4);
        REQUIRE(cache.get("key1").data_ == nullptr);
        REQUIRE(strcmp(cache.get("key2").data_, val2.data_) == 0);
        REQUIRE(strcmp(cache.get("key4").data_, val4.data_) == 0);
        cache.reset();
    }
}

TEST_CASE("FIFO Eviction test"){
    fifoEvictor FIFO = fifoEvictor();
    Cache cache = Cache(40, 0.75, &FIFO, hash_func);
    Cache::val_type val1 = {"value_1", 8};
    Cache::val_type val2 = {"testingValue_2", 16};
    Cache::val_type val3 = {"value3", 7};
    Cache::val_type val4 = {"value4", 8};
    Cache::val_type val5 = {"value5", 8};
    Cache::val_type val6 = {"value6", 8};
    SECTION("Testing deleting for non-existent values"){
        cache.set("key1", val1);
        cache.set("key2", val2);
        cache.set("key6", val6);
        cache.set("key3", val3);
        REQUIRE(cache.space_used() == 39);
        cache.del("key1");
        cache.set("key4", val4);

        REQUIRE(cache.space_used() == 39);
        REQUIRE(cache.get("key1").data_ == nullptr);
        REQUIRE(strcmp(cache.get("key2").data_, val2.data_) == 0);
        REQUIRE(strcmp(cache.get("key6").data_, val6.data_) == 0);
        REQUIRE(strcmp(cache.get("key3").data_, val3.data_) == 0);
    }

    SECTION("Testing deleting for existing values"){
        cache.set("key1", val1);
        cache.set("key2", val2);
        cache.set("key6", val6);
        cache.set("key3", val3);
        REQUIRE(cache.space_used() == 39);
        cache.set("key4", val4);
        REQUIRE(cache.space_used() == 39);
        REQUIRE(cache.get("key1").data_ == nullptr);
        REQUIRE(strcmp(cache.get("key2").data_, val2.data_) == 0);
        REQUIRE(strcmp(cache.get("key6").data_, val6.data_) == 0);
        REQUIRE(strcmp(cache.get("key3").data_, val3.data_) == 0);

    }
    
    SECTION("Testing FIFO Strategy"){
        
        cache.set("key3", val3);
        cache.set("key2", val2);
        cache.set("key1", val1);
        cache.set("key6", val6);
        cache.get("key2"); //Since this is FIFO, this should not matter.
        cache.set("key4", val4);
        REQUIRE(cache.get("key3").data_ == nullptr);
        REQUIRE(strcmp(cache.get("key2").data_, val2.data_) == 0);
        REQUIRE(strcmp(cache.get("key6").data_, val6.data_) == 0);
        REQUIRE(strcmp(cache.get("key1").data_, val1.data_) == 0);
        cache.set("key5", val4);
        REQUIRE(cache.get("key3").data_ == nullptr);
        REQUIRE(cache.get("key2").data_ == nullptr);
        REQUIRE(strcmp(cache.get("key6").data_, val6.data_) == 0);
        REQUIRE(strcmp(cache.get("key1").data_, val1.data_) == 0);
        
    }
    SECTION("Overwriting should not change eviction order"){
        cache.set("key3", val3);
        cache.set("key2", val2);
        cache.set("key1", val1);
        cache.set("key6", val6);
        cache.set("key3", val5);

        cache.set("key4", val4);
        REQUIRE(cache.get("key3").data_ == nullptr);
        REQUIRE(strcmp(cache.get("key2").data_, val2.data_) == 0);
        REQUIRE(strcmp(cache.get("key6").data_, val6.data_) == 0);
        REQUIRE(strcmp(cache.get("key1").data_, val1.data_) == 0);
        cache.set("key5", val4);
        REQUIRE(cache.get("key3").data_ == nullptr);
        REQUIRE(cache.get("key2").data_ == nullptr);
        REQUIRE(strcmp(cache.get("key6").data_, val6.data_) == 0);
        REQUIRE(strcmp(cache.get("key1").data_, val1.data_) == 0);
    }

    cache.reset();
    
}
*/
