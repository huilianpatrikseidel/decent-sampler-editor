#pragma once
#include <QString>
#include <QMap>
#include <QVariant>
#include <QXmlStreamWriter>
#include <memory>
#include <vector>

#include "CoreLibExport.h"

class CORELIB_EXPORT DsNode {
public:
    explicit DsNode(const QString& tagName);
    virtual ~DsNode() = default;

    // Move-only by design: the node owns its children. The explicit deletes also keep
    // MSVC from instantiating the copy operations, which dllexport would otherwise force
    // — and that instantiation fails (C2280) on the move-only m_children vector.
    DsNode(const DsNode&) = delete;
    DsNode& operator=(const DsNode&) = delete;
    DsNode(DsNode&&) = delete;
    DsNode& operator=(DsNode&&) = delete;

    void setAttribute(const QString& name, const QString& value);
    void setAttribute(const QString& name, int value);
    void setAttribute(const QString& name, long long value);
    void setAttribute(const QString& name, double value);
    
    QString getAttribute(const QString& name) const;
    bool hasAttribute(const QString& name) const;
    void removeAttribute(const QString& name);
    
    void setText(const QString& text);

    DsNode* addChild(std::unique_ptr<DsNode> child);
    DsNode* addChild(const QString& tagName);

    const std::vector<std::unique_ptr<DsNode>>& getChildren() const { return m_children; }
    
    // Serialization
    virtual void writeXml(QXmlStreamWriter& xml) const;

protected:
    QString m_tagName;
    QString m_text;
    QMap<QString, QString> m_attributes;
    std::vector<std::unique_ptr<DsNode>> m_children;
};

// Root element of a Decent Sampler file
class CORELIB_EXPORT DsDocument : public DsNode {
public:
    DsDocument();

    // Non-copyable for the same reason as the DsNode base (see above).
    DsDocument(const DsDocument&) = delete;
    DsDocument& operator=(const DsDocument&) = delete;

    // Helper to generate the entire XML string
    QString toXmlString() const;
    
    void writeXml(QXmlStreamWriter& xml) const override;
};
