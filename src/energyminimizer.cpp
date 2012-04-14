/******************************************************************************
**
** Copyright (C) 2009-2011 Kyle Lutz <kyle.r.lutz@gmail.com>
** All rights reserved.
**
** This file is a part of the chemkit project. For more information
** see <http://www.chemkit.org>.
**
** Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions
** are met:
**
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in the
**     documentation and/or other materials provided with the distribution.
**   * Neither the name of the chemkit project nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**
******************************************************************************/

#include "energyminimizer.h"

#include <QtConcurrentRun>

// --- Construction and Destruction ---------------------------------------- //
EnergyMinimizer::EnergyMinimizer(chemkit::Molecule *molecule)
    : QObject()
{
    m_molecule = molecule;
    m_moleculeChanged = true;
    m_optimizer = new chemkit::MoleculeGeometryOptimizer;
    m_forceFieldName = "uff";
    m_state = Stopped;
    connect(&m_minimizationWatcher, SIGNAL(finished()), SLOT(minimizationStepFinished()));
}

EnergyMinimizer::~EnergyMinimizer()
{
    delete m_optimizer;
}

// --- Properties ---------------------------------------------------------- //
void EnergyMinimizer::setMolecule(chemkit::Molecule *molecule)
{
    if(molecule == m_molecule){
        return;
    }

    m_molecule = molecule;
    m_moleculeChanged = true;
}

chemkit::Molecule* EnergyMinimizer::molecule() const
{
    return m_molecule;
}

void EnergyMinimizer::setMoleculeChanged(bool changed)
{
    m_moleculeChanged = changed;
}

bool EnergyMinimizer::moleculeChanged() const
{
    return m_moleculeChanged;
}

void EnergyMinimizer::setForceField(const QString &name)
{
    m_forceFieldName = name;
    m_moleculeChanged = true;
}

std::string EnergyMinimizer::forceField() const
{
    return m_optimizer->forceField();
}

int EnergyMinimizer::state() const
{
    return m_state;
}

QString EnergyMinimizer::stateString() const
{
    switch(m_state){
        case Running: return "Running";
        case Stopped: return "Stopped";
        case SettingUp: return "Setting Up";
        case SetupFailed: return "Setup Failed";
        case UpdateReady: return "Update Ready";
        case Converged: return "Converged";
        default: return "Unknown";
    }
}

chemkit::MoleculeGeometryOptimizer* EnergyMinimizer::optimizer() const
{
    return m_optimizer;
}

// --- Optimization -------------------------------------------------------- //
chemkit::Real EnergyMinimizer::energy() const
{
    return m_optimizer->energy();
}

// --- Slots --------------------------------------------------------------- //
void EnergyMinimizer::start()
{
    if(!m_molecule || m_molecule->isEmpty()){
        setState(SetupFailed);
        return;
    }

    if(m_moleculeChanged){
        QByteArray forceFieldNameString = m_forceFieldName.toAscii();
        bool ok = m_optimizer->setForceField(forceFieldNameString.constData());
        if(!ok){
            setState(SetupFailed);
            return;
        }

        setState(SettingUp);

        m_optimizer->setMolecule(m_molecule);
        m_optimizer->setup();
        if(!ok){
            setState(SetupFailed);
            return;
        }

        m_moleculeChanged = false;
    }

    QFuture<void> future =
        QtConcurrent::run(m_optimizer, &chemkit::MoleculeGeometryOptimizer::step);
    m_minimizationWatcher.setFuture(future);

    setState(Running);
}

void EnergyMinimizer::stop()
{
    setState(Stopped);
}

// --- Internal Methods ---------------------------------------------------- //
void EnergyMinimizer::setState(int state)
{
    if(state == m_state)
        return;

    m_state = state;
    emit stateChanged(state);
}

void EnergyMinimizer::minimizationStepFinished()
{
    if(m_state == Stopped){
        return;
    }

    if(m_optimizer->converged()){
        setState(Converged);
    }
    else{
        setState(UpdateReady);
    }
}
