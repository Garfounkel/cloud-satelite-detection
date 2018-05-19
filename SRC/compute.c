#include <gtk/gtk.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

/*******************************************************
IL EST FORMELLEMENT INTERDIT DE CHANGER LE PROTOTYPE
DES FONCTIONS
*******************************************************/

typedef struct {
  guchar radio;
  guchar *components;
  int group;
} point_t, *points_t;

#define CLUSTER_NB 8

void report_error(char *msg, int code) {
  fprintf(stderr, msg);
  exit(code);
}

void *safe_malloc(size_t size) {
  void *res;
  if ((res = malloc(size)) == NULL)
    report_error("Malloc failed an allocation... Exiting", 1);
  return res;
}

void init_pointVector(guchar *pv, guchar g1, guchar g2, guchar g3, guchar g4,
                      guchar g5) {
  pv[0] = g1;
  pv[1] = g2;
  pv[2] = g3;
  pv[3] = g4;
  pv[4] = g5;
}

void init_centers(points_t points, int len_points, guchar **cluster_centers) {
  for (int i = 1; i < CLUSTER_NB; i++) {
    guchar cent_val = 255 * i / CLUSTER_NB;
    cluster_centers[i - 1] = safe_malloc(sizeof(guchar) * 5);
    init_pointVector(cluster_centers[i - 1], cent_val, cent_val, cent_val,
                     cent_val, cent_val);
  }

  // Vmax, for the cloud cluster
  cluster_centers[CLUSTER_NB - 1] = safe_malloc(sizeof(guchar) * 5);
  init_pointVector(cluster_centers[CLUSTER_NB - 1], 255, 255, 255, 255, 255);
}

static int cmp_guchar(const void *p1, const void *p2) {
  return (*(guchar *)p1 > *(guchar *)p2);
}

void set_components(points_t points, int pos, int size_line, int size_col) {
  int i = pos / size_line;
  int j = pos % size_line;

  points[pos].components = safe_malloc(sizeof(guchar) * 5);
  init_pointVector(points[pos].components, points[pos].radio, points[pos].radio,
                   points[pos].radio, points[pos].radio, points[pos].radio);
  if (i - 1 >= 0)
    points[pos].components[1] = points[(i - 1) * size_line + j].radio;
  if (i + 1 < size_line)
    points[pos].components[2] = points[(i + 1) * size_line + j].radio;
  if (j - 1 >= 0)
    points[pos].components[3] = points[i * size_line + (j - 1)].radio;
  if (j + 1 < size_col)
    points[pos].components[4] = points[i * size_line + (j + 1)].radio;

  qsort(points[pos].components, 5, sizeof(guchar), cmp_guchar);
}

/* euclidean distance between a vector of pixels and the clusters centers */
int find_closest(guchar *pixelVec, guchar **cluster_centers) {
  int min = INT_MAX;
  int dist = 0;
  int index = 0;
  int changedAtLeastOnce = 0;
  int notChanged = 0;
  for (int i = 0; i < CLUSTER_NB; ++i) {
    guchar *center = cluster_centers[i];

    dist =
        pow((pixelVec[0] - center[0]), 2) + pow((pixelVec[1] - center[1]), 2) +
        pow((pixelVec[2] - center[2]), 2) + pow((pixelVec[3] - center[3]), 2) +
        pow((pixelVec[4] - center[4]), 2);

    // No need for square root in our distance since sqrt(x) < sqrt(y) is
    // the same as x < y
    // dist = sqrt(dist);

    if (dist < min) {
      changedAtLeastOnce = 1;
      min = dist;
      index = i;
      notChanged = 0;
    } else if (changedAtLeastOnce && notChanged >= 3) {
      /* Optimization: since our clusters are sorted and uniformally distributed
         we know that we found the closest center to the current point when the
         distance hasn't changed in a few loops (because from there the distance
         can only grow) */
      break;
    } else {
      notChanged++;
    }
  }

  return index;
}

