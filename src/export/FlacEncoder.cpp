#include "FlacEncoder.h"

#include <QCryptographicHash>
#include <QFile>
#include <QFileInfo>

#include <algorithm>
#include <vector>

#include "miniaudio.h"

namespace {

constexpr int kBlockSize = 4096;
constexpr int kMaxPartitionOrder = 6;
constexpr int kMaxFixedOrder = 4;

// Rice parameters are stored in 4 bits with coding method 0 and 5 bits with method 1.
// The top value of each field is reserved for escape coding, which this encoder does not
// emit — it falls back to a VERBATIM subframe instead, so the parameters stay below it.
constexpr int kMaxParamMethod0 = 14;
constexpr int kMaxParamMethod1 = 30;

// ── Bit writer ───────────────────────────────────────────────────────────────

// Big-endian bit packing, which is what FLAC uses throughout.
class BitWriter {
public:
    void writeBits(quint32 value, int bits) {
        while (bits > 0) {
            const int take = std::min(bits, 8 - m_pending);
            const int shift = bits - take;
            const quint32 chunk = (value >> shift) & ((1u << take) - 1u);
            m_acc = static_cast<quint8>((m_acc << take) | chunk);
            m_pending += take;
            bits -= take;
            if (m_pending == 8) {
                m_data.append(static_cast<char>(m_acc));
                m_acc = 0;
                m_pending = 0;
            }
        }
    }

    // `quotient` zero bits followed by a single one bit.
    void writeUnary(quint32 quotient) {
        while (quotient >= 32) {
            writeBits(0, 32);
            quotient -= 32;
        }
        writeBits(1, static_cast<int>(quotient) + 1);
    }

    void writeRice(quint32 folded, int k) {
        writeUnary(folded >> k);
        if (k > 0) writeBits(folded & ((1u << k) - 1u), k);
    }

    void alignToByte() {
        if (m_pending > 0) writeBits(0, 8 - m_pending);
    }

    void appendRaw(const char* data, int len) { m_data.append(data, len); }

    qint64 bitCount() const { return static_cast<qint64>(m_data.size()) * 8 + m_pending; }

