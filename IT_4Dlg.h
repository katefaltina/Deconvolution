
// IT_4Dlg.h: файл заголовка
//

#pragma once

#include "Drawer.h"

// Диалоговое окно CIT4Dlg
class CIT4Dlg : public CDialogEx
{
// Создание
public:
	CIT4Dlg(CWnd* pParent = nullptr);	// стандартный конструктор

// Данные диалогового окна
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_IT_4_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// поддержка DDX/DDV


// Реализация
protected:
	HICON m_hIcon;

	// Созданные функции схемы сообщений
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

	afx_msg void OnTimer(UINT_PTR nIDEvent); // таймер
public:
	afx_msg void OnBnClickedOk(); // кнопка для построения исходного, выходного сигнала и импульсной характеристики
	afx_msg void OnBnClickedDeconv(); // кнопка для вычисления функционала и построения восстановленного сигнала

	vector<double> N; // точки

	// исходный сигнал
	double x(int); // функция для гауссовых куполов
	Drawer Signal;
	vector <double> signal;

	// первый купол
	double _A1;
	double _m1;
	double _sigma1;
	// второй купол
	double _A2;
	double _m2;
	double _sigma2;
	// третий купол
	double _A3;
	double _m3;
	double _sigma3;

	// импульсная характеристика
	double h(int); // функция импульсной характеристики
	Drawer Impulse;
	vector <double> impulse;
	double _Ah; // амплитуда
	double _sigmah;

	// свёртка
	Drawer Convolution;
	vector <double> convolution;

	vector<double> lambda; // массив для лямбд
	void f2(); // функция для новых х

	double A; // максимальная амплитуда сигнала
	double A_conv; // максимальная амплитуда свёртки
	int _N; // количество отсчетов
	double _fd; // частота дискретизации

	double MHJ(int, vector<double>&); // метод хука-дживса

	// функционал
	double function(vector<double>&);
	double functional;

	// обратная свёртка
	vector <double> deconvolution;
	double* sum_k;

	// шум
	void Noise();
	double _alfa;

	double otclon; // отклонение восстановленного сигнала от истинного
	CString _eps; // точность вычислинений для MHJ
};
