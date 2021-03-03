#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "harness.h"
#include "queue.h"

/*
 * Create empty queue.
 * Return NULL if could not allocate space.
 */
queue_t *q_new()
{
    queue_t *q = malloc(sizeof(queue_t));

    if (!q) {
        return NULL;
    }

    q->head = NULL;
    q->tail = NULL;
    q->size = 0;
    return q;
}

/* Free all storage used by queue */
void q_free(queue_t *q)
{
    if (!q) {
        return;
    }

    list_ele_t *tmp = NULL;
    while (q->head) {
        tmp = q->head;
        q->head = q->head->next;
        free(tmp->value);
        free(tmp);
    }

    /* Free queue structure */
    free(q);
}

/*
 * Attempt to insert element at head of queue.
 * Return true if successful.
 * Return false if q is NULL or could not allocate space.
 * Argument s points to the string to be stored.
 * The function must explicitly allocate space and copy the string into it.
 */
bool q_insert_head(queue_t *q, char *s)
{
    list_ele_t *newh;

    if (!q) {
        return false;
    }

    newh = malloc(sizeof(list_ele_t));
    if (!newh) {
        return false;
    }

    int len = strlen(s) + 1;
    newh->value = malloc(len);
    if (!newh->value) {
        free(newh);
        return false;
    }
    bzero(newh->value, len);
    strncpy(newh->value, s, len - 1);

    newh->next = q->head;
    q->head = newh;

    if (!q->tail) {
        q->tail = q->head;
    }

    (q->size)++;

    return true;
}

/*
 * Attempt to insert element at tail of queue.
 * Return true if successful.
 * Return false if q is NULL or could not allocate space.
 * Argument s points to the string to be stored.
 * The function must explicitly allocate space and copy the string into it.
 */
bool q_insert_tail(queue_t *q, char *s)
{
    list_ele_t *newh;

    if (!q) {
        return false;
    }

    newh = malloc(sizeof(list_ele_t));
    if (!newh) {
        return false;
    }

    int len = strlen(s) + 1;
    newh->value = malloc(len);
    if (!newh->value) {
        free(newh);
        return false;
    }
    bzero(newh->value, len);
    strncpy(newh->value, s, len - 1);
    newh->next = NULL;

    if (!q->head) {
        q->tail = newh;
        q->head = q->tail;
    } else {
        q->tail->next = newh;
        q->tail = q->tail->next;
    }

    (q->size)++;

    return true;
}

/*
 * Attempt to remove element from head of queue.
 * Return true if successful.
 * Return false if queue is NULL or empty.
 * If sp is non-NULL and an element is removed, copy the removed string to *sp
 * (up to a maximum of bufsize-1 characters, plus a null terminator.)
 * The space used by the list element and the string should be freed.
 */
bool q_remove_head(queue_t *q, char *sp, size_t bufsize)
{
    if (!q || !q->head) {
        return false;
    }

    char *s = q->head->value;
    if (sp && s) {
        int len = strnlen(s, bufsize);
        if (len == bufsize) {
            len--;
        }
        bzero(sp, bufsize);
        strncpy(sp, s, len);
    }

    list_ele_t *tmp = q->head;
    q->head = q->head->next;
    free(s);
    free(tmp);

    (q->size)--;

    return true;
}

/*
 * Return number of elements in queue.
 * Return 0 if q is NULL or empty
 */
int q_size(queue_t *q)
{
    if (!q) {
        return 0;
    }
    return q->size;
}

/*
 * Reverse elements in queue
 * No effect if q is NULL or empty
 * This function should not allocate or free any list elements
 * (e.g., by calling q_insert_head, q_insert_tail, or q_remove_head).
 * It should rearrange the existing ones.
 */
void q_reverse(queue_t *q)
{
    if (!q || !q->head) {
        return;
    }

    q->tail = q->head;
    list_ele_t *next = NULL;
    list_ele_t *cur = q->head;
    q->head = NULL;
    while (cur) {
        next = cur->next;
        cur->next = q->head;
        q->head = cur;
        cur = next;
    }
}

void split(list_ele_t *head,
           list_ele_t **left_ref,
           list_ele_t **right_ref,
           int size,
           int *left_size,
           int *right_size)
{
    *left_size = size / 2 + size % 2;
    *right_size = size / 2;

    list_ele_t *tmp = head;
    for (int i = 0; i < *left_size - 1; ++i) {
        tmp = tmp->next;
    }
    *right_ref = tmp->next;
    tmp->next = NULL;
    *left_ref = head;
}

list_ele_t *merge(list_ele_t *left, list_ele_t *right)
{
    if (!left) {
        return right;
    }
    if (!right) {
        return left;
    }

    list_ele_t *result = NULL;
    if (strcmp(left->value, right->value) < 0) {
        result = left;
        result->next = merge(left->next, right);
    } else {
        result = right;
        result->next = merge(left, right->next);
    }

    return result;
}

list_ele_t *merge_sort(list_ele_t *head, int size)
{
    if (!head || !head->next) {
        return head;
    }

    list_ele_t *left = NULL, *right = NULL;
    int left_size = 0, right_size = 0;
    split(head, &left, &right, size, &left_size, &right_size);

    left = merge_sort(left, left_size);
    right = merge_sort(right, right_size);

    return merge(left, right);
}

/*
 * Sort elements of queue in ascending order
 * No effect if q is NULL or empty. In addition, if q has only one
 * element, do nothing.
 */
void q_sort(queue_t *q)
{
    if (!q || !q->head || !q->head->next) {
        return;
    }

    q->head = merge_sort(q->head, q->size);
    q->tail = q->head;
    while (q->tail->next) {
        q->tail = q->tail->next;
    }
}