    // Only meaningful while byte-aligned.
    const QByteArray& bytes() const { return m_data; }

private:
    QByteArray m_data;
    quint8 m_acc = 0;
    int m_pending = 0;
};

// ── Checksums ────────────────────────────────────────────────────────────────

quint8 crc8(const char* data, int len) {
    quint8 crc = 0;
    for (int i = 0; i < len; ++i) {
        crc ^= static_cast<quint8>(data[i]);
        for (int bit = 0; bit < 8; ++bit) {
            crc = (crc & 0x80) ? static_cast<quint8>((crc << 1) ^ 0x07)
                               : static_cast<quint8>(crc << 1);
        }
    }
    return crc;
}

quint16 crc16(const char* data, int len) {
    quint16 crc = 0;
    for (int i = 0; i < len; ++i) {
        crc ^= static_cast<quint16>(static_cast<quint8>(data[i]) << 8);
        for (int bit = 0; bit < 8; ++bit) {
            crc = (crc & 0x8000) ? static_cast<quint16>((crc << 1) ^ 0x8005)
                                 : static_cast<quint16>(crc << 1);
        }
    }
    return crc;
}

// ── Residual maths ───────────────────────────────────────────────────────────

// Signed residuals are stored zigzag-folded so small negatives stay small unsigned.
inline quint32 fold(qint32 residual) {
    return (static_cast<quint32>(residual) << 1) ^ static_cast<quint32>(residual >> 31);
}

inline int ilog2(quint64 value) {
    int bits = 0;
    while (value > 0) { ++bits; value >>= 1; }
    return bits;
}

// Fixed polynomial predictors of order 0-4. Residuals go to `out`, which holds n - order
// entries. int64 accumulation keeps the order-4 expansion (up to 16x the sample range)
// from overflowing before it is checked.
void fixedResidual(const qint32* x, int n, int order, qint32* out, quint64& sumAbs) {
    sumAbs = 0;
    for (int i = order; i < n; ++i) {
        qint64 v = x[i];
        switch (order) {
            case 1: v -= x[i - 1]; break;
            case 2: v -= 2LL * x[i - 1] - x[i - 2]; break;
            case 3: v -= 3LL * x[i - 1] - 3LL * x[i - 2] + x[i - 3]; break;
            case 4: v -= 4LL * x[i - 1] - 6LL * x[i - 2] + 4LL * x[i - 3] - x[i - 4]; break;
            default: break; // order 0 predicts zero
        }
        out[i - order] = static_cast<qint32>(v);
        sumAbs += static_cast<quint64>(v < 0 ? -v : v);
    }
}

qint64 partitionBits(const quint32* u, int count, int k) {
    qint64 bits = 0;
    for (int i = 0; i < count; ++i) {
        bits += static_cast<qint64>(u[i] >> k) + 1 + k;
    }
    return bits;
}

// Smallest k where n * 2^k covers the partition's total magnitude, then a one-step
// search either side of it — the usual cheap approximation of the optimal parameter.
int bestRiceParam(const quint32* u, int count, qint64& outBits) {
    quint64 sum = 0;
    for (int i = 0; i < count; ++i) sum += u[i];

    int k = 0;
    while (k < kMaxParamMethod1 && (static_cast<quint64>(count) << k) < sum) ++k;

    int best = k;
    qint64 bestBits = partitionBits(u, count, k);
    for (int candidate : {k - 1, k + 1}) {
        if (candidate < 0 || candidate > kMaxParamMethod1) continue;
        const qint64 bits = partitionBits(u, count, candidate);
        if (bits < bestBits) {
            bestBits = bits;
            best = candidate;
        }
    }
    outBits = bestBits;
    return best;
}

struct RicePlan {
    int partitionOrder = 0;
    int method = 0; // 0 = 4-bit parameters, 1 = 5-bit parameters
    std::vector<int> params;
    qint64 bits = -1; // whole RESIDUAL block, including method and parameter fields
};

// Picks the partition order that encodes these residuals in the fewest bits.
RicePlan planResidual(const quint32* u, int blockSize, int order) {
    RicePlan best;
    for (int p = 0; p <= kMaxPartitionOrder; ++p) {
        const int partitions = 1 << p;
        if (blockSize % partitions != 0) continue;

        const int perPartition = blockSize >> p;
        if (perPartition <= order) continue; // partition 0 loses `order` samples to warmup

        RicePlan plan;
        plan.partitionOrder = p;
        plan.params.resize(partitions);

        qint64 residualBits = 0;
        int maxParam = 0;
        int index = 0;
        for (int j = 0; j < partitions; ++j) {
            const int count = perPartition - (j == 0 ? order : 0);
            qint64 bits = 0;
            const int k = bestRiceParam(u + index, count, bits);
            plan.params[j] = k;
            maxParam = std::max(maxParam, k);
            residualBits += bits;
            index += count;
        }

        plan.method = (maxParam <= kMaxParamMethod0) ? 0 : 1;
        plan.bits = 2 + 4
                  + static_cast<qint64>(partitions) * (plan.method == 0 ? 4 : 5)
                  + residualBits;

        if (best.bits < 0 || plan.bits < best.bits) best = std::move(plan);
    }
    return best;
}

// ── Subframes ────────────────────────────────────────────────────────────────

// Scratch buffers reused across every block so encoding a long file does not churn the
// allocator once per subframe.
struct Scratch {
    std::vector<qint32> residual;
    std::vector<quint32> folded;
};

void writeVerbatim(BitWriter& bw, const qint32* x, int n, int bps) {
    bw.writeBits(0, 1);        // mandatory zero
    bw.writeBits(0b000001, 6); // VERBATIM
    bw.writeBits(0, 1);        // no wasted bits
    const quint32 mask = (bps >= 32) ? 0xFFFFFFFFu : ((1u << bps) - 1u);
    for (int i = 0; i < n; ++i) {
        bw.writeBits(static_cast<quint32>(x[i]) & mask, bps);
    }
}

void encodeSubframe(BitWriter& bw, const qint32* x, int n, int bps, Scratch& scratch) {
    const quint32 mask = (bps >= 32) ? 0xFFFFFFFFu : ((1u << bps) - 1u);

    // CONSTANT: the whole subframe collapses to a single value.
    bool constant = true;
    for (int i = 1; i < n && constant; ++i) constant = (x[i] == x[0]);
    if (constant) {
        bw.writeBits(0, 1);
        bw.writeBits(0b000000, 6); // CONSTANT
        bw.writeBits(0, 1);
        bw.writeBits(static_cast<quint32>(x[0]) & mask, bps);
        return;
    }

    // Pick the fixed predictor whose residuals are smallest overall, the standard cheap
    // proxy for which one will actually encode shortest.
    scratch.residual.resize(n);
    int bestOrder = -1;
    quint64 bestSum = 0;
    for (int order = 0; order <= kMaxFixedOrder; ++order) {
        if (n <= order) break;
        quint64 sumAbs = 0;
        fixedResidual(x, n, order, scratch.residual.data(), sumAbs);
        if (bestOrder < 0 || sumAbs < bestSum) {
            bestSum = sumAbs;
            bestOrder = order;
        }
    }

    RicePlan plan;
    if (bestOrder >= 0) {
        quint64 ignored = 0;
        fixedResidual(x, n, bestOrder, scratch.residual.data(), ignored);

        const int count = n - bestOrder;
        scratch.folded.resize(count);
        for (int i = 0; i < count; ++i) scratch.folded[i] = fold(scratch.residual[i]);

        plan = planResidual(scratch.folded.data(), n, bestOrder);
    }

    // VERBATIM is the safety net: it is always representable, so a block whose residuals
    // resist Rice coding still encodes correctly, just without compression.
    const qint64 verbatimBits = 8 + static_cast<qint64>(n) * bps;
    const qint64 fixedBits = (plan.bits < 0)
        ? -1
        : 8 + static_cast<qint64>(bestOrder) * bps + plan.bits;

    if (fixedBits < 0 || fixedBits >= verbatimBits) {
        writeVerbatim(bw, x, n, bps);
        return;
    }

    bw.writeBits(0, 1);
    bw.writeBits(static_cast<quint32>(0b001000 | bestOrder), 6); // FIXED, order in low bits
    bw.writeBits(0, 1);
    for (int i = 0; i < bestOrder; ++i) {
        bw.writeBits(static_cast<quint32>(x[i]) & mask, bps); // warmup samples
    }

    bw.writeBits(static_cast<quint32>(plan.method), 2);
    bw.writeBits(static_cast<quint32>(plan.partitionOrder), 4);

    const int partitions = 1 << plan.partitionOrder;
    const int perPartition = n >> plan.partitionOrder;
    int index = 0;
    for (int j = 0; j < partitions; ++j) {
        const int count = perPartition - (j == 0 ? bestOrder : 0);
        const int k = plan.params[j];
        bw.writeBits(static_cast<quint32>(k), plan.method == 0 ? 4 : 5);
        for (int i = 0; i < count; ++i) {
            bw.writeRice(scratch.folded[index + i], k);
        }
        index += count;
    }
}

// Rough cost of a channel, used only to choose between the stereo decorrelation modes.
// With Rice coding the bits per residual track log2 of the mean magnitude.
qint64 estimateChannelBits(const qint32* x, int n, std::vector<qint32>& scratch) {
    scratch.resize(n);
    quint64 bestSum = 0;
    bool have = false;
    for (int order = 0; order <= kMaxFixedOrder; ++order) {
        if (n <= order) break;
        quint64 sumAbs = 0;
        fixedResidual(x, n, order, scratch.data(), sumAbs);
        if (!have || sumAbs < bestSum) {
            bestSum = sumAbs;
            have = true;
        }
    }
    if (!have || n == 0) return 0;
    const quint64 mean = bestSum / static_cast<quint64>(n);
    return static_cast<qint64>(n) * (ilog2(mean) + 1);
}

// ── WAV header probing ───────────────────────────────────────────────────────

quint32 readLe32(const char* p) {
    return static_cast<quint32>(static_cast<quint8>(p[0]))
         | (static_cast<quint32>(static_cast<quint8>(p[1])) << 8)
         | (static_cast<quint32>(static_cast<quint8>(p[2])) << 16)
         | (static_cast<quint32>(static_cast<quint8>(p[3])) << 24);
}

quint16 readLe16(const char* p) {
    return static_cast<quint16>(static_cast<quint8>(p[0])
         | (static_cast<quint16>(static_cast<quint8>(p[1])) << 8));
}

// Parses just enough of the RIFF header to classify the file. Deliberately independent
// of miniaudio so canEncode() stays cheap — it must not read whole samples.
bool readWavFormat(const QString& path, int& bitsPerSample, int& channels, int& sampleRate) {
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) return false;

