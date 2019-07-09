#pragma once

class BaseSystem
{
private:
public:
	virtual ~BaseSystem() = default;
	virtual void Update(float deltaTime) = 0;
};