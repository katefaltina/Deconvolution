// Деконволюция свёртки методом максимума энтропии

// IT_4Dlg.cpp: файл реализации
//

#include "pch.h"
#include "framework.h"
#include "IT_4.h"
#include "IT_4Dlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

bool killtimer = false; // флаг убийства таймера
int timer; // переменная для таймера
bool lflag = false; // флаг для масива лямбд

// функция потока
DWORD dwThread; HANDLE hThread;
DWORD WINAPI func(PVOID pv)
{
	CIT4Dlg* p = (CIT4Dlg*)pv;
	p->MHJ(p->_N, p->lambda);
	return 0;
}

// Диалоговое окно CIT4Dlg

CIT4Dlg::CIT4Dlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_IT_4_DIALOG, pParent)
	, _A1(1)
	, _m1(0.5)
	, _sigma1(0.2)
	, _A2(3)
	, _m2(1.5)
	, _sigma2(0.3)
	, _A3(2)
	, _m3(2.4)
	, _sigma3(0.3)
	, _Ah(1)
	, _sigmah(0.2)
	, _N(32)
	, _fd(10)
	, functional(0)
	, _alfa(0.1)
	, otclon(0)
	, _eps(_T("1e-6"))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CIT4Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_A1, _A1);
	DDX_Text(pDX, IDC_M1, _m1);
	DDX_Text(pDX, IDC_SIGMA1, _sigma1);
	DDX_Text(pDX, IDC_A2, _A2);
	DDX_Text(pDX, IDC_M2, _m2);
	DDX_Text(pDX, IDC_SIGMA2, _sigma2);
	DDX_Text(pDX, IDC_A3, _A3);
	DDX_Text(pDX, IDC_M3, _m3);
	DDX_Text(pDX, IDC_SIGMA3, _sigma3);
	DDX_Text(pDX, IDC_A, _Ah);
	DDX_Text(pDX, IDC_SIGMA, _sigmah);
	DDX_Text(pDX, IDC_N, _N);
	DDX_Text(pDX, IDC_FD, _fd);
	DDX_Text(pDX, IDC_EDIT1, functional);
	DDX_Text(pDX, IDC_ALFA, _alfa);
	DDX_Text(pDX, IDC_OTCLON, otclon);
	DDX_Text(pDX, IDC_ALFA2, _eps);
}

BEGIN_MESSAGE_MAP(CIT4Dlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDOK, &CIT4Dlg::OnBnClickedOk)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_DECONV, &CIT4Dlg::OnBnClickedDeconv)
END_MESSAGE_MAP()


// Обработчики сообщений CIT4Dlg

BOOL CIT4Dlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Задает значок для этого диалогового окна.  Среда делает это автоматически,
	//  если главное окно приложения не является диалоговым
	SetIcon(m_hIcon, TRUE);			// Крупный значок
	SetIcon(m_hIcon, FALSE);		// Мелкий значок

	// TODO: добавьте дополнительную инициализацию
	//killtimer = false;

	Signal.Create(GetDlgItem(IDC_SIGNAL)->GetSafeHwnd());
	Impulse.Create(GetDlgItem(IDC_IMPULSE)->GetSafeHwnd());
	Convolution.Create(GetDlgItem(IDC_SVERTKA)->GetSafeHwnd());
	//Deconvolution.Create(GetDlgItem(IDC_SIGNAL)->GetSafeHwnd());

	return TRUE;  // возврат значения TRUE, если фокус не передан элементу управления
}

// При добавлении кнопки свертывания в диалоговое окно нужно воспользоваться приведенным ниже кодом,
//  чтобы нарисовать значок.  Для приложений MFC, использующих модель документов или представлений,
//  это автоматически выполняется рабочей областью.

void CIT4Dlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // контекст устройства для рисования

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Выравнивание значка по центру клиентского прямоугольника
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Нарисуйте значок
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// Система вызывает эту функцию для получения отображения курсора при перемещении
//  свернутого окна.
HCURSOR CIT4Dlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

// функция гауссовых куполов
double CIT4Dlg::x(int n)
{
	double dt = 1 / _fd;
	double s1 = _A1 * exp(-(pow((n * dt) - _m1, 2) / (2 * _sigma1 * _sigma1)));
	double s2 = _A2 * exp(-(pow((n * dt) - _m2, 2) / (2 * _sigma2 * _sigma2)));
	double s3 = _A3 * exp(-(pow((n * dt) - _m3, 2) / (2 * _sigma3 * _sigma3)));
	return s1 + s2 + s3;
}

