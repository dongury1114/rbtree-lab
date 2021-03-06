#include "rbtree.h"
#include <stdio.h>
#include <stdlib.h>

rbtree *new_rbtree(void) {
  rbtree *p = (rbtree *)calloc(1, sizeof(rbtree));
  node_t *nil = (node_t *)calloc(1, sizeof(node_t));

  nil->color = RBTREE_BLACK;
  p->nil = nil;
  p->root = nil;
  // p->root->parent = nil;
  return p;
}

void delete_node(rbtree *t, node_t *x) {
  if (x == t->nil)
    return;
  delete_node(t, x->left);
  delete_node(t, x->right);
  free(x);
}

void delete_rbtree(rbtree *t) {
  // TODO: reclaim the tree nodes's memory
  delete_node(t, t->root);
  free(t->nil);
  free(t);
}

// 추가된 함수
node_t *right_rotate(rbtree *t, node_t *z) {
  node_t *pvt;
  pvt = z->left;
  z->left = pvt->right;
  if (pvt->right != t->nil)
    pvt->right->parent = z;
  pvt->parent = z->parent;
  if (z->parent == t->nil)
    t->root = pvt;
  else if (z == z->parent->left)
    z->parent->left = pvt;
  else
    z->parent->right = pvt;
  z->parent = pvt;
  pvt->right = z;
  return t->root;
}

// 추가된 함수
node_t *left_rotate(rbtree *t, node_t *z) {
  node_t *pvt;
  pvt = z->right;
  z->right = pvt->left;
  if (pvt->left != t->nil)
    pvt->left->parent = z;
  pvt->parent = z->parent;
  if (z->parent == t->nil)
    t->root = pvt;
  else if (z == z->parent->left)
    z->parent->left = pvt;
  else
    z->parent->right = pvt;
  z->parent = pvt;
  pvt->left = z;
  return t->root;
}

node_t *rbtree_insert_fixup(rbtree *t, node_t *z) {
  node_t *y;
  while (z->parent->color == RBTREE_RED) {
    if (z->parent == z->parent->parent->left) {
      y = z->parent->parent->right;
      if (y->color == RBTREE_RED) {
        z->parent->color = RBTREE_BLACK;
        y->color = RBTREE_BLACK;
        z->parent->parent->color = RBTREE_RED;
        z = z->parent->parent;
      }
      else {
        if (z == z->parent->right) {
            z = z->parent;
            left_rotate(t, z);
        }
        z->parent->color = RBTREE_BLACK;
        z->parent->parent->color = RBTREE_RED;
        right_rotate(t, z->parent->parent);
      }
    }
    else {
      y = z->parent->parent->left;
      if (y->color == RBTREE_RED) {
        z->parent->color = RBTREE_BLACK;
        y->color = RBTREE_BLACK;
        z->parent->parent->color = RBTREE_RED;
        z = z->parent->parent;
      }
      else {
        if (z == z->parent->left) {
          z = z->parent;
          right_rotate(t, z);
        }
      z->parent->color = RBTREE_BLACK;
      z->parent->parent->color = RBTREE_RED;
      left_rotate(t, z->parent->parent);
      }
    }
  }
  t->root->color = RBTREE_BLACK;  
  return t->root; 
}

node_t *rbtree_insert(rbtree *t, const key_t key) {
  node_t *z = (node_t *)calloc(1, sizeof(node_t));
  node_t *x = t->root;
  node_t *y = t->nil;
  while (x != t->nil) {
    y = x;
    if (key < x->key)
      x = x->left;
    else
      x = x->right;
  }
  z->parent = y;
  if (y == t->nil)
    t->root = z;
  else if (key < y->key)
    y->left = z;
  else
    y->right = z;
  z->left = t->nil;
  z->right = t-> nil;
  z->key = key;
  z->color = RBTREE_RED;
  rbtree_insert_fixup(t, z);
  return z;
}

node_t *rbtree_find(const rbtree *t, const key_t key) {
  node_t *ptr = t->root;
  while (ptr != t->nil) {
    if (key < ptr->key)
      ptr = ptr->left;
    else if (key > ptr->key)
      ptr = ptr->right;
    else
      return ptr;
  }
  return NULL;
}

node_t *rbtree_min(const rbtree *t) {
  node_t *ptr = t->root;
  while (ptr->left != t->nil)
  {
    ptr = ptr->left;
  }
  return ptr;
}

node_t *rbtree_max(const rbtree *t) {
  node_t *ptr = t->root;
  while (ptr->right != t->nil)
  {
    ptr = ptr->right;
  }
  return ptr;
}

// 추가된 함수
void rbtree_transplant(rbtree *t, node_t *u, node_t *v) {
  if(u->parent == t->nil)
    t->root = v;
  else if (u == u->parent->left)
    u->parent->left = v;
  else u->parent->right = v;
    v->parent = u->parent;
}

// 추가된 함수
node_t *rbtree_p_right_min(const rbtree *t, node_t *n) {
  node_t *ptr;
  ptr = n;
  while (ptr->left != t->nil)
  {
    ptr = ptr->left;
  }
  return ptr;
}

