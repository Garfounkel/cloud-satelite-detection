#include <stdio.h>
#include <stdlib.h>
#include <gtk/gtk.h>
#include <math.h>
#include "compute.h"
#include <dirent.h>

int main (int argc, char **argv)
{
  DIR *dir;
  struct dirent *ent;
  if ((dir = opendir (".")) != NULL)
  {
    while ((ent = readdir (dir)) != NULL)
    {
      printf ("%s\n", ent->d_name);
    }
    closedir (dir);
  }
  else
  {
    /* could not open directory */
    perror ("");
    return EXIT_FAILURE;
  }

  /*
  GdkPixbuf* pGdkPixbufImaIn = gdk_pixbuf_new_from_file (pcFileName,NULL);
  // echec du chargement de l'image en memoire
  if (pGdkPixbufImaIn == NULL){
    printf("Impossible de charger le fichier %s\n", pcFileName);
    exit(0);
  }
  */
}
