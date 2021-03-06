#include "polynomial_torus.h"
#include "random_gen.h"

#include <cassert>

using namespace std;

/**
 * Instantiate TorusPolynomial class for available torus types
 */
EXPLICIT_INSTANTIATE_ALL_PRIMITIVE_TORUS(TorusPolynomial);

// TorusPolynomial = random
template<typename TORUS>
void TorusPolynomial<TORUS>::Uniform(
        TorusPolynomial<TORUS> *result,
        const PolynomialParams<TORUS> *params,
        TfheThreadContext *context,
        Allocator alloc) {
    const int32_t N = params->N;
    TORUS *x = result->coefs;

    for (int32_t i = 0; i < N; ++i)
        x[i] = RandomGenTorus<TORUS>::uniform();
}

// TorusPolynomial + p*TorusPolynomial
template<typename TORUS>
void TorusPolynomial<TORUS>::AddMulZ(
        TorusPolynomial<TORUS> *result,
        const TorusPolynomial<TORUS> *poly1,
        const INT_TYPE *p,
        const TorusPolynomial<TORUS> *poly2,
        const PolynomialParams<TORUS> *params,
        TfheThreadContext *context,
        Allocator alloc) {
    const int32_t N = params->N;
    assert(result != poly1); //if it fails here, please use AddMulZTo
    TORUS *r = result->coefs;
    const TORUS *a = poly1->coefs;
    const TORUS *b = poly2->coefs;

    for (int32_t i = 0; i < N; ++i)
        r[i] = a[i] + *p * b[i];
}

// TorusPolynomial += p*TorusPolynomial
template<typename TORUS>
void TorusPolynomial<TORUS>::AddMulZTo(
        TorusPolynomial<TORUS> *result,
        const INT_TYPE *p,
        const TorusPolynomial<TORUS> *poly2,
        const PolynomialParams<TORUS> *params,
        TfheThreadContext *context,
        Allocator alloc) {
    const int32_t N = params->N;
    TORUS *r = result->coefs;
    const TORUS *b = poly2->coefs;

    for (int32_t i = 0; i < N; ++i)
        r[i] += *p * b[i];
}

// TorusPolynomial - p*TorusPolynomial
template<typename TORUS>
void TorusPolynomial<TORUS>::SubMulZ(
        TorusPolynomial<TORUS> *result,
        const TorusPolynomial<TORUS> *poly1,
        const INT_TYPE *p,
        const TorusPolynomial<TORUS> *poly2,
        const PolynomialParams<TORUS> *params,
        TfheThreadContext *context,
        Allocator alloc) {
    const int32_t N = params->N;
    assert(result != poly1); //if it fails here, please use SubMulZTo
    TORUS *r = result->coefs;
    const TORUS *a = poly1->coefs;
    const TORUS *b = poly2->coefs;

    for (int32_t i = 0; i < N; ++i)
        r[i] = a[i] - *p * b[i];
}

// TorusPolynomial -= p*TorusPolynomial
template<typename TORUS>
void TorusPolynomial<TORUS>::SubMulZTo(
        TorusPolynomial<TORUS> *result,
        const INT_TYPE *p,
        const TorusPolynomial<TORUS> *poly2,
        const PolynomialParams<TORUS> *params,
        TfheThreadContext *context,
        Allocator alloc) {
    const int32_t N = params->N;
    TORUS *r = result->coefs;
    const TORUS *b = poly2->coefs;

    for (int32_t i = 0; i < N; ++i)
        r[i] -= *p * b[i];
}


// Infinity norm of the distance between two TorusPolynomial
template<typename TORUS>
double TorusPolynomial<TORUS>::NormInftyDist(
        const TorusPolynomial<TORUS> *poly1,
        const TorusPolynomial<TORUS> *poly2,
        const PolynomialParams<TORUS> *params,
        TfheThreadContext *context,
        Allocator alloc) {
    const int32_t N = params->N;
    double norm = 0;
    const ZModuleParams<TORUS> *const zparams =
            params->zmodule_params;

    // Max between the coefficients of abs(poly1-poly2)
    for (int32_t i = 0; i < N; ++i) {
        double r = TorusUtils<TORUS>::normInftyDist(poly1->coefs[i], poly2->coefs[i], zparams);
        if (r > norm) { norm = r; }
    }
    return norm;
}


