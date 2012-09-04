/** \file
 * \brief List Control
 *
 * See Copyright Notice in "iup.h"
 */

#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <memory.h>
#include <stdarg.h>

#include "iup.h"
#include "iupcbs.h"

#include "iup_object.h"
#include "iup_layout.h"
#include "iup_attrib.h"
#include "iup_str.h"
#include "iup_drv.h"
#include "iup_drvfont.h"
#include "iup_mask.h"
#include "iup_key.h"
#include "iup_image.h"
#include "iup_list.h"

#include "iupgtk_drv.h"


enum
{
  IUPGTK_LIST_IMAGE,  /* "pixbuf" */
  IUPGTK_LIST_TEXT,  /* "text" */
  IUPGTK_LIST_LAST_DATA  /* used as a count */
};

static void gtkListSelectionChanged(GtkTreeSelection* selection, Ihandle* ih);
static void gtkListComboBoxChanged(GtkComboBox* widget, Ihandle* ih);


void iupdrvListAddItemSpace(Ihandle* ih, int *h)
{
  (void)ih;
  *h += 3;
}

void iupdrvListAddBorders(Ihandle* ih, int *x, int *y)
{
  int border_size = 2*5;
  (*x) += border_size;
  (*y) += border_size;

  if (ih->data->is_dropdown)
  {
#ifdef HILDON
    (*x) += 9; /* extra space for the dropdown button */
#else
    (*x) += 5; /* extra space for the dropdown button */
#endif

    if (ih->data->has_editbox)
      (*x) += 5; /* another extra space for the dropdown button */
    else
    {
      (*y) += 4; /* extra padding space */
      (*x) += 4; /* extra padding space */
    }
  }
  else
  {
    if (ih->data->has_editbox)
      (*y) += 2*3; /* internal border between editbox and list */
  }
}

static int gtkListConvertXYToPos(Ihandle* ih, int x, int y)
{
  if (!ih->data->is_dropdown)
  {
    GtkTreePath* path;
    if (gtk_tree_view_get_dest_row_at_pos((GtkTreeView*)ih->handle, x, y, &path, NULL))
    {
      int* indices = gtk_tree_path_get_indices(path);
      int pos = indices[0]+1;  /* IUP starts at 1 */
      gtk_tree_path_free (path);
      return pos;
    }
  }

  return -1;
}

static GtkTreeModel* gtkListGetModel(Ihandle* ih)
{
  if (ih->data->is_dropdown)
    return gtk_combo_box_get_model((GtkComboBox*)ih->handle);
  else
    return gtk_tree_view_get_model((GtkTreeView*)ih->handle);
}

int iupdrvListGetCount(Ihandle* ih)
{
  GtkTreeModel *model = gtkListGetModel(ih);
  return gtk_tree_model_iter_n_children(model, NULL);
}

void iupdrvListAppendItem(Ihandle* ih, const char* value)
{
  GtkTreeModel *model = gtkListGetModel(ih);
  GtkTreeIter iter;
  gtk_list_store_append(GTK_LIST_STORE(model), &iter);

  gtk_list_store_set(GTK_LIST_STORE(model), &iter, IUPGTK_LIST_TEXT, iupgtkStrConvertToUTF8(value), -1);
  gtk_list_store_set(GTK_LIST_STORE(model), &iter, IUPGTK_LIST_IMAGE, NULL, -1);
}

void iupdrvListInsertItem(Ihandle* ih, int pos, const char* value)
{
  GtkTreeModel *model = gtkListGetModel(ih);
  GtkTreeIter iter;
  gtk_list_store_insert(GTK_LIST_STORE(model), &iter, pos);

  gtk_list_store_set(GTK_LIST_STORE(model), &iter, IUPGTK_LIST_TEXT, iupgtkStrConvertToUTF8(value), -1);
  gtk_list_store_set(GTK_LIST_STORE(model), &iter, IUPGTK_LIST_IMAGE, NULL, -1);

  iupListUpdateOldValue(ih, pos, 0);
}

void iupdrvListRemoveItem(Ihandle* ih, int pos)
{
  GtkTreeModel *model = gtkListGetModel(ih);
  GtkTreeIter iter;
  if (gtk_tree_model_iter_nth_child(model, &iter, NULL, pos))
  {
    if (ih->data->is_dropdown && !ih->data->has_editbox)
    {
      /* must check if removing the current item */
      int curpos = gtk_combo_box_get_active((GtkComboBox*)ih->handle);
      if (pos == curpos)
      {
        if (curpos > 0) 
          curpos--;
        else 
        {
          curpos=1;
          if (iupdrvListGetCount(ih)==1)
            curpos = -1; /* remove the selection */
        }

        g_signal_handlers_block_by_func(G_OBJECT(ih->handle), G_CALLBACK(gtkListComboBoxChanged), ih);
        gtk_combo_box_set_active((GtkComboBox*)ih->handle, curpos);
        g_signal_handlers_unblock_by_func(G_OBJECT(ih->handle), G_CALLBACK(gtkListComboBoxChanged), ih);
      }
    }

    gtk_list_store_remove(GTK_LIST_STORE(model), &iter);

    iupListUpdateOldValue(ih, pos, 1);
  }
}

void iupdrvListRemoveAllItems(Ihandle* ih)
{
  GtkTreeModel *model = gtkListGetModel(ih);
  gtk_list_store_clear(GTK_LIST_STORE(model));
}


/*********************************************************************************/


static int gtkListSetStandardFontAttrib(Ihandle* ih, const char* value)
{
  iupdrvSetStandardFontAttrib(ih, value);

  if (ih->handle)
  {
    if (ih->data->is_dropdown)
    {
      GtkCellRenderer* renderer = (GtkCellRenderer*)iupAttribGet(ih, "_IUPGTK_RENDERER");
      if (renderer)
      {
        g_object_set(G_OBJECT(renderer), "font-desc", (PangoFontDescription*)iupgtkGetPangoFontDescAttrib(ih), NULL);
        iupgtkFontUpdateObjectPangoLayout(ih, G_OBJECT(renderer));
      }
    }

    if (ih->data->has_editbox)
    {
      GtkEntry* entry = (GtkEntry*)iupAttribGet(ih, "_IUPGTK_ENTRY");
      gtk_widget_modify_font((GtkWidget*)entry, (PangoFontDescription*)iupgtkGetPangoFontDescAttrib(ih));
      iupgtkFontUpdatePangoLayout(ih, gtk_entry_get_layout(entry));
    }
  }
  return 1;
}

static char* gtkListGetIdValueAttrib(Ihandle* ih, int id)
{
  int pos = iupListGetPos(ih, id);
  if (pos >= 0)
  {
    GtkTreeIter iter;
    GtkTreeModel* model = gtkListGetModel(ih);
    if (gtk_tree_model_iter_nth_child(model, &iter, NULL, pos))
    {
      gchar *text = NULL;
      gtk_tree_model_get(model, &iter, IUPGTK_LIST_TEXT, &text, -1);
      if (text)
      {
        char* ret_str = iupStrGetMemoryCopy(iupgtkStrConvertFromUTF8(text));
        g_free(text);
        return ret_str;
      }
    }
  }
  return NULL;
}


static void gtkComboBoxChildrenToggleCb(GtkWidget *widget, gpointer client_data)
{
  if (GTK_IS_TOGGLE_BUTTON(widget))
  {
    GtkWidget** toggle = (GtkWidget**)client_data;
    *toggle = widget;
  }
}

static void gtkComboBoxChildrenBgColorCb(GtkWidget *widget, gpointer client_data)
{
  GdkColor* c = (GdkColor*)client_data;
  iupgtkBaseSetBgColor(widget, (unsigned char)c->red, (unsigned char)c->green, (unsigned char)c->blue);
}

