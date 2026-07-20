#include "DsDom.h"

DsNode::DsNode(const QString& tagName) : m_tagName(tagName) {}

void DsNode::setAttribute(const QString& name, const QString& value) {
    m_attributes[name] = value;
}

void DsNode::setAttribute(const QString& name, int value) {
    m_attributes.insert(name, QString::number(value));
}

void DsNode::setAttribute(const QString& name, long long value) {
    m_attributes.insert(name, QString::number(value));
}

void DsNode::setAttribute(const QString& name, double value) {
    m_attributes[name] = QString::number(value);
}

QString DsNode::getAttribute(const QString& name) const {
    return m_attributes.value(name);
}

bool DsNode::hasAttribute(const QString& name) const {
    return m_attributes.contains(name);
}

void DsNode::removeAttribute(const QString& name) {
    m_attributes.remove(name);
}

void DsNode::setText(const QString& text) {
    m_text = text;
}

DsNode* DsNode::addChild(std::unique_ptr<DsNode> child) {
    DsNode* ptr = child.get();
    m_children.push_back(std::move(child));
    return ptr;
}

DsNode* DsNode::addChild(const QString& tagName) {
    auto child = std::make_unique<DsNode>(tagName);
    DsNode* ptr = child.get();
    m_children.push_back(std::move(child));
    return ptr;
}

void DsNode::writeXml(QXmlStreamWriter& xml) const {
    xml.writeStartElement(m_tagName);
    
    // Write attributes (we sort them for stable output)
    auto keys = m_attributes.keys();
    // QMap keys are already sorted, which is great for predictable XML output.
    for (const QString& key : keys) {
        xml.writeAttribute(key, m_attributes[key]);
    }
    
    if (!m_text.isEmpty()) {
        xml.writeCharacters(m_text);
    }
    
    for (const auto& child : m_children) {
        child->writeXml(xml);
    }
    
    xml.writeEndElement();
}

// --- DsDocument ---

DsDocument::DsDocument() : DsNode("DecentSampler") {
    setAttribute("pluginVersion", 1);
}

QString DsDocument::toXmlString() const {
    QString output;
    QXmlStreamWriter xml(&output);
    xml.setAutoFormatting(true);
    
    xml.writeStartDocument();
    writeXml(xml);
    xml.writeEndDocument();
    
    return output;
}

void DsDocument::writeXml(QXmlStreamWriter& xml) const {
    DsNode::writeXml(xml);
}