template<typename TORUS>
void TorusPolynomial<TORUS>::MultNaive_plain_aux(
        TORUS *__restrict result,
        const INT_TYPE *__restrict poly1,
        const TORUS *__restrict poly2,
        const int32_t N,
        const ZModuleParams<TORUS> *const zparams,
        TfheThreadContext *context,
        Allocator alloc) {
    const int32_t _2Nm1 = 2 * N - 1;
    TORUS ri;

    for (int32_t i = 0; i < N; i++) {
        ri = 0;
        for (int32_t j = 0; j <= i; j++) {
            ri += poly1[j] * poly2[i - j];
        }
        result[i] = ri;
    }

    for (int32_t i = N; i < _2Nm1; i++) {
        ri = 0;
        for (int32_t j = i - N + 1; j < N; j++) {
            ri += poly1[j] * poly2[i - j];
        }
        result[i] = ri;
    }
}

template<typename TORUS>
void TorusPolynomial<TORUS>::MultNaive_aux(
        TORUS *__restrict result,
        const INT_TYPE *__restrict poly1,
        const TORUS *__restrict poly2,
        const int32_t N,
        const ZModuleParams<TORUS> *const zparams,
        TfheThreadContext *context,
        Allocator alloc) {
    TORUS ri;

    for (int32_t i = 0; i < N; i++) {
        ri = 0;
        for (int32_t j = 0; j <= i; j++) {
            ri += poly1[j] * poly2[i - j];
        }
        for (int32_t j = i + 1; j < N; j++) {
            ri -= poly1[j] * poly2[N + i - j];
        }
        result[i] = ri;
    }
}

/**
 * This is the naive external multiplication of an integer polynomial
 * with a torus polynomial. (this function should yield exactly the same
 * result as the karatsuba or fft version)
 */
template<typename TORUS>
void TorusPolynomial<TORUS>::MultNaive(
        TorusPolynomial<TORUS> *result,
        const IntPolynomial<TORUS> *poly1,
        const TorusPolynomial<TORUS> *poly2,
        const PolynomialParams<TORUS> *params,
        TfheThreadContext *context,
        Allocator alloc) {
    assert(result != poly2);

    const int32_t N = params->N;
    const ZModuleParams<TORUS> *const zparams =
            params->zmodule_params;

    TorusPolynomial<TORUS>::MultNaive_aux(result->coefs, poly1->coefs,
                                          poly2->coefs, N, zparams, context, alloc);
}

/**
 * This function multiplies 2 polynomials (an integer poly and a torus poly) by using Karatsuba
 * The karatsuba function is torusPolynomialMultKaratsuba: it takes in input two polynomials and multiplies them
 * To do that, it uses the auxiliary function Karatsuba_aux, which is recursive ad which works with
 * the vectors containing the coefficients of the polynomials (primitive types)
 */

// A and B of size = size
// R of size = 2*size-1
template<typename TORUS>
void TorusPolynomial<TORUS>::Karatsuba_aux(
        TORUS *R,
        const INT_TYPE *A,
        const TORUS *B,
        const int32_t size,
        const char *buf,
        const ZModuleParams<TORUS> *const zparams,
        TfheThreadContext *context,
        Allocator alloc) {
    const int32_t h = size / 2;
    const int32_t sm1 = size - 1;

    //we stop the karatsuba recursion at h=4, because on my machine,
    //it seems to be optimal
    if (h <= 4) {
        TorusPolynomial<TORUS>::MultNaive_plain_aux(R, A, B, size, zparams, context, alloc);
        return;
    }

    //we split the polynomials in 2
    INT_TYPE *Atemp = (INT_TYPE *) buf;
    buf += h * sizeof(INT_TYPE);
    TORUS *Btemp = (TORUS *) buf;
    buf += h * sizeof(TORUS);
    TORUS *Rtemp = (TORUS *) buf;
    buf += size * sizeof(TORUS);
    //Note: in the above line, I have put size instead of sm1 so that buf remains aligned on a power of 2

    for (int32_t i = 0; i < h; ++i)
        Atemp[i] = A[i] + A[h + i];
    for (int32_t i = 0; i < h; ++i)
        Btemp[i] = B[i] + B[h + i];

    // Karatsuba recursivly
    Karatsuba_aux(R, A, B, h, buf, zparams, context, alloc); // (R[0],R[2*h-2]), (A[0],A[h-1]), (B[0],B[h-1])
    Karatsuba_aux(R + size, A + h, B + h, h, buf, zparams, context,
                  alloc); // (R[2*h],R[4*h-2]), (A[h],A[2*h-1]), (B[h],B[2*h-1])
    Karatsuba_aux(Rtemp, Atemp, Btemp, h, buf, zparams, context, alloc);
    R[sm1] = 0; //this one needs to be copy manually
    for (int32_t i = 0; i < sm1; ++i)
        Rtemp[i] -= R[i] + R[size + i];
    for (int32_t i = 0; i < sm1; ++i)
        R[h + i] += Rtemp[i];

}