static int gtkListSetBgColorAttrib(Ihandle* ih, const char* value)
{
  unsigned char r, g, b;

  GtkScrolledWindow* scrolled_window = (GtkScrolledWindow*)iupAttribGet(ih, "_IUP_EXTRAPARENT");
  if (scrolled_window && !ih->data->is_dropdown)
  {
    /* ignore given value, must use only from parent for the scrollbars */
    char* parent_value = iupBaseNativeParentGetBgColor(ih);

    if (iupStrToRGB(parent_value, &r, &g, &b))
    {
      GtkWidget* sb;

      if (!GTK_IS_SCROLLED_WINDOW(scrolled_window))
        scrolled_window = (GtkScrolledWindow*)iupAttribGet(ih, "_IUPGTK_SCROLLED_WINDOW");

      iupgtkBaseSetBgColor((GtkWidget*)scrolled_window, r, g, b);

#if GTK_CHECK_VERSION(2, 8, 0)
      sb = gtk_scrolled_window_get_hscrollbar(scrolled_window);
      if (sb) iupgtkBaseSetBgColor(sb, r, g, b);

      sb = gtk_scrolled_window_get_vscrollbar(scrolled_window);
      if (sb) iupgtkBaseSetBgColor(sb, r, g, b);
#endif
    }
  }

  if (!iupStrToRGB(value, &r, &g, &b))
    return 0;

  if (ih->data->has_editbox)
  {
    GtkWidget* entry = (GtkWidget*)iupAttribGet(ih, "_IUPGTK_ENTRY");
    iupgtkBaseSetBgColor(entry, r, g, b);
  }

  if (ih->data->is_dropdown)
  {
    GdkColor c;
    GtkContainer *container = (GtkContainer*)ih->handle;
    c.blue = b;
    c.green = g;
    c.red = r;
    gtk_container_forall(container, gtkComboBoxChildrenBgColorCb, &c);

    if (!ih->data->has_editbox)
    {
      GtkWidget* box = (GtkWidget*)iupAttribGet(ih, "_IUP_EXTRAPARENT");
      if (box)
        iupgtkBaseSetBgColor(box, r, g, b);
    }
  }

  /* TODO: this test is not necessary, 
     but when dropdown=yes the color is not set for the popup menu, 
     and the result is very weird. So we avoid setting it when dropdown=yes
     until we figure out how to set for the popup menu. */
  if (!ih->data->is_dropdown)
  {
    GtkCellRenderer* renderer = (GtkCellRenderer*)iupAttribGet(ih, "_IUPGTK_RENDERER");
    if (renderer)
    {
      GdkColor color;
      iupgdkColorSet(&color, r, g, b);
      g_object_set(G_OBJECT(renderer), "cell-background-gdk", &color, NULL);
    }
  }

  return iupdrvBaseSetBgColorAttrib(ih, value);
}

static int gtkListSetFgColorAttrib(Ihandle* ih, const char* value)
{
  unsigned char r, g, b;
  if (!iupStrToRGB(value, &r, &g, &b))
    return 0;

  iupgtkBaseSetFgColor(ih->handle, r, g, b);

  if (ih->data->has_editbox)
  {
    GtkWidget* entry = (GtkWidget*)iupAttribGet(ih, "_IUPGTK_ENTRY");
    iupgtkBaseSetFgColor(entry, r, g, b);
  }

  /* TODO: see comment in BGCOLOR */
  if (!ih->data->is_dropdown)
  {
    GtkCellRenderer* renderer = (GtkCellRenderer*)iupAttribGet(ih, "_IUPGTK_RENDERER");
    if (renderer)
    {
      GdkColor color;
      iupgdkColorSet(&color, r, g, b);
      g_object_set(G_OBJECT(renderer), "foreground-gdk", &color, NULL);
    }
  }

  return 1;
}

static char* gtkListGetValueAttrib(Ihandle* ih)
{
  if (ih->data->has_editbox)
  {
    GtkEntry* entry = (GtkEntry*)iupAttribGet(ih, "_IUPGTK_ENTRY");
    return iupStrGetMemoryCopy(iupgtkStrConvertFromUTF8(gtk_entry_get_text(entry)));
  }
  else 
  {
    if (ih->data->is_dropdown)
    {
      int pos = gtk_combo_box_get_active((GtkComboBox*)ih->handle);
      char* str = iupStrGetMemory(50);
      sprintf(str, "%d", pos+1);  /* IUP starts at 1 */
      return str;
    }
    else
    {
      GtkTreeSelection* selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(ih->handle));
      if (!ih->data->is_multiple)
      {
        GtkTreeIter iter;
        GtkTreeModel* tree_model;
        if (gtk_tree_selection_get_selected(selection, &tree_model, &iter))
        {
          char* str;
          GtkTreePath *path = gtk_tree_model_get_path(tree_model, &iter);
          int* indices = gtk_tree_path_get_indices(path);
          str = iupStrGetMemory(50);
          sprintf(str, "%d", indices[0]+1);  /* IUP starts at 1 */
          gtk_tree_path_free (path);
          return str;
        }
      }
      else
      {
        GList *il, *list = gtk_tree_selection_get_selected_rows(selection, NULL);
        int count = iupdrvListGetCount(ih);
        char* str = iupStrGetMemory(count+1);
        memset(str, '-', count);
        str[count]=0;
        for (il=list; il; il=il->next)
        {
          GtkTreePath* path = (GtkTreePath*)il->data;
          int* indices = gtk_tree_path_get_indices(path);
          str[indices[0]] = '+';
          gtk_tree_path_free(path);
        }
        g_list_free(list);
        return str;
      }
    }
  }

  return NULL;
}

static int gtkListSetValueAttrib(Ihandle* ih, const char* value)
{
  if (ih->data->has_editbox)
  {
    GtkEntry* entry = (GtkEntry*)iupAttribGet(ih, "_IUPGTK_ENTRY");
    if (!value) value = "";
    iupAttribSetStr(ih, "_IUPGTK_DISABLE_TEXT_CB", "1");
    gtk_entry_set_text(entry, iupgtkStrConvertToUTF8(value));
    iupAttribSetStr(ih, "_IUPGTK_DISABLE_TEXT_CB", NULL);
  }
  else 
  {
    if (ih->data->is_dropdown)
    {
      int pos;
      GtkTreeModel *model = gtkListGetModel(ih);
      g_signal_handlers_block_by_func(G_OBJECT(ih->handle), G_CALLBACK(gtkListComboBoxChanged), ih);
      if (iupStrToInt(value, &pos)==1 && 
          (pos>0 && pos<=gtk_tree_model_iter_n_children(model, NULL)))
      {
        gtk_combo_box_set_active((GtkComboBox*)ih->handle, pos-1);    /* IUP starts at 1 */
        iupAttribSetInt(ih, "_IUPLIST_OLDVALUE", pos);
      }
      else
      {
        gtk_combo_box_set_active((GtkComboBox*)ih->handle, -1);    /* none */
        iupAttribSetStr(ih, "_IUPLIST_OLDVALUE", NULL);
      }
      g_signal_handlers_unblock_by_func(G_OBJECT(ih->handle), G_CALLBACK(gtkListComboBoxChanged), ih);
    }
    else
    {
      GtkTreeSelection* selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(ih->handle));
      if (!ih->data->is_multiple)
      {
        int pos;
        g_signal_handlers_block_by_func(G_OBJECT(selection), G_CALLBACK(gtkListSelectionChanged), ih);
        if (iupStrToInt(value, &pos)==1)
        {
          GtkTreePath* path = gtk_tree_path_new_from_indices(pos-1, -1);   /* IUP starts at 1 */
          gtk_tree_selection_select_path(selection, path);
          gtk_tree_path_free(path);
          iupAttribSetInt(ih, "_IUPLIST_OLDVALUE", pos);
        }
        else
        {
          gtk_tree_selection_unselect_all(selection);
          iupAttribSetStr(ih, "_IUPLIST_OLDVALUE", NULL);
        }
        g_signal_handlers_unblock_by_func(G_OBJECT(selection), G_CALLBACK(gtkListSelectionChanged), ih);
      }
      else
      {
        /* User has changed a multiple selection on a simple list. */
	      int i, len, count;

        g_signal_handlers_block_by_func(G_OBJECT(selection), G_CALLBACK(gtkListSelectionChanged), ih);

        /* Clear all selections */
        gtk_tree_selection_unselect_all(selection);

        if (!value)
        {
          iupAttribSetStr(ih, "_IUPLIST_OLDVALUE", NULL);
          return 0;
        }

	      len = strlen(value);
        count = iupdrvListGetCount(ih);
        if (len < count) 
          count = len;

        /* update selection list */
        for (i = 0; i<count; i++)
        {
          if (value[i]=='+')
          {
            GtkTreePath* path = gtk_tree_path_new_from_indices(i, -1);
            gtk_tree_selection_select_path(selection, path);
            gtk_tree_path_free(path);
          }
        }
        iupAttribStoreStr(ih, "_IUPLIST_OLDVALUE", value);
        g_signal_handlers_unblock_by_func(G_OBJECT(selection), G_CALLBACK(gtkListSelectionChanged), ih);
      }
    }
  }

  return 0;
}

