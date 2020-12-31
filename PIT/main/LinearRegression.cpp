#include <Arduino.h>
#include "LinearRegression.h"

namespace PIT{

    void LinearRegression::linearRegression(float* x, float* y, std::array<float, 2> lrCoef, uint8_t num){

        // pass x and y arrays (pointers), lrCoef pointer, and n.  The lrCoef array is comprised of the slope=lrCoef[0] and intercept=lrCoef[1]. n is length of the x and y arrays.

        // initialize variables
        float xbar=0.0;
        float ybar=0.0;
        float xybar=0.0;
        float xsqbar=0.0;

        // calculations required for linear regression
        for (int i=0; i<num; i++)
        {
            xbar=xbar+x[i];
            ybar=ybar+y[i];
            xybar=xybar+x[i]*y[i];
            xsqbar=xsqbar+x[i]*x[i];
        }
        
        xbar/=num;
        ybar/=num;
        xybar/=num;
        xsqbar/=num;

        // simple linear regression algorithm
        lrCoef[0]=(xybar-xbar*ybar)/(xsqbar-xbar*xbar);
        lrCoef[1]= (ybar-lrCoef[0]*xbar);
    }
}