#ifndef LINEAR_REGRESSION_H
#define LINEAR_REGRESSION_H

#include <array>

namespace PIT{

    class LinearRegression{
        
        private:
                //inline static float lrCoef[2] = {0, 0};
        public:
                static void linearRegression(float* x, float* y, std::array<float, 2> lrCoef, uint8_t num);
    };

}

#endif //LINEAR_REGRESSION_H