/** \file
 * \brief Tree control
 *
 * See Copyright Notice in iup.h
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#include "iup.h"
#include "iupcbs.h"

#include "iup_object.h"
#include "iup_attrib.h"
#include "iup_str.h"
#include "iup_drv.h"
#include "iup_drvfont.h"
#include "iup_stdcontrols.h"
#include "iup_layout.h"
#include "iup_tree.h"
#include "iup_assert.h"


#define ITREE_IMG_WIDTH   16
#define ITREE_IMG_HEIGHT  16

static void iTreeInitializeImages(void)
{
  Ihandle *image_leaf, *image_blank, *image_paper;  
  Ihandle *image_collapsed, *image_expanded;  

  unsigned char img_leaf[ITREE_IMG_WIDTH*ITREE_IMG_HEIGHT] = 
  {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 4, 4, 5, 5, 5, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 4, 5, 5, 1, 6, 1, 5, 0, 0, 0, 0, 0,
    0, 0, 0, 3, 4, 4, 5, 5, 1, 6, 1, 5, 0, 0, 0, 0,
    0, 0, 0, 3, 4, 4, 4, 5, 5, 1, 1, 5, 0, 0, 0, 0,
    0, 0, 0, 2, 3, 4, 4, 4, 5, 5, 5, 4, 0, 0, 0, 0,
    0, 0, 0, 2, 3, 3, 4, 4, 4, 5, 4, 4, 0, 0, 0, 0,
    0, 0, 0, 0, 2, 3, 3, 4, 4, 4, 4, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 2, 2, 3, 3, 3, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
  };

  unsigned char img_collapsed[ITREE_IMG_WIDTH*ITREE_IMG_HEIGHT] =
  {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  
    0, 0, 2, 2, 2, 2, 2, 3, 0, 0, 0, 0, 0, 0, 0, 0,  
    0, 2, 6, 5, 5, 7, 7, 2, 3, 0, 0, 0, 0, 0, 0, 0, 
    2, 4, 4, 4, 4, 4, 4, 4, 2, 2, 2, 2, 2, 2, 0, 0, 
    2, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 7, 4, 3, 0, 
    2, 5, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 1, 4, 3, 0, 
    2, 5, 7, 7, 7, 7, 7, 7, 7, 1, 7, 1, 7, 4, 3, 0, 
    2, 5, 7, 7, 7, 7, 7, 7, 7, 7, 1, 7, 1, 4, 3, 0, 
    2, 5, 7, 7, 7, 7, 7, 1, 7, 1, 7, 1, 7, 4, 3, 0, 
    2, 5, 7, 7, 7, 7, 7, 7, 1, 7, 1, 7, 1, 4, 3, 0, 
    2, 5, 7, 7, 7, 1, 7, 1, 7, 1, 7, 1, 1, 4, 3, 0, 
    2, 5, 1, 7, 1, 7, 1, 7, 1, 7, 1, 1, 1, 4, 3, 0, 
    2, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 3, 0,  
    0, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0  
  };

  unsigned char img_expanded[ITREE_IMG_WIDTH*ITREE_IMG_HEIGHT] =
  {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 2, 2, 2, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 2, 1, 1, 1, 1, 1, 2, 0, 0, 0, 0, 0, 0, 0, 
    0, 2, 1, 3, 3, 3, 3, 3, 1, 2, 2, 2, 2, 2, 2, 0, 
    0, 2, 1, 3, 3, 3, 3, 3, 3, 3, 1, 1, 1, 1, 6, 4, 
    0, 2, 1, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 6, 4, 
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 6, 3, 6, 4, 
    2, 1, 3, 3, 3, 3, 3, 3, 5, 3, 5, 6, 4, 6, 6, 4, 
    2, 1, 3, 3, 3, 3, 5, 3, 3, 5, 3, 6, 4, 6, 6, 4, 
    0, 2, 0, 3, 3, 3, 3, 3, 5, 3, 5, 5, 2, 4, 2, 4, 
    0, 2, 0, 3, 3, 5, 3, 5, 3, 5, 5, 5, 6, 4, 2, 4, 
    0, 0, 2, 0, 5, 3, 5, 3, 5, 5, 5, 5, 6, 2, 4, 4, 
    0, 0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 4, 4, 
    0, 0, 0, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 
  };

  unsigned char img_blank[ITREE_IMG_WIDTH*ITREE_IMG_HEIGHT] =
  {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 4, 3, 4, 3, 4, 3, 4, 3, 4, 3, 0, 0, 0, 0,
    0, 0, 3, 1, 1, 1, 1, 1, 1, 1, 1, 5, 4, 0, 0, 0,
    0, 0, 4, 1, 1, 1, 1, 1, 1, 1, 1, 5, 1, 4, 0, 0,
    0, 0, 3, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 0,
    0, 0, 4, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 5, 2, 0,
    0, 0, 3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 5, 2, 0,
    0, 0, 4, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 5, 2, 0,
    0, 0, 3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 5, 2, 0,
    0, 0, 4, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 5, 2, 0,
    0, 0, 3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 5, 2, 0,
    0, 0, 4, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 5, 2, 0,
    0, 0, 3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 5, 2, 0,
    0, 0, 4, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 5, 2, 0,
    0, 0, 3, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 2, 0,
    0, 0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 0
  };

  unsigned char img_paper[ITREE_IMG_WIDTH*ITREE_IMG_HEIGHT] =
  {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 4, 3, 4, 3, 4, 3, 4, 3, 4, 3, 0, 0, 0, 0,
    0, 0, 3, 1, 1, 1, 1, 1, 1, 1, 1, 5, 4, 0, 0, 0,
    0, 0, 4, 1, 1, 1, 1, 1, 1, 1, 1, 5, 1, 4, 0, 0,
    0, 0, 3, 1, 4, 3, 4, 3, 4, 3, 4, 2, 2, 2, 2, 0,
    0, 0, 4, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 5, 2, 0,
    0, 0, 3, 1, 3, 4, 3, 4, 3, 4, 3, 4, 1, 5, 2, 0,
    0, 0, 4, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 5, 2, 0,
    0, 0, 3, 1, 4, 3, 4, 3, 4, 3, 4, 3, 1, 5, 2, 0,
    0, 0, 4, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 5, 2, 0,
    0, 0, 3, 1, 3, 4, 3, 4, 3, 4, 3, 4, 1, 5, 2, 0,
    0, 0, 4, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 5, 2, 0,
    0, 0, 3, 1, 4, 3, 4, 3, 4, 3, 4, 3, 1, 5, 2, 0,
    0, 0, 4, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 5, 2, 0,
    0, 0, 3, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 2, 0,
    0, 0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 0
  };

  image_leaf      = IupImage(ITREE_IMG_WIDTH, ITREE_IMG_HEIGHT, img_leaf);
  image_collapsed = IupImage(ITREE_IMG_WIDTH, ITREE_IMG_HEIGHT, img_collapsed);
  image_expanded  = IupImage(ITREE_IMG_WIDTH, ITREE_IMG_HEIGHT, img_expanded);
  image_blank     = IupImage(ITREE_IMG_WIDTH, ITREE_IMG_HEIGHT, img_blank);
  image_paper     = IupImage(ITREE_IMG_WIDTH, ITREE_IMG_HEIGHT, img_paper);

  IupSetAttribute(image_leaf, "0", "BGCOLOR");
  IupSetAttribute(image_leaf, "1", "192 192 192");
  IupSetAttribute(image_leaf, "2", "56 56 56");
  IupSetAttribute(image_leaf, "3", "99 99 99");
  IupSetAttribute(image_leaf, "4", "128 128 128");
  IupSetAttribute(image_leaf, "5", "161 161 161");
  IupSetAttribute(image_leaf, "6", "222 222 222");

  IupSetAttribute(image_collapsed, "0", "BGCOLOR");
  IupSetAttribute(image_collapsed, "1", "255 206 156");
  IupSetAttribute(image_collapsed, "2", "156 156 0");
  IupSetAttribute(image_collapsed, "3", "0 0 0");
  IupSetAttribute(image_collapsed, "4", "206 206 99");
  IupSetAttribute(image_collapsed, "5", "255 255 206");
  IupSetAttribute(image_collapsed, "6", "247 247 247");
  IupSetAttribute(image_collapsed, "7", "255 255 156");

  IupSetAttribute(image_expanded, "0", "BGCOLOR");
  IupSetAttribute(image_expanded, "1", "255 255 255");
  IupSetAttribute(image_expanded, "2", "156 156 0");
  IupSetAttribute(image_expanded, "3", "255 255 156");
  IupSetAttribute(image_expanded, "4", "0 0 0");
  IupSetAttribute(image_expanded, "5", "255 206 156");
  IupSetAttribute(image_expanded, "6", "206 206 99");

  IupSetAttribute(image_blank, "0", "BGCOLOR");
  IupSetAttribute(image_blank, "1", "255 255 255");
  IupSetAttribute(image_blank, "2", "000 000 000");
  IupSetAttribute(image_blank, "3", "119 119 119");
  IupSetAttribute(image_blank, "4", "136 136 136");
  IupSetAttribute(image_blank, "5", "187 187 187");

  IupSetAttribute(image_paper, "0", "BGCOLOR");
  IupSetAttribute(image_paper, "1", "255 255 255");
  IupSetAttribute(image_paper, "2", "000 000 000");
  IupSetAttribute(image_paper, "3", "119 119 119");
  IupSetAttribute(image_paper, "4", "136 136 136");
  IupSetAttribute(image_paper, "5", "187 187 187");

  IupSetHandle("IMGLEAF",      image_leaf);
  IupSetHandle("IMGCOLLAPSED", image_collapsed);
  IupSetHandle("IMGEXPANDED",  image_expanded);
  IupSetHandle("IMGBLANK",     image_blank);
  IupSetHandle("IMGPAPER",     image_paper);
}

void iupTreeUpdateImages(Ihandle *ih)
{
  int inherit;

  char* value = iupAttribGet(ih, "IMAGELEAF");
  if (!value) value = "IMGLEAF";
  iupClassObjectSetAttribute(ih, "IMAGELEAF", value, &inherit);

  value = iupAttribGet(ih, "IMAGEBRANCHCOLLAPSED");
  if (!value) value = "IMGCOLLAPSED";
  iupClassObjectSetAttribute(ih, "IMAGEBRANCHCOLLAPSED", value, &inherit);

  value = iupAttribGet(ih, "IMAGEBRANCHEXPANDED");
  if (!value) value = "IMGEXPANDED";
  iupClassObjectSetAttribute(ih, "IMAGEBRANCHEXPANDED", value, &inherit);
}

void iupTreeSelectLastCollapsedBranch(Ihandle* ih, int *last_id)
{
  /* if last selected item is a branch, then select its children */
  if (iupStrEqual(IupTreeGetAttribute(ih, "KIND", *last_id), "BRANCH") && 
      iupStrEqual(IupTreeGetAttribute(ih, "STATE", *last_id), "COLLAPSED"))
  {
    int childcount = IupTreeGetInt(ih, "CHILDCOUNT", *last_id);
    if (childcount > 0)
    {
      int start = *last_id + 1;
      int end = *last_id + childcount;
      IupSetfAttribute(ih, "MARK", "%d-%d", start, end);
      *last_id = *last_id + childcount;
    }
  }
}

