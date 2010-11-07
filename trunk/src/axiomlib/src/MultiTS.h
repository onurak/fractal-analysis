/****************************************************************************
*						MultiTS
*
*	Description:	Класс опиывает структуру хранения данных многомерного врменного ряда
*	Author:		dk
*	History:     	-
*
****************************************************************************/
#ifndef __AXIOMS_LIB_MultiTS_HXX
#define __AXIOMS_LIB_MultiTS_HXX

#include <stdlib.h>
#include <vector>
    
namespace AxiomLib {

// Многомерный временной ряд 
class MultiTS {

  public:
	
  	// Поле для хранения многомерного временного ряда
	std::vector<std::vector<double> > data; // по числу paramNames
	
	// Поле хранения информации - какие из параметров из канонической системы параметров присутствуют в данном временном ряду
	std::vector<bool> validParams; // validParams[i]=0, если i-й параметр отсутсвует в ряде, =1 иначе
	
	// Функция выдачи по запросу одномерного врменного ряда по номеру параметра в канонической последовательности параметров
	bool getTSByIndex(std::vector<double> &vec, int i) const { // =1 если все ok; =0 если этого параметра нет; i - номер параметра
		// Проверяем корректность параметров
		if ((i < (int) validParams.size()) && (i >= 0)) {
		// Проверяем есть ли данный временной ряд
			if (validParams[i]) {
				vec.resize((data[i]).size());
				vec = data[i];
				return true;
			}
		}
		if ((i == -1) && (validParams.size() > 0)) {
			// значит запросили корректную разметку - которая по умолчанию находтся в последнем поле вектора data
			if (validParams[validParams.size() - 1]) {
				vec.resize((data[validParams.size() - 1]).size());
				vec = data[validParams.size() - 1];
				return true;
			}
		}
		return false;
	}

	// Функция установки одномерного врменного ряда по номеру параметра в канонической последовательности параметров
	bool setTSByIndex(std::vector<double> &vec, int i) { // =1 если все ok; =0 если не удалось установить данные
		// Проверяем корректность параметров
		if ((i < (int) validParams.size()) && (i >= 0)) {
			data[i].assign (vec.begin(), vec.end());
			validParams[i] = true;				
			return true;
		}
		if ((i == -1) && (validParams.size() > 0)) {
			// значит запросили корректную разметку - которая по умолчанию находтся в последнем поле вектора data
			data[validParams.size() - 1].assign (vec.begin(), vec.end());
			validParams[validParams.size() - 1] = true;
			return true;
		}
		return false;
	}
	
	// Функция возвращает число врменных рядов в данном мультиряде
	int size(void) const {
		return (int) data.size();
	}
	
}; //end of class
    

// Набор эталонных временных рядов для одного класса
typedef std::vector<MultiTS>  ClassTS;

// Набор эталонных временных рядов
// индекс ветора - номер класса во внутреннем смысле (см. classNames)
typedef std::vector<ClassTS> ReferenceClassesTS;  //или MalfunctionClassesTS

}; //  end of namespace

#endif /* __AXIOMS_LIB_MultiTS_HXX */
