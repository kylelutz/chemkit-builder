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

#include "displaysettingsdock.h"
#include "ui_displaysettingsdock.h"

#include "builderwindow.h"

#include <chemkit/molecule.h>
#include <chemkit/moleculewatcher.h>
#include <chemkit/graphicsmoleculeitem.h>

DisplaySettingsDock::DisplaySettingsDock(BuilderWindow *builder)
    : QDockWidget(builder),
      ui(new Ui::DisplaySettingsDock),
      m_builder(builder),
      m_batch(false)
{
    ui->setupUi(this);

    connect(ui->moleculeTypeComboBox, SIGNAL(currentIndexChanged(int)), SLOT(moleculeDisplayTypeChanged(int)));
    connect(ui->showHydrogensCheckBox, SIGNAL(clicked(bool)), SLOT(showHydrogensCheckClicked(bool)));
    connect(ui->showBondOrderCheckBox, SIGNAL(clicked(bool)), SLOT(showBondOrderCheckClicked(bool)));
    connect(ui->showPymolSESCheckBox, SIGNAL(clicked(bool)), SLOT(showPymolSESCheckClicked(bool)));
    connect(ui->showPymolSASCheckBox, SIGNAL(clicked(bool)), SLOT(showPymolSASCheckClicked(bool)));
    connect(builder, SIGNAL(moleculeChanged(chemkit::Molecule*)), SLOT(moleculeChanged(chemkit::Molecule*)));
}

DisplaySettingsDock::~DisplaySettingsDock()
{
    delete ui;
}

void DisplaySettingsDock::moleculeDisplayTypeChanged(int index)
{
    chemkit::GraphicsMoleculeItem *moleculeItem = m_builder->moleculeItem();
    if(!moleculeItem)
        return;

    if(index == 0)
        moleculeItem->setDisplayType(chemkit::GraphicsMoleculeItem::BallAndStick);
    else if(index == 1)
        moleculeItem->setDisplayType(chemkit::GraphicsMoleculeItem::Stick);
    else if(index == 2)
        moleculeItem->setDisplayType(chemkit::GraphicsMoleculeItem::SpaceFilling);

    if(!m_batch) {
        m_builder->view()->update();
    }
}

void DisplaySettingsDock::showHydrogensCheckClicked(bool checked)
{
    chemkit::GraphicsMoleculeItem *moleculeItem = m_builder->moleculeItem();
    if(moleculeItem) {
        moleculeItem->setHydrogensVisible(checked);
    }
    if(!m_batch) {
        m_builder->view()->update();
    }
}

void DisplaySettingsDock::showBondOrderCheckClicked(bool checked)
{
    chemkit::GraphicsMoleculeItem *moleculeItem = m_builder->moleculeItem();
    if(moleculeItem) {
        moleculeItem->setBondOrderVisible(checked);
    }
    if(!m_batch) {
        m_builder->view()->update();
    }
}

void DisplaySettingsDock::showPymolSESCheckClicked(bool checked)
{
    m_builder->showPymolSES(checked);
    if(!m_batch) {
        m_builder->view()->update();
    }
}

void DisplaySettingsDock::showPymolSASCheckClicked(bool checked)
{
    m_builder->showPymolSAS(checked);
    if(!m_batch) {
        m_builder->view()->update();
    }
}

void DisplaySettingsDock::moleculeChanged(chemkit::Molecule *molecule)
{
    Q_UNUSED(molecule);

    m_batch = true;
    moleculeDisplayTypeChanged(ui->moleculeTypeComboBox->currentIndex());
    showHydrogensCheckClicked(ui->showHydrogensCheckBox->checkState());
    showBondOrderCheckClicked(ui->showBondOrderCheckBox->checkState());
    showPymolSESCheckClicked(ui->showPymolSESCheckBox->checkState());
    showPymolSASCheckClicked(ui->showPymolSASCheckBox->checkState());
    m_batch = false;
}
