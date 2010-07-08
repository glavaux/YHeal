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
#include <string>
#include <fitshandle.h>
#include <healpix_map.h>
#include <healpix_map_fitsio.h>
#define typeid ctypeid
#include <yapi.h>
#undef typeid
#include "yheal.hpp"

using namespace std;

/*
 * Object constructor 1
 */

static void free_Yorick_healpix(void *a);

y_userobj_t healpix_obj = {
  (char *)"Healpix object",
  free_Yorick_healpix,
  0,
  0,
  0,
  0
};


static void free_Yorick_healpix(void *a)
{
  YorickHealpix *px = (YorickHealpix *)a;

  if (px->generic == 0)
    return;

  switch (px->type)
    {
    case Y_FLOAT:
      delete px->map_float;
      break;
    case Y_DOUBLE:
      delete px->map_double;
      break;
    }
}



extern "C"
void Y_healpix_map_init(int argc)
{
  if (yarg_subroutine())
    {
      y_error("healpix_map_init cannot be invoked as a subroutine");
    }
  
  y_check_arguments(argc, 2);

  YorickHealpix *px = ypush_healpix();
  argc++;
  
  int nside = ygets_i(argc-1);
  switch (ygets_i(argc-2))
    {
    case HEALPIX_FLOAT:      
      px->type = Y_FLOAT;
      px->map_float = new Healpix_Map<float>(nside, RING, SET_NSIDE);
      for (long i = 0; i < px->map_float->Npix(); i++)
        (*px->map_float)[i] = 0;
      break;
    case HEALPIX_DOUBLE:
      px->type = Y_DOUBLE;
      px->map_double = new Healpix_Map<double>(nside, RING, SET_NSIDE);
      for (long i = 0; i < px->map_double->Npix(); i++)
        (*px->map_double)[i] = 0;
      break;
    default:
      y_error("Unsupported healpix map type");
      return;
    }
  return;
}

/*
 * Object constructor from file
 */

extern "C"
void Y_healpix_map_load(int argc)
{
  if (yarg_subroutine())
    {
      y_error("healpix_map_load cannot be invoked as a subroutine");
      return;
    }

  if (argc < 2 || argc > 3)
    {
      y_error("invalid number of arguments");
      return;
    }


  int hdunum = 1;
  if (argc == 3)
    hdunum = ygets_i(argc-3);

  YorickHealpix *px = ypush_healpix();
  argc++;


  ystring_t fname = ygets_q(argc-1);
  int type = ygets_i(argc-2);
  switch (type) 
    {
    case HEALPIX_FLOAT:
      px->type = Y_FLOAT;
      px->map_float = new Healpix_Map<float>();
      try
	{
	  read_Healpix_map_from_fits(std::string(fname), *px->map_float, hdunum);
	}
      catch (const Message_error& e)
	{
	  y_error("Invalid FITS file");
	  return;
	}
      break;
    case HEALPIX_DOUBLE:
      px->type = Y_DOUBLE;
      px->map_double = new Healpix_Map<double>();
      try
	{
	  read_Healpix_map_from_fits(std::string(fname), *px->map_double, hdunum);
	}
      catch (const Message_error& e)
	{
	  y_error("Invalid FITS file");
	  return;
	}
      break;
    };
 
  return;
}


/*
 * Object storage
 */

template<typename T,typename T2>
void reallyStoreMap(const string& fname, Healpix_Map<T2>* map)
{
  fitshandle f;

  try
    {
      f.create(fname);
    }
  catch (const Message_error& e)
    {
      y_error("Could not create file (already present or disk full)");
      return;
    }
  
  write_Healpix_map_to_fits(f, *map, FITSUTIL<T>::DTYPE);
}

template<typename T>
void storeHealpixMap(const string& fname, Healpix_Map<T>* map, int type)
{
  switch (type)
    {
    case HEALPIX_FLOAT:
      reallyStoreMap<float,T>(fname, map);
      break;
    case HEALPIX_DOUBLE:
      reallyStoreMap<double,T>(fname, map);
      break;
    }
}

