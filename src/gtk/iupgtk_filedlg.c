/** \file
 * \brief IupFileDlg pre-defined dialog
 *
 * See Copyright Notice in "iup.h"
 */

#include <gtk/gtk.h>

#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <stdio.h>

#include "iup.h"
#include "iupcbs.h"

#include "iup_object.h"
#include "iup_attrib.h"
#include "iup_str.h"
#include "iup_drvinfo.h"
#include "iup_dialog.h"
#include "iup_strmessage.h"
#include "iup_array.h"
#include "iup_drvinfo.h"

#include "iupgtk_drv.h"


static void iupStrRemoveChar(char* str, char c)
{
  char* p = str;
  while (*str)
  {
    if (*str != c)
    {
      *p = *str;
      p++;
    }

    str++;
  }
  *p = 0;
}

static char* gtkFileDlgGetNextStr(char* str)
{
  /* after the 0 there is another string, 
     must know a priori how many strings are before using this */
  int len = (int)strlen(str);
  return str+len+1;
}

static void gtkFileDlgGetMultipleFiles(Ihandle* ih, GSList* list)
{
  int len, cur_len, dir_len = -1;
  char *filename, *all_names;
  Iarray* names_array = iupArrayCreate(1024, 1);  /* just set an initial size, but count is 0 */

  while (list)
  {
    filename = (char*)list->data;
    len = strlen(filename);

    if (dir_len == -1)
    {
      dir_len = len;

      while (dir_len && (filename[dir_len] != '/' && filename[dir_len] != '\\'))
        dir_len--;

      cur_len = iupArrayCount(names_array);
      all_names = iupArrayAdd(names_array, dir_len+1);
      memcpy(all_names+cur_len, filename, dir_len);
      all_names[cur_len+dir_len] = '0';
      iupAttribStoreStr(ih, "DIRECTORY", all_names);
      all_names[cur_len+dir_len] = '|';

      dir_len++; /* skip separator */
    }
    len -= dir_len; /* remove directory */

    cur_len = iupArrayCount(names_array);
    all_names = iupArrayAdd(names_array, len+1);
    memcpy(all_names+cur_len, filename+dir_len, len);
    all_names[cur_len+len] = '|';

    g_free(filename);
    list = list->next;
  }

  cur_len = iupArrayCount(names_array);
  all_names = iupArrayInc(names_array);
  all_names[cur_len+1] = 0;

  iupAttribStoreStr(ih, "VALUE", iupgtkStrConvertFromFilename(all_names));

  iupArrayDestroy(names_array);
}

#ifdef GTK_MAC
  #include <gdk/gdk.h>
#else
  #ifdef WIN32
    #include <gdk/gdkwin32.h>
  #else
    #include <gdk/gdkx.h>
  #endif
#endif

static void gtkFileDlgUpdatePreviewGLCanvas(Ihandle* ih)
{
  Ihandle* glcanvas = IupGetAttributeHandle(ih, "PREVIEWGLCANVAS");
  if (glcanvas)
  {
#ifndef GTK_MAC
  #ifdef WIN32                                 
      iupAttribSetStr(glcanvas, "HWND", iupAttribGet(ih, "HWND"));
  #else
      iupAttribSetStr(glcanvas, "XWINDOW", iupAttribGet(ih, "XWINDOW"));
  #endif
#endif
    glcanvas->iclass->Map(glcanvas);
  }
}

static void gtkFileDlgPreviewRealize(GtkWidget *widget, Ihandle *ih)
{
  iupAttribSetStr(ih, "PREVIEWDC", iupgtkGetNativeGraphicsContext(widget));
  iupAttribSetStr(ih, "WID", (char*)widget);

#ifndef GTK_MAC
  #ifdef WIN32                                 
    iupAttribSetStr(ih, "HWND", (char*)GDK_WINDOW_HWND(iupgtkGetWindow(widget)));
  #else
    iupAttribSetStr(ih, "XWINDOW", (char*)GDK_WINDOW_XID(iupgtkGetWindow(widget)));
    iupAttribSetStr(ih, "XDISPLAY", (char*)iupdrvGetDisplay());
  #endif
#endif

  gtkFileDlgUpdatePreviewGLCanvas(ih);
}