void Lloyd(points_t points, int len_points, int size_line,
           guchar **cluster_centers) {
  init_centers(points, len_points, cluster_centers);

  int changed;
  int nb_iteration = 0;
  do {
    changed = 0;
    unsigned int new_centers_radio[CLUSTER_NB][5] = {0};
    unsigned int new_centers_nb[CLUSTER_NB] = {0};
    for (int i = 0; i < len_points; i++) {
      int closest_center = find_closest(points[i].components, cluster_centers);
      if (closest_center != points[i].group) {
        points[i].group = closest_center;
        changed++;
      }
      for (int j = 0; j < 5; j++) {
        new_centers_radio[points[i].group][j] += points[i].components[j];
      }
      new_centers_nb[points[i].group] += 1;
    }

    for (int i = 0; i < CLUSTER_NB; i++) {
      guchar new_center[5] = {0};
      for (int j = 0; j < 5; j++) {
        if (new_centers_nb[i] > 0)
          new_center[j] = (guchar)((float)new_centers_radio[i][j] /
                                   (float)new_centers_nb[i]);
        else
          new_center[j] = cluster_centers[i][j];
        if (i == CLUSTER_NB - 1 && new_center[j] < 125)
          new_center[j] = cluster_centers[i][j];
      }
      init_pointVector(cluster_centers[i], new_center[0], new_center[1],
                       new_center[2], new_center[3], new_center[4]);
    }
    nb_iteration++;
  } while (changed > (len_points >> 8));
  printf("[%d iterations] ", nb_iteration);
}

void ComputeKmeans(int NbCol, int iNbPixelsTotal, points_t points,
                   guchar *pucImaRes, guchar *pucImaOrig) {
  int iNbChannels = 3; /* on travaille sur des images couleurs*/

  /* Segmentation de l'image (niveaux de gris) */
  for (int iNumPix = 0; iNumPix < iNbPixelsTotal * iNbChannels;
       iNumPix = iNumPix + iNbChannels) {
    /* moyenne sur les composantes RVB */
    guchar ucMeanPix =
        (unsigned char)((*(pucImaOrig + iNumPix) + *(pucImaOrig + iNumPix + 1) +
                         *(pucImaOrig + iNumPix + 2)) /
                        3);
    /* sauvegarde du resultat */
    for (int iNumChannel = 0; iNumChannel < iNbChannels; iNumChannel++)
      *(pucImaRes + iNumPix + iNumChannel) = ucMeanPix;

    points[iNumPix / 3].radio = ucMeanPix;
    points[iNumPix / 3].group = 0;
  }

  /* Linking each points to its neighbours */
  for (int i = 0; i < iNbPixelsTotal; i++) {
    set_components(points, i, NbCol, iNbPixelsTotal / NbCol);
  }

  guchar **cluster_centers = safe_malloc(sizeof(void *) * CLUSTER_NB);
  /* k-means (Lloyd's version) */
  Lloyd(points, iNbPixelsTotal, NbCol, cluster_centers);

  for (int i = 0; i < CLUSTER_NB; i++) {
    free(cluster_centers[i]);
  }
  free(cluster_centers);
}

// Used in main_auto:
double ComputeImageCloudRatio(guchar *pucImaOrig, int NbLine, int NbCol,
                              guchar *pucImaRes) {
  int iNbPixelsTotal = NbCol * NbLine;

  points_t points = safe_malloc(iNbPixelsTotal * sizeof(point_t));

  ComputeKmeans(NbCol, iNbPixelsTotal, points, pucImaRes, pucImaOrig);

  int cloudPixels = 0;
  for (int i = 0; i < iNbPixelsTotal; i++) {
    if (points[i].group >= CLUSTER_NB - 1) {
      cloudPixels++;
    }
  }

  for (int i = 0; i < NbLine * NbCol; i++) {
    free(points[i].components);
  }
  free(points);

  double cloudRatio = ((double)cloudPixels / iNbPixelsTotal) * 100;
  return cloudRatio;
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
void ComputeImage(guchar *pucImaOrig, int NbLine, int NbCol,
                  guchar *pucImaRes) {
  int iNbPixelsTotal = NbCol * NbLine;
  int iNbChannels = 3; /* on travaille sur des images couleurs*/

  points_t points = safe_malloc(iNbPixelsTotal * sizeof(point_t));

  ComputeKmeans(NbCol, iNbPixelsTotal, points, pucImaRes, pucImaOrig);

  /* draw red pixels on clouds */
  for (int i = 0; i < iNbPixelsTotal; i++) {
    if (points[i].group >= CLUSTER_NB - 1) {
      *(pucImaRes + (i * iNbChannels)) = 255;
      *(pucImaRes + (i * iNbChannels) + 1) = 0;
      *(pucImaRes + (i * iNbChannels) + 2) = 0;
    }
  }

  free(points);
}
