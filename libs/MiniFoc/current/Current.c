//
// Created by 邓志君 on 2023/3/2.
//

#include "Current.h"
#include <string.h>
#include "../core/list.h"

#define LOG_TAG "FOC CURRENT"


list g_currentList = {0};


void MiniCurrentManager_init(void)
{
  listInit(&g_currentList);
}

int MiniCurrentManager_registerCurrent(FocCurrent_t *current)
{
  listInsert(&g_currentList, current);

  return 0;
}


FocCurrent_t* MiniCurrentManager_findCurrentByName(const char *currentName)
{
  size_t num = listLength(&g_currentList);

  FocCurrent_t *current = NULL;
  for (int i = 0; i < num; ++i) {
	current = listFindByindex(&g_currentList, i);
	if (!current){
	  break;
	}

	if (!strncmp(currentName, current->name, strlen(current->name))){
	  return current;
	}
  }

  return NULL;
}

int MiniCurrentManager_unregisterCurrent(const char *currentName)
{
  FocCurrent_t *current = MiniCurrentManager_findCurrentByName(currentName);
  listRemove(&g_currentList, current);

  return 0;
}