static void gtkFileDlgRealize(GtkWidget *widget, Ihandle *ih)
{
  /* callback here always exists */
  IFnss cb = (IFnss)IupGetCallback(ih, "FILE_CB");
  cb(ih, NULL, "INIT");
  (void)widget;
}

static gboolean gtkFileDlgPreviewConfigureEvent(GtkWidget *widget, GdkEventConfigure *evt, Ihandle *ih)
{
  iupAttribSetInt(ih, "PREVIEWWIDTH", evt->width);
  iupAttribSetInt(ih, "PREVIEWHEIGHT", evt->height);

  (void)widget;
  return FALSE;
}

static gboolean gtkFileDlgPreviewExposeEvent(GtkWidget *widget, GdkEventExpose *evt, Ihandle *ih)
{
  GtkFileChooser *file_chooser = (GtkFileChooser*)iupAttribGet(ih, "_IUPDLG_FILE_CHOOSER");
  char *filename = gtk_file_chooser_get_preview_filename(file_chooser);

  /* callback here always exists */
  IFnss cb = (IFnss)IupGetCallback(ih, "FILE_CB");
  if (iupdrvIsFile(filename))
    cb(ih, iupgtkStrConvertFromFilename(filename), "PAINT");
  else
    cb(ih, NULL, "PAINT");

  g_free (filename);
 
  (void)evt;
  (void)widget;
  return TRUE;  /* stop other handlers */
}

static void gtkFileDlgUpdatePreview(GtkFileChooser *file_chooser, Ihandle* ih)
{
  char *filename = gtk_file_chooser_get_preview_filename(file_chooser);

  /* callback here always exists */
  IFnss cb = (IFnss)IupGetCallback(ih, "FILE_CB");
  if (iupdrvIsFile(filename))
    cb(ih, iupgtkStrConvertFromFilename(filename), "SELECT");
  else
    cb(ih, iupgtkStrConvertFromFilename(filename), "OTHER");

  g_free (filename);

  gtk_file_chooser_set_preview_widget_active(file_chooser, TRUE);
}
  