int iupTreeForEach(Ihandle* ih, iupTreeNodeFunc func, void* userdata)
{
  int i;
  for (i = 0; i < ih->data->node_count; i++)
  {
    if (!func(ih, ih->data->node_cache[i].node_handle, i, userdata))
      return 0;
  }

  return 1;
}

int iupTreeFindNodeId(Ihandle* ih, InodeHandle* node_handle)
{
  /* Unoptimized version:
  int i;
  for (i = 0; i < ih->data->node_count; i++)
  {
    if (ih->data->node_cache[i].node_handle == node_handle)
      return i;
  }
  */
  InodeData *node_cache = ih->data->node_cache;
  while(node_cache->node_handle != node_handle && 
        node_cache->node_handle != NULL)   /* the cache always have zeros at the end */
    node_cache++;

  if (node_cache->node_handle != NULL)
    return node_cache - ih->data->node_cache;
  else
    return -1;
}

static int iTreeFindUserDataId(Ihandle* ih, void* userdata)
{
  /* Unoptimized version:
  int i;
  for (i = 0; i < ih->data->node_count; i++)
  {
    if (ih->data->node_cache[i].node_handle == node_handle)
      return i;
  }
  */
  InodeData *node_cache = ih->data->node_cache;
  while(node_cache->userdata != userdata && 
        node_cache->node_handle != NULL)   /* the cache always have zeros at the end */
    node_cache++;

  if (node_cache->node_handle != NULL)
    return node_cache - ih->data->node_cache;
  else
    return -1;
}

