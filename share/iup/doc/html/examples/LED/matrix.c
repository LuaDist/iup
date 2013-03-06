/* To implement the callbacks associated to the ‘Alterar’ menu and to the title and dimension dialogs, use the C code below. */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "iup.h"
#include "iupkey.h"
#include "iupcontrols.h"
#include "cd.h"
#include "cdiup.h"

Ihandle *dlg, *moldura, *matriz;
int col = 2 ; /* number of columns in the matrix */
int lin = 3 ; /* number of lines in the matrix */
unsigned int dimensao_limite = 2 ; /* maximum number of characters in the dimension dialog */
unsigned int titulo_limite = 20 ; /* maximum number of characters in the title dialog */

/* callback of the cancel button */
int cancel_cb(void)
{
  return IUP_CLOSE ;
}

/* callback for entering text in the title dialog */
int titulo_texto_cb(Ihandle* self, int c, char* after)
{
  if(strlen(after)>titulo_limite)
  {
    IupMessage("Título","Título limitado a vinte caracteres");

    return IUP_IGNORE ;
  }
  return IUP_DEFAULT ;
} 

/* callback of the ok button in the title dialog */
int titulo_ok_cb(void)
{
  Ihandle* texto ;

  texto = IupGetHandle("titulo_texto");
  
  IupSetHandle("valor_do_titulo",IupGetAttribute(texto,"VALUE"));

  return IUP_CLOSE ;
}

/* function that changes the title */
void alterar_titulo(int modo)
{
  if((IupGetAttribute(matriz,"MARKED") == NULL)&&(modo != -1))
  {
    IupMessage("Alterar título","Não há células marcadas");
  }
  else
  {
    int i = 0 , c = 1 ;
    char* marked;
    char* string;
    char* temp;
    char* valor;
    Ihandle* titulo_texto ;
    Ihandle* dlg_titulo ;
    titulo_texto = IupGetHandle("titulo_texto");
    
    dlg_titulo = IupGetHandle("dlg_titulo");

    valor = (char*)malloc((sizeof(char)*titulo_limite)+1);

    IupSetFunction("titulo_texto_act", (Icallback)titulo_texto_cb);
    
    if(modo == -1)
    {
      IupSetAttribute(dlg_titulo, "TITLE", "Geral");
    }
    else
    if(modo == 0)
    {
      IupSetAttribute(dlg_titulo, "TITLE", "Coluna");
    }
    else
    {
      IupSetAttribute(dlg_titulo, "TITLE", "Linha");
    }

    IupSetAttribute(titulo_texto,"VALUE","");
    IupSetHandle("valor_do_titulo","\b");
    IupPopup(dlg_titulo,IUP_CENTER,IUP_CENTER);
    strcpy(valor, IupGetHandle("valor_do_titulo"));

    if(strcmp(valor,"\b")) 
    {
      if (modo == -1)
      {
        IupStoreAttribute(matriz, "0:0", valor);
      }
      else
      {
        marked = (char*)malloc(sizeof(char)*(lin*col)+1);
        strcpy(marked, IupGetAttribute(matriz,"MARKED"));
        string = (char*)malloc((sizeof(char)*2*dimensao_limite+strlen(":"))+1);
        temp = (char*)malloc((sizeof(char)*dimensao_limite)+1);
        
        while(i < col*lin)
        {
          if(marked[i] == '1')
          {
            if(modo == 0)
            {
              sprintf(temp,"%d",c);
              strcpy(string,"0:");
              strcat(string,temp);
            }
            else
            {
              sprintf(temp,"%d",i/col+1);
              strcpy(string,temp);
              strcat(string,":0");
            }
            IupStoreAttribute(matriz, string, valor);
          }
          i++;
          c++;
          if(c == col+1) c=1 ;
        }
        free(temp);
        free(string);
        free(marked);
      }
    }
    free(valor);
  }
}

int titulogeral_cb(void)
{
  alterar_titulo(-1) ;

  return IUP_DEFAULT;
}

int titulocol_cb(void)
{
  alterar_titulo(0) ;

  return IUP_DEFAULT;
}

int titulolin_cb(void)
{
  alterar_titulo(1) ;

  return IUP_DEFAULT;
}

