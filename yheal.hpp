#ifndef __YHEAL_HPP
#define __YHEAL_HPP

#include <alm.h>
#include <healpix_map.h>
#include <xcomplex.h>

#define typeid ctypeid
#include <yapi.h>
#undef typeid

#define HEALPIX_FLOAT 0
#define HEALPIX_DOUBLE 1

struct YorickHealpix
{
  int type;
  union
  {
    Healpix_Map<float> *map_float;
    Healpix_Map<double> *map_double;
    void *generic;
  };
};

struct YorickAlm
{
  int type;
  union
  {
    Alm<xcomplex<float> > *alm_float;
    Alm<xcomplex<double> > *alm_double;
    void *generic;
  };
};

/*
 * Healpix yorick object support functions.
 * We define the object and creation helpers.
 */

extern y_userobj_t healpix_obj;

static YorickHealpix *ypush_healpix()
{
  YorickHealpix *px = (YorickHealpix *)ypush_obj(&healpix_obj, sizeof(YorickHealpix));
  px->generic = 0;

  return px;
}

static YorickHealpix *yget_healpix(int iarg)
{
  return (YorickHealpix *)yget_obj(iarg, &healpix_obj);
}

/*
 * Alm helper functions
 */

extern y_userobj_t healpix_alm_obj;

static YorickAlm *ypush_alm()
{
  YorickAlm *px = (YorickAlm *)ypush_obj(&healpix_alm_obj, sizeof(YorickAlm));
  px->generic = 0;

  return px;
}

static YorickAlm *yget_alm(int iarg)
{
  return (YorickAlm *)yget_obj(iarg, &healpix_alm_obj);
}

#endif
