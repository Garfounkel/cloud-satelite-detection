#include <stdio.h>
#include <gtk/gtk.h>
#include <math.h>
#include <stdlib.h>
#include <stdbool.h>

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
  for (int i = 1; i < CLUSTER_NB; i++)
  {
    guchar cent_val = 255 * i / CLUSTER_NB;
    cluster_centers[i - 1] = safe_malloc(sizeof(guchar) * 5);
    init_pointVector(cluster_centers[i - 1], cent_val, cent_val, cent_val, cent_val, cent_val);
  }

  // Vmax, for the cloud cluster
  cluster_centers[CLUSTER_NB - 1] = safe_malloc(sizeof(guchar) * 5);
  init_pointVector(cluster_centers[CLUSTER_NB - 1], 255, 255, 255, 255, 255);
}

static int cmp_guchar(const void *p1, const void *p2)
{
   /* The actual arguments to this function are "pointers to
      pointers to char", but strcmp(3) arguments are "pointers
      to char", hence the following cast plus dereference */

     return (* (char * const *) p1 >  * (char * const *) p2);
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

  qsort(components, 5, sizeof(guchar), cmp_guchar);
}

int findClosest(guchar* pixelVec, guchar** cluster_centers)
{ /* euclidean distance between a vector of pixels and the clusters centers */
  int min = INT_MAX;
  int dist = 0;
  int index = 0;
  for (int i = 0; i < CLUSTER_NB; ++i)
  {
    guchar* center = cluster_centers[i];

    dist = pow((pixelVec[0] - center[0]), 2)
           + pow((pixelVec[1] - center[1]), 2)
           + pow((pixelVec[2] - center[2]), 2)
           + pow((pixelVec[3] - center[3]), 2)
           + pow((pixelVec[4] - center[4]), 2);

    dist = sqrt(dist);

    if (dist < min)
    {
      min = dist;
      index = i;
    }
  }

  return index;
}

void Lloyd(points_t points, int len_points, int size_line, guchar** cluster_centers)
{
  init_centers(points, len_points, cluster_centers);

  int changed;
  int nb_iteration = 0;
  do {
    changed = 0;
    for (int i = 0; i < len_points; i++)
    {
      guchar* components = safe_malloc(sizeof(guchar) * 5);
      find_components(components, points, len_points, i, size_line);

      int closest_center = findClosest(components, cluster_centers);
      if (closest_center != points[i].group) {
        points[i].group = closest_center;
        changed++;
      }
    }

    unsigned int new_centers_radio[CLUSTER_NB] = { 0 };
    unsigned int new_centers_nb[CLUSTER_NB] = { 0 };
    for (int i = 0; i < len_points; i++)
    { // Maybe try with mean of components instead of radios
      new_centers_radio[points[i].group] += points[i].radio;
      new_centers_nb[points[i].group] += 1;
    }

    printf("------- iteration: %d -------\n", nb_iteration);
    for (int i = 0; i < CLUSTER_NB; i++)
    {
      printf("%u, ", cluster_centers[i][0]);
      guchar new_center = (guchar)((float)new_centers_radio[i] / (float)new_centers_nb[i]);
      printf("%u / %u = %u\n", new_centers_radio[i], new_centers_nb[i], new_center);
      init_pointVector(cluster_centers[i], new_center, new_center, new_center, new_center, new_center);
    }
    printf("\n");
    nb_iteration++;
  } while(changed > (len_points >> 10));
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

  guchar** cluster_centers = safe_malloc(sizeof(void*) * CLUSTER_NB);
  Lloyd(points, iNbPixelsTotal, NbCol, cluster_centers);


  for (int i = 0; i < iNbPixelsTotal; i++)
  {
    //printf("pt(i = %d, radio = %u, group = %u)\n", i, points[i].radio, points[i].group);
    if (points[i].group > CLUSTER_NB - 1)
    {
      //printf("pt(i = %d, radio = %u, group = %u)\n", i, points[i].radio, points[i].group);
      *(pucImaRes + (i * iNbChannels)) = 255;
      *(pucImaRes + (i * iNbChannels) + 1) = 0;
      *(pucImaRes + (i * iNbChannels) + 2) = 0;
    }
  }
}
