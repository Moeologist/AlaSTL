#ifndef _ALA_MAP_H
#define _ALA_MAP_H

#define IS_MAP 1
#include <ala/detail/tree_node_inc.h>
#define IS_UNIQ 1
#include <ala/detail/tree_container_inc.h>
#undef IS_UNIQ
#define IS_UNIQ 0
#include <ala/detail/tree_container_inc.h>

#undef IS_MAP
#undef IS_UNIQ

#endif // HEAD