#ifndef LINEAR_REGRESSION_H
#define LINEAR_REGRESSION_H

using PIT{

    class LinReg{
        private:
                inline static float lrCoef[2] = {0, 0};
        public:
                static void linearRegression(float* x, float* y, float* lrCoef, uint8_t num);
    };

}

#endif //LINEAR_REGRESSION_H