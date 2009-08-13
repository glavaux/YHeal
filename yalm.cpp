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
#include <iostream>
#include <cstdlib>
#include <cstring>
#include <healpix_map.h>
#include <healpix_map_fitsio.h>
#include <alm.h>
#include <alm_healpix_tools.h>
#include <xcomplex.h>
#define typeid ctypeid
#include <yapi.h>
#undef typeid
#include "yheal.hpp"

using namespace std;


static void free_Yorick_healpix_alm(void *a);

y_userobj_t healpix_alm_obj = {
  (char *)"Healpix ALM object",
  free_Yorick_healpix_alm,
  0,
  0,
  0,
  0
};


static void free_Yorick_healpix_alm(void *a)
{
  YorickAlm *alm = (YorickAlm *)a;

  if (alm->generic == 0)
    return;

  switch (alm->type)
    {
    case Y_FLOAT:
      delete alm->alm_float;
      break;
    case Y_DOUBLE:
      delete alm->alm_double;
      break;
    }
}


extern "C"
void Y_healpix_alm_init(int argc)
{   
  if (argc != 3 && argc != 2)
    {
      y_error("healpix_map_init takes (two) three arguments: lmax (mmax) and type");
    }

  YorickAlm *alm = ypush_alm();

  int lmax, mmax, type;
  if (argc == 2)
    {
      mmax = lmax = ygets_i(2);
      type = ygets_i(1);
    }
  else
    {
      lmax = ygets_i(3);
      mmax = ygets_i(2);
      type = ygets_i(1);
    }

  switch (type)
    {
    case HEALPIX_FLOAT:      
      alm->type = Y_FLOAT;
      alm->alm_float = new Alm<xcomplex<float> >(lmax, mmax);
      break;
    case HEALPIX_DOUBLE:
      alm->type = Y_DOUBLE;
      alm->alm_double = new Alm<xcomplex<double> >(lmax, mmax);
      break;
    default:
      y_error("Unsupported healpix type");
      return;
    }
  return;

}


template<typename T>
static void doTransform1(Healpix_Map<T>& m, Alm<xcomplex<T> >& alm)
{
  arr<double> wgt(2*m.Nside());
  wgt.fill(1);

  map2alm(m, alm, wgt);
}

extern "C"
void Y_healpix_alm_map2alm(int argc)
{
  if (argc != 2)
    {
      y_error("wrong number of arguments");
      return;
    }

  YorickAlm *alm = yget_alm(1);
  YorickHealpix *px = yget_healpix(0);

  if (alm->type != px->type)
    y_error("Alm type is not compatible with Map type");

  switch (px->type)
    {
    case Y_FLOAT:
      doTransform1(*px->map_float, *alm->alm_float);
      break;
    case Y_DOUBLE:
      doTransform1(*px->map_double, *alm->alm_double);
      break;
    default:
      y_error("internal error");
    }  
}



template<typename T>
static void doTransform2(Healpix_Map<T>& m, Alm<xcomplex<T> >& alm)
{
  alm2map(alm, m);
}

extern "C"
void Y_healpix_alm_alm2map(int argc)
{
  if (argc != 2)
    {
      y_error("wrong number of arguments");
      return;
    }

  YorickAlm *alm = yget_alm(1);
  YorickHealpix *px = yget_healpix(0);

  if (alm->type != px->type)
    y_error("Alm type is not compatible with Map type");

  switch (px->type)
    {
    case Y_FLOAT:
      doTransform2(*px->map_float, *alm->alm_float);
      break;
    case Y_DOUBLE:
      doTransform2(*px->map_double, *alm->alm_double);
      break;
    default:
      y_error("internal error");
    }  
}

template<typename T>
static void buildAlmArray(Alm<xcomplex<T> >& alms)
{
  const arr<xcomplex<T> >& a = alms.Alms();
  long dims[] = { 1, a.size() };
  double *out = ypush_z(dims);

  for (long i = 0; i < a.size(); i++)
    {
      out[2*i + 0] = a[i].re;
      out[2*i + 1] = a[i].im;
    }
}

extern "C"
void Y_healpix_alm_get_alms(int argc)
{
  if (argc != 1)
    {
      y_error("wrong number of arguments");
      return;
    }

  YorickAlm *alm = yget_alm(0);
  switch (alm->type)
    {
    case Y_FLOAT:
      buildAlmArray(*alm->alm_float);
      break;
    case Y_DOUBLE:
      buildAlmArray(*alm->alm_double);
      break;
    default:
      y_error("internal error");
      break;
    }
  
}

template<typename T>
static void loadAlmArray(Alm<xcomplex<T> >& alms, double *in, long ntot)
{
  const arr<xcomplex<T> >& a = alms.Alms();

  if (a.size() != ntot)
    {
      y_error("wrong number of elements for input alms");
      return;
    }

  // Stupid hack to go around the limitation of the official API
  xcomplex<T> *alm_array = alms.mstart(0);

  for (long i = 0; i < a.size(); i++)
    {
      alm_array[i].re = in[2*i + 0];
      alm_array[i].im = in[2*i + 1];
    }
}

extern "C"
void Y_healpix_alm_put_alms(int argc)
{
  if (argc != 2)
    {
      y_error("wrong number of arguments");
      return;
    }

  YorickAlm *alm = yget_alm(1);
  int rank_in = yarg_rank(0);
  if (rank_in != 1)
    {
      y_error("wrong dimension for argument 2");
      return;
    }

  long dims[2];
  long ntot;
  double *in_alm = ygeta_z(0, &ntot, dims);

  switch (alm->type)
    {
    case Y_FLOAT:
      loadAlmArray(*alm->alm_float, in_alm, ntot);
      break;
    case Y_DOUBLE:
      loadAlmArray(*alm->alm_double, in_alm, ntot);
      break;
    default:
      y_error("internal error");
      break;
    }

  ypush_nil();
}

