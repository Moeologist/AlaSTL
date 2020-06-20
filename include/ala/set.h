#ifndef _ALA_SET_H
#define _ALA_SET_H

#define _ALA_IS_MAP 0
#include <ala/detail/tree_node.inc>
#define _ALA_IS_UNIQ 1
#include <ala/detail/tree_container.inc>
#undef _ALA_IS_UNIQ
#define _ALA_IS_UNIQ 0
#include <ala/detail/tree_container.inc>

#undef _ALA_IS_MAP
#undef _ALA_IS_UNIQ

#endif // HEAD