#include "MidiEngine.h"
#include <QDebug>

MidiEngine::MidiEngine() {
    try {
        m_midiIn = std::make_unique<RtMidiIn>();
    } catch (RtMidiError &error) {
        error.printMessage();
    }
}

MidiEngine::~MidiEngine() {
    stop();
    auto cb = m_callback.exchange(nullptr);
    if (cb) delete cb;
}

bool MidiEngine::initialize(int portIndex) {
    if (!m_midiIn) return false;
    
    if (m_midiIn->getPortCount() > 0) {
        if (portIndex >= static_cast<int>(m_midiIn->getPortCount())) {
            portIndex = 0; // Fallback to first port if saved index is out of range
        }
        m_midiIn->openPort(portIndex);
        m_midiIn->setCallback(&MidiEngine::midiCallback, this);
        m_midiIn->ignoreTypes(false, false, false);
        m_currentPort = portIndex;
        qDebug() << "MIDI port opened:" << QString::fromStdString(m_midiIn->getPortName(portIndex));
        return true;
    }
    qDebug() << "No MIDI ports found.";
    return false;
}

void MidiEngine::stop() {
    if (m_midiIn && m_midiIn->isPortOpen()) {
        m_midiIn->cancelCallback();
        m_midiIn->closePort();
        m_currentPort = -1;
    }
}

void MidiEngine::setCallback(std::function<void(unsigned char, unsigned char, unsigned char)> callback) {
    auto newCb = new std::function<void(unsigned char, unsigned char, unsigned char)>(std::move(callback));
    auto oldCb = m_callback.exchange(newCb, std::memory_order_acq_rel);
    if (oldCb) {
        // Apaga o antigo garantindo thread-safety
        delete oldCb;
    }
}

std::vector<QString> MidiEngine::enumeratePorts() const {
    std::vector<QString> ports;
    if (!m_midiIn) return ports;
    
    unsigned int count = m_midiIn->getPortCount();
    for (unsigned int i = 0; i < count; ++i) {
        ports.push_back(QString::fromStdString(m_midiIn->getPortName(i)));
    }
    return ports;
}

bool MidiEngine::switchPort(int portIndex) {
    if (!m_midiIn) return false;
    if (portIndex < 0 || portIndex >= static_cast<int>(m_midiIn->getPortCount())) return false;
    
    // Save current callback
    auto currentCb = m_callback.load(std::memory_order_acquire);
    
    // Close current port
    if (m_midiIn->isPortOpen()) {
        m_midiIn->cancelCallback();
        m_midiIn->closePort();
    }
    
    // Open new port
    try {
        m_midiIn->openPort(portIndex);
        m_midiIn->setCallback(&MidiEngine::midiCallback, this);
        m_midiIn->ignoreTypes(false, false, false);
        m_currentPort = portIndex;
        

        
        qDebug() << "MIDI port switched to:" << QString::fromStdString(m_midiIn->getPortName(portIndex));
        return true;
    } catch (RtMidiError& error) {
        error.printMessage();
        return false;
    }
}

QString MidiEngine::currentPortName() const {
    if (!m_midiIn || m_currentPort < 0 || m_currentPort >= static_cast<int>(m_midiIn->getPortCount())) {
        return QString();
    }
    return QString::fromStdString(m_midiIn->getPortName(m_currentPort));
}

void MidiEngine::midiCallback(double timeStamp, std::vector<unsigned char> *message, void *userData) {
    if (message->size() >= 3) {
        MidiEngine* engine = static_cast<MidiEngine*>(userData);
        auto cb = engine->m_callback.load(std::memory_order_acquire);
        if (cb && *cb) {
            (*cb)(message->at(0), message->at(1), message->at(2));
        }
    }
}
