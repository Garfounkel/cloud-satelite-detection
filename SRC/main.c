#include <stdio.h>
#include <stdlib.h>
#include <gtk/gtk.h>
#include <math.h>
#include "compute.h"

/*---------------------------------------
  Proto:
  void ShowImaOrig(GtkButton *button_ok,
  GtkWidget **ppwTabArgs)

  But: creer et afficher l'image originale dont le nom est
  contenu dans un FileSelection

  Entree: --->le button OK du FileSelection
  --->un tableau de Widgets avec :
  case 0: le GtkFileSelection contenant le nom du fichier image
  case 1: le GtkImage pour l'image originale et initialisee
  lors de l'appel de la fonction ShowImaOrig
  case 2 : la fenetre principale
  case 3: le tableau ou est dessine l'image originale
  case 4: le tableau ou est dessinee l'image resultat
  case 5: le GtkImage pour l'image originale et initialisee
  lors de l'appel de la fonction ShowImaComputed

  Sortie:

  Rem:

  Voir aussi:

  ---------------------------------------*/
void ShowImaOrig(GtkButton *button_ok,
		 GtkWidget **ppwTabArgs)
{
  GtkFileSelection *pfsOpenFile;
  gchar *pcFileName;
  GdkPixbuf* pGdkPixbufImaIn;

  /* recuperation du nom du fichier image */
  pfsOpenFile = GTK_FILE_SELECTION(*(ppwTabArgs));
  if (pfsOpenFile == NULL){
    printf("Mauvaise initialisation du GtkFileSection.\n");
    exit(0);
  }
  pcFileName = (gchar*) gtk_file_selection_get_filename(pfsOpenFile);

  /* chargement en memoire de l'image */
  pGdkPixbufImaIn = gdk_pixbuf_new_from_file (pcFileName,NULL);
  /* echec du chargement de l'image en memoire */
  if (pGdkPixbufImaIn == NULL){
    printf("Impossible de charger le fichier %s\n", pcFileName);
    exit(0);
  }
  /* reussite du chargement de l'image en memoire */
  else
  {
    /* on vide l'image precedente */
    gtk_image_clear(GTK_IMAGE(*(ppwTabArgs+1)));
    /* on cree la nouvelle GtkImage*/
    *(ppwTabArgs+1)= gtk_image_new_from_pixbuf(pGdkPixbufImaIn);
  }

  /* affichage de l'image */
  gtk_table_attach_defaults(GTK_TABLE(*(ppwTabArgs+3)),
			    *(ppwTabArgs+1),
 			    0,1,1,2);
  gtk_widget_show_all(*(ppwTabArgs+2));
}


/*---------------------------------------
  Proto:
  void ShowImaComputed(GtkObject *button_analyse,
  GtkWidget **ppwTabArgs)


  But: calculer et afficher l'image resultat

  Entrees: --->le button lancant le calcul
  --->un tableau de Widgets avec :
  case 0: le GtkFileSelection contenant le nom du fichier image
  case 1: le GtkImage pour l'image originale et initialisee
  lors de l'appel de la fonction ShowImaOrig
  case 2 : la fenetre principale
  case 3: le tableau ou est dessine l'image originale
  case 4: le tableau ou est dessinee l'image resultat
  case 5: le GtkImage pour l'image originale et initialisee
  lors de l'appel de la fonction ShowImaComputed

  Sortie:

  Rem:

  Voir aussi: la fonction ComputeImage dans compute.c

  ---------------------------------------*/
void ShowImaComputed(GtkObject *button_analyse,
		     GtkWidget **ppwTabArgs)
{
  int NbCol, NbLine;
  guchar *pucImaOrig, *pucImaRes;
  GdkPixbuf *pGdkPixbufImaOrig,*pGdkPixbufImaRes;

  /* recuperation du pixbuf original */
  pGdkPixbufImaOrig=gtk_image_get_pixbuf(GTK_IMAGE(*(ppwTabArgs+1)));
  if (pGdkPixbufImaOrig == NULL){
    printf("Pas de fichier image originale.\n");
    exit(0);
  }
  /* copie du pixbuff original */
  pGdkPixbufImaRes=gdk_pixbuf_copy(pGdkPixbufImaOrig);

  /* recuperation du nombre de lignes et de colonnes de l'image*/
  NbCol=gdk_pixbuf_get_width(pGdkPixbufImaOrig);
  NbLine=gdk_pixbuf_get_height(pGdkPixbufImaOrig);
  /* recuperation du tableau des pixels de l'image originale */
  pucImaOrig=gdk_pixbuf_get_pixels(pGdkPixbufImaOrig);
  /* recuperation du tableau des pixels de l'image resultat */
  pucImaRes=gdk_pixbuf_get_pixels(pGdkPixbufImaRes);

  /* on realise le calcul sur l'image */
  ComputeImage(pucImaOrig,NbLine,NbCol,pucImaRes);

  /* on vide l'image resultat precedente */
  gtk_image_clear(GTK_IMAGE(*(ppwTabArgs+5)));
  /* creation de la nouvelle image resultat */
  *(ppwTabArgs+5)= gtk_image_new_from_pixbuf(pGdkPixbufImaRes);
  gtk_table_attach_defaults(GTK_TABLE(*(ppwTabArgs+4)),
                            *(ppwTabArgs+5),
                            0,1,1,2);
  gtk_widget_show_all(*(ppwTabArgs+2));

}

