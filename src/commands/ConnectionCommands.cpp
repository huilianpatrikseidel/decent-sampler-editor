#include "ConnectionCommands.h"

AddConnectionCommand::AddConnectionCommand(ProjectManager* pm, const Connection& conn)
    : m_pm(pm), m_conn(conn) { setText("Add Connection"); }

void AddConnectionCommand::undo() { m_pm->removeConnection(m_conn); }
void AddConnectionCommand::redo() { m_pm->addConnection(m_conn); }

RemoveConnectionCommand::RemoveConnectionCommand(ProjectManager* pm, const Connection& conn)
    : m_pm(pm), m_conn(conn) { setText("Remove Connection"); }

void RemoveConnectionCommand::undo() { m_pm->addConnection(m_conn); }
void RemoveConnectionCommand::redo() { m_pm->removeConnection(m_conn); }
