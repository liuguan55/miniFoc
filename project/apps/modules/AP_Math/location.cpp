/// -*- tab-width: 4; Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil -*-
/*
 * location.cpp
 * Copyright (C) Andrew Tridgell 2011
 *
 * This file is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This file is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/*
 *  this module deals with calculations involving struct Location
 */
#include <AP_HAL/AP_HAL.h>
#include <stdlib.h>
#include "AP_Math.h"

// radius of earth in meters
#define RADIUS_OF_EARTH 6378100

// scaling factor from 1e-7 degrees to meters at equater
// == 1.0e-7 * DEG_TO_RAD * RADIUS_OF_EARTH
#define LOCATION_SCALING_FACTOR 0.011131884502145034f
// inverse of LOCATION_SCALING_FACTOR
#define LOCATION_SCALING_FACTOR_INV 89.83204953368922f

float longitude_scale(const struct Location &loc)
{
    static int32_t last_lat;
    static float scale = 1.0;
    if (labs(last_lat - loc.lat) < 100000) {
        // we are within 0.01 degrees (about 1km) of the
        // same latitude. We can avoid the cos() and return
        // the same scale factor.
        return scale;
    }
    scale = cosf(loc.lat * 1.0e-7f * DEG_TO_RAD);
    scale = constrain_float(scale, 0.01f, 1.0f);
    last_lat = loc.lat;
    return scale;
}



// return distance in meters between two locations
float get_distance(const struct Location &loc1, const struct Location &loc2)
{
    float dlat              = (float)(loc2.lat - loc1.lat);
    float dlong             = ((float)(loc2.lng - loc1.lng)) * longitude_scale(loc2);
    return pythagorous2(dlat, dlong) * LOCATION_SCALING_FACTOR;
}

// return distance in centimeters to between two locations
uint32_t get_distance_cm(const struct Location &loc1, const struct Location &loc2)
{
    return get_distance(loc1, loc2) * 100;
}

// return bearing in centi-degrees between two locations
int32_t get_bearing_cd(const struct Location &loc1, const struct Location &loc2)
{
    int32_t off_x = loc2.lng - loc1.lng;
    int32_t off_y = (loc2.lat - loc1.lat) / longitude_scale(loc2);
    int32_t bearing = 9000 + atan2f(-off_y, off_x) * 5729.57795f;
    if (bearing < 0) bearing += 36000;
    return bearing;
}

// see if location is past a line perpendicular to
// the line between point1 and point2. If point1 is
// our previous waypoint and point2 is our target waypoint
// then this function returns true if we have flown past
// the target waypoint
bool location_passed_point(const struct Location &location,
                           const struct Location &point1,
                           const struct Location &point2)
{
    // the 3 points form a triangle. If the angle between lines
    // point1->point2 and location->point2 is greater than 90
    // degrees then we have passed the waypoint
    Vector2f loc1(location.lat, location.lng);
    Vector2f pt1(point1.lat, point1.lng);
    Vector2f pt2(point2.lat, point2.lng);
    float angle = (loc1 - pt2).angle(pt1 - pt2);
    if (isinf(angle)) {
        // two of the points are co-located.
        // If location is equal to point2 then say we have passed the
        // waypoint, otherwise say we haven't
        if (get_distance(location, point2) == 0) {
            return true;
        }
        return false;
    } else if (angle == 0) {
        // if we are exactly on the line between point1 and
        // point2 then we are past the waypoint if the
        // distance from location to point1 is greater then
        // the distance from point2 to point1
        return get_distance(location, point1) >
               get_distance(point2, point1);

    }
    if (degrees(angle) > 90) {
        return true;
    }
    return false;
}

/*
 *  extrapolate latitude/longitude given bearing and distance
 * Note that this function is accurate to about 1mm at a distance of 
 * 100m. This function has the advantage that it works in relative
 * positions, so it keeps the accuracy even when dealing with small
 * distances and floating point numbers
 */
void location_update(struct Location &loc, float bearing, float distance)
{
    float ofs_north = cosf(radians(bearing))*distance;
    float ofs_east  = sinf(radians(bearing))*distance;
    location_offset(loc, ofs_north, ofs_east);
}