InodeHandle* iupTreeGetNode(Ihandle* ih, int id)
{
  if (id >= 0 && id < ih->data->node_count)
    return ih->data->node_cache[id].node_handle;
  else if (id == IUP_INVALID_ID && ih->data->node_count!=0)
    return iupdrvTreeGetFocusNode(ih);
  else
    return NULL;
}

InodeHandle* iupTreeGetNodeFromString(Ihandle* ih, const char* name_id)
{
  int id = IUP_INVALID_ID;
  iupStrToInt(name_id, &id);
  return iupTreeGetNode(ih, id);
}

static void iTreeAddToCache(Ihandle* ih, int id, InodeHandle* node_handle)
{
  iupASSERT(id >= 0 && id < ih->data->node_count);
  if (id < 0 || id >= ih->data->node_count)
    return;

  /* node_count here already contains the final count */

  if (id == ih->data->node_count-1)
    ih->data->node_cache[id].node_handle = node_handle;
  else
  {
    /* open space for the new id */
    int remain_count = ih->data->node_count-id;
    memmove(ih->data->node_cache+id+1, ih->data->node_cache+id, remain_count*sizeof(InodeData));
    ih->data->node_cache[id].node_handle = node_handle;
  }

  ih->data->node_cache[id].userdata = NULL;
}

