#ifndef BOUNCERS_NETWORK_H_
#define BOUNCERS_NETWORK_H_

#include "scalar.hpp"

namespace bouncers {

// A representation of a neural network with one middle layer. The template
// parameters are positive numbers for the sizes of the input, middle, and
// output.
template <int N_IN, int N_MID, int N_OUT> struct Network {
    scalar mid_weights[N_MID][N_IN];
    scalar out_weights[N_OUT][N_MID];

    // Computes the output for the given input.
    void compute(const scalar in[N_IN], scalar out[N_OUT]) const
    {
        scalar mid[N_MID];
        for (int i = 0; i < N_MID; ++i) {
            mid[i] = 0;
            for (int j = 0; j < N_IN; ++j) {
                mid[i] += this->mid_weights[i][j] * in[j];
            }
        }
        for (int i = 0; i < N_OUT; ++i) {
            out[i] = 0;
            for (int j = 0; j < N_MID; ++j) {
                out[i] += this->out_weights[i][j] * mid[j];
            }
        }
    }

    // Executes a function for each weight in the network, possibly modifying
    // the weights.
    template <typename F> void for_each_weight(F& f)
    {
        for (int i = 0; i < N_MID; ++i) {
            for (int j = 0; j < N_IN; ++j) {
                f(this->mid_weights[i][j]);
            }
        }
        for (int i = 0; i < N_OUT; ++i) {
            for (int j = 0; j < N_MID; ++j) {
                f(this->out_weights[i][j]);
            }
        }
    }
};

} /* namespace bouncers */

#endif /* BOUNCERS_NETWORK_H_ */
