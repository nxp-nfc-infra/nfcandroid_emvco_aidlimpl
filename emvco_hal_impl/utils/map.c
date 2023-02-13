/******************************************************************************
 *
 *  Copyright 2023 NXP
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 ******************************************************************************/

#include "map.h"
#include "emvco_log.h"
#include <stdlib.h>
#include <string.h>

struct map *map_create(void) {
  struct map *m = (struct map *)malloc(sizeof(struct map));
  m->head = NULL;
  m->size = 0;
  return m;
}

void map_set(struct map *m, const void *key, int key_size, const void *value,
             int value_size) {
  if (!m || !key || key_size <= 0) {
    return;
  }
  struct map_node *node = m->head;
  while (node != NULL) {
    if (key_size == node->key_size && memcmp(node->key, key, key_size) == 0) {
      if (value_size != node->value_size) {
        void *realloc_result = realloc(node->value, value_size);
        if (realloc_result == NULL) {
          return;
        }
        node->value = realloc_result;
        node->value_size = value_size;
      }
      memcpy(node->value, value, value_size);
      return;
    }
    node = node->next;
  }

  struct map_node *new_node =
      (struct map_node *)malloc(sizeof(struct map_node));
  if (new_node == NULL) {
    return;
  }
  new_node->key = malloc(key_size);
  if (new_node->key == NULL) {
    free(new_node);
    return;
  }
  memcpy(new_node->key, key, key_size);
  new_node->key_size = key_size;
  new_node->value = malloc(value_size);
  if (new_node->value == NULL) {
    free(new_node->key);
    free(new_node);
    return;
  }
  memcpy(new_node->value, value, value_size);
  new_node->value_size = value_size;
  new_node->next = m->head;
  m->head = new_node;
  m->size++;
}

void *map_get_value(const struct map *m, const void *key, int key_size,
                    unsigned int *value_size) {
  if (!m || !key || key_size <= 0) {
    return NULL;
  }
  struct map_node *node = m->head;
  while (node != NULL) {
    if (key_size == node->key_size && memcmp(node->key, key, key_size) == 0) {
      *value_size = node->value_size;
      return node->value;
    }
    node = node->next;
  }
  return NULL;
}

void set_string_val(bool value) { string_val = value; }
bool is_string_val() { return string_val; }