static void iTreeIncCacheMem(Ihandle* ih)
{
  /* node_count here already contains the final count */

  if (ih->data->node_count+10 > ih->data->node_cache_max)
  {
    int old_node_cache_max = ih->data->node_cache_max;
    ih->data->node_cache_max += 20;
    ih->data->node_cache = realloc(ih->data->node_cache, ih->data->node_cache_max*sizeof(InodeData));
    memset(ih->data->node_cache+old_node_cache_max, 0, 20*sizeof(InodeData));
  }
}

void iupTreeAddToCache(Ihandle* ih, int add, int kindPrev, InodeHandle* prevNode, InodeHandle* node_handle)
{
  int new_id = 0;

  ih->data->node_count++;

  /* node_count here already contains the final count */
  iTreeIncCacheMem(ih);

  if (prevNode)
  {
    if (add || kindPrev == ITREE_LEAF)
    {
      /* ADD implies always that id=prev_id+1 */
      /* INSERT after a leaf implies always that new_id=prev_id+1 */
      int prev_id = iupTreeFindNodeId(ih, prevNode);
      new_id = prev_id+1;
    }
    else
    {
      /* INSERT after a branch implies always that new_id=prev_id+1+child_count */
      int prev_id = iupTreeFindNodeId(ih, prevNode);
      int child_count = iupdrvTreeTotalChildCount(ih, prevNode);
      new_id = prev_id+1+child_count;
    }
  }

  iTreeAddToCache(ih, new_id, node_handle);
  iupAttribSetInt(ih, "LASTADDNODE", new_id);
}

void iupTreeDelFromCache(Ihandle* ih, int id, int count)
{
  int remain_count;

  /* id can be the last node, actually==node_count becase node_count is already updated */
  iupASSERT(id >= 0 && id <= ih->data->node_count);  
  if (id < 0 || id > ih->data->node_count)
    return;

  /* node_count here already contains the final count */

  /* remove id+count */
  remain_count = ih->data->node_count-id;
  memmove(ih->data->node_cache+id, ih->data->node_cache+id+count, remain_count*sizeof(InodeData));

  /* clear the remaining space */
  memset(ih->data->node_cache+ih->data->node_count, 0, count*sizeof(InodeData));
}

