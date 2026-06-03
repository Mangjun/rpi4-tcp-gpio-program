/*******************************************************************************
 * File Name    : <list.c>
 * Description  : <웹 클라이언트 소켓 연결리스트>
 * Author       : <김명준>
 * Date Created : <2026.06.03>
 *
 * History:
 * - <2026.06.03> : 최초 작성 (<김명준>)
 *******************************************************************************/

#include "list.h"

#include <stdlib.h>

/**
 * @brief   클라이언트 정보를 담고 있는 node
 */
struct node {
    int sockfd;             /**< 클라이언트 socket 디스크립터 */
    struct node* prev;      /**< 이전 node를 가리키는 포인터 */
    struct node* next;      /**< 다음 node를 가리키는 포인터 */
};

/**
 * @brief   실제 list 구조체
 * @details 파수꾼 노드를 둬서 NULL 안전
 */
struct list_internal {
    struct node host;   /**< 파수꾼 노드 */
    int size;           /**< 접속한 클라이언트의 수 */
};

static struct node* insert_first_impl(struct list * const self, const int sockfd);
static int remove_node_impl(struct list * const self, struct node* node);
static void for_each_impl(struct list * const self, int (*action)(const int, void *), void *arg);

/**
 * @brief   함수 포인터 바인딩
 * @details static으로 선언하여 1번만 생성
 */
static const struct ops _ops = {
    .insert_first   = insert_first_impl,
    .remove_node    = remove_node_impl,
    .for_each       = for_each_impl
};

struct list* list_create(void)
{
    struct list* new_list = (struct list *)malloc(sizeof(struct list));
    if (!new_list) {
        return NULL;
    }

    new_list->pimpl = (struct list_internal *)malloc(sizeof(struct list_internal));
    if (!new_list->pimpl) {
        free(new_list);
        return NULL;
    }

    new_list->pimpl->host.sockfd = -1;
    new_list->pimpl->host.prev = &new_list->pimpl->host;
    new_list->pimpl->host.next = &new_list->pimpl->host;
    new_list->pimpl->size = 0;

    new_list->ops = &_ops;

    return new_list;
}

int list_destroy(struct list * const self)
{
    if (!self) {
        return -1;
    }

    struct node *curr = self->pimpl->host.next;
    while (curr != &self->pimpl->host) {
        struct node *temp = curr;
        curr = curr->next;
        free(temp);
    }

    free(self->pimpl);
    free(self);

    return 0;
}

int node_get_fd(const struct node * const node)
{
    if (!node) {
        return -1;
    }

    return node->sockfd;
}

/**
 * @brief   리스트에 클라이언트 추가하는 헬퍼 함수
 * @details epoll data.ptr 사용하기 위해서 node 자체를 반환
 * @param   self    list
 * @param   sockfd  클라이언트 소켓
 * @return  성공(클라이언트 node), 실패(NULL)
 */
static struct node* insert_first_impl(struct list * const self, const int sockfd)
{
    if (!self || !self->pimpl || sockfd < 0) {
        return NULL;
    }

    struct node* new_node = (struct node *)malloc(sizeof(struct node));
    if (!new_node) {
        return NULL;
    }

    new_node->sockfd = sockfd;

    new_node->next = self->pimpl->host.next;
    new_node->prev = &self->pimpl->host;
    self->pimpl->host.next->prev = new_node;
    self->pimpl->host.next = new_node;

    self->pimpl->size++;

    return new_node;
}

/**
 * @brief   리스트에 클라이언트 삭제하는 헬퍼 함수
 * @details O(1)으로 접근하기 위해서 epoll의 data.ptr을 채용
 * @param   self    list
 * @param   node    삭제할 클라이언트 node
 * @return  성공(0), 실패(-1)
 */
static int remove_node_impl(struct list * const self, struct node* node)
{
    if (!self || !self->pimpl || self->pimpl->size == 0) {
        return -1;
    }

    node->prev->next = node->next;
    node->next->prev = node->prev;

    free(node);
    self->pimpl->size--;

    return 0;
}

/**
 * @brief   broadcast 하기 위한 헬퍼 함수
 * @details 의존성 낮추기 위해 콜백 함수로 작성
 * @param   self    list
 * @param   action  각각의 노드에게 상태 송신 콜백 함수
 * @param   arg     gpio 상태
 */
static void for_each_impl(struct list * const self, int (*action)(const int, void *), void *arg)
{
    if (!self || !self->pimpl || self->pimpl->size == 0) {
        return;
    }

    struct node *curr = self->pimpl->host.next;
    struct node *next = curr;

    while (curr != &self->pimpl->host) {
        next = curr->next;

        if (action(curr->sockfd, arg) < 0) {
            remove_node_impl(self, curr);
        }
        
        curr = next;
    }
}