/*
 * Poly.h
 *
 * Created: 6/4/2016 1:04:42 AM
 *  Author: Stuart
 */ 


#include <LinkedList/LinkedList.h>

#ifndef POLY_H_
#define POLY_H_


//Polynomial result;

void clear_p();
void addPoint( float x,  float y);
LinkedList<float>* getBestFit();
float fx( float x);
float getY(float x);
float getY(float x, float * nomials);

#endif /* POLY_H_ */