void iupTreeCopyMoveCache(Ihandle* ih, int id_src, int id_dst, int count, int is_copy)
{
  int remain_count;

  iupASSERT(id_src >= 0 && id_src < ih->data->node_count);
  if (id_src < 0 || id_src >= ih->data->node_count)
    return;

  iupASSERT(id_dst >= 0 && id_dst < ih->data->node_count);
  if (id_dst < 0 || id_dst >= ih->data->node_count)
    return;

  iupASSERT(id_dst < id_src || id_dst > id_src+count);
  if (id_dst >= id_src && id_dst <= id_src+count)
    return;

  /* id_dst here points to the final position for a copy operation */

  /* node_count here contains the final count for a copy operation */
  iTreeIncCacheMem(ih);

  /* add space for new nodes */
  remain_count = ih->data->node_count - (id_dst + count);
  memmove(ih->data->node_cache+id_dst+count, ih->data->node_cache+id_dst, remain_count*sizeof(InodeData));

  /* compensate because we add space for new nodes */
  if (id_src > id_dst)
    id_src += count;

  if (is_copy) 
  {
    /* during a copy, the userdata is not reused, so clear it */
    memset(ih->data->node_cache+id_dst, 0, count*sizeof(InodeData));
  }
  else /* move = copy + delete */
  {
    /* copy userdata from src to dst */
    memcpy(ih->data->node_cache+id_dst, ih->data->node_cache+id_src, count*sizeof(InodeData));

    /* remove the src */
    remain_count = ih->data->node_count - (id_src + count);
    memmove(ih->data->node_cache+id_src, ih->data->node_cache+id_src+count, remain_count*sizeof(InodeData));

    /* clear the remaining space */
    memset(ih->data->node_cache+ih->data->node_count-count, 0, count*sizeof(InodeData));
  }
}


/*************************************************************************/


char* iupTreeGetSpacingAttrib(Ihandle* ih)
{
  char *str = iupStrGetMemory(50);
  sprintf(str, "%d", ih->data->spacing);
  return str;
}

static char* iTreeGetMarkModeAttrib(Ihandle* ih)
{
  if (ih->data->mark_mode==ITREE_MARK_SINGLE)
    return "SINGLE";
  else
    return "MULTIPLE";
}

static int iTreeSetMarkModeAttrib(Ihandle* ih, const char* value)
{
  if (iupStrEqualNoCase(value, "MULTIPLE"))
    ih->data->mark_mode = ITREE_MARK_MULTIPLE;    
  else 
    ih->data->mark_mode = ITREE_MARK_SINGLE;

  if (ih->handle)
    iupdrvTreeUpdateMarkMode(ih); /* for this to work, must update during map */

  return 0;
}

static int iTreeSetShiftAttrib(Ihandle* ih, const char* value)
{
  if (iupStrBoolean(value) && iupAttribGetBoolean(ih, "CTRL"))
    iTreeSetMarkModeAttrib(ih, "MULTIPLE");
  else
    iTreeSetMarkModeAttrib(ih, "SINGLE");
  return 1;
}

static int iTreeSetCtrlAttrib(Ihandle* ih, const char* value)
{
  if (iupStrBoolean(value) && iupAttribGetBoolean(ih, "SHIFT"))
    iTreeSetMarkModeAttrib(ih, "MULTIPLE");
  else
    iTreeSetMarkModeAttrib(ih, "SINGLE");
  return 1;
}

static char* iTreeGetShowRenameAttrib(Ihandle* ih)
{
  if (ih->data->show_rename)
    return "YES";
  else
    return "NO";
}

static int iTreeSetShowRenameAttrib(Ihandle* ih, const char* value)
{
  if (iupStrBoolean(value))
    ih->data->show_rename = 1;
  else
    ih->data->show_rename = 0;

  return 0;
}

static char* iTreeGetShowToggleAttrib(Ihandle* ih)
{
  if (ih->data->show_toggle)
  {
    if (ih->data->show_toggle == 2)
      return "3STATE";
    else
      return "YES";
  }
  else
    return "NO";
}

static int iTreeSetShowToggleAttrib(Ihandle* ih, const char* value)
{
  if (iupStrEqualNoCase(value, "3STATE"))
    ih->data->show_toggle = 2;
  else if (iupStrBoolean(value))
    ih->data->show_toggle = 1;
  else
    ih->data->show_toggle = 0;

  return 0;
}