static int gtkListSetShowDropdownAttrib(Ihandle* ih, const char* value)
{
  if (ih->data->is_dropdown)
  {
    if (iupStrBoolean(value))
      gtk_combo_box_popup((GtkComboBox*)ih->handle); 
    else
      gtk_combo_box_popdown((GtkComboBox*)ih->handle); 
  }
  return 0;
}

static int gtkListSetTopItemAttrib(Ihandle* ih, const char* value)
{
  if (!ih->data->is_dropdown)
  {
    int pos = 1;
    if (iupStrToInt(value, &pos))
    {
      GtkTreePath* path = gtk_tree_path_new_from_indices(pos-1, -1);   /* IUP starts at 1 */
      gtk_tree_view_scroll_to_cell((GtkTreeView*)ih->handle, path, NULL, FALSE, 0, 0);
      gtk_tree_path_free(path);
    }
  }
  return 0;
}

static int gtkListSetSpacingAttrib(Ihandle* ih, const char* value)
{
  if (ih->data->is_dropdown)
    return 0;

  if (!iupStrToInt(value, &ih->data->spacing))
    ih->data->spacing = 0;

  if (ih->handle)
  {
    GtkCellRenderer* renderer = (GtkCellRenderer*)iupAttribGet(ih, "_IUPGTK_RENDERER");
    if (renderer)
      g_object_set(G_OBJECT(renderer), "xpad", ih->data->spacing, 
                                       "ypad", ih->data->spacing, 
                                       NULL);
    return 0;
  }
  else
    return 1; /* store until not mapped, when mapped will be set again */
}

static int gtkListSetPaddingAttrib(Ihandle* ih, const char* value)
{
  if (!ih->data->has_editbox)
    return 0;

  iupStrToIntInt(value, &ih->data->horiz_padding, &ih->data->vert_padding, 'x');
  if (ih->handle)
  {
#if GTK_CHECK_VERSION(2, 10, 0)
    GtkEntry* entry;
    GtkBorder border;
    border.bottom = border.top = ih->data->vert_padding;
    border.left = border.right = ih->data->horiz_padding;
    entry = (GtkEntry*)iupAttribGet(ih, "_IUPGTK_ENTRY");
    gtk_entry_set_inner_border(entry, &border);
#endif
    return 0;
  }
  else
    return 1; /* store until not mapped, when mapped will be set again */
}

static int gtkListSetSelectionAttrib(Ihandle* ih, const char* value)
{
  int start=1, end=1;
  GtkEntry* entry;
  if (!ih->data->has_editbox)
    return 0;
  if (!value)
    return 0;

  entry = (GtkEntry*)iupAttribGet(ih, "_IUPGTK_ENTRY");
  if (!value || iupStrEqualNoCase(value, "NONE"))
  {
    gtk_editable_select_region(GTK_EDITABLE(entry), 0, 0);
    return 0;
  }

  if (iupStrEqualNoCase(value, "ALL"))
  {
    gtk_editable_select_region(GTK_EDITABLE(entry), 0, -1);
    return 0;
  }

  if (iupStrToIntInt(value, &start, &end, ':')!=2) 
    return 0;

  if(start<1 || end<1) 
    return 0;

  start--; /* IUP starts at 1 */
  end--;

  gtk_editable_select_region(GTK_EDITABLE(entry), start, end);

  return 0;
}

static char* gtkListGetSelectionAttrib(Ihandle* ih)
{
  char *str;
  int start, end;
  GtkEntry* entry;
  if (!ih->data->has_editbox)
    return NULL;

  entry = (GtkEntry*)iupAttribGet(ih, "_IUPGTK_ENTRY");
  if (gtk_editable_get_selection_bounds(GTK_EDITABLE(entry), &start, &end))
  {
    start++; /* IUP starts at 1 */
    end++;
    str = iupStrGetMemory(100);
    sprintf(str, "%d:%d", (int)start, (int)end);
    return str;
  }

  return NULL;
}

static int gtkListSetSelectionPosAttrib(Ihandle* ih, const char* value)
{
  int start=0, end=0;
  GtkEntry* entry;
  if (!ih->data->has_editbox)
    return 0;
  if (!value)
    return 0;

  entry = (GtkEntry*)iupAttribGet(ih, "_IUPGTK_ENTRY");
  if (!value || iupStrEqualNoCase(value, "NONE"))
  {
    gtk_editable_select_region(GTK_EDITABLE(entry), 0, 0);
    return 0;
  }

  if (iupStrEqualNoCase(value, "ALL"))
  {
    gtk_editable_select_region(GTK_EDITABLE(entry), 0, -1);
    return 0;
  }

  if (iupStrToIntInt(value, &start, &end, ':')!=2) 
    return 0;

  if(start<0 || end<0) 
    return 0;

  gtk_editable_select_region(GTK_EDITABLE(entry), start, end);

  return 0;
}

static char* gtkListGetSelectionPosAttrib(Ihandle* ih)
{
  int start, end;
  char *str;
  GtkEntry* entry;
  if (!ih->data->has_editbox)
    return NULL;

  entry = (GtkEntry*)iupAttribGet(ih, "_IUPGTK_ENTRY");
  if (gtk_editable_get_selection_bounds(GTK_EDITABLE(entry), &start, &end))
  {
    str = iupStrGetMemory(100);
    sprintf(str, "%d:%d", (int)start, (int)end);
    return str;
  }

  return NULL;
}

static int gtkListSetSelectedTextAttrib(Ihandle* ih, const char* value)
{
  int start, end;
  GtkEntry* entry;
  if (!ih->data->has_editbox)
    return 0;
  if (!value)
    return 0;

  entry = (GtkEntry*)iupAttribGet(ih, "_IUPGTK_ENTRY");
  if (gtk_editable_get_selection_bounds(GTK_EDITABLE(entry), &start, &end))
  {
    /* disable callbacks */
    iupAttribSetStr(ih, "_IUPGTK_DISABLE_TEXT_CB", "1");
    gtk_editable_delete_selection(GTK_EDITABLE(entry));
    gtk_editable_insert_text(GTK_EDITABLE(entry), iupgtkStrConvertToUTF8(value), -1, &start);
    iupAttribSetStr(ih, "_IUPGTK_DISABLE_TEXT_CB", NULL);
  }

  return 0;
}

static char* gtkListGetSelectedTextAttrib(Ihandle* ih)
{
  int start, end;
  GtkEntry* entry;
  if (!ih->data->has_editbox)
    return NULL;

  entry = (GtkEntry*)iupAttribGet(ih, "_IUPGTK_ENTRY");
  if (gtk_editable_get_selection_bounds(GTK_EDITABLE(entry), &start, &end))
  {
    char* selectedtext = gtk_editable_get_chars(GTK_EDITABLE(entry), start, end);
    char* str = iupStrGetMemoryCopy(iupgtkStrConvertFromUTF8(selectedtext));
    g_free(selectedtext);
    return str;
  }

  return NULL;
}

static int gtkListSetCaretAttrib(Ihandle* ih, const char* value)
{
  int pos = 1;
  GtkEntry* entry;
  if (!ih->data->has_editbox)
    return 0;
  if (!value)
    return 0;

  sscanf(value,"%i",&pos);
  pos--; /* IUP starts at 1 */
  if (pos < 0) pos = 0;

  entry = (GtkEntry*)iupAttribGet(ih, "_IUPGTK_ENTRY");
  gtk_editable_set_position(GTK_EDITABLE(entry), pos);

  return 0;
}

