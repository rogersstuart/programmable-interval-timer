#ifndef LINEAR_REGRESSION_H
#define LINEAR_REGRESSION_H

#include <vector>

namespace PIT{

    class LinearRegression{
        
        private:
        public:
                static void linearRegression(float* x, float* y, std::vector<float>& lrCoef, uint8_t num);
    };

}

#endif //LINEAR_REGRESSION_H