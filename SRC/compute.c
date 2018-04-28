#include <stdio.h>
#include <gtk/gtk.h>

/*******************************************************
IL EST FORMELLEMENT INTERDIT DE CHANGER LE PROTOTYPE
DES FONCTIONS
*******************************************************/

typedef struct { guchar radio; int group; } point_t, *points_t;

typedef struct { guchar g1, g2, g3, g4, g5; } pointVec_t, *pointVecs_t;

#define CLUSTER_NB  8

void report_error(char* msg, int code)
{
  fprintf(stderr, msg);
  exit(code);
}

void* safe_malloc(size_t size)
{
  void* res;
  if ((res = malloc(size)) == NULL)
    report_error("Malloc failed an allocation... Exiting", 1);
  return res;
}

void init_pointVector(pointVec_t pv, guchar g1, guchar g2, guchar g3, guchar g4, guchar g5)
{
    pv.g1 = g1;
    pv.g2 = g2;
    pv.g3 = g3;
    pv.g4 = g4;
    pv.g5 = g5;
}

pointVecs_t init_centers(points_t points, int len_points)
{
  pointVecs_t cluster_centers = safe_malloc(sizeof(pointVec_t) * CLUSTER_NB);

  for (int i = 0; i < CLUSTER_NB - 1; i++)
  {
    guchar cent_val = 255 * i / CLUSTER_NB;
    init_pointVector(cluster_centers[i], cent_val, cent_val, cent_val, cent_val, cent_val);
  }

  // Vmax, for the cloud cluster
  init_pointVector(cluster_centers[CLUSTER_NB - 1], 255, 255, 255, 255, 255);

  return cluster_centers;
}

pointVecs_t Lloyd(points_t points, int len_points)
{
  pointVecs_t cluster_centers = init_centers(points, len_points);

  return cluster_centers;
}

/*---------------------------------------
  Proto:


  But:

  Entrees:
  --->le tableau des valeurs des pixels de l'image d'origine
  (les lignes sont mises les unes à la suite des autres)
  --->le nombre de lignes de l'image,
  --->le nombre de colonnes de l'image,
  --->le tableau des valeurs des pixels de l'image resultat
  (les lignes sont mises les unes à la suite des autres)


  Sortie:

  Rem:

  Voir aussi:

  ---------------------------------------*/
void ComputeImage(guchar *pucImaOrig,
		  int NbLine,
		  int NbCol,
		  guchar *pucImaRes)
{
  int iNbPixelsTotal = NbCol * NbLine;
  int iNbChannels = 3; /* on travaille sur des images couleurs*/
  guchar ucMeanPix;

  printf("Segmentation de l'image.... A vous!\n");

  points_t points = safe_malloc(iNbPixelsTotal * sizeof(point_t));

  for(int iNumPix = 0; iNumPix < iNbPixelsTotal * iNbChannels; iNumPix = iNumPix + iNbChannels) {
    /*moyenne sur les composantes RVB */
    ucMeanPix = (unsigned char)
      ((*(pucImaOrig + iNumPix) +
        *(pucImaOrig + iNumPix + 1) +
        *(pucImaOrig + iNumPix + 2))/3);
    /* sauvegarde du resultat */
    for(int iNumChannel = 0; iNumChannel < iNbChannels; iNumChannel++)
      *(pucImaRes + iNumPix + iNumChannel)= ucMeanPix;

    points[iNumPix / 3].radio = ucMeanPix;
    points[iNumPix / 3].group = 0;
  }

  pointVecs_t cluster_centers = Lloyd(points, iNbPixelsTotal);
}