// функция импульсной характеристики
double  CIT4Dlg::h(int n)
{
	double dt = 1 / _fd;
	if (n < _N / 2)
	{
		return _Ah * exp(-(n * dt * n * dt) / (2 * _sigmah * _sigmah));
	}
	else 
	{
		return _Ah * exp(-((n - _N) * dt * (n - _N) * dt) / (2 * _sigmah * _sigmah));
	}
}

// шум (накладываем на выходной сигнал, полученный после свёртки)
void CIT4Dlg::Noise()
{
	double* g = new double[_N];
	for (int i = 0; i < _N; i++)
	{
		g[i] = 0;
		for (int k = 0; k < 12; k++) g[i] += rand();
	}

	double betta = 0;

	double res_noize = 0; double res_g = 0;

	for (int i = 0; i < _N; i++)
	{
		res_noize += convolution[i] * convolution[i];
		res_g += g[i] * g[i];
	}

	betta = sqrt(_alfa * res_noize / res_g / 100);
	A_conv = 0;
	for (int i = 0; i < _N; i++)
	{
		convolution[i] = convolution[i] + betta * g[i];
		if (convolution[i] > A_conv) A_conv = convolution[i];
	}
}

// кнопка для вывода графиков исходного сигнала, импульс. характеристики и свёртки
void CIT4Dlg::OnBnClickedOk()
{
	signal.clear(); N.clear();
	impulse.clear(); convolution.clear();
	deconvolution.clear();

	UpdateData(true);

	// гауссовы купола (сигнал)
	A = 0;
	for (int n = 0; n < _N; n++)
	{
		double res = x(n);
		N.push_back(n);
		signal.push_back(res);
		if (signal[n] > A) A = signal[n];
	}
	// график сигнала
	Signal.Draw(3, signal, N, _N, A, N[_N - 1]);
	Signal.DrawRis(3, 1, signal, N, _N, A, N[_N - 1]);

	//N.clear();
	// импульсная характеристика
	for (int n = 0; n < _N; n++)
	{
		double res1 = h(n);
		impulse.push_back(res1);
		N.push_back(n);
	}
	// график импульсной характеристики
	Impulse.Draw(3, impulse, N, _N, _Ah, N[_N - 1]);
	Impulse.DrawRis(3, 1, impulse, N, _N, _Ah, N[_N - 1]);
	
	//N.clear();
	// свёртка
	A_conv = 0;
	double res;
	for (int i = 0; i < _N; i++)
	{
		res = 0;
		for (int k = 0; k < _N; k++)
		{
			res += signal[k] * impulse[abs(k - i)];
		}
		convolution.push_back(res);
		if (convolution[i] > A_conv) A_conv = convolution[i];
		N.push_back(i);
	}
	Noise(); // наложение шума
	// график свёртки
	Convolution.Draw(3, convolution, N, _N, A_conv, N[_N - 1]);
	Convolution.DrawRis(3, 1, convolution, N, _N, A_conv, N[_N - 1]);
}

//функционал
double CIT4Dlg::function(vector<double>& lambda)
{
	// Реализует оптимизируемую функцию
	// Возвращает значение функции, количество параметров является членом класса, в противном случае изменить сигнатуру функции
	double* sum_k = new double[_N];
	double sum_i;
	for (int k = 0; k < _N; k++)
	{
		sum_i = 0;
		for (int i = 0; i < _N; i++)
		{
			if ((k - i) < 0)
			{
				sum_i += lambda[i] * impulse[k - i + _N];
			}
			else
				sum_i += lambda[i] * impulse[k - i];
		}
		deconvolution.resize(_N);
		sum_k[k] = exp(-1 - sum_i);
		deconvolution[k] = sum_k[k];
	}
	double F = 0;
	double sum_m;
	for (int m = 0; m < _N; m++)
	{
		sum_m = 0;
		for (int j = 0; j < _N; j++)
		{
			if ((j - m) < 0)
			{
				sum_m += sum_k[j] * impulse[j - m + _N];
			}
			else sum_m += sum_k[j] * impulse[j - m];
		}
		F += (convolution[m] - sum_m) * (convolution[m] - sum_m);
	}
	delete[] sum_k;
	return F;
}

