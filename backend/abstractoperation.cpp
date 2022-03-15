#include "abstractoperation.h"

#include <QTimer>

AbstractOperation::AbstractOperation(QObject *parent):
    QObject(parent),
    m_timeoutTimer(new QTimer(this)),
    m_operationState(BasicOperationState::Ready)
{
    connect(m_timeoutTimer, &QTimer::timeout, this, &AbstractOperation::onOperationTimeout);

    m_timeoutTimer->setSingleShot(true);
    m_timeoutTimer->setInterval(30000);
}

void AbstractOperation::finish()
{
    stopTimeout();
    setOperationState(AbstractOperation::Finished);

    emit finished();
}

int AbstractOperation::operationState() const
{
    return m_operationState;
}

void AbstractOperation::setTimeout(int msec)
{
    m_timeoutTimer->setInterval(msec);
}

void AbstractOperation::onOperationTimeout()
{
    finishWithError(BackendError::TimeoutError, QStringLiteral("Operation timeout (generic)"));
}

void AbstractOperation::setOperationState(int state)
{
    m_operationState = state;
}

void AbstractOperation::finishWithError(BackendError::ErrorType error, const QString &errorString)
{
    setError(error, errorString);
    finish();
}

void AbstractOperation::startTimeout()
{
    m_timeoutTimer->start();
}

void AbstractOperation::stopTimeout()
{
    m_timeoutTimer->stop();
}