static char* iTreeGetShowDragDropAttrib(Ihandle* ih)
{
  if (ih->data->show_dragdrop)
    return "YES";
  else
    return "NO";
}

static int iTreeSetShowDragDropAttrib(Ihandle* ih, const char* value)
{
  /* valid only before map */
  if (ih->handle)
    return 0;

  if (iupStrBoolean(value))
    ih->data->show_dragdrop = 1;
  else
    ih->data->show_dragdrop = 0;

  return 0;
}

static int iTreeSetAddLeafAttrib(Ihandle* ih, int id, const char* value)
{
  if (!ih->handle)  /* do not do the action before map */
    return 0;
  iupdrvTreeAddNode(ih, id, ITREE_LEAF, value, 1);
  return 0;
}

static int iTreeSetAddBranchAttrib(Ihandle* ih, int id, const char* value)
{
  if (!ih->handle)  /* do not do the action before map */
    return 0;
  iupdrvTreeAddNode(ih, id, ITREE_BRANCH, value, 1);
  return 0;
}

static int iTreeSetInsertLeafAttrib(Ihandle* ih, int id, const char* value)
{
  if (!ih->handle)  /* do not do the action before map */
    return 0;
  iupdrvTreeAddNode(ih, id, ITREE_LEAF, value, 0);
  return 0;
}

static int iTreeSetInsertBranchAttrib(Ihandle* ih, int id, const char* value)
{
  if (!ih->handle)  /* do not do the action before map */
    return 0;
  iupdrvTreeAddNode(ih, id, ITREE_BRANCH, value, 0);
  return 0;
}

static char* iTreeGetAddExpandedAttrib(Ihandle* ih)
{
  if (ih->data->add_expanded)
    return "YES";
  else
    return "NO";
}

static int iTreeSetAddExpandedAttrib(Ihandle* ih, const char* value)
{
  if (iupStrBoolean(value))
    ih->data->add_expanded = 1;
  else
    ih->data->add_expanded = 0;

  return 0;
}

static char* iTreeGetCountAttrib(Ihandle* ih)
{
  char* str = iupStrGetMemory(10);
  sprintf(str, "%d", ih->data->node_count);
  return str;
}

static char* iTreeGetTotalChildCountAttrib(Ihandle* ih, int id)
{
  char* str;
  InodeHandle* node_handle = iupTreeGetNode(ih, id);
  if (!node_handle)
    return NULL;

  str = iupStrGetMemory(10);
  sprintf(str, "%d", iupdrvTreeTotalChildCount(ih, node_handle));
  return str;
}

static char* iTreeGetUserDataAttrib(Ihandle* ih, int id)
{
  if (id >= 0 && id < ih->data->node_count)
    return ih->data->node_cache[id].userdata;
  else if (id == IUP_INVALID_ID && ih->data->node_count!=0)
  {
    InodeHandle* node_handle = iupdrvTreeGetFocusNode(ih);
    id = iupTreeFindNodeId(ih, node_handle);
    if (id >= 0 && id < ih->data->node_count)
      return ih->data->node_cache[id].userdata;
  }
  return NULL;
}

static int iTreeSetUserDataAttrib(Ihandle* ih, int id, const char* value)
{
  if (id >= 0 && id < ih->data->node_count)
    ih->data->node_cache[id].userdata = (void*)value;
  else if (id == IUP_INVALID_ID && ih->data->node_count!=0)
  {
    InodeHandle* node_handle = iupdrvTreeGetFocusNode(ih);
    id = iupTreeFindNodeId(ih, node_handle);
    if (id >= 0 && id < ih->data->node_count)
      ih->data->node_cache[id].userdata = (void*)value;
  }
  return 0;
}


/*************************************************************************/

static int iTreeCreateMethod(Ihandle* ih, void **params)
{
  (void)params;

  ih->data = iupALLOCCTRLDATA();

  IupSetAttribute(ih, "RASTERSIZE", "400x200");
  IupSetAttribute(ih, "EXPAND", "YES");

  ih->data->add_expanded = 1;
  ih->data->node_cache_max = 20;
  ih->data->node_cache = calloc(ih->data->node_cache_max, sizeof(InodeData));

  return IUP_NOERROR;
}