extern "C"
void Y_healpix_map_save(int iarg)
{
   y_check_arguments(iarg, 3); 
 
   ystring_t fname = ygets_q(iarg-1);
   int type = ygets_i(iarg-2);
   YorickHealpix *map = yget_healpix(iarg-3);
  
   switch (map->type)
     {
     case Y_FLOAT:
       storeHealpixMap(string(fname), map->map_float, type);
       break;
     case Y_DOUBLE:
       storeHealpixMap(string(fname), map->map_double, type);
       break;
     }

}

/*
 * Object accessor
 */

extern "C"
void Y_healpix_map_get_pixel(int iarg)
{
  y_check_arguments(iarg, 2);

  YorickHealpix *px = yget_healpix(iarg-1);
  int pixel = ygets_i(iarg-2);

  switch (px->type) {
  case Y_FLOAT:
    if (pixel > px->map_float->Npix())
      y_error("Out of range pixel value");

    ypush_double((*px->map_float)[pixel]);
    break;
  case Y_DOUBLE:
    if (pixel > px->map_double->Npix())
      y_error("Out of range pixel value");

    ypush_double((*px->map_double)[pixel]);
    break;
  }
}

extern "C"
void Y_healpix_map_get_map(int iarg)
{
  y_check_arguments(iarg, 1);

  YorickHealpix *px = yget_healpix(iarg-1);
  switch (px->type) {
  case Y_FLOAT:
    {
      long dims[] = { 1, px->map_float->Npix() };
      float *full_map = ypush_f(dims);
      memcpy(full_map, &px->map_float->Map()[0], sizeof(float)*dims[1]);
      break;
    }
  case Y_DOUBLE:
    {
      long dims[] = { 1, px->map_double->Npix() };
      double *full_map = ypush_d(dims);
      memcpy(full_map, &px->map_double->Map()[0], sizeof(double)*dims[1]);
      break;
    }
  }
}

template<typename T>
void putMap(Healpix_Map<T> *map, T *data, long ntot)
{
  if (map->Npix() != ntot)
    y_error("Invalid number of elements in the input array");

  memcpy(&(*map)[0], data, sizeof(T)*ntot);
}


extern "C"
void Y_healpix_map_put_map(int iarg)
{
  long ntot;

  y_check_arguments(iarg, 2);


  YorickHealpix *px = yget_healpix(iarg-1);
  switch (px->type) {
  case Y_FLOAT:
    {
      float *full_map = ygeta_f(iarg-2, &ntot, 0);
      putMap(px->map_float, full_map, ntot);
      break;
    }
  case Y_DOUBLE:
    {
      double *full_map = ygeta_d(iarg-2, &ntot, 0);
      putMap(px->map_double, full_map, ntot);
      break;
    }
  }
}


template<typename T>
static void doInterpolation(Healpix_Map<T> *hmap, double *latitudes, double *longitudes,
			    int num, T *result, bool needInterpolation)
{
  if (needInterpolation)
    {
      for (int i = 0; i < num; i++)
	{
	  pointing p(latitudes[i], longitudes[i]);      
          p.normalize();
	  result[i] = hmap->interpolated_value(p);
	}
    }
  else
    {
      for (int i = 0; i < num; i++)
	{
	  pointing p(latitudes[i], longitudes[i]);      
          p.normalize();
	  result[i] = (*hmap)[ hmap->ang2pix(p) ];
	}
      
    }
}

