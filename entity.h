#pragma once
#include "observer.h"

class Entity : public ISubject
{
	void notify() override;
	void attach(IObserver* observer) override;
	void detach(IObserver* observer) override;
};

