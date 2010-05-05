/*
 * This is the yorick plugin for Healpix.
 *
 * Copyright(c) 2009, Guilhem Lavaux.
 * All rights reserved.
 *
 * http://www.iap.fr/users/lavaux/yheal.php
 * 
 * Read the accompanying LICENSE file for details.
 */

plug_in,"yheal";

HEALPIX_FLOAT = 0;
HEALPIX_DOUBLE = 1;

extern healpix_map_init;
/* DOCUMENT healpix_map_init(Nside, Type)
     returns a newly built healpix map object, with the given Nside
     and with an internal representation Type (either HEALPIX_FLOAT or
     HEALPIX_DOUBLE).

   SEE ALSO: healpix_map_load, healpix_map_get_pixel, healpix_map_get_map,
             healpix_map_get_direction, healpix_map_put_direction
 */

extern healpix_map_load;
/* DOCUMENT healpix_map_load(filename, Type)
    returns a newly built healpix map object with data obtained from
    the given FITS file, with an internal type representation given by
    Type (either HEALPIX_FLOAT or HEALPIX_DOUBLE).
   
   SEE ALSO: healpix_map_init, healpix_map_get_pixel, healpix_map_get_map,
             healpix_map_get_direction, healpix_map_put_direction
 */

extern healpix_map_save;
/* DOCUMENT healpix_map_save(filename, type, map)
    stores the given map into the file given by filename. If the filename is
    not prefixed by '!' the file will not be overwritten and an error
    will be thrown. Type indicates the storage format (either single or double
    precision).

  SEE ALSO: healpix_map_init, healpix_map_load
*/

extern healpix_map_get_pixel;
/* DOCUMENT healpix_map_get_pixel(map, pixel)
     returns the value of the given pixel of a healpix map object.
     Only scalar value of "pixel" are supported at the moment.
     
   SEE ALSO: healpix_map_init,healpix_map_load, healpix_map_get_map,
             healpix_map_get_direction, healpix_map_put_direction
 */

extern healpix_map_get_map;
/* DOCUMENT healpix_map_get_map(map)
     returns an array representing the total map. This array is a copy
     of the data contained in the healpix object can be modified safely.
     
   SEE ALSO: healpix_map_init, healpix_map_load, healpix_map_get_pixel,
             healpix_map_get_direction, healpix_map_put_direction
 */

extern healpix_map_put_map;

extern healpix_map_get_direction_internal;

func healpix_map_get_direction(map, latitudes, longitudes, interp=)
{
/* DOCUMENT healpix_map_get_direction(map, latitudes, longitudes)
     returns an array of the interpolated value of the healpix map at the given
     longitudes/latitudes. Longitudes and latitudes must be conformable.
     Longitudes/latitudes must be given in radians.
     The output array has the same dimensions as the "latitudes" array.
     
   SEE ALSO: healpix_map_init, healpix_map_load, healpix_map_get_pixel,
             healpix_map_get_map, healpix_map_put_direction
 */
  if (is_void(interp))
    interp=1;
  
  return healpix_map_get_direction_internal(map, latitudes, longitudes, interp);
}

extern healpix_map_put_direction;
/* DOCUMENT healpix_map_put_direction, map, data, latitudes, longitudes
     This subroutine sets the data contained in "map" using the provided samples
     in "data", given in the directions (latitudes,longitudes).
     Latitudes/longitudes must be given in radians. This subroutine implicitly
     bins the data the belong to a same healpix pixel.
     
   SEE ALSO: healpix_map_init, healpix_map_load, healpix_map_get_pixel,
             healpix_map_get_map, healpix_map_get_direction
 */

extern healpix_alm_init;
/* DOCUMENT healpix_alm_init(lmax,mmax,type) or healpix_alm_init(lmax,type)
     returns a newly built Alm object representation with (lmax,mmax) members.
     If no mmax are given, it will default to lmax.
   SEE ALSO: healpix_alm_map2alm, healpix_alm_alm2map,  healpix_alm_get_alms,
             healpix_alm_put_alms
 */

extern healpix_alm_load;
/* DOCUMENT healpix_alm_load(filename, type)
     returns a newly built Alm object representation and loads the alms
     values from the specified filename. The "type" is used to define
     the in-memory representation (float or double precision).

  SEE ALSO: healpix_alm_init
 */

extern healpix_alm_map2alm;
/* DOCUMENT healpix_alm_map2alm, alm, map
     This subroutine does a spherical harmonic transform of the given "map"
     and store the result in "alm".

   SEE ALSO: healpix_alm_init, healpix_alm_alm2map, healpix_alm_get_alms,
             healpix_alm_put_alms, healpix_map_init, healpix_alm_map2alm_iter
 */

