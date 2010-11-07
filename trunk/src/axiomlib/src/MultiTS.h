/****************************************************************************
*						MultiTS
*
*	Description:	����� �������� ��������� �������� ������ ������������ ��������� ����
*	Author:		dk
*	History:     	-
*
****************************************************************************/
#ifndef __AXIOMS_LIB_MultiTS_HXX
#define __AXIOMS_LIB_MultiTS_HXX

#include <stdlib.h>
#include <vector>
    
namespace AxiomLib {

// ����������� ��������� ��� 
class MultiTS {

  public:
	
  	// ���� ��� �������� ������������ ���������� ����
	std::vector<std::vector<double> > data; // �� ����� paramNames
	
	// ���� �������� ���������� - ����� �� ���������� �� ������������ ������� ���������� ������������ � ������ ��������� ����
	std::vector<bool> validParams; // validParams[i]=0, ���� i-� �������� ���������� � ����, =1 �����
	
	// ������� ������ �� ������� ����������� ��������� ���� �� ������ ��������� � ������������ ������������������ ����������
	bool getTSByIndex(std::vector<double> &vec, int i) const { // =1 ���� ��� ok; =0 ���� ����� ��������� ���; i - ����� ���������
		// ��������� ������������ ����������
		if ((i < (int) validParams.size()) && (i >= 0)) {
		// ��������� ���� �� ������ ��������� ���
			if (validParams[i]) {
				vec.resize((data[i]).size());
				vec = data[i];
				return true;
			}
		}
		if ((i == -1) && (validParams.size() > 0)) {
			// ������ ��������� ���������� �������� - ������� �� ��������� �������� � ��������� ���� ������� data
			if (validParams[validParams.size() - 1]) {
				vec.resize((data[validParams.size() - 1]).size());
				vec = data[validParams.size() - 1];
				return true;
			}
		}
		return false;
	}

	// ������� ��������� ����������� ��������� ���� �� ������ ��������� � ������������ ������������������ ����������
	bool setTSByIndex(std::vector<double> &vec, int i) { // =1 ���� ��� ok; =0 ���� �� ������� ���������� ������
		// ��������� ������������ ����������
		if ((i < (int) validParams.size()) && (i >= 0)) {
			data[i].assign (vec.begin(), vec.end());
			validParams[i] = true;				
			return true;
		}
		if ((i == -1) && (validParams.size() > 0)) {
			// ������ ��������� ���������� �������� - ������� �� ��������� �������� � ��������� ���� ������� data
			data[validParams.size() - 1].assign (vec.begin(), vec.end());
			validParams[validParams.size() - 1] = true;
			return true;
		}
		return false;
	}
	
	// ������� ���������� ����� �������� ����� � ������ ����������
	int size(void) const {
		return (int) data.size();
	}
	
}; //end of class
    

// ����� ��������� ��������� ����� ��� ������ ������
typedef std::vector<MultiTS>  ClassTS;

// ����� ��������� ��������� �����
// ������ ������ - ����� ������ �� ���������� ������ (��. classNames)
typedef std::vector<ClassTS> ReferenceClassesTS;  //��� MalfunctionClassesTS

}; //  end of namespace

#endif /* __AXIOMS_LIB_MultiTS_HXX */
