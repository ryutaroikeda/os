#ifndef _lib_h_
#define _lib_h_

/**
 * Declare and define a queue for the given struct.
 * This won't work for immutable structs.
 */

enum queue_error {
    QUEUE_ERROR_NONE
    ,QUEUE_ERROR_OVERFLOW
    ,QUEUE_ERROR_UNDERFLOW
};

#define QUEUE_DECLARE(type, size) \
    struct type ## _queue { \
        struct type queue[size]; \
        int front; \
        int back; \
        int error; \
    }; \
    void type ## _queue_enqueue( \
            struct type ## _queue*, struct type); \
    struct type type ## _queue_dequeue(struct type ## _queue*); \
    int type ## _queue_is_empty(struct type ## _queue*); \
    struct swallow_semicolon_after_declare ## type { int a; }

#define QUEUE_DEFINE(type, size) \
    void type ## _queue_enqueue( \
            struct type ## _queue* q, struct type t) { \
        if (q->back == (q->front - 1) % size) { \
            q->error = QUEUE_ERROR_OVERFLOW; \
            return; \
        } \
        q->queue[q->back] = t; \
        q->back = (q->back + 1) % size; \
    } \
    struct type type ## _queue_dequeue(struct type ## _queue* q) { \
        if (q->front == q->back) { \
            q->error = QUEUE_ERROR_UNDERFLOW; \
            struct type t; \
            return t; \
        } \
        struct type result = q->queue[q->front]; \
        q->front = (q->front + 1) % size; \
        return result; \
    } \
    int type ## _queue_is_empty(struct type ## _queue* q) { \
        return q->front == q->back; \
    } \
    struct swallow_semicolon_after_define ## type { int a; }

#endif