extern "C"
void Y_healpix_map_get_direction_internal(int iarg)
{
  if (yarg_subroutine())
    {
      y_error("healpix_map_get_direction cannot be invoked as a subroutine");
      return;
    }

  y_check_arguments(iarg, 4);
  
  bool needInterpolation = (ygets_i(iarg-4) != 0);

  YorickHealpix *px = yget_healpix(iarg-1);
  long ntot_lat;
  long ntot_long;

  int rank_lat = yarg_rank(iarg-2);
  int rank_long = yarg_rank(iarg-3);

  long *lat_dims;
  long *long_dims;

  lat_dims = (long *)alloca(sizeof(long)*(rank_lat+1));
  long_dims = (long *)alloca(sizeof(long)*(rank_long+1));

  // Arguments have been displaced by 2
  yarg_dims(iarg-2, lat_dims, 0);
  if (yarg_dims(iarg-3, long_dims, lat_dims) < 0)
    y_error("Argument 3 and 2 must be conformable");

  double *latitudes = ygeta_d(iarg-2, &ntot_lat, 0);
  double *longitudes = ygeta_d(iarg-3, &ntot_long, 0);

  if (long_dims[0] == 0)
    long_dims = 0;
  if (lat_dims[0] == 0)
    lat_dims = 0;

  switch (px->type) {
  case Y_FLOAT:
    {
      float *result = ypush_f(lat_dims);
      doInterpolation(px->map_float, latitudes, longitudes, ntot_lat, result, needInterpolation);
      break;
    }
  case Y_DOUBLE:
    {
      double *result = ypush_d(lat_dims);
      doInterpolation(px->map_double, latitudes, longitudes, ntot_lat, result, needInterpolation);
      break;
    }
  default:
    y_error("internal error");
  } 

}


/*
 * Project a set of data in directions dir on the healpix map
 */

template<typename T>
static void putDataInHealpix(Healpix_Map<T>& hmap, double *longit, double *latid, T *data, long numData)
{
  for (long i = 0; i < numData; i++)
    {
      pointing p(latid[i], longit[i]);
      p.normalize();
      int pix = hmap.ang2pix(p);

      hmap[pix] += data[i];
    }
}

extern "C"
void Y_healpix_map_put_direction(int iarg)
{
  if (!yarg_subroutine())
    {
      y_error("healpix_map_put_direction is a a subroutine");
      return;
    }

  y_check_arguments(iarg, 4);
  
  YorickHealpix *px = yget_healpix(iarg-1);
  long ntot_lat;
  long ntot_long;
  long ntot_data;

  int rank_data = yarg_rank(iarg-2);
  int rank_lat = yarg_rank(iarg-3);
  int rank_long = yarg_rank(iarg-4);
  long *lat_dims;
  long *long_dims;
  long *data_dims;

  cout << "Got ranks" << endl;
  {
    long tmp_dims[] = { 1, rank_lat+1 };
    lat_dims = ypush_l(tmp_dims);
  }

  {
    long tmp_dims[] = { 1, rank_long+1 };
    long_dims = ypush_l(tmp_dims);
  }

  {
    long tmp_dims[] = { 1, rank_data+1 };
    data_dims = ypush_l(tmp_dims);
  }
  iarg += 3;

  // Arguments have been displaced by 2
  yarg_dims(iarg-3, lat_dims, 0);
  if (yarg_dims(iarg-4, long_dims, lat_dims) < 0 || 
      yarg_dims(iarg-2, data_dims, lat_dims) < 0)
    y_error("Argument 2, 3 and 4 must be conformable");
  cout << "Got dim" << endl;

  double *latitudes = ygeta_d(iarg-3, &ntot_lat, 0);
  double *longitudes = ygeta_d(iarg-4, &ntot_long, 0);
  
  cout << "Got arrays" << endl;
  switch (px->type) {
  case Y_FLOAT:
    {
      float *dataf = ygeta_f(iarg-2, &ntot_data, 0);
      cout << "Got f data" << endl;
      putDataInHealpix(*px->map_float, longitudes, latitudes, dataf, ntot_data);
      break;
    }
  case Y_DOUBLE:
    {
      double *dataf = ygeta_d(iarg-2, &ntot_data, 0);
      cout << "Got d data" << endl;
      putDataInHealpix(*px->map_double, longitudes, latitudes, dataf, ntot_data);
      break;
    }
  }
}


