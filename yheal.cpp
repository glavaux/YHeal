#include <iostream>
#include <cstdlib>
#include <cstring>
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

#define POS_TYPE 1
#define POS_NSIDE 2

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
  
  if (argc != 2)
    {
      y_error("healpix_map_init takes exactly two arguments: Nside and type");
    }

  YorickHealpix *px = ypush_healpix();

  int nside = ygets_i(POS_NSIDE);
  cout << "   Nside = " << nside << " type = " << ygets_i(POS_TYPE) << endl;
  switch (ygets_i(POS_TYPE))
    {
    case HEALPIX_FLOAT:      
      px->type = Y_FLOAT;
      px->map_float = new Healpix_Map<float>(nside, RING, SET_NSIDE);
      break;
    case HEALPIX_DOUBLE:
      px->type = Y_DOUBLE;
      px->map_double = new Healpix_Map<double>(nside, RING, SET_NSIDE);
      break;
    default:
      y_error("Unsupported healpix map type");
      return;
    }
  return;
}

#undef POS_TYPE
#undef POS_NSIDE

/*
 * Object constructor from file
 */

#define POS_FNAME 2
#define POS_TYPE 1

extern "C"
void Y_healpix_map_load(int argc)
{
  if (yarg_subroutine())
    {
      y_error("healpix_map_load cannot be invoked as a subroutine");
      return;
    }

  if (argc != 2)
    {
      y_error("healpix_map_load: only two arguments");
      return;
    }

  YorickHealpix *px = ypush_healpix();
  ystring_t fname = ygets_q(POS_FNAME);
  int type = ygets_i(POS_TYPE);
  switch (type) 
    {
    case HEALPIX_FLOAT:
      px->type = Y_FLOAT;
      px->map_float = new Healpix_Map<float>();
      read_Healpix_map_from_fits(std::string(fname), *px->map_float);
      break;
    case HEALPIX_DOUBLE:
      px->type = Y_DOUBLE;
      px->map_double = new Healpix_Map<double>();
      read_Healpix_map_from_fits(std::string(fname), *px->map_double);
      break;
    };
 
  return;
}

#undef POS_FNAME
#undef POS_TYPE

/*
 * Object accessor
 */

extern "C"
void Y_healpix_map_get_pixel(int iarg)
{
  if (iarg != 2)
    {
      y_error("healpix_map_get_pixel needs two arguments");
    }

  YorickHealpix *px = yget_healpix(1);
  int pixel = ygets_i(0);

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
  if (iarg != 1)
    {
      y_error("healpix_map_get_map needs one arguments");
    }

  YorickHealpix *px = yget_healpix(0);
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
static void doInterpolation(Healpix_Map<T> *hmap, double *latitudes, double *longitudes,
		     int num, T *result)
{
  cout << "Interpolate in " << num << " directions" << endl;
  cout << latitudes << endl;
  cout << longitudes << endl;
  cout << result << endl;
  for (int i = 0; i < num; i++)
    {
      pointing p(latitudes[i], longitudes[i]);
      result[i] = hmap->interpolated_value(p);
    }
  cout << "Finished" << endl;
}

extern "C"
void Y_healpix_map_get_direction(int iarg)
{
  if (yarg_subroutine())
    {
      y_error("healpix_map_get_direction cannot be invoked as a subroutine");
      return;
    }


  if (iarg != 3)
    {
      y_error("healpix_map_get_direction needs three arguments");
    }

  YorickHealpix *px = yget_healpix(2);
  long ntot_lat;
  long ntot_long;


  int rank_lat = yarg_rank(1);
  int rank_long = yarg_rank(0);

  cout << "px=" << px << endl;
  cout << "px->generic=" << px->generic << endl;
  cout << "Ranks = " << rank_lat << " " << rank_long << endl;

  long *lat_dims;
  long *long_dims;

  {
    long tmp_dims[] = { 1, rank_lat+1 };
    lat_dims = ypush_l(tmp_dims);
  }

  {
    long tmp_dims[] = { 1, rank_long+1 };
    long_dims = ypush_l(tmp_dims);
  }

  // Arguments have been displaced by 2
  yarg_dims(3, lat_dims, 0);
  if (yarg_dims(2, long_dims, lat_dims) < 0)
    y_error("Argument 3 and 2 must be conformable");

  cout << "Dims=[";
  for (int i = 1; i < long_dims[0]; i++)
    cout << long_dims[i] << ",";
  cout << endl;
  cout << "Dims=[";
  for (int i = 1; i < long_dims[0]; i++)
    cout << lat_dims[i] << ",";
  cout << endl;

  double *latitudes = ygeta_d(3, &ntot_lat, 0);
  double *longitudes = ygeta_d(2, &ntot_long, 0);

  if (long_dims[0] == 0)
    long_dims = 0;
  if (lat_dims[0] == 0)
    lat_dims = 0;

  cout << "Do the interpolation" << endl;
  switch (px->type) {
  case Y_FLOAT:
    {
      float *result = ypush_f(lat_dims);
      doInterpolation(px->map_float, latitudes, longitudes, ntot_lat, result);
      break;
    }
  case Y_DOUBLE:
    {
      double *result = ypush_d(lat_dims);
      doInterpolation(px->map_double, latitudes, longitudes, ntot_lat, result);
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
      int pix = hmap.ang2pix(pointing(latid[i], longit[i]));

      hmap[pix] += data[i];
    }
}

extern "C"
void Y_healpix_map_put_direction(int iarg)
{
  if (yarg_subroutine())
    {
      y_error("healpix_map_put_direction is a a subroutine");
      return;
    }

  if (iarg != 4)
    {
      y_error("wrong number of arguments (must be 4)");
      return;
    }
  
  YorickHealpix *px = yget_healpix(3);
  long ntot_lat;
  long ntot_long;
  long ntot_data;

  int rank_lat = yarg_rank(1);
  int rank_long = yarg_rank(0);
  int rank_data = yarg_rank(2);
  long *lat_dims;
  long *long_dims;
  long *data_dims;

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

  // Arguments have been displaced by 2
  yarg_dims(4, lat_dims, 0);
  if (yarg_dims(3, long_dims, lat_dims) < 0 || yarg_dims(5, data_dims, lat_dims) < 0)
    y_error("Argument 2, 3 and 4 must be conformable");

  double *latitudes = ygeta_d(4, &ntot_lat, 0);
  double *longitudes = ygeta_d(3, &ntot_long, 0);
  
  switch (px->type) {
  case Y_FLOAT:
    {
      float *dataf = ygeta_f(5, &ntot_data, 0);
      putDataInHealpix(*px->map_float, longitudes, latitudes, dataf, ntot_data);
      break;
    }
  case Y_DOUBLE:
    {
      double *dataf = ygeta_d(5, &ntot_data, 0);
      putDataInHealpix(*px->map_double, longitudes, latitudes, dataf, ntot_data);
      break;
    }
  }
}


