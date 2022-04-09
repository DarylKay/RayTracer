#ifndef SAMPLEDSPECTRUM_H
#define SAMPLEDSPECTRUM_H

inline void XYZToRGB (const XYZ xyz, RGB rgb);
inline void RGBToXYZ (const RGB rgb, XYZ xyz);


class SampledSpectrum : public SuperSpectrum<nSpectralSamples> {
public:
    SampledSpectrum(float v = 0) : SuperSpectrum(v) {}
    SampledSpectrum(const SuperSpectrum<nSpectralSamples> &v)
            : SuperSpectrum<nSpectralSamples>(v) {}

    static void Init() {
        vector<pair<float, float>> CIEXVect;
        vector<pair<float, float>> CIEYVect;
        vector<pair<float, float>> CIEZVect;

        for (int i = 0; i < nCIESamples; i++) {
            CIEXVect.emplace_back(CIE_lambda[i], CIE_X[i]);
            CIEYVect.emplace_back(CIE_lambda[i], CIE_Y[i]);
            CIEZVect.emplace_back(CIE_lambda[i], CIE_Z[i]);
        }

        for (int i = 0; i < nSpectralSamples; i++) {
            float wl0 = Lerp(float(i) / float(nSpectralSamples), sampledLambdaStart, sampledLambaEnd);
            float wl1 = Lerp(float(i + 1) / float(nSpectralSamples), sampledLambdaStart, sampledLambaEnd);

            X.c[i] = AverageSpectrumSamples(CIEXVect, wl0, wl1);
            Y.c[i] = AverageSpectrumSamples(CIEYVect, wl0, wl1);
            Z.c[i] = AverageSpectrumSamples(CIEZVect, wl0, wl1);
        }
    }

    SampledSpectrum FromSampled(vector<pair<float, float>> lambdaValPairs) {
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

        //Handles endge cases
        if (lambdaEnd <= lambdaValPairs[0].first) {
            return lambdaValPairs[0].second;
        }
        if (lambdaStart >= lambdaValPairs[n-1].first) {
            return lambdaValPairs[n-1].second;
        }
        if (n == 1) {
            return lambdaValPairs[0].second;
        }

        float sum = 0;
        //Contributions of constant segments before/after the samples taken
        if(lambdaStart < lambdaValPairs[0].first) {
            sum += lambdaValPairs[0].second * (lambdaValPairs[0].first - lambdaStart);
        }
        if(lambdaEnd > lambdaValPairs[n-1].first) {
            sum += lambdaValPairs[n-1].second * (lambdaEnd - lambdaValPairs[n-1].first);
        }

        int i = 0;
        while (lambdaStart > lambdaValPairs[i + 1].first){
            ++i;
        }

        for (; i+1 < n && lambdaEnd >= lambdaValPairs[i].first; i++) {
            float segLambdaStart = max(lambdaStart, lambdaValPairs[i].first);
            float segLambdaEnd = min(lambdaEnd, lambdaValPairs[i + 1].first);
            sum += 0.5 * (Lerp((segLambdaStart - lambdaValPairs[i].first) / (lambdaValPairs[i+1].first - lambdaValPairs[i].first),
                               lambdaValPairs[i].second, lambdaValPairs[i+1].second)
                        + Lerp((segLambdaEnd - lambdaValPairs[i].first) / (lambdaValPairs[i+1].first - lambdaValPairs[i].first),
                               lambdaValPairs[i].second, lambdaValPairs[i+1].second))
                    * (segLambdaEnd - segLambdaStart);
        }

        return sum / (lambdaEnd - lambdaStart);
    }

    void toXYZ(XYZ xyz) {
        xyz = {0,0,0};

        for (int i = 0; i < nSpectralSamples; i++) {
            xyz[0] += c[i] * X.c[i];
            xyz[1] += c[i] * Y.c[i];
            xyz[2] += c[i] * Z.c[i];
        }

        float scale = float(sampledLambaEnd - sampledLambdaStart) / float(CIE_Y_Integral * nSpectralSamples);

        xyz *= scale;
    }

    float luminance() const{
        float L = 0;
        for(int i = 0; i < nSpectralSamples; i++) {
            L += Y.c[i] * c[i];
        }
        return L * float(sampledLambaEnd - sampledLambdaStart) / float(nSpectralSamples);
    }

    void ToRGB(RGB rgb) {
        XYZ xyz;
        toXYZ(xyz);
        XYZToRGB(xyz, rgb);
    }

private:
    static SampledSpectrum X,Y,Z;

    static SampledSpectrum rgbRefl2SpectWhite;
    static SampledSpectrum rgbRefl2SpectCyan;
    static SampledSpectrum rgbRefl2SpectMagenta;
    static SampledSpectrum rgbRefl2SpectYellow;
    static SampledSpectrum rgbRefl2SpectRed;
    static SampledSpectrum rgbRefl2SpectGreen;
    static SampledSpectrum rgbRefl2SpectBlue;

    static SampledSpectrum rgbIllum2SpectWhite;
    static SampledSpectrum rgbIllum2SpectCyan;
    static SampledSpectrum rgbIllum2SpectMagenta;
    static SampledSpectrum rgbIllum2SpectYellow;
    static SampledSpectrum rgbIllum2SpectRed;
    static SampledSpectrum rgbIllum2SpectGreen;
    static SampledSpectrum rgbIllum2SpectBlue;
};

SampledSpectrum SampledSpectrum::X;
SampledSpectrum SampledSpectrum::Y;
SampledSpectrum SampledSpectrum::Z;
SampledSpectrum SampledSpectrum::rgbRefl2SpectWhite;
SampledSpectrum SampledSpectrum::rgbRefl2SpectCyan;
SampledSpectrum SampledSpectrum::rgbRefl2SpectMagenta;
SampledSpectrum SampledSpectrum::rgbRefl2SpectYellow;
SampledSpectrum SampledSpectrum::rgbRefl2SpectRed;
SampledSpectrum SampledSpectrum::rgbRefl2SpectGreen;
SampledSpectrum SampledSpectrum::rgbRefl2SpectBlue;
SampledSpectrum SampledSpectrum::rgbIllum2SpectWhite;
SampledSpectrum SampledSpectrum::rgbIllum2SpectCyan;
SampledSpectrum SampledSpectrum::rgbIllum2SpectMagenta;
SampledSpectrum SampledSpectrum::rgbIllum2SpectYellow;
SampledSpectrum SampledSpectrum::rgbIllum2SpectRed;
SampledSpectrum SampledSpectrum::rgbIllum2SpectGreen;
SampledSpectrum SampledSpectrum::rgbIllum2SpectBlue;

inline SampledSpectrum Lerp(float t, const SampledSpectrum &s1, const SampledSpectrum &s2) {
    return (1 - t) * s1 + t * s2;
}

inline void XYZToRGB (const XYZ xyz, RGB rgb) {
    rgb[0] = (RGB( 3.240479,-1.537150,-0.498535) * xyz).sum();
    rgb[1] = (RGB(-0.969256, 1.875991, 0.041556) * xyz).sum();
    rgb[2] = (RGB( 0.055648,-0.204043, 1.057311) * xyz).sum();
}

inline void RGBToXYZ (const RGB rgb, XYZ xyz) {
    xyz[0] = (XYZ(0.412453, 0.357580, 0.180423) * rgb).sum();
    xyz[1] = (XYZ(0.212671, 0.715160, 0.072169) * rgb).sum();
    xyz[2] = (XYZ(0.019334, 0.119193, 0.950227) * rgb).sum();
}

#endif //SAMPLEDSPECTRUM_H