    char riff[12];
    if (file.read(riff, 12) != 12) return false;
    if (qstrncmp(riff, "RIFF", 4) != 0 || qstrncmp(riff + 8, "WAVE", 4) != 0) return false;

    while (true) {
        char head[8];
        if (file.read(head, 8) != 8) return false;
        const quint32 chunkSize = readLe32(head + 4);

        if (qstrncmp(head, "fmt ", 4) == 0) {
            if (chunkSize < 16) return false;
            QByteArray fmt = file.read(static_cast<qint64>(std::min<quint32>(chunkSize, 40)));
            if (fmt.size() < 16) return false;

            quint16 format = readLe16(fmt.constData());
            channels = readLe16(fmt.constData() + 2);
            sampleRate = static_cast<int>(readLe32(fmt.constData() + 4));
            bitsPerSample = readLe16(fmt.constData() + 14);

            // WAVE_FORMAT_EXTENSIBLE hides the real format in the subformat GUID.
            if (format == 0xFFFE) {
                if (fmt.size() < 26) return false;
                format = readLe16(fmt.constData() + 24);
            }
            return format == 1; // WAVE_FORMAT_PCM
        }

        // Chunks are padded to an even length.
        const qint64 skip = chunkSize + (chunkSize & 1);
        if (!file.seek(file.pos() + skip)) return false;
    }
}

