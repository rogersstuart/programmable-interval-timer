#include <Arduino.h>
#include <Poly.h>
#include <LinkedList/LinkedList.h>
	
#define DEGREE (3)
#define px (DEGREE + 1)
#define rs (2 * px - 1)

	long n = 0;

	float m[px][px + 1];

	float mpc[rs];

    float mpcClone[rs];

    float mClone[px][px+1];

     LinkedList<float> vals = LinkedList<float>();

	/**
	 * Add a point to the set of points that the polynomial must be fit to
	 * 
	 * @param x
	 *            The x coordinate of the point
	 * @param y
	 *            The y coordinate of the point


	 */
    float fastpow (float x, int m)
    {
        return pow(x, m);
        
        unsigned int n = m < 0 ? -m : m;
        float y = n % 2 ? x : 1;
        while (n >>= 1)
        {
            x = x * x;
            if (n % 2)
            y = y * x;
        }
        return m < 0 ? 1/y : y;
    }

    uint64_t maybefastpow_ull (float x, int m)
    {
        uint64_t result = 0;
        
        if(m >= 2)
            result = round(x*x);
        else
            if(m == 1)
                return x;
            else
                return 0;

        m-=2;

        for(uint8_t exp_ctr = 0; exp_ctr < m; exp_ctr++)
            result += round(result * x);

        return result;
    }

    float maybefastpow_f (float x, int m)
    {
        float result;
        
        if(x == 0.0)
            return 0.0;
        else
            if(m == 0)
                return 1.0;
            else
                if(m == 1)
                    return x;
                else
                {
                    result = x * x;
                    for(uint8_t exp_ctr = 2; exp_ctr < m; exp_ctr++)
                        result *= x;
                    return result;
                }
    }

    void clear_p()
    {
        memset((void*)&m, 0, sizeof(float)*px*(px+1));
        memset((void*)&mClone, 0, sizeof(float)*px*(px+1));
        memset((void*)&mpcClone, 0, sizeof(float)*rs);
        memset((void*)&mpc, 0, sizeof(float)*rs);
        
        vals.clear();

        n = 0;
    }

	void addPoint( float x,  float y)
    {
		//assert !float.isInfinite(x) && !float.isNaN(x);
		//assert !float.isInfinite(y) && !float.isNaN(y);
		n++;
		// process precalculation array
		for (int r = 1; r < rs; r++)
        {
			float f = maybefastpow_f(x ,r);
            //if(isnan(f) || isinf(f) || f > 4294967040.0 || f < -4294967040.0)
            //    mpc[r] += maybefastpow_uul(x, r);
            //else
                mpc[r] += f;
            
            //Serial.println(mpc[r]);
        }

		// process RH column cells
		m[0][px] += y;

		for (int r = 1; r < px; r++)
        {
			float f = maybefastpow_f(x, r) * y;

            //if(isnan(f) || isinf(f) || f > 4294967040.0 || f < -4294967040.0)
            //    m[r][px] += maybefastpow_uul(x, r) * y < 1 && y > 0.0 ? 1 : 0;
            //else
                m[r][px] += f;

        //Serial.println(String(m[r][px]));
        }
	}

    void gj_divide(float A[px][px + 1], int i, int j, int m)
    {
        for (int q = j + 1; q < m; q++)
        {
            //Serial.println(String(A[i][q], 8));
            //Serial.println(">");
            
            A[i][q] /= A[i][j];

            //Serial.println(String(A[i][q], 8));
        }

        A[i][j] = 1;
    }

    void gj_eliminate( float A[px][px + 1],  int i,  int j,  int n,  int m)
    {
        for (int k = 0; k < n; k++)
            if (k != i && A[k][j] != 0)
            {
                for (int q = j + 1; q < m; q++)
                {
                    //Serial.print(String(A[k][q], 8));
                    //Serial.print(">");

                    A[k][q] -= A[k][j] * A[i][q];

                    //Serial.println(String(A[k][q], 8));
                }   

                A[k][j] = 0;
                /*
                for(int xx = 0; xx < px; xx++)
                {
                    for(int yy = 0; yy < px+1; yy++)
                    {
                        Serial.print(String(A[xx][yy], 8));
                        Serial.print(" ");
                    }

                    Serial.println("");
                }
                */
            }
    }

    void gj_swap( float A[px][px + 1],  int i,  int j)
    {
        float temp[px+1];

        //for(int gg = 0; gg < px + 1; gg++)
         //   Serial.print(String(A[i][gg]) + " ");
            
        //Serial.println();

        //for(int gg = 0; gg < px + 1; gg++)
        //Serial.print(String(A[j][gg]) + " ");

        //Serial.println();

        //for(int gg = 0; gg < px + 1; gg++)
         //   temp[gg] = A[i][gg];

        memcpy((uint8_t*)&temp, (uint8_t*)&A[i], sizeof(float)*(px+1));

        //for(int gg = 0; gg < px + 1; gg++)
        //    A[i][gg] = A[j][gg];

            memcpy((uint8_t*)&A[i], (uint8_t*)&A[j], sizeof(float)*(px+1));

        //for(int gg = 0; gg < px + 1; gg++)
          //  A[j][gg] = temp[gg];

            memcpy((uint8_t*)&A[j], (uint8_t*)&temp, sizeof(float)*(px+1));

            //for(int gg = 0; gg < px + 1; gg++)
            //Serial.print(String(A[i][gg]) + " ");
            //
            //Serial.println();

            //for(int gg = 0; gg < px + 1; gg++)
            //Serial.print(String(A[j][gg]) + " ");

            //Serial.println();
    }

    void gj_echelonize(float A[px][px + 1])
    {
        int n = px;
        int m = px + 1;
        int i = 0;
        int j = 0;

        while (i < n && j < m)
        {
            // look for a non-zero entry in col j at or below row i
            int k = i;
            while (k < n && A[k][j] == 0)
            k++;

            // if such an entry is found at row k
            if (k < n)
            {
                // if k is not i, then swap row i with row k
                if (k != i)
                gj_swap(A, i, j);

                // if A[i][j] is not 1, then divide row i by A[i][j]
                if (A[i][j] != 1)
                {
                gj_divide(A, i, j, m);
                //Serial.println(String(i) + " " + String(j) + " " + String(m));
                }

                // eliminate all other non-zero entries from col j by
                // subtracting from each
                // row (other than i) an appropriate multiple of row i
                gj_eliminate(A, i, j, n, m);

                i++;
            }
            j++;
        }

        
    }

	/**
	 * Returns a polynomial that seeks to minimize the square of the total
	 * distance between the set of points and the polynomial.
	 * 
	 * @return A polynomial
	 */
	LinkedList<float>* getBestFit()
    {
		// = mpc.clone();
        memcpy((uint8_t*)&mpcClone, (uint8_t*)&mpc, sizeof(float)*rs);

		for (uint8_t x = 0; x < px; x++)
            memcpy((uint8_t*)&mClone + x, (uint8_t*)&m + x, sizeof(float) * (px + 1));

		mpcClone[0] += n;
		// populate square matrix section
		for (uint8_t r = 0; r < px; r++)
			for (uint8_t c = 0; c < px; c++)
            {
				mClone[r][c] = mpcClone[r + c];
                //Serial.print(String(mClone[r][c], 8));
            }

		gj_echelonize(mClone);

		for (uint8_t j = 0; j < px; j++)
        {
			vals.add(j, mClone[j][px]);
            //Serial.print(mClone[j][px], 8);
            //Serial.print(" ");
            }
            //Serial.println("");
        /*
        Serial.print(String(vals.get(0), 8));
        Serial.print(" ");
        Serial.print(String(vals.get(1), 8));
        Serial.print(" ");
        Serial.print(String(vals.get(2), 8));
        Serial.print(" ");
        Serial.print(String(vals.get(3), 8));
        Serial.print(" ");
        Serial.println();
        */

		return &vals;
	}

    

    

    
	/*
    float getY(float x)
    {
        float ret = 0;

        for (int p = vals.size() - 1; p >= 0; p--)
        {
            ret = vals.get(p) + (x * ret);
        }

        return ret;
    }
    */

    float getY(float x)
    {
        //float ret = vals.get(0) - vals.get(1) * x + vals.get(2) * maybefastpow_f(x, 2) + vals.get(3) * maybefastpow_f(x, 3) + vals.get(4) * maybefastpow_f(x, 4) + vals.get(5) * maybefastpow_f(x, 5);
        //float ret = vals.get(4) * maybefastpow_f(x, 4);
        float ret = vals.get(0) - vals.get(1) * x + vals.get(2) * maybefastpow_f(x, 2) + vals.get(3) * maybefastpow_f(x, 3);

        return ret;

        /*
        for (int p=0; p<vals.size(); p++)
        {
            float poly_part_comp = vals.get(p);
            poly_part_comp *= maybefastpow_f(x, p);
            ret += poly_part_comp;
        }
        return ret;
        */
    }

    float getY(float x, float * nomial_vals)
    {
        //float ret = vals.get(0) - vals.get(1) * x + vals.get(2) * maybefastpow_f(x, 2) + vals.get(3) * maybefastpow_f(x, 3) + vals.get(4) * maybefastpow_f(x, 4) + vals.get(5) * maybefastpow_f(x, 5);
        //float ret = vals.get(4) * maybefastpow_f(x, 4);
        float ret = nomial_vals[0] - nomial_vals[1] * x + nomial_vals[2] * maybefastpow_f(x, 2) + nomial_vals[3] * maybefastpow_f(x, 3);

        return ret;

        /*
        for (int p=0; p<vals.size(); p++)
        {
            float poly_part_comp = vals.get(p);
            poly_part_comp *= maybefastpow_f(x, p);
            ret += poly_part_comp;
        }
        return ret;
        */
    }

    

