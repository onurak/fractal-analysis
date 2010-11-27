#include "OpenDTSFromFileDialog.h"
#include "ui_OpenDTSFromFileDialog.h"

OpenDTSFromFileDialog::OpenDTSFromFileDialog(FL::Environment &env, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::OpenDTSFromFileDialog),
    env(env)
{
    ui->setupUi(this);
    m_ts = NULL;
    m_ds = NULL;
}

bool OpenDTSFromFileDialog::setFileName(const QString &fileName)
{
    m_fileName = fileName;
    ui->lbFileName->setText(fileName);
    if (m_file.loadFromFile(STR(fileName)))
    {
        ui->cbColumn->clear();
        std::string defaultColumnName =
                env["OpenDynamicTSFromFileDialog"].valueOf("DefaultColumn", "");
        int defaultColumnIndex = 0;
        std::vector<std::string>::const_iterator headerItem;
        int i = 0;
        for_each_(headerItem, m_file.header())
        {
            ui->cbColumn->addItem(QSTR(*headerItem));
            if (*headerItem == defaultColumnName)
                defaultColumnIndex = i;
            ++i;
        }
        ui->cbColumn->setCurrentIndex(defaultColumnIndex);

        ui->lbTSSize->setText( QString().setNum(m_file.rowCount()) );
        int staticPartSize = (int)(
                m_file.rowCount() *
                (double(env["OpenDynamicTSFromFileDialog"].valueOf("DefaultStaticPartPercent", 30)) / 100.0));
        if (staticPartSize > m_file.rowCount())
            staticPartSize = m_file.rowCount();
        ui->sbStaticPartSize->setValue(staticPartSize);
        ui->sbStaticPartSize->setMaximum(m_file.rowCount());
        ui->lbDynamicPartSize->setText(QString().setNum(m_file.rowCount() - staticPartSize));

        ui->sbUpdateInterval->setValue(
                env["OpenDynamicTSFromFileDialog"].valueOf("DefaultUpdateInterval", 1000));

        ui->sbUpdateInterval->setValue(
                env["OpenDynamicTSFromFileDialog"].valueOf("DefaultUpdateSize", 1));
        return true;
    }
    return false;
}

OpenDTSFromFileDialog::~OpenDTSFromFileDialog()
{
    delete ui;
}

void OpenDTSFromFileDialog::accept()
{
    m_ts = new FL::TimeSeries();
    if (ui->sbStaticPartSize->value() > 0)
    {
        m_file.loadTimeSeries(m_ts, ui->cbColumn->currentIndex());
        while (m_ts->size() > ui->sbStaticPartSize->value())
            m_ts->remove(m_ts->dimSize(0)-1);
    }

    m_ds = new FL::TSDataSourceFileCSV();
    m_ds->open(STR(m_fileName));
    static_cast<FL::TSDataSourceFileCSV*> (m_ds)->setCurrentRow(
            m_file.rowCount() - ui->sbStaticPartSize->value());

    m_updateInterval = ui->sbUpdateInterval->value();
    m_updateSize = ui->sbUpdateSize->value();
    QDialog::accept();
}

void OpenDTSFromFileDialog::on_sbStaticPartSize_valueChanged(int value)
{
    ui->lbDynamicPartSize->setText( QString().setNum(m_file.rowCount() - value) );
}