void alterarcol(char* modo, int pos)
{
  if(IupGetAttribute(matriz,"MARKED") == NULL)
  {
    IupMessage("Alterar coluna","Não há células marcadas");
  }
  else
  if((strcmp(modo,IUP_DELCOL)==0)&&(col == 1))
  {
    IupMessage("Alterar coluna","Última coluna não pode ser removida") ;
  }
  else
  {
    int i = 0 , c = pos ;
    char* marked;
    char* temp;
    marked = (char*)malloc((sizeof(char)*lin*col)+1);
    strcpy(marked, IupGetAttribute(matriz,"MARKED"));
    temp = (char*)malloc((sizeof(char)*dimensao_limite)+1);
    
    while(i < col*lin && marked[i] == '0') { i++ ; c++ ; if(c == col+pos) c=pos ; }

    if((((strcmp(modo,IUP_DELCOL)==0)&&(pos == 0)&&(c == 0)))||
       (((strcmp(modo,IUP_DELCOL)==0)&&(pos == 1)&&(c == col))))
    {
      IupMessage("Alterar coluna","Coluna fora da matriz") ;
    }
    else
    {
      sprintf(temp,"%d",c);
	    IupSetAttribute(matriz,modo,temp);
      if(strcmp(modo,IUP_ADDCOL)==0)
      {
	      col++ ;
      }
      else
      {
        col-- ;
      } 
    }
    free(temp);
    free(marked);
  }
}

int adicionarcolesq_cb(void)
{
  alterarcol(IUP_ADDCOL, 0);
    
  return IUP_DEFAULT;
}

int adicionarcoldir_cb(void)
{
  alterarcol(IUP_ADDCOL, 1);
    
  return IUP_DEFAULT;
}

int removercolesq_cb(void)
{
  alterarcol(IUP_DELCOL, 0);
    
  return IUP_DEFAULT;
}

int removercoldir_cb(void)
{
  alterarcol(IUP_DELCOL, 1);
    
  return IUP_DEFAULT;
}

void alterarlin(char* modo, int pos)
{
  if(IupGetAttribute(matriz,"MARKED") == NULL)
  {
    IupMessage("Alterar linha","Não há células marcadas");
  }
  else
  if((strcmp(modo,IUP_DELLIN)==0)&&(lin == 1))
  {
    IupMessage("Alterar linha","Última linha não pode ser removida") ;
  }
  else
  {
    int i = 0 , l = pos ;
    char* marked;
    char* temp;
    marked = (char*)malloc((sizeof(char)*lin*col)+1);
    strcpy(marked, IupGetAttribute(matriz,"MARKED"));
    temp = (char*)malloc((sizeof(char)*dimensao_limite)+1);
        
    while(i < col*lin && marked[i] == '0') i++ ;

    if((((strcmp(modo,IUP_DELLIN)==0)&&(pos == 0)&&(i/col+pos == 0)))||
       (((strcmp(modo,IUP_DELLIN)==0)&&(pos == 1)&&(i/col+pos == lin))))
    {
      IupMessage("Alterar linha","Linha fora da matriz");
    }
    else
    {
      sprintf(temp,"%d",i/col+pos);
	    IupSetAttribute(matriz,modo,temp);
      if(strcmp(modo,IUP_ADDLIN)==0)
      {
	      lin++ ;
      }
      else
      {
        lin-- ;
      }
    }
    free(temp);
    free(marked);
  }
}

int adicionarlinacima_cb(void)
{
  alterarlin(IUP_ADDLIN, 0);
  
  return IUP_DEFAULT;
}

int adicionarlinabaixo_cb(void)
{
  alterarlin(IUP_ADDLIN, 1);
  
  return IUP_DEFAULT;
}

int removerlinacima_cb(void)
{
  alterarlin(IUP_DELLIN, 0);
  
  return IUP_DEFAULT;
}

int removerlinabaixo_cb(void)
{
  alterarlin(IUP_DELLIN, 1);
  
  return IUP_DEFAULT;
}

