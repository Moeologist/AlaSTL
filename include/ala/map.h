#ifndef _ALA_MAP_H
#define _ALA_MAP_H

#define _ALA_IS_MAP 1
#include <ala/detail/tree_node_inc.h>
#define _ALA_IS_UNIQ 1
#include <ala/detail/tree_container_inc.h>
#undef _ALA_IS_UNIQ
#define _ALA_IS_UNIQ 0
#include <ala/detail/tree_container_inc.h>

#undef _ALA_IS_MAP
#undef _ALA_IS_UNIQ

#endif // HEAD