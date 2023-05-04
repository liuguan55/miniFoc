// -*- tab-width: 4; Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil -*-

#ifndef AP_MATH_H
#define AP_MATH_H

// Assorted useful math operations for ArduPilot(Mega)

#include <math.h>

#include <stdint.h>
#include "AP_HAL/AP_HAL.h"
#include "rotations.h"
#include "vector2.h"
#include "vector3.h"
#include "matrix3.h"
#include "quaternion.h"
#include "polygon.h"
#include "edc.h"


#if PRINTF_FORMAT_WARNING_DEBUG
# undef PSTR
 # define PSTR(_x)               _x             // help the compiler with printf_P
 # define float double                  // silence spurious format warnings for %f
#endif

#define FPSTR(s) (wchar_t *)(s)

#define ToRad(x) radians(x)	// *pi/180
#define ToDeg(x) degrees(x)	// *180/pi

/*
  check if bit bitnumber is set in value, returned as a
  bool. Bitnumber starts at 0 for the first bit
 */
#define BIT_IS_SET(value, bitnumber) (((value) & (1U<<(bitnumber))) != 0)

// get high or low bytes from 2 byte integer
#define LOWBYTE(i) ((uint8_t)(i))
#define HIGHBYTE(i) ((uint8_t)(((uint16_t)(i))>>8))



////////////////////////////////////////////////////////////////////////////////
/// @name	Types
///
/// Data structures and types used throughout the libraries and applications. 0 = default
/// bit 0: Altitude is stored               0: Absolute,	1: Relative
/// bit 1: Chnage Alt between WP            0: Gradually,	1: ASAP
/// bit 2: Direction of loiter command      0: Clockwise	1: Counter-Clockwise
/// bit 3: Req.to hit WP.alt to continue    0: No,          1: Yes
/// bit 4: Relative to Home					0: No,          1: Yes
/// bit 5:
/// bit 6:
/// bit 7: Move to next Command             0: YES,         1: Loiter until commanded

struct PACKED Location_Option_Flags {
    uint8_t relative_alt : 1;           // 1 if altitude is relateive to home
    uint8_t unused1      : 1;           // unused flag (defined so that loiter_ccw uses the correct bit)
    uint8_t loiter_ccw   : 1;           // 0 if clockwise, 1 if counter clockwise
    uint8_t terrain_alt  : 1;           // this altitude is above terrain
};

struct PACKED Location {
    union {
        Location_Option_Flags flags;                    ///< options bitmask (1<<0 = relative altitude)
        uint8_t options;                                /// allows writing all flags to eeprom as one byte
    };
    // by making alt 24 bit we can make p1 in a command 16 bit,
    // allowing an accurate angle in centi-degrees. This keeps the
    // storage cost per mission item at 15 bytes, and allows mission
    // altitudes of up to +/- 83km
    int32_t alt:24;                                     ///< param 2 - Altitude in centimeters (meters * 100)
    int32_t lat;                                        ///< param 3 - Lattitude * 10**7
    int32_t lng;                                        ///< param 4 - Longitude * 10**7
};

#ifndef M_PI_F
 #define M_PI_F 3.141592653589793f
#endif
#ifndef PI
 # define PI M_PI_F
#endif
#ifndef M_PI_2
 # define M_PI_2 1.570796326794897f
#endif
//Single precision conversions
#define DEG_TO_RAD 0.017453292519943295769236907684886f
#define RAD_TO_DEG 57.295779513082320876798154814105f

//GPS Specific double precision conversions
//The precision here does matter when using the wsg* functions for converting
//between LLH and ECEF coordinates. Test code in examlpes/location/location.pde
#if HAL_CPU_CLASS >= HAL_CPU_CLASS_75
	#define DEG_TO_RAD_DOUBLE 0.0174532925199432954743716805978692718781530857086181640625  // equals to (M_PI / 180.0)
	#define RAD_TO_DEG_DOUBLE 57.29577951308232286464772187173366546630859375               // equals to (180.0 / M_PI)
#endif

#define RadiansToCentiDegrees(x) ((x) * 5729.5779513082320876798154814105f)

// acceleration due to gravity in m/s/s
#define GRAVITY_MSS 9.80665f

// radius of earth in meters
#define RADIUS_OF_EARTH 6378100

#define ROTATION_COMBINATION_SUPPORT 0

// convert a longitude or latitude point to meters or centimeteres.
// Note: this does not include the longitude scaling which is dependent upon location
#define LATLON_TO_M  0.01113195f
#define LATLON_TO_CM 1.113195f