static char* gtkListGetCaretAttrib(Ihandle* ih)
{
  if (ih->data->has_editbox)
  {
    char* str = iupStrGetMemory(50);
    GtkEntry* entry = (GtkEntry*)iupAttribGet(ih, "_IUPGTK_ENTRY");
    int pos = gtk_editable_get_position(GTK_EDITABLE(entry));
    pos++; /* IUP starts at 1 */
    sprintf(str, "%d", (int)pos);
    return str;
  }
  else
    return NULL;
}

static int gtkListSetCaretPosAttrib(Ihandle* ih, const char* value)
{
  int pos = 0;
  GtkEntry* entry;
  if (!ih->data->has_editbox)
    return 0;
  if (!value)
    return 0;

  sscanf(value,"%i",&pos);
  if (pos < 0) pos = 0;

  entry = (GtkEntry*)iupAttribGet(ih, "_IUPGTK_ENTRY");
  gtk_editable_set_position(GTK_EDITABLE(entry), pos);

  return 0;
}

static char* gtkListGetCaretPosAttrib(Ihandle* ih)
{
  if (ih->data->has_editbox)
  {
    char* str = iupStrGetMemory(50);
    GtkEntry* entry = (GtkEntry*)iupAttribGet(ih, "_IUPGTK_ENTRY");
    int pos = gtk_editable_get_position(GTK_EDITABLE(entry));
    sprintf(str, "%d", (int)pos);
    return str;
  }
  else
    return NULL;
}

static int gtkListSetScrollToAttrib(Ihandle* ih, const char* value)
{
  int pos = 1;
  GtkEntry* entry;
  if (!ih->data->has_editbox)
    return 0;
  if (!value)
    return 0;

  sscanf(value,"%i",&pos);
  if (pos < 1) pos = 1;
  pos--;  /* return to GTK reference */

  entry = (GtkEntry*)iupAttribGet(ih, "_IUPGTK_ENTRY");
  gtk_editable_set_position(GTK_EDITABLE(entry), pos);

  return 0;
}

static int gtkListSetScrollToPosAttrib(Ihandle* ih, const char* value)
{
  int pos = 0;
  GtkEntry* entry;
  if (!ih->data->has_editbox)
    return 0;
  if (!value)
    return 0;

  sscanf(value,"%i",&pos);
  if (pos < 0) pos = 0;

  entry = (GtkEntry*)iupAttribGet(ih, "_IUPGTK_ENTRY");
  gtk_editable_set_position(GTK_EDITABLE(entry), pos);

  return 0;
}

static int gtkListSetInsertAttrib(Ihandle* ih, const char* value)
{
  gint pos;
  GtkEntry* entry;
  if (!ih->data->has_editbox)
    return 0;
  if (!value)
    return 0;

  iupAttribSetStr(ih, "_IUPGTK_DISABLE_TEXT_CB", "1");  /* disable callbacks */
  entry = (GtkEntry*)iupAttribGet(ih, "_IUPGTK_ENTRY");
  pos = gtk_editable_get_position(GTK_EDITABLE(entry));
  gtk_editable_insert_text(GTK_EDITABLE(entry), iupgtkStrConvertToUTF8(value), -1, &pos);
  iupAttribSetStr(ih, "_IUPGTK_DISABLE_TEXT_CB", NULL);

  return 0;
}

static int gtkListSetAppendAttrib(Ihandle* ih, const char* value)
{
  if (ih->data->has_editbox)
  {
    GtkEntry* entry = (GtkEntry*)iupAttribGet(ih, "_IUPGTK_ENTRY");
    gint pos = strlen(gtk_entry_get_text(entry))+1;
    iupAttribSetStr(ih, "_IUPGTK_DISABLE_TEXT_CB", "1"); /* disable callbacks */
    gtk_editable_insert_text(GTK_EDITABLE(entry), iupgtkStrConvertToUTF8(value), -1, &pos);
    iupAttribSetStr(ih, "_IUPGTK_DISABLE_TEXT_CB", NULL);
  }
  return 0;
}

static int gtkListSetNCAttrib(Ihandle* ih, const char* value)
{
  if (!ih->data->has_editbox)
    return 0;

  if (!iupStrToInt(value, &ih->data->nc))
    ih->data->nc = INT_MAX;

  if (ih->handle)
  {
    GtkEntry* entry = (GtkEntry*)iupAttribGet(ih, "_IUPGTK_ENTRY");
    gtk_entry_set_max_length(entry, ih->data->nc);
    return 0;
  }
  else
    return 1; /* store until not mapped, when mapped will be set again */
}

static int gtkListSetClipboardAttrib(Ihandle *ih, const char *value)
{
  GtkEntry* entry;
  if (!ih->data->has_editbox)
    return 0;

  /* disable callbacks */
  iupAttribSetStr(ih, "_IUPGTK_DISABLE_TEXT_CB", "1");
  entry = (GtkEntry*)iupAttribGet(ih, "_IUPGTK_ENTRY");
  if (iupStrEqualNoCase(value, "COPY"))
    gtk_editable_copy_clipboard(GTK_EDITABLE(entry));
  else if (iupStrEqualNoCase(value, "CUT"))
    gtk_editable_cut_clipboard(GTK_EDITABLE(entry));
  else if (iupStrEqualNoCase(value, "PASTE"))
    gtk_editable_paste_clipboard(GTK_EDITABLE(entry));
  else if (iupStrEqualNoCase(value, "CLEAR"))
    gtk_editable_delete_selection(GTK_EDITABLE(entry));
  iupAttribSetStr(ih, "_IUPGTK_DISABLE_TEXT_CB", NULL);
  return 0;
}

static int gtkListSetReadOnlyAttrib(Ihandle* ih, const char* value)
{
  GtkEntry* entry;
  if (!ih->data->has_editbox)
    return 0;
  entry = (GtkEntry*)iupAttribGet(ih, "_IUPGTK_ENTRY");
  gtk_editable_set_editable(GTK_EDITABLE(entry), !iupStrBoolean(value));
  return 0;
}

static char* gtkListGetReadOnlyAttrib(Ihandle* ih)
{
  GtkEntry* entry;
  if (!ih->data->has_editbox)
    return NULL;
  entry = (GtkEntry*)iupAttribGet(ih, "_IUPGTK_ENTRY");
  if (!gtk_editable_get_editable(GTK_EDITABLE(entry)))
    return "YES";
  else
    return "NO";
}

static int gtkListSetImageAttrib(Ihandle* ih, int id, const char* value)
{
  GtkTreeModel* model = gtkListGetModel(ih);
  GdkPixbuf* pixImage = iupImageGetImage(value, ih, 0);
  GtkTreeIter iter;
  int pos = iupListGetPos(ih, id);

  if (!ih->data->show_image || !gtk_tree_model_iter_nth_child(model, &iter, NULL, pos))
    return 0;

  gtk_list_store_set(GTK_LIST_STORE(model), &iter, IUPGTK_LIST_IMAGE, pixImage, -1);
  return 1;
}

/*********************************************************************************/


static void gtkListEditMoveCursor(GtkWidget* entry, GtkMovementStep step, gint count, gboolean extend_selection, Ihandle* ih)
{
  int pos;

  IFniii cb = (IFniii)IupGetCallback(ih, "CARET_CB");
  if (!cb) return;

  pos = gtk_editable_get_position(GTK_EDITABLE(entry));

  if (pos != ih->data->last_caret_pos)
  {
    ih->data->last_caret_pos = pos;

    cb(ih, 1, pos+1, pos);
  }

  (void)step;
  (void)count;
  (void)extend_selection;
}

