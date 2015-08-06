/*
 * Copyright 2010 Rockchip Electronics S.LSI Co. LTD
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef __RK_LIST_H__
#define __RK_LIST_H__

#include "rk_type.h"
#include "rk_thread.h"

/*
 * two list structures are defined, one for C++, the other for C
 */

#ifdef __cplusplus

// desctructor of list node
typedef void *(*node_destructor)(void *);

struct rk_list_node;
class rk_list {
public:
    rk_list(node_destructor func);
    ~rk_list();

    // for FIFO or FILO implement
    // adding functions support simple structure like C struct or C++ class pointer,
    // do not support C++ object
    RK_S32 add_at_head(void *data, RK_S32 size);
    RK_S32 add_at_tail(void *data, RK_S32 size);
    // deleting function will copy the stored data to input pointer with size as size
    // if NULL is passed to deleting functions, the node will be delete directly
    RK_S32 del_at_head(void *data, RK_S32 size);
    RK_S32 del_at_tail(void *data, RK_S32 size);

    // for status check
    RK_S32 list_is_empty();
    RK_S32 list_size();

    // for vector implement - not implemented yet
    // adding function will return a key
    RK_S32 add_by_key(void *data, RK_S32 size, RK_U32 *key);
    RK_S32 del_by_key(void *data, RK_S32 size, RK_U32 key);
    RK_S32 show_by_key(void *data, RK_U32 key);

    RK_S32 flush();

private:
    pthread_mutex_t         mutex;
    node_destructor         destroy;
    struct rk_list_node    *head;
    RK_S32                  count;
    static RK_U32           keys;
    static RK_U32           get_key();

    rk_list();
    rk_list(const rk_list &);
    rk_list &operator=(const rk_list &);
};
#endif


#ifdef __cplusplus
extern "C" {
#endif

struct list_head
{
    struct list_head *next, *prev;
};

#define LIST_HEAD_INIT(name) { &(name), &(name) }

#define LIST_HEAD(name) \
    struct list_head name = LIST_HEAD_INIT(name)

#define INIT_LIST_HEAD(ptr) do { \
        (ptr)->next = (ptr); (ptr)->prev = (ptr); \
    } while (0)

#define list_for_each_safe(pos, n, head) \
    for (pos = (head)->next, n = pos->next; pos != (head); \
            pos = n, n = pos->next)

#define list_entry(ptr, type, member) \
    ((type *)((char *)(ptr)-(unsigned long)(&((type *)0)->member)))

/*
 * due to typeof gcc extension list_for_each_entry and list_for_each_entry_safe
 * can not be used on windows platform
 * So we add a extra type parameter to the macro
 */
#define list_for_each_entry(pos, head, type, member) \
    for (pos = list_entry((head)->next, type, member); \
         &pos->member != (head); \
         pos = list_entry(pos->member.next, type, member))

#define list_for_each_entry_safe(pos, n, head, type, member) \
    for (pos = list_entry((head)->next, type, member),  \
        n = list_entry(pos->member.next, type, member); \
         &pos->member != (head);                    \
         pos = n, n = list_entry(n->member.next, type, member))

static inline void __list_add(struct list_head * _new,
                                  struct list_head * prev,
                                  struct list_head * next)
{
    next->prev = _new;
    _new->next = next;
    _new->prev = prev;
    prev->next = _new;
}

static inline void list_add(struct list_head *_new, struct list_head *head)
{
    __list_add(_new, head, head->next);
}

static inline void list_add_tail(struct list_head *_new, struct list_head *head)
{
    __list_add(_new, head->prev, head);
}

static inline void __list_del(struct list_head * prev, struct list_head * next)
{
    next->prev = prev;
    prev->next = next;
}

static inline void list_del_init(struct list_head *entry)
{
    __list_del(entry->prev, entry->next);

    INIT_LIST_HEAD(entry);
}

static inline int list_is_last(const struct list_head *list, const struct list_head *head)
{
    return list->next == head;
}

static inline int list_empty(struct list_head *head)
{
    return head->next == head;
}

#ifdef __cplusplus
}
#endif


#endif /*__RK_LIST_H__*/
