#pragma once
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

enum class FilterType {
    Peak = 0,
    LowShelf,
    HighShelf,
    LowPass,
    HighPass,
    BandPass,
    Notch
};

class BiquadFilter {
public:
    BiquadFilter() { reset(); }

    void reset() {
        x1 = x2 = y1 = y2 = 0.0;
    }

    void calculateCoefficients(FilterType type, double sampleRate, double frequency, double q, double gainDb) {
        if (sampleRate <= 0.0) return;
        
        // Clamp frequency to prevent Nyquist issues
        frequency = std::fmin(std::fmax(frequency, 10.0), sampleRate * 0.49);
        q = std::fmax(q, 0.1);
        gainDb = std::fmin(std::fmax(gainDb, -40.0), 40.0);

        double w0 = 2.0 * M_PI * frequency / sampleRate;
        double alpha = std::sin(w0) / (2.0 * q);
        double A = std::pow(10.0, gainDb / 40.0);
        double cosw0 = std::cos(w0);
        double a0 = 1.0;

        switch (type) {
            case FilterType::Peak:
                b0 = 1.0 + alpha * A;
                b1 = -2.0 * cosw0;
                b2 = 1.0 - alpha * A;
                a0 = 1.0 + alpha / A;
                a1 = -2.0 * cosw0;
                a2 = 1.0 - alpha / A;
                break;
            case FilterType::LowShelf: {
                double beta = std::sqrt(A) / q;
                b0 = A * ((A + 1.0) - (A - 1.0) * cosw0 + beta * std::sin(w0));
                b1 = 2.0 * A * ((A - 1.0) - (A + 1.0) * cosw0);
                b2 = A * ((A + 1.0) - (A - 1.0) * cosw0 - beta * std::sin(w0));
                a0 = (A + 1.0) + (A - 1.0) * cosw0 + beta * std::sin(w0);
                a1 = -2.0 * ((A - 1.0) + (A + 1.0) * cosw0);
                a2 = (A + 1.0) + (A - 1.0) * cosw0 - beta * std::sin(w0);
                break;
            }
            case FilterType::HighShelf: {
                double beta = std::sqrt(A) / q;
                b0 = A * ((A + 1.0) + (A - 1.0) * cosw0 + beta * std::sin(w0));
                b1 = -2.0 * A * ((A - 1.0) + (A + 1.0) * cosw0);
                b2 = A * ((A + 1.0) + (A - 1.0) * cosw0 - beta * std::sin(w0));
                a0 = (A + 1.0) - (A - 1.0) * cosw0 + beta * std::sin(w0);
                a1 = 2.0 * ((A - 1.0) - (A + 1.0) * cosw0);
                a2 = (A + 1.0) - (A - 1.0) * cosw0 - beta * std::sin(w0);
                break;
            }
            case FilterType::LowPass:
                b0 = (1.0 - cosw0) / 2.0;
                b1 = 1.0 - cosw0;
                b2 = (1.0 - cosw0) / 2.0;
                a0 = 1.0 + alpha;
                a1 = -2.0 * cosw0;
                a2 = 1.0 - alpha;
                break;
            case FilterType::HighPass:
                b0 = (1.0 + cosw0) / 2.0;
                b1 = -(1.0 + cosw0);
                b2 = (1.0 + cosw0) / 2.0;
                a0 = 1.0 + alpha;
                a1 = -2.0 * cosw0;
                a2 = 1.0 - alpha;
                break;
            case FilterType::BandPass:
                b0 = alpha;
                b1 = 0.0;
                b2 = -alpha;
                a0 = 1.0 + alpha;
                a1 = -2.0 * cosw0;
                a2 = 1.0 - alpha;
                break;
            case FilterType::Notch:
                b0 = 1.0;
                b1 = -2.0 * cosw0;
                b2 = 1.0;
                a0 = 1.0 + alpha;
                a1 = -2.0 * cosw0;
                a2 = 1.0 - alpha;
                break;
        }

        if (a0 != 0.0) {
            b0 /= a0;
            b1 /= a0;
            b2 /= a0;
            a1 /= a0;
            a2 /= a0;
        }
    }

    double process(double x) {
        double y = b0 * x + b1 * x1 + b2 * x2 - a1 * y1 - a2 * y2;
        
        // Denormal protection
        if (std::abs(y) < 1.0e-15) y = 0.0;

        x2 = x1;
        x1 = x;
        y2 = y1;
        y1 = y;
        return y;
    }
    
    // Evaluate magnitude response at a given frequency
    double evaluateMagnitude(double frequency, double sampleRate) const {
        if (sampleRate <= 0.0) return 1.0;
        double w = 2.0 * M_PI * frequency / sampleRate;
        double cosw = std::cos(w);
        double cos2w = std::cos(2.0 * w);
        
        double num = b0*b0 + b1*b1 + b2*b2 + 2.0*(b0*b1 + b1*b2)*cosw + 2.0*b0*b2*cos2w;
        double den = 1.0 + a1*a1 + a2*a2 + 2.0*(a1 + a1*a2)*cosw + 2.0*a2*cos2w;
        
        if (den == 0.0) return 0.0;
        return std::sqrt(num / den);
    }

private:
    double b0 = 1.0, b1 = 0.0, b2 = 0.0;
    double a1 = 0.0, a2 = 0.0;
    double x1 = 0.0, x2 = 0.0;
    double y1 = 0.0, y2 = 0.0;
};
