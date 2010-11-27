/* Author: Vladimir Belikov
 *
 * This file is part of Fractal Library.
 *
 * Fractal Library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Fractal Library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with Fractal Libray.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef OPENDTSFROMFILEDIALOG_H
#define OPENDTSFROMFILEDIALOG_H

#include <QDialog>
#include "../fractallib/FL.h"
#include "../fractallib/flqt.h"

namespace Ui {
    class OpenDTSFromFileDialog;
}

class OpenDTSFromFileDialog : public QDialog
{
    Q_OBJECT

public:
    explicit OpenDTSFromFileDialog(FL::Environment &env,
                                   QWidget *parent = 0);
    ~OpenDTSFromFileDialog();

    bool setFileName(const QString &fileName);
    FL::TimeSeries *staticTimeSeries() { return m_ts; }
    FL::TSDataSource *dataSource() { return m_ds; }
    int updateInterval() { return m_updateInterval; }
    int updateSize() { return m_updateSize; }
protected:
    virtual void accept();
private:
    Ui::OpenDTSFromFileDialog *ui;
    FL::Environment &env;
    FL::TimeSeries *m_ts;
    FL::TSDataSource *m_ds;
    int m_updateInterval;
    int m_updateSize;
    QString m_fileName;
    FL::FileCSV m_file;

private slots:
    void on_sbStaticPartSize_valueChanged(int );
};

#endif // OPENDTSFROMFILEDIALOG_H