/*
 *  extrapolate latitude/longitude given distances north and east
 *  This function costs about 80 usec on an AVR2560
 */
void location_offset(struct Location &loc, float ofs_north, float ofs_east)
{
    if (ofs_north != 0 || ofs_east != 0) {
        int32_t dlat = ofs_north * LOCATION_SCALING_FACTOR_INV;
        int32_t dlng = (ofs_east * LOCATION_SCALING_FACTOR_INV) / longitude_scale(loc);
        loc.lat += dlat;
        loc.lng += dlng;
    }
}

/*
  return the distance in meters in North/East plane as a N/E vector
  from loc1 to loc2
 */
Vector2f location_diff(const struct Location &loc1, const struct Location &loc2)
{
    return Vector2f((loc2.lat - loc1.lat) * LOCATION_SCALING_FACTOR,
                    (loc2.lng - loc1.lng) * LOCATION_SCALING_FACTOR * longitude_scale(loc1));
}

/*
  wrap an angle in centi-degrees to 0..35999
 */
int32_t wrap_360_cd(int32_t error)
{
    if (error > 360000 || error < -360000) {
        // for very large numbers use modulus
        error = error % 36000;
    }
    while (error >= 36000) error -= 36000;
    while (error < 0) error += 36000;
    return error;
}

/*
  wrap an angle in centi-degrees to -18000..18000
 */
int32_t wrap_180_cd(int32_t error)
{
    if (error > 360000 || error < -360000) {
        // for very large numbers use modulus
        error = error % 36000;
    }
    while (error > 18000) { error -= 36000; }
    while (error < -18000) { error += 36000; }
    return error;
}

/*
  wrap an angle in centi-degrees to 0..35999
 */
float wrap_360_cd_float(float angle)
{
    if (angle >= 72000.0f || angle < -36000.0f) {
        // for larger number use fmodulus
        angle = fmod(angle, 36000.0f);
    }
    if (angle >= 36000.0f) angle -= 36000.0f;
    if (angle < 0.0f) angle += 36000.0f;
    return angle;
}

/*
  wrap an angle in centi-degrees to -18000..18000
 */
float wrap_180_cd_float(float angle)
{
    if (angle > 54000.0f || angle < -54000.0f) {
        // for large numbers use modulus
        angle = fmod(angle,36000.0f);
    }
    if (angle > 18000.0f) { angle -= 36000.0f; }
    if (angle < -18000.0f) { angle += 36000.0f; }
    return angle;
}

/*
  wrap an angle defined in radians to -PI ~ PI (equivalent to +- 180 degrees)
 */
float wrap_PI(float angle_in_radians)
{
    if (angle_in_radians > 10*PI || angle_in_radians < -10*PI) {
        // for very large numbers use modulus
        angle_in_radians = fmodf(angle_in_radians, 2*PI);
    }
    while (angle_in_radians > PI) angle_in_radians -= 2*PI;
    while (angle_in_radians < -PI) angle_in_radians += 2*PI;
    return angle_in_radians;
}

/*
  print a int32_t lat/long in decimal degrees
 */
void print_latlon(AP_HAL::BetterStream *s, int32_t lat_or_lon)
{
    int32_t dec_portion, frac_portion;
    int32_t abs_lat_or_lon = labs(lat_or_lon);

    // extract decimal portion (special handling of negative numbers to ensure we round towards zero)
    dec_portion = abs_lat_or_lon / 10000000UL;

    // extract fractional portion
    frac_portion = abs_lat_or_lon - dec_portion*10000000UL;

    // print output including the minus sign
    if( lat_or_lon < 0 ) {
        s->printf_P(PSTR("-"));
    }
    s->printf_P(PSTR("%ld.%07ld"),(long)dec_portion,(long)frac_portion);
}

#if HAL_CPU_CLASS >= HAL_CPU_CLASS_75

