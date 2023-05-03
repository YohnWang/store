// This file is from https://github.com/YohnWang/gcc-magic-lib
#ifndef GML_VECTOR_H
#define GML_VECTOR_H

#include<stddef.h>
#include<stdlib.h>
#include<stdbool.h>
#include"macro_kit.h"
#include"exception.h"
#include"utils.h"

#define vector_debug(format, ...) fprintf(stderr,"[file: %s][line: %d][function: %s]"format,__FILE__,__LINE__,__func__,##__VA_ARGS__)
#define vector_error(format, ...) vector_debug(format,##__VA_ARGS__)
#define vector_warning(format, ...) vector_debug(format,##__VA_ARGS__)

#define vector(type) macro_cat(vector_,type)
#define vector_raii attr_cleanup(vector_del)

#define vector_local_typedef(vptr)\
    attr_unused typedef typeof(*(vptr)) vector_type_t;\
    attr_unused typedef typeof((vptr)->a[0]) elem_t

#define vector_def(type) \
typedef struct vector(type)\
{\
    ssize_t size;\
    ssize_t capacity;\
    type *a;\
}vector(type)

#define vector_element_t(vptr) typeof((vptr)->a[0])
#define vector_self_t(vptr) typeof(*(vptr))

#define vector_reset(vptr) \
({\
    auto v=vptr;\
    *v=(typeof(*v)){0};{}\
})

#define vector_init(vptr) \
({\
    auto v=vptr;\
    vector_reset(v);\
})

#define make_vector(type) ((vector(type)){0})

static inline void vector_del(void *vptr)
{
    vector_def(void);
    vector(void) *v=vptr;
    if(v->a!=NULL)
        free(v->a);
    vector_reset(v);
}

#define vector_clear(vptr) \
({\
    auto v=vptr;\
    v->size=0;{}\
})

#define vector_pop_back(vptr) \
({\
    auto v=vptr;\
    if(!vector_empty(v)) v->size--;\
    else throw(error_out_of_range,"vector pop when empty\n");\
})

#define vector_size(vptr) \
({\
    auto v=vptr;\
    v->size;\
})

#define vector_capacity(vptr) \
({\
    auto v=vptr;\
    v->capacity;\
})

#define vector_empty(vptr) \
({\
    auto v=vptr;\
    (bool){v->size==0};\
})

#define vector_at(vptr,index) \
({\
    auto v=vptr;\
    auto i=(ssize_t)(index);\
    if(!(i>=0 && i<vector_size(v))) throw(error_out_of_range,"vector access out of range\n");\
    &v->a[i];\
})

#define vector_at_const(vptr,index) \
({\
    auto v=vptr;\
    auto i=(ssize_t)(index);\
    (const vector_element_t(v) *)vector_at(v,i);\
})

#define vector_ref(vptr,index) (*vector_at(vptr,index))

#define vector_get(vptr,index) (*vector_at_const(vptr,index))
#define vector_set(vptr,index_in,e_in) ((void)(vector_ref(vptr,index_in)=e_in))

#define vector_back(vptr) vector_ref(vptr,vector_size(vptr)-1)
#define vector_front(vptr) vector_ref(vptr,0)

#define vector_data(vptr) \
({\
    auto v=vptr;\
    v->a;\
})

#define vector_divert(vptr)\
({\
    auto v=vptr;\
    vector_element_t(v)* ret=v->a;\
    vector_reset(v);\
    ret;\
})

#define vector_reserve(vptr,new_capacity_in)\
({\
    __label__ end;\
    auto v=vptr;\
    auto n=(ssize_t)(new_capacity_in);\
    auto capacity=vector_capacity(v);\
    if(n<=capacity) goto end;\
    resize_array(v->a,n);\
    v->capacity=n;\
    end:{}\
})

#define vector_resize(vptr,new_size_in) \
({\
    __label__ end;\
    auto v=vptr;\
    auto n=new_size_in;\
    if(n<0) throw(error_invalid_argument,"vector resize is lower than 0\n");\
    if(n<vector_size(v)) {v->size=n;goto end;}\
    if(n>vector_capacity(v)) vector_reserve(v,n);\
    array_fill(vector_data(v)+vector_size(v),n-vector_size(v),(vector_element_t(v)){0});\
    v->size=n;\
    end:{}\
})

#define vector_assign(tptr,vptr)\
({\
    __label__ end;\
    auto v=vptr;\
    auto t=tptr;\
    if(v==t) goto end;\
    if(vector_capacity(t)<vector_size(v)) vector_reserve(t,vector_size(v));\
    array_copy(vector_data(t),vector_data(v),vector_size(v));\
    t->size=vector_size(v);\
    end:{}\
})

// like c++ move
#define vector_move(tptr,vptr)\
({\
    auto t=tptr;\
    auto v=vptr;\
    swap(t,v);\
})

#define vector_shrink_to_fit(vptr)\
({\
    auto v=vptr;\
    vector_raii vector_self_t(v) t={};\
    vector_assign(&t,v);\
    vector_move(v,&t);\
})

static inline ssize_t vector_next_capacity_size(ssize_t n)
{
    return n+(n<<1)+1;
}

#define vector_alloc_back(vptr)\
({\
    auto v=vptr;\
    if(vector_size(v)==vector_capacity(v))\
        vector_reserve(v,vector_next_capacity_size(vector_capacity(v)));\
    v->size++;{}\
})

#define vector_push_back(vptr,e_in)\
({\
    auto v=vptr;\
    auto e=e_in;\
    vector_alloc_back(v);\
    vector_back(v)=e;{}\
})

#define vector_insert(vptr,index_in,e_in)\
({\
    vector_local_typedef(vptr);\
    inline void vector_insert_helper(vector_type_t *v,ssize_t index,elem_t e)\
    {\
        if(!(index>=0 && index<=vector_size(v)))\
        {\
            vector_error("vector insert out of range\n");\
            return;\
        }\
        if(vector_alloc_back(v)==0)\
            array_insert(vector_data(v),vector_size(v),index,e);\
        else\
            vector_error("vector insert failed\n");\
    }\
    vector_insert_helper(vptr,index_in,e_in);\
})

#define vector_erase(vptr,index_in)\
({\
    vector_local_typedef(vptr);\
    inline void vector_erase_helper(vector_type_t *v,ssize_t index)\
    {\
        if(!vector_empty(v) && !(index>=0 && index<vector_size(v)))\
        {\
            vector_error("vector erase out of range\n");\
            return;\
        }\
        array_erase(vector_data(v),vector_size(v),index);\
        vector_pop_back(v);\
    }\
    vector_erase_helper(vptr,index_in);\
})

#define vector_find(...) macro_function_guide(_g_vector_find,__VA_ARGS__)

#define _g_vector_find_2(vptr,elem) array_find(vector_data(vptr),vector_size(vptr),elem)
#define _g_vector_find_3(vptr,elem,eq) array_find(vector_data(vptr),vector_size(vptr),elem,eq)

#endif