static gboolean gtkListEditKeyPressEvent(GtkWidget* entry, GdkEventKey *evt, Ihandle *ih)
{
  if (iupgtkKeyPressEvent(entry, evt, ih) == TRUE)
    return TRUE;

  if ((evt->keyval == GDK_Up || evt->keyval == GDK_KP_Up) ||
      (evt->keyval == GDK_Prior || evt->keyval == GDK_KP_Page_Up) ||
      (evt->keyval == GDK_Down || evt->keyval == GDK_KP_Down) ||
      (evt->keyval == GDK_Next || evt->keyval == GDK_KP_Page_Down))
  {
    int pos = -1;
    GtkTreeIter iter;
    GtkTreeModel* model = NULL;
    GtkTreeSelection* selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(ih->handle));
    if (gtk_tree_selection_get_selected(selection, &model, &iter))
    {
      GtkTreePath *path = gtk_tree_model_get_path(model, &iter);
      int* indices = gtk_tree_path_get_indices(path);
      pos = indices[0];
      gtk_tree_path_free(path);
    }

    if (pos == -1)
      pos = 0;
    else if (evt->keyval == GDK_Up || evt->keyval == GDK_KP_Up)
    {
      pos--;
      if (pos < 0) pos = 0;
    }
    else if (evt->keyval == GDK_Prior || evt->keyval == GDK_KP_Page_Up)
    {
      pos -= 5;
      if (pos < 0) pos = 0;
    }
    else if (evt->keyval == GDK_Down || evt->keyval == GDK_KP_Down)
    {
      int count = gtk_tree_model_iter_n_children(model, NULL);
      pos++;
      if (pos > count-1) pos = count-1;
    }
    else if (evt->keyval == GDK_Next || evt->keyval == GDK_KP_Page_Down)
    {
      int count = gtk_tree_model_iter_n_children(model, NULL);
      pos += 5;
      if (pos > count-1) pos = count-1;
    }

    if (pos != -1)
    {
      GtkTreePath* path = gtk_tree_path_new_from_indices(pos, -1);
      g_signal_handlers_block_by_func(G_OBJECT(selection), G_CALLBACK(gtkListSelectionChanged), ih);
      gtk_tree_selection_select_path(selection, path);
      g_signal_handlers_unblock_by_func(G_OBJECT(selection), G_CALLBACK(gtkListSelectionChanged), ih);
      gtk_tree_path_free(path);
      iupAttribSetInt(ih, "_IUPLIST_OLDVALUE", pos);

      if (!model) model = gtkListGetModel(ih);

      if (gtk_tree_model_iter_nth_child(model, &iter, NULL, pos))
      {
        gchar *text = NULL;
        gtk_tree_model_get(model, &iter, IUPGTK_LIST_TEXT, &text, -1);
        if (text)
        {
          gtk_entry_set_text((GtkEntry*)entry, text);
          g_free(text);
        }
      }
    }
  }

  return FALSE;
}

static gboolean gtkListEditKeyReleaseEvent(GtkWidget *widget, GdkEventKey *evt, Ihandle *ih)
{
  gtkListEditMoveCursor(widget, 0, 0, 0, ih);
  (void)evt;
  return FALSE;
}

static gboolean gtkListEditButtonEvent(GtkWidget *widget, GdkEventButton *evt, Ihandle *ih)
{
  gtkListEditMoveCursor(widget, 0, 0, 0, ih);
  (void)evt;
  return FALSE;
}

static int gtkListCallEditCb(Ihandle* ih, GtkEditable *editable, const char* insert_value, int len, int start, int end)
{
  char *new_value, *value;
  int ret = -1, key = 0;

  IFnis cb = (IFnis)IupGetCallback(ih, "EDIT_CB");
  if (!cb && !ih->data->mask)
    return -1; /* continue */

  value = iupStrGetMemoryCopy(iupgtkStrConvertFromUTF8(gtk_entry_get_text(GTK_ENTRY(editable))));

  if (!insert_value)
  {
    new_value = iupStrDup(value);
    if (end<0) end = strlen(value)+1;
    iupStrRemove(new_value, start, end, 1);
  }
  else
  {
    if (!value)
      new_value = iupStrDup(insert_value);
    else
    {
      if (len < end-start)
      {
        new_value = iupStrDup(value);
        new_value = iupStrInsert(new_value, insert_value, start, end);
      }
      else
        new_value = iupStrInsert(value, insert_value, start, end);
    }
  }

  if (insert_value && insert_value[0]!=0 && insert_value[1]==0)
    key = insert_value[0];

  if (!new_value)
    return -1; /* continue */

  if (ih->data->nc && (int)strlen(new_value) > ih->data->nc)
  {
    if (new_value != value) free(new_value);
    return 0; /* abort */
  }

  if (ih->data->mask && iupMaskCheck(ih->data->mask, new_value)==0)
  {
    if (new_value != value) free(new_value);
    return 0; /* abort */
  }

  if (cb)
  {
    int cb_ret = cb(ih, key, (char*)new_value);
    if (cb_ret==IUP_IGNORE)
      ret = 0; /* abort */
    else if (cb_ret==IUP_CLOSE)
    {
      IupExitLoop();
      ret = 0; /* abort */
    }
    else if (cb_ret!=0 && key!=0 && 
             cb_ret != IUP_DEFAULT && cb_ret != IUP_CONTINUE)  
      ret = cb_ret; /* abort and replace */
  }

  if (new_value != value) free(new_value);
  return ret; /* continue */
}

static void gtkListEditDeleteText(GtkEditable *editable, int start, int end, Ihandle* ih)
{
  if (iupAttribGet(ih, "_IUPGTK_DISABLE_TEXT_CB"))
    return;

  if (gtkListCallEditCb(ih, editable, NULL, 0, start, end)==0)
    g_signal_stop_emission_by_name(editable, "delete_text");
}

static void gtkListEditInsertText(GtkEditable *editable, char *insert_value, int len, int *pos, Ihandle* ih)
{
  int ret;

  if (iupAttribGet(ih, "_IUPGTK_DISABLE_TEXT_CB"))
    return;

  ret = gtkListCallEditCb(ih, editable, iupStrGetMemoryCopy(iupgtkStrConvertFromUTF8(insert_value)), len, *pos, *pos);
  if (ret == 0)
    g_signal_stop_emission_by_name(editable, "insert_text");
  else if (ret != -1)
  {
    insert_value[0] = (char)ret;  /* replace key */

    /* disable callbacks */
    iupAttribSetStr(ih, "_IUPGTK_DISABLE_TEXT_CB", "1");
    gtk_editable_insert_text(editable, insert_value, 1, pos);
    iupAttribSetStr(ih, "_IUPGTK_DISABLE_TEXT_CB", NULL);

    g_signal_stop_emission_by_name(editable, "insert_text"); 
  }
}

static void gtkListEditChanged(void* dummy, Ihandle* ih)
{
  if (iupAttribGet(ih, "_IUPGTK_DISABLE_TEXT_CB"))
    return;

  iupBaseCallValueChangedCb(ih);
  (void)dummy;
}

static void gtkListComboBoxPopupShown(GtkComboBox* widget, GParamSpec *pspec, Ihandle* ih)
{
  IFni cb = (IFni)IupGetCallback(ih, "DROPDOWN_CB");
  iupAttribSetStr(ih, "_IUPDROPDOWN_POPUP", "1");
  if (cb)
  {
    gboolean popup_shown;
    g_object_get(widget, "popup-shown", &popup_shown, NULL);
    cb(ih, popup_shown);
  }
  (void)pspec;
}

static void gtkListComboBoxChanged(GtkComboBox* widget, Ihandle* ih)
{
  IFnsii cb = (IFnsii)IupGetCallback(ih, "ACTION");
  if (cb)
  {
    int pos = gtk_combo_box_get_active((GtkComboBox*)ih->handle);
    pos++;  /* IUP starts at 1 */
    iupListSingleCallActionCallback(ih, cb, pos);
  }

  if (!ih->data->has_editbox)
    iupBaseCallValueChangedCb(ih);

  (void)widget;
}

static gboolean gtkListSimpleKeyPressEvent(GtkWidget *widget, GdkEventKey *evt, Ihandle *ih)
{
  if (iupgtkKeyPressEvent(widget, evt, ih) == TRUE)
    return TRUE;

  if (evt->keyval == GDK_Return || evt->keyval == GDK_KP_Enter)
    return TRUE; /* used to avoid the call to DBLCLICK_CB in the default processing */
  return FALSE;
}

