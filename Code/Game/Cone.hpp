#pragma once


//-----------------------------------------------------------------------------------------------
#include "Game/Prop.hpp"


//-----------------------------------------------------------------------------------------------
class Cone : public Prop
{
public:
	Cone( Game* owner, Vec3 const& start, Vec3 const& end, float m_radius, Rgba8 const& color );
	~Cone();

	void Update() override;
	void Render() const override;

	Vec3  m_start;
	Vec3  m_end;
	float m_radius;
};