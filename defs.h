#ifndef RENDER_DEFS
#include <math.h>
#include <stdlib.h>
#define RENDER_DEFS
#define W 256
#define H 144
#define RATIO (W/(float)H)
#define LEN (W*H)
#define PLEN (3*W*H)
#define TAU 6.283185307179586

typedef struct vec3_t {
	double x, y, z;
} vec3;

#define TAG_SPHERE 1

typedef struct thing_t {
	int tag;
	int mat;
	union {
		struct {
			vec3 center;
			double radius; 
		} sphere;
	} data;
} thing;

vec3 sub3(const vec3 a, const vec3 b) {
	vec3 res = {a.x-b.x,a.y-b.y,a.z-b.z};
	return res;
}

vec3 add3(const vec3 a, const vec3 b) {
	vec3 res = {a.x+b.x,a.y+b.y,a.z+b.z};
	return res;
}

vec3 cmul(vec3 a, vec3 b) {
	vec3 res = {a.x*b.x,a.y*b.y,a.z*b.z};
	return res;
}

double dot(const vec3 a, const vec3 b) {
	return a.x*b.x+a.y*b.y+a.z*b.z;
}

double length(const vec3 a) {
	return sqrt(a.x*a.x + a.y*a.y + a.z*a.z);
}
double square(const vec3 a) {
	return a.x*a.x + a.y*a.y + a.z*a.z;
}

vec3 scale(const vec3 a, const double b) {
	vec3 res = {a.x*b,a.y*b,a.z*b};
	return res;
}

double normalize(vec3 *a) {
	double len = sqrt(a->x*a->x + a->y*a->y + a->z*a->z);
	a->x = a->x/len;
	a->y = a->y/len;
	a->z = a->z/len;
	return len;
}

vec3 normalized(vec3 a) {
	double len = sqrt(a.x*a.x + a.y*a.y + a.z*a.z);
	a.x /= len;
	a.y /= len;
	a.z /= len;
	return a;
}
double rdouble() {
	return ((double)rand())/RAND_MAX;
}
/*
vec3 rand_dir() {
	double lon = rdouble()*TAU;
	double z = rdouble()*2.0-1.0;
	vec3 res = {z*cos(lon),z*sin(lon),z};
	return res;
}*/

vec3 rand_dir() {
	vec3 res = {rdouble()*2.0-1.0,rdouble()*2.0-1.0,rdouble()*2.0-1.0};
	do {
		res.x = rdouble()*2.0-1.0;
		res.y = rdouble()*2.0-1.0;
		res.z = rdouble()*2.0-1.0;
	} while(square(res) > 1.0);
	return normalized(res);
}

vec3 fib_dir(int i, int n) {
	float lon = TAU*fmod(i/(0.5*(1.0+sqrt(5.0))),1.0);
	float z = i/n;
	float phi = acos(1.0-2.0*((double)i/n));
	vec3 res = {cos(lon)*sin(phi),sin(lon)*sin(phi),cos(phi)};
	return normalized(res);
}

vec3 jitter_dir(double x, double y) {
	double phi = (x+rdouble())*TAU;
	double theta = 2.0*acos(sqrt(1.0-(y+rdouble())));
	vec3 res = {sin(theta)*cos(phi),sin(theta)*sin(phi),cos(theta)};
	return res;
}

vec3 even_dir(int i, int n) {
	float lon = TAU*fmod(i/(0.5*(1.0+sqrt(5.0))),1.0);
	//float z = i/n;
	float phi = acos(1.0-2.0*((double)i/n));
	//float sin_lat = sqrt(1.0-z*z);
	vec3 res = {cos(lon)*sin(phi),sin(lon)*sin(phi),cos(phi)};
	return normalized(res);
}

vec3 even_hemi(int i, int n, vec3 nrm) {
	vec3 q = even_dir(i,n);
	if(dot(q,nrm) < 0) return scale(q,-1.0);
	return q;
}

vec3 rand_hemi(vec3 nrm) {
	vec3 q = rand_dir();
	if(dot(q,nrm) < 0) return scale(q,-1.0);
	return q;
}

#endif