static void gtkListRowActivated(GtkTreeView *tree_view, GtkTreePath *path, GtkTreeViewColumn *column, Ihandle* ih)
{
  IFnis cb = (IFnis) IupGetCallback(ih, "DBLCLICK_CB");
  if (cb)
  {
    int* indices = gtk_tree_path_get_indices(path);
    iupListSingleCallDblClickCallback(ih, cb, indices[0]+1);  /* IUP starts at 1 */
  }
  (void)column;
  (void)tree_view;
}

static void gtkListSelectionChanged(GtkTreeSelection* selection, Ihandle* ih)
{
  if (ih->data->has_editbox)
  {
    /* must manually update its contents */
    GtkTreeIter iter;
    GtkTreeModel* tree_model;
    if (gtk_tree_selection_get_selected(selection, &tree_model, &iter))
    {
      GtkTreePath *path = gtk_tree_model_get_path(tree_model, &iter);
      char* value = NULL;
            
      gtk_tree_model_get(tree_model, &iter, IUPGTK_LIST_TEXT, &value, -1);
      if (value)
      {
        GtkEntry* entry = (GtkEntry*)iupAttribGet(ih, "_IUPGTK_ENTRY");
        gtk_entry_set_text(entry, value);        
        g_free(value);
      }

      gtk_tree_path_free(path);
    }
  }

  if (!ih->data->is_multiple)
  {
    IFnsii cb = (IFnsii)IupGetCallback(ih, "ACTION");
    if (cb)
    {
      GtkTreeSelection* selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(ih->handle));
      GtkTreeIter iter;
      GtkTreeModel* tree_model;
      if (gtk_tree_selection_get_selected(selection, &tree_model, &iter))
      {
        GtkTreePath *path = gtk_tree_model_get_path(tree_model, &iter);
        int* indices = gtk_tree_path_get_indices(path);
        iupListSingleCallActionCallback(ih, cb, indices[0]+1);  /* IUP starts at 1 */
        gtk_tree_path_free (path);
      }
    }
  }
  else
  {
    IFns multi_cb = (IFns)IupGetCallback(ih, "MULTISELECT_CB");
    IFnsii cb = (IFnsii) IupGetCallback(ih, "ACTION");
    if (multi_cb || cb)
    {
      GList *il, *list = gtk_tree_selection_get_selected_rows(selection, NULL);
      int i, sel_count = g_list_length(list);
      int* pos = malloc(sizeof(int)*sel_count);
      for (il=list, i=0; il; il=il->next, i++)
      {
        GtkTreePath* path = (GtkTreePath*)il->data;
        int* indices = gtk_tree_path_get_indices(path);
        pos[i] = indices[0];
        gtk_tree_path_free(path);
      }
      g_list_free(list);

      iupListMultipleCallActionCallback(ih, cb, multi_cb, pos, sel_count);
      free(pos);
    }
  }

  if (!ih->data->has_editbox)
    iupBaseCallValueChangedCb(ih);
}

static gboolean gtkListComboFocusInOutEvent(GtkWidget *widget, GdkEventFocus *evt, Ihandle *ih)
{
  /* Used only when DROPDOWN=YES and EDITBOX=NO */
  if (iupAttribGetStr(ih, "_IUPDROPDOWN_POPUP"))
  {
    /* A dropdrop will generate leavewindow+killfocus, then enterwindow+getfocus,
       so on a get focus we reset the flag. */
    if (evt->in)
      iupAttribSetStr(ih, "_IUPDROPDOWN_POPUP", NULL);
    return FALSE;
  }

  return iupgtkFocusInOutEvent(widget, evt, ih);
}

static gboolean gtkListComboEnterLeaveEvent(GtkWidget *widget, GdkEventCrossing *evt, Ihandle *ih)
{
  /* Used only when DROPDOWN=YES and EDITBOX=NO */
  if (iupAttribGetStr(ih, "_IUPDROPDOWN_POPUP"))
    return FALSE;
  return iupgtkEnterLeaveEvent(widget, evt, ih);
}


/*********************************************************************************/


