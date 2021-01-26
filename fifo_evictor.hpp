//
//  fifoEvictor.hpp
//  
//
//  Created by dakshami on 10/11/20.
//

#ifndef fifo_evictor_hpp
#define fifo_evictor_hpp

#include "evictor.hh"
#include <vector>
#include <list>
#include <deque>
#include <queue>

using namespace std;

class fifoEvictor:public Evictor{
private:
    queue<key_type, list<key_type>> values;
public:
    void touch_key(const key_type&);
    const key_type evict();
    fifoEvictor();
    ~fifoEvictor();
};

#endif /* fifo_evictor_hpp */