// ── Decoding ─────────────────────────────────────────────────────────────────

// Decodes the file to interleaved samples right-aligned to `bps` bits. miniaudio widens
// integer formats into s32 by a plain left shift, so shifting back recovers the original
// values exactly.
bool decodeToSamples(const QByteArray& fileData, int bps, int expectedChannels,
                     std::vector<qint32>& samples, ma_uint64& frameCount) {
    ma_decoder_config config = ma_decoder_config_init(ma_format_s32, 0, 0);
    ma_decoder decoder;
    if (ma_decoder_init_memory(fileData.constData(), static_cast<size_t>(fileData.size()),
                               &config, &decoder) != MA_SUCCESS) {
        return false;
    }

    const int channels = static_cast<int>(decoder.outputChannels);
    if (channels != expectedChannels) {
        ma_decoder_uninit(&decoder);
        return false;
    }

    ma_uint64 total = 0;
    if (ma_decoder_get_length_in_pcm_frames(&decoder, &total) != MA_SUCCESS || total == 0) {
        ma_decoder_uninit(&decoder);
        return false;
    }

    samples.resize(static_cast<size_t>(total) * channels);

    ma_uint64 done = 0;
    while (done < total) {
        ma_uint64 read = 0;
        const ma_result result = ma_decoder_read_pcm_frames(
            &decoder, samples.data() + done * channels, total - done, &read);
        if (read == 0) break;
        done += read;
        if (result != MA_SUCCESS) break;
    }
    ma_decoder_uninit(&decoder);

    if (done == 0) return false;
    samples.resize(static_cast<size_t>(done) * channels);
    frameCount = done;

    const int shift = 32 - bps;
    for (qint32& sample : samples) sample >>= shift;
    return true;
}

void appendUint24(QByteArray& out, quint32 value) {
    out.append(static_cast<char>((value >> 16) & 0xFF));
    out.append(static_cast<char>((value >> 8) & 0xFF));
    out.append(static_cast<char>(value & 0xFF));
}