int dimensao_texto_cb(Ihandle* self, int c, char* after)
{
  if((c==K_BS)||(c==K_DEL))return IUP_DEFAULT;
  
  if((c<K_0)||(c>K_9))
  {
    IupMessage("Dimensao","Entrada inválida");

    return IUP_IGNORE ;
  }
  else
  if(strlen(after)>dimensao_limite)
  {
    IupMessage("Dimensao","Dimensao limitadas a duas casas decimais");

    return IUP_IGNORE ;
  }
  else
  if(strcmp(IupGetAttribute(self,"VALUE"),"0")==0)
  {
    char* string ;
    string = (char*)malloc((sizeof(char)*dimensao_limite)+1) ;
    sprintf(string,"%c",c) ;
    IupSetAttribute(self,"VALUE",string);
    IupSetAttribute(self,"CARET","2");
    free(string);
  }

  return IUP_DEFAULT ;
}

int dimensao_ok_cb(void)
{
  Ihandle* texto ;

  texto = IupGetHandle("dimensao_texto");
  if(strcmp(IupGetAttribute(texto,"VALUE"),"0")==0)
  {
    IupMessage("Dimensao","Dimensao não pode ser nula");

    return IUP_CLOSE ;
  }
  IupSetHandle("valor_da_dimensao",IupGetAttribute(texto,"VALUE"));

  return IUP_CLOSE ;
}

void alterar_dimensao(char* modo)
{
  if(IupGetAttribute(matriz,"MARKED") == NULL)
  {
    IupMessage("Alterar dimensão","Não há células marcadas");
  }
  else
  {
    int i = 0 , c = 1 ;
    char* marked;
    char* string;
    char* temp;
    char* valor;
    Ihandle* dimensao_texto ;
    Ihandle* dlg_dimensao ;
    valor = (char*)malloc((sizeof(char)*dimensao_limite)+1);

    IupSetFunction("dimensao_texto_act", (Icallback)dimensao_texto_cb);
    dlg_dimensao = IupGetHandle("dlg_dimensao");
    dimensao_texto = IupGetHandle("dimensao_texto");
    
    if(strcmp(modo,"HEIGHT")==0)
    {
      IupSetAttribute(dlg_dimensao, "TITLE", "Altura");
    }
    else
    {
      IupSetAttribute(dlg_dimensao, "TITLE", "Largura");
    }
    IupSetAttribute(dimensao_texto,"VALUE",0);
    IupSetHandle("valor_da_dimensao","0");
    IupPopup(dlg_dimensao,IUP_CENTER,IUP_CENTER);
    strcpy(valor, IupGetHandle("valor_da_dimensao"));

    if(strcmp(valor,"0")) 
    {
      marked = (char*)malloc(sizeof(char)*(lin*col)+1);
      strcpy(marked, IupGetAttribute(matriz,"MARKED"));
      string = (char*)malloc((sizeof(char)*strlen(modo)+dimensao_limite)+1);
      temp = (char*)malloc((sizeof(char)*dimensao_limite)+1);
        
      while(i < col*lin)
      {
        if(marked[i] == '1')
        {
          if(strcmp(modo,"HEIGHT")==0)
          {
            sprintf(temp,"%d",i/col+1);
          }
          else
          {
            sprintf(temp,"%d",c);
          }
          strcpy(string,modo);
          strcat(string,temp);
          IupSetAttribute(matriz, string, valor);
        }
        i++;
        c++;
        if(c == col+1) c=1 ;
      }
      free(temp);
      free(string);
      free(marked);
    }
    free(valor);
  }
}

int altura_cb(void)
{
  alterar_dimensao("HEIGHT") ;

  return IUP_DEFAULT;
}

int largura_cb(void)
{
  alterar_dimensao("WIDTH") ;

  return IUP_DEFAULT;
}

void alin(char* pos)
{
  if(IupGetAttribute(matriz,"MARKED") == NULL)
  {
    IupMessage("Alinhamento","Não há células marcadas");
  }
  else
  {
    int i = 0 , c = 1 ;
    char* marked;
    char* string;
    char* temp;
    marked = (char*)malloc(sizeof(char)*(lin*col)+1);
    strcpy(marked, IupGetAttribute(matriz,"MARKED"));
    string = (char*)malloc((sizeof(char)*strlen("ALIGNMENT")+dimensao_limite)+1);
    temp = (char*)malloc((sizeof(char)*dimensao_limite)+1);
        
    while(i < col*lin)
    {
      if(marked[i] == '1') 
      {
        sprintf(temp,"%d",c);
        strcpy(string,"ALIGNMENT");
        strcat(string,temp);
        IupSetAttribute(matriz,string,pos);
      }
     i++ ;
     c++ ;
     if(c == col+1) c=1 ;
    }
    free(temp);
    free(string);
    free(marked);
  }  
}

