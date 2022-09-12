Various improvements were made to our k-means algorithm in order to
improve performances.

These optimizations are:
  - Merged two mains loops into one. There is no use to run accros all points
    two times each iterations since we can record points per cluster right after
    we computed the closest center to a point. (decent speedup of about 5%)

  - When searching for the nearest center to a point, we found that we don't
    necessarly need to compute the distance from every center to this point.
    Indeed, because our center list is sorted, we know that every subsequent
    computation of our nearest center should improve the last distance until
    the nearest one has been found. After that, subsequent distances calculation
    wont improve the current value since the best value has already been found.
    This means that if we notice that the distance hasn't been improve two times
    in a row, we can be sure that we already found the nearest center, therefore
    there no need to compute the distance to the 8 center for every point on every
    iteration of the main loop.
    This was a good performance gain since the nearest center calculation is
    the function that takes most of the time in our program. (Speedup of about
    10%)

  - Removed the square root computation when calculating the distance between
    two vectors. Indeed, in a regular euclidean distance, we would be doing a
    square root of the sum of diferences between each component of our vectors.
    But in our case, since we only need the distance in order to compute the
    nearest center, and since `sqrt(x) < sqrt(y)` is the same as `x < y`,
    there's no need for sqrt calculation. (small speedup)

Also, there is a file named results.md which contains significants statistics we
had on interresting iterations of our code. It's worth taking a look at it.
