#pragma once
class Model
{
protected:
	int _id;
	Model(int id) : _id(id) {}

public: 
	int id() const { return _id; }
};

