//
//  fifo_evictor.cpp
//  
//
//  Created by dakshami on 10/11/20.
//

#include "fifo_evictor.hpp"

fifoEvictor::fifoEvictor(){}

void fifoEvictor::touch_key(const key_type& key){
    values.push(key);
}

const key_type fifoEvictor::evict(){
    if (!values.empty())
    {
        key_type lastkey = values.front();
        values.pop();
        return lastkey;
    }
    return "";
}

fifoEvictor::~fifoEvictor() = default;