int alinesq_cb(void)
{
  alin("ALEFT") ;

  return IUP_DEFAULT ;
}

int alincent_cb(void)
{
  alin("ACENTER") ;

  return IUP_DEFAULT ;
}

int alindir_cb(void)
{
  alin("ARIGHT") ;

  return IUP_DEFAULT ;
}

int marcacao_continua_cb(void)
{
  if(strcmp(IupGetAttribute(matriz,"AREA") , "CONTINUOUS") == 0)
  {
    IupSetAttribute(IupGetHandle("marcacao_continua"),"VALUE","OFF");
    IupSetAttribute(matriz,"AREA","NOT_CONTINUOUS");
  }
  else
  {
    IupSetAttribute(IupGetHandle("marcacao_continua"),"VALUE","ON");
    IupSetAttribute(matriz,"AREA","CONTINUOUS");
  }

  return IUP_DEFAULT;
}

int marcacao_multipla_cb(void)
{
  if(strcmp(IupGetAttribute(matriz,"MULTIPLE"), "YES") == 0)
  {
    IupSetAttribute(matriz,"MULTIPLE","NO");
    IupSetAttribute(IupGetHandle("marcacao_multipla"),"VALUE","OFF");
  }
  else
  {
    IupSetAttribute(matriz,"MULTIPLE","YES");
    IupSetAttribute(IupGetHandle("marcacao_multipla"),"VALUE","ON");
  }

  return IUP_DEFAULT;
}

int tamanho_editavel_cb(void)
{
  if(strcmp(IupGetAttribute(matriz,IUP_RESIZEMATRIX), "YES") == 0)
  {
    IupSetAttribute(matriz,IUP_RESIZEMATRIX,"NO");
    IupSetAttribute(IupGetHandle("tamanho_editavel"),"VALUE","OFF");
  }
  else
  {
    IupSetAttribute(matriz,IUP_RESIZEMATRIX,"YES");
    IupSetAttribute(IupGetHandle("tamanho_editavel"),"VALUE","ON");
  }

  return IUP_DEFAULT;
}

void alterar_cor(char* modo,char* cor)
{
  if(IupGetAttribute(matriz,"MARKED") == NULL)
  {
    IupMessage("Mudança de cor","Não há células marcadas");
  }
  else
  {
    int i = 0 , c = 1 ;
    char* marked;
    char* string;
    char* temp;
    marked = (char*)malloc(sizeof(char)*(lin*col)+1);
    strcpy(marked, IupGetAttribute(matriz,"MARKED"));
    string = (char*)malloc((sizeof(char)*strlen(modo)+2*dimensao_limite+strlen(":"))+1);
    temp = (char*)malloc((sizeof(char)*dimensao_limite)+1);
        
    while(i < col*lin)
    {
      if(marked[i] == '1')
      {
        sprintf(temp,"%d",i/col+1);
        strcpy(string,modo);
        strcat(string,temp);
        strcat(string,":");
        sprintf(temp,"%d",c);
        strcat(string,temp);
        IupSetAttribute(matriz, string, cor);
      }
      i++;
      c++;
      if(c == col+1) c=1 ;
    }
    free(temp);
    free(string);
    free(marked);
  }
}

int cor_de_frente_vermelha_cb(void)
{
  alterar_cor("FGCOLOR","255 0 0") ;

  return IUP_DEFAULT;
}

int cor_de_frente_verde_cb(void)
{
  alterar_cor("FGCOLOR","0 255 0");

  return IUP_DEFAULT;
}

int cor_de_frente_azul_cb(void)
{
  alterar_cor("FGCOLOR","0 0 255") ;

  return IUP_DEFAULT;
}

int cor_de_frente_preta_cb(void)
{
  alterar_cor("FGCOLOR","0 0 0") ;

  return IUP_DEFAULT;
}

int cor_de_frente_branca_cb(void)
{
  alterar_cor("FGCOLOR","255 255 255") ;

  return IUP_DEFAULT;
}

int cor_de_fundo_vermelha_cb(void)
{
  alterar_cor("BGCOLOR","255 0 0") ;

  return IUP_DEFAULT;
}

int cor_de_fundo_verde_cb(void)
{
  alterar_cor("BGCOLOR","0 255 0");

  return IUP_DEFAULT;
}