void wgsllh2ecef(const Vector3d &llh, Vector3d &ecef) {
  double d = WGS84_E * sin(llh[0]);
  double N = WGS84_A / sqrt(1. - d*d);

  ecef[0] = (N + llh[2]) * cos(llh[0]) * cos(llh[1]);
  ecef[1] = (N + llh[2]) * cos(llh[0]) * sin(llh[1]);
  ecef[2] = ((1 - WGS84_E*WGS84_E)*N + llh[2]) * sin(llh[0]);
}


void wgsecef2llh(const Vector3d &ecef, Vector3d &llh) {
  /* Distance from polar axis. */
  const double p = sqrt(ecef[0]*ecef[0] + ecef[1]*ecef[1]);

  /* Compute longitude first, this can be done exactly. */
  if (p != 0)
    llh[1] = atan2(ecef[1], ecef[0]);
  else
    llh[1] = 0;

  /* If we are close to the pole then convergence is very slow, treat this is a
   * special case. */
  if (p < WGS84_A*1e-16) {
    llh[0] = copysign(M_PI_2, ecef[2]);
    llh[2] = fabs(ecef[2]) - WGS84_B;
    return;
  }

  /* Caluclate some other constants as defined in the Fukushima paper. */
  const double P = p / WGS84_A;
  const double e_c = sqrt(1. - WGS84_E*WGS84_E);
  const double Z = fabs(ecef[2]) * e_c / WGS84_A;

  /* Initial values for S and C correspond to a zero height solution. */
  double S = Z;
  double C = e_c * P;

  /* Neither S nor C can be negative on the first iteration so
   * starting prev = -1 will not cause and early exit. */
  double prev_C = -1;
  double prev_S = -1;

  double A_n, B_n, D_n, F_n;

  /* Iterate a maximum of 10 times. This should be way more than enough for all
   * sane inputs */
  for (int i=0; i<10; i++)
  {
    /* Calculate some intermmediate variables used in the update step based on
     * the current state. */
    A_n = sqrt(S*S + C*C);
    D_n = Z*A_n*A_n*A_n + WGS84_E*WGS84_E*S*S*S;
    F_n = P*A_n*A_n*A_n - WGS84_E*WGS84_E*C*C*C;
    B_n = 1.5*WGS84_E*S*C*C*(A_n*(P*S - Z*C) - WGS84_E*S*C);

    /* Update step. */
    S = D_n*F_n - B_n*S;
    C = F_n*F_n - B_n*C;

    /* The original algorithm as presented in the paper by Fukushima has a
     * problem with numerical stability. S and C can grow very large or small
     * and over or underflow a double. In the paper this is acknowledged and
     * the proposed resolution is to non-dimensionalise the equations for S and
     * C. However, this does not completely solve the problem. The author caps
     * the solution to only a couple of iterations and in this period over or
     * underflow is unlikely but as we require a bit more precision and hence
     * more iterations so this is still a concern for us.
     *
     * As the only thing that is important is the ratio T = S/C, my solution is
     * to divide both S and C by either S or C. The scaling is chosen such that
     * one of S or C is scaled to unity whilst the other is scaled to a value
     * less than one. By dividing by the larger of S or C we ensure that we do
     * not divide by zero as only one of S or C should ever be zero.
     *
     * This incurs an extra division each iteration which the author was
     * explicityl trying to avoid and it may be that this solution is just
     * reverting back to the method of iterating on T directly, perhaps this
     * bears more thought?
     */

    if (S > C) {
      C = C / S;
      S = 1;
    } else {
      S = S / C;
      C = 1;
    }

    /* Check for convergence and exit early if we have converged. */
    if (fabs(S - prev_S) < 1e-16 && fabs(C - prev_C) < 1e-16) {
      break;
    } else {
      prev_S = S;
      prev_C = C;
    }
  }

  A_n = sqrt(S*S + C*C);
  llh[0] = copysign(1.0, ecef[2]) * atan(S / (e_c*C));
  llh[2] = (p*e_c*C + fabs(ecef[2])*S - WGS84_A*e_c*A_n) / sqrt(e_c*e_c*C*C + S*S);
}

#endif

