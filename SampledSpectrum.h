#ifndef SAMPLEDSPECTRUM_H
#define SAMPLEDSPECTRUM_H

#include <algorithm>

inline void XYZToRGB (const XYZ xyz, RGB& rgb);
inline void RGBToXYZ (const RGB rgb, XYZ& xyz);

enum class SpectrumType {Reflectance, Illuminant};

class SampledSpectrum : public SuperSpectrum<nSpectralSamples> {
public:
    SampledSpectrum(double v = 0) : SuperSpectrum(v) {}
    SampledSpectrum(const SuperSpectrum<nSpectralSamples> &v)
            : SuperSpectrum<nSpectralSamples>(v) {}

    static SampledSpectrum FromRGB(RGB rgb, SpectrumType type);

    static void Init() {
        vector<pair<double, double>> CIEXVect;
        vector<pair<double, double>> CIEYVect;
        vector<pair<double, double>> CIEZVect;

        vector<pair<double, double>> CIErgbRefl2SpectWhite;
        vector<pair<double, double>> CIErgbRefl2SpectCyan;
        vector<pair<double, double>> CIErgbRefl2SpectMagenta;
        vector<pair<double, double>> CIErgbRefl2SpectYellow;
        vector<pair<double, double>> CIErgbRefl2SpectRed;
        vector<pair<double, double>> CIErgbRefl2SpectGreen;
        vector<pair<double, double>> CIErgbRefl2SpectBlue;

        vector<pair<double, double>> CIErgbIllum2SpectWhite;
        vector<pair<double, double>> CIErgbIllum2SpectCyan;
        vector<pair<double, double>> CIErgbIllum2SpectMagenta;
        vector<pair<double, double>> CIErgbIllum2SpectYellow;
        vector<pair<double, double>> CIErgbIllum2SpectRed;
        vector<pair<double, double>> CIErgbIllum2SpectGreen;
        vector<pair<double, double>> CIErgbIllum2SpectBlue;


        for (int i = 0; i < nCIESamples; i++) {
            CIEXVect.emplace_back(CIE_lambda[i], CIE_X[i]);
            CIEYVect.emplace_back(CIE_lambda[i], CIE_Y[i]);
            CIEZVect.emplace_back(CIE_lambda[i], CIE_Z[i]);
        }

        for (int i =0; i < nRGB2SpectSamples; i++) {
            CIErgbRefl2SpectWhite.emplace_back(RGB2SpectLambda[i], RGBRefl2SpectWhite[i]);
            CIErgbRefl2SpectCyan.emplace_back(RGB2SpectLambda[i], RGBRefl2SpectCyan[i]);
            CIErgbRefl2SpectMagenta.emplace_back(RGB2SpectLambda[i], RGBRefl2SpectMagenta[i]);
            CIErgbRefl2SpectYellow.emplace_back(RGB2SpectLambda[i], RGBRefl2SpectYellow[i]);
            CIErgbRefl2SpectRed.emplace_back(RGB2SpectLambda[i], RGBRefl2SpectRed[i]);
            CIErgbRefl2SpectGreen.emplace_back(RGB2SpectLambda[i], RGBRefl2SpectGreen[i]);
            CIErgbRefl2SpectBlue.emplace_back(RGB2SpectLambda[i], RGBRefl2SpectBlue[i]);

            CIErgbIllum2SpectWhite.emplace_back(RGB2SpectLambda[i], RGBIllum2SpectWhite[i]);
            CIErgbIllum2SpectCyan.emplace_back(RGB2SpectLambda[i], RGBIllum2SpectCyan[i]);
            CIErgbIllum2SpectMagenta.emplace_back(RGB2SpectLambda[i], RGBIllum2SpectMagenta[i]);
            CIErgbIllum2SpectYellow.emplace_back(RGB2SpectLambda[i], RGBIllum2SpectYellow[i]);
            CIErgbIllum2SpectRed.emplace_back(RGB2SpectLambda[i], RGBIllum2SpectRed[i]);
            CIErgbIllum2SpectGreen.emplace_back(RGB2SpectLambda[i], RGBIllum2SpectGreen[i]);
            CIErgbIllum2SpectBlue.emplace_back(RGB2SpectLambda[i], RGBIllum2SpectBlue[i]);
        }

        for (int i = 0; i < nSpectralSamples; i++) {
            double wl0 = Lerp(double(i) / double(nSpectralSamples), sampledLambdaStart, sampledLambaEnd);
            double wl1 = Lerp(double(i + 1) / double(nSpectralSamples), sampledLambdaStart, sampledLambaEnd);

            X.c[i] = AverageSpectrumSamples(CIEXVect, wl0, wl1);
            Y.c[i] = AverageSpectrumSamples(CIEYVect, wl0, wl1);
            Z.c[i] = AverageSpectrumSamples(CIEZVect, wl0, wl1);

            rgbRefl2SpectWhite.c[i] = AverageSpectrumSamples(CIErgbRefl2SpectWhite, wl0, wl1);
            rgbRefl2SpectCyan.c[i] = AverageSpectrumSamples(CIErgbRefl2SpectCyan, wl0, wl1);
            rgbRefl2SpectMagenta.c[i] = AverageSpectrumSamples(CIErgbRefl2SpectMagenta, wl0, wl1);
            rgbRefl2SpectYellow.c[i] = AverageSpectrumSamples(CIErgbRefl2SpectYellow, wl0, wl1);
            rgbRefl2SpectRed.c[i] = AverageSpectrumSamples(CIErgbRefl2SpectRed, wl0, wl1);
            rgbRefl2SpectGreen.c[i] = AverageSpectrumSamples(CIErgbRefl2SpectGreen, wl0, wl1);
            rgbRefl2SpectBlue.c[i] = AverageSpectrumSamples(CIErgbRefl2SpectBlue, wl0, wl1);

            rgbIllum2SpectWhite.c[i] = AverageSpectrumSamples(CIErgbIllum2SpectWhite, wl0, wl1);
            rgbIllum2SpectCyan.c[i] = AverageSpectrumSamples(CIErgbIllum2SpectCyan, wl0, wl1);
            rgbIllum2SpectMagenta.c[i] = AverageSpectrumSamples(CIErgbIllum2SpectMagenta, wl0, wl1);
            rgbIllum2SpectYellow.c[i] = AverageSpectrumSamples(CIErgbIllum2SpectYellow, wl0, wl1);
            rgbIllum2SpectRed.c[i] = AverageSpectrumSamples(CIErgbIllum2SpectRed, wl0, wl1);
            rgbIllum2SpectGreen.c[i] = AverageSpectrumSamples(CIErgbIllum2SpectGreen, wl0, wl1);
            rgbIllum2SpectBlue.c[i] = AverageSpectrumSamples(CIErgbIllum2SpectBlue, wl0, wl1);
        }
    }