/************************************************
fonction principale
************************************************/
int main (int argc, char **argv)
{

  /*---------------------------------------------
    Declarations
    ---------------------------------------------*/

  /*--------------------*/
  /*la fenetre principale de plus haut niveau*/
  /*--------------------*/
  GtkWidget *pwMainWind;
  /*boite principale contenant tous les objets de
    la fenetre principale*/
  GtkWidget *pwMainBox;

  /*--------------------*/
  /*la sous-fenetre d'affichage de l'image d'origine*/
  /*--------------------*/
  GtkWidget *pwBoxOrig;
  /*decoration fenetre d'affichage de l'image*/
  GtkWidget *pwDecoOrig;
  /*tableau pour l'organisation de la sous-fenetre
    d'affichage de l'image originale*/
  GtkWidget *pwTabShowOrig;
  /*barre de boutons ouverture image*/
  GtkWidget *pwButtonBoxOpen;
  /*bouton ouverture image*/
  GtkWidget *pwButtonOpen;
  /*bouton quitter*/
  GtkWidget *pwButtonQuit;
  /*fenetre de selection des fichiers*/
  GtkWidget *pwOpenFileSelection;
  /*le tableau des GtkWidget permettant le dessin de l'image origine et
    de l'image segmentee
    case 0: le GtkFileSelection contenant le nom du fichier image
    case 1: le GtkImage pour l'image originale et initialisee
    lors de l'appel de la fonction ShowImaOrig
    case 2 : la fenetre principale
    case 3: le tableau ou est dessine l'image originale
    case 4: le tableau ou est dessinee l'image resultat
    case 5: le GtkImage pour l'image originale et initialisee
    lors de l'appel de la fonction ShowImaComputed*/
  GtkWidget *ppwTabArgs[6];
  /*GtkImage de l'image d'origine*/
  GtkWidget *pwImaOrig;

  /*--------------------*/
  /*la sous-fenetre d'affichage de l'image resultat*/
  /*--------------------*/
  GtkWidget *pwBoxRes;
  /*decoration fenetre d'affichage de l'image resultat*/
  GtkWidget *pwDecoRes;
  /*tableau pour l'organisation de la sous-fenetre
    d'affichage de l'image resultat*/
  GtkWidget *pwTabShowRes;
  /*barre de boutons fenetre de l'image resultat*/
  GtkWidget *pwButtonBoxRes;
  /*bouton calcul*/
  GtkWidget *pwButtonAnalyse;
  /*GtkImage de l'image resultat*/
  GtkWidget *pwImaRes;

  /* la taille par defaut des images */
  int NbCol=LargeurImage;

  /*---------------------------------------------
    actions
    ---------------------------------------------*/

  /*initialisation des appels des bibliotheques*/
  gtk_init(&argc,&argv);

  /*--------------
    fenetre principale
    --------------*/
  pwMainWind=gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_container_set_border_width(GTK_CONTAINER(pwMainWind),5);
  gtk_window_set_title(GTK_WINDOW(pwMainWind),
		       "ImaProjet");
  gtk_signal_connect(GTK_OBJECT(pwMainWind),"destroy_event",
		     (GtkSignalFunc) gtk_exit,NULL);
  gtk_signal_connect(GTK_OBJECT(pwMainWind),"delete_event",
		     (GtkSignalFunc) gtk_exit,NULL);
  pwMainBox=gtk_hbox_new(FALSE,0);
  gtk_container_add(GTK_CONTAINER(pwMainWind),pwMainBox);


  /*--------------
    fenetre d'affichage de l'image originale
    --------------*/
  /*la boite d'affichage de l'image originale*/
  pwBoxOrig=gtk_hbox_new(FALSE,5);
  gtk_box_pack_start_defaults(GTK_BOX(pwMainBox),pwBoxOrig);
  pwDecoOrig=gtk_frame_new("image originale");
  gtk_box_pack_start_defaults(GTK_BOX(pwBoxOrig),pwDecoOrig);
  pwTabShowOrig=gtk_table_new(2,1,FALSE);
  gtk_container_set_border_width(GTK_CONTAINER(pwTabShowOrig),5);
  gtk_container_add(GTK_CONTAINER(pwDecoOrig),pwTabShowOrig);

  /*la zone d'affichage de l'image originale*/
  pwImaOrig=gtk_image_new();
  gtk_widget_set_usize(pwImaOrig,NbCol,NbCol);
  gtk_table_attach_defaults(GTK_TABLE(pwTabShowOrig), pwImaOrig,
 			    0,1,1,2);

  /*la boite contenant les boutons ouverture*/
  pwButtonBoxOpen=gtk_hbutton_box_new();
  gtk_table_attach_defaults(GTK_TABLE(pwTabShowOrig), pwButtonBoxOpen,
			    0,1,0,1);
  /*le bouton ouverture*/
  pwButtonOpen=gtk_button_new_with_label("ouverture");
  gtk_container_add(GTK_CONTAINER(pwButtonBoxOpen),pwButtonOpen);

  /*le bouton quitter*/
  pwButtonQuit=gtk_button_new_with_label("quitter");
  gtk_signal_connect(GTK_OBJECT(pwButtonQuit),"clicked",
		     (GtkSignalFunc) gtk_exit,NULL);
  gtk_container_add(GTK_CONTAINER(pwButtonBoxOpen),pwButtonQuit);


  /*la fenetre de choix de l'image*/
  pwOpenFileSelection=gtk_file_selection_new("Choix de l'image");
  gtk_signal_connect(GTK_OBJECT(pwOpenFileSelection), "delete_event",
		     GTK_SIGNAL_FUNC(gtk_widget_hide),
		     pwOpenFileSelection);
  gtk_signal_connect(GTK_OBJECT(pwOpenFileSelection), "destroy_event",
		     GTK_SIGNAL_FUNC(gtk_widget_hide),
		     pwOpenFileSelection);
  gtk_signal_connect_object(GTK_OBJECT(pwButtonOpen),
			    "clicked",
			    GTK_SIGNAL_FUNC(gtk_widget_show),
			    GTK_OBJECT(pwOpenFileSelection));
  gtk_signal_connect_object(GTK_OBJECT(GTK_FILE_SELECTION(pwOpenFileSelection)
				       ->cancel_button),
			    "clicked",
			    GTK_SIGNAL_FUNC(gtk_widget_hide),
			    GTK_OBJECT(pwOpenFileSelection));
  /*les arguments pour le dessin de l'image*/
  /*case 0: le GtkFileSelection contenant le nom du fichier image*/
  ppwTabArgs[0]=pwOpenFileSelection;
  /*case 1: le GtkImage pour l'image originale et initialisee
    lors de l'appel de la fonction ShowImaOrig*/
  ppwTabArgs[1]=pwImaOrig;
  /*case 2 : la fenetre principale*/
  ppwTabArgs[2]=pwMainWind;
  /*case 3: le tableau ou est dessine l'image originale*/
  ppwTabArgs[3]=pwTabShowOrig;
  /*connexion avec le signal permettant de lancer la procedure de chargement
    et d'affichage de l'image dans la scrolled window*/
  gtk_signal_connect(GTK_OBJECT(GTK_FILE_SELECTION(pwOpenFileSelection)
				->ok_button),
		     "clicked",
		     GTK_SIGNAL_FUNC(ShowImaOrig),
		     (gpointer)ppwTabArgs);
  gtk_signal_connect_object(GTK_OBJECT(GTK_FILE_SELECTION(pwOpenFileSelection)
				       ->ok_button),
			    "clicked",
			    GTK_SIGNAL_FUNC(gtk_widget_hide),
			    GTK_OBJECT(pwOpenFileSelection));

  /*--------------
    fenetre d'affichage de l'image resultat
    --------------*/
  /*la boite d'affichage de l'image resultat*/
  pwBoxRes=gtk_hbox_new(FALSE,5);
  gtk_box_pack_start_defaults(GTK_BOX(pwMainBox),pwBoxRes);
  pwDecoRes=gtk_frame_new("image resultat");
  gtk_box_pack_start_defaults(GTK_BOX(pwBoxRes),pwDecoRes);
  pwTabShowRes=gtk_table_new(2,1,FALSE);
  gtk_container_set_border_width(GTK_CONTAINER(pwTabShowRes),5);
  gtk_container_add(GTK_CONTAINER(pwDecoRes),pwTabShowRes);
  /*case 4: le tableau ou est dessinee l'image resultat*/
  ppwTabArgs[4]=pwTabShowRes;

  /*la zone de dessin de l'image resultat*/
  pwImaRes=gtk_image_new();
  gtk_widget_set_usize(pwImaRes,NbCol,NbCol);
  gtk_table_attach_defaults(GTK_TABLE(pwTabShowRes), pwImaRes,
                            0,1,1,2);
  /*case 5: le GtkImage resultat*/
  ppwTabArgs[5]=pwImaRes;

  /*la boite contenant le bouton quitter*/
  pwButtonBoxRes=gtk_hbutton_box_new();
  gtk_table_attach_defaults(GTK_TABLE(pwTabShowRes), pwButtonBoxRes,
			    0,1,0,1);

  /*bouton pour le lancement des calculs de l'analyse*/
  pwButtonAnalyse=gtk_button_new_with_label("Analyse");
  gtk_container_add(GTK_CONTAINER(pwButtonBoxRes),pwButtonAnalyse);
  gtk_signal_connect(GTK_OBJECT(pwButtonAnalyse),
                     "clicked",
                     GTK_SIGNAL_FUNC(ShowImaComputed),
                     ppwTabArgs);

  /*--------------
    boucle principale
    ----------------*/
  gtk_widget_show_all(pwMainWind);
  gtk_main();
  return(0);

}