static void iTreeDestroyMethod(Ihandle* ih)
{
  if (ih->data->node_cache)
    free(ih->data->node_cache);
}

/*************************************************************************/

Ihandle* IupTree(void)
{
  return IupCreate("tree");
}

Iclass* iupTreeNewClass(void)
{
  Iclass* ic = iupClassNew(NULL);

  ic->name = "tree";
  ic->format = NULL; /* no parameters */
  ic->nativetype = IUP_TYPECONTROL;
  ic->childtype = IUP_CHILDNONE;
  ic->is_interactive = 1;
  ic->has_attrib_id = 1;   /* has attributes with IDs that must be parsed */

  /* Class functions */
  ic->New = iupTreeNewClass;
  ic->Create = iTreeCreateMethod;
  ic->LayoutUpdate = iupdrvBaseLayoutUpdateMethod;
  ic->Destroy = iTreeDestroyMethod;

  /* Callbacks */
  iupClassRegisterCallback(ic, "TOGGLEVALUE_CB",    "ii");
  iupClassRegisterCallback(ic, "SELECTION_CB",      "ii");
  iupClassRegisterCallback(ic, "MULTISELECTION_CB", "Ii");
  iupClassRegisterCallback(ic, "MULTIUNSELECTION_CB", "Ii");
  iupClassRegisterCallback(ic, "BRANCHOPEN_CB",     "i");
  iupClassRegisterCallback(ic, "BRANCHCLOSE_CB",    "i");
  iupClassRegisterCallback(ic, "EXECUTELEAF_CB",    "i");
  iupClassRegisterCallback(ic, "SHOWRENAME_CB",     "i");
  iupClassRegisterCallback(ic, "RENAME_CB",         "is");
  iupClassRegisterCallback(ic, "DRAGDROP_CB",       "iiii");
  iupClassRegisterCallback(ic, "RIGHTCLICK_CB",     "i");
  iupClassRegisterCallback(ic, "MOTION_CB", "iis");
  iupClassRegisterCallback(ic, "BUTTON_CB", "iiiis");

  /* Common Callbacks */
  iupBaseRegisterCommonCallbacks(ic);

  /* Common */
  iupBaseRegisterCommonAttrib(ic);

  /* Visual */
  iupBaseRegisterVisualAttrib(ic);

  /* Drag&Drop */
  iupdrvRegisterDragDropAttrib(ic);

  /* IupTree Attributes - GENERAL */
  iupClassRegisterAttribute(ic, "SHOWDRAGDROP", iTreeGetShowDragDropAttrib, iTreeSetShowDragDropAttrib, NULL, NULL, IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "SHOWRENAME",   iTreeGetShowRenameAttrib,   iTreeSetShowRenameAttrib,   NULL, NULL, IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "SHOWTOGGLE",   iTreeGetShowToggleAttrib,   iTreeSetShowToggleAttrib,   NULL, NULL, IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "ADDEXPANDED",  iTreeGetAddExpandedAttrib,  iTreeSetAddExpandedAttrib,  IUPAF_SAMEASSYSTEM, "YES", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "COUNT",        iTreeGetCountAttrib, NULL, NULL, NULL, IUPAF_NO_DEFAULTVALUE|IUPAF_READONLY|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "LASTADDNODE", NULL, NULL, IUPAF_SAMEASSYSTEM, NULL, IUPAF_READONLY|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "ADDROOT", NULL, NULL, IUPAF_SAMEASSYSTEM, "YES", IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "DROPEQUALDRAG", NULL, NULL, IUPAF_SAMEASSYSTEM, NULL, IUPAF_NO_INHERIT);
                                               
  /* IupTree Attributes - MARKS */
  iupClassRegisterAttribute(ic, "CTRL",  NULL, iTreeSetCtrlAttrib,  NULL, NULL, IUPAF_NOT_MAPPED);
  iupClassRegisterAttribute(ic, "SHIFT", NULL, iTreeSetShiftAttrib, NULL, NULL, IUPAF_NOT_MAPPED);
  iupClassRegisterAttribute(ic, "MARKMODE",  iTreeGetMarkModeAttrib, iTreeSetMarkModeAttrib,  IUPAF_SAMEASSYSTEM, "SINGLE", IUPAF_NOT_MAPPED);

  /* IupTree Attributes - ACTION */
  iupClassRegisterAttributeId(ic, "ADDLEAF",   NULL, iTreeSetAddLeafAttrib,   IUPAF_NOT_MAPPED|IUPAF_WRITEONLY|IUPAF_NO_INHERIT);
  iupClassRegisterAttributeId(ic, "ADDBRANCH", NULL, iTreeSetAddBranchAttrib, IUPAF_NOT_MAPPED|IUPAF_WRITEONLY|IUPAF_NO_INHERIT);
  iupClassRegisterAttributeId(ic, "INSERTLEAF",   NULL, iTreeSetInsertLeafAttrib,   IUPAF_NOT_MAPPED|IUPAF_WRITEONLY|IUPAF_NO_INHERIT);
  iupClassRegisterAttributeId(ic, "INSERTBRANCH", NULL, iTreeSetInsertBranchAttrib, IUPAF_NOT_MAPPED|IUPAF_WRITEONLY|IUPAF_NO_INHERIT);

  /* IupTree Attributes - NODES */
  iupClassRegisterAttributeId(ic, "TOTALCHILDCOUNT", iTreeGetTotalChildCountAttrib,   NULL, IUPAF_READONLY|IUPAF_NO_INHERIT);
  iupClassRegisterAttributeId(ic, "USERDATA", iTreeGetUserDataAttrib, iTreeSetUserDataAttrib, IUPAF_NO_STRING|IUPAF_NO_INHERIT);
  
  /* Default node images */
  iTreeInitializeImages();

  iupdrvTreeInitClass(ic);

  return ic;
}