static int gtkFileDlgPopup(Ihandle* ih, int x, int y)
{
  InativeHandle* parent = iupDialogGetNativeParent(ih);
  GtkWidget* dialog;
  GtkWidget* preview_canvas = NULL;
  GtkFileChooserAction action;
  IFnss file_cb;
  char* value;
  int response, filter_count = 0;

  iupAttribSetInt(ih, "_IUPDLG_X", x);   /* used in iupDialogUpdatePosition */
  iupAttribSetInt(ih, "_IUPDLG_Y", y);

  value = iupAttribGetStr(ih, "DIALOGTYPE");
  if (iupStrEqualNoCase(value, "SAVE"))
    action = GTK_FILE_CHOOSER_ACTION_SAVE;
  else if (iupStrEqualNoCase(value, "DIR"))
    action = GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER;
  else
    action = GTK_FILE_CHOOSER_ACTION_OPEN;

  value = iupAttribGet(ih, "TITLE");
  if (!value)
  {
    GtkStockItem item;

    if (action == GTK_FILE_CHOOSER_ACTION_SAVE)
      value = GTK_STOCK_SAVE_AS;
    else
      value = GTK_STOCK_OPEN;

    gtk_stock_lookup(value, &item);
    value = item.label;

    iupAttribStoreStr(ih, "TITLE", iupgtkStrConvertFromUTF8(value));
    value = iupAttribGet(ih, "TITLE");
    iupStrRemoveChar(value, '_');
  }

  dialog = gtk_file_chooser_dialog_new(iupgtkStrConvertToUTF8(value), (GtkWindow*)parent, action, 
                                       GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL, 
                                       NULL);
  if (!dialog)
    return IUP_ERROR;

  if (action == GTK_FILE_CHOOSER_ACTION_SAVE)
    gtk_dialog_add_button(GTK_DIALOG(dialog), GTK_STOCK_SAVE, GTK_RESPONSE_OK);
  else if (action == GTK_FILE_CHOOSER_ACTION_OPEN)
    gtk_dialog_add_button(GTK_DIALOG(dialog), GTK_STOCK_OPEN, GTK_RESPONSE_OK);
  else
    gtk_dialog_add_button(GTK_DIALOG(dialog), GTK_STOCK_OK, GTK_RESPONSE_OK);

  if (IupGetCallback(ih, "HELP_CB"))
    gtk_dialog_add_button(GTK_DIALOG(dialog), GTK_STOCK_HELP, GTK_RESPONSE_HELP);

#if GTK_CHECK_VERSION(2, 6, 0)
  if (iupAttribGetBoolean(ih, "SHOWHIDDEN"))
    gtk_file_chooser_set_show_hidden(GTK_FILE_CHOOSER(dialog), TRUE);
#endif

  if (iupAttribGetBoolean(ih, "MULTIPLEFILES") && action == GTK_FILE_CHOOSER_ACTION_OPEN)
    gtk_file_chooser_set_select_multiple(GTK_FILE_CHOOSER(dialog), TRUE);

#if GTK_CHECK_VERSION(2, 8, 0)
  if (!iupAttribGetBoolean(ih, "NOOVERWRITEPROMPT") && action == GTK_FILE_CHOOSER_ACTION_SAVE)
    gtk_file_chooser_set_do_overwrite_confirmation(GTK_FILE_CHOOSER(dialog), TRUE);
#endif

  /* just check for the path inside FILE */
  value = iupAttribGet(ih, "FILE");
  if (value && (value[0] == '/' || value[1] == ':'))
  {
    char* dir = iupStrFileGetPath(value);
    int len = strlen(dir);
    iupAttribStoreStr(ih, "DIRECTORY", dir);
    free(dir);
    iupAttribStoreStr(ih, "FILE", value+len);
  }

  value = iupAttribGet(ih, "DIRECTORY");
  if (value)
    gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(dialog), iupgtkStrConvertToFilename(value));

  value = iupAttribGet(ih, "FILE");
  if (value)
  {
    if (action == GTK_FILE_CHOOSER_ACTION_SAVE)
      gtk_file_chooser_set_current_name(GTK_FILE_CHOOSER(dialog), iupgtkStrConvertToFilename(value));
    else
    {
      if (iupdrvIsFile(value))  /* check if file exists */
        gtk_file_chooser_set_filename(GTK_FILE_CHOOSER(dialog), iupgtkStrConvertToFilename(value));
    }
  }

  value = iupAttribGet(ih, "EXTFILTER");
  if (value)
  {
    char *name, *pattern, *filters = iupStrDup(value);
    char atrib[30];
    int i, pattern_count, j;
    int filter_index = iupAttribGetInt(ih, "FILTERUSED");
    if (!filter_index)
      filter_index = 1;

    filter_count = iupStrReplace(filters, '|', 0) / 2;

    name = filters;
    for (i=0; i<filter_count && name[0]; i++)
    {
      GtkFileFilter *filter = gtk_file_filter_new();

      pattern = gtkFileDlgGetNextStr(name);

      pattern_count = iupStrReplace(pattern, ';', 0)+1;

      gtk_file_filter_set_name(filter, iupgtkStrConvertToUTF8(name));

      for (j=0; j<pattern_count && pattern[0]; j++)
      {
        gtk_file_filter_add_pattern(filter, pattern);
        if (j<pattern_count-1)
          pattern = gtkFileDlgGetNextStr(pattern);
      }

      gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(dialog), filter);

      sprintf(atrib, "_IUPDLG_FILTER%d", i+1);
      iupAttribSetStr(ih, atrib, (char*)filter);

      if (i+1 == filter_index)
        gtk_file_chooser_set_filter(GTK_FILE_CHOOSER(dialog), filter);

      name = gtkFileDlgGetNextStr(pattern);
    }

    free(filters);
  }
  else 
  {
    value = iupAttribGet(ih, "FILTER");
    if (value)
    {
      char* filters = iupStrDup(value), *fstr;
      int pattern_count, i;
      GtkFileFilter *filter = gtk_file_filter_new();
      char* info = iupAttribGet(ih, "FILTERINFO");
      if (!info)
        info = value;

      pattern_count = iupStrReplace(filters, ';', 0)+1;

      gtk_file_filter_set_name(filter, iupgtkStrConvertToUTF8(info));

      fstr = filters;
      for (i=0; i<pattern_count && fstr[0]; i++)
      {
        gtk_file_filter_add_pattern(filter, fstr);
        fstr = gtkFileDlgGetNextStr(fstr);
      }
      gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(dialog), filter);
      free(filters);
    }
  }

  file_cb = (IFnss)IupGetCallback(ih, "FILE_CB");
  if (file_cb && action != GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER)
  {
    g_signal_connect(GTK_FILE_CHOOSER(dialog), "update-preview", G_CALLBACK(gtkFileDlgUpdatePreview), ih);
    g_signal_connect(dialog, "realize", G_CALLBACK(gtkFileDlgRealize), ih);

    if (iupAttribGetBoolean(ih, "SHOWPREVIEW"))
    {
      GtkWidget* frame = gtk_frame_new(NULL);
      gtk_frame_set_shadow_type(GTK_FRAME(frame), GTK_SHADOW_ETCHED_IN);
      gtk_widget_set_size_request(frame, 180, 150);

      preview_canvas = gtk_drawing_area_new();
      gtk_widget_set_double_buffered(preview_canvas, FALSE);
      gtk_container_add(GTK_CONTAINER(frame), preview_canvas);
      gtk_widget_show(preview_canvas);

      g_signal_connect(preview_canvas, "configure-event", G_CALLBACK(gtkFileDlgPreviewConfigureEvent), ih);
      g_signal_connect(preview_canvas, "expose-event", G_CALLBACK(gtkFileDlgPreviewExposeEvent), ih);
      g_signal_connect(preview_canvas, "realize", G_CALLBACK(gtkFileDlgPreviewRealize), ih);

      iupAttribSetStr(ih, "_IUPDLG_FILE_CHOOSER", (char*)dialog);

      gtk_file_chooser_set_preview_widget(GTK_FILE_CHOOSER(dialog), frame);
    }
  }
  
  /* initialize the widget */
  gtk_widget_realize(GTK_WIDGET(dialog));
  
  ih->handle = GTK_WIDGET(dialog);
  iupDialogUpdatePosition(ih);
  ih->handle = NULL;  /* reset handle */

  do 
  {
    response = gtk_dialog_run(GTK_DIALOG(dialog));

    if (response == GTK_RESPONSE_HELP)
    {
      Icallback cb = IupGetCallback(ih, "HELP_CB");
      if (cb && cb(ih) == IUP_CLOSE)
        response = GTK_RESPONSE_CANCEL;
    }
    else if (response == GTK_RESPONSE_OK)
    {
      char *filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
      int file_exist = iupdrvIsFile(filename);
      int dir_exist = iupdrvIsDirectory(filename);
      g_free(filename);

      if (action == GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER)
      {
        if (!dir_exist)
        {
          iupStrMessageShowError(ih, "IUP_INVALIDDIR");
          response = GTK_RESPONSE_HELP; /* to leave the dialog open */
          continue;
        }
      }
      else if (!iupAttribGetBoolean(ih, "MULTIPLEFILES"))
      {
        if (dir_exist)
        {
          iupStrMessageShowError(ih, "IUP_FILEISDIR");
          response = GTK_RESPONSE_HELP; /* to leave the dialog open */
          continue;
        }

        if (!file_exist)  /* if do not exist check ALLOWNEW */
        {
          value = iupAttribGet(ih, "ALLOWNEW");
          if (!value)
          {
            if (action == GTK_FILE_CHOOSER_ACTION_SAVE)
              value = "YES";
            else
              value = "NO";
          }

          if (!iupStrBoolean(value))
          {
            iupStrMessageShowError(ih, "IUP_FILENOTEXIST");
            response = GTK_RESPONSE_HELP; /* to leave the dialog open */
            continue;
          }
        }

        if (file_cb)
        {
          char *filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
          int ret = file_cb(ih, iupgtkStrConvertFromFilename(filename), "OK");
          g_free(filename);
          
          if (ret == IUP_IGNORE)
          {
            response = GTK_RESPONSE_HELP; /* to leave the dialog open */
            continue;
          }
        }
      }
    }
  } while (response == GTK_RESPONSE_HELP);

  if (file_cb)
  {
    if (iupAttribGetBoolean(ih, "SHOWPREVIEW"))
      iupgtkReleaseNativeGraphicsContext(preview_canvas, (void*)iupAttribGet(ih, "PREVIEWDC"));

    file_cb(ih, NULL, "FINISH");
  }

  if (response == GTK_RESPONSE_OK)
  {
    int file_exist, dir_exist;

    if (filter_count)
    {
      int i;
      char atrib[30];
      GtkFileFilter* filter = gtk_file_chooser_get_filter(GTK_FILE_CHOOSER(dialog));

      for (i=0; i<filter_count; i++)
      {
        sprintf(atrib, "_IUPDLG_FILTER%d", i+1);
        if (filter == (GtkFileFilter*)iupAttribGet(ih, atrib))
          iupAttribSetInt(ih, "FILTERUSED", i+1);
      }
    }

    if (iupAttribGetBoolean(ih, "MULTIPLEFILES"))
    {
      GSList* file_list = gtk_file_chooser_get_filenames(GTK_FILE_CHOOSER(dialog));

      if (file_list->next) /* if more than one file */
        gtkFileDlgGetMultipleFiles(ih, file_list);
      else
      {
        char* filename = (char*)file_list->data;
        iupAttribStoreStr(ih, "VALUE", iupgtkStrConvertFromFilename(filename));
        g_free(filename);

        /* store the DIRECTORY */
        {
          char* dir = iupStrFileGetPath(iupAttribGet(ih, "VALUE"));
          iupAttribStoreStr(ih, "DIRECTORY", dir);
          free(dir);
        }
      }

      g_slist_free(file_list);
      file_exist = 1;
      dir_exist = 0;
    }
    else
    {
      char *filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
      iupAttribStoreStr(ih, "VALUE", iupgtkStrConvertFromFilename(filename));
      file_exist = iupdrvIsFile(filename);
      dir_exist = iupdrvIsDirectory(filename);

      if (file_exist)
      {
        char* dir = iupStrFileGetPath(filename);
        iupAttribStoreStr(ih, "DIRECTORY", dir);
        free(dir);
      }

      g_free(filename);
    }

    if (dir_exist)
    {
      iupAttribSetStr(ih, "FILEEXIST", NULL);
      iupAttribSetStr(ih, "STATUS", "0");
    }
    else
    {
      if (file_exist)  /* check if file exists */
      {
        iupAttribSetStr(ih, "FILEEXIST", "YES");
        iupAttribSetStr(ih, "STATUS", "0");
      }
      else
      {
        iupAttribSetStr(ih, "FILEEXIST", "NO");
        iupAttribSetStr(ih, "STATUS", "1");
      }
    }

    if (action != GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER && !iupAttribGetBoolean(ih, "NOCHANGEDIR"))  /* do change the current directory */
    {
      /* GtkFileChooser does not change the current directory */
      char* dir = gtk_file_chooser_get_current_folder(GTK_FILE_CHOOSER(dialog));
      if (dir) 
      {
        iupdrvSetCurrentDirectory(dir);
        g_free(dir);
      }
    }
  }
  else
  {
    iupAttribSetStr(ih, "FILTERUSED", NULL);
    iupAttribSetStr(ih, "VALUE", NULL);
    iupAttribSetStr(ih, "FILEEXIST", NULL);
    iupAttribSetStr(ih, "STATUS", "-1");
  }

  gtk_widget_destroy(GTK_WIDGET(dialog));  

  return IUP_NOERROR;
}

void iupdrvFileDlgInitClass(Iclass* ic)
{
  ic->DlgPopup = gtkFileDlgPopup;

  /* IupFileDialog Windows and GTK Only */
  iupClassRegisterAttribute(ic, "EXTFILTER", NULL, NULL, NULL, NULL, IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "FILTERINFO", NULL, NULL, NULL, NULL, IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "FILTERUSED", NULL, NULL, NULL, NULL, IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "MULTIPLEFILES", NULL, NULL, NULL, NULL, IUPAF_NO_INHERIT);
}
