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

extern healpix_map_get_direction;
/* DOCUMENT healpix_map_get_direction(map, latitudes, longitudes)
     returns an array of the interpolated value of the healpix map at the given
     longitudes/latitudes. Longitudes and latitudes must be conformable.
     Longitudes/latitudes must be given in radians.
     The output array has the same dimensions as the "latitudes" array.
     
   SEE ALSO: healpix_map_init, healpix_map_load, healpix_map_get_pixel,
             healpix_map_get_map, healpix_map_put_direction
 */

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

extern healpix_alm_map2alm;
/* DOCUMENT healpix_alm_map2alm, alm, map
     This subroutine does a spherical harmonic transform of the given "map"
     and store the result in "alm".

   SEE ALSO: healpix_alm_init, healpix_alm_alm2map, healpix_alm_get_alms,
             healpix_alm_put_alms, healpix_map_init
 */

extern healpix_alm_alm2map;
/* DOCUMENT healpix_alm_alm2map, alm, map
     This subroutine transforms the alm representation into a healpix map.
     The map should have been previously allocated using healpix_map_init
     or healpix_map_load.
     
   SEE ALSO: healpix_map_init, healpix_alm_alm2map, healpix_alm_get_alms,
             healpix_alm_put_alms
 */

extern healpix_alm_get_alms;
/* DOCUMENT healpix_alm_get_alms(alm)
     returns a complex array of the alms contained the "alm" object. This
     array is a copy of the internal data. Any modifications to "alm" must
     be done using healpix_alm_put_alms. The ordering follows healpix alm
     ordering.
     
   SEE ALSO: healpix_alm_init, healpix_alm_put_alms
 */

extern healpix_alm_put_alms;
/* DOCUMENT healpix_alm_put_alms, alm, array
     This sets the internal alm of the given "alm" object to the value
     sets in "array". The ordering of the array must follow healpix ordering.
     
   SEE ALSO: healpix_alm_init, healpix_alm_get_alms
 */
