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
#include <powspec.h>
#include <powspec_fitsio.h>
#include <xcomplex.h>
#define typeid ctypeid
#include <yapi.h>
#undef typeid
#include "yheal.hpp"

using namespace std;


extern "C"
void Y_healpix_powspec_load(int argc)
{
  ystring_t fname = ygets_q(argc-1);
  int nspec = ygets_i(argc-2);
  int lmax = ygets_i(argc-3);
  long dims[3] = { 2, lmax+1, nspec };
  double *full_powspec = ypush_d(dims);

  if (nspec != 1 && nspec != 4 && nspec != 6)
    {
       y_error("The number of spectra must be either 1, 4 or 6");
       return;
    }
  if (lmax < 0)
    {
       y_error("lmax should be positive.");
       return;
    }

  PowSpec pspec(nspec, lmax);

  read_powspec_from_fits(fname, pspec, nspec, lmax); 

  memcpy(&full_powspec[0*(lmax+1)], &pspec.tt()[0], sizeof(double)*(lmax+1));
  if (nspec >= 4) {
    memcpy(&full_powspec[1*(lmax+1)], &pspec.gg()[0], sizeof(double)*(lmax+1));
    memcpy(&full_powspec[2*(lmax+1)], &pspec.cc()[0], sizeof(double)*(lmax+1));
    memcpy(&full_powspec[3*(lmax+1)], &pspec.tg()[0], sizeof(double)*(lmax+1));
  }
  if (nspec == 6) {
    memcpy(&full_powspec[4*(lmax+1)], &pspec.gc()[0], sizeof(double)*(lmax+1));
    memcpy(&full_powspec[5*(lmax+1)], &pspec.tc()[0], sizeof(double)*(lmax+1));
  }
}