// Semi-major axis of the Earth, in meters.
#define WGS84_A 6378137.0
//Inverse flattening of the Earth
#define WGS84_IF 298.257223563
// The flattening of the Earth
#define WGS84_F (1/WGS84_IF)
// Semi-minor axis of the Earth in meters
#define WGS84_B (WGS84_A*(1-WGS84_F))
// Eccentricity of the Earth
#define WGS84_E (sqrt(2*WGS84_F - WGS84_F*WGS84_F))


// a varient of asin() that always gives a valid answer.
float           safe_asin(float v);

// a varient of sqrt() that always gives a valid answer.
float           safe_sqrt(float v);

// a faster varient of atan.  accurate to 6 decimal places for values between -1 ~ 1 but then diverges quickly
float           fast_atan(float v);

// fast_atan2 - faster version of atan2
//      126 us on AVR cpu vs 199 for regular atan2
//      absolute error is < 0.005 radians or 0.28 degrees
//      origin source: https://gist.github.com/volkansalma/2972237/raw/
float           fast_atan2(float y, float x);

#if ROTATION_COMBINATION_SUPPORT
// find a rotation that is the combination of two other
// rotations. This is used to allow us to add an overall board
// rotation to an existing rotation of a sensor such as the compass
enum Rotation           rotation_combination(enum Rotation r1, enum Rotation r2, bool *found = NULL);
#endif

// longitude_scale - returns the scaler to compensate for shrinking longitude as you move north or south from the equator
// Note: this does not include the scaling to convert longitude/latitude points to meters or centimeters
float                   longitude_scale(const struct Location &loc);

// return distance in meters between two locations
float                   get_distance(const struct Location &loc1, const struct Location &loc2);

// return distance in centimeters between two locations
uint32_t                get_distance_cm(const struct Location &loc1, const struct Location &loc2);

// return bearing in centi-degrees between two locations
int32_t                 get_bearing_cd(const struct Location &loc1, const struct Location &loc2);

// see if location is past a line perpendicular to
// the line between point1 and point2. If point1 is
// our previous waypoint and point2 is our target waypoint
// then this function returns true if we have flown past
// the target waypoint
bool        location_passed_point(const struct Location & location,
                                  const struct Location & point1,
                                  const struct Location & point2);

//  extrapolate latitude/longitude given bearing and distance
void        location_update(struct Location &loc, float bearing, float distance);

// extrapolate latitude/longitude given distances north and east
void        location_offset(struct Location &loc, float ofs_north, float ofs_east);

/*
  return the distance in meters in North/East plane as a N/E vector
  from loc1 to loc2
 */
Vector2f location_diff(const struct Location &loc1, const struct Location &loc2);

/*
  wrap an angle in centi-degrees
 */
int32_t wrap_360_cd(int32_t error);
int32_t wrap_180_cd(int32_t error);
float wrap_360_cd_float(float angle);
float wrap_180_cd_float(float angle);

/*
  wrap an angle defined in radians to -PI ~ PI (equivalent to +- 180 degrees)
 */
float wrap_PI(float angle_in_radians);

/*
  print a int32_t lat/long in decimal degrees
 */
void print_latlon(AP_HAL::BetterStream *s, int32_t lat_or_lon);

#if HAL_CPU_CLASS >= HAL_CPU_CLASS_75
// Converts from WGS84 geodetic coordinates (lat, lon, height)
// into WGS84 Earth Centered, Earth Fixed (ECEF) coordinates
// (X, Y, Z)
void wgsllh2ecef(const Vector3d &llh, Vector3d &ecef);

// Converts from WGS84 Earth Centered, Earth Fixed (ECEF) 
// coordinates (X, Y, Z), into WHS84 geodetic 
// coordinates (lat, lon, height)
void wgsecef2llh(const Vector3d &ecef, Vector3d &llh);
#endif

// constrain a value
float   constrain_float(float amt, float low, float high);
int16_t constrain_int16(int16_t amt, int16_t low, int16_t high);
int32_t constrain_int32(int32_t amt, int32_t low, int32_t high);

// degrees -> radians
float radians(float deg);

// radians -> degrees
float degrees(float rad);

// square
float sq(float v);

// sqrt of sum of squares
float pythagorous2(float a, float b);
float pythagorous3(float a, float b, float c);

#ifdef radians
#error "Build is including Arduino base headers"
#endif

/* The following three functions used to be arduino core macros */
#define max(a,b) ((a)>(b)?(a):(b))
#define min(a,b) ((a)<(b)?(a):(b))


#endif // AP_MATH_H

