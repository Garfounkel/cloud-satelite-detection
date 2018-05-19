#include "compute.h"
#include <dirent.h>
#include <gtk/gtk.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

double compute_image(char *path) {
  GdkPixbuf *pGdkPixbufImaOrig = gdk_pixbuf_new_from_file(path, NULL);
  // echec du chargement de l'image en memoire
  if (pGdkPixbufImaOrig == NULL) {
    printf("Impossible de charger le fichier %s\n", path);
    exit(0);
  }

  /* copie du pixbuff original */
  GdkPixbuf *pGdkPixbufImaRes = gdk_pixbuf_copy(pGdkPixbufImaOrig);

  /* recuperation du nombre de lignes et de colonnes de l'image*/
  int NbCol = gdk_pixbuf_get_width(pGdkPixbufImaOrig);
  int NbLine = gdk_pixbuf_get_height(pGdkPixbufImaOrig);

  /* recuperation du tableau des pixels de l'image originale */
  guchar *pucImaOrig = gdk_pixbuf_get_pixels(pGdkPixbufImaOrig);
  /* recuperation du tableau des pixels de l'image resultat */
  guchar *pucImaRes = gdk_pixbuf_get_pixels(pGdkPixbufImaRes);

  return ComputeImageCloudRatio(pucImaOrig, NbLine, NbCol, pucImaRes);
}

int main(int argc, char **argv) {
  DIR *dir;
  struct dirent *ent;
  if ((dir = opendir(".")) != NULL) {
    while ((ent = readdir(dir)) != NULL) {
      /* Only bmp files */
      if (strncmp(ent->d_name + strlen(ent->d_name) - 4, ".bmp", 4) == 0) {
        printf("%s: %f%%\n", ent->d_name, compute_image(ent->d_name));
      }
    }
    closedir(dir);
  } else {
    /* could not open directory */
    perror("");
    return EXIT_FAILURE;
  }
}
