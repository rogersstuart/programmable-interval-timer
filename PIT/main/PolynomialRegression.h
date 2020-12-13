/*
 * Poly.h
 *
 * Created: 6/4/2016 1:04:42 AM
 *  Author: Stuart
 */ 

#ifndef POLY_H_
#define POLY_H_

#include <LinkedList/LinkedList.h>

#define DEGREE (3)
#define px (DEGREE + 1)
#define rs (2 * px - 1)

//Polynomial result;

class PolynomialRegression
{
    private:
        long n = 0;
        float m[px][px + 1];
        float mpc[rs];
        float mpcClone[rs];
        float mClone[px][px+1];
        LinkedList<float> vals = LinkedList<float>();
    public:
        void clear_p();
        void addPoint( float x,  float y);
        LinkedList<float>* getBestFit();
        float fx( float x);
        float getY(float x);
        float getY(float x, float * nomials);
};

#endif /* POLY_H_ */