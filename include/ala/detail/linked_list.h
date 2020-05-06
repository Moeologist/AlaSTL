#ifndef _ALA_DETAIL_LINKED_LIST_H
#define _ALA_DETAIL_LINKED_LIST_H

namespace ala {
    template<class T>
    struct l_node {
        T _data;
        l_node *_pre, _suc;
    };
    
}

#endif // _ALA_DETAIL_LINKED_LIST_H