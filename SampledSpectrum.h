#ifndef SAMPLEDSPECTRUM_H
#define SAMPLEDSPECTRUM_H


class SampledSpectrum : public SuperSpectrum<nSpectralSamples> {
public:
    SampledSpectrum(float v = 0) : SuperSpectrum(v) {}
    SampledSpectrum(const SuperSpectrum<nSpectralSamples> &v)
            : SuperSpectrum<nSpectralSamples>(v) {}

    SampledSpectrum FromSampled(const vector<pair<float, float>> &lambdaValPairs) {
        sort(lambdaValPairs.begin(), lambdaValPairs.end());

        SampledSpectrum r;
        int n = lambdaValPairs.size();

        for(int i = 0; i < nSpectralSamples; i++) {
            float lambda0 = Lerp(float(i) / float(nSpectralSamples), sampledLambdaStart, sampledLambaEnd);
            float lambda1 = Lerp(float(i + 1) / float(nSpectralSamples), sampledLambdaStart, sampledLambaEnd);

            r.c[i] = AverageSpectrumSamples(lambdaValPairs, lambda0, lambda1);
        }

        return r;
    }

    static float AverageSpectrumSamples(const vector<pair<float,float>> &lambdaValPairs, float lambdaStart, float lambdaEnd) {
        int n = lambdaValPairs.size();

        if (lambdaEnd <= lambdaValPairs[0].first) {
            return lambdaValPairs[0].second;
        }
        if (lambdaStart >= lambdaValPairs[n-1].first) {
            return lambdaValPairs[n - 1].second;
        }
        if (n == 1) {
            return lambdaValPairs[0].second;
        }

        float sum = 0;


        return 0;
    }

    static SampledSpectrum X,Y,Z;
};

inline SampledSpectrum Lerp(float t, const SampledSpectrum &s1, const SampledSpectrum &s2) {
    return (1 - t) * s1 + t * s2;
}

#endif //SAMPLEDSPECTRUM_H