    static SampledSpectrum FromSampled(vector<pair<double, double>> lambdaValPairs) {
        sort(lambdaValPairs.begin(), lambdaValPairs.end());

        SampledSpectrum r;
        int n = lambdaValPairs.size();

        for(int i = 0; i < nSpectralSamples; i++) {
            double lambda0 = Lerp(double(i) / double(nSpectralSamples), sampledLambdaStart, sampledLambaEnd);
            double lambda1 = Lerp(double(i + 1) / double(nSpectralSamples), sampledLambdaStart, sampledLambaEnd);

            r.c[i] = AverageSpectrumSamples(lambdaValPairs, lambda0, lambda1);
        }

        return r;
    }

    static double AverageSpectrumSamples(const vector<pair<double,double>> &lambdaValPairs, double lambdaStart, double lambdaEnd) {
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

        double sum = 0;
        //Contributions of constant segments before/after the samples taken
        if(lambdaStart < lambdaValPairs[0].first) {
            sum += lambdaValPairs[0].second * (lambdaValPairs[0].first - lambdaStart);
        }
        if(lambdaEnd > lambdaValPairs[n-1].first) {
            sum += lambdaValPairs[n-1].second * (lambdaEnd - lambdaValPairs[n-1].first);
        }

        int i = 0;
        while (lambdaStart > lambdaValPairs[i + 1].first) ++i;

        for (; i+1 < n && lambdaEnd >= lambdaValPairs[i].first; i++) {
            double segLambdaStart = max(lambdaStart, lambdaValPairs[i].first);
            double segLambdaEnd = min(lambdaEnd, lambdaValPairs[i + 1].first);
            double first = Lerp((segLambdaStart - lambdaValPairs[i].first) / (lambdaValPairs[i+1].first - lambdaValPairs[i].first),
                 lambdaValPairs[i].second, lambdaValPairs[i+1].second);
            double second = Lerp((segLambdaEnd - lambdaValPairs[i].first) / (lambdaValPairs[i+1].first - lambdaValPairs[i].first),
                                lambdaValPairs[i].second, lambdaValPairs[i+1].second);
            sum += 0.5 * (first + second) * (segLambdaEnd - segLambdaStart);
        }

        if (isnan(sum)) {
            sum = 0;
        }
        double val = (sum / (lambdaEnd - lambdaStart));
        return val;
    }

