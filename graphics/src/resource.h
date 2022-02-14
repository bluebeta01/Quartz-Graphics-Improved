#pragma once

typedef void* NativeResource;

class Resource
{
public:
	virtual NativeResource getNativeResource() const = 0;
};