// 추가된 함수
void rbtree_erase_fixup(rbtree *t, node_t *x) {
// x는 삭제된 노드의 대체 노드, w는 x의 형제(sibling)를 가리키는 포인터
// x가 빨간색이 되면 검정색으로 칠하고 종료
// while
  node_t *w;
  while (x!= t->root && x->color == RBTREE_BLACK) {
    // case x->parent->left : x가 새로운 부모의 왼쪽 자식일 때
    if (x == x->parent->left) {
      // w는 부모의 오른쪽 자식
      w = x->parent->right;
      // case 1: 'w' sibling is red
      // 형제를 블랙, 부모를 레드, 부모 기준으로 좌회전
      // 형제를 재설정, 그러나 기존 형제red의 자식(bk)이 형제가 되어서 다시 루프
      // case 2, 3, 4 로 변환됨.
      if (w->color == RBTREE_RED) {
        w->color = RBTREE_BLACK;
        x->parent->color = RBTREE_RED;
        left_rotate(t, x->parent);
        w = x->parent->right;
      }
      // case 2: 'w' sibling is balck and its children are both black
      // 삼촌을 레드, x를 x의 부모로 변경
      if (w->left->color == RBTREE_BLACK && w->right->color == RBTREE_BLACK) {
        w->color = RBTREE_RED;
        x = x->parent;
      } 
      else {
        // case 3: 'w' sibling has right black child
        // 형제의 왼쪽 레드 자식을 블랙, 형제 자신을 레드로 변환
        // 형제를 기준으로 우회전
        if (w->right->color == RBTREE_BLACK) {
          w->left->color = RBTREE_BLACK;
          w->color = RBTREE_RED;
          right_rotate(t, w);
          w = x->parent->right;
        }
        // case 4: 형제 색이 그의 부모 색과
        w->color = x->parent->color;
        x->parent->color = RBTREE_BLACK;
        w->right->color = RBTREE_BLACK;
        left_rotate(t, x->parent);
        x = t->root;
      }
    }
    else {
    // (x == x->parent->right)
      w = x->parent->left;
      if (w->color == RBTREE_RED) {
        w->color = RBTREE_BLACK;
        x->parent->color = RBTREE_RED;
        right_rotate(t, x->parent);
        w = x->parent->left;
      }
      if (w->right->color == RBTREE_BLACK && w->left->color == RBTREE_BLACK) {
        w->color = RBTREE_RED;
        x = x->parent;
      }
      else {
        if (w->left->color == RBTREE_BLACK) {
          w->right->color = RBTREE_BLACK;
          w->color = RBTREE_RED;
          left_rotate(t, w);
          w = x->parent->left;
        }
        w->color = x->parent->color;
        x->parent->color = RBTREE_BLACK;
        w->left->color = RBTREE_BLACK;
        right_rotate(t, x->parent);
        x = t->root;
      }
    }
  }
x->color = RBTREE_BLACK;
}

int rbtree_erase(rbtree *t, node_t *p) {
  node_t *y = p;
  color_t y_og_color = y->color;
  node_t *x;

  if (p->left == t->nil){
    x = p->right;
    rbtree_transplant(t, p, p->right);
  }
  else if (p->right == t->nil){
    x = p->left;
    rbtree_transplant(t, p, p->left);
  }
  else{
    y = rbtree_p_right_min(t, p->right);
    y_og_color = y->color;
    x = y->right;
  
  if(y->parent == p)
    x->parent = y;
  else{
    rbtree_transplant(t, y, y->right);
    p->right->parent = y;
    y->right = p->right;
  }
  
  rbtree_transplant(t, p, y);
  y->left = p->left;
  y->left->parent = y;
  y->color = p->color;
  }

  if(y_og_color == RBTREE_BLACK)
    rbtree_erase_fixup(t, x);

  free(p);
  return 0;
}


// 추가된 함수
int inorder_tree_work(const rbtree *t, const node_t *x, key_t *arr, int i) {
  if (x->left != t->nil) 
    i = inorder_tree_work(t, x->left, arr, i); 
  arr[i] = x->key;
  i = i+1;
  if (x->right != t->nil) 
    i = inorder_tree_work(t, x->right, arr, i);
  return i;
  }



int rbtree_to_array(const rbtree *t, key_t *arr, const size_t n) {

  inorder_tree_work(t, t->root, arr, 0);
  return 0;
}

void inorder(rbtree *t, node_t *cur) {
  if (cur == t->nil) {
    return;
  }
  inorder(t, cur->left);
  printf("%d (%d) / ", cur->key, cur->color);
  inorder(t, cur->right);
}

// int main(){
//   rbtree *t = new_rbtree();
//   rbtree_insert(t, 10);
//   rbtree_insert(t, 5);
//   inorder(t, t->root);
// //   rbtree_insert(t, 8);
// //   rbtree_insert(t, 34);
// //   rbtree_insert(t, 67);
// //   rbtree_insert(t, 23);
// //   rbtree_insert(t, 156);
// //   rbtree_insert(t, 24);
// //   rbtree_insert(t, 2);
// //   rbtree_insert(t, 12);
//   // inorder(t, t->root);
//   puts("");
// }
