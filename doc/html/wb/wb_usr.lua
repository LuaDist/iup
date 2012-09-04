wb_usr = {
   contact = "iup@tecgraf.puc-rio.br",
   search_link = "http://www.tecgraf.puc-rio.br/iup",
   copyright_link = "http://www.tecgraf.puc-rio.br",
   copyright_name = "Tecgraf/PUC-Rio",
   title_bgcolor = "#3366CC",
   langs = {"en"},
   start_size = "180",
   file_title = "iup",
   start_open = "1"
}

wb_usr.messages = {
  en = {
     title = "IUP - Portable User Interface",
     bar_title = "IUP - Version 3"
  }
}

wb_usr.tree =
{
  name= {nl= "IUP"},
  link= "home.html",
  folder=
  {
    {
      name= {en= "Product"},
      link= "prod.html",
      folder=
      {
        {
          name= {en= "Overview"},
          link= "prod.html#visao_geral"
        },
        {
          name= {en= "Availability"},
          link= "prod.html#disponibilidade"
        },
        {
          name= {en= "Support"},
          link= "prod.html#suporte"
        },
        {
          name= {en= "Credits"},
          link= "prod.html#creditos"
        },
        {
          name= {en= "Documentation"},
          link= "prod.html#sobre"
        },
        {
          name= {en= "Publications"},
          link= "prod.html#publicacoes"
        },
        { link= "", name= {en= "" } },
        {
          name= {en= "Copyright/License"},
          link= "copyright.html"
        },
        {
          name= {nl= "Download"},
          link= "download.html",
          folder=
          {
            {
              name= {en= "Library Download Tips"},
              link= "download_tips.html"
            }
          }
        },
        {
          name= {nl= "CVS"},
          link= "cvs.html"
        },
        {
          name= {en= "History"},
          link= "history.html",
          folder=
          {
            {
              name= {en= "History 3.x"},
              link= "history3.html",
              folder=
              {
                {
                  name= {en= "Migration Guide"},
                  link= "migration_guide.html"
                }
              }
            },
            {
              name= {en= "History 2.x"},
              link= "history2.html"
            },
            {
              name= {en= "History 1.x"},
              link= "history1.html"
            }
          }
        },
        {
          name= {en= "To Do"},
          link= "to_do.html"
        },
        {
          name= {en= "Comparing"},
          link= "toolkits.html"
        },
        {
          name= {en= "Gallery"},
          link= "gallery.html",
          folder=
          {
            {
              name= {en= "Additional Controls"},
              link= "gallery_ctrl.html"
            },
            {
              name= {en= "Pre-defined Dialogs"},
              link= "gallery_dlg.html"
            }
          }
        },
        {
          name= {en= "Screenshots"},
          link= "screenshots.html"
        }
      }
    },
    {
      name= {en= "Guide"},
      link= "guide.html",
      folder=
      {
        {
          name= {en= "Getting Started"},
          link= "guide.html#start"
        },
        {
          name= {en= "Building Applications"},
          link= "guide.html#apl"
        },
        {
          name= {en= "Building The Library"},
          link= "guide.html#buildlib"
        },
        {
          name= {en= "Using IUP in C++"},
          link= "guide.html#cpp"
        },
        {
          name= {en= "Contributions"},
          link= "guide.html#contrib"
        },
        { link= "", name= {en= "" } },
        {
          name= {en= "Building in Linux"},
          link= "building.html"
        },
        {
          name= {en= "Building in Windows"},
          link= "buildwin.html"
        },
        {
          name= {en= "IDEs"},

          folder=
          {
            {
              name= {nl= "C++ BuilderX"},
              link= "ide_guide/cppbx.html"
            },
            {
              name= {nl= "Code Blocks"},
              link= "ide_guide/codeblocks.html"
            },
            {
              name= {nl= "Dev-C++"},
              link= "ide_guide/dev-cpp.html"
            },
            {
              name= {nl= "Eclipse for C++"},
              link= "ide_guide/eclipse.html"
            },
            {
              name= {nl= "Open Watcom C++"},
              link= "ide_guide/owc.html"
            },
            {
              name= {nl= "Visual C++ 7"},
              link= "ide_guide/msvc.html"
            },
            {
              name= {nl= "Visual C++ 8"},
              link= "ide_guide/msvc8.html"
            },
          }
        },
        {
          name= {en= "Executables"},
          link= "executables.html",
          folder=
          {
            {
              name= {en= "LED Compiler for C"},
              link= "ledc.html"
            },
          }
        },
        {
          name= {en= "Samples"},
          link= "samples.html",
          folder=
          {
            {
              name= {en= "Results"},
              link= "sample_results.html"
            },
            {
              name= {nl= "Dialog BACKGROUND"},
              link= "sample_results_background.html"
            },
            {
              name= {nl= "Dialog BGCOLOR"},
              link= "sample_results_bgcolor.html"
            },
            {
              name= {nl= "Children BGCOLOR"},
              link= "sample_results_bgcolor_indiv.html"
            },
          }
        },
        {
          name= {en= "Lua Binding"},
          link= "iuplua.html",
          folder=
          {
            {
              name= {en= "Basic IupLua"},
              link= "basic/index.html",
              folder=
              {
                {
                  name= {en= "Simple Output"},
                  link= "basic/index.html#Simple_Output"
                },
                {
                  name= {en= "Simple Input"},
                  link= "basic/index.html#Simple_Input"
                },
                {
                  name= {en= "Dialogs"},
                  link= "basic/index.html#Dialogs",
                  folder=
                  {
                    {
                      name= {en= "Layouts"},
                      link= "basic/index.html#Dialogs"
                    },
                    {
                      name= {en= "Timers and Idle"},
                      link= "basic/index.html#Timers"
                    },
                    {
                      name= {en= "Lists"},
                      link= "basic/index.html#Lists"
                    },
                    {
                      name= {en= "Trees"},
                      link= "basic/index.html#Trees"
                    },
                    {
                      name= {en= "Menus"},
                      link= "basic/index.html#Menus"
                    },
                    {
                      name= {en= "Plotting Data"},
                      link= "basic/index.html#Plott"
                    }
                  }
                }
              }
            },
            {
              name= {en= "IupLua Advanced"},
              link= "iuplua_adv.html"
            }
          }
        }
      }
    },
    { link= "", name= {en= "" } },
    {
      name= {en= "System"},
      link= "system.html",
      folder=
      {
        {
          name= {en= "Guide"},
          link= "sys_guide.html",
          folder=
          {
            {
              name= {en= "Initialization"},
              link= "sys_guide.html#init"
            },
            {
              name= {nl= "LED"},
              link= "sys_guide.html#led"
            },
          }
        },
        {
          name= {en= "Reference"},
          folder=
          {
            {
              name= {nl= "IupOpen"},
              link= "func/iupopen.html"
            },
            {
              name= {nl= "IupClose"},
              link= "func/iupclose.html"
            },
            {
              name= {nl= "iuplua_open"},
              link= "func/iuplua_open.html"
            },
            {
              name= {nl= "IupVersion"},
              link= "func/iupversion.html"
            },
            { link= "", name= {en= "" } },
            {
              name= {nl= "IupLoad"},
              link= "func/iupload.html"
            },
            { link= "", name= {en= "" } },
            {
              name= {nl= "IupSetLanguage"},
              link= "func/iupsetlanguage.html"
            },
            {
              name= {nl= "IupGetLanguage"},
              link= "func/iupgetlanguage.html"
            }
          }
        },
        {
          name= {nl= "Drivers"},
          folder=
          {
            {
              name= {nl= "Motif"},
              link= "drv/motif.html"
            },
            {
              name= {nl= "Win32"},
              link= "drv/win32.html"
            },
            {
              name= {nl= "GTK"},
              link= "drv/gtk.html"
            }
          }
        }
      }
    },
    {
      name= {en= "Attributes"},
      link= "attrib.html",
      folder=
      {
        {
          name= {en= "Guide"},
          link= "attrib_guide.html",
          folder=
          {
            {
              name= {nl= "Using"},
              link= "attrib_guide.html#Using"
            },
            {
              name= {nl= "Inheritance"},
              link= "attrib_guide.html#Inheritance"
            },
            {
              name= {nl= "Availability"},
              link= "attrib_guide.html#avail"
            },
            {
              name= {nl= "IupLua"},
              link= "attrib_guide.html#IupLua"
            }
          }
        },
        {
          name= {en= "Functions"},
          folder=
          {
            {
              name= {nl= "IupStoreAttribute"},
              link= "func/iupstoreattribute.html"
            },
            {
              name= {nl= "IupSetAttribute"},
              link= "func/iupsetattribute.html"
            },
            {
              name= {nl= "IupSetfAttribute"},
              link= "func/iupsetfattribute.html"
            },
            {
              name= {nl= "IupSetAttributes"},
              link= "func/iupsetattributes.html"
            },
            {
              name= {nl= "IupResetAttribute"},
              link= "func/iupresetattribute.html"
            },
            {
              name= {nl= "IupSetAtt"},
              link= "func/iupsetatt.html"
            },
            {
              name= {nl= "IupSetAttributeHandle"},
              link= "func/iupsetattributehandle.html"
            },
            { link= "", name= {en= "" } },
            {
              name= {nl= "IupGetAttributeHandle"},
              link= "func/iupgetattributehandle.html"
            },
            {
              name= {nl= "IupGetAttribute"},
              link= "func/iupgetattribute.html"
            },
            {
              name= {nl= "IupGetAllAttributes"},
              link= "func/iupgetallattributes.html"
            },
            {
              name= {nl= "IupGetAttributes"},
              link= "func/iupgetattributes.html"
            },
            {
              name= {nl= "IupGetFloat"},
              link= "func/iupgetfloat.html"
            },
            {
              name= {nl= "IupGetInt"},
              link= "func/iupgetint.html"
            },
            { link= "", name= {en= "" } },
            {
              name= {nl= "IupStoreGlobal"},
              link= "func/iupstoreglobal.html"
            },
            {
              name= {nl= "IupSetGlobal"},
              link= "func/iupsetglobal.html"
            },
            {
              name= {nl= "IupGetGlobal"},
              link= "func/iupgetglobal.html"
            }
          }
        },
        {
          name= {en= "Common"},
          folder=
          {
            {
              name= {nl= "ACTIVE"},
              link= "attrib/iup_active.html"
            },
            {
              name= {nl= "BGCOLOR"},
              link= "attrib/iup_bgcolor.html"
            },
            {
              name= {nl= "FGCOLOR"},
              link= "attrib/iup_fgcolor.html"
            },
            {
              name= {nl= "FONT"},
              link= "attrib/iup_font.html"
            },
            {
              name= {nl= "VISIBLE"},
              link= "attrib/iup_visible.html"
            },
            { link= "", name= {en= "" } },
            {
              name= {nl= "CLIENTSIZE"},
              link= "attrib/iup_clientsize.html"
            },
            {
              name= {nl= "CLIENTOFFSET"},
              link= "attrib/iup_clientoffset.html"
            },
            {
              name= {nl= "EXPAND"},
              link= "attrib/iup_expand.html"
            },
            {
              name= {nl= "MAXSIZE"},
              link= "attrib/iup_maxsize.html"
            },
            {
              name= {nl= "MINSIZE"},
              link= "attrib/iup_minsize.html"
            },
            {
              name= {nl= "RASTERSIZE"},
              link= "attrib/iup_rastersize.html"
            },
            {
              name= {nl= "SIZE"},
              link= "attrib/iup_size.html"
            },
            { link= "", name= {en= "" } },
            {
              name= {nl= "FLOATING"},
              link= "attrib/iup_floating.html"
            },
            {
              name= {nl= "POSITION"},
              link= "attrib/iup_position.html"
            },
            {
              name= {nl= "SCREENPOSITION"},
              link= "attrib/iup_screenposition.html"
            },
            { link= "", name= {en= "" } },
            {
              name= {nl= "NAME"},
              link= "attrib/iup_name.html"
            },
            {
              name= {nl= "TIP"},
              link= "attrib/iup_tip.html"
            },
            {
              name= {nl= "TITLE"},
              link= "attrib/iup_title.html"
            },
            {
              name= {nl= "VALUE"},
              link= "attrib/iup_value.html"
            },
            {
              name= {nl= "WID"},
              link= "attrib/iup_wid.html"
            },
            {
              name= {nl= "ZORDER"},
              link= "attrib/iup_zorder.html"
            },
            { link= "", name= {en= "" } },
            {
              name= {nl= "Drag & Drop"},
              link= "attrib/iup_dragdrop.html"
            },
          }
        },
        {
          name= {en= "Globals"},
          folder=
          {
            {
              name= {nl= "General"},
              link= "attrib/iup_globals.html#General"
            },
            {
              name= {nl= "System Control"},
              link= "attrib/iup_globals.html#Control"
            },
            {
              name= {nl= "System Mouse and Keyboard"},
              link= "attrib/iup_globals.html#Keyboard"
            },
            {
              name= {nl= "System Information"},
              link= "attrib/iup_globals.html#Information"
            },
            {
              name= {nl= "Screen Information"},
              link= "attrib/iup_globals.html#Screen"
            },
            {
              name= {nl= "System Data"},
              link= "attrib/iup_globals.html#Data"
            },
            {
              name= {nl= "Default Attributes"},
              link= "attrib/iup_globals.html#Default"
            },
          }
        }
      }
    },
    {
      name= {en= "Events"},
      link= "call.html",
      folder=
      {
        {
          name= {en= "Guide"},
          link= "call_guide.html",
          folder=
          {
            {
              name= {nl= "Using"},
              link= "call_guide.html#Using"
            },
            {
              name= {nl= "Main Loop"},
              link= "call_guide.html#mainloop"
            },
            {
              name= {nl= "IupLua"},
              link= "call_guide.html#IupLua"
            }
          }
        },
        {
          name= {en= "Functions"},
          folder=
          {
            {
              name= {nl= "IupMainLoop"},
              link= "func/iupmainloop.html"
            },
            {
              name= {nl= "IupMainLoopLevel"},
              link= "func/iupmainlooplevel.html"
            },
            {
              name= {nl= "IupLoopStep"},
              link= "func/iuploopstep.html"
            },
            {
              name= {nl= "IupExitLoop"},
              link= "func/iupexitloop.html"
            },
            {
              name= {nl= "IupFlush"},
              link= "func/iupflush.html"
            },
            { link= "", name= {en= "" } },
            {
              name= {nl= "IupGetCallback"},
              link= "func/iupgetcallback.html"
            },
            {
              name= {nl= "IupSetCallback"},
              link= "func/iupsetcallback.html"
            },
            {
              name= {nl= "IupSetCallbacks"},
              link= "func/iupsetcallbacks.html"
            },
            {
              name= {nl= "IupGetActionName"},
              link= "func/iupgetactionname.html"
            },
            {
              name= {nl= "IupGetFunction"},
              link= "func/iupgetfunction.html"
            },
            {
              name= {nl= "IupSetFunction"},
              link= "func/iupsetfunction.html"
            },
            { link= "", name= {en= "" } },
            {
              name= {nl= "IupRecordInput"},
              link= "func/iuprecordinput.html"
            },
            {
              name= {nl= "IupPlayInput"},
              link= "func/iupplayinput.html"
            },
          }
        },
        {
          name= {en= "Common"},
          folder=
          {
            {
              name= {nl= "DEFAULT_ACTION"},
              link= "call/iup_default_action.html"
            },
            {
              name= {nl= "IDLE_ACTION"},
              link= "call/iup_idle_action.html"
            },
            { link= "", name= {en= "" } },
            {
              name= {nl= "MAP_CB"},
              link= "call/iup_map_cb.html"
            },
            {
              name= {nl= "UNMAP_CB"},
              link= "call/iup_unmap_cb.html"
            },
            {
              name= {nl= "DESTROY_CB"},
              link= "call/iup_destroy_cb.html"
            },
            {
              name= {nl= "GETFOCUS_CB"},
              link= "call/iup_getfocus_cb.html"
            },
            {
              name= {nl= "KILLFOCUS_CB"},
              link= "call/iup_killfocus_cb.html"
            },
            {
              name= {nl= "ENTERWINDOW_CB"},
              link= "call/iup_enterwindow_cb.html"
            },
            {
              name= {nl= "LEAVEWINDOW_CB"},
              link= "call/iup_leavewindow_cb.html"
            },
            {
              name= {nl= "K_ANY"},
              link= "call/iup_k_any.html"
            },
            {
              name= {nl= "HELP_CB"},
              link= "call/iup_help_cb.html"
            },
            { link= "", name= {en= "" } },
            {
              name= {nl= "ACTION"},
              link= "call/iup_action.html"
            }
          }
        }
      }
    },
    {
      name= {en= "Layout"},
      link= "layout.html",
      folder=
      {
        {
          name= {nl= "Guide"},
          link= "layout_guide.html",
          folder=
          {
            {
              name= {nl= "Native Sizes"},
              link= "layout_guide.html#native"
            },
            {
              name= {nl= "IUP Sizes"},
              link= "layout_guide.html#iup"
            },
            {
              name= {nl= "Hierarchy"},
              link= "layout_guide.html#Hierarchy"
            },
            {
              name= {nl= "Display"},
              link= "layout_guide.html#Display"
            }
          }
        },
        {
          name= {en= "Construction"},
          folder=
          {
            {
              name= {nl= "IupCreate"},
              link= "func/iupcreate.html"
            },
            {
              name= {nl= "IupDestroy"},
              link= "func/iupdestroy.html"
            },
            {
              name= {nl= "IupMap"},
              link= "func/iupmap.html"
            },
            {
              name= {nl= "IupUnmap"},
              link= "func/iupunmap.html"
            },
            { link= "", name= {en= "" } },
            {
              name= {nl= "IupGetAllClasses"},
              link= "func/iupgetallclasses.html"
            },
            {
              name= {nl= "IupGetClassName"},
              link= "func/iupgetclassname.html"
            },
            {
              name= {nl= "IupGetClassType"},
              link= "func/iupgetclasstype.html"
            },
            {
              name= {nl= "IupClassMatch"},
              link= "func/iupclassmatch.html"
            },
            {
              name= {nl= "IupGetClassAttributes"},
              link= "func/iupgetclassattributes.html"
            },
            {
              name= {nl= "IupGetClassCallbacks"},
              link= "func/iupgetclasscallbacks.html"
            },
            {
              name= {nl= "IupSaveClassAttributes"},
              link= "func/iupsaveclassattributes.html"
            },
            {
              name= {nl= "IupCopyClassAttributes"},
              link= "func/iupcopyclassattributes.html"
            },
            {
              name= {nl= "IupSetClassDefaultAttribute"},
              link= "func/iupsetclassdefaultattribute.html"
            },
          }
        },
        {
          name= {en= "Composition"},
          folder=
          {
            {
              name= {nl= "IupFill"},
              link= "elem/iupfill.html"
            },
            {
              name= {nl= "IupHbox"},
              link= "elem/iuphbox.html"
            },
            {
              name= {nl= "IupVbox"},
              link= "elem/iupvbox.html"
            },
            {
              name= {nl= "IupZbox"},
              link= "elem/iupzbox.html"
            },
            { link= "", name= {en= "" } },
            {
              name= {nl= "IupRadio"},
              link= "elem/iupradio.html"
            },
            {
              name= {nl= "IupNormalizer"},
              link= "elem/iupnormalizer.html"
            },
            { link= "", name= {en= "" } },
            {
              name= {nl= "IupCbox"},
              link= "elem/iupcbox.html"
            },
            {
              name= {nl= "IupSbox"},
              link= "elem/iupsbox.html"
            },
            {
              name= {nl= "IupSplit"},
              link= "elem/iupsplit.html"
            }
          }
        },
        {
          name= {en= "Hierarchy"},
          folder=
          {
            {
              name= {nl= "IupAppend"},
              link= "func/iupappend.html"
            },
            {
              name= {nl= "IupDetach"},
              link= "func/iupdetach.html"
            },
            {
              name= {nl= "IupInsert"},
              link= "func/iupinsert.html"
            },
            {
              name= {nl= "IupReparent"},
              link= "func/iupreparent.html"
            },
            { link= "", name= {en= "" } },
            {
              name= {nl= "IupGetParent"},
              link= "func/iupgetparent.html"
            },
            {
              name= {nl= "IupGetChild"},
              link= "func/iupgetchild.html"
            },
            {
              name= {nl= "IupGetChildPos"},
              link= "func/iupgetchildpos.html"
            },
            {
              name= {nl= "IupGetChildCount"},
              link= "func/iupgetchildcount.html"
            },
            {
              name= {nl= "IupGetNextChild"},
              link= "func/iupgetnextchild.html"
            },
            {
              name= {nl= "IupGetBrother"},
              link= "func/iupgetbrother.html"
            },
            {
              name= {nl= "IupGetDialog"},
              link= "func/iupgetdialog.html"
            },
            {
              name= {nl= "IupGetDialogChild"},
              link= "func/iupgetdialogchild.html"
            }
          }
        },
        {
          name= {en= "Utilities"},
          folder=
          {
            {
              name= {nl= "IupRefresh"},
              link= "func/iuprefresh.html"
            },
            {
              name= {nl= "IupRefreshChildren"},
              link= "func/iuprefreshchildren.html"
            },
            {
              name= {nl= "IupUpdate"},
              link= "func/iupupdate.html"
            },
            {
              name= {nl= "IupRedraw"},
              link= "func/iupredraw.html"
            },
            { link= "", name= {en= "" } },
            {
              name= {nl= "IupConvertXYToPos"},
              link= "func/iupconvertxytopos.html"
            }
          }
        }
      }
    },
    {
      name= {en= "Dialogs"},
      link= "dialogs.html",
      folder=
      {
        {
          name= {en= "Reference"},
          folder=
          {
            {
              name= {nl= "IupDialog"},
              link= "dlg/iupdialog.html",

              folder=
              {
                {
                  name= {nl= "Attributes"},
                  link= "dlg/iupdialog.html#Attributes",

                  folder=
                  {
                    {
                      name= {nl= "CURSOR"},
                      link= "attrib/iup_cursor.html"
                    },
                    {
                      name= {nl= "ICON"},
                      link= "attrib/iup_icon.html"
                    },
                    {
                      name= {nl= "PARENTDIALOG"},
                      link= "attrib/iup_parentdialog.html"
                    },
                    {
                      name= {nl= "SHRINK"},
                      link= "attrib/iup_shrink.html"
                    },
                    {
                      name= {nl= "CONTROL"},
                      link= "attrib/iup_control.html"
                    }
                  }
                },
                {
                  name= {nl= "Callbacks"},
                  link= "dlg/iupdialog.html#Callbacks",

                  folder=
                  {
                    {
                      name= {nl= "CLOSE_CB"},
                      link= "call/iup_close_cb.html"
                    },
                    {
                      name= {nl= "DROPFILES_CB"},
                      link= "call/iup_dropfiles_cb.html"
                    },
                    {
                      name= {nl= "RESIZE_CB"},
                      link= "call/iup_resize_cb.html"
                    },
                    {
                      name= {nl= "SHOW_CB"},
                      link= "call/iup_show_cb.html"
                    }
                  }
                }
              }
            },
            { link= "", name= {en= "" } },
            {
              name= {nl= "IupPopup"},
              link= "func/iuppopup.html"
            },
            {
              name= {nl= "IupShow"},
              link= "func/iupshow.html"
            },
            {
              name= {nl= "IupShowXY"},
              link= "func/iupshowxy.html"
            },
            {
              name= {nl= "IupHide"},
              link= "func/iuphide.html"
            }
          }
        },
        {
          name= {en= "Predefined"},
          folder=
          {
            {
              name= {nl= "IupFileDlg"},
              link= "dlg/iupfiledlg.html"
            },
            {
              name= {nl= "IupMessageDlg"},
              link= "dlg/iupmessagedlg.html"
            },
            {
              name= {nl= "IupColorDlg"},
              link= "dlg/iupcolordlg.html"
            },
            {
              name= {nl= "IupFontDlg"},
              link= "dlg/iupfontdlg.html"
            },
            { link= "", name= {en= "" } },
            {
              name= {nl= "IupAlarm"},
              link= "dlg/iupalarm.html"
            },
            {
              name= {nl= "IupGetFile"},
              link= "dlg/iupgetfile.html"
            },
            {
              name= {nl= "IupGetColor"},
              link= "dlg/iupgetcolor.html"
            },
            {
              name= {nl= "IupGetParam"},
              link= "dlg/iupgetparam.html"
            },
            {
              name= {nl= "IupGetText"},
              link= "dlg/iupgettext.html"
            },
            {
              name= {nl= "IupListDialog"},
              link= "dlg/iuplistdialog.html"
            },
            {
              name= {nl= "IupMessage"},
              link= "dlg/iupmessage.html"
            },
            {
              name= {nl= "IupScanf"},
              link= "dlg/iupscanf.html"
            },
            { link= "", name= {en= "" } },
            {
              name= {nl= "IupLayoutDialog"},
              link= "dlg/iuplayoutdialog.html"
            },
            {
              name= {nl= "IupElementPropertiesDialog"},
              link= "dlg/iupelementpropdialog.html"
            },
          }
        }
      }
    },
    {
      name= {en= "Controls"},
      link= "controls.html",
      folder=
      {
        {
          name= {en= "Standard"},

          folder=
          {
            {
              name= {nl= "IupButton"},
              link= "elem/iupbutton.html"
            },
            {
              name= {nl= "IupCanvas"},
              link= "elem/iupcanvas.html",

              folder=
              {
                {
                  name= {nl= "Attributes"},
                  link= "elem/iupcanvas.html#Attributes",

                  folder=
                  {
                    {
                      name= {nl= "CURSOR"},
                      link= "attrib/iup_cursor.html"
                    },
                    {
                      name= {nl= "SCROLLBAR"},
                      link= "attrib/iup_scrollbar.html"
                    },
                    {
                      name= {nl= "DX"},
                      link= "attrib/iup_dx.html"
                    },
                    {
                      name= {nl= "DY"},
                      link= "attrib/iup_dy.html"
                    },
                    {
                      name= {nl= "POSX"},
                      link= "attrib/iup_posx.html"
                    },
                    {
                      name= {nl= "POSY"},
                      link= "attrib/iup_posy.html"
                    },
                    {
                      name= {nl= "XMIN"},
                      link= "attrib/iup_xmin.html"
                    },
                    {
                      name= {nl= "XMAX"},
                      link= "attrib/iup_xmax.html"
                    },
                    {
                      name= {nl= "YMIN"},
                      link= "attrib/iup_ymin.html"
                    },
                    {
                      name= {nl= "YMAX"},
                      link= "attrib/iup_ymax.html"
                    }
                  }
                },
                {
                  name= {nl= "Callbacks"},
                  link= "elem/iupcanvas.html#Callbacks",

                  folder=
                  {
                    {
                      name= {nl= "BUTTON_CB"},
                      link= "call/iup_button_cb.html"
                    },
                    {
                      name= {nl= "DROPFILES_CB"},
                      link= "call/iup_dropfiles_cb.html"
                    },
                    {
                      name= {nl= "MOTION_CB"},
                      link= "call/iup_motion_cb.html"
                    },
                    {
                      name= {nl= "KEYPRESS_CB"},
                      link= "call/iup_keypress_cb.html"
                    },
                    {
                      name= {nl= "RESIZE_CB"},
                      link= "call/iup_resize_cb.html"
                    },
                    {
                      name= {nl= "SCROLL_CB"},
                      link= "call/iup_scroll_cb.html"
                    },
                    {
                      name= {nl= "WHEEL_CB"},
                      link= "call/iup_wheel_cb.html"
                    },
                    {
                      name= {nl= "WOM_CB"},
                      link= "call/iup_wom_cb.html"
                    }
                  }
                }
              }
            },
            {
              name= {nl= "IupFrame"},
              link= "elem/iupframe.html"
            },
            {
              name= {nl= "IupLabel"},
              link= "elem/iuplabel.html"
            },
            {
              name= {nl= "IupList"},
              link= "elem/iuplist.html"
            },
            {
              name= {nl= "IupMultiLine"},
              link= "elem/iupmultiline.html"
            },
            {
              name= {nl= "IupProgressBar"},
              link= "elem/iupprogressbar.html"
            },
            {
              name= {nl= "IupSpin"},
              link= "elem/iupspin.html"
            },
            {
              name= {nl= "IupTabs"},
              link= "elem/iuptabs.html"
            },
            {
              name= {nl= "IupText"},
              link= "elem/iuptext.html",
              folder=
              {
                {
                  name= {nl= "FORMATTING"},
                  link= "attrib/iup_formatting.html"
                },
                {
                  name= {nl= "MASK"},
                  link= "attrib/iup_mask.html"
                }
              }
            },
            {
              name= {nl= "IupToggle"},
              link= "elem/iuptoggle.html"
            },
            {
              name= {nl= "IupTree"},
              link= "elem/iuptree.html",

              folder=
              {
                {
                  name= {nl= "Attributes"},
                  link= "elem/iuptree_attrib.html"
                },
                {
                  name= {nl= "Callbacks"},
                  link= "elem/iuptree_cb.html"
                }
              }
            },
            {
              name= {nl= "IupVal"},
              link= "elem/iupval.html"
            }
          }
        },
        {
          name= {en= "Additional"},
          link= "iupcontrols.html",

          folder=
          {
            {
              name= {nl= "IupCells"},
              link= "ctrl/iupcells.html"
            },
            {
              name= {nl= "IupColorbar"},
              link= "ctrl/iupcolorbar.html"
            },
            {
              name= {nl= "IupColorBrowser"},
              link= "ctrl/iupcolorbrowser.html"
            },
            {
              name= {nl= "IupDial"},
              link= "ctrl/iupdial.html"
            },
            {
              name= {nl= "IupMatrix"},
              link= "ctrl/iupmatrix.html",

              folder=
              {
                {
                  name= {nl= "Attributes"},
                  link= "ctrl/iupmatrix_attrib.html"
                },
                {
                  name= {nl= "Callbacks"},
                  link= "ctrl/iupmatrix_cb.html"
                }
              }
            },
            { link= "", name= {en= "" } },
            {
              name= {nl= "IupGauge (OLD)"},
              link= "ctrl/iupgauge.html"
            },
            { link= "", name= {en= "" } },
            {
              name= {nl= "IupGLCanvas"},
              link= "ctrl/iupglcanvas.html"
            },
            {
              name= {nl= "IupPPlot"},
              link= "ctrl/iup_pplot.html"
            },
            {
              name= {nl= "IupMglPlot"},
              link= "ctrl/iup_mglplot.html",
              folder=
              {
                {
                  name= {nl= "DS_MODE"},
                  link= "ctrl/iup_mglplot_modes.html"
                }
              }
            },
            {
              name= {nl= "IupOleControl"},
              link= "ctrl/iupole.html"
            },
            {
              name= {nl= "IupWebBrowser"},
              link= "ctrl/iupweb.html"
            },
          }
        }
      }
    },
    {
      name= {en= "Resources"},
      link= "resources.html",
      folder=
      {
        {
          name= {en= "Fonts"},

          folder=
          {
            {
              name= {nl= "FONT (2.x)"},
              link= "attrib/iup_font2.html"
            },
            {
              name= {nl= "FONT (3.0)"},
              link= "attrib/iup_font.html"
            },
            {
              name= {nl= "IupMapFont"},
              link= "func/iupmapfont.html"
            },
            {
              name= {nl= "IupUnMapFont"},
              link= "func/iupunmapfont.html"
            },
          }
        },
        {
          name= {en= "Images"},

          folder=
          {
            {
              name= {nl= "IupImage"},
              link= "elem/iupimage.html"
            },
            {
              name= {nl= "IupImageLib (2.x)"},
              link= "iupimglib2.html"
            },
            {
              name= {nl= "IupImageLib (3.0)"},
              link= "iupimglib.html"
            },
            {
              name= {nl= "Iup-IM"},
              link= "iupim.html"
            },
            {
              name= {nl= "IupSaveImageAsText"},
              link= "func/iupsaveimageastext.html"
            }
          }
        },
        {
          name= {en= "Keyboard"},
          link= "keyboard.html",
          folder=
          {
            {
              name= {en= "Codes"},
              link= "attrib/key.html"
            },
            {
              name= {en= "Reference"},
              folder=
              {
                {
                  name= {nl= "IupNextField"},
                  link= "func/iupnextfield.html"
                },
                {
                  name= {nl= "IupPreviousField"},
                  link= "func/iuppreviousfield.html"
                },
                {
                  name= {nl= "IupGetFocus"},
                  link= "func/iupgetfocus.html"
                },
                {
                  name= {nl= "IupSetFocus"},
                  link= "func/iupsetfocus.html"
                }
              }
            }
          }
        },
        {
          name= {en= "Menus"},

          folder=
          {
            {
              name= {nl= "IupItem"},
              link= "elem/iupitem.html"
            },
            {
              name= {nl= "IupMenu"},
              link= "elem/iupmenu.html"
            },
            {
              name= {nl= "IupSeparator"},
              link= "elem/iupseparator.html"
            },
            {
              name= {nl= "IupSubmenu"},
              link= "elem/iupsubmenu.html"
            },
            {
              name= {en= "Attributes"},

              folder=
              {
                {
                  name= {nl= "KEY"},
                  link= "attrib/iup_key.html"
                }
              }
            },
            {
              name= {en= "Callbacks"},

              folder=
              {
                {
                  name= {nl= "HIGHLIGHT_CB"},
                  link= "call/iup_highlight_cb.html"
                },
                {
                  name= {nl= "OPEN_CB"},
                  link= "call/iup_open_cb.html"
                },
                {
                  name= {nl= "MENUCLOSE_CB"},
                  link= "call/iup_menuclose_cb.html"
                }
              }
            }
          }
        },
        {
          name= {en= "Names"},

          folder=
          {
            {
              name= {nl= "IupSetHandle"},
              link= "func/iupsethandle.html"
            },
            {
              name= {nl= "IupGetHandle"},
              link= "func/iupgethandle.html"
            },
            {
              name= {nl= "IupGetName"},
              link= "func/iupgetname.html"
            },
            {
              name= {nl= "IupGetAllNames"},
              link= "func/iupgetallnames.html"
            },
            {
              name= {nl= "IupGetAllDialogs"},
              link= "func/iupgetalldialogs.html"
            }
          }
        },
        { link= "", name= {en= "" } },
        {
          name= {nl= "IupClipboard"},
          link= "elem/iupclipboard.html"
        },
        {
          name= {nl= "IupTimer"},
          link= "elem/iuptimer.html"
        },
        {
          name= {nl= "IupTuio"},
          link= "ctrl/iuptuio.html"
        },
        {
          name= {nl= "IupUser"},
          link= "elem/iupuser.html"
        },
        { link= "", name= {en= "" } },
        {
          name= {nl= "IupHelp"},
          link= "func/iuphelp.html"
        },
        {
          name= {nl= "iupMask"},
          link= "ctrl/iupmask.html"
        }
      }
    },
    { link= "", name= {en= "" } },
    {
      name= {en= "Internal SDK"},
      link= "../doxygen/index.html",
      folder=
      {
        {
          name= {en= "Control SDK"},

          link= "../doxygen/group__cpi.html",
          folder=
          {
            {
              name= {nl= "Guide"},
              link= "cpi.html"
            },
            {
              name= {nl= "Ihandle Object"},
              link= "../doxygen/group__object.html"
            },
            {
              name= {nl= "Ihandle Class"},
              link= "../doxygen/group__iclass.html"
            },
            {
              name= {nl= "Class Object Functions"},
              link= "../doxygen/group__iclassobject.html"
            },
            {
              name= {nl= "Base Class"},
              link= "../doxygen/group__iclassbase.html"
            },
            {
              name= {nl= "Base Class Methods"},
              link= "../doxygen/group__iclassbasemethod.html"
            },
            {
              name= {nl= "Base Class Attribute Functions"},
              link= "../doxygen/group__iclassbaseattribfunc.html"
            },
            {
              name= {nl= "Base Class Utilities"},
              link= "../doxygen/group__iclassbaseutil.html"
            },
            {
              name= {nl= "Class Registration"},
              link= "../doxygen/group__register.html"
            },
            {
              name= {nl= "Attribute Environment"},
              link= "../doxygen/group__attrib.html"
            },
            {
              name= {nl= "Child Tree"},
              link= "../doxygen/group__childtree.html"
            },
            {
              name= {nl= "List of Dialogs"},
              link= "../doxygen/group__dlglist.html"
            },
            {
              name= {nl= "Keyboard Focus"},
              link= "../doxygen/group__focus.html"
            },
            {
              name= {nl= "Key Coding"},
              link= "../doxygen/group__key.html"
            }
          }
        },
        {
          name= {en= "Driver Interface"},

          link= "../doxygen/group__drv.html",
          folder=
          {
            {
              name= {nl= "Driver Font Interface"},
              link= "../doxygen/group__drvfont.html"
            },
            {
              name= {nl= "Driver Information Interface"},
              link= "../doxygen/group__drvinfo.html"
            }
          }
        },
        {
          name= {en= "Utilities"},
          link= "../doxygen/group__util.html",

          folder=
          {
            {
              name= {nl= "Assert Utilities"},
              link= "../doxygen/group__assert.html"
            },
            {
              name= {nl= "Hash Table"},
              link= "../doxygen/group__table.html"
            },
            {
              name= {nl= "Language Dependent Messages"},
              link= "../doxygen/group__strmessage.html"
            },
            {
              name= {nl= "Simple Array"},
              link= "../doxygen/group__iarray.html"
            },
            {
              name= {nl= "Simple Draw"},
              link= "../doxygen/group__draw.html"
            },
            {
              name= {nl= "String Utilities"},
              link= "../doxygen/group__str.html"
            },
          }
        },
        {
          name= {en= "Globals"},

          link= "../doxygen/globals.html"
        }
      }
    }
  }
}
