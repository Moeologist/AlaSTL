#ifndef _ALA_CONFIG_FEATURES_H
#define _ALA_CONFIG_FEATURES_H

#ifndef ALA_USE_RTTI
    #define ALA_USE_RTTI 1
#endif

#ifndef ALA_USE_EXCEPTION
    #define ALA_USE_EXCEPTION 1
#endif

// Use following macros only for test
#ifndef ALA_INSERTION_THRESHOLD
    #define ALA_INSERTION_THRESHOLD 28
#endif

#ifndef ALA_USE_ALLOC_REBIND
    #define ALA_USE_ALLOC_REBIND 0
#endif

#endif // HEAD