extern healpix_alm_alm2map;
/* DOCUMENT healpix_alm_alm2map, alm, map
     This subroutine transforms the alm representation into a healpix map.
     The map should have been previously allocated using healpix_map_init
     or healpix_map_load.
     
   SEE ALSO: healpix_map_init, healpix_alm_alm2map, healpix_alm_get_alms,
             healpix_alm_put_alms
 */

extern healpix_alm_map2alm_iter;
/* DOCUMENT healpix_alm_map2alm_iter, alm, map, iter
     This subroutine does a spherical harmonic transform of the given "map"
     and store the result in "alm".
     This is the iterated variant of the transform for better precision.

   SEE ALSO: healpix_alm_alm2map_iter
 */

extern healpix_alm_get_alms1;
extern healpix_alm_get_alms2;

func healpix_alm_get_alms(alm,llist,mlist)
{
/* DOCUMENT healpix_alm_get_alms(alm[,llist,mlist])
     returns a complex array of the alms contained the "alm" object. This
     array is a copy of the internal data. Any modifications to "alm" must
     be done using healpix_alm_put_alms. The ordering follows healpix alm
     ordering.
     Optionally, one may give a list of Ls and Ms in  llist and mlist.
     In that case the returned array will contain the alms ordered in
     exactly the list specify it, with the same dimensions. llist and mlist
     must have the same dimensions.
     
   SEE ALSO: healpix_alm_init, healpix_alm_put_alms
 */
  if (llist == [] && mlist == [])
    return healpix_alm_get_alms1(alm);

  if (dimsof(llist)(1) != dimsof(mlist)(1))
    error,"Rank does not match for (l,m)s";

  if (allof(dimsof(llist) != dimsof(mlist)))
    error,"Dimensions of the (l,m)s must be the same";

  if (anyof(llist < mlist) || anyof(-llist > mlist))
    error,"-l <= m <= l for all elements of llist and mlist";
  return healpix_alm_get_alms2(alm,llist,mlist);
}

extern healpix_alm_put_alms1;
extern healpix_alm_put_alms2;

func healpix_alm_put_alms(alm,data,llist,mlist)
{
/* DOCUMENT healpix_alm_put_alms,alm,data[,llist,mlist]
     sets the complex array of the alms contained the "alm" object. This
     array is a copy of the internal data. Any modifications to "alm" must
     be done using healpix_alm_put_alms. The ordering follows healpix alm
     ordering.
     Optionally, one may give a list of Ls and Ms in  llist and mlist.
     In that case the returned array will contain the alms ordered in
     exactly the list specify it, with the same dimensions. llist and mlist
     must have the same dimensions.
     
   SEE ALSO: healpix_alm_init, healpix_alm_get_alms
 */
  if (llist == [] && mlist == [])
    healpix_alm_put_alms1,alm,data;

  if (dimsof(llist)(1) != dimsof(mlist)(1))
    error,"Rank does not match for (l,m)s";

  if (allof(dimsof(llist) != dimsof(mlist)))
    error,"Dimensions of the (l,m)s must be the same";

  if (anyof(llist < mlist) || anyof(-llist > mlist))
    error,"-l <= m <= l for all elements of llist and mlist";
  healpix_alm_put_alms2,alm,data,llist,mlist;
}

extern healpix_alm_get_lmmax;
/* DOCUMENT [lmax,mmax]=healpix_alm_get_lmmax(alm)
      returns the number of spherical harmonics mode supported by this
      this "alm" object.
      
   SEE ALSO: healpix_alm_init
 */

extern healpix_alm_scaleL;
/* DOCUMENT healpix_alm_scaleL,alm,scalingL
   scales the alm according the scalingL array. Each (l-1)-th entry of the entry must be
   a number which serves as scaling homogeneously the a_{l,m} for all m such that -l <= m <= l.
   scalingL must have the adequate size as given by healpix_alm_get_lmmax.
   
   SEE ALSO: healpix_alm_get_lmmax
 */

func healpix_smooth_with_Gaussian(alm, radius)
{
  sigma2fwhm=2.3548200450309493; // sqrt(8*log(2.))
  degr2rad = pi/180;
  
  fct = (radius>=0) ? 1 : -1;
  sigma = radius/60*degr2rad/sigma2fwhm;

  lmax = healpix_alm_get_lmmax(alm)(1);
  l = indgen(lmax+1)-1;
  gb = exp(-0.5 * fct * l*(l+1)*sigma*sigma);

  healpix_alm_scaleL,alm,gb;
}