// poly1, poly2 and result are polynomials mod X^N+1
template<typename TORUS>
void TorusPolynomial<TORUS>::MultKaratsuba(
        TorusPolynomial<TORUS> *result,
        const IntPolynomial<TORUS> *poly1,
        const TorusPolynomial<TORUS> *poly2,
        const PolynomialParams<TORUS> *params,
        TfheThreadContext *context,
        Allocator alloc) {
    const int32_t N = params->N;
    TORUS *R = new TORUS[2 * N - 1];
    char *buf = new char[4 * N *
                         sizeof(TORUS)]; //that's large enough to store every tmp variables (2*2*N*4) TODO: see if there is unused memory (before generic torus byte cnt was 16*N)
    const ZModuleParams<TORUS> *const zparams =
            params->zmodule_params;

    // Karatsuba
    Karatsuba_aux(R, poly1->coefs, poly2->coefs, N, buf, zparams, context, alloc);

    // reduction mod X^N+1
    for (int32_t i = 0; i < N - 1; ++i)
        result->coefs[i] = R[i] - R[N + i];
    result->coefs[N - 1] = R[N - 1];

    delete[] buf;
    delete[] R;
}

// poly1, poly2 and result are polynomials mod X^N+1
template<typename TORUS>
void TorusPolynomial<TORUS>::AddMulRKaratsuba(
        TorusPolynomial<TORUS> *result,
        const IntPolynomial<TORUS> *poly1,
        const TorusPolynomial<TORUS> *poly2,
        const PolynomialParams<TORUS> *params,
        TfheThreadContext *context,
        Allocator alloc) {
    const int32_t N = params->N;
    TORUS *R = new TORUS[2 * N - 1];
    char *buf = new char[16 * N]; //that's large enough to store every tmp variables (2*2*N*4)
    const ZModuleParams<TORUS> *const zparams =
            params->zmodule_params;

    // Karatsuba
    Karatsuba_aux(R, poly1->coefs, poly2->coefs, N, buf, zparams, context, alloc);

    // reduction mod X^N+1
    for (int32_t i = 0; i < N - 1; ++i)
        result->coefs[i] += R[i] - R[N + i];
    result->coefs[N - 1] += R[N - 1];

    delete[] R;
    delete[] buf;
}

// poly1, poly2 and result are polynomials mod X^N+1
template<typename TORUS>
void TorusPolynomial<TORUS>::SubMulRKaratsuba(
        TorusPolynomial<TORUS> *result,
        const IntPolynomial<TORUS> *poly1,
        const TorusPolynomial<TORUS> *poly2,
        const PolynomialParams<TORUS> *params,
        TfheThreadContext *context,
        Allocator alloc) {
    const int32_t N = params->N;
    TORUS *R = new TORUS[2 * N - 1];
    char *buf = new char[16 * N]; //that's large enough to store every tmp variables (2*2*N*4)
    const ZModuleParams<TORUS> *const zparams =
            params->zmodule_params;

    // Karatsuba
    Karatsuba_aux(R, poly1->coefs, poly2->coefs, N, buf, zparams, context, alloc);

    // reduction mod X^N+1
    for (int32_t i = 0; i < N - 1; ++i)
        result->coefs[i] -= R[i] - R[N + i];
    result->coefs[N - 1] -= R[N - 1];

    delete[] R;
    delete[] buf;
}