/********************************************************************************************/

void IupTreeSetAttribute(Ihandle* ih, const char* a, int id, const char* v)
{
  IupSetAttributeId(ih, a, id, v);
}

void IupTreeStoreAttribute(Ihandle* ih, const char* a, int id, const char* v)
{
  IupStoreAttributeId(ih, a, id, v);
}

char* IupTreeGetAttribute(Ihandle* ih, const char* a, int id)
{
  return IupGetAttributeId(ih, a, id);
}

int IupTreeGetInt(Ihandle* ih, const char* a, int id)
{
  return IupGetIntId(ih, a, id);
}

float IupTreeGetFloat(Ihandle* ih, const char* a, int id)
{
  return IupGetFloatId(ih, a, id);
}

void IupTreeSetfAttribute(Ihandle* ih, const char* a, int id, const char* f, ...)
{
  int size;
  char* v = iupStrGetLargeMem(&size);
  va_list arglist;
  va_start(arglist, f);
  vsnprintf(v, size, f, arglist);
  va_end(arglist);
  IupStoreAttributeId(ih, a, id, v);
}

void IupTreeSetAttributeHandle(Ihandle* ih, const char* a, int id, Ihandle* ih_named)
{
  char* attr = iupStrGetMemory(50);
  sprintf(attr, "%s%d", a, id);
  IupSetAttributeHandle(ih, attr, ih_named);
}

/************************************************************************************/

int IupTreeSetUserId(Ihandle* ih, int id, void* userdata)
{
  iupASSERT(iupObjectCheck(ih));
  if (!iupObjectCheck(ih))
    return 0;

  if (id >= 0 && id < ih->data->node_count)
  {
    ih->data->node_cache[id].userdata = userdata;
    return 1;
  }

  return 0;
}

int IupTreeGetId(Ihandle* ih, void *userdata)
{
  iupASSERT(iupObjectCheck(ih));
  if (!iupObjectCheck(ih))
    return -1;

  return iTreeFindUserDataId(ih, userdata);
}

void* IupTreeGetUserId(Ihandle* ih, int id)
{
  iupASSERT(iupObjectCheck(ih));
  if (!iupObjectCheck(ih))
    return NULL;

  if (id >= 0 && id < ih->data->node_count)
    return ih->data->node_cache[id].userdata;

  return NULL;
}