static int gtkListMapMethod(Ihandle* ih)
{
  GtkScrolledWindow* scrolled_window = NULL;
  GtkListStore *store;
  GtkCellRenderer *renderer = NULL, *renderer_img = NULL;

  store = gtk_list_store_new(IUPGTK_LIST_LAST_DATA, GDK_TYPE_PIXBUF, G_TYPE_STRING);

  if (ih->data->is_dropdown)
  {
    if (ih->data->has_editbox)
    {
#if GTK_CHECK_VERSION(2, 24, 0)
      ih->handle = gtk_combo_box_new_with_model_and_entry(GTK_TREE_MODEL(store));
      gtk_combo_box_set_entry_text_column ((GtkComboBox*)ih->handle, IUPGTK_LIST_TEXT);
#else
      ih->handle = gtk_combo_box_entry_new_with_model(GTK_TREE_MODEL(store), IUPGTK_LIST_TEXT);
#endif
    }
    else
      ih->handle = gtk_combo_box_new_with_model(GTK_TREE_MODEL(store));

    g_object_unref(store);

    if (!ih->handle)
      return IUP_ERROR;

    g_object_set(G_OBJECT(ih->handle), "has-frame", TRUE, NULL);

    if(ih->data->show_image)
    {
      renderer_img = gtk_cell_renderer_pixbuf_new();
      gtk_cell_layout_pack_start(GTK_CELL_LAYOUT(ih->handle), renderer_img, FALSE);
      gtk_cell_layout_set_attributes(GTK_CELL_LAYOUT(ih->handle), renderer_img, "pixbuf", IUPGTK_LIST_IMAGE, NULL);
      iupAttribSetStr(ih, "_IUPGTK_RENDERER_IMG", (char*)renderer_img);
    }

    if (ih->data->has_editbox)
    {
      GtkWidget *entry;
#if GTK_CHECK_VERSION(2, 12, 0)
      GList* list = gtk_cell_layout_get_cells(GTK_CELL_LAYOUT(ih->handle));
      renderer = list->data;
      g_list_free(list);
#endif
      entry = gtk_bin_get_child(GTK_BIN(ih->handle));
      iupAttribSetStr(ih, "_IUPGTK_ENTRY", (char*)entry);

      g_signal_connect(G_OBJECT(entry), "focus-in-event",     G_CALLBACK(iupgtkFocusInOutEvent), ih);
      g_signal_connect(G_OBJECT(entry), "focus-out-event",    G_CALLBACK(iupgtkFocusInOutEvent), ih);
      g_signal_connect(G_OBJECT(entry), "enter-notify-event", G_CALLBACK(iupgtkEnterLeaveEvent), ih);
      g_signal_connect(G_OBJECT(entry), "leave-notify-event", G_CALLBACK(iupgtkEnterLeaveEvent), ih);
      g_signal_connect(G_OBJECT(entry), "show-help",          G_CALLBACK(iupgtkShowHelp), ih);
      g_signal_connect(G_OBJECT(entry), "key-press-event",    G_CALLBACK(iupgtkKeyPressEvent), ih);

      g_signal_connect(G_OBJECT(entry), "delete-text", G_CALLBACK(gtkListEditDeleteText), ih);
      g_signal_connect(G_OBJECT(entry), "insert-text", G_CALLBACK(gtkListEditInsertText), ih);
      /* g_signal_connect(G_OBJECT(entry), "changed", G_CALLBACK(gtkListEditChanged), ih); */
      g_signal_connect_after(G_OBJECT(entry), "move-cursor", G_CALLBACK(gtkListEditMoveCursor), ih);  /* only report some caret movements */
      g_signal_connect_after(G_OBJECT(entry), "key-release-event", G_CALLBACK(gtkListEditKeyReleaseEvent), ih);
      g_signal_connect(G_OBJECT(entry), "button-press-event", G_CALLBACK(gtkListEditButtonEvent), ih);  /* if connected "after" then it is ignored */
      g_signal_connect(G_OBJECT(entry), "button-release-event",G_CALLBACK(gtkListEditButtonEvent), ih);

      if (!iupAttribGetBoolean(ih, "CANFOCUS"))
        iupgtkSetCanFocus(ih->handle, 0);

      if(ih->data->show_image)
        gtk_cell_layout_reorder(GTK_CELL_LAYOUT(ih->handle), renderer_img, IUPGTK_LIST_IMAGE);
    }
    else
    {
      GtkWidget *toggle;

      /* had to add an event box so it can be positioned in IupMatrix */
      GtkWidget *box = gtk_event_box_new();
      gtk_container_add((GtkContainer*)box, ih->handle);
      iupAttribSetStr(ih, "_IUP_EXTRAPARENT", (char*)box);

      /* use the internal toggle for keyboard, focus and enter/leave */
      gtk_container_forall((GtkContainer*)ih->handle, gtkComboBoxChildrenToggleCb, &toggle);

      renderer = gtk_cell_renderer_text_new();
      gtk_cell_layout_pack_start(GTK_CELL_LAYOUT(ih->handle), renderer, TRUE);
      gtk_cell_layout_set_attributes(GTK_CELL_LAYOUT(ih->handle), renderer, "text", IUPGTK_LIST_TEXT, NULL);

      g_signal_connect(G_OBJECT(toggle), "focus-in-event",  G_CALLBACK(gtkListComboFocusInOutEvent), ih);
      g_signal_connect(G_OBJECT(toggle), "focus-out-event", G_CALLBACK(gtkListComboFocusInOutEvent), ih);
      g_signal_connect(G_OBJECT(toggle), "enter-notify-event", G_CALLBACK(gtkListComboEnterLeaveEvent), ih);
      g_signal_connect(G_OBJECT(toggle), "leave-notify-event", G_CALLBACK(gtkListComboEnterLeaveEvent), ih);
      g_signal_connect(G_OBJECT(toggle), "key-press-event", G_CALLBACK(iupgtkKeyPressEvent), ih);
      g_signal_connect(G_OBJECT(toggle), "show-help",       G_CALLBACK(iupgtkShowHelp), ih);

      if (!iupAttribGetBoolean(ih, "CANFOCUS"))
      {
        iupgtkSetCanFocus(ih->handle, 0);
        gtk_combo_box_set_focus_on_click((GtkComboBox*)ih->handle, FALSE);
      }
      else
      {
        gtk_combo_box_set_focus_on_click((GtkComboBox*)ih->handle, TRUE);
        iupgtkSetCanFocus(toggle, 1);
      }
    }

    g_signal_connect(ih->handle, "changed", G_CALLBACK(gtkListComboBoxChanged), ih);
    g_signal_connect(ih->handle, "notify::popup-shown", G_CALLBACK(gtkListComboBoxPopupShown), ih);

    if (renderer)
    {
#if GTK_CHECK_VERSION(2, 18, 0)
      gtk_cell_renderer_set_padding(renderer, 0, 0);
#else
      renderer->xpad = 0;
      renderer->ypad = 0;
#endif
      iupAttribSetStr(ih, "_IUPGTK_RENDERER", (char*)renderer);
    }
  }
  else
  {
    GtkTreeSelection* selection;
    GtkTreeViewColumn *column;
    GtkPolicyType scrollbar_policy;

    ih->handle = gtk_tree_view_new_with_model(GTK_TREE_MODEL(store));
    g_object_unref(store);

    if (!ih->handle)
      return IUP_ERROR;

    scrolled_window = (GtkScrolledWindow*)gtk_scrolled_window_new(NULL, NULL);

    if (ih->data->has_editbox)
    {
      GtkBox* vbox = (GtkBox*)gtk_vbox_new(FALSE, 0);

      GtkWidget *entry = gtk_entry_new();
      gtk_widget_show(entry);
      gtk_box_pack_start(vbox, entry, FALSE, FALSE, 0);
      iupAttribSetStr(ih, "_IUPGTK_ENTRY", (char*)entry);

      gtk_widget_show((GtkWidget*)vbox);
      gtk_box_pack_end(vbox, (GtkWidget*)scrolled_window, TRUE, TRUE, 0);
      iupAttribSetStr(ih, "_IUP_EXTRAPARENT", (char*)vbox);
      iupAttribSetStr(ih, "_IUPGTK_SCROLLED_WINDOW", (char*)scrolled_window);

      iupgtkSetCanFocus(ih->handle, 0);  /* focus goes only to the edit box */
      if (!iupAttribGetBoolean(ih, "CANFOCUS"))
        iupgtkSetCanFocus(entry, 0);

      g_signal_connect(G_OBJECT(entry), "focus-in-event",     G_CALLBACK(iupgtkFocusInOutEvent), ih);
      g_signal_connect(G_OBJECT(entry), "focus-out-event",    G_CALLBACK(iupgtkFocusInOutEvent), ih);
      g_signal_connect(G_OBJECT(entry), "enter-notify-event", G_CALLBACK(iupgtkEnterLeaveEvent), ih);
      g_signal_connect(G_OBJECT(entry), "leave-notify-event", G_CALLBACK(iupgtkEnterLeaveEvent), ih);
      g_signal_connect(G_OBJECT(entry), "show-help",          G_CALLBACK(iupgtkShowHelp), ih);

      g_signal_connect(G_OBJECT(entry), "delete-text", G_CALLBACK(gtkListEditDeleteText), ih);
      g_signal_connect(G_OBJECT(entry), "insert-text", G_CALLBACK(gtkListEditInsertText), ih);
      g_signal_connect(G_OBJECT(entry), "changed", G_CALLBACK(gtkListEditChanged), ih);
      g_signal_connect_after(G_OBJECT(entry), "move-cursor", G_CALLBACK(gtkListEditMoveCursor), ih);  /* only report some caret movements */
      g_signal_connect(G_OBJECT(entry), "key-press-event",    G_CALLBACK(gtkListEditKeyPressEvent), ih);
      g_signal_connect_after(G_OBJECT(entry), "key-release-event", G_CALLBACK(gtkListEditKeyReleaseEvent), ih);
      g_signal_connect(G_OBJECT(entry), "button-press-event", G_CALLBACK(gtkListEditButtonEvent), ih);  /* if connected "after" then it is ignored */
      g_signal_connect(G_OBJECT(entry), "button-release-event",G_CALLBACK(gtkListEditButtonEvent), ih);
    }
    else
    {
      iupAttribSetStr(ih, "_IUP_EXTRAPARENT", (char*)scrolled_window);

      if (!iupAttribGetBoolean(ih, "CANFOCUS"))
        iupgtkSetCanFocus(ih->handle, 0);

      g_signal_connect(G_OBJECT(ih->handle), "focus-in-event",     G_CALLBACK(iupgtkFocusInOutEvent), ih);
      g_signal_connect(G_OBJECT(ih->handle), "focus-out-event",    G_CALLBACK(iupgtkFocusInOutEvent), ih);
      g_signal_connect(G_OBJECT(ih->handle), "enter-notify-event", G_CALLBACK(iupgtkEnterLeaveEvent), ih);
      g_signal_connect(G_OBJECT(ih->handle), "leave-notify-event", G_CALLBACK(iupgtkEnterLeaveEvent), ih);
      g_signal_connect(G_OBJECT(ih->handle), "key-press-event",    G_CALLBACK(gtkListSimpleKeyPressEvent), ih);
      g_signal_connect(G_OBJECT(ih->handle), "show-help",          G_CALLBACK(iupgtkShowHelp), ih);
    }

    column = gtk_tree_view_column_new();

    if (ih->data->show_image)
    {
      renderer_img = gtk_cell_renderer_pixbuf_new();
      gtk_cell_layout_pack_start(GTK_CELL_LAYOUT(column), renderer_img, FALSE);
      gtk_cell_layout_set_attributes(GTK_CELL_LAYOUT(column), renderer_img, "pixbuf", IUPGTK_LIST_IMAGE, NULL);
      iupAttribSetStr(ih, "_IUPGTK_RENDERER_IMG", (char*)renderer_img);
    }

    renderer = gtk_cell_renderer_text_new();
    gtk_cell_layout_pack_start(GTK_CELL_LAYOUT(column), renderer, TRUE);
    gtk_cell_layout_set_attributes(GTK_CELL_LAYOUT(column), renderer, "text", IUPGTK_LIST_TEXT, NULL);
    iupAttribSetStr(ih, "_IUPGTK_RENDERER", (char*)renderer);
    g_object_set(G_OBJECT(renderer), "xpad", 0, NULL);
    g_object_set(G_OBJECT(renderer), "ypad", 0, NULL);

    gtk_tree_view_append_column(GTK_TREE_VIEW(ih->handle), column);
    gtk_tree_view_set_headers_visible(GTK_TREE_VIEW(ih->handle), FALSE);
    gtk_tree_view_set_enable_search(GTK_TREE_VIEW(ih->handle), FALSE);   /* TODO: check "start-interactive-search" signal */

    gtk_container_add((GtkContainer*)scrolled_window, ih->handle);
    gtk_widget_show((GtkWidget*)scrolled_window);
    gtk_scrolled_window_set_shadow_type(scrolled_window, GTK_SHADOW_IN); 

    if (ih->data->sb)
    {
      if (iupAttribGetBoolean(ih, "AUTOHIDE"))
        scrollbar_policy = GTK_POLICY_AUTOMATIC;
      else
        scrollbar_policy = GTK_POLICY_ALWAYS;
    }
    else
      scrollbar_policy = GTK_POLICY_NEVER;

    gtk_scrolled_window_set_policy(scrolled_window, scrollbar_policy, scrollbar_policy);

    selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(ih->handle));
    if (!ih->data->has_editbox && ih->data->is_multiple)
    {
      gtk_tree_selection_set_mode(selection, GTK_SELECTION_MULTIPLE);
  #if GTK_CHECK_VERSION(2, 10, 0)
      gtk_tree_view_set_rubber_banding(GTK_TREE_VIEW(ih->handle), TRUE);
  #endif
    }
    else
      gtk_tree_selection_set_mode(selection, GTK_SELECTION_BROWSE);

    g_signal_connect(G_OBJECT(selection), "changed",  G_CALLBACK(gtkListSelectionChanged), ih);
    g_signal_connect(G_OBJECT(ih->handle), "row-activated", G_CALLBACK(gtkListRowActivated), ih);
    g_signal_connect(G_OBJECT(ih->handle), "motion-notify-event",G_CALLBACK(iupgtkMotionNotifyEvent), ih);
    g_signal_connect(G_OBJECT(ih->handle), "button-press-event", G_CALLBACK(iupgtkButtonEvent), ih);
    g_signal_connect(G_OBJECT(ih->handle), "button-release-event",G_CALLBACK(iupgtkButtonEvent), ih);
  }

  if (iupAttribGetBoolean(ih, "SORT"))
    gtk_tree_sortable_set_sort_column_id(GTK_TREE_SORTABLE(store), IUPGTK_LIST_TEXT, GTK_SORT_ASCENDING);

  /* add to the parent, all GTK controls must call this. */
  iupgtkBaseAddToParent(ih);

  if (scrolled_window)
    gtk_widget_realize((GtkWidget*)scrolled_window);
  gtk_widget_realize(ih->handle);

  /* configure for DRAG&DROP */
  if (IupGetCallback(ih, "DROPFILES_CB"))
    iupAttribSetStr(ih, "DROPFILESTARGET", "YES");

  IupSetCallback(ih, "_IUP_XY2POS_CB", (Icallback)gtkListConvertXYToPos);

  iupListSetInitialItems(ih);

  return IUP_NOERROR;
}

