#include <stdio.h>
#include <gtk/gtk.h>
#include <math.h>
#include <stdlib.h>

/*******************************************************
IL EST FORMELLEMENT INTERDIT DE CHANGER LE PROTOTYPE
DES FONCTIONS
*******************************************************/

typedef struct { guchar radio; int group; } point_t, *points_t;

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

void init_pointVector(guchar* pv, guchar g1, guchar g2, guchar g3, guchar g4, guchar g5)
{
  pv[0] = g1;
  pv[1] = g2;
  pv[2] = g3;
  pv[3] = g4;
  pv[4] = g5;
}

void init_centers(points_t points, int len_points, guchar** cluster_centers)
{
  for (int i = 0; i < CLUSTER_NB - 1; i++)
  {
    guchar cent_val = 255 * i / CLUSTER_NB;
    cluster_centers[i] = safe_malloc(sizeof(guchar) * 5);
    init_pointVector(cluster_centers[i], cent_val, cent_val, cent_val, cent_val, cent_val);
  }

  // Vmax, for the cloud cluster
  cluster_centers[CLUSTER_NB - 1] = safe_malloc(sizeof(guchar) * 5);
  init_pointVector(cluster_centers[CLUSTER_NB - 1], 255, 255, 255, 255, 255);
}

void sort_components(guchar* components)
{
  int n = 4;
  bool swapped = false;
  do {
    swapped = false;
    for (int i = 1; i < n; i++)
    {
      if (components[i - 1] > components[i])
      {
        guchar temp = components[i - 1];
        components[i - 1] = components[i];
        components[i] = temp;
        swapped = true;
      }
      n = n - 1
    }
  } while(!swapped);
}

void find_components(guchar* components, points_t points, int len_points, int pos, int size_line)
{
  int i = pos / size_line;
  int j = pos % size_line;

  init_pointVector(components, points[pos].radio, 0, 0, 0, 0);
  if (i - 1 >= 0)
    components[1] = points[(i - 1) * size_line + j].radio;
  if (i + 1 < size_line)
    components[2] = points[(i + 1) * size_line + j].radio;
  if (j - 1 >= 0)
    components[3] = points[i * size_line + (j - 1)].radio;
  if (j + 1 >= 0)
    components[4] = points[i * size_line + (j + 1)].radio;

  sort_components(components);
}

pointVecs_t Lloyd(points_t points, int len_points, int size_line)
{
  guchar** cluster_centers = safe_malloc(sizeof(void*) * CLUSTER_NB);
  init_centers(points, len_points, cluster_centers);

  // guchar* components = safe_malloc(sizeof(guchar) * 5);

  return cluster_centers;
}

int findClosest(pointVec_t pixelVec, pointVecs_t cluster_centers)
{ /* euclidean distance between a vector of pixels and the clusters centers */
  int min = INT_MAX;
  int dist = 0;
  for (int i = 0; i < CLUSTER_NB; ++i)
  {
    pointVec_t center = cluster_centers[i];

    dist = pow((pixelVec.g1 - center.g1), 2)
      + pow((pixelVec.g2 - center.g2), 2)
      + pow((pixelVec.g3 - center.g3), 2)
      + pow((pixelVec.g4 - center.g4), 2)
      + pow((pixelVec.g5 - center.g5), 2);

    dist = sqrt(dist);

    if (dist < min)
      min = dist;
  }

  return min;
}

/*---------------------------------------
  Proto:


  But:

  Entrees:
  --->le tableau des valeurs des pixels de l'image d'origine
  (les lignes sont mises les unes � la suite des autres)
  --->le nombre de lignes de l'image,
  --->le nombre de colonnes de l'image,
  --->le tableau des valeurs des pixels de l'image resultat
  (les lignes sont mises les unes � la suite des autres)


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
    /* moyenne sur les composantes RVB */
    ucMeanPix = (unsigned char)
      ((*(pucImaOrig + iNumPix) +
        *(pucImaOrig + iNumPix + 1) +
        *(pucImaOrig + iNumPix + 2))/3);
    /* sauvegarde du resultat */
    for(int iNumChannel = 0; iNumChannel < iNbChannels; iNumChannel++)
      *(pucImaRes + iNumPix + iNumChannel) = ucMeanPix;

    points[iNumPix / 3].radio = ucMeanPix;
    points[iNumPix / 3].group = 0;
  }

  pointVecs_t cluster_centers = Lloyd(points, iNbPixelsTotal, NbCol);
}