// метод Хука-Дживса
double CIT4Dlg::MHJ(int kk, vector<double>& x)
{
	// kk - количество параметров; x - массив параметров
	killtimer = false;
	double  TAU = _wtof(_eps); // Точность вычислений
	int i, j, bs, ps;
	double z, h, k, fi, fb;
	double* b = new double[kk];
	double* y = new double[kk];
	double* p = new double[kk];

	h = 0.1;
	x[0] = 1.;
	for (i = 1; i < kk; i++)
	{
		x[i] = (double)rand() / RAND_MAX -0.5; // Задается начальное приближение
	}

	k = h;
	for (i = 0; i < kk; i++)
	{
		y[i] = p[i] = b[i] = x[i];
	}
	fi = function(x);
	ps = 0;
	bs = 1;
	fb = fi;

	j = 0;
	while (1)
	{
		//calc++; // Счетчик итераций. Можно игнорировать
		x[j] = y[j] + k;
		z = function(x);
		if (z >= fi)
		{
			x[j] = y[j] - k;
			z = function(x);
			if (z < fi)
			{
				y[j] = x[j];
			}
			else  x[j] = y[j];
		}
		else  y[j] = x[j];
		fi = function(x);

		if (j < kk - 1)
		{
			j++;
			continue;
		}
		if (fi + 1e-8 >= fb)
		{
			if (ps == 1 && bs == 0)
			{
				for (i = 0; i < kk; i++)
				{
					p[i] = y[i] = x[i] = b[i];
				}
				z = function(x);
				bs = 1;
				ps = 0;
				fi = z;
				fb = z;
				j = 0;
				continue;
			}
			k /= 10.;
			if (k < TAU)
			{
				break;
			}
			j = 0;
			continue;
		}

		for (i = 0; i < kk; i++)
		{
			p[i] = 2 * y[i] - b[i];
			b[i] = y[i];
			x[i] = p[i];

			y[i] = x[i];
		}
		z = function(x);
		fb = fi;
		ps = 1;
		bs = 0;
		fi = z;
		functional = fi;
		j = 0;
	} //  end of while(1)

	for (i = 0; i < kk; i++)
	{
		x[i] = p[i];
	}

	delete b;
	delete y;
	delete p;
	killtimer = true;
	return fb;
}

// вычисление х восстановленного, вычисление функционала и отклонения
void CIT4Dlg::f2()
{
	A = 0;
	double* sum_k = new double[_N];
	double sum_i;
	for (int k = 0; k < _N; k++)
	{
		sum_i = 0;
		for (int i = 0; i < _N; i++)
		{
			if ((k - i) < 0)
			{
				sum_i += lambda[i] * impulse[k - i + _N];
			}
			else
				sum_i += lambda[i] * impulse[k - i];
		}
		deconvolution.resize(_N);
		sum_k[k] = exp(-1 - sum_i);
		deconvolution[k] = sum_k[k];
		if (deconvolution[k] > A) A = deconvolution[k];
		N.push_back(k);
	}
	// график восстановленного сигнала и исходного
	Signal.Draw(3, signal, N, _N, A, N[_N - 1]);
	Signal.DrawRis(3, 1, signal, N, _N, A, N[_N - 1]);
	Signal.DrawRis(3, 2, deconvolution, N, _N, A, N[_N - 1]);

	// значение функционала
	functional = function(lambda);

	// коэффициент отклонения
	double Edif = 0, dif = 0;
	for (int i = 0; i < _N; i++)
	{
		dif = deconvolution[i] - signal[i];
		Edif += dif * dif;
	}
	otclon = sqrt(Edif / _N);

	UpdateData(false);
}

// кнопка для вывода графика восстановленного сигнала
void CIT4Dlg::OnBnClickedDeconv()
{
	deconvolution.clear();

	if (lflag == true)
	{
		lambda.clear(); 
	}
	lflag = true;

	//lambda = new float[_N];
	for (int i = 0; i < _N; i++)
	{
		lambda.push_back((double)rand() / RAND_MAX - 0.5);
	}

	timer = SetTimer(1, 100, NULL);

	hThread = CreateThread(NULL, 0, func, this, 0, &dwThread);
}

// таймер 
void CIT4Dlg::OnTimer(UINT_PTR nIDEvent)
{
	f2();
	if (killtimer)
	{
		KillTimer(timer);
	}
	CDialog::OnTimer(nIDEvent);
}