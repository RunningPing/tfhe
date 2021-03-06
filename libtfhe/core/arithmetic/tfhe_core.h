#ifndef TFHE_CORE_H
#define TFHE_CORE_H

///@file
///@brief This file declares only the structures names
#include <stdint.h>

void die_dramatically(const char *message);

#define PREVENT_STACK_COPY(TYPENAME) \
    TYPENAME(const TYPENAME&) = delete; \
    void operator=(const TYPENAME &)= delete; \
    ~TYPENAME() = delete

template<typename TORUS>
class TLweParams;

template<typename TORUS>
class TLwePublicParams;

template<typename TORUS>
struct TLweKey;

template<typename TORUS>
struct TLwePublicKey;

template<typename TORUS>
struct TLweSample;

template<typename TORUS>
struct TLweKeySwitchKey;

template<typename TORUS>
struct TRLweParams;

template<typename TORUS>
struct TRLweKey;

template<typename TORUS>
struct TRLweSample;

template<typename TORUS>
struct TRLweSampleFFT;

template<typename TORUS>
struct TRGswParams;

template<typename TORUS>
struct TRGswKey;

template<typename TORUS>
struct TRGswSample;

template<typename TORUS>
struct TRGswSampleFFT;

// *** ()
template<typename TORUS>
struct TLweBootstrappingKey;

template<typename TORUS>
struct TLweBootstrappingKeyFFT;

template<typename INT_TYPE>
struct IntPolynomial;

template<typename TORUS>
struct TorusPolynomial;

class BigInt;
class BigTorus;


struct LagrangeHalfCPolynomial;

// **** Platinum
//struct SemiTGswParams;
//struct SemiTGswKey;
//struct SemiTGswSample;

#endif //TFHE_CORE_H
