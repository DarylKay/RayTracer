#ifndef SUPERSPECTRUM_H
#define SUPERSPECTRUM_H


static const int sampledLambdaStart = 400;
static const int sampledLambaEnd = 700;
static const int nSpectralSamples = 60;
static const int nCIESamples = 471;
static const int nRGB2SpectSamples = 32;
static const double CIE_Y_Integral = 106.856895;
#include "CIE_Values.h"

template<int nSpectrumSamples> class SuperSpectrum {
public:
    SuperSpectrum(double v = 0) {
        for(int i = 0; i < nSpectrumSamples; i++) {
            c[i] = v;
        }
    }

    SuperSpectrum &operator+=(const SuperSpectrum &s2){
        for (int i = 0; i < nSpectrumSamples; i++) {
            c[i] += s2.c[i];
        }
        return *this;
    }

    SuperSpectrum operator+(const SuperSpectrum &s2) const{
        SuperSpectrum out = *this;
        for(int i = 0; i < nSpectrumSamples; i++) {
            out.c[i] += s2.c[i];
        }
        return out;
    }

    SuperSpectrum operator*(const SuperSpectrum &s2) const{
        SuperSpectrum out = *this;
        for (int i = 0; i < nSpectrumSamples; i++) {
            out.c[i] *= s2.c[i];
        }
        return out;
    }

    SuperSpectrum operator*(double a) const{
        SuperSpectrum out = *this;
        for (int i = 0; i < nSpectrumSamples; i++) {
            out.c[i] *= a;
        }
        return out;
    }

    SuperSpectrum &operator*=(const SuperSpectrum &s2){
        for(int i = 0; i < nSpectrumSamples; i++) {
            c[i] *= s2.c[i];
        }
        return *this;
    }

    SuperSpectrum &operator*=(double a){
        for (int i = 0; i < nSpectrumSamples; i++) {
            c[i] *= a;
        }
        return *this;
    }

    friend inline SuperSpectrum operator*(double a, const SuperSpectrum &s){
        return s * a;
    }

    SuperSpectrum Sqrt(const SuperSpectrum &s) {
        SuperSpectrum out;
        for (int i = 0; i < nSpectrumSamples; i++) {
            out.c[i] = sqrt(s.c[i]);
        }
        return out;
    }

    //subtraction
    //division
    //negative

    SuperSpectrum Clamp(double min = 0, double max = infinity) const {
        SuperSpectrum out;
        for(int i = 0; i < nSpectrumSamples; i++) {
            out.c[i] = clamp(c[i], min, max);
        }
        return out;
    }

    bool IsBlack() const {
        for (int i = 0; i < nSpectrumSamples; i++) {
            if (c[i] != 0) {
                return false;
            }
        }
        return true;
    }

    double &operator[](int i) {
        return c[i];
    }

    double operator[](int i) const {
        return c[i];
    }

    static const int nSamples = nSpectrumSamples;

protected:
    double c[nSpectrumSamples];
};

#endif //SUPERSPECTRUM_H