void iupdrvListInitClass(Iclass* ic)
{
  /* Driver Dependent Class functions */
  ic->Map = gtkListMapMethod;

  /* Driver Dependent Attribute functions */

  /* Overwrite Common */
  iupClassRegisterAttribute(ic, "STANDARDFONT", NULL, gtkListSetStandardFontAttrib, IUPAF_SAMEASSYSTEM, "DEFAULTFONT", IUPAF_NO_SAVE|IUPAF_NOT_MAPPED);

  /* Visual */
  iupClassRegisterAttribute(ic, "BGCOLOR", NULL, gtkListSetBgColorAttrib, IUPAF_SAMEASSYSTEM, "TXTBGCOLOR", IUPAF_DEFAULT);

  /* Special */
  iupClassRegisterAttribute(ic, "FGCOLOR", NULL, gtkListSetFgColorAttrib, IUPAF_SAMEASSYSTEM, "TXTFGCOLOR", IUPAF_DEFAULT);

  /* IupList only */
  iupClassRegisterAttributeId(ic, "IDVALUE", gtkListGetIdValueAttrib, iupListSetIdValueAttrib, IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "VALUE", gtkListGetValueAttrib, gtkListSetValueAttrib, NULL, NULL, IUPAF_NO_DEFAULTVALUE|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "SHOWDROPDOWN", NULL, gtkListSetShowDropdownAttrib, NULL, NULL, IUPAF_WRITEONLY|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "TOPITEM", NULL, gtkListSetTopItemAttrib, NULL, NULL, IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "SPACING", iupListGetSpacingAttrib, gtkListSetSpacingAttrib, IUPAF_SAMEASSYSTEM, "0", IUPAF_NOT_MAPPED);

  iupClassRegisterAttribute(ic, "PADDING", iupListGetPaddingAttrib, gtkListSetPaddingAttrib, IUPAF_SAMEASSYSTEM, "0x0", IUPAF_NOT_MAPPED);
  iupClassRegisterAttribute(ic, "SELECTEDTEXT", gtkListGetSelectedTextAttrib, gtkListSetSelectedTextAttrib, NULL, NULL, IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "SELECTION", gtkListGetSelectionAttrib, gtkListSetSelectionAttrib, NULL, NULL, IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "SELECTIONPOS", gtkListGetSelectionPosAttrib, gtkListSetSelectionPosAttrib, NULL, NULL, IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "CARET", gtkListGetCaretAttrib, gtkListSetCaretAttrib, NULL, NULL, IUPAF_NO_SAVE|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "CARETPOS", gtkListGetCaretPosAttrib, gtkListSetCaretPosAttrib, IUPAF_SAMEASSYSTEM, "0", IUPAF_NO_SAVE|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "INSERT", NULL, gtkListSetInsertAttrib, NULL, NULL, IUPAF_WRITEONLY|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "APPEND", NULL, gtkListSetAppendAttrib, NULL, NULL, IUPAF_WRITEONLY|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "READONLY", gtkListGetReadOnlyAttrib, gtkListSetReadOnlyAttrib, NULL, NULL, IUPAF_DEFAULT);
  iupClassRegisterAttribute(ic, "NC", iupListGetNCAttrib, gtkListSetNCAttrib, NULL, NULL, IUPAF_NOT_MAPPED);
  iupClassRegisterAttribute(ic, "CLIPBOARD", NULL, gtkListSetClipboardAttrib, NULL, NULL, IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "SCROLLTO", NULL, gtkListSetScrollToAttrib, NULL, NULL, IUPAF_WRITEONLY|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "SCROLLTOPOS", NULL, gtkListSetScrollToPosAttrib, NULL, NULL, IUPAF_WRITEONLY|IUPAF_NO_INHERIT);

  iupClassRegisterAttributeId(ic, "IMAGE", NULL, gtkListSetImageAttrib, IUPAF_IHANDLENAME|IUPAF_WRITEONLY|IUPAF_NO_INHERIT);

  /* Not Supported */
  iupClassRegisterAttribute(ic, "VISIBLE_ITEMS", NULL, NULL, IUPAF_SAMEASSYSTEM, "5", IUPAF_NOT_SUPPORTED);
  iupClassRegisterAttribute(ic, "DROPEXPAND", NULL, NULL, IUPAF_SAMEASSYSTEM, "Yes", IUPAF_NOT_SUPPORTED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "AUTOREDRAW", NULL, NULL, IUPAF_SAMEASSYSTEM, "Yes", IUPAF_NOT_SUPPORTED|IUPAF_NO_INHERIT);
}
