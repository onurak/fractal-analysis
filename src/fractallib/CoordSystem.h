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
  * \brief Система координат. Преобразует виртуальные координаты в физические
  *
  * Преобразует координаты виратульной системы координат в физические:
  * \code
  * [minX .. maxX]x[minY .. maxY] -> [borderSize .. width-borderSize]x[borderSize .. height-borderSize]
  * \endcode
  * Используется для вывода
  */
class CoordSystem {
public:
    //! Сообщение, генерируемое при изменении параметров системы координат
    typedef GDelegate0<void> NotifyCoordSystemChange;

    //! Конструктор
    /*! \param ppx Pixels Per Ox - количество пикселей в одном делении по X
      * \param ppy Pixels Per Oy - количество пикселей в одном делении по Y
      */
    CoordSystem(double ppx, double ppy) {
        onChange = NULL;
        m_ppx = ppx;
        m_ppy = ppy;
        m_borderSize = 20;
        m_height = m_width = 0;
        m_xOffset = 0;
    }

    /* функции прямого преобразования координат */

    //! Преобразует целую координату x из внутреннего представления в физические кординаты
    int tX(int x) const { return int(x*m_ppx) + m_borderSize + m_xOffset; }
    //! Преобразует целую координату y из внутреннего представления в физические кординаты
    int tY(int y) const { return int(-y*m_ppy) + m_middle; }
    //! Преобразует вещественную координату x из внутреннего представления в физические кординаты
    int tX(double x) const { return int(x*m_ppx) + m_borderSize + m_xOffset; }
    //! Преобразует вещественную координату y из внутреннего представления в физические кординаты
    int tY(double y) const { return int(-y*m_ppy) + m_middle; }

    //! Особое преобразование без смещения. Используется FL::QtRender для вывода интерфейса
    int systX(int x) const { return int(x*m_ppx) + m_borderSize; }

    /* функции обратного преобразования координат */
    //! Обратное преобразование координаты Y
    double bY(int y) const { return double(m_middle - y)/m_ppy; }



    /* свойства */
    // ширина
    //! Возвращает ширину физического устройства
    int width() const { return m_width; }

    //! Устанавливает ширину физического устройства
    /*! Генерирует событие onChange
        \param value Новая ширина
      */
    void setWidth(int value) { m_width = value; onChange(); }

    // высота
    //! Возвращает высоту физического устройства
    int height() const { return m_height; }

    //! Устанавливает высоту физического устройства
    /*! Генерирует событие onChange
      * \param value Новая ширина
      */
    void setHeight(int value) {
        m_height = value;
        m_middle = value / 2;
        onChange();
    }

    // количество пикселей в одном делении по X
    //! Возвращает количество пикселей в одном делении по X
    double ppx() const { return m_ppx; }

    //! Устанавливает количество пикселей в одном делении по X
    /*! Генерирует событие onChange
      * \param value количество пикселей в одном делении по X
      */
    void setPpx(double value) { m_ppx = value; onChange(); }

    // количество пикселей в одном делении по Y
    //! Возвращает количество пикселей в одном делении по X
    double ppy() const { return m_ppy; }

    //! Устанавливает количество пикселей в одном делении по X
    /*! Генерирует событие onChange
      * \param value количество пикселей в одном делении по X
      */
    void setPpy(double value) { m_ppy = value; onChange(); }

    // размер отступа от краев (в пикселах)
    //! Возвращает размер отступа от краев (в пикселах)
    int borderSize() const { return m_borderSize; }

    //! Устанавливает размер отступа от краев (в пикселах)
    void setBorderSize(int value) { m_borderSize = value; onChange(); }

    // минимальные и максимальные отображаемые значения
    //! Возвращает минимальное значение отображаемого прямоугольника вдоль Ox
    double minX() const { return  0; }

    //! Возвращает максимальное значение отображаемого прямоугольника вдоль Ox
    double maxX() const { return  double(m_width - 2*m_borderSize) / m_ppx; }

    //! Возвращает минимальное значение отображаемого прямоугольника вдоль Oy
    double minY() const { return -double(m_middle) / m_ppy; }

    //! Возвращает максимальное значение отображаемого прямоугольника вдоль Oy
    double maxY() const { return  double(m_middle) / m_ppy; }

    //! Возвращает смещение вдоль Ox, прибавляемое ко всем координатам
    int xOffset() const { return m_xOffset; }

    //! Устанавливает смещение вдоль Ox, прибавляемое ко всем координатам
    void setXOffset(int value) { m_xOffset = value; onChange(); }

public:
    //! Событие, генерируемое при изменении параметров системы координат
    NotifyCoordSystemChange onChange;
private:
    //! Высота физического устройства
    int m_height;
    //! Ширина физического устройства
    int m_width;
    //! Середина высоты физического устройства (кэшированное значение)
    int m_middle;
    //! Размер отступа от краев физического устройства
    int m_borderSize;
    //! Количество пикселей в одном делении по X
    double m_ppx;
    //! Количество пикселей в одном делении по Y
    double m_ppy;
    //! Смещение вдоль Ox, прибавляемое ко всем координатам
    int m_xOffset;
};

/*@}*/

} // namespace

#endif // COORDSYSTEM_H
