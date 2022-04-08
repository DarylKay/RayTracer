#ifndef SUPERSPECTRUM_H
#define SUPERSPECTRUM_H

static const int sampledLambdaStart = 400;
static const int sampledLambaEnd = 700;
static const int nSpectralSamples = 60;


template<int nSpectrumSamples> class SuperSpectrum {
public:
    SuperSpectrum(float v = 0) {
        for(int i = 0; i < nSpectrumSamples; i++) {
            c[i] = v;
        }
    }

    SuperSpectrum &operator+=(const SuperSpectrum &s2) const{
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

    SuperSpectrum operator*(float a) const{
        SuperSpectrum out = *this;
        for (int i = 0; i < nSpectrumSamples; i++) {
            out.c[i] *= a;
        }
        return out;
    }

    SuperSpectrum &operator*=(const SuperSpectrum &s2) const{
        for(int i = 0; i < nSpectrumSamples; i++) {
            c[i] *= s2.c[i];
        }
        return *this;
    }

    SuperSpectrum &operator*=(float a) const{
        for (int i = 0; i < nSpectrumSamples; i++) {
            c[i] *= a;
        }
        return *this;
    }

    friend inline SuperSpectrum operator*(float a, const SuperSpectrum &s){
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

    SuperSpectrum Clamp(float min = 0, float max = infinity) const {
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

    float &operator[](int i) {
        return c[i];
    }

    float c[nSpectrumSamples];
    static const int nSamples = nSpectrumSamples;
};

void SortSpectrumSamples(float *lambda, float *vals, int n) {
    vector<pair<float, float>> sortVec;
    sortVec.reserve;

}


#endif //SUPERSPECTRUM_H
