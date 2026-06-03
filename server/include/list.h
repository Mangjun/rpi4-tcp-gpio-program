/*******************************************************************************
 * File Name    : <list.h>
 * Description  : <웹 클라이언트 소켓 연결리스트 함수 선언부>
 * Author       : <김명준>
 * Date Created : <2026.06.03>
 *
 * History:
 * - <2026.06.03> : 최초 작성 (<김명준>)
 *******************************************************************************/

#pragma once

struct list;
struct node;
struct list_internal;

/**
 * @brief   list 멤버 함수
 */
struct ops {
    struct node* (*insert_first)(struct list * const self, const int sockfd);
    int (*remove_node)(struct list * const self, struct node*);
    void (*for_each)(struct list * const self, int (*action)(const int, void *), void *arg);
};

/**
 * @brief   리스트 인터페이스 구조체
 */
struct list {
    struct list_internal *pimpl; /**< 컴파일 의존성 낮춤 */
    const struct ops *ops;       /**< 함수 포인터 테이블 */
};

/**
 * @brief   list 생성
 * @return  list 포인터
 */
struct list* list_create(void);

/**
 * @brief   list 삭제
 * @param   self    list 포인터
 * @return  성공(0), 실패(-1)
 */
int list_destroy(struct list * const self);

/**
 * @brief   클라이언트 socket 디스크립터 반환
 * @param   node    클라이언트 node
 * @return  socket 디스크립터
 */
int node_get_fd(const struct node * const node);