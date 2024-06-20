
// IT_4.h: главный файл заголовка для приложения PROJECT_NAME
//

#pragma once

#ifndef __AFXWIN_H__
	#error "включить pch.h до включения этого файла в PCH"
#endif

#include "resource.h"		// основные символы


// CIT4App:
// Сведения о реализации этого класса: IT_4.cpp
//

class CIT4App : public CWinApp
{
public:
	CIT4App();

// Переопределение
public:
	virtual BOOL InitInstance();

// Реализация

	DECLARE_MESSAGE_MAP()
};

extern CIT4App theApp;
