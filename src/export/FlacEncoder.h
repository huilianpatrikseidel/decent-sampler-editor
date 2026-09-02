#pragma once

#include <QByteArray>
#include <QString>

// A small, self-contained FLAC encoder for the bundle exporter.
//
// It covers exactly what the exporter needs — integer PCM WAV in, a complete FLAC
// stream out, bit-exact — using fixed predictors with Rice-coded residuals and stereo
// decorrelation. Those are the same tools the reference encoder uses at its lower
// presets, so the output is ordinary FLAC that any decoder reads; it simply does not
// search LPC coefficients, which costs a few percent of ratio and keeps this small
// enough to vendor instead of pulling in libFLAC.
namespace FlacEncoder {

// True when the file is integer PCM this encoder can reproduce losslessly: 8/16/24-bit,
// 1-8 channels, sample rate below 2^20. Float and 32-bit WAVs return false so the caller
// stores them unchanged rather than silently degrading them.
//
// Reads only the WAV header, and is deterministic. The transpiler (which writes sample
// paths into the preset XML) and the exporter (which writes the zip entries) both call
// it, and they must always reach the same answer for a given file or the preset would
// point at a name the bundle does not contain.
bool canEncode(const QString& wavPath);

// Encodes wavPath into a complete FLAC stream. Returns false and leaves `out` untouched
// if the file cannot be read or decoded.
bool encodeFile(const QString& wavPath, QByteArray& out);

} // namespace FlacEncoder
