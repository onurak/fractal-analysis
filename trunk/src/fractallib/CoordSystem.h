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

#ifndef COORDSYSTEM_H
#define COORDSYSTEM_H

#include "G/GDelegate.h"

namespace FL {

/*! \addtogroup FLGUI
  */
/*@{*/

/*! \class CoordSystem
  * \brief ������� ���������. ����������� ����������� ���������� � ����������
  *
  * ����������� ���������� ����������� ������� ��������� � ����������:
  * \code
  * [minX .. maxX]x[minY .. maxY] -> [borderSize .. width-borderSize]x[borderSize .. height-borderSize]
  * \endcode
  * ������������ ��� ������
  */
class CoordSystem {
public:
    //! ���������, ������������ ��� ��������� ���������� ������� ���������
    typedef GDelegate0<void> NotifyCoordSystemChange;

    //! �����������
    /*! \param ppx Pixels Per Ox - ���������� �������� � ����� ������� �� X
      * \param ppy Pixels Per Oy - ���������� �������� � ����� ������� �� Y
      */
    CoordSystem(double ppx, double ppy) {
        onChange = NULL;
        m_ppx = ppx;
        m_ppy = ppy;
        m_borderSize = 20;
        m_height = m_width = 0;
        m_xOffset = 0;
    }

    /* ������� ������� �������������� ��������� */

    //! ����������� ����� ���������� x �� ����������� ������������� � ���������� ���������
    int tX(int x) const { return int(x*m_ppx) + m_borderSize + m_xOffset; }
    //! ����������� ����� ���������� y �� ����������� ������������� � ���������� ���������
    int tY(int y) const { return int(-y*m_ppy) + m_middle; }
    //! ����������� ������������ ���������� x �� ����������� ������������� � ���������� ���������
    int tX(double x) const { return int(x*m_ppx) + m_borderSize + m_xOffset; }
    //! ����������� ������������ ���������� y �� ����������� ������������� � ���������� ���������
    int tY(double y) const { return int(-y*m_ppy) + m_middle; }

    //! ������ �������������� ��� ��������. ������������ FL::QtRender ��� ������ ����������
    int systX(int x) const { return int(x*m_ppx) + m_borderSize; }

    /* ������� ��������� �������������� ��������� */
    //! �������� �������������� ���������� Y
    double bY(int y) const { return double(m_middle - y)/m_ppy; }



    /* �������� */
    // ������
    //! ���������� ������ ����������� ����������
    int width() const { return m_width; }

    //! ������������� ������ ����������� ����������
    /*! ���������� ������� onChange
        \param value ����� ������
      */
    void setWidth(int value) { m_width = value; onChange(); }

    // ������
    //! ���������� ������ ����������� ����������
    int height() const { return m_height; }

    //! ������������� ������ ����������� ����������
    /*! ���������� ������� onChange
      * \param value ����� ������
      */
    void setHeight(int value) {
        m_height = value;
        m_middle = value / 2;
        onChange();
    }

    // ���������� �������� � ����� ������� �� X
    //! ���������� ���������� �������� � ����� ������� �� X
    double ppx() const { return m_ppx; }

    //! ������������� ���������� �������� � ����� ������� �� X
    /*! ���������� ������� onChange
      * \param value ���������� �������� � ����� ������� �� X
      */
    void setPpx(double value) { m_ppx = value; onChange(); }

    // ���������� �������� � ����� ������� �� Y
    //! ���������� ���������� �������� � ����� ������� �� X
    double ppy() const { return m_ppy; }

    //! ������������� ���������� �������� � ����� ������� �� X
    /*! ���������� ������� onChange
      * \param value ���������� �������� � ����� ������� �� X
      */
    void setPpy(double value) { m_ppy = value; onChange(); }

    // ������ ������� �� ����� (� ��������)
    //! ���������� ������ ������� �� ����� (� ��������)
    int borderSize() const { return m_borderSize; }

    //! ������������� ������ ������� �� ����� (� ��������)
    void setBorderSize(int value) { m_borderSize = value; onChange(); }

    // ����������� � ������������ ������������ ��������
    //! ���������� ����������� �������� ������������� �������������� ����� Ox
    double minX() const { return  0; }

    //! ���������� ������������ �������� ������������� �������������� ����� Ox
    double maxX() const { return  double(m_width - 2*m_borderSize) / m_ppx; }

    //! ���������� ����������� �������� ������������� �������������� ����� Oy
    double minY() const { return -double(m_middle) / m_ppy; }

    //! ���������� ������������ �������� ������������� �������������� ����� Oy
    double maxY() const { return  double(m_middle) / m_ppy; }

    //! ���������� �������� ����� Ox, ������������ �� ���� �����������
    int xOffset() const { return m_xOffset; }

    //! ������������� �������� ����� Ox, ������������ �� ���� �����������
    void setXOffset(int value) { m_xOffset = value; onChange(); }

public:
    //! �������, ������������ ��� ��������� ���������� ������� ���������
    NotifyCoordSystemChange onChange;
private:
    //! ������ ����������� ����������
    int m_height;
    //! ������ ����������� ����������
    int m_width;
    //! �������� ������ ����������� ���������� (������������ ��������)
    int m_middle;
    //! ������ ������� �� ����� ����������� ����������
    int m_borderSize;
    //! ���������� �������� � ����� ������� �� X
    double m_ppx;
    //! ���������� �������� � ����� ������� �� Y
    double m_ppy;
    //! �������� ����� Ox, ������������ �� ���� �����������
    int m_xOffset;
};

/*@}*/

} // namespace

#endif // COORDSYSTEM_H
