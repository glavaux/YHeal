/* codger-generated yorick package wrapper file */
#include "play.h"
#include "ydata.h"

/*----------------begin yheal.i */
extern BuiltIn Y_healpix_map_init;
extern BuiltIn Y_healpix_map_load;
extern BuiltIn Y_healpix_map_save;
extern BuiltIn Y_healpix_map_get_pixel;
extern BuiltIn Y_healpix_map_get_map;
extern BuiltIn Y_healpix_map_put_map;
extern BuiltIn Y_healpix_map_get_direction_internal;
extern BuiltIn Y_healpix_map_put_direction;
extern BuiltIn Y_healpix_alm_init;
extern BuiltIn Y_healpix_alm_load;
extern BuiltIn Y_healpix_alm_save;
extern BuiltIn Y_healpix_alm_map2alm;
extern BuiltIn Y_healpix_alm_alm2map;
extern BuiltIn Y_healpix_alm_map2alm_iter;
extern BuiltIn Y_healpix_alm_get_alms1;
extern BuiltIn Y_healpix_alm_get_alms2;
extern BuiltIn Y_healpix_alm_put_alms1;
extern BuiltIn Y_healpix_alm_put_alms2;
extern BuiltIn Y_healpix_alm_get_lmmax;
extern BuiltIn Y_healpix_alm_scaleL;
extern BuiltIn Y_healpix_powspec_load;

/*----------------list include files */

static char *y0_includes[] = {
  "yheal.i",
  0,
  0
};

/*----------------collect pointers and names */

static BuiltIn *y0_routines[] = {
  &Y_healpix_map_init,
  &Y_healpix_map_load,
  &Y_healpix_map_save,
  &Y_healpix_map_get_pixel,
  &Y_healpix_map_get_map,
  &Y_healpix_map_put_map,
  &Y_healpix_map_get_direction_internal,
  &Y_healpix_map_put_direction,
  &Y_healpix_alm_init,
  &Y_healpix_alm_load,
  &Y_healpix_alm_save,
  &Y_healpix_alm_map2alm,
  &Y_healpix_alm_alm2map,
  &Y_healpix_alm_map2alm_iter,
  &Y_healpix_alm_get_alms1,
  &Y_healpix_alm_get_alms2,
  &Y_healpix_alm_put_alms1,
  &Y_healpix_alm_put_alms2,
  &Y_healpix_alm_get_lmmax,
  &Y_healpix_alm_scaleL,
  &Y_healpix_powspec_load,
  0
};

static void *y0_values[] = {
  0
};

static char *y0_names[] = {
  "healpix_map_init",
  "healpix_map_load",
  "healpix_map_save",
  "healpix_map_get_pixel",
  "healpix_map_get_map",
  "healpix_map_put_map",
  "healpix_map_get_direction_internal",
  "healpix_map_put_direction",
  "healpix_alm_init",
  "healpix_alm_load",
  "healpix_alm_save",
  "healpix_alm_map2alm",
  "healpix_alm_alm2map",
  "healpix_alm_map2alm_iter",
  "healpix_alm_get_alms1",
  "healpix_alm_get_alms2",
  "healpix_alm_put_alms1",
  "healpix_alm_put_alms2",
  "healpix_alm_get_lmmax",
  "healpix_alm_scaleL",
  "healpix_powspec_load",
  0
};

/*----------------define package initialization function */

PLUG_EXPORT char *yk_yheal(char ***,
                         BuiltIn ***, void ***, char ***);
static char *y0_pkgname = "yheal";

char *
yk_yheal(char ***ifiles,
       BuiltIn ***code, void ***data, char ***varname)
{
  *ifiles = y0_includes;
  *code = y0_routines;
  *data = y0_values;
  *varname = y0_names;
  return y0_pkgname;
}
