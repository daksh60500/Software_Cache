//
//  lru_evictor.hpp
//  
//
//  Created by dakshami on 10/11/20.
//

#ifndef lru_evictor_hpp
#define lru_evictor_hpp

#include "evictor.hh"
#include <vector>
#include <list>
#include <deque>
#include <queue>
#include <algorithm>

using namespace std;

class lru_Evictor:public Evictor
{
private:
   deque<key_type> values;
public:
    const key_type evict();
    void touch_key(const key_type&);
    lru_Evictor();
    ~lru_Evictor();
};

#endif /* lru_evictor_hpp */
