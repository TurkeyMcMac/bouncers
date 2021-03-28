#ifndef BOUNCERS_NETWORK_H_
#define BOUNCERS_NETWORK_H_

#include "scalar.hpp"

namespace bouncers {

template <int N_IN, int N_MID, int N_OUT> struct Network {
    scalar mid_weights[N_MID][N_IN];
    scalar out_weights[N_OUT][N_MID];

    void compute(const scalar in[N_IN], scalar out[N_OUT])
    {
        scalar mid_compute[N_MID];
        for (int i = 0; i < N_MID; ++i) {
            mid_compute[i] = 0;
            for (int j = 0; j < N_IN; ++j) {
                mid_compute[i] += this->mid_weights[i][j] * in[j];
            }
        }
        for (int i = 0; i < N_OUT; ++i) {
            out[i] = 0;
            for (int j = 0; j < N_MID; ++j) {
                out[i] += this->out_weights[i][j] * in[j];
            }
        }
    }
};

} /* namespace bouncers */

#endif /* BOUNCERS_NETWORK_H_ */
