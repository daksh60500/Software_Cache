//
//  lru_evictor.cpp
//  
//
//  Created by dakshami on 10/11/20.
//

#include "lru_evictor.hpp"

lru_Evictor::lru_Evictor(){

}

lru_Evictor::~lru_Evictor() = default;


const key_type lru_Evictor::evict(){
    if (values.empty() == false)
    {
        key_type lastkey = values.front();
        values.pop_front();
        return lastkey;
    }
    return values.front();
}


void lru_Evictor::touch_key(const key_type& key){
    auto location  = std::find(values.begin(), values.end(), key);
    if (location != values.end())
    {
        values.erase(location);
    }
    
    values.push_back(key);
}
