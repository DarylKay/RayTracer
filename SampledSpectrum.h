#ifndef SAMPLEDSPECTRUM_H
#define SAMPLEDSPECTRUM_H


class SampledSpectrum : public SuperSpectrum<nSpectralSamples> {
public:
    SampledSpectrum(float v = 0) : SuperSpectrum(v) {}
    SampledSpectrum(const SuperSpectrum<nSpectralSamples> &v)
            : SuperSpectrum<nSpectralSamples>(v) {}

    static SampledSpectrum FromSampled(const vector<pair<float, float>> &lambdaValPairs) {
        sort(lambdaValPairs.begin(), lambdaValPairs.end());

        SampledSpectrum r;
        int n = lambdaValPairs.size();

        for(int i = 0; i < nSpectralSamples; i++) {
            float lambda0;
            float lambda1;

            r.c[i] = AverageSpectrumSamples(lambdaValPairs, lambda0, lambda1);
        }

        return r;
    }

    static float AverageSpectrumSamples(const vector<pair<float,float>> &lambdaValPairs, float lambdaStart, float lambdaEnd) {

        return 0;
    }

    static SampledSpectrum X,Y,Z;
};

inline SampledSpectrum Lerp(float t, const SampledSpectrum &s1, const SampledSpectrum &s2) {
    return (1 - t) * s1 + t * s2;
}

#endif //SAMPLEDSPECTRUM_H
