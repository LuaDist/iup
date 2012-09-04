-- This file is executed with the "iup" table already as the globalindex

----------------------------------------------------------------------------
--  Common return values              
----------------------------------------------------------------------------
iup.ERROR   =  1
iup.NOERROR =  0
iup.OPENED  = -1
iup.INVALID = -1

----------------------------------------------------------------------------
--  Callback return values              
----------------------------------------------------------------------------
iup.IGNORE   = -1
iup.DEFAULT  = -2
iup.CLOSE    = -3
iup.CONTINUE = -4

----------------------------------------------------------------------------
--  IupPopup e IupShowXY        
----------------------------------------------------------------------------
iup.CENTER       = 65535
iup.LEFT         = 65534
iup.RIGHT        = 65533
iup.MOUSEPOS     = 65532
iup.CURRENT      = 65531
iup.CENTERPARENT = 65530
iup.TOP = LEFT
iup.BOTTOM = RIGHT

----------------------------------------------------------------------------
--  Scrollbar
----------------------------------------------------------------------------
iup.SBUP      = 0  
iup.SBDN      = 1  
iup.SBPGUP    = 2  
iup.SBPGDN    = 3  
iup.SBPOSV    = 4  
iup.SBDRAGV   = 5  
iup.SBLEFT    = 6  
iup.SBRIGHT   = 7  
iup.SBPGLEFT  = 8  
iup.SBPGRIGHT = 9  
iup.SBPOSH    = 10 
iup.SBDRAGH   = 11 

----------------------------------------------------------------------------
--  SHOW_CB                      
----------------------------------------------------------------------------
iup.SHOW     = 0
iup.RESTORE  = 1
iup.MINIMIZE = 2
iup.MAXIMIZE = 3
iup.HIDE     = 4

----------------------------------------------------------------------------
--  BUTTON_CB        
----------------------------------------------------------------------------
iup.BUTTON1 = string.byte('1')
iup.BUTTON2 = string.byte('2')
iup.BUTTON3 = string.byte('3')
iup.BUTTON4 = string.byte('4')
iup.BUTTON5 = string.byte('5')

----------------------------------------------------------------------------
--  Record Input
----------------------------------------------------------------------------
iup.RECBINARY = 0
iup.RECTEXT = 1

----------------------------------------------------------------------------
--  Pre-Defined Masks        
----------------------------------------------------------------------------
iup.MASK_FLOAT   = "[+/-]?(/d+/.?/d*|/./d+)"
iup.MASK_UFLOAT  = "(/d+/.?/d*|/./d+)"
iup.MASK_EFLOAT  = "[+/-]?(/d+/.?/d*|/./d+)([eE][+/-]?/d+)?"
iup.MASK_INT     = "[+/-]?/d+"
iup.MASK_UINT    = "/d+"

----------------------------------------------------------------------------
--  IupGetParam Callback situations
----------------------------------------------------------------------------
iup.GETPARAM_OK     = -1
iup.GETPARAM_INIT   = -2
iup.GETPARAM_CANCEL = -3
iup.GETPARAM_HELP   = -4
