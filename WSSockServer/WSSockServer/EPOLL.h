#pragma once

#include "LogManager.h"
class CEPOLL
{
public:
	CEPOLL();
	~CEPOLL();

	bool InitServer();
	void Update();
	void CloseServer();
};

