/*
 * Copyright (c) 1991, 1993
 *      The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *      This product includes software developed by the University of
 *      California, Berkeley and its contributors.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * BSD linked list implementation - extracted for use with liberasurecode
 *
 */

#ifndef _LIST_H
#define _LIST_H

/*
 * A singly-linked list is headed by a single forward pointer. The elements
 * are singly linked for minimum space and pointer manipulation overhead at
 * the expense of O(n) removal for arbitrary elements. New elements can be
 * added to the list after an existing element or at the head of the list.
 * Elements being removed from the head of the list should use the explicit
 * macro for this purpose for optimum efficiency. A singly-linked list may
 * only be traversed in the forward direction.  Singly-linked lists are ideal
 * for applications with large datasets and few or no removals or for
 * implementing a LIFO queue.
 */
/*
 * Singly-linked List declarations.
 */
#define SLIST_HEAD(name, type)                                          \
struct name {                                                           \
        struct type *slh_first; /* first element */                     \
}

#define SLIST_HEAD_INITIALIZER(head)                                    \
        { NULL }

#define SLIST_ENTRY(type)                                               \
struct {                                                                \
        struct type *sle_next;  /* next element */                      \
}

/*
 * Singly-linked List functions.
 */
#define SLIST_EMPTY(head)       ((head)->slh_first == NULL)

#define SLIST_FIRST(head)       ((head)->slh_first)

#define SLIST_FOREACH(var, head, field)                                 \
        for ((var) = SLIST_FIRST((head));                               \
            (var);                                                      \
            (var) = SLIST_NEXT((var), field))

#define SLIST_INIT(head) do {                                           \
        SLIST_FIRST((head)) = NULL;                                     \
} while (0)

#define SLIST_INSERT_AFTER(slistelm, elm, field) do {                   \
        SLIST_NEXT((elm), field) = SLIST_NEXT((slistelm), field);       \
        SLIST_NEXT((slistelm), field) = (elm);                          \
} while (0)

#define SLIST_INSERT_HEAD(head, elm, field) do {                        \
        SLIST_NEXT((elm), field) = SLIST_FIRST((head));                 \
        SLIST_FIRST((head)) = (elm);                                    \
} while (0)

#define SLIST_NEXT(elm, field)  ((elm)->field.sle_next)

#define SLIST_REMOVE(head, elm, type, field) do {                       \
        if (SLIST_FIRST((head)) == (elm)) {                             \
                SLIST_REMOVE_HEAD((head), field);                       \
        }                                                               \
        else {                                                          \
                struct type *curelm = SLIST_FIRST((head));              \
                while (SLIST_NEXT(curelm, field) != (elm))              \
                        curelm = SLIST_NEXT(curelm, field);             \
                SLIST_NEXT(curelm, field) =                             \
                    SLIST_NEXT(SLIST_NEXT(curelm, field), field);       \
        }                                                               \
} while (0)

#define SLIST_REMOVE_HEAD(head, field) do {                             \
        SLIST_FIRST((head)) = SLIST_NEXT(SLIST_FIRST((head)), field);   \
} while (0)

#endif  // _LIST_H
