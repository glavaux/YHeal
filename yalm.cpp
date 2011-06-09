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
#include <alm_fitsio.h>
#include <alm_healpix_tools.h>
#include <xcomplex.h>
#include <fitshandle.h>
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
      return;
    }

  YorickAlm *alm = ypush_alm();

  int lmax, mmax, type;
  if (argc == 2)
    {
      mmax = lmax = ygets_i(argc);
      type = ygets_i(argc-1);
    }
  else
    {
      lmax = ygets_i(argc);
      mmax = ygets_i(argc-1);
      type = ygets_i(argc-2);
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
Alm<xcomplex<T> > *loadAlm(const std::string& fname)
{
  Alm<xcomplex<T> > *alms;
  int lmax, mmax;

  try {
    get_almsize(fname, lmax, mmax);

    alms = new Alm<xcomplex<T> >(lmax, mmax);

    read_Alm_from_fits(fname, *alms, lmax, mmax);
  } catch (const PlanckError& e) {
    y_error("unable to open file");
    return 0;
  }

  return alms;
  
}

extern "C"
void Y_healpix_alm_load(int argc)
{
  y_check_arguments(argc, 2);

  int type = ygets_i(argc-2);
  ystring_t s = ygets_q(argc-1);
  YorickAlm *alm = ypush_alm();

  switch (type) {
  case HEALPIX_FLOAT:   
    alm->type = Y_FLOAT;
    alm->alm_float = loadAlm<float>(std::string(s));
    break;
  case HEALPIX_DOUBLE:
    alm->type = Y_DOUBLE;
    alm->alm_double = loadAlm<double>(std::string(s));
    break;
  default:
    y_error("Unsuported healpix type");
    return;
    break;
  }
}

template<typename T>
void saveHealpixAlm(const std::string& fname, Alm<xcomplex<T> > *alms)
{
  fitshandle f;

  try
    {
      f.create(fname.c_str());
    }
  catch (const PlanckError& e)
    {
      y_error(e.what());
    }
  write_Alm_to_fits(f, *alms, alms->Lmax(), alms->Mmax(), planckType<T>());
}

extern "C"
void Y_healpix_alm_save(int argc)
{
  y_check_arguments(argc, 2);
  ystring_t s = ygets_q(argc-1);
  YorickAlm *alm = yget_alm(argc-2);

  switch (alm->type) {
  case Y_FLOAT:
    saveHealpixAlm<float>(std::string(s), alm->alm_float);
    break;
  case Y_DOUBLE:
    saveHealpixAlm<double>(std::string(s), alm->alm_double);
    break;
  }
  ypush_nil();
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
  y_check_arguments(argc, 2);

  YorickAlm *alm = yget_alm(argc-1);
  YorickHealpix *px = yget_healpix(argc-2);

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
static void doTransform1bis(Healpix_Map<T>& m, Alm<xcomplex<T> >& alm, int iter)
{
  arr<double> wgt(2*m.Nside());
  wgt.fill(1);

  map2alm_iter(m, alm, iter, wgt);
}

extern "C"
void Y_healpix_alm_map2alm_iter(int argc)
{
  y_check_arguments(argc, 3);

  YorickAlm *alm = yget_alm(argc-1);
  YorickHealpix *px = yget_healpix(argc-2);
  int iter = ygets_i(argc-3);

  if (alm->type != px->type)
    y_error("Alm type is not compatible with Map type");

  switch (px->type)
    {
    case Y_FLOAT:
      doTransform1bis(*px->map_float, *alm->alm_float, iter);
      break;
    case Y_DOUBLE:
      doTransform1bis(*px->map_double, *alm->alm_double, iter);
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
  y_check_arguments(argc, 2);

  YorickAlm *alm = yget_alm(argc-1);
  YorickHealpix *px = yget_healpix(argc-2);

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
void Y_healpix_alm_get_alms1(int argc)
{
  y_check_arguments(argc, 1);

  YorickAlm *alm = yget_alm(argc-1);

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
static void buildAlmWithList(Alm<xcomplex<T> >& alms,
			     int *llist, int *mlist, long ntot)
{
  long dims[] = { 1, ntot };
  double *out = ypush_z(dims);

  for (long i = 0; i < ntot; i++)
    {
      const xcomplex<T>& c = alms(llist[i], mlist[i]);
      out[2*i + 0] = c.re;
      out[2*i + 1] = c.im;
    }
}


extern "C"
void Y_healpix_alm_get_alms2(int argc)
{
  y_check_arguments(argc, 3);

  YorickAlm *alm = yget_alm(argc-1);
  long ntotl, ntotm;

  int *llist = ygeta_i(argc-2, &ntotl, 0);
  int *mlist = ygeta_i(argc-3, &ntotm, 0);

  if (ntotl != ntotm)
    {
      y_error("Internal error: argument 2 and 3 should have the same number of elements");
      return;
    }

  switch (alm->type)
    {
    case Y_FLOAT:
      buildAlmWithList(*alm->alm_float, llist, mlist, ntotl);
      break;
    case Y_DOUBLE:
      buildAlmWithList(*alm->alm_double, llist, mlist, ntotl);
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
void Y_healpix_alm_put_alms1(int argc)
{
  y_check_arguments(argc, 2);

  YorickAlm *alm = yget_alm(argc-1);
  int rank_in = yarg_rank(argc-2);
  if (rank_in != 1)
    {
      y_error("wrong dimension for argument 2");
      return;
    }

  long dims[2];
  long ntot;
  double *in_alm = ygeta_z(argc-2, &ntot, dims);

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

template<typename T>
static void loadAlmWithList(Alm<xcomplex<T> >& alms, double *in,
                            int *llist, int *mlist, long ntot)
{
  for (long i = 0; i < ntot; i++)
    {
      alms(llist[i], mlist[i]).re = in[2*i + 0];
      alms(llist[i], mlist[i]).im = in[2*i + 1];
    }
}



extern "C"
void Y_healpix_alm_put_alms2(int argc)
{
  y_check_arguments(argc, 4);

  YorickAlm *alm = yget_alm(argc-1);
  long ntotl, ntotm;

  int *llist = ygeta_i(argc-3, &ntotl, 0);
  int *mlist = ygeta_i(argc-4, &ntotm, 0);

  if (ntotl != ntotm)
    {
      y_error("Internal error: argument 2 and 3 should have the same number of elements");
      return;
    }

  long dims[2];
  long ntot;
  double *in_alm = ygeta_z(argc-2, &ntot, dims);
  switch (alm->type)
    {
    case Y_FLOAT:
      loadAlmWithList(*alm->alm_float, in_alm, llist, mlist, ntotl);
      break;
    case Y_DOUBLE:
      loadAlmWithList(*alm->alm_double, in_alm, llist, mlist, ntotl);
      break;
    }
}


extern "C"
void Y_healpix_alm_get_lmmax(int argc)
{
  y_check_arguments(argc, 1);

  YorickAlm *alm = yget_alm(argc-1);
  long dims[2] = { 1, 2 };
  int *a = ypush_i(dims);
  switch (alm->type)
    {
    case Y_FLOAT:
      a[0] = alm->alm_float->Lmax();
      a[1] = alm->alm_float->Mmax();
      break;
    case Y_DOUBLE:
      a[0] = alm->alm_double->Lmax();
      a[1] = alm->alm_double->Mmax();
      break;
    }  
}

template<typename T>
void scaleALM(Alm<xcomplex<T> >& alm, double *scaling, long ntot)
{
  if (ntot < alm.Lmax())
    y_error("Insufficient scaling data");

  long mmax = alm.Mmax(), lmax = alm.Lmax();

  for (int m=0; m<=mmax; ++m)
    for (int l=m; l<=lmax; ++l)
      alm(l,m) *= scaling[l];
}

extern "C"
void Y_healpix_alm_scaleL(int argc)
{
  y_check_arguments(argc, 2);

  YorickAlm *alm = yget_alm(argc-1);
  int rank_in = yarg_rank(argc-2);
  if (rank_in != 1)
    {
      y_error("wrong dimension for argument 2");
      return;
    }

  long dims[2];
  long ntot;
  double *in_scaling = ygeta_z(argc-2, &ntot, dims);

  switch (alm->type) {
  case Y_FLOAT:
    scaleALM(*alm->alm_float, in_scaling, ntot);
    break;
  case Y_DOUBLE:
    scaleALM(*alm->alm_double, in_scaling, ntot);
    break;
  default:
    y_error("internal error");
    break;
  }

  
}
