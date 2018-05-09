
#include <gtk/gtk.h>

#define LargeurImage 360

void ComputeImage(guchar *pucImaOrig,
		  guint uiNbCol,
		  guint uiNbLine,
		  guchar *pucImaRes);

double ComputeImageCloudRatio(guchar *pucImaOrig,
          		                int NbLine,
          		                int NbCol,
          		                guchar *pucImaRes);
