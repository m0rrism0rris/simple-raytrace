#include <stdio.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#include "defs.h"

unsigned char img[PLEN];

const int n_objects = 2;
thing world[2];

vec3 intersect;
vec3 nrm;
double dist;
int mat;

const vec3 zero = {0.0,0.0,0.0};
const vec3 white = {0.5,0.5,0.5};

vec3 mat_diff(int depth);

unsigned char conv(double c) {
	c /= (1.0+c);
	if (c>=1.0) return 255;
	if (c<=0.0) return 0;
	return (unsigned char)((c>0.0031308?1.055*pow(c,1.0/2.4)-0.055:12.92*c)*255);
}

int sphere_intersect(vec3 origin, vec3 dir, vec3 center, double r) {
	vec3 oc = sub3(origin,center);
	double a = square(dir);
	double b = dot(oc,dir);
	double c = square(oc) - r*r;
	double d = b*b - a*c;
	if (d < 0) return 0;
	d = sqrt(d);
	double root = (-b - d) / a;
	if (root <= 0) {
		root = (d - b) / a;
		if (root <= 0) return 0;
	}
	dist = root;
	intersect = add3(scale(dir,root),origin);
	nrm = scale(sub3(intersect,center),1.0/r);
	return 1;
}

int thing_intersect(thing t, vec3 origin, vec3 dir) {
	mat = t.mat;
	switch(t.tag) {
		case TAG_SPHERE: return sphere_intersect(origin,dir,t.data.sphere.center,t.data.sphere.radius); break;
		default: return 0;
	}
}

int raycast(vec3 origin, vec3 dir) {
	double mindist = INFINITY;
	vec3 minpos;
	vec3 minnrm;
	int minmat;
	for(int i = 0; i < n_objects; i++) {
		if(thing_intersect(world[i],origin,dir) && dist < mindist && dist >= 0.000000001) {
			mindist = dist;
			minpos = intersect;
			minnrm = nrm;
			minmat = mat;
		}
	}
	if (mindist == INFINITY) return 0;
	intersect = minpos;
	nrm = minnrm;
	mat = minmat;
	return 1;
}

const int maxdepth = 3;

vec3 ray_color(vec3 origin, vec3 dir, int depth) {
	if (depth >= maxdepth) return zero;
	depth++;
	if(raycast(origin,dir)) {
		switch(mat) {
			case 0: return mat_diff(depth);
			case 1:
				vec3 b = scale(nrm,dot(dir,nrm)*2.0);
				return scale(ray_color(intersect,normalized(sub3(dir,b)),depth),0.25);
		}
	}
	//double sky = dir.y >= 0.99 ? 500.0 : 0.0;
	double sky = dir.y > 0.0 ? 1.0 : 0.0;
	vec3 res = {sky,sky,sky};
	return res;
}



vec3 mat_diff(int depth) {
	int samples = 10-depth; //64 >> depth;
	vec3 pos = intersect;
	vec3 normal = nrm;
	vec3 res = zero;
	for (int x = 0; x < samples; x++) for (int y = 0; y < samples; y++) {
		res = add3(res,ray_color(pos,normalized(add3(normal,jitter_dir(((double)x)/samples,((double)y)/samples))),depth));
	}
	//for (int x = 0; x < samples; x++) for (int y = 0; y < samples; y++) {
	//	res = add3(res,ray_color(pos,normalized(add3(normal,fib_dir(x+samples*y,samples*samples))),depth));
	//}
	return scale(res,(1.0/(samples*samples))*0.66);
}

vec3 pix(int x, int y) {
	vec3 dir = {((double)(x-W/2))/(H/2),((double)(H/2-y))/(H/2),-1.0};
	normalize(&dir);
	return ray_color(zero,dir,-1);
}


//TODO: track sample number of pixel along with depth, use for jittering
void render(char const *filename) {
	for (int i = 0; i < PLEN; i++) {
		int z = i%(3*W)/3;
		if (z == 0) printf("row %d \r",i/(3*W));
		vec3 c = pix(z,i/(3*W));
		img[i] = conv(c.x); i++;
		img[i] = conv(c.y); i++;
		img[i] = conv(c.z);
	}
	stbi_write_png(filename,W,H,3,img,3*W);
}

int main() {
	srand(1);
	//materials: 0 lambert, 1 mirror
	const vec3 sph = {0.0,-0.1,-1.0};
	world[0].tag = TAG_SPHERE;
	world[0].mat = 1;
	world[0].data.sphere.center = sph;
	world[0].data.sphere.radius = 0.5;
	const vec3 sph2 = {0.0,-100.6,-1.0};
	world[1].tag = TAG_SPHERE;
	world[1].mat = 0;
	world[1].data.sphere.center = sph2;
	world[1].data.sphere.radius = 100.0;
	render("img.png");
	return 0;
}