#pragma once


//-----------------------------------------------------------------------------------------------
#include "Game/Prop.hpp"


//-----------------------------------------------------------------------------------------------
class Arrow : public Prop
{
public:
	Arrow( Game* owner, const Rgba8& color );
	~Arrow();

	void Update() override;
	void Render() const override;
};