void writeUtf8Number(BitWriter& bw, quint32 value) {
    if (value < 0x80) {
        bw.writeBits(value, 8);
        return;
    }
    int extra;
    quint32 lead;
    if (value < 0x800)          { extra = 1; lead = 0xC0; }
    else if (value < 0x10000)   { extra = 2; lead = 0xE0; }
    else if (value < 0x200000)  { extra = 3; lead = 0xF0; }
    else if (value < 0x4000000) { extra = 4; lead = 0xF8; }
    else                        { extra = 5; lead = 0xFC; }

    bw.writeBits(lead | (value >> (6 * extra)), 8);
    for (int i = extra - 1; i >= 0; --i) {
        bw.writeBits(0x80 | ((value >> (6 * i)) & 0x3F), 8);
    }
}

} // namespace

namespace FlacEncoder {

bool canEncode(const QString& wavPath) {
    if (QFileInfo(wavPath).suffix().compare(QStringLiteral("wav"), Qt::CaseInsensitive) != 0) {
        return false;
    }

    int bits = 0, channels = 0, sampleRate = 0;
    if (!readWavFormat(wavPath, bits, channels, sampleRate)) return false;

    if (bits != 8 && bits != 16 && bits != 24) return false;
    if (channels < 1 || channels > 8) return false;
    // STREAMINFO stores the rate in 20 bits.
    if (sampleRate < 1 || sampleRate >= (1 << 20)) return false;
    return true;
}

bool encodeFile(const QString& wavPath, QByteArray& out) {
    int bps = 0, channels = 0, sampleRate = 0;
    if (!readWavFormat(wavPath, bps, channels, sampleRate)) return false;
    if (bps != 8 && bps != 16 && bps != 24) return false;
    if (channels < 1 || channels > 8) return false;
    if (sampleRate < 1 || sampleRate >= (1 << 20)) return false;

    QFile file(wavPath);
    if (!file.open(QIODevice::ReadOnly)) return false;
    const QByteArray fileData = file.readAll();
    file.close();

    std::vector<qint32> samples;
    ma_uint64 frameCount = 0;
    if (!decodeToSamples(fileData, bps, channels, samples, frameCount)) return false;

    const int bytesPerSample = bps / 8;
    QCryptographicHash md5(QCryptographicHash::Md5);

    // Deinterleaved block, plus the two derived channels used by stereo decorrelation.
    std::vector<std::vector<qint32>> planar(channels, std::vector<qint32>(kBlockSize));
    std::vector<qint32> mid(kBlockSize), side(kBlockSize);
    std::vector<qint32> estimateScratch;
    Scratch scratch;

    QByteArray frames;
    quint32 minBlock = 0xFFFF, maxBlock = 0;
    quint32 minFrame = 0xFFFFFF, maxFrame = 0;
    quint32 frameNumber = 0;

    for (ma_uint64 start = 0; start < frameCount; start += kBlockSize) {
        const int n = static_cast<int>(std::min<ma_uint64>(kBlockSize, frameCount - start));
        const qint32* block = samples.data() + static_cast<size_t>(start) * channels;

        // MD5 in STREAMINFO covers the raw samples, little-endian and interleaved.
        QByteArray pcm;
        pcm.reserve(n * channels * bytesPerSample);
        for (int i = 0; i < n * channels; ++i) {
            const quint32 v = static_cast<quint32>(block[i]);
            for (int b = 0; b < bytesPerSample; ++b) {
                pcm.append(static_cast<char>((v >> (8 * b)) & 0xFF));
            }
        }
        md5.addData(pcm);

        for (int c = 0; c < channels; ++c) {
            for (int i = 0; i < n; ++i) planar[c][i] = block[i * channels + c];
        }

        // Stereo decorrelation: side often encodes far smaller than a second full channel.
        int assignment = channels - 1;
        if (channels == 2) {
            for (int i = 0; i < n; ++i) {
                const qint32 l = planar[0][i];
                const qint32 r = planar[1][i];
                mid[i] = (l + r) >> 1;
                side[i] = l - r;
            }
            const qint64 costL = estimateChannelBits(planar[0].data(), n, estimateScratch);
            const qint64 costR = estimateChannelBits(planar[1].data(), n, estimateScratch);
            const qint64 costM = estimateChannelBits(mid.data(), n, estimateScratch);
            const qint64 costS = estimateChannelBits(side.data(), n, estimateScratch);

            const qint64 options[4] = { costL + costR, costL + costS, costS + costR, costM + costS };
            const int codes[4] = { 0, 8, 9, 10 };
            int bestOption = 0;
            for (int i = 1; i < 4; ++i) {
                if (options[i] < options[bestOption]) bestOption = i;
            }
            assignment = codes[bestOption];
        }

        BitWriter frame;
        frame.writeBits(0x3FFE, 14);                     // sync code
        frame.writeBits(0, 1);                           // reserved
        frame.writeBits(0, 1);                           // fixed block size
        const bool fullBlock = (n == kBlockSize);
        frame.writeBits(fullBlock ? 0b1100u : 0b0111u, 4); // block size (4096, or 16-bit below)
        frame.writeBits(0, 4);                           // sample rate: read from STREAMINFO
        frame.writeBits(static_cast<quint32>(assignment), 4);
        frame.writeBits(0, 3);                           // sample size: read from STREAMINFO
        frame.writeBits(0, 1);                           // reserved
        writeUtf8Number(frame, frameNumber);
        if (!fullBlock) frame.writeBits(static_cast<quint32>(n - 1), 16);

        const QByteArray& headerBytes = frame.bytes();
        frame.writeBits(crc8(headerBytes.constData(), headerBytes.size()), 8);

        if (channels == 2 && assignment != 0) {
            // The difference channel needs one extra bit of range.
            switch (assignment) {
                case 8: // left / side
                    encodeSubframe(frame, planar[0].data(), n, bps, scratch);
                    encodeSubframe(frame, side.data(), n, bps + 1, scratch);
                    break;
                case 9: // side / right
                    encodeSubframe(frame, side.data(), n, bps + 1, scratch);
                    encodeSubframe(frame, planar[1].data(), n, bps, scratch);
                    break;
                default: // 10: mid / side
                    encodeSubframe(frame, mid.data(), n, bps, scratch);
                    encodeSubframe(frame, side.data(), n, bps + 1, scratch);
                    break;
            }
        } else {
            for (int c = 0; c < channels; ++c) {
                encodeSubframe(frame, planar[c].data(), n, bps, scratch);
            }
        }

        frame.alignToByte();
        const QByteArray& body = frame.bytes();
        frame.writeBits(crc16(body.constData(), body.size()), 16);

        const QByteArray& encoded = frame.bytes();
        frames.append(encoded);

        const quint32 size = static_cast<quint32>(encoded.size());
        minFrame = std::min(minFrame, size);
        maxFrame = std::max(maxFrame, size);
        minBlock = std::min(minBlock, static_cast<quint32>(n));
        maxBlock = std::max(maxBlock, static_cast<quint32>(n));
        ++frameNumber;
    }

    if (frameNumber == 0) return false;

    out.clear();
    out.append("fLaC", 4);
    out.append(static_cast<char>(0x80)); // last metadata block, type 0 (STREAMINFO)
    appendUint24(out, 34);

    BitWriter info;
    info.writeBits(minBlock, 16);
    info.writeBits(maxBlock, 16);
    info.writeBits(minFrame, 24);
    info.writeBits(maxFrame, 24);
    info.writeBits(static_cast<quint32>(sampleRate), 20);
    info.writeBits(static_cast<quint32>(channels - 1), 3);
    info.writeBits(static_cast<quint32>(bps - 1), 5);
    info.writeBits(static_cast<quint32>((frameCount >> 32) & 0xF), 4); // 36-bit total, high nibble
    info.writeBits(static_cast<quint32>(frameCount & 0xFFFFFFFFull), 32);
    out.append(info.bytes());
    out.append(md5.result());

    out.append(frames);
    return true;
}

} // namespace FlacEncoder