    void toXYZ(XYZ &xyz) {
        xyz = {0,0,0};

        for (int i = 0; i < nSpectralSamples; i++) {
            xyz[0] += c[i] * X.c[i];
            xyz[1] += c[i] * Y.c[i];
            xyz[2] += c[i] * Z.c[i];
        }

        double scale = double(sampledLambaEnd - sampledLambdaStart) / double(CIE_Y_Integral * nSpectralSamples);

        xyz *= scale;
    }

    double luminance() const{
        double L = 0;
        for(int i = 0; i < nSpectralSamples; i++) {
            L += Y.c[i] * c[i];
        }
        return L * double(sampledLambaEnd - sampledLambdaStart) / double(nSpectralSamples);
    }

    void ToRGB(RGB &rgb) {
        XYZ xyz;
        toXYZ(xyz);
        XYZToRGB(xyz, rgb);
    }

public:
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

extern const double CIE_X[nCIESamples];
extern const double CIE_Y[nCIESamples];
extern const double CIE_Z[nCIESamples];
extern const double CIE_lambda[nCIESamples];

extern const double RGB2SpectLambda[nRGB2SpectSamples];
extern const double RGBRefl2SpectWhite[nRGB2SpectSamples];
extern const double RGBRefl2SpectCyan[nRGB2SpectSamples];
extern const double RGBRefl2SpectMagenta[nRGB2SpectSamples];
extern const double RGBRefl2SpectYellow[nRGB2SpectSamples];
extern const double RGBRefl2SpectRed[nRGB2SpectSamples];
extern const double RGBRefl2SpectGreen[nRGB2SpectSamples];
extern const double RGBRefl2SpectBlue[nRGB2SpectSamples];
extern const double RGBIllum2SpectWhite[nRGB2SpectSamples];
extern const double RGBIllum2SpectCyan[nRGB2SpectSamples];
extern const double RGBIllum2SpectMagenta[nRGB2SpectSamples];
extern const double RGBIllum2SpectYellow[nRGB2SpectSamples];
extern const double RGBIllum2SpectRed[nRGB2SpectSamples];
extern const double RGBIllum2SpectGreen[nRGB2SpectSamples];
extern const double RGBIllum2SpectBlue[nRGB2SpectSamples];

inline SampledSpectrum Lerp(double t, const SampledSpectrum &s1, const SampledSpectrum &s2) {
    return (1 - t) * s1 + t * s2;
}

double InterpolateSpectrumSamples(const SampledSpectrum& spectrum, double lambda) {
    if (lambda <= sampledLambdaStart) {
        return spectrum[0];
    }
    if (lambda >= sampledLambaEnd) {
        return spectrum[nSpectralSamples - 1];
    }

    int offset = 0;
    int i = 0;
    for (; i < nSpectralSamples; i++) {
        offset = i * ((sampledLambaEnd - sampledLambdaStart) / nSpectralSamples);
        if (sampledLambdaStart + offset > lambda) {
            offset -= (sampledLambaEnd - sampledLambdaStart) / nSpectralSamples;
            i ++;
            break;
        }
    }
    i -= 2;

    double t = (lambda - (sampledLambdaStart + offset)) / (((sampledLambaEnd - sampledLambdaStart) / nSpectralSamples));

    return Lerp(t, spectrum[i], spectrum[i + 1]);
}

inline void XYZToRGB (const XYZ xyz, RGB &rgb) {
    rgb[0] = 3.240479f * xyz[0] - 1.537150f * xyz[1] - 0.498535f * xyz[2];
    rgb[1] = -0.969256f * xyz[0] + 1.875991f * xyz[1] + 0.041556f * xyz[2];
    rgb[2] = 0.055648f * xyz[0] - 0.204043f * xyz[1] + 1.057311f * xyz[2];
}

inline void RGBToXYZ (const RGB rgb, XYZ &xyz) {
    xyz[0] = (XYZ(0.412453, 0.357580, 0.180423) * rgb).sum();
    xyz[1] = (XYZ(0.212671, 0.715160, 0.072169) * rgb).sum();
    xyz[2] = (XYZ(0.019334, 0.119193, 0.950227) * rgb).sum();
}

SampledSpectrum SampledSpectrum::FromRGB(RGB rgb, SpectrumType type = SpectrumType::Reflectance) {
    SampledSpectrum r;

    double R = rgb[0];
    double G = rgb[1];
    double B = rgb[2];

    if (type == SpectrumType::Reflectance) {
        if (R <= G && R <= B) {
            r += R * rgbRefl2SpectWhite;
            if (G <= B) {
                r += (G - R) * rgbRefl2SpectCyan;
                r += (B - G) * rgbRefl2SpectBlue;
            } else {
                r += (B - R) * rgbRefl2SpectCyan;
                r += (G - B) * rgbRefl2SpectGreen;
            }
        } else if (G <= R && G <= B) {
            r += G * rgbRefl2SpectWhite;
            if (R <= B) {
                r += (R - G) * rgbRefl2SpectMagenta;
                r += (B - R) * rgbRefl2SpectBlue;
            } else {
                r += (B - G) * rgbRefl2SpectMagenta;
                r += (R - B) * rgbRefl2SpectRed;
            }

        } else {
            r += B * rgbRefl2SpectWhite;
            if (R <= G) {
                r += (R - B) * rgbRefl2SpectYellow;
                r += (G - R) * rgbRefl2SpectGreen;
            } else {
                r += (G - B) * rgbRefl2SpectYellow;
                r += (R - G) * rgbRefl2SpectRed;
            }

        }
        r *= .94;
    } else {
        if (R <= G && R <= B) {
            r += R * rgbIllum2SpectWhite;
            if (G <= B) {
                r += (G - R) * rgbIllum2SpectCyan;
                r += (B - G) * rgbIllum2SpectBlue;
            } else {
                r += (B - R) * rgbIllum2SpectCyan;
                r += (G - B) * rgbIllum2SpectGreen;
            }

        } else if (G <= R && G <= B) {
           r += G * rgbIllum2SpectWhite;
            if (R <= B) {
                r += (R - G) * rgbIllum2SpectMagenta;
                r += (B - R) * rgbIllum2SpectBlue;
            } else {
                r += (B - G) * rgbIllum2SpectMagenta;
                r += (R - B) * rgbIllum2SpectRed;
            }

        } else {
            r += B * rgbIllum2SpectWhite;
            if (R <= G) {
                r += (R - B) * rgbIllum2SpectYellow;
                r += (G - R) * rgbIllum2SpectGreen;
            } else {
                r += (G - B) * rgbIllum2SpectYellow;
                r += (R - G) * rgbIllum2SpectRed;
            }
        }
        r *= .86445;
    }

    return r.Clamp();
}

#endif //SAMPLEDSPECTRUM_H
