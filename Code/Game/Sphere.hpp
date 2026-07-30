#pragma once


//-----------------------------------------------------------------------------------------------
#include "Game/Prop.hpp"


//-----------------------------------------------------------------------------------------------
class Sphere : public Prop 
{
public:
	Sphere( Game* owner, const Rgba8& color );
	~Sphere();

	void Update() override;
	void Render() const override;
};