int cor_de_fundo_azul_cb(void)
{
  alterar_cor("BGCOLOR","0 0 255") ;

  return IUP_DEFAULT;
}

int cor_de_fundo_preta_cb(void)
{
  alterar_cor("BGCOLOR","0 0 0") ;

  return IUP_DEFAULT;
}

int cor_de_fundo_branca_cb(void)
{
  alterar_cor("FGCOLOR","255 255 255") ;

  return IUP_DEFAULT;
}

/* main program */
int main(int argc, char **argv)
{
  char *error=NULL;

  /* IUP initialization */
  IupOpen(&argc, &argv);       

  /* initializes the matrix library; must be
   * called only once during the program */
  IupControlsOpen();  

  /* loads LED */
  if(error = IupLoad("iupmatrix.led"))
  {
    IupMessage("LED error", error);
    return 1 ;
  }

  dlg = IupGetHandle("dlg");
  matriz = IupGetHandle("matriz");
  moldura = IupGetHandle("moldura");

  /* registers callbacks */
  IupSetFunction("titulogeral_act", (Icallback) titulogeral_cb);
  IupSetFunction("titulocol_act", (Icallback) titulocol_cb);
  IupSetFunction("titulolin_act", (Icallback) titulolin_cb);

  IupSetFunction("adicionarcolesq_act", (Icallback) adicionarcolesq_cb);
  IupSetFunction("adicionarcoldir_act", (Icallback) adicionarcoldir_cb);
  IupSetFunction("adicionarlinacima_act", (Icallback) adicionarlinacima_cb);
  IupSetFunction("adicionarlinabaixo_act", (Icallback) adicionarlinabaixo_cb);

  IupSetFunction("removercolesq_act", (Icallback) removercolesq_cb);
  IupSetFunction("removercoldir_act", (Icallback) removercoldir_cb);
  IupSetFunction("removerlinacima_act", (Icallback) removerlinacima_cb);
  IupSetFunction("removerlinabaixo_act", (Icallback) removerlinabaixo_cb);

  IupSetFunction("altura_act", (Icallback) altura_cb);
  IupSetFunction("largura_act", (Icallback) largura_cb);

  IupSetFunction("alinesq_act", (Icallback) alinesq_cb);
  IupSetFunction("alincent_act", (Icallback) alincent_cb);
  IupSetFunction("alindir_act", (Icallback) alindir_cb);

  IupSetFunction("marcacao_continua_act", (Icallback) marcacao_continua_cb);
  IupSetFunction("marcacao_multipla_act", (Icallback) marcacao_multipla_cb);
  IupSetFunction("tamanho_editavel_act", (Icallback) tamanho_editavel_cb);

  IupSetFunction("cor_de_frente_vermelha_act", (Icallback) cor_de_frente_vermelha_cb);
  IupSetFunction("cor_de_frente_verde_act", (Icallback) cor_de_frente_verde_cb);
  IupSetFunction("cor_de_frente_azul_act", (Icallback) cor_de_frente_azul_cb);
  IupSetFunction("cor_de_frente_preta_act", (Icallback) cor_de_frente_preta_cb);
  IupSetFunction("cor_de_frente_branca_act", (Icallback) cor_de_frente_branca_cb);

  IupSetFunction("cor_de_fundo_vermelha_act", (Icallback) cor_de_fundo_vermelha_cb);
  IupSetFunction("cor_de_fundo_verde_act", (Icallback) cor_de_fundo_verde_cb);
  IupSetFunction("cor_de_fundo_azul_act", (Icallback) cor_de_fundo_azul_cb);
  IupSetFunction("cor_de_fundo_preta_act", (Icallback) cor_de_fundo_preta_cb);
  IupSetFunction("cor_de_fundo_branca_act", (Icallback) cor_de_fundo_branca_cb);

  IupSetFunction("titulo_ok_act", (Icallback) titulo_ok_cb);
  IupSetFunction("dimensao_ok_act", (Icallback) dimensao_ok_cb);

  IupSetFunction("cancel_act",(Icallback) cancel_cb);
  
  /* shows the dialog */
  IupShowXY(dlg,IUP_CENTER,IUP_CENTER);

  /* main loop */
  IupMainLoop();

  IupDestroy(dlg);

  /* ends IUP */
  IupClose();

  return 0 ;
}
