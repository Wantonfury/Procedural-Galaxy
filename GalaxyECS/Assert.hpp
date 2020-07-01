#pragma once

template <typename FUNC>
void assertFunc(bool expr, FUNC foo)
{
	if (!expr)
	{
		foo();
		abort();
	}
}